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
#include <knl.h>
#include <sclDef.h>
#include <scDef.h>
#include <scpProtocol.h>
#include <sclGeneral.h>
#include <sccCommunication.h>

/**
 * @file sclDescribeCol.c
 * @brief Communication Layer DescribeCol Protocol Routines
 */

/**
 * @addtogroup cmlProtocol
 * @{
 */


/**
 * @brief DescribeCol Command Protocol을 읽는다.
 * @param[in]  aHandle       communication handle
 * @param[out] aControl      Protocol Control
 * @param[out] aStatementId  statement identifier
 * @param[out] aColumnNumber column number
 * @param[in]  aEnv          env
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
stlStatus sclReadDescribeColCommand( sclHandle      * aHandle,
                                     stlInt8        * aControl,
                                     stlInt64       * aStatementId,
                                     stlInt32       * aColumnNumber,
                                     sclEnv         * aEnv )
{
    stlInt16    sResultType;
    scpCursor   sCursor;

    STL_TRY( scpBeginReading( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_READ_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );
    SCP_READ_INT8( aHandle, &sCursor, aControl, aEnv );
    SCP_READ_STATEMENT_ID( aHandle, &sCursor, aStatementId, aEnv );
    SCP_READ_VAR_INT32( aHandle, &sCursor, aColumnNumber, aEnv );

    STL_DASSERT( sResultType == CML_COMMAND_DESCRIBECOL );
    
    STL_TRY( scpEndReading( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief DescribeCol Result Protocol을 기록한다.
 * @param[in] aHandle                 communication handle
 * @param[in] aCatalogName            catalog name
 * @param[in] aSchemaName             schema name
 * @param[in] aTableName              table name
 * @param[in] aBaseTableName          base table name
 * @param[in] aColumnAliasName        column alias name
 * @param[in] aColumnLabel            column label
 * @param[in] aBaseColumnName         base column name
 * @param[in] aLiteralPrefix          literal prefix
 * @param[in] aLiteralSuffix          literal suffix
 * @param[in] aDbType                 db type
 * @param[in] aIntervalCode           interval code
 * @param[in] aNumPrecRadix           numeric interval radix
 * @param[in] aDatetimeIntervPrec     datatime interval precision
 * @param[in] aDisplaySize            display size
 * @param[in] aStringLengthUnit       string length unit
 * @param[in] aCharacterLength        character length
 * @param[in] aOctetLength            octet length
 * @param[in] aPrecision              precision
 * @param[in] aScale                  scale
 * @param[in] aNullable               nullable flag
 * @param[in] aAliasUnnamed           alias unnamed flag
 * @param[in] aCaseSensitive          case sensitive flag
 * @param[in] aUnsigned               unsigned flag
 * @param[in] aFixedPrecScale         fixed precision scale flag
 * @param[in] aUpdatable              updatable flag
 * @param[in] aAutoUnique             auto unique flag
 * @param[in] aRowVersion             row version flag
 * @param[in] aAbleLike               able like flag
 * @param[in] aErrorStack             기록할 Error Stack
 * @param[in] aEnv                    env
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
 * field sequence : if r == 0 then t-r-l-h-b-e-i-C-B-p-S-D-v-x-C-P-L-T-o-R-A-u-N-E-g-f-U-O-w-k
 *                  else if r == 1 then t-r-c-[s-a-m]^c-l-h-b-e-i-C-B-p-S-D-v-x-C-P-L-T-o-R-A-u-N-E-g-f-U-O-w-k
 *                  else t-r-c-[s-a-m]^c
 * 
 * @endcode
 */
stlStatus sclWriteDescribeColResult( sclHandle           * aHandle,
                                     stlChar             * aCatalogName,
                                     stlChar             * aSchemaName,
                                     stlChar             * aTableName,
                                     stlChar             * aBaseTableName,
                                     stlChar             * aColumnAliasName,
                                     stlChar             * aColumnLabel,
                                     stlChar             * aBaseColumnName,
                                     stlChar             * aLiteralPrefix,
                                     stlChar             * aLiteralSuffix,
                                     dtlDataType           aDbType,
                                     stlInt16              aIntervalCode,
                                     stlInt16              aNumPrecRadix,
                                     stlInt16              aDatetimeIntervPrec,
                                     stlInt64              aDisplaySize,
                                     dtlStringLengthUnit   aStringLengthUnit,
                                     stlInt64              aCharacterLength,
                                     stlInt64              aOctetLength,
                                     stlInt32              aPrecision,
                                     stlInt32              aScale,
                                     stlBool               aNullable,
                                     stlBool               aAliasUnnamed,
                                     stlBool               aCaseSensitive,
                                     stlBool               aUnsigned,
                                     stlBool               aFixedPrecScale,
                                     stlBool               aUpdatable,
                                     stlBool               aAutoUnique,
                                     stlBool               aRowVersion,
                                     stlBool               aAbleLike,
                                     stlErrorStack       * aErrorStack,
                                     sclEnv              * aEnv )
{
    scpCursor    sCursor;
    stlInt32     sLength;
    stlInt8      sDbType = aDbType;
    stlInt8      sStringLengthUnit = aStringLengthUnit;
    stlInt16     sResultType = CML_COMMAND_DESCRIBECOL;

    STL_TRY( scpBeginWriting( aHandle,
                              &sCursor,
                              aEnv )
             == STL_SUCCESS );

    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &sResultType, aEnv );

    STL_TRY( scpWriteErrorResult( aHandle,
                                  &sCursor,
                                  aErrorStack,
                                  aEnv ) == STL_SUCCESS );

    sLength = (aCatalogName == NULL ) ? 0 : stlStrlen( aCatalogName ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aCatalogName, sLength, aEnv );
    sLength = (aSchemaName == NULL ) ? 0 : stlStrlen( aSchemaName ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aSchemaName, sLength, aEnv );
    sLength = (aTableName == NULL ) ? 0 : stlStrlen( aTableName ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aTableName, sLength, aEnv );
    sLength = (aBaseTableName == NULL ) ? 0 : stlStrlen( aBaseTableName ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aBaseTableName, sLength, aEnv );
    sLength = (aColumnAliasName == NULL ) ? 0 : stlStrlen( aColumnAliasName ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aColumnAliasName, sLength, aEnv );
    sLength = (aColumnLabel == NULL ) ? 0 : stlStrlen( aColumnLabel ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aColumnLabel, sLength, aEnv );
    sLength = (aBaseColumnName == NULL ) ? 0 : stlStrlen( aBaseColumnName ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aBaseColumnName, sLength, aEnv );
    sLength = (aLiteralPrefix == NULL ) ? 0 : stlStrlen( aLiteralPrefix ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aLiteralPrefix, sLength, aEnv );
    sLength = (aLiteralSuffix == NULL ) ? 0 : stlStrlen( aLiteralSuffix ); 
    SCP_WRITE_N_VAR( aHandle, &sCursor, aLiteralSuffix, sLength, aEnv );

    SCP_WRITE_INT8( aHandle, &sCursor, &sDbType, aEnv );
    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &aIntervalCode, aEnv );
    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &aNumPrecRadix, aEnv );
    SCP_WRITE_VAR_INT16( aHandle, &sCursor, &aDatetimeIntervPrec, aEnv );

    SCP_WRITE_VAR_INT64( aHandle, &sCursor, &aDisplaySize, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &sStringLengthUnit, aEnv );
    SCP_WRITE_VAR_INT64( aHandle, &sCursor, &aCharacterLength, aEnv );
    SCP_WRITE_VAR_INT64( aHandle, &sCursor, &aOctetLength, aEnv );

    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aPrecision, aEnv );
    SCP_WRITE_VAR_INT32( aHandle, &sCursor, &aScale, aEnv );
    
    SCP_WRITE_INT8( aHandle, &sCursor, &aNullable, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aAliasUnnamed, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aCaseSensitive, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aUnsigned, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aFixedPrecScale, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aUpdatable, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aAutoUnique, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aRowVersion, aEnv );
    SCP_WRITE_INT8( aHandle, &sCursor, &aAbleLike, aEnv );

    STL_TRY( scpEndWriting( aHandle,
                            &sCursor,
                            aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
