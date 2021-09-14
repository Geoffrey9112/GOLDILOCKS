package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;

public class GoldilocksRowId extends RowIdBase
{
    public static GoldilocksRowId createRowId(String aRowIdString) throws SQLException
    {
        byte[] sRowIdData = new byte[ROW_ID_BYTE_LEN];
        toBytes(aRowIdString, sRowIdData);
        return new GoldilocksRowId(sRowIdData);
    }
    
    GoldilocksRowId(byte[] aValue)
    {
        super(aValue);
    }
}

