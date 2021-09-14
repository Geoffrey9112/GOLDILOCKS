/*******************************************************************************
 * qlpParserMacro.c
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

/**
 * @file qlpParserMacro.c
 * @brief SQL Processor Layer Parser macro
 */


#include <qlpParserMacro.h>

/**
 * @addtogroup qlpParser
 * @{
 */



/**
 * @brief parser에서 사용할 memory allocator
 * @param[in]     aContext     parser의 context 정보 
 * @param[in]     aAllocSize   공간 할당 받을 크기 
 * @param[in,out] aAddr        할당 받은 공간 포인터
 * @param[in,out] aErrorStack  에러 스택
 */
stlStatus qlpAllocator( void            * aContext,
                        stlInt32          aAllocSize,
                        void           ** aAddr,
                        stlErrorStack   * aErrorStack )
{
    STL_TRY( knlAllocRegionMem( ((qlpParseContext*)aContext)->mMemMgr,
                                aAllocSize,
                                aAddr,
                                KNL_ENV( ((qlpParseContext*)aContext)->mEnv ) )
             == STL_SUCCESS );

    stlMemset( *aAddr, 0x00, aAllocSize );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlChar *qlpMakeHostVariable( stlParseParam * aParam )
{
    stlInt32   sToken;
    stlChar  * sHostIdentifier = NULL;

    sToken = qlpParserGetNextToken( aParam, aParam->mScanner );
    STL_TRY( qlpParserIsIdentifier( sToken,
                                    aParam->mBuffer + aParam->mCurrLoc,
                                    aParam->mLength )
             == STL_TRUE );

    STL_TRY( aParam->mAllocator( aParam->mContext,
                                 aParam->mLength + 1,
                                 (void**)&sHostIdentifier,
                                 aParam->mErrorStack )
             == STL_SUCCESS );

    stlStrncpy( sHostIdentifier,
                (const stlChar*)aParam->mBuffer + aParam->mCurrLoc,
                aParam->mLength + 1 );

    return sHostIdentifier;

    STL_FINISH;

    aParam->mErrStatus = STL_FAILURE;

    return NULL;
}




/** @} qlpParser */


