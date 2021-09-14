/*******************************************************************************
 * qlnvTableNode.c
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

/**
 * @file qlnvTableNode.c
 * @brief SQL Processor Layer Validation - Table Node Functions
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qlnv
 * @{
 */


/****************************************************
 * Inline Functions
 ****************************************************/ 

/**
 * @brief TABLE NODE 구문을 Validation 한다.
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in,out]  aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aParseTableNode      Table Node Parse Tree
 * @param[out]     aInitTableNode       Table Node
 * @param[in,out]  aTableCount          Table Count
 * @param[in]      aEnv                 Environment
 */
inline stlStatus qlvTableNodeValidation( qlvStmtInfo          * aStmtInfo,
                                         ellObjectList        * aValidationObjList,
                                         qlvRefTableList      * aRefTableList,
                                         qllNode              * aParseTableNode,
                                         qlvInitNode         ** aInitTableNode,
                                         stlInt32             * aTableCount,
                                         qllEnv               * aEnv )
{
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Table Node Validation
     **********************************************************/

    switch( aParseTableNode->mType )
    {
        case QLL_BASE_TABLE_NODE_TYPE:
            /**
             * Leaf Table Node Validatioin
             */
            STL_TRY( qlvValidateLeafTableNode( aStmtInfo,
                                               aValidationObjList,
                                               aRefTableList,
                                               (qlpBaseTableNode*)aParseTableNode,
                                               aInitTableNode,
                                               aTableCount,
                                               aEnv )
                     == STL_SUCCESS );
            break;
        case QLL_SUB_TABLE_NODE_TYPE:
            /**
             * Sub Table Node Validation
             */
            STL_TRY( qlvValidateSubTableNode( aStmtInfo,
                                              aValidationObjList,
                                              aRefTableList,
                                              (qlpSubTableNode*)aParseTableNode,
                                              aInitTableNode,
                                              aTableCount,
                                              aEnv )
                     == STL_SUCCESS );
            break;
        case QLL_JOIN_TABLE_NODE_TYPE:
            /**
             * Join Table Node Validation
             */
            STL_TRY( qlvValidateJoinTableNode( aStmtInfo,
                                               aValidationObjList,
                                               aRefTableList,
                                               (qlpJoinTableNode*)aParseTableNode,
                                               aInitTableNode,
                                               aTableCount,
                                               aEnv )
                     == STL_SUCCESS );
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;
    
    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Relation Name을 생성한다.
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aCatalog         Catalog Name
 * @param[in]  aSchema          Schema Name
 * @param[in]  aTable           Table Name
 * @param[in]  aAlias           Alias
 * @param[out] aRelationName    Relation Name
 * @param[in]  aEnv             Environment
 */
inline stlStatus qlvMakeRelationName( knlRegionMem      * aRegionMem,
                                      stlChar           * aCatalog,
                                      stlChar           * aSchema,
                                      stlChar           * aTable,
                                      qlpAlias          * aAlias,
                                      qlvRelationName  ** aRelationName,
                                      qllEnv            * aEnv )
{
    qlvRelationName     * sRelationName;
    stlInt32              sLen;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * Relation Name 생성
     */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRelationName ),
                                (void **) & sRelationName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**********************************************************
     * Relation Name 설정
     **********************************************************/

    if( aAlias == NULL )
    {
        /**
         * Catalog Name
         */

        if( aCatalog == NULL )
        {
            sRelationName->mCatalog = NULL;
        }
        else
        {
            /**
             * @todo 향수 catalog가 추가될 경우 구현해야 함
             */
        }


        /**
         * Schema Name
         */

        if( aSchema == NULL )
        {
            sRelationName->mSchema = NULL;
        }
        else
        {
            sLen = stlStrlen( aSchema );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        sLen + 1,
                                        (void **) & sRelationName->mSchema,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sRelationName->mSchema, aSchema, sLen );
            sRelationName->mSchema[sLen] = '\0';
        }


        /**
         * Table Name
         */

        if( aTable == NULL )
        {
            sRelationName->mTable = NULL;
        }
        else
        {
            sLen = stlStrlen( aTable );

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        sLen + 1,
                                        (void **) & sRelationName->mTable,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemcpy( sRelationName->mTable, aTable, sLen );
            sRelationName->mTable[sLen] = '\0';
        }
    }
    else
    {
        /**
         * Catalog Name
         */

        sRelationName->mCatalog = NULL;


        /**
         * Schema Name
         */

        sRelationName->mSchema  = NULL;


        /**
         * Table Name
         */

        sLen = stlStrlen( aAlias->mAliasName->mValue.mString );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    sLen + 1,
                                    (void **) & sRelationName->mTable,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemcpy( sRelationName->mTable,
                   aAlias->mAliasName->mValue.mString,
                   sLen );
        sRelationName->mTable[sLen] = '\0';
    }

    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Ouput 설정
     */

    *aRelationName = sRelationName;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Table Node로부터 Column Name에 해당하는 컬럼들을 찾아
 *        Reference Column List에 추가하고 그 Expression을 반환한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aColumnName          Column Name
 * @param[in]  aColumnPos           Column Name Position
 * @param[in]  aTableNode           Table Node
 * @param[in]  aNeedCopy            Expr의 copy 필요여부
 * @param[in,out] aIsUpdatable      Updatable Column 여부 
 * @param[out] aExpr                Expression
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvAddRefColumnListByColumnName( qlvStmtInfo           * aStmtInfo,
                                                  qlvExprPhraseType       aPhraseType,
                                                  knlRegionMem          * aRegionMem,
                                                  stlChar               * aColumnName,
                                                  stlInt32                aColumnPos,
                                                  qlvInitNode           * aTableNode,
                                                  stlBool                 aNeedCopy,
                                                  stlBool               * aIsUpdatable,
                                                  qlvInitExpression    ** aExpr,
                                                  qllEnv                * aEnv )
{
    stlBool               sExist            = STL_FALSE;
    qlvInitExpression   * sExpr             = NULL;
    qlvRefExprItem      * sRefColumnItem    = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Find Column By Column Name
     **********************************************************/

    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:
            STL_TRY( qlvFindOrAddColumnOnBaseTable( aStmtInfo,
                                                    aPhraseType,
                                                    aRegionMem,
                                                    aColumnName,
                                                    aColumnPos,
                                                    (qlvInitBaseTableNode*)aTableNode,
                                                    aNeedCopy,
                                                    &sExist,
                                                    aIsUpdatable,
                                                    &sRefColumnItem,
                                                    aEnv )
                     == STL_SUCCESS );

            if( sExist == STL_TRUE )
            {
                sExpr = sRefColumnItem->mExprPtr;
            }
            break;
        case QLV_NODE_TYPE_SUB_TABLE:
            STL_TRY( qlvFindOrAddColumnOnSubTable( aStmtInfo,
                                                   aPhraseType,
                                                   aRegionMem,
                                                   aColumnName,
                                                   aColumnPos,
                                                   (qlvInitSubTableNode*)aTableNode,
                                                   aNeedCopy,
                                                   &sExist,
                                                   aIsUpdatable,
                                                   &sRefColumnItem,
                                                   aEnv )
                     == STL_SUCCESS );

            if( sExist == STL_TRUE )
            {
                sExpr = sRefColumnItem->mExprPtr;
            }
            break;
        case QLV_NODE_TYPE_JOIN_TABLE:
            STL_TRY( qlvFindOrAddColumnOnJoinTable( aStmtInfo,
                                                    aPhraseType,
                                                    aRegionMem,
                                                    aColumnName,
                                                    aColumnPos,
                                                    (qlvInitJoinTableNode*)aTableNode,
                                                    aNeedCopy,
                                                    &sExist,
                                                    aIsUpdatable,
                                                    &sRefColumnItem,
                                                    aEnv )
                     == STL_SUCCESS );

            if( sExist == STL_TRUE )
            {
                sExpr = sRefColumnItem->mExprPtr;
            }
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aExpr = sExpr;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Base Table에서 Column Name에 해당하는 Column을 찾아
 *        Reference Column Item을 반환한다.
 *        <BR> 만약 찾은 Column이 Base Table Node의 
 *        Reference Column List에 없으면 추가해준다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr 이 사용된 Phrase 유형
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aColumnName          Column Name
 * @param[in]  aColumnPos           Column Position
 * @param[in]  aBaseTableNode       Base Table Node
 * @param[in]  aNeedCopy            Expr의 copy 필요여부
 * @param[out] aExist               존재 여부
 * @param[out] aIsUpdatable         Updatable Column 여부 
 * @param[out] aRefColumnItem       Reference Column Item
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvFindOrAddColumnOnBaseTable( qlvStmtInfo             * aStmtInfo,
                                                qlvExprPhraseType         aPhraseType,
                                                knlRegionMem            * aRegionMem,
                                                stlChar                 * aColumnName,
                                                stlInt32                  aColumnPos,
                                                qlvInitBaseTableNode    * aBaseTableNode,
                                                stlBool                   aNeedCopy,
                                                stlBool                 * aExist,
                                                stlBool                 * aIsUpdatable,
                                                qlvRefExprItem         ** aRefColumnItem,
                                                qllEnv                  * aEnv )
{
    stlBool               sExist                = STL_FALSE;
    ellDictHandle         sTempColumnHandle;
    ellDictHandle       * sColumnHandle         = &sTempColumnHandle;
    qlvRefExprItem      * sRefColumnItem        = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Find Column Handle
     **********************************************************/

    STL_TRY( ellGetColumnDictHandle( aStmtInfo->mTransID,
                                     aStmtInfo->mSQLStmt->mViewSCN,
                                     & aBaseTableNode->mTableHandle,
                                     aColumnName,
                                     sColumnHandle,
                                     &sExist,
                                     ELL_ENV(aEnv) )
             == STL_SUCCESS );


    if( sExist == STL_TRUE )
    {
        *aIsUpdatable &= ellGetColumnUpdatable( sColumnHandle );
        
        STL_TRY( qlvAddRefColumnItemOnBaseTable( aRegionMem,
                                                 aPhraseType,
                                                 aColumnPos,
                                                 aBaseTableNode,
                                                 sColumnHandle,
                                                 aNeedCopy,
                                                 &sRefColumnItem,
                                                 aEnv )
                 == STL_SUCCESS );
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aExist         = sExist;
    *aRefColumnItem = sRefColumnItem;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table에서 Column Name에 해당하는 Column을 찾아
 *        Reference Column Item을 반환한다.
 *        <BR> 만약 찾은 Column이 Sub Table Node의 
 *        Reference Column List에 없으면 추가해준다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aColumnName          Column Name
 * @param[in]  aColumnPos           Column Position
 * @param[in]  aSubTableNode        Sub Table Node
 * @param[in]  aNeedCopy            Expr의 copy 필요여부
 * @param[out] aExist               존재 여부
 * @param[in,out] aIsUpdatable      Updatable Column 여부 
 * @param[out] aRefColumnItem       Reference Column Item
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvFindOrAddColumnOnSubTable( qlvStmtInfo              * aStmtInfo,
                                               qlvExprPhraseType          aPhraseType,
                                               knlRegionMem             * aRegionMem,
                                               stlChar                  * aColumnName,
                                               stlInt32                   aColumnPos,
                                               qlvInitSubTableNode      * aSubTableNode,
                                               stlBool                    aNeedCopy,
                                               stlBool                  * aExist,
                                               stlBool                  * aIsUpdatable,
                                               qlvRefExprItem          ** aRefColumnItem,
                                               qllEnv                   * aEnv )
{
    stlInt32              i                 = 0;
    stlInt32              sColumnCount      = 0;
    stlBool               sExist            = STL_FALSE;
    qlvInitTarget       * sColumns          = NULL;
    qlvRefExprItem      * sRefColumnItem    = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Find Column Handle
     **********************************************************/

    sColumns     = aSubTableNode->mColumns;
    sColumnCount = aSubTableNode->mColumnCount;
    
    sExist = STL_FALSE;
    for( i = 0; i < sColumnCount; i++ )
    {
        if( sColumns[i].mAliasName == NULL )
        {
            if( stlStrcmp( aColumnName, sColumns[i].mDisplayName ) == 0 )
            {
                /**
                 * Column 절에서 동일 이름을 찾는데
                 * 동일한 이름이 2개 이상일 수 있다.
                 */

                STL_TRY_THROW( sExist == STL_FALSE, RAMP_ERR_COLUMN_AMBIGUOUS );

                STL_TRY( qlvAddRefColumnItemOnSubTable( aRegionMem,
                                                        aPhraseType,
                                                        aColumnPos,
                                                        aSubTableNode,
                                                        i,
                                                        sColumns[i].mExpr,
                                                        aNeedCopy,
                                                        &sRefColumnItem,
                                                        aEnv )
                         == STL_SUCCESS );

                sColumns[i].mExpr = sRefColumnItem->mExprPtr;

                sExist = STL_TRUE;
            }
        }
        else
        {
            if( stlStrcmp( aColumnName, sColumns[i].mAliasName ) == 0 )
            {
                /**
                 * Column 절에서 동일 이름을 찾는데
                 * 동일한 이름이 2개 이상일 수 있다.
                 */

                STL_TRY_THROW( sExist == STL_FALSE, RAMP_ERR_COLUMN_AMBIGUOUS );

                STL_TRY( qlvAddRefColumnItemOnSubTable( aRegionMem,
                                                        aPhraseType,
                                                        aColumnPos,
                                                        aSubTableNode,
                                                        i,
                                                        sColumns[i].mExpr,
                                                        aNeedCopy,
                                                        &sRefColumnItem,
                                                        aEnv )
                         == STL_SUCCESS );

                sColumns[i].mExpr = sRefColumnItem->mExprPtr;
                sColumns[i].mExpr->mColumnName = sColumns[i].mAliasName;

                sExist = STL_TRUE;
            }
        }
    }
    

    /**********************************************************
     * 마무리
     **********************************************************/

    if ( sExist == STL_TRUE )
    {
        /**
         * @todo Updatable View 가 지원되면, Updatable Column 여부를 다시 판단해야 함.
         */
        
        *aIsUpdatable = STL_FALSE;
    }
    
    /**
     * Output 설정
     */

    *aExist         = sExist;
    *aRefColumnItem = sRefColumnItem;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table에서 Column Name에 해당하는 Column을 찾아
 *        Reference Column Item을 반환한다.
 *        <BR> 만약 찾은 Column이 Join Table Node의 
 *        Reference Column List에 없으면 추가해준다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aColumnName          Column Name
 * @param[in]  aColumnPos           Column Name Position
 * @param[in]  aJoinTableNode       Join Table Node
 * @param[in]  aNeedCopy            Expr의 copy 필요여부
 * @param[out] aExist               존재 여부
 * @param[in,out] aIsUpdatable      Updatable Column 여부 
 * @param[out] aRefColumnItem       Reference Column Item
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvFindOrAddColumnOnJoinTable( qlvStmtInfo              * aStmtInfo,
                                                qlvExprPhraseType          aPhraseType,
                                                knlRegionMem             * aRegionMem,
                                                stlChar                  * aColumnName,
                                                stlInt32                   aColumnPos,
                                                qlvInitJoinTableNode     * aJoinTableNode,
                                                stlBool                    aNeedCopy,
                                                stlBool                  * aExist,
                                                stlBool                  * aIsUpdatable,
                                                qlvRefExprItem          ** aRefColumnItem,
                                                qllEnv                   * aEnv )
{
    stlBool               sExist            = STL_FALSE;
    qlvRefExprItem      * sRefColumnItem    = NULL;

    qlvInitExpression   * sLeftExpr         = NULL;
    qlvInitExpression   * sRightExpr        = NULL;

    qlvNamedColumnItem  * sNamedColumnItem  = NULL;

    qlvRefExprItem      * sLeftRefColumnItem    = NULL;
    qlvRefExprItem      * sRightRefColumnItem   = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Find Column Handle
     **********************************************************/

    /**
     * Using 절에 동일 필드명이 있는지 검색
     */

    sExist = STL_FALSE;
    if( (aJoinTableNode->mJoinSpec != NULL) &&
        (aJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
    {
        sNamedColumnItem = aJoinTableNode->mJoinSpec->mNamedColumnList->mHead;
        while( sNamedColumnItem != NULL )
        {
            if( stlStrcmp( sNamedColumnItem->mName, aColumnName ) == 0 )
            {
                /**
                 * Join Table Node의 Reference Column List에 추가한다.
                 */

                /**
                 * Left Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                         aPhraseType,
                                                         aColumnPos,
                                                         aJoinTableNode,
                                                         STL_TRUE,
                                                         sNamedColumnItem->mLeftExpr,
                                                         STL_FALSE,
                                                         &sLeftRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );


                /**
                 * Right Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                         aPhraseType,
                                                         aColumnPos,
                                                         aJoinTableNode,
                                                         STL_FALSE,
                                                         sNamedColumnItem->mRightExpr,
                                                         STL_FALSE,
                                                         &sRightRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );

                sExist = STL_TRUE;
                sRefColumnItem = sLeftRefColumnItem;

                break;
            }
            sNamedColumnItem = sNamedColumnItem->mNext;
        }
    }

    /**
     * 위 Using 절에서 찾지 못한 경우 Left Node와
     * Right Node에 대하여 계속 탐색한다.
     */

    if( sExist == STL_FALSE )
    {
        /**
         * Left Table Node
         */

        if( ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_SEMI ) &&
            ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_ANTI_SEMI ) &&
            ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA ) &&
            ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI ) )
        {
            STL_TRY( qlvAddRefColumnListByColumnName( aStmtInfo,
                                                      aPhraseType,
                                                      aRegionMem,
                                                      aColumnName,
                                                      aColumnPos,
                                                      aJoinTableNode->mLeftTableNode,
                                                      STL_FALSE,
                                                      aIsUpdatable,
                                                      &sLeftExpr,
                                                      aEnv )
                     == STL_SUCCESS );
        }
        

        /**
         * Right Table Node
         */

        if( ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_SEMI ) &&
            ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_ANTI_SEMI ) &&
            ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA ) &&
            ( aJoinTableNode->mJoinType != QLV_JOIN_TYPE_INVERTED_LEFT_SEMI ) )
        {
            STL_TRY( qlvAddRefColumnListByColumnName( aStmtInfo,
                                                      aPhraseType,
                                                      aRegionMem,
                                                      aColumnName,
                                                      aColumnPos,
                                                      aJoinTableNode->mRightTableNode,
                                                      STL_FALSE,
                                                      aIsUpdatable,
                                                      &sRightExpr,
                                                      aEnv )
                     == STL_SUCCESS );
        }

        /**
         * Left와 Right에서 모두 찾은 경우 Ambiguous
         */

        STL_TRY_THROW( ((sLeftExpr == NULL) || (sRightExpr == NULL)),
                       RAMP_ERR_COLUMN_AMBIGUOUS );

        /**
         * 해당 컬럼을 찾은 경우
         * Join Table Node의 Reference Column List에 추가하고,
         * output Reference Column List에 찾은 Reference Column List를
         * 추가한다.
         */

        if( sLeftExpr != NULL )
        {
            /**
             * Join Table Node의 Reference Column List에 추가
             */

            STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                     aPhraseType,
                                                     aColumnPos,
                                                     aJoinTableNode,
                                                     STL_TRUE,
                                                     sLeftExpr,
                                                     aNeedCopy,
                                                     &sRefColumnItem,
                                                     aEnv )
                     == STL_SUCCESS );

            sExist = STL_TRUE;
        }
        else if( sRightExpr != NULL )
        {
            /**
             * Join Table Node의 Reference Column List에 추가
             */

            STL_TRY( qlvAddRefColumnItemOnJoinTable( aRegionMem,
                                                     aPhraseType,
                                                     aColumnPos,
                                                     aJoinTableNode,
                                                     STL_FALSE,
                                                     sRightExpr,
                                                     aNeedCopy,
                                                     &sRefColumnItem,
                                                     aEnv )
                     == STL_SUCCESS );

            sExist = STL_TRUE;
        }
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    if ( sExist == STL_TRUE )
    {
        switch ( aJoinTableNode->mJoinType )
        {
            case QLV_JOIN_TYPE_NONE:
            case QLV_JOIN_TYPE_CROSS:
                {
                    break;
                }
            case QLV_JOIN_TYPE_INNER:
                {
                    if ( aJoinTableNode->mJoinSpec != NULL )
                    {
                        if ( aJoinTableNode->mJoinSpec->mNamedColumnList != NULL )
                        {
                            /**
                             * INNER JOIN .. USING
                             */
                            *aIsUpdatable = STL_FALSE;
                        }
                    }
                    else
                    {
                        /* nothing to do */
                    }
                    break;
                }
            case QLV_JOIN_TYPE_LEFT_OUTER:
            case QLV_JOIN_TYPE_RIGHT_OUTER:
            case QLV_JOIN_TYPE_FULL_OUTER:
            case QLV_JOIN_TYPE_LEFT_SEMI:
            case QLV_JOIN_TYPE_RIGHT_SEMI:
            case QLV_JOIN_TYPE_INVERTED_LEFT_SEMI:
            case QLV_JOIN_TYPE_INVERTED_RIGHT_SEMI:
            case QLV_JOIN_TYPE_LEFT_ANTI_SEMI:
            case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI:
            case QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA:
            case QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA:
                {
                    *aIsUpdatable = STL_FALSE;
                    break;
                }
            default:
                {
                    STL_DASSERT(0);
                    break;
                }
        }
    }
    
    /**
     * Output 설정
     */

    *aExist         = sExist;
    *aRefColumnItem = sRefColumnItem;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join의 Using절에 있는 컬럼의 개수를 구한다.
 * @param[in]  aTableNode           Table Node
 * @param[out] aCount               Named Column Count
 * @param[in]  aEnv                 Environment
 */
inline stlStatus qlvGetJoinNamedColumnCount( qlvInitNode    * aTableNode,
                                             stlInt32       * aCount,
                                             qllEnv         * aEnv )
{
    qlvInitJoinTableNode    * sJoinTableNode;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Join의 Using절에 있는 컬럼 개수 구하기
     **********************************************************/

    if( aTableNode->mType == QLV_NODE_TYPE_JOIN_TABLE )
    {
        /**
         * Join Table Node인 경우
         */

        sJoinTableNode = (qlvInitJoinTableNode*)aTableNode;
        if( (sJoinTableNode->mJoinSpec != NULL) &&
            (sJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
        {
            /**
             * Using절이 있는 경우
             */

            *aCount += sJoinTableNode->mJoinSpec->mNamedColumnList->mCount;
        }


        /**
         * Left Node 탐색
         */

        STL_TRY( qlvGetJoinNamedColumnCount( sJoinTableNode->mLeftTableNode,
                                             aCount,
                                             aEnv )
                 == STL_SUCCESS );


        /**
         * Right Node 탐색
         */

        STL_TRY( qlvGetJoinNamedColumnCount( sJoinTableNode->mRightTableNode,
                                             aCount,
                                             aEnv )
                 == STL_SUCCESS );

    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/****************************************************
 * Normal Functions
 ****************************************************/ 

/**
 * @brief Leaf TABLE NODE 구문을 Validation 한다.
 * @param[in]      aStmtInfo                Stmt Information
 * @param[in,out]  aValidationObjList       Validation Object List
 * @param[in]      aRefTableList            Reference Table List
 * @param[in]      aParseBaseTableNode      Base Table Node Parse Tree
 * @param[out]     aTableNode               Table Node
 * @param[in,out]  aTableCount              Table Count
 * @param[in]      aEnv                     Environment
 */
stlStatus qlvValidateLeafTableNode( qlvStmtInfo        * aStmtInfo,
                                    ellObjectList      * aValidationObjList,
                                    qlvRefTableList    * aRefTableList,
                                    qlpBaseTableNode   * aParseBaseTableNode,
                                    qlvInitNode       ** aTableNode,
                                    stlInt32           * aTableCount,
                                    qllEnv             * aEnv )
{
    ellDictHandle         * sAuthHandle     = NULL;

    qlpObjectName         * sObjectName     = NULL;
    stlBool                 sObjectExist    = STL_FALSE;

    qlvInitNode           * sInitTableNode = NULL;
    
    ellDictHandle           sSchemaHandle;
    ellDictHandle           sTableHandle;
    ellTableType            sTableType = ELL_TABLE_TYPE_NA;

    stlBool                 sHasCycle = STL_FALSE;
    stlBool                 sDeleted = STL_FALSE;
    
    ellInitDictHandle( & sSchemaHandle );
    ellInitDictHandle( & sTableHandle );

    /**********************************************************
     * Paramter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseBaseTableNode->mType == QLL_BASE_TABLE_NODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * 기본 정보 획득
     */

    sAuthHandle = ellGetCurrentUserHandle( ELL_ENV(aEnv) );


    /**********************************************************
     * Table Handle 획득
     **********************************************************/

    /**
     * Schema 존재 여부 확인
     */

    sObjectName = aParseBaseTableNode->mName;

    if ( sObjectName->mSchema == NULL )
    {
        /**
         * Schema Name 이 명시되지 않은 경우
         */

        STL_TRY( qlvGetTableDictHandleByAuthHandleNTblName( aStmtInfo->mTransID,
                                                            aStmtInfo->mDBCStmt,
                                                            aStmtInfo->mSQLStmt,
                                                            aStmtInfo->mSQLString,
                                                            sAuthHandle,
                                                            sObjectName->mObject,
                                                            sObjectName->mObjectPos,
                                                            & sTableHandle,
                                                            & sObjectExist,
                                                            aEnv )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXISTS );
    }
    else
    {
        /**
         * Schema Name 이 명시된 경우
         */

        ellInitDictHandle( & sSchemaHandle );
        STL_TRY( ellGetSchemaDictHandle( aStmtInfo->mTransID,
                                         aStmtInfo->mSQLStmt->mViewSCN,
                                         sObjectName->mSchema,
                                         & sSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_SCHEMA_NOT_EXISTS );

        /**
         * Table 존재 여부 확인
         */

        STL_TRY( qlvGetTableDictHandleBySchHandleNTblName( aStmtInfo->mTransID,
                                                           aStmtInfo->mDBCStmt,
                                                           aStmtInfo->mSQLStmt,
                                                           aStmtInfo->mSQLString,
                                                           & sSchemaHandle,
                                                           sObjectName->mObject,
                                                           sObjectName->mObjectPos,
                                                           & sTableHandle,
                                                           & sObjectExist,
                                                           aEnv )
                 == STL_SUCCESS );
        
        STL_TRY_THROW( sObjectExist == STL_TRUE, RAMP_ERR_TABLE_NOT_EXISTS );
    }

    /**********************************************************
     * Table 유형별 Validation
     **********************************************************/

    sTableType = ellGetTableType( & sTableHandle );
    
    switch ( sTableType )
    {
        case ELL_TABLE_TYPE_BASE_TABLE:
        case ELL_TABLE_TYPE_FIXED_TABLE:
        case ELL_TABLE_TYPE_DUMP_TABLE:
            {
                /**
                 * Physical Table Validation
                 */
                
                STL_TRY( qlvValidatePhysicalTable( aStmtInfo,
                                                   aValidationObjList,
                                                   aParseBaseTableNode,
                                                   & sTableHandle,
                                                   & sInitTableNode,
                                                   aEnv )
                         == STL_SUCCESS );
                break;
            }
        case ELL_TABLE_TYPE_VIEW:
            {
                /**
                 * View 가 Cycle 이 발생하는 지 검사
                 */

                STL_TRY( ellAddNewObjectItem( aStmtInfo->mSQLStmt->mViewCycleList,
                                              & sTableHandle,
                                              & sHasCycle,
                                              & aStmtInfo->mDBCStmt->mShareMemStmt,
                                              ELL_ENV(aEnv) )
                         == STL_SUCCESS );

                STL_TRY_THROW( sHasCycle == STL_FALSE, RAMP_ERR_VIEW_CYCLE_ENCOUTERED );
                
                /**
                 * Viewed Table Validation
                 */

                STL_TRY( qlvValidateViewedTable( aStmtInfo,
                                                 aValidationObjList,
                                                 aRefTableList,
                                                 aParseBaseTableNode,
                                                 & sTableHandle,
                                                 & sInitTableNode,
                                                 aEnv )
                         == STL_SUCCESS );

                /**
                 * View Cycle List 에서 제거
                 */

                ellDeleteObjectItem( aStmtInfo->mSQLStmt->mViewCycleList,
                                     & sTableHandle,
                                     & sDeleted );

                STL_DASSERT( sDeleted == STL_TRUE );
                                     
                break;
            }
        case ELL_TABLE_TYPE_GLOBAL_TEMPORARY:
        case ELL_TABLE_TYPE_LOCAL_TEMPORARY:
        case ELL_TABLE_TYPE_SYSTEM_VERSIONED:
            {
                STL_DASSERT(0);
                break;
            }
        case ELL_TABLE_TYPE_SEQUENCE:
        case ELL_TABLE_TYPE_SYNONYM:
        default:
            {
                STL_DASSERT(0);
                break;
            }
    }
    
    /**
     * Output 설정 
     */

    *aTableNode = sInitTableNode;
    *aTableCount += 1;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_SCHEMA_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_SCHEMA_NOT_EXISTS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mSchemaPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mSchema );
    }

    STL_CATCH( RAMP_ERR_TABLE_NOT_EXISTS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_TABLE_OR_VIEW_NOT_EXIST,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_VIEW_CYCLE_ENCOUTERED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_CIRCULAR_VIEW_DEFINITION_ENCOUNTERED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}



/**
 * @brief Physical Table 을 Validation 한다.
 * @param[in]      aStmtInfo                Stmt Information
 * @param[in,out]  aValidationObjList       Validation Object List
 * @param[in]      aParseBaseTableNode      Base Table Node Parse Tree
 * @param[in]      aTableHandle             Table Handle
 * @param[out]     aTableNode               Table Node
 * @param[in]      aEnv                     Environment
 */
stlStatus qlvValidatePhysicalTable( qlvStmtInfo        * aStmtInfo,
                                    ellObjectList      * aValidationObjList,
                                    qlpBaseTableNode   * aParseBaseTableNode,
                                    ellDictHandle      * aTableHandle,
                                    qlvInitNode       ** aTableNode,
                                    qllEnv             * aEnv )
{
    qlvInitBaseTableNode  * sBaseTableNode  = NULL;
    qlvRelationName       * sRelationName   = NULL;

    qlpObjectName         * sObjectName     = NULL;
    stlChar               * sDumpOptStr     = NULL;

    ellTableType            sTableType;

    stlBool  sObjectExist = STL_FALSE;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseBaseTableNode->mType == QLL_BASE_TABLE_NODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );

    /**
     * Base Table Node 생성
     */
                
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitBaseTableNode ),
                                (void **) & sBaseTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
                
    stlMemset( sBaseTableNode, 0x00, STL_SIZEOF( qlvInitBaseTableNode ) );
    sBaseTableNode->mType = QLV_NODE_TYPE_BASE_TABLE;

    /**
     * Table Handle 설정
     */
                
    stlMemcpy( & sBaseTableNode->mTableHandle, aTableHandle, STL_SIZEOF(ellDictHandle) );
    sBaseTableNode->mTablePhyHandle = ellGetTableHandle( aTableHandle );

    /**
     * Schema Handle 정보 획득
     */
    
    STL_TRY( ellGetSchemaDictHandleByID( aStmtInfo->mTransID,
                                         aStmtInfo->mSQLStmt->mViewSCN,
                                         ellGetTableSchemaID( aTableHandle ),
                                         & sBaseTableNode->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sObjectExist == STL_TRUE );
    
    
    /**
     * Relation Name
     * (alias가 있으면 Relation을 대표하는 name은 alias가 되고,
     *  alias가 없으면 Relation을 대표하는 name은 table name이 된다.)
     */
                
    sObjectName = aParseBaseTableNode->mName;
    
    STL_TRY( qlvMakeRelationName( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                  NULL, /* sObjectName->mCatalog */
                                  sObjectName->mSchema,
                                  sObjectName->mObject,
                                  aParseBaseTableNode->mAlias,
                                  & sRelationName,
                                  aEnv )
             == STL_SUCCESS );
                
    sBaseTableNode->mRelationName = sRelationName;

    /**
     * 참조하는 Column들을 중복없이 Column Idx 순서로 관리하는 
     * Reference Column List를 생성한다.
     */
                
    STL_TRY( qlvCreateRefExprList( & sBaseTableNode->mRefColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    /**
     * Outer Column을 관리하기 위한 Expression List를 생성한다.
     */
                
    STL_TRY( qlvCreateRefExprList( & sBaseTableNode->mOuterColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Dump Table Option 설정
     */

    sTableType = ellGetTableType( aTableHandle );
    
    if( aParseBaseTableNode->mDumpOpt == NULL )
    {
        /**
         * desc D$XXX 등을 처리하기 위해
         * ( desc D$XXX => SELECT * FROM D$XXX 로 정보를 획득함. )
         * error 설정을 하지 않는다
         */
        /* STL_TRY_THROW( sTableType != ELL_TABLE_TYPE_DUMP_TABLE, RAMP_ERR_DUMP_OPTION_REQUIRED ); */
    }
    else
    {
        STL_TRY_THROW( sTableType == ELL_TABLE_TYPE_DUMP_TABLE, RAMP_ERR_NOT_DUMP_TABLE );

        /**
         * D$XXX( '' ) 과 같이 사용한 경우를 검사
         */
        
        sDumpOptStr = QLP_GET_PTR_VALUE( aParseBaseTableNode->mDumpOpt );
        STL_TRY_THROW( sDumpOptStr != NULL, RAMP_ERR_DUMP_OPTION_REQUIRED ); 
        
        STL_TRY( knlAllocRegionMem(
                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                     stlStrlen( sDumpOptStr ) + 1,
                     (void **) & sBaseTableNode->mDumpOption,
                     KNL_ENV(aEnv) )
                 == STL_SUCCESS );
                    
        stlStrcpy( sBaseTableNode->mDumpOption,
                   QLP_GET_PTR_VALUE( aParseBaseTableNode->mDumpOpt ) );
    }

    /**
     * Validation Object List에 Table Handle 등록
     */
    
    STL_TRY( ellAddNewObjectItemWithQuantifiedName( aValidationObjList,
                                                    aTableHandle,
                                                    NULL,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /**
     * Output 설정 
     */

    *aTableNode = (qlvInitNode*) sBaseTableNode;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_DUMP_OPTION_REQUIRED )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mObjectPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_CATCH( RAMP_ERR_NOT_DUMP_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_APPLICABLE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseBaseTableNode->mDumpOpt->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      sObjectName->mObject );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief Viewed Table 을 Validation 한다.
 * @param[in]      aStmtInfo                Stmt Information
 * @param[in,out]  aValidationObjList       Validation Object List
 * @param[in]      aRefTableList            Reference Table List
 * @param[in]      aParseBaseTableNode      Base Table Node Parse Tree
 * @param[in]      aViewHandle              View Handle
 * @param[out]     aTableNode               Table Node
 * @param[in]      aEnv                     Environment
 */
stlStatus qlvValidateViewedTable( qlvStmtInfo         * aStmtInfo,
                                  ellObjectList       * aValidationObjList,
                                  qlvRefTableList     * aRefTableList,
                                  qlpBaseTableNode    * aParseBaseTableNode,
                                  ellDictHandle       * aViewHandle,
                                  qlvInitNode        ** aTableNode,
                                  qllEnv              * aEnv )
{
    stlStatus sStatus;
    
    qlvInitSubTableNode * sSubTableNode     = NULL;

    qlpObjectName       * sObjectName     = NULL;

    stlChar * sOrgString = NULL;
    stlChar * sViewColumnString = NULL;
    stlChar * sViewQueryString = NULL;

    qllNode * sParseTree = NULL;
    stlInt32      sBindCount = 0;
    
    qlpPhraseViewedTable * sPhraseViewTable = NULL;
    qlpSelect            * sPhraseViewQuery = NULL;

    ellDictHandle sCurrentUser;

    stlInt64      sViewOwnerID = ELL_DICT_OBJECT_ID_NA;
    ellDictHandle sViewOwner;
    
    stlBool       sObjectExist = STL_FALSE;

    qlvInitNode        * sInitQuery  = NULL;
    stlInt32             sTargetCount = 0;
    qlvInitTarget      * sTargetList = NULL;
    qlvInitTarget      * sTargetItem = NULL;

    stlInt32         sViewColumnCount = 0;

    qlpListElement * sListElement = NULL;

    qlpValue       * sColNode = NULL;
    stlChar        * sColName = NULL;
    stlInt32         sColNameLen = 0;
    
    stlChar       ** sViewColumnName = NULL;
    
    stlInt32  i = 0;
    stlInt32  j = 0;
    
    /**
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRefTableList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aViewHandle != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );

    /****************************************************************
     * 기본 정보 획득
     ****************************************************************/

    /**
     * Sub Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitSubTableNode ),
                                (void **) & sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sSubTableNode, 0x00, STL_SIZEOF( qlvInitSubTableNode ) );
    
    sSubTableNode->mType         = QLV_NODE_TYPE_SUB_TABLE;
    
    sSubTableNode->mIsView    = STL_TRUE;
    sSubTableNode->mHasHandle = STL_TRUE;
    stlMemcpy( & sSubTableNode->mViewHandle, aViewHandle, STL_SIZEOF(ellDictHandle) );

    /**
     * Schema Handle 정보 획득
     */
    
    STL_TRY( ellGetSchemaDictHandleByID( aStmtInfo->mTransID,
                                         aStmtInfo->mSQLStmt->mViewSCN,
                                         ellGetTableSchemaID( aViewHandle ),
                                         & sSubTableNode->mSchemaHandle,
                                         & sObjectExist,
                                         ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sObjectExist == STL_TRUE );
    
    
    /**
     * Relation Name
     */
    
    sObjectName = aParseBaseTableNode->mName;
    
    STL_TRY( qlvMakeRelationName( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                  NULL, /* sObjectName->mCatalog */
                                  sObjectName->mSchema,
                                  sObjectName->mObject,
                                  aParseBaseTableNode->mAlias,
                                  & sSubTableNode->mRelationName,
                                  aEnv )
             == STL_SUCCESS );

    /**
     * 참조하는 Column들을 중복없이 Column Idx 순서로 관리하는 
     * Reference Column List를 생성한다.
     */

    STL_TRY( qlvCreateRefExprList( & sSubTableNode->mRefColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );

    /**
     * Outer Column을 관리하기 위한 Expression List를 생성한다.
     */
                
    STL_TRY( qlvCreateRefExprList( & sSubTableNode->mOuterColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Validation Object List에 Table Handle 등록
     */
    
    STL_TRY( ellAddNewObjectItemWithQuantifiedName( aValidationObjList,
                                                    aViewHandle,
                                                    NULL,
                                                    QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                    ELL_ENV(aEnv) )
             == STL_SUCCESS );
    
    /****************************************************************
     * View Phase Parsing
     ****************************************************************/

    /**
     * View Phrase 획득
     */
    
    sViewColumnString = ellGetViewColumnString( aViewHandle );
    sViewQueryString  = ellGetViewQueryString( aViewHandle );

    STL_DASSERT( sViewColumnString != NULL );
    STL_DASSERT( sViewQueryString != NULL );
    
    /**
     * View Column Phrase Parsing
     */

    qlgSetQueryPhaseMemMgr( aStmtInfo->mDBCStmt, QLL_PHASE_PARSING, aEnv );
    
    sStatus = qlgParseInternal( aStmtInfo->mDBCStmt,
                                sViewColumnString,
                                & sParseTree,
                                & sBindCount,
                                aEnv );

    qlgSetQueryPhaseMemMgr( aStmtInfo->mDBCStmt, QLL_PHASE_VALIDATION, aEnv );
    
    STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );
    
    STL_DASSERT( sBindCount == 0 );
    STL_DASSERT( sParseTree->mType == QLL_PHRASE_VIEWED_TABLE_TYPE );

    sPhraseViewTable = (qlpPhraseViewedTable *) sParseTree;
    
    if ( sPhraseViewTable->mColumnList == NULL )
    {
        sViewColumnCount = 0;
    }
    else
    {
        sViewColumnCount = QLP_LIST_GET_COUNT( sPhraseViewTable->mColumnList );
    }

    /**
     * View Query Parsing
     */
    
    qlgSetQueryPhaseMemMgr( aStmtInfo->mDBCStmt, QLL_PHASE_PARSING, aEnv );
    
    sStatus = qlgParseInternal( aStmtInfo->mDBCStmt,
                                sViewQueryString,
                                & sParseTree,
                                & sBindCount,
                                aEnv );

    qlgSetQueryPhaseMemMgr( aStmtInfo->mDBCStmt, QLL_PHASE_VALIDATION, aEnv );
    
    STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );
    
    STL_DASSERT( sBindCount == 0 );
    STL_DASSERT( sParseTree->mType == QLL_STMT_SELECT_TYPE );
    
    sPhraseViewQuery = (qlpSelect *) sParseTree;

    STL_DASSERT( sPhraseViewQuery->mQueryNode != NULL );
    STL_DASSERT( sPhraseViewQuery->mInto == NULL );
    STL_DASSERT( sPhraseViewQuery->mUpdatability == NULL );

    /****************************************************************
     * View Query Validation 
     ****************************************************************/

    /**
     * Current User 정보 획득
     */

    stlMemcpy( & sCurrentUser, ellGetCurrentUserHandle( ELL_ENV(aEnv) ), STL_SIZEOF( ellDictHandle ) );

    /**
     * View Owner 로 Authorization Stack 변경
     */

    sViewOwnerID = ellGetTableOwnerID( aViewHandle );

    STL_TRY( ellGetAuthDictHandleByID( aStmtInfo->mTransID,
                                       aStmtInfo->mSQLStmt->mViewSCN,
                                       sViewOwnerID,
                                       & sViewOwner,
                                       & sObjectExist,
                                       ELL_ENV(aEnv) )
             == STL_SUCCESS );
    STL_DASSERT( sObjectExist == STL_TRUE );

    ellSetCurrentUser( & sViewOwner, ELL_ENV(aEnv) );

    /**
     * View Query Validation
     */

    sOrgString = aStmtInfo->mSQLString;
    aStmtInfo->mSQLString = sViewQueryString;
    
    sStatus = qlvQueryNodeValidation( aStmtInfo,
                                      STL_FALSE,
                                      aValidationObjList,
                                      aRefTableList,
                                      sPhraseViewQuery->mQueryNode,
                                      & sSubTableNode->mQueryNode,
                                      aEnv);

    aStmtInfo->mSQLString = sOrgString;
    
    STL_TRY_THROW( sStatus == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );
    
    /**
     * Current User 로 Authorization Stack 원복
     */

    ellSetCurrentUser( & sCurrentUser, ELL_ENV(aEnv) );

    /****************************************************************
     * View Target Validation
     * - not-compiled view 나 affected view 와 같이 invalid view 인 경우가 존재함.
     ****************************************************************/

    sInitQuery = sSubTableNode->mQueryNode;
    switch( sInitQuery->mType )
    {
        case QLV_NODE_TYPE_QUERY_SET :
            {
                sTargetCount = ((qlvInitQuerySetNode*) sInitQuery)->mSetTargetCount;
                sTargetList  = ((qlvInitQuerySetNode*) sInitQuery)->mSetTargets;
                break;
            }
        case QLV_NODE_TYPE_QUERY_SPEC :
            {
                sTargetCount = ((qlvInitQuerySpecNode*) sInitQuery)->mTargetCount;
                sTargetList  = ((qlvInitQuerySpecNode*) sInitQuery)->mTargets;
                break;
            }
        default :
            {
                STL_DASSERT( 0 );
                break;
            }
    }

    /**
     * Target 에 View 에서 사용할 수 없는 Expression (SEQUENCE) 이 존재하는 지 확인
     */

    for ( i = 0; i < sTargetCount; i++ )
    {
        sTargetItem = & sTargetList[i];

        STL_TRY_THROW( qlvValidateSequenceInTarget( sViewQueryString,
                                                    sTargetItem->mExpr,
                                                    aEnv )
                       == STL_SUCCESS, RAMP_ERR_VIEW_HAS_ERRORS );
    }

    
    /**
     * View Column Name 구축
     */

    if ( sViewColumnCount > 0 )
    {
        /**
         * VIEW Column List 가 명시된 경우,
         */

        STL_TRY_THROW( sViewColumnCount == sTargetCount, RAMP_ERR_MISMATCH_DEGREE );

        /**
         * Column Name List 구축 
         */
        
        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                    STL_SIZEOF(stlChar *) * sViewColumnCount,
                                    (void **) & sViewColumnName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sViewColumnName, 0x00, STL_SIZEOF(stlChar *) * sViewColumnCount );

        i = 0;
        QLP_LIST_FOR_EACH( sPhraseViewTable->mColumnList, sListElement )
        {
            sColNode = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElement );
            sColName    = QLP_GET_PTR_VALUE( sColNode );
            sColNameLen = stlStrlen( sColName );

            /**
             * Column Name 저장
             * - CREATE (FORCE) VIEW 시점에 검사되어, 중복 여부를 검사할 필요가 없다.
             */
            
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                        sColNameLen + 1,
                                        (void **) & sViewColumnName[i],
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlStrcpy( sViewColumnName[i], sColName );

            i++;
        }
    }
    else
    {
        /**
         * View Column List 가 명시되지 않은 경우
         * 이름이 식별 가능하고, 중복되지 않아야 함
         */

        sViewColumnCount = sTargetCount;

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                    STL_SIZEOF(stlChar *) * sViewColumnCount,
                                    (void **) & sViewColumnName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        stlMemset( sViewColumnName, 0x00, STL_SIZEOF(stlChar *) * sViewColumnCount );

        for( i = 0; i < sTargetCount; i++ )
        {
            sTargetItem = & sTargetList[i];

            /**
             * 명시적인 Alias Name 이거나, Column Name 이어야 함
             */

            if ( sTargetItem->mAliasName != NULL )
            {
                sColName = sTargetItem->mAliasName;
            }
            else
            {
                STL_TRY_THROW( sTargetItem->mExpr->mColumnName != NULL, RAMP_ERR_NEED_ALIAS_NAME );
                    
                sColName = sTargetItem->mExpr->mColumnName;
            }
            
            sColNameLen = stlStrlen( sColName );

            /**
             * 동일한 Column Name 이 존재하는지 검사
             */

            for ( j = 0; j < i; j++ )
            {
                STL_TRY_THROW( stlStrcmp( sColName, sViewColumnName[j] ) != 0,
                               RAMP_ERR_SAME_COLUMN_EXIST );
            }

            /**
             * Column Name 저장
             */
            
            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN(aStmtInfo->mDBCStmt),
                                        sColNameLen + 1,
                                        (void **) & sViewColumnName[i],
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            stlStrcpy( sViewColumnName[i], sColName );
        }
    }

    /********************************************************************
     * Inline View 와 동일한 형태로 Column 정보 설정
     ********************************************************************/

    /**
     * View Column 공간 할당
     */

    sSubTableNode->mColumnCount = sViewColumnCount;
    
    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitTarget ) * sSubTableNode->mColumnCount,
                                (void **) & sSubTableNode->mColumns,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sSubTableNode->mColumns,
               sTargetList,
               STL_SIZEOF( qlvInitTarget ) * sSubTableNode->mColumnCount );

    /**
     * Column Name 설정
     * - Viewed Table 은 실제로 Column Name 이 FROM 절에 등장하지 않는다.
     * - ex) viewed table : SELECT * FROM v1
     * - ex) inline view  : SELECT * FROM ( SELECT * FROM t1 ) v1 (a1, a2)
     */

    for ( i = 0; i < sSubTableNode->mColumnCount; i++ )
    {
        sSubTableNode->mColumns[i].mAliasName   = sViewColumnName[i];
        sSubTableNode->mColumns[i].mDisplayName = sViewColumnName[i];
        sSubTableNode->mColumns[i].mDisplayPos  = sObjectName->mNodePos;
    }
    
    /**
     * Output 설정 
     */

    *aTableNode = (qlvInitNode*) sSubTableNode;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_VIEW_HAS_ERRORS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_VIEW_HAS_ERRORS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sSubTableNode->mSchemaHandle ),
                      ellGetTableName( aViewHandle ) );
    }

    STL_CATCH( RAMP_ERR_MISMATCH_DEGREE )
    {
        sColNode = QLP_LIST_GET_POINTER_VALUE( QLP_LIST_GET_FIRST( sPhraseViewTable->mColumnList ) );
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_COLUMN_NAMES_SPECIFIED,
                      qlgMakeErrPosString( sViewColumnString,
                                           sColNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_VIEW_HAS_ERRORS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sSubTableNode->mSchemaHandle ),
                      ellGetTableName( aViewHandle ) );
    }

    STL_CATCH( RAMP_ERR_NEED_ALIAS_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_MUST_NAME_THIS_EXPRESSION_WTIH_COLUMN_ALIAS,
                      qlgMakeErrPosString( sViewQueryString,
                                           sTargetItem->mDisplayPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_VIEW_HAS_ERRORS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sSubTableNode->mSchemaHandle ),
                      ellGetTableName( aViewHandle ) );
    }

    STL_CATCH( RAMP_ERR_SAME_COLUMN_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( sViewQueryString,
                                           sTargetItem->mDisplayPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );

        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_VIEW_HAS_ERRORS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sObjectName->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      ellGetSchemaName( & sSubTableNode->mSchemaHandle ),
                      ellGetTableName( aViewHandle ) );
    }
    
    /* STL_CATCH( RAMP_ERR_NOT_IMPLEMENTED ) */
    /* { */
    /*     stlPushError( STL_ERROR_LEVEL_ABORT, */
    /*                   QLL_ERRCODE_NOT_IMPLEMENTED, */
    /*                   NULL, */
    /*                   QLL_ERROR_STACK(aEnv), */
    /*                   "qlvValidateViewedTable()" ); */
    /* } */
    
    STL_FINISH;

    /**
     * Current User 를 Session User 로 원복 
     */
    
    ellRollbackCurrentUser( ELL_ENV(aEnv) );
    
    return STL_FAILURE;
}


/**
 * @brief SUB TABLE NODE 구문을 Validation 한다.
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in,out]  aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aParseSubTableNode   Sub Table Node Parse Tree
 * @param[out]     aTableNode           Table Node
 * @param[in,out]  aTableCount          Table Count
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateSubTableNode( qlvStmtInfo       * aStmtInfo,
                                   ellObjectList     * aValidationObjList,
                                   qlvRefTableList   * aRefTableList,
                                   qlpSubTableNode   * aParseSubTableNode,
                                   qlvInitNode      ** aTableNode,
                                   stlInt32          * aTableCount,
                                   qllEnv            * aEnv )
{
    qlvInitSubTableNode * sSubTableNode     = NULL;
    qlvRelationName     * sRelationName     = NULL;
    qlvInitTarget       * sColumns          = NULL;
    qlpListElement      * sListElem         = NULL;
    qlpValue            * sValue            = NULL;    
    stlInt32              sColumnCount      = 0;
    stlInt32              sLoop             = 0;
    

    /**********************************************************
     * Paramter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseSubTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseSubTableNode->mType == QLL_SUB_TABLE_NODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/
    
    /**
     * Sub Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitSubTableNode ),
                                (void **) & sSubTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sSubTableNode, 0x00, STL_SIZEOF( qlvInitSubTableNode ) );
    sSubTableNode->mType = QLV_NODE_TYPE_SUB_TABLE;

    sSubTableNode->mIsView    = STL_TRUE;
    sSubTableNode->mHasHandle = STL_FALSE;

    /**********************************************************
     * Subquery Validation
     **********************************************************/

    /**
     * Query Node Validation
     */

    STL_TRY( qlvQueryNodeValidation( aStmtInfo,
                                     STL_FALSE,
                                     aValidationObjList,
                                     aRefTableList,
                                     aParseSubTableNode->mQueryNode,
                                     &(sSubTableNode->mQueryNode),
                                     aEnv)
             == STL_SUCCESS );


    /**********************************************************
     * Relation Name 설정
     **********************************************************/

    /**
     * Relation Name
     * (alias가 있으면 Relation을 대표하는 name은 alias가 되고,
     *  alias가 없으면 Relation을 대표하는 name은 없다.)
     */

    STL_TRY( qlvMakeRelationName( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                  NULL,
                                  NULL,
                                  NULL,
                                  aParseSubTableNode->mAlias,
                                  &sRelationName,
                                  aEnv )
             == STL_SUCCESS );

    sSubTableNode->mRelationName = sRelationName;


    /**********************************************************
     * Reference Column List 생성
     **********************************************************/

    /**
     * 참조하는 Column들을 중복없이 Column Idx 순서로 관리하는 
     * Reference Column List를 생성한다.
     */

    STL_TRY( qlvCreateRefExprList( & sSubTableNode->mRefColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Outer Column을 관리하기 위한 Expression List를 생성한다.
     */
                
    STL_TRY( qlvCreateRefExprList( & sSubTableNode->mOuterColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * Column
     **********************************************************/

    if( sSubTableNode->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SET )
    {
        /**
         * Query Set Node인 경우
         */

        sSubTableNode->mColumns     = ((qlvInitQuerySetNode*)sSubTableNode->mQueryNode)->mSetTargets;
        sSubTableNode->mColumnCount = ((qlvInitQuerySetNode*)sSubTableNode->mQueryNode)->mSetTargetCount;
    }
    else if( sSubTableNode->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SPEC )
    {
        /**
         * Query Spec Node인 경우
         */

        sSubTableNode->mColumns     = ((qlvInitQuerySpecNode*)sSubTableNode->mQueryNode)->mTargets;
        sSubTableNode->mColumnCount = ((qlvInitQuerySpecNode*)sSubTableNode->mQueryNode)->mTargetCount;
    }
    else
    {
        STL_DASSERT( 0 );
    }


    /* Target Expression 내에 Sequence 가 있는지 확인 */
    for( sLoop = 0 ; sLoop < sSubTableNode->mColumnCount ; sLoop++ )
    {
        STL_TRY( qlvValidateSequenceInTarget( aStmtInfo->mSQLString,
                                              sSubTableNode->mColumns[ sLoop ].mExpr,
                                              aEnv )
                 == STL_SUCCESS );
    }


    /**
     * TARGET절 Validation : Init 영역의 Column 공간 할당
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitTarget ) * sSubTableNode->mColumnCount,
                                (void **) & sColumns,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemcpy( sColumns,
               sSubTableNode->mColumns,
               STL_SIZEOF( qlvInitTarget ) * sSubTableNode->mColumnCount );

    if( aParseSubTableNode->mColAliasList == NULL )
    {
        /* Do Nothing */
    }
    else  /* 새로운 target list 구성 */
    {
        /**
         * Alias 설정
         */

        STL_TRY_THROW( aParseSubTableNode->mColAliasList->mCount
                       == sSubTableNode->mColumnCount,
                       RAMP_ERR_MISMATCH_DEGREE );

        sColumnCount = 0;
        QLP_LIST_FOR_EACH( aParseSubTableNode->mColAliasList, sListElem )
        {
            /* Alias 공간 할당 */
            sValue = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElem );

            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        stlStrlen( sValue->mValue.mString ) + 1,
                                        (void **) & sColumns[ sColumnCount ].mAliasName,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );
            
            /* Alias 설정 */
            stlStrcpy( sColumns[ sColumnCount ].mAliasName,
                       sValue->mValue.mString );

            sColumns[ sColumnCount ].mDisplayName = sColumns[ sColumnCount ].mAliasName;
            
            /* Alias 는 같은 이름이 올 수 없다. */
            for( sLoop = 0 ; sLoop < sColumnCount ; sLoop++ )
            {
                STL_TRY_THROW( stlStrcmp( sColumns[ sLoop ].mAliasName,
                                          sColumns[ sColumnCount ].mAliasName ) != 0,
                               RAMP_ERR_SAME_COLUMN_EXIST );
            }

            sColumnCount++;
        }
    }

    
    /**
     * Column List 설정
     */

    sSubTableNode->mColumns = sColumns;
    
    
    /**********************************************************
     * 마무리 
     **********************************************************/
    
    /**
     * Output 설정 
     */

    *aTableNode = (qlvInitNode*)sSubTableNode;
    *aTableCount += 1;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_MISMATCH_DEGREE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_NUMBER_OF_COLUMN_NAMES_SPECIFIED,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseSubTableNode->mAlias->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_SAME_COLUMN_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           sValue->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief JOIN TABLE NODE 구문을 Validation 한다.
 * @param[in]      aStmtInfo            Stmt Information
 * @param[in,out]  aValidationObjList   Validation Object List
 * @param[in]      aRefTableList        Reference Table List
 * @param[in]      aParseJoinTableNode  Join Table Node Parse Tree
 * @param[out]     aTableNode           Table Node
 * @param[in,out]  aTableCount          Table Count
 * @param[in]      aEnv                 Environment
 */
stlStatus qlvValidateJoinTableNode( qlvStmtInfo          * aStmtInfo,
                                    ellObjectList        * aValidationObjList,
                                    qlvRefTableList      * aRefTableList,
                                    qlpJoinTableNode     * aParseJoinTableNode,
                                    qlvInitNode         ** aTableNode,
                                    stlInt32             * aTableCount,
                                    qllEnv               * aEnv )
{
    qlvInitJoinTableNode    * sJoinTableNode        = NULL;
    qlvInitJoinSpec         * sJoinSpec             = NULL;

    qlvRefTableList         * sRefTableList         = NULL;

    qlvNamedColumnItem      * sNamedColumnItem      = NULL;
    qlvRefExprItem          * sLeftRefColumnItem    = NULL;
    qlvRefExprItem          * sRightRefColumnItem   = NULL;

    qlvInitExpression       * sFuncExpr             = NULL;

    qlvNamedColumnList        sTempNamedColumnList;
    qlvNamedColumnList      * sNamedColumnList      = &sTempNamedColumnList;

    qlvInitExpression      ** sExprArr              = NULL;
    stlInt32                  sIdx;


    /**********************************************************
     * Paramter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aValidationObjList != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aParseJoinTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aParseJoinTableNode->mType == QLL_JOIN_TABLE_NODE_TYPE),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * Join Table Node 생성
     */

    STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                STL_SIZEOF( qlvInitJoinTableNode ),
                                (void **) & sJoinTableNode,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );
    
    stlMemset( sJoinTableNode, 0x00, STL_SIZEOF( qlvInitJoinTableNode ) );
    sJoinTableNode->mType = QLV_NODE_TYPE_JOIN_TABLE;


    /**********************************************************
     * Join Type 및 Join Method 설정
     **********************************************************/

    /**
     * Set Join Type
     */

    switch( aParseJoinTableNode->mJoinType )
    {
        case QLP_JOIN_TYPE_NONE:
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_NONE;
            break;
        case QLP_JOIN_TYPE_CROSS:
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_CROSS;
            break;
        case QLP_JOIN_TYPE_INNER:
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_INNER;
            break;
        case QLP_JOIN_TYPE_LEFT_OUTER:
            if( aParseJoinTableNode->mIsNatural == STL_TRUE )
            {
                STL_THROW( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_NATURAL_JOIN );
            }
            if( (aParseJoinTableNode->mJoinSpec != NULL) &&
                (aParseJoinTableNode->mJoinSpec->mNamedColumns != NULL) )
            {
                STL_THROW( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_USING_CLAUSE );
            }

            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_LEFT_OUTER;
            break;
        case QLP_JOIN_TYPE_RIGHT_OUTER:
            if( aParseJoinTableNode->mIsNatural == STL_TRUE )
            {
                STL_THROW( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_NATURAL_JOIN );
            }
            if( (aParseJoinTableNode->mJoinSpec != NULL) &&
                (aParseJoinTableNode->mJoinSpec->mNamedColumns != NULL) )
            {
                STL_THROW( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_USING_CLAUSE );
            }

            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_RIGHT_OUTER;
            break;
        case QLP_JOIN_TYPE_FULL_OUTER:
            if( aParseJoinTableNode->mIsNatural == STL_TRUE )
            {
                STL_THROW( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_NATURAL_JOIN );
            }
            if( (aParseJoinTableNode->mJoinSpec != NULL) &&
                (aParseJoinTableNode->mJoinSpec->mNamedColumns != NULL) )
            {
                STL_THROW( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_USING_CLAUSE );
            }

            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_FULL_OUTER;
            break;
        case QLP_JOIN_TYPE_LEFT_SEMI:
            if( aParseJoinTableNode->mJoinSpec != NULL )
            {
                if( aParseJoinTableNode->mJoinSpec->mNamedColumns != NULL )
                {
                    STL_THROW( RAMP_ERR_NOT_SUPPORTED_SEMI_JOIN_BY_USING_CLAUSE );
                }
            }
            
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_LEFT_SEMI;
            break;
        case QLP_JOIN_TYPE_RIGHT_SEMI:
            if( aParseJoinTableNode->mJoinSpec != NULL )
            {
                if( aParseJoinTableNode->mJoinSpec->mNamedColumns != NULL )
                {
                    STL_THROW( RAMP_ERR_NOT_SUPPORTED_SEMI_JOIN_BY_USING_CLAUSE );
                }
            }

            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_RIGHT_SEMI;
            break;
        case QLP_JOIN_TYPE_LEFT_ANTI_SEMI:
            if( aParseJoinTableNode->mJoinSpec != NULL )
            {
                if( aParseJoinTableNode->mJoinSpec->mNamedColumns != NULL )
                {
                    STL_THROW( RAMP_ERR_NOT_SUPPORTED_ANTI_SEMI_JOIN_BY_USING_CLAUSE );
                }
            }

            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_LEFT_ANTI_SEMI_NA;
            break;
        case QLP_JOIN_TYPE_RIGHT_ANTI_SEMI:
            if( aParseJoinTableNode->mJoinSpec != NULL )
            {
                if( aParseJoinTableNode->mJoinSpec->mNamedColumns != NULL )
                {
                    STL_THROW( RAMP_ERR_NOT_SUPPORTED_ANTI_SEMI_JOIN_BY_USING_CLAUSE );
                }
            }

            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_RIGHT_ANTI_SEMI_NA;
            break;
        default:
            STL_DASSERT( 0 );
            break;
    }


    /**********************************************************
     * Join Table Node Validation
     **********************************************************/

    /**
     * Left Table Node
     */

    STL_TRY( qlvTableNodeValidation( aStmtInfo,
                                     aValidationObjList,
                                     aRefTableList,
                                     aParseJoinTableNode->mLeftTableNode,
                                     &(sJoinTableNode->mLeftTableNode),
                                     aTableCount,
                                     aEnv )
             == STL_SUCCESS );


    /**
     * Right Table Node
     */

    STL_TRY( qlvTableNodeValidation( aStmtInfo,
                                     aValidationObjList,
                                     aRefTableList,
                                     aParseJoinTableNode->mRightTableNode,
                                     &(sJoinTableNode->mRightTableNode),
                                     aTableCount,
                                     aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * Reference Column List 생성
     **********************************************************/

    /**
     * 참조하는 Column들을 중복없이 Column Idx 순서로 관리하는 
     * Reference Column List를 생성한다.
     */

    STL_TRY( qlvCreateRefExprList( & sJoinTableNode->mRefColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );


    /**
     * Outer Column을 관리하기 위한 Expression List를 생성한다.
     */
                
    STL_TRY( qlvCreateRefExprList( & sJoinTableNode->mOuterColumnList,
                                   QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                   aEnv )
             == STL_SUCCESS );


    /**********************************************************
     * Natural Join의 동일 Column 처리
     **********************************************************/

    if( aParseJoinTableNode->mIsNatural == STL_TRUE )
    {
        STL_DASSERT( aParseJoinTableNode->mJoinSpec == NULL );

        /**
         * @todo Left Table Node와 Right Table Node에서
         * 모든 Column Name을 읽어와 동일한 Column을 찾고
         * 이를 Join Specification의 Named Column에 넣는다.
         */

        sNamedColumnList->mCount    = 0;
        sNamedColumnList->mHead     = NULL;
        sNamedColumnList->mTail     = NULL;

        STL_TRY( qlvGetNamedColumnListForNaturalJoin( aStmtInfo,
                                                      sJoinTableNode,
                                                      sNamedColumnList,
                                                      aEnv )
                 == STL_SUCCESS );

        if( sNamedColumnList->mCount > 0 )
        {
            /**
             * 동일한 Column Name이 존재하는 경우
             */

            /**
             * Join Specification 생성
             */

            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        STL_SIZEOF( qlvInitJoinSpec ),
                                        (void **) & sJoinSpec,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            stlMemset( sJoinSpec, 0x00, STL_SIZEOF( qlvInitJoinSpec ) );


            /**
             * Named Column List 생성
             */

            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        STL_SIZEOF( qlvNamedColumnList ),
                                        (void **) &sJoinSpec->mNamedColumnList,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sJoinSpec->mNamedColumnList->mCount = sNamedColumnList->mCount;
            sJoinSpec->mNamedColumnList->mHead  = sNamedColumnList->mHead;
            sJoinSpec->mNamedColumnList->mTail  = sNamedColumnList->mTail;


            /**
             * Named Columns의 Column들을 Reference Column List에 추가
             */

            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        STL_SIZEOF( qlvInitExpression* ) * sNamedColumnList->mCount,
                                        (void **) & sExprArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sIdx = 0;

            sNamedColumnItem = sJoinSpec->mNamedColumnList->mHead;
            while( sNamedColumnItem != NULL )
            {
                /**
                 * Left Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         QLV_EXPR_PHRASE_FROM,
                                                         sNamedColumnItem->mPosition,
                                                         sJoinTableNode,
                                                         STL_TRUE,
                                                         sNamedColumnItem->mLeftExpr,
                                                         STL_TRUE,
                                                         &sLeftRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );


                /**
                 * Right Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         QLV_EXPR_PHRASE_FROM,
                                                         sNamedColumnItem->mPosition,
                                                         sJoinTableNode,
                                                         STL_FALSE,
                                                         sNamedColumnItem->mRightExpr,
                                                         STL_TRUE,
                                                         &sRightRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );


                /**
                 * Named Columns의 Column들을 Join Condition에 추가
                 */

                STL_TRY( qlvMakeFuncEqual( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                           sNamedColumnItem->mPosition,
                                           sLeftRefColumnItem->mExprPtr,
                                           sRightRefColumnItem->mExprPtr,
                                           &sFuncExpr,
                                           aEnv )
                         == STL_SUCCESS );

                sExprArr[sIdx] = sFuncExpr;
                sIdx++;

                sNamedColumnItem = sNamedColumnItem->mNext;
            }

            STL_DASSERT( sIdx == sNamedColumnList->mCount );
            if( sIdx > 1 )
            {
                STL_TRY( qlvMakeFuncAndNArg( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             sJoinSpec->mNamedColumnList->mHead->mPosition,
                                             sIdx,
                                             sExprArr,
                                             &(sJoinSpec->mJoinCondition),
                                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sIdx == 1 );
                sJoinSpec->mJoinCondition = sExprArr[0];
            }
        }
        else
        {
            sJoinTableNode->mJoinType = QLV_JOIN_TYPE_CROSS;
        }
    }


    /**********************************************************
     * Join Specification Validation
     **********************************************************/

    if( aParseJoinTableNode->mJoinSpec == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        STL_DASSERT( aParseJoinTableNode->mIsNatural == STL_FALSE );

        /**
         * Join Specification 생성
         */

        STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                    STL_SIZEOF( qlvInitJoinSpec ),
                                    (void **) & sJoinSpec,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sJoinSpec, 0x00, STL_SIZEOF( qlvInitJoinSpec ) );

        if( aParseJoinTableNode->mJoinSpec->mJoinCondition == NULL )
        {
            /**
             * named columns가 있는 경우
             */

            STL_TRY( qlvValidateJoinSpecNamedColumns( aStmtInfo,
                                                      QLV_EXPR_PHRASE_FROM,
                                                      QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                      (qlvInitNode*)sJoinTableNode,
                                                      aParseJoinTableNode->mJoinSpec->mNamedColumns,
                                                      &sJoinSpec->mNamedColumnList,
                                                      aEnv )
                     == STL_SUCCESS );

            STL_TRY( knlAllocRegionMem( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                        STL_SIZEOF( qlvInitExpression* ) * sJoinSpec->mNamedColumnList->mCount,
                                        (void **) & sExprArr,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sIdx = 0;

            sNamedColumnItem = sJoinSpec->mNamedColumnList->mHead;
            while( sNamedColumnItem != NULL )
            {
                /**
                 * Named Columns의 Column들을 Reference Column List에 추가
                 */

                /**
                 * Left Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         QLV_EXPR_PHRASE_FROM,
                                                         sNamedColumnItem->mPosition,
                                                         sJoinTableNode,
                                                         STL_TRUE,
                                                         sNamedColumnItem->mLeftExpr,
                                                         STL_TRUE,
                                                         &sLeftRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );


                /**
                 * Right Expr의 Reference Column Item 추가
                 */

                STL_TRY( qlvAddRefColumnItemOnJoinTable( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                                         QLV_EXPR_PHRASE_FROM,
                                                         sNamedColumnItem->mPosition,
                                                         sJoinTableNode,
                                                         STL_FALSE,
                                                         sNamedColumnItem->mRightExpr,
                                                         STL_TRUE,
                                                         &sRightRefColumnItem,
                                                         aEnv )
                         == STL_SUCCESS );


                /**
                 * Named Columns의 Column들을 Join Condition에 추가
                 */

                STL_TRY( qlvMakeFuncEqual( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                           sNamedColumnItem->mPosition,
                                           sLeftRefColumnItem->mExprPtr,
                                           sRightRefColumnItem->mExprPtr,
                                           &sFuncExpr,
                                           aEnv )
                         == STL_SUCCESS );

                sExprArr[sIdx] = sFuncExpr;
                sIdx++;

                sNamedColumnItem = sNamedColumnItem->mNext;
            }

            STL_DASSERT( sIdx == sJoinSpec->mNamedColumnList->mCount );
            if( sIdx > 1 )
            {
                STL_TRY( qlvMakeFuncAndNArg( QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                             sJoinSpec->mNamedColumnList->mHead->mPosition,
                                             sIdx,
                                             sExprArr,
                                             &(sJoinSpec->mJoinCondition),
                                             aEnv )
                         == STL_SUCCESS );
            }
            else
            {
                STL_DASSERT( sIdx == 1 );
                sJoinSpec->mJoinCondition = sExprArr[0];
            }
        }
        else
        {
            /**
             * Join Condition이 있는 경우
             */

            /**
             * Temp Reference Table List 생성
             */

            STL_TRY( qlvCreateRefTableList( &QLL_VALIDATE_MEM( aEnv ),
                                            aRefTableList,
                                            &sRefTableList,
                                            aEnv )
                     == STL_SUCCESS );


            /**
             * Table Node를 Table Item에 등록
             * --------------------------------------
             *  Join Specification의 Condition을 validation할때
             *  Column을 찾을 범위를 해당 현재 Join Table Node 이하로 설정
             */

            sRefTableList->mHead->mTableNode = (qlvInitNode*)sJoinTableNode;

            STL_TRY( qlvValidateValueExpr( aStmtInfo,
                                           QLV_EXPR_PHRASE_FROM,
                                           aValidationObjList,
                                           sRefTableList,
                                           STL_FALSE,
                                           STL_FALSE,  /* Row Expr */
                                           0 /* Allowed Aggregation Depth */,
                                           (qllNode*)aParseJoinTableNode->mJoinSpec->mJoinCondition,
                                           &sJoinSpec->mJoinCondition,
                                           NULL,
                                           KNL_BUILTIN_FUNC_INVALID,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                           QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                           aEnv )
                     == STL_SUCCESS );

            /**
             * Rewrite Expression
             */

            STL_TRY( qlvRewriteExpr( sJoinSpec->mJoinCondition,
                                     DTL_ITERATION_TIME_FOR_EACH_EXPR,
                                     aStmtInfo->mQueryExprList,
                                     & sJoinSpec->mJoinCondition,
                                     QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                                     aEnv )
                     == STL_SUCCESS );
        }
    }

    sJoinTableNode->mJoinSpec = sJoinSpec;


    /**********************************************************
     * Outer Join 은 Join Condition 이 있어야 함.
     **********************************************************/

    switch( sJoinTableNode->mJoinType )
    {
        case QLV_JOIN_TYPE_LEFT_OUTER:
        case QLV_JOIN_TYPE_RIGHT_OUTER:
        case QLV_JOIN_TYPE_FULL_OUTER:
            STL_DASSERT( sJoinTableNode->mJoinSpec != NULL );
            break;
        default:
            /* Do Nothing */
            break;
    }


    /**********************************************************
     * 마무리 
     **********************************************************/
    
    /**
     * Output 설정 
     */

    *aTableNode = (qlvInitNode*)sJoinTableNode;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_NATURAL_JOIN )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_SUPPORTED_QUERY,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseJoinTableNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      " (natural outer join)" );
    }

    STL_CATCH( RAMP_ERR_NOT_SUPPORTED_OUTER_JOIN_BY_USING_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_SUPPORTED_QUERY,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseJoinTableNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      " (outer join including using clause)" );
    }

    STL_CATCH( RAMP_ERR_NOT_SUPPORTED_SEMI_JOIN_BY_USING_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_SUPPORTED_QUERY,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseJoinTableNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      " (semi join including using clause)" );
    }

    STL_CATCH( RAMP_ERR_NOT_SUPPORTED_ANTI_SEMI_JOIN_BY_USING_CLAUSE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_NOT_SUPPORTED_QUERY,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aParseJoinTableNode->mNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      " (anti-semi join including using clause)" );
    }

    STL_FINISH;
    
    return STL_FAILURE;
}


/**
 * @brief JOIN SPECIFICATION의 NAMED COLUMNS 구문을 Validation 한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aTableNode           Table Node
 * @param[in]  aColumnList          Column List
 * @param[out] aNamedColumnList     Named Column List
 * @param[in]  aEnv                 Environment
 */
stlStatus qlvValidateJoinSpecNamedColumns( qlvStmtInfo          * aStmtInfo,
                                           qlvExprPhraseType      aPhraseType,
                                           knlRegionMem         * aRegionMem,
                                           qlvInitNode          * aTableNode,
                                           qlpList              * aColumnList,
                                           qlvNamedColumnList  ** aNamedColumnList,
                                           qllEnv               * aEnv )
{
    qlpListElement      * sListElem             = NULL;
    qlpValue            * sValue                = NULL;

    qlvNamedColumnList  * sNamedColumnList      = NULL;
    

    /**********************************************************
     * Paramter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnList != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * 준비 작업
     **********************************************************/

    /**
     * Named Column List 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvNamedColumnList ),
                                (void **) &sNamedColumnList,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sNamedColumnList->mCount    = 0;
    sNamedColumnList->mHead     = NULL;
    sNamedColumnList->mTail     = NULL;


    /**********************************************************
     * Join Named Column List Validation
     **********************************************************/

    QLP_LIST_FOR_EACH( aColumnList, sListElem )
    {
        sValue = (qlpValue *) QLP_LIST_GET_POINTER_VALUE( sListElem );


        STL_TRY( qlvValidateJoinSpecNamedColumnInternal( aStmtInfo,
                                                         aPhraseType,
                                                         aRegionMem,
                                                         aTableNode,
                                                         sValue->mValue.mString,
                                                         sValue->mNodePos,
                                                         STL_FALSE,
                                                         sNamedColumnList,
                                                         aEnv )
                 == STL_SUCCESS );
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aNamedColumnList = sNamedColumnList;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief JOIN SPECIFICATION의 NAMED COLUMNS 구문을 Validation 한다.
 * @param[in]  aStmtInfo            Stmt Information
 * @param[in]  aPhraseType          Expr이 사용된 Phrase 유형
 * @param[in]  aRegionMem           Region Memory
 * @param[in]  aTableNode           Table Node
 * @param[in]  aColumnName          Column Name
 * @param[in]  aNodePos             Node Position
 * @param[in]  aIsSetOuterMark      Outer Join Operator 설정 여부
 * @param[out] aNamedColumnList     Named Column List
 * @param[in]  aEnv                 Environment
 */
stlStatus qlvValidateJoinSpecNamedColumnInternal( qlvStmtInfo          * aStmtInfo,
                                                  qlvExprPhraseType      aPhraseType,
                                                  knlRegionMem         * aRegionMem,
                                                  qlvInitNode          * aTableNode,
                                                  stlChar              * aColumnName,
                                                  stlInt32               aNodePos,
                                                  stlBool                aIsSetOuterMark,
                                                  qlvNamedColumnList   * aNamedColumnList,
                                                  qllEnv               * aEnv )
{
    stlInt32              sLen                  = 0;

    qlvNamedColumnItem  * sNamedColumnItem      = NULL;

    qlvInitExpression   * sLeftExpr             = NULL;
    qlvInitExpression   * sRightExpr            = NULL;

    qlvRelationName     * sLeftRelationName     = NULL;
    qlvRelationName     * sRightRelationName    = NULL;

    stlBool               sIsUpdatable = STL_FALSE;

    /**********************************************************
     * Paramter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnName != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Join Named Column List Validation
     **********************************************************/

    /**
     * Left Node로부터 Column Name에 해당하는
     * Expression을 가져온다.
     */

    sLeftExpr = NULL;
    sIsUpdatable = STL_TRUE;
    STL_TRY( qlvAddRefColumnListByColumnName( aStmtInfo,
                                              aPhraseType,
                                              aRegionMem,
                                              aColumnName,
                                              aNodePos,
                                              ((qlvInitJoinTableNode*)aTableNode)->mLeftTableNode,
                                              STL_TRUE,
                                              &sIsUpdatable,
                                              &sLeftExpr,
                                              aEnv )
             == STL_SUCCESS );

    /** 해당 컬럼명이 없으면 에러 */
    STL_TRY_THROW( sLeftExpr != NULL,
                   RAMP_ERR_COLUMN_NOT_EXIST );


    /**
     * Right Node로부터 Column Name에 해당하는
     * Expression을 가져온다.
     */

    sRightExpr = NULL;
    STL_TRY( qlvAddRefColumnListByColumnName( aStmtInfo,
                                              aPhraseType,
                                              aRegionMem,
                                              aColumnName,
                                              aNodePos,
                                              ((qlvInitJoinTableNode*)aTableNode)->mRightTableNode,
                                              STL_TRUE,
                                              &sIsUpdatable,
                                              &sRightExpr,
                                              aEnv )
             == STL_SUCCESS );

    /** 해당 컬럼명이 없으면 에러 */
    STL_TRY_THROW( sRightExpr != NULL,
                   RAMP_ERR_COLUMN_NOT_EXIST );


    /**
     * Error Check
     * -----------------------------------------------------
     * Left Node와 Right Node가 모두 Join Table Node가 아니면서
     * 찾는 Column의 Table Name이 동일한 경우
     *    => Column Ambiguously Defined
     */

    /**
     * Left Relation Name 찾기
     */

    if( sLeftExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
    {
        switch( sLeftExpr->mExpr.mInnerColumn->mRelationNode->mType )
        {
            case QLV_NODE_TYPE_SUB_TABLE:
                sLeftRelationName = ((qlvInitSubTableNode*)sLeftExpr->mExpr.mInnerColumn->mRelationNode)->mRelationName;
                break;
            case QLV_NODE_TYPE_JOIN_TABLE:
                sLeftRelationName = NULL;
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }
    else if( sLeftExpr->mType == QLV_EXPR_TYPE_COLUMN )
    {
        sLeftRelationName = ((qlvInitBaseTableNode*)sLeftExpr->mExpr.mColumn->mRelationNode)->mRelationName;
    }
    else
    {
        STL_DASSERT( 0 );
    }


    /**
     * Right Relation Name 찾기
     */

    if( sRightExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
    {
        switch( sRightExpr->mExpr.mInnerColumn->mRelationNode->mType )
        {
            case QLV_NODE_TYPE_SUB_TABLE:
                sRightRelationName = ((qlvInitSubTableNode*)sRightExpr->mExpr.mInnerColumn->mRelationNode)->mRelationName;
                break;
            case QLV_NODE_TYPE_JOIN_TABLE:
                sRightRelationName = NULL;
                break;
            default:
                STL_DASSERT( 0 );
                break;
        }
    }
    else if( sRightExpr->mType == QLV_EXPR_TYPE_COLUMN )
    {
        sRightRelationName = ((qlvInitBaseTableNode*)sRightExpr->mExpr.mColumn->mRelationNode)->mRelationName;
    }
    else
    {
        STL_DASSERT( 0 );
    }


    /**
     * Error Check
     */

    if( (sLeftRelationName != NULL) && (sRightRelationName != NULL) )
    {
        if( (sLeftRelationName->mTable != NULL) && (sRightRelationName->mTable != NULL) )
        {
            if( stlStrcmp( sLeftRelationName->mTable, sRightRelationName->mTable ) == 0 )
            {
                /**
                 * 두 Relation Name의 Table Name이 동일한 경우
                 */

                STL_THROW( RAMP_ERR_COLUMN_AMBIGUOUS );
            }
        }
    }


    /**
     * 이미 존재하는 컬럼인지 판별
     * -------------------------------------------------------
     *  using(i1, i1)에 대한 처리
     *    Oracle) 정상 수행 (결과에도 i1이 두개 나옴)
     *    Postgress) 에러 (column name "i1" appears more than once in USING clause)
     *  => Gliese는 Postgress처럼 에러로 처리한다.
     */

    sNamedColumnItem = aNamedColumnList->mHead;
    while( sNamedColumnItem != NULL )
    {
        if( stlStrcmp( sNamedColumnItem->mName, aColumnName ) == 0 )
        {
            STL_THROW( RAMP_ERR_DUPLICATE_COLUMN_NAME );
        }

        sNamedColumnItem = sNamedColumnItem->mNext;
    }


    /**
     * Named Column Item 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvNamedColumnItem ),
                                (void **) &sNamedColumnItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Column Name 생성
     */

    sLen = stlStrlen( aColumnName );
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                sLen + 1,
                                (void **) &sNamedColumnItem->mName,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Named Column Item 설정
     */

    stlMemcpy( sNamedColumnItem->mName, aColumnName, sLen );
    sNamedColumnItem->mName[sLen]       = '\0';
    sNamedColumnItem->mPosition         = aNodePos;
    sNamedColumnItem->mLeftExpr         = sLeftExpr;
    sNamedColumnItem->mRightExpr        = sRightExpr;
    sNamedColumnItem->mNext             = NULL;


    /**
     * Named Column List에 Named Column Item 추가
     * --------------------------------------------------------
     *  Named Column List에 저장된 순서가 Output 순서이다.
     */

    if( aNamedColumnList->mCount == 0 )
    {
        aNamedColumnList->mHead = sNamedColumnItem;
        aNamedColumnList->mTail = sNamedColumnItem;
    }
    else
    {
        aNamedColumnList->mTail->mNext  = sNamedColumnItem;
        aNamedColumnList->mTail         = sNamedColumnItem;
    }
    aNamedColumnList->mCount++;


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_COLUMN_NOT_EXIST )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_INVALID_IDENTIFIER,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv),
                      aColumnName );
    }

    STL_CATCH( RAMP_ERR_COLUMN_AMBIGUOUS )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_COLUMN_AMBIGUOUS,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_CATCH( RAMP_ERR_DUPLICATE_COLUMN_NAME )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_DUPLICATE_COLUMN_NAME,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aNodePos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Base Table Node에서 Reference Column Item 검색 및 추가
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aPhraseType      Expr 이 사용된 Phrase 유형
 * @param[in]  aNodePos         Node Position
 * @param[in]  aBaseTableNode   Base Table Node
 * @param[in]  aColumnHandle    Column Handle
 * @param[in]  aNeedCopy        Expr의 copy 필요여부
 * @param[out] aRefColumnItem   Reference Column Item
 * @param[in]  aEnv             Environment
 */
stlStatus qlvAddRefColumnItemOnBaseTable( knlRegionMem          * aRegionMem,
                                          qlvExprPhraseType       aPhraseType,
                                          stlInt32                aNodePos,
                                          qlvInitBaseTableNode  * aBaseTableNode,
                                          ellDictHandle         * aColumnHandle,
                                          stlBool                 aNeedCopy,
                                          qlvRefExprItem       ** aRefColumnItem,
                                          qllEnv                * aEnv )
{
    qlvRefExprItem      * sRefColumnItem        = NULL;
    qlvRefExprItem      * sPrevRefColumnItem    = NULL;

    qlvInitColumn       * sCodeColumn           = NULL;
    qlvInitExpression   * sCodeExpr             = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnHandle != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Reference Column List에 Column Handle에 대한 Reference Column Item 처리
     **********************************************************/

    /**
     * Reference Column List에서 Column Handle을 찾음
     */

    sRefColumnItem      = aBaseTableNode->mRefColumnList->mHead;
    sPrevRefColumnItem  = NULL;
    while( sRefColumnItem != NULL )
    {
        if( ellGetColumnIdx( aColumnHandle ) <=
            ellGetColumnIdx( sRefColumnItem->mExprPtr->mExpr.mColumn->mColumnHandle ) )
        {
            break;
        }

        sPrevRefColumnItem  = sRefColumnItem;
        sRefColumnItem      = sRefColumnItem->mNext;
    }

    if( sRefColumnItem == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( ellGetColumnIdx( aColumnHandle ) ==
            ellGetColumnIdx( sRefColumnItem->mExprPtr->mExpr.mColumn->mColumnHandle ) )
        {
            if( (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_NA) ||
                (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_CONDITION) ||
                (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_FROM) )
            {
                sRefColumnItem->mExprPtr->mPhraseType = aPhraseType;
            }

            if( aNeedCopy == STL_TRUE )
            {
                /**
                 * Column Handle을 찾은 경우 기존 Expression 을 복사
                 */

                STL_TRY( qlvCopyExpr( sRefColumnItem->mExprPtr,
                                      & sCodeExpr,
                                      aRegionMem,
                                      aEnv )
                         == STL_SUCCESS );

                /* node potision 설정 */
                sCodeExpr->mPosition = aNodePos;
                

                /**
                 * Reference Column Item 생성
                 */

                STL_TRY( knlAllocRegionMem( aRegionMem,
                                            STL_SIZEOF( qlvRefExprItem ),
                                            (void **) & sRefColumnItem,
                                            KNL_ENV(aEnv) )
                         == STL_SUCCESS );

                sRefColumnItem->mExprPtr = sCodeExpr;
                sRefColumnItem->mNext    = NULL;
            }
            
            STL_THROW( RAMP_FINISH );
        }
        else
        {
            /* Do Nothing */
        }
    }
    

    /**
     * Column Handle을 찾지 못한 경우
     */

    /**
     * Target Expr 설정 : CODE COLUMN 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitColumn ),
                                (void **) & sCodeColumn,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Column Handle 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( ellDictHandle ),
                                (void **) & sCodeColumn->mColumnHandle,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sCodeColumn->mRelationNode = (qlvInitNode *) aBaseTableNode;
    stlMemcpy( sCodeColumn->mColumnHandle,
               aColumnHandle,
               STL_SIZEOF( ellDictHandle ) );


    /**
     * Target Expr 설정 : CODE EXPRESSION 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sCodeExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCodeExpr,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
    
    sCodeExpr->mType           = QLV_EXPR_TYPE_COLUMN;
    sCodeExpr->mPhraseType     = aPhraseType;
    sCodeExpr->mPosition       = aNodePos;
    sCodeExpr->mIncludeExprCnt = 1;
    sCodeExpr->mExpr.mColumn   = sCodeColumn;
    sCodeExpr->mIterationTime  = DTL_ITERATION_TIME_FOR_EACH_EXPR;

    if( ellGetColumnName( aColumnHandle ) != NULL )
    {
        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    stlStrlen( ellGetColumnName( aColumnHandle ) ) + 1,
                                    (void **) & sCodeExpr->mColumnName,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlStrcpy( sCodeExpr->mColumnName, ellGetColumnName( aColumnHandle ) );
    }
    else
    {
        /* unused column */
        sCodeExpr->mColumnName = NULL;
    }


    /**
     * Reference Column Item 생성
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvRefExprItem ),
                                (void **) & sRefColumnItem,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    sRefColumnItem->mExprPtr        = sCodeExpr;
    sRefColumnItem->mNext           = NULL;


    /**
     * New Reference Column Item을
     * Previous Reference Column Item 다음에 추가
     */

    if( sPrevRefColumnItem == NULL )
    {
        /**
         * Head에 추가해야 되는 경우
         */

        if( aBaseTableNode->mRefColumnList->mCount == 0 )
        {
            aBaseTableNode->mRefColumnList->mHead = sRefColumnItem;
            aBaseTableNode->mRefColumnList->mTail = sRefColumnItem;
        }
        else
        {
            sRefColumnItem->mNext                 = aBaseTableNode->mRefColumnList->mHead;
            aBaseTableNode->mRefColumnList->mHead = sRefColumnItem;
        }
    }
    else
    {
        /**
         * 중간에 추가해야 되는 경우
         */
        sRefColumnItem->mNext       = sPrevRefColumnItem->mNext;
        sPrevRefColumnItem->mNext   = sRefColumnItem;
    }

    aBaseTableNode->mRefColumnList->mCount++;

    
    STL_RAMP( RAMP_FINISH );


    /**********************************************************
     * 마무리
     **********************************************************/
    
    /**
     * Output 설정
     */

    *aRefColumnItem = sRefColumnItem;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Sub Table Node에서 Reference Column Item 검색 및 추가
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aPhraseType      Expr이 사용된 Phrase 유형
 * @param[in]  aNodePos         Node Position
 * @param[in]  aSubTableNode    Sub Table Node
 * @param[in]  aTargetIdx       Target Index
 * @param[in]  aExpr            Target Expression
 * @param[in]  aNeedCopy        Expr의 copy 필요여부
 * @param[out] aRefColumnItem   Reference Column Item
 * @param[in]  aEnv             Environment
 */
stlStatus qlvAddRefColumnItemOnSubTable( knlRegionMem           * aRegionMem,
                                         qlvExprPhraseType        aPhraseType,
                                         stlInt32                 aNodePos,
                                         qlvInitSubTableNode    * aSubTableNode,
                                         stlInt32                 aTargetIdx,
                                         qlvInitExpression      * aExpr,
                                         stlBool                  aNeedCopy,
                                         qlvRefExprItem        ** aRefColumnItem,
                                         qllEnv                 * aEnv )
{
    qlvRefExprItem      * sRefColumnItem        = NULL;
    qlvRefExprItem      * sPrevRefColumnItem    = NULL;

    qlvInitInnerColumn  * sCodeInnerColumn      = NULL;
    qlvInitExpression   * sCodeExpr             = NULL;
    stlBool               sIsExist              = STL_FALSE;
    

    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSubTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTargetIdx >= 0, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Reference Column List에 Column Handle에 대한 Reference Column Item 처리
     **********************************************************/

    /**
     * Reference Column List에서 Column Handle을 찾음
     */

    sRefColumnItem      = aSubTableNode->mRefColumnList->mHead;
    sPrevRefColumnItem  = NULL;
    while( sRefColumnItem != NULL )
    {
        if( aTargetIdx <= *(sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx) )
        {
            sIsExist = ( aTargetIdx == *(sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx) );
            break;
        }

        sPrevRefColumnItem  = sRefColumnItem;
        sRefColumnItem      = sRefColumnItem->mNext;
    }

    if( sIsExist == STL_TRUE )
    {
        if( (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_NA) ||
            (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_CONDITION) ||
            (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_FROM) )
        {
            sRefColumnItem->mExprPtr->mPhraseType = aPhraseType;
        }

        if( aNeedCopy == STL_TRUE )
        {
            /**
             * Column Handle을 찾은 경우 기존 Expression 을 복사
             */

            STL_TRY( qlvCopyExpr( sRefColumnItem->mExprPtr,
                                  & sCodeExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );


            /**
             * Reference Column Item 생성
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvRefExprItem ),
                                        (void **) & sRefColumnItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sRefColumnItem->mExprPtr = sCodeExpr;
            sRefColumnItem->mNext    = NULL;
        }
    }
    else
    {
        /**
         * Column Handle을 찾지 못한 경우
         */

        /**
         * Target Expr 설정 : CODE INNER COLUMN 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void **) & sCodeInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void **) & sCodeInnerColumn->mIdx,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sCodeInnerColumn->mRelationNode = (qlvInitNode *) aSubTableNode;
        *sCodeInnerColumn->mIdx         = aTargetIdx;

        sCodeInnerColumn->mOrgExpr = aExpr;


        /**
         * Target Expr 설정 : CODE EXPRESSION 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression ),
                                    (void **) & sCodeExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sCodeExpr,
                   0x00,
                   STL_SIZEOF( qlvInitExpression ) );

        QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
        
        sCodeExpr->mType                = QLV_EXPR_TYPE_INNER_COLUMN;
        sCodeExpr->mPhraseType          = aPhraseType;
        sCodeExpr->mPosition            = aNodePos;
        sCodeExpr->mIncludeExprCnt      = 1;
        sCodeExpr->mExpr.mInnerColumn   = sCodeInnerColumn;
        sCodeExpr->mIterationTime       = DTL_ITERATION_TIME_FOR_EACH_EXPR;
        sCodeExpr->mColumnName          = aExpr->mColumnName;

        
        /**
         * Reference Column Item 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void **) & sRefColumnItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefColumnItem->mExprPtr        = sCodeExpr;
        sRefColumnItem->mNext           = NULL;


        /**
         * New Reference Column Item을
         * Previous Reference Column Item 다음에 추가
         */

        if( sPrevRefColumnItem == NULL )
        {
            /**
             * Head에 추가해야 되는 경우
             */

            if( aSubTableNode->mRefColumnList->mCount == 0 )
            {
                aSubTableNode->mRefColumnList->mHead = sRefColumnItem;
                aSubTableNode->mRefColumnList->mTail = sRefColumnItem;
            }
            else
            {
                sRefColumnItem->mNext                   = aSubTableNode->mRefColumnList->mHead;
                aSubTableNode->mRefColumnList->mHead    = sRefColumnItem;
            }
        }
        else
        {
            /**
             * 중간에 추가해야 되는 경우
             */
            sRefColumnItem->mNext       = sPrevRefColumnItem->mNext;
            sPrevRefColumnItem->mNext   = sRefColumnItem;
        }

        aSubTableNode->mRefColumnList->mCount++;
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aRefColumnItem = sRefColumnItem;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Join Table Node에서 Reference Column Item 추가
 * @param[in]  aRegionMem       Region Memory
 * @param[in]  aPhraseType      Expr이 사용된 Phrase 유형
 * @param[in]  aNodePos         Node Position
 * @param[in]  aJoinTableNode   Join Table Node
 * @param[in]  aIsLeft          Left Table Node의 Expr로부터 유도된 것인지 여부
 * @param[in]  aExpr            Target Expression
 * @param[in]  aNeedCopy        Expr의 copy 필요여부
 * @param[out] aRefColumnItem   Reference Column Item
 * @param[in]  aEnv             Environment
 */
stlStatus qlvAddRefColumnItemOnJoinTable( knlRegionMem           * aRegionMem,
                                          qlvExprPhraseType        aPhraseType,
                                          stlInt32                 aNodePos,
                                          qlvInitJoinTableNode   * aJoinTableNode,
                                          stlBool                  aIsLeft,
                                          qlvInitExpression      * aExpr,
                                          stlBool                  aNeedCopy,
                                          qlvRefExprItem        ** aRefColumnItem,
                                          qllEnv                 * aEnv )
{
    qlvRefExprItem      * sRefColumnItem        = NULL;
    qlvRefExprItem      * sPrevRefColumnItem    = NULL;
    qlvRefExprItem      * sTempRefColumnItem    = NULL;

    qlvInitInnerColumn  * sCodeInnerColumn      = NULL;
    qlvInitExpression   * sCodeExpr             = NULL;

    stlBool               sIsSame               = STL_FALSE;
    qlvInitExpression   * sOrgExpr              = NULL;

    qlvInitNode         * sExprRelationNode     = NULL;
    qlvInitNode         * sOrgRelationNode      = NULL;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aExpr != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( (aExpr->mType == QLV_EXPR_TYPE_COLUMN) ||
                        (aExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN) ||
                        (aExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN),
                        QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Reference Column List에 Column Handle에 대한 Reference Column Item 처리
     **********************************************************/

    #define GET_RELATION_NODE( aMacroExpr, aMacroRelNode ) {                    \
        if( (aMacroExpr)->mType == QLV_EXPR_TYPE_COLUMN )                       \
        {                                                                       \
            (aMacroRelNode) = (aMacroExpr)->mExpr.mColumn->mRelationNode;       \
        }                                                                       \
        else if( (aMacroExpr)->mType == QLV_EXPR_TYPE_INNER_COLUMN )            \
        {                                                                       \
            (aMacroRelNode) = (aMacroExpr)->mExpr.mInnerColumn->mRelationNode;  \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            (aMacroRelNode) = (aMacroExpr)->mExpr.mRowIdColumn->mRelationNode;  \
        }                                                                       \
    }


    /**
     * Reference Column List에서는 Left Table Node의 Column들이 리스트 앞쪽에 위치하고
     * Right Table Node의 Column들이 리스트 뒤쪽에 위치하도록 정렬해야 한다.
     * 또한 동일 Table Node에서는 OrgExpr의 순서에 따라 정렬해야 한다.
     * 그리고 동일 Table Node에서 RowId Column은 맨 마지막에 연결된다.
     * (OrgExpr이 InnerColumn인 경우 Idx의 정렬, Column인 경우 Column ID의 정렬)
     * 
     * 예) select t1.rowid, t2.rowid, t1.*, t2.* from t1, t2;
     *     t1.i1 -> t1.i2 -> t1.rowid -> t2.i1 -> t2.i2 -> t2.rowid
     */

    sRefColumnItem      = aJoinTableNode->mRefColumnList->mHead;
    sPrevRefColumnItem  = NULL;
    GET_RELATION_NODE( aExpr, sExprRelationNode );
    while( sRefColumnItem != NULL )
    {
        sOrgExpr = sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mOrgExpr;
        GET_RELATION_NODE( sOrgExpr, sOrgRelationNode );
        if( sExprRelationNode == sOrgRelationNode )
        {
            /**
             * 두 Relatioin Node가 일치하는 경우
             */

            if( aExpr->mType == QLV_EXPR_TYPE_COLUMN )
            {
                if( sOrgExpr->mType == QLV_EXPR_TYPE_COLUMN )
                {
                    /**
                     * Column ID로 크기 비교
                     */

                    if( ellGetColumnIdx( aExpr->mExpr.mColumn->mColumnHandle ) <=
                        ellGetColumnIdx( sOrgExpr->mExpr.mColumn->mColumnHandle ) )
                    {
                        if( ellGetColumnIdx( aExpr->mExpr.mColumn->mColumnHandle ) ==
                            ellGetColumnIdx( sOrgExpr->mExpr.mColumn->mColumnHandle ) )
                        {
                            sIsSame = STL_TRUE;
                        }

                        break;
                    }
                }
            }
            else if( aExpr->mType == QLV_EXPR_TYPE_INNER_COLUMN )
            {
                /**
                 * Idx로 크기 비교
                 */

                if( *(aExpr->mExpr.mInnerColumn->mIdx) <=
                    *(sOrgExpr->mExpr.mInnerColumn->mIdx) )
                {
                    if( *(aExpr->mExpr.mInnerColumn->mIdx) ==
                        *(sOrgExpr->mExpr.mInnerColumn->mIdx) )
                    {
                        sIsSame = STL_TRUE;
                    }

                    break;
                }
            }
            else
            {
                /**
                 * RowId Column인 경우 
                 */

                if( sOrgExpr->mType == QLV_EXPR_TYPE_ROWID_COLUMN ) 
                {                
                    sIsSame = STL_TRUE;
                }

                break;
            }            

            sPrevRefColumnItem  = sRefColumnItem;
            sRefColumnItem      = sRefColumnItem->mNext;
        }
        else
        {
            /**
             * 두 Relation Node가 일치하지 않는 경우
             */

            if( sExprRelationNode == aJoinTableNode->mLeftTableNode )
            {
                /**
                 * 삽입하려는 Expression의 Relation Node가 Left Table Node인 경우
                 */

                break;
            }
            else
            {
                /**
                 * 삽입하려는 Expression의 Relation Node가 Right Table Node인 경우
                 */

                sPrevRefColumnItem  = sRefColumnItem;
                sRefColumnItem      = sRefColumnItem->mNext;
            }
        }
    }

    if( sIsSame == STL_TRUE )
    {
        if( (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_NA) ||
            (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_CONDITION) ||
            (sRefColumnItem->mExprPtr->mPhraseType == QLV_EXPR_PHRASE_FROM) )
        {
            sRefColumnItem->mExprPtr->mPhraseType = aPhraseType;
        }

        if( aNeedCopy == STL_TRUE )
        {
            /**
             * Column Handle을 찾은 경우 기존 Expression 을 복사
             */

            STL_TRY( qlvCopyExpr( sRefColumnItem->mExprPtr,
                                  & sCodeExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );


            /**
             * Reference Column Item 생성
             */

            STL_TRY( knlAllocRegionMem( aRegionMem,
                                        STL_SIZEOF( qlvRefExprItem ),
                                        (void **) & sRefColumnItem,
                                        KNL_ENV(aEnv) )
                     == STL_SUCCESS );

            sRefColumnItem->mExprPtr = sCodeExpr;
            sRefColumnItem->mNext    = NULL;
        }
    }
    else
    {
        /**
         * Expr과 동일한 포인터를 찾지 못한 경우
         */

        /**
         * Target Expr 설정 : CODE COLUMN 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitInnerColumn ),
                                    (void **) & sCodeInnerColumn,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( stlInt32 ),
                                    (void **) & sCodeInnerColumn->mIdx,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sCodeInnerColumn->mRelationNode = (qlvInitNode *) aJoinTableNode;
        *sCodeInnerColumn->mIdx         = aJoinTableNode->mRefColumnList->mCount;

        STL_TRY( qlvCopyExpr( aExpr,
                              & sCodeInnerColumn->mOrgExpr,
                              aRegionMem,
                              aEnv )
                 == STL_SUCCESS );


        /**
         * Target Expr 설정 : CODE EXPRESSION 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvInitExpression ),
                                    (void **) & sCodeExpr,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        stlMemset( sCodeExpr,
                   0x00,
                   STL_SIZEOF( qlvInitExpression ) );

        QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );
        
        sCodeExpr->mType                = QLV_EXPR_TYPE_INNER_COLUMN;
        sCodeExpr->mPhraseType          = aPhraseType;
        sCodeExpr->mPosition            = aNodePos;
        sCodeExpr->mIncludeExprCnt      = 1;
        sCodeExpr->mExpr.mInnerColumn   = sCodeInnerColumn;
        sCodeExpr->mIterationTime       = DTL_ITERATION_TIME_FOR_EACH_EXPR;
        sCodeExpr->mColumnName          = aExpr->mColumnName;


        /**
         * Reference Column Item 생성
         */

        STL_TRY( knlAllocRegionMem( aRegionMem,
                                    STL_SIZEOF( qlvRefExprItem ),
                                    (void **) & sRefColumnItem,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        sRefColumnItem->mExprPtr        = sCodeExpr;
        sRefColumnItem->mIsLeft         = aIsLeft;
        sRefColumnItem->mNext           = NULL;


        /**
         * Reference Column Item을
         * Previous Reference Column Item 다음에 추가
         */

        if( sPrevRefColumnItem == NULL )
        {
            /**
             * Head에 추가해야 되는 경우
             */

            if( aJoinTableNode->mRefColumnList->mCount == 0 )
            {
                aJoinTableNode->mRefColumnList->mHead = sRefColumnItem;
                aJoinTableNode->mRefColumnList->mTail = sRefColumnItem;
            }
            else
            {
                sRefColumnItem->mNext                 = aJoinTableNode->mRefColumnList->mHead;
                aJoinTableNode->mRefColumnList->mHead = sRefColumnItem;

                /**
                 * Idx 재조정
                 */

                *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx = 0;
                sTempRefColumnItem = sRefColumnItem->mNext;
                while( sTempRefColumnItem != NULL )
                {
                    (*sTempRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx)++;
                    sTempRefColumnItem = sTempRefColumnItem->mNext;
                }
            }
        }
        else
        {
            /**
             * 중간에 추가해야 되는 경우
             */

            sRefColumnItem->mNext       = sPrevRefColumnItem->mNext;
            sPrevRefColumnItem->mNext   = sRefColumnItem;

            if( sPrevRefColumnItem == aJoinTableNode->mRefColumnList->mTail )
            {
                /* 마지막 노드로 추가되는 경우 */
                aJoinTableNode->mRefColumnList->mTail = sRefColumnItem;
            }

            /**
             * Idx 재조정
             */

            *sRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx = 
                *(sPrevRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx) + 1;
            sTempRefColumnItem = sRefColumnItem->mNext;
            while( sTempRefColumnItem != NULL )
            {
                (*sTempRefColumnItem->mExprPtr->mExpr.mInnerColumn->mIdx)++;
                sTempRefColumnItem = sTempRefColumnItem->mNext;
            }
        }

        aJoinTableNode->mRefColumnList->mCount++;
    }


    /**********************************************************
     * 마무리
     **********************************************************/

    /**
     * Output 설정
     */

    *aRefColumnItem = sRefColumnItem;


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


static inline stlInt32 qlvCmpColumnName( void   * aCmpInfo,
                                         void   * aValAPtr,
                                         void   * aValBPtr )
{
    return stlStrcmp( *((stlChar**)aValAPtr), *((stlChar**)aValBPtr) );
}


/**
 * @brief Natural Join에서 동일한 이름의 Column들을 Named Column List로 구성
 * @param[in]  aStmtInfo        Stmt Information
 * @param[in]  aJoinTableNode   Join Table Node
 * @param[out] aNamedColumnList Named Column List
 * @param[in]  aEnv             Environment
 */
stlStatus qlvGetNamedColumnListForNaturalJoin( qlvStmtInfo          * aStmtInfo,
                                               qlvInitJoinTableNode * aJoinTableNode,
                                               qlvNamedColumnList   * aNamedColumnList,
                                               qllEnv               * aEnv )
{
    stlChar            ** sLeftColumnNames      = NULL;
    stlInt32              sLeftColumnNameCount  = 0;

    stlChar            ** sRightColumnNames     = NULL;
    stlInt32              sRightColumnNameCount = 0;

    stlChar            ** sLeftExceptNames      = NULL;
    stlInt32              sLeftExceptNameCount  = 0;

    stlChar            ** sRightExceptNames     = NULL;
    stlInt32              sRightExceptNameCount = 0;

    stlInt32              sTempCount            = 0;

    stlChar             * sColumnName           = NULL;

    knlQSortContext       sQSortContext;

    stlInt32              sLeft                 = 0;
    stlInt32              sRight                = 0;
    stlInt32              sCmp                  = 0;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aJoinTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aNamedColumnList != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Left Table의 Column Name List 수집
     **********************************************************/

    /**
     * Left Table의 Except Name List를 위한 공간할당
     */

    sLeftExceptNameCount = 0;
    STL_TRY( qlvGetJoinNamedColumnCount( aJoinTableNode->mLeftTableNode,
                                         &sLeftExceptNameCount,
                                         aEnv )
             == STL_SUCCESS );

    if( sLeftExceptNameCount == 0 )
    {
        sLeftExceptNames = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlChar* ) * sLeftExceptNameCount,
                                    (void **) &sLeftExceptNames,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * Left Table의 모든 Column의 개수를 가져오기
     */

    STL_TRY( qlvGetAsteriskTargetCountMultiTables( aStmtInfo,
                                                   aJoinTableNode->mLeftTableNode,
                                                   0,
                                                   sLeftExceptNames,
                                                   &sLeftColumnNameCount,
                                                   aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sLeftColumnNameCount > 0 );


    /**
     * Left Table의 Column Name List 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                STL_SIZEOF( stlChar* ) * sLeftColumnNameCount,
                                (void **) &sLeftColumnNames,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Left Table의 모든 Column Name List를 가져오기
     */

    sTempCount = 0;
    STL_TRY( qlvGetAllColumnNamesFromTableNode( aJoinTableNode->mLeftTableNode,
                                                0,
                                                sLeftExceptNames,
                                                sLeftColumnNames,
                                                &sTempCount,
                                                aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sLeftColumnNameCount == sTempCount );


    /**
     * Column Name List를 오름차순으로 정렬
     */

    sQSortContext.mElemType = KNL_QSORT_ELEM_8BYTE;
    sQSortContext.mIsUnique = STL_FALSE;
    sQSortContext.mCompare  = qlvCmpColumnName;
    sQSortContext.mCmpInfo  = NULL;

    (void)knlQSort( sLeftColumnNames,
                    sLeftColumnNameCount,
                    &sQSortContext );


    /**********************************************************
     * Right Table의 Column Name List 수집
     **********************************************************/

    /**
     * Right Table의 Except Name List를 위한 공간할당
     */

    sRightExceptNameCount = 0;
    STL_TRY( qlvGetJoinNamedColumnCount( aJoinTableNode->mRightTableNode,
                                         &sRightExceptNameCount,
                                         aEnv )
             == STL_SUCCESS );

    if( sRightExceptNameCount == 0 )
    {
        sRightExceptNames = NULL;
    }
    else
    {
        STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                    STL_SIZEOF( stlChar* ) * sRightExceptNameCount,
                                    (void **) &sRightExceptNames,
                                    KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }


    /**
     * Right Table의 모든 Column의 개수를 가져오기
     */

    STL_TRY( qlvGetAsteriskTargetCountMultiTables( aStmtInfo,
                                                   aJoinTableNode->mRightTableNode,
                                                   0,
                                                   sRightExceptNames,
                                                   &sRightColumnNameCount,
                                                   aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sRightColumnNameCount > 0 );


    /**
     * Right Table의 Column Name List 할당
     */

    STL_TRY( knlAllocRegionMem( &QLL_VALIDATE_MEM( aEnv ),
                                STL_SIZEOF( stlChar* ) * sRightColumnNameCount,
                                (void **) &sRightColumnNames,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );


    /**
     * Right Table의 모든 Column Name List를 가져오기
     */

    sTempCount = 0;
    STL_TRY( qlvGetAllColumnNamesFromTableNode( aJoinTableNode->mRightTableNode,
                                                0,
                                                sRightExceptNames,
                                                sRightColumnNames,
                                                &sTempCount,
                                                aEnv )
             == STL_SUCCESS );

    STL_DASSERT( sRightColumnNameCount == sTempCount );


    /**
     * Column Name List를 오름차순으로 정렬
     */

    sQSortContext.mElemType = KNL_QSORT_ELEM_8BYTE;
    sQSortContext.mIsUnique = STL_FALSE;
    sQSortContext.mCompare  = qlvCmpColumnName;
    sQSortContext.mCmpInfo  = NULL;

    (void)knlQSort( sRightColumnNames,
                    sRightColumnNameCount,
                    &sQSortContext );


    /**
     * Left Column Name List와 Right Column Name List를
     * 순차적으로 읽어가며 동일 Column 찾기
     */

    sLeft = 0;
    sRight = 0;
    while( STL_TRUE )
    {
        sColumnName = sLeftColumnNames[sLeft];
        sCmp = stlStrcmp( sColumnName, sRightColumnNames[sRight] );
        if( sCmp == 0 )
        {
            /**
             * Left Column Name과 Right Column Name이 동일한 경우
             */

            /**
             * Left Column Name의 다음번에 동일한 이름이 있는지 검사
             */

            if( ((sLeft + 1) < sLeftColumnNameCount) &&
                stlStrcmp( sColumnName, sLeftColumnNames[sLeft + 1] ) == 0 )
            {
                /**
                 * 동일한 이름이 있는 경우
                 */

                STL_THROW( RAMP_ERR_EXIST_SAME_COLUMN_NAME_IN_LEFT_TABLE );
            }


            /**
             * Right Column Name의 다음번에 동일한 이름이 있는지 검사
             */

            if( ((sRight + 1) < sRightColumnNameCount) &&
                stlStrcmp( sColumnName, sRightColumnNames[sRight + 1] ) == 0 )
            {
                /**
                 * 동일한 이름이 있는 경우
                 */

                STL_THROW( RAMP_ERR_EXIST_SAME_COLUMN_NAME_IN_RIGHT_TABLE );
            }


            /**
             * Column Name List에 추가
             */

            STL_TRY( qlvValidateJoinSpecNamedColumnInternal( 
                             aStmtInfo,
                             QLV_EXPR_PHRASE_FROM,
                             QLL_INIT_PLAN( aStmtInfo->mDBCStmt ),
                             (qlvInitNode*)aJoinTableNode,
                             sColumnName,
                             0, /* NodePos: 여기서는 Node Position이 없으므로 0으로 한다. */
                             STL_FALSE,
                             aNamedColumnList,
                             aEnv )
                     == STL_SUCCESS );

            sLeft++;
            sRight++;
        }
        else if( sCmp < 0 )
        {
            /**
             * Left Column Name의 위치이동
             */

            sLeft++;
        }
        else
        {
            /**
             * Right Column Name의 위치이동
             */

            sRight++;
        }

        if( sLeft >= sLeftColumnNameCount )
        {
            /**
             * Left Column Name을 모두 읽었음
             */

            break;
        }

        if( sRight >= sRightColumnNameCount )
        {
            /**
             * Right Column Name을 모두 읽었음
             */

            break;
        }
    }


    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EXIST_SAME_COLUMN_NAME_IN_LEFT_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_EXIST_SAME_COLUMN_NAME_IN_TABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sColumnName,
                      "left" );
    }

    STL_CATCH( RAMP_ERR_EXIST_SAME_COLUMN_NAME_IN_RIGHT_TABLE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_EXIST_SAME_COLUMN_NAME_IN_TABLE,
                      NULL,
                      QLL_ERROR_STACK(aEnv),
                      sColumnName,
                      "right");
    }

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Natural Join에서 동일한 이름의 Column들을 Named Column List로 구성
 * @param[in]  aTableNode       Table Node
 * @param[in]  aExceptNameCount Except Column Name Count
 * @param[in]  aExceptNames     Except Column Name List
 * @param[out] aColumnNames     Column Name List
 * @param[out] aColumnNameCount Column Name Count
 * @param[in]  aEnv             Environment
 */
stlStatus qlvGetAllColumnNamesFromTableNode( qlvInitNode    * aTableNode,
                                             stlInt32         aExceptNameCount,
                                             stlChar       ** aExceptNames,
                                             stlChar       ** aColumnNames,
                                             stlInt32       * aColumnNameCount,
                                             qllEnv         * aEnv )
{
    stlInt32                  i                 = 0;
    stlInt32                  j                 = 0;
    stlBool                   sExist            = STL_FALSE;
    stlChar                 * sColumnName       = NULL;

    ellDictHandle           * sTableHandle      = NULL;
    stlInt32                  sColumnCount      = 0;
    ellDictHandle           * sColumnHandles    = NULL;

    qlvInitSubTableNode     * sSubTableNode     = NULL;
    stlInt32                  sTargetCount      = 0;
    qlvInitTarget           * sTargets          = NULL;

    qlvInitJoinTableNode    * sJoinTableNode    = NULL;
    qlvNamedColumnItem      * sNamedColumnItem  = NULL;
    stlChar                ** sExceptNames      = NULL;
    stlInt32                  sExceptNameCount  = 0;


    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aTableNode != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnNames != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aColumnNameCount != NULL, QLL_ERROR_STACK(aEnv) );


    /**********************************************************
     * Table Node에서 모든 Column Name을 가져오기
     **********************************************************/

    switch( aTableNode->mType )
    {
        case QLV_NODE_TYPE_BASE_TABLE:

            /**
             * Base Table Node인 경우
             */

            sTableHandle    = & ((qlvInitBaseTableNode*)aTableNode)->mTableHandle;
            sColumnCount    = ellGetTableColumnCnt( sTableHandle );
            sColumnHandles  = ellGetTableColumnDictHandle( sTableHandle );

            for( i = 0; i < sColumnCount; i++ )
            {
                if ( ellGetColumnUnused( &sColumnHandles[i] ) == STL_TRUE )
                {
                    /**
                     * UNUSED Column 임
                     */
                    continue;
                }
                else
                {
                    sColumnName = ellGetColumnName( &sColumnHandles[i] );


                    /**
                     * Column Name이 Except Name List에 존재하는지 검사
                     */

                    sExist = STL_FALSE;
                    for( j = 0; j < aExceptNameCount; j++ )
                    {
                        if( stlStrcmp( sColumnName, aExceptNames[j] ) == 0 )
                        {
                            sExist = STL_TRUE;
                            break;
                        }
                    }

                    if( sExist == STL_TRUE )
                    {
                        /**
                         * Column이 Except Column Name에 존재하므로
                         * 다음 컬럼으로 넘어간다.
                         */

                        continue;
                    }

                    aColumnNames[*aColumnNameCount] = sColumnName;
                    (*aColumnNameCount)++;
                }
            }

            break;

        case QLV_NODE_TYPE_SUB_TABLE:

            /**
             * Sub Table Node인 경우
             */

            sSubTableNode = (qlvInitSubTableNode*)aTableNode;
            if( sSubTableNode->mQueryNode->mType == QLV_NODE_TYPE_QUERY_SET )
            {
                sTargetCount = ((qlvInitQuerySetNode*)(sSubTableNode->mQueryNode))->mSetTargetCount;
                sTargets     = ((qlvInitQuerySetNode*)(sSubTableNode->mQueryNode))->mSetTargets;
            }
            else
            {
                sTargetCount = ((qlvInitQuerySpecNode*)(sSubTableNode->mQueryNode))->mTargetCount;
                sTargets     = ((qlvInitQuerySpecNode*)(sSubTableNode->mQueryNode))->mTargets;
            }

            for( i = 0; i < sTargetCount; i++ )
            {
                sColumnName = sTargets[i].mDisplayName;


                /**
                 * Column Name이 Except Name List에 존재하는지 검사
                 */

                sExist = STL_FALSE;
                for( j = 0; j < aExceptNameCount; j++ )
                {
                    if( stlStrcmp( sColumnName, aExceptNames[j] ) == 0 )
                    {
                        sExist = STL_TRUE;
                        break;
                    }
                }

                if( sExist == STL_TRUE )
                {
                    /**
                     * Column이 Except Column Name에 존재하므로
                     * 다음 컬럼으로 넘어간다.
                     */

                    continue;
                }

                aColumnNames[*aColumnNameCount] = sColumnName;
                (*aColumnNameCount)++;
            }

            break;

        case QLV_NODE_TYPE_JOIN_TABLE:

            /**
             * Join Table Node인 경우
             */

            sJoinTableNode      = (qlvInitJoinTableNode*)aTableNode;
            sExceptNames        = aExceptNames;
            sExceptNameCount    = aExceptNameCount;

            if( (sJoinTableNode->mJoinSpec != NULL) &&
                (sJoinTableNode->mJoinSpec->mNamedColumnList != NULL) )
            {
                /**
                 * Named Column List가 존재하는 경우
                 */

                sNamedColumnItem = sJoinTableNode->mJoinSpec->mNamedColumnList->mHead;
                while( sNamedColumnItem != NULL )
                {
                    sColumnName = sNamedColumnItem->mName;


                    /**
                     * Column Name이 Except Name List에 존재하는지 검사
                     */

                    sExist = STL_FALSE;
                    for( j = 0; j < aExceptNameCount; j++ )
                    {
                        if( stlStrcmp( sColumnName, aExceptNames[j] ) == 0 )
                        {
                            sExist = STL_TRUE;
                            break;
                        }
                    }

                    if( sExist == STL_TRUE )
                    {
                        /**
                         * Column이 Except Column Name에 존재하므로
                         * 다음 컬럼으로 넘어간다.
                         */

                        continue;
                    }

                    aColumnNames[*aColumnNameCount] = sColumnName;
                    (*aColumnNameCount)++;


                    /**
                     * Except Column Name List에 추가
                     */

                    sExceptNames[sExceptNameCount] = sColumnName;
                    sExceptNameCount++;

                    sNamedColumnItem = sNamedColumnItem->mNext;
                }
            }


            /**
             * Left Table Node 처리
             */

            STL_TRY( qlvGetAllColumnNamesFromTableNode( sJoinTableNode->mLeftTableNode,
                                                        sExceptNameCount,
                                                        sExceptNames,
                                                        aColumnNames,
                                                        aColumnNameCount,
                                                        aEnv )
                     == STL_SUCCESS );


            /**
             * Right Table Node 처리
             */

            STL_TRY( qlvGetAllColumnNamesFromTableNode( sJoinTableNode->mRightTableNode,
                                                        sExceptNameCount,
                                                        sExceptNames,
                                                        aColumnNames,
                                                        aColumnNameCount,
                                                        aEnv )
                     == STL_SUCCESS );

            break;

        default:
            STL_DASSERT( 0 );
            break;
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief ROWID pseudo Column 정보를 생성하고, Base Table에 ROWID 정보를 설정한다.
 * @param[in]  aStmtInfo               Stmt Information
 * @param[in]  aPhraseType             Expr이 사용된 Phrase 유형
 * @param[in]  aRegionMem              Region Memory
 * @param[in]  aColumnPos              Column Position
 * @param[in]  aBaseTableNode          Base Table Node
 * @param[in]  aNeedCopy               Expr의 copy 필요여부
 * @param[out] aExist                  존재 여부
 * @param[out] aRefRowIdColExpr        RowId Column Expression
 * @param[in]  aEnv                    Environment
 */
stlStatus qlvMakeAndAddRowIdColumnOnBaseTable( qlvStmtInfo             * aStmtInfo,
                                               qlvExprPhraseType         aPhraseType,
                                               knlRegionMem            * aRegionMem,
                                               stlInt32                  aColumnPos,
                                               qlvInitBaseTableNode    * aBaseTableNode,
                                               stlBool                   aNeedCopy,
                                               stlBool                 * aExist,
                                               qlvInitExpression      ** aRefRowIdColExpr,
                                               qllEnv                  * aEnv )
{
    qlvInitExpression  * sCodeExpr     = NULL;
    qlvInitRowIdColumn * sCodeRowIdCol = NULL;
    ellTableType         sTableType;
    
    /**********************************************************
     * Parameter Validation
     **********************************************************/

    STL_PARAM_VALIDATE( aStmtInfo != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aBaseTableNode != NULL, QLL_ERROR_STACK(aEnv) );

    /**
     * Base Table이 아닌 경우, ROWID pseudo column을 쓸 수 없다.
     *  예) X$, D$ TABLE에는 ROWID pseudo column을 쓸 수 없다.
     * 
     * @todo 중간질의결과에 대한 ROWID 조회를 에러처리할 수 있도록 해야함.
     *       <BR> join view, 키 보존 테이블 ??? 위키 참조 ...
     *
     */

    sTableType = ellGetTableType( &(aBaseTableNode->mTableHandle) );
    STL_TRY_THROW( sTableType == ELL_TABLE_TYPE_BASE_TABLE,
                   RAMP_ERR_PSEUDO_COLUMN_CANNOT_SELECT );
    
    /**********************************************************
     * RowId Pseudo Column 에 대한 처리
     * **********************************************************/

    if( aBaseTableNode->mRefRowIdColumn == NULL )
    {
        /* Do Nothing */
    }
    else
    {
        if( (aBaseTableNode->mRefRowIdColumn->mPhraseType == QLV_EXPR_PHRASE_NA) ||
            (aBaseTableNode->mRefRowIdColumn->mPhraseType == QLV_EXPR_PHRASE_CONDITION) ||
            (aBaseTableNode->mRefRowIdColumn->mPhraseType == QLV_EXPR_PHRASE_FROM) )
        {
            aBaseTableNode->mRefRowIdColumn->mPhraseType = aPhraseType;
        }

        if( aNeedCopy == STL_TRUE )
        {
            /**
             * RowId Pseudo Column이 있는 경우, 기존 Expression 을 복사.
             */
            
            STL_TRY( qlvCopyExpr( aBaseTableNode->mRefRowIdColumn,
                                  & sCodeExpr,
                                  aRegionMem,
                                  aEnv )
                     == STL_SUCCESS );

            /* node position 설정 */
            sCodeExpr->mPosition = aColumnPos;
        }
        else
        {
            sCodeExpr = aBaseTableNode->mRefRowIdColumn;
        }

        STL_THROW( RAMP_FINISH );        
    }

    /**
     * aBaseTableNode->mRefRowIdColumn == NULL 인 경우,
     * RowId Pseudo Column에 대한 expression 생성
     */
   
    /**
     * Code Expression 공간 할당
     */

    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitExpression ),
                                (void **) & sCodeExpr,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCodeExpr,
               0x00,
               STL_SIZEOF( qlvInitExpression ) );

    /**
     * Init RowId Column 생성
     */
    
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( qlvInitRowIdColumn ),
                                (void **) & sCodeRowIdCol,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    stlMemset( sCodeRowIdCol,
               0x00,
               STL_SIZEOF( qlvInitRowIdColumn ) );

    /**
     * RowId Column Expression 에 Relation Node 설정
     */
    
    sCodeRowIdCol->mRelationNode = (qlvInitNode *)aBaseTableNode;

    /**
     * Code Expression 설정
     */

    QLV_VALIDATE_INCLUDE_EXPR_CNT( 1, aEnv );

    sCodeExpr->mType              = QLV_EXPR_TYPE_ROWID_COLUMN;
    sCodeExpr->mPhraseType        = aPhraseType;
    sCodeExpr->mPosition          = aColumnPos;
    sCodeExpr->mColumnName        = NULL;
    sCodeExpr->mIncludeExprCnt    = 1;
    sCodeExpr->mExpr.mRowIdColumn = sCodeRowIdCol;
    sCodeExpr->mIterationTime     = gPseudoColInfoArr[KNL_PSEUDO_COL_ROWID].mIterationTime;

    /**
     * Base Table Node 에 Ref RowId Column Expression 정보 설정
     */
    
    aBaseTableNode->mRefRowIdColumn = sCodeExpr;
    

    STL_RAMP( RAMP_FINISH );


    /**********************************************************
     * 마무리
     **********************************************************/
    
    /**
     * Output 설정
     */

    *aExist           = STL_TRUE;
    *aRefRowIdColExpr = sCodeExpr;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_PSEUDO_COLUMN_CANNOT_SELECT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      QLL_ERRCODE_PSEUDO_COLUMN_CANNOT_SELECT_FROM_NOT_WRITABLE,
                      qlgMakeErrPosString( aStmtInfo->mSQLString,
                                           aColumnPos,
                                           aEnv ),
                      QLL_ERROR_STACK(aEnv) );
    }   
    
    
    STL_FINISH;
    
    return STL_FAILURE;
}


/** @} qlnv */
