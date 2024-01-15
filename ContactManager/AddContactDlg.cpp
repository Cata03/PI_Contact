// AddContactDlg.cpp : implementation file
//

#include "pch.h"
#include "ContactManager.h"
#include "afxdialogex.h"
#include "AddContactDlg.h"
#include "mysql.h"
#include "MainDlg.h"


// AddContactDlg dialog

IMPLEMENT_DYNAMIC(AddContactDlg, CDialogEx)

AddContactDlg::AddContactDlg(CWnd* pParent /*=nullptr*/, const CString& loggedInUsername)
	: CDialogEx(IDD_DIALOG5, pParent), m_LoggedInUsername(loggedInUsername)
	, m_AddCName(_T(""))
	, m_AddCNumber(_T(""))
	, m_AddCEmail(_T(""))
{

}

AddContactDlg::~AddContactDlg()
{
}

void AddContactDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ACNUME, m_AddCName);
	DDX_Text(pDX, IDC_ACNUMBER, m_AddCNumber);
	DDX_Text(pDX, IDC_ACEMAIL, m_AddCEmail);
}


BEGIN_MESSAGE_MAP(AddContactDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ADDCONTACT, &AddContactDlg::OnBnClickedAddcontact)
END_MESSAGE_MAP()


// AddContactDlg message handlers


void AddContactDlg::OnBnClickedAddcontact()
{
	// TODO: Add your control notification handler code here
		// Get the entered values
	UpdateData(TRUE);

	// Validate email format
	if (!IsValidEmailFormat(m_AddCEmail))
	{
		AfxMessageBox(L"Invalid email format. Please enter a valid email address.", MB_ICONWARNING);
		return;
	}

	// Add the contact to the database
	MYSQL* con = mysql_init(NULL);

	if (con == NULL)
	{
		AfxMessageBox(L"Error initializing MySQL connection.", MB_ICONERROR);
		return;
	}

	if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
	{
		AfxMessageBox(L"Error connecting to MySQL server.", MB_ICONERROR);
		mysql_close(con);
		return;
	}

	if (mysql_query(con, "USE appcontacte"))
	{
		AfxMessageBox(L"Error selecting database.", MB_ICONERROR);
		mysql_close(con);
		return;
	}

	CString query;
	query.Format(L"INSERT INTO contacts (UserID, Name, PhoneNumber, Email) VALUES ((SELECT UserID FROM users WHERE Username = '%s'), '%s', '%s', '%s')",
		m_LoggedInUsername, m_AddCName, m_AddCNumber, m_AddCEmail);

	if (mysql_query(con, CStringA(query)))
	{
		AfxMessageBox(L"Failed to add contact. Please try again.", MB_ICONERROR);
		mysql_close(con);
		return;
	}

	AfxMessageBox(L"Contact added successfully!", MB_ICONINFORMATION);
	mysql_close(con);

	EndDialog(IDOK); // Close the dialog on success
}
bool AddContactDlg::IsValidEmailFormat(const CString& email)
{
	// Implement email format validation logic
	// You can use regular expressions or a simpler logic depending on your requirements

	// Example: Check for a basic format (at least one character before and after @)
	int atIndex = email.Find(L'@');
	return (atIndex > 0 && atIndex < email.GetLength() - 1);
}
