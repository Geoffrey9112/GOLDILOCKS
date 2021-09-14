/*******************************************************************************
 * ztdMain.c
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
#include <ztdDef.h>
#include <ztdFileExecute.h>
#include <ztdBufferedFile.h>

extern stlInt32        gZtdReadBlockCount;
extern stlInt8         gZtdEndian;
extern dtlCharacterSet gZtdDatabaseCharacterSet;
extern stlInt32        gZtdBinaryFileFormatVersion;
extern SQLSMALLINT     gZtdColumnCount;
extern stlBool         gZtdRunState;
extern stlUInt32       gZtdErrorCount;
extern stlUInt32       gZtdMaxErrorCount;

/**
 * @brief 주어진 value length를 저장될 형태로 변환하고, 그 크기를 반환한다.
 * @param [out] aWritePtr         write buffer
 * @param [in]  aValueLength      data value length
 */
static stlInt32 ztdGetValueWriteLength( stlChar  * aWritePtr,
                                        stlInt64   aValueLength )
{
    stlInt32  sWriteLength = 0;
    stlUInt16 sTmpInt16    = 0;
    stlUInt32 sTmpInt32    = 0;

    if( aValueLength <= ZTD_VALUE_LEN_1BYTE_MAX )
    {
        sWriteLength = 1;
        aWritePtr[0] = aValueLength;
    }
    else if( aValueLength <= ZTD_VALUE_LEN_2BYTE_MAX )
    {
        sTmpInt16 = aValueLength;
        sWriteLength = 3;
        STL_WRITE_INT16( &aWritePtr[1], &sTmpInt16 );
        aWritePtr[0] = (stlChar)ZTD_VALUE_LEN_2BYTE_FLAG;
    }
    else
    {
        sTmpInt32 = aValueLength;
        sWriteLength = 5;
        STL_WRITE_INT32( &aWritePtr[1], &sTmpInt32 );
        aWritePtr[0] = (stlChar)ZTD_VALUE_LEN_4BYTE_FLAG;
    }

    return sWriteLength;
}
                                        
/**
 * @brief write buffered file을 초기화한다.
 * @param [out]    aWriteBufferedFile   write buffered file
 * @param [in]     aInputArguments      input arguments
 * @param [in]     aControlInfo         control information
 * @param [in]     aBufferSize          buffer size
 * @param [in/out] aErrorStack          error stack
 */
stlStatus ztdInitializeWriteBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                          ztdInputArguments    * aInputArguments,
                                          ztdControlInfo       * aControlInfo,
                                          stlAllocator         * aAllocator,
                                          stlErrorStack        * aErrorStack )
{
    stlInt32 sSize   = 0;

    stlMemset( aWriteBufferedFile,
               0x00,
               STL_SIZEOF( ztdWriteBufferedFile ) );
    
    /**
     * 파일을 write mode로 연다.
     */
    STL_TRY( ztdCreateDataFile( &aWriteBufferedFile->mFileDesc,
                                aInputArguments,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * aWriteBufferedFile의 각 멤버를 초기화한다.
     */
    aWriteBufferedFile->mCurrentBufferIndex = 0;
    aWriteBufferedFile->mCurrentFileSize    = 0;
    aWriteBufferedFile->mMaxFileSize        = aInputArguments->mMaxFileSize;

    /**
     * buffer에 메모리를 할당 받는다.
     */
    sSize = ZTD_WRITE_BUFFER_SIZE + ZTD_ALIGN_512;

    STL_TRY( stlAllocRegionMem( aAllocator,
                                sSize,
                                (void **) &aWriteBufferedFile->mBuffer,
                                aErrorStack )
             == STL_SUCCESS );

    stlMemset( aWriteBufferedFile->mBuffer, 0x00, sSize );

    ZTD_ALIGN_BUFFER( aWriteBufferedFile->mBuffer,
                      aWriteBufferedFile->mBuffer,
                      ZTD_ALIGN_512 );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief write buffered file을 종료한다.
 * @param [in]     aWriteBufferedFile   write buffered file
 * @param [in/out] aErrorStack          error stack
 */
stlStatus ztdTerminateWriteBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                         stlErrorStack        * aErrorStack )
{
   /**
     * 파일을 닫는다.
     */
    STL_TRY( stlCloseFile( &aWriteBufferedFile->mFileDesc,
                           aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief read buffered file을 초기화한다.
 * @param [out]    aReadBufferedFile    read buffered file
 * @param [in]     aInputArguments      input arguments
 * @param [in]     aBufferSize          buffer size
 * @param [in/out] aErrorStack          error stack
 */
stlStatus ztdInitializeReadBufferedFile( ztdReadBufferedFile  * aReadBufferedFile,
                                         ztdInputArguments    * aInputArguments,
                                         stlAllocator         * aAllocator,
                                         stlErrorStack        * aErrorStack )
{
    stlInt32 sPerm;
    stlInt32 sFlag;
    
    sPerm = STL_FPERM_UREAD | STL_FPERM_UWRITE |
            STL_FPERM_GREAD | STL_FPERM_GWRITE |
            STL_FPERM_WREAD | STL_FPERM_WWRITE ;

    sFlag = STL_FOPEN_READ | STL_FOPEN_LARGEFILE | STL_FOPEN_BINARY;

    if( aInputArguments->mDirectIo == STL_TRUE )
    {
        sFlag |= STL_FOPEN_DIRECT;
    }

    /**
     * double buffer를 초기화한다.
     */
    STL_TRY( ztdInitializeReadDoubleBuffer( &aReadBufferedFile->mDoubleBuffer,
                                            ZTD_READ_BUFFER_SIZE,
                                            aAllocator,
                                            aErrorStack )
             == STL_SUCCESS );
    
    /**
     * 파일을 read mode로 연다.
     */
    STL_TRY( stlOpenFile( &aReadBufferedFile->mFileDesc,
                          aInputArguments->mData,
                          sFlag,
                          sPerm,
                          aErrorStack )
             == STL_SUCCESS );

    /**
     * aWriteBufferedFile의 각 멤버를 초기화한다.
     */
    aReadBufferedFile->mCurrentBufferIndex = 0;
    aReadBufferedFile->mCurrentDataSize    = 0;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief read buffered file을 종료한다.
 * @param [in]     aReadBufferedFile   read buffered file
 * @param [in/out] aErrorStack         error stack
 */
stlStatus ztdTerminateReadBufferedFile( ztdReadBufferedFile  * aReadBufferedFile,
                                        stlErrorStack        * aErrorStack )
{
    /**
     * double buffer를 닫는다.
     */
    STL_TRY( ztdTerminateReadDoubleBuffer( &aReadBufferedFile->mDoubleBuffer,
                                           aErrorStack )
             == STL_SUCCESS );
    
    /**
     * 파일을 닫는다.
     */
    STL_TRY( stlCloseFile( &aReadBufferedFile->mFileDesc,
                           aErrorStack )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief BufferedFile의 File을 교체한다.
 * @param [in/out]  aWriteBufferedFile   write buffered file
 * @param [in]      aInputArguments      input arguments
 * @param [in]      aHeader              header value
 * @param [in]      aHeaderSize          header size
 * @param [in/out]  aErrorStack          error stack
 */
stlStatus ztdChangeBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                 ztdInputArguments    * aInputArguments,
                                 stlChar              * aHeader,
                                 stlInt32               aHeaderSize,
                                 stlErrorStack        * aErrorStack )
{
    /**
     * 현재 Buffer의 내용을 File에 쓴다.
     */
    STL_TRY( stlWriteFile( &aWriteBufferedFile->mFileDesc,
                           (void *) aWriteBufferedFile->mBuffer,
                           ZTD_WRITE_BUFFER_SIZE,
                           NULL,
                           aErrorStack )
             == STL_SUCCESS );

    /**
     * 파일을 닫는다.
     */
    STL_TRY( stlCloseFile( &aWriteBufferedFile->mFileDesc,
                           aErrorStack )
             == STL_SUCCESS );

    /**
     * 새로운 파일을 연다.
     */
    STL_TRY( ztdCreateDataFile( &aWriteBufferedFile->mFileDesc,
                                aInputArguments,
                                aErrorStack )
             == STL_SUCCESS );

    /**
     * Header의 내용을 파일에 쓴다.
     */
    STL_TRY( stlWriteFile( &aWriteBufferedFile->mFileDesc,
                           aHeader,
                           aHeaderSize,
                           NULL,
                           aErrorStack )
             == STL_SUCCESS );

    /**
     * BufferedFile의 내용을 초기화한다.
     */
    stlMemset( aWriteBufferedFile->mBuffer,
               0x00,
               ZTD_WRITE_BUFFER_SIZE );

    aWriteBufferedFile->mCurrentBufferIndex = 0;
    aWriteBufferedFile->mCurrentFileSize    = aHeaderSize;
            
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief value block list의 실제 데이터를 파일에 쓴다.
 * @param [in/put]  aReadBufferedFile   write buffered file
 * @param [in]      aValueBlockList     valeu block list
 * @param [in]      aInputArguments     input arguments
 * @param [in]      aControlInfo        control information
 * @param [in]      aData               value memory
 * @param [in]      aHeader             header value
 * @param [in]      aHeaderSize         header size
 * @param [in/out]  aErrorStack         error stack
 */
stlStatus ztdWriteValueBlockListData( ztdWriteBufferedFile  * aWriteBufferedFile,
                                      dtlValueBlockList     * aValueBlockList,
                                      ztdInputArguments     * aInputArguments,
                                      ztdControlInfo        * aControlInfo,
                                      stlChar               * aHeader,
                                      stlInt32                aHeaderSize,
                                      stlErrorStack         * aErrorStack )
{
    stlInt32            sBlockIndex                                  = 0;
    
    dtlValueBlockList * sValueBlockList                              = aValueBlockList;
    dtlValueBlockList * sTempBlockList                               = NULL;
    dtlDataValue      * sDataValue                                   = NULL;
    
    stlInt64            sValueBlockSize                              = 0;
    stlInt32            sBlockCount                                  = 0;

    stlInt32            sSize                                        = 0;
    stlChar             sLengthBuffer[ZTD_VALUE_LEN_MAX_SIZE];

    stlInt64            sTotalFileSize                               = 0;
        
    sBlockCount = sValueBlockList->mValueBlock->mUsedBlockCnt;

    /**
     * ValueBlockList의 전체 Size를 확인해야 한다.
     * Max File Size이 설정된 경우, 한 개의 ValueBlockList가 두 개의 파일에 나누어지는 상황을 피하기 위해서 Check한다.
     */
    STL_DASSERT( aWriteBufferedFile->mMaxFileSize >= 0 );
    
    if( aWriteBufferedFile->mMaxFileSize != 0 )
    {
        sTempBlockList = sValueBlockList;
        while( STL_TRUE )
        {
            if( sTempBlockList != NULL )
            {
                sDataValue = sTempBlockList->mValueBlock->mDataValue;
                for( sBlockIndex = 0; sBlockIndex < sBlockCount; sBlockIndex++ )
                {
                    sValueBlockSize += sTempBlockList->mValueBlock->mDataValue[sBlockIndex].mLength;
                }
            }
            else
            {
                break;
            }
            
            sTempBlockList = sTempBlockList->mNext;
        }

        /**
         * ValueBlockCount의 Block Count값도 Binary File에 써진다.
         */
        sValueBlockSize += STL_SIZEOF( stlInt32 );
        /**
         * 각 Value의 Size값도 Binary File에 써진다. 총 Value의 Size값은 Column Count * Block Count이다.
         */
        sValueBlockSize += ( (ZTD_VALUE_LEN_MAX_SIZE - 1) * gZtdColumnCount * sBlockCount );
        
        sTotalFileSize = aWriteBufferedFile->mCurrentFileSize + sValueBlockSize + ( aWriteBufferedFile->mCurrentBufferIndex + 1 );
        if( (sTotalFileSize > aWriteBufferedFile->mMaxFileSize) &&
            (aWriteBufferedFile->mCurrentFileSize > aHeaderSize) )
        {
            STL_TRY( ztdChangeBufferedFile( aWriteBufferedFile,
                                            aInputArguments,
                                            aHeader,
                                            aHeaderSize,
                                            aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }
    }
    else
    {
        /* Do Nothing */
    }
    
    /**
     * ValueBlockList를 File Format에 맞게 써나간다.
     */
    STL_TRY( ztdWriteDataBufferedFile( aWriteBufferedFile,
                                       aInputArguments,
                                       aControlInfo,
                                       (stlChar *)&sBlockCount,
                                       STL_SIZEOF( stlInt32 ),
                                       aHeader,
                                       aHeaderSize,
                                       STL_FALSE,
                                       aErrorStack ) == STL_SUCCESS );

    sTempBlockList = sValueBlockList;
    while( STL_TRUE )
    {
        if( sTempBlockList != NULL )
        {
            /**
             * column의 value를 block read count만큼 기록해야 한다.
             */
            sDataValue = sTempBlockList->mValueBlock->mDataValue;
            for( sBlockIndex = 0; sBlockIndex < sBlockCount; sBlockIndex++ )
            {
                sSize = ztdGetValueWriteLength( sLengthBuffer,
                                                sDataValue[sBlockIndex].mLength );
                
                STL_TRY( ztdWriteDataBufferedFile( aWriteBufferedFile,
                                                   aInputArguments,
                                                   aControlInfo,
                                                   sLengthBuffer,
                                                   sSize,
                                                   aHeader,
                                                   aHeaderSize,
                                                   STL_FALSE,
                                                   aErrorStack ) == STL_SUCCESS );
                
                /**
                 * 파일에 기록될 데이터의 value 값을 구성한다.
                 */
                STL_TRY( ztdWriteDataBufferedFile( aWriteBufferedFile,
                                                   aInputArguments,
                                                   aControlInfo,
                                                   sDataValue[sBlockIndex].mValue,
                                                   sDataValue[sBlockIndex].mLength,
                                                   aHeader,
                                                   aHeaderSize,
                                                   STL_FALSE,
                                                   aErrorStack ) == STL_SUCCESS );
            }
        }
        else
        {
            break;
        }

        sTempBlockList = sTempBlockList->mNext;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 버퍼 단위로 데이터를 모아서 파일에 기록한다.
 * @param [in/out]  aWriteBufferedFile   write buffered file
 * @param [in]      aInputArguments      input arguments
 * @param [in]      aControlInfo         control information
 * @param [in]      aData                data value
 * @param [in]      aDataSize            data size
 * @param [in]      aHeader              header value
 * @param [in]      aHeaderSize          header size
 * @param [in]      aIsFlush             flush flag
 * @param [in/out]  aErrorStack          error stack
 */
stlStatus ztdWriteDataBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                    ztdInputArguments    * aInputArguments,
                                    ztdControlInfo       * aControlInfo,
                                    stlChar              * aData,
                                    stlInt32               aDataSize,
                                    stlChar              * aHeader,
                                    stlInt32               aHeaderSize,
                                    stlBool                aIsFlush,
                                    stlErrorStack        * aErrorStack )
{
    stlInt32 sBufferIndex        = 0;
    
    stlInt32 sCopySize           = 0;
    stlInt32 sFlushSize          = 0;
    
    /**
     * ztdWriteDataBufferedFile()이 호출되기 전에 File Size에 대한 검사가 수행되었어도 File Size가 Max File Size를 초과할 수 있다.
     */
    if( aWriteBufferedFile->mCurrentBufferIndex != 0 )
    {
        /**
         * buffer에 남아있는 데이터를 고려
         */
        sCopySize = ( (ZTD_WRITE_BUFFER_SIZE - 1) - aWriteBufferedFile->mCurrentBufferIndex ) + 1;

        if( sCopySize > aDataSize )
        {
            stlMemcpy( &aWriteBufferedFile->mBuffer[aWriteBufferedFile->mCurrentBufferIndex],
                       &aData[sBufferIndex],
                       aDataSize );

            sBufferIndex                            += aDataSize;
            aWriteBufferedFile->mCurrentBufferIndex += aDataSize;
            aDataSize                                = 0;
        }
        else
        {
            stlMemcpy( &aWriteBufferedFile->mBuffer[aWriteBufferedFile->mCurrentBufferIndex],
                       &aData[sBufferIndex],
                       sCopySize );
        
            STL_TRY( stlWriteFile( &aWriteBufferedFile->mFileDesc,
                                   aWriteBufferedFile->mBuffer,
                                   ZTD_WRITE_BUFFER_SIZE,
                                   NULL,
                                   aErrorStack )
                     == STL_SUCCESS );

            stlMemset( aWriteBufferedFile->mBuffer,
                       0x00,
                       ZTD_WRITE_BUFFER_SIZE );

            sBufferIndex                            += sCopySize;
            aDataSize                               -= sCopySize;
            aWriteBufferedFile->mCurrentFileSize    += ZTD_WRITE_BUFFER_SIZE;
            aWriteBufferedFile->mCurrentBufferIndex  = 0;
        }
    }
    else
    {
        /* Do Nothing */
    }

    /**
     * aData의 내용을 ZTD_WRITE_BUFFER_SIZE씩 데이터를 파일에 써나간다.
     * aData에 남은 내용이 ZTD_WRITE_BUFFER_SIZE가 안될 경우 버퍼에 복사 후, 종료
     */
    while( aDataSize > 0 )
    {
        if( aDataSize > ZTD_WRITE_BUFFER_SIZE )
        {
            STL_TRY( stlWriteFile( &aWriteBufferedFile->mFileDesc,
                                   &aData[sBufferIndex],
                                   ZTD_WRITE_BUFFER_SIZE,
                                   NULL,
                                   aErrorStack )
                     == STL_SUCCESS );

            aDataSize                            -= ZTD_WRITE_BUFFER_SIZE;
            sBufferIndex                         += ZTD_WRITE_BUFFER_SIZE;
            aWriteBufferedFile->mCurrentFileSize += ZTD_WRITE_BUFFER_SIZE;
        }
        else
        {
            stlMemcpy( &aWriteBufferedFile->mBuffer[aWriteBufferedFile->mCurrentBufferIndex],
                       &aData[sBufferIndex],
                       aDataSize );

            aWriteBufferedFile->mCurrentBufferIndex += aDataSize;
            sBufferIndex                            += aDataSize;
            aDataSize                               -= aDataSize;
        }
    }

    /**
     * check flush flag
     */
    if( aIsFlush == STL_TRUE )
    {
        ZTD_ALIGN( sFlushSize, aWriteBufferedFile->mCurrentBufferIndex, ZTD_ALIGN_512 );
        
        STL_TRY( stlWriteFile( &aWriteBufferedFile->mFileDesc,
                               aWriteBufferedFile->mBuffer,
                               sFlushSize,
                               NULL,
                               aErrorStack )
                 == STL_SUCCESS );

        aWriteBufferedFile->mCurrentFileSize    += ( aWriteBufferedFile->mCurrentBufferIndex + 1 );
        aWriteBufferedFile->mCurrentBufferIndex  = 0;
    }
    else
    {
        /* Do Nothing */
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 버퍼 단위로 데이터를 모아서 파일에 기록한다.
 * @param [in/out]  aWriteBufferedFile   write buffered file
 * @param [in]      aData                data value
 * @param [in]      aDataSize            data size
 * @param [in]      aIsFlush             flush flag
 * @param [in/out]  aErrorStack          error stack
 */
stlStatus ztdWriteBadBufferedFile( ztdWriteBufferedFile * aWriteBufferedFile,
                                   stlChar              * aData,
                                   stlInt32               aDataSize,
                                   stlBool                aIsFlush,
                                   stlErrorStack        * aErrorStack )
{
    stlFile     sBadFile;
    stlChar   * sWriteBuffer = NULL;
    
    stlInt32    sBufferIndex = 0;
    stlInt32    sDataIndex   = 0;

    sBadFile     = aWriteBufferedFile->mFileDesc;
    sWriteBuffer = aWriteBufferedFile->mBuffer;
    sBufferIndex = aWriteBufferedFile->mCurrentBufferIndex;

    /**
     * Data를 Write Buffer에 복사하며 Write Size 에 도달하면, Buffer의 내용을 Bad File에 쓰는 작업을 수행
     */
    for( sDataIndex = 0; sDataIndex < aDataSize; sDataIndex++ )
    {
        /**
         * Write Size에 도달했다면, File에 Write Buffer의 내용을 출력한다.
         */
        if( sBufferIndex >= ( ZTD_WRITE_BUFFER_SIZE - 1 ) )
        {
            STL_TRY( stlWriteFile( &sBadFile,
                                   (void *) sWriteBuffer,
                                   ZTD_WRITE_BUFFER_SIZE,
                                   NULL,
                                   aErrorStack )
                     == STL_SUCCESS );

            sBufferIndex = 0;

            stlMemset( sWriteBuffer,
                       0x00,
                       ZTD_WRITE_BUFFER_SIZE );
        }

        /**
         * Data의 내용을 Write Buffer에 복사한다.
         */
        sWriteBuffer[sBufferIndex] = aData[sDataIndex];
        
        sBufferIndex++;
    }

    /**
     * Check flush flag
     */
    if( aIsFlush == STL_TRUE )
    {
        STL_TRY( stlWriteFile( &sBadFile,
                               (void *) sWriteBuffer,
                               sBufferIndex,
                               NULL,
                               aErrorStack )
                 == STL_SUCCESS );

        sBufferIndex = 0;

        stlMemset( sWriteBuffer,
                   0x00,
                   ZTD_WRITE_BUFFER_SIZE );
    }

    aWriteBufferedFile->mCurrentBufferIndex = sBufferIndex;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief data file에 기록될 header 의 내용을 구성한다.
 * @param [in]      aWriteBufferedFile   write buffered file
 * @param [out]     aHeader              header value
 * @param [out]     aHeaderSize          header size
 * @param [in/out]  aErrorStack          error stack
 */
stlStatus ztdOrganizeHeader( dtlValueBlockList * aValueBlockList,
                             stlChar           * aHeader,
                             stlInt32            aHeaderSize,
                             stlErrorStack     * aErrorStack )
{
    stlInt32 sHeaderIndex       = 0;
    stlInt32 sCharacterSet      = 0;

    /**
     * Valid Check Value (Checking Start Header)
     */
    ZTD_SET_VALID_CHECK_VALUES( &aHeader[sHeaderIndex] );
    sHeaderIndex += ZTD_VALID_CHECK_VALUE_SIZE;
    
    /**
     * endian 정보를 구성
     */
    stlMemcpy( &aHeader[sHeaderIndex],
               &gZtdEndian,
               STL_SIZEOF( stlInt8 ) );
    sHeaderIndex += STL_SIZEOF( stlInt8 );

    /**
     * version 정보를 구성
     */
    stlMemcpy( &aHeader[sHeaderIndex],
               &gZtdBinaryFileFormatVersion,
               STL_SIZEOF( stlInt32 ) );
    sHeaderIndex += STL_SIZEOF( stlInt32 );

    /**
     * database character set 정보를 구성
     */
    sCharacterSet = (stlInt32)gZtdDatabaseCharacterSet;
    
    stlMemcpy( &aHeader[sHeaderIndex],
               &sCharacterSet,
               STL_SIZEOF( stlInt32 ) );
    sHeaderIndex += STL_SIZEOF( stlInt32 );

    /**
     * column count 정보를 구성
     */
    stlMemcpy( &aHeader[sHeaderIndex],
               &gZtdColumnCount,
               STL_SIZEOF( SQLSMALLINT ) );
    sHeaderIndex  += STL_SIZEOF( SQLSMALLINT );

    /**
     * Valid Check Value (Checking Column Information)
     */
    ZTD_SET_VALID_CHECK_VALUES( &aHeader[sHeaderIndex] );
    sHeaderIndex += ZTD_VALID_CHECK_VALUE_SIZE;
    
    /**
     * 각 column의 정보를 구성한다.
     */
    while( aValueBlockList != NULL )
    {
        stlMemcpy( &aHeader[sHeaderIndex],
                   &aValueBlockList->mValueBlock->mArgNum1,
                   STL_SIZEOF( stlInt64 ) );
        sHeaderIndex += STL_SIZEOF( stlInt64 );

        stlMemcpy( &aHeader[sHeaderIndex],
                   &aValueBlockList->mValueBlock->mArgNum2,
                   STL_SIZEOF( stlInt64 ) );
        sHeaderIndex += STL_SIZEOF( stlInt64 );

        stlMemcpy( &aHeader[sHeaderIndex],
                   &aValueBlockList->mValueBlock->mStringLengthUnit,
                   STL_SIZEOF( dtlStringLengthUnit ) );
        sHeaderIndex += STL_SIZEOF( dtlStringLengthUnit );
        
        stlMemcpy( &aHeader[sHeaderIndex],
                   &aValueBlockList->mValueBlock->mIntervalIndicator,
                   STL_SIZEOF( dtlIntervalIndicator ) );
        sHeaderIndex += STL_SIZEOF( dtlIntervalIndicator );

        stlMemcpy( &aHeader[sHeaderIndex],
                   &aValueBlockList->mValueBlock->mAllocBufferSize,
                   STL_SIZEOF( stlInt32 ) );
        sHeaderIndex += STL_SIZEOF( stlInt32 );

        stlMemcpy( &aHeader[sHeaderIndex],
                   &aValueBlockList->mValueBlock->mDataValue->mBufferSize,
                   STL_SIZEOF( stlInt32 ) );
        sHeaderIndex += STL_SIZEOF( stlInt32 );
        
        stlMemcpy( &aHeader[sHeaderIndex],
                   &aValueBlockList->mValueBlock->mDataValue->mType,
                   STL_SIZEOF( dtlDataType ) );
        sHeaderIndex += STL_SIZEOF( dtlDataType );

        aValueBlockList = aValueBlockList->mNext;
    }

    /**
     * Valid Check Value (Checking End Header)
     */
    ZTD_SET_VALID_CHECK_VALUES( &aHeader[sHeaderIndex] );
    sHeaderIndex += ZTD_VALID_CHECK_VALUE_SIZE;
    
    return STL_SUCCESS;
}

/**
 * @brief value block list가 저장된 data file에서 header부분을 읽어들인다.
 * @param [in]      aReadBufferedFile       read buffered file
 * @param [in]      aHeader                 header
 * @param [in]      aHeaderSize             header size
 * @param [in]      aColumnInfo             db에서 얻은 column 정보.
 * @param [out]     aEndian                 endian
 * @param [out]     aVersion                file format version
 * @param [out]     aCharacterSet           저장된 character set
 * @param [in]      aColumnCount            column count
 * @param [out]     aValueBlockColumnInfo   datafile에서 얻은 column의 정보
 * @param [in/out]  aErrorStack             error stack
 */
stlStatus ztdReadHeaderBufferedFile( ztdReadBufferedFile     * aReadBufferedFile,
                                     stlChar                 * aHeader,
                                     stlInt32                  aHeaderSize,
                                     ztdColumnInfo           * aColumnInfo,
                                     stlInt8                 * aEndian,
                                     stlInt32                * aVersion,
                                     dtlCharacterSet         * aCharacterSet,
                                     SQLSMALLINT             * aColumnCount,
                                     ztdValueBlockColumnInfo * aValueBlockInfo,
                                     stlErrorStack           * aErrorStack )
{
    stlInt32    sHdIdx  = 0;
    stlInt32    sColIdx = 0;
    stlInt32    sCharacterSet    = 0;
    stlChar     sValidCheckValue[ZTD_VALID_CHECK_VALUE_SIZE];
    SQLSMALLINT sColumnCount;
    
    
    STL_TRY( stlReadFile( &( aReadBufferedFile->mFileDesc ),
                          aHeader,
                          (stlSize) aHeaderSize,
                          NULL,
                          aErrorStack )
             == STL_SUCCESS );

    stlMemcpy( &sValidCheckValue,
               &aHeader[sHdIdx],
               ZTD_VALID_CHECK_VALUE_SIZE );
    sHdIdx += ZTD_VALID_CHECK_VALUE_SIZE;

    STL_TRY_THROW( ZTD_VALID_CHECK( sValidCheckValue ) == STL_TRUE,
                   RAMP_ERR_INVALID_HEADER );
           
    stlMemcpy( aEndian,
               &aHeader[sHdIdx],
               STL_SIZEOF( stlInt8 ) );
    sHdIdx += STL_SIZEOF( stlInt8 );

    stlMemcpy( aVersion,
               &aHeader[sHdIdx],
               STL_SIZEOF( stlInt32 ) );
    sHdIdx += STL_SIZEOF( stlInt32 );

    stlMemcpy( &sCharacterSet,
               &aHeader[sHdIdx],
               STL_SIZEOF( stlInt32 ) );
    sHdIdx += STL_SIZEOF( stlInt32 );

    *aCharacterSet = (dtlCharacterSet)sCharacterSet;

    stlMemcpy( &sColumnCount,
               &aHeader[sHdIdx],
               STL_SIZEOF( SQLSMALLINT ) );
    sHdIdx += STL_SIZEOF( SQLSMALLINT );

    /**
     * endian 변경
     */
    if( gZtdEndian != *aEndian )
    {
        STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                           aVersion,
                                           STL_FALSE,
                                           aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                           aCharacterSet,
                                           STL_FALSE,
                                           aErrorStack )
                 == STL_SUCCESS );

        STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_SMALLINT,
                                           &sColumnCount,
                                           STL_FALSE,
                                           aErrorStack )
                 == STL_SUCCESS );
    }

    STL_TRY_THROW( *aColumnCount == sColumnCount, RAMP_ERR_COLUMN_COUNT );

    stlMemcpy( &sValidCheckValue,
               &aHeader[sHdIdx],
               ZTD_VALID_CHECK_VALUE_SIZE );
    sHdIdx += ZTD_VALID_CHECK_VALUE_SIZE;

    STL_TRY_THROW( ZTD_VALID_CHECK( sValidCheckValue ) == STL_TRUE,
                   RAMP_ERR_INVALID_HEADER );
    
    for( sColIdx = 0; sColIdx < sColumnCount; sColIdx++ )
    {
        stlMemcpy( (void *) &aValueBlockInfo[sColIdx].mArgNum1,
                   &aHeader[sHdIdx],
                   STL_SIZEOF( stlInt64 ) );
        sHdIdx += STL_SIZEOF( stlInt64 );

        stlMemcpy( (void *) &aValueBlockInfo[sColIdx].mArgNum2,
                   &aHeader[sHdIdx],
                   STL_SIZEOF( stlInt64 ) );
        sHdIdx += STL_SIZEOF( stlInt64 );

        stlMemcpy( (void *) &aValueBlockInfo[sColIdx].mStringLengthUnit,
                   &aHeader[sHdIdx],
                   STL_SIZEOF( dtlStringLengthUnit ) );
        sHdIdx += STL_SIZEOF( dtlStringLengthUnit );

        stlMemcpy( (void *) &aValueBlockInfo[sColIdx].mIntervalIndicator,
                   &aHeader[sHdIdx],
                   STL_SIZEOF( dtlIntervalIndicator ) );
        sHdIdx += STL_SIZEOF( dtlIntervalIndicator );

        stlMemcpy( (void *) &aValueBlockInfo[sColIdx].mAllocBufferSize,
                   &aHeader[sHdIdx],
                   STL_SIZEOF( stlInt32 ) );
        sHdIdx += STL_SIZEOF( stlInt32 );

        stlMemcpy( (void *) &aValueBlockInfo[sColIdx].mBufferSize,
                   &aHeader[sHdIdx],
                   STL_SIZEOF( stlInt32 ) );
        sHdIdx += STL_SIZEOF( stlInt32 );
        
        stlMemcpy( (void *) &aValueBlockInfo[sColIdx].mType,
                   &aHeader[sHdIdx],
                   STL_SIZEOF( dtlDataType ) );
        sHdIdx += STL_SIZEOF( dtlDataType );

        /**
         * endian 변경
         */
        if( gZtdEndian != *aEndian )
        {
            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_BIGINT,
                                               &aValueBlockInfo[sColIdx].mArgNum1,
                                               STL_FALSE,
                                               aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_BIGINT,
                                               &aValueBlockInfo[sColIdx].mArgNum2,
                                               STL_FALSE,
                                               aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                               &aValueBlockInfo[sColIdx].mStringLengthUnit,
                                               STL_FALSE,
                                               aErrorStack )
                     == STL_SUCCESS );
            
            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                               &aValueBlockInfo[sColIdx].mIntervalIndicator,
                                               STL_FALSE,
                                               aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                               &aValueBlockInfo[sColIdx].mAllocBufferSize,
                                               STL_FALSE,
                                               aErrorStack )
                     == STL_SUCCESS );

            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                               &aValueBlockInfo[sColIdx].mBufferSize,
                                               STL_FALSE,
                                               aErrorStack )
                 == STL_SUCCESS );

            STL_TRY( dtlReverseByteOrderValue( DTL_TYPE_NATIVE_INTEGER,
                                               &aValueBlockInfo[sColIdx].mType,
                                               STL_FALSE,
                                               aErrorStack )
                     == STL_SUCCESS );
        }
        else
        {
            /* Do Nothing */
        }

        STL_TRY_THROW( aColumnInfo[sColIdx].mDbType == aValueBlockInfo[sColIdx].mType,
                       RAMP_ERR_COLUMN_TYPE );
    }

    stlMemcpy( &sValidCheckValue,
               &aHeader[sHdIdx],
               ZTD_VALID_CHECK_VALUE_SIZE );
    sHdIdx += ZTD_VALID_CHECK_VALUE_SIZE;

    STL_TRY_THROW( ZTD_VALID_CHECK( sValidCheckValue ) == STL_TRUE,
                   RAMP_ERR_INVALID_HEADER );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INVALID_HEADER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      ZTD_ERRCODE_INVALID_BINARY_FILE,
                      NULL,
                      aErrorStack );
    }

    STL_CATCH( RAMP_ERR_COLUMN_COUNT )
    {
        (void) stlPushError( STL_ERROR_LEVEL_ABORT,
                             ZTD_ERRCODE_INVALID_COLUMN_COUNT,
                             NULL,
                             aErrorStack );
    }

    STL_CATCH( RAMP_ERR_COLUMN_TYPE )
    {
        (void) stlPushError( STL_ERROR_LEVEL_ABORT,
                             ZTD_ERRCODE_INVALID_COLUMN_TYPE,
                             NULL,
                             aErrorStack,
                             sColIdx );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}

/**
 * @brief value block list가 저장된 data file에서 data 부분을 읽는다.
 * @param [in/out]  aReadBufferedFile    read buffered file
 * @param [out]     aReadData            read buffer
 * @param [in]      aReadSize            read size
 * @param [out]     aIsEndOfFile         end of file flag
 * @param [in/out]  aErrorStack          error stack
 */
stlStatus ztdReadDataBufferedFile( ztdReadBufferedFile * aReadBufferedFile,
                                   stlChar             * aReadData,
                                   stlInt32              aReadSize,
                                   stlBool             * aIsEndOfFile,
                                   stlErrorStack       * aErrorStack )
{
    register stlInt32       sDataSize       = 0;
    register stlInt32       sAccrueDataSize = 0;

    register stlInt32       sReadDataIndex  = 0;

    ztdBinaryDoubleBuffer * sDoubleBuffer   = NULL;
    
    stlChar               * sReadBuffer     = NULL;

    sDoubleBuffer = &aReadBufferedFile->mDoubleBuffer;
    
    if( sDoubleBuffer->mGetState == ZTD_DOUBLE_BUFFER_ONE )
    {
        sReadBuffer = sDoubleBuffer->mBuffer1;
    }
    else if( sDoubleBuffer->mGetState == ZTD_DOUBLE_BUFFER_TWO )
    {
        sReadBuffer = sDoubleBuffer->mBuffer2;
    }
    else if( sDoubleBuffer->mGetState == ZTD_DOUBLE_BUFFER_THREE )
    {
        sReadBuffer = sDoubleBuffer->mBuffer3;
    }
    else if( sDoubleBuffer->mGetState == ZTD_DOUBLE_BUFFER_NONE )
    {
        sDoubleBuffer->mGetState = ZTD_DOUBLE_BUFFER_ONE;
        sReadBuffer = sDoubleBuffer->mBuffer1;

        /**
         * 처음 시작하는 경우, 한 번의 read가 있을 때까지는 대기한다.
         */
        while( STL_TRUE )
        {
            STL_TRY_THROW( gZtdRunState == STL_TRUE, RAMP_FINISH );

            if( (gZtdErrorCount > gZtdMaxErrorCount) &&
                (gZtdMaxErrorCount > 0 ) )
            {
                STL_THROW( RAMP_FINISH );
            }
                
            if( sDoubleBuffer->mFilled > 0 )
            {
                aReadBufferedFile->mCurrentDataSize     = ZTD_READ_BUFFER_SIZE;
                aReadBufferedFile->mCurrentBufferIndex  = 0;
        
                break;
            }
            else
            {
                if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_END_OF_FILE )
                {
                    *aIsEndOfFile = STL_TRUE;
                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    stlYieldThread();
                }
            }
        }
    }
    else
    {
        STL_ASSERT( 0 );
    }

    /**
     * aReadSize 만큼 sReadData에 데이터를 읽을 때까지 반복
     */
    while( sAccrueDataSize < aReadSize )
    {
        if( aReadBufferedFile->mCurrentDataSize > 0 )
        {
            /**
             * aBufferedFile의 buffer에 남아 있는 데이터를 aReadBuffer로 복사한다.
             */
            sDataSize = aReadSize - sAccrueDataSize;
            
            if( sDataSize > aReadBufferedFile->mCurrentDataSize )
            {
                sDataSize = aReadBufferedFile->mCurrentDataSize;
            }
            else
            {
                /* Do Nothing */
            }

            stlMemcpy( &aReadData[sReadDataIndex],
                       &sReadBuffer[aReadBufferedFile->mCurrentBufferIndex],
                       sDataSize );
            
            sAccrueDataSize                        += sDataSize;
            aReadBufferedFile->mCurrentDataSize    -= sDataSize;
            aReadBufferedFile->mCurrentBufferIndex += sDataSize;
            sReadDataIndex                         += sDataSize;
        }
        else
        {
            (void)stlAtomicDec32( (volatile stlUInt32 *)&sDoubleBuffer->mFilled );
            
            while( STL_TRUE )
            {
                STL_TRY_THROW( gZtdRunState == STL_TRUE, RAMP_FINISH );

                if( (gZtdErrorCount > gZtdMaxErrorCount) &&
                    (gZtdMaxErrorCount > 0 ) )
                {
                    STL_THROW( RAMP_FINISH );
                }

                if( sDoubleBuffer->mFilled > 0 )
                {
                    if( sDoubleBuffer->mGetState == ZTD_DOUBLE_BUFFER_ONE )
                    {
                        sDoubleBuffer->mGetState = ZTD_DOUBLE_BUFFER_TWO;
                        sReadBuffer = sDoubleBuffer->mBuffer2;
                    }
                    else if( sDoubleBuffer->mGetState == ZTD_DOUBLE_BUFFER_TWO )
                    {
                        sDoubleBuffer->mGetState = ZTD_DOUBLE_BUFFER_THREE;
                        sReadBuffer = sDoubleBuffer->mBuffer3;
                    }
                    else if( sDoubleBuffer->mGetState == ZTD_DOUBLE_BUFFER_THREE )
                    {
                        sDoubleBuffer->mGetState = ZTD_DOUBLE_BUFFER_ONE;
                        sReadBuffer = sDoubleBuffer->mBuffer1;
                    }
                    else
                    {
                        STL_ASSERT( 0 );
                    }

                    break;
                }
                else
                {
                    if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_END_OF_FILE )
                    {
                        *aIsEndOfFile = STL_TRUE;
                        STL_THROW( RAMP_FINISH );
                    }
                    else
                    {
                        stlBusyWait();
                    }
                }
            }

            aReadBufferedFile->mCurrentDataSize    = ZTD_READ_BUFFER_SIZE;
            aReadBufferedFile->mCurrentBufferIndex = 0;
        }
    }

    STL_RAMP( RAMP_FINISH );
    
    return STL_SUCCESS;
}

/**
 * @brief read를 위한 double buffer를 초기화한다.
 * @param [out]     aBinaryDoubleBuffer  double buffer
 * @param [in]      aBufferSize          buffer size
 * @param [in]      aAllocator           memory allocator
 * @param [in/out]  aErrorStack          error stack
 */
stlStatus ztdInitializeReadDoubleBuffer( ztdBinaryDoubleBuffer * aDoubleBuffer,
                                         stlInt32                aBufferSize,
                                         stlAllocator          * aAllocator,
                                         stlErrorStack         * aErrorStack )
{
    stlInt32 sAllocBufferSize = 0;

    /**
     * buffer를 할당 받는다.
     */
    sAllocBufferSize  = aBufferSize + ZTD_ALIGN_512;
    
    STL_TRY( stlAllocRegionMem( aAllocator,
                                sAllocBufferSize,
                                (void **) &aDoubleBuffer->mBuffer1,
                                aErrorStack )
             == STL_SUCCESS );

    stlMemset( aDoubleBuffer->mBuffer1,
               0x00,
               sAllocBufferSize );

    ZTD_ALIGN_BUFFER( aDoubleBuffer->mBuffer1,
                      aDoubleBuffer->mBuffer1,
                      ZTD_ALIGN_512 );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                sAllocBufferSize,
                                (void **) &aDoubleBuffer->mBuffer2,
                                aErrorStack )
             == STL_SUCCESS );

    stlMemset( aDoubleBuffer->mBuffer2,
               0x00,
               sAllocBufferSize );

    ZTD_ALIGN_BUFFER( aDoubleBuffer->mBuffer2,
                      aDoubleBuffer->mBuffer2,
                      ZTD_ALIGN_512 );

    STL_TRY( stlAllocRegionMem( aAllocator,
                                sAllocBufferSize,
                                (void **) &aDoubleBuffer->mBuffer3,
                                aErrorStack )
             == STL_SUCCESS );

    stlMemset( aDoubleBuffer->mBuffer3,
               0x00,
               sAllocBufferSize );

    ZTD_ALIGN_BUFFER( aDoubleBuffer->mBuffer3,
                      aDoubleBuffer->mBuffer3,
                      ZTD_ALIGN_512 );
    
    /**
     * buffer의 상태를 지정한다.
     */
    aDoubleBuffer->mPutState = ZTD_DOUBLE_BUFFER_NONE;
    aDoubleBuffer->mGetState = ZTD_DOUBLE_BUFFER_NONE;

    aDoubleBuffer->mFilled   = 0;
    
    return STL_SUCCESS;

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief read를 위한 double buffer를 종료한다.
 * @param [in]      aBinaryDoubleBuffer  double buffer
 * @param [in/out]  aErrorStack          error stack
 */
stlStatus ztdTerminateReadDoubleBuffer( ztdBinaryDoubleBuffer * aDoubleBuffer,
                                        stlErrorStack         * aErrorStack )
{
    return STL_SUCCESS;
}

/**
 * @brief read를 위한 double buffer를 종료한다.
 * @param [in]      aThread      stlthread.
 * @param [in/out]  aArg         thread arguments.
 */
void * STL_THREAD_FUNC ztdReadBinaryFile( stlThread * aThread,
                                          void      * aArg )
{
    ztdReadBufferedFile   * sReadBufferedFile                    = NULL;
    ztdBinaryDoubleBuffer * sDoubleBuffer                        = NULL;
    stlErrorStack         * sErrorStack                          = NULL;

    stlChar               * sReadBuffer                          = NULL;

    sReadBufferedFile = ( ( ztdDoubleBufferThreadArg * ) aArg )->mReadBufferedFile;
    sDoubleBuffer     = ( ( ztdDoubleBufferThreadArg * ) aArg )->mDoubleBuffer;
    sErrorStack       = &( ( ztdDoubleBufferThreadArg * ) aArg )->mErrorStack;

    sDoubleBuffer->mPutState = ZTD_DOUBLE_BUFFER_ONE;
    
    while( STL_TRUE )
    {
        if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_ONE )
        {
            sReadBuffer = sDoubleBuffer->mBuffer1;
        }
        else if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_TWO )
        {
            sReadBuffer = sDoubleBuffer->mBuffer2;
        }
        else if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_THREE )
        {
            sReadBuffer = sDoubleBuffer->mBuffer3;
        }
        else
        {
            STL_ASSERT( 0 );
        }

        /**
         * 빈 버퍼가 있을 경우에만 read 한다.
         */
        while( STL_TRUE )
        {
            /**
             * gloader의 비정상 종료를 확인한다.
             */
            STL_TRY( gZtdRunState == STL_TRUE );

            if( (gZtdErrorCount > gZtdMaxErrorCount) &&
                (gZtdMaxErrorCount > 0 ) )
            {
                STL_THROW( RAMP_FINISH );
            }

            if( sDoubleBuffer->mFilled < ZTD_DOUBLE_BUFFER_COUNT )
            {
                if( stlReadFile( &( sReadBufferedFile->mFileDesc ),
                                 sReadBuffer,
                                 ZTD_READ_BUFFER_SIZE,
                                 NULL,
                                 sErrorStack ) == STL_FAILURE )
                {
                    STL_TRY( stlGetLastErrorCode( sErrorStack ) == STL_ERRCODE_EOF );
                    
                    /**
                     * 파일의 끝까지 읽었으므로 해당 에러는 pop한다.
                     */
                    stlPopError( sErrorStack );
                    sDoubleBuffer->mPutState = ZTD_DOUBLE_BUFFER_END_OF_FILE;

                    STL_THROW( RAMP_FINISH );
                }
                else
                {
                    (void)stlAtomicInc32( (volatile stlUInt32 *)&sDoubleBuffer->mFilled );
                }

                if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_ONE )
                {
                    sDoubleBuffer->mPutState = ZTD_DOUBLE_BUFFER_TWO;
                }
                else if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_TWO )
                {
                    sDoubleBuffer->mPutState = ZTD_DOUBLE_BUFFER_THREE;
                }
                else if( sDoubleBuffer->mPutState == ZTD_DOUBLE_BUFFER_THREE )
                {
                    sDoubleBuffer->mPutState = ZTD_DOUBLE_BUFFER_ONE;
                }
                else
                {
                    STL_ASSERT( 0 );
                }
        
                break;
            }
            else
            {
                stlBusyWait();
            }
        }
    }

    STL_RAMP( RAMP_FINISH );

    STL_TRY( stlExitThread( aThread,
                            STL_SUCCESS,
                            sErrorStack )
             == STL_SUCCESS );

    return NULL;

    STL_FINISH;

    gZtdRunState = STL_FALSE;

    STL_TRY( stlExitThread( aThread,
                            STL_FAILURE,
                            sErrorStack )
             == STL_SUCCESS );
    
    return NULL;
}
