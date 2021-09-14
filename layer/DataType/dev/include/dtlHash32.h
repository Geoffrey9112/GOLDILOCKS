/*******************************************************************************
 * dtlHash32.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtlHash32.h 8643 2013-06-05 02:42:53Z mkkim $
 *
 * NOTES
 *
 *
 ******************************************************************************/
#ifndef _DTL_HASH_32_H_
#define _DTL_HASH_32_H_ 1


/**
 * @file dtlHash32.h
 * @brief DataType Layer 32 Bit Hash 함수 
 */

/**
 * @addtogroup dtlHash32
 * @{
 */

stlUInt32 dtlHash32Numeric( register const stlUInt8 * aKeyValue,
                            register stlInt64         aKeyLength );

stlUInt32 dtlHash32Smallint( register const stlUInt8 * aKeyValue,
                             register stlInt64         aKeyLength );

stlUInt32 dtlHash32Int( register const stlUInt8 * aKeyValue,
                        register stlInt64         aKeyLength );

stlUInt32 dtlHash32BigInt( register const stlUInt8 * aKeyValue,
                           register stlInt64         aKeyLength );

stlUInt32 dtlHash32CharString( register const stlUInt8 * aKeyValue,
                               register stlInt64         aKeyLength );

stlUInt32 dtlHash32BinaryString( register const stlUInt8 * aKeyValue,
                                 register stlInt64         aKeyLength );

stlUInt32 dtlHash32Any( register const stlUInt8 * aKeyValue,
                        register stlInt64         aKeyLength );

stlUInt32 dtlHash32IntervalYM( register const stlUInt8 * aKeyValue,
                               register stlInt64         aKeyLength );

stlUInt32 dtlHash32IntervalDS( register const stlUInt8 * aKeyValue,
                               register stlInt64         aKeyLength );

/** @} dtlHash32 */


#endif /* _DTL_HASH_32_H_ */
