/* Copyright (c) 2005-2011-2012 by Tensilica Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Tensilica Inc. and its licensors, and are licensed to the recipient
 * under the terms of a separate license agreement.  They may be
 * adapted and modified by bona fide purchasers under the terms of the
 * separate license agreement for internal use, but no adapted or
 * modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the
 * prior written consent of Tensilica Inc.
 */

/*****************************************************************************/
/*                                                                           */
/*  File Name        : xa_api_defs.h                                         */
/*                                                                           */
/*  Description      : API version related constant hash defines             */
/*                                                                           */
/*  List of Functions: None                                                  */
/*                                                                           */
/*  Issues / Problems: None                                                  */
/*                                                                           */
/*  Revision History :                                                       */
/*                                                                           */
/*        DD MM YYYY       Author                Changes                     */
/*        29 07 2005       Tejaswi/Vishal        Created                     */
/*                                                                           */
/*****************************************************************************/

#ifndef __XA_API_DEFS_H__
#define __XA_API_DEFS_H__

/*****************************************************************************/
/* Constant hash defines                                                     */
/*****************************************************************************/
/* A constant to let API copy small strings to buffers outside */
#define XA_API_STR_LEN          30
#define XA_APIVERSION_MAJOR 1
#define XA_APIVERSION_MINOR 12

/* last compatible version */
/* sometimes a new API version is just for a bugfix, or a added feature  in */
/* this case it is better to use a newer version even though a library  was */
/* made for an older version, library API can then be upgraded to newer API */
/* version after checking for compatibility or by adding features           */
#define XA_LASTCOMP_APIVERSION_MAJOR        1
#define XA_LASTCOMP_APIVERSION_MINOR        10

#define XA_STR(str)                         str
#define XA_MAKE_VERSION_STR(maj, min)       XA_STR(maj) "." XA_STR(min)
#define XA_APIVERSION                       XA_MAKE_VERSION_STR(\
                                            XA_APIVERSION_MAJOR, \
                                            XA_APIVERSION_MINOR)

#define XA_LAST_COMP_APIVERSION             XA_MAKE_VERSION_STR(\
                                            XA_LASTCOMP_APIVERSION_MAJOR, \
                                            XA_LASTCOMP_APIVERSION_MINOR)

#endif /* __XA_API_DEFS_H__ */
