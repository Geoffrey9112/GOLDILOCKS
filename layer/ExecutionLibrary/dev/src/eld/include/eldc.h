/*******************************************************************************
 * eldc.h
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

#ifndef _ELDC_H_
#define _ELDC_H_ 1

/**
 * @file eldc.h
 * @brief Execution Layer Header Files.
 */

#include <elDef.h>

#include <eldcDef.h>
#include <eldcDictHash.h>

/*
 * Cache for SQL-Object descriptor
 */

#include <eldcAuthorizationDesc.h>
#include <eldcCatalogDesc.h>
#include <eldcColumnDesc.h>
#include <eldcConstraintDesc.h>
#include <eldcFxColumnDesc.h>
#include <eldcFxTableDesc.h>
#include <eldcIndexDesc.h>
#include <eldcSchemaDesc.h>
#include <eldcSequenceDesc.h>
#include <eldcTableDesc.h>
#include <eldcTablespaceDesc.h>
#include <eldcSynonymDesc.h>
#include <eldcPublicSynonymDesc.h>
#include <eldcProfileDesc.h>

/*
 * Cache for Privilege descriptor
 */

#include <eldcPrivDesc.h>

#include <eldcColumnPrivDesc.h>
#include <eldcDatabasePrivDesc.h>
#include <eldcSchemaPrivDesc.h>
#include <eldcTablePrivDesc.h>
#include <eldcTablespacePrivDesc.h>
#include <eldcUsagePrivDesc.h>

/*
 * Non-Service Cache for SQL-Object descriptor
 */

#include <eldcNonServiceCache.h>

#endif /* _ELDC_H_ */
