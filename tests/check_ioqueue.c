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
    int i;

    /* IOQ_PUT */
    for (i=0;i<Q_SIZE;++i) {
        sprintf(msg, "msg[%d]", i);
        fail_unless(IOQ_PUT(q, strdup(msg), strlen(msg), 1), "IOQ_PUT");
        printf("put: [%d] %s (%d)\n", i, (IOQ_GET_NV(q)+i)->iov_base, (IOQ_GET_NV(q)+i)->iov_len);
    }
    fail_if(IOQ_PUT(q, "baba4", sizeof("baba4") - 1, 0), "IOQ_PUT");

    for (i=0;i<Q_SIZE;++i) {
        sprintf(msg, "msg[%d]", i);
        printf("get: [%d] %s (%d)\n", (int)q->rear, IOQ_GET_NV(q)->iov_base, IOQ_GET_NV(q)->iov_len);
        fail_unless(iov = IOQ_GET(q), "IOQ_GET");
        fail_if(strcmp((iov->iov_base), msg), "node->iov_base != msg (%s/%s)", iov->iov_base, msg);
        AQUEUE_FIN_GET(q);
    }

    for (i=0;i<Q_SIZE;++i) {
        sprintf(msg, "msg[%d]", i);
        fail_unless(IOQ_PUT(q, strdup(msg), strlen(msg), 1), "IOQ_PUT");
        printf("put: [%d] %s (%d)\n", i, (IOQ_GET_NV(q)+i)->iov_base, (IOQ_GET_NV(q)+i)->iov_len);
    }
    AQUEUE_FIN_GET(q);
    fail_unless(IOQ_NODES_READY(q) == 3, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 3);
    fail_unless(AQUEUE_NODES_FREE(q) == 1, "AQUEUE_NODES_FREE : %d/%d", AQUEUE_NODES_FREE(q), 1);
    sprintf(msg, "msg[0]");
    fail_unless(IOQ_PUT(q, strdup(msg), strlen(msg), 1), "IOQ_PUT");
    fail_unless(AQUEUE_NODES_FREE(q) == 0, "AQUEUE_NODES_FREE : %d/%d", AQUEUE_NODES_FREE(q), 0);
    fail_unless(q->used == 4, "IOQ_NODES_USED : %d/%d", q->used, 4);

    IOQ_BYTES_EXPECTED(q, i);
    fail_unless(i == strlen(msg)*3, "IOQ_BYTES_EXPECTED : %d/%d", i, strlen(msg)*3);
    fail_if(ioq_write_nv(q, 1) != 3, "ioq_write_nv");
    fail_unless(IOQ_NODES_READY(q) == q->used, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), q->used);
    fail_if(ioq_write_nv(q, 1) != 1, "ioq_write_nv");
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
