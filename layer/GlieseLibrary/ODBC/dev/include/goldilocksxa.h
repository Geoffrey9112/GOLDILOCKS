/*******************************************************************************
 * goldilocksxa.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: goldilocksxa.h 7852 2013-03-28 03:22:38Z htkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _GOLDILOCKSXA_H_
#define _GOLDILOCKSXA_H_ 1

/**
 * @file goldilocksxa.h
 * @brief GOLDILOCKS XA Routines.
 */

/**
 * @defgroup goldilocks_xa XA related routines
 * @{
 */

extern xa_switch_t goldilocks_xa_switch;

xa_switch_t* SQLGetXaSwitch();
SQLHANDLE    SQLGetXaConnectionHandle(int *rm_id);

/** @} */

#endif /* _GOLDILOCKSXA_H_ */
