#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([heliostv], [0.1.0])
AM_INIT_AUTOMAKE([-Wall foreign])

GST_REQUIRED=1.0.0

LT_PREREQ([2.2])
LT_INIT()

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([libheliostv/ControlChannel.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AX_CXX_COMPILE_STDCXX_11()
AX_BOOST_BASE([1.48],, [AC_MSG_ERROR([libfoo needs Boost, but it was not found in your system])])
AX_BOOST_SYSTEM
AX_BOOST_THREAD

# Checks for header files.
#AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/param.h unistd.h stdbool.h])

# check for libraries
AC_CHECK_LIB([msgpack], [msgpack_unpack])
#PKG_CHECK_MODULES([MSGPACK], [msgpackcpputest >= 3.6], 
#      [have_cpputest=yes], [have_cpputest=no])


PKG_CHECK_MODULES(GST, [
  gstreamer-1.0 >= $GST_REQUIRED
  gstreamer-base-1.0 >= $GST_REQUIRED
  gstreamer-controller-1.0 >= $GST_REQUIRED
], [
  AC_SUBST(GST_CFLAGS)
  AC_SUBST(GST_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the GStreamer development
      packages on your system. On debian-based systems these are
      libgstreamer1.0-dev and libgstreamer-plugins-base1.0-dev.
      on RPM-based systems gstreamer1.0-devel, libgstreamer1.0-devel
      or similar. The minimum version required is $GST_REQUIRED.
  ])
])


AC_CONFIG_FILES([Makefile src/Makefile include/Makefile libheliostv/Makefile])

AC_OUTPUT

echo "
$PACKAGE_NAME version $PACKAGE_VERSION :
Prefix.........: $prefix
Debug Build....: $debug
C++ Compiler...: $CXX $CXXFLAGS $CPPFLAGS
Linker.........: $LD $LDFLAGS $LIBS"

