/*******************************************************************************
 * ztppEval.c
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
#include <goldilocks.h>
#include <sqlext.h>
#include <ztpDef.h>
#include <ztpMisc.h>
#include <ztppCommon.h>
#include <ztppEval.h>
#include <ztppCalcFunc.h>
#include <ztppMacro.h>
#include <ztppExpandFile.h>
#include <ztpfPathControl.h>

/**
 * @file ztppEval.c
 * @brief C Preprocessor Parser Routines
 */

/**
 * @addtogroup ztppEval
 * @{
 */

stlInt64 ztppEvalConstExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPConstExpr         *aConstExpr )
{
    aConstExpr->mResult = ztppEvalCondExpr( aAllocator,
                                            aErrorStack,
                                            aConstExpr->mCondExpr );

    return aConstExpr->mResult;
}

stlInt64 ztppEvalCondExpr( stlAllocator           *aAllocator,
                           stlErrorStack          *aErrorStack,
                           ztpPPCondExpr          *aCondExpr )
{
    /*
     * <cond_expr>
     *   1. -> <or_expr>
     *   2. -> <or_expr> ? <const_expr> : <cond_expr>
     *
     */

    stlInt64  sResult;

    sResult = ztppEvalOrExpr( aAllocator,
                              aErrorStack,
                              aCondExpr->mOrExpr );

    if( ( aCondExpr->mConstExpr != NULL )
        && ( aCondExpr->mCondExpr != NULL ) )
    {
        /*
         * 위 주석의 2번 케이스
         */
        if( sResult != 0 )
        {
            sResult = ztppEvalConstExpr( aAllocator,
                                         aErrorStack,
                                         aCondExpr->mConstExpr );
        }
        else
        {
            sResult = ztppEvalCondExpr( aAllocator,
                                        aErrorStack,
                                        aCondExpr->mCondExpr );
        }
    }

    return sResult;
}

stlInt64 ztppEvalOrExpr( stlAllocator           *aAllocator,
                         stlErrorStack          *aErrorStack,
                         ztpPPOrExpr            *aOrExpr )
{
    /*
     * <or_expr>
     *   1.  -> <and_expr>
     *   2.  -> <or_expr> || <and_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalAndExpr( aAllocator,
                               aErrorStack,
                               aOrExpr->mAndExpr );

    if( aOrExpr->mOrExpr != NULL )
    {
        sResult2 = ztppEvalOrExpr( aAllocator,
                                   aErrorStack,
                                   aOrExpr->mOrExpr );
        sResult = (sResult || sResult2);
    }

    return sResult;
}

stlInt64 ztppEvalAndExpr( stlAllocator           *aAllocator,
                          stlErrorStack          *aErrorStack,
                          ztpPPAndExpr           *aAndExpr )
{
    /*
     * <and_expr>
     *   1.  -> <bit_or_expr>
     *   2.  -> <and_expr> && <bit_or_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalBitOrExpr( aAllocator,
                                 aErrorStack,
                                 aAndExpr->mBitOrExpr );

    if( aAndExpr->mAndExpr != NULL )
    {
        sResult2 = ztppEvalAndExpr( aAllocator,
                                    aErrorStack,
                                    aAndExpr->mAndExpr );
        sResult = (sResult && sResult2);
    }

    return sResult;
}

stlInt64 ztppEvalBitOrExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPBitOrExpr         *aBitOrExpr )
{
    /*
     * <bit_or_expr>
     *   1.  -> <bit_xor_expr>
     *   2.  -> <bit_or_expr> | <bit_xor_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalBitXorExpr( aAllocator,
                                  aErrorStack,
                                  aBitOrExpr->mBitXorExpr );

    if( aBitOrExpr->mBitOrExpr != NULL )
    {
        sResult2 = ztppEvalBitOrExpr( aAllocator,
                                      aErrorStack,
                                      aBitOrExpr->mBitOrExpr );
        sResult |= sResult2;
    }

    return sResult;
}

stlInt64 ztppEvalBitXorExpr( stlAllocator           *aAllocator,
                             stlErrorStack          *aErrorStack,
                             ztpPPBitXorExpr        *aBitXorExpr )
{
    /*
     * <bit_xor_expr>
     *   1.  -> <bit_and_expr>
     *   2.  -> <bit_xor_expr> ^ <bit_and_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalBitAndExpr( aAllocator,
                                  aErrorStack,
                                  aBitXorExpr->mBitAndExpr );

    if( aBitXorExpr->mBitXorExpr != NULL )
    {
        sResult2 = ztppEvalBitXorExpr( aAllocator,
                                       aErrorStack,
                                       aBitXorExpr->mBitXorExpr );
        sResult ^= sResult2;
    }

    return sResult;
}

stlInt64 ztppEvalBitAndExpr( stlAllocator           *aAllocator,
                             stlErrorStack          *aErrorStack,
                             ztpPPBitAndExpr        *aBitAndExpr )
{
    /*
     * <bit_and_expr>
     *   1.  -> <equal_expr>
     *   2.  -> <bit_and_expr> & <equal_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalEqualExpr( aAllocator,
                                 aErrorStack,
                                 aBitAndExpr->mEqualExpr );

    if( aBitAndExpr->mBitAndExpr != NULL )
    {
        sResult2 = ztppEvalBitAndExpr( aAllocator,
                                       aErrorStack,
                                       aBitAndExpr->mBitAndExpr );
        sResult &= sResult2;
    }

    return sResult;
}

stlInt64 ztppEvalEqualExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPEqualExpr         *aEqualExpr )
{
    /*
     * <equal_expr>
     *   1.  -> <relational_expr>
     *   2.  -> <equal_expr> == <relational_expr>
     *       -> <equal_expr> != <relational_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalRelationalExpr( aAllocator,
                                      aErrorStack,
                                      aEqualExpr->mRelationalExpr );

    if( aEqualExpr->mEqualExpr != NULL )
    {
        sResult2 = ztppEvalEqualExpr( aAllocator,
                                      aErrorStack,
                                      aEqualExpr->mEqualExpr );

        switch( aEqualExpr->mOperator )
        {
            case ZTP_PPOP_EQ:
                sResult = (sResult == sResult2) ? 1 : 0;
                break;
            case ZTP_PPOP_NE:
                sResult = (sResult == sResult2) ? 0 : 1;
                break;
            default:
                break;
        }
    }

    return sResult;
}

stlInt64 ztppEvalRelationalExpr( stlAllocator           *aAllocator,
                                 stlErrorStack          *aErrorStack,
                                 ztpPPRelationalExpr    *aRelationalExpr )
{
    /*
     * <relational_expr>
     *   1.  -> <shift_expr>
     *   2.  -> <relational_expr> < <shift_expr>
     *       -> <relational_expr> > <shift_expr>
     *       -> <relational_expr> <= <shift_expr>
     *       -> <relational_expr> >= <shift_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalShiftExpr( aAllocator,
                                 aErrorStack,
                                 aRelationalExpr->mShiftExpr );

    if( aRelationalExpr->mRelationalExpr != NULL )
    {
        sResult2 = ztppEvalRelationalExpr( aAllocator,
                                           aErrorStack,
                                           aRelationalExpr->mRelationalExpr );

        switch( aRelationalExpr->mOperator )
        {
            case ZTP_PPOP_LT:
                sResult = (sResult < sResult2) ? 1 : 0;
                break;
            case ZTP_PPOP_GT:
                sResult = (sResult > sResult2) ? 1 : 0;
                break;
            case ZTP_PPOP_LE:
                sResult = (sResult <= sResult2) ? 1 : 0;
                break;
            case ZTP_PPOP_GE:
                sResult = (sResult >= sResult2) ? 1 : 0;
                break;
            default:
                break;
        }
    }

    return sResult;
}

stlInt64 ztppEvalShiftExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPShiftExpr         *aShiftExpr )
{
    /*
     * <shift_expr>
     *   1.  -> <add_expr>
     *   2.  -> <shift_expr> << <add_expr>
     *       -> <shift_expr> >> <add_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalAddExpr( aAllocator,
                               aErrorStack,
                               aShiftExpr->mAddExpr );

    if( aShiftExpr->mShiftExpr != NULL )
    {
        sResult2 = ztppEvalShiftExpr( aAllocator,
                                      aErrorStack,
                                      aShiftExpr->mShiftExpr );

        switch( aShiftExpr->mOperator )
        {
            case ZTP_PPOP_SL:
                sResult <<= sResult2;
                break;
            case ZTP_PPOP_SR:
                sResult >>= sResult2;
                break;
            default:
                break;
        }
    }

    return sResult;
}

stlInt64 ztppEvalAddExpr( stlAllocator           *aAllocator,
                          stlErrorStack          *aErrorStack,
                          ztpPPAddExpr           *aAddExpr )
{
    /*
     * <add_expr>
     *   1.  -> <mul_expr>
     *   2.  -> <add_expr> + <mul_expr>
     *       -> <add_expr> - <mul_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalMulExpr( aAllocator,
                               aErrorStack,
                               aAddExpr->mMulExpr );

    if( aAddExpr->mAddExpr != NULL )
    {
        sResult2 = ztppEvalAddExpr( aAllocator,
                                    aErrorStack,
                                    aAddExpr->mAddExpr );

        switch( aAddExpr->mOperator )
        {
            case ZTP_PPOP_ADD:
                sResult += sResult2;
                break;
            case ZTP_PPOP_SUB:
                sResult -= sResult2;
                break;
            default:
                break;
        }
    }

    return sResult;
}

stlInt64 ztppEvalMulExpr( stlAllocator           *aAllocator,
                          stlErrorStack          *aErrorStack,
                          ztpPPMulExpr           *aMulExpr )
{
    /*
     * <mul_expr>
     *   1.  -> <unary_expr>
     *   2.  -> <mul_expr> * <unary_expr>
     *       -> <mul_expr> / <unary_expr>
     *       -> <mul_expr> % <unary_expr>
     *
     */
    stlInt64      sResult;
    stlInt64      sResult2;

    sResult = ztppEvalUnaryExpr( aAllocator,
                                 aErrorStack,
                                 aMulExpr->mUnaryExpr );

    if( aMulExpr->mMulExpr != NULL )
    {
        sResult2 = ztppEvalMulExpr( aAllocator,
                                    aErrorStack,
                                    aMulExpr->mMulExpr );

        switch( aMulExpr->mOperator )
        {
            case ZTP_PPOP_MUL:
                sResult *= sResult2;
                break;
            case ZTP_PPOP_DIV:
                sResult /= sResult2;
                break;
            case ZTP_PPOP_MOD:
                sResult %= sResult2;
                break;
            default:
                break;
        }
    }

    return sResult;
}

stlInt64 ztppEvalUnaryExpr( stlAllocator           *aAllocator,
                            stlErrorStack          *aErrorStack,
                            ztpPPUnaryExpr         *aUnaryExpr )
{
    /*
     * <unary_expr>
     *   1.  -> <primary_expr>
     *   2.  ->  + <unary_expr>
     *       ->  - <unary_expr>
     *       ->  ~ <unary_expr>
     *       ->  ! <unary_expr>
     *
     */
    stlInt64      sResult;

    if( aUnaryExpr->mPrimaryExpr != NULL )
    {
        sResult = ztppEvalPrimaryExpr( aAllocator,
                                       aErrorStack,
                                       aUnaryExpr->mPrimaryExpr );
    }
    else
    {
        STL_DASSERT( aUnaryExpr->mUnaryExpr != NULL );

        sResult = ztppEvalUnaryExpr( aAllocator,
                                     aErrorStack,
                                     aUnaryExpr->mUnaryExpr );

        switch( aUnaryExpr->mOperator )
        {
            case ZTP_PPOP_PLUS:
                break;
            case ZTP_PPOP_MINUS:
                sResult = -sResult;
                break;
            case ZTP_PPOP_TILDE:
                sResult = ~sResult;
                break;
            case ZTP_PPOP_NOT:
                sResult = !sResult;
                break;
            default:
                break;
        }
    }

    return sResult;
}

stlInt64 ztppEvalPrimaryExpr( stlAllocator           *aAllocator,
                              stlErrorStack          *aErrorStack,
                              ztpPPPrimaryExpr       *aPrimaryExpr )
{
    stlInt64 sResult = ZTP_TERMINAL_TYPE_MAX;

    switch( aPrimaryExpr->mType )
    {
        case ZTP_TERMINAL_TYPE_VALUE:
            sResult = aPrimaryExpr->mValue;
            break;
        case ZTP_TERMINAL_TYPE_EXPRESSION:
            sResult = ztppEvalConstExpr( aAllocator,
                                         aErrorStack,
                                         aPrimaryExpr->mConstExpr );
            break;
        case ZTP_TERMINAL_TYPE_OBJECT_LIKE_MACRO:
            sResult = ztppEvalObjectLikeMacro( aAllocator,
                                               aErrorStack,
                                               aPrimaryExpr->mMacroName );
            break;
        case ZTP_TERMINAL_TYPE_FUNCTION_LIKE_MACRO:
            sResult = ztppEvalFunctionLikeMacro( aAllocator,
                                                 aErrorStack,
                                                 aPrimaryExpr->mMacroName,
                                                 aPrimaryExpr->mParameterList );
            break;
        default:
            break;
    }

    return sResult;
}

stlInt64 ztppEvalObjectLikeMacro( stlAllocator      *aAllocator,
                                  stlErrorStack     *aErrorStack,
                                  stlChar           *aMacroName )
{
    stlInt64          sResult;
    ztpMacroSymbol   *sMacroSymbol = NULL;

    ztppFindMacro( aMacroName,
                   &sMacroSymbol,
                   aErrorStack );

    if( sMacroSymbol != NULL )
    {
        ztppCalcParseIt( aAllocator,
                         aErrorStack,
                         sMacroSymbol->mContents,
                         stlStrlen( sMacroSymbol->mContents ),
                         &sResult );
    }
    else
    {
        sResult = 0;
    }

    return sResult;
}

stlInt64 ztppEvalFunctionLikeMacro( stlAllocator        *aAllocator,
                                    stlErrorStack       *aErrorStack,
                                    stlChar             *aMacroName,
                                    ztpPPParameterList  *aParameterList )
{
    stlInt64             sResult;
    ztpMacroSymbol      *sMacroSymbol = NULL;
    ztpPPParameterList  *sParameter;
    ztpArgNameList      *sArgNameList;
    stlInt32             sParameterCount;
    stlInt32             sDefinedArgumentCount;
    stlChar             *sMacroReplaceBuffer = NULL;

    /*
     * 구분 : Parameter VS Argument
     * 정의 :
     *     - 일반적으로 구분 없이 사용하고 있으나, 임밀하게 따져본다면
     *     - 호출하는 쪽에서 사용된 인자를 Parameter 라고 칭한다.
     *     - 호출된 쪽에서 인식된 이름을 Argument 라고 칭한다.
     *
     * 예제 :
     *     #define  MAX( a,b)    ((a) > (b) ? (a) : (b))
     *
     *     #if  MAX( 1, 2 )
     *     ...
     *
     *     여기서 MAX 매크로의 정의의 a, b 는 argument
     *     사용하는 곳에서 1, 2 는 parameter로 구분한다.
     */

    ztppFindMacro( aMacroName,
                   &sMacroSymbol,
                   aErrorStack );

    STL_TRY( sMacroSymbol != NULL );

    sParameterCount = 0;
    sParameter = aParameterList;
    while( sParameter != NULL )
    {
        if( sParameter->mConstExpr != NULL )
        {
            ztppEvalConstExpr( aAllocator,
                               aErrorStack,
                               sParameter->mConstExpr );
            sParameter->mValue = sParameter->mConstExpr->mResult;
        }

        sParameter = sParameter->mNext;
        sParameterCount ++;
    }

    sDefinedArgumentCount = 0;
    sArgNameList = sMacroSymbol->mArgNameList;
    while( sArgNameList != NULL )
    {
        sArgNameList = sArgNameList->mNext;
        sDefinedArgumentCount ++;
    }

    STL_TRY( sParameterCount == sDefinedArgumentCount );

    /*
     * Macro 정의 내용   : sMacroSymbol->mContents
     * Macro argument  : sMacroSymbol->mArgNameList
     * Macro Parameter : aParameterList
     *
     * 계산기로 넘기기 전에, parameter를 macro contents에 치환한다.
     */

    STL_TRY( stlAllocHeap( (void **)&sMacroReplaceBuffer,
                           ZTP_MACRO_REPLACE_BUFFER_SIZE,
                           aErrorStack )
             == STL_SUCCESS );
    stlMemset( sMacroReplaceBuffer, 0x00, ZTP_MACRO_REPLACE_BUFFER_SIZE );

    STL_TRY( ztppReplaceMacro( sMacroReplaceBuffer,
                               sMacroSymbol->mContents,
                               sMacroSymbol->mArgNameList,
                               aParameterList,
                               sParameterCount,
                               aErrorStack )
             == STL_SUCCESS );

    STL_TRY( ztppCalcParseIt( aAllocator,
                              aErrorStack,
                              sMacroReplaceBuffer,
                              stlStrlen( sMacroReplaceBuffer ),
                              &sResult )
             == STL_SUCCESS );

    stlFreeHeap( sMacroReplaceBuffer );

    return sResult;

    STL_FINISH;

    if( sMacroReplaceBuffer != NULL )
    {
        stlFreeHeap( sMacroReplaceBuffer );
    }

    /*
     * set false value
     */
    return 0;
}


/** @} */

