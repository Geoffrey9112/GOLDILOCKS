/*******************************************************************************
 * eldt.h
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

#ifndef _ELDT_H_
#define _ELDT_H_ 1

/**
 * @file eldt.h
 * @brief Execution Layer Header Files.
 */

#include <elDef.h>
#include <eldtDef.h>

/*
 * Definition Schema
 */
#include <eldtDEFINITION_SCHEMA.h>

/*
 * Dictionary Tables
 */

/*
 * SQL standard core table
 */
#include <eldtTABLES.h>
#include <eldtCOLUMNS.h>
#include <eldtDATA_TYPE_DESCRIPTOR.h>

/*
 * SQL standard table
 */

#include <eldtAUTHORIZATIONS.h>
#include <eldtCATALOG_NAME.h>
#include <eldtCHECK_COLUMN_USAGE.h>
#include <eldtCHECK_CONSTRAINTS.h>
#include <eldtCHECK_TABLE_USAGE.h>
#include <eldtCOLUMN_PRIVILEGES.h>
#include <eldtKEY_COLUMN_USAGE.h>
#include <eldtREFERENTIAL_CONSTRAINTS.h>
#include <eldtROLE_AUTHORIZATION_DESCRIPTORS.h>
#include <eldtSCHEMATA.h>
#include <eldtSEQUENCES.h>
#include <eldtSQL_CONFORMANCE.h>
#include <eldtSQL_IMPLEMENTATION_INFO.h>
#include <eldtSQL_SIZING.h>
#include <eldtTABLE_CONSTRAINTS.h>
#include <eldtTABLE_PRIVILEGES.h>
#include <eldtUSAGE_PRIVILEGES.h>
#include <eldtVIEWS.h>
#include <eldtVIEW_TABLE_USAGE.h>

#include <eldtUnsupportedTables.h>

/*
 * Non standard table
 */

#include <eldtTYPE_INFO.h>
#include <eldtDATABASE_PRIVILEGES.h>
#include <eldtINDEX_KEY_COLUMN_USAGE.h>
#include <eldtINDEX_KEY_TABLE_USAGE.h>
#include <eldtINDEXES.h>
#include <eldtPENDING_TRANS.h>
#include <eldtSCHEMA_PRIVILEGES.h>
#include <eldtTABLESPACE_PRIVILEGES.h>
#include <eldtTABLESPACES.h>
#include <eldtUSER_SCHEMA_PATH.h>
#include <eldtUSERS.h>
#include <eldtSYNONYMS.h>
#include <eldtPUBLIC_SYNONYMS.h>
#include <eldtPROFILES.h>
#include <eldtPROFILE_PASSWORD_PARAMETER.h>
#include <eldtPROFILE_KERNEL_PARAMETER.h>
#include <eldtUSER_PASSWORD_HISTORY.h>

#endif /* _ELDT_H_ */
