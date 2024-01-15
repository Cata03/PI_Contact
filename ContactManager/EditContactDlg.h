#pragma once
#include "afxdialogex.h"
#include "MainDlg.h"

// EditContactDlg dialog
class EditContactDlg : public CDialogEx
{
	DECLARE_DYNAMIC(EditContactDlg)

public:
	EditContactDlg::EditContactDlg(CWnd* pParent /*=nullptr*/, int contactID /*= -1*/);
	virtual ~EditContactDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG6 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:;
	  CString m_EditCName;
	  CString EditCNumber;
	  CString EditCEmail;
	  int m_ContactID;

	afx_msg void OnBnClickedSaveedit();
};

