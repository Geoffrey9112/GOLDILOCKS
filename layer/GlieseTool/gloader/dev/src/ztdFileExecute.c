/*******************************************************************************
 * ztdFileExecute.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
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
#include <ztdDef.h>
#include <ztdDisplay.h>
#include <ztdFileExecute.h>

stlFile              gZtdBadFile;
stlInt32             gZtdBinaryDataFileCount;
extern ztdFormatType gZtdFormatType;
extern ztdModeType   gZtdModeType;

/**
 * @brief ztdCloseFiles()는 작업에 필요한 File들을 close한다.
 * @param [in]     aFileAndBuffer    File 포인터를 지닌 struct.
 * @param [in/out] aErrorStack       error stack
 */
stlStatus ztdCloseFiles( ztdFileAndBuffer * aFileAndBuffer,
                         stlErrorStack    * aErrorStack )
{

    if( aFileAndBuffer->mControlFlag == STL_TRUE )
    {
        STL_TRY( stlCloseFile( &(aFileAndBuffer->mControlFile),
                               aErrorStack )
                 == STL_SUCCESS );
    }
    
    if( aFileAndBuffer->mDataFlag == STL_TRUE )
    {
        STL_TRY( stlCloseFile( &(aFileAndBuffer->mDataFile),
                               aErrorStack )
                 == STL_SUCCESS );
    }
    
    if( aFileAndBuffer->mLogFlag == STL_TRUE )
    {
        STL_TRY( stlCloseFile( &(aFileAndBuffer->mLogFile),
                               aErrorStack )
                 == STL_SUCCESS );
    }

    if( aFileAndBuffer->mBadFlag == STL_TRUE )
    {
        STL_TRY( stlCloseFile( &(aFileAndBuffer->mBadFile),
                               aErrorStack )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;    
}

/**
 * @brief ztdOpenFiles()는 작업에 필요한 File들을 open한다.
 * @param [in]  aFileAndBuffer   FILE 포인터를 지닌 struct
 * @param [in]  aInputArguments  command Arguments를 저장한 struct
 * @param [out] aErrorStack      error stack 
 */
stlStatus ztdOpenFiles( ztdFileAndBuffer  * aFileAndBuffer,
                        ztdInputArguments * aInputArguments,
                        stlErrorStack     * aErrorStack  )
{
    stlInt32  sFlag;
    stlInt32  sPerm;

    aFileAndBuffer->mLogBuffer[0]  = '\0';
    
    aFileAndBuffer->mBadRecord     = NULL;
    
    aFileAndBuffer->mControlFlag   = STL_FALSE;
    aFileAndBuffer->mDataFlag      = STL_FALSE;
    aFileAndBuffer->mLogFlag       = STL_FALSE;
    aFileAndBuffer->mBadFlag       = STL_FALSE;
    
    sPerm = STL_FPERM_UREAD | STL_FPERM_UWRITE |
        STL_FPERM_GREAD | STL_FPERM_GWRITE |
        STL_FPERM_WREAD | STL_FPERM_WWRITE ;
    
    STL_TRY( stlOpenFile( &(aFileAndBuffer->mControlFile),
                          aInputArguments->mControl,
                          STL_FOPEN_READ | STL_FOPEN_LARGEFILE,
                          sPerm,
                          aErrorStack )
             == STL_SUCCESS );
    
    if( gZtdFormatType == ZTD_FORMAT_TYPE_TEXT )
    {
        if( gZtdModeType == ZTD_MODE_TYPE_EXPORT )
        {
            sFlag = STL_FOPEN_WRITE  | STL_FOPEN_TRUNCATE |
                    STL_FOPEN_CREATE | STL_FOPEN_LARGEFILE;

            if( aInputArguments->mDirectIo == STL_TRUE )
            {
                sFlag |= STL_FOPEN_DIRECT;
            }
            
            STL_TRY( stlOpenFile( &(aFileAndBuffer->mDataFile),
                                  aInputArguments->mData,
                                  sFlag,
                                  sPerm,
                                  aErrorStack )
                     == STL_SUCCESS );
            

        }
        else if( gZtdModeType == ZTD_MODE_TYPE_IMPORT )
        {
            sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE;

            if( aInputArguments->mDirectIo == STL_TRUE )
            {
                sFlag |= STL_FOPEN_DIRECT;
            }

            STL_TRY( stlOpenFile( &(aFileAndBuffer->mDataFile),
                                  aInputArguments->mData,
                                  sFlag,
                                  sPerm,
                                  aErrorStack )
                     == STL_SUCCESS );

            sFlag = STL_FOPEN_WRITE  | STL_FOPEN_TRUNCATE |
                    STL_FOPEN_CREATE | STL_FOPEN_LARGEFILE;

            STL_TRY( stlOpenFile( &(aFileAndBuffer->mBadFile),
                                  aInputArguments->mBad,
                                  sFlag,
                                  sPerm,
                                  aErrorStack )
                     == STL_SUCCESS );
        
            aFileAndBuffer->mBadFlag = STL_TRUE;
        }
    }
    else
    {
        /* Do Nothing */
    }

    /**
     * log file을 open한다.
     */
    sFlag = STL_FOPEN_WRITE  | STL_FOPEN_TRUNCATE |
            STL_FOPEN_CREATE | STL_FOPEN_LARGEFILE;

    STL_TRY( stlOpenFile( &(aFileAndBuffer->mLogFile),
                          aInputArguments->mLog,
                          sFlag,
                          sPerm,
                          aErrorStack )
             == STL_SUCCESS );

    aFileAndBuffer->mControlFlag = STL_TRUE;
    aFileAndBuffer->mDataFlag    = STL_TRUE;
    aFileAndBuffer->mLogFlag     = STL_TRUE;
    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;                         
}

/**
 * @brief ztdReadFile()은 Data File에서 Data를 읽고, Data Buffer와 EOF Flag Return.
 * @param [out]    aReadBuffer     Data를 담는 Buffer
 * @param [in]     aFileAndBuffer  File 포인터를 지닌 struct.
 * @param [in]     aDataReadSize   read size
 * @param [out]    aNoDataFlag     EOF 판단 Flag
 * @param [in/out] aErrorStack     error stack 
 */ 
stlStatus ztdReadFile( stlChar          * aReadBuffer,
                       ztdFileAndBuffer * aFileAndBuffer,
                       stlSize          * aDataReadSize,
                       stlBool          * aNoDataFlag,
                       stlErrorStack    * aErrorStack )
{
    stlStatus sRet;

    sRet = stlReadFile( &(aFileAndBuffer->mDataFile),
                        aReadBuffer,
                        ZTD_MAX_READ_RECORD_BUFFER_SIZE,
                        aDataReadSize,
                        aErrorStack );

    if( sRet == STL_FAILURE )
    {
        STL_TRY( stlGetLastErrorCode( aErrorStack ) == STL_ERRCODE_EOF );
        stlPopError( aErrorStack );        

        *aNoDataFlag = STL_TRUE;
    }
    else
    {
         aReadBuffer[ *aDataReadSize ] = ZTD_CONSTANT_NULL;
    }
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Data File 에 data를 기록한다.
 * @param [in]     aFile         file descriptor
 * @param [in/out] aRecordBuffer data buffer struct
 * @param [in/out] aErrorStack   error stack
 */
stlStatus ztdWriteData( stlFile         * aFile,
                        ztdRecordBuffer * aRecordBuffer,
                        stlErrorStack   * aErrorStack )
{
    STL_TRY( stlWriteFile( aFile,
                           aRecordBuffer->mAlignedBuffer,
                           aRecordBuffer->mBufferSize,
                           NULL,
                           aErrorStack ) == STL_SUCCESS );
    
    stlMemset( aRecordBuffer->mAlignedBuffer, ZTD_CONSTANT_SPACE, aRecordBuffer->mBufferSize );

    aRecordBuffer->mCurrentPtr = aRecordBuffer->mAlignedBuffer;
   
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief LOG File 에 log를 기록한다.
 * @param [in]  aFileAndBuffer File Buffer
 * @param [out] aErrorStack    error stack
 */
stlStatus ztdWriteLog( ztdFileAndBuffer * aFileAndBuffer,
                       stlErrorStack    * aErrorStack )
{
    STL_TRY( stlPutStringFile( aFileAndBuffer->mLogBuffer,
                               & aFileAndBuffer->mLogFile,
                               aErrorStack )
             == STL_SUCCESS );
        
    aFileAndBuffer->mLogBuffer[0] = '\0';

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BAD File 에 log를 기록한다.
 * @param [in]  aFileAndBuffer  File Buffer and String Buffer
 * @param [in]  aBadRecord      Bad Record Buffer 
 * @param [out] aErrorStack     error stack
 */
stlStatus ztdWriteBad( ztdFileAndBuffer * aFileAndBuffer,
                       ztdBadRecord     * aBadRecord,
                       stlErrorStack    * aErrorStack )
{
    STL_TRY( stlWriteFile( & aFileAndBuffer->mBadFile,
                           (void *)aBadRecord->mBuffer,
                           aBadRecord->mOffset,
                           NULL,
                           aErrorStack)
             == STL_SUCCESS );

    aBadRecord->mOffset = 0;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ztdWriteBadSync() File Wring에서 Thread들의 동기화를 위해 세마포어를 이용하여 File Writing.
 * @param [in]  aFileAndBuffer  File Buffer and String Buffer
 * @param [in]  aBadRecord      Bad Record Buffer
 * @param [in]  aSemaphore      Bad file semaphore
 * @param [out] aErrorStack     error stack
 */ 
stlStatus ztdWriteBadSync( ztdFileAndBuffer * aFileAndBuffer,
                           ztdBadRecord     * aBadRecord,
                           stlSemaphore     * aSemaphore,
                           stlErrorStack    * aErrorStack )
{
    stlInt32 sState = 0;
    
    STL_TRY( stlAcquireSemaphore( aSemaphore,
                                  aErrorStack )
             == STL_SUCCESS );

    sState = 1;
    STL_TRY( ztdWriteBad( aFileAndBuffer,
                          aBadRecord,
                          aErrorStack )
             == STL_SUCCESS );

    sState = 2;
    STL_TRY( stlReleaseSemaphore( aSemaphore,
                                  aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
        case 1:
            (void) stlReleaseSemaphore( aSemaphore, aErrorStack );
        default:
            break;
    }
    
    return STL_FAILURE;
}

/**
 * @brief 새로운 data file을 생성하여 반환한다.
 * @param [out]     aFileDesc       write buffered file
 * @param [in]      aControlInfo    control infomation
 * @param [in/out]  aErrorStack     error stack
 */
stlStatus ztdCreateDataFile( stlFile           * aFileDesc,
                             ztdInputArguments * aInputArguments,
                             stlErrorStack     * aErrorStack )
{
    stlChar  sDataFileName[STL_MAX_FILE_NAME_LENGTH];
    stlInt32 sPerm;
    stlInt32 sFlag;

    sPerm = STL_FPERM_UREAD | STL_FPERM_UWRITE |
            STL_FPERM_GREAD | STL_FPERM_GWRITE |
            STL_FPERM_WREAD | STL_FPERM_WWRITE ;

    sFlag = STL_FOPEN_WRITE  | STL_FOPEN_BINARY   |
            STL_FOPEN_CREATE | STL_FOPEN_TRUNCATE |
            STL_FOPEN_LARGEFILE;

    if( aInputArguments->mDirectIo == STL_TRUE )
    {
        sFlag |=STL_FOPEN_DIRECT;
    }

    /**
     * 새로운 data file의 이름을 생성한다.
     */
    if( gZtdBinaryDataFileCount == 0 )
    {
        stlStrncpy( sDataFileName,
                    aInputArguments->mData,
                    STL_MAX_FILE_NAME_LENGTH );
    }
    else
    {
        stlSnprintf( sDataFileName,
                     STL_MAX_FILE_NAME_LENGTH,
                     "%s.%03d",
                     aInputArguments->mData,
                     gZtdBinaryDataFileCount );
    }
    
    /**
     * open file
     */
    STL_TRY( stlOpenFile( aFileDesc,
                          sDataFileName,
                          sFlag,
                          sPerm,
                          aErrorStack )
             == STL_SUCCESS );

    gZtdBinaryDataFileCount++;

    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}
