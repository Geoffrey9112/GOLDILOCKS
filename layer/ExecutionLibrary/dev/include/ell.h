/*******************************************************************************
 * ell.h
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

#ifndef _ELL_H_
#define _ELL_H_ 1

/**
 * @file ell.h
 * @brief Execution Layer Header Files.
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <sml.h>

#include <ellCursorDef.h>
#include <ellDef.h>
#include <ellGeneral.h>
#include <ellTypeDef.h>
#include <ellHashValue.h>
#include <ellSuppLogDef.h>

/*
 * Dictionary Management
 */

#include <ellTransDDL.h>
#include <ellObjectHandle.h>
#include <ellPrivilegeHandle.h>

/*
 * SQL standard core table
 */


#include <ellDictTable.h>
#include <ellDictColumn.h>
#include <ellDictColumnType.h>

/*
 * SQL standard table
 */
#include <ellDictAuthorization.h>
#include <ellDictCheckColumnUsage.h>
#include <ellDictCheckColumnUsage.h>
#include <ellDictCheckConstraint.h>
#include <ellDictCheckTableUsage.h>
#include <ellDictColumnPrivilege.h>
#include <ellDictDatabase.h>
#include <ellDictKeyColumnUsage.h>
#include <ellDictReferentialConstraint.h>
#include <ellDictRoleAuthorizationDescriptor.h>
#include <ellDictSchema.h>
#include <ellDictSqlConformance.h>
#include <ellDictSqlImplementationInfo.h>
#include <ellDictSqlSizing.h>
#include <ellDictSequence.h>
#include <ellDictTableConstraint.h>
#include <ellDictTablePrivilege.h>
#include <ellDictUsagePrivilege.h>
#include <ellDictViewTableUsage.h>
#include <ellDictView.h>
#include <ellDictProfile.h>

/*
 * Non standard table
 */

#include <ellDictBuiltInDataType.h>
#include <ellDictDatabasePrivilege.h>
#include <ellDictIndexKeyColumnUsage.h>
#include <ellDictIndexKeyTableUsage.h>
#include <ellDictIndex.h>
#include <ellDictSchemaPrivilege.h>
#include <ellDictTablespacePrivilege.h>
#include <ellDictTablespace.h>
#include <ellDictUserSchemaPath.h>
#include <ellDictUser.h>
#include <ellDictPendingTrans.h>
#include <ellDictSynonym.h>
#include <ellDictPublicSynonym.h>

/*
 * Session Object
 */

#include <ellCursor.h>

/*
 * DML Support API
 */

#include <ellDataValue.h>

/*
 * Nodes
 */

#include <ellNodeDef.h>

/*
 * Functions
 */

#include <ellFunction.h>

/*
 * SESSION FUNCTION
 */

#include <ellSessionFunc.h>

/*
 * SCAN & FILTER
 */

#include <ellFilter.h>

/*
 * PSEUDO COLUMN
 */

#include <ellPseudoCol.h>

#endif /* _ELL_H_ */
