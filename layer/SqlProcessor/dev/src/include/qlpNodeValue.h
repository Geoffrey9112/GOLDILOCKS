/*******************************************************************************
 * qlpNodeValue.h
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

#ifndef _QLPNODEVALUE_H_
#define _QLPNODEVALUE_H_ 1

/**
 * @file qlpNodeValue.h
 * @brief SQL Processor Layer Value Nodes 
 */


/**
 * @defgroup qlpNodeValue Value Nodes 
 * @ingroup qlpNode
 * @{
 */


/*******************************************************************************
 * DEFINITIONS 
 ******************************************************************************/

#define QLP_GET_INT_VALUE( node )   ( ((qlpValue*)(node))->mValue.mInteger )
#define QLP_GET_PTR_VALUE( node )   ( ((qlpValue*)(node))->mValue.mString )


/*******************************************************************************
 * STRUCTURES 
 ******************************************************************************/


/**
 * @brief value : QLP_INTEGER_TYPE,
 *                QLP_FLOAT_TYPE,
 *                QLP_STRING_TYPE,
 *                QLP_BITSTRING_TYPE,
 *                QLP_NULL_TYPE (qlpConstantValue : parsetree)
 */
struct qlpValue
{
    qllNodeType     mType;           /**< node type */
    stlInt32        mNodePos;        /**< node position */

    union
    {
        stlInt64    mInteger;        /**< integer value : integer */
        stlChar   * mString;         /**< string value : string, float, bit-string */
    } mValue;
    
};




/*******************************************************************************
 * FUNCTIONS 
 ******************************************************************************/


        
/** @} */

#endif /* _QLPNODEVALUE_H_ */
