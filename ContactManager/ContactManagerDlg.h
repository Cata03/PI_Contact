
// ContactManagerDlg.h : header file
//

#pragma once


// CContactManagerDlg dialog
class CContactManagerDlg : public CDialogEx
{
// Construction
public:
	CContactManagerDlg(CWnd* pParent = nullptr);	// standard constructor
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONTACTMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strUsername;
	CString m_strPassword;
	afx_msg void OnBnClickedLoginButton();
	afx_msg void OnBnClickedRegisterButton();
	CString CaesarEncrypt(const CString& input, int shift);
	CString CaesarDecrypt(const CString& encrypted, int shift);
};
