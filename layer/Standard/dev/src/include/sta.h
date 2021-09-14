/*******************************************************************************
 * sta.h
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


#ifndef _STA_H_
#define _STA_H_ 1

STL_BEGIN_DECLS

stlUInt32 staRead32( volatile stlUInt32 * aMem );
void staSet32( volatile stlUInt32 * aMem,
               stlUInt32            aVal );
stlUInt32 staAdd32( volatile stlUInt32 * aMem,
                    stlUInt32            aVal );
void staSub32( volatile stlUInt32 * aMem,
               stlUInt32            aVal );
stlInt32 staDec32( volatile stlUInt32 * aMem );
stlUInt32 staXchg32( volatile stlUInt32 * aMem,
                     stlUInt32            aVal );

#if( STL_SIZEOF_VOIDP == 8 )

stlUInt64 staRead64( volatile stlUInt64 * aMem );

void staSet64( volatile stlUInt64 * aMem,
               stlUInt64            aVal );
stlUInt64 staAdd64( volatile stlUInt64 * aMem,
                    stlUInt64            aVal );
void staSub64( volatile stlUInt64 * aMem,
               stlUInt64            aVal );
stlInt64 staDec64( volatile stlUInt64 * aMem );
stlUInt64 staCas64( volatile stlUInt64 * aMem,
                    stlUInt64            aWith,
                    stlUInt64            aCmp );
stlUInt64 staXchg64( volatile stlUInt64 * aMem,
                     stlUInt64           aVal );

#endif

void * staCasPtr( volatile void ** aMem,
                  void           * aWith,
                  const void     * aCmp );
void * staXchgPtr( volatile void ** aMem,
                   void           * aWith );

STL_END_DECLS

#endif /* _STA_H_ */
