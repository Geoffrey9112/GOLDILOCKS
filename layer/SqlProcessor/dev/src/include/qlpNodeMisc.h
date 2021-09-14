/*******************************************************************************
 * qlpNodeMisc.h
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

#ifndef _QLPNODEMISC_H_
#define _QLPNODEMISC_H_ 1

/**
 * @file qlpNodeMisc.h
 * @brief SQL Processor Layer Misc Nodes
 */


/**
 * @defgroup qlpNodeMisc Misc Nodes
 * @ingroup qlpNode
 * @{
 */


/*******************************************************************************
 * DEFINITIONS 
 ******************************************************************************/


#define QLP_PARSE_GET_MEM_MGR( parse_param )                    \
    ( ((qlpParseContext*)(parse_param->mContext))->mMemMgr )

#define QLP_PARSE_GET_ENV( parse_param )                        \
    ( ((qlpParseContext*)(parse_param->mContext))->mEnv )

#define QLP_PARSE_GET_PARAM( parse_param )                      \
    ( ((qlpParseContext*)(parse_param->mContext))->mParamList )

#define QLP_PARSE_GET_ALIAS( parse_param )                      \
    ( ((qlpParseContext*)(parse_param->mContext))->mAliasList )


/*******************************************************************************
 * STRUCTURES 
 ******************************************************************************/

struct qlpParseContext
{
    knlRegionMem  * mMemMgr;
    qllEnv        * mEnv;
    qlpList       * mParamList;     /**< qlpParameter */
    qlpList       * mAliasList;     /**< qlpAlias */
};


/*******************************************************************************
 * FUNCTIONS 
 ******************************************************************************/



    // QLP_TRIGGERDATA_TYPE,           /* IN ???.H */
    // QLP_RETURNSETINFO_TYPE,         /* IN ???.H */
    // QLP_WINDOWOBJECTDATA_TYPE,      /* PRIVATE IN ???.C */
    // QLP_TIDBITMAP_TYPE,             /* IN ???.H */
    // QLP_INLINECODEBLOCK_TYPE        /* IN ???.H */

        
/** @} */

#endif /* _QLPNODEMISC_H_ */
