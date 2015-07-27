
lib_LTLIBRARIES = libheliostv.la
libheliostv_la_SOURCES = ControlChannel.cpp DataChannel.cpp IChannelFactory.cpp ITransportClient.cpp Stream.cpp TcpChannelFactory.cpp TcpClient.cpp

libheliostv_la_CPPFLAGS = -I$(top_srcdir)/include $(BOOST_CPPFLAGS)
libheliostv_la_LIBADD = $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB) 
libheliostv_la_LDFLAGS = $(BOOST_LDFLAGS)
