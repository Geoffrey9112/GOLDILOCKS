/*******************************************************************************
 * smb.h
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


#ifndef _SMB_H_
#define _SMB_H_ 1

/**
 * @file smb.h
 * @brief Storage Manager Layer Database Internal Routines
 */

/**
 * @defgroup smb Database
 * @ingroup smInternal
 * @{
 */

stlStatus smbCreateDatabaseAtNoMount( smlEnv * aEnv );
stlStatus smbCreateDatabaseAtMount( smlEnv * aEnv );
stlStatus smbCreateDatabaseAtOpen( smlEnv * aEnv );
stlStatus smbDropDatabase( smlEnv * aEnv );

/** @} */
    
#endif /* _SMB_H_ */
