#include "pch.h"
#include "ContactManager.h"
#include "afxdialogex.h"
#include "AddReminderDlg.h"
#include "mysql.h"

// AddReminderDlg dialog
IMPLEMENT_DYNAMIC(AddReminderDlg, CDialogEx)

AddReminderDlg::AddReminderDlg(CWnd* pParent /*= nullptr*/, const int& userID /*= 0*/)
    : CDialogEx(IDD_DIALOG7, pParent),ADDTEXTREM(_T("")), m_userID(userID)
{
}

AddReminderDlg::~AddReminderDlg()
{
}

void AddReminderDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DATEPICKER, m_datePicker);
    DDX_Control(pDX, IDC_TIMEPICKER, m_timePicker);
    DDX_Text(pDX, IDC_ADDTEXTREM, ADDTEXTREM);
}

BEGIN_MESSAGE_MAP(AddReminderDlg, CDialogEx)
    ON_BN_CLICKED(IDC_SAVECONTACT, &AddReminderDlg::OnBnClickedSavecontact)
END_MESSAGE_MAP()

BOOL AddReminderDlg::OnInitDialog()
{
    // ...

    // Subclass date picker control
    m_datePicker.SubclassDlgItem(IDC_DATEPICKER, this);
    m_datePicker.SetFormat(L"yyyy-MM-dd");

    // Subclass time picker control
    m_timePicker.SubclassDlgItem(IDC_TIMEPICKER, this);
    m_timePicker.SetFormat(L"HH:mm");

    return TRUE;
}

void AddReminderDlg::OnBnClickedSavecontact()
{
    // Get the selected date and time from the date and time pickers
    COleDateTime selectedDate, selectedTime;
    m_datePicker.GetTime(selectedDate);
    m_timePicker.GetTime(selectedTime);

    // Format the date and time as needed (adjust the format based on your database schema)
    CString formattedDate = selectedDate.Format(_T("%Y-%m-%d"));
    CString formattedTime = selectedTime.Format(_T("%H:%M:%S"));

    // Get the text entered by the user
    UpdateData(TRUE);  // Transfer data from controls to variables
    CString reminderText = ADDTEXTREM;

    // Assuming m_userID is the ID of the logged-in user
    int userID = m_userID;  // Replace this with your actual user ID retrieval

    // Insert the new reminder into the database
    CString query;
    query.Format(L"INSERT INTO reminders (UserID, ExpirationDate, Time, Description) VALUES (%d, '%s', '%s', '%s')",
        userID, formattedDate, formattedTime, reminderText);

    // Execute the SQL query
    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        AfxMessageBox(L"Failed to initialize MySQL connection.", MB_ICONERROR);
        return;
    }

    if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL)
    {
        AfxMessageBox(L"Failed to connect to MySQL server.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    if (mysql_query(con, "USE appcontacte"))
    {
        AfxMessageBox(L"Failed to select database.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    if (mysql_query(con, CStringA(query.GetString())))
    {
        CString errorMessage = L"Failed to execute query. Error: ";
        errorMessage += mysql_error(con);

        AfxMessageBox(errorMessage, MB_ICONERROR);
        AfxMessageBox(query, MB_ICONERROR);  // Display the query for debugging

        mysql_close(con);
        return;
    }

    // Clean up resources
    mysql_close(con);

    // Close the dialog (optional)
    EndDialog(IDOK);
}



