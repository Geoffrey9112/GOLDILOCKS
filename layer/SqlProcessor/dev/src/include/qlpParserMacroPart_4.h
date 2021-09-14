/*******************************************************************************
 * qlpParserMacroPart_4.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        vId: qlpParserMacroPart_4.h 2754 2011-12-15 02:27:23Z jhkim v
 *
 * NOTES
 *
 *
 ******************************************************************************/

#ifndef _QLPPARSERMACROPART_4_H_
#define _QLPPARSERMACROPART_4_H_ 1

/**
 * @file qlpParserMacroPart_4.h
 * @brief SQL Processor Layer Parser macro Part #4
 */

#include <qlpParse.h>

/**
 * @addtogroup qlpParser
 * @{
 */


/**
 * sequence_function : qlpFunction
 */

#define STL_PARSER_SEQUENCE_FUNCTION____CURRVAL( )                              \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpObjectName * sObjName =  qlpMakeObjectName( param, v3 );             \
        sObjName->mType = QLL_SEQUENCE_NAME_TYPE;                               \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                sObjName ) );                   \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakePseudoColumn( param,                      \
                                                    KNL_PSEUDO_COL_CURRVAL,     \
                                                    a1,                         \
                                                    QLP_GET_LAST_POS(a1) - a1,  \
                                                    sList ) );                  \
    }

#define STL_PARSER_SEQUENCE_FUNCTION____NEXTVAL( )                              \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpObjectName * sObjName =  qlpMakeObjectName( param, v3 );             \
        sObjName->mType = QLL_SEQUENCE_NAME_TYPE;                               \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                sObjName ) );                   \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakePseudoColumn( param,                      \
                                                    KNL_PSEUDO_COL_NEXTVAL,     \
                                                    a1,                         \
                                                    QLP_GET_LAST_POS(a1) - a1,  \
                                                    sList ) );                  \
    }

#define STL_PARSER_SEQUENCE_FUNCTION____NEXT_VALUE_FOR( )                       \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpObjectName * sObjName =  qlpMakeObjectName( param, v4 );             \
        sObjName->mType = QLL_SEQUENCE_NAME_TYPE;                               \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                sObjName ) );                   \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakePseudoColumn( param,                      \
                                                    KNL_PSEUDO_COL_NEXTVAL,     \
                                                    a1,                         \
                                                    QLP_GET_LAST_POS(a1) - a1,  \
                                                    sList ) );                  \
    }

/**
 * numeric_value_function : qlpFunction
 */

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ABSOLUTE( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____POSITIVE( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____NEGATIVE( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ADDITION( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SUBTRACTION( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____MULTIPLICATION( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____DIVISION( )    \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____DEGREES( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____FACTORIAL( )   \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____LOG( ) \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____PI( )  \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____RADIANS( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____RANDOM( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROUND( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SIGN( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TRUNC( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____MODULUS( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____NATURAL_LOGARITHM( )       \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____EXPONENTIAL( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____POWER( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____CUBE_ROOT( )   \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SQUARE_ROOT( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____FLOOR( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____CEILING( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____WIDTH_BUCKET( )    \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ACOS( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ASIN( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ATAN( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ATAN2( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____COS( ) \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____COT( ) \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SIN( ) \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TAN( ) \
    {                                               \
        STL_PARSER_CHECK( v0 = v1 );                \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITAND( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITNOT( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITOR( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BITXOR( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SHIFT_LEFT( )  \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____SHIFT_RIGHT( ) \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____BIT_LENGTH( )  \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____CHAR_LENGTH( ) \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____OCTET_LENGTH( )    \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____POSITION( )    \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____INSTR( )    \
    {                                                    \
        STL_PARSER_CHECK( v0 = v1 );                     \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____EXTRACT( )    \
    {                                                       \
        STL_PARSER_CHECK( v0 = v1 );                        \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_OBJECT_ID()  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_PAGE_ID()  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_ROW_NUMBER()    \
    {                                                              \
        STL_PARSER_CHECK( v0 = v1 );                               \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____ROWID_TABLESPACE_ID()    \
    {                                                                 \
        STL_PARSER_CHECK( v0 = v1 );                                  \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TO_NUMBER()              \
    {                                                                 \
        STL_PARSER_CHECK( v0 = v1 );                                  \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TO_NATIVE_REAL()         \
    {                                                                 \
        STL_PARSER_CHECK( v0 = v1 );                                  \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____TO_NATIVE_DOUBLE()       \
    {                                                                 \
        STL_PARSER_CHECK( v0 = v1 );                                  \
    }

#define STL_PARSER_NUMERIC_VALUE_FUNCTION____STATEMENT_VIEW_SCN()     \
    {                                                                 \
        STL_PARSER_CHECK( v0 = v1 );                                  \
    }


/**
 * absolute_value_function : qlpFunction
 */
#define STL_PARSER_ABSOLUTE_FUNCTION____( )                             \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ABS,   \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * positive_value_function
 */
#define STL_PARSER_POSITIVE_FUNCTION____( )                                 \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_POSITIVE,  \
                                                a1,                         \
                                                a4 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * negative_value_function
 */
#define STL_PARSER_NEGATIVE_FUNCTION____( )                                 \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_NEGATIVE,  \
                                                a1,                         \
                                                a4 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * addition_value_function
 */
#define STL_PARSER_ADDITION_FUNCTION____( )                             \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ADD,   \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * subtraction_value_function
 */
#define STL_PARSER_SUBTRACTION_FUNCTION____( )                          \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_SUB,   \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * multiplication_value_function
 */
#define STL_PARSER_MULTIPLICATION_FUNCTION____( )                       \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_MUL,   \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * division_value_function
 */
#define STL_PARSER_DIVISION_FUNCTION____( )                             \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_DIV,   \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * degrees_value_function
 */
#define STL_PARSER_DEGREES_FUNCTION____( )                                  \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_DEGREES,   \
                                                a1,                         \
                                                a4 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * factorial_value_function
 */
#define STL_PARSER_FACTORIAL_FUNCTION____( )                                \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_FACTORIAL, \
                                                a1,                         \
                                                a4 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * log10_value_function
 */
#define STL_PARSER_LOG10_FUNCTION____( )                                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_LOG10, \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * log_value_function
 */
#define STL_PARSER_LOG_FUNCTION____( )                                  \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_LOG,   \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * pi_value_function
 */
#define STL_PARSER_PI_FUNCTION____( )                                   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_PI,    \
                                                a1,                     \
                                                a3 - a1 + 1,            \
                                                NULL ) );               \
    }

/**
 * radians_value_function
 */
#define STL_PARSER_RADIANS_FUNCTION____( )                                  \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_RADIANS,   \
                                                a1,                         \
                                                a4 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * random_value_function
 */
#define STL_PARSER_RANDOM_FUNCTION____( )                                   \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_RANDOM,    \
                                                a1,                         \
                                                a6 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * round_integer_function
 */
#define STL_PARSER_ROUND_INTEGER_FUNCTION____( )                          \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ROUND, \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * round_scale_function
 */
#define STL_PARSER_ROUND_SCALE_FUNCTION____( )                          \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ROUND, \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * sign_function
 */
#define STL_PARSER_SIGN_FUNCTION____( )                                 \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_SIGN,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * trunc_integer_function
 */
#define STL_PARSER_TRUNC_INTEGER_FUNCTION____( )                        \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_TRUNC, \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * trunc_scale_function
 */
#define STL_PARSER_TRUNC_SCALE_FUNCTION____( )                          \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_TRUNC, \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * modulus_value_function : qlpFunction
 */
#define STL_PARSER_MODULUS_FUNCTION____( )                              \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_MOD,   \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }


/**
 * natural_logarithm : qlpFunction
 */

#define STL_PARSER_NATURAL_LOGARITHM____( )                             \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_LN,    \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }


/**
 * exponential_function : qlpFunction
 */

#define STL_PARSER_EXPONENTIAL_FUNCTION____( )                          \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_EXP,   \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }


/**
 * power_function : qlpFunction
 */

#define STL_PARSER_POWER_FUNCTION____( )                                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_POWER, \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * cube_root : qlpFunction
 */

#define STL_PARSER_CUBE_ROOT____( )                                     \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_CBRT,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * square_root : qlpFunction
 */

#define STL_PARSER_SQUARE_ROOT____( )                                   \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_SQRT,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }


/**
 * floor_function : qlpFunction
 */

#define STL_PARSER_FLOOR_FUNCTION____( )                                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_FLOOR, \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }


/**
 * ceiling_function : qlpFunction
 */

#define STL_PARSER_CEILING_FUNCTION____( )                              \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_CEIL,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * width_bucket_function
 */
#define STL_PARSER_WIDTH_BUCKET_FUNCTION____( )                                 \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v9 ) );                         \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_WIDTHBUCKET,   \
                                                a1,                             \
                                                a10 - a1 + 1,                   \
                                                sList ) );                      \
    }

/**
 * acos_function
 */
#define STL_PARSER_ACOS_FUNCTION____( )                                 \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ACOS,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * asin_function
 */
#define STL_PARSER_ASIN_FUNCTION____( )                                 \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ASIN,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * atan_function
 */
#define STL_PARSER_ATAN_FUNCTION____( )                                 \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ATAN,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * atan2_function
 */
#define STL_PARSER_ATAN2_FUNCTION____( )                                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_ATAN2, \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * cos_function
 */
#define STL_PARSER_COS_FUNCTION____( )                                  \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_COS,   \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * cot_function
 */
#define STL_PARSER_COT_FUNCTION____( )                                  \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_COT,   \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * sin_function
 */
#define STL_PARSER_SIN_FUNCTION____( )                                  \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_SIN,   \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * tan_function
 */
#define STL_PARSER_TAN_FUNCTION____( )                                  \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_TAN,   \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * bitand_function
 */
#define STL_PARSER_BITAND_FUNCTION____( )                                       \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_BITWISE_AND,   \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * bitnot_function
 */
#define STL_PARSER_BITNOT_FUNCTION____( )                                       \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_BITWISE_NOT,   \
                                                a1,                             \
                                                a4 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * bitor_function
 */
#define STL_PARSER_BITOR_FUNCTION____( )                                        \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_BITWISE_OR,    \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * bitxor_function
 */
#define STL_PARSER_BITXOR_FUNCTION____( )                                       \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_BITWISE_XOR,   \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * shift_left_function
 */
#define STL_PARSER_SHIFT_LEFT_FUNCTION____( )                                       \
    {                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                v3 ) );                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                v5 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_BITWISE_SHIFTLEFT, \
                                                a1,                                 \
                                                a6 - a1 + 1,                        \
                                                sList ) );                          \
    }

/**
 * shift_right_function
 */
#define STL_PARSER_SHIFT_RIGHT_FUNCTION____( )                                          \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v5 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_BITWISE_SHIFTRIGHT,    \
                                                a1,                                     \
                                                a6 - a1 + 1,                            \
                                                sList ) );                              \
    }

/**
 * bit_length_function
 */
#define STL_PARSER_BIT_LENGTH_FUNCTION____( )                               \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_BITLENGTH, \
                                                a1,                         \
                                                a4 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * char_length_function
 */
#define STL_PARSER_CHAR_LENGTH_FUNCTION____( )                                  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_CHARLENGTH,    \
                                                a1,                             \
                                                a4 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * octet_length_function
 */
#define STL_PARSER_OCTET_LENGTH_FUNCTION____( )                                 \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_OCTETLENGTH,   \
                                                a1,                             \
                                                a4 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * position_function
 */
#define STL_PARSER_POSITION_FUNCTION____( )                                 \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_POSITION,  \
                                                a1,                         \
                                                a6 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * instr_function
 */
#define STL_PARSER_INSTR_FUNCTION____DEFAULT( )                                 \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_INSTR,         \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a6 ) - a1,    \
                                                sList ) );                      \
    }

#define STL_PARSER_INSTR_FUNCTION____POSITION( )                                \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_INSTR,         \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a8 ) - a1,    \
                                                sList ) );                      \
    }

#define STL_PARSER_INSTR_FUNCTION____OCCURRENCE( )                              \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v9 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_INSTR,         \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a10 ) - a1,   \
                                                sList ) );                      \
    }


/**
 * rowid_object_id_value_function
 */
#define STL_PARSER_ROWID_OBJECT_ID_FUNCTION____( )                                  \
    {                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_ROWID_OBJECT_ID,   \
                                                a1,                                 \
                                                a4 - a1 + 1,                        \
                                                sList ) );                          \
    }

/**
 * rowid_page_id_value_function
 */
#define STL_PARSER_ROWID_PAGE_ID_FUNCTION____( )                                    \
    {                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_ROWID_PAGE_ID,     \
                                                a1,                                 \
                                                a4 - a1 + 1,                        \
                                                sList ) );                          \
    }

/**
 * rowid_row_number_value_function
 */
#define STL_PARSER_ROWID_ROW_NUMBER_FUNCTION____( )                                 \
    {                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_ROWID_ROW_NUMBER,  \
                                                a1,                                 \
                                                a4 - a1 + 1,                        \
                                                sList ) );                          \
    }

/**
 * rowid_tablespace_id_value_function
 */
#define STL_PARSER_ROWID_TABLESPACE_ID_FUNCTION____( )                                 \
    {                                                                                  \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                 \
                                                sList,                                 \
                                                v3 ) );                                \
                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                 \
                                                KNL_BUILTIN_FUNC_ROWID_TABLESPACE_ID,  \
                                                a1,                                    \
                                                a4 - a1 + 1,                           \
                                                sList ) );                             \
    }

/**
 * to_number_value_function
 */
#define STL_PARSER_TO_NUMBER_FUNCTION____DEFAULT( )                                   \
    {                                                                                 \
        qlpList     * sToNumberArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
                                                                                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                \
                                                sToNumberArgList,                     \
                                                v3 ) );                               \
                                                                                      \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                \
                                                KNL_BUILTIN_FUNC_TO_NUMBER,           \
                                                a1,                                   \
                                                QLP_GET_LAST_POS( a4 ) - a1,          \
                                                sToNumberArgList ) );                 \
    }

#define STL_PARSER_TO_NUMBER_FUNCTION____( )                                \
    {                                                                       \
        qlpList     * sToNumberArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpFunction * sFunc   = NULL;                                                 \
                                                                                      \
                                                                                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                \
                                                sFormatArgList,                       \
                                                v5 ) );                               \
                                                                                      \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                             \
                                                   KNL_BUILTIN_FUNC_TO_NUMBER_FORMAT, \
                                                   a5,                                \
                                                   QLP_GET_LAST_POS( a5 ) - a5,       \
                                                   sFormatArgList ) );                \
                                                                                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                \
                                                sToNumberArgList,                     \
                                                v3 ) );                               \
                                                                                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                \
                                                sToNumberArgList,                     \
                                                sFunc ) );                            \
                                                                                      \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                \
                                                KNL_BUILTIN_FUNC_TO_NUMBER,           \
                                                a1,                                   \
                                                QLP_GET_LAST_POS( a6 ) - a1,          \
                                                sToNumberArgList ) );                 \
    }


/**
 * to_native_real_value_function
 */
#define STL_PARSER_TO_NATIVE_REAL_FUNCTION____DEFAULT( )                                   \
    {                                                                                      \
        qlpList     * sToNativeRealArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                     \
                                                sToNativeRealArgList,                      \
                                                v3 ) );                                    \
                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                     \
                                                KNL_BUILTIN_FUNC_TO_NATIVE_REAL,           \
                                                a1,                                        \
                                                QLP_GET_LAST_POS( a4 ) - a1,               \
                                                sToNativeRealArgList ) );                \
    }

#define STL_PARSER_TO_NATIVE_REAL_FUNCTION____( )                                          \
    {                                                                                      \
        qlpList     * sToNativeRealArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        qlpList     * sFormatArgList       = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        qlpFunction * sFunc   = NULL;                                                      \
                                                                                           \
                                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                     \
                                                sFormatArgList,                            \
                                                v5 ) );                                    \
                                                                                           \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                                  \
                                                   KNL_BUILTIN_FUNC_TO_NUMBER_FORMAT,      \
                                                   a5,                                     \
                                                   QLP_GET_LAST_POS( a5 ) - a5,            \
                                                   sFormatArgList ) );                     \
                                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                     \
                                                sToNativeRealArgList,                      \
                                                v3 ) );                                    \
                                                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                     \
                                                sToNativeRealArgList,                      \
                                                sFunc ) );                                 \
                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                     \
                                                KNL_BUILTIN_FUNC_TO_NATIVE_REAL,           \
                                                a1,                                        \
                                                QLP_GET_LAST_POS( a6 ) - a1,               \
                                                sToNativeRealArgList ) );                  \
    }

/**
 * to_native_double_value_function
 */
#define STL_PARSER_TO_NATIVE_DOUBLE_FUNCTION____DEFAULT( )                                   \
    {                                                                                        \
        qlpList     * sToNativeDoubleArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                       \
                                                sToNativeDoubleArgList,                      \
                                                v3 ) );                                      \
                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                       \
                                                KNL_BUILTIN_FUNC_TO_NATIVE_DOUBLE,           \
                                                a1,                                          \
                                                QLP_GET_LAST_POS( a4 ) - a1,                 \
                                                sToNativeDoubleArgList ) );                  \
    }

#define STL_PARSER_TO_NATIVE_DOUBLE_FUNCTION____( )                                          \
    {                                                                                        \
        qlpList     * sToNativeDoubleArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        qlpList     * sFormatArgList         = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        qlpFunction * sFunc   = NULL;                                                        \
                                                                                             \
                                                                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                       \
                                                sFormatArgList,                              \
                                                v5 ) );                                      \
                                                                                             \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                                    \
                                                   KNL_BUILTIN_FUNC_TO_NUMBER_FORMAT,        \
                                                   a5,                                       \
                                                   QLP_GET_LAST_POS( a5 ) - a5,              \
                                                   sFormatArgList ) );                       \
                                                                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                       \
                                                sToNativeDoubleArgList,                      \
                                                v3 ) );                                      \
                                                                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                       \
                                                sToNativeDoubleArgList,                      \
                                                sFunc ) );                                   \
                                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                       \
                                                KNL_BUILTIN_FUNC_TO_NATIVE_DOUBLE,           \
                                                a1,                                          \
                                                QLP_GET_LAST_POS( a6 ) - a1,                 \
                                                sToNativeDoubleArgList ) );                  \
    }

/**
 * statement_view_scn_function
 */
#define STL_PARSER_STATEMENT_VIEW_SCN_FUNCTION____( )                                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_STATEMENT_VIEW_SCN,    \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

/**
 * extract_function
 */
#define STL_PARSER_EXTRACT_FUNCTION____( )                                                          \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                v3 ) );                                             \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                v5 ) );                                             \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                              \
                                                KNL_BUILTIN_FUNC_EXTRACT,                           \
                                                a1,                                                 \
                                                a6 - a1 + 1,                                        \
                                                sList ) );                                          \
    }

/**
 * date_part_function
 */
#define STL_PARSER_DATE_PART_FUNCTION____( )                                                        \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                v3 ) );                                             \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                v5 ) );                                             \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                              \
                                                KNL_BUILTIN_FUNC_DATE_PART,                         \
                                                a1,                                                 \
                                                a6 - a1 + 1,                                        \
                                                sList ) );                                          \
    }   

/**
 * date_part_field
 */
#define STL_PARSER_EXTRACT_FIELD____DATE_FIELD_STRING( )                                            \
    {                                                                                               \
                                                                                                    \
        stlChar * sBuffer = NULL;                                                                   \
        stlChar * sBuffer2 = NULL;                                                                  \
        stlInt32  sNumLen = 0;                                                                      \
        if( v1 == NULL )                                                                            \
        {                                                                                           \
            sNumLen = 1;                                                                            \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            sNumLen = stlStrlen(  v1  ) + 1;                                                        \
        }                                                                                           \
                                                                                                    \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, sNumLen ) );                              \
        STL_PARSER_CHECK( sBuffer2 = qlpMakeBuffer( param, 2 ) );                                   \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFieldStringConstant( param,                                   \
                                                           a1,                                      \
                                                           v1,                                      \
                                                           sBuffer,                                 \
                                                           sBuffer2 ) );                            \
    }

/**
 * extract_field
 */
#define STL_PARSER_EXTRACT_FIELD____PRIMARY_DATETIME_FIELD( )                                       \
    {                                                                                               \
        stlChar * sBuffer = NULL;                                                                   \
                                                                                                    \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, 2 ) );                                    \
        sBuffer[0] = QLP_GET_INT_VALUE( v1 ) + '0';                                                 \
        sBuffer[1] = '\0';                                                                          \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,                                        \
                                                      a1,                                           \
                                                      sBuffer ) );                                  \
                                                                                                    \
    }

#define STL_PARSER_EXTRACT_FIELD____TIME_ZONE_FIELD( )                                              \
    {                                                                                               \
        stlChar * sBuffer = NULL;                                                                   \
                                                                                                    \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, 2 ) );                                    \
        sBuffer[0] = QLP_GET_INT_VALUE( v1 ) + '0';                                                 \
        sBuffer[1] = '\0';                                                                          \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,                                        \
                                                      a1,                                           \
                                                      sBuffer ) );                                  \
                                                                                                    \
    }

/**
 * time_zone_field
 */
#define STL_PARSER_TIME_ZONE_FIELD____TIME_ZONE_HOUR( )                                          \
    {                                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                   \
                                                        a1,                                      \
                                                        DTL_DATETIME_FIELD_TIMEZONE_HOUR ) );    \
    }                                                                                         

#define STL_PARSER_TIME_ZONE_FIELD____TIME_ZONE_MINUTE( )                                           \
    {                                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                      \
                                                        a1,                                         \
                                                        DTL_DATETIME_FIELD_TIMEZONE_MINUTE ) );     \
    }

/**
 * string_value_function : qlpFunction
 */

#define STL_PARSER_STRING_VALUE_FUNCTION____( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * character_value_function : qlpFunction
 */

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____SUBSTRING( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____SUBSTRB( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____FOLD( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____CONCATENATE( )   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____TRIM( )  \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____OVERLAY( )      \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____PAD( )          \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____REPEAT( )       \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____REPLACE( )      \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____SPLITPART( )    \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____TRANSLATE( )    \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____TO_CHAR( )      \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }

#define STL_PARSER_CHARACTER_VALUE_FUNCTION____CHR( )          \
    {                                                          \
        STL_PARSER_CHECK( v0 = v1 );                           \
    }


/**
 * character_substring_function : qlpFunction
 */

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM( )                      \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SUBSTRING,     \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM_FOR( )                          \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v5 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v7 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_SUBSTRING,             \
                                                a1,                                     \
                                                a8 - a1 + 1,                            \
                                                sList ) );                              \
    }

/*
#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM_USING( )                        \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v5 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v7 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_SUBSTRING,             \
                                                a1,                                     \
                                                a8 - a1 + 1,                            \
                                                sList ) );                              \
    }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____FROM_FOR_USING( )            \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v9 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SUBSTRING,     \
                                                a1,                             \
                                                a10 - a1 + 1,                   \
                                                sList ) );                      \
    }
*/

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____COMMA( )                             \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v5 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v7 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_SUBSTRING,             \
                                                a1,                                     \
                                                a8 - a1 + 1,                            \
                                                sList ) );                              \
    }

#define STL_PARSER_CHARACTER_SUBSTRING_FUNCTION____SINGLE_COMMA( )              \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SUBSTRING,     \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }


/**
 * char_length_units : qlpValue : integer
 */

#define STL_PARSER_CHAR_LENGTH_UNITS____CHARACTER( )                                    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                a1,                                      \
                                                DTL_STRING_LENGTH_UNIT_CHARACTERS ) );  \
    }

#define STL_PARSER_CHAR_LENGTH_UNITS____OCTET( )                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                a1,                                      \
                                                DTL_STRING_LENGTH_UNIT_OCTETS ) );      \
    }

/**
 * character_substrb_function : qlpFunction
 */

#define STL_PARSER_CHARACTER_SUBSTRB_FUNCTION____SINGLE_COMMA( )                \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SUBSTRB,       \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }

#define STL_PARSER_CHARACTER_SUBSTRB_FUNCTION____COMMA( )                       \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SUBSTRB,       \
                                                a1,                             \
                                                a8 - a1 + 1,                    \
                                                sList ) );                      \
    }


/**
 * fold : qlpFunction
 */

#define STL_PARSER_FOLD____INITCAP( )                                       \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_INITCAP,   \
                                                a1,                         \
                                                a4 - a1 + 1,                \
                                                sList ) );                  \
    }


#define STL_PARSER_FOLD____UPPER( )                                     \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_UPPER, \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

#define STL_PARSER_FOLD____LOWER( )                                     \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_LOWER, \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * concatenate_function
 */
#define STL_PARSER_CONCATENATE_FUNCTION____( )                                          \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v5 ) );                                 \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_CONCATENATE,           \
                                                a1,                                     \
                                                a6 - a1 + 1,                            \
                                                sList ) );                              \
    }

/**
 * trim_function : qlpFunction
 */

#define STL_PARSER_TRIM_FUNCTION____( )                                 \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_TRIM,  \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                v3 ) );                 \
    }

/**
 * ltrim_function
 */
#define STL_PARSER_LTRIM_FUNCTION____DEFAULT( )                         \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_LTRIM, \
                                                a1,                     \
                                                a4 - a1 + 1,            \
                                                sList ) );              \
    }

#define STL_PARSER_LTRIM_FUNCTION____( )                                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_LTRIM,  \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * rtrim_function
 */
#define STL_PARSER_RTRIM_FUNCTION____DEFAULT( )                          \
    {                                                                    \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
                                                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                   \
                                                sList,                   \
                                                v3 ) );                  \
                                                                         \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                   \
                                                KNL_BUILTIN_FUNC_RTRIM,  \
                                                a1,                      \
                                                a4 - a1 + 1,             \
                                                sList ) );               \
    }

#define STL_PARSER_RTRIM_FUNCTION____( )                                \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v5 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                  \
                                                KNL_BUILTIN_FUNC_RTRIM,  \
                                                a1,                     \
                                                a6 - a1 + 1,            \
                                                sList ) );              \
    }

/**
 * trim_operands : qlpList : qllNode
 */

#define STL_PARSER_TRIM_OPERANDS____EXPR( )                             \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        stlChar * sBuffer = NULL;                                       \
                                                                        \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, 2 ) );        \
        sBuffer[0] = DTL_FUNC_TRIM_BOTH + '0';                          \
        sBuffer[1] = '\0';                                              \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList(                         \
                              param,                                    \
                              sList,                                    \
                              qlpMakeNumberConstant( param,             \
                                                     0,                 \
                                                     sBuffer ) ) );     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

#define STL_PARSER_TRIM_OPERANDS____TRIM_FROM_EXPR( )                   \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

#define STL_PARSER_TRIM_OPERANDS____EXPR_FROM_EXPR( )                   \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        stlChar * sBuffer = NULL;                                       \
                                                                        \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, 2 ) );        \
        sBuffer[0] = DTL_FUNC_TRIM_BOTH + '0';                          \
        sBuffer[1] = '\0';                                              \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList(                         \
                              param,                                    \
                              sList,                                    \
                              qlpMakeNumberConstant( param,             \
                                                     0,                 \
                                                     sBuffer ) ) );     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v3 ) );                 \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

#define STL_PARSER_TRIM_OPERANDS____TRIM_EXPR_FROM_EXPR( )              \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v4 ) );                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v2 ) );                 \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }


/**
 * trim_specification : qlpValue : string
 */

#define STL_PARSER_TRIM_SPECIFICATION____LEADING( )                     \
    {                                                                   \
        stlChar * sBuffer = NULL;                                       \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, 2 ) );        \
        sBuffer[0] = DTL_FUNC_TRIM_LTRIM + '0';                         \
        sBuffer[1] = '\0';                                              \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,            \
                                                      0,                \
                                                      sBuffer ) );      \
    }

#define STL_PARSER_TRIM_SPECIFICATION____TRAILING( )                    \
    {                                                                   \
        stlChar * sBuffer = NULL;                                       \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, 2 ) );        \
        sBuffer[0] = DTL_FUNC_TRIM_RTRIM + '0';                         \
        sBuffer[1] = '\0';                                              \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,            \
                                                      0,                \
                                                      sBuffer ) );      \
    }

#define STL_PARSER_TRIM_SPECIFICATION____BOTH( )                        \
    {                                                                   \
        stlChar * sBuffer = NULL;                                       \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, 2 ) );        \
        sBuffer[0] = DTL_FUNC_TRIM_BOTH + '0';                          \
        sBuffer[1] = '\0';                                              \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,            \
                                                      0,                \
                                                      sBuffer ) );      \
    }


/**
 * character_overlay_function : qlpFunction
 */

#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM( )                        \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_OVERLAY,       \
                                                a1,                             \
                                                a8 - a1 + 1,                    \
                                                sList ) );                      \
    }

#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM_FOR( )                            \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v5 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v7 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v9 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_OVERLAY,               \
                                                a1,                                     \
                                                a10 - a1 + 1,                           \
                                                sList ) );                              \
    }

/*
#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM_USING( )                          \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v3 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v5 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v7 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                v9 ) );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                qlpMakeNullConstant( param, 0 ) ) );    \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_OVERLAY,               \
                                                a1,                                     \
                                                a10 - a1 + 1,                           \
                                                sList ) );                              \
    }
*/

/*
#define STL_PARSER_CHARACTER_OVERLAY_FUNCTION____FROM_FOR_USING( )              \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v9 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v11 ) );                        \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_OVERLAY,       \
                                                a1,                             \
                                                a12 - a1 + 1,                   \
                                                sList ) );                      \
    }
*/

/**
 * lpad_function
 */
#define STL_PARSER_LPAD_FUNCTION____BOTH( )                                     \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_LPAD,          \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * lpad_function
 */
#define STL_PARSER_LPAD_FUNCTION____BASIC( )                                    \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_LPAD,          \
                                                a1,                             \
                                                a8 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * rpad_function
 */
#define STL_PARSER_RPAD_FUNCTION____BOTH( )                                     \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_RPAD,          \
                                                a1,                             \
                                                a6 - a1 + 1,                    \
                                                sList ) );                      \
    }

/**
 * rpad_function
 */
#define STL_PARSER_RPAD_FUNCTION____BASIC( )                                     \
    {                                                                            \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v3 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v5 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v7 ) );                          \
                                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                           \
                                                KNL_BUILTIN_FUNC_RPAD,           \
                                                a1,                              \
                                                a8 - a1 + 1,                     \
                                                sList ) );                       \
    }

/**
 * repeat_function
 */
#define STL_PARSER_REPEAT_FUNCTION____( )                                   \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_REPEAT,    \
                                                a1,                         \
                                                a6 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * replace_function
 */
#define STL_PARSER_REPLACE_FUNCTION____BOTH( )                                  \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_REPLACE,   \
                                                a1,                         \
                                                a6 - a1 + 1,                \
                                                sList ) );                  \
    }

#define STL_PARSER_REPLACE_FUNCTION____BASIC( )                                     \
    {                                                                            \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v3 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v5 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v7 ) );                          \
                                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                           \
                                                KNL_BUILTIN_FUNC_REPLACE,        \
                                                a1,                              \
                                                a8 - a1 + 1,                     \
                                                sList ) );                       \
    }

/**
 * split_part_function
 */
#define STL_PARSER_SPLITPART_FUNCTION____( )                                     \
    {                                                                            \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v3 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v5 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v7 ) );                          \
                                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                           \
                                                KNL_BUILTIN_FUNC_SPLIT_PART,     \
                                                a1,                              \
                                                a8 - a1 + 1,                     \
                                                sList ) );                       \
    }

/**
 * translate_function
 */
#define STL_PARSER_TRANSLATE_FUNCTION____( )                                     \
    {                                                                            \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v3 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v5 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v7 ) );                          \
                                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                           \
                                                KNL_BUILTIN_FUNC_TRANSLATE,      \
                                                a1,                              \
                                                a8 - a1 + 1,                     \
                                                sList ) );                       \
    }

/**
 * to_char_function
 */
#define STL_PARSER_TO_CHAR_FUNCTION____DEFAULT( )                         \
    {                                                                     \
        qlpList     * sToCharArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpFunction * sFunc   = NULL;                                               \
                                                                                    \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                           \
                                                   KNL_BUILTIN_FUNC_TO_CHAR_FORMAT, \
                                                   0,                               \
                                                   0,                               \
                                                   NULL ) );                        \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToCharArgList,                     \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToCharArgList,                     \
                                                sFunc ) );                          \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_TO_CHAR,           \
                                                a1,                                 \
                                                QLP_GET_LAST_POS( a4 ) - a1,        \
                                                sToCharArgList ) );                 \
    }

#define STL_PARSER_TO_CHAR_FUNCTION____( )                                \
    {                                                                     \
        qlpList     * sToCharArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpFunction * sFunc   = NULL;                                               \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                v5 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                           \
                                                   KNL_BUILTIN_FUNC_TO_CHAR_FORMAT, \
                                                   a5,                              \
                                                   QLP_GET_LAST_POS( a5 ) - a5,     \
                                                   sFormatArgList ) );              \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToCharArgList,                     \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToCharArgList,                     \
                                                sFunc ) );                          \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_TO_CHAR,           \
                                                a1,                                 \
                                                QLP_GET_LAST_POS( a6 ) - a1,        \
                                                sToCharArgList ) );                 \
    }

/**
 * chr_function
 */
#define STL_PARSER_CHR_FUNCTION____( )                                          \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_CHR,           \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1,    \
                                                sList ) );                      \
    }


/**
 * time_zone
 */

#define STL_PARSER_TIME_ZONE____( )             \
    {                                           \
    }


/**
 * time_zone_specifier
 */

#define STL_PARSER_TIME_ZONE_SPECIFIER____LOCAL( )      \
    {                                                   \
    }

#define STL_PARSER_TIME_ZONE_SPECIFIER____TIME_ZONE( )  \
    {                                                   \
    }


/**
 * interval_qualifier
 */

#define STL_PARSER_INTERVAL_QUALIFIER____START_TO_END( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeQualifier( param,         \
                                                 a1,            \
                                                 v1,            \
                                                 v2,            \
                                                 NULL ) );      \
    }

#define STL_PARSER_INTERVAL_QUALIFIER____SINGLE_DATETIME( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeQualifier( param,         \
                                                 a1,            \
                                                 NULL,          \
                                                 NULL,          \
                                                 v1 ) );        \
    }


/**
 * start_field
 */

#define STL_PARSER_START_FIELD____NON_SECOND( )                 \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,  \
                                                a1,             \
                                                v1,             \
                                                NULL,           \
                                                NULL ) );       \
    }

#define STL_PARSER_START_FIELD____NON_SECOND_INTERVAL( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,  \
                                                a1,             \
                                                v1,             \
                                                v3,             \
                                                NULL ) );       \
    }


/**
 * end_field
 */

#define STL_PARSER_END_FIELD____NON_SECOND( )                   \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,  \
                                                a1,             \
                                                v2,             \
                                                NULL,           \
                                                NULL ) );       \
    }

#define STL_PARSER_END_FIELD____TO_SECOND( )                                    \
    {                                                                           \
        qlpValue * sValue = qlpMakeIntParamConstant( param,                     \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_SECOND );  \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,                  \
                                                a1,                             \
                                                sValue,                         \
                                                NULL,                           \
                                                NULL ) );                       \
    }

#define STL_PARSER_END_FIELD____TO_SECOND_INTERVAL( )   \
    {                                                   \
        qlpValue * sValue = qlpMakeIntParamConstant( param,                     \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_SECOND );  \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,                  \
                                                a1,                             \
                                                sValue,                         \
                                                NULL,                           \
                                                v4 ) );                         \
    }


/**
 * single_datetime_field
 */

#define STL_PARSER_SINGLE_DATETIME_FIELD____NON_SECOND( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,  \
                                                a1,             \
                                                v1,             \
                                                NULL,           \
                                                NULL ) );       \
    }

#define STL_PARSER_SINGLE_DATETIME_FIELD____NON_SECOND_INTERVAL( )  \
    {                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,      \
                                                a1,                 \
                                                v1,                 \
                                                v3,                 \
                                                NULL ) );           \
    }

#define STL_PARSER_SINGLE_DATETIME_FIELD____SECOND( )                           \
    {                                                                           \
        qlpValue * sValue = qlpMakeIntParamConstant( param,                     \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_SECOND );  \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,                  \
                                                a1,                             \
                                                sValue,                         \
                                                NULL,                           \
                                                NULL ) );                       \
    }

#define STL_PARSER_SINGLE_DATETIME_FIELD____SECOND_INTERVAL( )                  \
    {                                                                           \
        qlpValue * sValue = qlpMakeIntParamConstant( param,                     \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_SECOND );  \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,                  \
                                                a1,                             \
                                                sValue,                         \
                                                v3,                             \
                                                NULL ) );                       \
    }

#define STL_PARSER_SINGLE_DATETIME_FIELD____SECOND_INTERVAL_INTERVAL( )         \
    {                                                                           \
        qlpValue * sValue = qlpMakeIntParamConstant( param,                     \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_SECOND );  \
        STL_PARSER_CHECK( v0 = qlpMakeIntvPriFieldDesc( param,                  \
                                                a1,                             \
                                                sValue,                         \
                                                v3,                             \
                                                v5 ) );                         \
    }

/**
 * primary_datetime_field
 */

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_PRIMARY_DATETIME_FIELD____SECOND( )                                          \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                        a1,                                     \
                                                        DTL_INTERVAL_PRIMARY_SECOND ) );        \
    }


/**
 * non_second_primary_datetime_field
 */

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____YEAR( )                 \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                  \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_YEAR ) );  \
    }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____MONTH( )                \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                  \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_MONTH ) ); \
    }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____DAY( )                  \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                  \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_DAY ) );   \
    }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____HOUR( )                 \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                  \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_HOUR ) );  \
    }

#define STL_PARSER_NON_SECOND_PRIMARY_DATETIME_FIELD____MINUTE( )               \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                  \
                                                a1,                             \
                                                DTL_INTERVAL_PRIMARY_MINUTE ) );\
    }


/**
 * interval_leading_field_precision
 */

#define STL_PARSER_INTERVAL_LEADING_FIELD_PRECISION____( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * interval_fractional_seconds_precision
 */

#define STL_PARSER_INTERVAL_FRACTIONAL_SECONDS_PRECISION____( ) \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * datetime_value_function : qlpFunction
 */

#define STL_PARSER_DATETIME_VALUE_FUNCTION____SYSDATE( )                        \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SYSDATE,       \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____SYSTIME( )                        \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SYSTIME,       \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____SYSTIMESTAMP( )                   \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_SYSTIMESTAMP,  \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_DATE( )                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_CLOCK_DATE,    \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_DATE( )                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_STATEMENT_DATE,        \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_DATE( )                       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_TRANSACTION_DATE,      \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_TIME( )                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_CLOCK_TIME,    \
                                                a1,                             \
                                                QLP_GET_LAST_POS(a1) - a1,      \
                                                NULL ) );                       \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_TIME( )                         \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_STATEMENT_TIME,        \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_TIME( )                       \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_TRANSACTION_TIME,      \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_TIMESTAMP( )                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_CLOCK_TIMESTAMP,       \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_TIMESTAMP( )                    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_STATEMENT_TIMESTAMP,   \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_TIMESTAMP( )                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_TRANSACTION_TIMESTAMP, \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_LOCALTIME( )                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_CLOCK_LOCALTIME,       \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_LOCALTIME( )                    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_STATEMENT_LOCALTIME,   \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_LOCALTIME( )                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIME, \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____CLOCK_LOCALTIMESTAMP( )                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                  \
                                                KNL_BUILTIN_FUNC_CLOCK_LOCALTIMESTAMP,  \
                                                a1,                                     \
                                                QLP_GET_LAST_POS(a1) - a1,              \
                                                NULL ) );                               \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____STATEMENT_LOCALTIMESTAMP( )                       \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                          \
                                                KNL_BUILTIN_FUNC_STATEMENT_LOCALTIMESTAMP,      \
                                                a1,                                             \
                                                QLP_GET_LAST_POS(a1) - a1,                      \
                                                NULL ) );                                       \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TRANSACTION_LOCALTIMESTAMP( )                     \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                          \
                                                KNL_BUILTIN_FUNC_TRANSACTION_LOCALTIMESTAMP,    \
                                                a1,                                             \
                                                QLP_GET_LAST_POS(a1) - a1,                      \
                                                NULL ) );                                       \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____ADDDATE( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____ADDTIME( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____DATEADD( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____DATE_ADD( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_DATE( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIME( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIME_WITH_TIME_ZONE( ) \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIMESTAMP( )        \
    {                                                                \
        STL_PARSER_CHECK( v0 = v1 );                                 \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____TO_TIMESTAMP_WITH_TIME_ZONE( ) \
    {                                                                        \
        STL_PARSER_CHECK( v0 = v1 );                                         \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____LAST_DAY( )                    \
    {                                                                        \
        STL_PARSER_CHECK( v0 = v1 );                                         \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____ADD_MONTHS( )                    \
    {                                                                          \
        STL_PARSER_CHECK( v0 = v1 );                                           \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____FROM_UNIXTIME( )                 \
    {                                                                          \
        STL_PARSER_CHECK( v0 = v1 );                                           \
    }

#define STL_PARSER_DATETIME_VALUE_FUNCTION____DATEDIFF()               \
    {                                                                  \
        STL_PARSER_CHECK( v0 = v1 );                                   \
    }

/**
 * time_precision : qlpValue : float
 */

#define STL_PARSER_TIME_PRECISION____( )        \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * timestamp_precision : qlpValue : float
 */

#define STL_PARSER_TIMESTAMP_PRECISION____( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * time_fractional_seconds_precision : qlpValue : float
 */

#define STL_PARSER_TIME_FRACTIONAL_SECONDS_PRECISION____( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

/**
 * adddate_value_function
 */
#define STL_PARSER_ADDDATE_FUNCTION____( )                                  \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_ADDDATE,   \
                                                a1,                         \
                                                a6 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * addtime_value_function
 */
#define STL_PARSER_ADDTIME_FUNCTION____( )                                  \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_ADDTIME,   \
                                                a1,                         \
                                                a6 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * dateadd_value_function
 */
#define STL_PARSER_DATETIME_FUNCTION____DATEADD( )                              \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v7 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_DATEADD,       \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a8 ) - a1,    \
                                                sList ) );                      \
    }

/**
 * datetime_part
 */
#define STL_PARSER_DATETIME_PART____DATETIME_PART( )                            \
    {                                                                           \
        stlChar     * sBuffer = NULL;                                           \
        stlInt32      sNumLen = 3;                                              \
        stlInt32      sTmpVal = 0;                                              \
                                                                                \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, sNumLen ) );          \
                                                                                \
        if( QLP_GET_INT_VALUE( v1 ) < 10 )                                      \
        {                                                                       \
            sBuffer[0] = QLP_GET_INT_VALUE( v1 ) + '0';                         \
            sBuffer[1] = '\0';                                                  \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            sTmpVal = (QLP_GET_INT_VALUE( v1 ) / 10);                           \
            sBuffer[1] = (QLP_GET_INT_VALUE( v1 ) - (sTmpVal * 10)) + '0';      \
            sBuffer[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                 \
            sBuffer[2] = '\0';                                                  \
        }                                                                       \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,                    \
                                                      a1,                       \
                                                      sBuffer ) );              \
                                                                                \
    }


/**
 * datetime_part_field
 */
#define STL_PARSER_DATETIME_PART____PRIMARY_DATETIME_FIELD( )           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_DATETIME_PART____QUARTER( )                                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        DTL_DATETIME_PART_QUARTER ) );  \
    }

#define STL_PARSER_DATETIME_PART____DAYOFYEAR( )                                          \
    {                                                                                     \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                            \
                                                        a1,                               \
                                                        DTL_DATETIME_PART_DAYOFYEAR ) );  \
    }

#define STL_PARSER_DATETIME_PART____WEEK( )                                          \
    {                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                       \
                                                        a1,                          \
                                                        DTL_DATETIME_PART_WEEK ) );  \
    }

#define STL_PARSER_DATETIME_PART____WEEKDAY( )                                          \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                        a1,                             \
                                                        DTL_DATETIME_PART_WEEKDAY ) );  \
    }

#define STL_PARSER_DATETIME_PART____MILLISECOND( )                                          \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                              \
                                                        a1,                                 \
                                                        DTL_DATETIME_PART_MILLISECOND ) );  \
    }

#define STL_PARSER_DATETIME_PART____MICROSECOND( )                                          \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                              \
                                                        a1,                                 \
                                                        DTL_DATETIME_PART_MICROSECOND ) );  \
    }

/**
 * date_add_value_function
 */
#define STL_PARSER_DATE_ADD_FUNCTION____( )                                 \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                      \
                                                KNL_BUILTIN_FUNC_ADDDATE,   \
                                                a1,                         \
                                                a6 - a1 + 1,                \
                                                sList ) );                  \
    }

/**
 * to_date_function
 */
#define STL_PARSER_TO_DATE_FUNCTION____DEFAULT( )                         \
    {                                                                     \
        qlpList     * sToDateArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_DATE / 10);                                             \
        sStr[1] = (DTL_TYPE_DATE - (sTmpVal * 10)) + '0';                           \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   0,                                   \
                                                   0,                                   \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToDateArgList,                     \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToDateArgList,                     \
                                                sFunc ) );                          \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_TO_DATE,           \
                                                a1,                                 \
                                                QLP_GET_LAST_POS( a4 ) - a1,        \
                                                sToDateArgList ) );                 \
    }

#define STL_PARSER_TO_DATE_FUNCTION____( )                                          \
    {                                                                               \
        qlpList     * sToDateArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_DATE / 10);                                             \
        sStr[1] = (DTL_TYPE_DATE - (sTmpVal * 10)) + '0';                           \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToDateArgList,                     \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                v5 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   a5,                                  \
                                                   QLP_GET_LAST_POS( a5 ) - a5,         \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sToDateArgList,              \
                                                sFunc ) );                   \
                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_TO_DATE,    \
                                                a1,                          \
                                                QLP_GET_LAST_POS( a6 ) - a1, \
                                                sToDateArgList ) );          \
    }


/**
 * to_time_function
 */
#define STL_PARSER_TO_TIME_FUNCTION____DEFAULT( )                         \
    {                                                                     \
        qlpList     * sToTimeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIME / 10);                                             \
        sStr[1] = (DTL_TYPE_TIME - (sTmpVal * 10)) + '0';                           \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   0,                                   \
                                                   0,                                   \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimeArgList,                     \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimeArgList,                     \
                                                sFunc ) );                          \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_TO_TIME,           \
                                                a1,                                 \
                                                QLP_GET_LAST_POS( a4 ) - a1,        \
                                                sToTimeArgList ) );                 \
    }

#define STL_PARSER_TO_TIME_FUNCTION____( )                                          \
    {                                                                               \
        qlpList     * sToTimeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIME / 10);                                             \
        sStr[1] = (DTL_TYPE_TIME - (sTmpVal * 10)) + '0';                           \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimeArgList,                     \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                v5 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   a5,                                  \
                                                   QLP_GET_LAST_POS( a5 ) - a5,         \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sToTimeArgList,                         \
                                                sFunc ) );                   \
                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_TO_TIME,    \
                                                a1,                          \
                                                QLP_GET_LAST_POS( a6 ) - a1, \
                                                sToTimeArgList ) );          \
    }


/**
 * to_time_with_time_zone_function
 */
#define STL_PARSER_TO_TIME_WITH_TIME_ZONE_FUNCTION____DEFAULT( )          \
    {                                                                     \
        qlpList     * sToTimeWithTimeZoneArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIME_WITH_TIME_ZONE / 10);                              \
        sStr[1] = (DTL_TYPE_TIME_WITH_TIME_ZONE - (sTmpVal * 10)) + '0';            \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   0,                                   \
                                                   0,                                   \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimeWithTimeZoneArgList,         \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimeWithTimeZoneArgList,         \
                                                sFunc ) );                          \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_TO_TIME_WITH_TIME_ZONE, \
                                                a1,                                 \
                                                QLP_GET_LAST_POS( a4 ) - a1,        \
                                                sToTimeWithTimeZoneArgList ) );     \
    }

#define STL_PARSER_TO_TIME_WITH_TIME_ZONE_FUNCTION____( )                           \
    {                                                                               \
        qlpList     * sToTimeWithTimeZoneArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIME_WITH_TIME_ZONE / 10);                              \
        sStr[1] = (DTL_TYPE_TIME_WITH_TIME_ZONE - (sTmpVal * 10)) + '0';            \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimeWithTimeZoneArgList,         \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                v5 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   a5,                                  \
                                                   QLP_GET_LAST_POS( a5 ) - a5,         \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sToTimeWithTimeZoneArgList,             \
                                                sFunc ) );                   \
                                                                             \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_TO_TIME_WITH_TIME_ZONE, \
                                                a1,                          \
                                                QLP_GET_LAST_POS( a6 ) - a1, \
                                                sToTimeWithTimeZoneArgList ) );          \
    }

/**
 * to_timestamp function
 */

#define STL_PARSER_TO_TIMESTAMP_FUNCTION____DEFAULT( )                              \
    {                                                                               \
        qlpList     * sToTimestampArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIMESTAMP / 10);                                        \
        sStr[1] = (DTL_TYPE_TIMESTAMP - (sTmpVal * 10)) + '0';                      \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   0,                                   \
                                                   0,                                   \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sToTimestampArgList,                    \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimestampArgList,                \
                                                sFunc ) );                          \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_TO_TIMESTAMP,      \
                                                a1,                                 \
                                                QLP_GET_LAST_POS( a4 ) - a1,        \
                                                sToTimestampArgList ) );            \
    }

#define STL_PARSER_TO_TIMESTAMP_FUNCTION____( )                                     \
    {                                                                               \
        qlpList     * sToTimestampArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIMESTAMP / 10);                                        \
        sStr[1] = (DTL_TYPE_TIMESTAMP - (sTmpVal * 10)) + '0';                      \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimestampArgList,                \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                v5 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   a5,                                  \
                                                   QLP_GET_LAST_POS( a5 ) - a5,         \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sToTimestampArgList,                    \
                                                sFunc ) );                              \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_TO_TIMESTAMP,  \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a6 ) - a1,    \
                                                sToTimestampArgList ) );        \
    }


/**
 * to_timestamp_with_time_zone
 */

#define STL_PARSER_TO_TIMESTAMP_WITH_TIME_ZONE_FUNCTION____DEFAULT( )               \
    {                                                                               \
        qlpList     * sToTimestampWithTimeZoneArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE / 10);                         \
        sStr[1] = (DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE - (sTmpVal * 10)) + '0';       \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   0,                                   \
                                                   0,                                   \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sToTimestampWithTimeZoneArgList,        \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimestampWithTimeZoneArgList,    \
                                                sFunc ) );                          \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                KNL_BUILTIN_FUNC_TO_TIMESTAMP_WITH_TIME_ZONE, \
                                                a1,                                 \
                                                QLP_GET_LAST_POS( a4 ) - a1,        \
                                                sToTimestampWithTimeZoneArgList ) );  \
    }

#define STL_PARSER_TO_TIMESTAMP_WITH_TIME_ZONE_FUNCTION____( )                      \
    {                                                                               \
        qlpList     * sToTimestampWithTimeZoneArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        qlpList     * sFormatArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
        stlInt32      sNumLen = 3;                                                  \
        stlInt32      sTmpVal = 0;                                                  \
        qlpFunction * sFunc   = NULL;                                               \
        qlpValue    * sValue  = NULL;                                               \
        stlChar     * sStr    = qlpMakeBuffer( param,                               \
                                               sNumLen );                           \
                                                                                    \
        sTmpVal = (DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE / 10);                         \
        sStr[1] = (DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE - (sTmpVal * 10)) + '0';       \
        sStr[0] = (sTmpVal - ((sTmpVal / 10)*10)) + '0';                            \
        sStr[2] = '\0';                                                             \
                                                                                    \
        STL_PARSER_CHECK( sValue =                                                  \
                          qlpMakeNumberConstant( param,                             \
                                                 0,                                 \
                                                 sStr ) );                          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sToTimestampWithTimeZoneArgList,    \
                                                v3 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                sValue ) );                         \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFormatArgList,                     \
                                                v5 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                               \
                                                   KNL_BUILTIN_FUNC_TO_DATETIME_FORMAT, \
                                                   a5,                                  \
                                                   QLP_GET_LAST_POS( a5 ) - a5,         \
                                                   sFormatArgList ) );                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sToTimestampWithTimeZoneArgList,        \
                                                sFunc ) );                              \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_TO_TIMESTAMP_WITH_TIME_ZONE,  \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a6 ) - a1,    \
                                                sToTimestampWithTimeZoneArgList ) );        \
    }


/**
 * last_day_function
 */
#define STL_PARSER_LAST_DAY_FUNCTION____( )  \
    {                                        \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_LAST_DAY,      \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1 ,   \
                                                sList ) );                      \
    }

/**
 * add_months_function
 */
#define STL_PARSER_ADD_MONTHS_FUNCTION____( )  \
    {                                        \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_ADD_MONTHS,    \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a6 ) - a1 ,   \
                                                sList ) );                      \
    }

/**
 * from_unixtime_function
 */
#define STL_PARSER_FROM_UNIXTIME_FUNCTION____( )                                \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_FROM_UNIXTIME, \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1 ,   \
                                                sList ) );                      \
    }

/**
 * datediff_function
 */
#define STL_PARSER_DATEDIFF_FUNCTION____( )                                      \
    {                                                                            \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v3 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v5 ) );                          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                           \
                                                sList,                           \
                                                v7 ) );                          \
                                                                                 \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                           \
                                                KNL_BUILTIN_FUNC_DATEDIFF,       \
                                                a1,                              \
                                                QLP_GET_LAST_POS( a8 ) - a1,     \
                                                sList ) );                       \
    }

/**
 * nvl_function
 */
#define STL_PARSER_NVL_FUNCTION____( )                                      \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
                                                                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForNvl( param,                                  \
                                                      a1,                                     \
                                                      QLP_GET_LAST_POS( a6 ) - a1,            \
                                                      QLL_NVL_EXPR,                           \
                                                      KNL_BUILTIN_FUNC_NVL,                   \
                                                      sList ) );                              \
    }

#define STL_PARSER_NVL2_FUNCTION____( )                                     \
    {                                                                       \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
                                                                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v3 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v5 ) );                     \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                v7 ) );                     \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForNvl( param,                             \
                                                      a1,                                \
                                                      QLP_GET_LAST_POS( a8 ) - a1,       \
                                                      QLL_NVL2_EXPR,                     \
                                                      KNL_BUILTIN_FUNC_NVL2,             \
                                                      sList ) );                         \
    }
 

/**
 * dump_function
 */

#define STL_PARSER_DUMP_FUNCTION____( )                                         \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_DUMP,          \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1 ,   \
                                                sList ) );                      \
    }


/**
 * decode_function
 */

#define STL_PARSER_DECODE_FUNCTION____( )                                                  \
    {                                                                                      \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForDecode( param,                            \
                                                         a1,                               \
                                                         QLP_GET_LAST_POS( a4 ) - a1,      \
                                                         QLL_DECODE_EXPR,                  \
                                                         KNL_BUILTIN_FUNC_DECODE,          \
                                                         v3 ) );                           \
    }

/**
 * decode_expr_list
 */

#define STL_PARSER_DECODE_LIST____DECODE_FUNC( )                        \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

#define STL_PARSER_DECODE_LIST____DECODE_FUNC_LIST( )                   \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * case2_function
 */

#define STL_PARSER_CASE2_FUNCTION____( )                                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCaseExprForCase2( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a4 ) - a1,    \
                                                        QLL_CASE2_EXPR,                 \
                                                        KNL_BUILTIN_FUNC_CASE2,         \
                                                        v3 ) );                         \
    }

/**
 * case2_expr_list
 */

#define STL_PARSER_CASE2_LIST____CASE2_FUNC( )                          \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
        STL_PARSER_CHECK( v0 = sList );                                 \
    }

#define STL_PARSER_CASE2_LIST____CASE2_FUNC_LIST( )                     \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

/**
 * general_comparison_function
 */

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION____GREATEST_FUNC( )             \
    {                                                                          \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                         \
                                                KNL_BUILTIN_FUNC_GREATEST,     \
                                                a1,                            \
                                                QLP_GET_LAST_POS( a4 ) - a1,   \
                                                v3 ) );                        \
    }

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION____LEAST_FUNC( )                \
    {                                                                          \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                         \
                                                KNL_BUILTIN_FUNC_LEAST,        \
                                                a1,                            \
                                                QLP_GET_LAST_POS( a4 ) - a1,   \
                                                v3 ) );                        \
    }

/**
 * general_comparison_function:
 */

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION_ARG_LIST____GENERAL_COMPARISON_FUNC( )   \
    {                                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                  \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );                   \
        STL_PARSER_CHECK( v0 = sList );                                                 \
    }

#define STL_PARSER_GENERAL_COMPARISON_FUNCTION_ARG_LIST____GENERAL_COMPARISON_FUNC_LIST( ) \
    {                                                                                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );                         \
        STL_PARSER_CHECK( v0 = v1 );                                                       \
    }




/**
 * boolean_value_expression : qllNode
 */

#define STL_PARSER_BOOLEAN_VALUE_EXPRESSION____SINGLE( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_BOOLEAN_VALUE_EXPRESSION____OR( )                                            \
    {                                                                                           \
        qlpList * sList;                                                                        \
        if( (QLL_NODE_TYPE( v1 ) != QLL_FUNCTION_TYPE) ||                                       \
            (((qlpFunction*)(v1))->mFuncId != KNL_BUILTIN_FUNC_OR ) )                           \
        {                                                                                       \
            sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v1 ) );                                     \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v3 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_OR,                        \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a3 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            sList = qlpGetFunctionArgList( v1 );                                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v3 ) );                       \
            STL_PARSER_CHECK( v0 = v1 );                                                        \
        }                                                                                       \
    }


/**
 * boolean_term : qllNode
 */

#define STL_PARSER_BOOLEAN_TERM____BOOLEAN( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_BOOLEAN_TERM____BOOLEAN_AND_BOOLEAN( )                       \
    {                                                                           \
        qlpList * sList;                                                                        \
        if( (QLL_NODE_TYPE( v1 ) != QLL_FUNCTION_TYPE) ||                                       \
            (((qlpFunction*)(v1))->mFuncId != KNL_BUILTIN_FUNC_AND ) )                          \
        {                                                                                       \
            sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v1 ) );                                     \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v3 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_AND,                       \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a3 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            sList = qlpGetFunctionArgList( v1 );                                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v3 ) );                       \
            STL_PARSER_CHECK( v0 = v1 );                                                        \
        }                                                                                       \
    }


/**
 * boolean_comparison_predicate : qllNode
 */

#define STL_PARSER_BOOLEAN_COMPARISON_PREDICATE____VALUE()      \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_BOOLEAN_COMPARISON_PREDICATE____FUNCTION()                   \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                QLP_GET_INT_VALUE( v2 ),        \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1 ,   \
                                                sList ) );                      \
    }


/**
 * boolean_factor : qllNode
 */

#define STL_PARSER_BOOLEAN_FACTOR____BOOLEAN( )  \
    {                                            \
        STL_PARSER_CHECK( v0 = v1 );             \
    }

#define STL_PARSER_BOOLEAN_FACTOR____NOT_BOOLEAN( )                                             \
    {                                                                                           \
        if( qlpIsExistsFunc( (void *) v2 ) == STL_TRUE )                                        \
        {                                                                                       \
            ((qlpListFunc *) v2)->mFuncId = KNL_BUILTIN_FUNC_NOT_EXISTS;                        \
            ((qlpListFunc *) v2)->mNodePos = a1;                                                \
            ((qlpListFunc *) v2)->mNodeLen = QLP_GET_LAST_POS( a2 ) - a1;                       \
            ((qlpListCol *) ((qlpListFunc *) v2)->mArgs->mHead->mData.mPtrValue )->mFuncId =    \
                KNL_BUILTIN_FUNC_NOT_EXISTS;                                                    \
                                                                                                \
            STL_PARSER_CHECK( v0 = v2 );                                                        \
        }                                                                                       \
        else                                                                                    \
        {                                                                                       \
            qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                      \
                                                                                                \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                    sList,                                      \
                                                    v2 ) );                                     \
                                                                                                \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                      \
                                                    KNL_BUILTIN_FUNC_NOT,                       \
                                                    a1,                                         \
                                                    QLP_GET_LAST_POS( a2 ) - a1 ,               \
                                                    sList ) );                                  \
        }                                                                                       \
    }


/**
 * boolean_test : qllNode
 */

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_TRUTH( )                          \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS,            \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1 ,   \
                                                sList ) );                      \
    }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_NOT_TRUTH( )                                      \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v4 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS_NOT,        \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1 ,   \
                                                sList ) );                      \
    }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_NULL( )                           \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS_NULL,       \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1 ,   \
                                                sList ) );                      \
    }

#define STL_PARSER_BOOLEAN_TEST____BOOLEAN_IS_NOT_NULL( )                       \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS_NOT_NULL,   \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1 ,   \
                                                sList ) );                      \
    }


/**
 * truth_value : qlpValue : string
 */

#define STL_PARSER_TRUTH_VALUE____TRUE( )                               \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_TRUE ) );   \
    }

#define STL_PARSER_TRUTH_VALUE____FALSE( )                              \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,          \
                                                        a1,             \
                                                        STL_FALSE ) );  \
    }

#define STL_PARSER_TRUTH_VALUE____UNKNOWN( )                    \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeNullConstant( param,      \
                                                    a1 ) );     \
    }


/**
 * boolean_primary : qllNode
 */

#define STL_PARSER_BOOLEAN_PRIMARY____LITERAL( )        \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_BOOLEAN_PRIMARY____PREDICATE( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_BOOLEAN_PRIMARY____BOOLEAN_PREDICATE( )      \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * boolean_predicand : qllNode
 */

#define STL_PARSER_BOOLEAN_PREDICAND____( )     \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * parenthesized_boolean_value_expression : qllNode
 */

#define STL_PARSER_PARENTHESIZED_BOOLEAN_VALUE_EXPRESSION____( )        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v2 );                                    \
    }


/**
 * nonparenthesized_value_expression_primary : qllNode
 */

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____VALUE( )        \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v1 );                                            \
    }

#define STL_PARSER_NONPARENTHESIZED_VALUE_EXPRESSION_PRIMARY____CAST( ) \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * unsigned_value_specification
 */

#define STL_PARSER_UNSIGNED_VALUE_SPECIFICATION____LITERAL( )   \
    {                                                           \
        STL_PARSER_CHECK( v0 = (qllNode *) v1 );                \
    }

#define STL_PARSER_UNSIGNED_VALUE_SPECIFICATION____VALUE( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * unsigned_literal
 */

#define STL_PARSER_UNSIGNED_LITERAL____NUMERIC_LITERAL( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_UNSIGNED_LITERAL____GENERAL_LITERAL( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * unsigned_numeric_literal
 */

#define STL_PARSER_UNSIGNED_NUMERIC_LITERAL____EXACT( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_UNSIGNED_NUMERIC_LITERAL____APPROXIMATE( )   \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }


/**
 * exact_numeric_literal
 */

#define STL_PARSER_EXACT_NUMERIC_LITERAL____UNSIGNED_INTEGER( ) \
    {                                                                   \
        qlpValue  * sValueNode;                                         \
        STL_PARSER_CHECK( sValueNode = qlpMakeNumberConstant( param,        \
                                                          a1,           \
                                                          v1 ) );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                  \
                                                a1,                     \
                                                sValueNode ) );         \
    }


#define STL_PARSER_EXACT_NUMERIC_LITERAL____EXACT_NUMERIC( )            \
    {                                                                   \
        qlpValue  * sValueNode;                                         \
        STL_PARSER_CHECK( sValueNode = qlpMakeNumberConstant( param,        \
                                                          a1,           \
                                                          v1 ) );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                  \
                                                a1,                     \
                                                sValueNode ) );         \
                                                                        \
    }


/**
 * approximate_numeric_literal
 */

#define STL_PARSER_APPROXIMATE_NUMERIC_LITERAL____( )           \
    {                                                                   \
        qlpValue  * sValueNode;                                         \
        STL_PARSER_CHECK( sValueNode = qlpMakeNumberConstant( param,        \
                                                          a1,           \
                                                          v1 ) );       \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                  \
                                                a1,                     \
                                                sValueNode ) );         \
                                                                        \
    }



/**
 * signed_integer
 */

#define STL_PARSER_SIGNED_INTEGER____UNSIGNED_INTERGER( )       \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_SIGNED_INTEGER____SIGNED_INTERGER( )                         \
    {                                                                           \
        qlpValue  * sValue = v2;                                                \
        stlInt32    sNumLen = stlStrlen( QLP_GET_PTR_VALUE( sValue ) ) + 2;     \
        stlChar   * sStr    = qlpMakeBuffer( param,                             \
                                             sNumLen );                         \
                                                                                \
        sStr[ 0 ] = ((stlChar*)v1)[ 0 ];                                        \
        stlStrcpy( & sStr[ 1 ], QLP_GET_PTR_VALUE( sValue ) );                  \
        sStr[ sNumLen - 1 ] = '\0';                                             \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param, a1, sStr ) );      \
    }


/**
 * unsigned_integer
 */

#define STL_PARSER_UNSIGNED_INTEGER____( )                      \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeNumberConstant( param,    \
                                                      a1,       \
                                                      v1 ) );   \
    }


/**
 * general_literal
 */

#define STL_PARSER_GENERAL_LITERAL____CHARACTER( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_GENERAL_LITERAL____BINARY( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_GENERAL_LITERAL____DATETIME( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_GENERAL_LITERAL____INTERVAL( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_GENERAL_LITERAL____BOOLEAN( )        \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_GENERAL_LITERAL____NULL( )        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNullConstant( param, a1 ) );         \
    }

/**
 * character_string_literal
 */

#define STL_PARSER_CHARACTER_STRING_LITERAL____STRING( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,          \
                                                a1,             \
                                                v1 ) );         \
    }

#define STL_PARSER_CHARACTER_STRING_LITERAL____INTRODUCER_STRING( )                     \
    {                                                                                   \
        stlChar   * sBuffer;                                                            \
        stlInt32    sStrSize1 = stlStrlen( QLP_GET_PTR_VALUE( v2 ) );                   \
        stlInt32    sStrSize2 = stlStrlen( QLP_GET_PTR_VALUE( v3 ) );                   \
        stlInt32    sBufSize  = sStrSize1 + sStrSize2 + 2;                              \
        qlpValue  * sValueNode = NULL;                                                  \
                                                                                        \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, sBufSize ) );                 \
                                                                                        \
        stlMemcpy( sBuffer, QLP_GET_PTR_VALUE( v2 ), sStrSize1 );                       \
                                                                                        \
        sBuffer[ sStrSize1 ] = '\'';                                                    \
                                                                                        \
        stlMemcpy( & sBuffer[ sStrSize1 + 1 ], QLP_GET_PTR_VALUE( v3 ), sStrSize2 );    \
                                                                                        \
        sBuffer[ sBufSize - 1 ] = '\0';                                                 \
                                                                                        \
        STL_PARSER_CHECK( sValueNode = qlpMakeStrConstant( param,                          \
                                                        a2,                             \
                                                        sBuffer ) );                    \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                                  \
                                                a1,                                     \
                                                sValueNode ) );                         \
    }


/**
 * quote_character_representation_list
 */

#define STL_PARSER_QUOTE_CHARACTER_REPRESENTATION_LIST____STRING( )     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_QUOTE_CHARACTER_REPRESENTATION_LIST____STRING_LIST( )                \
    {                                                                                   \
        stlChar  * sBuffer;                                                             \
        stlInt32   sStrSize1 = stlStrlen( QLP_GET_PTR_VALUE( v1 ) );                    \
        stlInt32   sStrSize2 = stlStrlen( QLP_GET_PTR_VALUE( v3 ) );                    \
        stlInt32   sBufSize  = sStrSize1 + sStrSize2 + 1;                               \
                                                                                        \
        STL_PARSER_CHECK( sBuffer = qlpMakeBuffer( param, sBufSize ) );                 \
                                                                                        \
        stlMemcpy( sBuffer, QLP_GET_PTR_VALUE( v1 ), sStrSize1 );                       \
        stlMemcpy( & sBuffer[ sStrSize1 ], QLP_GET_PTR_VALUE( v3 ), sStrSize2 );        \
                                                                                        \
        sBuffer[ sBufSize - 1 ] = '\0';                                                 \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param,                                  \
                                                a1,                                     \
                                                sBuffer ) );                            \
                                                                                        \
    }


/**
 * quote_character_representation
 */

#define STL_PARSER_QUOTE_CHARACTER_REPRESENTATION____STRING( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param,       \
                                                   a1,          \
                                                   v1 ) );      \
    }


/**
 * separator
 */

#define STL_PARSER_SEPARATOR____( )             \
    {                                           \
    }


/**
 * separator_2
 */

#define STL_PARSER_SEPARATOR_2____( )           \
    {                                           \
    }


/**
 * introducer
 */

#define STL_PARSER_INTRODUCER____( )            \
    {                                           \
    }


/**
 * character_set_specification
 */

#define STL_PARSER_CHARACTER_SET_SPECIFICATION____( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1);                     \
    }


/**
 * standard_character_set_name
 */

#define STL_PARSER_STANDARD_CHARACTER_SET_NAME____( )   \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1);                     \
    }


/**
 * character_set_name
 */

#define STL_PARSER_CHARACTER_SET_NAME____( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1);             \
    }


/**
 * SQL_language_identifier
 */

#define STL_PARSER_SQL_LANGUAGE_IDENTIFIER____( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1);                     \
    }


/**
 * column_name
 */

#define STL_PARSER_COLUMN_NAME____( )           \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * identifier_chain
 */

#define STL_PARSER_IDENTIFIER_CHAIN____IDENTIFIER( )                    \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_IDENTIFIER_CHAIN____IDENTIFIER_LIST( )       \
    {                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,          \
                                                v1,             \
                                                v3 ) );         \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

/**
 * rowid_identifier_chain
 */

#define STL_PARSER_IDENTIFIER_CHAIN____ROWID_IDENTIFIER( )              \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_IDENTIFIER_CHAIN____ROWID_IDENTIFIER_LIST( ) \
    {                                                           \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,          \
                                                v1,             \
                                                v3 ) );         \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

/**
 * rowid_pseudo_column
 */

#define STL_PARSER_REGUALAR_IDENTIFIER____ROWID_IDENTIFIER( )                 \
    {                                                                         \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );         \
    }

/**
 * identifier
 */

#define STL_PARSER_IDENTIFIER____( )                                    \
    {                                                                   \
        STL_PARSER_CHECK( qlpCheckIdentifierLength( param, v1 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * actual_identifier
 */

#define STL_PARSER_ACTUAL_IDENTIFIER____REGULAR( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1);                     \
    }

#define STL_PARSER_ACTUAL_IDENTIFIER____DELIMITED( )    \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1);                     \
    }


/**
 * regualar_identifier
 */

#define STL_PARSER_REGUALAR_IDENTIFIER____IDENTIFIER( )                 \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );      \
    }

#define STL_PARSER_REGUALAR_IDENTIFIER____NON_RESERVED_WORD( )          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );      \
    }

#define STL_PARSER_REGUALAR_IDENTIFIER____QSQL_KEYWORD( )               \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );      \
    }

/**
 * delimited_identifier
 */

#define STL_PARSER_DELIMITED_IDENTIFIER____( )                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );      \
    }


/**
 * cast_specification
 */

#define STL_PARSER_CAST_SPECIFICATION____( )  \
    {                                                           \
        qlpTypeCast  * sTypeCast = NULL;                        \
        STL_PARSER_CHECK( sTypeCast = qlpMakeTypeCast( param,   \
                                                       a1,      \
                                                       v3,      \
                                                       v5 ) );  \
                                                                \
        STL_PARSER_CHECK( v0 = (qllNode *) sTypeCast );         \
    }


/**
 * cast_operand
 */

#define STL_PARSER_CAST_OPERAND____VALUE_EXPR( ) \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_CAST_OPERAND____IMPLICITLY_TYPED_VALUE( )     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode *) v1 );                        \
    }


/**
 * cast_target
 */

#define STL_PARSER_CAST_TARGET____( )           \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * implicitly_typed_value_specification
 */

#define STL_PARSER_IMPLICITLY_TYPED_VALUE_SPECIFICATION____NULL( )      \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNullConstant( param, a1 ) );      \
    }

#define STL_PARSER_IMPLICITLY_TYPED_VALUE_SPECIFICATION____PAREN_NULL( )        \
    {                                                                           \
        STL_PARSER_CHECK( v0 = v2 );                                            \
    }


/**
 * data_type
 */

#define STL_PARSER_DATA_TYPE____( )             \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * predefined_type
 */

#define STL_PARSER_PREDEFINED_TYPE____CHARACTER( )      \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_PREDEFINED_TYPE____CHARACTER_CHARACTER_SET( )        \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

#define STL_PARSER_PREDEFINED_TYPE____NATIONAL_CHARACTER( )     \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_PREDEFINED_TYPE____BINARY( ) \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_PREDEFINED_TYPE____NUMERIC( )        \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_PREDEFINED_TYPE____BOOLEAN( )        \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_PREDEFINED_TYPE____DATETIME( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_PREDEFINED_TYPE____INTERVAL( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_PREDEFINED_TYPE____ROWID()           \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }


/**
 * character_string_type : qlpTypeName
 */

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR( )                     \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,              \
                                                    a1,                 \
                                                    stlStrlen( v1 ),    \
                                                    DTL_TYPE_CHAR,      \
                                                    a1,                 \
                                                    NULL,               \
                                                    NULL,               \
                                                    0 )                 \
            );                                                          \
    }

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR_LENGTH( )              \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,              \
                                                    a1,                 \
                                                    a4 - a1 + 1,        \
                                                    DTL_TYPE_CHAR,      \
                                                    a1,                 \
                                                    v3,                 \
                                                    NULL,               \
                                                    0 )                 \
            );                                                          \
    }

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR_VARYING_LENGTH( )      \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,              \
                                                    a1,                 \
                                                    a5 - a1 + 1,        \
                                                    DTL_TYPE_VARCHAR,   \
                                                    a1,                 \
                                                    v4,                 \
                                                    NULL,               \
                                                    0 )                 \
            );                                                          \
    }

#define STL_PARSER_CHARACTER_STRING_TYPE____VARCHAR_NON_PRECISION( )    \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,              \
                                                    a1,                 \
                                                    stlStrlen( v1 ),    \
                                                    DTL_TYPE_VARCHAR,   \
                                                    a1,                 \
                                                    NULL,               \
                                                    NULL,               \
                                                    0 )                 \
            );                                                          \
    }

#define STL_PARSER_CHARACTER_STRING_TYPE____CHAR_VARYING_NON_PRECISION( )   \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,                  \
                                                    a1,                     \
                                                    a2 - a1 + 1,            \
                                                    DTL_TYPE_VARCHAR,       \
                                                    a1,                     \
                                                    NULL,                   \
                                                    NULL,                   \
                                                    0 )                     \
            );                                                              \
    }

#define STL_PARSER_CHARACTER_STRING_TYPE____VARCHAR_LENGTH( )           \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,              \
                                                    a1,                 \
                                                    a4 - a1 + 1,        \
                                                    DTL_TYPE_VARCHAR,   \
                                                    a1,                 \
                                                    v3,                 \
                                                    NULL,               \
                                                    0 )                 \
            );                                                          \
    }

#define STL_PARSER_CHARACTER_STRING_TYPE____LONG_CHAR_VARYING_LENGTH( )             \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,                          \
                                                    a1,                             \
                                                    QLP_GET_LAST_POS( a3 ) - a1,    \
                                                    DTL_TYPE_LONGVARCHAR,           \
                                                    a1,                             \
                                                    NULL,                           \
                                                    NULL,                           \
                                                    0 )                             \
            );                                                                      \
    }

#define STL_PARSER_CHARACTER_STRING_TYPE____LONG_VARCHAR_LENGTH( )                  \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeCharTypeName( param,                          \
                                                    a1,                             \
                                                    QLP_GET_LAST_POS( a2 ) - a1,    \
                                                    DTL_TYPE_LONGVARCHAR,           \
                                                    a1,                             \
                                                    NULL,                           \
                                                    NULL,                           \
                                                    0 )                             \
            );                                                                      \
    }



/**
 * character_length : qlpCharLength
 */

#define STL_PARSER_CHARACTER_LENGTH____LENGTH( )                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeCharLength( param,                                \
                                                  a1,                                   \
                                                  DTL_STRING_LENGTH_UNIT_NA,            \
                                                  a1,                                   \
                                                  v1 ) );                               \
    }

#define STL_PARSER_CHARACTER_LENGTH____LENGTH_UNIT( )                           \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeCharLength( param,                        \
                                                  a1,                           \
                                                  QLP_GET_INT_VALUE( v2 ),      \
                                                  a2,                           \
                                                  v1 ) );                       \
    }


/**
 * national_character_string_type
 */

#define STL_PARSER_NATIONAL_CHARACTER_STRING_TYPE____( )    \
    {                                                       \
        STL_PARSER_NOT_SUPPORT( a1 );                       \
    }


/**
 * binary_string_type : qlpTypeName
 */

#define STL_PARSER_BINARY_STRING_TYPE____BINARY( )                      \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,            \
                                                      a1,               \
                                                      stlStrlen( v1 ),  \
                                                      DTL_TYPE_BINARY,  \
                                                      a1,               \
                                                      NULL )            \
            );                                                          \
    }

#define STL_PARSER_BINARY_STRING_TYPE____BINARY_LENGTH( )               \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,            \
                                                      a1,               \
                                                      a4 - a1 + 1,      \
                                                      DTL_TYPE_BINARY,  \
                                                      a1,               \
                                                      v3 )              \
            );                                                          \
    }

#define STL_PARSER_BINARY_STRING_TYPE____VARBINARY_NON_PRECISION( )         \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,                \
                                                      a1,                   \
                                                      stlStrlen( v1 ),      \
                                                      DTL_TYPE_VARBINARY,   \
                                                      a1,                   \
                                                      NULL )                \
            );                                                              \
    }

#define STL_PARSER_BINARY_STRING_TYPE____BINARY_VARYING_NON_PRECISION( )    \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,                \
                                                      a1,                   \
                                                      a2 - a1 + 1,          \
                                                      DTL_TYPE_VARBINARY,   \
                                                      a1,                   \
                                                      NULL )                \
            );                                                              \
    }

#define STL_PARSER_BINARY_STRING_TYPE____BINARY_VARYING_LENGTH( )               \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,                    \
                                                      a1,                       \
                                                      a5 - a1 + 1,              \
                                                      DTL_TYPE_VARBINARY,       \
                                                      a1,                       \
                                                      v4 )                      \
            );                                                                  \
    }

#define STL_PARSER_BINARY_STRING_TYPE____VARBINARY_LENGTH( )                    \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,                    \
                                                      a1,                       \
                                                      a4 - a1 + 1,              \
                                                      DTL_TYPE_VARBINARY,       \
                                                      a1,                       \
                                                      v3 )                      \
            );                                                                  \
    }

#define STL_PARSER_BINARY_STRING_TYPE____LONG_BINARY_VARYING_LENGTH( )              \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,                        \
                                                      a1,                           \
                                                      QLP_GET_LAST_POS( a3 ) - a1,  \
                                                      DTL_TYPE_LONGVARBINARY,       \
                                                      a1,                           \
                                                      NULL )                        \
            );                                                                      \
    }

#define STL_PARSER_BINARY_STRING_TYPE____LONG_VARBINARY_LENGTH( )                   \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeBinaryTypeName( param,                        \
                                                      a1,                           \
                                                      QLP_GET_LAST_POS( a2 ) - a1,  \
                                                      DTL_TYPE_LONGVARBINARY,       \
                                                      a1,                           \
                                                      NULL )                        \
            );                                                                      \
    }


/**
 * numeric_type : qlpTypeName
 */

#define STL_PARSER_NUMERIC_TYPE____EXACT( )     \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_NUMERIC_TYPE____APPROXIMATE( )       \
    {                                                   \
        STL_PARSER_CHECK( v0 = v1 );                    \
    }

#define STL_PARSER_NUMERIC_TYPE____NATIVE( )    \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

/**
 * exact_numeric_type : qlpTypeName
 */

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMERIC( )                                     \
    {                                                                                   \
        qlpValue  * sValue =                                                            \
            qlpMakeNumberConstant( param,                                               \
                                   0,                                                   \
                                   DTL_DECIMAL_NUMERIC_DEFAULT_PRECISION_STRING );      \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_NUMBER,                 \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_DECIMAL,  \
                                                       sValue,                          \
                                                       NULL )                           \
            );                                                                          \
    }

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMBER( )                                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_NUMBER,                 \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_DECIMAL,  \
                                                       NULL,                            \
                                                       NULL )                           \
            );                                                                          \
    }

#define STL_PARSER_EXACT_NUMERIC_TYPE____DECIMAL_SMALLINT( )                            \
    {                                                                                   \
        qlpValue * sValue =                                                             \
            qlpMakeNumberConstant( param,                                               \
                                   0,                                                   \
                                   DTL_DECIMAL_SMALLINT_DEFAULT_PRECISION_STRING );     \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_NUMBER,                 \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_DECIMAL,  \
                                                       sValue,                          \
                                                       NULL ) );                        \
    }

#define STL_PARSER_EXACT_NUMERIC_TYPE____DECIMAL_INTEGER( )                             \
    {                                                                                   \
        qlpValue * sValue =                                                             \
            qlpMakeNumberConstant( param,                                               \
                                   0,                                                   \
                                   DTL_DECIMAL_INTEGER_DEFAULT_PRECISION_STRING );      \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_NUMBER,                 \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_DECIMAL,  \
                                                       sValue,                          \
                                                       NULL ) );                        \
    }

#define STL_PARSER_EXACT_NUMERIC_TYPE____DECIMAL_BIGINT( )                              \
    {                                                                                   \
        qlpValue * sValue =                                                             \
            qlpMakeNumberConstant( param,                                               \
                                   0,                                                   \
                                   DTL_DECIMAL_BIGINT_DEFAULT_PRECISION_STRING );       \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_NUMBER,                 \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_DECIMAL,  \
                                                       sValue,                          \
                                                       NULL ) );                        \
    }

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMERIC_PRECISION( )                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       a4 - a1 + 1,                     \
                                                       DTL_TYPE_NUMBER,                 \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_DECIMAL,  \
                                                       v3,                              \
                                                       NULL )                           \
            );                                                                          \
    }

#define STL_PARSER_EXACT_NUMERIC_TYPE____NUMERIC_PRECISION_SCALE( )                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       a6 - a1 + 1,                     \
                                                       DTL_TYPE_NUMBER,                 \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_DECIMAL,  \
                                                       v3,                              \
                                                       v5 )                             \
            );                                                                          \
    }



/**
 * precision
 */

#define STL_PARSER_PRECISION____( )             \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * scale
 */

#define STL_PARSER_SCALE____( )                 \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * approximate_numeric_type : qlpTypeName
 */

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____FLOAT()                                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_FLOAT,                  \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_BINARY,   \
                                                       NULL,                            \
                                                       NULL )                           \
            );                                                                          \
    }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____FLOAT_PRECISION()                        \
    {                                                                                   \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_FLOAT,                  \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_BINARY,   \
                                                       v3,                              \
                                                       NULL )                           \
            );                                                                          \
    }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____DECIMAL_REAL( )                          \
    {                                                                                   \
        qlpValue * sValue =                                                             \
            qlpMakeNumberConstant( param,                                               \
                                   0,                                                   \
                                   DTL_DECIMAL_REAL_DEFAULT_PRECISION_STRING );         \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_FLOAT,                  \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_BINARY,   \
                                                       sValue,                          \
                                                       NULL )                           \
            );                                                                          \
    }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____DECIMAL_DOUBLE_PRECISION( )              \
    {                                                                                   \
        qlpValue * sValue =                                                             \
            qlpMakeNumberConstant( param,                                               \
                                   0,                                                   \
                                   DTL_DECIMAL_DOUBLE_DEFAULT_PRECISION_STRING );       \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_FLOAT,                  \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_BINARY,   \
                                                       sValue,                          \
                                                       NULL )                           \
            );                                                                          \
    }

#define STL_PARSER_APPROXIMATE_NUMERIC_TYPE____DECIMAL_DOUBLE( )                        \
    {                                                                                   \
        qlpValue * sValue =                                                             \
            qlpMakeNumberConstant( param,                                               \
                                   0,                                                   \
                                   DTL_DECIMAL_DOUBLE_DEFAULT_PRECISION_STRING );       \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeNumericTypeName( param,                           \
                                                       a1,                              \
                                                       stlStrlen( v1 ),                 \
                                                       DTL_TYPE_FLOAT,                  \
                                                       a1,                              \
                                                       DTL_NUMERIC_PREC_RADIX_BINARY,   \
                                                       sValue,                          \
                                                       NULL )                           \
            );                                                                          \
    }

/**
 * native_numeric_type
 */

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_SMALLINT()                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStaticTypeName( param,                    \
                                                      a1,                       \
                                                      stlStrlen( v1 ),          \
                                                      DTL_TYPE_NATIVE_SMALLINT, \
                                                      a1 ) );                   \
    }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_INTEGER()                      \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStaticTypeName( param,                    \
                                                      a1,                       \
                                                      stlStrlen( v1 ),          \
                                                      DTL_TYPE_NATIVE_INTEGER,  \
                                                      a1 ) );                   \
    }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_BIGINT()                       \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStaticTypeName( param,                    \
                                                      a1,                       \
                                                      stlStrlen( v1 ),          \
                                                      DTL_TYPE_NATIVE_BIGINT,   \
                                                      a1 ) );                   \
    }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_REAL()                     \
    {                                                                       \
        STL_PARSER_CHECK( v0 = qlpMakeStaticTypeName( param,                \
                                                      a1,                   \
                                                      stlStrlen( v1 ),      \
                                                      DTL_TYPE_NATIVE_REAL, \
                                                      a1 ) );               \
    }

#define STL_PARSER_NATIVE_NUMERIC_TYPE____NATIVE_DOUBLE()                       \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeStaticTypeName( param,                    \
                                                      a1,                       \
                                                      stlStrlen( v1 ),          \
                                                      DTL_TYPE_NATIVE_DOUBLE,   \
                                                      a1 ) );                   \
    }

/**
 * length
 */

#define STL_PARSER_LENGTH____( )                \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * boolean_type : qlpTypeName
 */

#define STL_PARSER_BOOLEAN_TYPE____( )                                  \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStaticTypeName( param,            \
                                                      a1,               \
                                                      stlStrlen( v1 ),  \
                                                      DTL_TYPE_BOOLEAN, \
                                                      a1 )              \
            );                                                          \
    }


/**
 * datetime_type
 */

#define STL_PARSER_DATETIME_TYPE____DATE( )                                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                  \
                                                        a1,                     \
                                                        stlStrlen( v1 ),        \
                                                        DTL_TYPE_DATE,          \
                                                        a1,                     \
                                                        NULL ) );               \
    }

#define STL_PARSER_DATETIME_TYPE____TIME( )                                     \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                  \
                                                        a1,                     \
                                                        stlStrlen( v1 ),        \
                                                        DTL_TYPE_TIME,          \
                                                        a1,                     \
                                                        NULL ) );               \
    }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP( )                                \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                  \
                                                        a1,                     \
                                                        stlStrlen( v1 ),        \
                                                        DTL_TYPE_TIMESTAMP,     \
                                                        a1,                     \
                                                        NULL ) );               \
    }

#define STL_PARSER_DATETIME_TYPE____TIME_PRECISION( )                   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,          \
                                                        a1,             \
                                                        a4 - a1 + 1,    \
                                                        DTL_TYPE_TIME,  \
                                                        a1,             \
                                                        v3 ) );         \
    }

#define STL_PARSER_DATETIME_TYPE____TIME_WITH( )                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a2 ) - a1,    \
                                                        DTL_TYPE_TIME_WITH_TIME_ZONE,   \
                                                        a1,                             \
                                                        NULL ) );                       \
    }

#define STL_PARSER_DATETIME_TYPE____TIME_WITHOUT( )                                     \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a4 ) - a1,    \
                                                        DTL_TYPE_TIME,                  \
                                                        a1,                             \
                                                        NULL ) );                       \
    }

#define STL_PARSER_DATETIME_TYPE____TIME_PRECISION_WITH( )                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a5 ) - a1,    \
                                                        DTL_TYPE_TIME_WITH_TIME_ZONE,   \
                                                        a1,                             \
                                                        v3 ) );                         \
    }

#define STL_PARSER_DATETIME_TYPE____TIME_PRECISION_WITHOUT( )                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a7 ) - a1,    \
                                                        DTL_TYPE_TIME,                  \
                                                        a1,                             \
                                                        v3 ) );                         \
    }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_PRECISION( )                      \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                  \
                                                        a1,                     \
                                                        a4 - a1 + 1,            \
                                                        DTL_TYPE_TIMESTAMP,     \
                                                        a1,                     \
                                                        v3 ) );                 \
    }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_WITH( )                                           \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                                  \
                                                        a1,                                     \
                                                        QLP_GET_LAST_POS( a2 ) - a1,            \
                                                        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      \
                                                        a1,                                     \
                                                        NULL ) );                               \
    }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_WITHOUT( )                                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a4 ) - a1,    \
                                                        DTL_TYPE_TIMESTAMP,             \
                                                        a1,                             \
                                                        NULL ) );                       \
    }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_PRECISION_WITH( )                                 \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                                  \
                                                        a1,                                     \
                                                        QLP_GET_LAST_POS( a5 ) - a1,            \
                                                        DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,      \
                                                        a1,                                     \
                                                        v3 ) );                                 \
    }

#define STL_PARSER_DATETIME_TYPE____TIMESTAMP_PRECISION_WITHOUT( )                      \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeTypeName( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a7 ) - a1,    \
                                                        DTL_TYPE_TIMESTAMP,             \
                                                        a1,                             \
                                                        v3 ) );                         \
    }


/**
 * interval_type : @todo node length를 v2로부터 얻어야 한다.
 */

#define STL_PARSER_INTERVAL_TYPE____( )                                                 \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntervalTypeName( param,                          \
                                                        a1,                             \
                                                        0,                              \
                                                        a1,                             \
                                                        v2 ) );                         \
    }

/**
 * rowid_type
 */

#define STL_PARSER_ROWID_TYPE____()                                                \
    {                                                                              \
        STL_PARSER_CHECK( v0 = qlpMakeStaticTypeName( param,                       \
                                                      a1,                          \
                                                      stlStrlen( v1 ),             \
                                                      DTL_TYPE_ROWID,              \
                                                      a1 ) );                      \
    }

/**
 * binary_string_literal
 */

#define STL_PARSER_BINARY_STRING_LITERAL____( )             \
    {                                                                   \
        qlpValue  * sValueNode = NULL;                                  \
        STL_PARSER_CHECK( sValueNode = qlpMakeBitStrConstant( param,       \
                                                           a1,          \
                                                           v1 ) );      \
                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                  \
                                                a1,                     \
                                                sValueNode ) );         \
    }


/**
 * datetime_literal
 */

#define STL_PARSER_DATETIME_LITERAL____DATE( )                  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,          \
                                                a1,             \
                                                v1 ) );         \
    }

#define STL_PARSER_DATETIME_LITERAL____TIME( )                  \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,          \
                                                a1,             \
                                                v1 ) );         \
    }

#define STL_PARSER_DATETIME_LITERAL____TIMESTAMP( )             \
    {                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,          \
                                                a1,             \
                                                v1 ) );         \
    }


/**
 * date_literal
 */

#define STL_PARSER_DATE_LITERAL____( )                                               \
    {                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                       \
                                                        a2,                          \
                                                        QLL_BNF_DATE_CONSTANT_TYPE,  \
                                                        v2 ) );                      \
    }

#define STL_PARSER_DATE_LITERAL____ESCAPE_SEQUENCE_DATE( )                           \
    {                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                       \
                                                        a3,                          \
                                                        QLL_BNF_DATE_CONSTANT_TYPE,  \
                                                        v3 ) );                      \
    }


/**
 * time_literal
 */

#define STL_PARSER_TIME_LITERAL____( )                                               \
    {                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                       \
                                                        a2,                          \
                                                        QLL_BNF_TIME_CONSTANT_TYPE,  \
                                                        v2 ) );                      \
    }

#define STL_PARSER_TIME_LITERAL____ESCAPE_SEQUENCE_TIME( )                           \
    {                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                       \
                                                        a3,                          \
                                                        QLL_BNF_TIME_CONSTANT_TYPE,  \
                                                        v3 ) );                      \
    }

#define STL_PARSER_TIME_WITH_LITERAL____( )                                             \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                          \
                                                        a3,                             \
                                                        QLL_BNF_TIME_TZ_CONSTANT_TYPE,  \
                                                        v3 ) );                         \
    }


/**
 * timestamp_literal
 */

#define STL_PARSER_TIMESTAMP_LITERAL____( )                                               \
    {                                                                                     \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                            \
                                                        a2,                               \
                                                        QLL_BNF_TIMESTAMP_CONSTANT_TYPE,  \
                                                        v2 ) );                           \
    }

#define STL_PARSER_TIMESTAMP_LITERAL____ESCAPE_SEQUENCE_TIMESTAMP( )                      \
    {                                                                                     \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                            \
                                                        a3,                               \
                                                        QLL_BNF_TIMESTAMP_CONSTANT_TYPE,  \
                                                        v3 ) );                           \
    }

#define STL_PARSER_TIMESTAMP_WITH_LITERAL____( )                                             \
    {                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeDateTimeConstant( param,                               \
                                                        a3,                                  \
                                                        QLL_BNF_TIMESTAMP_TZ_CONSTANT_TYPE,  \
                                                        v3 ) );                              \
    }

/**
 * interval_literal
 */

#define STL_PARSER_INTERVAL_LITERAL____INTERVAL( )                               \
    {                                                                            \
        qlpTypeName  * sTypeName = NULL;                                         \
        qlpTypeCast  * sTypeCast = NULL;                                         \
        STL_PARSER_CHECK( sTypeName = qlpMakeIntervalTypeName( param,            \
                                                               a1,               \
                                                               0,                \
                                                               a1,               \
                                                               v3 ) );           \
        STL_PARSER_CHECK( sTypeCast = qlpMakeTypeCast( param,                    \
                                                       a1,                       \
                                                       v2,                       \
                                                       sTypeName ) );            \
        STL_PARSER_CHECK( v0 = (qllNode *) sTypeCast );                          \
    }


/**
 * interval_string
 */

#define STL_PARSER_INTERVAL_STRING____( )                                   \
    {                                                                       \
        STL_PARSER_CHECK( qlpCheckIntervalLiteralEmptyString( param,        \
                                                              a1,           \
                                                              v1 ) );       \
                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param,                   \
                                                   a1,                      \
                                                   v1 ) );                  \
    }

/**
 * unquoted_interval_string
 */

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____YEAR_MONTH( )     \
    {                                                           \
    }

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____DAY_TIME( )       \
    {                                                           \
    }

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____SIGN_YEAR_MONTH( )        \
    {                                                                   \
    }

#define STL_PARSER_UNQUOTED_INTERVAL_STRING____SIGN_DAY_TIME( )  \
    {                                                           \
    }


/**
 * year_month_literal
 */

#define STL_PARSER_YEAR_MONTH_LITERAL____YEAR_MONTH( )   \
    {                                                   \
    }


/**
 * day_time_literal
 */

#define STL_PARSER_DAY_TIME_LITERAL____DAY_TIME( )       \
    {                                                   \
    }

#define STL_PARSER_DAY_TIME_LITERAL____TIME( )   \
    {                                           \
    }


/**
 * day_time_interval
 */

#define STL_PARSER_DAY_TIME_INTERVAL____DAY( )   \
    {                                           \
    }

#define STL_PARSER_DAY_TIME_INTERVAL____DAY_HOUR( )      \
    {                                                   \
    }

#define STL_PARSER_DAY_TIME_INTERVAL____DAY_HOUR_MINUTE( )       \
    {                                                           \
    }

#define STL_PARSER_DAY_TIME_INTERVAL____DAY_HOUR_MINUTE_SECOND( )        \
    {                                                                   \
    }


/**
 * time_interval
 */

#define STL_PARSER_TIME_INTERVAL____HOUR_MINUTE( )       \
    {                                                   \
    }

#define STL_PARSER_TIME_INTERVAL____HOUR_MINUTE_SECOND( )        \
    {                                                           \
    }


/**
 * seconds_value
 */

#define STL_PARSER_SECONDS_VALUE____INTEGER( )   \
    {                                           \
    }

#define STL_PARSER_SECONDS_VALUE____INTEGER_FRACTION( )  \
    {                                                   \
    }


/**
 * seconds_integer_value
 */

#define STL_PARSER_SECONDS_INTEGER_VALUE____( )  \
    {                                           \
    }


/**
 * seconds_fraction
 */

#define STL_PARSER_SECONDS_FRACTION____( )       \
    {                                           \
    }


/**
 * datetime_value
 */

#define STL_PARSER_DATETIME_VALUE____( )         \
    {                                           \
    }


/**
 * boolean_literal : qlpConstant
 */

#define STL_PARSER_BOOLEAN_LITERAL____TRUE( )                                   \
    {                                                                           \
        qlpValue  * sValueNode = NULL;                                          \
        STL_PARSER_CHECK( sValueNode = qlpMakeIntParamConstant( param,          \
                                                                a1,             \
                                                                STL_TRUE ) );   \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                          \
                                                a1,                             \
                                                sValueNode ) );                 \
    }

#define STL_PARSER_BOOLEAN_LITERAL____FALSE( )                                  \
    {                                                                           \
        qlpValue  * sValueNode = NULL;                                          \
        STL_PARSER_CHECK( sValueNode = qlpMakeIntParamConstant( param,          \
                                                                a1,             \
                                                                STL_FALSE ) );  \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                          \
                                                a1,                             \
                                                sValueNode ) );                 \
    }

#define STL_PARSER_BOOLEAN_LITERAL____UNKNOWN( )                                        \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeConstant( param,                                  \
                                                a1,                                     \
                                                qlpMakeNullConstant( param, a1 ) ) );   \
    }

/**
 * row_value_predicand
 */
#define STL_PARSER_ROW_VALUE_PREDICAND____CONSTRUCTOR_VALUE( )  \
    {                                                           \
        STL_PARSER_CHECK( v0 = v1 );                            \
    }

#define STL_PARSER_ROW_VALUE_PREDICAND____CONSTRUCTOR_NONPARENT_VALUE( )   \
    {                                                                       \
        STL_PARSER_CHECK( v0 = v1 );                                        \
    }

/**
 * row_value_constructor_predicate
 */
#define STL_PARSER_ROW_VALUE_CONSTRUCTOR____( ) \
    {                                                                       \
        STL_PARSER_CHECK( v0 = v2 );                                        \
    }

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR____EXPLICIT_VALUE( )               \
    {                                                                       \
        STL_PARSER_CHECK( v0 = v1 );                                        \
    }

/**
 * explicit_row_value_constructor
 */

#define STL_PARSER_ROW_VALUE____CONSTRUCTOR( )                                  \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                \
        STL_PARSER_CHECK( v0 = sList );                                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v2 ) );           \
                                                                                \
        STL_PARSER_CHECK( qlpAddListToList( (qlpList *)v0, (qlpList *)v4 ) );   \
    }

/**
 * row_value_constructor_element_list
 */

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR_LIST____ELEMENT( )              \
    {                                                                   \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v1 ) );   \
    }                                                         

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR_LIST____ELEMENT_LIST( )         \
    {                                                                   \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, v3 ) );      \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }

/**
 * row_value_constructor_element
 */

#define STL_PARSER_ROW_VALUE_CONSTRUCTOR____ELEMENT( )        \
    {                                                         \
        STL_PARSER_CHECK( v0 = v1 );                          \
    }

/**
 * general_value_specification : qllNode
 */

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____HOST_PARAMETER( )                 \
    {                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        STL_PARSER_CHECK(                                                           \
            qlpAddPtrValueToList( param,                                            \
                                  sList,                                            \
                                  qlpMakeStrConstant( param, a1, v1 ) ) );          \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, sList, v2 ) );               \
                                                                                    \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeParameter( param,                 \
                                                             a1,                    \
                                                             QLP_PARAM_TYPE_HOST,   \
                                                             KNL_BIND_TYPE_IN,      \
                                                             a1,                    \
                                                             sList ) );             \
    }


#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____COLUMN( )                 \
    {                                                                       \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeColumnRef( param,         \
                                                             a1,            \
                                                             v1,            \
                                                             STL_FALSE ) ); \
    }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____COLUMN_WITH_OUTER_JOIN_OPERATOR( )    \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeColumnRef( param,                     \
                                                             a1,                        \
                                                             v1,                        \
                                                             STL_TRUE ) );              \
    }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____ROWID_COLUMN( )           \
    {                                                                       \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeColumnRef( param,         \
                                                             a1,            \
                                                             v1,            \
                                                             STL_FALSE ) ); \
    }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____ROWID_COLUMN_WITH_OUTER_JOIN_OPERATOR( )  \
    {                                                                                       \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeColumnRef( param,                         \
                                                             a1,                            \
                                                             v1,                            \
                                                             STL_TRUE ) );                  \
    }

#define STL_PARSER_GENERAL_VALUE_SPECIFICATION____DYNAMIC_PARAMETER( )                  \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = (qllNode *) qlpMakeParameter( param,                     \
                                                             a1,                        \
                                                             QLP_PARAM_TYPE_DYNAMIC,    \
                                                             KNL_BIND_TYPE_IN,          \
                                                             a1,                        \
                                                             NULL ) );                  \
    }


/**
 * indicator_paramter : qlpValue : string
 */

#define STL_PARSER_INDICATOR_PARAMTER____NO_INDICATOR( )        \
    {                                                           \
        STL_PARSER_CHECK( v0 = NULL );                          \
    }

#define STL_PARSER_INDICATOR_PARAMTER____EMPTY_INDICATOR( )             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a1, v1 ) );   \
    }

#define STL_PARSER_INDICATOR_PARAMTER____INDICATOR( )                   \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeStrConstant( param, a2, v2 ) );   \
    }


/**
 * predicate : qllNode
 */

#define STL_PARSER_PREDICATE____COMPARISON( )   \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_PREDICATE____BETWEEN( )      \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_PREDICATE____LIKE( )         \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_PREDICATE____IN( )           \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }

#define STL_PARSER_PREDICATE____NULL( )         \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * comparison_predicate : qlpFunction
 */

#define STL_PARSER_COMPARISON_PREDICATE____VALUE_VALUE( )                               \
    {                                                                                   \
                                                                                        \
        if( ( ((qllNode*)v1)->mType == QLL_SUB_TABLE_NODE_TYPE ) &&                     \
            ( ((qllNode*)v3)->mType == QLL_SUB_TABLE_NODE_TYPE ) )                      \
        {                                                                               \
            qlpRowExpr   * sRow1     = NULL;                                            \
            qlpRowExpr   * sRow2     = NULL;                                            \
            qlpList      * sRowArg1  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
            qlpList      * sRowArg2  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
            qlpList      * sListArg1 = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
            qlpList      * sListArg2 = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
            qlpListCol   * sListCol1 = NULL;                                            \
            qlpListCol   * sListCol2 = NULL;                                            \
            qlpList      * sFuncArg = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sRowArg1,                           \
                                                    v1 ) );                             \
                                                                                        \
            STL_PARSER_CHECK( sRow1 = qlpMakeRowExpr( param,                            \
                                                      a1,                               \
                                                      sRowArg1 ) );                     \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sListArg1,                          \
                                                    sRow1 ) );                          \
                                                                                        \
            STL_PARSER_CHECK( sListCol1 = qlpMakeListColumn( param,                     \
                                                             a1,                        \
                                                             QLP_GET_INT_VALUE( v2 ),   \
                                                             DTL_LIST_PREDICATE_LEFT,   \
                                                             sListArg1 ) );             \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sRowArg2,                           \
                                                    v3 ) );                             \
                                                                                        \
            STL_PARSER_CHECK( sRow2 = qlpMakeRowExpr( param,                            \
                                                      a3,                               \
                                                      sRowArg2 ) );                     \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sListArg2,                          \
                                                    sRow2 ) );                          \
                                                                                        \
            STL_PARSER_CHECK( sListCol2 = qlpMakeListColumn( param,                     \
                                                             a3,                        \
                                                             QLP_GET_INT_VALUE( v2 ),   \
                                                             DTL_LIST_PREDICATE_RIGHT,  \
                                                             sListArg2 ) );             \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sFuncArg,                           \
                                                    sListCol2 ) );                      \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sFuncArg,                           \
                                                    sListCol1 ) );                      \
                                                                                        \
            STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a3 ) - a1,    \
                                                        a2,                             \
                                                        QLP_GET_INT_VALUE( v2 ),        \
                                                        sFuncArg ) );                   \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sList,                              \
                                                    v1 ) );                             \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sList,                              \
                                                    v3 ) );                             \
                                                                                        \
            stlInt32  sLen = 0;                                                         \
            switch ( ((qllNode*)v3)->mType )                                            \
            {                                                                           \
                case QLL_VALUE_EXPR_TYPE:                                               \
                case QLL_FUNCTION_TYPE:                                                 \
                    {                                                                   \
                        sLen = ((qlpValueExpr*)v3)->mNodePos -                          \
                            ((qlpValueExpr*)v1)->mNodePos +                             \
                            ((qlpValueExpr*)v3)->mNodeLen;                              \
                        break;                                                          \
                    }                                                                   \
                default:                                                                \
                    {                                                                   \
                        sLen = QLP_GET_LAST_POS( a3 ) - a1;                             \
                        break;                                                          \
                    }                                                                   \
            }                                                                           \
                                                                                        \
            STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                              \
                                                    QLP_GET_INT_VALUE( v2 ),            \
                                                    a1,                                 \
                                                    sLen,                               \
                                                    sList ) );                          \
        }                                                                               \
    }

#define STL_PARSER_COMPARISON_PREDICATE____VALUE_ROW( )                                 \
    {                                                                                   \
        qlpList      * sRowArg  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpList      * sListArg = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpRowExpr   * sRow     = NULL;                                                 \
        qlpListCol   * sListCol = NULL;                                                 \
        qlpList      * sFuncArg = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sRowArg,                                \
                                                v1 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                                 \
                                                 a1,                                    \
                                                 sRowArg ) );                           \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sListArg,                               \
                                                sRow ) );                               \
                                                                                        \
        STL_PARSER_CHECK( sListCol = qlpMakeListColumn( param,                          \
                                                        a1,                             \
                                                        ((qlpListCol*)v2)->mFuncId,     \
                                                        DTL_LIST_PREDICATE_LEFT,        \
                                                        sListArg ) );                   \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sFuncArg,                               \
                                                v2 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sFuncArg,                               \
                                                sListCol ) );                           \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                              \
                                                    a1,                                 \
                                                    QLP_GET_LAST_POS (a2 ) - a1,        \
                                                    a2,                                 \
                                                    ((qlpListCol*)v2)->mFuncId,         \
                                                    sFuncArg ) );                       \
    }

#define STL_PARSER_COMPARISON_PREDICATE____ROW_VALUE( )                                 \
    {                                                                                   \
            qlpRowExpr   * sRow1     = NULL;                                            \
            qlpRowExpr   * sRow2     = NULL;                                            \
            qlpList      * sRowArg1  = v1;                                              \
            qlpList      * sRowArg2  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
            qlpList      * sListArg1 = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
            qlpList      * sListArg2 = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
            qlpListCol   * sListCol1 = NULL;                                            \
            qlpListCol   * sListCol2 = NULL;                                            \
            qlpList      * sFuncArg = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
                                                                                        \
            STL_PARSER_CHECK( sRow1 = qlpMakeRowExpr( param,                            \
                                                      a1,                               \
                                                      sRowArg1 ) );                     \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sListArg1,                          \
                                                    sRow1 ) );                          \
                                                                                        \
            STL_PARSER_CHECK( sListCol1 = qlpMakeListColumn( param,                     \
                                                             a1,                        \
                                                             QLP_GET_INT_VALUE( v2 ),   \
                                                             DTL_LIST_PREDICATE_LEFT,   \
                                                             sListArg1 ) );             \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sRowArg2,                           \
                                                    v3 ) );                             \
                                                                                        \
            STL_PARSER_CHECK( sRow2 = qlpMakeRowExpr( param,                            \
                                                      a3,                               \
                                                      sRowArg2 ) );                     \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sListArg2,                          \
                                                    sRow2 ) );                          \
                                                                                        \
            STL_PARSER_CHECK( sListCol2 = qlpMakeListColumn( param,                     \
                                                             a3,                        \
                                                             QLP_GET_INT_VALUE( v2 ),   \
                                                             DTL_LIST_PREDICATE_RIGHT,  \
                                                             sListArg2 ) );             \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sFuncArg,                           \
                                                    sListCol2 ) );                      \
                                                                                        \
            STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                    sFuncArg,                           \
                                                    sListCol1 ) );                      \
                                                                                        \
            STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                          \
                                                        a1,                             \
                                                        QLP_GET_LAST_POS( a3 ) - a1,    \
                                                        a2,                             \
                                                        QLP_GET_INT_VALUE( v2 ),        \
                                                        sFuncArg ) );                   \
    }

#define STL_PARSER_COMPARISON_PREDICATE____ROW_ROW( )                                   \
    {                                                                                   \
        qlpRowExpr   * sRow      = NULL;                                                \
        qlpList      * sList     = qlpMakeList( param, QLL_POINTER_LIST_TYPE );         \
        qlpListCol   * sListCol  = NULL;                                                \
        qlpList      * sFuncArg  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );         \
                                                                                        \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                                 \
                                                 a1,                                    \
                                                 v1 ) );                                \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                sRow ) );                               \
                                                                                        \
        STL_PARSER_CHECK( sListCol = qlpMakeListColumn( param,                          \
                                                        a1,                             \
                                                        ((qlpListCol*)v2)->mFuncId,     \
                                                        DTL_LIST_PREDICATE_LEFT,        \
                                                        sList ) );                      \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sFuncArg,                               \
                                                v2 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sFuncArg,                               \
                                                sListCol ) );                           \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                              \
                                                    a1,                                 \
                                                    QLP_GET_LAST_POS( a2 ) - a1,        \
                                                    a2,                                 \
                                                    ((qlpListCol*)v2)->mFuncId,         \
                                                    sFuncArg ) );                       \
    }


/**
 * comparison_predicate_part_2
 */

#define STL_PARSER_COMP_PREDICATE_PART_2____( )                                     \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                            \
                                                  a1,                               \
                                                  QLP_GET_INT_VALUE( v1 ),          \
                                                  ((qlpListCol*)v2)->mPredicate,    \
                                                  ((qlpListCol*)v2)->mArgs ) );     \
    }


/**
 * row_predicate_value
 */

#define STL_PARSER_ROW_PREDICATE____PREDICATE_VALUE( )                          \
    {                                                                           \
        qlpList     * sColList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpRowExpr  * sRow = NULL;                                              \
                                                                                \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                         \
                                                 a1,                            \
                                                 v1 ) );                        \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sColList,                       \
                                                sRow ) );                       \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                        \
                                                  a1,                           \
                                                  KNL_BUILTIN_FUNC_MAX,         \
                                                  DTL_LIST_PREDICATE_RIGHT,     \
                                                  sColList ) );                 \
    }


/**
 * comp_op : qlpValue : integer
 */

#define STL_PARSER_COMP_OP____EQUAL( )                                          \
    {                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                          \
                                                a1,                              \
                                                KNL_BUILTIN_FUNC_IS_EQUAL ) );  \
    }

#define STL_PARSER_COMP_OP____NOT_EQUAL( )                                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                a1,                                      \
                                                KNL_BUILTIN_FUNC_IS_NOT_EQUAL ) );      \
    }

#define STL_PARSER_COMP_OP____LESS_THAN( )                                              \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                a1,                                      \
                                                KNL_BUILTIN_FUNC_IS_LESS_THAN ) );      \
    }

#define STL_PARSER_COMP_OP____LESS_THAN_EQUAL( )                                                \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                          \
                                                a1,                                              \
                                                KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL ) );        \
    }

#define STL_PARSER_COMP_OP____GREATER_THAN( )                                           \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                  \
                                                a1,                                      \
                                                KNL_BUILTIN_FUNC_IS_GREATER_THAN ) );   \
    }

#define STL_PARSER_COMP_OP____GREATER_THAN_EQUAL( )                                             \
    {                                                                                           \
        STL_PARSER_CHECK( v0 = qlpMakeIntParamConstant( param,                                          \
                                                a1,                                              \
                                                KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL ) );     \
    }


/**
 * between_predicate : qlpFunction
 */

#define STL_PARSER_BETWEEN_PREDICATE____( )                                                     \
    {                                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                          \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                v1 ) );                                         \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                v1 ) );                                         \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                ((qlpBetween *)v2)->mElem1 ) );                 \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                ((qlpBetween *)v2 )->mElem2 ) );                \
                                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                          \
                                                ((qlpBetween*)v2)->mFuncID,                     \
                                                a1,                                             \
                                                QLP_GET_LAST_POS( a2 ) - a1,                    \
                                                sList ));                                       \
                                                                                                \
    }

#define STL_PARSER_NOT_BETWEEN_PREDICATE____( )                                                 \
    {                                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                          \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                v1 ) );                                         \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                v1 ) );                                         \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                ((qlpBetween *)v3)->mElem1 ) );                 \
                                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                          \
                                                sList,                                          \
                                                ((qlpBetween *)v3 )->mElem2 ) );                \
                                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                          \
                                                KNL_BUILTIN_FUNC_NOT_BETWEEN_SYMMETRIC,         \
                                                a1,                                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,                    \
                                                sList ));                                       \
    }

#define STL_PARSER_BETWEEN_PREDICATE____REWRITE_ASYM()                                              \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
        qlpList * sGreatEqualList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                    \
        qlpList * sLessEqualList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                     \
                                                                                                    \
        qlpFunction * sGreatEqualFunc;                                                              \
        qlpFunction * sLessEqualFunc;                                                               \
                                                                                                    \
        /* (v1 >= elem1) 생성 */                                                                    \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sGreatEqualList,                                    \
                                                v1 ) );                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sGreatEqualList,                                    \
                                                ((qlpBetween *)v2)->mElem1 ) );                     \
                                                                                                    \
        STL_PARSER_CHECK( sGreatEqualFunc = qlpMakeFunction( param,                                 \
                                                             KNL_BUILTIN_FUNC_IS_GREATER_THAN_EQUAL, \
                                                             a1,                                    \
                                                             QLP_GET_LAST_POS( a2 ) - a1,           \
                                                             sGreatEqualList ) );                   \
                                                                                                    \
        /* (v1 =< elem2) 생성 */                                                                    \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sLessEqualList,                                     \
                                                v1 ) );                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sLessEqualList,                                     \
                                                ((qlpBetween *)v2)->mElem2 ) );                     \
                                                                                                    \
        STL_PARSER_CHECK( sLessEqualFunc = qlpMakeFunction( param,                                  \
                                                            KNL_BUILTIN_FUNC_IS_LESS_THAN_EQUAL,    \
                                                            a1,                                     \
                                                            QLP_GET_LAST_POS( a2 ) - a1,            \
                                                            sLessEqualList ) );                     \
                                                                                                    \
        /* AND 생성 */                                                                              \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                sGreatEqualFunc ) );                                \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                sLessEqualFunc ) );                                 \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                              \
                                                KNL_BUILTIN_FUNC_AND,                               \
                                                a1,                                                 \
                                                QLP_GET_LAST_POS( a2 ) - a1 ,                       \
                                                sList ) );                                          \
    }

#define STL_PARSER_NOT_BETWEEN_PREDICATE____REWRITE_ASYM()                                          \
    {                                                                                               \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                              \
        qlpList * sGreatList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                         \
        qlpList * sLessList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );                          \
                                                                                                    \
        qlpFunction * sGreatFunc;                                                                   \
        qlpFunction * sLessFunc;                                                                    \
                                                                                                    \
                                                                                                    \
        /* (v1 < elem1) 생성 */                                                                     \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sLessList,                                          \
                                                v1 ) );                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sLessList,                                          \
                                                ((qlpBetween *)v3)->mElem1 ) );                     \
                                                                                                    \
        STL_PARSER_CHECK( sLessFunc = qlpMakeFunction( param,                                       \
                                                       KNL_BUILTIN_FUNC_IS_LESS_THAN,               \
                                                       a1,                                          \
                                                       QLP_GET_LAST_POS( a3 ) - a1,                 \
                                                       sLessList ) );                               \
                                                                                                    \
        /* (v1 > elem2) 생성 */                                                                     \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sGreatList,                                         \
                                                v1 ) );                                             \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sGreatList,                                         \
                                                ((qlpBetween *)v3)->mElem2 ) );                     \
                                                                                                    \
        STL_PARSER_CHECK( sGreatFunc = qlpMakeFunction( param,                                      \
                                                        KNL_BUILTIN_FUNC_IS_GREATER_THAN,           \
                                                        a1,                                         \
                                                        QLP_GET_LAST_POS( a3 ) - a1,                \
                                                        sGreatList ) );                             \
                                                                                                    \
        /* OR 생성 */                                                                               \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                sLessFunc ) );                                      \
                                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                              \
                                                sList,                                              \
                                                sGreatFunc ) );                                     \
                                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                                              \
                                                KNL_BUILTIN_FUNC_OR,                                \
                                                a1,                                                 \
                                                QLP_GET_LAST_POS( a3 ) - a1 ,                       \
                                                sList ) );                                          \
    }

#define STL_PARSER_BETWEEN_REWRITE_ASYMETRIC_PREDICATE____WITHOUT_ASYM()                \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeBetween( param,                                   \
                                               v2,                                      \
                                               v4,                                      \
                                               QLP_BETWEEN_TYPE_BETWEEN_ASYMMETRIC ) ); \
    }

#define STL_PARSER_BETWEEN_REWRITE_ASYMETRIC_PREDICATE____WITH_ASYM()                   \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeBetween( param,                                   \
                                               v3,                                      \
                                               v5,                                      \
                                               QLP_BETWEEN_TYPE_BETWEEN_ASYMMETRIC ) ); \
    }

/**
 * between_predicate_part_2
 */

#define STL_PARSER_BETWEEN_PREDICATE_PART_2____BETWEEN( )                               \
    {                                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeBetween( param,                                   \
                                               v3,                                      \
                                               v5,                                      \
                                               QLP_BETWEEN_TYPE_BETWEEN_SYMMETRIC ) );  \
    }



/**
 * like_predicate : qlpFunction
 */

#define STL_PARSER_LIKE_PREDICATE____( )        \
    {                                           \
        STL_PARSER_CHECK( v0 = v1 );            \
    }


/**
 * character_like_predicate : qlpFunction
 */

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____( )  \
    {                                               \
        STL_PARSER_NOT_SUPPORT( a2 );               \
    }


/**
 * character_like_predicate_part_2
 */

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____LIKE( )                          \
    {                                                                           \
        qlpList * sLikeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList * sPatternArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );\
        qlpFunction * sFunc;                                                    \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_LIKE_PATTERN,  \
                                                a3,                             \
                                                QLP_GET_LAST_POS( a3 ) - a3,    \
                                                sPatternArgList ) );            \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                sFunc ) );                      \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_LIKE,          \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,    \
                                                sLikeArgList ) );               \
    }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____NOT_LIKE( )                      \
    {                                                                           \
        qlpList * sLikeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList * sPatternArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );\
        qlpFunction * sFunc;                                                    \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v4 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_LIKE_PATTERN,  \
                                                a4,                             \
                                                QLP_GET_LAST_POS( a4 ) - a4,    \
                                                sPatternArgList ) );            \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                sFunc ) );                      \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_NOT_LIKE,      \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a4 ) - a1,    \
                                                sLikeArgList ) );               \
    }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____LIKE_ESCAPE( )                   \
    {                                                                           \
        qlpList * sLikeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList * sPatternArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );\
        qlpFunction * sFunc;                                                    \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v5 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_LIKE_PATTERN,  \
                                                a3,                             \
                                                QLP_GET_LAST_POS( a5 ) - a3,    \
                                                sPatternArgList ) );            \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                sFunc ) );                      \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_LIKE,          \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a5 ) - a1,    \
                                                sLikeArgList ) );               \
    }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____NOT_LIKE_ESCAPE( )               \
    {                                                                           \
        qlpList * sLikeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList * sPatternArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );\
        qlpFunction * sFunc;                                                    \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v4 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v6 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_LIKE_PATTERN,  \
                                                a4,                             \
                                                QLP_GET_LAST_POS( a6 ) - a4,    \
                                                sPatternArgList ) );            \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                sFunc ) );                      \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_NOT_LIKE,      \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a6 ) - a1,    \
                                                sLikeArgList ) );               \
    }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____LIKE_ESCAPE_SEQUENCE( )          \
    {                                                                           \
        qlpList * sLikeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList * sPatternArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );\
        qlpFunction * sFunc;                                                    \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v6 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_LIKE_PATTERN,  \
                                                a3,                             \
                                                QLP_GET_LAST_POS( a6 ) - a3,    \
                                                sPatternArgList ) );            \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                sFunc ) );                      \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_LIKE,          \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a7 ) - a1,    \
                                                sLikeArgList ) );               \
    }

#define STL_PARSER_CHARACTER_LIKE_PREDICATE____NOT_LIKE_ESCAPE_SEQUENCE( )      \
    {                                                                           \
        qlpList * sLikeArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList * sPatternArgList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );\
        qlpFunction * sFunc;                                                    \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v4 ) );                         \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sPatternArgList,                \
                                                v7 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sFunc = qlpMakeFunction( param,                       \
                                                KNL_BUILTIN_FUNC_LIKE_PATTERN,  \
                                                a4,                             \
                                                QLP_GET_LAST_POS( a7 ) - a4,    \
                                                sPatternArgList ) );            \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sLikeArgList,                   \
                                                sFunc ) );                      \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_NOT_LIKE,      \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a8 ) - a1,    \
                                                sLikeArgList ) );               \
    }

/**
 * in_predicate
 */
#define STL_PARSER_IN_PREDICATE____( )                                                      \
    {                                                                                       \
        qlpList      * sValue    = qlpMakeList( param, QLL_POINTER_LIST_TYPE );             \
        qlpRowExpr   * sRow      = NULL;                                                    \
        qlpList      * sList     = qlpMakeList( param, QLL_POINTER_LIST_TYPE );             \
        qlpListCol   * sListCol  = NULL;                                                    \
        qlpList      * sFuncArg  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );             \
                                                                                            \
                                                                                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                sValue,                                     \
                                                v1 ) );                                     \
                                                                                            \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                                     \
                                                 a1,                                        \
                                                 sValue ) );                                \
                                                                                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                sList,                                      \
                                                sRow ) );                                   \
                                                                                            \
        STL_PARSER_CHECK( sListCol = qlpMakeListColumn( param,                              \
                                                        a1,                                 \
                                                        ((qlpListCol*)v2)->mFuncId,         \
                                                        DTL_LIST_PREDICATE_LEFT,            \
                                                        sList ) );                          \
                                                                                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                sFuncArg,                                   \
                                                v2 ) );                                     \
                                                                                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                      \
                                                sFuncArg,                                   \
                                                sListCol ) );                               \
                                                                                            \
        STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                                  \
                                                    a1,                                     \
                                                    QLP_GET_LAST_POS( a3 ) - a1,            \
                                                    a2,                                     \
                                                    ((qlpListCol*)v2)->mFuncId,             \
                                                    sFuncArg ) );                           \
    }


#define STL_PARSER_IN_PREDICATE____LIST( )                                          \
    {                                                                               \
        qlpRowExpr   * sRow      = NULL;                                            \
        qlpList      * sList     = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
        qlpListCol   * sListCol  = NULL;                                            \
        qlpList      * sFuncArg  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
                                                                                    \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                             \
                                                 a1,                                \
                                                 v1 ) );                            \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                sRow ) );                           \
                                                                                    \
        STL_PARSER_CHECK( sListCol = qlpMakeListColumn( param,                      \
                                                        a1,                         \
                                                        ((qlpListCol*)v2)->mFuncId, \
                                                        DTL_LIST_PREDICATE_LEFT,    \
                                                        sList ) );                  \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFuncArg,                           \
                                                v2 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFuncArg,                           \
                                                sListCol ) );                       \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                          \
                                                    a1,                             \
                                                    QLP_GET_LAST_POS( a3 ) - a1,    \
                                                    a2,                             \
                                                    ((qlpListCol*)v2)->mFuncId,     \
                                                    sFuncArg ) );                   \
    }


/**
 * in_predicate_part_2
 */
#define STL_PARSER_IN_PREDICATE____IN( )                                            \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                            \
                                                  a1,                               \
                                                  KNL_BUILTIN_FUNC_IN,              \
                                                  ((qlpListCol*)v2)->mPredicate,    \
                                                  ((qlpListCol*)v2)->mArgs ) );     \
    }

#define STL_PARSER_IN_PREDICATE____NOT_IN( )                                        \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                            \
                                                  a1,                               \
                                                  KNL_BUILTIN_FUNC_NOT_IN,          \
                                                  ((qlpListCol*)v3)->mPredicate,    \
                                                  ((qlpListCol*)v3)->mArgs ) );     \
    }

/**
 * in_predicate_value
 */
#define STL_PARSER_IN_PREDICATE____PREDICATE_VALUE( )                               \
    {                                                                               \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                            \
                                                  a1,                               \
                                                  KNL_BUILTIN_FUNC_MAX,             \
                                                  DTL_LIST_PREDICATE_RIGHT,         \
                                                  (qlpList *)v2 ) );                \
    }

#define STL_PARSER_IN_PREDICATE____TABLE_SUBQUERY( )                            \
    {                                                                           \
        qlpList     * sColList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList     * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );      \
        qlpRowExpr  * sRow = NULL;                                              \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sColList,                       \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                         \
                                                 a1,                            \
                                                 sColList ) );                  \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                sRow ) );                       \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                        \
                                                  a1,                           \
                                                  KNL_BUILTIN_FUNC_MAX,         \
                                                  DTL_LIST_PREDICATE_RIGHT,     \
                                                  sList ) );                    \
    }

#define STL_PARSER_IN_PREDICATE____TABLE_SUBQUERY_PREDICATE_VALUE( )            \
    {                                                                           \
        qlpList     * sColList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpList     * sRowList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );   \
        qlpRowExpr  * sRow = NULL;                                              \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sRowList,                       \
                                                v2 ) );                         \
                                                                                \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                         \
                                                 a2,                            \
                                                 sRowList ) );                  \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sColList,                       \
                                                sRow ) );                       \
                                                                                \
        STL_PARSER_CHECK( qlpAddListToList( sColList,                           \
                                            v4 ) );                             \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                        \
                                                  a1,                           \
                                                  KNL_BUILTIN_FUNC_MAX,         \
                                                  DTL_LIST_PREDICATE_RIGHT,     \
                                                  sColList ) );                 \
    }


/**
 * in_value_list
 */
#define STL_PARSER_IN_PREDICATE____IN_SINGLE( )                             \
    {                                                                       \
        qlpList     * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );  \
        qlpRowExpr  * sRow = NULL;                                          \
                                                                            \
        STL_PARSER_CHECK( v0 = sList );                                     \
                                                                            \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                     \
                                                 a1,                        \
                                                 v1 ) );                    \
                                                                            \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                      \
                                                sList,                      \
                                                sRow ) );                   \
    }   

#define STL_PARSER_IN_PREDICATE____IN_LIST( )                           \
    {                                                                   \
        qlpRowExpr  * sRow = NULL;                                      \
                                                                        \
        STL_PARSER_CHECK( v0 = v1 );                                    \
                                                                        \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                 \
                                                 a3,                    \
                                                 v3 ) );                \
                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param, v1, sRow ) );    \
    }


/**
 * row_list
 */
#define STL_PARSER_IN_PREDICATE____ROW_SINGLE( )                        \
    {                                                                   \
        qlpList  * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE ); \
                                                                        \
        STL_PARSER_CHECK( v0 = sList );                                 \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                  \
                                                sList,                  \
                                                v1 ) );                 \
    }

#define STL_PARSER_IN_PREDICATE____ROW_LIST( )                          \
    {                                                                   \
        STL_PARSER_CHECK( v0 = v1 );                                    \
    }


/**
 * exists_predicate
 */
#define STL_PARSER_EXISTS_PREDICATE____( )                                              \
    {                                                                                   \
        qlpList     * sColList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );           \
        qlpList     * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );              \
        qlpList     * sFuncArg  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        qlpRowExpr  * sRow = NULL;                                                      \
        qlpListCol  * sListCol = NULL;                                                  \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sColList,                               \
                                                v2 ) );                                 \
                                                                                        \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                                 \
                                                 a2,                                    \
                                                 sColList ) );                          \
                                                                                        \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sList,                                  \
                                                sRow ) );                               \
                                                                                        \
        STL_PARSER_CHECK( sListCol = qlpMakeListColumn( param,                          \
                                                        a2,                             \
                                                        KNL_BUILTIN_FUNC_EXISTS,        \
                                                        DTL_LIST_PREDICATE_RIGHT,       \
                                                        sList ) );                      \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                                  \
                                                sFuncArg,                               \
                                                sListCol ) );                           \
                                                                                        \
        STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                              \
                                                    a1,                                 \
                                                    QLP_GET_LAST_POS( a2 ) - a1,        \
                                                    a1,                                 \
                                                    KNL_BUILTIN_FUNC_EXISTS,            \
                                                    sFuncArg ) );                       \
    }


/**
 * quantified_comparison_predicate
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____( )                                 \
    {                                                                               \
        qlpList      * sValue    = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
        qlpRowExpr   * sRow      = NULL;                                            \
        qlpList      * sList     = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
        qlpListCol   * sListCol  = NULL;                                            \
        qlpList      * sFuncArg  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
                                                                                    \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sValue,                             \
                                                v1 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                             \
                                                 a1,                                \
                                                 sValue ) );                        \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                sRow ) );                           \
                                                                                    \
        STL_PARSER_CHECK( sListCol = qlpMakeListColumn( param,                      \
                                                        a1,                         \
                                                        ((qlpListCol*)v2)->mFuncId, \
                                                        DTL_LIST_PREDICATE_LEFT,    \
                                                        sList ) );                  \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFuncArg,                           \
                                                v2 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFuncArg,                           \
                                                sListCol ) );                       \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                          \
                                                    a1,                             \
                                                    QLP_GET_LAST_POS( a2) - a1,     \
                                                    a2,                             \
                                                    ((qlpListCol*)v2)->mFuncId,     \
                                                    sFuncArg ) );                   \
    }


#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____LIST( )                             \
    {                                                                               \
        qlpRowExpr   * sRow      = NULL;                                            \
        qlpList      * sList     = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
        qlpListCol   * sListCol  = NULL;                                            \
        qlpList      * sFuncArg  = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
                                                                                    \
        STL_PARSER_CHECK( sRow = qlpMakeRowExpr( param,                             \
                                                 a1,                                \
                                                 v1 ) );                            \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sList,                              \
                                                sRow ) );                           \
                                                                                    \
        STL_PARSER_CHECK( sListCol = qlpMakeListColumn( param,                      \
                                                        a1,                         \
                                                        ((qlpListCol*)v2)->mFuncId, \
                                                        DTL_LIST_PREDICATE_LEFT,    \
                                                        sList ) );                  \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFuncArg,                           \
                                                v2 ) );                             \
                                                                                    \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                              \
                                                sFuncArg,                           \
                                                sListCol ) );                       \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeListFunction( param,                          \
                                                    a1,                             \
                                                    QLP_GET_LAST_POS( a2) - a1,     \
                                                    a2,                             \
                                                    ((qlpListCol*)v2)->mFuncId,     \
                                                    sFuncArg ) );                   \
    }

/**
 * quantified_comparison_predicate_part_2
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____PART_2( )                           \
    {                                                                               \
        qlpValue   * sValue = NULL;                                                 \
                                                                                    \
        STL_PARSER_CHECK( sValue = qlpMakeQuantifier( param,                        \
                                                      a1,                           \
                                                      QLP_GET_INT_VALUE( v1 ),      \
                                                      (qlpQuantifier)v2 ) );        \
                                                                                    \
        STL_PARSER_CHECK( v0 = qlpMakeListColumn( param,                            \
                                                  a1,                               \
                                                  QLP_GET_INT_VALUE( sValue ),      \
                                                  ((qlpListCol*)v3)->mPredicate,    \
                                                  ((qlpListCol*)v3)->mArgs ) );     \
    }

/**
 * quantifier
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____QUANTIFIER_ALL( )   \
    {                                                               \
        STL_PARSER_CHECK( v0 = v1 );                                \
    }

#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____QUANTIFIER_SOME( )   \
    {                                                                \
        STL_PARSER_CHECK( v0 = v1 );                                 \
    }

/**
 * all
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____ALL( )          \
    {                                                           \
        STL_PARSER_CHECK( v0 = (qllNode*)QLP_QUANTIFIER_ALL );  \
    }

/**
 * some
 */
#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____SOME( )         \
    {                                                           \
        STL_PARSER_CHECK( v0 = (qllNode*)QLP_QUANTIFIER_ANY );  \
    }

#define STL_PARSER_QUANTIFIED_COMP_PREDICATE____ANY( )          \
    {                                                           \
        STL_PARSER_CHECK( v0 = (qllNode*)QLP_QUANTIFIER_ANY );  \
    }

/**
 * concatenation
 */

#define STL_PARSER_CONCATENATION____SINGLE( )                                   \
    {                                                                           \
        qlpList      * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_CONCATENATE,   \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,    \
                                                sList ) );                      \
    }

#define STL_PARSER_CONCATENATION____LIST( )                                     \
    {                                                                           \
        qlpList      * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );     \
                                                                                \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v3 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_CONCATENATE,   \
                                                a1,                             \
                                                QLP_GET_LAST_POS( a3 ) - a1,    \
                                                sList ) );                      \
    }


/**
 * null_predicate : qlpFunction
 */

#define STL_PARSER_NULL_PREDICATE____IS_NULL( )                                 \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS_NULL,       \
                                                a1,                             \
                                                a3 - a1 + stlStrlen( v3 ),      \
                                                sList ) );                      \
    }

#define STL_PARSER_NULL_PREDICATE____IS_NOT_NULL( )                             \
    {                                                                           \
        qlpList * sList = qlpMakeList( param, QLL_POINTER_LIST_TYPE );          \
        STL_PARSER_CHECK( qlpAddPtrValueToList( param,                          \
                                                sList,                          \
                                                v1 ) );                         \
                                                                                \
        STL_PARSER_CHECK( v0 = qlpMakeFunction( param,                          \
                                                KNL_BUILTIN_FUNC_IS_NOT_NULL,   \
                                                a1,                             \
                                                a4 - a1 + stlStrlen( v4 ),      \
                                                sList ) );                      \
    }


/**
 * table_primary : qlpRelInfo
 */

#define STL_PARSER_TABLE_PRIMARY____NAME( )                             \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRelInfo( param, a1, v1, NULL ) ); \
    }

#define STL_PARSER_TABLE_PRIMARY____NAME_ALIAS_WITH_AS( )               \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRelInfo( param, a1, v1, v3 ) );   \
    }

#define STL_PARSER_TABLE_PRIMARY____NAME_ALIAS_WITH_BLANK( )            \
    {                                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeRelInfo( param, a1, v1, v2 ) );   \
    }


/**
 * correlation_name : qlpAlias
 */

#define STL_PARSER_CORRELATION_NAME____( )              \
    {                                                   \
        STL_PARSER_CHECK( v0 = qlpMakeAlias( param,     \
                                             a1,        \
                                             v1 ) );    \
    }

/** @} qlpParser */


#endif /* _QLPPARSERMACROPART_4_H_ */


