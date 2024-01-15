#pragma once
#include "afxdialogex.h"


// AddContactDlg dialog

class AddContactDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AddContactDlg)

public:
	AddContactDlg(CWnd* pParent = nullptr, const CString& loggedInUsername = L"");   // standard constructor
	virtual ~AddContactDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	CString m_LoggedInUsername;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedAddcontact();
	CString m_AddCName;
	CString m_AddCNumber;
	CString m_AddCEmail;
	afx_msg bool IsValidEmailFormat(const CString& email);
	afx_msg void OnBnClickedSendsms();
	afx_msg void OnBnClickedSendSms();
};
