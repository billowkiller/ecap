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
	RequestHandler.h\
	parser.h\
	urlparser.h\
	qs_parse.h\
	acsm.h\
	SQLFactory.h\
	StrMatcher.h\
	RuleAccepter.h\
	EventFuncs.h\
	time_utility.h\
	ConfigEvent.h\
	ConfigTimer.h\
	Monitor.h\
	autoconf.h 

# request
ecap_request_la_SOURCES = ReqXaction.cc \
				Debugger.cc\
				RequestHandler.cc\
				parser.cc\
				urlparser.cc\
				qs_parse.cc\
				KeywordHandler.cc\
				Log.cc\
				SQLFactory.cc\
				DBHandler.cc\
				RuleHandler.cc\
				StrMatcher.cc\
				RuleAccepter.cc\
				acsmx.cc\
				Pz_id.cc\
				Pz_kw.cc\
				EventFuncs.cc\
				time_utility.cc\
				ConfigEvent.cc\
				ConfigTimer.cc\
				Monitor.cc
				
				
				

ecap_request_la_LDFLAGS = -module -avoid-version $(libecap_LIBS)
ecap_request_la_LIBADD= $(LIBECAP_LIBS) -lglib-2.0 -lsqlite3 -lboost_thread -lrt -lboost_system  -lboost_thread

# response
ecap_response_la_SOURCES = RespXaction.cc \
	Service.cc \
	Debugger.cc \
	Gzipper.cc \
	SubsFilter.cc 

ecap_response_la_LDFLAGS = -module -avoid-version $(libecap_LIBS)
ecap_response_la_LIBADD= $(LIBECAP_LIBS) -lz -lboost_system -lboost_thread

# -shared -export-dynamic -Wl,-soname,ecap_noop_adapter.so

DISTCLEANFILES = \
        autoconf.h

AM_CPPFLAGS = -I$(top_srcdir)/src

