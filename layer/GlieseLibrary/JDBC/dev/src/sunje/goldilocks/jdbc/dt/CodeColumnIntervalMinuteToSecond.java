package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalMinuteToSecond extends CodeColumnInterval
{
    static CodeColumnIntervalMinuteToSecond PROTOTYPE = new CodeColumnIntervalMinuteToSecond();
    
    private CodeColumnIntervalMinuteToSecond()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_MINUTE_TO_SECOND;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalMinuteToSecond(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_MINUTE_TO_SECOND;
    }
}
