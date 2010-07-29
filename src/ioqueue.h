#ifndef _IOQUEUE_H
#define _IOQUEUE_H

#include <sys/uio.h>

#include <arrayqueue.h>

struct _ioq_node {
    struct iovec *vec;
    int    autofree;
};

DEFINE_STRUCT_QUEUE(_ioq, struct _ioq_node);

typedef struct _ioq ioq;
typedef struct _ioq_node ioq_node;

#define IOQ_NODES_READY(q) ( (q)->used ? ( (q)->front <= (q)->rear ? (q)->size - (q)->rear : (q)->used ) : 0 )

#define IOQ_PUT_NV(q, d, l, af) (             \
    AQUEUE_FRONT_NV(q)->vec->iov_base = (d),  \
    AQUEUE_FRONT_NV(q)->vec->iov_len  = (l),  \
    AQUEUE_FRONT_NV(q)->autofree = (af),      \
    AQUEUE_FIN_PUT(q),                        \
    1 )

#define IOQ_PUT(q, d, l, af) ( AQUEUE_FULL(q) ? 0 : IOQ_PUT_NV(q, d, l, af) )

#define IOQ_GET_NV(q) (AQUEUE_REAR_NV(q)->vec)

#define IOQ_GET(q)    (AQUEUE_EMPTY(q) ? NULL : IOQ_GET_NV(q))

#define IOQ_FIN_WRITE(q, n) do {                        \
    size_t __iter;                                      \
    for ( __iter = 0; __iter < (n); ++__iter ) {        \
        if ( AQUEUE_REAR_NV(q)->autofree )              \
            free(AQUEUE_REAR_NV(q)->vec->iov_base);     \
        AQUEUE_FIN_GET(q);                              \
    }                                                   \
} while (0)

#define IOQ_BYTES_EXPECTED(q, total) do { \
    size_t __iter; total = 0;                                 \
    for ( __iter = 0; __iter < IOQ_NODES_READY(q); ++__iter ) \
        total += (IOQ_GET_NV(q)+__iter)->iov_len;             \
} while (0)

ssize_t ioq_write_nv(ioq *q, int fd);
ioq *ioq_new(size_t size);
void ioq_free(ioq *q);

#endif /* _IOQUEUE_H */
