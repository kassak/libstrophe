cmake_minimum_required (VERSION 2.6)
project (libstrophe)

if(WIN32)
  set(NWIN32 OFF)
else(WIN32)
  set(NWIN32 ON)
endif(WIN32)

OPTION(USE_GNUTLS "Use gnutls" OFF)
OPTION(USE_DUMMYTLS "Use dummy tls" OFF)
OPTION(USE_OPENSSL "Use openssl for tls" ${NWIN32})
OPTION(USE_WINSSL "Use windows ssl for tls" WIN32)

OPTION(USE_LIBXML2 "Use libxml2 parser" ON)
OPTION(USE_LIBEXPAT "Use expat parser" OFF)

include_directories(.)

set (SOURCES
  src/auth.c
  src/conn.c
  src/ctx.c
  src/event.c
  src/handler.c
  src/hash.c
  src/jid.c
  src/md5.c
  src/oocontext.cpp
  src/oostanza.cpp
#  src/parser_expat.c
#  src/parser_libxml2.c
  src/sasl.c
  src/sha1.c
  src/sock.c
  src/stanza.c
  src/thread.c
#  src/tls_dummy.c
#  src/tls_gnutls.c
#  src/tls_openssl.c
#  src/tls_schannel.c
  src/util.c
)

if(USE_LIBEXPAT)
  set (SOURCES ${SOURCES} src/parser_expat.c)
elseif(USE_LIBXML2)
  set (SOURCES ${SOURCES} src/parser_libxml2.c)
endif(USE_LIBEXPAT)

if(USE_WINSSL)
  set (SOURCES ${SOURCES} src/tls_schannel.c)
elseif(USE_OPENSSL)
  set (SOURCES ${SOURCES} src/tls_openssl.c)
elseif(USE_GNUTLS)
  set (SOURCES ${SOURCES} src/tls_gnutls.c)
else(USE_WINSSL)
  set (SOURCES ${SOURCES} src/tls_dummy.c)
endif(USE_WINSSL)

message(${SOURCES})

add_library (libstrophe SHARED ${SOURCES})

if(USE_LIBEXPAT)
  find_package(LibExpat REQUIRED)
  include_directories(${LIBEXPAT_INCLUDE_DIR})
  target_link_libraries(libstrophe ${LIBEXPAT_LIBRARIES})
elseif(USE_LIBXML2)
  find_package(LibXml2 REQUIRED)
  include_directories(${LIBXML2_INCLUDE_DIR})
  target_link_libraries(libstrophe ${LIBXML2_LIBRARIES})
endif(USE_LIBEXPAT)

if(USE_OPENSSL)
  find_package(OpenSSL REQUIRED)
  include_directories(${OPENSSL_INCLUDE_DIR})
  target_link_libraries(libstrophe ${OPENSSL_LIBRARIES})
elseif(USE_GNUTLS)
  find_package(GnuTLS REQUIRED)
  include_directories(${GNUTLS_INCLUDE_DIR})
  target_link_libraries(libstrophe ${GNUTLS_LIBRARIES})
endif(USE_OPENSSL)
