/*******************************************************************************
 * ellHashValue.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 *
 ******************************************************************************/


#ifndef _ELL_HASH_VALUE_H_
#define _ELL_HASH_VALUE_H_ 1

/**
 * @file ellHashValue.h
 * @brief Execution Layer 의 Hash Value 관리 
 */

/**
 * @defgroup ellHashValue Hash Value 관리 
 * @ingroup elExternal
 * @{
 */

#define ellROT(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
 * Hash Value 생성 함수
 */

stlUInt32  ellHashInt64( stlInt64 aID );
stlUInt32  ellHashString( stlChar * aName );

stlUInt32  ellGetHashValueOneID( stlInt64 aObjID );
stlUInt32  ellGetHashValueTwoID( stlInt64 aObjID1, stlInt64 aObjID2 );
stlUInt32  ellGetHashValueOneName( stlChar * aObjName );
stlUInt32  ellGetHashValueNameAndID( stlInt64 aObjID, stlChar * aObjName );

stlUInt32  ellGetPrimeBucketCnt( stlInt64 aRecordCnt );

/** @} ellHashValue */

#endif /* _ELL_HASH_VALUE_H_ */
