package sunje.goldilocks.jdbc.cm;

import java.sql.SQLException;

public interface LinkFailureListener
{
    void notifyLinkFailure(SQLException sException);
}
