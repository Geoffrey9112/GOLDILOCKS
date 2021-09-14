package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalYear extends CodeColumnInterval
{
    static CodeColumnIntervalYear PROTOTYPE = new CodeColumnIntervalYear();
    
    private CodeColumnIntervalYear()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_YEAR_TO_MONTH;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_YEAR;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalYear(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_YEAR;
    }
}
