#pragma once

#include <pthread.h>
#include <stdatomic.h>

#include "utils/_utils.h"

typedef struct {
    BMP *bmp_source;
    BMP *bmp_conv;
    char *name_file;
} data_queue;

data_queue *init_data_queue(BMP *bmp_source, BMP *bmp_conv, char *name_file);

typedef struct node {
    data_queue *data;
    struct node *next;
} node_queue;

typedef struct {
    node_queue *head;
    node_queue *tail;
    pthread_mutex_t *mutex_push;
    pthread_mutex_t *mutex_pop;
    atomic_bool *terminal;
} Queue;

Queue *init_queue();

void free_queue(Queue *queue);

int push_queue(Queue *queue, data_queue *data);

data_queue *pop_queue(Queue *queue);
