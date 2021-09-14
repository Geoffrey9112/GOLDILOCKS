package sunje.goldilocks.tool.diff;

import java.sql.SQLException;

import sunje.goldilocks.jdbc.cm.CmChannel;
import sunje.goldilocks.jdbc.cm.Protocol;

public class TableColumnList
{
    private byte[] mData;
    private int mLength;
    private TableColumn[] mColumns;

    public TableColumnList(TableColumn[] aColumns)
    {
        mColumns = aColumns;
        int sMaxLen = 0;
        for (TableColumn sCol : aColumns)
        {
            sMaxLen += sCol.mMaxLength;
        }
        mData = new byte[sMaxLen];
        for (TableColumn sCol : aColumns)
        {
            sCol.setDataBuf(mData);
        }
    }
    
    public void readData(CmChannel aChannel) throws SQLException
    {
        mLength = 0;
        for (TableColumn sCol : mColumns)
        {
            sCol.readData(aChannel, mLength);
            mLength += sCol.mLength;
        }
    }
    
    public int writeType(CmChannel aChannel, long aStatementId, int aStartIndex, byte aDependency) throws SQLException
    {
        for (TableColumn sCol : mColumns)
        {
            sCol.writeType(aChannel, aStatementId, aStartIndex, aDependency);
            aStartIndex++;
            aDependency = Protocol.DEPENDENCY_BODY;
        }
        return aStartIndex;
    }
    
    public void writeData(CmChannel aChannel) throws SQLException
    {
        for (TableColumn sCol : mColumns)
        {
            sCol.writeData(aChannel);
        }
    }
    
    public int compare(TableColumnList aTarget)
    {
        if (isSame(aTarget))
        {
            return 0;
        }
        
        int sResult = 0;
        for (int i=0; i<mColumns.length; i++)
        {
            sResult = mColumns[i].compare(aTarget.mColumns[i]);
            if (sResult != 0)
            {
                break;
            }
        }
        return sResult;
    }
    
    public boolean isSame(TableColumnList aTarget)
    {
        if (mLength != aTarget.mLength)
        {
            return false;
        }
        for (int i=0; i<mLength; i++)
        {
            if (mData[i] != aTarget.mData[i])
            {
                return false;
            }
        }
        return true;
    }
    
    public StringBuffer appendNameList(StringBuffer aBuf)
    {
        if (aBuf == null)
        {
            aBuf = new StringBuffer();
        }
        if (mColumns.length > 0)
        {
            aBuf.append(mColumns[0].mName);
            for (int i=1; i<mColumns.length; i++)
            {
                aBuf.append(',');
                aBuf.append(mColumns[i].mName);
            }
        }

        return aBuf;
    }
    
    public StringBuffer appendSetList(StringBuffer aBuf, String aDelimiter)
    {
        if (aBuf == null)
        {
            aBuf = new StringBuffer();
        }
        if (mColumns.length > 0)
        {
            aBuf.append(mColumns[0].mName);
            aBuf.append('=');
            aBuf.append('?');
            for (int i=1; i<mColumns.length; i++)
            {
                aBuf.append(aDelimiter);
                aBuf.append(mColumns[i].mName);
                aBuf.append('=');
                aBuf.append('?');
            }
        }

        return aBuf;
    }
    
    public int getColumnCount()
    {
        return mColumns.length;
    }
    
    public void copyDataFrom(TableColumnList aSource)
    {
        System.arraycopy(aSource.mData, 0, mData, 0, aSource.mLength);
        mLength = aSource.mLength;
        for (int i=0; i<mColumns.length; i++)
        {
            mColumns[i].copyDataFrom(aSource.mColumns[i]);
        }
    }
    
    public void readDataFrom(FileInByteBuffer aBuf) throws SQLException
    {
        mLength = 0;
        for (int i=0; i<mColumns.length; i++)
        {
            mLength += mColumns[i].readLengthFrom(aBuf, mLength);
        }
        aBuf.readBytes(mData, 0, mLength);
    }

    public void writeDataTo(FileOutByteBuffer aBuf) throws SQLException
    {
        for (int i=0; i<mColumns.length; i++)
        {
            mColumns[i].writeLengthTo(aBuf);
        }
        aBuf.writeBytes(mData, 0, mLength);
    }
    
    public void writeCodeTo(FileOutByteBuffer aBuf) throws SQLException
    {
        aBuf.writeInt(mColumns.length);
        for (int i=0; i<mColumns.length; i++)
        {
            mColumns[i].writeCodeTo(aBuf);
        }
    }
    
    public TableColumnList cloneCode()
    {
        TableColumn[] sColumns = new TableColumn[mColumns.length];
        for (int i=0; i<sColumns.length; i++)
        {
            sColumns[i] = mColumns[i].cloneCode();
        }
        return new TableColumnList(sColumns);
    }
    
    public void verifyCodeSame(TableColumnList aTarget) throws SQLException
    {
        if (mColumns.length != aTarget.mColumns.length)
        {
            throw new SQLException("Column count is not same: " + mColumns.length + ", " + aTarget.mColumns.length);
        }
        for (int i=0; i<mColumns.length; i++)
        {
            mColumns[i].verifyCodeSame(aTarget.mColumns[i]);
        }
    }
    
    public String getValueList()
    {
        StringBuffer sBuf = new StringBuffer(mColumns[0].getValue());
        for (int i=1; i<mColumns.length; i++)
        {
            sBuf.append('|');
            sBuf.append(mColumns[i].getValue());
        }
        return sBuf.toString();        
    }
}
