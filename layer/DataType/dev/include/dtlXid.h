/****************************************
 * dtlXid.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlXid.h 2163 2011-10-27 08:44:10Z jhkim $
 *
 * NOTES
 *    
 *
 ***************************************/


#ifndef _DTLXID_H_
#define _DTLXID_H_ 1

/**
 * @file dtlXid.h
 * @brief Datatype Layer XID
 */

STL_BEGIN_DECLS


/**
 * @addtogroup dtlXid
 * @{
 */

stlBool dtlValidateXid( stlXid * aXid );
    
stlBool dtlIsEqualXid( stlXid  * aXid1,
                       stlXid  * aXid2 );

stlStatus dtlXidToString( stlXid        * aXid,
                          stlInt32        aBufferSize,
                          stlChar       * aXidString,
                          stlErrorStack * aErrorStack );

stlStatus dtlStringToXid( stlChar       * aXidString,
                          stlXid        * aXid,
                          stlErrorStack * aErrorStack );

/* @} */
    
STL_END_DECLS

#endif /* _DTLXID_H_ */
