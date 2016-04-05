
// tblcoderDlg.h : header file
//

#pragma once

#include <string>

using namespace std;

// CtblcoderDlg dialog
class CtblcoderDlg : public CDialog
{
// Construction
public:
	CtblcoderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TBLCODER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	string m_strLoadFile;
	string m_strOutputPath;
	string m_strPropDir;
	
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnXmlPath();

	afx_msg void OnBnClickedBtnOutCpp();
	afx_msg void OnBnClickedBtnOutCpp2();
    afx_msg void OnBnClickedBtnOutCs();
	afx_msg void OnBnClickedBtnOutPy();

    afx_msg void OnBnClickedBtnCompileCpp();
	afx_msg void OnBnClickedBtnCompileCpp2();
    afx_msg void OnBnClickedBtnCompilerCs();
	afx_msg void OnBnClickedBtnCompilePy();

    afx_msg void OnBnClickedOk();
};
