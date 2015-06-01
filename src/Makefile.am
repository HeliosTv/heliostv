
bin_PROGRAMS = heliostv

heliostv_SOURCES = \
    main.cpp \
    pipeline.cpp

heliostv_CPPFLAGS = -I$(topdir)∕include $(GST_CFLAGS) $(BOOST_CPPFLAGS) $(GIO_CFLAGS)
#heliostv_LDFLAGS = $(BOOST_LDFLAGS)
heliostv_LDADD = $(GST_LIBS) $(BOOST_THREAD_LIB) $(BOOST_SYSTEM_LIB) $(GIO_LIBS)


