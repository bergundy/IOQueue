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

#define IOQ_FIN_WRITE(n) do { \
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

#define IOQ_WRITE_NV(q, fd, onsockerror) do { \
    ssize_t _bytes_written;                                                                    \
    size_t _bytes_expected, _nodes_ready = IOQ_NODES_READY(q);                                 \
    IOQ_BYTES_EXPECTED(q, _bytes_expected);                                                    \
    if ( ( _bytes_written = writev( (fd), IOQ_GET_NV(q), _nodes_ready ) ) < _bytes_expected )  \
        switch (_bytes_written) {                                                              \
            case -1:                                                                           \
                switch (errno) {                                                               \
                    case EAGAIN:                                                               \
                    case EINTR:                                                                \
                        break;                                                                 \
                    /* TODO: deal with EINVAL */                                               \
                    case EINVAL:                                                               \
                    default:                                                                   \
                        goto onsockerror;                                                      \
                }                                                                              \
                break;                                                                         \
            default:                                                                           \
                while ( ( _bytes_written -= IOQ_GET_NV(q)->iov_len ) > 0 )                     \
                        IOQ_FIN_WRITE(1);                                                      \
                if ( _bytes_written < 0 ) {                                                    \
                    IOQ_GET_NV(q)->iov_base += IOQ_GET_NV(q)->iov_len + _bytes_written;        \
                    IOQ_GET_NV(q)->iov_len   = -_bytes_written;                                \
                }                                                                              \
        }                                                                                      \
    else                                                                                       \
        IOQ_FIN_WRITE(_nodes_ready);                                                           \
} while (false)

ioq *ioq_new(size_t size);
void ioq_free(ioq *q);

#endif /* _IOQUEUE_H */
