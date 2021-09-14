package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalMinute extends CodeColumnInterval
{
    static CodeColumnIntervalMinute PROTOTYPE = new CodeColumnIntervalMinute();
    
    private CodeColumnIntervalMinute()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_MINUTE;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalMinute(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_MINUTE;
    }
}
