// RegisterDlg.cpp : implementation file
//

#include "pch.h"
#include "ContactManager.h"
#include "afxdialogex.h"
#include "RegisterDlg.h"
#include "mysql.h"


// RegisterDlg dialog

IMPLEMENT_DYNAMIC(RegisterDlg, CDialogEx)

RegisterDlg::RegisterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_strRegisterUsername(_T(""))
	, m_strRegisterPassword(_T(""))
{

}

RegisterDlg::~RegisterDlg()
{
}

void RegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERNAME_REG, m_strRegisterUsername);
	DDX_Text(pDX, IDC_PASSWORD_REG, m_strRegisterPassword);
}


BEGIN_MESSAGE_MAP(RegisterDlg, CDialogEx)
	ON_BN_CLICKED(IDC_REGISTER, &RegisterDlg::OnBnClickedRegister)
END_MESSAGE_MAP()

CString RegisterDlg::CaesarEncrypt(const CString& input, int shift) {
    CString encrypted;

    for (int i = 0; i < input.GetLength(); ++i) {
        TCHAR ch = input[i];

        // Apply shift to alphabetical characters
        if (_istalpha(ch)) {
            TCHAR baseChar = (_istlower(ch) ? 'a' : 'A');
            TCHAR encryptedChar = (ch - baseChar + shift) % 26 + baseChar;
            encrypted.AppendChar(encryptedChar);
        }
        else {
            // For non-alphabetical characters, apply a shift directly to ASCII values
            TCHAR encryptedChar = (ch + shift) % 128;  // You can adjust the modulus value
            encrypted.AppendChar(encryptedChar);
        }
    }

    return encrypted;
}


// RegisterDlg message handlers

void RegisterDlg::OnBnClickedRegister()
{
    UpdateData(TRUE); // Update controls' data to variables

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

        // Check password requirements
        bool hasSpecialChar = false;
        bool hasCapitalLetter = false;
        bool hasNumber = false;

        for (int i = 0; i < m_strRegisterPassword.GetLength(); ++i) {
            TCHAR ch = m_strRegisterPassword.GetAt(i);

            if (_istalnum(ch)) {
                if (_istdigit(ch)) {
                    hasNumber = true;
                }
                else if (_istupper(ch)) {
                    hasCapitalLetter = true;
                }
            }
            else {
                hasSpecialChar = true;
            }
        }

        if (!(hasSpecialChar && (hasCapitalLetter || hasNumber))) {
            AfxMessageBox(_T("Password must contain at least one special character, one capital letter, or a number."));
            mysql_close(con);
            return;
        }

        // Check if the username is already taken
        CStringA usernameA(m_strRegisterUsername);
        CStringA checkQuery;
        checkQuery.Format("SELECT * FROM users WHERE Username='%s'", usernameA.GetString());

        if (mysql_real_query(con, checkQuery, static_cast<unsigned long>(checkQuery.GetLength()))) {
            fprintf(stderr, "Error executing username check query: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while checking the username. Please try again later."));
            mysql_close(con);
            return;
        }

        MYSQL_RES* checkRes = mysql_store_result(con);
        if (checkRes == NULL) {
            fprintf(stderr, "Error storing username check result: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while checking the username. Please try again later."));
            mysql_close(con);
            return;
        }

        int numCheckRows = mysql_num_rows(checkRes);

        if (numCheckRows > 0) {
            // Username already taken
            AfxMessageBox(_T("Username is already taken. Please choose a different one."));
        }
        else {
            // Username is available, proceed with registration
            // Choose a shift value for encryption
            int shift = 3;  // Use an appropriate shift value
            CString encryptedPassword = CaesarEncrypt(m_strRegisterPassword, shift);

            AfxMessageBox(_T("Password after encryption: ") + encryptedPassword);

            // Convert CString to const char*
            CT2CA pszConvertedAnsiString(encryptedPassword);
            const char* szAnsi = pszConvertedAnsiString;

            CStringA registerQuery;
            registerQuery.Format("INSERT INTO users (Username, PasswordHash) VALUES ('%s', '%s')",
                usernameA.GetString(), szAnsi);

            if (mysql_real_query(con, registerQuery, static_cast<unsigned long>(registerQuery.GetLength()))) {
                fprintf(stderr, "Error executing registration query: %s\n", mysql_error(con));
                AfxMessageBox(_T("Error during registration. MySQL error: ") + CString(mysql_error(con)));
            }
            else {
                AfxMessageBox(_T("Registration successful!"));
                EndDialog(IDOK);
            }
        }

        mysql_free_result(checkRes);
        mysql_close(con);
    }
    catch (...) {
        AfxMessageBox(_T("An unexpected error occurred. Please try again later."));
    }
}


