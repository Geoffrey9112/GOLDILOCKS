import java.util.StringTokenizer;
import java.util.Vector;

//DataManager에서 사용할 데이터 형태와 메소드
class DataType
{
	Vector<String> Error_Code;
	Vector<String> SQLSTATE;
	Vector<String> Message;

	// 생성자 : 구성요소를 생성 및 초기화한다.
	public DataType()
	{
		Error_Code = new Vector<String>();
		SQLSTATE   = new Vector<String>();
		Message    = new Vector<String>();
	}

	public int Size()
	{
		return Error_Code.size();
	}

	public int GetErrorCodeIndex( String ec )
	{
		return Error_Code.indexOf( ec );
	}

	public int GetSQLSTATEIndex( String qs )
	{
		return SQLSTATE.indexOf( qs );
	}

	// 입력받은 자료를 분리하여 Error_Code와 SQLSTATE, Massing에 넣는다.
	public void Input( String Line )
	{
        int StringIndex  = 0;
        int BeginIndex   = 0;
        int EndIndex     = 0;
        int Length       = 0;

        Length = Line.length();

        /**
         * Step 1. Parse Error Code.
         */
        
        /* Find Begin Index */
        for( int i = 0; i < Length; i++ )
        {
            if( Line.charAt( i ) != ' ' )
            {
                StringIndex = i + 1;
                BeginIndex  = i;
                
                break;
            }
            else
            {
                /* Do Notting */
            }
        }

        /* Find End Index */
        for( int i = StringIndex; i < Length; i++ )
        {
            if( Line.charAt( i ) == ' ' )
            {
                StringIndex = i + 1;
                EndIndex    = i;

                break;
            }
            else
            {
                /* Do Notting */
            }
        }

        /* Copy Error Code */
        Error_Code.add( Line.substring( BeginIndex, EndIndex ) );

        /**
         * Step 2. Parse SQLSTATE
         */

        /* Find Begin Index */
        for( int i = StringIndex; i < Length; i++ )
        {
            if( Line.charAt( i ) != ' ' )
            {
                StringIndex = i + 1;
                BeginIndex  = i;

                break;
            }
            else
            {
                /* Do Notting */
            }
        }

        /* Find End Index */
        for( int i = StringIndex; i < Length; i++ )
        {
            if( Line.charAt( i ) == ' ' )
            {
                StringIndex = i + 1;
                EndIndex    = i;

                break;
            }
            else
            {
                /* Do Notting */
            }
        }
        
        /* Copy SQLSTATE */
        SQLSTATE.add( Line.substring( BeginIndex, EndIndex ));

        /**
         * Step 3. Parse Message
         */

        /* Find Begin Index */
        for( int i = StringIndex; i < Length; i++ )
        {
            if( Line.charAt( i ) != ' ' )
            {
                BeginIndex = i;

                break;
            }
            else
            {
                /* Do Notting */
            }
        }

        /* Copy Message */
        Message.add( Line.substring( BeginIndex, Length ) );
	}

	public String GetErrorCode( int index )
	{
		return Error_Code.get( index );
	}

	public String GetSQLSTATE( int index )
	{
		return SQLSTATE.get( index );
	}

	public String GetMessage( int index )
	{
		return Message.get( index );
	}
}

class BufferedVector
{
	static Vector<String> b1 = new Vector<String>();
	static Vector<String> b2 = new Vector<String>();
	static Vector<String> b3 = new Vector<String>();
	static Vector<String> b4 = new Vector<String>();
	static Vector<String> b5 = new Vector<String>();
	static Vector<String> b6 = new Vector<String>();
	static Vector<String> b7 = new Vector<String>();
	static Vector<String> b8 = new Vector<String>();
	static Vector<String> b9 = new Vector<String>();
	static Vector<String> b10 = new Vector<String>();
	static Vector<String> b11 = new Vector<String>();
	static int point = 1;
	static int startPoint = 0;
	static int endPoint = 0;

	public void Add( Vector<String> data )
	{
		if ( startPoint == 0 )
		{
			startPoint = 1;
			endPoint = 1;
			b1 = data;
		}
		else
		{
			if ( point == 1 )
			{
				if ( endPoint != 11 )
				{
					endPoint ++;
				}

				b11.clear();
				b11.addAll( b10 );
				b10.clear();
				b10.addAll( b9 );
				b9.clear();
				b9.addAll( b8 );
				b8.clear();
				b8.addAll( b7 );
				b7.clear();
				b7.addAll( b6 );
				b6.clear();
				b6.addAll( b5 );
				b5.clear();
				b5.addAll( b4 );
				b4.clear();
				b4.addAll( b3 );
				b3.clear();
				b3.addAll( b2 );
				b2.clear();
				b2.addAll( b1 );
				b1.clear();
				b1 = data;
			}
			else
			{
				if ( point == 2 )
				{
					b1.clear();
					b1.addAll( data );
					point = 1;
				}
				if ( point >= 3 )
				{
					for ( int i = 0; i < ( point - 2 ); i ++ )
					{
						b2.clear();
						b2.addAll( b3 );
						b3.clear();
						b3.addAll( b4 );
						b4.clear();
						b4.addAll( b5 );
						b5.clear();
						b5.addAll( b6 );
						b6.clear();
						b6.addAll( b7 );
						b7.clear();
						b7.addAll( b8 );
						b8.clear();
						b8.addAll( b9 );
						b9.clear();
						b9.addAll( b10 );
						b10.clear();
						b10.addAll( b11 );
						b11.clear();
					}
					endPoint --;
					point = 1;
				}
			}

		}
	}

	public Vector<String> undo()
	{
		if ( point == 1 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b2;
			}
			return b1;
		}
		if ( point == 2 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b3;
			}
			return b2;
		}
		if ( point == 3 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b4;
			}
			return b3;
		}
		if ( point == 4 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b5;
			}
			return b4;
		}
		if ( point == 5 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b6;
			}
			return b5;
		}
		if ( point == 6 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b7;
			}
			return b5;
		}
		if ( point == 7 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b8;
			}
			return b7;
		}
		if ( point == 8 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b9;
			}
			return b8;
		}
		if ( point == 9 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b10;
			}
			return b9;
		}
		if ( point == 10 )
		{
			if ( point != endPoint )
			{
				point ++;
				return b11;
			}
			return b10;
		}
		if ( point == 11 )
		{
			return b11;
		}
		return b1;
	}

	public Vector<String> Resume()
	{
		if ( point == 1 )
		{
			return b1;
		}
		if ( point == 2 )
		{
			point = 1;
			return b1;
		}
		if ( point == 3 )
		{
			point = 2;
			return b2;
		}
		if ( point == 4 )
		{
			point = 3;
			return b3;
		}
		if ( point == 5 )
		{
			point = 4;
			return b4;
		}
		if ( point == 6 )
		{
			point = 5;
			return b5;
		}
		if ( point == 7 )
		{
			point = 6;
			return b6;
		}
		if ( point == 8 )
		{
			point = 7;
			return b7;
		}
		if ( point == 9 )
		{
			point = 8;
			return b8;
		}
		if ( point == 10 )
		{
			point = 9;
			return b9;
		}
		if ( point == 11 )
		{
			point = 10;
			return b10;
		}
		return b1;
	}
}

// Data를 관리하는 클래스
class DataManager
{
	static DataType       gData = new DataType();
	static Vector<String> vTemp = new Vector<String>();
	static BufferedVector gBuff = new BufferedVector();

	public void Input( Vector<String> data )
	{
		int size;

		size = data.size();
        
        /* 1~2번 줄은 실제 데이터가 아니므로 처리하지 않는다. */
		for ( int i = 2; i < size; i ++ )
		{
			gData.Input( data.get( i ) );
		}
	}

	public Vector<String> GegAreaTextTotal()
	{
		Vector<String> vTemp = new Vector<String>();
		String temp;
		int size;

		size = gData.Size();
		temp = "ErrorCode       SQLSTATE       Message";
		vTemp.add( temp );
		temp = "---------------       ---------------      -----------------------------------------------------------------------------------";
		vTemp.add( temp );
		for ( int i = 0; i < size; i ++ )
		{
			temp = "      " + gData.GetErrorCode( i ) + "            " + gData.GetSQLSTATE( i ) + "       " + gData.GetMessage( i );
			vTemp.add( temp );
		}

		return vTemp;
	}

	public Vector<String> GetAreaTextErrorCode( String data )
	{
		Vector<String> vTemp = new Vector<String>();
		String temp;
		int size;

		temp = "ErrorCode       SQLSTATE       Message";
		vTemp.add( temp );
		temp = "---------------       ---------------      -----------------------------------------------------------------------------------";
		vTemp.add( temp );
		size = gData.Size();
		for ( int i = 0; i < size; i ++ )
		{
			if ( gData.GetSQLSTATE( i ).equalsIgnoreCase( data ) )
			{
				temp = "      " + gData.GetErrorCode( i ) + "            " + gData.GetSQLSTATE( i ) + "       " + gData.GetMessage( i );
				vTemp.add( temp );
			}
		}

		return vTemp;
	}

	public Vector<String> GetAreaTextSQLSTATE( String data )
	{
		Vector<String> vTemp = new Vector<String>();
		String temp;
		int size;

		temp = "ErrorCode       SQLSTATE       Message";
		vTemp.add( temp );
		temp = "---------------       ---------------      -----------------------------------------------------------------------------------";
		vTemp.add( temp );
		size = gData.Size();
		for ( int i = 0; i < size; i ++ )
		{
			if ( gData.GetErrorCode( i ).equalsIgnoreCase( data ) )
			{
				temp = "      " + gData.GetErrorCode( i ) + "            " + gData.GetSQLSTATE( i ) + "       " + gData.GetMessage( i );
				vTemp.add( temp );
			}
		}

		return vTemp;
	}
}