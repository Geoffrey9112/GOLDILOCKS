/*******************************************************************************
 * knpPropertyFile.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *        Property를 관리 관련된 함수
 *
 ******************************************************************************/


/**
 * @file knpPropertyFile.c
 * @brief Kernel Layer Property Internal Routines
 */

#include <knl.h>

#include <knDef.h>
#include <knpDef.h>
#include <knpDataType.h>
#include <knpPropertyManager.h>
#include <knpPropertyFile.h>

/**
 * @addtogroup knpProperty
 * @{
 */

/**
 * @brief 내용이 없는 Binary 프로퍼티 파일을 생성한다. 
 * @param[in]     aFileName   PropertyFileName
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS]
 * 파일 처리가 정상적이지 않습니다.
 * - 파일 처리 또는 파일을 확인해야 합니다.
 * @endcode
 * @remarks
 */

stlStatus knpCreateBinaryPropertyWithBlank( stlChar  * aFileName,
                                            knlEnv   * aKnlEnv )
{
    stlFile               sFile;
    stlSize               sWriteSize      = 0;
    knpPropertyFileHeader sHeader;
    stlInt32              sStatus         = 0;
    stlBool               sSuccess        = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /** FILE이 있을 경우 덮어씌워야 함 !! */
    STL_TRY( knpGetFileExist( aFileName, 
                              &sSuccess,
                              aKnlEnv ) == STL_SUCCESS );
    
    if ( sSuccess == STL_TRUE )
    {
        /** Remove File */
        STL_TRY( stlRemoveFile( aFileName,
                                KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );           
    }
    
    STL_TRY( stlOpenFile( &sFile,
                          aFileName,
                          STL_FOPEN_CREATE | STL_FOPEN_EXCL | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                          STL_FPERM_OS_DEFAULT,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus  = 1;
    
    sHeader.mFileSize      = STL_SIZEOF(knpPropertyFileHeader);
    sHeader.mChecksum      = 0;
    sHeader.mPropertyCount = 0;
    sHeader.mPadding       = 0;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 2;
    
    /**
     * Write Blank Header
     */
    STL_TRY( stlWriteFile( &sFile,
                           (void*)&sHeader,
                           STL_SIZEOF( knpPropertyFileHeader ),
                           &sWriteSize,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sWriteSize == STL_SIZEOF(knpPropertyFileHeader), RAMP_ERR_INVALID_FILE_PROCESS );
    
    sStatus = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    switch( sStatus )
    {
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  KNL_ERROR_STACK( aKnlEnv ) );
        case 1:
            (void) stlCloseFile( &sFile,
                                 KNL_ERROR_STACK( aKnlEnv ) );
        default:
            break;   
    }
    
    return STL_FAILURE;
}


/**
 * @brief Binary 프로퍼티 파일에 해당 프로퍼티의 값을 쓴다.
 * @param[in]     aFileName   BinaryFileName
 * @param[in]     aContent    knpPropertyContent
 * @param[in]     aValue      저장할 값
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS]
 * 파일 처리가 정상적이지 않습니다.
 * - 파일 처리 또는 파일을 확인해야 합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST]
 * File이 존재하지 않습니다.
 * - 해당 경로에 파일이 있는지 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpWritePropertyAtBinaryFile ( stlChar             * aFileName,
                                         knpPropertyContent  * aContent,
                                         void                * aValue,
                                         knlEnv              * aKnlEnv )
{
    stlFile               sFile;
    stlChar             * sWriteBuff       = NULL;      /** File 내용을 저장하기 위한 Buffer */
    
    stlOffset             sOffset          = 0;
    stlOffset             sFileSize        = 0;
    stlSize               sReadBytes       = 0;         /** File에서 한번에 읽은 Byte Size */
    stlSize               sWrittenBytes    = 0;         /** File에 한번에 쓴 Byte Size */
    
    stlBool               sFileExist       = STL_FALSE;
    stlInt32              sStatus          = 0;
    stlInt32              sIdx             = 0;
    stlInt64              sTmpNumberValue  = 0;
    
    knpPropertyFileBody   sBody;
    knpPropertyFileHeader sHeader;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aContent  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /**
     * 기존에 Property가 있을 경우를 위해서 해당 Property를 제거한다.
     */
    
    STL_TRY( knpRemovePropertyAtBinaryFile( aFileName,
                                            aContent,
                                            aKnlEnv ) == STL_SUCCESS );
    
    /** File Exist ? */
    STL_TRY( knpGetFileExist( aFileName, 
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    if( sFileExist == STL_FALSE )
    {
        STL_TRY( knpCreateBinaryPropertyWithBlank( aFileName,
                                                   aKnlEnv ) == STL_SUCCESS );
    }
    
    /** File Open */
    STL_TRY ( stlOpenFile( &sFile,
                           aFileName,
                           STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 2;
    
    
    /**
     * PropertyFileHeader 읽어두기
     */
    sReadBytes = 0;
    STL_TRY( stlReadFile( &sFile,
                          (void*)&sHeader,
                          STL_SIZEOF( knpPropertyFileHeader ),
                          &sReadBytes,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    /** File Validation : file exist but blank */
    
    sFileSize = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    /**
     * Property 구성
     */
    
    /** Memory Allocation for Property */
    STL_TRY( stlAllocHeap( (void**)&sWriteBuff, 
                           1024 * 2,        //임시로 2k 정도의 크기를 할당한다.
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 3;
    
    stlMemset( sWriteBuff, 0x00, 1024 * 2 );
    stlMemset( &sBody, 0x00, STL_SIZEOF( knpPropertyFileBody ) );
    
    sBody.mNameLength = stlStrlen( aContent->mName );
    sBody.mDataType   = aContent->mDataType;
    
    switch( aContent->mDataType )
    {
        case KNL_PROPERTY_TYPE_BOOL:
        {
            sBody.mValueLength = STL_SIZEOF( stlInt64 );
            sTmpNumberValue    = (stlInt64)*(stlBool*)aValue;
            
            sIdx = 0;
            
            stlMemcpy( &sWriteBuff[sIdx], &sBody, STL_SIZEOF( knpPropertyFileBody ) );
            sIdx += STL_SIZEOF( knpPropertyFileBody );
            
            stlMemcpy( &sWriteBuff[sIdx], aContent->mName, stlStrlen( aContent->mName ) );
            sIdx += stlStrlen( aContent->mName );
            
            stlMemcpy( &sWriteBuff[sIdx], &sTmpNumberValue, sBody.mValueLength );
            sIdx += sBody.mValueLength;
            break;
        }
        case KNL_PROPERTY_TYPE_BIGINT:
        {
            sBody.mValueLength = STL_SIZEOF( stlInt64 );
            sTmpNumberValue    = *(stlInt64*)aValue;

            sIdx = 0;
            
            stlMemcpy( &sWriteBuff[sIdx], &sBody, STL_SIZEOF( knpPropertyFileBody ) );
            sIdx += STL_SIZEOF( knpPropertyFileBody );
            
            stlMemcpy( &sWriteBuff[sIdx], aContent->mName, stlStrlen( aContent->mName ) );
            sIdx += stlStrlen( aContent->mName );
            
            stlMemcpy( &sWriteBuff[sIdx], &sTmpNumberValue, sBody.mValueLength );
            sIdx += sBody.mValueLength;
            break;
        }
        case KNL_PROPERTY_TYPE_VARCHAR:
        {
            sBody.mValueLength = stlStrlen( aValue );
            
            sIdx = 0;
            
            stlMemcpy( &sWriteBuff[sIdx], &sBody, STL_SIZEOF( knpPropertyFileBody ) );
            sIdx += STL_SIZEOF( knpPropertyFileBody );
            
            stlMemcpy( &sWriteBuff[sIdx], aContent->mName, stlStrlen( aContent->mName ) );
            sIdx += stlStrlen( aContent->mName );
            
            stlMemcpy( &sWriteBuff[sIdx], aValue, sBody.mValueLength );
            sIdx += sBody.mValueLength;
            break;
        }
        case KNL_PROPERTY_TYPE_MAX:
        default:
        {
            STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
            break;
        }
    }
    
    STL_TRY( stlWriteFile( &sFile,
                           sWriteBuff,
                           sIdx,
                           &sWrittenBytes,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sWrittenBytes == sIdx, RAMP_ERR_INVALID_FILE_PROCESS );
    
    sStatus = 2;
    (void) stlFreeHeap( sWriteBuff );
    
    /**
     * PropertyFileHeader Update
     */
    sHeader.mPropertyCount++;
    sHeader.mFileSize += sIdx;
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY( stlWriteFile( &sFile,
                           (void*)&sHeader,
                           STL_SIZEOF( knpPropertyFileHeader ),
                           &sWrittenBytes,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sWrittenBytes == STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    sStatus = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    switch( sStatus )
    {
        case 3:
            (void) stlFreeHeap( sWriteBuff );
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  KNL_ERROR_STACK( aKnlEnv ) );
        case 1:
            (void) stlCloseFile( &sFile,
                                 KNL_ERROR_STACK( aKnlEnv ) );
        default:
            break;   
    }
    
    return STL_FAILURE;         
}


/**
 * @brief Binary 프로퍼티 파일에 해당 프로퍼티의 값을 삭제한다.
 * @param[in]     aFileName   BinaryFileName
 * @param[in]     aContent    knpPropertyContent
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS]
 * 파일 처리가 정상적이지 않습니다.
 * - 파일 처리 또는 파일을 확인해야 합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * [KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST]
 * File이 존재하지 않습니다.
 * - 해당 경로에 파일이 있는지 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpRemovePropertyAtBinaryFile ( stlChar             * aFileName,
                                          knpPropertyContent  * aContent,
                                          knlEnv              * aKnlEnv )
{
    stlFile               sFile;
    stlChar             * sFileContent     = NULL;      /** File 내용을 저장하기 위한 Buffer */
    
    stlOffset             sOffset          = 0;
    stlOffset             sFileSize        = 0;
    stlOffset             sReadOffset      = 0;         /** File에서 읽은 Offset (누적됨) */
    stlSize               sReadBytes       = 0;         /** File에서 한번에 읽은 Byte Size */
    stlSize               sWrittenBytes    = 0;         /** File에 한번에 쓴 Byte Size */
    
    stlBool               sFound           = STL_FALSE;
    stlBool               sFileExist       = STL_FALSE;
    stlInt32              sStatus          = 0;
    stlInt32              sIdx             = 0;
    
    stlChar               sPropertyName[KNL_PROPERTY_NAME_MAX_LENGTH];
    
    knpPropertyFileHeader sHeader;
    knpPropertyFileBody   sBody;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aContent  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /** File Exist ? */
    STL_TRY( knpGetFileExist( aFileName, 
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_SKIP );
    
    /** File Open */
    STL_TRY ( stlOpenFile( &sFile,
                           aFileName,
                           STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 2;
    
    /** File Validation : file exist but blank */
    sFileSize = 0;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileSize >= STL_SIZEOF( knpPropertyFileHeader ),
                   RAMP_ERR_INVALID_FILE_PROCESS );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sOffset == 0, RAMP_ERR_INVALID_FILE_PROCESS );
    
    /** Read Header */
    sReadBytes = 0;
    STL_TRY( stlReadFile( &sFile,
                          (void*)&sHeader,
                          STL_SIZEOF(knpPropertyFileHeader),
                          &sReadBytes,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileHeader ),
                   RAMP_ERR_INVALID_FILE_PROCESS );
    
    STL_TRY_THROW( sHeader.mFileSize == sFileSize,
                   RAMP_ERR_INVALID_FILE_PROCESS );
    
    
    /** if .. Property exist */
    if( sHeader.mPropertyCount > 0 )
    {
        /** Memory Allocation for Property */
        STL_TRY( stlAllocHeap( (void**)&sFileContent, 
                               sFileSize, 
                               KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
        sStatus = 3;
        
        /** 
         * 파일에서 읽은 Offset을 저장
         * 해당 시점에서 읽은 Offset이 아니라, 바로 전에 읽었던 Property의 Offset을 의미한다.
         */
        sReadOffset = STL_SIZEOF(knpPropertyFileHeader);
        
        /** Validate Property Exist in File*/
        for( sIdx = 0; sIdx < sHeader.mPropertyCount; sIdx++ )
        {
            /** re-initialize */
            stlMemset( sPropertyName, 0x00, STL_SIZEOF( sPropertyName ) );
            sReadBytes   = 0;
        
            /** Read knpPropertyFileContent */
            STL_TRY( stlReadFile( &sFile,
                                  &sBody,
                                  STL_SIZEOF( knpPropertyFileBody ),
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
        
            STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileBody ),
                           RAMP_ERR_INVALID_FILE_PROCESS);
            
            STL_TRY( stlReadFile( &sFile,
                                  sPropertyName,
                                  sBody.mNameLength,
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
        
            STL_TRY_THROW( sReadBytes == sBody.mNameLength,
                           RAMP_ERR_INVALID_FILE_PROCESS);

            STL_TRY( stlReadFile( &sFile,
                                  sFileContent,         //임시로 파일을 읽기위해 사용한다.
                                  sBody.mValueLength,
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
            
            /** if Property Found at Binary file */
            if ( stlStrcmp( aContent->mName, sPropertyName ) == 0 )
            {
                /**
                 * 만약 파일에서 해당 프로퍼티가 있다면 다음의 과정을 거친다.
                 * 1. 이후 File내용을 읽어온다.
                 * 2. 전에 읽었던 Property의 Offset으로 이동한 뒤(sReadOffset을 이용)
                 * 3. 1번 단계에서 읽었던 내용을 쓴다. (즉, 해당 Property 내용을 지운다.)
                 * 4. Header를 변경한다.
                 */
                
                sFound = STL_TRUE;      //Header의 mPropertyCount를 변경하기 위함
                
                stlMemset( sFileContent, 0x00, sFileSize );
                sReadBytes   = 0;
                
                /**
                 * 마지막 Property가 아닐 경우
                 */
                if ( sHeader.mPropertyCount-1 != sIdx )
                {        
                    /**
                     * 1. 이후 File 내용을 읽어온다
                     */
                    STL_TRY( stlReadFile( &sFile,
                                          (void*)sFileContent,
                                          sFileSize,        //끝까지 읽는다.
                                          &sReadBytes,      //실제 읽어온 사이즈
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                
                    /** 2. Offset 이동 */
                    STL_TRY( stlSeekFile( &sFile,
                                          STL_FSEEK_SET,
                                          &sReadOffset,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                     
                    /** 3. 1번에 복사한 내용을 파일에 쓴다. 나머지를 Truncate 시킨다. */
                    STL_TRY( stlWriteFile( &sFile,
                                           (void*)sFileContent,
                                           sReadBytes,
                                           &sWrittenBytes,
                                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                
                    STL_TRY_THROW( sReadBytes == sWrittenBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                
                    /**
                     * Truncate : Write한 이후부터 Truncate 해야함 
                     * 기존에 있던 데이터보다 작을 수 있으므로 다음 내용을 제거해야 한다.
                     */
                    STL_TRY( stlTruncateFile( &sFile,
                                              sReadOffset + sWrittenBytes,
                                              KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                 
                    break;   
                }
                else
                {
                    /**
                     * 마지막 Property일 경우에는 File을 이미 다 읽었기 때문에 Replace 처리가 필요하지 않다.
                     * 다만 마지막 Property를 날려야 하기 때문에 Truncate 처리는 해야한다.
                     */
                    /** 1. Offset 이동 */
                    STL_TRY( stlSeekFile( &sFile,
                                          STL_FSEEK_SET,
                                          &sReadOffset,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    /**
                     * 2. Truncate : 전에 읽은 Offset 이후부터 다 날리면 된다.
                     */
                    STL_TRY( stlTruncateFile( &sFile,
                                              sReadOffset,
                                              KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    break;
                }
            }// end if 
            
            /** Offset 변경 */
            sReadOffset += STL_SIZEOF( knpPropertyFileBody ) +                   //Body Length
                           sBody.mNameLength +                                   //Name Length
                           sBody.mValueLength;                                   //Value Length
            
        } // end for..loop
        
        sStatus = 2;
        (void) stlFreeHeap( sFileContent );
    } // end if
    
    /** Header 정보 세팅 */
    if( sFound == STL_TRUE )
    {
        sOffset = 0;        
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_END,
                              &sOffset,
                              KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );

        sHeader.mFileSize = (stlInt64)sOffset;
        sHeader.mPropertyCount--;

        /**
         * 4. File 처음으로 이동한 이후에, Header를 변경한다.
         */
        sOffset = 0;
        STL_TRY( stlSeekFile( &sFile,
                              STL_FSEEK_SET,
                              &sOffset,
                              KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
        STL_TRY( stlWriteFile( &sFile,
                               (void*)&sHeader,
                               STL_SIZEOF(knpPropertyFileHeader),
                               &sWrittenBytes,
                               KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
        
        STL_TRY_THROW( sWrittenBytes == STL_SIZEOF(knpPropertyFileHeader),
                       RAMP_ERR_INVALID_FILE_PROCESS );
    }
    
    sStatus = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );

    /*
     * 프로퍼티 파일에 더 이상 프로퍼티가 존재하지 않으면 파일 삭제
     */
    if( sHeader.mPropertyCount == 0 )
    {
        STL_TRY( stlRemoveFile( aFileName,
                                KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    }
    
    STL_RAMP( RAMP_SKIP );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_FINISH;
    
    switch( sStatus )
    {
        case 3:
            (void) stlFreeHeap( sFileContent );
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  KNL_ERROR_STACK( aKnlEnv ) );
        case 1:
            (void) stlCloseFile( &sFile,
                                 KNL_ERROR_STACK( aKnlEnv ) );
        default:
            break;   
    }
    
    return STL_FAILURE;         
}


/**
 * @brief Binary 프로퍼티 파일을 읽는다.
 * @param[in]     aFileName   FileName
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS]
 * 파일 처리가 정상적이지 않습니다.
 * - 파일 처리 또는 파일을 확인해야 합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST]
 * File이 존재하지 않습니다.
 * - 해당 경로에 파일이 있는지 확인합니다.
 * @endcode
 * @remarks
 * <BR> 
 */

stlStatus knpProcessBinaryPropertyFile( stlChar  * aFileName,
                                        knlEnv   * aKnlEnv )
{
    stlOffset             sOffset          = 0;
    stlOffset             sFileSize        = 0;
    stlSize               sReadBytes       = 0;         /** File에서 한번에 읽은 Byte Size */
    
    stlInt32              sStatus          = 0;
    stlInt32              sIdx             = 0;
    stlBool               sFileExist       = STL_FALSE;
    
    stlFile               sFile;
    stlChar               sTmpStringValue[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlChar               sPropertyName[KNL_PROPERTY_NAME_MAX_LENGTH];
    stlInt64              sReadValue  = 0;
    stlBool               sTmpBoolValue = STL_FALSE;
    
    knpPropertyFileBody     sBody;
    knpPropertyFileHeader   sHeader;    
    knpPropertyContent    * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFileName != NULL, KNL_ERROR_STACK( aKnlEnv ) );    
    
    /** File Exist ? */
    STL_TRY( knpGetFileExist( aFileName, 
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
     /** File Open */
    STL_TRY ( stlOpenFile( &sFile,
                           aFileName,
                           STL_FOPEN_READ | STL_FOPEN_WRITE | STL_FOPEN_BINARY,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_EXCLUSIVE,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 2;
    
    /** File Validation : file exist but blank */
    sFileSize = 0;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileSize >= STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sOffset == 0, RAMP_ERR_INVALID_FILE_PROCESS );
    
    /** Read Header */
    sReadBytes = 0;
    STL_TRY( stlReadFile( &sFile,
                          (void*)&sHeader,
                          STL_SIZEOF( knpPropertyFileHeader ),
                          &sReadBytes,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    STL_TRY_THROW( sHeader.mFileSize == sFileSize, RAMP_ERR_INVALID_FILE_PROCESS );

    /** if .. Property exist */
    if( sHeader.mPropertyCount > 0 )
    {
        /** Validate Property Exist in File*/
        for( sIdx = 0; sIdx < sHeader.mPropertyCount; sIdx++ )
        {
            /** re-initialize */
            sContent     = NULL;
            sReadBytes   = 0;
        
            /** Read knpPropertyFileContent */
            STL_TRY( stlReadFile( &sFile,
                                  (void*)&sBody,
                                  STL_SIZEOF( knpPropertyFileBody ),
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
        
            STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileBody ), RAMP_ERR_INVALID_FILE_PROCESS);
            
            STL_TRY_THROW( sBody.mNameLength <= KNL_PROPERTY_NAME_MAX_LENGTH, RAMP_ERROR_INVALID_PROPERTY_NAME );
            
            stlMemset( sPropertyName, 0x00, STL_SIZEOF( sPropertyName ) );
            
            STL_TRY( stlReadFile( &sFile,
                                  sPropertyName,
                                  sBody.mNameLength,
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
            
            STL_TRY_THROW( sReadBytes == sBody.mNameLength, RAMP_ERR_INVALID_FILE_PROCESS);
            
            //Get Property Handle using Name
            STL_TRY( knpGetPropertyHandleByName( sPropertyName,
                                                 (void**)&sContent,
                                                 aKnlEnv ) == STL_SUCCESS );
            
            STL_DASSERT( sContent != NULL );
            
            /** Read Value */
            switch( sContent->mDataType )
            {
                case KNL_PROPERTY_TYPE_BOOL:
                {
                    STL_TRY( stlReadFile( &sFile,
                                          (void*)&sReadValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    /**
                     * BINARY FILE Write시 Value Validation하기 때문에 
                     * read시에는 Validation을 하지 않음
                     */
                    sTmpBoolValue = (stlBool)sReadValue;
                    STL_TRY( knpAddPropertyValue( sContent,
                                                  (void*)&sTmpBoolValue,
                                                  KNL_PROPERTY_MODE_SYS,
                                                  STL_FALSE,
                                                  aKnlEnv ) == STL_SUCCESS );
                    
                    sContent->mSource = KNP_SOURCE_BINARYFILE;
                    break;
                }
                case KNL_PROPERTY_TYPE_BIGINT:
                {
                    STL_TRY( stlReadFile( &sFile,
                                          (void*)&sReadValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    STL_TRY( knpAddPropertyValue( sContent,
                                                  (void*)&sReadValue,
                                                  KNL_PROPERTY_MODE_SYS,
                                                  STL_FALSE,
                                                  aKnlEnv ) == STL_SUCCESS );
                    
                    sContent->mSource = KNP_SOURCE_BINARYFILE;                    
                    break;
                }
                case KNL_PROPERTY_TYPE_VARCHAR:
                {
                    stlMemset( sTmpStringValue, 0x00, STL_SIZEOF( sTmpStringValue ) );
                    
                    STL_TRY( stlReadFile( &sFile,
                                          (void*)&sTmpStringValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    STL_TRY( knpAddPropertyValue( sContent,
                                                  (void*)&sTmpStringValue,
                                                  KNL_PROPERTY_MODE_SYS,
                                                  STL_FALSE,
                                                  aKnlEnv ) == STL_SUCCESS );
                    
                    sContent->mSource = KNP_SOURCE_BINARYFILE;                    
                    break;
                }
                case KNL_PROPERTY_TYPE_MAX:
                default:
                {
                    STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                    break;
                }
            }// end switch
        }//end for ... loop
    }    
    
    sStatus = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERROR_INVALID_PROPERTY_NAME )
    {
        //Deprecate 처리관련 생각 좀 해봐야 함
        /*
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_ID,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
        */
    }
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aFileName );
    }
    STL_FINISH;
    
    switch( sStatus )
    {
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  KNL_ERROR_STACK( aKnlEnv ) );
        case 1:
            (void) stlCloseFile( &sFile,
                                 KNL_ERROR_STACK( aKnlEnv ) );
        default:
            break;   
    }
    
    
    return STL_FAILURE;     
}


/**
 * @brief Binary 파일에 해당 Property 값을 저장한다.
 * @param[in]     aContent    knpPropertyContent
 * @param[in]     aValue      저장할 값
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST]
 * File이 존재하지 않습니다.
 * - 해당 경로에 파일이 있는지 확인합니다.
 * @endcode
 * @remarks
 * <BR> Binary 파일과 Read Only파일은 같은 형식을 갖고 있으므로 같은 함수를 사용한다.
 */

stlStatus knpAddPropertyValueAtFile( knpPropertyContent  * aContent,
                                     void                * aValue,
                                     knlEnv              * aKnlEnv )
{
    stlChar   sBinaryFileName[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlChar   sTextFileName[KNL_PROPERTY_STRING_MAX_LENGTH]   = {0};
    stlBool   sFileExist = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aValue   != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /** Get FileName */
    stlSnprintf( sBinaryFileName, STL_SIZEOF(sBinaryFileName), 
                 "%s"STL_PATH_SEPARATOR"%s", gKnpPropertyMgr->mHomeDir, KNP_DEFAULT_BINARY_PROPERTY_FILE );
    
    /**
     * Binary Property File이 없을 경우에는 Text PropertyFile로 부터 Binary PropertyFile을 생성한다.
     * @todo 여기서 Binary를 만들지 아니면 서버 처음 시작시 만들지는 생각해 봐야함.. 
     *       그러나 여기서도 필요하다..(Startup 이후에 Text PropertyFile을 지웠을 경우)
     */
    STL_TRY( knpGetFileExist( sBinaryFileName,
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    if( sFileExist == STL_FALSE )
    {
        stlSnprintf( sTextFileName, STL_SIZEOF(sTextFileName), 
                     "%s"STL_PATH_SEPARATOR"%s", gKnpPropertyMgr->mHomeDir, KNP_DEFAULT_PROPERTY_FILE );
        
        STL_TRY( knpGetFileExist( sTextFileName,
                                  &sFileExist,
                                  aKnlEnv ) == STL_SUCCESS );
        
        STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
        
        STL_TRY( knpConvertPropertyTextToBinaryFile( sTextFileName,
                                                     sBinaryFileName,
                                                     aKnlEnv ) == STL_SUCCESS );
    }
    
    STL_TRY( knpWritePropertyAtBinaryFile( sBinaryFileName,
                                           aContent,
                                           aValue,
                                           aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      sTextFileName );
    }
    STL_FINISH;
    
    return STL_FAILURE;     
}


/**
 * @brief Binary 파일에 해당 Property 값을 삭제한다.
 * @param[in]     aContent    knpPropertyContent
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 */

stlStatus knpRemovePropertyValueAtFile( knpPropertyContent  * aContent,
                                        knlEnv              * aKnlEnv )
{
    stlChar   sBinaryFileName[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlBool   sFileExist = STL_TRUE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aContent != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /** Get FileName */
    stlSnprintf( sBinaryFileName, STL_SIZEOF(sBinaryFileName), 
                 "%s"STL_PATH_SEPARATOR"%s", gKnpPropertyMgr->mHomeDir, KNP_DEFAULT_BINARY_PROPERTY_FILE );
    
    STL_TRY( knpGetFileExist( sBinaryFileName,
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    if( sFileExist == STL_TRUE )
    {
        STL_TRY( knpRemovePropertyAtBinaryFile( sBinaryFileName,
                                                aContent,
                                                aKnlEnv ) == STL_SUCCESS );
    }
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;     
}


/**
 * @brief 해당 파일이 있는지 확인한다.
 * @param[in]     aFileName   knpPropertyContent
 * @param[out]    aIsExist    파일 존재 여부
 * @param[in,out] aKnlEnv     Kernel Environment
 * @remarks
 * <BR> 해당 파일이 있을 경우에는 aIsExist는 STL_TRUE를 리턴한다.
 */

stlStatus knpGetFileExist( stlChar  * aFileName,
                           stlBool  * aIsExist,
                           knlEnv   * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIsExist  != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /** File Exist ? */
    STL_TRY( stlExistFile( aFileName,
                           aIsExist,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;         
}


/**
 * @brief Default Text PropertyFile을 Default Binary Property File로 변환한다.
 * @param[in,out] aKnlEnv     Kernel Environment
 * @remarks
 */

stlStatus knpConvertPropertyTextToBinaryFileByDefault( knlEnv  * aKnlEnv )
{
    stlChar  sDefaultBinaryFileName[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlChar  sDefaultTextFileName[KNL_PROPERTY_STRING_MAX_LENGTH]   = {0};
    
    stlSnprintf( sDefaultBinaryFileName, 
                 STL_SIZEOF(sDefaultBinaryFileName), 
                 "%s"STL_PATH_SEPARATOR"%s", 
                 gKnpPropertyMgr->mHomeDir, 
                 KNP_DEFAULT_BINARY_PROPERTY_FILE );
    
    stlSnprintf( sDefaultTextFileName, 
                 STL_SIZEOF(sDefaultTextFileName), 
                 "%s"STL_PATH_SEPARATOR"%s", 
                 gKnpPropertyMgr->mHomeDir, 
                 KNP_DEFAULT_PROPERTY_FILE );  
        
    STL_TRY( knpConvertPropertyTextToBinaryFile( sDefaultTextFileName,
                                                 sDefaultBinaryFileName,
                                                 aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Default Binary PropertyFile을 Default Text Property File로 변환한다.
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * @endcode
 * @remarks
 */

stlStatus knpConvertPropertyBinaryToTextFileByDefault( knlEnv  * aKnlEnv )
{
    stlChar  sDefaultBinaryFileName[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlChar  sDefaultTextFileName[KNL_PROPERTY_STRING_MAX_LENGTH]   = {0};
    
    stlSnprintf( sDefaultBinaryFileName, 
                 STL_SIZEOF( sDefaultBinaryFileName ), 
                 "%s"STL_PATH_SEPARATOR"%s", 
                 gKnpPropertyMgr->mHomeDir, 
                 KNP_DEFAULT_BINARY_PROPERTY_FILE );
    
    stlSnprintf( sDefaultTextFileName, 
                 STL_SIZEOF( sDefaultTextFileName ), 
                 "%s"STL_PATH_SEPARATOR"%s", 
                 gKnpPropertyMgr->mHomeDir, 
                 KNP_DEFAULT_PROPERTY_FILE );  
    
    STL_TRY( knpConvertPropertyBinaryToTextFile( sDefaultBinaryFileName,
                                                 sDefaultTextFileName,
                                                 aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Binary PropertyFile을 Text Property File로 변환한다.
 * @param[in]     aBFileName  Binary Property FileName
 * @param[in]     aTFileName  Text Property FileName
 * @param[in,out] aKnlEnv     Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS]
 * 파일 처리가 정상적이지 않습니다.
 * - 파일 처리 또는 파일을 확인해야 합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_ID]
 * 해당 Property의 ID가 정상적이지 않습니다.
 * - 해당 Property의 ID를 확인합니다.
 * [KNL_ERRCODE_PROPERTY_INVALID_DATATYPE]
 * 해당 Property의 DataType이 정상적이지 않습니다.
 * - 해당 Property의 DataType을 확인합니다.
 * [KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST]
 * File이 존재하지 않습니다.
 * - 해당 경로에 파일이 있는지 확인합니다.
 * @endcode
 * @remarks
 */

stlStatus knpConvertPropertyBinaryToTextFile( stlChar  * aBFileName,
                                              stlChar  * aTFileName,  
                                              knlEnv   * aKnlEnv )
{
    stlOffset             sOffset          = 0;
    stlOffset             sFileSize        = 0;
    stlSize               sReadBytes       = 0;         /** File에서 한번에 읽은 Byte Size */
    
    stlInt32              sStatus          = 0;
    stlInt32              sIdx             = 0;
    stlBool               sFileExist       = STL_FALSE;
    stlChar               sPropertyName[KNL_PROPERTY_NAME_MAX_LENGTH] = {0};
    
    stlChar               sWriteBuffer[KNL_PROPERTY_NAME_MAX_LENGTH + KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlInt32              sWriteLength  = 0;
    stlSize               sWrittenBytes = 0;
            
    stlFile               sTextFile;
    stlFile               sBinaryFile;
    stlChar               sTmpStringValue[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlInt64              sTmpNumberValue  = 0;
    stlInt64              sTmpBoolValue    = 0;
    
    knpPropertyFileBody     sBody;
    knpPropertyFileHeader   sHeader;    
    knpPropertyContent    * sContent = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aBFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aTFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    /** File Exist ? 
     * Source가 되는 Binary PropertyFile은 반드시 있어야 하며,
     * Dest가 되는 Text PropertyFile은 파일이 존재하더라도 덮어씌운다.
     */
    STL_TRY( knpGetFileExist( aBFileName, 
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
    STL_TRY( knpGetFileExist( aTFileName,
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    if ( sFileExist == STL_TRUE )
    {
        /** Remove File */
        STL_TRY( stlRemoveFile( aTFileName,
                                KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS ); 
    }
    
     /** File Open */
    STL_TRY ( stlOpenFile( &sBinaryFile,
                           aBFileName,
                           STL_FOPEN_READ | STL_FOPEN_BINARY,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 1;
    
    STL_TRY( stlLockFile( &sBinaryFile, 
                          STL_FLOCK_SHARED,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 2;
    
    STL_TRY ( stlOpenFile( &sTextFile,
                           aTFileName,
                           STL_FOPEN_CREATE | STL_FOPEN_EXCL | STL_FOPEN_WRITE,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 3;
    
    /** File Validation : file exist but blank */
    sFileSize = 0;
    
    STL_TRY( stlSeekFile( &sBinaryFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileSize >= STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sBinaryFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sOffset == 0, RAMP_ERR_INVALID_FILE_PROCESS );
    
    /** Read Header */
    sReadBytes = 0;
    STL_TRY( stlReadFile( &sBinaryFile,
                          (void*)&sHeader,
                          STL_SIZEOF( knpPropertyFileHeader ),
                          &sReadBytes,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    STL_TRY_THROW( sHeader.mFileSize == sFileSize, RAMP_ERR_INVALID_FILE_PROCESS );

    /** if .. Property exist */
    if( sHeader.mPropertyCount > 0 )
    {
        /** Validate Property Exist in File*/
        for( sIdx = 0; sIdx < sHeader.mPropertyCount; sIdx++ )
        {
            /** re-initialize */
            sContent     = NULL;
            sReadBytes   = 0;
            sWriteLength = 0;
            stlMemset( sWriteBuffer,  0x00, STL_SIZEOF( sWriteBuffer ) );
            stlMemset( sPropertyName, 0x00, STL_SIZEOF( sPropertyName ) );
        
            /** Read knpPropertyFileContent */
            STL_TRY( stlReadFile( &sBinaryFile,
                                  (void*)&sBody,
                                  STL_SIZEOF( knpPropertyFileBody ),
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
        
            STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileBody ), RAMP_ERR_INVALID_FILE_PROCESS );
            
            STL_TRY( stlReadFile( &sBinaryFile,
                                  sPropertyName,
                                  sBody.mNameLength,
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
            
            STL_TRY_THROW( sReadBytes == sBody.mNameLength, RAMP_ERR_INVALID_FILE_PROCESS );
            
            //Get Property Handle using Name
            STL_TRY( knpGetPropertyHandleByName( sPropertyName,
                                                 (void**)&sContent,
                                                 aKnlEnv ) == STL_SUCCESS );
            
            STL_DASSERT( sContent != NULL );
            
            /** Read Value */
            switch( sContent->mDataType )
            {
                case KNL_PROPERTY_TYPE_BOOL:
                {
                    STL_TRY( stlReadFile( &sBinaryFile,
                                          (void*)&sTmpBoolValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    if ( sTmpBoolValue == STL_TRUE )
                    {
                        sWriteLength = stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                                    "%s = TRUE\n", sPropertyName );
                    }
                    else
                    {
                        sWriteLength = stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                                    "%s = FALSE\n", sPropertyName );
                    }
                    
                    STL_TRY( stlWriteFile( &sTextFile,
                                           (void*)sWriteBuffer,
                                           sWriteLength,
                                           &sWrittenBytes,
                                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );                        
                    
                    STL_TRY_THROW( sWriteLength == sWrittenBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    break;
                }
                case KNL_PROPERTY_TYPE_BIGINT:
                {
                    STL_TRY( stlReadFile( &sBinaryFile,
                                          (void*)&sTmpNumberValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    sWriteLength = stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                                "%s = %ld\n", sPropertyName, sTmpNumberValue );
                    
                    STL_TRY( stlWriteFile( &sTextFile,
                                           (void*)sWriteBuffer,
                                           sWriteLength,
                                           &sWrittenBytes,
                                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );   
                    
                    STL_TRY_THROW( sWriteLength == sWrittenBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    break;
                }
                case KNL_PROPERTY_TYPE_VARCHAR:
                {
                    STL_TRY( stlReadFile( &sBinaryFile,
                                          (void*)&sTmpStringValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    sWriteLength = stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                                "%s = '%s'\n", sPropertyName, sTmpStringValue );
                    
                    STL_TRY( stlWriteFile( &sTextFile,
                                           (void*)sWriteBuffer,
                                           sWriteLength,
                                           &sWrittenBytes,
                                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );   
                    
                    STL_TRY_THROW( sWriteLength == sWrittenBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    break;
                }
                case KNL_PROPERTY_TYPE_MAX:
                default:
                {
                    STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                    break;
                }
            }// end switch
        }//end for ... loop
    }    
    
    sStatus = 2;
    STL_TRY( stlCloseFile( &sTextFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 1;
    STL_TRY( stlUnlockFile( &sBinaryFile, 
                            KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sBinaryFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aBFileName );
    }
    STL_FINISH;
    
    switch(sStatus)
    {
        case 3:
            (void) stlCloseFile( &sTextFile,
                                 KNL_ERROR_STACK( aKnlEnv ) );
        case 2:
            (void) stlUnlockFile( &sBinaryFile, 
                                  KNL_ERROR_STACK( aKnlEnv ) );
        case 1:
            (void) stlCloseFile( &sBinaryFile,
                                 KNL_ERROR_STACK( aKnlEnv ) );
        default:
            break;   
    }
    
    return STL_FAILURE;       
}

stlStatus knpDumpBinaryPropertyFile( stlChar  * aFileName,
                                     knlEnv   * aKnlEnv )
{
    stlOffset             sOffset          = 0;
    stlOffset             sFileSize        = 0;
    stlSize               sReadBytes       = 0;         /** File에서 한번에 읽은 Byte Size */
    
    stlInt32              sStatus          = 0;
    stlInt32              sIdx             = 0;
    stlBool               sFileExist       = STL_FALSE;
    stlChar               sPropertyName[KNL_PROPERTY_NAME_MAX_LENGTH] = {0};
    
    stlChar               sWriteBuffer[KNL_PROPERTY_NAME_MAX_LENGTH + KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
            
    stlFile               sFile;
    stlChar               sTmpStringValue[KNL_PROPERTY_STRING_MAX_LENGTH] = {0};
    stlInt64              sTmpNumberValue  = 0;
    stlInt64              sTmpBoolValue    = 0;
    
    knpPropertyFileBody     sBody;
    knpPropertyFileHeader   sHeader;    
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFileName != NULL, KNL_ERROR_STACK(aKnlEnv) );
    
    STL_TRY( knpGetFileExist( aFileName, 
                              &sFileExist,
                              aKnlEnv ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileExist == STL_TRUE, RAMP_ERR_FILE_NOT_EXIST );
    
     /** File Open */
    STL_TRY ( stlOpenFile( &sFile,
                           aFileName,
                           STL_FOPEN_READ | STL_FOPEN_BINARY,
                           STL_FPERM_OS_DEFAULT,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 1;
    
    STL_TRY( stlLockFile( &sFile, 
                          STL_FLOCK_SHARED, //STL_FLOCK_EXCLUSIVE,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    sStatus = 2;
    
    /** File Validation : file exist but blank */
    sFileSize = 0;
    
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_END,
                          &sFileSize,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sFileSize >= STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    sOffset = 0;
    STL_TRY( stlSeekFile( &sFile,
                          STL_FSEEK_SET,
                          &sOffset,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sOffset == 0, RAMP_ERR_INVALID_FILE_PROCESS );
    
    /** Read Header */
    sReadBytes = 0;
    STL_TRY( stlReadFile( &sFile,
                          (void*)&sHeader,
                          STL_SIZEOF( knpPropertyFileHeader ),
                          &sReadBytes,
                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileHeader ), RAMP_ERR_INVALID_FILE_PROCESS );
    
    STL_TRY_THROW( sHeader.mFileSize == sFileSize, RAMP_ERR_INVALID_FILE_PROCESS );

    /** if .. Property exist */
    if( sHeader.mPropertyCount > 0 )
    {
        /** Validate Property Exist in File*/
        for( sIdx = 0; sIdx < sHeader.mPropertyCount; sIdx++ )
        {
            /** re-initialize */
            sReadBytes   = 0;
            stlMemset( sWriteBuffer,  0x00, STL_SIZEOF( sWriteBuffer ) );
            stlMemset( sPropertyName, 0x00, STL_SIZEOF( sPropertyName ) );
        
            /** Read knpPropertyFileContent */
            STL_TRY( stlReadFile( &sFile,
                                  (void*)&sBody,
                                  STL_SIZEOF( knpPropertyFileBody ),
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
        
            STL_TRY_THROW( sReadBytes == STL_SIZEOF( knpPropertyFileBody ), RAMP_ERR_INVALID_FILE_PROCESS );
            
            STL_TRY( stlReadFile( &sFile,
                                  sPropertyName,
                                  sBody.mNameLength,
                                  &sReadBytes,
                                  KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
            
            STL_TRY_THROW( sReadBytes == sBody.mNameLength, RAMP_ERR_INVALID_FILE_PROCESS );
            
            /** Read Value */
            switch( sBody.mDataType )
            {
                case KNL_PROPERTY_TYPE_BOOL:
                {
                    STL_TRY( stlReadFile( &sFile,
                                          (void*)&sTmpBoolValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    if ( sTmpBoolValue == STL_TRUE )
                    {
                        stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                     "%s = TRUE\n", sPropertyName );
                    }
                    else
                    {
                        stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                     "%s = FALSE\n", sPropertyName );
                    }
                    stlPrintf("%s\n", sWriteBuffer );
                    
                    break;
                }
                case KNL_PROPERTY_TYPE_BIGINT:
                {
                    STL_TRY( stlReadFile( &sFile,
                                          (void*)&sTmpNumberValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                 "%s = %ld\n", sPropertyName, sTmpNumberValue );
                    
                    stlPrintf("%s\n", sWriteBuffer );                    
                    break;
                }
                case KNL_PROPERTY_TYPE_VARCHAR:
                {
                    STL_TRY( stlReadFile( &sFile,
                                          (void*)&sTmpStringValue,
                                          sBody.mValueLength,
                                          &sReadBytes,
                                          KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
                    
                    STL_TRY_THROW( sBody.mValueLength == sReadBytes, RAMP_ERR_INVALID_FILE_PROCESS );
                    
                    stlSnprintf( sWriteBuffer, STL_SIZEOF( sWriteBuffer ),
                                 "%s = '%s'\n", sPropertyName, sTmpStringValue );
                    
                    stlPrintf("%s\n", sWriteBuffer );
                    break;
                }
                case KNL_PROPERTY_TYPE_MAX:
                default:
                {
                    STL_THROW( RAMP_ERROR_INVALID_DATATYPE );
                    break;
                }
            }// end switch
        }//end for ... loop
    }    

    sStatus = 1;
    STL_TRY( stlUnlockFile( &sFile, 
                            KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    sStatus = 0;
    STL_TRY( stlCloseFile( &sFile,
                           KNL_ERROR_STACK( aKnlEnv ) ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_CATCH( RAMP_ERR_INVALID_FILE_PROCESS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_FILE_PROCESS,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERROR_INVALID_DATATYPE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_INVALID_DATATYPE,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_FILE_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_PROPERTY_FILE_NOT_EXIST,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv),
                      aFileName );
    }
    STL_FINISH;
    
    switch(sStatus)
    {
        case 2:
            (void) stlUnlockFile( &sFile, 
                                  KNL_ERROR_STACK( aKnlEnv ) );
        case 1:
            (void) stlCloseFile( &sFile,
                                 KNL_ERROR_STACK( aKnlEnv ) );
        default:
            break;   
    }
    
    return STL_FAILURE;
}

/** @} */
