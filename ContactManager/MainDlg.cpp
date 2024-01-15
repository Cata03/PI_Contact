// MainDlg.cpp : implementation file
//

#include "pch.h"
#include "ContactManager.h"
#include "afxdialogex.h"
#include "MainDlg.h"
#include "AddEmailDlg.h"
#include "mysql.h"
#include "AddContactDlg.h"
#include "EditContactDlg.h"
#include "ReminderDlg.h"
#define TIMER_ID_REMINDER_CHECK 1
// MainDlg dialog

IMPLEMENT_DYNAMIC(MainDlg, CDialogEx)

void MainDlg::OnTimer(UINT_PTR nIDEvent)
{
    TRACE(L"Timer fired: %d\n", nIDEvent);

    if (nIDEvent == TIMER_ID_REMINDER_CHECK)
    {
        CheckUpcomingReminders();
    }

    CDialogEx::OnTimer(nIDEvent);
}

MainDlg::MainDlg(CWnd* pParent /*=nullptr*/, const CString& loggedInUsername)
    : CDialogEx(IDD_DIALOG2, pParent), m_LoggedInUsername(loggedInUsername), m_nSortColumn(-1), m_bSortAscending(true)
{
}

MainDlg::~MainDlg()
{
}
void MainDlg::PopulateContactList()
{
    // Clear existing items in the list
    m_contactList.DeleteAllItems();

    // Fetch data from the database based on the logged-in username
    CString query;
    query.Format(L"SELECT * FROM contacts WHERE UserID = (SELECT UserID FROM users WHERE Username = '%s')", m_LoggedInUsername);

    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        // Handle error initializing MySQL connection
        return;
    }

    if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
    {
        // Handle error connecting to MySQL server
        mysql_close(con);
        return;
    }

    if (mysql_query(con, "USE appcontacte"))
    {
        // Handle error selecting database
        mysql_close(con);
        return;
    }

    if (mysql_query(con, CStringA(query.GetString())))
    {
        // Handle error executing query
        mysql_close(con);
        return;
    }

    MYSQL_RES* res = mysql_store_result(con);

    if (res == NULL)
    {
        // Handle error storing query result
        mysql_close(con);
        return;
    }

    // Loop through the result set and populate the list control
    int rowIndex = 0;
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        // Assuming the order of columns in the result set matches the order in the list control
        int contactID = atoi(row[0]);
        CString id = CString(row[0]);  // Convert char* to CString
        CString name = CString(row[2]);  // Convert char* to CString
        CString phoneNumber = CString(row[3]);  // Convert char* to CString
        CString email = CString(row[4]);  // Convert char* to CString
        int groupID = 0;
        if (row[5] != NULL)
        {
            // Attempt to convert the group ID to an integer
            groupID = atoi(row[5]);

            // Check if the conversion was successful
            if (groupID == 0 && strcmp(row[5], "0") != 0)
            {
                // Handle the error (e.g., log the issue, display an error message)
                TRACE(L"Error converting group ID to integer: %s\n", row[5]);
            }
        }

        // Add a new row to the list control
        int itemIndex = m_contactList.InsertItem(rowIndex, id);
        m_contactList.SetItemText(itemIndex, 1, name);
        m_contactList.SetItemText(itemIndex, 2, phoneNumber);
        m_contactList.SetItemText(itemIndex, 3, email);

        // Set the checkbox state based on the favorite status from the database
        BOOL isFavorite = atoi(row[6]) != 0;
        m_contactList.SetCheck(itemIndex, isFavorite);

        rowIndex++;
    }
    // Clean up resources
    mysql_free_result(res);
    mysql_close(con);
}

BOOL MainDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetTimer(TIMER_ID_REMINDER_CHECK, 30000, nullptr);
    m_contactList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_CHECKBOXES | LVS_EX_AUTOSIZECOLUMNS | LVS_EX_HEADERDRAGDROP);
    m_contactList.InsertColumn(0, L"Favorite/ID", LVCFMT_LEFT, 80);
    m_contactList.InsertColumn(1, L"Name", LVCFMT_LEFT, 105);
    m_contactList.InsertColumn(2, L"Phone Number", LVCFMT_LEFT, 100);
    m_contactList.InsertColumn(3, L"Email", LVCFMT_LEFT, 155);
    m_contactList.InsertColumn(4, L"Group", LVCFMT_LEFT, 85);
    PopulateContactList();
    return TRUE;
}

void MainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CONTACT_LIST, m_contactList);
}

BEGIN_MESSAGE_MAP(MainDlg, CDialogEx)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONTACT_LIST, &MainDlg::OnLvnItemchangedContactList)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_CONTACT_LIST, &MainDlg::OnColumnClick)
    ON_BN_CLICKED(IDC_EXPCSV, &MainDlg::OnBnClickedExpcsv)
    ON_BN_CLICKED(IDC_IMPCSV, &MainDlg::OnBnClickedImpcsv)
    ON_BN_CLICKED(IDC_ADDMAILBUTTON, &MainDlg::OnBnClickedAddmailbutton)
    ON_BN_CLICKED(IDC_MinimizeMain, &MainDlg::OnBnClickedMinimizemain)
    ON_BN_CLICKED(IDC_BUTTON1, &MainDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_DELCONTACT, &MainDlg::OnBnClickedDelcontact)
    ON_BN_CLICKED(IDC_EDITCONTACT, &MainDlg::OnBnClickedEditcontact)
    ON_BN_CLICKED(IDC_Reminders, &MainDlg::OnBnClickedReminders)
    ON_WM_TIMER()
END_MESSAGE_MAP()
void MainDlg::CheckUpcomingReminders()
{
    TRACE(L"Entering CheckUpcomingReminders\n");

    // Connect to the database
    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        AfxMessageBox(L"Error initializing MySQL connection.", MB_ICONERROR);
        return;
    }

    if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL)
    {
        AfxMessageBox(L"Error connecting to MySQL server.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    TRACE(L"Connected to MySQL database successfully\n");

    if (mysql_query(con, "USE appcontacte"))
    {
        AfxMessageBox(L"Error selecting database.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    // Get the current date and time
    CTime currentTime = CTime::GetCurrentTime();
    CString currentDate = currentTime.Format(L"%Y-%m-%d");

    // Construct the SQL query to check for upcoming reminders (compare only the date part)
    CString query;
    query.Format(L"SELECT * FROM reminders WHERE ExpirationDate >= '%s'", currentDate);

    if (mysql_query(con, CStringA(query.GetString())))
    {
        AfxMessageBox(L"Error executing query.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    TRACE(L"Query executed successfully\n");

    MYSQL_RES* res = mysql_store_result(con);

    if (res == NULL)
    {
        AfxMessageBox(L"Error storing query result.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    // Loop through the result set
    MYSQL_ROW row;
    int userID = GetUserIDFromDatabase(m_LoggedInUsername);

    int numRows = mysql_num_rows(res);
    TRACE(L"Number of rows fetched: %d\n", numRows);

    while ((row = mysql_fetch_row(res)) != NULL)
    {
        // Extract data from the row
        int reminderID = atoi(CStringA(row[0]));
        int reminderUserID = atoi(CStringA(row[1]));
        CString expirationDateStr = CString(row[2]);
        CString timeStr = CString(row[3]);
        CString description = CString(row[4]);

        // Convert string to COleDateTime for date and time
        COleDateTime expirationDate, time;

        if (!expirationDate.ParseDateTime(expirationDateStr))
        {
            AfxMessageBox(L"Failed to parse expiration date.", MB_ICONERROR);
            continue;  // Skip to the next iteration
        }

        if (!time.ParseDateTime(timeStr))
        {
            AfxMessageBox(L"Failed to parse expiration time.", MB_ICONERROR);
            continue;  // Skip to the next iteration
        }

        // Combine date and time to get the full expiration datetime
        COleDateTime expirationDateTime(expirationDate.GetYear(), expirationDate.GetMonth(), expirationDate.GetDay(),
            time.GetHour(), time.GetMinute(), time.GetSecond());

        // Get the current date and time
        COleDateTime currentDateTime = COleDateTime::GetCurrentTime();

        // Calculate the time difference in minutes
        COleDateTimeSpan timeDifference = expirationDateTime - currentDateTime;
        int minutesDifference = static_cast<int>(timeDifference.GetTotalMinutes());

        // Check if the reminder is within ±1 minute range
        if (minutesDifference >= -1 && minutesDifference <= 1)
        {
            // Format date and time
            CString formattedDate = expirationDate.Format(_T("%Y-%m-%d"));
            CString formattedHour = time.Format(_T("%H:%M:%S"));

            // Show a notification
            CString notificationMessage;
            notificationMessage.Format(L"Reminder: %s\nDate: %s\nTime: %s", description, formattedDate, formattedHour);
            TRACE(L"Notification displayed for reminder: %s\n", description);
            MessageBox(notificationMessage, L"Reminder", MB_ICONINFORMATION);

            // Delete the reminder after showing the notification
            DeleteReminderFromDatabase(reminderID);

            TRACE(L"Reminder deleted from the database: ID %d\n", reminderID);
        }
    }


    TRACE(L"Exiting CheckUpcomingReminders\n");

    // Clean up resources
    mysql_free_result(res);
    mysql_close(con);
}


BOOL MainDlg::IsReminderDue(const CString& expirationDate, const CString& time)
{
    // Implement logic to check if the reminder is due based on the current date and time
    // For simplicity, compare the expirationDate and time with the current date and time

    // Get the current date and time
    CTime currentTime = CTime::GetCurrentTime();
    CString currentDateTime = currentTime.Format(L"%Y-%m-%d %H:%M:%S");

    // Construct the full reminder datetime by combining expirationDate and time
    CString fullReminderDateTime = expirationDate + L" " + time;

    // Compare the full reminder datetime with the current datetime
    return currentDateTime >= fullReminderDateTime;
}

void MainDlg::DeleteReminderFromDatabase(int reminderID)
{
    // Connect to the database
    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        AfxMessageBox(L"Error initializing MySQL connection.", MB_ICONERROR);
        return;
    }

    if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL)
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

    // Construct the SQL DELETE statement
    CString deleteQuery;
    deleteQuery.Format(L"DELETE FROM reminders WHERE ReminderID = %d", reminderID);

    if (mysql_query(con, CStringA(deleteQuery.GetString())))
    {
        AfxMessageBox(L"Error executing delete query.", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    // Clean up resources
    mysql_close(con);
}

void MainDlg::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
    int nClickedColumn = pNMListView->iSubItem;

    if (nClickedColumn == m_nSortColumn)
    {
        // Same column clicked, toggle the sort order
        m_bSortAscending = !m_bSortAscending;
    }
    else
    {
        // Different column clicked, set the new column and default to ascending order
        m_nSortColumn = nClickedColumn;
        m_bSortAscending = true;
    }

    try
    {
        // Add a debug statement
        TRACE(L"Column clicked: %d, Sort Order: %s\n", nClickedColumn, m_bSortAscending ? L"Ascending" : L"Descending");

        // Sort the contact list based on the clicked column and order
        SortContactList(m_nSortColumn, m_bSortAscending);
    }
    catch (CException* e)
    {
        // Handle the exception (e.g., display a message)
        e->ReportError();
        e->Delete();
    }

    *pResult = 0;
}
  int CALLBACK MainDlg::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
      MainDlg* pThis = reinterpret_cast<MainDlg*>(lParamSort);
      int nSortColumn = LOWORD(lParamSort);
      bool bSortAscending = HIWORD(lParamSort) != 0;

      // Extract the item data
      int itemIndex1 = static_cast<int>(lParam1);
      int itemIndex2 = static_cast<int>(lParam2);

      // Check if indices are valid
      if (itemIndex1 < 0 || itemIndex2 < 0)
          return 0;

      // Get the column values for the items
      CString strItem1 = pThis->m_contactList.GetItemText(itemIndex1, nSortColumn);
      CString strItem2 = pThis->m_contactList.GetItemText(itemIndex2, nSortColumn);

      // Implement your sorting logic based on the column values
      int result = strItem1.CompareNoCase(strItem2);

      // Adjust the result based on the sorting order
      return bSortAscending ? result : -result;
}
void MainDlg::SortContactList(int nSortColumn, bool bSortAscending)
{
    m_nSortColumn = nSortColumn;
    m_bSortAscending = bSortAscending;

    // Set the sorting order and column for the CompareItems callback
    LPARAM lParamSort = MAKELPARAM(nSortColumn, bSortAscending);

    // Sort the contact list based on the clicked column and order
    m_contactList.SortItems(CompareItems, lParamSort);
}

void MainDlg::OnLvnItemchangedContactList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    // Check if the state of the item has changed
    if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState != pNMLV->uOldState))
    {
        // Handle checkbox (favorite) changes here
        int itemIndex = pNMLV->iItem;
        BOOL isFavorite = m_contactList.GetCheck(itemIndex);

        // Update the IsFavorite status in the database
        UpdateFavoriteStatusInDatabase(itemIndex, isFavorite);
    }

    *pResult = 0;
}

void MainDlg::UpdateFavoriteStatusInDatabase(int itemIndex, BOOL isFavorite)
{
    // Get the ContactID for the selected item
    int contactID = _wtoi(m_contactList.GetItemText(itemIndex, 0));

    // Update the IsFavorite status in the database
    CString query;
    query.Format(L"UPDATE contacts SET IsFavorite = %d WHERE ContactID = %d", isFavorite, contactID);

    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        TRACE(L"Error initializing MySQL connection.\n");
        MessageBox(L"Error initializing MySQL connection.", L"Error", MB_ICONERROR);
        return;
    }

    if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
    {
        TRACE(L"Error connecting to MySQL server: %s\n", mysql_error(con));
        MessageBox(L"Error connecting to MySQL server.", L"Error", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    if (mysql_query(con, "USE appcontacte"))
    {
        TRACE(L"Error selecting database: %s\n", mysql_error(con));
        MessageBox(L"Error selecting database.", L"Error", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    if (mysql_query(con, CStringA(query)))
    {
        TRACE(L"Error executing query: %s\n", mysql_error(con));
        MessageBox(L"Error executing query.", L"Error", MB_ICONERROR);
        mysql_close(con);
        return;
    }

    // Clean up resources
    mysql_close(con);
}

void MainDlg::OnBnClickedExpcsv()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlg(FALSE, L"csv", L"Contacts.csv", OFN_OVERWRITEPROMPT, L"CSV Files (*.csv)|*.csv||", this);

    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();
        ExportContactListToCSV(filePath);
    }
}
void MainDlg::ExportContactListToCSV(const CString& filePath)
{
    CStdioFile csvFile;

    if (csvFile.Open(filePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        // Write CSV header
        csvFile.WriteString(L"Favorite,Name,Phone Number,Email,Group\n");

        // Write each contact to the CSV file
        int itemCount = m_contactList.GetItemCount();

        for (int i = 0; i < itemCount; ++i)
        {
            CString line;
            line.Format(L"%d,%s,%s,%s,%s\n",
                m_contactList.GetCheck(i) ? 1 : 0,
                m_contactList.GetItemText(i, 1),
                m_contactList.GetItemText(i, 2),
                m_contactList.GetItemText(i, 3),
                m_contactList.GetItemText(i, 4));

            csvFile.WriteString(line);
        }

        AfxMessageBox(L"Contact list exported successfully!", MB_ICONINFORMATION);
        csvFile.Close();
    }
    else
    {
        AfxMessageBox(L"Failed to open the CSV file for writing!", MB_ICONERROR);
    }
}

void MainDlg::OnBnClickedImpcsv()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlg(TRUE, L"csv", L"", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, L"CSV Files (*.csv)|*.csv||", this);

    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();
        ImportCSVToDatabase(filePath);
    }
}
void MainDlg::ImportCSVToDatabase(const CString& filePath)
{
    CStdioFile csvFile;
    if (csvFile.Open(filePath, CFile::modeRead | CFile::typeText))
    {
        CString line;
        // Skip the header line
        csvFile.ReadString(line);

        MYSQL* con = mysql_init(NULL);

        if (con == NULL)
        {
            // Handle error initializing MySQL connection
            AfxMessageBox(L"Error initializing MySQL connection.", MB_ICONERROR);
            return;
        }

        if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
        {
            // Handle error connecting to MySQL server
            AfxMessageBox(L"Error connecting to MySQL server.", MB_ICONERROR);
            mysql_close(con);
            return;
        }

        if (mysql_query(con, "USE appcontacte"))
        {
            // Handle error selecting database
            AfxMessageBox(L"Error selecting database.", MB_ICONERROR);
            mysql_close(con);
            return;
        }

        // Loop through CSV lines
        while (csvFile.ReadString(line))
        {
            CStringArray fields;
            int count = ParseCSVLine(line, fields);

            if (count >= 4) // Ensure there are at least 4 fields (name, phone, email, group)
            {
                CString name = fields[1];
                CString phoneNumber = fields[2];
                CString email = fields[3];

                // Check for duplicates
                if (!IsDuplicateContact(email, phoneNumber))
                {
                    // Add the contact to the database
                    CString insertQuery;
                    insertQuery.Format(L"INSERT INTO contacts (UserID, Name, PhoneNumber, Email) VALUES ((SELECT UserID FROM users WHERE Username = '%s'), '%s', '%s', '%s')",
                        m_LoggedInUsername, name, phoneNumber, email);

                    if (mysql_query(con, CStringA(insertQuery.GetString())))
                    {
                        // Handle error executing query
                        AfxMessageBox(L"Error executing query.", MB_ICONERROR);
                        mysql_close(con);
                        csvFile.Close();
                        return;
                    }
                }
                else
                {
                    // Handle duplicate (e.g., log the issue, display a message)
                    TRACE(L"Duplicate contact found: Name=%s, Phone=%s, Email=%s\n", name, phoneNumber, email);
                    AfxMessageBox(L"Duplicate contact found. Check output window for details.", MB_ICONWARNING);
                }
            }
            else
            {
                // Handle invalid CSV format
                TRACE(L"Invalid CSV line: %s\n", line);
            }
        }

        // Clean up resources
        mysql_close(con);
        csvFile.Close();

        // Now that data is imported, populate the list control
        PopulateContactList();
    }
    else
    {
        // Handle error opening CSV file
        AfxMessageBox(L"Failed to open the CSV file for reading!", MB_ICONERROR);
    }
}
int MainDlg::ParseCSVLine(const CString& line, CStringArray& fields)
{
    // Implement CSV line parsing logic here
    // Split the line into fields based on commas or other delimiters
    // You can use CString::Tokenize or other methods for parsing

    // Example:
    fields.RemoveAll();
    CString delimiter = L",";
    int start = 0;
    int end = line.Find(delimiter);

    while (end != -1)
    {
        CString field = line.Mid(start, end - start);
        fields.Add(field);
        start = end + delimiter.GetLength();
        end = line.Find(delimiter, start);
    }

    // Add the last field
    CString lastField = line.Mid(start);
    fields.Add(lastField);

    return fields.GetSize();
}
bool MainDlg::IsDuplicateContact(const CString& email, const CString& phoneNumber)
{
    // Check if the contact with the given email or phone number already exists in the database
    // You may need to modify the SQL query based on your database schema
    CString query;
    query.Format(L"SELECT COUNT(*) FROM contacts WHERE UserID = (SELECT UserID FROM users WHERE Username = '%s') AND (Email = '%s' OR PhoneNumber = '%s')",
        m_LoggedInUsername, email, phoneNumber);

    // Execute the query to check for duplicates
    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        // Handle error initializing MySQL connection
        return false;
    }

    if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
    {
        // Handle error connecting to MySQL server
        mysql_close(con);
        return false;
    }

    if (mysql_query(con, "USE appcontacte"))
    {
        // Handle error selecting database
        mysql_close(con);
        return false;
    }

    if (mysql_query(con, CStringA(query.GetString())))
    {
        // Handle error executing query
        mysql_close(con);
        return false;
    }

    MYSQL_RES* res = mysql_store_result(con);

    if (res == NULL)
    {
        // Handle error storing query result
        mysql_close(con);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    int count = atoi(row[0]);

    // Clean up resources
    mysql_free_result(res);
    mysql_close(con);

    // If count is greater than 0, a duplicate contact exists
    return count > 0;
}


void MainDlg::OnBnClickedAddmailbutton()
{
    AddEmailDlg addEmailDlg(this, m_LoggedInUsername);

    // Open the AddEmailDlg dialog
    addEmailDlg.DoModal();
}


void MainDlg::OnBnClickedMinimizemain()
{
    // TODO: Add your control notification handler code here
    ShowWindow(SW_MINIMIZE);
}



void MainDlg::OnBnClickedButton1()
{
    // TODO: Add your control notification handler code here
    AddContactDlg addContactDlg(this, m_LoggedInUsername);

    // Open the AddContactDlg dialog
    if (addContactDlg.DoModal() == IDOK)
    {
        // Dialog was closed successfully (e.g., contact was added), you can handle additional logic here if needed
        // For example, you might want to refresh the contact list in the main dialog.
        PopulateContactList(); // Assuming PopulateContactList is a function to refresh the contact list
    }
}


void MainDlg::OnBnClickedDelcontact()
{
    POSITION pos = m_contactList.GetFirstSelectedItemPosition();

    if (pos != NULL)
    {
        int itemIndex = m_contactList.GetNextSelectedItem(pos);
        int contactID = _wtoi(m_contactList.GetItemText(itemIndex, 0));

        // Open a confirmation dialog before deleting the contact
        if (AfxMessageBox(L"Are you sure you want to delete this contact?", MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            // Delete the contact from the database
            if (DeleteContactFromDatabase(contactID))
            {

                // Select another item (if available) to ensure the list control has a valid selection
                int itemCount = m_contactList.GetItemCount();
                if (itemCount > 0)
                {
                    int newSelection = (itemIndex < itemCount - 1) ? itemIndex : itemIndex - 1;
                    m_contactList.SetItemState(newSelection, LVIS_SELECTED, LVIS_SELECTED);
                }

                // Display success message
                AfxMessageBox(L"Contact deleted successfully!", MB_ICONINFORMATION);
                PopulateContactList();
            }
            else
            {
                // Display error message if deletion from the database fails
                AfxMessageBox(L"Failed to delete contact from the database.", MB_ICONERROR);
            }
        }
    }
    else
    {
        // No item selected, display a message
        AfxMessageBox(L"Please select a contact to delete.", MB_ICONWARNING);
    }
}

bool MainDlg::DeleteContactFromDatabase(int contactID)
{
    // Initialize MySQL library
    MYSQL* con = mysql_init(NULL);

    if (con == NULL)
    {
        TRACE(L"Error initializing MySQL connection.\n");
        MessageBox(L"Error initializing MySQL connection.", L"Error", MB_ICONERROR);
        return false;
    }

    // Replace 'localhost', 'root', 'root', and 'appcontacte' with your actual database credentials
    if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL)
    {
        TRACE(L"Error connecting to MySQL server: %s\n", mysql_error(con));
        MessageBox(L"Error connecting to MySQL server.", L"Error", MB_ICONERROR);
        mysql_close(con);
        return false;
    }

    // Use the database
    if (mysql_query(con, "USE appcontacte"))
    {
        TRACE(L"Error selecting database: %s\n", mysql_error(con));
        MessageBox(L"Error selecting database.", L"Error", MB_ICONERROR);
        mysql_close(con);
        return false;
    }

    // Construct the SQL DELETE statement
    char query[256];
    int result = snprintf(query, sizeof(query), "DELETE FROM contacts WHERE ContactID = %d", contactID);

    // Check for buffer overflow or other errors
    if (result < 0 || result >= sizeof(query))
    {
        TRACE(L"Error constructing delete query.\n");
        MessageBox(L"Error constructing delete query.", L"Error", MB_ICONERROR);
        mysql_close(con);
        return false;
    }

    // Execute the delete command
    if (mysql_query(con, query) != 0)
    {
        TRACE(L"Error executing delete query: %s\n", mysql_error(con));
        MessageBox(L"Failed to execute delete query.", L"Error", MB_ICONERROR);
        mysql_close(con);
        return false;
    }

    // Clean up resources
    mysql_close(con);


    return true;
}
void MainDlg::OnBnClickedEditcontact()
{
    POSITION pos = m_contactList.GetFirstSelectedItemPosition();

    if (pos != NULL)
    {
        int itemIndex = m_contactList.GetNextSelectedItem(pos);

        CString name, phoneNumber, email;
        int contactID = 0;
        if (GetContactInformation(itemIndex, name, phoneNumber, email, contactID))
        {

            // Pass the contact information and contact ID to the EditContactDlg
            EditContactDlg editContactDlg(nullptr, contactID);
            editContactDlg.m_EditCName = name;
            editContactDlg.EditCNumber = phoneNumber;
            editContactDlg.EditCEmail = email;

            if (editContactDlg.DoModal() == IDOK)
            {
                // Handle any logic after editing the contact if needed
                // For example, you might want to refresh the contact list in the main dialog.
                PopulateContactList(); // Assuming PopulateContactList is a function to refresh the contact list
            }
        }
    }
    else
    {
        // No item selected, display a message or perform other handling as needed
        AfxMessageBox(L"Please select a contact to edit.", MB_ICONWARNING);
    }
}
bool MainDlg::GetContactInformation(int itemIndex, CString& name, CString& phoneNumber, CString& email, int& contactID)
{
    // Check if the indices are valid
    if (itemIndex < 0 || itemIndex >= m_contactList.GetItemCount())
        return false;

    // Get the contact information from the selected item
    contactID = _wtoi(m_contactList.GetItemText(itemIndex, 0));
    name = m_contactList.GetItemText(itemIndex, 1);
    phoneNumber = m_contactList.GetItemText(itemIndex, 2);
    email = m_contactList.GetItemText(itemIndex, 3);

    return true;
}



void MainDlg::OnBnClickedReminders()
{
    int userID = GetUserIDFromDatabase(m_LoggedInUsername);

    if (userID != -1)
    {
        // Open the ReminderDlg dialog and pass the username
        ReminderDlg reminderDlg(this, userID);

        // Call DoModal to display the dialog as a modal dialog
        reminderDlg.DoModal();
    }
    else
    {
        // Handle the case where user ID retrieval fails
        TRACE(L"Failed to retrieve user ID for username: %s\n", m_LoggedInUsername);
        AfxMessageBox(L"Failed to retrieve user ID for the logged-in user.", MB_ICONERROR);
    }
}
int MainDlg::GetUserIDFromDatabase(const CString& username)
{
    // Initialize MySQL library
    MYSQL* con = mysql_init(NULL);

    // Check if MySQL initialization is successful
    if (con == NULL)
    {
        TRACE(L"Error initializing MySQL connection.\n");
        AfxMessageBox(L"Error initializing MySQL connection.", MB_ICONERROR);
        return -1;
    }

    // Convert CString to CStringA for MySQL functions
    CStringA usernameA(username);

    // Construct the SQL query to get UserID
    CStringA query;
    query.Format("SELECT UserID FROM users WHERE Username='%s'", usernameA.GetString());

    // Execute the query
    if (mysql_real_connect(con, "localhost", "root", "root", "appcontacte", 0, NULL, 0) == NULL)
    {
        TRACE(L"Error connecting to MySQL server: %s\n", mysql_error(con));
        AfxMessageBox(L"Error connecting to MySQL server.", MB_ICONERROR);
        mysql_close(con);
        return -1;
    }

    if (mysql_query(con, "USE appcontacte"))
    {
        TRACE(L"Error selecting database: %s\n", mysql_error(con));
        AfxMessageBox(L"Error selecting database.", MB_ICONERROR);
        mysql_close(con);
        return -1;
    }

    if (mysql_query(con, query))
    {
        TRACE(L"Error executing query to get UserID: %s\n", mysql_error(con));
        AfxMessageBox(_T("An error occurred while retrieving UserID. Please try again later."));
        mysql_close(con);
        return -1;
    }

    // Store the query result
    MYSQL_RES* res = mysql_store_result(con);
    if (res == NULL)
    {
        TRACE(L"Error storing query result: %s\n", mysql_error(con));
        AfxMessageBox(_T("An error occurred while retrieving UserID. Please try again later."));
        mysql_close(con);
        return -1;
    }

    // Fetch the row
    MYSQL_ROW row;
    int userID = -1;

    if ((row = mysql_fetch_row(res)) != NULL) {
        // Extract UserID from the result
        userID = atoi(row[0]);
    }
    else {
        // No result found
        AfxMessageBox(_T("No UserID found for the specified username."), MB_ICONINFORMATION);
    }

    // Clean up resources
    mysql_free_result(res);
    mysql_close(con);

    return userID;
}

