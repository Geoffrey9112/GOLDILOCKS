/*******************************************************************************
 * ztpuString.c
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
#include <ztpuString.h>

/**
 * @file ztpuString.c
 * @brief Gliese Embedded SQL in C precompiler utility functions
 */


/**
 * @addtogroup ztpuString
 * @{
 */

stlStatus ztpuTrimSqlStmt(stlChar *aSrcSqlStmt, stlChar *aDestSqlStmt)
{
    stlInt32    sSrcIdx = 0;
    stlInt32    sDestIdx = 0;

    while(aSrcSqlStmt[sSrcIdx] != '\0')
    {
        if(aSrcSqlStmt[sSrcIdx] == '\n')
        {
            aDestSqlStmt[sDestIdx] = '\\';
            aDestSqlStmt[sDestIdx + 1] = 'n';
            sDestIdx += 2;

            sSrcIdx ++;
            continue;
        }
        else if(stlIsspace(aSrcSqlStmt[sSrcIdx]) != 0)
        {
            aDestSqlStmt[sDestIdx] = ' ';
            sDestIdx ++;

            sSrcIdx ++;
            continue;
        }
        else if(aSrcSqlStmt[sSrcIdx] == '"')
        {
            aDestSqlStmt[sDestIdx] = '\\';
            sDestIdx ++;
        }

        aDestSqlStmt[sDestIdx] = aSrcSqlStmt[sSrcIdx];
        sSrcIdx ++;
        sDestIdx ++;
    }

    return STL_SUCCESS;
}

stlChar *ztpuMakeStaticConnString( stlAllocator   *aAllocator,
                                   stlErrorStack  *aErrorStack,
                                   stlChar        *aConnName )
{
    stlChar   *sConnName;
    stlInt32   sConnNameLen;
    stlInt32   sSrcIdx;
    stlInt32   sDestIdx;

    /*
     * string 내부에 " 가 존재할 수 있으므로, "는 \" 로 변환한다.
     * 모든 string의 내용이 " 문자일 경우를 가정하여,
     * 주어진 string의 길이 * 2
     * 여기에 변환되는 string의 앞 뒤에 " 문자가 들어가므로, + 2
     * string 끝의 null 문자를 위한 + 1
     */
    sConnNameLen = stlStrlen( aConnName ) * 2 + 2 + 1;

    STL_TRY( stlAllocRegionMem( aAllocator,
                                sConnNameLen,
                                (void**)&sConnName,
                                aErrorStack )
             == STL_SUCCESS );

    sDestIdx = 0;
    sSrcIdx = 0;

    sConnName[sDestIdx] = '"';
    sDestIdx ++;

    while( aConnName[sSrcIdx] != '\0' )
    {
        if( aConnName[sSrcIdx] == '"' )
        {
            sConnName[sDestIdx] = '\\';
            sDestIdx ++;
        }
        sConnName[sDestIdx] = aConnName[sSrcIdx];
        sDestIdx ++;
        sSrcIdx ++;
    }

    sConnName[sDestIdx] = '"';
    sDestIdx ++;
    sConnName[sDestIdx] = '\0';

    return sConnName;

    STL_FINISH;

    return NULL;
}

stlStatus ztpuAppendErrorMsg( stlAllocator   *aAllocator,
                              stlErrorStack  *aErrorStack,
                              stlChar       **aOrgMsg,
                              stlChar        *aMsg )
{
    stlInt32   sOldMsgLength;
    stlInt32   sGivenMsgLength;
    stlInt32   sErrMsgLength;
    stlChar   *sErrMsg;

    sOldMsgLength = stlStrlen( *aOrgMsg );
    sGivenMsgLength = stlStrlen( aMsg );
    sErrMsgLength = sOldMsgLength + sGivenMsgLength + 1;

    STL_TRY( stlAllocRegionMem( aAllocator,
                                sErrMsgLength,
                                (void**)&sErrMsg,
                                aErrorStack )
             == STL_SUCCESS );

    stlStrncpy( sErrMsg, *aOrgMsg, sOldMsgLength + 1 );
    stlStrncat( sErrMsg, aMsg, sGivenMsgLength + 1 );

    /*
     * Region Memory를 사용하기 때문에, 기존 Memory를 free하지 않는다.
     */
    *aOrgMsg = sErrMsg;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
