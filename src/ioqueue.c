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

ssize_t ioq_write_nv(ioq *q, int fd)
{
    ssize_t bytes_written;
    size_t  bytes_expected, nodes_ready = IOQ_NODES_READY(q);
    ssize_t nodes_written = 0;
    IOQ_BYTES_EXPECTED(q, bytes_expected);
    if ( ( bytes_written = writev(fd, IOQ_GET_NV(q), nodes_ready) ) < bytes_expected )
        switch (bytes_written) {
            case -1:
                return -1;
            default:
                while ( ( bytes_written -= IOQ_GET_NV(q)->iov_len ) > 0 ) {
                    IOQ_FIN_WRITE(q, 1);
                    ++nodes_written;
                }
                if ( bytes_written < 0 ) {
                    IOQ_GET_NV(q)->iov_base += IOQ_GET_NV(q)->iov_len + bytes_written;
                    IOQ_GET_NV(q)->iov_len   = -bytes_written;
                }
                return nodes_written;
        }
    else {
        IOQ_FIN_WRITE((q), nodes_ready);
        return nodes_ready;
    }
}
