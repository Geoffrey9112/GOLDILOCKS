/*******************************************************************************
 * eldtPROFILE_KERNEL_PARAMETER.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: eldtPROFILE_KERNEL_PARAMETER.h 14729 2015-04-06 03:16:14Z leekmo $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ELDT_PROFILE_KERNEL_PARAMETER_H_
#define _ELDT_PROFILE_KERNEL_PARAMETER_H_ 1

/**
 * @file eldtPROFILE_KERNEL_PARAMETER.h
 * @brief PROFILE_KERNEL_PARAMETER dictionary base table
 */

/**
 * @defgroup eldtPROFILE_KERNEL_PARAMETER PROFILE_KERNEL_PARAMETER table
 * @ingroup eldtNonStandard
 * @{
 */

/**
 * @brief PROFILE_KERNEL_PARAMETER 테이블의 컬럼 순서
 */
typedef enum eldtColumnOrderPROFILE_KERNEL_PARAMETER
{
    /*
     * 식별자 영역
     */
    
    ELDT_KernelParam_ColumnOrder_PROFILE_ID = 0,
    ELDT_KernelParam_ColumnOrder_PARAMETER_ID,
    ELDT_KernelParam_ColumnOrder_PARAMETER_NAME,

    /*
     * 정보 영역
     */

    ELDT_KernelParam_ColumnOrder_IS_DEFAULT,
    ELDT_KernelParam_ColumnOrder_PARAMETER_VALUE,
    ELDT_KernelParam_ColumnOrder_PARAMETER_STRING,
    
    /*
     * 부가 정보 영역
     */


    ELDT_KernelParam_ColumnOrder_MAX

} eldtColumnOrderPROFILE_KERNEL_PARAMETER;


extern eldtDefinitionTableDesc   gEldtTableDescPROFILE_KERNEL_PARAMETER;
extern eldtDefinitionColumnDesc  gEldtColumnDescPROFILE_KERNEL_PARAMETER[ELDT_KernelParam_ColumnOrder_MAX];


/**
 * @brief PROFILE_KERNEL_PARAMETER 테이블의 KEY 제약 조건
 * - 정의가 필요한 제약 조건 
 */

typedef enum eldtKeyConstPROFILE_KERNEL_PARAMETER
{
    /*
     * Primary Key 영역
     */

    ELDT_KernelParam_Const_PRIMARY_KEY = 0,                  
    
    /*
     * Unique 영역
     */

    /*
     * Foreign Key 영역
     */

    ELDT_KernelParam_Const_FOREIGN_KEY_PROFILES,           /**< 비표준  */
    
    ELDT_KernelParam_Const_MAX
    
} eldtKeyConstPROFILE_KERNEL_PARAMETER;

extern eldtDefinitionKeyConstDesc  gEldtKeyConstDescPROFILE_KERNEL_PARAMETER[ELDT_KernelParam_Const_MAX];


/**
 * @brief PROFILE_KERNEL_PARAMETER 테이블의 부가적 INDEX
 */

typedef enum eldtIndexPROFILE_KERNEL_PARAMETER
{
    ELDT_KernelParam_Index_MAX = 0
    
} eldtIndexPROFILE_KERNEL_PARAMETER;




    
/** @} eldtPROFILE_KERNEL_PARAMETER */



#endif /* _ELDT_PROFILE_KERNEL_PARAMETER_H_ */
