/*******************************************************************************
 * ztpMisc.c
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
#include <dtl.h>
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztpSqlLex.h>
#include <ztpSqlParser.h>
#include <ztpdType.h>
#include <ztppMacro.h>
#include <ztpcHostVar.h>
#include <ztpuFile.h>
#include <ztpuString.h>

/**
 * @file ztpMisc.c
 * @brief Gliese Embedded SQL in C precompiler miscellaneous functions
 */

stlChar gErrMsgBuffer[STL_MAX_ERROR_MESSAGE];


/**
 * @addtogroup ztpMisc
 * @{
 */

void ztpPrintErrorStack(stlErrorStack * aErrorStack)
{
    stlInt32 i;
    stlInt32 sErrorCode;
    stlInt32 sExtErrCode;
    stlChar  sSqlState[6];

    for(i = aErrorStack->mTop; i >= 0; i--)
    {
        sErrorCode = aErrorStack->mErrorData[i].mErrorCode;
        sExtErrCode = stlGetExternalErrorCode( &aErrorStack->mErrorData[i] );
        stlMakeSqlState(sExtErrCode, sSqlState);

        stlPrintf("ERR-%s(%d): %s %s\n",
                  sSqlState,
                  sErrorCode,
                  aErrorStack->mErrorData[i].mErrorMessage,
                  aErrorStack->mErrorData[i].mDetailErrorMessage);
    }
}

/**
 * @brief Parsing Error 는 아니나 Pre-Compile 할 수 없는 경우 해당 SQL 위치에 대한 에러 위치 메시지를 생성한다.
 * @param[in] aSQLString    SQL String
 * @param[in] aPosition     Error Position
 * @notes qllMakeErrPosString() 함수를 참조했으나, character set 을 고려하지 않음
 * @remarks
 */
stlChar * ztpMakeErrPosString( stlChar * aSQLString,
                               stlInt32  aPosition )
{
    stlInt32 sOffset = 0;
    stlInt32 sLineNo = 0;
    stlInt32 sLineStartOffset = 0;
    stlInt32 sLineEndOffset = 0;

    gErrMsgBuffer[0] = '\0';

    while( aSQLString[sOffset] != '\0' )
    {
        if ( sOffset >= aPosition )
        {
            /**
             * 위치를 찾음
             */

            sLineEndOffset = sOffset;
            while ( (aSQLString[sLineEndOffset] != '\n') &&
                    (aSQLString[sLineEndOffset] != '\0') )
            {
                sLineEndOffset ++;
            }

            break;
        }

        if ( aSQLString[sOffset] == '\n' )
        {
            sLineNo++;
            sLineStartOffset = sOffset + 1;
        }

        sOffset ++;
    }

    /**
     * 한 라인의 길이가 너무 길 경우 시작 위치를 조정한다.
     */

    if ( ( (sLineEndOffset - sLineStartOffset) * 2 + 20 ) > STL_MAX_ERROR_MESSAGE )
    {
        sLineStartOffset = sLineEndOffset - (STL_MAX_ERROR_MESSAGE / 2) + 20;
    }
    else
    {
        /* 충분한 길이임 */
    }

    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "\n" );

    for ( sOffset = sLineStartOffset; sOffset < sLineEndOffset; sOffset++ )
    {
        stlSnprintf( gErrMsgBuffer,
                     STL_MAX_ERROR_MESSAGE,
                     "%s%c",
                     gErrMsgBuffer,
                     aSQLString[sOffset] );
    }

    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "%s\n",
                 gErrMsgBuffer );

    for ( sOffset = sLineStartOffset; sOffset < aPosition;  )
    {
        stlSnprintf( gErrMsgBuffer,
                     STL_MAX_ERROR_MESSAGE,
                     "%s.",
                     gErrMsgBuffer );
        sOffset ++;
    }
    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "%s%c\n",
                 gErrMsgBuffer,
                 '^' );

    stlSnprintf( gErrMsgBuffer,
                 STL_MAX_ERROR_MESSAGE,
                 "%sError at line %d:\n",
                 gErrMsgBuffer,
                 sLineNo + 1);

    return gErrMsgBuffer;
}

stlStatus ztpExtractNParseSql(ztpCParseParam   *aParam,
                              stlInt32          aStartPos,
                              stlInt32         *aEndPos)
{
    ztpParseTree *sParseTree = NULL;
    stlChar      *sSqlString = NULL;
    stlInt32      sSqlLength = 0;

    stlMemset( gSqlBuffer, 0x00, gSqlBufLen );

    sSqlString = aParam->mBuffer + aStartPos;

    STL_TRY( ztpSqlParseIt(sSqlString,
                           &sSqlLength,
                           aParam->mAllocator,
                           &sParseTree,
                           aParam->mErrorStack )
             == STL_SUCCESS );

    aParam->mSQLStartPos = aStartPos;
    aParam->mParseTree   = sParseTree;
    *aEndPos             = aStartPos + sSqlLength;
    //aParam->mBuffer[*aEndPos]= '\0';

    //stlStrncpy( gSqlBuffer, sSqlString, sSqlLength + 1);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/************************************************************************
 * Cursor
 ************************************************************************/



stlStatus ztpAddCursorToSymTab(ztpSymTabHeader  *aSymTab,
                               ztpDeclCursor    *aDeclCursor)
{
    aDeclCursor->mNext = aSymTab->mCursorListHead;
    aSymTab->mCursorListHead = aDeclCursor;

    return STL_SUCCESS;
}

/**
 * @brief Cursor Name 에 해당하는 Cursor Symbol 을 찾는다.
 * @param[in]  aCursorName   Cursor Name
 * @param[out] aDeclCursor   검색된 Cursor
 */
void ztpFindCursorInSymTab( stlChar        * aCursorName,
                            ztpDeclCursor ** aDeclCursor )
{
    ztpDeclCursor     *sCursorEntry = NULL;

    /**
     * Parameter Validation
     */

    STL_DASSERT( aDeclCursor != NULL );

    /**
     * Cursor는 symbol table에서 global scope을 갖는다.
     */

    *aDeclCursor = NULL;
    sCursorEntry = gRootSymTab->mCursorListHead;
    while( sCursorEntry != NULL )
    {
        if(stlStrncmp(sCursorEntry->mCursorName,
                      aCursorName,
                      STL_MAX_SQL_IDENTIFIER_LENGTH) == 0)
        {
            *aDeclCursor = sCursorEntry;
            break;
        }

        sCursorEntry = sCursorEntry->mNext;
    }

    /**
     * Output 설정
     */

    *aDeclCursor = sCursorEntry;
}

stlStatus ztpMakeNameFromNameTag(ztpCParseParam *aParam,
                                 ztpNameTag     *aSrc,
                                 stlChar        *aDest)
{
    if(aSrc != NULL)
    {
        switch(aSrc->mType)
        {
            case ZTP_NAMETYPE_LITERAL:
                stlSnprintf(aDest, stlStrlen(aSrc->mName) + 3,
                            "\"%s\"", aSrc->mName);
                break;
            case ZTP_NAMETYPE_HOSTVAR:
                stlSnprintf(aDest, stlStrlen(aSrc->mName) + 1,
                            "%s", aSrc->mName);
                break;
            default:
                break;
        }
    }
    else
    {
        stlStrncpy(aDest, "NULL", 5);
    }

    return STL_SUCCESS;
}

stlStatus ztpMakeExceptionString(ztpCParseParam *aParam,
                                 stlInt32        aCondType,
                                 stlChar        *aExceptionStr)
{
    ztpExecSqlException  *sException = aParam->mException[aCondType];
    //stlChar               sErrorMsg[STL_MAX_ERROR_MESSAGE];

    STL_TRY_THROW(sException != NULL, EXIT_FUNCTION);

    STL_TRY(gExceptCondFunc[sException->mCondition->mType](aParam,
                                                           aCondType,
                                                           aExceptionStr)
            == STL_SUCCESS);

    STL_TRY(gExceptActFunc[sException->mAction->mType](aParam,
                                                       aCondType,
                                                       aExceptionStr)
            == STL_SUCCESS);

    STL_RAMP(EXIT_FUNCTION);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpCreateSymTab(stlAllocator   *aAllocator,
                          stlErrorStack  *aErrorStack)
{
    ztpSymTabHeader   *sSymTabHeader;

    STL_TRY(gRootSymTab == NULL);

    /* 현재 Symbol Table에 없는 경우 최초 생성 */
    STL_TRY(stlAllocRegionMem(aAllocator,
                              STL_SIZEOF(ztpSymTabHeader),
                              (void**)&sSymTabHeader,
                              aErrorStack)
            == STL_SUCCESS);

    /* Depth가 0이면, Global scope를 의미함 */
    sSymTabHeader->mDepth = 0;
    sSymTabHeader->mCVariableHead = NULL;
    sSymTabHeader->mCursorListHead = NULL;
    sSymTabHeader->mTypedefListHead = NULL;
    sSymTabHeader->mPrevDepth = NULL;
    sSymTabHeader->mNextDepth = NULL;

    gRootSymTab = sSymTabHeader;
    gCurrSymTab = sSymTabHeader;

    /*
    STL_TRY( ztppCreateSymbolTable( aAllocator,
                                    aErrorStack )
             == STL_SUCCESS );
    */

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus ztpDestroySymTab(stlAllocator   *aAllocator,
                           stlErrorStack  *aErrorStack)
{
    gRootSymTab = NULL;
    gCurrSymTab = NULL;

    /*
    STL_TRY( ztppDestroySymbolTable( aErrorStack )
             == STL_SUCCESS );
    */

    return STL_SUCCESS;

    /*
    STL_FINISH;

    return STL_FAILURE;
    */
}

/** @} */
