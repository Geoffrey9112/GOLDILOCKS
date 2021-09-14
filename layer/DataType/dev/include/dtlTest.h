#define INPUT_ARGUMENT_CNT_ONE   ( 1 )
#define INPUT_ARGUMENT_CNT_TWO   ( 2 )
#define INPUT_ARGUMENT_CNT_THREE ( 3 )

#define STRING_BUFFER_SIZE 1024

typedef enum
{
    TEST_INTEGER_VALUE,
    TEST_STRING_VALUE
} TestInputValueType;

void TEST_PRINT_ERRORSTACK( stlErrorStack * aErrorStack );

stlStatus createAndInitValueEntry( dtlValueEntry ** aValueEntry,
                                   stlUInt16        aMaxEntryCount,
                                   stlErrorStack  * aErrorStack );

stlStatus makeDtlDataValue( dtlDataType     aType,
                            stlInt64        aPrecision,
                            dtlDataValue  * aDtlDataValue,
                            stlInt64      * aBufferLength,
                            stlErrorStack * aErrorStack );

stlStatus setValueInDtlDataValue( dtlDataType        aType,
                                  TestInputValueType aInputValueType,
                                  stlInt64           aIntegerValue,
                                  stlChar          * aStringValue,
                                  stlInt64           aPrecision,
                                  stlInt64           aScale,
                                  stlInt64           aAvailableSize,
                                  dtlDataValue     * aValue,
                                  stlErrorStack    * aErrorStack );

stlStatus printDtlValue( dtlDataValue    * aValue,
                         stlInt64          aAvailableSize,
                         stlChar         * aBuffer,
                         stlErrorStack   * aErrorStack );

stlStatus setBigInt( dtlBigIntType        aBigInt,
                     dtlDataValue       * aValue,
                     stlErrorStack      * aErrorStack );

stlStatus setInteger( dtlIntegerType       aInteger,
                      dtlDataValue       * aValue,
                      stlErrorStack      * aErrorStack );

stlStatus setDouble( dtlDoubleType        aDouble,
                     dtlDataValue       * aValue,
                     stlErrorStack      * aErrorStack );


stlStatus setNumericTrace( const stlChar      * aString,
                           stlInt32           * aPrecision,
                           stlInt32           * aScale,
                           dtlDataValue       * aValue,
                           stlErrorStack      * aErrorStack );

stlStatus setNumeric( const stlChar      * aString,
                      dtlDataValue       * aValue,
                      stlErrorStack      * aErrorStack );

stlStatus setNumericEx( const stlChar      * aString,
                        stlInt32             aPrecision,
                        stlInt32             aScale,
                        dtlDataValue       * aValue,
                        stlErrorStack      * aErrorStack );

stlStatus setVarChar( stlChar            * aInput,
                      dtlDataValue       * aResult,
                      stlErrorStack      * aErrorStack );

stlStatus setVarCharLength( stlInt64             aLength,
                            dtlDataValue       * aResult,
                            stlErrorStack      * aErrorStack );

stlStatus setVarBinary( stlUInt8           * aInput,
                        stlInt64             aInputLength,
                        dtlDataValue       * aResult,
                        stlErrorStack      * aErrorStack );

void dtfFreeDtlData( dtlDataValue * sValue );
