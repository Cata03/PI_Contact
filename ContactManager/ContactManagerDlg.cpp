
// ContactManagerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ContactManager.h"
#include "ContactManagerDlg.h"
#include "afxdialogex.h"
#include "mysql.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CContactManagerDlg dialog



CContactManagerDlg::CContactManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONTACTMANAGER_DIALOG, pParent)
	, m_strUsername(_T(""))
	, m_strPassword(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CContactManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_USERNAME_EDIT, m_strUsername);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_strPassword);
}

BEGIN_MESSAGE_MAP(CContactManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOGIN_BUTTON, &CContactManagerDlg::OnBnClickedLoginButton)
END_MESSAGE_MAP()


// CContactManagerDlg message handlers

BOOL CContactManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CContactManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CContactManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CContactManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CContactManagerDlg::OnBnClickedLoginButton()
{
    // Retrieve entered username and password
    UpdateData(TRUE); // Update controls' data to variables

    try {
        MYSQL* con = mysql_init(NULL);

        if (con == NULL)
        {
            fprintf(stderr, "Error initializing MySQL connection: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred while initializing the connection. Please try again later."));
            return;
        }

        if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
        {
            fprintf(stderr, "Error connecting to MySQL server: %s\n", mysql_error(con));
            AfxMessageBox(_T("Failed to connect to the MySQL server. Please check your connection and try again."));
            mysql_close(con);
            return;
        }

        if (mysql_query(con, "USE appcontacte"))
        {
            fprintf(stderr, "Error selecting database: %s\n", mysql_error(con));
            AfxMessageBox(_T("Failed to select the database. Please try again later."));
            mysql_close(con);
            return;
        }

        // Build the login query using the entered username and password
        CStringA usernameA(m_strUsername);
        CStringA passwordA(m_strPassword);
        CStringA query;
        query.Format("SELECT * FROM users WHERE Username='%s' AND PasswordHash='%s'",
            usernameA.GetString(), passwordA.GetString());

        // Debug output
        CString debugMsg;
        debugMsg.Format(_T("Username: %s, PasswordHash: %s"), m_strUsername, m_strPassword);
        AfxMessageBox(debugMsg);

        // Execute the login query
        if (mysql_real_query(con, query, static_cast<unsigned long>(query.GetLength())))
        {
            fprintf(stderr, "Error executing login query: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred during login. Please try again later."));
            mysql_close(con);
            return;
        }

        MYSQL_RES* res = mysql_store_result(con);
        if (res == NULL)
        {
            fprintf(stderr, "Error storing login query result: %s\n", mysql_error(con));
            AfxMessageBox(_T("An error occurred during login. Please try again later."));
            mysql_close(con);
            return;
        }

        int num_fields = mysql_num_fields(res);
        MYSQL_ROW row;

        // Check if there is at least one result
        if ((row = mysql_fetch_row(res)) != NULL) {
            // Successful login
            AfxMessageBox(_T("Login successful!"));

            // Optionally, close the login dialog
            EndDialog(IDOK);

            // Open the main page or switch to a new dialog
            // CMainPageDlg mainPageDlg;
            // mainPageDlg.DoModal();
        }
        else {
            // Failed login
            AfxMessageBox(_T("Invalid credentials. Please try again."));
        }

        // Clean up resources
        mysql_free_result(res);
        mysql_close(con);
    }
    catch (...) {
        // Handle any other errors
        AfxMessageBox(_T("An unexpected error occurred. Please try again later."));
    }
}
