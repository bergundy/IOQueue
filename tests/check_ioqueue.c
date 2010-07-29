#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "ioqueue.h"

#define Q_SIZE 5

START_TEST(test_ioqueue)
{
    ioq *q = ioq_new(Q_SIZE);
    ioq_node *node = q->input_p;
    int i;
    for ( i = 1; i < Q_SIZE; ++i )
        node = node->next;

    fail_unless(node == q->output_p && node->next == q->input_p, "ioq size");

    fail_unless(IOQ_NODES_FREE(q) == Q_SIZE - 1, "IOQ_NODES_FREE : %d/%d", IOQ_NODES_FREE(q), Q_SIZE - 1);

    q->output_p = q->output_p->next;
    q->input_p = q->input_p->next;
    fail_unless(IOQ_NODES_FREE(q) == Q_SIZE - 1, "IOQ_NODES_FREE : %d/%d", IOQ_NODES_FREE(q), Q_SIZE - 1);

    fail_unless(IOQ_PUT(q, "baba1", sizeof("baba1") - 1, false), "IOQ_PUT");
    fail_unless(IOQ_NODES_READY(q) == 1, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 1);

    node = q->output_p;
    q->output_p = q->nodes_end;
    fail_unless(IOQ_NODES_READY(q) == 2, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 2);

    q->output_p = q->nodes_end - 1;
    fail_unless(IOQ_NODES_READY(q) == 1, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 1);

    q->output_p = node;

    fail_unless(IOQ_PUT(q, "baba2", sizeof("baba2") - 1, false), "IOQ_PUT");
    fail_unless(IOQ_PUT(q, "baba3", sizeof("baba3") - 1, false), "IOQ_PUT");
    fail_unless(IOQ_PUT(q, "baba4", sizeof("baba4") - 1, false), "IOQ_PUT");
    fail_if(IOQ_PUT(q, "baba4", sizeof("baba4") - 1, false), "IOQ_PUT");

    fail_unless(IOQ_NODES_READY(q) == 4, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 4);
    fail_unless(IOQ_NODES_FREE(q) == 0, "IOQ_NODES_FREE : %d/%d", IOQ_NODES_FREE(q), 0);
    node = q->output_p;
    q->output_p = q->output_p->next;
    fail_unless(IOQ_NODES_READY(q) == 3, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 3);
    fail_unless(IOQ_NODES_USED(q) == 4, "IOQ_NODES_USED : %d/%d", IOQ_NODES_READY(q), 4);
    q->output_p = node;
    IOQ_BYTES_EXPECTED(q, i);
    fail_unless(i == sizeof("baba1")*4 - 4, "IOQ_BYTES_EXPECTED : %d/%d", i, sizeof("baba1")*4 - 4 );
    fail_if(ioq_write_nv(q, 1) < 0, "ioq_write_nv");
    fail_unless(IOQ_NODES_READY(q) == 0, "IOQ_NODES_READY : %d/%d", IOQ_NODES_READY(q), 0);
    fail_unless(IOQ_EMPTY(q), "IOQ_EMPTY : %d/%d", IOQ_EMPTY(q), 0);

    return;

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
