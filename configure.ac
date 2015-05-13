#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([heliostv], [0.1.0])
AM_INIT_AUTOMAKE([-Wall foreign])

LT_PREREQ([2.2])
LT_INIT()

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([libheliostv/ControlChannel.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for header files.
#AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/param.h unistd.h stdbool.h])

# check for libraries
AC_CHECK_LIB([msgpack], [msgpack_unpack])
#PKG_CHECK_MODULES([MSGPACK], [msgpackcpputest >= 3.6], 
#      [have_cpputest=yes], [have_cpputest=no])


AC_CONFIG_FILES([Makefile src/Makefile include/Makefile libheliostv/Makefile])

AC_OUTPUT

echo "
$PACKAGE_NAME version $PACKAGE_VERSION :
Prefix.........: $prefix
Debug Build....: $debug
C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
Linker.........: $LD $LDFLAGS $LIBS"

