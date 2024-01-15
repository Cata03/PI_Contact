#pragma once
#include "afxdialogex.h"
#include "mysql.h"


// AddEmailDlg dialog

class AddEmailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AddEmailDlg)

public:
	AddEmailDlg(CWnd* pParent = nullptr, const CString& loggedInUser = L"");   // standard constructor
	virtual ~AddEmailDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CString m_LoggedInUsername;
	afx_msg void OnBnClickedAdchsave();
	CString m_ADCHemail;
	afx_msg bool IsEmailValid(const CString& email);
	afx_msg void GetExistingDataFromDatabase(const CString& username, CString& existingEmail, CString& existingPassword);
	CString m_ADCHempass;
};
