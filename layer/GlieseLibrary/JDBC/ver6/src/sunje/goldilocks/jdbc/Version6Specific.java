package sunje.goldilocks.jdbc;

import java.lang.management.ManagementFactory;
import java.math.BigDecimal;
import java.math.MathContext;
import java.sql.RowId;
import java.sql.SQLException;
import java.sql.SQLFeatureNotSupportedException;
import java.util.Formatter;

import sunje.goldilocks.jdbc.core4.RowIdBase;
import sunje.goldilocks.jdbc.core4.VersionSpecific;

public class Version6Specific extends VersionSpecific
{
    private static MathContext mMathContextForMultiply = new MathContext(2);
    
    public static void initialize()
    {
        if (VersionSpecific.SINGLETON == null)
        {
            VersionSpecific.SINGLETON = new Version6Specific();
        }
    }

    public Class<? extends SQLException> getNotSupportedExceptionClass()
    {
        return SQLFeatureNotSupportedException.class;
    }
    
    public RowIdBase createConcreteRowId(byte[] aData)
    {
        return new GoldilocksRowId(aData);
    }
    
    public String getRowIdClassName()
    {
        return RowId.class.getName();
    }
    
    public int getProcessId()
    {
        try
        {
            String sProcessName = ManagementFactory.getRuntimeMXBean().getName();
            return Integer.parseInt(sProcessName.substring(0, sProcessName.indexOf('@')));
        }
        catch (Exception sException)
        {
        }
        return 0;
    }
    
    public String byteToHexString(byte[] aData, boolean aToUpperCase)
    {
        Formatter sFormatter = new Formatter();
        String sFormat;
        String sResult;
        
        if (aToUpperCase)
        {
            sFormat = "%02X";
        }
        else
        {
            sFormat = "%02x";
        }
        for (byte b : aData)
        {
            sFormatter.format(sFormat, b);
        }
        
        sResult = sFormatter.toString();
        sFormatter.close();
        return sResult;
    }
    
    public int log10ToInt(double x)
    {
        return (int)Math.log10(x);
    }
    
    public double multiplyAccurately(double x, double y)
    {
        return (new BigDecimal(x)).multiply(new BigDecimal(y), mMathContextForMultiply).doubleValue();
    }
}
