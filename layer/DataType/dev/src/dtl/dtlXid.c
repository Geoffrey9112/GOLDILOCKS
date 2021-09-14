/*******************************************************************************
 * dtlXid.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlXid.c 2163 2011-10-27 08:44:10Z jhkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <dtl.h>
#include <dtdDataType.h>

/**
 * @file dtlXid.c
 * @brief Datatype Layer XID Routines
 */


/**
 * @addtogroup dtlXid
 * @{
 */


/**
 * @brief 주어진 XID의 유효성을 검사한다.
 * @param[in] aXid 검사할 XID
 */
stlBool dtlValidateXid( stlXid * aXid )
{
    if( (aXid->mTransLength == 0) ||
        (aXid->mBranchLength == 0) )
    {
        return STL_FALSE;
    }

    return STL_TRUE;
}


/**
 * @brief 주어진 XID 2개를 비교한다.
 * @param[in] aXid1 검사할 XID1
 * @param[in] aXid2 검사할 XID2
 */
stlBool dtlIsEqualXid( stlXid  * aXid1,
                       stlXid  * aXid2 )
{
    if( stlMemcmp( aXid2,
                   aXid1,
                   ( (STL_SIZEOF(stlInt64)*3) +
                     aXid2->mTransLength +
                     aXid2->mBranchLength ) ) != 0 )
    {
        return STL_FALSE;
    }
    
    return STL_TRUE;
}


/**
 * @brief 주어진 XID를 문자열로 변환한다.
 * @param[in]     aXid         대상 바이너리 XID
 * @param[in]     aBufferSize  aXidString의 버퍼 크기
 * @param[out]    aXidString   변환된 XID 문자열
 * @param[in,out] aErrorStack  Error Stack Pointer
 * @remarks  XID string :  'format_id.trans_hex_string.branch_hex_string'
 */
stlStatus dtlXidToString( stlXid        * aXid,
                          stlInt32        aBufferSize,
                          stlChar       * aXidString,
                          stlErrorStack * aErrorStack )
{
    stlChar  sHexTransString[STL_XID_DATA_SIZE * 2];
    stlChar  sHexBranchString[STL_XID_DATA_SIZE * 2];
    
    STL_TRY( stlBinary2HexString( (stlUInt8*)aXid->mData,
                                  aXid->mTransLength,
                                  sHexTransString,
                                  STL_XID_DATA_SIZE * 2,
                                  aErrorStack )
             == STL_SUCCESS );
        
    STL_TRY( stlBinary2HexString( (stlUInt8*)&aXid->mData[aXid->mTransLength],
                                  aXid->mBranchLength,
                                  sHexBranchString,
                                  STL_XID_DATA_SIZE * 2,
                                  aErrorStack )
             == STL_SUCCESS );
        
    stlSnprintf( aXidString,
                 aBufferSize,
                 "%ld.%s.%s",
                 aXid->mFormatId,
                 sHexTransString,
                 sHexBranchString );
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 문자열을 XID로 변환한다.
 * @param[in]     aXid         대상 바이너리 XID
 * @param[out]    aXidString   변환된 XID 문자열
 * @param[in,out] aErrorStack  Error Stack Pointer
 * @remarks  XID string :  'format_id.trans_hex_string.branch_hex_string'
 */
stlStatus dtlStringToXid( stlChar       * aXidString,
                          stlXid        * aXid,
                          stlErrorStack * aErrorStack )
{
    stlChar  * sTokenDelimeter = ".";
    stlChar  * sTokenSavePtr;
    stlChar  * sToken;
    stlChar  * sEndPtr;
    stlInt64   sLength;
    stlChar    sXidString[STL_XID_STRING_SIZE];

    stlStrncpy( sXidString, aXidString, STL_XID_STRING_SIZE );
    
    sToken = stlStrtok( sXidString,
                        sTokenDelimeter,
                        &sTokenSavePtr );
    
    STL_TRY_THROW( sToken != NULL, RAMP_ERR_INVALID_XID_STRING );

    STL_TRY( stlStrToInt64( sToken,
                            STL_NTS,
                            &sEndPtr,
                            10,
                            &aXid->mFormatId,
                            aErrorStack )
             == STL_SUCCESS );
    
    sToken = stlStrtok( NULL,
                        sTokenDelimeter,
                        &sTokenSavePtr );

    STL_TRY_THROW( sToken != NULL, RAMP_ERR_INVALID_XID_STRING );
    
    STL_TRY( dtdToBinaryStringFromString( sToken,
                                          stlStrlen(sToken),
                                          STL_XID_TRANS_SIZE,
                                          (stlInt8*)&aXid->mData[0],
                                          &sLength,
                                          aErrorStack )
             == STL_SUCCESS );

    aXid->mTransLength = sLength;
                                          
    sToken = stlStrtok( NULL,
                        sTokenDelimeter,
                        &sTokenSavePtr );
    
    STL_TRY_THROW( sToken != NULL, RAMP_ERR_INVALID_XID_STRING );
    
    STL_TRY( dtdToBinaryStringFromString( sToken,
                                          stlStrlen(sToken),
                                          STL_XID_BRANCH_SIZE,
                                          (stlInt8*)&aXid->mData[sLength],
                                          &sLength,
                                          aErrorStack )
             == STL_SUCCESS );

    aXid->mBranchLength = sLength;
                                          
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_XID_STRING )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      DTL_ERRCODE_INVALID_XID_STRING,
                      NULL,
                      aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
