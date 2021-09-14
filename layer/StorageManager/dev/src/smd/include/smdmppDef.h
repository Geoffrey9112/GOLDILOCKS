/*******************************************************************************
 * smdmppDef.h
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


#ifndef _SMDMPPDEF_H_
#define _SMDMPPDEF_H_ 1


/**
 * @file smdmppDef.h
 * @brief Storage Manager Layer Memory Pending Table Internal Definition
 */

/**
 * @defgroup smdmppInternal Component Internal Routines
 * @ingroup smd
 * @{
 */

typedef struct smdmppIterator
{
    smiIterator   mCommon;
    smlRid        mRowRid;
    void        * mSegIterator;
    smlPid        mPageBufPid;
    stlInt64      mPageBuf[SMP_PAGE_SIZE/STL_SIZEOF(stlInt64)];
} smdmppIterator;

#define SMDMPP_IS_DELETED( aRow ) ( (((stlChar*)aRow)[0] == 1) ? STL_TRUE : STL_FALSE )
#define SMDMPP_SET_DELETED( aRow )              \
    {                                           \
        ( ((stlChar*)aRow)[0] = 1 );            \
    }
#define SMDMPP_UNSET_DELETED( aRow )            \
    {                                           \
        ( ((stlChar*)aRow)[0] = 0 );            \
    }
#define SMDMPP_SET_VALID_SEQ( aRow, aValidSeq )                     \
    {                                                               \
        STL_WRITE_INT64( (((stlChar*)(aRow)) + 1), &(aValidSeq) );  \
    }
#define SMDMPP_GET_VALID_SEQ( aRow, aValidSeq )                     \
    {                                                               \
        STL_WRITE_INT64( &(aValidSeq), (((stlChar*)(aRow)) + 1) ); \
    }
#define SMDMPP_ROW_HDR_SIZE    (1 + STL_SIZEOF(stlInt64))

/** @} */
    
#endif /* _SMDDEF_H_ */
