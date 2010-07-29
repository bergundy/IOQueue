#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([ioqueue], [0.1], [roey.berman@gmail.com])
AC_CONFIG_SRCDIR([src/ioqueue.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([color-tests foreign -Wall -Werror])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

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

# Checks for libraries.

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
