/*******************************************************************************
 * cmlDescribeCol.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: cmlDescribeCol.c 6676 2012-12-13 08:39:46Z egon $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stl.h>
#include <dtl.h>
#include <cmlDef.h>
#include <cmDef.h>
#include <cmlProtocol.h>
#include <cmlGeneral.h>
#include <cmpCursor.h>
#include <cmpProtocol.h>

/**
 * @file cmlDescribeCol.c
 * @brief Communication Layer DescribeCol Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */

/**
 * @brief DescribeCol Command Protocol을 기록한다.
 * @param[in] aHandle       communication handle
 * @param[in] aControl      Protocol Control
 * @param[in] aStatementId  statement identifier
 * @param[in] aColumnNumber column number
 * @param[in] aErrorStack   error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - command (t)ype   : command type            var_int bytes
 * - (p)eriod         : protocol period         1 byte
 * - (s)tatement id   : statement identifier    var_int,var_int byte
 * - column (n)umber  : column number           var_int byte
 *
 * field sequence : t-p-s-n
 * 
 * @endcode
 */
stlStatus cmlWriteDescribeColCommand( cmlHandle      * aHandle,
                                      stlInt8          aControl,
                                      stlInt64         aStatementId,
                                      stlInt32         aColumnNumber,
                                      stlErrorStack  * aErrorStack )
{
    stlInt16    sCommandType = CML_COMMAND_DESCRIBECOL;
    cmpCursor   sCursor;

    STL_TRY( cmpBeginWriting( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_WRITE_VAR_INT16( aHandle, &sCursor, &sCommandType, aErrorStack );
    CMP_WRITE_INT8( aHandle, &sCursor, &aControl, aErrorStack );
    CMP_WRITE_STATEMENT_ID_ENDIAN( aHandle, &sCursor, &aStatementId, aErrorStack );
    CMP_WRITE_VAR_INT32( aHandle, &sCursor, &aColumnNumber, aErrorStack );

    STL_TRY( cmpEndWriting( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DescribeCol Result Protocol을 읽는다.
 * @param[in]  aHandle                 communication handle
 * @param[out] aIgnored                Ignore protocol 여부
 * @param[out] aCatalogName            catalog name
 * @param[out] aSchemaName             schema name
 * @param[out] aTableName              table name
 * @param[out] aBaseTableName          base table name
 * @param[out] aColumnAliasName        column alias name
 * @param[out] aColumnLabel            column label
 * @param[out] aBaseColumnName         base column name
 * @param[out] aLiteralPrefix          literal prefix
 * @param[out] aLiteralSuffix          literal suffix
 * @param[out] aDbType                 db type
 * @param[out] aIntervalCode           interval code
 * @param[out] aNumPrecRadix           numeric interval radix
 * @param[out] aDatetimeIntervPrec     datatime interval precision
 * @param[out] aDisplaySize            display size
 * @param[out] aStringLengthUnit       string length unit
 * @param[out] aCharacterLength        character length
 * @param[out] aOctetLength            octet length
 * @param[out] aPrecision              precision
 * @param[out] aScale                  scale
 * @param[out] aNullable               nullable flag
 * @param[out] aAliasUnnamed           alias unnamed flag
 * @param[out] aCaseSensitive          case sensitive flag
 * @param[out] aUnsigned               unsigned flag
 * @param[out] aFixedPrecScale         fixed precision scale flag
 * @param[out] aUpdatable              updatable flag
 * @param[out] aAutoUnique             auto unique flag
 * @param[out] aRowVersion             row version flag
 * @param[out] aAbleLike               able like flag
 * @param[in]  aErrorStack             error stack pointer
 * 
 * @par protocol sequence:
 * @code
 * 
 * - result (t)ype         : result type              var_int bytes
 * - e(r)ror level         : error level              1 byte
 * - cata(l)og name        : catalog name             variable
 * - sc(h)ema name         : schema name              variable
 * - ta(b)le name          : table name               variable
 * - bas(e) table name     : base table name          variable
 * - column al(i)as name   : column alias name        variable
 * - (C)olumn label        : column lebel             variable
 * - (B)ase column name    : base column name         variable
 * - literal (p)refix      : literal prefix           variable
 * - literal (S)uffix      : literal suffix           variable
 * - (D)b type             : db type                  1 byte
 * - inter(v)al code       : interval code            var_int bytes
 * - numeric prec. radi(x) : numeric precision radix  var_int bytes
 * - datetime int. pre(C). : datetime interval prec.  var_int bytes
 * - dis(P)lay size        : display size             var_int bytes
 * - string (L)ength unit  : string length unit       1 byte
 * - charac(T)er length    : character length         var_int bytes
 * - (o)ctet length        : octet length             var_int bytes
 * - p(R)ecision           : precision                var_int bytes
 * - sc(A)le               : scale                    var_int bytes
 * - n(u)llable            : nullable                 1 byte
 * - alias Un(N)amed       : alias unnamed            1 byte
 * - cas(E) sensitive      : case sensitive           1 byte
 * - unsi(g)ned            : unsigned                 1 byte
 * - (f)ixed prec. scale   : fixed precision scale    1 byte
 * - (U)pdatable           : updatable                1 byte
 * - aut(O) unique         : auto unique              1 byte
 * - ro(w) version         : row version              1 byte
 * - able li(k)e           : able like                1 byte
 * - error (c)ount         : error count              1 byte
 * - (s)ql state           : sql state                4 bytes
 * - n(a)tive error        : native error             4 bytes
 * - (m)essage text        : message text             variable
 *
 * field sequence : if r == 0 then t-r-l-h-b-e-i-C-B-p-S-D-v-x-C-P-T-o-R-A-u-N-E-g-f-U-O-w-k
 *                  else if r == 1 then t-r-c-[s-a-m]^c-l-h-b-e-i-C-B-p-S-D-v-x-C-P-T-o-R-A-u-N-E-g-f-U-O-w-k
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus cmlReadDescribeColResult( cmlHandle           * aHandle,
                                    stlBool             * aIgnored,
                                    stlChar             * aCatalogName,
                                    stlChar             * aSchemaName,
                                    stlChar             * aTableName,
                                    stlChar             * aBaseTableName,
                                    stlChar             * aColumnAliasName,
                                    stlChar             * aColumnLabel,
                                    stlChar             * aBaseColumnName,
                                    stlChar             * aLiteralPrefix,
                                    stlChar             * aLiteralSuffix,
                                    dtlDataType         * aDbType,
                                    stlInt16            * aIntervalCode,
                                    stlInt16            * aNumPrecRadix,
                                    stlInt16            * aDatetimeIntervPrec,
                                    stlInt64            * aDisplaySize,
                                    dtlStringLengthUnit * aStringLengthUnit,
                                    stlInt64            * aCharacterLength,
                                    stlInt64            * aOctetLength,
                                    stlInt32            * aPrecision,
                                    stlInt32            * aScale,
                                    stlBool             * aNullable,
                                    stlBool             * aAliasUnnamed,
                                    stlBool             * aCaseSensitive,
                                    stlBool             * aUnsigned,
                                    stlBool             * aFixedPrecScale,
                                    stlBool             * aUpdatable,
                                    stlBool             * aAutoUnique,
                                    stlBool             * aRowVersion,
                                    stlBool             * aAbleLike,
                                    stlErrorStack       * aErrorStack )
{
    cmpCursor    sCursor;
    stlInt16     sResultType;
    stlInt8      sDbType;
    stlInt8      sStringLengthUnit;
    stlInt32     sLength;

    STL_TRY( cmpBeginReading( aHandle,
                              &sCursor,
                              aErrorStack )
             == STL_SUCCESS );

    CMP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aErrorStack );
    STL_DASSERT( sResultType == CML_COMMAND_DESCRIBECOL );

    STL_TRY( cmpReadErrorResult( aHandle,
                                 aIgnored,
                                 &sCursor,
                                 aErrorStack )
             == STL_SUCCESS );

    STL_TRY_THROW( *aIgnored == STL_FALSE, RAMP_IGNORE );

    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aCatalogName, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aSchemaName, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aTableName, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aBaseTableName, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aColumnAliasName, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aColumnLabel, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aBaseColumnName, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aLiteralPrefix, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    CMP_READ_VAR_STRING_ENDIAN( aHandle, &sCursor, aLiteralSuffix, sLength, STL_MAX_SQL_IDENTIFIER_LENGTH, aErrorStack );
    
    CMP_READ_INT8( aHandle, &sCursor, &sDbType, aErrorStack );
    CMP_READ_VAR_INT16( aHandle, &sCursor, aIntervalCode, aErrorStack );
    CMP_READ_VAR_INT16( aHandle, &sCursor, aNumPrecRadix, aErrorStack );
    CMP_READ_VAR_INT16( aHandle, &sCursor, aDatetimeIntervPrec, aErrorStack );

    CMP_READ_VAR_INT64( aHandle, &sCursor, aDisplaySize, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, &sStringLengthUnit, aErrorStack );
    CMP_READ_VAR_INT64( aHandle, &sCursor, aCharacterLength, aErrorStack );
    CMP_READ_VAR_INT64( aHandle, &sCursor, aOctetLength, aErrorStack );

    CMP_READ_VAR_INT32( aHandle, &sCursor, aPrecision, aErrorStack );
    CMP_READ_VAR_INT32( aHandle, &sCursor, aScale, aErrorStack );
    
    CMP_READ_INT8( aHandle, &sCursor, aNullable, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aAliasUnnamed, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aCaseSensitive, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aUnsigned, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aFixedPrecScale, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aUpdatable, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aAutoUnique, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aRowVersion, aErrorStack );
    CMP_READ_INT8( aHandle, &sCursor, aAbleLike, aErrorStack );

    STL_TRY( cmpEndReading( aHandle,
                            &sCursor,
                            aErrorStack )
             == STL_SUCCESS );

    *aDbType           = sDbType;
    *aStringLengthUnit = sStringLengthUnit;

    return STL_SUCCESS;

    STL_CATCH( RAMP_IGNORE )
    {
        STL_ASSERT( cmpEndReading( aHandle,
                                   &sCursor,
                                   aErrorStack ) == STL_SUCCESS );
        return STL_SUCCESS;
    }
    
    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
