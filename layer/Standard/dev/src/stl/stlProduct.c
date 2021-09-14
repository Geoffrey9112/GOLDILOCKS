/*******************************************************************************
 * stlProduct.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stlProduct.c.in 6772 2012-12-18 09:10:39Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file stlGeneral.c
 * @brief Standard Layer General Routines
 */

#include <stlDef.h>
#include <stlMemorys.h>
#include <stlSignal.h>
#include <stlError.h>
#include <ste.h>
#include <stlStrings.h>
#include <stlProduct.h>

/**
 * @addtogroup stlProduct
 * @{
 */

/**
 * @brief Stdout으로 Copyright 메세지를 출력한다.
 * @return Copyright String
 */
void stlShowCopyright()
{
    stlPrintf( " %s\n", STL_LEGAL_COPYRIGHT );
}

/**
 * @brief Stdout으로 Version 메세지를 출력한다.
 * @return Copyright String
 */
void stlShowVersionInfo()
{

#ifdef STL_DEBUG
    stlPrintf( " %s ", "Debug" );
#else
    stlPrintf( " %s ", "Release" );
#endif

    stlPrintf( "%s.%d.%d.%d revision(%s)\n",
               STL_PRODUCT_VERSION,
               STL_MAJOR_VERSION,
               STL_MINOR_VERSION,
               STL_PATCH_VERSION,
               STL_SOURCE_SHORT_REVISION );
}

/**
 * @brief Buffer 에 Version 메세지를 생성한다.
 * @param[out]  aBuffer     String Buffer
 * @param[in]   aBufferSize Buffer Size 
 * @return Version Information String
 */
void stlGetVersionString( stlChar * aBuffer,
                          stlInt32  aBufferSize )
{

#ifdef STL_DEBUG
    stlSnprintf( aBuffer,
                 aBufferSize,
                 "Debug %s.%d.%d.%d revision(%s)",
                 STL_PRODUCT_VERSION,
                 STL_MAJOR_VERSION,
                 STL_MINOR_VERSION,
                 STL_PATCH_VERSION,
                 STL_SOURCE_SHORT_REVISION );
#else
    stlSnprintf( aBuffer,
                 aBufferSize,
                 "Release %s.%d.%d.%d revision(%s)",
                 STL_PRODUCT_VERSION,
                 STL_MAJOR_VERSION,
                 STL_MINOR_VERSION,
                 STL_PATCH_VERSION,
                 STL_SOURCE_SHORT_REVISION );
#endif

}

/**
 * @brief Gliese Version 정보를 얻는다.
 * @param[out]  aVersionInfo  Version Information Pointer
 * @return Version Information String
 */
void stlGetVersionInfo( stlVersionInfo * aVersionInfo )
{
    stlMemset( aVersionInfo, 0x00, STL_SIZEOF(stlVersionInfo) );
    stlMemcpy( aVersionInfo->mProduct,
               STL_PRODUCT_VERSION,
               STL_PRODUCT_VERSION_LENGTH );

    aVersionInfo->mMajor = STL_MAJOR_VERSION;
    aVersionInfo->mMinor = STL_MINOR_VERSION;
    aVersionInfo->mPatch = STL_PATCH_VERSION;

    stlMemcpy( aVersionInfo->mRevision,
               STL_SOURCE_REVISION,
               STL_SOURCE_REVISION_LENGTH );

    stlMemcpy( aVersionInfo->mShortRevision,
               STL_SOURCE_SHORT_REVISION,
               STL_SOURCE_SHORT_REVISION_LENGTH );
}

/**
 * @brief 주어진 2개의 Gliese Version이 일치하는지 검사한다.
 * @param[in]  aVersionInfoA  Version Information A
 * @param[in]  aVersionInfoB  Version Information B
 * @return Version Information String
 */
stlBool stlMatchVersion( stlVersionInfo * aVersionInfoA,
                         stlVersionInfo * aVersionInfoB )
{
    stlBool sResult = STL_TRUE;
    
    if( stlStrncmp( aVersionInfoA->mProduct,
                    aVersionInfoB->mProduct,
                    STL_PRODUCT_VERSION_LENGTH ) != 0 )
    {
        sResult = STL_FALSE;
    }

    if( aVersionInfoA->mMajor != aVersionInfoB->mMajor )
    {
        sResult = STL_FALSE;
    }
    
    if( aVersionInfoA->mMinor != aVersionInfoB->mMinor )
    {
        sResult = STL_FALSE;
    }
    
    if( aVersionInfoA->mPatch != aVersionInfoB->mPatch )
    {
        sResult = STL_FALSE;
    }
    
    if( stlStrncmp( aVersionInfoA->mRevision, 
                    aVersionInfoB->mRevision, 
                    STL_SOURCE_REVISION_LENGTH ) != 0 ) 
    { 
        sResult = STL_FALSE; 
    } 
 
    if( stlStrncmp( aVersionInfoA->mShortRevision, 
                    aVersionInfoB->mShortRevision, 
                    STL_SOURCE_SHORT_REVISION_LENGTH ) != 0 ) 
    {
        sResult = STL_FALSE;
    }

    return sResult;
}

/** @} */
