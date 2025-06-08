#include "conv.h"

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "seq/conv.h"

int count_files = 0;
atomic_size_t index_file = 0;

int count_ths_reader = 0;
int count_ths_conv = 0;
atomic_size_t count_ths_reader_finished = 0;
atomic_size_t count_ths_conv_finished = 0;

typedef struct {
    Queue *q_reader;
    char **files;
    char *name_dir_in;
} data_thread_reader;

void bopen_parallel(void *_data) {
    data_thread_reader *data_th = (data_thread_reader *)_data;
    Queue *q_reader = (Queue *)data_th->q_reader;
    char **files = (char **)data_th->files;
    char *name_dir_in = (char *)data_th->name_dir_in;

    size_t index;
    while ((index = atomic_fetch_add(&index_file, 1)) < count_files) {
        char file_source[64];
        snprintf(file_source, sizeof(file_source), "%s/%s", name_dir_in,
                 files[index]);

        BMP *bmp_source = bopen(file_source);
        if (bmp_source == NULL) {
            continue;
        }
        data_queue *data_q = init_data_queue(bmp_source, NULL, files[index]);

        if (push_queue(q_reader, data_q)) {
            bclose(bmp_source);
            free(data_q);
        }
    }

    if (atomic_fetch_add(&count_ths_reader_finished, 1) ==
        count_ths_reader - 1) {
        atomic_exchange(q_reader->terminal, 1);
        log("Log: readers finished\n");
    }
}

typedef struct {
    Queue *q_reader;
    Queue *q_conv;
    Options opt;
} data_thread_conv;

void conv_parallel(void *_data) {
    data_thread_conv *data_th = (data_thread_conv *)_data;
    Queue *q_reader = (Queue *)data_th->q_reader;
    Queue *q_conv = (Queue *)data_th->q_conv;
    Options opt = (Options)data_th->opt;

    while (!atomic_load(q_reader->terminal) || q_reader->head) {
        data_queue *data_q = pop_queue(q_reader);
        if (data_q == NULL || data_q->bmp_source == NULL) {
            continue;
        }

        BMP *bmp_conv = conv_seq(data_q->bmp_source, opt);
        if (bmp_conv == NULL) {
            continue;
        }
        data_q->bmp_conv = bmp_conv;
        bclose(data_q->bmp_source);

        if (push_queue(q_conv, data_q)) {
            bclose(bmp_conv);
            free(data_q);
        }
    }

    if (atomic_fetch_add(&count_ths_conv_finished, 1) == count_ths_conv - 1) {
        atomic_exchange(q_conv->terminal, 1);
        log("Log: workers finished\n");
    }
}

typedef struct {
    Queue *q_conv;
    char *name_dir_out;
} data_thread_writer;

void bwrite_parallel(void *_data) {
    data_thread_writer *data_th = (data_thread_writer *)_data;
    Queue *q_conv = (Queue *)data_th->q_conv;
    char *name_dir_out = (char *)data_th->name_dir_out;

    while (!atomic_load(q_conv->terminal) || q_conv->head) {
        data_queue *data_q = pop_queue(q_conv);
        if (data_q == NULL || data_q->bmp_conv == NULL) {
            continue;
        }

        char file_target[64];
        snprintf(file_target, sizeof(file_target), "%s/%s", name_dir_out,
                 data_q->name_file);

        bwrite(data_q->bmp_conv, file_target);
        printf("Check %s\n", file_target);
        bclose(data_q->bmp_conv);
        free(data_q);
    }
}

int cleanup_and_exit(char **files, Queue *q_reader, Queue *q_conv, int code) {
    free(files);
    free_queue(q_reader);
    if (q_conv) {
        free_queue(q_conv);
    }
    return code;
}

int queue_mode(char **argv, Options opt, int counts_thread[3]) {
    DIR *dir_in = opendir(argv[1]);
    if (dir_in == NULL) {
        error("Error: opening input directory failed\n");
        return 1;
    }

    char **files = (char **)malloc(256 * sizeof(char *));
    if (files == NULL) {
        error(ERROR_MALLOC);
        closedir(dir_in);
        return 1;
    }

    count_files = parse_files(dir_in, files);
    closedir(dir_in);
    if (count_files < 1) {
        error("Empty input directory\n");
        free(files);
        return 1;
    }
    log("Log: %d parsed files\n", count_files);

    Queue *q_reader = init_queue();
    count_ths_reader = counts_thread[0];
    pthread_t ths_reader[count_ths_reader];
    data_thread_reader data_ths_reader[count_ths_reader];
    for (int i = 0; i < counts_thread[0]; i++) {
        data_thread_reader data_th = {
            .q_reader = q_reader,
            .files = files,
            .name_dir_in = argv[1],
        };
        data_ths_reader[i] = data_th;
        if (pthread_create(ths_reader + i, NULL, (void *)bopen_parallel,
                           (void *)(data_ths_reader + i))) {
            error(ERROR_PTHREAD_CREATION);
            if (count_ths_reader > 1) {
                count_ths_reader--;
            } else {
                return cleanup_and_exit(files, q_reader, NULL, 1);
            }
        }
    }

    Queue *q_conv = init_queue();
    count_ths_conv = counts_thread[1];
    pthread_t ths_conv[count_ths_conv];
    data_thread_conv data_ths_conv[count_ths_conv];
    for (int i = 0; i < counts_thread[1]; i++) {
        data_thread_conv data_th = {
            .q_reader = q_reader,
            .q_conv = q_conv,
            .opt = opt,
        };
        data_ths_conv[i] = data_th;
        if (pthread_create(ths_conv + i, NULL, (void *)conv_parallel,
                           (void *)(data_ths_conv + i))) {
            error(ERROR_PTHREAD_CREATION);
            if (count_ths_conv > 1) {
                count_ths_conv--;
            } else {
                return cleanup_and_exit(files, q_reader, q_conv, 1);
            }
        }
    }

    int count_ths_writer = counts_thread[2];
    pthread_t ths_writer[count_ths_writer];
    data_thread_writer data_ths_writer[count_ths_writer];
    for (int i = 0; i < counts_thread[2]; i++) {
        data_thread_writer data_th = {
            .q_conv = q_conv,
            .name_dir_out = argv[2],
        };
        data_ths_writer[i] = data_th;
        if (pthread_create(ths_writer + i, NULL, (void *)bwrite_parallel,
                           (void *)(data_ths_writer + i))) {
            error(ERROR_PTHREAD_CREATION);
            if (count_ths_writer > 1) {
                count_ths_writer--;
            } else {
                return cleanup_and_exit(files, q_reader, q_conv, 1);
            }
        }
    }

    for (int i = 0; i < count_ths_reader; ++i) {
        if (pthread_join(*(ths_reader + i), NULL)) {
            error(ERROR_PTHREAD_JOINING);
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }
    for (int i = 0; i < count_ths_conv; ++i) {
        if (pthread_join(*(ths_conv + i), NULL)) {
            error(ERROR_PTHREAD_JOINING);
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }
    for (int i = 0; i < count_ths_writer; ++i) {
        if (pthread_join(*(ths_writer + i), NULL)) {
            error(ERROR_PTHREAD_JOINING);
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }

    return cleanup_and_exit(files, q_reader, q_conv, 0);
}
