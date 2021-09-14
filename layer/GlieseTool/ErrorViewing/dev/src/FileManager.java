import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.StringTokenizer;
import java.util.Vector;

public class FileManager
{
	// 파일 쓰기와 관련된 인스턴스
	static FileWriter fw;
	static BufferedWriter bw;

	// 파일 읽기와 관련된 인스턴스
	static FileInputStream fis;
	static BufferedReader dr;

	// file이 존재하는지 확인한다.
	public int FileEQ( String fn )
	{
		try
		{
			fis = new FileInputStream( fn );
		} catch ( FileNotFoundException e )
		{
			// TODO Auto-generated catch block
			return -1;
		}

		return 0;
	}

	// file을 쓰기모드로 연다.
	public void FileWriterOpen( String FileName )
	{
		try
		{
			fw = new FileWriter( FileName );
			bw = new BufferedWriter( fw );
		} catch ( IOException e )
		{
			System.out.println( "Error : \"" + FileName + "\" Can not find the file." );
		}
	}

	// file에 한 줄의 데이터를 쓴다.
	public void Writer( String txt )
	{
		try
		{
			bw.write( txt );
			bw.newLine();
		} catch ( IOException e )
		{
			System.out.println( "Error : Data writing error." );
		}
	}

	// file 쓰기모드를 닫는다.
	public void FileWriterClose()
	{
		try
		{
			bw.close();
		} catch ( IOException e )
		{
		}
	}

	// file을 읽기모드로 연다.
	public void FileReadOpen( String FileName )
	{
		try
		{
			fis = new FileInputStream( FileName );
			dr = new BufferedReader( new InputStreamReader( fis ) );
		} catch ( FileNotFoundException e )
		{
			System.out.println( "Error : \"" + FileName + "\" Can not find the file." );
			System.exit( 0 );
		}
	}

	// file에서 자료를 읽어서 Vector<String> 형으로 반환한다.
	public Vector<String> Read()
	{
		Vector<String> v = new Vector<String>();
		String temp;

		try
		{
			while ( ( temp = dr.readLine() ) != null )
			{
				v.add( temp );
			}
		} catch ( IOException e )
		{
			System.out.println( "Error : Data read errors." );
		}

		return v;
	}

	// file에서 자료를 읽어서 String 형으로 반환한다.
	public String ReadString()
	{
		String text = "";
		String temp = "";
		try
		{
			while ( ( temp = dr.readLine() ) != null )
			{
				text = text + temp;
			}
		} catch ( IOException e )
		{
			System.out.println( "Error : Data read errors." );
		}

		return text;
	}

	// file 읽기모드를 닫는다.
	public void FileReadClose()
	{
		try
		{
			dr.close();
		} catch ( IOException e )
		{
		}
	}

	// 인자로 받은 ReadFileName 파일의 내용을 WriterFileName로 복사한다.
	public void FileCopy( String ReadFileName, String WriterFileName )
	{
		this.FileReadOpen( ReadFileName );
		this.FileWriterOpen( WriterFileName );

		Vector<String> v = this.Read();
		int size = v.size();

		for ( int i = 0; i < size; i ++ )
		{
			this.Writer( v.get( i ) );
		}

		this.FileReadClose();
		this.FileWriterClose();
	}

	// 인자로 받은 Vector의 내용을 파일에 쓴다.
	public void VectorWriter( Vector<String> v )
	{
		int size = v.size();

		for ( int i = 0; i < size; i ++ )
		{
			this.Writer( v.get( i ) );
		}
	}

	public Vector<String> MsgToCsv()
	{
		Vector<String> vTemp = new Vector<String>();
		String         Line;

		FileReadOpen( System.getenv( "DB_HOME" ) + "/msg/gliese_error.msg" );

		try
		{
			while ( ( Line = dr.readLine() ) != null )
			{
                /* 공백 라인은 저장하지 않는다. */
                if( Line.length() != 0 )
                {
                    vTemp.add( Line );
                }
			}
		} catch ( IOException e )
		{
			System.out.println( "Error : Data read errors." );
		}

		FileReadClose();

		return vTemp;
	}
}
