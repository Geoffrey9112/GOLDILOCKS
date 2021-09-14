package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalDayToHour extends CodeColumnInterval
{
    static CodeColumnIntervalDayToHour PROTOTYPE = new CodeColumnIntervalDayToHour();
    
    private CodeColumnIntervalDayToHour()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY_TO_HOUR;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalDayToHour(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_DAY_TO_HOUR;
    }
}
