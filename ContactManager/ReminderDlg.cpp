#include "pch.h"
#include "ContactManager.h"
#include "afxdialogex.h"
#include "ReminderDlg.h"
#include "AddReminderDlg.h"  // Include the header file for AddReminderDlg
#include "mysql.h"

// ReminderDlg dialog
IMPLEMENT_DYNAMIC(ReminderDlg, CDialogEx)

ReminderDlg::ReminderDlg(CWnd* pParent /*= nullptr*/, const int& userID /*= 0*/)
    : CDialogEx(IDD_DIALOG4, pParent), m_userID(userID)
{
}

ReminderDlg::~ReminderDlg()
{
}

void ReminderDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LISTCTRLREM, ListaReminders1);
}

BEGIN_MESSAGE_MAP(ReminderDlg, CDialogEx)
    ON_BN_CLICKED(IDC_ADDREM, &ReminderDlg::OnBnClickedAddrem)
    ON_BN_CLICKED(IDC_DELREM, &ReminderDlg::OnBnClickedDelrem)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRLREM, &ReminderDlg::OnLvnItemchangedListctrlrem)
    ON_NOTIFY(LVN_GETINFOTIP, IDC_LISTCTRLREM, &ReminderDlg::OnLvnGetInfoTip)
END_MESSAGE_MAP()

BOOL ReminderDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Set up the reminder list control
    ListaReminders1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_HEADERDRAGDROP);
    ListaReminders1.InsertColumn(0, L"ID", LVCFMT_LEFT, 50);
    ListaReminders1.InsertColumn(1, L"Date", LVCFMT_LEFT, 80);
    ListaReminders1.InsertColumn(2, L"Hour", LVCFMT_LEFT, 80);
    ListaReminders1.InsertColumn(3, L"Text", LVCFMT_LEFT, 295);

    // Populate the reminder list
    PopulateReminderList();

    return TRUE;
}

void ReminderDlg::OnBnClickedAddrem()
{
    // AddReminderDlg handles adding reminders to the database
    AddReminderDlg addReminderDlg(this, m_userID);
    if (addReminderDlg.DoModal() == IDOK)
    {
        // Refresh the reminder list after adding a new reminder
        PopulateReminderList();
    }
}

void ReminderDlg::OnBnClickedDelrem()
{
    // Delete the selected reminder from the list and the database
    DeleteSelectedReminder();
    // Refresh the reminder list after deletion
    PopulateReminderList();
}

void ReminderDlg::PopulateReminderList()
{
    // Clear existing items in the list
    ListaReminders1.DeleteAllItems();

    // Fetch data from the database based on the logged-in user ID
    CString query;
    query.Format(L"SELECT * FROM reminders WHERE UserID = %d", m_userID);

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
        AfxMessageBox(L"Failed to execute query.", MB_ICONERROR);
        AfxMessageBox(query, MB_ICONERROR);  // Display the query for debugging
        mysql_close(con);
        return;
    }

    MYSQL_RES* res = mysql_store_result(con);

    if (res == NULL)
    {
        AfxMessageBox(L"Failed to store query result.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    // Loop through the result set and populate the list control
    int rowIndex = 0;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        int reminderID = atoi(row[0]);

        // Parse date and time using COleDateTime
        COleDateTime expirationDate, time;

        if (!expirationDate.ParseDateTime(CString(row[2])))
        {
            AfxMessageBox(L"Failed to parse expiration date.", MB_ICONERROR);
            continue;  // Skip to the next iteration
        }

        if (!time.ParseDateTime(CString(row[3])))
        {
            AfxMessageBox(L"Failed to parse expiration time.", MB_ICONERROR);
            continue;  // Skip to the next iteration
        }

        // Format date and time
        CString formattedDate = expirationDate.Format(_T("%Y-%m-%d"));
        CString formattedHour = time.Format(_T("%H:%M:%S"));
        CString text = CString(row[4]);

        // Add a new row to the list control
        CString strReminderID;
        strReminderID.Format(_T("%d"), reminderID);

        int itemIndex = ListaReminders1.InsertItem(rowIndex, strReminderID);

        if (itemIndex != -1)
        {
            // Set the formatted strings as item text for additional columns
            ListaReminders1.SetItemText(itemIndex, 1, formattedDate);
            ListaReminders1.SetItemText(itemIndex, 2, formattedHour);
            ListaReminders1.SetItemText(itemIndex, 3, text);
        }
        else
        {
            AfxMessageBox(L"Failed to insert item in the list.", MB_ICONERROR);
            mysql_free_result(res);
            mysql_close(con);
            return;
        }

        rowIndex++;
    }

    // Clean up resources
    mysql_free_result(res);
    mysql_close(con);

    if (rowIndex == 0)
    {
        AfxMessageBox(L"No reminders found for the current user.", MB_ICONINFORMATION);
    }
}

void ReminderDlg::OnLvnGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVGETINFOTIP* pGetInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);

    int itemIndex = pGetInfoTip->iItem;
    int subItemIndex = pGetInfoTip->iSubItem;

    CString infoTipText;

    // Assuming column indices for ID, Date, Hour, and Text
    switch (subItemIndex)
    {
    case 0: // ID
        infoTipText.Format(L"ID: %s", ListaReminders1.GetItemText(itemIndex, subItemIndex));
        break;
    case 1: // Date
        infoTipText.Format(L"DATE: %s", ListaReminders1.GetItemText(itemIndex, subItemIndex));
        break;
    case 2: // Hour
        infoTipText.Format(L"HOUR: %s", ListaReminders1.GetItemText(itemIndex, subItemIndex));
        break;
    case 3: // Text
        infoTipText.Format(L"DETAILS: %s", ListaReminders1.GetItemText(itemIndex, subItemIndex));
        break;
    default:
        break;
    }

    wcsncpy_s(pGetInfoTip->pszText, pGetInfoTip->cchTextMax, infoTipText, infoTipText.GetLength());
    *pResult = 0;
}


void ReminderDlg::DeleteSelectedReminder()
{
    // Get the selected item index
    int selectedIndex = ListaReminders1.GetNextItem(-1, LVNI_SELECTED);

    if (selectedIndex != -1)
    {
        // Open a confirmation dialog before deleting the reminder
        if (AfxMessageBox(L"Are you sure you want to delete this reminder?", MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            // Get the reminder ID from the selected item
            int reminderID = _wtoi(ListaReminders1.GetItemText(selectedIndex, 0));

            // Assuming m_userID is an integer variable holding the user ID
            MYSQL* con = mysql_init(NULL);

            if (con == NULL)
            {
                // Handle error initializing MySQL connection
                AfxMessageBox(L"Failed to initialize MySQL connection.", MB_ICONERROR);
                return;
            }

            if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL)
            {
                // Handle error connecting to MySQL server
                AfxMessageBox(L"Failed to connect to MySQL server.", MB_ICONERROR);
                mysql_close(con);
                return;
            }

            if (mysql_query(con, "USE appcontacte"))
            {
                // Handle error selecting database
                AfxMessageBox(L"Failed to select database.", MB_ICONERROR);
                mysql_close(con);
                return;
            }

            // Assuming 'reminders' is the name of your reminders table
            CStringA query;
            query.Format("DELETE FROM reminders WHERE UserID = %d AND ReminderID = %d", m_userID, reminderID);

            if (mysql_query(con, query))
            {
                // Handle error executing query
                AfxMessageBox(L"Failed to execute delete query.", MB_ICONERROR);
                mysql_close(con);
                return;
            }

            // Remove the selected item from the list
            ListaReminders1.DeleteItem(selectedIndex);

            // Clean up resources
            mysql_close(con);
        }
    }
    else
    {
        // No item selected, display a message
        AfxMessageBox(L"Please select a reminder to delete.", MB_ICONWARNING);
    }
}

void ReminderDlg::OnLvnItemchangedListctrlrem(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;
}
