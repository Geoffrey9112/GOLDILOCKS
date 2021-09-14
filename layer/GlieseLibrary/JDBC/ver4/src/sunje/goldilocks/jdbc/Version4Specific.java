package sunje.goldilocks.jdbc;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.core4.VersionSpecific;


public class Version4Specific extends VersionSpecific
{
    private static final char[][] HEX = {{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'},
                                         { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}};
    
    public static void initialize()
    {
        if (VersionSpecific.SINGLETON == null)
        {
            VersionSpecific.SINGLETON = new Version4Specific();
        }
    }

    public Class<? extends SQLException> getNotSupportedExceptionClass()
    {
        return SQLException.class;
    }
    
    public RowIdBase createConcreteRowId(byte[] aData)
    {
        return new GoldilocksRowId(aData);
    }
    
    public String getRowIdClassName()
    {
        return GoldilocksRowId.class.getName();
    }

    public int getProcessId()
    {
        return 0;
    }
    
    public String byteToHexString(byte[] aData, boolean aToUpperCase)
    {
        StringBuffer sBuf = new StringBuffer(aData.length * 2);
        int sHexIndex = aToUpperCase ? 1 : 0;
        
        for (byte b : aData)
        {
            int x = (b & 0xF0) >> 4;
            sBuf.append(HEX[sHexIndex][x]);
            x = b & 0x0F;
            sBuf.append(HEX[sHexIndex][x]);
        }
        
        return sBuf.toString();
    }
    
    public int log10ToInt(double x)
    {
        if (x == Double.NaN || x < 0)
        {
            return (int)Double.NaN;
        }
        else if (x == Double.POSITIVE_INFINITY)
        {
            return (int)Double.POSITIVE_INFINITY;
        }
        else if (x == 0)
        {
            return (int)Double.NEGATIVE_INFINITY;
        }
        else if (x == 1)
        {
            return 0; 
        }
        else if (x > 1)
        {
            int sResult = 0;
            while (x/10 >= 1)
            {
                sResult++;
                x = x/10;
            }
            return sResult;
        }
        else
        {
            int sResult = 0;
            while (x*10 <= 1)
            {
                sResult--;
                x = x*10;
            }
            return sResult;
        }
    }
    
    public double multiplyAccurately(double x, double y)
    {
        return x * y;
    }
}
