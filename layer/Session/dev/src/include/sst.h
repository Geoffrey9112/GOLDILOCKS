/*******************************************************************************
 * sst.h
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


#ifndef _SST_H_
#define _SST_H_ 1

/**
 * @file sst.h
 * @brief Session Statement Routines
 */

stlStatus sstFind( stlInt64        aStatementId,
                   sslStatement ** aStatement,
                   sslEnv        * aEnv );

#endif /* _SST_H_ */
