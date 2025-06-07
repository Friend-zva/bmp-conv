#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#include "_conv.h"
#include "seq/_conv.h"

sig_atomic_t count_files = 0;
atomic_size_t index_file = 0;

atomic_bool signal_finished_reading = 0;
atomic_bool signal_adding_to_q_reader = 0;
atomic_bool signal_finished_conv = 0;
atomic_bool signal_adding_to_q_conv = 0;

pthread_mutex_t mutex_push_q;
pthread_mutex_t mutex_pop_q;

typedef struct {
    BMP *bmp_source;
    BMP *bmp_conv;
    char *name_file;
} data_queue;

typedef struct queue_t {
    data_queue *data;
    struct queue_t *next;
} queue_s;

typedef struct {
    queue_s *head;
    queue_s *tail;
} Queue;

data_queue *init_data_queue(BMP *bmp_source, BMP *bmp_conv, char *name_file) {
    data_queue *data_q = (data_queue *)malloc(sizeof(data_queue));
    if (data_q == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    data_q->bmp_source = bmp_source;
    data_q->bmp_conv = bmp_conv;
    data_q->name_file = name_file;
    return data_q;
}

Queue *init_queue() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    queue->head = queue->tail = NULL;
    return queue;
}

int push_queue(Queue *queue, data_queue *data) {
    pthread_mutex_lock(&mutex_push_q);

    if (queue == NULL || data == NULL) {
        fprintf(stderr, "Invalid queue or data\n");
        return 1;
    }

    queue_s *q_tail_new = malloc(sizeof(queue_s));
    if (q_tail_new == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }

    q_tail_new->data = data;
    q_tail_new->next = NULL;

    if (queue->tail) {
        queue->tail->next = q_tail_new;
    } else {
        queue->head = q_tail_new;
    }
    queue->tail = q_tail_new;

    pthread_mutex_unlock(&mutex_push_q);
    return 0;
}

data_queue *pop_queue(Queue *queue) {
    pthread_mutex_lock(&mutex_pop_q);

    if (queue == NULL) {
        fprintf(stderr, "Invalid queue\n");
        return NULL;
    }

    while (queue->head == NULL) {
        continue;
    }

    queue_s *q_head = queue->head;
    data_queue *data = q_head->data;

    queue->head = q_head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    free(q_head);
    pthread_mutex_unlock(&mutex_pop_q);
    return data;
}

typedef struct {
    Queue *q_reader;
    char **files;
    char *name_dir;
} data_ths_reader;

void bopen_helper(void *_data) {
    data_ths_reader *data_t = (data_ths_reader *)_data;
    Queue *q_reader = (Queue *)data_t->q_reader;
    char **files = (char **)data_t->files;
    char *name_dir_in = (char *)data_t->name_dir;

    size_t index;
    while ((index = atomic_fetch_add(&index_file, 1)) < count_files) {
        char file_source[64];
        snprintf(file_source, sizeof(file_source), "%s/%s", name_dir_in,
                 files[index]);

        BMP *bmp_source = bopen(file_source);
        if (bmp_source == NULL) {
            fprintf(stderr, "Error opening input file\n");
            continue;
        }

        data_queue *data_q = init_data_queue(bmp_source, NULL, files[index]);
        push_queue(q_reader, data_q);
        atomic_exchange(&signal_adding_to_q_reader, 1);
    }

    atomic_exchange(&signal_finished_reading, 1);
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

    while (!signal_finished_reading || q_reader->head != NULL) {
        if (!signal_adding_to_q_reader) {
            continue;
        }
        atomic_exchange(&signal_adding_to_q_reader, 0);

        data_queue *data_q = pop_queue(q_reader);
        if (data_q->bmp_source != NULL) {
            BMP *bmp_conv = conv_seq(data_q->bmp_source, opt);
            if (bmp_conv == NULL) {
                continue;
            }

            data_q->bmp_conv = bmp_conv;
            bclose(data_q->bmp_source);
            push_queue(q_conv, data_q);
            atomic_exchange(&signal_adding_to_q_conv, 1);
        }
    }

    atomic_exchange(&signal_finished_conv, 1);
}

typedef struct {
    Queue *q_conv;
    char *name_dir;
} data_ths_writer;

void bwrite_helper(void *_data) {
    data_ths_writer *data_t = (data_ths_writer *)_data;
    Queue *q_conv = (Queue *)data_t->q_conv;
    char *name_dir_out = (char *)data_t->name_dir;

    while (!signal_finished_conv || q_conv->head != NULL) {
        if (!signal_adding_to_q_conv) {
            continue;
        }
        atomic_exchange(&signal_adding_to_q_conv, 0);

        data_queue *data_q = pop_queue(q_conv);
        if (data_q->bmp_conv != NULL) {
            char file_target[64];
            snprintf(file_target, sizeof(file_target), "%s/%s", name_dir_out,
                     data_q->name_file);

            bwrite(data_q->bmp_conv, file_target);
            bclose(data_q->bmp_conv);
        }
    }
}

int cleanup_and_exit(char **files, Queue *q_reader, Queue *q_conv, int code) {
    pthread_mutex_destroy(&mutex_pop_q);
    pthread_mutex_destroy(&mutex_push_q);
    free(files);
    free(q_reader);
    if (q_conv != NULL) {
        free(q_conv);
    }
    return code;
}

int queue_mode(char **argv, Options opt, int count_ths[3]) {
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

    pthread_mutex_init(&mutex_push_q, NULL);
    pthread_mutex_init(&mutex_pop_q, NULL);

    Queue *q_reader = (Queue *)malloc(sizeof(Queue));
    pthread_t ths_reader[count_ths[0]];
    data_ths_reader data_t_reader[count_ths[0]];

    for (int i = 0; i < count_ths[0]; i++) {
        data_ths_reader data_t = {
            .q_reader = q_reader, .files = files, .name_dir = argv[1]};
        data_t_reader[i] = data_t;
        if (pthread_create(ths_reader + i, NULL, (void *)bopen_helper,
                           (void *)(data_t_reader + i))) {
            fprintf(stderr, "Error creating pthread\n");
            return cleanup_and_exit(files, q_reader, NULL, 1);
        }
    }

    Queue *q_conv = (Queue *)malloc(sizeof(Queue));
    pthread_t ths_conv[count_ths[1]];
    data_ths_conv data_t_conv[count_ths[1]];

    for (int i = 0; i < count_ths[1]; i++) {
        data_ths_conv data_t = {
            .q_reader = q_reader, .q_conv = q_conv, .opt = opt};
        data_t_conv[i] = data_t;
        if (pthread_create(ths_conv + i, NULL, (void *)conv_helper,
                           (void *)(data_t_conv + i))) {
            fprintf(stderr, "Error creating pthread\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }

    pthread_t ths_writer[count_ths[1]];
    data_ths_writer data_t_writer[count_ths[1]];

    for (int i = 0; i < count_ths[2]; i++) {
        data_ths_writer data_t = {.q_conv = q_conv, .name_dir = argv[2]};
        data_t_writer[i] = data_t;
        if (pthread_create(ths_writer + i, NULL, (void *)bwrite_helper,
                           (void *)(data_t_writer + i))) {
            fprintf(stderr, "Error creating pthread\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }

    for (int i = 0; i < count_ths[0]; ++i) {
        if (pthread_join(*(ths_reader + i), NULL)) {
            fprintf(stderr, "Error joining pthreads\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }
    for (int i = 0; i < count_ths[1]; ++i) {
        if (pthread_join(*(ths_conv + i), NULL)) {
            fprintf(stderr, "Error joining pthreads\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }
    for (int i = 0; i < count_ths[2]; ++i) {
        if (pthread_join(*(ths_writer + i), NULL)) {
            fprintf(stderr, "Error joining pthreads\n");
            return cleanup_and_exit(files, q_reader, q_conv, 1);
        }
    }

    return cleanup_and_exit(files, q_reader, q_conv, 0);
}
