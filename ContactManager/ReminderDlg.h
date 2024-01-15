#pragma once
#include "afxdialogex.h"
#include "afxwin.h"
#include "AddReminderDlg.h"
#include "MainDlg.h"

class ReminderDlg : public CDialogEx
{
    DECLARE_DYNAMIC(ReminderDlg)

public:
    ReminderDlg(CWnd* pParent = nullptr, const int& userID = -1);  // modified constructor
    virtual ~ReminderDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG4 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
    DECLARE_MESSAGE_MAP()

public:
    int m_userID;  // modified to int
    afx_msg void OnBnClickedAddrem();
    afx_msg void OnBnClickedDelrem();
    afx_msg void DeleteSelectedReminder();
    afx_msg void PopulateReminderList();
    virtual BOOL OnInitDialog();
    CListCtrl ListaReminders1;
    afx_msg void OnLvnItemchangedListctrlrem(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLvnGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult);
};
