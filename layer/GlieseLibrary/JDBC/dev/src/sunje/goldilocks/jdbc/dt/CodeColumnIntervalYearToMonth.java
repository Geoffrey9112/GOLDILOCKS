package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalYearToMonth extends CodeColumnInterval
{
    static CodeColumnIntervalYearToMonth PROTOTYPE = new CodeColumnIntervalYearToMonth();
    
    private CodeColumnIntervalYearToMonth()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_YEAR_TO_MONTH;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalYearToMonth(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_YEAR_TO_MONTH;
    }
}
