package sunje.goldilocks.tool.diff;

import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Vector;

import sunje.goldilocks.jdbc.GoldilocksConnection;
import sunje.goldilocks.jdbc.GoldilocksDatabaseMetaData;
import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;
import sunje.goldilocks.jdbc.dt.CodeColumn;
import sunje.goldilocks.jdbc.ex.ErrorMgr;

public class FetchHandler extends ErrorHandler
{
    private boolean mHold = false;
    private Connection mCon;
    private CmChannel mChannel;
    private TableColumn mRowId;
    private TableColumn[] mKeyCols;
    private TableColumn[] mRowCols;
    private TableColumnList mKeyList;
    private TableColumnList mRowColList;
    private int mReceivedRowCount = 0;
    private int mCurrentRowIdx = 0;
    private String[] mKeyNames;
    private String[] mRowColNames;
    private SessionEnv mEnv;
    private long mStatementId = Protocol.STATEMENT_ID_UNDEFINED;
    
    private static boolean isIncludedIn(String aString, String[] aSet)
    {
        for (String sTarget : aSet)
        {
            if (sTarget.equals(aString))
            {
                return true;
            }
        }
        return false;
    }
    
    private static String toNameList(String[] aNames)
    {
        StringBuffer sBuf = new StringBuffer();
        sBuf.append(aNames[0]);
        for (int i=1; i<aNames.length; i++)
        {
            sBuf.append(',');
            sBuf.append(aNames[i]);
        }
        return sBuf.toString();
    }
    
    public FetchHandler(Connection aCon, String aSchema, String aTableName, String[] aExcludedCols, String aWhere) throws SQLException
    {
        mCon = aCon;
        mChannel = ((GoldilocksConnection)mCon).getChannel();
        mEnv = new SessionEnv(((GoldilocksConnection)mCon).getNewDecoder().charset(), mChannel.getByteOrder());
        
        for (int i=0; i<aExcludedCols.length; i++)
        {
            aExcludedCols[i] = aExcludedCols[i].toUpperCase();
        }
        
        Statement sStmt = mCon.createStatement();
        sStmt.execute("alter session set block_read_count=20");
        sStmt.close();
        
        DatabaseMetaData sMeta = mCon.getMetaData();
        ResultSet sRs = ((GoldilocksDatabaseMetaData)sMeta).getPrimaryKeysOrderByKeySeq(null, aSchema, aTableName);
        Vector<String> sColumns = new Vector<String>();
        while (sRs.next())
        {
            String sColName = sRs.getString(4);
            if (isIncludedIn(sColName, aExcludedCols))
            {
                throw new SQLException("key column cannot be excluded: " + sColName);
            }
            sColumns.add(sColName);
        }
        sRs.close();
        mKeyNames = sColumns.toArray(new String[0]);
        
        if (mKeyNames.length == 0)
        {
            throw new SQLException(aTableName + " table has no primary key");
        }
        
        sColumns.clear();
        sRs = sMeta.getColumns(null, aSchema, aTableName, null);
        while (sRs.next())
        {
            String sColName = sRs.getString(4);
            if (isIncludedIn(sColName, aExcludedCols))
            {
                continue;
            }
            if (isIncludedIn(sColName, mKeyNames))
            {
                continue;
            }
            sColumns.add(sColName);
        }
        sRs.close();
        mRowColNames = sColumns.toArray(new String[0]);

        mKeyCols = new TableColumn[mKeyNames.length];
        mRowCols = new TableColumn[mRowColNames.length];
        
        StringBuffer sSql = new StringBuffer("SELECT /*+ INDEX(");
        sSql.append(aTableName);
        sSql.append(") */ ROWID, ");
        String sKeyList = toNameList(mKeyNames);
        sSql.append(sKeyList);
        if (mRowColNames.length > 0)
        {
            sSql.append(',');
            sSql.append(toNameList(mRowColNames));
        }
        sSql.append(" FROM ");
        sSql.append(aTableName);
        
        if (aWhere != null && aWhere.length() > 0)
        {
            sSql.append(" WHERE ");
            sSql.append(aWhere);
        }
        /*
         * execute, fetch
         */
        writeProtocol(sSql.toString());
        mChannel.sendAndReceive();
        readProtocol();
        testError();
    }
    
    public void free() throws SQLException
    {
        if (mStatementId != Protocol.STATEMENT_ID_UNDEFINED)
        {
            mChannel.initToSend();
            mChannel.writeVarInt(Protocol.PROTOCOL_CODE_FREESTMT);
            mChannel.writeByte(Protocol.DEPENDENCY_ALONE);
            mChannel.writeStatementId(mStatementId);
            mChannel.writeVarInt(Protocol.FREESTMT_OPTION_DROP);
            mChannel.sendAndReceive();
         
            readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_FREESTMT, this);
            mStatementId = Protocol.STATEMENT_ID_UNDEFINED;
        }
    }
    
    private void writeProtocol(String aSql) throws SQLException
    {
        /* JDBC를 거치지 않고 직접 프로토콜을 사용할 때 매우 신중한 코딩이 필요하다.
         * write를 하고 read를 다하지 않은 상태에서 새로운 protocol을 write하면
         * 문제가 발생한다.
         * write-read의 pair는 반드시 지켜야 한다.
         * 하지만 이 메소드 호출후 readProtocol에서 fetch의 내용물은 read하지 않은 채
         * 재어권이 상위(TableDiff)로 넘어가는데 TableDiff에서는 next()를 통해 계속해서 fetch 내용을 받는다.
         * fetch 도중에는 error 결과가 넘어올 수 없기 때문에 write-read pair의 위반이 문제가 되지 않는다.
         * 하지만 에러 결과가 올 수 있는 상황에서는 반드시 write-read pair가 지켜져야 한다.
         */
        mChannel.initToSend();
        /*
         * execute direct
         */
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_EXECDIRECT);
        mChannel.writeByte(Protocol.DEPENDENCY_HEAD);
        mChannel.writeStatementId(mStatementId);
        mChannel.writeBoolean(true);
        byte[] sSql = aSql.getBytes();
        mChannel.writeVariable(sSql, 0, sSql.length);
        
        /*
         * target type  
         */
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_TARGETTYPE);
        mChannel.writeByte(Protocol.DEPENDENCY_BODY);
        mChannel.writeStatementId(Protocol.STATEMENT_ID_UNDEFINED);
        
        /*
         * fetch
         */
        mChannel.writeVarInt(Protocol.PROTOCOL_CODE_FETCH);
        mChannel.writeByte(Protocol.DEPENDENCY_TAIL);
        mChannel.writeStatementId(Protocol.STATEMENT_ID_UNDEFINED);
        mChannel.writeVarInt(1);
        mChannel.writeVarInt(Protocol.FETCH_COUNT_UNLIMITED);
    }
    
    private void readProtocol() throws SQLException
    {
        if (readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_EXECDIRECT, this))
        {
            mStatementId = mChannel.readStatementId();
            mChannel.readVarInt(); // statement type;
            mChannel.readVarInt(); // affected row count
            mChannel.readBoolean(); // recompiled
            mChannel.readBoolean(); // has resultset
            mChannel.readBoolean(); // holdability
            mChannel.readBoolean(); // concurrency
            mChannel.readByte(); // sensitivity
            mChannel.readBoolean(); // has transaction
            byte sDclAttrCount = mChannel.readByte(); // dcl attr count;
            if (sDclAttrCount > 0)
            {
                mChannel.readVarInt();
                byte sDclAttrValueType = mChannel.readByte();
                if (sDclAttrValueType == CodeColumn.GOLDILOCKS_DATA_TYPE_NATIVE_INTEGER)
                {
                    mChannel.readVarInt();
                }
                else
                {
                    mChannel.readString();
                }
            }
        }
        
        int sColCount = 0;
        if (readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_TARGETTYPE, this))
        {
            mChannel.readBoolean(); // changed?
            sColCount = (int)mChannel.readVarInt();
            if (sColCount != mKeyCols.length + mRowCols.length + 1)
            {
                throw new SQLException("column count(" + sColCount + " is not matched with " + (mKeyCols.length + mRowCols.length + 1));
            }
            for (int i=1; i<=sColCount; i++)
            {
                byte sType = mChannel.readByte();
                long sPrecision = mChannel.readVarInt();
                long sScale = mChannel.readVarInt();
                byte sLenUnit = mChannel.readByte();
                byte sIntervalInd = mChannel.readByte();
                
                if (i == 1)
                {
                    mRowId = new TableColumn(mEnv, i, mKeyNames[i-1], sType, sPrecision, sScale, sLenUnit, sIntervalInd);
                    mRowId.setToRowId();
                }
                else if (i <= mKeyCols.length + 1)
                {
                    mKeyCols[i-2] = new TableColumn(mEnv, i-1, mKeyNames[i-2], sType, sPrecision, sScale, sLenUnit, sIntervalInd); 
                }
                else
                {
                    mRowCols[i-mKeyCols.length-2] = new TableColumn(mEnv, i-1, mRowColNames[i-mKeyCols.length-2], sType, sPrecision, sScale, sLenUnit, sIntervalInd);
                }
            }
        }
        
        if (readProtocolCommon(mChannel, Protocol.PROTOCOL_CODE_FETCH, this))
        {
            int sFetchedColCount = (int)mChannel.readVarInt();
            if (sColCount != sFetchedColCount)
            {
                throw new SQLException("fetched column count(" + sFetchedColCount + ") is not matched with " + sColCount);
            }
           
            mChannel.readVarInt(); // first row idx
    
            mKeyList = new TableColumnList(mKeyCols);
            mRowColList = new TableColumnList(mRowCols);
        }
    }

    public static boolean readProtocolCommon(CmChannel aChannel, short aCode, ErrorHandler aHandler) throws SQLException
    {
        short sCode = (short)aChannel.readVarInt();
        if (sCode != aCode)
        {
            ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, sCode + "!=" + aCode);
        }
        return checkErrorResult(aChannel, aChannel.readByte(), aHandler);
    }
    
    private static boolean checkErrorResult(CmChannel aChannel, byte aResultCode, ErrorHandler aHandler) throws SQLException
    {
        boolean sResult; // 현재 result를 계속해서 읽어야 하는지의 여부를 알려준다.
        
        if (aResultCode == Protocol.RESULT_SUCCESS)
        {
            sResult = true;
        }
        else if (aResultCode == Protocol.RESULT_WARNING)
        {
            byte sErrorCount = aChannel.readByte();
            
            if (sErrorCount <= 0)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, "Error Result");
            }
            
            for (byte i=0; i<sErrorCount; i++)
            {
                aChannel.readInt();
                aChannel.readInt();
                aChannel.readString();
                aChannel.readString();
                /*
                 * warning은 무시한다.
                 */
            }
            sResult = true;
        }
        else if (aResultCode == Protocol.RESULT_IGNORE)
        {
            sResult = false;
        }
        else
        {
            if ((aResultCode != Protocol.RESULT_ABORT) && (aResultCode != Protocol.RESULT_FATAL))
            {
                ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, "Invalid result code");
            }
            
            byte sErrorCount = aChannel.readByte();
            
            if (sErrorCount <= 0)
            {
                ErrorMgr.raise(ErrorMgr.INVALID_PROTOCOL, "Error Result");
            }
            
            sResult = false;
            int sStateCode = aChannel.readInt();
            int sCode = aChannel.readInt();
            String sMessage = aChannel.readString();
            String sDetailedMessage = aChannel.readString();
            aHandler.addError(new SQLException(Protocol.makeErrorMessage(sMessage, sDetailedMessage), Protocol.decodeSqlState(sStateCode), sCode));
        }
        return sResult;
    }
    
    public boolean next() throws SQLException
    {
        if (mHold)
        {
            mHold = false;
            return true;
        }
        
        if (mReceivedRowCount < 0)
        {
            return false;
        }
        if (mReceivedRowCount == mCurrentRowIdx)
        {
            mReceivedRowCount = (int)mChannel.readVarInt();
            if (mReceivedRowCount == 0)
            {
                mChannel.readVarInt(); // last row idx
                mChannel.readBoolean(); // server cursor closed?
            }
            else if (mReceivedRowCount < 0)
            {
                /*
                 * sRowCount가 음수이면 fetch 도중에 에러가 발생한 경우이다.
                 */           
                checkErrorResult(mChannel, Protocol.RESULT_ABORT, this);
            }
            mCurrentRowIdx = 0;
        }
        
        if (mReceivedRowCount > 0)
        {
            byte sRowStatus = mChannel.readByte();
            if (sRowStatus != Protocol.FETCH_ROW_STATUS_DELETED)
            {
                mRowId.readData(mChannel, 0);
                mKeyList.readData(mChannel);
                mRowColList.readData(mChannel);
            }
            mCurrentRowIdx++;
            return true;
        }
        else
        {
            mReceivedRowCount = -1;
            return false;
        }
    }
    
    public void hold()
    {
        mHold = true;
    }
    
    public TableColumn getRowId()
    {
        return mRowId;
    }
    
    public TableColumnList getKey()
    {
        return mKeyList;
    }
    
    public TableColumnList getRow()
    {
        return mRowColList;
    }
}
