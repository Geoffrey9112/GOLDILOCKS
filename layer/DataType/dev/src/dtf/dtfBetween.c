/*******************************************************************************
 * dtfBetween.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *        
 * NOTES
 * 
 ******************************************************************************/

/**
 * @file dtfBetween.c
 * @brief Between Function DataType Library Layer
 */

#include <dtl.h>
#include <dtDef.h>

/*******************************************************************************
 *   Between Symmetric
 *   Between Symmetric ==> [ P, S ]
 * ex) 3 Between Symmetric 4 and 1 = true
 *******************************************************************************/

/**
 * @brief Between Symmetric 
 *        <BR> expr Between Symmetric expr and expr
 *        <BR>   Between Symmetric => [ P, S ]
 *        <BR> ex) 3 Between Symmetric 4 and 1  =>  true
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC BETWEEN SYMMETRIC NUMERIC AND NUMERIC
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                               dtlValueEntry  * aInputArgument,
                               void           * aResultValue,
                               void           * aInfo,
                               dtlFuncVector  * aVectorFunc,
                               void           * aVectorArg,
                               void           * aEnv )
{
    dtlDataValue      * sValue1[ 2 ];
    dtlDataValue      * sValue2;
    dtlDataValue      * sValue3;
    dtlCompareResult    sResultComp;

    /* symmetric은 A between symmetric b and c 시
     * A가 b 와 c 각각 cast를 붙여줘야 하므로
     * A.1 A.2를 갖게 된다.
     * 그러므로 argument는 3 이지만 실질적 값은 +1 인 4이다.
     */ 

    DTL_PARAM_VALIDATE( ( aInputArgumentCnt == 3 + 1 ), (stlErrorStack *)aEnv );

    sValue1[0] = aInputArgument[0].mValue.mDataValue;
    sValue1[1] = aInputArgument[1].mValue.mDataValue;
    
    sValue2 = aInputArgument[2].mValue.mDataValue;
    sValue3 = aInputArgument[3].mValue.mDataValue;

    /* A 와 B 의 대소비교 */
    sResultComp = ( dtlPhysicalCompareFuncList
                    [ sValue1[0]->mType ]
                    [ sValue2->mType ]( sValue1[0]->mValue,
                                        sValue1[0]->mLength,
                                        sValue2->mValue,
                                        sValue2->mLength ) ); 
    switch( sResultComp )
    {
        case DTL_COMPARISON_EQUAL :
            {
                /*
                 * Symmetric은
                 *   ( A >= B ) && ( A <= C ) || ( A >= C ) && ( A <= B ) 이므로
                 *
                 *   ( A < B ) && ( A < C ),
                 *   ( A > B ) && ( A > C ) 면 FALSE이다.
                 *
                 *   그러므로 ( A = B ), ( A = C ) 면 무조건 TRUE이다.
                 */
                
                DTL_BOOLEAN( aResultValue ) = STL_TRUE;
                break;
            }
        case DTL_COMPARISON_LESS :
            {               
                /*
                 * A between symmetric B and C           |----A----|
                 * A < B 인 경우                   |-----C ------- B-----|  A >= C 이어야 한다. 
                 */

                sResultComp = ( dtlPhysicalCompareFuncList
                                [ sValue1[1]->mType ]
                                [ sValue3->mType ]( sValue1[1]->mValue,
                                                    sValue1[1]->mLength,
                                                    sValue3->mValue,
                                                    sValue3->mLength ) ); 
        
                DTL_BOOLEAN( aResultValue ) = ( sResultComp == DTL_COMPARISON_LESS ) ? STL_FALSE : STL_TRUE;
                break;
            }
        case DTL_COMPARISON_GREATER :
            {
                /*
                 * A between symmetric B and C          |----A----|
                 * A > B 인 경우,                 |-----B ------- C-----|  A <= C 이어야 한다.
                 */
        
                sResultComp = ( dtlPhysicalCompareFuncList
                                [ sValue1[1]->mType ]
                                [ sValue3->mType ]( sValue1[1]->mValue,
                                                    sValue1[1]->mLength,
                                                    sValue3->mValue,
                                                    sValue3->mLength ) );

                DTL_BOOLEAN( aResultValue ) = ( sResultComp == DTL_COMPARISON_GREATER ) ? STL_FALSE : STL_TRUE;
                break;
            }
        default:
            STL_DASSERT( 0 );
            break;
    }
    
    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/*******************************************************************************
 *   Not Between Symmetric
 *   Not Between Symmetric ==> [ P, S ]
 * ex) 3 Not Between Symmetric 4 and 15 = true
 *******************************************************************************/

/**
 * @brief Not Between Symmetric 
 *        <BR> expr Not Between Symmetric expr and expr
 *        <BR>   Not Between Symmetric => [ P, S ]
 *        <BR> ex) 3 Not Between Symmetric 5 and 4  =>  true
 *        
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 *                               <BR> NUMERIC NOT BETWEEN SYMMETRIC NUMERIC AND NUMERIC
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[in,out]  aInfo         function 수행에 필요한 부가 정보
 * @param[in]  aVectorFunc       Function Vector
 * @param[in]  aVectorArg        Vector Argument
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNotBetweenSymmetric( stlUInt16        aInputArgumentCnt,
                                  dtlValueEntry  * aInputArgument,
                                  void           * aResultValue,
                                  void           * aInfo,
                                  dtlFuncVector  * aVectorFunc,
                                  void           * aVectorArg,
                                  void           * aEnv )
{
    DTL_PARAM_VALIDATE( aInputArgumentCnt == 3 + 1, (stlErrorStack *)aEnv );
 
    STL_TRY( dtfBetweenSymmetric( aInputArgumentCnt,
                                  aInputArgument,
                                  aResultValue,
                                  aInfo,
                                  aVectorFunc,
                                  aVectorArg,
                                  aEnv ) == STL_SUCCESS );
    
    DTL_BOOLEAN( aResultValue ) = ( ( DTL_BOOLEAN( aResultValue ) == STL_TRUE ) ?
                                    STL_FALSE : STL_TRUE );

    DTL_SET_BOOLEAN_LENGTH( aResultValue );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
