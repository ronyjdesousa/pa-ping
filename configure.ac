#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([paping], [1.6.0], [https://github.com/ronyjdesousa/paping/issues])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname gettimeofday inet_ntoa select socket])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 src/Makefile])

AM_INIT_AUTOMAKE([foreign])

AC_OUTPUT
