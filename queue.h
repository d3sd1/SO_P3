#ifndef HEADER_FILE
#define HEADER_FILE

struct element {
    /**
     * Machine type
     */
    int type;
    /**
     * Machine elapsed time use
     */
    int time;
};

typedef struct queue {
    /**
     * Queue size with queue assertions
     */
    int length;
    /**
     * Elements in queue
     */
    struct element *ring;

    /**
     * First element of the queue
     */
    int head;

    /**
     * Last element of the queue
     */
    int tail;
    /**
     * Queue length (fixed)
     */
    int size;
} queue;


struct param {
    /**
     * Initialization ID
     */
    int id;
    /**
     * Operation type
     */
    int op;
};

/**
 * Given code below
 */
queue *queue_init(int size);

int queue_destroy(queue *q);

int queue_put(queue *q, struct element *elem);

struct element *queue_get(queue *q);

int queue_empty(queue *q);

int queue_full(queue *q);

#endif
