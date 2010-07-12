TESTS = ioqueue.t
check_PROGRAMS = $(TESTS)

srcdir = $(top_builddir)/src
AM_CFLAGS = -I$(srcdir) 

ioqueue_t_SOURCES = check_ioqueue.c ioqueue.h
ioqueue_t_CFLAGS  = @CHECK_CFLAGS@ $(AM_CFLAGS)
ioqueue_t_LDADD   = @CHECK_LIBS@ $(srcdir)/ioqueue.o
