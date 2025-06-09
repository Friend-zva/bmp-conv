#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

data_queue *init_data_queue(BMP *bmp_source, BMP *bmp_conv, char *name_file) {
    data_queue *data_q = (data_queue *)malloc(sizeof(data_queue));
    if (data_q == NULL) {
        error(ERROR_MALLOC);
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
        error(ERROR_MALLOC);
        return NULL;
    }
    queue->head = queue->tail = NULL;

    queue->mutex_push = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    queue->mutex_pop = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    queue->terminal = (atomic_bool *)calloc(1, sizeof(atomic_bool));
    if (queue->mutex_push == NULL || queue->mutex_pop == NULL ||
        queue->terminal == NULL) {
        error(ERROR_MALLOC);
        free_queue(queue);
        return NULL;
    }

    pthread_mutex_init(queue->mutex_push, NULL);
    pthread_mutex_init(queue->mutex_pop, NULL);

    return queue;
}

void free_queue(Queue *queue) {
    if (queue->mutex_push) pthread_mutex_destroy(queue->mutex_push);
    if (queue->mutex_pop) pthread_mutex_destroy(queue->mutex_pop);
    free(queue->mutex_push);
    free(queue->mutex_pop);
    free(queue->terminal);
    free(queue);
}

int push_queue(Queue *queue, data_queue *data) {
    pthread_mutex_lock(queue->mutex_push);

    if (queue == NULL || data == NULL) {
        error("Error: nullable queue or data\n");
        return 1;
    }

    node_queue *tail_new = malloc(sizeof(node_queue));
    if (tail_new == NULL) {
        error(ERROR_MALLOC);
        return 1;
    }

    tail_new->data = data;
    tail_new->next = NULL;

    if (queue->tail) {
        queue->tail->next = tail_new;
    } else {
        queue->head = tail_new;
    }
    queue->tail = tail_new;

    pthread_mutex_unlock(queue->mutex_push);
    return 0;
}

data_queue *pop_queue(Queue *queue) {
    pthread_mutex_lock(queue->mutex_pop);

    if (queue == NULL) {
        error("Error: nullable queue\n");
        return NULL;
    }

    while (queue->head == NULL) {
        if (*(queue->terminal)) {
            pthread_mutex_unlock(queue->mutex_pop);
            return NULL;
        }
        continue;
    }

    node_queue *head = queue->head;
    data_queue *data = head->data;

    queue->head = head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    free(head);
    pthread_mutex_unlock(queue->mutex_pop);
    return data;
}
