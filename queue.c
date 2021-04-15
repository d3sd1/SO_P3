

#include "queue.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Instanciate queue.
 * @param size
 * @return
 */
queue *queue_init(int size) {
  queue *q = (queue *)malloc(sizeof(queue));
  q->ring = malloc(size * sizeof(struct element));
  q->size = size;
  q->length = 0;
  q->head = 0;
  q->tail = 0;
  return q;
}


/**
 * Enqueue element. Head is inserted and tail gets out.
 * @param q
 * @param x
 * @return
 */
int queue_put(queue *q, struct element *x) {
  if (queue_full(q) == 0) {
    q->ring[q->head] = *x;
    q->head = (q->head + 1) % q->size;
    q->length = q->length + 1;
    return 0;
  }
  return -1;
}

/**
 * Dequeue element.
 * @param q
 * @return
 */
struct element *queue_get(queue *q) {
  struct element *elem;
  if (queue_empty(q) == 0) {
    elem = &(q->ring[q->tail]);
    q->tail = (q->tail + 1) % q->size;
    q->length = q->length - 1;
  }
  return elem;
}

/**
 * Check Queue state.
 * @param q
 * @return
 */
int queue_empty(queue *q) {
  if (q->length == 0)
    return 1;
  return 0;
}

int queue_full(queue *q) {
  if (q->length == q->size)
    return 1;
  return 0;
}

// To destroy the queue and free the resources
int queue_destroy(queue *q) {
  free(q);
  return 0;
}
