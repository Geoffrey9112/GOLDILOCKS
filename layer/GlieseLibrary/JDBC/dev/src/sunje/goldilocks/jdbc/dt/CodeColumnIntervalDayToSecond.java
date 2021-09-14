package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalDayToSecond extends CodeColumnInterval
{
    static CodeColumnIntervalDayToSecond PROTOTYPE = new CodeColumnIntervalDayToSecond();
    
    private CodeColumnIntervalDayToSecond()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY_TO_SECOND;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalDayToSecond(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_DAY_TO_SECOND;
    }
}
