AC_PREREQ([2.59])
AC_INIT([ioqueue], [1.0], [roey.berman@gmail.com])
AC_CONFIG_SRCDIR([src/ioqueue.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_ARG_ENABLE(tests,[  --enable-tests          build tests],[ 
	PKG_CHECK_MODULES([CHECK], [check >= 0.9.4]) 
	build_tests=true
]
)

AM_CONDITIONAL([BUILD_TESTS], [test x$build_tests = xtrue ])

LT_INIT

AC_SUBST(LIBTOOL_DEPS)

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_CC_C_O

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h arrayqueue.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])

AC_CONFIG_MACRO_DIR([m4])

AC_OUTPUT
