#include "pch.h"
#include "ContactManager.h"
#include "afxdialogex.h"
#include "EditContactDlg.h"
#include "mysql.h"
#include "MainDlg.h"

// EditContactDlg dialog

IMPLEMENT_DYNAMIC(EditContactDlg, CDialogEx)

EditContactDlg::EditContactDlg(CWnd* pParent /*=nullptr*/, int contactID /*= -1*/)
	: CDialogEx(IDD_DIALOG6, pParent)
	, m_ContactID(contactID)
{
}


EditContactDlg::~EditContactDlg()
{
}

void EditContactDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITCNAME, m_EditCName);
	DDX_Text(pDX, IDC_EDITCNUMBER, EditCNumber);
	DDX_Text(pDX, IDC_EDITCEMAIL, EditCEmail);
}

BEGIN_MESSAGE_MAP(EditContactDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SAVEEDIT, &EditContactDlg::OnBnClickedSaveedit)
END_MESSAGE_MAP()

BOOL EditContactDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Initialize your edit controls here
	GetDlgItem(IDC_EDITCNAME)->SetWindowText(m_EditCName);
	GetDlgItem(IDC_EDITCNUMBER)->SetWindowText(EditCNumber);
	GetDlgItem(IDC_EDITCEMAIL)->SetWindowText(EditCEmail);

	return TRUE;
}

void EditContactDlg::OnBnClickedSaveedit()
{
    UpdateData(TRUE);

    try {
        MYSQL* con = mysql_init(NULL);

        if (con == NULL) {
            AfxMessageBox(_T("An error occurred while initializing the connection."));
            return;
        }

        if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL) {
            AfxMessageBox(_T("Failed to connect to the MySQL server. Please check your connection and try again."));
            mysql_close(con);
            return;
        }

        CStringA updateQuery;
        updateQuery.Format("UPDATE contacts SET Name='%s', PhoneNumber='%s', Email='%s' WHERE ContactID=%d",
            CStringA(m_EditCName).GetString(), CStringA(EditCNumber).GetString(), CStringA(EditCEmail).GetString(), m_ContactID);

        // Display a message box to check the update query
        AfxMessageBox(_T("Update Query: ") + CString(updateQuery));

        if (mysql_query(con, updateQuery)) {
            AfxMessageBox(_T("An error occurred while updating contact information in the database."));
            mysql_close(con);
            return;
        }

        mysql_close(con);

        AfxMessageBox(_T("Contact information updated successfully!"));
        EndDialog(IDOK);
    }
    catch (...) {
        AfxMessageBox(_T("An unexpected error occurred while updating contact information."));
    }
}
