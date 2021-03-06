// KEGIES.cpp 
//

#include "stdafx.h"
#include "KEGIES.h"
#include "MainFrm.h"

#include "KEGIESDoc.h"
#include "KEGIESView.h"
#include "View2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CKEGIESApp

BEGIN_MESSAGE_MAP(CKEGIESApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CKEGIESApp::OnAppAbout)

	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)

	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_TOOL_CONVERTSTLTOOBJ, &CKEGIESApp::OnToolConvertSTLtoObj)
	ON_COMMAND(ID_TOOL_TEST, &CKEGIESApp::OnToolTest)
	ON_COMMAND(ID_TOOL_START, &CKEGIESApp::OnToolStartSimulation)
	ON_COMMAND(ID_TOOL_CONVERTSTLTOCENTERLINE, &CKEGIESApp::OnToolConvertstltocenterline)
END_MESSAGE_MAP()


// CKEGIESApp 

CKEGIESApp::CKEGIESApp()
{
	// TODO: 
	// InitInstance
}




CKEGIESApp theApp;


int CKEGIESApp::ExitInstance()
{
	AppSetting::saveSetting();

	return CWinApp::ExitInstance();
}

BOOL CKEGIESApp::InitInstance()
{


	// InitCommonControlsEx()�� 
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("Registry for KEGIES"));
	LoadStdProfileSettings(4);  // MRU?
	//Init
	AppSetting::loadApplicationSetting();


	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CKEGIESDoc),
		RUNTIME_CLASS(CMainFrame),      
		RUNTIME_CLASS(CKEGIESView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// command line
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// �Shell command
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;


	// Show main window
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	//  DragAcceptFiles
	//DragAcceptFiles(m_pMainWnd, TRUE);
	//  ProcessShellCommand

	//Init every thing

	return TRUE;
}


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CKEGIESApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/*
 * Convert STL to obj
 */
void CKEGIESApp::OnToolConvertSTLtoObj()
{

}

//Main test
void CKEGIESApp::OnToolTest()
{
	//AfxGetApp()->m_pMainWnd->GetActiveWindow()->test();
}

//////////////////////////////////////////////////////////////////////////
//End test
//////////////////////////////////////////////////////////////////////////

void CKEGIESApp::OnToolStartSimulation()
{
	// TODO: Add your command handler code here
}


void CKEGIESApp::OnToolConvertstltocenterline()
{

}
