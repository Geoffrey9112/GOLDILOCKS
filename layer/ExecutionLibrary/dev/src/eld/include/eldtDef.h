/*******************************************************************************
 * eldtDef.h
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


#ifndef _ELDTDEF_H_
#define _ELDTDEF_H_ 1

/**
 * @file eldtDef.h
 * @brief Execution Library Dictionary Table Defintions
 */


/**
 * @defgroup eldt DEFINITION_SCHEMA
 * @ingroup eld
 * @brief
 * <BR> DEFINITION_SCHEMA 에 속한 Dictionary Table들은 다음과 같은 종속 관계를 갖는다.
 * <BR> 아래의 hierarchy가 모든 관계를 의미하는 것은 아니며, 주요 종속 관계를 기준으로 표현하였다.
 * - CATALOG_NAME
 *  - SCHEMATA
 *   - TABLES
 *    - COLUMNS
 *     - DATA_TYPE_DESCRIPTOR
 *      - DATA_TYPES
 *    - TABLE_CONSTRAINTS
 *     - CHECK_CONSTRAINTS
 *      - CHECK_TABLE_USAGE
 *      - CHECK_COLUMN_USAGE
 *     - KEY_COLUMN_USAGE
 *     - REFERENTIAL_CONSTRAINTS
 *   - VIEWS
 *    - VIEW_COLUMN_USAGE
 *    - VIEW_TABLE_USAGE
 *   - SEQUENCES
 *   - INDEXES
 *    - INDEX_KEY_COLUMN_USAGE
 *    - INDEX_KEY_TABLE_USAGE
 *  - TABLESPACES  
 *  - AUTHORIZATIONS
 *   - ROLE_AUTHORIZATION_DESCRIPTORS
 *   - USERS
 *    - USER_SCHEMA_PATH
 *   - DATABASE_PRIVILEGES
 *   - TABLESPACE_PRIVILEGES
 *   - SCHEMA_PRIVILEGES
 *    - TABLE_PRIVILEGES
 *    - COLUMN_PRIVILEGES
 *    - USAGE_PRIVILEGES
 *    - INDEX_PRIVILEGES
 *    
 * @{
 */

/**
 * @defgroup eldtSCHEMA DEFINITION_SCHEMA 정의 
 * @ingroup eldt
 * @{
 */

/** @} eldtSCHEMA */

/**
 * @defgroup eldtCore SQL Standard Core Base Tables
 * @ingroup eldt
 * @{
 * @remarks SQL 표준에서 정의하고 있으며, 반드시 필요한 Dictionary Table 임.
 */

/** @} eldtCore */

/**
 * @defgroup eldtStandard SQL Standard Base Tables
 * @ingroup eldt
 * @{
 * @remarks SQL 표준에서 정의하고 지원하는 Dictionary Table 임.
 */

/** @} eldtStandard */

/**
 * @defgroup eldtNonStandard Non-Standard Base Tables
 * @ingroup eldt
 * @{
 * @remarks SQL 표준에서 정의하고 있지 않은 Dictionary Table 임.
 */

/** @} eldtNonStandard */


/**
 * @defgroup eldtUnsupported Unsupported Feature Base Tables
 * @ingroup eldt
 * @{
 * @remarks SQL 표준에 정의된 Dictionary Table이나, 해당 기능을 지원하지 않는 테이블들.
 */

/** @} eldtUnsupported */

/** @} eldt */

/** @} elInternal */


#endif /* _ELDTDEF_H_ */
