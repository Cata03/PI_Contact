// AddEmailDlg.cpp : implementation file
//

#include "pch.h"
#include "ContactManager.h"
#include "afxdialogex.h"
#include "AddEmailDlg.h"
#include "MainDlg.h"
#include "mysql.h"


// AddEmailDlg dialog

IMPLEMENT_DYNAMIC(AddEmailDlg, CDialogEx)

AddEmailDlg::AddEmailDlg(CWnd* pParent /*=nullptr*/, const CString& loggedInUsername)
    : CDialogEx(IDD_DIALOG3, pParent)
    , m_LoggedInUsername(loggedInUsername)
	, m_ADCHemail(_T(""))
    , m_ADCHempass(_T(""))
{

}

AddEmailDlg::~AddEmailDlg()
{
}

BOOL AddEmailDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Initialize the email edit control with the existing email if available
    if (!m_LoggedInUsername.IsEmpty()) {
        CString existingEmail, existingPassword;
        GetExistingDataFromDatabase(m_LoggedInUsername, existingEmail, existingPassword);

        // Populate the email and password edit controls
        m_ADCHemail = existingEmail;
        m_ADCHempass = existingPassword;
        UpdateData(FALSE); // Update the dialog controls
    }

    return TRUE;
}

void AddEmailDlg::GetExistingDataFromDatabase(const CString& username, CString& existingEmail, CString& existingPassword)
{
    try {
        MYSQL* con = mysql_init(NULL);

        if (con == NULL) {
            fprintf(stderr, "Error initializing MySQL connection: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while initializing the connection. Please try again later."));
            return;
        }

        if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL) {
            fprintf(stderr, "Error connecting to MySQL server: %s\n", mysql_error(con));
            AfxMessageBox(_T("Failed to connect to the MySQL server. Please check your connection and try again."));
            mysql_close(con);
            return;
        }

        // Construct the SELECT query to retrieve the email and password based on the username
        CStringA usernameA(username);
        CStringA selectQuery;
        selectQuery.Format("SELECT Email, EmailPassword FROM users WHERE Username='%s'", usernameA.GetString());

        if (mysql_query(con, selectQuery)) {
            fprintf(stderr, "Error executing SELECT query: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while retrieving the existing data. Please try again later."));
            mysql_close(con);
            return;
        }

        MYSQL_RES* result = mysql_store_result(con);
        if (result == NULL) {
            fprintf(stderr, "Error storing SELECT result: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while retrieving the existing data. Please try again later."));
            mysql_close(con);
            return;
        }

        // Check if there is a row in the result set
        if (mysql_num_rows(result) > 0) {
            MYSQL_ROW row = mysql_fetch_row(result);
            existingEmail = row[0]; // Assuming the email is in the first column
            existingPassword = row[1]; // Assuming the password is in the second column
        }
        else {
            AfxMessageBox(_T("No existing data found for the provided username."));
        }

        mysql_free_result(result);
        mysql_close(con);
    }
    catch (...) {
        AfxMessageBox(_T("An unexpected error occurred while retrieving the existing data. Please try again later."));
    }
}

void AddEmailDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_ADCHEDIT, m_ADCHemail);
    DDX_Text(pDX, IDC_ADCHEMPASS, m_ADCHempass);
}


BEGIN_MESSAGE_MAP(AddEmailDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ADCHSAVE, &AddEmailDlg::OnBnClickedAdchsave)
END_MESSAGE_MAP()


// AddEmailDlg message handlers


void AddEmailDlg::OnBnClickedAdchsave()
{
    UpdateData(TRUE); // Update controls' data to variables

    // Validate the email format
    if (!IsEmailValid(m_ADCHemail)) {
        AfxMessageBox(_T("Invalid email format. Please enter a valid email address."));
        return;
    }

    try {
        MYSQL* con = mysql_init(NULL);

        if (con == NULL) {
            fprintf(stderr, "Error initializing MySQL connection: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while initializing the connection. Please try again later."));
            return;
        }

        if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL) {
            fprintf(stderr, "Error connecting to MySQL server: %s\n", mysql_error(con));
            AfxMessageBox(_T("Failed to connect to the MySQL server. Please check your connection and try again."));
            mysql_close(con);
            return;
        }

        if (mysql_query(con, "USE appcontacte")) {
            fprintf(stderr, "Error selecting database: %s\n", mysql_error(con));
            AfxMessageBox(_T("Failed to select the database. Please try again later."));
            mysql_close(con);
            return;
        }

        // Proceed with saving email and password
        CStringA usernameA(m_LoggedInUsername);
        CStringA emailA(m_ADCHemail);
        CStringA passwordA(m_ADCHempass);
        CStringA saveQuery;
        saveQuery.Format("UPDATE users SET Email='%s', EmailPassword='%s' WHERE Username='%s'", emailA.GetString(), passwordA.GetString(), usernameA.GetString());

        if (mysql_real_query(con, saveQuery, static_cast<unsigned long>(saveQuery.GetLength()))) {
            fprintf(stderr, "Error executing save email query: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while saving the email. Please try again later."));
        }
        else {
            AfxMessageBox(_T("Email and password saved successfully!"));
            EndDialog(IDOK);
        }

        mysql_close(con);
    }
    catch (...) {
        AfxMessageBox(_T("An unexpected error occurred. Please try again later."));
    }
}
bool AddEmailDlg::IsEmailValid(const CString& email)
{
    // Basic email validation, you can enhance this as needed
    // This is a simple check for the presence of '@' and '.'
    return email.Find('@') != -1 && email.Find('.') != -1;
}
