/* ctx.c
** strophe XMPP client library -- run-time context implementation
**
** Copyright (C) 2005-2009 Collecta, Inc. 
**
**  This software is provided AS-IS with no warranty, either express 
**  or implied.
**
**  This software is distributed under license and may not be copied,
**  modified or distributed except as expressly authorized under the
**  terms of the license contained in the file LICENSE.txt in this
**  distribution.
*/

/** @file
 *  Runtime contexts, library initialization and shutdown, and versioning.
 */

/** @defgroup Context Context objects
 *  These functions create and manipulate Strophe context objects.
 *
 *  In order to support usage in a variety of environments, the
 *  Strophe library uses a runtime context object.  This object
 *  contains the information on how to do memory allocation and
 *  logging.  This allows the user to control how memory is allocated
 *  and what do to with log messages.
 *
 *  These issues do not affect programs in the common case, but many
 *  environments require special treatment.  Abstracting these into a runtime
 *  context object makes it easy to use Strophe on embedded platforms.
 *
 *  Objects in Strophe are reference counted to ease memory management issues,
 *  but the context objects are not.
 */

/** @defgroup Init Initialization, shutdown, and versioning
 *  These functions initialize and shutdown the library, and also allow
 *  for API version checking.  Failure to properly call these functions may
 *  result in strange (and platform dependent) behavior.
 *
 *  Specifically, the socket library on Win32 platforms must be initialized
 *  before use (although this is not the case on POSIX systems).  The TLS 
 *  subsystem must also seed the random number generator.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define OCCAM_STANDARD_ALLOCATOR
#include "strophe.h"
#include "common.h"
#include "util.h"

/** Initialize the Strophe library.
 *  This function initializes subcomponents of the Strophe library and must
 *  be called for Strophe to operate correctly.
 *
 *  @ingroup Init
 */
 void xmpp_initialize(void)
{
    sock_initialize();
    tls_initialize();
}

/** Shutdown the Strophe library.
 *
 *  @ingroup Init
 */
void xmpp_shutdown(void)
{
    tls_shutdown();
    sock_shutdown();
}

/* version information */

#ifndef LIBXMPP_VERSION_MAJOR
/** @def LIBXMPP_VERSION_MAJOR
 *  The major version number of Strophe.
 */
#define LIBXMPP_VERSION_MAJOR (0)
#endif
#ifndef LIBXMPP_VERSION_MINOR
/** @def LIBXMPP_VERSION_MINOR
 *  The minor version number of Strophe.
 */
#define LIBXMPP_VERSION_MINOR (0)
#endif

/** Check that Strophe supports a specific API version.
 *
 *  @param major the major version number
 *  @param minor the minor version number
 *
 *  @return TRUE if the version is supported and FALSE if unsupported
 *
 *  @ingroup Init
 */
int xmpp_version_check(int major, int minor)
{
    return (major == LIBXMPP_VERSION_MAJOR) &&
	   (minor >= LIBXMPP_VERSION_MINOR);
}

/* log levels and names */
static const occam_log_level_t _xmpp_default_logger_levels[] = {
   OCCAM_LOG_LEVEL_TRACE,
   OCCAM_LOG_LEVEL_DEBUG,
   OCCAM_LOG_LEVEL_INFO,
   OCCAM_LOG_LEVEL_WARN,
   OCCAM_LOG_LEVEL_ERROR
};

/** Log a message.
 *  The default logger writes to stderr.
 *
 *  @param userdata the opaque data used by the default logger.  This contains
 *      the filter level in the default logger.
 *  @param level the level to log at
 *  @param area the area the log message is for
 *  @param msg the log message
 */
void xmpp_default_logger(void * userdata,
			 const occam_log_level_t level,
			 const char * area,
			 const char * fmt, va_list args)
{
   occam_log_level_t filter_level = * (occam_log_level_t*)userdata;
   if(level >= filter_level)
   {
      fprintf(stderr, "%s %s ", area, occam_log_level_name(level));
      vfprintf(stderr, fmt, args);
      fprintf(stderr, "\n");
   }
}

static const occam_logger_t _xmpp_default_loggers[] = {
  {&xmpp_default_logger, (void*)&_xmpp_default_logger_levels[OCCAM_LOG_LEVEL_TRACE]},
  {&xmpp_default_logger, (void*)&_xmpp_default_logger_levels[OCCAM_LOG_LEVEL_DEBUG]},
	{&xmpp_default_logger, (void*)&_xmpp_default_logger_levels[OCCAM_LOG_LEVEL_INFO]},
	{&xmpp_default_logger, (void*)&_xmpp_default_logger_levels[OCCAM_LOG_LEVEL_WARN]},
	{&xmpp_default_logger, (void*)&_xmpp_default_logger_levels[OCCAM_LOG_LEVEL_ERROR]}
};

/** Get a default logger with filtering.
 *  The default logger provides a basic logging setup which writes log
 *  messages to stderr.  Only messages where level is greater than or
 *  equal to the filter level will be logged.
 *
 *  @param level the highest level the logger will log at
 *
 *  @return the log structure for the given level
 *
 *  @ingroup Context
 */
occam_logger_t *xmpp_get_default_logger(occam_log_level_t level)
{
    /* clamp to the known range */
    if (level > OCCAM_LOG_LEVEL_ERROR) level = OCCAM_LOG_LEVEL_ERROR;
    if (level < OCCAM_LOG_LEVEL_DEBUG) level = OCCAM_LOG_LEVEL_DEBUG;

    return (occam_logger_t*)&_xmpp_default_loggers[level];
}

/* convenience functions for accessing the context */

/** Allocate memory in a Strophe context.
 *  All Strophe functions will use this to allocate memory. 
 *
 *  @param ctx a Strophe context object
 *  @param size the number of bytes to allocate
 *
 *  @return a pointer to the allocated memory or NULL on an error
 */
void *xmpp_alloc(const xmpp_ctx_t * const ctx, const size_t size)
{
    return occam_alloc(ctx->mem, size);
}

/** Free memory in a Strophe context.
 *  All Strophe functions will use this to free allocated memory.
 *
 *  @param ctx a Strophe context object
 *  @param p a pointer referencing memory to be freed
 */
void xmpp_free(const xmpp_ctx_t * const ctx, void *p)
{
   occam_free(ctx->mem, p);
}

/** Reallocate memory in a Strophe context.
 *  All Strophe functions will use this to reallocate memory.
 *
 *  @param ctx a Strophe context object
 *  @param p a pointer to previously allocated memory
 *  @param size the new size in bytes to allocate
 *
 *  @return a pointer to the reallocated memory or NULL on an error
 */
void *xmpp_realloc(const xmpp_ctx_t * const ctx, void *p, const size_t size)
{
   return occam_realloc(ctx->mem, p, size);
}

/** Write to the log at the ERROR level.
 *  This is a convenience function for writing to the log at the
 *  ERROR level.  It takes a printf-style format string followed by a 
 *  variable list of arguments for formatting.
 *
 *  @param ctx a Strophe context object
 *  @param area the area to log for
 *  @param fmt a printf-style format string followed by a variable list of
 *      arguments to format
 */
void xmpp_error(const xmpp_ctx_t * const ctx,
                const char * const area,
                const char * const fmt,
                ...)
{
    va_list ap;

    va_start(ap, fmt);
    occam_logv(ctx->log, OCCAM_LOG_LEVEL_ERROR, area, fmt, ap);
    va_end(ap);
}

/** Write to the log at the WARN level.
 *  This is a convenience function for writing to the log at the WARN level.
 *  It takes a printf-style format string followed by a variable list of
 *  arguments for formatting.
 *
 *  @param ctx a Strophe context object
 *  @param area the area to log for
 *  @param fmt a printf-style format string followed by a variable list of
 *      arguments to format
 */
void xmpp_warn(const xmpp_ctx_t * const ctx,
                const char * const area,
                const char * const fmt,
                ...)
{
    va_list ap;

    va_start(ap, fmt);
    occam_logv(ctx->log, OCCAM_LOG_LEVEL_WARN, area, fmt, ap);
    va_end(ap);
}

/** Write to the log at the INFO level.
 *  This is a convenience function for writing to the log at the INFO level.
 *  It takes a printf-style format string followed by a variable list of
 *  arguments for formatting.
 *
 *  @param ctx a Strophe context object
 *  @param area the area to log for
 *  @param fmt a printf-style format string followed by a variable list of
 *      arguments to format
 */
void xmpp_info(const xmpp_ctx_t * const ctx,
                const char * const area,
                const char * const fmt,
                ...)
{
    va_list ap;

    va_start(ap, fmt);
    occam_logv(ctx->log, OCCAM_LOG_LEVEL_INFO, area, fmt, ap);
    va_end(ap);
}

/** Write to the log at the DEBUG level.
 *  This is a convenience function for writing to the log at the DEBUG level.
 *  It takes a printf-style format string followed by a variable list of
 *  arguments for formatting.
 *
 *  @param ctx a Strophe context object
 *  @param area the area to log for
 *  @param fmt a printf-style format string followed by a variable list of
 *      arguments to format
 */
void xmpp_debug(const xmpp_ctx_t * const ctx,
                const char * const area,
                const char * const fmt,
                ...)
{
    va_list ap;

    va_start(ap, fmt);
    occam_logv(ctx->log, OCCAM_LOG_LEVEL_DEBUG, area, fmt, ap);
    va_end(ap);
}

/** Create and initialize a Strophe context object.
 *  If mem is NULL, a default allocation setup will be used which
 *  wraps malloc(), free(), and realloc() from the standard library.
 *  If log is NULL, a default logger will be used which does no
 *  logging.  Basic filtered logging to stderr can be done with the
 *  xmpp_get_default_logger() convenience function.
 *
 *  @param mem a pointer to an xmpp_mem_t structure or NULL
 *  @param log a pointer to an xmpp_log_t structure or NULL
 *
 *  @return the allocated Strophe context object or NULL on an error
 *
 *  @ingroup Context
 */
xmpp_ctx_t *xmpp_ctx_new(const occam_allocator_t * const mem, 
         const occam_logger_t * const log)
{
   xmpp_ctx_t *ctx = NULL;

   if (mem == NULL)
      ctx = occam_alloc(&occam_standard_allocator, sizeof(xmpp_ctx_t));
   else
      ctx = occam_alloc(mem, sizeof(xmpp_ctx_t));

   if (ctx != NULL) {
	if (mem != NULL) 
	    ctx->mem = mem;
	else 
	    ctx->mem = &occam_standard_allocator;

   ctx->log = log;

	ctx->connlist = NULL;
	ctx->loop_status = XMPP_LOOP_NOTSTARTED;
    }

    return ctx;
}

/** Free a Strophe context object that is no longer in use.
 *
 *  @param ctx a Strophe context object
 *
 *  @ingroup Context
 */
void xmpp_ctx_free(xmpp_ctx_t * const ctx)
{
    /* mem and log are owned by their suppliers */
    xmpp_free(ctx, ctx); /* pull the hole in after us */
}

