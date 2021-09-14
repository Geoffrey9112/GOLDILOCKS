package sunje.goldilocks.jdbc.core4;

import java.sql.SQLException;

public abstract class VersionSpecific
{
    public static VersionSpecific SINGLETON;
    
    public abstract Class<? extends SQLException> getNotSupportedExceptionClass();
    public abstract RowIdBase createConcreteRowId(byte[] aData);
    public abstract String getRowIdClassName();
    public abstract int getProcessId();
    public abstract String byteToHexString(byte[] aData, boolean aToUpperCase);
    public abstract int log10ToInt(double x);
    public abstract double multiplyAccurately(double x, double y);
}
