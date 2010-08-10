/**
 * =====================================================================================
 * @file     ioqueue.h
 * @brief    header file for ioqueue.c -
 *           A queue (buffer) library for event driven applications.
 * @date     07/05/2010
 * @author   Roey Berman, (roey.berman@gmail.com)
 * @version  1.0
 *
 * Copyright (c) 2010, Roey Berman, (roeyb.berman@gmail.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Roey Berman.
 * 4. Neither the name of Roey Berman nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROEY BERMAN ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROEY BERMAN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * =====================================================================================
 */

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

#define IOQ_PUT_NV(q, d, l, af) (              \
    AQUEUE_FRONT_NV(q)->vec->iov_base = (d),   \
    AQUEUE_FRONT_NV(q)->vec->iov_len  = (l),   \
    AQUEUE_FRONT_NV(q)->autofree      = (af),  \
    AQUEUE_FIN_PUT(q),                         \
    1 )

#define IOQ_PUT(q, d, l, af) ( AQUEUE_FULL(q) ? 0 : IOQ_PUT_NV(q, d, l, af) )

#define IOQ_GET_NV(q, i) ((AQUEUE_REAR_NV(q)+i)->vec)

#define IOQ_GET(q)       (AQUEUE_EMPTY(q) ? NULL : IOQ_GET_NV(q, 0))

#define IOQ_FIN_WRITE(q, n) do {                       \
    size_t __iter;                                     \
    for ( __iter = 0; __iter < (n); ++__iter ) {       \
        if ( AQUEUE_REAR_NV(q)->autofree )             \
            free(AQUEUE_REAR_NV(q)->vec->iov_base);    \
        AQUEUE_FIN_GET(q);                             \
    }                                                  \
} while (0)

ssize_t ioq_write(ioq *q, int fd);
ioq    *ioq_new(size_t size);
void    ioq_free(ioq *q);

#endif /* _IOQUEUE_H */
