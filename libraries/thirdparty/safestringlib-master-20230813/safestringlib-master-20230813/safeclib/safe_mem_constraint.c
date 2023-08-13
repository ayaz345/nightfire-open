/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2012 Jonathan Toppins <jtoppins@users.sourceforge.net>
 *  Copyright (c) 2008-2012 Cisco Systems
 *  Copyright (c) 2018 Intel Corp
 */

#include "safeclib_private.h"
#include "safe_mem_constraint.h"
#include "safe_mem_lib.h"

static constraint_handler_t mem_handler = NULL;

/**
 * NAME
 *    set_mem_constraint_handler_s
 *
 * SYNOPSIS
 *    #include "safe_mem_lib.h"
 *    constraint_handler_t
 *    set_mem_constraint_handler_straint_handler_t handler)
 *
 * DESCRIPTION
 *    The set_mem_constraint_handler_s function sets the runtime-constraint
 *    handler to be handler. The runtime-constraint handler is the function to
 *    be called when a library function detects a runtime-constraint
 *   order:
 *        1.    A pointer to a character string describing the
 *              runtime-constraint violation.
 *        2.    A null pointer or a pointer to an implementation defined
 *              object.
 *        3.    If the function calling the handler has a return type declared
 *              as errno_t, the return value of the function is passed.
 *              Otherwise, a positive value of type errno_t is passed.
 *    The implementation has a default constraint handler that is used if no
 *    calls to the set_constraint_handler_s function have been made. The
 *    behavior of the default handler is implementation-defined, and it may
 *    cause the program to exit or abort.  If the handler argument to
 *    set_constraint_handler_s is a null pointer, the implementation default
 *    handler becomes the current constraint handler.
 *
 * SPECIFIED IN
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *   *msg            Pointer to the message describing the error
 *
 *   *ptr            Pointer to aassociated data.  Can be NULL.
 *
 *    error          The error code encountered.
 *
 * OUTPUT PARAMETERS
 *    none
 *
 * RETURN VALUE
 *    none
 *
 * ALSO SEE
 *    set_str_constraint_handler_s()
 */
constraint_handler_t
set_mem_constraint_handler_s (constraint_handler_t handler)
{
    constraint_handler_t prev_handler = mem_handler;
    if (NULL == handler) {
        mem_handler = sl_default_handler;
    } else {
        mem_handler = handler;
    }
    return prev_handler;
}
EXPORT_SYMBOL(set_mem_constraint_handler_s)


/**
 * NAME
 *    invoke_safe_mem_constraint_handler
 *
 * SYNOPSIS
 *    #include "safe_mem_constraint.h"
 *    void
 *    invoke_safe_mem_constraint_handler(const char *msg,
 *                                void *ptr,
 *                                errno_t error)
 *
 * DESCRIPTION
 *    Invokes the currently set constraint handler or the default.
 *
 * INPUT PARAMETERS
 *   *msg            Pointer to the message describing the error
 *
 *   *ptr            Pointer to aassociated data.  Can be NULL.
 *
 *    error          The error code encountered.
 *
 * OUTPUT PARAMETERS
 *    none
 *
 * RETURN VALUE
 *    none
 *
 */
void
invoke_safe_mem_constraint_handler (const char *msg,
                                    void *ptr,
                                    errno_t error)
{
    if (NULL != mem_handler) {
        mem_handler(msg, ptr, error);
    } else {
        sl_default_handler(msg, ptr, error);
    }
}
