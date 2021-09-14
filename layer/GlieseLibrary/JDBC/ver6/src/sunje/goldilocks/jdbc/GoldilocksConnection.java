package sunje.goldilocks.jdbc;

import java.sql.SQLException;
import java.util.Properties;

import sunje.goldilocks.jdbc.core4.Jdbc4CommonDataSource;
import sunje.goldilocks.jdbc.core4.Jdbc4DatabaseMetaData;
import sunje.goldilocks.jdbc.core4.Jdbc4PreparedStatement;
import sunje.goldilocks.jdbc.core4.Jdbc4Savepoint;
import sunje.goldilocks.jdbc.core4.Jdbc4Statement;
import sunje.goldilocks.jdbc.core6.Jdbc6Connection;

public class GoldilocksConnection extends Jdbc6Connection
{
    public GoldilocksConnection(String aHost, int aPort, String aDBName, Properties aProp, Jdbc4CommonDataSource aFrom) throws SQLException
    {
        super(aHost, aPort, aDBName, aProp, aFrom);
    }
    
    protected Jdbc4Statement createConcreteStatement(int aResultSetType, int aResultSetConcurrency, int aResultSetHoldability)
    {
        return new GoldilocksStatement(this, aResultSetType, aResultSetConcurrency, aResultSetHoldability);
    }
    
    protected Jdbc4PreparedStatement createConcretePreparedStatement(String aSql, int aType, int aConcurrency, int aHoldability) throws SQLException
    {
        return new GoldilocksPreparedStatement(this, aSql, aType, aConcurrency, aHoldability);
    }
    
    protected Jdbc4DatabaseMetaData createConcreteDatabaseMetaData()
    {
        return new GoldilocksDatabaseMetaData(this); 
    }

    protected Jdbc4Savepoint createConcreteSavepoint(int aId)
    {
        return new GoldilocksSavepoint(aId);
    }
    
    protected Jdbc4Savepoint createConcreteSavepoint(String aName)
    {
        return new GoldilocksSavepoint(aName);
    }
}
