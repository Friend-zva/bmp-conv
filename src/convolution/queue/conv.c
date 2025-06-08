#include "conv.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "seq/_conv.h"

sig_atomic_t count_files = 0;
atomic_size_t index_file = 0;

sig_atomic_t count_t_reader = 0;
sig_atomic_t count_t_conv = 0;
atomic_size_t count_t_reader_finished = 0;
atomic_size_t count_t_conv_finished = 0;

typedef struct {
    Queue *q_reader;
    char **files;
    char *name_dir_in;
} data_ths_reader;

void bopen_helper(void *_data) {
    data_ths_reader *data_t = (data_ths_reader *)_data;
    Queue *q_reader = (Queue *)data_t->q_reader;
    char **files = (char **)data_t->files;
    char *name_dir_in = (char *)data_t->name_dir_in;

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

    if (atomic_fetch_add(&count_t_reader_finished, 1) == count_t_reader - 1) {
        atomic_exchange(q_reader->terminal, 1);
        printf("q_reader->terminal\n");
    }
}

typedef struct {
    Queue *q_reader;
    Queue *q_conv;
    Options opt;
} data_ths_conv;

void conv_helper(void *_data) {
    data_ths_conv *data_t = (data_ths_conv *)_data;
    Queue *q_reader = (Queue *)data_t->q_reader;
    Queue *q_conv = (Queue *)data_t->q_conv;
    Options opt = (Options)data_t->opt;

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

    if (atomic_fetch_add(&count_t_conv_finished, 1) == count_t_conv - 1) {
        atomic_exchange(q_conv->terminal, 1);
        printf("q_conv->terminal\n");
    }
}

typedef struct {
    Queue *q_conv;
    char *name_dir_out;
} data_ths_writer;

void bwrite_helper(void *_data) {
    data_ths_writer *data_t = (data_ths_writer *)_data;
    Queue *q_conv = (Queue *)data_t->q_conv;
    char *name_dir_out = (char *)data_t->name_dir_out;

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
        fprintf(stderr, "Error opening input directory\n");
        return 1;
    }

    char **files = (char **)malloc(256 * sizeof(char *));
    if (files == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        closedir(dir_in);
        return 1;
    }

    count_files = parse_files(dir_in, files);
    closedir(dir_in);
    if (count_files < 1) {
        fprintf(stderr, "Empty input directory\n");
        free(files);
        return 1;
    }

    Queue *q_reader = init_queue();
    count_t_reader = counts_thread[0];
    pthread_t ths_reader[count_t_reader];
    data_ths_reader data_t_reader[count_t_reader];
    for (int i = 0; i < count_t_reader; i++) {
        data_ths_reader data_t = {
            .q_reader = q_reader,
            .files = files,
            .name_dir_in = argv[1],
        };
        data_t_reader[i] = data_t;
        if (pthread_create(ths_reader + i, NULL, (void *)bopen_helper,
                           (void *)(data_t_reader + i))) {
            fprintf(stderr, "Error creating pthread\n");
            if (count_t_reader > 1) {
                count_t_reader--;
            } else {
                return cleanup_and_exit(files, q_reader, NULL, 1);
            }
        }
    }

    Queue *q_conv = init_queue();
    count_t_conv = counts_thread[1];
    pthread_t ths_conv[count_t_conv];
    data_ths_conv data_t_conv[count_t_conv];
    for (int i = 0; i < count_t_conv; i++) {
        data_ths_conv data_t = {
            .q_reader = q_reader,
            .q_conv = q_conv,
            .opt = opt,
        };
        data_t_conv[i] = data_t;
        if (pthread_create(ths_conv + i, NULL, (void *)conv_helper,
                           (void *)(data_t_conv + i))) {
            fprintf(stderr, "Error creating pthread\n");
            if (count_t_conv > 1) {
                count_t_conv--;
            } else {
                return cleanup_and_exit(files, q_reader, q_conv, 1);
            }
        }
    }

    int count_t_writer = counts_thread[2];
    pthread_t ths_writer[count_t_writer];
    data_ths_writer data_t_writer[count_t_writer];
    for (int i = 0; i < count_t_writer; i++) {
        data_ths_writer data_t = {
            .q_conv = q_conv,
            .name_dir_out = argv[2],
        };
        data_t_writer[i] = data_t;
        if (pthread_create(ths_writer + i, NULL, (void *)bwrite_helper,
                           (void *)(data_t_writer + i))) {
            fprintf(stderr, "Error creating pthread\n");
            if (count_t_writer > 1) {
                count_t_writer--;
            } else {
                return cleanup_and_exit(files, q_reader, q_conv, 1);
            }
        }
    }

    for (int i = 0; i < count_t_reader; ++i) {
        if (pthread_join(*(ths_reader + i), NULL)) {
            fprintf(stderr, "Error joining pthreads\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }
    for (int i = 0; i < count_t_conv; ++i) {
        if (pthread_join(*(ths_conv + i), NULL)) {
            fprintf(stderr, "Error joining pthreads\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }
    for (int i = 0; i < count_t_writer; ++i) {
        if (pthread_join(*(ths_writer + i), NULL)) {
            fprintf(stderr, "Error joining pthreads\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }

    return cleanup_and_exit(files, q_reader, q_conv, 0);
}
