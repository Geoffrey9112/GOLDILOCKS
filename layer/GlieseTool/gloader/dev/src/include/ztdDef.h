/*******************************************************************************
 * ztdDef.h
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

#ifndef _ZTDDEF_H_
#define _ZTDDEF_H_ 1
#include <goldilocks.h>
#include <dtl.h>

#define ZTD_MAX_FILE_NAME_LENGTH            STL_MAX_FILE_NAME_LENGTH
#define ZTD_MAX_FILE_PATH_LENGTH            STL_MAX_FILE_PATH_LENGTH
#define ZTD_MAX_IDENTIFIER_LENGTH           STL_MAX_SQL_IDENTIFIER_LENGTH
#define ZTD_MAX_SCHEMA_NAME                 STL_MAX_SQL_IDENTIFIER_LENGTH
#define ZTD_MAX_TABLE_NAME                  STL_MAX_SQL_IDENTIFIER_LENGTH
#define ZTD_MAX_COLUMN_NAME                 STL_MAX_SQL_IDENTIFIER_LENGTH
#define ZTD_MAX_CHARACTERSET_NAME           STL_MAX_SQL_IDENTIFIER_LENGTH
#define ZTD_MAX_MODE_TYPE_LENGTH            (7)
#define ZTD_QUALIFIER_LEN                   (1)


/**
 * @brief: Size Definition.
 */
#define ZTD_DEFAULT_COMMAND_SIZE            (1024)
#define ZTD_MAX_COMMAND_BUFFER_SIZE         (1024 * 2)
#define ZTD_MAX_LOG_STRING_SIZE             (1024)
#define ZTD_MAX_READ_RECORD_BUFFER_SIZE     (1024 * 1024 * 30)
#define ZTD_MAX_WRITE_RECORD_BUFFER_SIZE    (1024 * 1024 * 30)
#define ZTD_MAX_READ_CONTROL_BUFFER_SIZE    (1024)
#define ZTD_MAX_THREAD_STATE_STRING_SIZE    (1024)

#define ZTD_DEFAULT_THREAD_COUNT            (1)
#define ZTD_DEFAULT_BLOCK_READ_COUNT        (40)

#define ZTD_MONITOR_RECORD_UNIT             (1000)

#define ZTD_REGION_INIT_SIZE                (1024 * 1024 * 10)
#define ZTD_REGION_NEXT_SIZE                (1024 * 8)

#define ZTD_DYNAMIC_INIT_SIZE               (1024 * 1024)
#define ZTD_DYNAMIC_NEXT_SIZE               ZTD_DYNAMIC_INIT_SIZE

#define ZTD_RECORD_ARRAY_SIZE               (1000)
#define ZTD_COMMIT_FREQUENCY                (5000)
/**
 * @todo align이 512로 사용되는데 디스크 장비의 block size에 따라 align이 달라지기도 한다.
 */ 
#define ZTD_ALIGN_512                       (512)
#define ZTD_ALIGN_128                       (128)

#define ZTD_LONG_DATA_INIT_SIZE             (1024)

/**
 * @brief: Import Mode 중 Column Size
 */ 
#define ZTD_INTEGER_SIGNIFICANT_FIGURE      (15)
#define ZTD_SMALLINT_SIGNIFICANT_FIGURE     (10)
#define ZTD_BIGINT_SIGNIFICANT_FIGURE       (25)
#define ZTD_REAL_SIGNIFICANT_FIGURE         (50)
#define ZTD_DOUBLE_SIGNIFICANT_FIGURE       (340)
#define ZTD_FLOAT_SIGNIFICANT_FIGURE        (150)

/**
 * @brief: Export Mode 중 Column Size
 */ 
#define ZTD_LONG_VAR_TYPE_SIZE              (1024 * 16)

#define ZTD_RECORD_EXTRA_SIZE               (3)

/**
 * @brief: Constant string definition
 */ 
#define ZTD_CONSTANT_STATE_LINK_FAILURE     "08S01"
#define ZTD_CONSTANT_IMPORT                 "IMPORT"
#define ZTD_CONSTANT_EXPORT                 "EXPORT"
#define ZTD_CONSTANT_TEXT_FORMAT            "text"
#define ZTD_CONSTANT_BINARY_FORMAT          "binary"
#define ZTD_CONSTANT_THREAD_WAITING         "Waiting"
#define ZTD_CONSTANT_THREAD_PROGRESSION     "Progression"
#define ZTD_CONSTANT_THREAD_COMPLETE        "Complete"
#define ZTD_CONSTANT_NULL                   '\0'
#define ZTD_CONSTANT_LINE_FEED              '\n'
#define ZTD_CONSTANT_CARRIAGE_RETURN        '\r'
#define ZTD_CONSTANT_TAB                    '\t'
#define ZTD_CONSTANT_SPACE                  ' '
#define ZTD_CONSTANT_DOT                    '.'
#define ZTD_CONSTANT_DOUBLEQUOTE            '"'
#define ZTD_CONSTANT_COMMASPACE             ", "
#define ZTD_CONSTANT_LENGTH_LINE_FEED       (1)
#define ZTD_CONSTANT_LENGTH_DOUBLEQUOTE     (1)
#define ZTD_CONSTANT_LENGTH_COMMASPACE      (2)
#define ZTD_CONSTANT_VALID_CHECK_VALUE1     (0x10)
#define ZTD_CONSTANT_VALID_CHECK_VALUE2     (0x11)
#define ZTD_CONSTANT_VALID_CHECK_VALUE3     (0x12)
#define ZTD_CONSTANT_VALID_CHECK_VALUE4     (0x13)
#define ZTD_DEFAULT_DSN                     "GOLDILOCKS"

/**
 * @brief: Checking valid
 */
#define ZTD_VALID_CHECK_VALUE_SIZE          (4)

#define ZTD_SET_VALID_CHECK_VALUES( aValue )            \
    {                                                   \
        (aValue)[0] = ZTD_CONSTANT_VALID_CHECK_VALUE1;  \
        (aValue)[1] = ZTD_CONSTANT_VALID_CHECK_VALUE2;  \
        (aValue)[2] = ZTD_CONSTANT_VALID_CHECK_VALUE3;  \
        (aValue)[3] = ZTD_CONSTANT_VALID_CHECK_VALUE4;  \
    }

#define ZTD_VALID_CHECK( aValue )                                               \
    ( ((aValue)[0] == ZTD_CONSTANT_VALID_CHECK_VALUE1) &&                       \
      ((aValue)[1] == ZTD_CONSTANT_VALID_CHECK_VALUE2) &&                       \
      ((aValue)[2] == ZTD_CONSTANT_VALID_CHECK_VALUE3) &&                       \
      ((aValue)[3] == ZTD_CONSTANT_VALID_CHECK_VALUE4) ) ? STL_TRUE : STL_FALSE

/**
 * @brief: gloader mode type
 */
typedef enum
{
    ZTD_MODE_TYPE_INIT = 0,
    ZTD_MODE_TYPE_EXPORT,
    ZTD_MODE_TYPE_IMPORT,
    ZTD_MODE_TYPE_MAX
} ztdModeType;

extern const stlChar *const gZtdModeString[];

/**
 * @brief: gloader format type
 */
typedef enum
{
    ZTD_FORMAT_TYPE_TEXT = 0,
    ZTD_FORMAT_TYPE_BINARY,
    ZTD_FORMAT_TYPE_MAX
} ztdFormatType;

/**
 * @brief: Import Mode 중 Parsing State
 */ 
#define ZTD_STATE_INIT                      (0)
#define ZTD_STATE_ROW_COMPLETE              (1)
#define ZTD_STATE_COLUMN_COMPLETE           (2)
#define ZTD_STATE_BAD                       (3)

/**
 * @brief: Import Mode 중 Datafile의 Record가 Parsing되어서 Array 단위로 저장되는 Chunk의 Size.
 */ 
#define ZTD_CHUNK_SIZE                      (1024 * 1024 * 10)

/**
 * @defgroup ztdError Error
 * @{
 */
/**
 * @brief syntax error
 */
#define ZTD_ERRCODE_SERVICE_NOT_AVAILABLE      STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 0 )
#define ZTD_ERRCODE_INVALID_CONTROL_FILE       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 1 )
#define ZTD_ERRCODE_BUFFER_OVERFLOW            STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 2 )
#define ZTD_ERRCODE_USE_SAME_FILE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 3 )
#define ZTD_ERRCODE_INVALID_HANDLE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 4 )
#define ZTD_ERRCODE_UNACCEPTABLE_VALUE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 5 )
#define ZTD_ERRCODE_DELIMITER_QUALIFIER        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 6 )
#define ZTD_ERRCODE_MULTI_THREAD_COUNT         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 7 )
#define ZTD_ERRCODE_ARRAY_SIZE                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 8 )
#define ZTD_ERRCODE_EXPORT_MULTI_THREAD        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 9 )
#define ZTD_ERRCODE_COMMUNICATION_LINK_FAILURE STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 10 )
#define ZTD_ERRCODE_INVALID_FILE_SIZE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 11 )
#define ZTD_ERRCODE_TEXT_MAX_FILE_SIZE         STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 12 )
#define ZTD_ERRCODE_INVALID_COLUMN_TYPE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 13 )
#define ZTD_ERRCODE_INVALID_COLUMN_COUNT       STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 14 )
#define ZTD_ERRCODE_INVALID_DATA_SIZE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 15 )
#define ZTD_ERRCODE_INVALID_BINARY_FILE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_GLIESE_TOOL_GLOADER, 16 )

#define ZTD_MAX_ERROR     (17)
/** @} */

/**
 * @brief input argument를 저장하는 구조체
 */
typedef struct ztdInputArguments
{
    stlChar       mId[ZTD_MAX_IDENTIFIER_LENGTH];
    stlChar       mPassword[ZTD_MAX_IDENTIFIER_LENGTH];
    stlChar       mControl[ZTD_MAX_FILE_PATH_LENGTH];
    stlChar       mLog[ZTD_MAX_FILE_PATH_LENGTH];
    stlChar       mBad[ZTD_MAX_FILE_PATH_LENGTH];
    stlChar       mData[ZTD_MAX_FILE_PATH_LENGTH];
    stlChar       mDsn[ZTD_MAX_IDENTIFIER_LENGTH];
    stlInt64      mArraySize;
    stlInt64      mCommitUnit;
    stlInt64      mThreadUnit;
    stlInt64      mMaxFileSize;
    stlUInt32     mDisplayUnit;
    stlUInt32     mErrorCount;
    stlBool       mUseAtomic;
    stlBool       mSilent;
    stlBool       mDirectIo;
} ztdInputArguments;

#define ZTD_DEFAULT_MAX_ERROR_COUNT   (100)
/**
 * @brief CONTROL FILE 의 contents를 담는 구조체
 */
typedef struct ztdControlInfo
{
    stlChar   mCharacterSet[ZTD_MAX_CHARACTERSET_NAME];
    stlChar   mSchema[ZTD_MAX_SCHEMA_NAME];
    stlChar   mTable[ZTD_MAX_TABLE_NAME];
    stlChar * mDelimiter;
    stlChar   mOpQualifier;
    stlChar   mClQualifier;
} ztdControlInfo;

typedef struct ztdCmdTableName
{
    stlChar   mSchema[ZTD_MAX_SCHEMA_NAME];
    stlChar   mTable[ZTD_MAX_TABLE_NAME];
} ztdCmdTableName;

typedef struct ztdCmdQualifier
{
    stlChar mOpQualifier;
    stlChar mClQualifier;
} ztdCmdQualifier;

typedef struct ztdCmdParseParam
{
    stlInt32         mPosition;     /* Pasing 중 Token 위치 */
    stlInt32         mLength;       /* mBuffer의 길이. */  
    stlInt32         mLineNo;       /* mBuffer의 현재 line number */
    stlInt32         mColumn;       /* 한 line에서 token 위치 */
    stlInt32         mColumnLen;    /* token의 길이 */
    stlInt32         mCurPosition;  /* Buffer에서 현재 문자 위치 */
    stlInt32         mNextPosition;
    void           * mYyScanner;    /* lex에서 사용되는 사용자 yyscanner */
    ztdControlInfo * mControlInfo;  /* Control File의 내용이 저장되는 구조체 */
    stlAllocator   * mAllocator;    /* flex와 bision에서 사용되는 allocator */
    stlErrorStack  * mErrorStack;   /* flex와 bision에서 사용되는 error stack */
    stlStatus        mErrStatus;    /* Error 확인 flag */
    stlChar        * mBuffer;       /* Control File을 파싱하기 전 내용을 저장하는 Buffer */
    stlChar        * mErrMsg;       /* Error 발생시 Error 메시지를 저장하는 Buffer */
} ztdCmdParseParam;

#define ZTD_CMD_KEY_WORD_COUNT( list ) ( STL_SIZEOF( list ) / STL_SIZEOF( list[0] ) )

typedef struct ztdCmdKeyWord
{
    const stlChar  * mName;
    stlInt16         mTokenCode;
} ztdCmdKeyWord;

extern const stlInt16 ztdCmdKeyWordCount;

typedef struct ztdBadRecord
{
    stlInt64   mOffset;
    stlInt64   mBufferSize;
    stlChar  * mBuffer;
} ztdBadRecord;

/**
 * @brief FILE desc와 각 FILE에 씌어질 Buffer를 담는 구조체
 */
typedef struct ztdFileAndBuffer
{
    stlChar        mLogBuffer[ZTD_MAX_LOG_STRING_SIZE];
    stlFile        mControlFile;
    stlFile        mDataFile;
    stlFile        mLogFile;
    stlFile        mBadFile;
    stlBool        mControlFlag;
    stlBool        mDataFlag;
    stlBool        mLogFlag;
    stlBool        mBadFlag;
    ztdBadRecord * mBadRecord;
} ztdFileAndBuffer;

/**
 * @brief Text Data를 Export할 때 사용되는 Buffer
 */
typedef struct ztdRecordBuffer
{
    stlChar  * mBuffer;
    stlChar  * mAlignedBuffer;
    stlChar  * mCurrentPtr;
    stlInt32   mBufferSize;
} ztdRecordBuffer;

#define ZTD_GET_REMAIN_BUFFER_LENGTH( aRecord )                                 \
    (stlInt32)( &(aRecord)->mAlignedBuffer[(aRecord)->mBufferSize - 1] -        \
                (aRecord)->mCurrentPtr + 1 )

/**
 * @brief Data FILE을 Parsing 하는 함수들 간 정보를 공유하는 구조체 
 */
typedef struct ztdParseInfo
{
    stlSize     mReadSize;       /* Data File을 Read한 크기 */
    stlInt64    mReadOffset;     /* ReadBuffer의 인덱스 */
    stlInt64    mPrevRecOffset;  /* 이전 Record의 Offset */ 
    stlInt64    mErrorCount;     /* bad record 개수 */
    stlInt64    mRecordCount;    /* Record의 총 개수 */
    stlInt64    mIdxRecord;      /* ColumnData.mDataBuffer의 Data 간격. FileAndBuffer.mBad의 인덱스  */
    stlInt16    mIdxColumn;      /* ColumnData의 인덱스 */
    stlInt16    mState;          /* Consumer Thread의 Parsing에서 Parsing State */
    SQLSMALLINT mColumnCount;    /* Column의 총 개수 */    
    stlBool     mDataStartFlag;  /* Qualifier가 출현 여부 flag */ 
    stlBool     mNoDataFlag;     /* 파일의 유무 확인 */
    stlBool     mExitParsing;    /* Chunk에 문자열 Parsing을 끊는 Flag */
} ztdParseInfo;

#define ZTD_PARSER_PREV_RECORD_INIT_OFFSET (-1)

#define ZTD_INIT_PARSE_INFO( aParseInfo )                                       \
    {                                                                           \
        (aParseInfo).mState = ZTD_STATE_INIT;                                   \
        (aParseInfo).mIdxRecord = 0;                                            \
        (aParseInfo).mIdxColumn = 0;                                            \
        (aParseInfo).mReadOffset = 0;                                           \
        (aParseInfo).mPrevRecOffset = ZTD_PARSER_PREV_RECORD_INIT_OFFSET;       \
        (aParseInfo).mErrorCount = 0;                                           \
        (aParseInfo).mRecordCount = 0;                                          \
        (aParseInfo).mColumnCount = 0;                                          \
        (aParseInfo).mReadSize = 0;                                             \
        (aParseInfo).mDataStartFlag = STL_FALSE;                                \
        (aParseInfo).mNoDataFlag = STL_FALSE;                                   \
        (aParseInfo).mExitParsing = STL_FALSE;                                  \
    }

/**
 * @brief SQLDescribeCol() 에서 얻은 컬럼 정보를 담는 Column 크기의 1차원 배열 구조체.
 */ 
typedef struct ztdColumnInfo
{
    stlChar     mName[ZTD_MAX_COLUMN_NAME];
    dtlDataType mDbType;
    SQLULEN     mDataSize;
    SQLLEN      mBufferSize;
    SQLSMALLINT mNameLength;
    SQLSMALLINT mSQLType;
    SQLSMALLINT mDecimalDigits;
    SQLSMALLINT mNullable;
    SQLLEN      mDisplaySize;
} ztdColumnInfo;

/**
 * @brief 실제 컬럼의 데이터를 저장하는 1차원 배열 구조체.
 * Import: array binding 을 위해 mData 구조체를 Array 개수만큼 할당.
 */ 
typedef struct ztdColDataImp
{
    stlInt64                           mDataOffset;
    SQLLEN                           * mIndicator;
    SQL_LONG_VARIABLE_LENGTH_STRUCT  * mData;
} ztdColDataImp;

typedef struct ztdColDataExp
{
    SQLLEN                             mIndicator;
    SQLLEN                             mBufferSize;
    stlInt64                           mOffset;
    stlChar                          * mBuffer;
} ztdColDataExp;

#define ZTD_ALIGN_BUFFER( aDesc, aSrc, aNum )                           \
    {                                                                   \
        (aDesc) = (void*)( STL_INT_FROM_POINTER(aSrc) +                 \
                           ((aNum) -                                    \
                            (STL_INT_FROM_POINTER(aSrc) % (aNum))) );   \
    }

#define ZTD_SET_INDICATOR( aIndi, aVal ) \
    {                                    \
        if( (aVal) == 0 )                \
        {                                \
            (aIndi) = -1;                \
        }                                \
        else                             \
        {                                \
            (aIndi) = (aVal);            \
        }                                \
    }

#define ZTD_SET_BUFFER( aStruct,                        \
                        aTarget,                        \
                        aIdxRecord )                    \
    {                                                   \
        (aTarget) = (aStruct)->mData[aIdxRecord].arr;   \
    }


/**
 * @brief: Array단위로 자른 Chunk(Records Buffer).
 */
typedef struct ztdChunkItem
{
    stlInt64       mSize;          /* mBuffer의 Size            */
    stlInt64       mOffset;        /* mBuffer의 Offset          */
    stlInt64       mStartRecNum;   /* Data file 기준 Record 번호 */
    stlInt64       mPrevRecOffset; /* 이전 Record의 Offset       */  
    stlChar      * mBuffer;
} ztdChunkItem;

#define ZTD_INIT_CHUNK_ITEM( aChunk )           \
    {                                           \
        (aChunk).mSize          = 0;            \
        (aChunk).mOffset        = 0;            \
        (aChunk).mStartRecNum      = 0;            \
        (aChunk).mPrevRecOffset = 0;            \
        (aChunk).mBuffer        = NULL;         \
    }
        
#define ZTD_QUEUE_NODE_FACTOR               (10)
#define ZTD_QUEUE_STATE_EMPTY               (0)

/**
 * @brief: 처리된 데이터의 개수를 관리하는 변수의 동시성 제어를 위한 상수
 */
#define ZTD_COUNT_STATE_RELEASE 0
#define ZTD_COUNT_STATE_ACQUIRE 1

/**
 * @brief: Queue Node.(Circular Queue의 Node로 사용).
 */
typedef struct ztdQueueNode
{
    struct ztdQueueNode * mLink;
    ztdChunkItem          mItem;
} ztdQueueNode;

/**
 * @brief: Queue.(Queue의 Front Node, Rear Node만 저장).
 */
typedef struct ztdQueueInfo
{
    stlSemaphore    mQueueSema;
    ztdQueueNode  * mFront;
    ztdQueueNode  * mRear;
    stlUInt32       mFreeCnt;
} ztdQueueInfo;

/**
 * @brief: Thread 들을 관리 위해 필요한 Semaphore와 정보들.
 */
typedef struct ztdManager
{
    stlSemaphore    mFileSema;
    stlSpinLock     mConsAddLock; /* Consumer Thread에서 Count를 위한 Lock */
    stlUInt64       mCntRecord;   /* 모든 Record 개수 */
    stlUInt64       mCntBad;      /* Bad Record 개수  */
} ztdManager;

/**
 * @brief: Consumer(Insert 수행) Thread 생성 후 Thread 함수에서 필요한 정보들.
 */ 
typedef struct ztdThreadArg
{
    SQLHENV             mEnv;
    SQLSMALLINT         mColumnCount;
    stlInt64          * mDataSize;
    stlErrorStack       mErrorStack;
    ztdInputArguments * mInputArguments;
    ztdQueueInfo      * mQueue;
    ztdControlInfo    * mControlInfo;
    ztdColumnInfo     * mColumnInfo;
    ztdFileAndBuffer  * mFileAndBuffer;
} ztdThreadArg;

#define ZTD_BUFFER_COUNT         (2)

#define ZTD_BUFFER_STATE_INIT    (0)
#define ZTD_BUFFER_STATE_READY   (1)
#define ZTD_BUFFER_STATE_PARSE   (2)
#define ZTD_BUFFER_STATE_FINISH  (3)

#define ZTD_IO_SLEEP_SEC         (10000)
#define ZTD_CPU_SLEEP_SEC        (1000)

/**
 * @brief: Disk I/O의 Double Buffering을 위한 Structure.
 */
typedef struct ztdDataManager
{
    stlChar      ** mReadBuffer;
    stlChar      ** mAlignedBuffer;
    stlSemaphore  * mSema;
    stlSize       * mSize;
    stlInt32        mProdIdx;
    stlInt32        mReadIdx;
    stlInt32      * mState;   
} ztdDataManager;

/**
 * @brief: Disk I/O를 수행하는 Thread를 위한 Argument.
 */
typedef struct ztdReadThreadArg
{
    ztdFileAndBuffer  * mDataFile;     /* 데이터 파일 포인터 */
    ztdDataManager    * mDataManager;
    stlErrorStack       mErrorStack;
} ztdReadThreadArg;

/**
 * @brief: Producer 사용. Buffer의 Overflow 체크 후 문자 하나 저장. Overflow면 두배의 크기로 확장 
 * 1. Record가 1개도 안들어가는 경우 확장한다.
 * 2. ReadBuffer(Parsing 되는 문자열)에 Record의 시작이 없는 경우 확장한다.
 */ 
#define ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( aChunkItem,                           \
                                                  aParseInfo,                           \
                                                  aReadOffset,                          \
                                                  aBuffer,                              \
                                                  aChar,                                \
                                                  aCompleted,                           \
                                                  aErrorStack )                         \
    {                                                                                   \
                                                                                        \
        if( (aChunkItem)->mOffset >= ((aChunkItem)->mSize - 2) )                        \
        {                                                                               \
            /* 파싱중인 Record가 처음 레코드인가? */                                         \
            /* ReadBuffer에 현재 파싱중인 레코드의 시작 문자가 포함하나? */                     \
            if( ((aParseInfo)->mIdxRecord == (aChunkItem)->mStartRecNum ) ||            \
                ((aParseInfo)->mPrevRecOffset == ZTD_PARSER_PREV_RECORD_INIT_OFFSET) )  \
            {                                                                           \
                STL_TRY( ztdExpandHeap( &((aChunkItem)->mBuffer),                       \
                                        ((aChunkItem)->mSize),                          \
                                        &((aChunkItem)->mSize),                         \
                                        (aErrorStack) ) == STL_SUCCESS );               \
                                                                                        \
                (aBuffer) = &(aChunkItem)->mBuffer[(aChunkItem)->mOffset];              \
            }                                                                           \
            else                                                                        \
            {                                                                           \
                (aParseInfo)->mIdxColumn = 0;                                           \
                (aCompleted) = STL_TRUE;                                                \
                (aChunkItem)->mOffset = (aChunkItem)->mPrevRecOffset;                   \
                (aReadOffset) = (aParseInfo)->mPrevRecOffset;                           \
                (aBuffer) = &(aChunkItem)->mBuffer[(aChunkItem)->mOffset];              \
                (aParseInfo)->mExitParsing = STL_TRUE;                                  \
                STL_THROW( EXIT_SUCCESS_FUNCTION );                                     \
            }                                                                           \
        }                                                                               \
                                                                                        \
        *(aBuffer) = (aChar);                                                           \
        (aBuffer)++;                                                                    \
        (aChunkItem)->mOffset++;                                                        \
    }

#define ZTD_PRODUCER_BAD_RECORD( aChunkItem,                            \
                                 aChunkBuffer,                          \
                                 aChar,                                 \
                                 aReadBuffer,                           \
                                 aReadOffset,                           \
                                 aParseInfo,                            \
                                 aDataManager,                          \
                                 aCompleted,                            \
                                 aErrorStack )                          \
    {                                                                   \
        do{                                                             \
            ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( (aChunkItem),     \
                                                      (aParseInfo),     \
                                                      (aReadOffset),    \
                                                      (aChunkBuffer),   \
                                                      (aChar),          \
                                                      (aCompleted),     \
                                                      (aErrorStack) );  \
                                                                        \
            if( (aReadOffset) >= (aParseInfo)->mReadSize )              \
            {                                                           \
                STL_TRY( ztdRequestData( (aReadBuffer),                 \
                                         (aDataManager),                \
                                         (aParseInfo),                  \
                                         (aErrorStack) )                \
                         == STL_SUCCESS );                              \
                                                                        \
                if( (aParseInfo)->mNoDataFlag == STL_TRUE )             \
                {                                                       \
                    (aParseInfo)->mIdxRecord++;                         \
                    STL_THROW( EXIT_SUCCESS_FUNCTION );                 \
                }                                                       \
                else                                                    \
                {                                                       \
                    (aReadOffset) = 0;                                  \
                }                                                       \
            }                                                           \
            (aChar) = (*(aReadBuffer))[ (aReadOffset) ];                \
            (aReadOffset)++;                                            \
        } while( (aChar) != ZTD_CONSTANT_LINE_FEED );                   \
                                                                        \
        ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( (aChunkItem),         \
                                                  (aParseInfo),         \
                                                  (aReadOffset),        \
                                                  (aChunkBuffer),       \
                                                  (aChar),              \
                                                  (aCompleted),         \
                                                  (aErrorStack) );      \
    }

#define ZTD_PRODUCER_CHECK_DELIMITER( aReadBuffer,                      \
                                      aDataManager,                     \
                                      aParseInfo,                       \
                                      aReadOffset,                      \
                                      aChunkItem,                       \
                                      aChunkBuffer,                     \
                                      aDelimiter,                       \
                                      aDelimiterLen,                    \
                                      aCompleted,                       \
                                      aSuccess,                         \
                                      aErrorStack )                     \
{                                                                       \
    stlInt32 i;                                                         \
    stlChar  sChar;                                                     \
                                                                        \
    (aSuccess) = STL_TRUE;                                              \
    for( i = 1; i < (aDelimiterLen); i++ )                              \
    {                                                                   \
        if( (aReadOffset) >= (aParseInfo)->mReadSize )                  \
        {                                                               \
            STL_TRY( ztdRequestData( (aReadBuffer),                     \
                                     (aDataManager),                    \
                                     (aParseInfo),                      \
                                     (aErrorStack) ) == STL_SUCCESS );  \
                                                                        \
            if( aParseInfo->mNoDataFlag == STL_TRUE )                   \
            {                                                           \
                (aParseInfo)->mIdxRecord++;                             \
                STL_THROW( EXIT_SUCCESS_FUNCTION );                     \
            }                                                           \
            else                                                        \
            {                                                           \
                (aReadOffset) = 0;                                      \
            }                                                           \
        }                                                               \
                                                                        \
        sChar = (*(aReadBuffer))[aReadOffset];                          \
                                                                        \
        if( sChar != (aDelimiter)[i] )                                  \
        {                                                               \
            (aSuccess) = STL_FALSE;                                     \
            (aReadOffset) -= (i - 1);                                   \
            (aChunkItem)->mOffset -= (i - 1);                           \
            (aChunkBuffer) -= (i - 1);                                  \
            break;                                                      \
        }                                                               \
                                                                        \
        ZTD_PRODUCER_CHECK_ASSIGN_CHAR_TO_BUFFER( (aChunkItem),         \
                                                  (aParseInfo),         \
                                                  (aReadOffset),        \
                                                  (aChunkBuffer),       \
                                                  sChar,                \
                                                  (aCompleted),         \
                                                  (aErrorStack) );      \
                                                                        \
        (aReadOffset)++;                                                \
    }                                                                   \
}

/**
 * @brief: Consumer에서 사용.Buffer의 Overflow 체크 후 문자 하나 저장. Overflow면 두배의 크기로 확장.
 */
#define ZTD_CONSUMER_CHECK_ASSIGN_CHAR_TO_BUFFER( aColumnData,                                   \
                                                  aParseInfo,                                    \
                                                  aDataBuffer,                                   \
                                                  aDataOffset,                                   \
                                                  aChar,                                         \
                                                  aErrorStack )                                  \
{                                                                                                \
    if( (aDataOffset) >= ( (aColumnData)->mData[ (aParseInfo)->mIdxRecord ].len - 2 ) )          \
    {                                                                                            \
        (aDataBuffer)[(aDataOffset)] = ZTD_CONSTANT_NULL;                                        \
                                                                                                 \
        STL_TRY( ztdExpandHeap( (stlChar**) &(aColumnData)->mData[(aParseInfo)->mIdxRecord].arr, \
                                (aColumnData)->mData[(aParseInfo)->mIdxRecord].len,              \
                                &(aColumnData)->mData[(aParseInfo)->mIdxRecord].len,             \
                                (aErrorStack) )                                                  \
                 == STL_SUCCESS );                                                               \
                                                                                                 \
        ZTD_SET_BUFFER( (aColumnData),                                                           \
                        (aDataBuffer),                                                           \
                        (aParseInfo)->mIdxRecord );                                              \
    }                                                                                            \
                                                                                                 \
    (aDataBuffer)[ (aDataOffset) ] = ( aChar );                                                  \
    (aDataOffset)++;                                                                             \
}

/**
 * @brief: Consumer에서 사용. BadRecord에 한 문자씩 저장.
 */ 
#define ZTD_CONSUMER_SET_BAD_BUFFER( aChar,                                     \
                                     aBadRecord,                                \
                                     aErrorStack )                              \
    {                                                                           \
        if( (aBadRecord)->mOffset == ( (aBadRecord)->mBufferSize - 2 ) )        \
        {                                                                       \
            (aBadRecord)->mBuffer[(aBadRecord)->mOffset] = ZTD_CONSTANT_NULL;   \
            STL_TRY( ztdExpandHeap( &(aBadRecord)->mBuffer,                     \
                                    (aBadRecord)->mBufferSize,                  \
                                    &(aBadRecord)->mBufferSize,                 \
                                    (aErrorStack ) )                            \
                     == STL_SUCCESS );                                          \
        }                                                                       \
                                                                                \
        (aBadRecord)->mBuffer[(aBadRecord)->mOffset] = (aChar);                 \
        (aBadRecord)->mOffset++;                                                \
                                                                                \
        STL_ASSERT( (aBadRecord)->mOffset < (aBadRecord)->mBufferSize );        \
    }

/**
 * @remark producer에서는 readoffset을 검사했지만 consumer에서는 chunkitem이 이미 정해진 크기라 검사하지 않는다.
 */ 
#define ZTD_CONSUMER_CHECK_DELIMITER( aReadBuffer,                              \
                                      aParseInfo,                               \
                                      aDelimiter,                               \
                                      aDelimiterLen,                            \
                                      aBadRecord,                               \
                                      aSuccess,                                 \
                                      aErrorStack )                             \
{                                                                               \
    stlInt32 i;                                                                 \
    stlChar  sChar;                                                             \
                                                                                \
    (aSuccess) = STL_TRUE;                                                      \
                                                                                \
    for( i = 1; i < (aDelimiterLen); i++ )                                      \
    {                                                                           \
        sChar = (aReadBuffer)[ (aParseInfo)->mReadOffset ];                     \
        (aParseInfo)->mReadOffset++;                                            \
                                                                                \
        if( sChar != (aDelimiter)[i] )                                          \
        {                                                                       \
            (aParseInfo)->mReadOffset -= i;                                     \
            (aBadRecord)->mOffset -= (i - 1);                                   \
            (aSuccess) = STL_FALSE;                                             \
            break;                                                              \
        }                                                                       \
                                                                                \
        ZTD_CONSUMER_SET_BAD_BUFFER( sChar,                                     \
                                     (aBadRecord),                              \
                                     (aErrorStack) );                           \
                                                                                \
    }                                                                           \
}

/**
 * @brief: buffered write/read size
 */
#define ZTD_WRITE_BUFFER_SIZE                 (1024 * 1024 * 30)
#define ZTD_READ_BUFFER_SIZE                  (1024 * 1024 * 30)
#define ZTD_BINARY_HEADER_SIZE                (4096)

#define ZTD_READ_BLOCK_COUNT                  (40)

#define ZTD_FILE_INFORMATION_SIZE             ( 1 + STL_SIZEOF( stlInt32 ) + STL_SIZEOF( stlInt32 ) + STL_SIZEOF( stlInt64 ) ) + (STL_SIZEOF( stlInt32 ) * 3)        // endian + version + database character set + column count + ( check value * 3 )

#define ZTD_COLUMN_INFORMATION_SIZE           ( STL_SIZEOF( stlInt64 ) + STL_SIZEOF( stlInt64 ) + STL_SIZEOF( dtlStringLengthUnit ) + STL_SIZEOF( dtlIntervalIndicator ) + STL_SIZEOF( stlInt64 ) + STL_SIZEOF( dtlDataType ) )  // ArgNum1 + ArgNum2 + StringLengthUnit + IntervalIndicator + AllocBufferSize + data type

#define ZTD_MAX_BAD_BUFFER_SIZE               (10 * 1024 * 1024)

#define ZTD_ALIGN( aAlignSize, sTempSize, aBoundary )                               \
    {                                                                               \
        stlInt64 sModulus = 0;                                                      \
                                                                                    \
        sModulus = (sTempSize) % (aBoundary);                                       \
                                                                                    \
        if( sModulus == 0 )                                                         \
        {                                                                           \
            aAlignSize = ( ((sTempSize) / (aBoundary) ) * (aBoundary) );            \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            aAlignSize = ( ( ((sTempSize) / (aBoundary) ) + 1 ) * (aBoundary) );    \
        }                                                                           \
    }                                                                               \

#define ZTD_DOUBLE_BUFFER_COUNT (3)

typedef enum ztdDoubleBufferState
{
    ZTD_DOUBLE_BUFFER_NONE = 0,
    ZTD_DOUBLE_BUFFER_ONE,
    ZTD_DOUBLE_BUFFER_TWO,
    ZTD_DOUBLE_BUFFER_THREE,
    ZTD_DOUBLE_BUFFER_END_OF_FILE,
    ZTD_DOUBLE_BUFFER_MAX
} ztdDoubleBufferState;

typedef struct ztdBinaryDoubleBuffer
{
    stlChar              * mBuffer1;
    stlChar              * mBuffer2;
    stlChar              * mBuffer3;
    ztdDoubleBufferState   mPutState;
    ztdDoubleBufferState   mGetState;
    stlInt32               mFilled;
} ztdBinaryDoubleBuffer;

typedef struct ztdWriteBufferedFile
{
    stlFile         mFileDesc;
    stlChar       * mBuffer;
    stlInt32        mCurrentBufferIndex;
    stlInt32        mMaxFileSize;
    stlInt32        mCurrentFileSize;
} ztdWriteBufferedFile;

typedef struct ztdReadBufferedFile
{
    stlFile                 mFileDesc;
    ztdBinaryDoubleBuffer   mDoubleBuffer;
    stlInt32                mCurrentBufferIndex;
    stlInt32                mCurrentDataSize;
} ztdReadBufferedFile;

typedef struct ztdValueBlockListDataFile
{
    stlChar                            mFileName[STL_MAX_FILE_NAME_LENGTH];
    struct ztdValueBlockListDataFile * mNext;
} ztdValueBlockListDataFile;

/**
 * @brief : ztdImportValueBlockList
 */
typedef struct ztdExportValueBlockList
{
    dtlValueBlockList  * mValueBlockList;
} ztdExportValueBlockList;

typedef struct ztdImportValueBlockList
{
    dtlValueBlockList ** mValueBlockList;
} ztdImportValueBlockList;

/**
 * @brief : ztdBinaryDataQueue
 */ 
#define ZTD_VALUE_BLOCK_LIST_WRITE_QUEUE_SIZE (4)
#define ZTD_EXECUTE_QUEUE_SIZE                (100)
#define ZTD_WRITE_BAD_QUEUE_SIZE              (4)

/**
 * @brief : binary data length.
 */
#define ZTD_VALUE_LEN_1BYTE_SIZE (1)
#define ZTD_VALUE_LEN_2BYTE_SIZE (3)
#define ZTD_VALUE_LEN_4BYTE_SIZE (5)
#define ZTD_VALUE_LEN_MAX_SIZE   (5)
#define ZTD_VALUE_LEN_2BYTE_FLAG ((stlUInt8)0xFE)
#define ZTD_VALUE_LEN_4BYTE_FLAG ((stlUInt8)0xFF)
#define ZTD_VALUE_LEN_1BYTE_MAX  ((stlUInt8)0xFD)
#define ZTD_VALUE_LEN_2BYTE_MAX  (STL_UINT16_MAX)
#define ZTD_VALUE_LEN_4BYTE_MAX  (STL_UINT32_MAX)

typedef struct ztdBinaryWriteDataQueue
{
    ztdExportValueBlockList * mBuffer[ZTD_VALUE_BLOCK_LIST_WRITE_QUEUE_SIZE];
    stlSemaphore              mItem;
    stlSemaphore              mEmpty;
    stlInt32                  mPutNumber;
    stlInt32                  mGetNumber;
} ztdBinaryWriteDataQueue;

typedef struct ztdBinaryReadDataQueue
{
    ztdImportValueBlockList * mBuffer;
    stlSemaphore              mItem;
    stlSemaphore              mEmpty;
    stlSpinLock               mPushLock; /* Push 과정에서 동시성 제어를 위해서 사용될 Spin Lock */
    stlInt32                  mItemCount;
    stlInt32                  mPutNumber;
    stlInt32                  mGetNumber;
} ztdBinaryReadDataQueue;

#define ZTD_BINARY_QUEUE_DATA_SWAP( aValueBlockList1, aValueBlockList2 )    \
    {                                                                       \
        dtlValueBlockList * sTemp;                                          \
                                                                            \
        sTemp = aValueBlockList1;                                           \
        aValueBlockList1 = aValueBlockList2;                                \
        aValueBlockList2 = sTemp;                                           \
    }

#define ZTD_GET_USED_BLOCK_COUNT( aValueBlockList )         (aValueBlockList)->mValueBlock->mUsedBlockCnt
#define ZTD_SET_USED_BLOCK_COUNT( aValueBlockList, aValue ) (aValueBlockList)->mValueBlock->mUsedBlockCnt = (aValue)

#define ZTD_GET_DATA_VALUE( aValueBlock, aRowIndex )        &( (aValueBlock)->mDataValue[ (aRowIndex) ] )

/**
 * @brief : column information
 */
typedef struct ztdValueBlockColumnInfo
{
    stlInt64             mArgNum1;
    stlInt64             mArgNum2;
    dtlStringLengthUnit  mStringLengthUnit;
    dtlIntervalIndicator mIntervalIndicator;
    stlInt32             mAllocBufferSize;
    stlInt32             mBufferSize;
    dtlDataType          mType;
} ztdValueBlockColumnInfo;

/**
 * @brief : ztdDataValue
 */
typedef struct ztdDataValue
{
    dtlDataValue        * mDataValue;
    struct ztdDataValue * mNext;
} ztdDataValue;


/**
 * @brief : binary mode write thread arg
 */
typedef struct ztdBinaryModeWriteThreadArg
{
    SQLHENV                   mEnv;
    SQLSMALLINT               mColumnCount;
    ztdInputArguments       * mInputArguments;
    ztdControlInfo          * mControlInfo;
    
    dtlValueBlockList       * mValueBlockList;
    
    ztdBinaryWriteDataQueue * mBinaryWriteQueue;
    
    stlChar                 * mHeader;
    stlInt32                  mHeaderSize;

    stlChar                 * mFileName;
    ztdValueBlockColumnInfo * mValueBlockColumnInfo;

    ztdColumnInfo           * mColumnInfo;

    ztdFileAndBuffer        * mFileAndBuffer;

    stlAllocator            * mAllocator;
    stlDynamicAllocator     * mDynamicAllocator;

    stlErrorStack             mErrorStack;
} ztdBinaryModeWriteThreadArg;

/**
 * @brief : binary mode read thread arg
 */
typedef struct ztdBinaryModeReadThreadArg
{
    SQLHENV                    mEnv;
    SQLSMALLINT                mColumnCount;
    ztdInputArguments        * mInputArguments;
    ztdControlInfo           * mControlInfo;
    
    dtlValueBlockList       ** mValueBlockList;
    ztdImportValueBlockList  * mExecuteValueBlockList;
    ztdImportValueBlockList  * mWriteBadValueBlockList;
    
    ztdBinaryReadDataQueue   * mBinaryExecuteQueue;
    ztdBinaryReadDataQueue   * mBinaryWriteBadQueue;
    stlInt32                   mMyQueueIndex;
    
    stlInt32                   mThreadUnit;
    
    stlChar                  * mHeader;
    stlInt32                   mHeaderSize;

    ztdValueBlockColumnInfo  * mValueBlockColumnInfo;

    ztdColumnInfo            * mColumnInfo;

    ztdFileAndBuffer         * mFileAndBuffer;

    stlAllocator             * mAllocator;
    stlDynamicAllocator      * mDynamicAllocator;

    stlErrorStack              mErrorStack;
} ztdBinaryModeReadThreadArg;

typedef struct ztdDoubleBufferThreadArg
{
    ztdReadBufferedFile   * mReadBufferedFile;
    ztdBinaryDoubleBuffer * mDoubleBuffer;
    stlErrorStack           mErrorStack;
} ztdDoubleBufferThreadArg;

#endif /* _ZTDDEF_H_ */

