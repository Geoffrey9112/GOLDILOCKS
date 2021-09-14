package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalHour extends CodeColumnInterval
{
    static CodeColumnIntervalHour PROTOTYPE = new CodeColumnIntervalHour();
    
    private CodeColumnIntervalHour()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_HOUR;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalHour(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_HOUR;
    }
}
