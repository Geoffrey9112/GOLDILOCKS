/*******************************************************************************
 * ztppFunc.c
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

#include <stl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztppCommon.h>
#include <ztppFunc.h>
#include <ztppCalcFunc.h>
#include <ztppMacro.h>
#include <ztppExpandFile.h>
#include <ztpfPathControl.h>
#include <ztpuFile.h>

/**
 * @file ztppFunc.c
 * @brief C Preprocessor Parser Routines
 */

/**
 * @addtogroup ztppFunc
 * @{
 */

ztpArgNameList *ztppMakeArgNameList( ztpPPParseParam  *aParam,
                                     stlChar          *aArgName,
                                     ztpArgNameList   *aList )
{
    ztpArgNameList   *sArgNameList = NULL;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpArgNameList),
                              (void**)&sArgNameList,
                              aParam->mErrorStack)
            == STL_SUCCESS);
    sArgNameList->mArgName = aArgName;
    sArgNameList->mNext = aList;

    return sArgNameList;

    STL_FINISH;

    return NULL;
}

stlStatus ztppMakeIncludeHeaderFile( ztpPPParseParam *aParam,
                                     stlChar         *aHeaderFileName )
{
    stlBool             sFileExist = STL_FALSE;
    stlChar             sFileName[STL_MAX_FILE_NAME_LENGTH + 1];
    stlChar             sFilePath[STL_MAX_FILE_PATH_LENGTH + 1];

    STL_TRY( ztpfSearchFile( aParam->mInFileName,
                             aHeaderFileName,
                             sFileName,
                             sFilePath,
                             &sFileExist,
                             aParam->mErrorStack )
             == STL_SUCCESS );

    //STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );

    if(sFileExist == STL_TRUE)
    {
        STL_TRY( ztppExpandHeaderFileInternal( aParam->mAllocator,
                                               aParam->mErrorStack,
                                               aParam->mOutFile,
                                               sFilePath )
                 == STL_SUCCESS );
    }

    aParam->mErrorStatus = STL_SUCCESS;

    return STL_SUCCESS;

    /*
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_FILE_NOT_EXIST,
                      NULL,
                      aParam->mErrorStack,
                      sFileName );
    }
    */
    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return STL_FAILURE;
}

stlStatus ztppMakeMacro( ztpPPParseParam *aParam,
                         stlChar         *aMacroName,
                         stlInt32         aArgCount,
                         stlBool          aIsVarArgs,
                         ztpArgNameList  *aArgNameList,
                         stlChar         *aReplacementList )
{
    stlInt32         sMacroNameLen;
    stlInt32         sReplacementListLen;
    ztpMacroSymbol  *sMacroSymbol;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpMacroSymbol),
                              (void**)&sMacroSymbol,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    ZTP_INIT_MACRO_SYMBOL( sMacroSymbol );

    sMacroNameLen = stlStrlen( aMacroName );
    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              sMacroNameLen + 1,
                              (void**)&sMacroSymbol->mMacroName,
                              aParam->mErrorStack)
            == STL_SUCCESS);
    stlStrncpy( sMacroSymbol->mMacroName, aMacroName, sMacroNameLen + 1 );

    sMacroSymbol->mArgCount     = aArgCount;
    sMacroSymbol->mArgNameList  = aArgNameList;
    sMacroSymbol->mIsVarArgs    = aIsVarArgs;

    if( aReplacementList != NULL )
    {
        sReplacementListLen = stlStrlen( aReplacementList );
        STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                                  sReplacementListLen + 1,
                                  (void**)&sMacroSymbol->mContents,
                                  aParam->mErrorStack)
                == STL_SUCCESS);
        stlStrncpy( sMacroSymbol->mContents,
                    aReplacementList,
                    sReplacementListLen + 1 );
    }

    STL_TRY( ztppAddMacroSymbol( aParam->mErrorStack,
                                 sMacroSymbol )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztppMakeFunctionLikeMacro( ztpPPParseParam *aParam,
                                     stlChar         *aMacroName,
                                     stlBool          aIsVarArgs,
                                     ztpArgNameList  *aArgNameList,
                                     stlChar         *aReplacementList )
{
    const stlChar     *sPPDefine = "define";
    stlChar           *sMacroName = NULL;
    stlInt32           sMacroNameLen;
    stlInt32           sPPDefineLen;
    stlInt32           sArgCount = 0;
    ztpArgNameList    *sArgNameList = aArgNameList;
    stlChar           *sMacroContents;
    stlChar           *sBeginTokenPtr;
    ztpPPMacroTokenId  sMacroTokenId;

    /*
     * 주어진 Macro name은
     * <white-space><#><white-space><define><white-space><identifier><'('>
     * 과 같은 형식으로 입력된다.
     * 여기서 <identifier> 부분만 Macro name으로 추출한다.
     */
    sPPDefineLen = stlStrlen( sPPDefine );
    sBeginTokenPtr = stlStrstr( aMacroName, sPPDefine );

    STL_DASSERT( sBeginTokenPtr != NULL );

    sBeginTokenPtr += sPPDefineLen;

    /*
     * LTRIM white-space
     */
    while( stlIsspace( *sBeginTokenPtr ) != 0 )
    {
        sBeginTokenPtr ++;
    }

    /*
     * 마지막에 붙은 '('는 제외한다.
     */
    sMacroNameLen = stlStrlen( sBeginTokenPtr ) - 1;
    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              sMacroNameLen + 1,
                              (void**)&sMacroName,
                              aParam->mErrorStack)
            == STL_SUCCESS);
    stlStrncpy( sMacroName, sBeginTokenPtr, sMacroNameLen + 1 );

    sMacroContents = aReplacementList;
    stlMemset( aParam->mMacroReplaceBuffer, 0x00, ZTP_MACRO_REPLACE_BUFFER_SIZE );

    if( sMacroContents != NULL )
    {
        sMacroTokenId = ZTP_MACRO_TOKEN_MAX;
        //stlPrintf( "Macro Name: %s\nMacro contents: %s\n", sMacroName, aReplacementList );

        while( sMacroTokenId != ZTP_MACRO_TOKEN_NULL )
        {
            sBeginTokenPtr = ztppScanToken( &sMacroContents,
                                            &sMacroTokenId );

            stlMemset( aParam->mMacroTokenBuffer, 0x00, ZTP_MACRO_REPLACE_BUFFER_SIZE );

            switch( sMacroTokenId )
            {
                case ZTP_MACRO_TOKEN_ID :
                    stlSnprintf( aParam->mMacroTokenBuffer, sMacroContents - sBeginTokenPtr + 1,
                                 "%s", sBeginTokenPtr );

                    sArgCount = 0;
                    sArgNameList = aArgNameList;
                    while( sArgNameList != NULL )
                    {
                        sArgCount ++;
                        if( stlStrcmp( sArgNameList->mArgName,
                                       aParam->mMacroTokenBuffer )
                            == 0 )
                        {
                            break;
                        }

                        sArgNameList = sArgNameList->mNext;
                    }

                    if( sArgNameList == NULL )
                    {
                        stlStrcat( aParam->mMacroReplaceBuffer, aParam->mMacroTokenBuffer );
                    }
                    else
                    {
                        stlSnprintf( aParam->mMacroTokenBuffer, ZTP_MACRO_REPLACE_BUFFER_SIZE,
                                     "___GPEC_MACRO_ARGS_%d___", sArgCount );
                        stlStrcat( aParam->mMacroReplaceBuffer, aParam->mMacroTokenBuffer );
                    }
                    break;
                case ZTP_MACRO_TOKEN_ETC :
                    stlSnprintf( aParam->mMacroTokenBuffer, sMacroContents - sBeginTokenPtr + 1,
                                 "%s", sBeginTokenPtr );
                    stlStrcat( aParam->mMacroReplaceBuffer, aParam->mMacroTokenBuffer );
                    break;
                default:
                    break;
            }

        }
    }

    sArgCount = 0;
    sArgNameList = aArgNameList;
    while( sArgNameList != NULL )
    {
        sArgCount ++;
        sArgNameList = sArgNameList->mNext;
    }

    return ztppMakeMacro( aParam,
                          sMacroName,
                          sArgCount,
                          aIsVarArgs,
                          aArgNameList,
                          aParam->mMacroReplaceBuffer );

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztppProcessUndefMacro( ztpPPParseParam *aParam,
                                 stlChar         *aMacroName )
{
    ztpMacroSymbol *sMacroSymbol = NULL;

    STL_TRY( ztppLookupMacroSymbol( aParam->mErrorStack,
                                    aMacroName,
                                    &sMacroSymbol )
             == STL_SUCCESS );

    if( sMacroSymbol != NULL )
    {
        STL_TRY( ztppDeleteMacroSymbol( aParam->mErrorStack,
                                        sMacroSymbol )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


stlChar *ztppMakeStringByPPTokens( ztpPPParseParam  *aParam )
{
    stlStrncpy(aParam->mTempBuffer,
               aParam->mBuffer + aParam->mTokenStream.mStartPos,
               aParam->mTokenStream.mEndPos - aParam->mTokenStream.mStartPos + 1);

    ZTP_INIT_TOKEN_STREAM( aParam->mTokenStream );

    return aParam->mTempBuffer;
}

ztpTokenStream *ztppMakePPTokens( ztpPPParseParam  *aParam,
                                  stlInt32          aStartPos,
                                  stlInt32          aEndPos,
                                  stlBool           aIsLast )
{
    if(aIsLast == STL_TRUE)
    {
        aParam->mTokenStream.mEndPos = aEndPos;
    }

    aParam->mTokenStream.mStartPos = aStartPos;

    return &aParam->mTokenStream;
}

ztpPPParameterList *ztppMakeParameterList( ztpPPParseParam     *aParam,
                                           ztpPPParameterList  *aParameterList,
                                           ztpPPConstExpr      *aConstExpr )
{
    ztpPPParameterList   *sParameterList = NULL;

    STL_TRY(stlAllocRegionMem(aParam->mAllocator,
                              STL_SIZEOF(ztpPPParameterList),
                              (void**)&sParameterList,
                              aParam->mErrorStack)
            == STL_SUCCESS);

    sParameterList->mConstExpr = aConstExpr;
    sParameterList->mNext      = aParameterList;

    return sParameterList;

    STL_FINISH;

    return NULL;
}

ztpPPPrimaryExpr *ztppMakePPPrimaryExpr( ztpPPParseParam     *aParam,
                                         ztpPPTerminalType    aType,
                                         stlInt64             aValue,
                                         ztpPPConstExpr      *aConstExpr,
                                         stlChar             *aMacroName,
                                         ztpPPParameterList  *aParameterList )
{
    ztpPPPrimaryExpr   *sPPPrimaryExpr;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPPrimaryExpr),
                                (void**)&sPPPrimaryExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPPrimaryExpr->mType           = aType;
    sPPPrimaryExpr->mValue          = aValue;
    sPPPrimaryExpr->mConstExpr      = aConstExpr;
    sPPPrimaryExpr->mMacroName      = aMacroName;
    sPPPrimaryExpr->mParameterList  = aParameterList;

    return sPPPrimaryExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPUnaryExpr *ztppMakePPUnaryExpr( ztpPPParseParam     *aParam,
                                     ztpPPUnaryExpr      *aPPUnaryExpr,
                                     ztpPPPrimaryExpr    *aPPPrimaryExpr,
                                     ztpPPOperator        aOperator )
{
    ztpPPUnaryExpr   *sPPUnaryExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || ( aOperator == ZTP_PPOP_PLUS )
                 || ( aOperator == ZTP_PPOP_MINUS
                      )
                 || ( aOperator == ZTP_PPOP_TILDE )
                 || ( aOperator == ZTP_PPOP_NOT ) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPUnaryExpr),
                                (void**)&sPPUnaryExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPUnaryExpr->mUnaryExpr      = aPPUnaryExpr;
    sPPUnaryExpr->mPrimaryExpr    = aPPPrimaryExpr;
    sPPUnaryExpr->mOperator       = aOperator;

    return sPPUnaryExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPMulExpr *ztppMakePPMulExpr( ztpPPParseParam     *aParam,
                                 ztpPPMulExpr        *aPPMulExpr,
                                 ztpPPUnaryExpr      *aPPUnaryExpr,
                                 ztpPPOperator        aOperator )
{
    ztpPPMulExpr   *sPPMulExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || ( aOperator == ZTP_PPOP_MUL )
                 || ( aOperator == ZTP_PPOP_DIV )
                 || ( aOperator == ZTP_PPOP_MOD ) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPMulExpr),
                                (void**)&sPPMulExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPMulExpr->mMulExpr        = aPPMulExpr;
    sPPMulExpr->mUnaryExpr      = aPPUnaryExpr;
    sPPMulExpr->mOperator       = aOperator;

    return sPPMulExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPAddExpr *ztppMakePPAddExpr( ztpPPParseParam     *aParam,
                                 ztpPPAddExpr        *aPPAddExpr,
                                 ztpPPMulExpr        *aPPMulExpr,
                                 ztpPPOperator        aOperator )
{
    ztpPPAddExpr   *sPPAddExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || ( aOperator == ZTP_PPOP_ADD )
                 || ( aOperator == ZTP_PPOP_SUB ) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPAddExpr),
                                (void**)&sPPAddExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPAddExpr->mAddExpr        = aPPAddExpr;
    sPPAddExpr->mMulExpr        = aPPMulExpr;
    sPPAddExpr->mOperator       = aOperator;

    return sPPAddExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPShiftExpr *ztppMakePPShiftExpr( ztpPPParseParam     *aParam,
                                     ztpPPShiftExpr      *aPPShiftExpr,
                                     ztpPPAddExpr        *aPPAddExpr,
                                     ztpPPOperator        aOperator )
{
    ztpPPShiftExpr   *sPPShiftExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || ( aOperator == ZTP_PPOP_SL )
                 || ( aOperator == ZTP_PPOP_SR ) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPShiftExpr),
                                (void**)&sPPShiftExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPShiftExpr->mShiftExpr      = aPPShiftExpr;
    sPPShiftExpr->mAddExpr        = aPPAddExpr;
    sPPShiftExpr->mOperator       = aOperator;

    return sPPShiftExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPRelationalExpr *ztppMakePPRelationalExpr( ztpPPParseParam     *aParam,
                                               ztpPPRelationalExpr *aPPRelationalExpr,
                                               ztpPPShiftExpr      *aPPShiftExpr,
                                               ztpPPOperator        aOperator )
{
    ztpPPRelationalExpr   *sPPRelationalExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || ( aOperator == ZTP_PPOP_LT )
                 || ( aOperator == ZTP_PPOP_GT )
                 || ( aOperator == ZTP_PPOP_LE )
                 || ( aOperator == ZTP_PPOP_GE ) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPRelationalExpr),
                                (void**)&sPPRelationalExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPRelationalExpr->mRelationalExpr = aPPRelationalExpr;
    sPPRelationalExpr->mShiftExpr      = aPPShiftExpr;
    sPPRelationalExpr->mOperator       = aOperator;

    return sPPRelationalExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPEqualExpr *ztppMakePPEqualExpr( ztpPPParseParam     *aParam,
                                     ztpPPEqualExpr      *aPPEqualExpr,
                                     ztpPPRelationalExpr *aPPRelationalExpr,
                                     ztpPPOperator        aOperator )
{
    ztpPPEqualExpr   *sPPEqualExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || ( aOperator == ZTP_PPOP_EQ )
                 || ( aOperator == ZTP_PPOP_NE ) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPEqualExpr),
                                (void**)&sPPEqualExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPEqualExpr->mEqualExpr      = aPPEqualExpr;
    sPPEqualExpr->mRelationalExpr = aPPRelationalExpr;
    sPPEqualExpr->mOperator       = aOperator;

    return sPPEqualExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPBitAndExpr *ztppMakePPBitAndExpr( ztpPPParseParam     *aParam,
                                       ztpPPBitAndExpr     *aPPBitAndExpr,
                                       ztpPPEqualExpr      *aPPEqualExpr,
                                       ztpPPOperator        aOperator )
{
    ztpPPBitAndExpr   *sPPBitAndExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || (aOperator == ZTP_PPOP_BIT_AND) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPBitAndExpr),
                                (void**)&sPPBitAndExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPBitAndExpr->mBitAndExpr = aPPBitAndExpr;
    sPPBitAndExpr->mEqualExpr = aPPEqualExpr;

    return sPPBitAndExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPBitXorExpr *ztppMakePPBitXorExpr( ztpPPParseParam     *aParam,
                                       ztpPPBitXorExpr     *aPPBitXorExpr,
                                       ztpPPBitAndExpr     *aPPBitAndExpr,
                                       ztpPPOperator        aOperator )
{
    ztpPPBitXorExpr   *sPPBitXorExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || (aOperator == ZTP_PPOP_BIT_XOR) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPBitXorExpr),
                                (void**)&sPPBitXorExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPBitXorExpr->mBitXorExpr = aPPBitXorExpr;
    sPPBitXorExpr->mBitAndExpr = aPPBitAndExpr;

    return sPPBitXorExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPBitOrExpr *ztppMakePPBitOrExpr( ztpPPParseParam     *aParam,
                                     ztpPPBitOrExpr      *aPPBitOrExpr,
                                     ztpPPBitXorExpr     *aPPBitXorExpr,
                                     ztpPPOperator        aOperator )
{
    ztpPPBitOrExpr   *sPPBitOrExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || (aOperator == ZTP_PPOP_BIT_OR) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPBitOrExpr),
                                (void**)&sPPBitOrExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPBitOrExpr->mBitOrExpr = aPPBitOrExpr;
    sPPBitOrExpr->mBitXorExpr = aPPBitXorExpr;

    return sPPBitOrExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPAndExpr *ztppMakePPAndExpr( ztpPPParseParam     *aParam,
                                 ztpPPAndExpr        *aPPAndExpr,
                                 ztpPPBitOrExpr      *aPPBitOrExpr,
                                 ztpPPOperator        aOperator )
{
    ztpPPAndExpr   *sPPAndExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || (aOperator == ZTP_PPOP_AND) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPAndExpr),
                                (void**)&sPPAndExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPAndExpr->mAndExpr = aPPAndExpr;
    sPPAndExpr->mBitOrExpr = aPPBitOrExpr;

    return sPPAndExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPOrExpr *ztppMakePPOrExpr( ztpPPParseParam     *aParam,
                               ztpPPOrExpr         *aPPOrExpr,
                               ztpPPAndExpr        *aPPAndExpr,
                               ztpPPOperator        aOperator )
{
    ztpPPOrExpr   *sPPOrExpr;

    STL_DASSERT( ( aOperator == ZTP_PPOP_NONE )
                 || ( aOperator == ZTP_PPOP_OR ) );

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPOrExpr),
                                (void**)&sPPOrExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPOrExpr->mOrExpr = aPPOrExpr;
    sPPOrExpr->mAndExpr = aPPAndExpr;

    return sPPOrExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPCondExpr *ztppMakePPCondExpr( ztpPPParseParam     *aParam,
                                   ztpPPOrExpr         *aPPOrExpr,
                                   ztpPPConstExpr      *aPPConstExpr,
                                   ztpPPCondExpr       *aPPCondExpr )
{
    ztpPPCondExpr   *sPPCondExpr;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPCondExpr),
                                (void**)&sPPCondExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPCondExpr->mOrExpr = aPPOrExpr;
    sPPCondExpr->mConstExpr = aPPConstExpr;
    sPPCondExpr->mCondExpr = aPPCondExpr;

    return sPPCondExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

ztpPPConstExpr *ztppMakePPConstExpr( ztpPPParseParam     *aParam,
                                     ztpPPCondExpr       *aPPCondExpr )
{
    ztpPPConstExpr   *sPPConstExpr;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                STL_SIZEOF(ztpPPConstExpr),
                                (void**)&sPPConstExpr,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sPPConstExpr->mCondExpr = aPPCondExpr;

    return sPPConstExpr;

    STL_FINISH;

    aParam->mErrorStatus = STL_FAILURE;

    return NULL;
}

stlInt64 ztppEvaluateExpression( ztpPPParseParam   *aParam,
                                 stlChar           *aExpression )
{
    stlInt32         sExprLen;
    stlInt64         sExprResult = 0;
    stlChar         *sIdStart;
    stlChar          sId[1024];
    /*
    stlChar          sReplace[8192];
    stlInt32         sDestIdx;
    */
    stlInt32         sIdLen;
    stlInt32         sSrcIdx;
    ztpMacroSymbol  *sMacroSymbol;
    stlChar          sTmpStr[] = "(((3) > (1) ? (3) : (1)) - 1)";

    sExprLen = stlStrlen( aExpression );

    sSrcIdx = 0;
    /*
    sDestIdx = 0;
    */

    ztppCalcParseIt( aParam->mAllocator,
                     aParam->mErrorStack,
                     sTmpStr,
                     stlStrlen( sTmpStr ),
                     &sExprResult );

    for( ; sSrcIdx < sExprLen; sSrcIdx ++ )
    {
        if( stlIsalpha( aExpression[sSrcIdx] ) != 0 )
        {
            sIdStart = aExpression + sSrcIdx;
            sIdLen = 0;
            while( stlIsalnum( sIdStart[sIdLen] ) != 0 )
            {
                sIdLen ++;
            }
            stlStrncpy( sId, sIdStart, sIdLen + 1 );

            ztppFindMacro( sId,
                           &sMacroSymbol,
                           aParam->mErrorStack );


        }
        else
        {
	  /*
            sReplace[sDestIdx] = aExpression[sSrcIdx];
	  */
        }
    }

    return sExprResult;
}

stlStatus ztppReplaceUndefinedMacro( ztpPPParseParam  *aParam,
                                     stlChar          *aReplacedMacro,
                                     stlInt32         *aReplacedMacroLen )
{
    stlStrcpy( aReplacedMacro, "0" );
    *aReplacedMacroLen = 1;

    return STL_SUCCESS;
}

void ztppGoPrevToken( ztpPPParseParam  *aParam )
{
    ztppUnputToken( aParam->mYyScanner,
                    (stlChar *)aParam->mBuffer + aParam->mNextLoc,
                    aParam->mLength );
    aParam->mNextLoc -= aParam->mLength;
}

void ztppPushedBackInputStream( ztpPPParseParam  *aParam,
                                stlChar          *aReplaceString,
                                stlInt32          aLength )
{
    ztppUnputToken( aParam->mYyScanner,
                    aReplaceString,
                    aLength );
    aParam->mNextLoc -= aLength;
}

void ztppWritePPToken( ztpPPParseParam  *aParam,
                       stlChar          *aToken )
{
    ztpuSendStringToOutFile( aParam->mOutFile,
                             aToken,
                             aParam->mErrorStack );
}

void ztppTraceMacroToken( ztpPPParseParam  *aParam,
                          stlChar          *aMacro,
                          stlChar          *aFinalText )
{
    stlChar         *sMacroName   = NULL;
    ztpMacroSymbol  *sMacroSymbol = NULL;
    stlInt32         sToken;
    stlInt32         sParenDepth = 0;
    stlBool          sExitFlag = STL_FALSE;

    stlMemset( aParam->mMacroReplaceBuffer, 0x00, ZTP_MACRO_REPLACE_BUFFER_SIZE );
    stlMemset( aParam->mMacroTokenBuffer, 0x00, ZTP_MACRO_REPLACE_BUFFER_SIZE );

    stlStrcpy( aParam->mMacroReplaceBuffer, aMacro );

    ztppFindMacro( aParam->mMacroReplaceBuffer,
                   &sMacroSymbol,
                   aParam->mErrorStack );

    while( sMacroSymbol != NULL )
    {
        sMacroName = sMacroSymbol->mContents;

        if( ( sMacroSymbol->mContents == NULL )
            || ( sMacroSymbol->mContents[0] == '\0' ) )
        {
            if( sMacroSymbol->mArgCount >= 0 )
            {
                sParenDepth = 0;
                sExitFlag = STL_FALSE;

                while( sExitFlag != STL_TRUE )
                {
                    sToken = ztppGetNextToken( aParam,
                                               aParam->mYyScanner );

                    if( ztppIsLParen( sToken ) == STL_TRUE )
                    {
                        sParenDepth ++;
                    }
                    else if( ztppIsRParen( sToken ) == STL_TRUE )
                    {
                        sParenDepth --;
                        if( sParenDepth <= 0 )
                        {
                            sExitFlag = STL_TRUE;
                        }
                    }
                    else
                    {
                        /*
                         * pass another tokens
                         *   : do nothing
                         */
                    }
                }
            }

            aParam->mMacroReplaceBuffer[0] = '\0';
            break;
        }

        stlMemcpy( aParam->mMacroTokenBuffer,
                   sMacroName,
                   ZTP_MACRO_REPLACE_BUFFER_SIZE );

        STL_TRY( ztppReplaceMacro( aParam->mMacroReplaceBuffer,
                                   aParam->mMacroTokenBuffer,
                                   NULL,
                                   NULL,
                                   0,
                                   aParam->mErrorStack )
                 == STL_SUCCESS );

        ztppFindMacro( aParam->mMacroReplaceBuffer,
                       &sMacroSymbol,
                       aParam->mErrorStack );
    }

    stlStrcpy( aFinalText, aParam->mMacroReplaceBuffer );

    return;

    STL_FINISH;

    stlStrcpy( aFinalText, aParam->mMacroReplaceBuffer );

    return;
}

void ztppWritePPMacroToken( ztpPPParseParam  *aParam,
                            stlChar          *aMacro )
{
    stlChar         *sTempBuffer = NULL;
    ztpMacroSymbol  *sMacroSymbol = NULL;

    STL_TRY( stlAllocHeap( (void **)&sTempBuffer,
                           ZTP_MACRO_REPLACE_BUFFER_SIZE,
                           aParam->mErrorStack )
             == STL_SUCCESS );
    stlMemset( sTempBuffer, 0x00, ZTP_MACRO_REPLACE_BUFFER_SIZE );

    ztppFindMacro( aMacro,
                   &sMacroSymbol,
                   aParam->mErrorStack );

    ztppTraceMacroToken( aParam,
                         aMacro,
                         sTempBuffer );

    ztppWritePPToken( aParam, sTempBuffer );

    stlFreeHeap( sTempBuffer );

    return;

    STL_FINISH;

    if( sTempBuffer != NULL )
    {
        stlFreeHeap( sTempBuffer );
    }

    ztppWritePPToken( aParam, aMacro );

    return;
}


/**
 * @note 이름을 변경할수 없다.
 */
stlInt32 ztpPPerror( YYLTYPE          *aLloc,
                     ztpPPParseParam  *aParam,
                     void             *aScanner,
                     const stlChar    *aMsg )
{
    stlInt32   i;
    stlInt32   sCurLineNo = 1;
    stlChar  * sStartPtr = aParam->mBuffer;
    stlChar  * sEndPtr;
    stlChar  * sErrMsg;
    stlChar    sErrLineMsgBuffer[128];
    stlInt32   sErrMsgLength;

    /**
     * 에러 메세지의 총길이는 (원본 SQL의 길이 + 에러 위치 메세지 길이 +
     * 에러 라인 메세지 길이)의 총합과 같다.
     * (에러 위치 메시지 길이 + 에러 라인 메세지 길이)를 정확히 예상할수도 있지만
     * 대략 원본 메세지 길이의 4배보다는 작음을 보장할수 있다.
     */
    sErrMsgLength = (stlStrlen( sStartPtr ) + 1) * 4;

    STL_TRY( stlAllocRegionMem( aParam->mAllocator,
                                sErrMsgLength,
                                (void**)&sErrMsg,
                                aParam->mErrorStack )
             == STL_SUCCESS );

    sErrMsg[0] = '\0';

    while( sCurLineNo < aParam->mLineNo )
    {
        if( *sStartPtr == '\n' )
        {
            sCurLineNo++;
        }
        sStartPtr++;
    }
    sEndPtr = sStartPtr;
    while( (*sEndPtr != '\n') && (*sEndPtr != '\0') )
    {
        sEndPtr++;
    }

    /**
     * 원본 SQL 저장
     */
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );
    stlStrnncat( sErrMsg, sStartPtr, sErrMsgLength, sEndPtr - sStartPtr + 1 );
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );

    /**
     * 에러 위치 메세지 저장
     */
    for( i = 1; i < aParam->mColumn; i++ )
    {
        stlStrnncat( sErrMsg, ".", sErrMsgLength, STL_UINT32_MAX );
    }
    stlStrnncat( sErrMsg, "^", sErrMsgLength, STL_UINT32_MAX );
    for( i = 0; i < aParam->mColumnLen - 2; i++ )
    {
        stlStrnncat( sErrMsg, " ", sErrMsgLength, STL_UINT32_MAX );
    }
    if( aParam->mColumnLen >= 2 )
    {
        stlStrnncat( sErrMsg, "^", sErrMsgLength, STL_UINT32_MAX );
    }
    stlStrnncat( sErrMsg, "\n", sErrMsgLength, STL_UINT32_MAX );

    /**
     * 에러 라인 메세지 저장
     */
    stlSnprintf( sErrLineMsgBuffer,
                 128,
                 "Error at line %d, in file %s",
                 aParam->mLineNo,
                 aParam->mInFileName );
    stlStrnncat( sErrMsg, sErrLineMsgBuffer, sErrMsgLength, STL_UINT32_MAX );

    aParam->mErrorStatus = STL_FAILURE;
    aParam->mErrorMsg = sErrMsg;

    return 0;

    STL_FINISH;

    return 0;
}

stlStatus ztppParseIt(ztpConvertContext  *aContext)
{
    ztpPPParseParam  sParam;

    sParam.mNeedPrint          = aContext->mNeedPrint;
    sParam.mOccurConditionTrue = STL_FALSE;
    sParam.mIfLevel            = 0;
    sParam.mBuffer             = aContext->mBuffer;
    sParam.mAllocator          = aContext->mAllocator;
    sParam.mErrorStack         = aContext->mErrorStack;
    sParam.mLength             = aContext->mLength;
    sParam.mFileLength         = aContext->mLength;
    sParam.mPosition           = 0;
    sParam.mLineNo             = 1;
    sParam.mColumn             = 1;
    sParam.mColumnLen          = 0;
    sParam.mParseTree          = NULL;
    sParam.mErrorMsg           = NULL;
    sParam.mErrorStatus        = STL_SUCCESS;
    sParam.mOutFile            = aContext->mOutFile;
    sParam.mInFileName         = aContext->mInFileName;
    sParam.mOutFileName        = aContext->mOutFileName;
    sParam.mCCodeStartLoc      = 0;
    sParam.mCCodeEndLoc        = 0;
    sParam.mCurrLoc            = 0;
    sParam.mNextLoc            = 0;
    sParam.mContext            = NULL;

    ZTP_INIT_TOKEN_STREAM( sParam.mTokenStream );
    STL_TRY(stlAllocRegionMem(sParam.mAllocator,
                              sParam.mFileLength,
                              (void **)&sParam.mTempBuffer,
                              sParam.mErrorStack)
            == STL_SUCCESS);

    STL_TRY(stlAllocRegionMem(sParam.mAllocator,
                              ZTP_MACRO_REPLACE_BUFFER_SIZE,
                              (void **)&sParam.mMacroReplaceBuffer,
                              sParam.mErrorStack)
            == STL_SUCCESS);

    STL_TRY(stlAllocRegionMem(sParam.mAllocator,
                              ZTP_MACRO_REPLACE_BUFFER_SIZE,
                              (void **)&sParam.mMacroTokenBuffer,
                              sParam.mErrorStack)
            == STL_SUCCESS);

    ztpPPlex_init(&sParam.mYyScanner);
    ztpPPset_extra(&sParam, sParam.mYyScanner);
    ztpPPparse(&sParam, sParam.mYyScanner);
    ztpPPlex_destroy(sParam.mYyScanner);

    STL_TRY_THROW(sParam.mErrorStatus == STL_SUCCESS, RAMP_ERR_SYNTAX);

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SYNTAX )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTP_ERRCODE_SYNTAX,
                      sParam.mErrorMsg,
                      sParam.mErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

