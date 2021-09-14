/*******************************************************************************
 * dtfStringMisc.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfStringMisc.c 1398 2011-07-13 03:22:22Z ehpark $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file dtfStringMisc.c
 * @brief String Function Misc DataType Library Layer
 */

#include <dtl.h>

/**
 * @addtogroup dtfStringMisc String Misc
 * @ingroup dtf
 * @internal
 * @{
 */

/*******************************************************************************
 * ascii(string) 첫번째문자의 ASCII코드 반환.
 *   ascii(string) => [ P, M, O ] 
 * <BR>  ex) ascii('x')  =>  120
 *******************************************************************************/

/** 
 * @brief ascii(string)
 *        <BR> 첫번째문자의 ASCII코드 출력.
 *        <BR> ascii(string) 
 *        <BR>   ascii(string) => [ P, M, O ] 
 *        <BR> ex) ascii('x')  =>  120
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> ascii(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfAscii( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    void           * aEnv )
{
    
    return STL_SUCCESS;
}


/*******************************************************************************
 * 아래 세개의 convert 함수를 하나의 함수로 통합해서 처리할 수도 ...
 *******************************************************************************/
/*******************************************************************************
 * convert(string bytea, src_encoding name, dest_encoding name)
 *   convert(string bytea, src_encoding name, dest_encoding name) => [ P ]
 *   convert(expr, dest_char_set [, source_char_set]) => [ O ] 구문의 argument순서가 다름
 * <BR>    : src_encoding을 dest_encoding으로 변환 
 * <BR>  ex) convert('text_in_utf8', 'UTF8', 'UHC')
 *******************************************************************************/

/**
 * @brief convert(string, src_encoding, dest_encoding)
 *        <BR> src_encoding을 dest_encoding으로 변환
 *        <BR> convert(string bytea, src_encoding name, dest_encoding name)
 *        <BR>   convert(string bytea, src_encoding name, dest_encoding name) => [ P ]
 *        <BR>   convert(expr, dest_char_set [, source_char_set]) => [ O ] 구문의 argument순서가다름
 *        <BR> ex) convert('text_in_utf8', 'UTF8', 'UHC')
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> convert(VARBINARY, VARCHAR, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfConvert( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * convert_from(string, src_encoding)
 *   convert_from(string, src_encoding) => [ P ]
 * <BR>    : src_encoding을 database encoding으로 변환
 * <BR>  ex) convert('text_in_utf8', 'UTF8')
 *******************************************************************************/

/**
 * @brief convert_from(string, src_encoding)
 *        <BR> src_encoding을 database encoding으로 변환
 *        <BR> convert_from(string, src_encoding)
 *        <BR>   convert_from(string, src_encoding) => [ P ]
 *        <BR> ex) convert('text_in_utf8', 'UTF8')
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> convert_from(BARBINARY, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfConvertFrom( stlUInt16        aInputArgumentCnt,
                          dtlValueEntry  * aInputArgument,
                          void           * aResultValue,
                          void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * convert_to(string, dest_encoding)
 *   convert_to(string, dest_encoding) => [ P ]
 *   convert(expr, dest_char_set [, source_char_set]) => [ O ]
 *   convert(character value expression USING transcoding name)   => [ S, M ] 표준구문
 * <BR>    : string 을 dest_encoding으로 변환 
 * <BR>  ex) convert_to('some text', 'UTF8')
 *******************************************************************************/

/**
 * @brief convert_to(string, dest_encoding)
 *        <BR> string 을 dest_encoding으로 변환
 *        <BR> convert_to(string, dest_encoding)
 *        <BR>   convert_to(string, dest_encoding) => [ P ]
 *        <BR>   convert(expr, dest_char_set [, source_char_set]) => [ O ]
 *        <BR>   convert(character value expression USING transcoding name)   => [ S, M ] 표준구문
 *        <BR> ex) convert_to('some text', 'UTF8')
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> convert_to(VARCHAR, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfConvertTo( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 *   아래의 decode 함수는 O사의 decode함수와는 다른 의미임.
 *   O사에 지원하는 decode함수는 case구문으로도 대체할 수 있을 듯 하고,
 *   구문을 지원할 수도 ...
 *******************************************************************************/


/*******************************************************************************
 * decode(string, type)
 *   decode(string, type) => [ P, M ]  P, M사 다른 의미인 듯. ???
 *   O사의 decode함수와는 다른 의미임.
 * <BR>    : encode로 encode된 string을 binary data로 decode.
 * <BR>  ex) decode('MTIzAAE=', 'base64')  =>  123000001
 *******************************************************************************/

/**
 * @brief decode(string, type)
 *        <BR> encode로 encode된 string을 binary data로 decode.
 *        <BR> decode(string, type)
 *        <BR>   decode(string, type) => [ P, M ]  P, M사 다른 의미인 듯. ???
 *        <BR> O사의 decode함수와는 다른 의미임. 그럼, O사에서와 같은 decode 함수 지원은 ?
 *        <BR> O사에 지원하는 decode함수는 case구문으로도 대체할 수 있을 듯 하고,
 *        <BR> 구문을 지원할 수도 ...
 *        <BR> ex) decode('MTIzAAE=', 'base64')  =>  123000001
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> decode(VARCHAR, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfDecode( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aEnv )
{
    /**
     * aType의 종류는 ???????
     */
    
    return STL_SUCCESS;
}


/*******************************************************************************
 * encode(data, type)
 *   encode(data, type) => [ P, M ] P, M사 다른 의미인 듯. ??? 
 * <BR>    : binary data를 다른 표현방식으로 encode.
 * <BR>  ex) encode(E'123\\000\\001', 'base64')  =>  MTIzAAE= 
 *******************************************************************************/

/**
 * @brief  encode(data, type)
 *         <BR> binary data를 다른 표현방식으로 encode.
 *         <BR> encode(data, type)
 *         <BR>   encode(data, type) => [ P, M ] P, M사 다른 의미인 듯. ??? 
 *         <BR> ex) encode(E'123\\000\\001', 'base64')  =>  MTIzAAE= 
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> encode(VARBINARY, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfEncode( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aEnv )
{
    /**
     * aType의 종류는 ???????
     */
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * length(string)
 * <BR>    : string내의 문자수
 * <BR>  ex) length('jose')  =>  4
 *******************************************************************************/

/**
 * @brief  length(string)
 *         <BR> string내의 문자수
 *         <BR> length(string)
 *         <BR> ex) length('jose')  =>  4
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> length(VARCHAR)
 * @param[out] aResultValue     연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv             environment (stlErrorStack)
 */
stlStatus dtfStringLength( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    /*
     * 동일한 기능을 지원하는 dtfCharLength()를 호출하거나
     * parsing/validation단계에서 함수포인터 연결시 바로 연결해 주거나,
     * length(string)함수는 byte length를 반환하는 함수로 정의하거나 ...
     */
    
    return STL_SUCCESS;
}

/**
 * @brief  length(string)
 *         <BR>    : string내의 문자수 binarystring의 length
 *         <BR> length(string)
 *         <BR> ex) length('jose')  =>  4
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> length(VARBINARY)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringLength( stlUInt16        aInputArgumentCnt,
                                 dtlValueEntry  * aInputArgument,
                                 void           * aResultValue,
                                 void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * length(string, encoding)
 *   length(string, encoding) => [ P ]
 * <BR>    : encoding내 string에서의 문자수 반환.
 * <BR>  ex) length('jose', 'UTF8')  =>  4
 *******************************************************************************/

/**
 * @brief  length(string, encoding)
 *         <BR> encoding내 string에서의 문자수 반환.
 *         <BR> length(string, encoding)
 *         <BR>   length(string, encoding) => [ P ]
 *         <BR> ex) length('jose', 'UTF8')  =>  4
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> length(VARBINARY, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfLengthInEncoding( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aEnv )
{
    
    return STL_SUCCESS;
}


/*******************************************************************************
 * md5(string)
 *   md5(string) => [ P, M ]
 * <BR>    : string 의 MD5 hash를 계산해 16진수로 변환
 * <BR>  ex) md5('abc')  =>  900150983cd24fb0 d6963f7d28e17f72
 *******************************************************************************/

/**
 * @brief  md5(string)
 *         <BR> string 의 MD5 hash를 계산해 16진수로 변환
 *         <BR> md5(string)
 *         <BR>   md5(string) => [ P, M ]
 *         <BR> ex) md5('abc')  =>  900150983cd24fb0 d6963f7d28e17f72
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> md5VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfStringMd5( stlUInt16        aInputArgumentCnt,
                        dtlValueEntry  * aInputArgument,
                        void           * aResultValue,
                        void           * aEnv )
{
    
    return STL_SUCCESS;
}

/**
 * @brief  md5(string)
 *         <BR> string 의 MD5 hash를 계산해 16진수로 변환
 *         <BR> md5(string)
 *         <BR>   md5(string) => [ P, M ]
 *         <BR> ex) md5('abc')  =>  900150983cd24fb0 d6963f7d28e17f72
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> md5(VARBINARY)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBinaryStringMd5( stlUInt16        aInputArgumentCnt,
                              dtlValueEntry  * aInputArgument,
                              void           * aResultValue,
                              void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * quote_ident(string)
 *   quote_ident(string) => [ P ]
 * <BR>    : SQL문의 string내에서 식별자로 사용할 수 있도록 적절한 인용부호를 붙여 반환.
 * <BR>  ex) quote_ident('Foo bar')  =>  "Foo bar"
 *******************************************************************************/

/**
 * @brief  quote_ident(string)
 *         <BR> SQL문의 string내에서 식별자로 사용할 수 있도록 적절한 인용부호를 붙여 반환.
 *         <BR> quote_ident(string)
 *         <BR>   quote_ident(string) => [ P ]
 *         <BR> ex) quote_ident('Foo bar')  =>  "Foo bar"
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> quote_ident(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfQuoteIdent( stlUInt16        aInputArgumentCnt,
                         dtlValueEntry  * aInputArgument,
                         void           * aResultValue,
                         void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * quote_literal(string)
 *   quote_literal(string) => [ P ]
 *   quote(str) => [ M ] 와 유사한 듯. 
 * <BR>    : SQL문의 string내에서 literal로 사용할 수 있도록 적절한 인용부호를 붙여 반환.
 * <BR>  ex) quote_literal('O\'Reilly')  =>  'O''Reilly'
 *******************************************************************************/

/**
 * @brief  quote_literal(string)
 *         <BR> SQL문의 string내에서 literal로 사용할 수 있도록 적절한 인용부호를 붙여 반환.
 *         <BR> quote_literal(string)
 *         <BR>   quote_literal(string) => [ P ]
 *         <BR>   quote(str) => [ M ] 와 유사한 듯. 
 *         <BR> ex) quote_literal('O\'Reilly')  =>  'O''Reilly'
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> quote_literal(VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfQuoteLiteral( stlUInt16        aInputArgumentCnt,
                           dtlValueEntry  * aInputArgument,
                           void           * aResultValue,
                           void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * quote_nullable(string)
 *   quote_nullable(string) => [ P ]
 *   quote(str) => [ M ] 와 유사한 듯. 
 * <BR>    : SQL문의 string내에서 literal로 사용할 수 있도록 적절한 인용부호를 붙여 반환.
 * <BR>  ex) quote_nullable(NULL)  =>  NULL
 *******************************************************************************/

/**
 * @brief  quote_nullable(string)
 *         <BR> SQL문의 string내에서 literal로 사용할 수 있도록 적절한 인용부호를 붙여 반환.
 *         <BR> quote_nullable(string)
 *         <BR>   quote_nullable(string) => [ P ]
 *         <BR>   quote(str) => [ M ] 와 유사한 듯. 
 *         <BR> ex) quote_nullable(NULL)  =>  NULL
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> quote_nullable(VARCHAR) 
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfQuoteNullable( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * regexp_matches(string, pattern [, flags])
 *   regexp_matches(string, pattern [, flags]) => [ P ]
 * <BR>    : POSIX 정규표현에 일치하는 모든 substring 반환
 * <BR>  ex) regexp_matches('foobarbequebaz', '(bar)(beque)')  =>  {bar,beque}
 *******************************************************************************/

/**
 * @brief  regexp_matches(string, pattern [, flags])
 *         <BR> POSIX 정규표현에 일치하는 모든 substring 반환
 *         <BR> regexp_matches(string, pattern [, flags])
 *         <BR>   regexp_matches(string, pattern [, flags]) => [ P ]
 *         <BR> ex) regexp_matches('foobarbequebaz', '(bar)(beque)')  =>  {bar,beque}
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> regexp_matches(VARCHAR) 
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfRegexpMatches( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    /**
     * ????????????????????????????
     * 인터페이스를 좀더 상세히 재설계 ...
     * 현재 지원하기로 한 데이타타입 범위에서 구현가능???
     */
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * regexp_split_to_table(string, pattern [, flags ])
 *   regexp_split_to_table(string, pattern [, flags ]) => [ P ]
 * <BR>    : 구분자로 POSIX 정규표현을 이용하여 string 분리 
 * <BR>  ex) regexp_split_to_table('hello world', E'\\s+')  =>  hello
 *                                                              world
 *                                                              (2 rows)
 *******************************************************************************/

/**
 * @brief  regexp_split_to_table(string, pattern [, flags ])
 *         <BR> 구분자로 POSIX 정규표현을 이용하여 string 분리
 *         <BR> regexp_split_to_table(string, pattern [, flags ])
 *         <BR>   regexp_split_to_table(string, pattern [, flags ]) => [ P ]
 *         <BR> ex) regexp_split_to_table('hello world', E'\\s+')  =>  hello
 *         <BR>                                                        world
 *         <BR>                                                      (2 rows)
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> regexp_split_to_table(VARCHAR, VARCHAR, VARCHAR)
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfRegexpReplace( stlUInt16        aInputArgumentCnt,
                            dtlValueEntry  * aInputArgument,
                            void           * aResultValue,
                            void           * aEnv )
{
    /**
     * ????????????????????????????
     * 인터페이스를 좀더 상세히 재설계 ...
     * 현재 지원하기로 한 데이타타입 범위에서 구현가능???
     */
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * to_hex(number int or bigint) , hex(n)
 *   to_hex(number int or bigint) => [ P ]
 *   hex(n)                       => [ M ]
 * <BR>    : 16진수로 변환 
 * <BR>  ex) to_hex(2147483647)  =>  7fffffff
 *******************************************************************************/

/**
 * @brief  to_hex(number int or bigint)
 *         <BR> 16진수로 변환
 *         <BR> to_hex(number int or bigint) , hex(n)
 *         <BR>   to_hex(number int or bigint) => [ P ]
 *         <BR>   hex(n)                       => [ M ]
 *         <BR> ex) to_hex(2147483647)  =>  7fffffff
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> to_hex(INTEGER) 
 * @param[out] aResultValue      연산 결과 (결과타입 VARCHAR)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfToHex( stlUInt16        aInputArgumentCnt,
                    dtlValueEntry  * aInputArgument,
                    void           * aResultValue,
                    void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * get_bit(string, offset)  ?????
 *   get_bit(string, offset) => [ P ]
 * <BR>    : string으로부터 offset의 bit 추출 
 * <BR>  ex) get_bit(E'Th\\000omas'::bytea, 45)  =>  1
 *******************************************************************************/

/**
 * @brief  get_bit(string, offset)
 *         <BR> string으로부터 offset의 bit 추출
 *         <BR> get_bit(string, offset)  ?????
 *         <BR>   get_bit(string, offset) => [ P ]
 *         <BR> ex) get_bit(E'Th\\000omas'\::bytea, 45)  =>  1
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> get_bit(VARBINARY, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfGetBit( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * get_byte(string, offset)  ?????
 *   get_byte(string, offset) => [ P ]
 * <BR>    : string으로부터 offset의 byte 추출 
 * <BR>  ex) get_byte(E'Th\\000omas'::bytea, 4)  =>  109
 *******************************************************************************/

/**
 * @brief  get_byte(string, offset)  ?????
 *         <BR> string으로부터 offset의 byte 추출
 *         <BR> get_byte(string, offset)  ?????
 *         <BR>   get_byte(string, offset) => [ P ]
 *         <BR> ex) get_byte(E'Th\\000omas'\::bytea, 4)  =>  109
 *         
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> get_byte(VARBINARY, INTEGER)
 * @param[out] aResultValue      연산 결과 (결과타입 INTEGER)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfGetByte( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * set_bit(string, offset, newvalue) ?????
 *   set_bit(string, offset, newvalue) => [ P ]
 * <BR>  ex) set_bit(E'Th\\000omas'::bytea, 45, 0)  =>  Th\000omAs
 *******************************************************************************/

/**
 * @brief  set_bit(string, offset, newvalue) ?????
 *         <BR> set_bit(string, offset, newvalue) ?????
 *         <BR>   set_bit(string, offset, newvalue) => [ P ]
 *         <BR> ex) set_bit(E'Th\\000omas'\::bytea, 45, 0)  =>  Th000omAs
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> set_bit(VARBINARY, INTEGER, VARBINARY)        
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfSetBit( stlUInt16        aInputArgumentCnt,
                     dtlValueEntry  * aInputArgument,
                     void           * aResultValue,
                     void           * aEnv )
{
    
    return STL_SUCCESS;
}

/*******************************************************************************
 * set_byte(string, offset, newvalue) ?????
 *   set_byte(string, offset, newvalue) => [ P ]
 * <BR>  ex) set_byte(E'Th\\000omas'\::bytea, 4, 65)  =>  Th000oAas
 *******************************************************************************/

/**
 * @brief  set_byte(string, offset, newvalue) ?????
 *         <BR> set_byte(string, offset, newvalue) ?????
 *         <BR>   set_byte(string, offset, newvalue) => [ P ]
 *         <BR> ex) set_byte(E'Th\\000omas'\::bytea, 4, 65)  =>  Th000oAas
 * 
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> set_byte(VARBINARY, INTEGER, VARBINRY)
 * @param[out] aResultValue      연산 결과 (결과타입 VARBINARY)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfSetByte( stlUInt16        aInputArgumentCnt,
                      dtlValueEntry  * aInputArgument,
                      void           * aResultValue,
                      void           * aEnv )
{
    
    return STL_SUCCESS;
}



/** @} */
