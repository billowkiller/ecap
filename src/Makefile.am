lib_LTLIBRARIES = \
	ecap_response.la \
	ecap_request.la

noinst_HEADERS = \
	sample.h \
	RespHeader.h \
	Debugger.h\
	Gzipper.h\
	SubsFilter.h\
	BufferString.h\
	RuleHandler.h\
	RuleAccepter.h\
	\
	autoconf.h 

# request
ecap_request_la_SOURCES = ReqXaction.cc \
				Debugger.cc\
				RuleAccepter.cc

ecap_request_la_LDFLAGS = -module -avoid-version $(libecap_LIBS)
ecap_request_la_LIBADD= $(LIBECAP_LIBS) -lboost_system -lboost_thread

# response
ecap_response_la_SOURCES = RespXaction.cc \
	Service.cc \
	Debugger.cc \
	Gzipper.cc \
	SubsFilter.cc 

ecap_response_la_LDFLAGS = -module -avoid-version $(libecap_LIBS)
ecap_response_la_LIBADD= $(LIBECAP_LIBS) -lz -lboost_system

# -shared -export-dynamic -Wl,-soname,ecap_noop_adapter.so

DISTCLEANFILES = \
        autoconf.h

AM_CPPFLAGS = -I$(top_srcdir)/src
