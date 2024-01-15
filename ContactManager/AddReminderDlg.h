#pragma once
#include "afxdialogex.h"

// AddReminderDlg dialog
class AddReminderDlg : public CDialogEx
{
    DECLARE_DYNAMIC(AddReminderDlg)

public:
    AddReminderDlg::AddReminderDlg(CWnd* pParent /*= nullptr*/, const int& userID = -1);
    virtual ~AddReminderDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG7 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

public:
    int m_userID;  // modified to int
    CDateTimeCtrl m_datePicker;
    CDateTimeCtrl m_timePicker;
    CString ADDTEXTREM;
    afx_msg void OnBnClickedSavecontact();
};
