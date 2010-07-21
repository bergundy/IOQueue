#ifndef _IOQUEUE_H
#define _IOQUEUE_H

#include <stdbool.h>
#include <errno.h>
#include <sys/uio.h>

struct _ioq_node {
    struct iovec *vec;
    bool   autofree;
    struct _ioq_node *next;
};

typedef struct _ioq_node ioq_node;

struct _ioq {
    ioq_node *input_p;
    ioq_node *output_p;
    ioq_node *nodes_begin;
    ioq_node *nodes_end;
};

typedef struct _ioq ioq;

#define IOQ_FULL(q)        ( (q)->input_p == (q)->output_p )

#define IOQ_NODES_FREE(q)  ( (q)->output_p - (q)->input_p >= 0 ? (q)->output_p - (q)->input_p \
    : (q)->nodes_end - (q)->input_p + (q)->output_p - (q)->nodes_begin + 1 )

#define IOQ_EMPTY(q)       ( (q)->input_p == (q)->output_p->next )

#define IOQ_NODES_READY(q) ( (q)->input_p - (q)->output_p->next > 0 ? (q)->input_p - (q)->output_p->next \
    : (q)->nodes_end - (q)->output_p )

#define IOQ_PUT_NV(q, d, l, af) (       \
    (q)->input_p->vec->iov_base = (d),  \
    (q)->input_p->vec->iov_len  = (l),  \
    (q)->input_p->autofree     = (af),  \
    (q)->input_p = (q)->input_p->next,  \
    true )

#define IOQ_PUT(q, d, l, af) ( IOQ_FULL(q) ? false : IOQ_PUT_NV(q, d, l, af) )

#define IOQ_GET_NV(q) ( (q)->output_p->next->vec )

#define IOQ_GET(q) ( IOQ_EMPTY(q) ? NULL : IOQ_GET_NV(q) )

#define IOQ_FIN_WRITE(q, n) do { \
    size_t __iter;                                 \
    for ( __iter = 0; __iter < (n); ++__iter ) {   \
        (q)->output_p = (q)->output_p->next;       \
        if ( (q)->output_p->autofree )             \
            free( (q)->output_p->vec->iov_base );  \
    }                                              \
} while (false)

#define IOQ_BYTES_EXPECTED(q, total) do { \
    size_t __iter; total = 0;                                 \
    for ( __iter = 0; __iter < IOQ_NODES_READY(q); ++__iter ) \
        total += (IOQ_GET_NV(q)+__iter)->iov_len;             \
} while (false)

typedef void (*sockerror_cb_p_t)(void *);

void ioq_write_nv( ioq *q, int fd, sockerror_cb_p_t onsockerror, void *onsockerror_arg);
ioq *ioq_new(size_t size);
void ioq_free(ioq *q);

#endif /* _IOQUEUE_H */
