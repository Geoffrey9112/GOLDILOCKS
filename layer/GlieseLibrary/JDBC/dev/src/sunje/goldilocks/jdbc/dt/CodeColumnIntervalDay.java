package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalDay extends CodeColumnInterval
{
    static CodeColumnIntervalDay PROTOTYPE = new CodeColumnIntervalDay();
    
    private CodeColumnIntervalDay()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_DAY;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalDay(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_DAY;
    }
}
