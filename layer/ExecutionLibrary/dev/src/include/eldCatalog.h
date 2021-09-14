/*******************************************************************************
 * eldCatalog.h
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


#ifndef _ELD_CATALOG_H_
#define _ELD_CATALOG_H_ 1

/**
 * @file eldCatalog.h
 * @brief Catalog Dictionary 관리 루틴 
 */


/**
 * @defgroup eldCatalog Catalog(database) Descriptor 관리 루틴 
 * @internal 
 * @ingroup eldDesc
 * @{
 */

stlStatus eldInsertCatalogDesc( smlTransId            aTransID,
                                smlStatement        * aStmt,
                                stlChar             * aDatabaseName,
                                stlChar             * aDatabaseComment,
                                dtlCharacterSet       aCharacterSetID,
                                stlChar             * aCharacterSetName,
                                dtlStringLengthUnit   aCharLengthUnit,
                                stlChar             * aTimeZoneIntervalString,
                                stlInt64              aDefaultDataTablespaceID,
                                stlInt64              aDefaultTempTablespaceID,
                                ellEnv              * aEnv );


/** @} eldCatalog */

#endif /* _ELD_CATALOG_H_ */
