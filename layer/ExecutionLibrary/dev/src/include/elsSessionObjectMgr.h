/*******************************************************************************
 * elsSessionObjectMgr.h
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


#ifndef _ELS_SESSION_OBJECT_MGR_H_
#define _ELS_SESSION_OBJECT_MGR_H_ 1

/**
 * @file elsSessionObjectMgr.h
 * @brief Execution Library Layer Session Object Manager
 */

/**
 * @defgroup elsSessObjMgr Session Object Manager
 * @ingroup els
 * @{
 */

#define ELS_SESSION_OBJECT_MEM_INIT_SIZE    ( 8 * 1024 )
#define ELS_SESSION_OBJECT_MEM_NEXT_SIZE    ( 8 * 1024 )

#define ELS_SESSION_HASH_MEM_INIT_SIZE      ( 8 * 1024 )
#define ELS_SESSION_HASH_MEM_NEXT_SIZE      ( 8 * 1024 )

#define ELS_SESSION_CURSOR_SLOT_MEM_INIT_SIZE    ( 8 * 1024 )
#define ELS_SESSION_CURSOR_SLOT_MEM_NEXT_SIZE    ( 8 * 1024 )

stlStatus  elsInitSessObjectMgr( ellSessionEnv    * aSessEnv,
                                 knlSessionType     aSessType,
                                 knlSessEnvType     aSessEnvType,
                                 ellEnv           * aEnv );

stlStatus  elsFiniSessObjectMgr( ellEnv * aEnv );

/** @} elsSessObjMgr */
    
#endif /* _ELS_SESSION_OBJECT_MGR_H_ */
