lib_LTLIBRARIES = \
	ecap_adapter_fbac.la

noinst_HEADERS = \
	sample.h \
	header.h \
	Debugger.h\
	Gzipper.h\
	SubsFilter.h\
	BufferString.h\
	\
	autoconf.h 

# minimal
ecap_adapter_fbac_la_SOURCES = Xaction.cc \
	Service.cc \
	Debugger.cc \
	Gzipper.cc \
	SubsFilter.cc 

ecap_adapter_fbac_la_LDFLAGS = -module -avoid-version $(libecap_LIBS)
ecap_adapter_fbac_la_LIBADD= $(LIBECAP_LIBS) -lz

# -shared -export-dynamic -Wl,-soname,ecap_noop_adapter.so

DISTCLEANFILES = \
        autoconf.h

AM_CPPFLAGS = -I$(top_srcdir)/src

