/*******************************************************************************
 * stlFile.c
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

#include <stlDef.h>
#include <stf.h>
#include <stlStrings.h>
#include <stlError.h>

/**
 * @file stlFile.c
 * @brief Standard Layer File Routines
 */


/**
 * @addtogroup stlFile
 * @{
 */

/**
 * @brief 주어진 파일 핸들과 관련된 정보를 얻는다.
 * @param[out] aFileInfo 
 * @param[in] aWanted 얻을 정보의 종류
 * @param[in] aFile 정보를 얻을 대상 파일
 * @param[out] aErrorStack 에러 스택
 * @see aWanted : stlFileInfoFlags
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INCOMPLETE]
 *     aWanted에 요구한 정보를 aFileInfo에 채울수 없는 경우에 발생함.
 * [STL_ERRCODE_FILE_STAT]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlGetFileStatByHandle( stlFileInfo   * aFileInfo,
                                         stlInt32        aWanted,
                                         stlFile       * aFile,
                                         stlErrorStack * aErrorStack )
{
    return stfGetFileInfo( aFileInfo, aWanted, aFile, aErrorStack );
}

/**
 * @brief 주어진 파일 이름으로 부터 파일 정보를 얻는다.
 * @param[out] aFileInfo 
 * @param[in] aFileName 정보를 얻을 대상 파일이름
 * @param[in] aWanted 얻을 정보의 종류
 * @param[out] aErrorStack 에러 스택
 * @see aWanted : stlFileInfoFlags
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INCOMPLETE]
 *     aWanted에 요구한 정보를 aFileInfo에 채울수 없는 경우에 발생함.
 * [STL_ERRCODE_FILE_STAT]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlGetFileStatByName( stlFileInfo    * aFileInfo,
                                       const stlChar  * aFileName,
                                       stlInt32         aWanted,
                                       stlErrorStack  * aErrorStack )
{
    return stfGetStat( aFileInfo, aFileName, aWanted, aErrorStack );
}
    
/**
 * @brief 주어진 파일이름에 해당하는 파일포인터를 얻는다.
 * @param[out] aNewFile 얻은 파일포인터
 * @param[in] aFileName 대상 파일이름
 * @param[in] aFlag 파일 Flag 정보
 * @param[in] aPerm 파일의 퍼미션 정보
 * @param[out] aErrorStack 에러 스택
 * @see aFlag : stlFileOpenFlags
 * @see aPerm : stlFilePermFlags
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_ACCESS]
 *     (1) aFlag에 STL_FOPEN_READ나 STL_FOPEN_WRITE가 설정되지 않은 경우.
 *     (2) 주어진 파일에 접근 권한이 없는 경우.
 *     (3) 같은 이름의 파일이 존재하고, aFlag에 STL_FOPEN_EXCL이 설정된 경우
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     OS 커널 메모리가 부족한 경우에 발생함.
 * [STL_ERRCODE_DISK_FULL]
 *     디스크 공간이 부족한 경우에 발생함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 파일이 존재하지 않는 경우에 발생함.
 * [STL_ERRCODE_FILE_OPEN]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlOpenFile( stlFile        * aNewFile, 
                              const stlChar  * aFileName, 
                              stlInt32         aFlag, 
                              stlInt32         aPerm,
                              stlErrorStack  * aErrorStack )
{
    return stfOpen( aNewFile, aFileName, aFlag, aPerm, aErrorStack );
}

/**
 * @brief 주어진 파일이 존재하는지 검사한다.
 * @param[in] aFileName 대상 파일이름
 * @param[out] aExist   대상 파일의 존재 여부
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_ACCESS]
 *     주어진 파일에 접근 권한이 없는 경우.
 * [STL_ERRCODE_FILE_EXIST]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlExistFile( const stlChar  * aFileName,
                               stlBool        * aExist,
                               stlErrorStack  * aErrorStack )
{
    return stfExist( aFileName, aExist, aErrorStack );
}

/**
 * @brief 열린 파일포인터를 닫는다.
 * @param[in] aFile 닫을 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_CLOSE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlCloseFile( stlFile       * aFile,
                               stlErrorStack * aErrorStack )
{
    return stfClose( aFile, aErrorStack );
}

/**
 * @brief 파일을 삭제한다.
 * @param[in] aPath 삭제할 파일 경로
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_ACCESS]
 *     주어진 파일이나 디렉토리에 접근 권한이 없는 경우.
 * [STL_ERRCODE_NAMETOOLONG]
 *     aPath가 너무 긴 경우에 발생함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 파일이 존재하지 않는 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     OS 커널 메모리가 부족한 경우에 발생함.
 * [STL_ERRCODE_FILE_REMOVE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlRemoveFile( const stlChar * aPath,
                                stlErrorStack * aErrorStack )
{
    return stfRemove( aPath, aErrorStack );
}

/**
 * @brief 파일의 심볼릭 링크를 만든다.
 * @param[in] aFromPath 생성될 심볼릭 링크 경로
 * @param[in] aToPath 링크가 걸릴 파일 경로
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_ACCESS]
 *     주어진 파일이나 디렉토리에 접근 권한이 없는 경우.
 * [STL_ERRCODE_NAMETOOLONG]
 *     aPath가 너무 긴 경우에 발생함.
 * [STL_ERRCODE_NO_ENTRY]
 *     주어진 디렉토리가 존재하지 않는 경우에 발생함.
 * [STL_ERRCODE_DISK_FULL]
 *     디스크 공간이 부족한 경우에 발생함.
 * [STL_ERRCODE_INSUFFICIENT_RESOURCE]
 *     프로세스나 시스템에서 열수 있는 파일 개수의 제한을 넘어선 경우에 발생함.
 * [STL_ERRCODE_OUT_OF_MEMORY]
 *     OS 커널 메모리가 부족한 경우에 발생함.
 * [STL_ERRCODE_FILE_LINK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlLinkFile( const stlChar * aFromPath,
                              const stlChar * aToPath,
                              stlErrorStack * aErrorStack )
{
    return stfLink( aFromPath, aToPath, aErrorStack );
}

/**
 * @brief 파일 포지션을 이동시킨다.
 * @param[in] aFile 대상 파일 포인터
 * @param[in] aWhere 이동될 시작 위치 (File Seek Flags)
 * @param[in,out] aOffset in: aWhere로부터 이동할 상대 위치
 *                   <BR> out: 이동한 곳의 절대 위치 
 * @param[out] aErrorStack 에러 스택
 * @see aWhere : stlFileSeekFlags
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_SEEK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlSeekFile( stlFile        * aFile,
                              stlInt32         aWhere,
                              stlOffset      * aOffset,
                              stlErrorStack  * aErrorStack )
{
    return stfSeek( aFile, aWhere, aOffset, aErrorStack );
}

/**
 * @brief aOffset 이후의 파일 내용을 삭제한다.
 * @param[in] aFile 대상 파일 포인터
 * @param[in] aOffset 삭제할 파일 옵셋
 * @param[out] aErrorStack 에러 스택
 * @remark Truncate 이후 파일 포인터는 맨끝을 가리킨다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_TRUNCATE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlTruncateFile( stlFile        * aFile,
                                  stlOffset        aOffset,
                                  stlErrorStack  * aErrorStack )
{
    return stfTruncate( aFile, aOffset, aErrorStack );
}

/**
 * @brief aFromPath 파일을 aToPath 파일로 복사한다.
 * @param[in] aFromPath 소스 파일의 경로
 * @param[in] aToPath 목적 파일의 경로
 * @param[in] aFlags 생성될 목적 파일의 Open Flag 정보
 * @param[in] aPerms 생성될 목적 파일의 퍼미션 정보
 * @param[in] aBuffer 파일 복사 시 사용할 Buffer
 * @param[in] aBufferSize 파일 복사 시 사용할 Buffer 크기
 * @param[out] aErrorStack 에러 스택
 * @see aPerms : stlFilePermFlags
 * @par Error Codes:
 * <PRE>
 * stlCopyFile은 유틸리티 형태의 함수로서 내부적으로 아래와 같은 함수들을 사용하기 때문에,
 * 아래 함수들에서 발생할수 있는 에러들이 리턴된다.<BR>
 * stlOpenFile
 * stlGetFileInfo
 * stlReadFile
 * stlWriteFile
 * stlCloseFile
 * </PRE>
 */
inline stlStatus stlCopyFile( const stlChar * aFromPath,
                              const stlChar * aToPath,
                              stlInt32        aFlags,
                              stlInt32        aPerms,
                              stlChar       * aBuffer,
                              stlInt32        aBufferSize,
                              stlErrorStack * aErrorStack )
{
    return stfTransferContents( aFromPath,
                                aToPath,
                                aFlags,
                                aPerms,
                                aBuffer,
                                aBufferSize,
                                aErrorStack );
}

/**
 * @brief aToPath 파일에 aFromPath 파일을 덧붙인다.
 * @param[in] aFromPath 소스 파일의 경로
 * @param[in] aToPath 목적 파일의 경로
 * @param[in] aPerms 생성될 목적 파일의 퍼미션 정보
 * @param[out] aErrorStack 에러 스택
 * @see aPerms : stlFilePermFlags
 * @par Error Codes:
 * <PRE>
 * stlAppendFile은 유틸리티 형태의 함수로서 내부적으로 아래와 같은 함수들을 사용하기 때문에,
 * 아래 함수들에서 발생할수 있는 에러들이 리턴된다.<BR>
 * stlOpenFile
 * stlGetFileInfo
 * stlReadFile
 * stlWriteFile
 * stlCloseFile
 * </PRE>
 */
inline stlStatus stlAppendFile( const stlChar * aFromPath,
                                const stlChar * aToPath,
                                stlInt32        aPerms,
                                stlErrorStack * aErrorStack )
{
    return stfTransferContents( aFromPath,
                                aToPath,
                                STL_FOPEN_WRITE |
                                STL_FOPEN_CREATE |
                                STL_FOPEN_APPEND,
                                aPerms,
                                NULL,
                                0,
                                aErrorStack );
}

/**
 * @brief 주어진 파일에서 aBytes 만큼 aBuffer로 읽는다.
 * @param[in] aFile 소스 파일포인터
 * @param[out] aBuffer 읽은 데이터가 저장될 버퍼 공간
 * @param[in] aBytes 읽을 데이터의 바이트 단위 크기 
 * @param[out] aReadBytes 읽은 데이터의 바이트 단위 크기 
 * @param[out] aErrorStack 에러 스택
 * @remark
 *        <BR> 실제로 파일에서 읽은 데이터 크기(aReadBytes)가 aBytes보다 작을수 있다.
 *        <BR> 이러한 경우는 파일의 끝부분에서 발생할수 있으며,
 *        <BR> 이후 다시 stlReadFile이 호출되면 aReadBytes는 0으로 설정되고, 
 *        <BR> 에러 코드는 STL_ERRCODE_EOF가 설정된다.
 *        <BR> aBytes가 1보다 작은 경우, aReadBytes는 0으로 설정되며 에러없이 리턴된다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_EOF]
 *     파일의 끝을 만났을 경우에 발생함.
 * [STL_ERRCODE_FILE_BAD]
 *     파일이 유효하지 않거나, 파일이 읽기 모드로 열리지 않은 경우에 발생함.
 * [STL_ERRCODE_FILE_READ]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlReadFile( stlFile       * aFile,
                              void          * aBuffer,
                              stlSize         aBytes,
                              stlSize       * aReadBytes,
                              stlErrorStack * aErrorStack )
{
    stlSize  sReadBytes = 0;
    stlSize  sBytes = 0;
    stlSize  sTotalReadBytes = 0;

    while( sTotalReadBytes < aBytes )
    {
        sBytes = (aBytes - sTotalReadBytes);

        STL_TRY( stfRead( aFile,
                          (stlChar*)aBuffer + sTotalReadBytes,
                          sBytes,
                          &sReadBytes,
                          aErrorStack )
                 == STL_SUCCESS );

        sTotalReadBytes += sReadBytes;
        
        if( sBytes != sReadBytes )
        {
            break;   
        }
    }

    if( aReadBytes != NULL )
    {
        *aReadBytes = sTotalReadBytes;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 파일에 aBuffer의 내용을 aBytes 만큼 기록한다.
 * @param[in] aFile 소스 파일포인터
 * @param[in] aBuffer 기록할 데이터가 저장된 버퍼 공간
 * @param[in] aBytes 기록할 데이터의 바이트 단위 크기 
 * @param[out] aWrittenBytes 기록한 데이터의 바이트 단위 크기 
 * @param[out] aErrorStack 에러 스택
 * @remark
 *        <BR> aBytes가 1보다 작은 경우, aWrittenBytes는 0으로 설정되며 에러없이 리턴된다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_DISK_FULL]
 *     디스크 공간이 부족한 경우에 발생함.
 * [STL_ERRCODE_FILE_WRITE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlWriteFile( stlFile       * aFile,
                               void          * aBuffer,
                               stlSize         aBytes,
                               stlSize       * aWrittenBytes,
                               stlErrorStack * aErrorStack )
{
    stlSize  sWrittenBytes = 0;
    stlSize  sBytes = 0;
    stlSize  sTotalWrittenBytes = 0;

    while( sTotalWrittenBytes < aBytes )
    {
        sBytes = (aBytes - sTotalWrittenBytes);

        STL_TRY( stfWrite( aFile,
                           (stlChar*)aBuffer + sTotalWrittenBytes,
                           sBytes,
                           &sWrittenBytes,
                           aErrorStack )
                 == STL_SUCCESS );

        STL_ASSERT( sBytes == sWrittenBytes );
        
        sTotalWrittenBytes += sBytes;
    }

    if( aWrittenBytes != NULL )
    {
        *aWrittenBytes = sTotalWrittenBytes;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 주어진 파일에 한개의 문자를 기록한다.
 * @param[in] aCharacter 기록할 문자
 * @param[in] aFile 소스 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_DISK_FULL]
 *     디스크 공간이 부족한 경우에 발생함.
 * [STL_ERRCODE_FILE_WRITE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlPutCharacterFile( stlChar         aCharacter,
                                      stlFile       * aFile,
                                      stlErrorStack * aErrorStack )
{
    return stfPutCharacter( aCharacter, aFile, aErrorStack );
}

/**
 * @brief 주어진 파일에 한개의 문자를 읽는다.
 * @param[in] aCharacter 읽은 문자가 저장될 버퍼 공간
 * @param[in] aFile 소스 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_EOF]
 *     파일의 끝을 만났을 경우에 발생함.
 * [STL_ERRCODE_FILE_READ]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlGetCharacterFile( stlChar       * aCharacter,
                                      stlFile       * aFile,
                                      stlErrorStack * aErrorStack )
{
    return stfGetCharacter( aCharacter, aFile, aErrorStack );
}

/**
 * @brief 주어진 파일에 한라인의 문자열을 기록한다.
 * @param[in] aString 기록할 문자열
 * @param[in] aFile 소스 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_DISK_FULL]
 *     디스크 공간이 부족한 경우에 발생함.
 * [STL_ERRCODE_FILE_WRITE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlPutStringFile( const stlChar * aString,
                                   stlFile       * aFile,
                                   stlErrorStack * aErrorStack )
{
    return stfPutString( aString, aFile, aErrorStack );
}

/**
 * @brief 주어진 파일에 한라인의 문자열을 읽는다.
 * @param[in] aString 읽은 문자열이 저장될 버퍼 공간
 * @param[in] aLength aString 버퍼 공간의 바이트 단위 크기
 * @param[in] aFile 소스 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @remark
 *        <BR> 성공하는 경우에 항상 문자열의 끝은 널문자로 채워짐.
 *        <BR>  - aLength가 1보다 작은 경우는 aString에 변화가 없고 에러없이 리턴됨.
 *        <BR>  - aLength가 1인 경우는 aString[0]에 널문자가 채워짐.
 *        <BR> STL_ERRCODE_EOF가 발생하면 aString에 변화는 없다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_EOF]
 *     파일의 끝을 만났을 경우에 발생함.
 * [STL_ERRCODE_FILE_READ]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlGetStringFile( stlChar       * aString,
                                   stlInt32        aLength,
                                   stlFile       * aFile,
                                   stlErrorStack * aErrorStack )
{
    return stfGetString( aString, aLength, aFile, aErrorStack );
}

/**
 * @brief 커널 버퍼의 내용을 디스크에 기록한다.
 * @param[in] aFile 소스 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_SYNC]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlSyncFile( stlFile       * aFile,
                              stlErrorStack * aErrorStack )
{
    return stfSync( aFile, aErrorStack );
}

/**
 * @brief 파일 포인터를 복제한다.
 * @param[out] aNewFile 복제된 파일포인터
 * @param[in] aOldFile 복제할 파일포인터
 * @param[in] aOption  복제 방법( 1 or 2 )
 * @param[out] aErrorStack 에러 스택
 * @remark aOption이 1인 경우는 복제된 파일 포인터가 aNewFile에 기록한다.<BR>
 * aOption이 2인 경우는 aNewFile이 닫히고, aNewFile의 파일 포인터를 재사용해서 복제한다.
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_INVALID_ARGUMENT]
 *     case 1 : aNewFile이나 aOldFile이 NULL인 경우.
 *     case 2 : aWhichDup이 3이상이거나 0이하인경우.
 * [STL_ERRCODE_FILE_DUP]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlDuplicateFile( stlFile        * aNewFile,
                                   stlFile        * aOldFile,
                                   stlInt32         aOption,
                                   stlErrorStack  * aErrorStack )
{
    return stfDuplicate( aNewFile, aOldFile, aOption, aErrorStack );
}

/**
 * @brief 파일에 잠금을 설정한다.
 * @param[in] aFile 대상 파일포인터
 * @param[in] aType 잠금 형태
 * @param[out] aErrorStack 에러 스택
 * @see aType : stlFileLockFlags
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_AGAIN]
 *     이미 파일에 잠금이 설정되어 있는 경우에 발생함.
 * [STL_ERRCODE_FILE_LOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlLockFile( stlFile        * aFile,
                              stlInt32         aType,
                              stlErrorStack  * aErrorStack )
{
    return stfLock( aFile, aType, aErrorStack );
}

/**
 * @brief 파일에 잠금을 해제한다.
 * @param[in] aFile 대상 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_UNLOCK]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlUnlockFile( stlFile        * aFile,
                                stlErrorStack  * aErrorStack )
{
    return stfUnlock( aFile, aErrorStack );
}

/**
 * @brief 주어진 2개의 파일에 파이프를 설정한다.
 * @param[out] aInFile 입력 받을 파일포인터
 * @param[out] aOutFile 출력할 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_FILE_PIPE]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlCreatePipe( stlFile        * aInFile,
                                stlFile        * aOutFile,
                                stlErrorStack  * aErrorStack )
{
    return stfCreatePipe( aInFile, aOutFile, aErrorStack );
}

/**
 * @brief 표준 입력 파일을 연다.
 * @param[in] aFile 입력 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * @endcode
 */
inline stlStatus stlOpenStdinFile( stlFile        * aFile,
                                   stlErrorStack  * aErrorStack )
{
    return stfOpenStdin( aFile, aErrorStack );
}

/**
 * @brief 표준 출력 파일을 연다.
 * @param[in] aFile 출력 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * @endcode
 */
inline stlStatus stlOpenStdoutFile( stlFile        * aFile,
                                    stlErrorStack  * aErrorStack )
{
    return stfOpenStdout( aFile, aErrorStack );
}

/**
 * @brief 표준 에러 파일을 연다.
 * @param[in] aFile 에러 파일포인터
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * @endcode
 */
inline stlStatus stlOpenStderrFile( stlFile        * aFile,
                                    stlErrorStack  * aErrorStack )
{
    return stfOpenStderr( aFile, aErrorStack );
}

/**
 * @brief 주어진 aRootPath와 aAddPath를 aNewPath로 병합한다.
 * @param[in] aRootPath 
 * @param[in] aAddPath 
 * @param[in] aNewPath 
 * @param[in] aNewPathBufferLen 
 * @param[out] aNewPathLen 병합된 aNewPath의 스트링 길이
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NAMETOOLONG]
 *     병합한 aNewPath의 길이가 STL_MAX_FILE_PATH_LENGTH를 넘어설때 발생한다.
 * @endcode
 * @remark aAddPath가 root로부터 시작된다면 aRootPath는 무시한다.
 * @note aAddPath가 root로부터 시작되는 경우 aRootPath는 반드시 root로부터 시작되어야 한다.
 */
inline stlStatus stlMergeAbsFilePath( stlChar       * aRootPath,
                                      stlChar       * aAddPath,
                                      stlChar       * aNewPath,
                                      stlInt32        aNewPathBufferLen,
                                      stlInt32      * aNewPathLen,
                                      stlErrorStack * aErrorStack )
{
    stlChar sNewPath[STL_MAX_FILE_PATH_LENGTH * 2];
    stlChar sOrgPath[STL_MAX_FILE_PATH_LENGTH];
    
    STL_PARAM_VALIDATE( aRootPath != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aAddPath != NULL, aErrorStack );
    STL_PARAM_VALIDATE( aNewPath != NULL, aErrorStack );

    if( aAddPath[0] != STL_PATH_SEPARATOR[0] )
    {
        STL_PARAM_VALIDATE( aRootPath[0] == STL_PATH_SEPARATOR[0], aErrorStack );

        /**
         * Filename의 앞뒤 공백을 제거한다.
         */

        STL_TRY_THROW( stlStrlen(aAddPath) < STL_MAX_FILE_PATH_LENGTH,
                       RAMP_ERR_NAMETOOLONG );

        stlStrcpy( sOrgPath, aAddPath );
        stlTrimString( sOrgPath );

        *aNewPathLen = stlSnprintf( sNewPath,
                                    STL_MAX_FILE_PATH_LENGTH * 2,
                                    "%s" STL_PATH_SEPARATOR "%s",
                                    aRootPath,
                                    sOrgPath );
    }
    else
    {
        /*
         * AddPath[0]가 Root Path를 갖는다면 aRootPath는 무시한다.
         */
        *aNewPathLen = stlSnprintf( sNewPath,
                                    STL_MAX_FILE_PATH_LENGTH * 2,
                                    "%s",
                                    aAddPath );
    }

    STL_TRY_THROW( *aNewPathLen <= STL_MAX_FILE_PATH_LENGTH, RAMP_ERR_NAMETOOLONG );

    stlStrcpy( aNewPath, sNewPath );
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NAMETOOLONG )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_NAMETOOLONG,
                      NULL,
                      aErrorStack,
                      sNewPath );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 현재 작업 디렉토리 경로를 얻는다.
 * @param[out] aPath 작업 경로
 * @param[in] aPathBufLen aPath의 버퍼 크기 
 * @param[out] aErrorStack 에러 스택
 * @par Error Codes:
 * @code
 * [STL_ERRCODE_NAMETOOLONG]
 *     현재 작업 디렉토리 경로의 길이가 aPathBufLen보다 클때 발생한다.
 * [STL_ERRCODE_FILE_PATH]
 *     대표 에러 (자세한 에러는 System Error Code를 확인할것)
 * @endcode
 */
inline stlStatus stlGetCurFilePath( stlChar       * aPath,
                                    stlInt32        aPathBufLen,
                                    stlErrorStack * aErrorStack )
{
    return stfGetCurFilePath( aPath, aPathBufLen, aErrorStack );
}

/** @} */
