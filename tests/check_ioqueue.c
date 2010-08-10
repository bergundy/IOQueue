/**
 * =====================================================================================
 * @file     check_ioqueue.c
 * @brief    test suite for libioqueue functions and macros.
 * @date     08/09/2010
 * @author   Roey Berman, (royb@walla.net.il), Walla!
 * @version  1.0
 *
 * Copyright (c) 2010, Walla! (www.walla.co.il)
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
 *    This product includes software developed by Walla!.
 * 4. Neither the name of Walla! nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY WALLA ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL WALLA BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "ioqueue.h"

#define Q_SIZE 4

START_TEST(test_ioqueue)
{
    ioq *q = ioq_new(Q_SIZE);
    struct iovec *iov;
    char msg[200];
    int i, bytes_expected = 0;

    /* IOQ_PUT */
    for (i=0;i<Q_SIZE;++i) {
        sprintf(msg, "msg[%d]", i);
        fail_unless(IOQ_PUT(q, strdup(msg), strlen(msg), 1), "IOQ_PUT");
        printf("put: [%d] %s (%d)\n", i, IOQ_GET_NV(q,i)->iov_base, IOQ_GET_NV(q,i)->iov_len);
    }
    fail_if(IOQ_PUT(q, "baba4", sizeof("baba4") - 1, 0), "IOQ_PUT");

    for (i=0;i<Q_SIZE;++i) {
        sprintf(msg, "msg[%d]", i);
        printf("get: [%d] %s (%d)\n", (int)q->rear, IOQ_GET_NV(q, 0)->iov_base, IOQ_GET_NV(q, 0)->iov_len);
        fail_unless(iov = IOQ_GET(q), "IOQ_GET");
        fail_if(strcmp((iov->iov_base), msg), "node->iov_base != msg (%s/%s)", iov->iov_base, msg);
        AQUEUE_FIN_GET(q);
    }

    for (i=0;i<Q_SIZE;++i) {
        sprintf(msg, "msg[%d]", i);
        fail_unless(IOQ_PUT(q, strdup(msg), strlen(msg), 1), "IOQ_PUT");
        printf("put: [%d] %s (%d)\n", i, IOQ_GET_NV(q,i)->iov_base, IOQ_GET_NV(q,i)->iov_len);
    }
    AQUEUE_FIN_GET(q);
    fail_unless(IOQ_NODES_READY(q) == 3, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 3);
    fail_unless(AQUEUE_NODES_FREE(q) == 1, "AQUEUE_NODES_FREE : %d/%d", AQUEUE_NODES_FREE(q), 1);
    sprintf(msg, "msg[0]");
    fail_unless(IOQ_PUT(q, strdup(msg), strlen(msg), 1), "IOQ_PUT");
    fail_unless(AQUEUE_NODES_FREE(q) == 0, "AQUEUE_NODES_FREE : %d/%d", AQUEUE_NODES_FREE(q), 0);
    fail_unless(q->used == 4, "IOQ_NODES_USED : %d/%d", q->used, 4);

    for (i = 0; i < IOQ_NODES_READY(q); ++i)
        bytes_expected += IOQ_GET_NV(q,i)->iov_len;

    fail_unless(bytes_expected == strlen(msg)*3, "IOQ_BYTES_EXPECTED : %d/%d", bytes_expected, strlen(msg)*3);
    i = ioq_write(q, 1);
    fail_if(i != 3, "ioq_write: %d/%d", i, 3);
    fail_unless(IOQ_NODES_READY(q) == q->used, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), q->used);
    fail_if(ioq_write(q, 1) != 1, "ioq_write (2)");
    fail_unless(AQUEUE_EMPTY(q), "IOQ_EMPTY : %d/%d", AQUEUE_EMPTY(q), 0);
}
END_TEST

Suite *local_suite(void)
{
    Suite *s  = suite_create(__FILE__);
    TCase *tc = tcase_create("ioqueue");

    tcase_add_test(tc, test_ioqueue);

    suite_add_tcase(s, tc);
    return s;
}

int main()
{
    SRunner *sr;
    Suite *s;
    int failed;

    s = local_suite();
    sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);

    failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
