/*******************************************************************************
 * sts.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: sts.h 1925 2011-09-17 15:55:45Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _STS_H_
#define _STS_H_ 1

STL_BEGIN_DECLS

stlStatus stsGetCpuInfo( stlCpuInfo    * aCpuInfo,
                         stlErrorStack * aErrorStack );

stlStatus stsGetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack );

stlStatus stsSetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack );

STL_END_DECLS

#endif /* _STS_H_ */
