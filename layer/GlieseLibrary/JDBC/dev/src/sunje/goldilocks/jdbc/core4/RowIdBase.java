package sunje.goldilocks.jdbc.core4;

import java.sql.SQLException;
import java.util.Arrays;

import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class RowIdBase
{
    private static final char[] BASE64 =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/".toCharArray();
    private static final byte[] BASE64_LOOKUP = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
      
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

    protected static final int ROW_ID_STR_LEN = 23;
    protected static final int ROW_ID_BYTE_LEN = 16;
    
    private byte[] mValue;
    private String mStringValue;
    
    public static void toBytes(String aRowId, byte[] aResult) throws SQLException
    {
        if ((aRowId == null) || (aRowId.length() != ROW_ID_STR_LEN))
        {
            ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aRowId, "RowId");
        }
        for (int i=0; i<ROW_ID_STR_LEN; i++)
        {
            if (BASE64_LOOKUP[(int)aRowId.charAt(i)] == -1)
            {
                ErrorMgr.raise(ErrorMgr.VALUE_OUT_OF_RANGE, aRowId, "RowId");
            }
        }
        
        long sValue = 0;
        
        sValue = ((long)BASE64_LOOKUP[(int)aRowId.charAt(0)] << 60) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(1)] << 54) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(2)] << 48) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(3)] << 42) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(4)] << 36) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(5)] << 30) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(6)] << 24) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(7)] << 18) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(8)] << 12) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(9)] << 6) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(10)]);
        
        aResult[0] = (byte)((sValue >> 56) & 0xFF);
        aResult[1] = (byte)((sValue >> 48) & 0xFF);
        aResult[2] = (byte)((sValue >> 40) & 0xFF);
        aResult[3] = (byte)((sValue >> 32) & 0xFF);
        aResult[4] = (byte)((sValue >> 24) & 0xFF);
        aResult[5] = (byte)((sValue >> 16) & 0xFF);
        aResult[6] = (byte)((sValue >> 8) & 0xFF);
        aResult[7] = (byte)(sValue & 0xFF);
        
        sValue = ((long)BASE64_LOOKUP[(int)aRowId.charAt(11)] << 12) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(12)] << 6) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(13)]);
 
        aResult[8] = (byte)((sValue >> 8) & 0xFF);
        aResult[9] = (byte)(sValue & 0xFF);
        
        sValue = ((long)BASE64_LOOKUP[(int)aRowId.charAt(14)] << 30) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(15)] << 24) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(16)] << 18) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(17)] << 12) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(18)] << 6) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(19)]);

        aResult[10] = (byte)((sValue >> 24) & 0xFF);
        aResult[11] = (byte)((sValue >> 16) & 0xFF);
        aResult[12] = (byte)((sValue >> 8) & 0xFF);
        aResult[13] = (byte)(sValue & 0xFF);

        sValue = ((long)BASE64_LOOKUP[(int)aRowId.charAt(20)] << 12) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(21)] << 6) |
                 ((long)BASE64_LOOKUP[(int)aRowId.charAt(22)]);
        
        aResult[14] = (byte)((sValue >> 8) & 0xFF);
        aResult[15] = (byte)(sValue & 0xFF);
    }
    
    public static String toString(byte[] aRowId)
    {
        char[] sChars = new char[ROW_ID_STR_LEN];
        
        /* Object ID */
        long sTemp = (((long)(aRowId[0] & 0xFF)) << 56) |
                     (((long)(aRowId[1] & 0xFF)) << 48) |
                     (((long)(aRowId[2] & 0xFF)) << 40) |
                     (((long)(aRowId[3] & 0xFF)) << 32) |
                     (((long)(aRowId[4] & 0xFF)) << 24) |
                     (((long)(aRowId[5] & 0xFF)) << 16) |
                     (((long)(aRowId[6] & 0xFF)) << 8) |
                     ((long)(aRowId[7] & 0xFF));
        
        sChars[0] = BASE64[((int)(sTemp >> 60)) & 0x3F];
        sChars[1] = BASE64[((int)(sTemp >> 54)) & 0x3F];
        sChars[2] = BASE64[((int)(sTemp >> 48)) & 0x3F];
        sChars[3] = BASE64[((int)(sTemp >> 42)) & 0x3F];
        sChars[4] = BASE64[((int)(sTemp >> 36)) & 0x3F];
        sChars[5] = BASE64[((int)(sTemp >> 30)) & 0x3F];
        sChars[6] = BASE64[((int)(sTemp >> 24)) & 0x3F];
        sChars[7] = BASE64[((int)(sTemp >> 18)) & 0x3F];
        sChars[8] = BASE64[((int)(sTemp >> 12)) & 0x3F];
        sChars[9] = BASE64[((int)(sTemp >> 6)) & 0x3F];
        sChars[10] = BASE64[((int)(sTemp)) & 0x3F];
        
        /* Tablespace ID */
        sTemp = (((long)(aRowId[8] & 0xFF)) << 8) |
                ((long)(aRowId[9] & 0xFF));
        
        sChars[11] = BASE64[((int)(sTemp >> 12)) & 0x3F];
        sChars[12] = BASE64[((int)(sTemp >> 6)) & 0x3F];
        sChars[13] = BASE64[((int)(sTemp)) & 0x3F];
        
        /* Page ID */
        sTemp = (((long)(aRowId[10] & 0xFF)) << 24) |
                (((long)(aRowId[11] & 0xFF)) << 16) |
                (((long)(aRowId[12] & 0xFF)) << 8) |
                ((long)(aRowId[13] & 0xFF));
        
        sChars[14] = BASE64[((int)(sTemp >> 30)) & 0x3F];
        sChars[15] = BASE64[((int)(sTemp >> 24)) & 0x3F];
        sChars[16] = BASE64[((int)(sTemp >> 18)) & 0x3F];
        sChars[17] = BASE64[((int)(sTemp >> 12)) & 0x3F];
        sChars[18] = BASE64[((int)(sTemp >> 6)) & 0x3F];
        sChars[19] = BASE64[((int)(sTemp)) & 0x3F];

        /* Offset */
        sTemp = (((long)(aRowId[14] & 0xFF)) << 8) |
                ((long)(aRowId[15] & 0xFF));

        sChars[20] = BASE64[((int)(sTemp >> 12)) & 0x3F];
        sChars[21] = BASE64[((int)(sTemp >> 6)) & 0x3F];
        sChars[22] = BASE64[((int)(sTemp)) & 0x3F];

        return new String(sChars);        
    }
    
    public RowIdBase(byte[] aValue)
    {
        mValue = new byte[aValue.length];
        System.arraycopy(aValue, 0, mValue, 0, aValue.length);
        mStringValue = null;
    }

    public byte[] getBytes()
    {
        return mValue;
    }
    
    public boolean equals(Object aObj)
    {
        if (aObj instanceof RowIdBase)
        {
            byte[] sTarget = ((RowIdBase)aObj).mValue;
            return Arrays.equals(mValue, sTarget);
        }
        return false;
    }
    
    public int hashCode()
    {
        int sResult = 0;
        
        for (int i=0; i<4; i++)
        {
            int sInt = ((mValue[i*4] & 0xFF) << 24) |
                       ((mValue[i*4+1] & 0xFF) << 16) |
                       ((mValue[i*4+2] & 0xFF) << 8) |
                       (mValue[i*4+3] &0xFF);
            sResult = sResult ^ sInt;
        }
        return sResult;
    }
    
    public String toString()
    {
        if (mStringValue == null)
        {
            mStringValue = toString(mValue);
        }
        return mStringValue;
    }
}

