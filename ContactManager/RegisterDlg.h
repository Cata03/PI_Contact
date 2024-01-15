#pragma once
#include "afxdialogex.h"


// RegisterDlg dialog

class RegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~RegisterDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strRegisterUsername;
	CString m_strRegisterPassword;
	afx_msg void OnBnClickedRegister();
	CString CaesarEncrypt(const CString& input, int shift);
};
