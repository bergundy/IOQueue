#include <stdlib.h>
#include "ioqueue.h"

ioq *ioq_new(size_t size)
{
    ioq *q = NULL;
    ioq_node *p = NULL;
    size_t i;

    if ( ( q = (ioq *)malloc(sizeof(ioq)) ) == NULL )
        return NULL;

    if ( ( q->nodes_begin = (ioq_node *)malloc( sizeof(ioq) * (size) ) ) == NULL )
        goto nodes_malloc_error;

    if ( ( q->nodes_begin->vec = (struct iovec *)malloc( sizeof(struct iovec) * (size) ) ) == NULL )
        goto iovec_malloc_error;

    p = q->input_p = q->nodes_begin;

    for ( i = 1; i < size; ++i ) {
        p = p->next = q->nodes_begin + i;
        p->vec      = q->nodes_begin->vec + i;
    }
        
    q->output_p = q->nodes_end = p;
    q->nodes_end->next = q->nodes_begin;

    return q;

iovec_malloc_error:
    free(q->nodes_begin);

nodes_malloc_error:
    free(q);
    return NULL;
}

void ioq_free(ioq *q)
{
    free(q->nodes_begin->vec);
    free(q->nodes_begin);
    free(q);
}
