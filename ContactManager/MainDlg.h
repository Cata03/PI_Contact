#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// MainDlg dialog

class MainDlg : public CDialogEx
{
    DECLARE_DYNAMIC(MainDlg)

public:
    MainDlg(CWnd* pParent = nullptr, const CString& loggedInUsername = L"");
    virtual ~MainDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG2 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();  // Override OnInitDialog

    DECLARE_MESSAGE_MAP()
public:
    CString m_LoggedInUsername;
    CListCtrl m_contactList;
    int m_nSortColumn;
    bool m_bSortAscending;
    int m_ContactID;
    int m_userID;
    int m_SelectedContactID;
    afx_msg void CheckUpcomingReminders();
    afx_msg void DeleteReminderFromDatabase(int reminderID);
    afx_msg BOOL IsReminderDue(const CString& expirationDate, const CString& time);
    afx_msg void OnLvnItemchangedContactList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void PopulateContactList();
    afx_msg void UpdateFavoriteStatusInDatabase(int itemIndex, BOOL isFavorite);
    afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
    afx_msg void SortContactList(int nSortColumn, bool bSortAscending);
    afx_msg void OnBnClickedExpcsv();
    afx_msg void OnBnClickedImpcsv();
    afx_msg void ExportContactListToCSV(const CString& filePath);
    afx_msg void ImportCSVToDatabase(const CString& filePath);
    afx_msg int ParseCSVLine(const CString& line, CStringArray& fields);
    afx_msg bool IsDuplicateContact(const CString& email, const CString& phoneNumber);
    afx_msg void OnBnClickedAddmailbutton();
    afx_msg void OnBnClickedMinimizemain();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedDelcontact();
    afx_msg bool DeleteContactFromDatabase(int contactID);
    afx_msg void OnBnClickedEditcontact();
    afx_msg bool GetContactInformation(int itemIndex, CString& name, CString& phoneNumber, CString& email, int& contactID);
    afx_msg void OnBnClickedReminders();
    afx_msg int GetUserIDFromDatabase(const CString& username);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedSendmail();
    afx_msg void OnBnClickedSeemail();
};
