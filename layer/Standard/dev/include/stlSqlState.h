/*******************************************************************************
 * stlSqlState.h
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


#ifndef _STLSQLSTATE_H_
#define _STLSQLSTATE_H_ 1

/**
 * @file stlSqlState.h
 * @brief Standard Layer SQLSTATE definition
 */

/**
 * @defgroup stlExternalSQLSTATECode External SQLSTATE Codes (SQL-2008, ODBC 3.0)
 * @ingroup stlError
 * @{
 * @brief 표준 SQLSTATE 코드 (SQL-2008 + ODBC3.0)
 * @remarks
 * - 올바른 SQLSTATE 코드 찾는 쉬운 방법
 *  - 예를 들어 다음과 같은 상황에 대한 적절한 SQLSTATE 설정 방법을 찾아보자.
 *  - 너무 긴 이름이 사용된 경우
 *   - 오라클에서 유사한 에러 코드를 발생시킨다.
 *    - SQL> CREATE TABLE too_long_table_name_too_long_table_name_too_long_table_name_too_long_table_name ( i1 integer );
 *    - ERROR at line 1:
 *    - ORA-00972: identifier is too long
 *   - SQLSTATE 와 ORACLE 에러 코드 매핑 테이블을 확인한다.
 *    - 참조 URL: http://download.oracle.com/docs/cd/E11882_01/appdev.112/e10827/appd.htm#i635429
 *    - 위의 예는 다음과 같이 Mapping 되어 있다.
 *     - SQLSTATE : 42000
 *     - Message : syntax error or access rule violation
 *     - Oracle Code : ORA-00900 .. 00999
 *   - 42000 을 파일 또는 Doxygen 주석에서 검색한다.
 *    - 검색하면 다음과 같은 에러코드이다.
 *    - STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS
 *
 * 
 * 외부 상태코드 체계는 SQL 호환성을 위해 ISO/IEC 9075:(SQL-2008 기준) SQLSTATE 코드 체계를 준수한다. <BR>
 * Postgres SQLSTATE 를 사용하지 않고 재정립하려는 이유는 다음과 같다.
 * - 누락된 ISO-9075 SQLSTATE 가 많다.
 *  - 전체 100 여개 .vs. 전체 200 개 이상
 *  - 실제 표준에 해당하는 SQLSTATE는 아래의 SQLSTATE를 제외하면 많이 부족하다.
 * - 독자적인 SQLSTATE 가 많다.
 *  - XX___,
 *  - __P__
 *  - 동일한 개념의 제품이 아니라면 Postgres의 자체 SQLSTATE를 사용하기는 어렵다.
 *  
 * - 벤더별 SQLSTATE 특징
 *  - IBM DB2 : http://publib.boulder.ibm.com/infocenter/db2luw/v9r8/topic/com.ibm.db2.luw.messages.doc/doc/rdb2stt.html
 *   - 사용자 관점의 모든 상황을 SQLSTATE로 정리한 느낌.
 *    - 표준 SQLSTATE + 자체 SQLSTATE
 *     - 독자적인 class : 51___, 53___, 54, 55, 56, 57, 58, 5U
 *     - 표준 class 내 독자적인 subclass : __5__, __6__, ...
 *    - ex) 42727: No default primary tablespace exists for the new table
 *   - ODBC 3.0 SQLSTATE는 없음 : HY___
 *  - MS-SQL ODBC : http://msdn.microsoft.com/en-us/library/ms714687%28v=VS.85%29.aspx
 *   - ODBC 함수 관점의 상황을 SQLSTATE로 정리하고 있음.
 *    - 표준 SQLSTATE + 자체 SQLSTATE
 *     - 독자적인 class : IM___
 *     - 표준 class 내 독자적인 subclass : __S__
 *    - ODBC에 국한된 관점으로 SQLSTATE를 정리하고 있음
 *     - 서버 관점의 자원 상황등은 SQLSTATE로 표현되지 않는 것으로 보임.
 *     - 서버에 국한된 표준 SQLSTATE도 많이 누락되어 있음.
 *  - MySQL : http://dev.mysql.com/doc/refman/5.6/en/error-messages-server.html
 *   - MS-SQL의 ODBC SQLSTATE 만을 지원함
 *   - Server Error Code는 SQLSTATE에 N:1 로 매핑하고 있음.
 *    - ex) internal error codes : HY000 = N:1
 *  - Oracle : http://download.oracle.com/docs/cd/E11882_01/appdev.112/e10825/pc_09err.htm#i6768
 *   - 표준 SQLSTATE + 자체 SQLSTATE
 *    - 독자적인 class : 6____, 7____, 8____
 *    - 표준 class 내 독자적인 subclass : 없음
 *    - ODBC 3.0 SQLSTATE 는 없음 : HY___
 *   - SQLSTATE 코드와 자체 에러 코드 ORA-XXXXX 를 모두 매핑하고 있음.
 *    - SQLSTATE 코드는 SQL92 기준으로 ORA-XXXXX가 존재하지 않아도 모두 나열하고 있음.
 *    
 * - SQLSTATE 코드 체계
 *  - ISO/IEC 9075 는 SQLSTATE 코드의 각 문자를 다음과 같이 분류하고 있다.
 *   - SQLSTATE : 다섯 글자의 영문자 및 숫자로 구성
 *   - CLASS VALUE : 앞의 두 글자
 *   - SUBCLASS VALUE : 뒤의 세 글자
 *  - CLASS VALUE 의 첫 글자는 다음과 같이 영역이 구분되어 있다.
 *   - Standard-defined classes
 *    - ISO/IEC 9075 또는 X/Open 등의 다른 국제 표준 규약에서 정의한 표준 코드 영역
 *    - 문자 : A, B, C, D, E, F, G, H
 *    - 숫자 : 0, 1, 2, 3, 4
 *   - Implementation-defined classes
 *    - Vendor-specific code 영역
 *    - 문자 : 표준 코드 영역 이외의 문자 (I ~ Z)
 *    - 숫자 : 표준 코드 영역 이외의 숫자 (5 ~ 9)
 *    - 표준에 없는 SQLSTATE를 정의하기 위해서는 위 영역의 문자와 숫자를 사용해야 한다.
 *  - SUBCLASS VALUE 의 첫 글자는 다음과 같이 영역이 구분되어 있다.
 *   - 000 : subclass가 없는 SQLSTATE.
 *   - CLASS VALUE 에 따라 다음과 같은 의미를 갖는다.
 *    - Standard-defined class 내에서 사용될 경우
 *     - Standard-defined subclasses
 *      - ISO/IEC 9075 또는 X/Open 등의 다른 국제 표준 규약에서 정의한 표준 코드 영역
 *      - 문자 : A, B, C, D, E, F, G, H
 *      - 숫자 : 0, 1, 2, 3, 4
 *     - Implementation-defined subclasses
 *      - Vendor-specific code 영역
 *      - 문자 : 표준 코드 영역 이외의 문자 (I ~ Z)
 *      - 숫자 : 표준 코드 영역 이외의 숫자 (5 ~ 9)
 *      - Standard-defined class 내에서 표준에 없는 SQLSTATE를 정의하기 위해서는 위 영역의 문자와 숫자를 사용해야 한다.
 *       - 믿지 말자
 *       - ODBC 3.0 에서는 01S01 과 같이 모든 DBMS 가 S값을 사용하는 경우도 있다.
 *    - Implementation-defined class 내에서 사용될 경우
 *     - 000 을 제외하고 표준에서 정의한 영역은 없음.
 *  - 독자적인 SQLSTATE를 정의하려면...
 *   - 유사한 class 내에서 표준에서 정의하지 않은 subclass 영역을 정의하여 사용한다.
 *   - 유사한 class 가 없다면 표준에서 정의하지 않은 class 영역을 정의하여 사용한다.
 *   - 정의하지 않는것이 바람직할 듯.
 *   
 * - SQLSTATE 코드의 Category 분류
 *  - ISO/IEC 9075 는 SQLSTATE를 다음과 같은 네 개의 category로 분류한다.
 *  - Category는 SQLSTATE class 코드로 구분 가능하다.
 *   - Category S [00]: 성공 (Successful completion)
 *   - Category W [01]: 경고를 포함한 성공 (successful completion with Warning)
 *   - Category N [02]: 데이타가 없는 완료 (No-data situation)
 *   - Category X [>02]: 예외 상황 (eXcetption situation)
 *
 * - 상태 코드의 물리적 명명 규칙
 *  - 표준 SQLSTATE 코드
 *   - ISO/IEC 9075 SQLSTATE 코드를 그대로 사용
 *    - 참조 문서) 아래 각 문서의 SQLSTATE class and subclass values
 *    - ISO/IEC 9075-02:2008 Part-2 Foundation (SQL/Foundation)
 *    - ISO/IEC 9075-03:2008 Part-3 Call-Level Interface (SQL/CLI)
 *    - ISO/IEC 9075-04:2008 Part-4 Persitent Stored Modules (SQL/PSM)
 *    - ISO/IEC 9075-09:2008 Part-9 Management of External Data (SQL/MED)
 *    - ISO/IEC 9075-10:2008 Part-10 Object Language Bindings (SQL/OLB)
 *    - ISO/IEC 9075-13:2008 Part-13 SQL Routines and Types Using the Java Programming Language (SQL/JRT)
 *    - ISO/IEC 9075-14:2008 Part-14 XML-Related Specifications (SQL/XML)
 *   - XOPEN SQLSTATE 코드
 *    - ODBC 3.0 을 사용함
 *     - XOPEN SQLSTATE 명세를 찾지 못함.
 *      - HY___ 의 대부분은 ISO-9075-3 에 포함됨
 *     - ODBC 의 de facto standard는 MS-SQL ODBC Driver임.
 *      - 독자적인 class : IM___ (Driver specific condition)
 *      - 독자적인 subclass : 표준 class 내에서 __S__ 를 사용하고 있음.
 *    - 참조 URL)
 *     - http://msdn.microsoft.com/en-us/library/ms714687%28v=VS.85%29.aspx
 *  - SQLSTATE 코드와 주석(또는 메시지)
 *   - 참조 
 *    - 참조 A) 각 표준의 SQLSTATE TABLE - SQLSTATE class and subclass values
 *    - 참조 B) 9075-02 의 13.4 <externally-invoked procedure> 의 Interface.SQL template 정의
 *   - DEFINE 상수 이름 : STL_EXT_ERRCODE_ + 참조 B
 *   - SQLSTATE 코드   : 참조 A의 Class와 Subclass 컬럼
 *   - 주석(또는 메시지) : 참조 A의 Condition과 Subcondition 컬럼
 *  - Example)
 *   - 상수명    : STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_DOES_NOT_EXIST
 *   - SQLSTATE : STL_MAKE_SQLSTATE('0','8','0','0','3')
 *   - 주석      : connection exception: connection does not exist
 *
 * - 우리의 SQLSTATE 정책은 Category X 의 주석을 참조.
 */

/**
 * @brief 1문자를 6비트에 저장하기 위한 매크로
 */
#define STL_MAKE_SIXBIT( aCh ) ( ( ( aCh ) - '0' ) & 0x3F )

/**
 * @brief 6비트에서 1문자를 읽기 위한 매크로
 */
#define STL_MAKE_UNSIXBIT( aVal ) ( ( ( aVal ) & 0x3F ) + '0' )

/**
 * @brief 5자리의 외부 에러 문자열을 4바이트 정수에 기록하기 위한 매크로
 */
#define STL_MAKE_SQLSTATE( aCh1, aCh2, aCh3, aCh4, aCh5 )       \
    ( ( STL_MAKE_SIXBIT( aCh1 ) ) +                             \
      ( STL_MAKE_SIXBIT( aCh2 ) << 6 ) +                        \
      ( STL_MAKE_SIXBIT( aCh3 ) << 12 ) +                       \
      ( STL_MAKE_SIXBIT( aCh4 ) << 18 ) +                       \
      ( STL_MAKE_SIXBIT( aCh5 ) << 24 ) )

#define STL_SQLSTATE_LENGTH  (5)

/**
 * @defgroup stlExternalCategoryS Category S: 성공 (Successful completion)
 * @ingroup stlExternalSQLSTATECode
 * @{
 * @remarks
 * - class 00 : successful completion
 *  - [ISO-9075-2]
 */

/**
 *  00000 [CLASS 00] [ISO-9075-2] <BR>
 *  successful completion: general <BR>
 */
#define STL_EXT_ERRCODE_SUCCESSFUL_COMPLETION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','0','0','0','0')

/** @} stlExternalCategoryS */


/**
 * @defgroup stlExternalCategoryW Category W: 경고를 포함한 성공 (successful completion with Warning)
 * @ingroup stlExternalSQLSTATECode
 * @{
 * @remarks
 * - class 01 : warning
 *  - [ISO-9075-2]
 *  - [ISO-9075-4]
 *  - [ISO-9075-13] 
 *  - [ISO-9075-14]
 *  - [ODBC-3]
 */

/**
 *  01000 [CLASS 01] [ISO/IEC 9075-2] [ISO/IEC 9075-4] [ISO/IEC 9075-10] [ISO/IEC 9075-14] [ODBC-3] <BR>
 *  general warning <BR>
 */
#define STL_EXT_ERRCODE_WARNING_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','1','0','0','0')

/**
 *  01001 [CLASS 01] [ISO/IEC 9075-2] [ODBC-3] <BR>
 *  warning: cursor operation conflict <BR>
 */
#define STL_EXT_ERRCODE_WARNING_CURSOR_OPERATION_CONFLICT  \
        STL_MAKE_SQLSTATE('0','1','0','0','1')

/**
 *  01002 [CLASS 01] [ISO/IEC 9075-2] [ODBC-3] <BR>
 *  warning: disconnect error <BR>
 */
#define STL_EXT_ERRCODE_WARNING_DISCONNECT_ERROR  \
        STL_MAKE_SQLSTATE('0','1','0','0','2')

/**
 *  01003 [CLASS 01] [ISO/IEC 9075-2] [ODBC-3] <BR>
 *  warning: null value eliminated in set function <BR>
 */
#define STL_EXT_ERRCODE_WARNING_NULL_VALUE_ELIMINATED_IN_SET_FUNCTION  \
        STL_MAKE_SQLSTATE('0','1','0','0','3')

/**
 *  01004 [CLASS 01] [ISO/IEC 9075-2] [ODBC-3] <BR>
 *  warning: string data, right truncation <BR>
 */
#define STL_EXT_ERRCODE_WARNING_STRING_DATA_RIGHT_TRUNCATION_WARNING  \
        STL_MAKE_SQLSTATE('0','1','0','0','4')

/**
 *  01005 [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: insufficient item descriptor areas <BR>
 */
#define STL_EXT_ERRCODE_WARNING_INSUFFICIENT_ITEM_DESCRIPTOR_AREAS  \
        STL_MAKE_SQLSTATE('0','1','0','0','5')

/**
 *  01006 [CLASS 01] [ISO/IEC 9075-2] [ODBC-3] <BR>
 *  warning: privilege not revoked <BR>
 */
#define STL_EXT_ERRCODE_WARNING_PRIVILEGE_NOT_REVOKED  \
        STL_MAKE_SQLSTATE('0','1','0','0','6')

/**
 *  01007 [CLASS 01] [ISO/IEC 9075-2] [ODBC-3] <BR>
 *  warning: privilege not granted <BR>
 */
#define STL_EXT_ERRCODE_WARNING_PRIVILEGE_NOT_GRANTED  \
        STL_MAKE_SQLSTATE('0','1','0','0','7')

/**
 *  01009 [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: search condition too long for information schema <BR>
 */
#define STL_EXT_ERRCODE_WARNING_SEARCH_CONDITION_TOO_LONG_FOR_INFORMATION_SCHEMA  \
        STL_MAKE_SQLSTATE('0','1','0','0','9')

/**
 *  0100A [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: query expression too long for information schema <BR>
 */
#define STL_EXT_ERRCODE_WARNING_QUERY_EXPRESSION_TOO_LONG_FOR_INFORMATION_SCHEMA  \
        STL_MAKE_SQLSTATE('0','1','0','0','A')

/**
 *  0100B [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: default value too long for information schema <BR>
 */
#define STL_EXT_ERRCODE_WARNING_DEFAULT_VALUE_TOO_LONG_FOR_INFORMATION_SCHEMA  \
        STL_MAKE_SQLSTATE('0','1','0','0','B')

/**
 *  0100C [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: result sets returned <BR>
 */
#define STL_EXT_ERRCODE_WARNING_RESULT_SETS_RETURNED  \
        STL_MAKE_SQLSTATE('0','1','0','0','C')

/**
 *  0100D [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: additional result sets returned <BR>
 */
#define STL_EXT_ERRCODE_WARNING_ADDITIONAL_RESULT_SETS_RETURNED  \
        STL_MAKE_SQLSTATE('0','1','0','0','D')

/**
 *  0100E [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: attempt to return too many result sets <BR>
 */
#define STL_EXT_ERRCODE_WARNING_ATTEMPT_TO_RETURN_TOO_MANY_RESULT_SETS  \
        STL_MAKE_SQLSTATE('0','1','0','0','E')

/**
 *  0100F [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: statement too long for information schema <BR>
 */
#define STL_EXT_ERRCODE_WARNING_STATEMENT_TOO_LONG_FOR_INFORMATION_SCHEMA  \
        STL_MAKE_SQLSTATE('0','1','0','0','F')

/**
 *  01010 [CLASS 01] [ISO/IEC 9075-14] <BR>
 *  warning: column cannot be mapped <BR>
 */
#define STL_EXT_ERRCODE_WARNING_COLUMN_CANNOT_BE_MAPPED  \
        STL_MAKE_SQLSTATE('0','1','0','1','0')

/**
 * 01011 [CLASS 01] [ISO/IEC 9075-13] <BR>
 * warning : SQL-Java path too long for information schema  <BR>
 */
#define STL_EXT_ERRCODE_WARNING_SQL_JAVA_PATH_TOO_LONG_FOR_INFORMATION_SCHEMA   \
        STL_MAKE_SQLSTATE('0','1','0','1','1')

/**
 *  01012 [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: invalid number of conditions <BR>
 */
#define STL_EXT_ERRCODE_WARNING_INVALID_NUMBER_OF_CONDITIONS  \
        STL_MAKE_SQLSTATE('0','1','0','1','2')

/**
 *  0102F [CLASS 01] [ISO/IEC 9075-2] <BR>
 *  warning: array data, right truncation <BR>
 */
#define STL_EXT_ERRCODE_WARNING_ARRAY_DATA_RIGHT_TRUNCATION  \
        STL_MAKE_SQLSTATE('0','1','0','2','F')

/**
 *  01S00 [CLASS 01] [ODBC-3] <BR>
 *  warning: invalid connection string attribute <BR>
 */
#define STL_EXT_ERRCODE_WARNING_INVALID_CONNECTION_STRING_ATTRIBUTE  \
        STL_MAKE_SQLSTATE('0','1','S','0','0')

/**
 *  01S01 [CLASS 01] [ODBC-3] <BR>
 *  warning: error in row <BR>
 */
#define STL_EXT_ERRCODE_WARNING_ERROR_IN_ROW  \
        STL_MAKE_SQLSTATE('0','1','S','0','1')

/**
 *  01S02 [CLASS 01] [ODBC-3] <BR>
 *  warning: option value changed <BR>
 */
#define STL_EXT_ERRCODE_WARNING_OPTION_VALUE_CHANGED  \
        STL_MAKE_SQLSTATE('0','1','S','0','2')

/**
 *  01S06 [CLASS 01] [ODBC-3] <BR>
 *  warning: attempt to fetch before the result set returned the first rowset <BR>
 */
#define STL_EXT_ERRCODE_WARNING_ATTEMPT_TO_FETCH_BEFORE_THE_RESULT_SET_RETURNED_THE_FIRST_ROWSET  \
        STL_MAKE_SQLSTATE('0','1','S','0','6')

/**
 *  01S07 [CLASS 01] [ODBC-3] <BR>
 *  warning: fractional truncation <BR>
 */
#define STL_EXT_ERRCODE_WARNING_FRACTIONAL_TRUNCATION  \
        STL_MAKE_SQLSTATE('0','1','S','0','7')

/**
 *  01S08 [CLASS 01] [ODBC-3] <BR>
 *  warning: error saving File DSN <BR>
 */
#define STL_EXT_ERRCODE_WARNING_ERROR_SAVING_FILE_DSN  \
        STL_MAKE_SQLSTATE('0','1','S','0','8')

/**
 *  01S09 [CLASS 01] [ODBC-3] <BR>
 *  warning: invalid keyword <BR>
 */
#define STL_EXT_ERRCODE_WARNING_INVALID_KEYWORD  \
        STL_MAKE_SQLSTATE('0','1','S','0','9')

/** @} stlExternalCategoryW */


/**
 * @defgroup stlExternalCategoryN Category N: 데이타가 없는 완료 (No-data situation)
 * @ingroup stlExternalSQLSTATECode
 * @{
 * @remarks
 * - class 02 : no data
 *  - [ISO-9075-2]
 */

/**
 *  02000 [CLASS 02] [ISO/IEC 9075-2] <BR>
 *  no data: general <BR>
 */
#define STL_EXT_ERRCODE_NO_DATA_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','2','0','0','0')

/**
 *  02001 [CLASS 02] [ISO/IEC 9075-2] <BR>
 *  no data: no additional result sets returned <BR>
 */
#define STL_EXT_ERRCODE_NO_DATA_NO_ADDITIONAL_RESULT_SETS_RETURNED  \
        STL_MAKE_SQLSTATE('0','2','0','0','1')

/** @} stlExternalCategoryN */


/**
 * @defgroup stlExternalCategoryX Category X: 예외 상황 (eXcetption situation)
 * @ingroup stlExternalSQLSTATECode
 * @{
 * @remarks
 * 
 * 우리의 SQLSTATE 코드 설정 정책
 * - ISO 표준은 SQLSTATE 코드를 class와 subclass 로 의미별로 분류하고 있다.
 *  - SQLSTATE 코드만으로 각 모듈(SQL, PSM, CLI)에 적절한 코드를 판단하기 어렵다.
 *  - 즉, 하나의 class를 여러 표준에서 함께 사용하고 있다.
 *  - 해당 SQLSTATE 코드가 어떤 표준에서 정의하고 있는지 확인해야 한다.
 * - SQLSTATE 코드는 다음과 같은 표준에서 서로 중복되지 않도록 결정하고 있다.
 *  - 모듈에 국한된 에러라면 부합하는 영역의 코드를 사용해야 한다.
 *  - [ISO-9075-2] Foundation, 모든 모듈에서 사용하는 코드
 *  - [ISO-9075-3] CLI, Call Level Interface 에서만 정의한 코드
 *  - [ISO-9075-4] PSM 에서만 정의한 코드
 *  - [ISO-9075-9] Datalink 와 같이 외부데이터를 사용을 위해 정의한 코드
 *  - [ISO-9075-10] 객체지향 언어을 위해 정의한 코드
 *  - [ISO-9075-13] Java 언어를 위해 정의한 코드
 *  - [ISO-9075-14] XML 을 위해 정의한 코드
 *  - [ODBC-3] ODBC driver를 위해 정의한 코드
 *   - 대부분 ISO-9075-3 에 함께 정의되어 있음.
 *   - 그러나, __S__ 와 같은 독자적인 코드 체계도 존재한다.
 * - SQLSTATE 코드 설정 정책을 결정함에 있어 다음과 같은 원칙에 대한 공감대가 필요하다.
 *  - 가급적 자체 SQLSTATE 코드를 만들지 않는다.
 *   - 존재하지 않을 경우,
 *    - 원하는 SQLSTATE class 가 없다면 HY000 을 설정
 *     - 다음과 같은 general SQLSTATE 를 설정하고 internal code를 참조하도록 유도
 *     - HY000 : general error   (ISO-9075-3, ODBC-3)
 *     - 01000 : general warning (ISO-9075-2)
 *     - 02000 : general no data (ISO-9075-2)
 *    - 원하는 SQLSTATE class 가 있으나, subclass가 없다면 __000 을 설정
 *   - SQLSTATE 코드의 추가가 불가피하다면...
 *    - 별도의 자체 class를 정의한다.
 *  - ODBC
 *   - De factor standard 인 MS의 ODBC driver manager 를 따르는 것이 바람직하다.
 *   - [ODBC-3] 로 정의된 코드만 사용한다.
 *   - 서버에서 설정된 SQLSTATE 가 [ODBC-3] 가 아니라면 [ODBC-3]로 매핑해 주어야 한다.
 *  - JDBC
 *   - SQLSTATE로 SQL 표준과 XOPEN 를 모두 사용함.
 *   - 표준의 모든 SQLSTATE 코드를 사용가능하므로 ODBC와 달리 서버 SQLSTATE 의 별도 매핑이 필요없다.
 *   - 자체 모듈에서 발새한 SQLSTATE 코드는 다음 표준을 사용
 *    - 권고 표준 : ISO-9075-13(Java), ISO-9075-10(객체지향), ISO-9075-3(CLI) 
 *  - 서버
 *   - ODBC, JDBC 모두를 위해서는 가급적 [ISO-9075] [ODBC-3] 가 모두 설정되어 있는 에러 코드를 사용
 *   - 해당 모듈에 부합되는 표준을 선택
 *    - Storage Manager
 *     - 권고 표준 : ISO-9075-2(기본), ODBC-3
 *      - 적절한 SQLSTATE 가 많지 않아 보임.
 *      - 그러나, 아예 무시하면 안됨.
 *       - 40001 (ISO-9075-2) : transaction rollback: serialization failure
 *       - 42S12 (ODBC-3) : index not found
 *     - 다음과 같은 General Code를 가장 많이 사용할 듯.
 *      - HY000, 01000, 02000
 *    - Query Processor
 *     - 권고 표준 : ISO-9075-2(기본), ODBC-3
 *      - 원하는 SQLSTATE가 거의 있을 것으로 보임.
 *     - 1.0 스펙에서는 ISO-9075-4(PSM), ISO-9075-14(XML)는 사용할 일 없음.
 *    - Connection & Session
 *     - 권고 표준 : ISO-9075-2(기본), ISO-9075-3(CLI), ODBC-3
 *  
 * - 유의 사항 예제
 *  - 의미가 동일한 상태코드가 여러개 있을 수 있다.
 *   - 예를 들어, invalid string length에 해당하는 상태코드는 다음과 같은 것들이 있다.
 *   - HY090 [ISO-9075-3] 
 *   - HV090 [ISO-9075-9]
 *   - CLI 는 HY090을 설정해야 함.
 *  - 의미가 동일하나 사용하면 안되는 상태코드가 있다.
 *   - 예를 들어, memory allocation error 에 해당하는 상태코드는 다음과 같은 것들이 있다.
 *   - HY001 [ISO-9075-3] : CLI 에서의 memory error
 *   - HV001 [ISO-9075-9] : FSM 에서의 memory error
 *   - 서버에서의 memory allocation error는 SQLSTATE를 HY000(general error)로 정의한다.
 *  - SQLSTATE 구현 및 주석
 *   - 모든 표준 SQLSTATE (ISO-9075 + ODBC-3)를 나열함.
 *   - class 구분은 다음과 같이 구성되어 있다.
 *    - class name [subclass cnt] : class description
 *     - 정의하고 있는 표준 [ISO-XXXX]
 *   - SQLSTATE 주석은 다음과 같이 구성되어 있다.
 *    - SQLSTATE 코드 [class code] [관련 표준 1] [관련 표준 2] ...
 *    - class message: subclass message
 * 
 *  
 * 각 class별 정보
 * 
 * - class 07 [*] : dynamic SQL error
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 08 [*] : connection exception
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 09 [0] : triggered action exception
 *  - [ISO-9075-2]
 * - class 0A [*] : feature not supported
 *  - [ISO-9075-2]
 * - class 0D [0] : invalid target type specification
 *  - [ISO-9075-2]
 * - class 0E [0] : invalid schema name list specification
 *  - [ISO-9075-2]
 * - class 0F [*] : locator exception
 *  - [ISO-9075-2]
 * - class 0K [0] : resignal when handler not active
 *  - [ISO-9075-4]
 * - class 0L [0] : invalid grantor
 *  - [ISO-9075-2]
 * - class 0M [0] : invalid SQL-invoked procedure reference
 *  - [ISO-9075-2]
 * - class 0N [*] : SQL/XML mapping error
 *  - [ISO-9075-14]
 * - class 0P [0] : invalid role specification
 *  - [ISO-9075-2]
 * - class 0S [0] : invalid transform group name specification
 *  - [ISO-9075-2]
 * - class 0T [0] : target table disagrees with cursor specification
 *  - [ISO-9075-2]
 * - class 0U [0] : attempt to assign to non-updatable
 *  - [ISO-9075-2]
 * - class 0V [0] : attempt to assign to ordering column
 *  - [ISO-9075-2]
 * - class 0W [*] : prohibited statement encountered during trigger execution
 *  - [ISO-9075-2]
 * - class 0X [0] : invalid foreign server specification
 *  - [ISO-9075-9]
 * - class 0Y [*] : pass-through specific condition
 *  - [ISO-9075-9]
 * - class 0Z [*] : diagnostics exception
 *  - [ISO-9075-2]
 *  - [ISO-9075-4]
 *
 * - class 10 [0] : XQuery error 
 *  - [ISO-9075-14]
 * - class 20 [0] : case not found for case statement
 *  - [ISO-9075-4]
 * - class 21 [*] : cardinality violation
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 22 [*] : data exception
 *  - [ISO-9075-2]
 *  - [ISO-9075-4]
 *  - [ISO-9075-9]
 *  - [ISO-9075-14]
 *  - [ODBC-3]
 * - class 23 [*] : integrity constraint violation
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 24 [*] : invalid cursor state
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 25 [*] : invalid transaction state
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 26 [0] : invalid SQL statement name
 *  - [ISO-9075-2]
 * - class 27 [*] : triggered data change violation
 *  - [ISO-9075-2]
 * - class 28 [0] : invalid authorization specification
 *  - [ISO-9075-2]
 * - class 2B [0] : dependent privilege descriptors
 *  - [ISO-9075-2]
 * - class 2C [0] : invalid character set name
 *  - [ISO-9075-2]
 * - class 2D [0] : invalid transaction termination
 *  - [ISO-9075-2]
 * - class 2E [0] : invalid connection name
 *  - [ISO-9075-2]
 * - class 2F [*] : SQL routine exception
 *  - [ISO-9075-2]
 * - class 2H [0] : invalid collation name
 *  - [ISO-9075-2]
 * 
 * - class 30 [0] : invalid SQL statement identifier
 *  - [ISO-9075-2]
 * - class 33 [0] : invalid SQL descriptor name
 *  - [ISO-9075-2]
 * - class 34 [0] : invalid cursor name
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 35 [0] : invalid condition number
 *  - [ISO-9075-2]
 * - class 36 [*] : cursor sensitivity exception
 *  - [ISO-9075-2]
 * - class 38 [*] : external routine exception
 *  - [ISO-9075-2]
 * - class 39 [*] : external routine invocation exception
 *  - [ISO-9075-2]
 * - class 3B [*] : savepoint exception
 *  - [ISO-9075-2]
 * - class 3C [0] : ambiguous cursor name
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 3D [0] : invalid catalog name
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 3F [0] : invalid schema name
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 *
 * - class 40 [*] : transaction rollback
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 42 [*] : syntax error or access rule violation
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 44 [0] : with check option violation
 *  - [ISO-9075-2]
 *  - [ODBC-3]
 * - class 45 [0] : unhandled user-defined exception
 *  - [ISO-9075-4]
 * - class 46 [*] : 다음과 같이 여러 모듈이 하나의 class 를 공유하고 있음.  그러나, subclass는 충돌하지 않음
 *  - [ISO-9075-10] OLB-specific error
 *  - [ISO-9075-13] Java DDL
 *  - [ISO-9075-13] Java Execution
 *
 * - class HV [*] : FDW-specific condition 
 *  - [ISO-9075-9]
 * - class HW [*] : datalink exception 
 *  - [ISO-9075-9]
 * - class HY [*] : CLI-specific condition
 *  - [ISO-9075-3]
 *  - [ISO-9075-9]
 *  - [ODBC-3]
 * - class HZ [0] : Remote Database Access
 *  - [ISO-9075-2]
 *
 * - class IM [*] : ODBC driver manager가 생성하는 에러코드임.
 *  - [ODBC-3]
 *
 * - class RP [0] : Operating system resource exception (Resource Platform)
 *  - [비표준]
 * - class RD [0] : Database resource exception (Resource Database)
 *  - [비표준]
 */

/**
 * 07000 [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: general <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','7','0','0','0')

/**
 * 07001 [CLASS 07] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * dynamic SQL error: using clause does not match dynamic parameter specifications <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_DYNAMIC_PARAMETER_SPEC   \
        STL_MAKE_SQLSTATE('0','7','0','0','1')

/**
 * 07002 [CLASS 07] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * dynamic SQL error: using clause does not match target specifications <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_DOES_NOT_MATCH_TARGET_SPEC   \
        STL_MAKE_SQLSTATE('0','7','0','0','2')

/**
 * 07003 [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: cursor specification cannot be executed <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_CURSOR_SPECIFICATION_CANNOT_BE_EXECUTED   \
        STL_MAKE_SQLSTATE('0','7','0','0','3')

/**
 * 07004 [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: using clause required for dynamic parameters <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_REQUIRED_FOR_DYNAMIC_PARAMETERS   \
        STL_MAKE_SQLSTATE('0','7','0','0','4')

/**
 * 07005 [CLASS 07] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * dynamic SQL error: prepared statement not a cursor specification <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_PREPARED_STATEMENT_NOT_A_CURSOR_SPECIFICATION   \
        STL_MAKE_SQLSTATE('0','7','0','0','5')

/**
 * 07006 [CLASS 07] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * dynamic SQL error: restricted data type attribute violation <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_RESTRICTED_DATA_TYPE_ATTRIBUTE_VIOLATION   \
        STL_MAKE_SQLSTATE('0','7','0','0','6')

/**
 * 07007 [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: using clause required for result fields <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_USING_CLAUSE_REQUIRED_FOR_RESULT_FIELDS   \
        STL_MAKE_SQLSTATE('0','7','0','0','7')

/**
 * 07008 [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: invalid descriptor count <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_DESCRIPTOR_COUNT   \
        STL_MAKE_SQLSTATE('0','7','0','0','8')

/**
 * 07009 [CLASS 07] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * dynamic SQL error: invalid descriptor index <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_DESCRIPTOR_INDEX   \
        STL_MAKE_SQLSTATE('0','7','0','0','9')

/**
 * 0700B [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: data type transform function violation <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_DATA_TYPE_TRANSFORM_FUNCTION_VIOLATION   \
        STL_MAKE_SQLSTATE('0','7','0','0','B')

/**
 * 0700C [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: undefined DATA value <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_UNDEFINED_DATA_VALUE   \
        STL_MAKE_SQLSTATE('0','7','0','0','C')

/**
 * 0700D [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: invalid DATA target <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_DATA_TARGET   \
        STL_MAKE_SQLSTATE('0','7','0','0','D')

/**
 * 0700E [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: invalid LEVEL value <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_LEVEL_VALUE   \
        STL_MAKE_SQLSTATE('0','7','0','0','E')

/**
 * 0700F [CLASS 07] [ISO/IEC 9075-2] <BR>
 * dynamic SQL error: invalid DATETIME_INTERVAL_CODE <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_DATETIME_INTERVAL_CODE   \
        STL_MAKE_SQLSTATE('0','7','0','0','F')

/**
 * 07S01 [CLASS 07] [ODBC-3] <BR>
 * dynamic SQL error: invalid use of default parameter <BR>
 */
#define STL_EXT_ERRCODE_DYNAMIC_SQL_ERROR_INVALID_USE_OF_DEFAULT_PARAMETER   \
        STL_MAKE_SQLSTATE('0','7','S','0','1')

/**
 * 08000 [CLASS 08] [ISO/IEC 9075-2] <BR>
 * connection exception: general <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_NO_SUBCLASS    \
        STL_MAKE_SQLSTATE('0','8','0','0','0')

/**
 * 08001 [CLASS 08] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * connection exception: SQL-client unable to establish SQL-connection <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_SQLCLIENT_UNABLE_TO_ESTABLISH_SQLCONNECTION    \
        STL_MAKE_SQLSTATE('0','8','0','0','1')

/**
 * 08002 [CLASS 08] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * connection exception: connection name in use <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_NAME_IN_USE   \
        STL_MAKE_SQLSTATE('0','8','0','0','2')

/**
 * 08003 [CLASS 08] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * connection exception: connection does not exist  <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_DOES_NOT_EXIST  \
        STL_MAKE_SQLSTATE('0','8','0','0','3')

/**
 * 08004 [CLASS 08] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * connection exception: SQL-server rejected establishment of SQL-connection <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_SQLSERVER_REJECTED_ESTABLISHMENT_OF_SQLCONNECTION  \
        STL_MAKE_SQLSTATE('0','8','0','0','4')

/**
 * 08006 [CLASS 08] [ISO/IEC 9075-2] <BR>
 * connection exception: connection failure <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_CONNECTION_FAILURE   \
        STL_MAKE_SQLSTATE('0','8','0','0','6')

/**
 * 08007 [CLASS 08] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * connection exception: transaction resolution unknown <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_TRANSACTION_RESOLUTION_UNKNOWN  \
        STL_MAKE_SQLSTATE('0','8','0','0','7')

/**
 * 08S01 [CLASS 08] [ODBC-3] <BR>
 * connection exception: communication link failure <BR>
 */
#define STL_EXT_ERRCODE_CONNECTION_EXCEPTION_COMMUNICATION_LINK_FAILURE  \
        STL_MAKE_SQLSTATE('0','8','S','0','1')

/**
 * 09000 [CLASS 09] [ISO/IEC 9075-2] <BR>
 * triggered action exception: general <BR>
 */
#define STL_EXT_ERRCODE_TRIGGERED_ACTION_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','9','0','0','0')

/**
 * 0A000 [CLASS 0A] [ISO/IEC 9075-2] <BR>
 * feature not supported: general error  <BR>
 */
#define STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','A','0','0','0')

/**
 * 0A001 [CLASS 0A] [ISO/IEC 9075-2] <BR>
 * feature not supported: multiple server transactions  <BR>
 */
#define STL_EXT_ERRCODE_FEATURE_NOT_SUPPORTED_MULTIPLE_ENVIRONMENT_TRANSACTIONS   \
        STL_MAKE_SQLSTATE('0','A','0','0','1')

/**
 * 0D000 [CLASS 0D] [ISO/IEC 9075-2] <BR>
 * invalid target type specification: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TARGET_TYPE_SPECIFICATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','D','0','0','0')

/**
 * 0E000 [CLASS 0E] [ISO/IEC 9075-2] <BR>
 * invalid schema name list specification: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_SCHEMA_NAME_LIST_SPECIFICATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','E','0','0','0')

/**
 * 0F000 [CLASS 0F] [ISO/IEC 9075-2] <BR>
 * locator exception: general  <BR>
 */
#define STL_EXT_ERRCODE_LOCATOR_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','F','0','0','0')

/**
 * 0F001 [CLASS 0F] [ISO/IEC 9075-2] <BR>
 * locator exception: invalid specification  <BR>
 */
#define STL_EXT_ERRCODE_LOCATOR_EXCEPTION_INVALID_SPECIFICATION   \
        STL_MAKE_SQLSTATE('0','F','0','0','1')

/**
 * 0K000 [CLASS 0K] [ISO/IEC 9075-4] <BR>
 * resignal when handler not active: general  <BR>
 */
#define STL_EXT_ERRCODE_RESIGNAL_WHEN_HANDLER_NOT_ACTIVE_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','K','0','0','0')

/**
 * 0L000 [CLASS 0L] [ISO/IEC 9075-2] <BR>
 * invalid grantor: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_GRANTOR_STATE_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','L','0','0','0')

/**
 * 0M000 [CLASS 0M] [ISO/IEC 9075-2] <BR>
 * invalid SQL-invoked procedure reference: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_SQL_INVOKED_PROCEDURE_REFERENCE_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','M','0','0','0')

/**
 * 0N000 [CLASS 0N] [ISO/IEC 9075-14] <BR>
 * SQL/XML mapping error: general  <BR>
 */
#define STL_EXT_ERRCODE_SQL_XML_MAPPING_ERROR_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','N','0','0','0')

/**
 * 0N001 [CLASS 0N] [ISO/IEC 9075-14] <BR>
 * SQL/XML mapping error: unmappable XML Name  <BR>
 */
#define STL_EXT_ERRCODE_SQL_XML_MAPPING_ERROR_UNMAPPABLE_XML_NAME   \
        STL_MAKE_SQLSTATE('0','N','0','0','1')

/**
 * 0N002 [CLASS 0N] [ISO/IEC 9075-14] <BR>
 * SQL/XML mapping error: invalid XML character  <BR>
 */
#define STL_EXT_ERRCODE_SQL_XML_MAPPING_ERROR_INVALID_XML_CHARACTER   \
        STL_MAKE_SQLSTATE('0','N','0','0','2')

/**
 * 0P000 [CLASS 0P] [ISO/IEC 9075-2] <BR>
 * invalid role specification: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_ROLE_SPECIFICATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','P','0','0','0')

/**
 * 0S000 [CLASS 0S] [ISO/IEC 9075-2] <BR>
 * invalid transform group name specification: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSFORM_GROUP_NAME_SPECIFICATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','S','0','0','0')

/**
 * 0T000 [CLASS 0T] [ISO/IEC 9075-2] <BR>
 * target table disagrees with cursor specification: general <BR>
 */
#define STL_EXT_ERRCODE_TARGET_TABLE_DISAGREES_WITH_CURSOR_SPECIFICATION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','T','0','0','0')

/**
 * 0U000 [CLASS 0U] [ISO/IEC 9075-2] <BR>
 * attempt to assign to non-updatable: general <BR>
 */
#define STL_EXT_ERRCODE_ATTEMPT_TO_ASSIGN_TO_NON_UPDATABLE_COLUMN_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','U','0','0','0')

/**
 * 0V000 [CLASS 0V] [ISO/IEC 9075-2] <BR>
 * attempt to assign to ordering column: general <BR>
 */
#define STL_EXT_ERRCODE_ATTEMPT_TO_ASSIGN_TO_ORDERING_COLUMN_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','V','0','0','0')

/**
 * 0W000 [CLASS 0W] [ISO/IEC 9075-2] <BR>
 * prohibited statement encountered during trigger execution: general  <BR>
 */
#define STL_EXT_ERRCODE_PROHIBITED_STATEMENT_DURING_TRIGGER_EXECUTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','W','0','0','0')

/**
 * 0W001 [CLASS 0W] [ISO/IEC 9075-2] <BR>
 * prohibited statement encountered during trigger execution: modify table modified by data change delta table  <BR>
 */
#define STL_EXT_ERRCODE_PROHIBITED_STATEMENT_DURING_TRIGGER_EXECUTION_MODIFY_TABLE_MODIFIED_BY_DATA_CHANGE_DELTA_TABLE  \
        STL_MAKE_SQLSTATE('0','W','0','0','1')

/**
 * 0X000 [CLASS 0X] [ISO/IEC 9075-9] <BR>
 * invalid foreign server specification: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_FOREIGN_SERVER_SPECIFICATION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','X','0','0','0')

/**
 * 0Y000 [CLASS 0Y] [ISO/IEC 9075-9] <BR>
 * pass-through specific condition: general  <BR>
 */
#define STL_EXT_ERRCODE_PASS_THROUGH_SPECIFIC_CONDITION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('0','Y','0','0','0')

/**
 * 0Y001 [CLASS 0Y] [ISO/IEC 9075-9] <BR>
 * pass-through specific condition: invalid cursor option  <BR>
 */
#define STL_EXT_ERRCODE_PASS_THROUGH_SPECIFIC_CONDITION_INVALID_CURSOR_OPTION  \
        STL_MAKE_SQLSTATE('0','Y','0','0','1')

/**
 * 0Y002 [CLASS 0Y] [ISO/IEC 9075-9] <BR>
 * pass-through specific condition: invalid cursor allocation  <BR>
 */
#define STL_EXT_ERRCODE_PASS_THROUGH_SPECIFIC_CONDITION_INVALID_CURSOR_ALLOCATION  \
        STL_MAKE_SQLSTATE('0','Y','0','0','2')

/**
 * 0Z000 [CLASS 0Z] [ISO/IEC 9075-2] [ISO/IEC 9075-4] <BR>
 * diagnostics exception: general <BR>
 */
#define STL_EXT_ERRCODE_DIAGNOSTICS_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('0','Z','0','0','0')

/**
 * 0Z001 [CLASS 0Z] [ISO/IEC 9075-2] <BR>
 * diagnostics exception: maximum number of stacked diagnostics areas exceeded <BR>
 */
#define STL_EXT_ERRCODE_DIAGNOSTICS_EXCEPTION_MAXIMUM_NUMBER_OF_DIAGNOSTICS_AREAS_EXCEEDED   \
        STL_MAKE_SQLSTATE('0','Z','0','0','1')

/**
 * 0Z002 [CLASS 0Z] [ISO/IEC 9075-4] <BR>
 * diagnostics exception: stacked diagnostics accessed without active handler <BR>
 */
#define STL_EXT_ERRCODE_DIAGNOSTICS_EXCEPTION_STACKED_DIAGNOSTICS_ACCESSED_WITHOUT_ACTIVE_HANDLER   \
        STL_MAKE_SQLSTATE('0','Z','0','0','2')

/**
 * 10000 [CLASS 10] [ISO/IEC 9075-14] <BR>
 * XQuery error: general <BR>
 */
#define STL_EXT_ERRCODE_XQUERY_ERROR_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('1','0','0','0','0')

/**
 * 20000 [CLASS 20] [ISO/IEC 9075-4] <BR>
 * case not found for case statement: general <BR>
 */
#define STL_EXT_ERRCODE_CASE_NOT_FOUND_FOR_CASE_STATEMENT_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('2','0','0','0','0')

/**
 * 21000 [CLASS 21] [ISO/IEC 9075-2] <BR>
 * cardinality violation: general <BR>
 */
#define STL_EXT_ERRCODE_CARDINALITY_VIOLATION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('2','1','0','0','0')

/**
 * 21S01 [CLASS 21] [ODBC-3] <BR>
 * cardinality violation: insert value list does not match column list <BR>
 */
#define STL_EXT_ERRCODE_CARDINALITY_VIOLATION_INSERT_VALUE_LIST_DOES_NOT_MATCH_COLUMN_LIST  \
        STL_MAKE_SQLSTATE('2','1','S','0','1')

/**
 * 21S02 [CLASS 21] [ODBC-3] <BR>
 * cardinality violation: degree of derived table does not match column list <BR>
 */
#define STL_EXT_ERRCODE_CARDINALITY_VIOLATION_DEGREE_OF_DERIVED_TABLE_DOES_NOT_MATCH_COLUMN_LIST  \
        STL_MAKE_SQLSTATE('2','1','S','0','2')

/**
 * 22000 [CLASS 22] [ISO/IEC 9075-2] [ISO/IEC 9075-4] [ISO/IEC 9075-9] [ISO/IEC 9075-14] <BR>
 * data exception: general <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','2','0','0','0')

/**
 * 22001 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: string data, right truncation <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_RIGHT_TRUNCATION   \
        STL_MAKE_SQLSTATE('2','2','0','0','1')

/**
 * 22002 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: null value, no indicator parameter <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_NO_INDICATOR_PARAMETER   \
        STL_MAKE_SQLSTATE('2','2','0','0','2')

/**
 * 22003 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: numeric value out of range <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NUMERIC_VALUE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('2','2','0','0','3')

/**
 * 22004 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: null value not allowed <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_NOT_ALLOWED   \
        STL_MAKE_SQLSTATE('2','2','0','0','4')

/**
 * 22005 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: error in assignment <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_ERROR_IN_ASSIGNMENT   \
        STL_MAKE_SQLSTATE('2','2','0','0','5')

/**
 * 22006 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid interval format <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_INTERVAL_FORMAT   \
        STL_MAKE_SQLSTATE('2','2','0','0','6')

/**
 * 22007 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: invalid datetime format <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_DATETIME_FORMAT   \
        STL_MAKE_SQLSTATE('2','2','0','0','7')

/**
 * 22008 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: datetime field overflow <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_DATETIME_FIELD_OVERFLOW   \
        STL_MAKE_SQLSTATE('2','2','0','0','8')

/**
 * 22009 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid time zone displacement value <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_TIME_ZONE_DISPLACEMENT_VALUE   \
        STL_MAKE_SQLSTATE('2','2','0','0','9')

/**
 * 2200B [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: escape character conflict <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_ESCAPE_CHARACTER_CONFLICT   \
        STL_MAKE_SQLSTATE('2','2','0','0','B')

/**
 * 2200C [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid use of escape character <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_USE_OF_ESCAPE_CHARACTER   \
        STL_MAKE_SQLSTATE('2','2','0','0','C')

/**
 * 2200D [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid escape octet <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ESCAPE_OCTET   \
        STL_MAKE_SQLSTATE('2','2','0','0','D')

/**
 * 2200E [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: null value in array target <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_IN_ARRAY_TARGET   \
        STL_MAKE_SQLSTATE('2','2','0','0','E')

/**
 * 2200F [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: zero-length character string <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_ZERO_LENGTH_CHARACTER_STRING   \
        STL_MAKE_SQLSTATE('2','2','0','0','F')

/**
 * 2200G [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: most specific type mismatch <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_MOST_SPECIFIC_TYPE_MISMATCH   \
        STL_MAKE_SQLSTATE('2','2','0','0','G')

/**
 * 2200H [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: sequence generator limit exceeded <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_SEQUENCE_GENERATOR_LIMIT_EXCEEDED   \
        STL_MAKE_SQLSTATE('2','2','0','0','H')

/**
 * 2200J [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: nonidentical notations with the same name <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NON_IDENTICAL_NOTAIONTS_WITH_THE_SAME_NAME   \
        STL_MAKE_SQLSTATE('2','2','0','0','J')

/**
 * 2200K [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: nonidentical unparsed entities with the same name <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NON_IDENTICAL_UNPARSED_ENTITIES_WITH_THE_SAME_NAME   \
        STL_MAKE_SQLSTATE('2','2','0','0','K')

/**
 * 2200L [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: not an XML document <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NOT_AN_XML_DOCUMENT   \
        STL_MAKE_SQLSTATE('2','2','0','0','L')

/**
 * 2200M [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: invalid XML document <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_XML_DOCUMENT   \
        STL_MAKE_SQLSTATE('2','2','0','0','M')

/**
 * 2200N [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: invalid XML content <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_XML_CONTENT   \
        STL_MAKE_SQLSTATE('2','2','0','0','N')

/**
 * 2200P [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: interval value out of range <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_VALUE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('2','2','0','0','P')

/**
 * 2200Q [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: multiset value overflow <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_MULTISET_VALUE_OVERFLOW   \
        STL_MAKE_SQLSTATE('2','2','0','0','Q')

/**
 * 2200R [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: XML value overflow <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_XML_VALUE_OVERFLOW   \
        STL_MAKE_SQLSTATE('2','2','0','0','R')

/**
 * 2200S [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: invalid comment <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_COMMENT   \
        STL_MAKE_SQLSTATE('2','2','0','0','S')

/**
 * 2200T [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: invalid processing instruction <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PROCESSING_INSTRUCTION   \
        STL_MAKE_SQLSTATE('2','2','0','0','T')

/**
 * 2200U [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: not an XQuery document node <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NOT_AN_XQUERY_DOCUMENT_NODE   \
        STL_MAKE_SQLSTATE('2','2','0','0','U')

/**
 * 2200V [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: invalid XQuery context item <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_XQUERY_CONTEXT_ITEM   \
        STL_MAKE_SQLSTATE('2','2','0','0','V')

/**
 * 2200W [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: XQuery serialization error <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_XQUERY_SERIALIZATION_ERROR   \
        STL_MAKE_SQLSTATE('2','2','0','0','W')

/**
 * 22010 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid indicator parameter value <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_INDICATOR_PARAMETER_VALUE   \
        STL_MAKE_SQLSTATE('2','2','0','1','0')

/**
 * 22011 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: substring error <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_SUBSTRING_ERROR   \
        STL_MAKE_SQLSTATE('2','2','0','1','1')

/**
 * 22012 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: division by zero <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_DIVISION_BY_ZERO   \
        STL_MAKE_SQLSTATE('2','2','0','1','2')

/**
 * 22013 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid preceding or following size in window function <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PRECEDING_OR_FOLLOWING_SIZE_IN_WINDOW_FUNCTION   \
        STL_MAKE_SQLSTATE('2','2','0','1','3')

/**
 * 22014 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid argument for NTILE function <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ARGUMENT_FOR_NTILE_FUNCTION   \
        STL_MAKE_SQLSTATE('2','2','0','1','4')

/**
 * 22015 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: interval field overflow <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INTERVAL_FIELD_OVERFLOW   \
        STL_MAKE_SQLSTATE('2','2','0','1','5')

/**
 * 22016 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid argument for NTH_VALUE function <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ARGUMENT_FOR_NTH_VALUE_FUNCTION   \
        STL_MAKE_SQLSTATE('2','2','0','1','6')

/**
 * 22017 [CLASS 22] [ISO/IEC 9075-9] <BR>
 * data exception: invalid data specified for datalink <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_DATA_TYPE_SPECIFIED_FOR_DATALINK   \
        STL_MAKE_SQLSTATE('2','2','0','1','7')

/**
 * 22018 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: invalid character value for cast <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_CHARACTER_VALUE_FOR_CAST   \
        STL_MAKE_SQLSTATE('2','2','0','1','8')

/**
 * 22019 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: invalid escape character <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ESCAPE_CHARACTER   \
        STL_MAKE_SQLSTATE('2','2','0','1','9')

/**
 * 2201A [CLASS 22] [ISO/IEC 9075-9] <BR>
 * data exception: null argument passed to datalink constructor <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_ARGUMENT_PASSED_TO_DATALINK_CONSTRUCTOR   \
        STL_MAKE_SQLSTATE('2','2','0','1','A')

/**
 * 2201B [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid regular expression <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_REGULAR_EXPRESSION   \
        STL_MAKE_SQLSTATE('2','2','0','1','B')

/**
 * 2201C [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: null row not permitted in table <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_ROW_NOT_PERMITTED_IN_TABLE   \
        STL_MAKE_SQLSTATE('2','2','0','1','C')

/**
 * 2201D [CLASS 22] [ISO/IEC 9075-9] <BR>
 * data exception: datalink value exceeds maximum length <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_DATALINK_VALUE_EXCEEDS_MAXIMUM_LENGTH   \
        STL_MAKE_SQLSTATE('2','2','0','1','D')

/**
 * 2201E [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid argument for natural logarithm <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ARGUMENT_FOR_NATURAL_LOGARITHM   \
        STL_MAKE_SQLSTATE('2','2','0','1','E')

/**
 * 2201F [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid argument for power function <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ARGUMENT_FOR_POWER_FUNCTION   \
        STL_MAKE_SQLSTATE('2','2','0','1','F')

/**
 * 2201G [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid argument for width bucket function <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ARGUMENT_FOR_WIDTH_BUCKET_FUNCTION   \
        STL_MAKE_SQLSTATE('2','2','0','1','G')

/**
 * 2201H [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid row version <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ROW_VERSION   \
        STL_MAKE_SQLSTATE('2','2','0','1','H')

/**
 * 2201J [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: XQuery sequence cannot be validated <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_XQUERY_SEQUENCE_CANNOT_BE_VALIDATED   \
        STL_MAKE_SQLSTATE('2','2','0','1','J')

/**
 * 2201K [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: XQuery document node cannot be validated <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_XQUERY_DOCUMENT_NODE_CANNOT_BE_VALIDATED   \
        STL_MAKE_SQLSTATE('2','2','0','1','K')

/**
 * 2201L [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: no XML schema found <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NO_XML_SCHEMA_FOUND   \
        STL_MAKE_SQLSTATE('2','2','0','1','L')

/**
 * 2201M [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: element namespace not declared <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_ELEMENT_NAMESPACE_NOT_DECLARED   \
        STL_MAKE_SQLSTATE('2','2','0','1','M')

/**
 * 2201N [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: global element not declared <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_GLOBAL_ELEMENT_NOT_DECLARED   \
        STL_MAKE_SQLSTATE('2','2','0','1','N')

/**
 * 2201P [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: no XML element with the specified QName <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NO_XML_ELEMENT_WITH_THE_SPECIFIED_QNAME   \
        STL_MAKE_SQLSTATE('2','2','0','1','P')

/**
 * 2201Q [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: no XML element with the specified namespace <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NO_XML_ELEMENT_WITH_THE_SPECIFIED_NAMESPACE   \
        STL_MAKE_SQLSTATE('2','2','0','1','Q')

/**
 * 2201R [CLASS 22] [ISO/IEC 9075-14] <BR>
 * data exception: validation failure <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_VALIDATION_FAILURE   \
        STL_MAKE_SQLSTATE('2','2','0','1','R')

/**
 * 2201S [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid XQuery regular expression <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_XQUERY_REGULAR_EXPRESSION   \
        STL_MAKE_SQLSTATE('2','2','0','1','S')

/**
 * 2201T [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid XQuery option flag <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_XQUERY_OPTION_FLAG   \
        STL_MAKE_SQLSTATE('2','2','0','1','T')

/**
 * 2201U [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: attempt to replace a zero-length string <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_ATTEMPT_TO_REPLACE_A_ZERO_LENGTH_STRING   \
        STL_MAKE_SQLSTATE('2','2','0','1','U')

/**
 * 2201V [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid XQuery replacement string <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_XQUERY_REPLACEMENT_STRING   \
        STL_MAKE_SQLSTATE('2','2','0','1','V')

/**
 * 2201W [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid row count in fetch first clause <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ROW_COUNT_IN_FETCH_FIRST_CLAUSE   \
        STL_MAKE_SQLSTATE('2','2','0','1','W')

/**
 * 2201X [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid row count in result offset clause <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ROW_COUNT_IN_RESULT_OFFSET_CLAUSE   \
        STL_MAKE_SQLSTATE('2','2','0','1','X')

/**
 * 22021 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: character not in repertoire <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_CHARACTER_NOT_IN_REPERTOIRE   \
        STL_MAKE_SQLSTATE('2','2','0','2','1')

/**
 * 22022 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: indicator overflow <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INDICATOR_OVERFLOW   \
        STL_MAKE_SQLSTATE('2','2','0','2','2')

/**
 * 22023 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid parameter value <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_PARAMETER_VALUE   \
        STL_MAKE_SQLSTATE('2','2','0','2','3')

/**
 * 22024 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: unterminated C string <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_UNTERMINATED_C_STRING   \
        STL_MAKE_SQLSTATE('2','2','0','2','4')

/**
 * 22025 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: invalid escape sequence <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_ESCAPE_SEQUENCE   \
        STL_MAKE_SQLSTATE('2','2','0','2','5')

/**
 * 22026 [CLASS 22] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * data exception: string data, length mismatch <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_STRING_DATA_LENGTH_MISMATCH   \
        STL_MAKE_SQLSTATE('2','2','0','2','6')

/**
 * 22027 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: trim error <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_TRIM_ERROR   \
        STL_MAKE_SQLSTATE('2','2','0','2','7')

/**
 * 22029 [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: noncharacter in UCS string <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NONCHARACTER_IN_UCS_STRING   \
        STL_MAKE_SQLSTATE('2','2','0','2','9')

/**
 * 2202A [CLASS 22] [ISO/IEC 9075-4] <BR>
 * data exception: null value in field reference <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_IN_FIELD_REFERENCE   \
        STL_MAKE_SQLSTATE('2','2','0','2','A')

/**
 * 2202D [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: null value substituted for mutator subject parameter <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_NULL_VALUE_SUBSTITUTED_FOR_MUTATOR_SUBJECT_PARAMETER   \
        STL_MAKE_SQLSTATE('2','2','0','2','D')

/**
 * 2202E [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: array element error <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_ARRAY_ELEMENT_ERROR   \
        STL_MAKE_SQLSTATE('2','2','0','2','E')

/**
 * 2202F [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: array data, right truncation <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_ARRAY_DATA_RIGHT_TRUNCATION   \
        STL_MAKE_SQLSTATE('2','2','0','2','F')

/**
 * 2202G [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid repeat argument in a sample clause <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_REPEAT_ARGUMENT_IN_A_SAMPLE_CLAUSE   \
        STL_MAKE_SQLSTATE('2','2','0','2','G')

/**
 * 2202H [CLASS 22] [ISO/IEC 9075-2] <BR>
 * data exception: invalid sample size <BR>
 */
#define STL_EXT_ERRCODE_DATA_EXCEPTION_INVALID_SAMPLE_SIZE   \
        STL_MAKE_SQLSTATE('2','2','0','2','H')

/**
 * 23000 [CLASS 23] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * integrity constraint violation: general  <BR>
 */
#define STL_EXT_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','3','0','0','0')

/**
 * 23001 [CLASS 23] [ISO/IEC 9075-2] <BR>
 * integrity constraint violation: restrict violation  <BR>
 */
#define STL_EXT_ERRCODE_INTEGRITY_CONSTRAINT_VIOLATION_RESTRICT_VIOLATION   \
        STL_MAKE_SQLSTATE('2','3','0','0','1')

/**
 * 24000 [CLASS 24] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * invalid cursor state: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_CURSOR_STATE_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','4','0','0','0')

/**
 * 25000 [CLASS 25] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * invalid transaction state: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','5','0','0','0')

/**
 * 25001 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: active SQL-transaction  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_ACTIVE_SQL_TRANSACTION   \
        STL_MAKE_SQLSTATE('2','5','0','0','1')

/**
 * 25002 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: branch transaction already active  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_BRANCH_TRANSACTION_ALREADY_ACTIVE   \
        STL_MAKE_SQLSTATE('2','5','0','0','2')

/**
 * 25003 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: inappropriate access mode for branch transaction  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_INAPPROPRIATE_ACCESS_MODE_FOR_BRANCH_TRANSACTION   \
        STL_MAKE_SQLSTATE('2','5','0','0','3')

/**
 * 25004 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: inappropriate isolation level for branch transaction  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_INAPPROPRIATE_ISOLATION_LEVEL_FOR_BRANCH_TRANSACTION   \
        STL_MAKE_SQLSTATE('2','5','0','0','4')

/**
 * 25005 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: no active SQL-transaction for branch transaction  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_NO_ACTIVE_SQL_TRANSACTION_FOR_BRANCH_TRANSACTION   \
        STL_MAKE_SQLSTATE('2','5','0','0','5')

/**
 * 25006 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: read-only SQL-transaction  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_READ_ONLY_SQL_TRANSACTION   \
        STL_MAKE_SQLSTATE('2','5','0','0','6')

/**
 * 25007 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: schema and data statement mixing not supported  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_SCHEMA_AND_DATA_STATEMENT_MIXING_NOT_SUPPORTED   \
        STL_MAKE_SQLSTATE('2','5','0','0','7')

/**
 * 25008 [CLASS 25] [ISO/IEC 9075-2] <BR>
 * invalid transaction state: held cursor requires same isolation level  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_HELD_CURSOR_REQUIRES_SAME_ISOLATION_LEVEL   \
        STL_MAKE_SQLSTATE('2','5','0','0','8')

/**
 * 25S01 [CLASS 25] [ODBC-3] <BR>
 * invalid transaction state: transaction state  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_TRANSACTION_STATE   \
        STL_MAKE_SQLSTATE('2','5','S','0','1')

/**
 * 25S02 [CLASS 25] [ODBC-3] <BR>
 * invalid transaction state: transaction is still active  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_TRANSACTION_IS_STILL_ACTIVE   \
        STL_MAKE_SQLSTATE('2','5','S','0','2')

/**
 * 25S03 [CLASS 25] [ODBC-3] <BR>
 * invalid transaction state: transaction is rolled back  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_STATE_TRANSACTION_IS_ROLLED_BACK   \
        STL_MAKE_SQLSTATE('2','5','S','0','3')


/**
 * 26000 [CLASS 26] [ISO/IEC 9075-2] <BR>
 * invalid SQL statement name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_SQL_STATEMENT_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','6','0','0','0')

/**
 * 27000 [CLASS 27] [ISO/IEC 9075-2] <BR>
 * triggered data change violation: general <BR>
 */
#define STL_EXT_ERRCODE_TRIGGERED_DATA_CHANGE_VIOLATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','7','0','0','0')

/**
 * 27001 [CLASS 27] [ISO/IEC 9075-2] <BR>
 * triggered data change violation: modify table modified by data change delta table  <BR>
 */
#define STL_EXT_ERRCODE_TRIGGERED_DATA_CHANGE_VIOLATION_MODIFY_TABLE_MODIFIED_BY_DATA_CHANGE_DELTA_TABLE   \
        STL_MAKE_SQLSTATE('2','7','0','0','1')

/**
 * 28000 [CLASS 28] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * invalid authorization specification: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_AUTHORIZATION_SPECIFICATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','8','0','0','0')

/**
 * 2B000 [CLASS 2B] [ISO/IEC 9075-2] <BR>
 * dependent privilege descriptors: general <BR>
 */
#define STL_EXT_ERRCODE_DEPENDENT_PRIVILEGE_DESCRIPTORS_STILL_EXIST_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','B','0','0','0')

/**
 * 2C000 [CLASS 2C] [ISO/IEC 9075-2] <BR>
 * invalid character set name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_CHARACTER_SET_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','C','0','0','0')

/**
 * 2D000 [CLASS 2D] [ISO/IEC 9075-2] <BR>
 * invalid transaction termination: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_TRANSACTION_TERMINATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','D','0','0','0')

/**
 * 2E000 [CLASS 2E] [ISO/IEC 9075-2] <BR>
 * invalid connection name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_CONNECTION_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','E','0','0','0')

/**
 * 2F000 [CLASS 2F] [ISO/IEC 9075-2] <BR>
 * SQL routine exception: general <BR>
 */
#define STL_EXT_ERRCODE_SQL_ROUTINE_EXCEPTION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('2','F','0','0','0')

/**
 * 2F002 [CLASS 2F] [ISO/IEC 9075-2] <BR>
 * SQL routine exception: modifying SQL-data not permitted <BR>
 */
#define STL_EXT_ERRCODE_SQL_ROUTINE_EXCEPTION_MODIFYING_SQL_DATA_NOT_PERMITTED  \
        STL_MAKE_SQLSTATE('2','F','0','0','2')

/**
 * 2F003 [CLASS 2F] [ISO/IEC 9075-2] <BR>
 * SQL routine exception: prohibited SQL-statement attempted <BR>
 */
#define STL_EXT_ERRCODE_SQL_ROUTINE_EXCEPTION_PROHIBITED_SQL_STATEMENT_ATTEMPTED  \
        STL_MAKE_SQLSTATE('2','F','0','0','3')

/**
 * 2F004 [CLASS 2F] [ISO/IEC 9075-2] <BR>
 * SQL routine exception: reading SQL-data not permitted <BR>
 */
#define STL_EXT_ERRCODE_SQL_ROUTINE_EXCEPTION_READING_SQL_DATA_NOT_PERMITTED  \
        STL_MAKE_SQLSTATE('2','F','0','0','4')

/**
 * 2F005 [CLASS 2F] [ISO/IEC 9075-2] <BR>
 * SQL routine exception: function executed no return statement <BR>
 */
#define STL_EXT_ERRCODE_SQL_ROUTINE_EXCEPTION_FUNCTION_EXECUTED_NO_RETURN_STATEMENT  \
        STL_MAKE_SQLSTATE('2','F','0','0','5')


/**
 * 2H000 [CLASS 2H] [ISO/IEC 9075-2] <BR>
 * invalid collation name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_COLLATION_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('2','H','0','0','0')

/**
 * 30000 [CLASS 30] [ISO/IEC 9075-2] <BR>
 * invalid SQL statement identifier: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_SQL_STATEMENT_IDENTIFIER_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','0','0','0','0')

/**
 * 33000 [CLASS 33] [ISO/IEC 9075-2] <BR>
 * invalid SQL descriptor name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_SQL_DESCRIPTOR_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','3','0','0','0')

/**
 * 34000 [CLASS 34] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * invalid cursor name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_CURSOR_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','4','0','0','0')

/**
 * 35000 [CLASS 35] [ISO/IEC 9075-2] <BR>
 * invalid condition number: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_CONDITION_NUMBER_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','5','0','0','0')

/**
 * 36000 [CLASS 36] [ISO/IEC 9075-2] <BR>
 * cursor sensitivity exception: general <BR>
 */
#define STL_EXT_ERRCODE_CURSOR_SENSITIVITY_EXCEPTION_NO_SUBCLASS    \
        STL_MAKE_SQLSTATE('3','6','0','0','0')

/**
 * 36001 [CLASS 36] [ISO/IEC 9075-2] <BR>
 * cursor sensitivity exception: request rejected <BR>
 */
#define STL_EXT_ERRCODE_CURSOR_SENSITIVITY_EXCEPTION_REQUEST_REJECTED   \
        STL_MAKE_SQLSTATE('3','6','0','0','1')

/**
 * 36002 [CLASS 36] [ISO/IEC 9075-2] <BR>
 * cursor sensitivity exception: request failed <BR>
 */
#define STL_EXT_ERRCODE_CURSOR_SENSITIVITY_EXCEPTION_REQUEST_FAILED   \
        STL_MAKE_SQLSTATE('3','6','0','0','2')


/**
 * 38000 [CLASS 38] [ISO/IEC 9075-2] <BR>
 * external routine exception: general <BR>
 */
#define STL_EXT_ERRCODE_EXTERNAL_ROUTINE_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','8','0','0','0')

/**
 * 38001 [CLASS 38] [ISO/IEC 9075-2] <BR>
 * external routine exception: containing SQL not permitted <BR>
 */
#define STL_EXT_ERRCODE_EXTERNAL_ROUTINE_EXCEPTION_CONTAINING_SQL_NOT_PERMITTED   \
        STL_MAKE_SQLSTATE('3','8','0','0','1')

/**
 * 38002 [CLASS 38] [ISO/IEC 9075-2] <BR>
 * external routine exception: modifying SQL-data not permitted <BR>
 */
#define STL_EXT_ERRCODE_EXTERNAL_ROUTINE_EXCEPTION_MODIFYING_SQL_DATA_NOT_PERMITTED   \
        STL_MAKE_SQLSTATE('3','8','0','0','2')

/**
 * 38003 [CLASS 38] [ISO/IEC 9075-2] <BR>
 * external routine exception: prohibited SQL-statement attempted <BR>
 */
#define STL_EXT_ERRCODE_EXTERNAL_ROUTINE_EXCEPTION_PROHIBITED_SQL_STATEMENT_ATTEMPTED   \
        STL_MAKE_SQLSTATE('3','8','0','0','3')

/**
 * 38004 [CLASS 38] [ISO/IEC 9075-2] <BR>
 * external routine exception: reading SQL-data not permitted <BR>
 */
#define STL_EXT_ERRCODE_EXTERNAL_ROUTINE_EXCEPTION_READING_SQL_DATA_NOT_PERMITTED   \
        STL_MAKE_SQLSTATE('3','8','0','0','4')

/**
 * 39000 [CLASS 39] [ISO/IEC 9075-2] <BR>
 * external routine invocation exception: general  <BR>
 */
#define STL_EXT_ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','9','0','0','0')

/**
 * 39004 [CLASS 39] [ISO/IEC 9075-2] <BR>
 * external routine invocation exception: null value not allowed  <BR>
 */
#define STL_EXT_ERRCODE_EXTERNAL_ROUTINE_INVOCATION_EXCEPTION_NULL_VALUE_NOT_ALLOWED   \
        STL_MAKE_SQLSTATE('3','9','0','0','4')

/**
 * 3B000 [CLASS 3B] [ISO/IEC 9075-2] <BR>
 * savepoint exception: general <BR>
 */
#define STL_EXT_ERRCODE_SAVEPOINT_EXCEPTION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('3','B','0','0','0')

/**
 * 3B001 [CLASS 3B] [ISO/IEC 9075-2] <BR>
 * savepoint exception: invalid specification <BR>
 */
#define STL_EXT_ERRCODE_SAVEPOINT_EXCEPTION_INVALID_SPECIFICATION  \
        STL_MAKE_SQLSTATE('3','B','0','0','1')

/**
 * 3B002 [CLASS 3B] [ISO/IEC 9075-2] <BR>
 * savepoint exception: too many <BR>
 */
#define STL_EXT_ERRCODE_SAVEPOINT_EXCEPTION_TOO_MANY  \
        STL_MAKE_SQLSTATE('3','B','0','0','2')

/**
 * 3C000 [CLASS 3C] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * ambiguous cursor name: general <BR>
 */
#define STL_EXT_ERRCODE_AMBIGUOUS_CURSOR_NAME_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('3','C','0','0','0')

/**
 * 3D000 [CLASS 3D] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * invalid catalog name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_CATALOG_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','D','0','0','0')

/**
 * 3F000 [CLASS 3F] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * invalid schema name: general  <BR>
 */
#define STL_EXT_ERRCODE_INVALID_SCHEMA_NAME_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('3','F','0','0','0')

/**
 * 40000 [CLASS 40] [ISO/IEC 9075-2] <BR>
 * transaction rollback: general <BR>
 */
#define STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('4','0','0','0','0')

/**
 * 40001 [CLASS 40] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * transaction rollback: serialization failure <BR>
 */
#define STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_SERIALIZATION_FAILURE   \
        STL_MAKE_SQLSTATE('4','0','0','0','1')

/**
 * 40002 [CLASS 40] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * transaction rollback: integrity constraint violation <BR>
 */
#define STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_INTEGRITY_CONSTRAINT_VIOLATION   \
        STL_MAKE_SQLSTATE('4','0','0','0','2')

/**
 * 40003 [CLASS 40] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * transaction rollback: statement completion unknown <BR>
 */
#define STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_STATEMENT_COMPLETION_UNKNOWN   \
        STL_MAKE_SQLSTATE('4','0','0','0','3')

/**
 * 40004 [CLASS 40] [ISO/IEC 9075-2] <BR>
 * transaction rollback: triggered action exception <BR>
 */
#define STL_EXT_ERRCODE_TRANSACTION_ROLLBACK_TRIGGERED_ACTION_EXCEPTION   \
        STL_MAKE_SQLSTATE('4','0','0','0','4')

/**
 * 42000 [CLASS 42] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * syntax error or access rule violation: general <BR>
 */
#define STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('4','2','0','0','0')

/**
 * 42S01 [CLASS 42] [ODBC-3] <BR>
 * syntax error or access rule violation: base table or view already exists <BR>
 */
#define STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_BASE_TABLE_OR_VIEW_ALREADY_EXISTS  \
        STL_MAKE_SQLSTATE('4','2','S','0','1')

/**
 * 42S02 [CLASS 42] [ODBC-3] <BR>
 * syntax error or access rule violation: base table or view not found <BR>
 */
#define STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_BASE_TABLE_OR_VIEW_NOT_FOUND  \
        STL_MAKE_SQLSTATE('4','2','S','0','2')

/**
 * 42S11 [CLASS 42] [ODBC-3] <BR>
 * syntax error or access rule violation: index already exists <BR>
 */
#define STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_INDEX_ALREADY_EXISTS  \
        STL_MAKE_SQLSTATE('4','2','S','1','1')

/**
 * 42S12 [CLASS 42] [ODBC-3] <BR>
 * syntax error or access rule violation: index not found <BR>
 */
#define STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_INDEX_NOT_FOUND  \
        STL_MAKE_SQLSTATE('4','2','S','1','2')

/**
 * 42S21 [CLASS 42] [ODBC-3] <BR>
 * syntax error or access rule violation: column already exists <BR>
 */
#define STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_COLUMN_ALREADY_EXISTS  \
        STL_MAKE_SQLSTATE('4','2','S','2','1')

/**
 * 42S22 [CLASS 42] [ODBC-3] <BR>
 * syntax error or access rule violation: column not found <BR>
 */
#define STL_EXT_ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION_COLUMN_NOT_FOUND  \
        STL_MAKE_SQLSTATE('4','2','S','2','2')

/**
 * 44000 [CLASS 44] [ISO/IEC 9075-2] [ODBC-3] <BR>
 * with check option violation: general  <BR>
 */
#define STL_EXT_ERRCODE_WITH_CHECK_OPTION_VIOLATION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('4','4','0','0','0')

/**
 * 45000 [CLASS 45] [ISO/IEC 9075-4] [ISO/IEC 9075-5] <BR>
 * unhandled user-defined exception: general  <BR>
 */
#define STL_EXT_ERRCODE_UNHANDLED_USER_DEFINED_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('4','5','0','0','0')

/**
 * 46000 [CLASS 46] [ISO/IEC 9075-10] [ISO/IEC 9075-13] <BR>
 * OLB-specific error: general  <BR>
 * Java DDL: general <BR>
 * Java Execution: general <BR>
 */
#define STL_EXT_ERRCODE_OLB_SPECIFIC_ERROR_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('4','6','0','0','0')

/**
 * 46001 [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: invalid URL  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_INVALID_URL   \
        STL_MAKE_SQLSTATE('4','6','0','0','1')

/**
 * 46002 [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: invalid JAR name  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_INVALID_JAR_NAME   \
        STL_MAKE_SQLSTATE('4','6','0','0','2')

/**
 * 46003 [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: invalid class deletion  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_INVALID_CLASS_DELETION   \
        STL_MAKE_SQLSTATE('4','6','0','0','3')

/**
 * 46005 [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: invalid replacement  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_INVALID_REPLACEMENT   \
        STL_MAKE_SQLSTATE('4','6','0','0','5')

/**
 * 4600A [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: attempt to replace uninstalled JAR  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_ATTEMPT_TO_REPLACE_UNINSTALLED_JAR   \
        STL_MAKE_SQLSTATE('4','6','0','0','A')

/**
 * 4600B [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: attempt to remove uninstalled JAR  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_ATTEMPT_TO_REMOVE_UNINSTALLED_JAR   \
        STL_MAKE_SQLSTATE('4','6','0','0','B')

/**
 * 4600C [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: invalid JAR removal  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_INVALID_JAR_REMOVAL   \
        STL_MAKE_SQLSTATE('4','6','0','0','C')

/**
 * 4600D [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: invalid path  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_INVALID_PATH   \
        STL_MAKE_SQLSTATE('4','6','0','0','D')

/**
 * 4600E [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java DDL: self-referencing path  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_DDL_SELF_REFERENCING_PATH   \
        STL_MAKE_SQLSTATE('4','6','0','0','E')

/**
 * 46102 [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java execution: invalid JAR name in path  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_EXECUTION_INVALID_JAR_NAME_IN_PATH   \
        STL_MAKE_SQLSTATE('4','6','1','0','2')

/**
 * 46103 [CLASS 46] [ISO/IEC 9075-13] <BR>
 * Java execution: unresolved class name  <BR>
 */
#define STL_EXT_ERRCODE_JAVA_EXECUTION_UNRESOLVED_CLASS_NAME   \
        STL_MAKE_SQLSTATE('4','6','1','0','2')

/**
 * 46110 [CLASS 46] [ISO/IEC 9075-10] <BR>
 * OLB-specific error: unsupported feature  <BR>
 */
#define STL_EXT_ERRCODE_OLB_SPECIFIC_ERROR_UNSUPPORTED_FEATURE   \
        STL_MAKE_SQLSTATE('4','6','1','1','0')

/**
 * 46120 [CLASS 46] [ISO/IEC 9075-10] <BR>
 * OLB-specific error: invalid class declaration  <BR>
 */
#define STL_EXT_ERRCODE_OLB_SPECIFIC_ERROR_INVALID_CLASS_DECLARATION   \
        STL_MAKE_SQLSTATE('4','6','1','2','0')

/**
 * 46121 [CLASS 46] [ISO/IEC 9075-10] <BR>
 * OLB-specific error: invalid column name  <BR>
 */
#define STL_EXT_ERRCODE_OLB_SPECIFIC_ERROR_INVALID_COLUMN_NAME   \
        STL_MAKE_SQLSTATE('4','6','1','2','1')

/**
 * 46122 [CLASS 46] [ISO/IEC 9075-10] <BR>
 * OLB-specific error: invalid number of columns  <BR>
 */
#define STL_EXT_ERRCODE_OLB_SPECIFIC_ERROR_INVALID_NUMBER_OF_COLUMNS   \
        STL_MAKE_SQLSTATE('4','6','1','2','2')

/**
 * 46130 [CLASS 46] [ISO/IEC 9075-10] <BR>
 * OLB-specific error: invalid profile state  <BR>
 */
#define STL_EXT_ERRCODE_OLB_SPECIFIC_ERROR_INVALID_PROFILE_STATE   \
        STL_MAKE_SQLSTATE('4','6','1','3','0')

/**
 * HV000 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: general  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('H','V','0','0','0')

/**
 * HV001 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: memory allocation error  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_MEMORY_ALLOCATION_ERROR   \
        STL_MAKE_SQLSTATE('H','V','0','0','1')

/**
 * HV002 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: dynamic parameter value needed  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_DYNAMIC_PARAMETER_VALUE_NEEDED   \
        STL_MAKE_SQLSTATE('H','V','0','0','2')

/**
 * HV004 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid data type  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_DATA_TYPE   \
        STL_MAKE_SQLSTATE('H','V','0','0','4')

/**
 * HV005 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: column name not found  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_COLUMN_NAME_NOT_FOUND   \
        STL_MAKE_SQLSTATE('H','V','0','0','5')

/**
 * HV006 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid data type descriptors  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_DATA_TYPE_DESCRIPTORS   \
        STL_MAKE_SQLSTATE('H','V','0','0','6')

/**
 * HV007 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid column name  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_COLUMN_NAME   \
        STL_MAKE_SQLSTATE('H','V','0','0','7')

/**
 * HV008 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid column number  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_COLUMN_NUMBER   \
        STL_MAKE_SQLSTATE('H','V','0','0','8')

/**
 * HV009 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid use of null pointer <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_USE_OF_NULL_POINTER   \
        STL_MAKE_SQLSTATE('H','V','0','0','9')

/**
 * HV00A [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid string format  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_STRING_FORMAT   \
        STL_MAKE_SQLSTATE('H','V','0','0','A')

/**
 * HV00B [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid handle  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_HANDLE   \
        STL_MAKE_SQLSTATE('H','V','0','0','B')

/**
 * HV00C [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid option index  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_OPTION_INDEX   \
        STL_MAKE_SQLSTATE('H','V','0','0','C')

/**
 * HV00D [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid option name  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_OPTION_NAME   \
        STL_MAKE_SQLSTATE('H','V','0','0','D')

/**
 * HV00J [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: option name not found  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_OPTION_NAME_NOT_FOUND   \
        STL_MAKE_SQLSTATE('H','V','0','0','J')

/**
 * HV00K [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: reply handle  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_REPLY_HANDLE   \
        STL_MAKE_SQLSTATE('H','V','0','0','K')

/**
 * HV00L [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: unable to create execution  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_UNABLE_TO_CREATE_EXECUTION   \
        STL_MAKE_SQLSTATE('H','V','0','0','L')

/**
 * HV00M [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: unable to create reply  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_UNABLE_TO_CREATE_REPLY   \
        STL_MAKE_SQLSTATE('H','V','0','0','M')

/**
 * HV00N [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: unable to establish connection  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_UNABLE_TO_ESTABLISH_CONNECTION   \
        STL_MAKE_SQLSTATE('H','V','0','0','N')

/**
 * HV00P [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: no schemas  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_NO_SCHEMAS   \
        STL_MAKE_SQLSTATE('H','V','0','0','P')

/**
 * HV00Q [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: schema not found  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_SCHEMA_NOT_FOUND   \
        STL_MAKE_SQLSTATE('H','V','0','0','Q')

/**
 * HV00R [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: table not found  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_TABLE_NOT_FOUND   \
        STL_MAKE_SQLSTATE('H','V','0','0','R')

/**
 * HV010 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: function sequence error  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_FUNCTION_SEQUENCE_ERROR   \
        STL_MAKE_SQLSTATE('H','V','0','1','0')

/**
 * HV014 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: limit on number of handles exceeded  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_LIMIT_ON_NUMBER_OF_HANDLES_EXCEEDED   \
        STL_MAKE_SQLSTATE('H','V','0','1','4')

/**
 * HV021 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: inconsistent descriptor information  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INCONSISTENT_DESCRIPTOR_INFORMATION   \
        STL_MAKE_SQLSTATE('H','V','0','2','1')

/**
 * HV024 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid attribute value  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_ATTRIBUTE_VALUE   \
        STL_MAKE_SQLSTATE('H','V','0','2','4')

/**
 * HV090 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid string length or buffer length  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_STRING_LENGTH_OR_BUFFER_LENGTH   \
        STL_MAKE_SQLSTATE('H','V','0','9','0')

/**
 * HV091 [CLASS HV] [ISO/IEC 9075-9] <BR>
 * FDW-specific condition: invalid descriptor field identifier  <BR>
 */
#define STL_EXT_ERRCODE_FDW_SPECIFIC_CONDITION_INVALID_DESCRIPTOR_FIELD_IDENTIFIER   \
        STL_MAKE_SQLSTATE('H','V','0','9','1')



/**
 * HW000 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: general  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('H','W','0','0','0')

/**
 * HW001 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: external file not linked  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_EXTERNAL_FILE_NOT_LINKED   \
        STL_MAKE_SQLSTATE('H','W','0','0','1')

/**
 * HW002 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: external file already linked  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_EXTERNAL_FILE_ALREADY_LINKED   \
        STL_MAKE_SQLSTATE('H','W','0','0','2')

/**
 * HW003 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: referenced file does not exist  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_REFERENCED_FILE_DOES_NOT_EXIST   \
        STL_MAKE_SQLSTATE('H','W','0','0','3')

/**
 * HW004 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: invalid write token  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_INVALID_WRITE_TOKEN   \
        STL_MAKE_SQLSTATE('H','W','0','0','4')

/**
 * HW005 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: invalid datalink construction  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_INVALID_DATALINK_CONSTRUCTION   \
        STL_MAKE_SQLSTATE('H','W','0','0','5')

/**
 * HW006 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: invalid write permission for update  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_INVALID_WRITE_PERMISSION_FOR_UPDATE   \
        STL_MAKE_SQLSTATE('H','W','0','0','6')

/**
 * HW007 [CLASS HW] [ISO/IEC 9075-9] <BR>
 * datalink exception: referenced file not valid  <BR>
 */
#define STL_EXT_ERRCODE_DATALINK_EXCEPTION_REFERENCED_FILE_NOT_VALID   \
        STL_MAKE_SQLSTATE('H','W','0','0','7')

/**
 * HY000 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * general error <BR>
 */
#define STL_EXT_ERRCODE_GENERAL_ERROR_NO_SUBCLASS   \
        STL_MAKE_SQLSTATE('H','Y','0','0','0')

/**
 * HY001 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: memory allocation error  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_MEMORY_ALLOCATION_ERROR   \
        STL_MAKE_SQLSTATE('H','Y','0','0','1')

/**
 * HY003 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid data type in application descriptor  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_DATA_TYPE_IN_APPLICATION_DESCRIPTOR   \
        STL_MAKE_SQLSTATE('H','Y','0','0','3')

/**
 * HY004 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid data type  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_DATA_TYPE   \
        STL_MAKE_SQLSTATE('H','Y','0','0','4')

/**
 * HY007 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: associated statement is not prepared  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_ASSOCIATED_STATEMENT_IS_NOT_PREPARED   \
        STL_MAKE_SQLSTATE('H','Y','0','0','7')

/**
 * HY008 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: operation canceled  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPERATION_CANCELED   \
        STL_MAKE_SQLSTATE('H','Y','0','0','8')

/**
 * HY009 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid use of null pointer  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_USE_OF_NULL_POINTER   \
        STL_MAKE_SQLSTATE('H','Y','0','0','9')

/**
 * HY010 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: function sequence error  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_FUNCTION_SEQUENCE_ERROR   \
        STL_MAKE_SQLSTATE('H','Y','0','1','0')

/**
 * HY011 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: attribute cannot be set now <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_ATTRIBUTE_CANNOT_BE_SET_NOW   \
        STL_MAKE_SQLSTATE('H','Y','0','1','1')

/**
 * HY012 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid transaction operation code <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_TRANSACTION_OPERATION_CODE   \
        STL_MAKE_SQLSTATE('H','Y','0','1','2')

/**
 * HY013 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: memory management error <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_MEMORY_MANAGEMENT_ERROR   \
        STL_MAKE_SQLSTATE('H','Y','0','1','3')

/**
 * HY014 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: limit on number of handles exceeded <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_LIMIIT_ON_NUMBER_OF_HANDLES_EXCEEDED   \
        STL_MAKE_SQLSTATE('H','Y','0','1','4')

/**
 * HY015 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: no cursor name available <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_NO_CURSOR_NAME_AVAILABLE   \
        STL_MAKE_SQLSTATE('H','Y','0','1','5')

/**
 * HY016 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: cannot modify an implementation row descriptor <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_CANNOT_MODIFY_AN_IMPLEMENTATION_ROW_DESCRIPTROR   \
        STL_MAKE_SQLSTATE('H','Y','0','1','6')

/**
 * HY017 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid use of automatically allocated descriptor handle <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_USE_OF_AUTOMATICALLY_ALLOCATED_DESCRIPTOR_HANDLE   \
        STL_MAKE_SQLSTATE('H','Y','0','1','7')

/**
 * HY018 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: server declined the cancellation request <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_SERVER_DECLINED_THE_CANCELLATION_REQUEST   \
        STL_MAKE_SQLSTATE('H','Y','0','1','8')

/**
 * HY019 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: non-string data cannot be sent in pieces <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_NON_STRING_DATA_CANNOT_BE_SENT_IN_PIECES   \
        STL_MAKE_SQLSTATE('H','Y','0','1','9')

/**
 * HY020 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: attempt to concatenate a null value <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_ATTEMPT_TO_CONCATENATE_A_NULL_VALUE   \
        STL_MAKE_SQLSTATE('H','Y','0','2','0')

/**
 * HY021 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: inconsistent descriptor information  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INCONSISTENT_DESCRIPTOR_INFORMATION   \
    STL_MAKE_SQLSTATE('H','Y','0','2','1')

/**
 * HY024 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid attribute value  <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_ATTRIBUTE_VALUE   \
    STL_MAKE_SQLSTATE('H','Y','0','2','4')

/**
 * HY055 [CLASS HY] [ISO/IEC 9075-3] <BR>
 * CLI-specific condition: non-string data cannot be used with string routine <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_NON_STRING_DATA_CANNOT_BE_USED_WITH_STRING_ROUTINE   \
        STL_MAKE_SQLSTATE('H','Y','0','5','5')

/**
 * HY090 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid string length or buffer length <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_STRING_LENGTH_OR_BUFFER_LENGTH   \
        STL_MAKE_SQLSTATE('H','Y','0','9','0')

/**
 * HY091 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid descriptor field identifier <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_DESCRIPTOR_FIELD_IDENTIFIER   \
        STL_MAKE_SQLSTATE('H','Y','0','9','1')

/**
 * HY092 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid attribute identifier <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_ATTRIBUTE_IDENTIFIER   \
        STL_MAKE_SQLSTATE('H','Y','0','9','2')

/**
 * HY093 [CLASS HY] [ISO/IEC 9075-9] <BR>
 * CLI-specific condition: invalid datalink value <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_DATALINK_VALUE   \
        STL_MAKE_SQLSTATE('H','Y','0','9','3')

/**
 * HY095 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid FunctionId specified <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_FUNCTION_ID_SPECIFIED   \
        STL_MAKE_SQLSTATE('H','Y','0','9','5')

/**
 * HY096 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid information type <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_INFORMATION_TYPE   \
        STL_MAKE_SQLSTATE('H','Y','0','9','6')

/**
 * HY097 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: column type out of range <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_COLUMN_TYPE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('H','Y','0','9','7')

/**
 * HY098 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: scope out of range <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_SCOPE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('H','Y','0','9','8')

/**
 * HY099 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: nullable type out of range <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_NULLABLE_TYPE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('H','Y','0','9','9')

/**
 * HY100 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: uniqueness option type out of range <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_UNIQUENESS_OPTION_TYPE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('H','Y','1','0','0')

/**
 * HY101 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: accuracy option type out of range <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_ACCURACY_OPTION_TYPE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('H','Y','1','0','1')

/**
 * HY103 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid retrieval code <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_RETRIEVAL_CODE   \
        STL_MAKE_SQLSTATE('H','Y','1','0','3')

/**
 * HY104 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid Length/Precision value <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_LENGTH_PRECISION_VALUE   \
        STL_MAKE_SQLSTATE('H','Y','1','0','4')

/**
 * HY105 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid parameter mode <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_PARAMETER_MODE   \
        STL_MAKE_SQLSTATE('H','Y','1','0','5')

/**
 * HY106 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid fetch orientation <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_FETCH_ORIENTATION   \
        STL_MAKE_SQLSTATE('H','Y','1','0','6')

/**
 * HY107 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: row value out of range <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_ROW_VALUE_OUT_OF_RANGE   \
        STL_MAKE_SQLSTATE('H','Y','1','0','7')

/**
 * HY109 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: invalid cursor position <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_CURSOR_POSITION   \
        STL_MAKE_SQLSTATE('H','Y','1','0','9')

/**
 * HY110 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: invalid driver completion <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_DRIVER_COMPLETION   \
        STL_MAKE_SQLSTATE('H','Y','1','1','0')

/**
 * HY111 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: invalid bookmark value <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_INVALID_BOOKMARK_VALUE   \
        STL_MAKE_SQLSTATE('H','Y','1','1','1')

/**
 * HYC00 [CLASS HY] [ISO/IEC 9075-3] [ODBC-3] <BR>
 * CLI-specific condition: optional feature not implemented <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_OPTIONAL_FEATURE_NOT_IMPLEMENTED   \
        STL_MAKE_SQLSTATE('H','Y','C','0','0')

/**
 * HYT00 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: timeout expired <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_TIMEOUT_EXPIRED   \
        STL_MAKE_SQLSTATE('H','Y','T','0','0')

/**
 * HYT01 [CLASS HY] [ODBC-3] <BR>
 * CLI-specific condition: connection timeout expired <BR>
 */
#define STL_EXT_ERRCODE_CLI_SPECIFIC_CONDITION_CONNECTION_TIMEOUT_EXPIRED   \
        STL_MAKE_SQLSTATE('H','Y','T','0','1')

/**
 * HZ000 [CLASS HZ] [ISO/IEC 9075-2] [ISO/IEC 9579] <BR>
 * Remote Database Access: general <BR>
 */
#define STL_EXT_ERRCODE_REMOTE_DATABASE_ACCESS_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('H','Z','0','0','0')

/**
 * IM001 [CLASS IM] [ODBC-3] <BR>
 * Driver does not support this function <BR>
 */
#define STL_EXT_ERRCODE_DRIVER_DOES_NOT_SUPPORT_THIS_FUNCTION  \
        STL_MAKE_SQLSTATE('I','M','0','0','1')

/**
 * IM002 [CLASS IM] [ODBC-3] <BR>
 * Data source name not found and no default driver specified <BR>
 */
#define STL_EXT_ERRCODE_DATA_SOURCE_NAME_NOT_FOUND_AND_NO_DEFAULT_DRIVER_SPECIFIED  \
        STL_MAKE_SQLSTATE('I','M','0','0','2')

/**
 * IM003 [CLASS IM] [ODBC-3] <BR>
 * Specified driver could not be loaded <BR>
 */
#define STL_EXT_ERRCODE_SPECIFIED_DRIVER_COULD_NOT_BE_LOADED  \
        STL_MAKE_SQLSTATE('I','M','0','0','3')

/**
 * IM004 [CLASS IM] [ODBC-3] <BR>
 * Driver's SQLAllocHandle on SQL_HANDLE_ENV failed <BR>
 */
#define STL_EXT_ERRCODE_DRIVERS_SQLALLOCHANDLE_ON_SQL_HANDLE_ENV_FAILED  \
        STL_MAKE_SQLSTATE('I','M','0','0','4')

/**
 * IM005 [CLASS IM] [ODBC-3] <BR>
 * Driver's SQLAllocHandle on SQL_HANDLE_DBC failed <BR>
 */
#define STL_EXT_ERRCODE_DRIVERS_SQLALLOCHANDLE_ON_SQL_HANDLE_DBC_FAILED  \
        STL_MAKE_SQLSTATE('I','M','0','0','5')

/**
 * IM006 [CLASS IM] [ODBC-3] <BR>
 * Driver's SQLSetConnectAttr failed <BR>
 */
#define STL_EXT_ERRCODE_DRIVERS_SQLSETCONNECTATTR_FAILED  \
        STL_MAKE_SQLSTATE('I','M','0','0','6')

/**
 * IM007 [CLASS IM] [ODBC-3] <BR>
 * No data source or driver specified; dialog prohibited <BR>
 */
#define STL_EXT_ERRCODE_NO_DATA_SOURCE_OR_DRIVER_SPECIFIED_DIALOG_PROHIBITED  \
        STL_MAKE_SQLSTATE('I','M','0','0','7')

/**
 * IM008 [CLASS IM] [ODBC-3] <BR>
 * Dialog failed <BR>
 */
#define STL_EXT_ERRCODE_DIAGLOG_FAILED  \
        STL_MAKE_SQLSTATE('I','M','0','0','8')

/**
 * IM009 [CLASS IM] [ODBC-3] <BR>
 * Unable to load translation DLL <BR>
 */
#define STL_EXT_ERRCODE_UNABLE_TO_LOAD_TRNSLATION_DLL  \
        STL_MAKE_SQLSTATE('I','M','0','0','9')

/**
 * IM010 [CLASS IM] [ODBC-3] <BR>
 * Data source name too long <BR>
 */
#define STL_EXT_ERRCODE_DATA_SOURCE_NAME_TOO_LONG  \
        STL_MAKE_SQLSTATE('I','M','0','1','0')

/**
 * IM011 [CLASS IM] [ODBC-3] <BR>
 * Driver name too long <BR>
 */
#define STL_EXT_ERRCODE_DRIVER_NAME_TOO_LONG  \
        STL_MAKE_SQLSTATE('I','M','0','1','1')

/**
 * IM012 [CLASS IM] [ODBC-3] <BR>
 * DRIVER keyword syntax error <BR>
 */
#define STL_EXT_ERRCODE_DRIVER_KEYWORD_SYNTAX_ERROR  \
        STL_MAKE_SQLSTATE('I','M','0','1','2')

/**
 * IM013 [CLASS IM] [ODBC-3] <BR>
 * Trace file error <BR>
 */
#define STL_EXT_ERRCODE_TRACE_FILE_ERROR  \
        STL_MAKE_SQLSTATE('I','M','0','1','3')

/**
 * IM014 [CLASS IM] [ODBC-3] <BR>
 * Invalid name of File DSN <BR>
 */
#define STL_EXT_ERRCODE_INVALID_NAME_OF_FILE_DSN  \
        STL_MAKE_SQLSTATE('I','M','0','1','4')

/**
 * IM015 [CLASS IM] [ODBC-3] <BR>
 * Corrupt file data source <BR>
 */
#define STL_EXT_ERRCODE_CORRUPT_FILE_DATA_SOURCE  \
        STL_MAKE_SQLSTATE('I','M','0','1','5')

/**
 * RP000 [CLASS RP] [비표준] <BR>
 * Operating system resource exception: general <BR>
 */
#define STL_EXT_ERRCODE_OPERATING_SYSTEM_RESOURCE_EXCEPTION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('R','P','0','0','0')

/**
 * RD000 [CLASS RD] [비표준] <BR>
 * Database resource exception: general <BR>
 */
#define STL_EXT_ERRCODE_DATABASE_RESOURCE_EXCEPTION_NO_SUBCLASS  \
        STL_MAKE_SQLSTATE('R','D','0','0','0')

/** @} stlExternalCategoryX */

/** @} stlExternalSQLSTATECode */

/** @} stlExternalErrorCode */


/** @} */

#endif /* _STLSQLSTATE_H_ */
