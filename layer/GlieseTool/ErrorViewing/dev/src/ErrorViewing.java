import java.awt.Checkbox;
import java.awt.CheckboxGroup;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.ScrollPane;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.Vector;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.border.Border;

public class ErrorViewing
{
	// Main Frame 구성요소
	static JFrame gMainFrame = new JFrame( "Gliese - ErrorViewing" );
	static JTextField gSearchField = new JTextField();
	static JButton gSearchButton = new JButton( "Search" );
	static JTextArea gTextArea = new JTextArea();
	static ScrollPane gScrollPanel = new ScrollPane();
	static JButton gTotalListButton = new JButton( "Total List" );
	static JComboBox gChoiceBox = new JComboBox();
	static JButton gBackButton = new JButton( "Back" );
	static JButton gResumeButton = new JButton( "Resume" );
	static CheckboxGroup gGroup1 = new CheckboxGroup();
	static Checkbox gErrorCode = new Checkbox("ErrorCode", gGroup1, true);
	static Checkbox gSQLSTATE = new Checkbox("SQLSTATE", gGroup1, false);

	// 기타 구성요소
	static Toolkit gToolKit;
	static Dimension gScreenSize;
	static DataManager gDm = new DataManager();
	static FileManager gFm = new FileManager();
	static Vector<String> gOldList1 = new Vector<String>();
	static Vector<String> gOldList2 = new Vector<String>();
	static Vector<String> gOldList3 = new Vector<String>();
	static Vector<String> gOldList4 = new Vector<String>();
	static Vector<String> gOldList5 = new Vector<String>();
	static Vector<String> gOldList6 = new Vector<String>();
	static int gOldCount = 0;

	// Main Frame 설정
	static void MainFrameSet()
	{
		//Choice 설정
		gChoiceBox.addItem( "Error Code" );
		gChoiceBox.addItem( "SQL STATE" );

		// 컨테이너 및 컴포넌트 설정
		gMainFrame.setSize( 1150, 800 );
		gMainFrame.setResizable( false );
		gMainFrame.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
		gMainFrame.setLocation( 600, 200 );
		gMainFrame.setLayout( null );

		gSearchField.setBounds( 835, 15, 150, 25 );
		gSearchButton.setBounds( 1005, 15, 110, 25 );
		gScrollPanel.setBounds( 25, 60, 1090, 690 );
		gTotalListButton.setBounds( 295, 15, 110, 25 );
		gChoiceBox.setBounds( 705, 15, 110, 25 );
		gBackButton.setBounds( 25, 15, 110, 25 );
		gResumeButton.setBounds( 160, 15, 110, 25 );
		gErrorCode.setBounds( 630, 15, 100, 25 );
		gSQLSTATE.setBounds( 730, 15, 100, 25 );

		gTextArea.setEditable( false );
		Border sLineBorder =
				BorderFactory.createLineBorder( Color.DARK_GRAY, 1 );
		Border sEmptyBorder = BorderFactory.createEmptyBorder( 1, 1, 1, 1 );
		gTextArea.setBorder( BorderFactory.createCompoundBorder( sLineBorder, sEmptyBorder ) );
		gTextArea.setBackground( Color.LIGHT_GRAY );
		gChoiceBox.setBackground( Color.WHITE );

		gScrollPanel.add( gTextArea );

		//Main Frame에 컴포넌트 추가
		gMainFrame.add( gSearchField );
		gMainFrame.add( gSearchButton );
		gMainFrame.add( gScrollPanel );
		gMainFrame.add( gTotalListButton );
		gMainFrame.add( gResumeButton );
		gMainFrame.add( gBackButton );
		gMainFrame.add( gErrorCode );
		gMainFrame.add( gSQLSTATE );

		//화면에 Frame 보이기
		gMainFrame.setVisible( true );

		//Search버튼 이벤트
		gSearchButton.addActionListener( new ActionListener()
		{
			@Override
			public void actionPerformed( ActionEvent e )
			{
				Vector<String> vTemp = new Vector<String>();

				if ( gErrorCode.getState() )
				{
					vTemp = gDm.GetAreaTextSQLSTATE( gSearchField.getText() );
					TextPrint( vTemp );
					gDm.gBuff.Add( vTemp );
				}

				if ( gSQLSTATE.getState() )
				{
					vTemp = gDm.GetAreaTextErrorCode( gSearchField.getText() );
					TextPrint( vTemp );
					gDm.gBuff.Add( vTemp );
				}
			}
		} );

		//TotalList버튼 이벤트
		gTotalListButton.addActionListener( new ActionListener()
		{
			@Override
			public void actionPerformed( ActionEvent e )
			{
				Vector<String> vTemp = new Vector<String>();

				vTemp = gDm.GegAreaTextTotal();
				TextPrint( vTemp );
				gDm.gBuff.Add( vTemp );
			}
		} );

		//Back버튼 이벤트
		gBackButton.addActionListener( new ActionListener()
		{
			@Override
			public void actionPerformed( ActionEvent e )
			{
				TextPrint(gDm.gBuff.undo());
			}
		} );
		
		gResumeButton.addActionListener( new ActionListener()
		{
			@Override
			public void actionPerformed( ActionEvent e )
			{
				TextPrint(gDm.gBuff.Resume());
			}
		} );

		//엔터키 이벤트
		gSearchField.addKeyListener( new KeyListener()
		{
			//키가 눌렀을 때.
			@SuppressWarnings( "static-access" )
			@Override
			public void keyPressed( KeyEvent e )
			{
				// TODO Auto-generated method stub
				if ( e.getKeyCode() == e.VK_ENTER )
				{
					@SuppressWarnings( "unused" )
					String temp = ( String ) gChoiceBox.getSelectedItem();
					Vector<String> vTemp = new Vector<String>();

					if ( gErrorCode.getState() )
					{
						vTemp = gDm.GetAreaTextSQLSTATE( gSearchField.getText() );
						TextPrint( vTemp );
						gDm.gBuff.Add( vTemp );
					}

					if ( gSQLSTATE.getState() )
					{
						vTemp = gDm.GetAreaTextErrorCode( gSearchField.getText() );
						TextPrint( vTemp );
						gDm.gBuff.Add( vTemp );
					}
				}
			}

			//키를 놓았을 때.
			@Override
			public void keyReleased( KeyEvent e )
			{
				// TODO Auto-generated method stub
			}

			//키를 타이핑했을 때.
			@Override
			public void keyTyped( KeyEvent e )
			{
				// TODO Auto-generated method stub
			}
		} );

		//엔터키 이벤트
		gSearchButton.addKeyListener( new KeyListener()
		{
			//키가 눌렀을 때.
			@SuppressWarnings( "static-access" )
			@Override
			public void keyPressed( KeyEvent e )
			{
				// TODO Auto-generated method stub
				if ( e.getKeyCode() == e.VK_ENTER )
				{
					String temp = ( String ) gChoiceBox.getSelectedItem();
					Vector<String> vTemp = new Vector<String>();

					if ( temp.equalsIgnoreCase( "Error Code" ) )
					{
						vTemp =
								gDm.GetAreaTextSQLSTATE( gSearchField.getText() );
						TextPrint( vTemp );
						gDm.gBuff.Add( vTemp );
					}

					if ( temp.equalsIgnoreCase( "SQL STATE" ) )
					{
						vTemp =
								gDm.GetAreaTextErrorCode( gSearchField.getText() );
						TextPrint( vTemp );
						gDm.gBuff.Add( vTemp );
					}
				}
			}

			//키를 놓았을 때.
			@Override
			public void keyReleased( KeyEvent e )
			{
				// TODO Auto-generated method stub
			}

			//키를 타이핑했을 때.
			@Override
			public void keyTyped( KeyEvent e )
			{
				// TODO Auto-generated method stub
			}
		} );

		//엔터키 이벤트
		gTotalListButton.addKeyListener( new KeyListener()
		{
			//키가 눌렀을 때.
			@SuppressWarnings( "static-access" )
			@Override
			public void keyPressed( KeyEvent e )
			{
				// TODO Auto-generated method stub
				if ( e.getKeyCode() == e.VK_ENTER )
				{
					Vector<String> vTemp = new Vector<String>();

					vTemp = gDm.GegAreaTextTotal();
					TextPrint( vTemp );
					gDm.gBuff.Add( vTemp );
				}
			}

			//키를 놓았을 때.
			@Override
			public void keyReleased( KeyEvent e )
			{
				// TODO Auto-generated method stub
			}

			//키를 타이핑했을 때.
			@Override
			public void keyTyped( KeyEvent e )
			{
				// TODO Auto-generated method stub
			}
		} );
	}

	static void TextPrint( Vector<String> data )
	{
		int size = 0;

		size = data.size();
		gTextArea.setFont( new Font( "", Font.BOLD, 13 ) );
		gTextArea.setText( "" );
		for ( int i = 0; i < size; i ++ )
		{
			gTextArea.append( data.get( i ) + "\n" );
		}
	}

	public static void main( String[] args )
	{
		Vector<String> vTemp = new Vector<String>();

		vTemp = gFm.MsgToCsv();

		gDm.Input( vTemp );

		vTemp.clear();
		vTemp = gDm.GegAreaTextTotal();
		TextPrint( vTemp );
		gDm.gBuff.Add( vTemp );

		MainFrameSet();
	}
}
