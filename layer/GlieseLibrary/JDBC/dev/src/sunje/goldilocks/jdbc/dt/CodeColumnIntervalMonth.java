package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalMonth extends CodeColumnInterval
{
    static CodeColumnIntervalMonth PROTOTYPE = new CodeColumnIntervalMonth();
    
    private CodeColumnIntervalMonth()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_MONTH;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalMonth(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_MONTH;
    }
}
