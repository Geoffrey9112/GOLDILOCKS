package sunje.goldilocks.jdbc.dt;

public class CodeColumnIntervalSecond extends CodeColumnInterval
{
    static CodeColumnIntervalSecond PROTOTYPE = new CodeColumnIntervalSecond();
    
    private CodeColumnIntervalSecond()
    {        
    }
    
    public byte getDataType()
    {
        return GOLDILOCKS_DATA_TYPE_INTERVAL_DAY_TO_SECOND;
    }
    
    public String getTypeName()
    {
        return GOLDILOCKS_DATA_TYPE_NAME_INTERVAL_SECOND;
    }
    
    Column getInstance()
    {
        return new ColumnIntervalSecond(PROTOTYPE);
    }

    byte getDefaultIntervalIndicator()
    {
        return INTERVAL_INDICATOR_SECOND;
    }
}
