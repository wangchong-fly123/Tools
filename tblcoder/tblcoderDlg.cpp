
// tblcoderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tblcoder.h"
#include "tblcoderDlg.h"
#include "Compiler.h"
#include "PropCompiler.h"
#include "markupstl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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

// CtblcoderDlg dialog

CtblcoderDlg::CtblcoderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtblcoderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtblcoderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtblcoderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_XML_PATH, &CtblcoderDlg::OnBnClickedBtnXmlPath)
	ON_BN_CLICKED(IDC_BTN_OUT_CPP, &CtblcoderDlg::OnBnClickedBtnOutCpp)
	ON_BN_CLICKED(IDC_BTN_OUT_CPP2, &CtblcoderDlg::OnBnClickedBtnOutCpp2)
	ON_BN_CLICKED(IDC_BTN_OUT_CS, &CtblcoderDlg::OnBnClickedBtnOutCs)
	ON_BN_CLICKED(IDC_BTN_OUT_PY, &CtblcoderDlg::OnBnClickedBtnOutPy)
	ON_BN_CLICKED(IDC_BTN_COMPILE_CPP, &CtblcoderDlg::OnBnClickedBtnCompileCpp)
	ON_BN_CLICKED(IDC_BTN_COMPILE_CPP2, &CtblcoderDlg::OnBnClickedBtnCompileCpp2)
	ON_BN_CLICKED(IDC_BTN_COMPILE_CS, &CtblcoderDlg::OnBnClickedBtnCompilerCs)
	ON_BN_CLICKED(IDC_BTN_COMPILE_PY, &CtblcoderDlg::OnBnClickedBtnCompilePy)
    ON_BN_CLICKED(IDOK, &CtblcoderDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CtblcoderDlg message handlers

BOOL CtblcoderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	GetDlgItem(IDC_EDIT_XML_PATH)->SetWindowText(
		"..\\..\\..\\..\\..\\..\\TSSG_CONT\\trunk\\GameData\\tbdata\\table.xml");
	GetDlgItem(IDC_EDIT_OUT_CPP2)->SetWindowText(
		"..\\..\\src\\table\\");
	GetDlgItem(IDC_EDIT_OUT_CS)->SetWindowText(
		"..\\..\\..\\client\\Framework\\tbdata\\");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtblcoderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtblcoderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtblcoderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtblcoderDlg::OnBnClickedBtnXmlPath()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlgFile(TRUE, "*.xml", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
        "XML Files (*.xml)|*.xml||", this);

    if(IDOK == dlgFile.DoModal())
    {
        GetDlgItem(IDC_EDIT_XML_PATH)->SetWindowText(dlgFile.GetPathName());
    }
}

void CtblcoderDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here

    CDialog::OnCancel();
}

void CtblcoderDlg::OnBnClickedBtnOutCpp()
{
	/*
    // TODO: Add your control notification handler code here
    BROWSEINFO bInfo;
    memset(&bInfo, '\0', sizeof(BROWSEINFO));
    bInfo.hwndOwner = GetSafeHwnd();
    bInfo.lpszTitle = "选择输出的目录";
    bInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    LPITEMIDLIST pidl;
    pidl = ::SHBrowseForFolder(&bInfo);
    if(pidl == NULL)
        return;
    char szPath[MAX_PATH];
    szPath[0] = '\0';
    if(SHGetPathFromIDList(pidl, szPath) == FALSE)
        return;
    //如果路径以'\'结尾，则将其去掉
    int n = strlen(szPath);
    if('\\' == szPath[n-1])
        szPath[n-1] = '\0';

    GetDlgItem(IDC_EDIT_OUT_CPP)->SetWindowText(szPath);
	*/
}

void CtblcoderDlg::OnBnClickedBtnOutCpp2()
{
    BROWSEINFO bInfo;
    memset(&bInfo, '\0', sizeof(BROWSEINFO));
    bInfo.hwndOwner = GetSafeHwnd();
    bInfo.lpszTitle = "选择输出的目录";
    bInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    LPITEMIDLIST pidl;
    pidl = ::SHBrowseForFolder(&bInfo);
    if(pidl == NULL)
        return;
    char szPath[MAX_PATH];
    szPath[0] = '\0';
    if(SHGetPathFromIDList(pidl, szPath) == FALSE)
        return;
    //如果路径以'\'结尾，则将其去掉
    int n = strlen(szPath);
    if('\\' == szPath[n-1])
        szPath[n-1] = '\0';

    GetDlgItem(IDC_EDIT_OUT_CPP2)->SetWindowText(szPath);
}

void CtblcoderDlg::OnBnClickedBtnOutCs()
{
    // TODO: Add your control notification handler code here
    BROWSEINFO bInfo;
    memset(&bInfo, '\0', sizeof(BROWSEINFO));
    bInfo.hwndOwner = GetSafeHwnd();
    bInfo.lpszTitle = "选择输出的目录";
    bInfo.ulFlags = BIF_RETURNONLYFSDIRS;

    LPITEMIDLIST pidl;
    pidl = ::SHBrowseForFolder(&bInfo);
    if(pidl == NULL)
        return;
    char szPath[MAX_PATH];
    szPath[0] = '\0';
    if(SHGetPathFromIDList(pidl, szPath) == FALSE)
        return;
    //如果路径以'\'结尾，则将其去掉
    int n = strlen(szPath);
    if('\\' == szPath[n-1])
        szPath[n-1] = '\0';

    GetDlgItem(IDC_EDIT_OUT_CS)->SetWindowText(szPath);
}

void CtblcoderDlg::OnBnClickedBtnOutPy()
{
	BROWSEINFO bInfo;
	memset(&bInfo, '\0', sizeof(BROWSEINFO));
	bInfo.hwndOwner = GetSafeHwnd();
	bInfo.lpszTitle = "选择输出的目录";
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidl;
	pidl = ::SHBrowseForFolder(&bInfo);
	if(pidl == NULL)
		return;
	char szPath[MAX_PATH];
	szPath[0] = '\0';
	if(SHGetPathFromIDList(pidl, szPath) == FALSE)
		return;
	//如果路径以'\'结尾，则将其去掉
	int n = strlen(szPath);
	if('\\' == szPath[n-1])
		szPath[n-1] = '\0';

	GetDlgItem(IDC_EDIT_OUT_PY)->SetWindowText(szPath);
}

void CtblcoderDlg::OnBnClickedBtnCompileCpp()
{
	/*
    CCompiler oCompiler;

    //strFile.Format("%s\\table.xml", GetRootPath());
    CString strVal;
    GetDlgItem(IDC_EDIT_XML_PATH)->GetWindowText(strVal);
    m_strLoadFile = strVal;

    GetDlgItem(IDC_EDIT_OUT_CPP)->GetWindowText(strVal);

    if(strVal.Right(1) == '/' || strVal.Right(1) == '\\')
    {
        strVal.SetAt(strVal.GetLength()-1, '\0');
    }

    m_strOutputPath = strVal;
    oCompiler.SetOutputPath(m_strOutputPath.c_str());
    
    oCompiler.CompileCpp(m_strLoadFile.c_str());

    GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(oCompiler.m_strErrMsg);
	*/
}

void CtblcoderDlg::OnBnClickedBtnCompileCpp2()
{
	CCompiler oCompiler;

    //strFile.Format("%s\\table.xml", GetRootPath());
    CString strVal;
    GetDlgItem(IDC_EDIT_XML_PATH)->GetWindowText(strVal);
    m_strLoadFile = strVal;

    GetDlgItem(IDC_EDIT_OUT_CPP2)->GetWindowText(strVal);

    if(strVal.Right(1) == '/' || strVal.Right(1) == '\\')
    {
        strVal.SetAt(strVal.GetLength()-1, '\0');
    }

    m_strOutputPath = strVal;
    oCompiler.SetOutputPath(m_strOutputPath.c_str());
    
    oCompiler.CompileCpp2(m_strLoadFile.c_str());

    GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(oCompiler.m_strErrMsg);
}

void CtblcoderDlg::OnBnClickedBtnCompilerCs()
{
    CCompiler oCompiler;

    //strFile.Format("%s\\table.xml", GetRootPath());
    CString strVal;
    GetDlgItem(IDC_EDIT_XML_PATH)->GetWindowText(strVal);
    m_strLoadFile = strVal;

    GetDlgItem(IDC_EDIT_OUT_CS)->GetWindowText(strVal);
    if(strVal.Right(1) == '/' || strVal.Right(1) == '\\')
    {
        strVal.SetAt(strVal.GetLength()-1, '\0');
    }
    m_strOutputPath = strVal;
    oCompiler.SetOutputPath(m_strOutputPath.c_str());

    oCompiler.CompileCS(m_strLoadFile.c_str());
    GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(oCompiler.m_strErrMsg);
}

void CtblcoderDlg::OnBnClickedBtnCompilePy()
{
	CCompiler oCompiler;

	//strFile.Format("%s\\table.xml", GetRootPath());
	CString strVal;
	GetDlgItem(IDC_EDIT_XML_PATH)->GetWindowText(strVal);
	m_strLoadFile = strVal;

	GetDlgItem(IDC_EDIT_OUT_PY)->GetWindowText(strVal);

	if(strVal.Right(1) == '/' || strVal.Right(1) == '\\')
	{
		strVal.SetAt(strVal.GetLength()-1, '\0');
	}

	m_strOutputPath = strVal;
	oCompiler.SetOutputPath(m_strOutputPath.c_str());

	oCompiler.CompilePy(m_strLoadFile.c_str());

	GetDlgItem(IDC_EDIT_RESULT)->SetWindowText(oCompiler.m_strErrMsg);
}
