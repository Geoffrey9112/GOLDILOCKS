package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalHourToSecond extends CodeColumnInterval
{
    static CodeColumnIntervalHourToSecond PROTOTYPE = new CodeColumnIntervalHourToSecond();
    
    private CodeColumnIntervalHourToSecond()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_HOUR_TO_SECOND;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalHourToSecond(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_HOUR_TO_SECOND;
    }
}
