/*******************************************************************************
 * eldtPROFILE_PASSWORD_PARAMETER.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldtPROFILE_PASSWORD_PARAMETER.h 14729 2015-04-06 03:16:14Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELDT_PROFILE_PASSWORD_PARAMETER_H_
#define _ELDT_PROFILE_PASSWORD_PARAMETER_H_ 1

/**
 * @file eldtPROFILE_PASSWORD_PARAMETER.h
 * @brief PROFILE_PASSWORD_PARAMETER dictionary base table
 */

/**
 * @defgroup eldtPROFILE_PASSWORD_PARAMETER PROFILE_PASSWORD_PARAMETER table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief PROFILE_PASSWORD_PARAMETER 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderPROFILE_PASSWORD_PARAMETER
{
    /*
     * 식별자 영역
     */
    
    ELDT_PassParam_ColumnOrder_PROFILE_ID = 0,
    ELDT_PassParam_ColumnOrder_PARAMETER_ID,
    ELDT_PassParam_ColumnOrder_PARAMETER_NAME,

    /*
     * 정보 영역
     */

    ELDT_PassParam_ColumnOrder_IS_DEFAULT,
    ELDT_PassParam_ColumnOrder_PARAMETER_VALUE,
    ELDT_PassParam_ColumnOrder_PARAMETER_STRING,
    
    /*
     * 부가 정보 영역
     */


    ELDT_PassParam_ColumnOrder_MAX

} eldtColumnOrderPROFILE_PASSWORD_PARAMETER;


extern eldtDefinitionTableDesc   gEldtTableDescPROFILE_PASSWORD_PARAMETER;
extern eldtDefinitionColumnDesc  gEldtColumnDescPROFILE_PASSWORD_PARAMETER[ELDT_PassParam_ColumnOrder_MAX];


/**
 * @brief PROFILE_PASSWORD_PARAMETER 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 */

typedef enum eldtKeyConstPROFILE_PASSWORD_PARAMETER
{
    /*
     * Primary Key 영역
     */

    ELDT_PassParam_Const_PRIMARY_KEY = 0,                  
    
    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */

    ELDT_PassParam_Const_FOREIGN_KEY_PROFILES,           /**< 비표준  */
    
    ELDT_PassParam_Const_MAX
    
} eldtKeyConstPROFILE_PASSWORD_PARAMETER;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescPROFILE_PASSWORD_PARAMETER[ELDT_PassParam_Const_MAX];


/**
 * @brief PROFILE_PASSWORD_PARAMETER 테이블의 부가적 INDEX
 */

typedef enum eldtIndexPROFILE_PASSWORD_PARAMETER
{
    ELDT_PassParam_Index_MAX = 0
    
} eldtIndexPROFILE_PASSWORD_PARAMETER;




    
/** @} eldtPROFILE_PASSWORD_PARAMETER */



#endif /* _ELDT_PROFILE_PASSWORD_PARAMETER_H_ */
