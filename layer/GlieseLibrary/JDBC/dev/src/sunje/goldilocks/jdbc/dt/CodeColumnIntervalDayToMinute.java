package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalDayToMinute extends CodeColumnInterval
{
    static CodeColumnIntervalDayToMinute PROTOTYPE = new CodeColumnIntervalDayToMinute();
    
    private CodeColumnIntervalDayToMinute()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY_TO_MINUTE;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalDayToMinute(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_DAY_TO_MINUTE;
    }
}
