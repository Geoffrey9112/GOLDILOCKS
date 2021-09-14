/*******************************************************************************
 * zlvVarBinaryToC.c
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

#include <cml.h>
#include <goldilocks.h>
#include <zlDef.h>
#include <zle.h>
#include <zlvSqlToC.h>

/**
 * @file zlvVarBinaryToC.c
 * @brief Gliese API Internal Converting Data from SQL Variable Binary to C Data Types Routines.
 */

/**
 * @addtogroup zlvVarBinaryToC
 * @{
 */

/*
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms713559%28v=VS.85%29.aspx
 */

/*
 * SQL_VARBINARY -> SQL_C_CHAR
 */
ZLV_DECLARE_SQL_TO_C( VarBinary, Char )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * (Byte length of data) * 2 < BufferLength  | Data            | Length of data in bytes | n/a
     * (Byte length of data) * 2 >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ================================================================================================
     */
    
    return zlvSqlBinaryToCChar( aStmt,
                                aValueBlockList,
                                aDataValue,
                                aOffset,
                                aTargetValuePtr,
                                aIndicator,
                                aArdRec,
                                aReturn,
                                aErrorStack );
}

/*
 * SQL_VARBINARY -> SQL_C_WCHAR
 */
ZLV_DECLARE_SQL_TO_C( VarBinary, Wchar )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * (Byte length of data) * 2 < BufferLength  | Data            | Length of data in bytes | n/a
     * (Byte length of data) * 2 >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ================================================================================================
     */
    
    return zlvSqlBinaryToCWchar( aStmt,
                                 aValueBlockList,
                                 aDataValue,
                                 aOffset,
                                 aTargetValuePtr,
                                 aIndicator,
                                 aArdRec,
                                 aReturn,
                                 aErrorStack );
}

/*
 * SQL_VARBINARY -> SQL_C_LONGVARCHAR
 */
ZLV_DECLARE_SQL_TO_C( VarBinary, LongVarChar )
{
    /*
     * ================================================================================================
     * Test                                      | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ================================================================================================
     * (Byte length of data) * 2 < BufferLength  | Data            | Length of data in bytes | n/a
     * (Byte length of data) * 2 >= BufferLength | Truncated data  | Length of data in bytes | 01004
     * ================================================================================================
     */
    
    return zlvSqlBinaryToCLongVarChar( aStmt,
                                       aValueBlockList,
                                       aDataValue,
                                       aOffset,
                                       aTargetValuePtr,
                                       aIndicator,
                                       aArdRec,
                                       aReturn,
                                       aErrorStack );
}

/*
 * SQL_VARBINARY -> SQL_C_BINARY
 */
ZLV_DECLARE_SQL_TO_C( VarBinary, Binary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */
    
    return zlvSqlBinaryToCBinary( aStmt,
                                  aValueBlockList,
                                  aDataValue,
                                  aOffset,
                                  aTargetValuePtr,
                                  aIndicator,
                                  aArdRec,
                                  aReturn,
                                  aErrorStack );
}

/*
 * SQL_VARBINARY -> SQL_C_LONGVARBINARY
 */
ZLV_DECLARE_SQL_TO_C( VarBinary, LongVarBinary )
{
    /*
     * ==========================================================================================
     * Test                                | *TargetValuePtr | *StrLen_or_IndPtr       | SQLSTATE
     * ==========================================================================================
     * Byte length of data <= BufferLength | Data            | Length of data in bytes | n/a
     * Byte length of data > BufferLength  | Truncated data  | Length of data in bytes | 01004
     * ==========================================================================================
     */
    
    return zlvSqlBinaryToCLongVarBinary( aStmt,
                                         aValueBlockList,
                                         aDataValue,
                                         aOffset,
                                         aTargetValuePtr,
                                         aIndicator,
                                         aArdRec,
                                         aReturn,
                                         aErrorStack );
}

/** @} */

