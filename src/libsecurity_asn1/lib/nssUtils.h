/*
 * Copyright (c) 2003-2004 Apple Computer, Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 *
 * nssUtils.h
 */
 
#ifndef	_NSS_UTILS_H_
#define _NSS_UTILS_H_

#include <Security/cssmtype.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* return true if two CSSM_DATAs (or two CSSM_OIDs) compare equal */
int nssCompareCssmData(
	const CSSM_DATA *data1,
	const CSSM_DATA *data2);

/* How many items in a NULL-terminated array of pointers? */
size_t nssArraySize(
	const void **array);

#ifdef __cplusplus
}
#endif

#endif	/* _NSS_UTILS_H_ */

