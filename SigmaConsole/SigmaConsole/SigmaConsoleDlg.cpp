// ----------------------------------------------------------------------------------------------
// SigmaGraph
// Data Plotting and Analysis Software
// Copyright(C) 1997-2020  Pr. Sidi HAMADY
// http://www.hamady.org
// sidi@hamady.org
// Released under the MIT License:
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
// AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------------------------

// :TODO: complete doc and translate to english

#include "stdafx.h"

#include "SigmaConsole.h"
#include "SigmaConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    return TRUE;
}
// CSigmaConsoleDlg dialog


// App command to run the dialog
void CSigmaConsoleDlg::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}


CSigmaConsoleDlg::CSigmaConsoleDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSigmaConsoleDlg::IDD, pParent)
{
    m_bEditInitialized = FALSE;
    m_bInitialized = FALSE;
    m_bTopmost = FALSE;
    m_hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);

    m_pNumPadDlg = NULL;
    m_pVarlistDlg = NULL;
}

void CSigmaConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_CONSOLE, m_edtConsole);
}

BEGIN_MESSAGE_MAP(CSigmaConsoleDlg, CDialog)
    ON_COMMAND(ID_CONSOLE_RESET, &CSigmaConsoleDlg::OnConsoleReset)
    ON_COMMAND(ID_CONSOLE_FONT, &CSigmaConsoleDlg::OnConsoleFont)
    ON_COMMAND(ID_CONSOLE_FONTCOLOR, &CSigmaConsoleDlg::OnConsoleFontColor)
    ON_COMMAND(ID_CONSOLE_SAVEAS, &CSigmaConsoleDlg::OnConsoleSaveAs)
    ON_COMMAND(ID_CONSOLE_OPEN, &CSigmaConsoleDlg::OnConsoleOpen)
    ON_COMMAND(ID_CONSOLE_CUT, &CSigmaConsoleDlg::OnConsoleCut)
    ON_COMMAND(ID_CONSOLE_COPY, &CSigmaConsoleDlg::OnConsoleCopy)
    ON_COMMAND(ID_CONSOLE_PASTE, &CSigmaConsoleDlg::OnConsolePaste)
    ON_COMMAND(ID_PARSER_FUNC80, &CSigmaConsoleDlg::OnConsoleFunc80)
    ON_COMMAND(ID_PARSER_FUNC81, &CSigmaConsoleDlg::OnConsoleFunc81)
    ON_COMMAND(ID_PARSER_FUNC83, &CSigmaConsoleDlg::OnConsoleFunc83)
    ON_COMMAND(ID_PARSER_FUNC84, &CSigmaConsoleDlg::OnConsoleFunc84)
    ON_COMMAND(ID_PARSER_FUNC01, &CSigmaConsoleDlg::OnConsoleFunc01)
    ON_COMMAND(ID_PARSER_FUNC02, &CSigmaConsoleDlg::OnConsoleFunc02)
    ON_COMMAND(ID_PARSER_FUNC03, &CSigmaConsoleDlg::OnConsoleFunc03)
    ON_COMMAND(ID_PARSER_FUNC04, &CSigmaConsoleDlg::OnConsoleFunc04)
    ON_COMMAND(ID_PARSER_FUNC05, &CSigmaConsoleDlg::OnConsoleFunc05)
    ON_COMMAND(ID_PARSER_FUNC06, &CSigmaConsoleDlg::OnConsoleFunc06)
    ON_COMMAND(ID_PARSER_FUNC10, &CSigmaConsoleDlg::OnConsoleFunc10)
    ON_COMMAND(ID_PARSER_FUNC20, &CSigmaConsoleDlg::OnConsoleFunc20)
    ON_COMMAND(ID_PARSER_FUNC30, &CSigmaConsoleDlg::OnConsoleFunc30)
    ON_COMMAND(ID_PARSER_CONST01, &CSigmaConsoleDlg::OnConsoleConst01)
    ON_COMMAND(ID_PARSER_CONST10, &CSigmaConsoleDlg::OnConsoleConst10)
    ON_COMMAND(ID_PARSER_CONST11, &CSigmaConsoleDlg::OnConsoleConst11)
    ON_COMMAND(ID_PARSER_CONST12, &CSigmaConsoleDlg::OnConsoleConst12)
    ON_COMMAND(ID_PARSER_CONST13, &CSigmaConsoleDlg::OnConsoleConst13)
    ON_COMMAND(ID_PARSER_CONST14, &CSigmaConsoleDlg::OnConsoleConst14)
    ON_COMMAND(ID_PARSER_CONST15, &CSigmaConsoleDlg::OnConsoleConst15)
    ON_COMMAND(ID_PARSER_CONST16, &CSigmaConsoleDlg::OnConsoleConst16)
    ON_COMMAND(ID_PARSER_RAD, &CSigmaConsoleDlg::OnConsoleRad)
    ON_COMMAND(ID_PARSER_DEG, &CSigmaConsoleDlg::OnConsoleDeg)
    ON_COMMAND(ID_PARSER_GRAD, &CSigmaConsoleDlg::OnConsoleGrad)
    ON_COMMAND(ID_CONSOLE_HELP, &CSigmaConsoleDlg::OnConsoleHelp)
    ON_COMMAND(ID_APP_ABOUT, &CSigmaConsoleDlg::OnAppAbout)
    ON_COMMAND(ID_OPTIONS_TOPMOST, &CSigmaConsoleDlg::OnOptionsTopmost)
    ON_COMMAND(ID_CONSOLE_CLOSE, &CSigmaConsoleDlg::OnConsoleClose)
    ON_COMMAND(ID_VIEW_INPUT, &CSigmaConsoleDlg::OnViewInput)
    ON_COMMAND(ID_VIEW_VARLIST, &CSigmaConsoleDlg::OnViewVarlist)
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSigmaConsoleDlg message handlers

BOOL CSigmaConsoleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
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
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here
    m_edtConsole.SetLimitText(ML_STRSIZEL);
    m_edtConsole.SetPromptText(_T("$ "));
    m_edtConsole.SetHistorySize(CONSOLE_HISTORY);
    m_edtConsole.SetSel(-1, -1);

    // >> History permanent
    char_t szLoc[ML_STRSIZE];
    char_t szLine[ML_STRSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    memset(szLine, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*) (SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("ConsoleHistory.par"));
    long_t ilen = (long_t)fl_filesize((const char_t*)szLoc);
    if ((ilen > 1) && (ilen <= (ML_STRSIZE * CONSOLE_HISTORY))) {
        FILE *fp = NULL;
        if ((fp = _wfopen((const char_t*)szLoc, _T("r"))) != NULL) {
            int_t nlines = 0, nLen = 0;
            m_edtConsole.m_nTotalCommand = 0;
            while(1) {
                if (fgetws(szLine, ML_STRSIZE - 1, fp) == NULL) {
                    break;
                }
                nLen = (int_t)_tcslen((LPCTSTR)szLine);
                if ((nLen > 1) && (nLen < ML_STRSIZE)) {
                    if ((szLine[nLen - 1] == _T('\n')) || (szLine[nLen - 1] == _T('\r'))) {
                        szLine[nLen - 1] = _T('\0');
                    }
                    if ((szLine[nLen - 2] == _T('\n')) || (szLine[nLen - 2] == _T('\r'))) {
                        szLine[nLen - 2] = _T('\0');
                    }
                    m_edtConsole.m_aCommandHistory[m_edtConsole.m_nTotalCommand] = (LPCTSTR)szLine;
                    m_edtConsole.m_nTotalCommand += 1;
                }
                nlines += 1;
                if (nlines > CONSOLE_HISTORY) {
                    break;
                }
            }
            fclose(fp);    fp = NULL;
        }
    }
    // <<

    // >> Get permanent
    LOGFONT lft;
    CFont* pFont = m_edtConsole.GetFont();
    if (pFont) {
        pFont->GetLogFont(&lft);
    }
    int_t iStyle = FW_NORMAL;
    BOOL bItalic = FALSE;
    if ((SigmaApp.m_Console.font.style & 0xF0) == 0x10) {
        iStyle = FW_BOLD;
    }
    if ((SigmaApp.m_Console.font.style & 0x0F) == 0x01) {
        bItalic = TRUE;
    }
    HDC hDCX = ::GetDC(NULL);
    if (hDCX) {
        m_Font.CreateFont(-MulDiv(SigmaApp.m_Console.font.size, ::GetDeviceCaps(hDCX, LOGPIXELSY), 72), 0, 0, 0, iStyle, bItalic, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH|FF_SWISS, (const char_t*)(SigmaApp.m_Console.font.name));
        ::ReleaseDC(NULL, hDCX);
    }
    // <<

    m_edtConsole.SetFont(&m_Font);

    m_Color = RGB(SigmaApp.m_Console.font.color.r, SigmaApp.m_Console.font.color.g, SigmaApp.m_Console.font.color.b);

    CMenu* pMenu = GetMenu();
    if (pMenu) {
        if (ml_angleunit == 0x00) {
            pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_CHECKED);
            pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
        }
        else if (ml_angleunit == 0x01) {
            pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_CHECKED);
            pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
        }
        else if (ml_angleunit == 0x02) {
            pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_CHECKED);
        }
    }

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    m_bInitialized = TRUE;

    UpdateData(FALSE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSigmaConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSigmaConsoleDlg::OnPaint()
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
        CDialog::OnPaint();
        if (m_bEditInitialized == FALSE) {
            m_edtConsole.SetSel(-1,-1);
            m_bEditInitialized = TRUE;
        }
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSigmaConsoleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CSigmaConsoleDlg::OnConsoleReset()
{
    m_edtConsole.m_nCurCommand = m_edtConsole.m_nTotalCommand = 0;
    m_edtConsole.m_aCommandHistory.RemoveAll();
    m_edtConsole.SetHistorySize(CONSOLE_HISTORY);
    m_edtConsole.EmptyUndoBuffer();
    m_edtConsole.SetWindowText(_T(""));
    m_edtConsole.SetWindowText((LPCTSTR)(m_edtConsole.m_strPromptText));
    m_edtConsole.SetSel(m_edtConsole.m_strPromptText.GetLength(), -1);

    // Parser
    SigmaApp.m_nVarnum = 0;
    const char_t *szAZ[_T('z') - _T('a') + 1] = {_T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("h"), _T("i"), _T("j"), _T("k"), _T("l"), _T("m"),
        _T("n"), _T("o"), _T("p"), _T("q"), _T("r"), _T("s"), _T("t"), _T("u"), _T("v"), _T("w"), _T("x"), _T("y"), _T("z")};

    for (int_t ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {
        SigmaApp.m_fParam[ii] = 0.0;
        _tcscpy(SigmaApp.m_szParam[ii], szAZ[ii]);        // Params name : by default : a, b, c, ..., z
        SigmaApp.m_bParam[ii] = FALSE;                        // Param set ?
    }
}

void CSigmaConsoleDlg::OnConsoleFont()
{
    LOGFONT lft;
    memset(&lft, 0, sizeof(lft));
    m_Font.GetLogFont(&lft);
    if (SigmaApp.GetFont(m_hWnd, &lft)) {
        m_Font.DeleteObject();
        // >> Set permanent
        SigmaApp.LogfontToFont(lft, &(SigmaApp.m_Console.font));
        // <<
        m_Font.CreateFontIndirect(&lft);
        m_edtConsole.SetFont(&m_Font);
    }
}

void CSigmaConsoleDlg::OnConsoleFontColor()
{
    COLORREF clr = m_Color;
    if (SigmaApp.GetColor(m_hWnd, &clr)) {
        m_Color = clr;
        SigmaApp.m_Console.font.color.r = GetRValue(clr);
        SigmaApp.m_Console.font.color.g = GetGValue(clr);
        SigmaApp.m_Console.font.color.b = GetBValue(clr);
    }
}

HBRUSH CSigmaConsoleDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->m_hWnd == m_edtConsole.m_hWnd) {
        pDC->SetTextColor(m_Color);
    }

    return hbr;
}

void CSigmaConsoleDlg::OnConsoleSaveAs()
{
    int_t nLines = m_edtConsole.GetLineCount();
    if ((nLines < 1) || (nLines > CONSOLE_MAXLINE)) {
        SigmaApp.Output(_T("Couldn't save console content.\r\n"), SIGMA_OUTPUT_WARNING);
    }

    char_t szFilters[] = _T("SigmaConsole Files (*.txt)|*.txt||");

    CFileDialog fileDlg(FALSE, _T("txt"), _T("*.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        szFilters, this);

    if (fileDlg.DoModal() != IDOK) {
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }

    CString fileName = fileDlg.GetPathName();
    CString strT;

    CStdioFile fileT;
    try {
        CFileException excT;
        if (!fileT.Open((LPCTSTR)fileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &excT)) {
            char_t szError[ML_STRSIZEW];
            excT.GetErrorMessage(szError, ML_STRSIZEW);
            // >> OUTPUT
            strT.Format(_T("Couldn't save console file: %s\r\n"), szError);
            SigmaApp.Output(strT, SIGMA_OUTPUT_ERROR);
            // <<
            m_edtConsole.SetFocus();
            m_edtConsole.SetSel(-1, -1);
            return;
        }
        int_t ii = 0, nLength = 0;
        char_t szBuffer[ML_STRSIZE];
        memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
        for (ii = 0; ii < nLines; ii++) {
            nLength = m_edtConsole.GetLine(ii, (char_t*)szBuffer, ML_STRSIZE - 1);
            if ((nLength > 2) && (nLength <= (ML_STRSIZE - 1))) {
                szBuffer[nLength] = _T('\0');
                fileT.WriteString((const char_t*)szBuffer);
                fileT.WriteString(_T("\r\n"));
            }
        }
        fileT.Close();

        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
    catch (...) {
        fileT.Close();
        // >> OUTPUT
        SigmaApp.Output(_T("Couldn't save console content.\r\n"), SIGMA_OUTPUT_WARNING);
        // <<
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
}

void CSigmaConsoleDlg::OnConsoleOpen()
{
    char_t szFilters[] = _T("SigmaConsole Files (*.txt)|*.txt||");

    CFileDialog fileDlg(TRUE, _T("txt"), _T("*.txt"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        szFilters, this);

    if (fileDlg.DoModal() != IDOK) {
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }

    CString fileName = fileDlg.GetPathName();
    CString strT;

    CStdioFile fileT;
    try {
        CFileException excT;
        if (!fileT.Open((LPCTSTR)fileName, CFile::modeRead | CFile::typeText | CFile::shareExclusive, &excT)) {
            char_t szError[ML_STRSIZEW];
            excT.GetErrorMessage(szError, ML_STRSIZEW);
            // >> OUTPUT
            strT.Format(_T("Couldn't read console file: %s\r\n"), szError);
            SigmaApp.Output(strT, SIGMA_OUTPUT_WARNING);
            // <<
            m_edtConsole.SetFocus();
            m_edtConsole.SetSel(-1, -1);
            return;
        }
        // Nead Confirmation
        OnConsoleReset();
        int_t nLen = (int_t)(fileT.GetLength());
        if ((nLen < 5) || (nLen > (ML_STRSIZE * CONSOLE_MAXLINE))) {
            // >> OUTPUT
            fileT.Close();
            SigmaApp.Output(_T("Couldn't read console file: invalid size.\r\n"), SIGMA_OUTPUT_WARNING);
            // <<
            m_edtConsole.SetFocus();
            m_edtConsole.SetSel(-1, -1);
            return;
        }
        int_t ii = 0, nLength;
        char_t *pszT = NULL;
        for (;;) {
            if (fileT.ReadString(m_edtConsole.m_szCommand, ML_STRSIZE - 1) == NULL) {
                break;
            }
            ii += 1;
            nLength = (int_t)_tcslen((const char_t*)(m_edtConsole.m_szCommand));
            if ((ii > CONSOLE_MAXLINE) || (nLength < 2) || (nLength >= ML_STRSIZE)) {
                break;
            }
            if ((m_edtConsole.m_szCommand[nLength - 1] == _T('\n')) || (m_edtConsole.m_szCommand[nLength - 1] == _T('\r'))) {
                m_edtConsole.m_szCommand[nLength - 1] = _T('\0');
            }
            if ((m_edtConsole.m_szCommand[nLength - 2] == _T('\n')) || (m_edtConsole.m_szCommand[nLength - 2] == _T('\r'))) {
                m_edtConsole.m_szCommand[nLength - 2] = _T('\0');
            }
            if (m_edtConsole.m_szCommand[0] == m_edtConsole.m_strPromptText[0]) {
                pszT = &(m_edtConsole.m_szCommand[m_edtConsole.m_strPromptText.GetLength()]);
                m_edtConsole.AppendText((const char_t*)pszT);
                m_edtConsole.DoParse(m_edtConsole.m_strPromptText.GetLength());
                m_edtConsole.AppendText(_T("\r\n"));
                m_edtConsole.AppendText(m_edtConsole.m_strPromptText);
            }
        }
        fileT.Close();

        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
    catch (...) {
        fileT.Close();
        // >> OUTPUT
        SigmaApp.Output(_T("Couldn't read console file.\r\n"), SIGMA_OUTPUT_WARNING);
        // <<
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
}

inline void CSigmaConsoleDlg::InsertText(const char_t *szText, int_t nRelPos)
{
    if (!m_edtConsole.IsEditingLine() || m_edtConsole.IsPromptArea(0)) {
        return;
    }
    int_t nLine = 0, nPos = 0;
    m_edtConsole.ReplaceSel(szText);
    if (nRelPos != 0) {
        m_edtConsole.GetCaretPos(nLine, nPos);
        m_edtConsole.SetCaretPos(nPos - nRelPos);
    }
}

void CSigmaConsoleDlg::OnConsoleFunc80()
{
    InsertText(_T("sqrt()"));
}

void CSigmaConsoleDlg::OnConsoleFunc81()
{
    InsertText(_T("abs()"));
}

void CSigmaConsoleDlg::OnConsoleFunc83()
{
    InsertText(_T("ceil()"));
}

void CSigmaConsoleDlg::OnConsoleFunc84()
{
    InsertText(_T("int()"));
}

void CSigmaConsoleDlg::OnConsoleFunc01()
{
    InsertText(_T("sin()"));
}

void CSigmaConsoleDlg::OnConsoleFunc02()
{
    InsertText(_T("cos()"));
}

void CSigmaConsoleDlg::OnConsoleFunc03()
{
    InsertText(_T("tan()"));
}

void CSigmaConsoleDlg::OnConsoleFunc04()
{
    InsertText(_T("asin()"));
}

void CSigmaConsoleDlg::OnConsoleFunc05()
{
    InsertText(_T("acos()"));
}

void CSigmaConsoleDlg::OnConsoleFunc06()
{
    InsertText(_T("atan()"));
}

void CSigmaConsoleDlg::OnConsoleFunc10()
{
    InsertText(_T("ln()"));
}

void CSigmaConsoleDlg::OnConsoleFunc20()
{
    InsertText(_T("exp()"));
}

void CSigmaConsoleDlg::OnConsoleFunc30()
{
    InsertText(_T("pow(,)"), 2);
}

void CSigmaConsoleDlg::OnConsoleConst01()
{
    InsertText(_T("pi"), 0);
}

void CSigmaConsoleDlg::OnConsoleConst10()
{
    InsertText(_T("_q"), 0);
}

void CSigmaConsoleDlg::OnConsoleConst11()
{
    InsertText(_T("_m"), 0);
}

void CSigmaConsoleDlg::OnConsoleConst12()
{
    InsertText(_T("_k"), 0);
}

void CSigmaConsoleDlg::OnConsoleConst13()
{
    InsertText(_T("_h"), 0);
}

void CSigmaConsoleDlg::OnConsoleConst14()
{
    InsertText(_T("_c"), 0);
}

void CSigmaConsoleDlg::OnConsoleConst15()
{
    InsertText(_T("_e"), 0);
}

void CSigmaConsoleDlg::OnConsoleConst16()
{
    InsertText(_T("_n"), 0);
}

void CSigmaConsoleDlg::OnConsoleRad()
{
    ml_angleunit = 0x00;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
}

void CSigmaConsoleDlg::OnConsoleDeg()
{
    ml_angleunit = 0x01;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
}

void CSigmaConsoleDlg::OnConsoleGrad()
{
    ml_angleunit = 0x02;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_CHECKED);
    }
}

void CSigmaConsoleDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    if (m_bInitialized) {
        CRect recTT;
        GetClientRect(recTT);
        m_edtConsole.MoveWindow(recTT);
    }
}

void CSigmaConsoleDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH2;
    lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT2;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CSigmaConsoleDlg::OnConsoleCut()
{
    m_edtConsole.Cut();
}

void CSigmaConsoleDlg::OnConsoleCopy()
{
    m_edtConsole.Copy();
}

void CSigmaConsoleDlg::OnConsolePaste()
{
    m_edtConsole.Paste();
}

void CSigmaConsoleDlg::OnConsoleHelp()
{
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) SigmaApp.m_szHelpFilename, HH_DISPLAY_TOC, NULL);
}

void CSigmaConsoleDlg::OnClose()
{
    SigmaApp.setOptions();
    if (m_pNumPadDlg) {
        m_pNumPadDlg->DestroyWindow();
        delete m_pNumPadDlg;
        m_pNumPadDlg = NULL;
    }
    if (m_pVarlistDlg) {
        m_pVarlistDlg->DestroyWindow();
        delete m_pVarlistDlg;
        m_pVarlistDlg = NULL;
    }

    // >> History permanent
    char_t szLoc[ML_STRSIZE];
    char_t szLine[ML_STRSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    memset(szLine, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*) (SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("ConsoleHistory.par"));
    int_t ilen = (int_t)(m_edtConsole.m_nTotalCommand);
    if ((ilen > 1) && (ilen <= CONSOLE_HISTORY)) {
        FILE *fp = NULL;
        if ((fp = _wfopen((const char_t*)szLoc, _T("w"))) != NULL) {
            int_t jj;
            for (jj = 0; jj < ilen; jj++) {
                SigmaApp.CStringToChar(m_edtConsole.m_aCommandHistory[jj], szLine, ML_STRSIZE - 1);
                if (fputws((const char_t*)szLine, fp) == EOF) {
                    break;
                }
                fputws(_T("\n"), fp);
            }
            fclose(fp);    fp = NULL;
        }
    }
    else {
        ::DeleteFile((LPCTSTR)szLoc);
    }
    // <<

    CDialog::OnClose();
}

void CSigmaConsoleDlg::OnConsoleClose()
{
    SigmaApp.setOptions();
    if (m_pNumPadDlg) {
        m_pNumPadDlg->DestroyWindow();
        delete m_pNumPadDlg;
        m_pNumPadDlg = NULL;
    }
    if (m_pVarlistDlg) {
        m_pVarlistDlg->DestroyWindow();
        delete m_pVarlistDlg;
        m_pVarlistDlg = NULL;
    }
    CDialog::EndDialog(0);
}

void CSigmaConsoleDlg::OnOptionsTopmost()
{
    if (m_bTopmost == TRUE) {
        ::SetWindowPos(GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        m_bTopmost = FALSE;
    }
    else {
        ::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        m_bTopmost = TRUE;
    }

    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_OPTIONS_TOPMOST, (m_bTopmost == TRUE) ? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED));
    }
}

void CSigmaConsoleDlg::OnViewInput()
{
    if (m_pNumPadDlg != NULL) {
        if (m_pNumPadDlg->GetSafeHwnd() == NULL) {
            delete m_pNumPadDlg;
            m_pNumPadDlg = NULL;
        }
    }

    if (m_pNumPadDlg == NULL) {
        m_pNumPadDlg = new CNumPadDlg();
        if (m_pNumPadDlg) {
            m_pNumPadDlg->m_pConsoleEdit = &m_edtConsole;
            if (m_pNumPadDlg->Create(IDD_NUMPAD, this)) {
                RECT rcT = {0,0,0,0}, rcTT = {0,0,0,0};
                GetWindowRect(&rcT);
                m_pNumPadDlg->GetWindowRect(&rcTT);
                //rcT.bottom = rcT.top + (rcTT.bottom - rcTT.top);
                rcT.left = rcT.right - (rcTT.right - rcTT.left);
                m_pNumPadDlg->SetWindowPos(NULL, rcT.left, rcT.top + 48, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
        }
    }

    if (m_pNumPadDlg == NULL) {
        return;
    }

    if (m_pNumPadDlg) {
        m_pNumPadDlg->ShowWindow(SW_SHOW);
        m_pNumPadDlg->SetForegroundWindow();
    }
}

void CSigmaConsoleDlg::OnViewVarlist()
{
    if (m_pVarlistDlg != NULL) {
        if (m_pVarlistDlg->GetSafeHwnd() == NULL) {
            delete m_pVarlistDlg;
            m_pVarlistDlg = NULL;
        }
    }

    if (m_pVarlistDlg == NULL) {
        m_pVarlistDlg = new CVarlistDlg();
        if (m_pVarlistDlg) {
            if (m_pVarlistDlg->Create(IDD_VARLIST, this)) {
                RECT rcT = {0,0,0,0}, rcTT = {0,0,0,0};
                GetWindowRect(&rcT);
                m_pVarlistDlg->GetWindowRect(&rcTT);
                //rcT.bottom = rcT.top + (rcTT.bottom - rcTT.top);
                rcT.left = rcT.right - (rcTT.right - rcTT.left);
                m_pVarlistDlg->SetWindowPos(NULL, rcT.left, rcT.top + 64, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
        }
    }

    if (m_pVarlistDlg == NULL) {
        return;
    }

    if (m_pVarlistDlg) {
        m_pVarlistDlg->ShowWindow(SW_SHOW);
        m_pVarlistDlg->SetForegroundWindow();
        m_pVarlistDlg->UpdateVarlist();
    }
}

BOOL CSigmaConsoleDlg::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if (CTRL_Pressed()) {
                if ((pMsg->wParam == _T('s')) || (pMsg->wParam == _T('S'))) {
                    if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                        return TRUE;
                    }
                }
                if ((pMsg->wParam == _T('k')) || (pMsg->wParam == _T('K'))) {
                    if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                        return TRUE;
                    }
                }
            }
            else if ((pMsg->wParam == VK_F4)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
            else if ((pMsg->wParam == VK_F5)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CSigmaConsoleDlg::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CDialog::OnDestroy();
}