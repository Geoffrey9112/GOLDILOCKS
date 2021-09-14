#ifdef WIN32

#include <cml.h>

#include <odbcinst.h>
#include <zluResource.h>

HINSTANCE gZluInstance; 

typedef struct zluConfigDSNAttr
{
    stlChar * mDriver;
    stlChar   mDSN[256];
    stlChar   mCSMode[16];
    stlChar   mHost[16];
    stlChar   mPort[8];
    stlChar   mUID[32];
    stlChar   mDATE[64];
    stlChar   mTIME[64];
    stlChar   mTIMETZ[64];
    stlChar   mTIMESTAMP[64];
    stlChar   mTIMESTAMPTZ[64];
    stlChar   mCharLengthUnit[16];
} zluConfigDSNAttr;

stlChar * zluGetNextAttribute( stlChar * aElementList )
{
    stlChar * sPos = NULL;

    if( aElementList != NULL )
    {
        sPos = aElementList;

        sPos += stlStrlen(aElementList) + 1;

        if( *sPos == '\0' )
        {
            sPos = NULL;
    	}
    }

    return sPos;
}

stlChar * zluGetAttrValue( stlChar * aElement )
{
    stlChar * sPos = NULL;

    if( aElement != NULL )
    {
        sPos = stlStrchr( aElement, '=' );
		
        if( sPos != NULL )
        {
            sPos++;
        }
    }

    return sPos;
}

stlChar * zluGetAttrValueByKey( stlChar * aElementList,
                                stlChar * aKey )
{
    stlChar * sPos = NULL;
    stlChar * sVal = NULL;

    if( (aElementList != NULL) && (aKey != NULL) )
    {
        sPos = aElementList;
		
        do
        {
            if( stlStrncasecmp( sPos, aKey, stlStrlen(aKey) ) == 0 )
            {
                sVal = zluGetAttrValue( sPos );
                break;
            }
			
            sPos = zluGetNextAttribute( sPos );
        } while (sPos != NULL);
    }
	
    return sVal;
}

INT_PTR CALLBACK ConfigDSNProc( HWND   aDialogHandle,
                                UINT   aMsg,
                                WPARAM aWParam,
                                LPARAM aLParam )
{
    zluConfigDSNAttr * sDSNAttr = NULL;
    stlBool            sFocus   = STL_FALSE;

    switch( aMsg )
    {
	case WM_INITDIALOG :
            sDSNAttr = (zluConfigDSNAttr*)aLParam;

            if( sDSNAttr->mDSN[0] == '\0' )
            {
                SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_DSN) );
                sFocus = STL_TRUE;
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_DSN, sDSNAttr->mDSN );
                EnableWindow( GetDlgItem(aDialogHandle, IDC_EDIT_DSN), FALSE );
            }

            if( sDSNAttr->mHost[0] == '\0' )
            {
                if( sFocus == STL_FALSE )
                {
                    SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_HOST) );
                    sFocus = STL_FALSE;
                }
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_HOST, sDSNAttr->mHost );
            }

            if( sDSNAttr->mPort[0] == '\0' )
            {
                if( sFocus == STL_FALSE )
                {
                    SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_PORT) );
                    sFocus = STL_TRUE;
                }
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_PORT, sDSNAttr->mPort );
            }

            if( sDSNAttr->mUID[0] == '\0' )
            {
                if( sFocus == STL_FALSE )
                {
                    SetFocus( GetDlgItem(aDialogHandle, IDC_EDIT_UID) );
                    sFocus = STL_TRUE;
                }
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_UID, sDSNAttr->mUID );
            }

            if( sDSNAttr->mCSMode[0] == '\0' )
            {
                SendDlgItemMessage( aDialogHandle, IDC_RADIO_CS_MODE_DEFAULT, BM_SETCHECK, BST_CHECKED, 0 );
            }
            else
            {
                if( stlStrcasecmp(sDSNAttr->mCSMode, "DEDICATED") == 0 )
                {
                    SendDlgItemMessage( aDialogHandle, IDC_RADIO_CS_MODE_DEDICATED, BM_SETCHECK, BST_CHECKED, 0 );
                }
                else if( stlStrcasecmp(sDSNAttr->mCSMode, "SHARED") == 0 )
                {
                    SendDlgItemMessage( aDialogHandle, IDC_RADIO_CS_MODE_SHARED, BM_SETCHECK, BST_CHECKED, 0 );
                }
                else
                {
                    SendDlgItemMessage( aDialogHandle, IDC_RADIO_CS_MODE_DEFAULT, BM_SETCHECK, BST_CHECKED, 0 );
                }
            }

            if( sDSNAttr->mDATE[0] == '\0' )
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_DATE_FORMAT, "SYYYY-MM-DD HH24:MI:SS" ); 
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_DATE_FORMAT, sDSNAttr->mDATE );
            }

            if( sDSNAttr->mTIME[0] == '\0' )
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIME_FORMAT, "HH24:MI:SS.FF6" ); 
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIME_FORMAT, sDSNAttr->mTIME );
            }

            if( sDSNAttr->mTIMETZ[0] == '\0' )
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIME_WITH_TIMEZONE_FORMAT, "HH24:MI:SS.FF6 TZH:TZM" ); 
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIME_WITH_TIMEZONE_FORMAT, sDSNAttr->mTIMETZ );
            }

            if( sDSNAttr->mTIMESTAMP[0] == '\0' )
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIMESTAMP_FORMAT, "SYYYY-MM-DD HH24:MI:SS.FF6" ); 
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIMESTAMP_FORMAT, sDSNAttr->mTIMESTAMP );
            }

            if( sDSNAttr->mTIMESTAMPTZ[0] == '\0' )
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIMESTAMP_WITH_TIMEZONE_FORMAT, "SYYYY-MM-DD HH24:MI:SS.FF6 TZH:TZM"); 
            }
            else
            {
                SetDlgItemText( aDialogHandle, IDC_EDIT_TIMESTAMP_WITH_TIMEZONE_FORMAT, sDSNAttr->mTIMESTAMPTZ);
            }

            if( sDSNAttr->mCharLengthUnit[0] == '\0' )
            {
                SendDlgItemMessage( aDialogHandle,
                                    IDC_RADIO_CHAR_LENGTH_UNIT_CHARACTERS,
                                    BM_SETCHECK,
                                    BST_CHECKED,
                                    0 );
            }
            else
            {
                if( (stlStrcasecmp(sDSNAttr->mCharLengthUnit, "BYTE")   == 0) ||
                    (stlStrcasecmp(sDSNAttr->mCharLengthUnit, "OCTETS") == 0) )
                {
                    SendDlgItemMessage( aDialogHandle,
                                        IDC_RADIO_CHAR_LENGTH_UNIT_OCTETS,
                                        BM_SETCHECK,
                                        BST_CHECKED,
                                        0);
                }
                else
                {
                    SendDlgItemMessage( aDialogHandle,
                                        IDC_RADIO_CHAR_LENGTH_UNIT_CHARACTERS,
                                        BM_SETCHECK,
                                        BST_CHECKED,
                                        0);
                }
            }

            SetWindowLongPtr( aDialogHandle, DWLP_USER, aLParam );
            break;
	case WM_COMMAND:
            switch( LOWORD(aWParam) )
            {
		case IDOK:
                    sDSNAttr = (zluConfigDSNAttr*)GetWindowLongPtr( aDialogHandle, DWLP_USER );

                    GetDlgItemText( aDialogHandle, IDC_EDIT_DSN,  sDSNAttr->mDSN,  STL_SIZEOF(sDSNAttr->mDSN)  );
                    GetDlgItemText( aDialogHandle, IDC_EDIT_HOST, sDSNAttr->mHost, STL_SIZEOF(sDSNAttr->mHost) );
                    GetDlgItemText( aDialogHandle, IDC_EDIT_PORT, sDSNAttr->mPort, STL_SIZEOF(sDSNAttr->mPort) );
                    GetDlgItemText( aDialogHandle, IDC_EDIT_UID,  sDSNAttr->mUID,  STL_SIZEOF(sDSNAttr->mUID)  );

                    if( SendDlgItemMessage( aDialogHandle,
                                            IDC_RADIO_CS_MODE_DEDICATED,
                                            BM_GETCHECK,
                                            0,
                                            0) == BST_CHECKED )
                    {
                        stlSnprintf( sDSNAttr->mCSMode, STL_SIZEOF(sDSNAttr->mCSMode),  "DEDICATED" );
                    }
                    else if(SendDlgItemMessage( aDialogHandle,
                                                IDC_RADIO_CS_MODE_SHARED,
                                                BM_GETCHECK,
                                                0,
                                                0) == BST_CHECKED )
                    {
                        stlSnprintf( sDSNAttr->mCSMode, STL_SIZEOF(sDSNAttr->mCSMode),  "SHARED" );
                    }
                    else
                    {
                        stlSnprintf( sDSNAttr->mCSMode, STL_SIZEOF(sDSNAttr->mCSMode),  "DEFAULT" );
                    }

                    GetDlgItemText( aDialogHandle,
                                    IDC_EDIT_DATE_FORMAT,
                                    sDSNAttr->mDATE,
                                    STL_SIZEOF(sDSNAttr->mDATE) );
                    
                    GetDlgItemText( aDialogHandle,
                                    IDC_EDIT_TIME_FORMAT,
                                    sDSNAttr->mTIME,
                                    STL_SIZEOF(sDSNAttr->mTIME) );
                    
                    GetDlgItemText( aDialogHandle,
                                    IDC_EDIT_TIME_WITH_TIMEZONE_FORMAT,
                                    sDSNAttr->mTIMETZ,
                                    STL_SIZEOF(sDSNAttr->mTIMETZ) );
                    
                    GetDlgItemText( aDialogHandle,
                                    IDC_EDIT_TIMESTAMP_FORMAT,
                                    sDSNAttr->mTIMESTAMP,
                                    STL_SIZEOF(sDSNAttr->mTIMESTAMP) );
                    
                    GetDlgItemText( aDialogHandle,
                                    IDC_EDIT_TIMESTAMP_WITH_TIMEZONE_FORMAT,
                                    sDSNAttr->mTIMESTAMPTZ,
                                    STL_SIZEOF(sDSNAttr->mTIMESTAMPTZ) );

                    if(SendDlgItemMessage( aDialogHandle,
                                           IDC_RADIO_CHAR_LENGTH_UNIT_OCTETS,
                                           BM_GETCHECK,
                                           0,
                                           0) == BST_CHECKED )
                    {
                        stlSnprintf( sDSNAttr->mCharLengthUnit, STL_SIZEOF(sDSNAttr->mCharLengthUnit), "OCTETS" );
                    }
                    else
                    {
                        stlSnprintf( sDSNAttr->mCharLengthUnit, STL_SIZEOF(sDSNAttr->mCharLengthUnit), "CHARACTERS" );
                    }
		case IDCANCEL:
                    EndDialog( aDialogHandle, LOWORD(aWParam) == IDOK );
                    return TRUE;
            }
    }

    return FALSE;
}

BOOL WriteDSNAttributes( zluConfigDSNAttr * aDSNAttr )
{
    BOOL sRet = FALSE;

    sRet = SQLWriteDSNToIni(aDSNAttr->mDSN, aDSNAttr->mDriver);

    STL_TRY( sRet == TRUE );
    
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "HOST", aDSNAttr->mHost, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "PORT", aDSNAttr->mPort, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "UID", aDSNAttr->mUID, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "CS_MODE", aDSNAttr->mCSMode, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "DATE_FORMAT", aDSNAttr->mDATE, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "TIME_FORMAT", aDSNAttr->mTIME, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "TIME_WITH_TIME_ZONE_FORMAT", aDSNAttr->mTIMETZ, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "TIMESTAMP_FORMAT", aDSNAttr->mTIMESTAMP, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "TIMESTAMP_WITH_TIME_ZONE_FORMAT", aDSNAttr->mTIMESTAMPTZ, "ODBC.INI" );
    SQLWritePrivateProfileString( aDSNAttr->mDSN, "CHAR_LENGTH_UNITS", aDSNAttr->mCharLengthUnit, "ODBC.INI" );

    return TRUE;

    STL_FINISH;

    return FALSE;
}

BOOL INSTAPI ConfigDSN( HWND    aWindowHandle,
                        WORD    aRequest,
                        LPCSTR  aDriver,
                        LPCSTR  aAttributes)
{
    zluConfigDSNAttr   sDSNAttr;
    INT_PTR            sRet;
    stlChar          * sValue = NULL;

    memset(&sDSNAttr, 0x00, STL_SIZEOF(zluConfigDSNAttr));

    sDSNAttr.mDriver = (stlChar*)aDriver;

    switch( aRequest )
    {
	case ODBC_ADD_DSN :
            sRet = DialogBoxParam( gZluInstance,
                                   MAKEINTRESOURCE(IDD_DIALOG_DRIVER_CONFIGURATION),
                                   aWindowHandle,
                                   ConfigDSNProc,
                                   (LPARAM)&sDSNAttr);

            STL_TRY( sRet >= 0 ); 
			
            if( sRet == 1 )
            {
                STL_TRY( WriteDSNAttributes( &sDSNAttr ) == TRUE );
            }
            break;
	case ODBC_CONFIG_DSN :
            sValue = zluGetAttrValueByKey( (stlChar*)aAttributes, "DSN" );

            STL_TRY_THROW( sValue != NULL, RAMP_ERR_INVALID_KEYWORD_VALUE );

            stlStrncpy( sDSNAttr.mDSN, sValue, STL_SIZEOF(sDSNAttr.mDSN) );

            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "HOST",
                                        "",
                                        sDSNAttr.mHost,
                                        STL_SIZEOF(sDSNAttr.mHost),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "PORT",
                                        "",
                                        sDSNAttr.mPort,
                                        STL_SIZEOF(sDSNAttr.mPort),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "UID",
                                        "",
                                        sDSNAttr.mUID,
                                        STL_SIZEOF(sDSNAttr.mUID),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "CS_MODE",
                                        "",
                                        sDSNAttr.mCSMode,
                                        STL_SIZEOF(sDSNAttr.mCSMode),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "DATE_FORMAT",
                                        "",
                                        sDSNAttr.mDATE,
                                        STL_SIZEOF(sDSNAttr.mDATE),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "TIME_FORMAT",
                                        "",
                                        sDSNAttr.mTIME,
                                        STL_SIZEOF(sDSNAttr.mTIME),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "TIME_WITH_TIME_ZONE_FORMAT",
                                        "",
                                        sDSNAttr.mTIMETZ,
                                        STL_SIZEOF(sDSNAttr.mTIMETZ),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "TIMESTAMP_FORMAT",
                                        "",
                                        sDSNAttr.mTIMESTAMP,
                                        STL_SIZEOF(sDSNAttr.mTIMESTAMP),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "TIMESTAMP_WITH_TIME_ZONE_FORMAT",
                                        "",
                                        sDSNAttr.mTIMESTAMPTZ,
                                        STL_SIZEOF(sDSNAttr.mTIMESTAMPTZ),
                                        "ODBC.INI" );
            
            SQLGetPrivateProfileString( sDSNAttr.mDSN,
                                        "CHAR_LENGTH_UNITS",
                                        "",
                                        sDSNAttr.mCharLengthUnit,
                                        STL_SIZEOF(sDSNAttr.mCharLengthUnit),
                                        "ODBC.INI" );

            sRet = DialogBoxParam( gZluInstance,
                                   MAKEINTRESOURCE(IDD_DIALOG_DRIVER_CONFIGURATION),
                                   aWindowHandle,
                                   ConfigDSNProc,
                                   (LPARAM)&sDSNAttr );

            STL_TRY( sRet >= 0 );

            if( sRet == 1 )
            {
                STL_TRY( WriteDSNAttributes( &sDSNAttr ) == TRUE );
            }
            break;
	case ODBC_REMOVE_DSN :
            sValue = zluGetAttrValueByKey( (stlChar*)aAttributes, "DSN" );

            STL_TRY_THROW( sValue != NULL, RAMP_ERR_INVALID_KEYWORD_VALUE);

            SQLRemoveDSNFromIni( sValue );
            break;
	default :
            STL_THROW( RAMP_ERR_INVALID_REQUEST_TYPE );
            break;
    }

    return TRUE;

    STL_CATCH( RAMP_ERR_INVALID_KEYWORD_VALUE )
    {
        SQLPostInstallerError( ODBC_ERROR_INVALID_KEYWORD_VALUE,
                               "The lpszAttributes argument contained a syntax error." );
    }

    STL_CATCH( RAMP_ERR_INVALID_REQUEST_TYPE )
    {
        SQLPostInstallerError( ODBC_ERROR_INVALID_REQUEST_TYPE,
                               "The fRequest argument was not one of the following: \n"
                               "  ODBC_ADD_DSN\n"
                               "  ODBC_CONFIG_DSN\n"
                               "  ODBC_REMOVE_DSN" );
    }
    
    STL_FINISH;

    return FALSE;
}

BOOL WINAPI DllMain( HANDLE aModuleHandle,
                     DWORD  aReason,
                     LPVOID aReserved )
{
    gZluInstance = (HINSTANCE)aModuleHandle;

    return TRUE;
}

#endif
