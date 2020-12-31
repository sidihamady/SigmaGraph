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
#include "Sigma.h"
#include "SigmaConsole.h"

#include "MainFrm.h"

// CSigmaConsole dialog

IMPLEMENT_DYNAMIC(CSigmaConsole, CDialog)

CSigmaConsole::CSigmaConsole(CWnd* pParent /*=NULL*/)
: CDialog(CSigmaConsole::IDD, pParent)
{
    m_bInitialized = FALSE;
    m_pNumPadDlg = NULL;
    m_pVarlistDlg = NULL;
    m_bTopmost = FALSE;
}

CSigmaConsole::~CSigmaConsole()
{
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
    m_Font.DeleteObject();
}

void CSigmaConsole::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_CONSOLE, m_edtConsole);
}


BEGIN_MESSAGE_MAP(CSigmaConsole, CDialog)
    ON_COMMAND(ID_CONSOLE_CLOSE, &CSigmaConsole::OnConsoleClose)
    ON_COMMAND(ID_CONSOLE_RESET, &CSigmaConsole::OnConsoleReset)
    ON_COMMAND(ID_CONSOLE_FONT, &CSigmaConsole::OnConsoleFont)
    ON_COMMAND(ID_CONSOLE_FONTCOLOR, &CSigmaConsole::OnConsoleFontColor)
    ON_COMMAND(ID_CONSOLE_SAVEAS, &CSigmaConsole::OnConsoleSaveAs)
    ON_COMMAND(ID_CONSOLE_OPEN, &CSigmaConsole::OnConsoleOpen)
    ON_COMMAND(ID_CONSOLE_CUT, &CSigmaConsole::OnConsoleCut)
    ON_COMMAND(ID_CONSOLE_COPY, &CSigmaConsole::OnConsoleCopy)
    ON_COMMAND(ID_CONSOLE_PASTE, &CSigmaConsole::OnConsolePaste)
    ON_COMMAND(ID_PARSER_FUNC80, &CSigmaConsole::OnConsoleFunc80)
    ON_COMMAND(ID_PARSER_FUNC81, &CSigmaConsole::OnConsoleFunc81)
    ON_COMMAND(ID_PARSER_FUNC83, &CSigmaConsole::OnConsoleFunc83)
    ON_COMMAND(ID_PARSER_FUNC84, &CSigmaConsole::OnConsoleFunc84)
    ON_COMMAND(ID_PARSER_FUNC01, &CSigmaConsole::OnConsoleFunc01)
    ON_COMMAND(ID_PARSER_FUNC02, &CSigmaConsole::OnConsoleFunc02)
    ON_COMMAND(ID_PARSER_FUNC03, &CSigmaConsole::OnConsoleFunc03)
    ON_COMMAND(ID_PARSER_FUNC04, &CSigmaConsole::OnConsoleFunc04)
    ON_COMMAND(ID_PARSER_FUNC05, &CSigmaConsole::OnConsoleFunc05)
    ON_COMMAND(ID_PARSER_FUNC06, &CSigmaConsole::OnConsoleFunc06)
    ON_COMMAND(ID_PARSER_FUNC10, &CSigmaConsole::OnConsoleFunc10)
    ON_COMMAND(ID_PARSER_FUNC20, &CSigmaConsole::OnConsoleFunc20)
    ON_COMMAND(ID_PARSER_FUNC30, &CSigmaConsole::OnConsoleFunc30)
    ON_COMMAND(ID_PARSER_CONST01, &CSigmaConsole::OnConsoleConst01)
    ON_COMMAND(ID_PARSER_CONST10, &CSigmaConsole::OnConsoleConst10)
    ON_COMMAND(ID_PARSER_CONST11, &CSigmaConsole::OnConsoleConst11)
    ON_COMMAND(ID_PARSER_CONST12, &CSigmaConsole::OnConsoleConst12)
    ON_COMMAND(ID_PARSER_CONST13, &CSigmaConsole::OnConsoleConst13)
    ON_COMMAND(ID_PARSER_CONST14, &CSigmaConsole::OnConsoleConst14)
    ON_COMMAND(ID_PARSER_CONST15, &CSigmaConsole::OnConsoleConst15)
    ON_COMMAND(ID_PARSER_CONST16, &CSigmaConsole::OnConsoleConst16)
    ON_COMMAND(ID_PARSER_RAD, &CSigmaConsole::OnConsoleRad)
    ON_COMMAND(ID_PARSER_DEG, &CSigmaConsole::OnConsoleDeg)
    ON_COMMAND(ID_PARSER_GRAD, &CSigmaConsole::OnConsoleGrad)
    ON_COMMAND(ID_VIEW_INPUT, &CSigmaConsole::OnViewInput)
    ON_COMMAND(ID_HELP_CONTENTS, &CSigmaConsole::OnHelpContents)
    ON_COMMAND(ID_CONSOLE_TOPMOST, &CSigmaConsole::OnConsoleTopmost)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CSigmaConsole::OnUpdateEditSelectAll)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CSigmaConsole::OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CSigmaConsole::OnUpdateEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CSigmaConsole::OnUpdateEditPaste)
    ON_UPDATE_COMMAND_UI(ID_CONSOLE_TOPMOST, &CSigmaConsole::OnUpdateConsoleTopmost)
    ON_COMMAND(ID_VIEW_VARLIST, &CSigmaConsole::OnViewVarlist)
    ON_WM_SYSCOMMAND()
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// CSigmaConsole message handlers
BOOL CSigmaConsole::OnInitDialog()
{
    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_CONSOLE), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

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
    long_t ilen = (long_t) fl_filesize((const char_t*) szLoc);
    if ((ilen > 1) && (ilen <= (ML_STRSIZE * CONSOLE_HISTORY))) {
        FILE *fp = NULL;
        if ((fp = _wfopen((const char_t*) szLoc, _T("r"))) != NULL) {
            int_t nlines = 0, nLen = 0;
            m_edtConsole.m_nTotalCommand = 0;
            while (1) {
                if (fgetws(szLine, ML_STRSIZE - 1, fp) == NULL) {
                    break;
                }
                nLen = (int_t) _tcslen((LPCTSTR) szLine);
                if ((nLen > 1) && (nLen < ML_STRSIZE)) {
                    if ((szLine[nLen - 1] == _T('\n')) || (szLine[nLen - 1] == _T('\r'))) {
                        szLine[nLen - 1] = _T('\0');
                    }
                    if ((szLine[nLen - 2] == _T('\n')) || (szLine[nLen - 2] == _T('\r'))) {
                        szLine[nLen - 2] = _T('\0');
                    }
                    m_edtConsole.m_aCommandHistory[m_edtConsole.m_nTotalCommand] = (LPCTSTR) szLine;
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
    BOOL bItalic = FALSE, bUnderline = FALSE;
    if ((SigmaApp.m_Options.console.font.style & 0xF0) == 0x10) {
        iStyle = FW_BOLD;
    }
    if ((SigmaApp.m_Options.console.font.style & 0x01) != 0x00) {
        bItalic = TRUE;
    }
    if ((SigmaApp.m_Options.console.font.style & 0x02) != 0x00) {
        bUnderline = TRUE;
    }
    HDC hDCX = ::GetDC(NULL);
    if (hDCX) {
        m_Font.CreateFont(-MulDiv(SigmaApp.m_Options.console.font.size, ::GetDeviceCaps(hDCX, LOGPIXELSY), 72), 0, 0, 0, iStyle, bItalic, bUnderline, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH | FF_SWISS, (const char_t*) (SigmaApp.m_Options.console.font.name));
        ::ReleaseDC(NULL, hDCX);
        hDCX = NULL;
    }
    // <<

    m_edtConsole.SetFont(&m_Font);

    m_Color = RGB(SigmaApp.m_Options.console.font.color.r, SigmaApp.m_Options.console.font.color.g, SigmaApp.m_Options.console.font.color.b);

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

    return TRUE;
}

void CSigmaConsole::OnClose()
{
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
    ShowWindow(SW_HIDE);
}

void CSigmaConsole::PostNcDestroy()
{
    // >> History permanent
    char_t szLoc[ML_STRSIZE];
    char_t szLine[ML_STRSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    memset(szLine, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*) (SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("ConsoleHistory.par"));
    int_t ilen = (int_t) (m_edtConsole.m_nTotalCommand);
    if ((ilen > 1) && (ilen <= CONSOLE_HISTORY)) {
        FILE *fp = NULL;
        if ((fp = _wfopen((const char_t*) szLoc, _T("w"))) != NULL) {
            int_t jj;
            for (jj = 0; jj < ilen; jj++) {
                SigmaApp.CStringToChar(m_edtConsole.m_aCommandHistory[jj], szLine, ML_STRSIZE - 1);
                if (fputws((const char_t*) szLine, fp) == EOF) {
                    break;
                }
                fputws(_T("\n"), fp);
            }
            fclose(fp);    fp = NULL;
        }
    }
    else {
        ::DeleteFile((LPCTSTR) szLoc);
    }
    // <<

    CDialog::PostNcDestroy();
}

void CSigmaConsole::OnConsoleClose()
{
    OnClose();
}

void CSigmaConsole::OnOK()
{
}

void CSigmaConsole::OnCancel()
{
    OnClose();
}

void CSigmaConsole::OnConsoleReset()
{
    m_edtConsole.m_nCurCommand = m_edtConsole.m_nTotalCommand = 0;
    m_edtConsole.m_aCommandHistory.RemoveAll();
    m_edtConsole.SetHistorySize(CONSOLE_HISTORY);
    m_edtConsole.EmptyUndoBuffer();
    m_edtConsole.SetWindowText(_T(""));
    m_edtConsole.SetWindowText((LPCTSTR) (m_edtConsole.m_strPromptText));
    m_edtConsole.SetSel(m_edtConsole.m_strPromptText.GetLength(), -1);

    // Parser
    SigmaApp.m_nVarnum = 0;
    const char_t *szAZ[_T('z') - _T('a') + 1] = { _T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("h"), _T("i"), _T("j"), _T("k"), _T("l"), _T("m"),
        _T("n"), _T("o"), _T("p"), _T("q"), _T("r"), _T("s"), _T("t"), _T("u"), _T("v"), _T("w"), _T("x"), _T("y"), _T("z") };

    for (int_t ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {
        SigmaApp.m_fParam[ii] = 0.0;
        _tcscpy(SigmaApp.m_szParam[ii], szAZ[ii]);        // Params name : by default : a, b, c, ..., z
        SigmaApp.m_bParam[ii] = FALSE;                        // Param set ?
    }
}

void CSigmaConsole::OnConsoleFont()
{
    LOGFONT lft;
    memset(&lft, 0, sizeof(lft));
    m_Font.GetLogFont(&lft);
    if (SigmaApp.GetFont(m_hWnd, &lft)) {
        m_Font.DeleteObject();
        // >> Set permanent
        SigmaApp.LogfontToFont(lft, &(SigmaApp.m_Options.console.font));
        // <<
        m_Font.CreateFontIndirect(&lft);
        m_edtConsole.SetFont(&m_Font);
    }
}

void CSigmaConsole::OnConsoleFontColor()
{
    COLORREF clr = m_Color;
    if (SigmaApp.GetColor(m_hWnd, &clr)) {
        m_Color = clr;
        SigmaApp.m_Options.console.font.color.r = GetRValue(clr);
        SigmaApp.m_Options.console.font.color.g = GetGValue(clr);
        SigmaApp.m_Options.console.font.color.b = GetBValue(clr);
    }
}

HBRUSH CSigmaConsole::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd->m_hWnd == m_edtConsole.m_hWnd) {
        pDC->SetTextColor(m_Color);
    }

    return hbr;
}

void CSigmaConsole::OnConsoleSaveAs()
{
    int_t nLines = m_edtConsole.GetLineCount();
    if ((nLines < 1) || (nLines > CONSOLE_MAXLINES)) {
        SigmaApp.Output(_T("Cannot save console content.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
    }

    char_t szFilters[] = _T("Console Files (*.txt)|*.txt||");

    CFileDialog *pFileDlg = new CFileDialog(FALSE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
    if (pFileDlg->DoModal() != IDOK) {
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString fileName = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    CString strT;

    CStdioFile fileT;
    try {
        CFileException excT;
        if (!fileT.Open((LPCTSTR) fileName, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &excT)) {
            char_t szError[ML_STRSIZEW];
            excT.GetErrorMessage(szError, ML_STRSIZEW);
            // >> OUTPUT
            strT.Format(_T("Cannot save console file: %s\r\n"), szError);
            SigmaApp.Output(strT, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            // <<
            m_edtConsole.SetFocus();
            m_edtConsole.SetSel(-1, -1);
            return;
        }
        int_t ii = 0, nLength = 0;
        char_t szBuffer[ML_STRSIZE];
        memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
        for (ii = 0; ii < nLines; ii++) {
            nLength = m_edtConsole.GetLine(ii, (char_t*) szBuffer, ML_STRSIZE - 1);
            if ((nLength > 2) && (nLength <= (ML_STRSIZE - 1))) {
                szBuffer[nLength] = _T('\0');
                fileT.WriteString((const char_t*) szBuffer);
                fileT.WriteString(_T("\r\n"));
            }
        }
        fileT.Close();

        // >> OUTPUT
        strT.Format(_T("Console file saved: %d lines.\r\n"), ii);
        SigmaApp.Output(strT, TRUE);
        // <<

        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
    catch (...) {
        fileT.Close();
        // >> OUTPUT
        SigmaApp.Output(_T("Cannot save console content.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
        // <<
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
}

void CSigmaConsole::OnConsoleOpen()
{
    char_t szFilters[] = _T("Console Files (*.txt)|*.txt||");

    CFileDialog *pFileDlg = new CFileDialog(TRUE, _T("txt"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);
    if (pFileDlg->DoModal() != IDOK) {
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString fileName = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    CString strT;

    CStdioFile fileT;
    try {
        CFileException excT;
        if (!fileT.Open((LPCTSTR) fileName, CFile::modeRead | CFile::typeText | CFile::shareExclusive, &excT)) {
            char_t szError[ML_STRSIZEW];
            excT.GetErrorMessage(szError, ML_STRSIZEW);
            // >> OUTPUT
            strT.Format(_T("Cannot read console file: %s\r\n"), szError);
            SigmaApp.Output(strT, TRUE, TRUE, SIGMA_OUTPUT_WARNING);
            // <<
            m_edtConsole.SetFocus();
            m_edtConsole.SetSel(-1, -1);
            return;
        }
        // Nead Confirmation
        OnConsoleReset();
        int_t nLen = (int_t) (fileT.GetLength());
        if ((nLen < 5) || (nLen >(ML_STRSIZE * CONSOLE_MAXLINES))) {
            // >> OUTPUT
            fileT.Close();
            SigmaApp.Output(_T("Cannot read console file: invalid size.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
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
            nLength = (int_t) _tcslen((const char_t*) (m_edtConsole.m_szCommand));
            if ((ii > CONSOLE_MAXLINES) || (nLength < 2) || (nLength >= ML_STRSIZE)) {
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
                m_edtConsole.AppendText((const char_t*) pszT);
                m_edtConsole.DoParse(m_edtConsole.m_strPromptText.GetLength());
                m_edtConsole.AppendText(_T("\r\n"));
                m_edtConsole.AppendText(m_edtConsole.m_strPromptText);
            }
        }
        fileT.Close();

        // >> OUTPUT
        strT.Format(_T("Console file read: %d lines.\r\n"), ii);
        SigmaApp.Output(strT, TRUE);
        // <<
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
    catch (...) {
        fileT.Close();
        // >> OUTPUT
        SigmaApp.Output(_T("Cannot read console file.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
        // <<
        m_edtConsole.SetFocus();
        m_edtConsole.SetSel(-1, -1);
        return;
    }
}

inline void CSigmaConsole::InsertText(const char_t *szText, int_t nRelPos)
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

void CSigmaConsole::OnConsoleFunc80()
{
    InsertText(_T("sqrt()"));
}

void CSigmaConsole::OnConsoleFunc81()
{
    InsertText(_T("abs()"));
}

void CSigmaConsole::OnConsoleFunc83()
{
    InsertText(_T("ceil()"));
}

void CSigmaConsole::OnConsoleFunc84()
{
    InsertText(_T("int()"));
}

void CSigmaConsole::OnConsoleFunc01()
{
    InsertText(_T("sin()"));
}

void CSigmaConsole::OnConsoleFunc02()
{
    InsertText(_T("cos()"));
}

void CSigmaConsole::OnConsoleFunc03()
{
    InsertText(_T("tan()"));
}

void CSigmaConsole::OnConsoleFunc04()
{
    InsertText(_T("asin()"));
}

void CSigmaConsole::OnConsoleFunc05()
{
    InsertText(_T("acos()"));
}

void CSigmaConsole::OnConsoleFunc06()
{
    InsertText(_T("atan()"));
}

void CSigmaConsole::OnConsoleFunc10()
{
    InsertText(_T("ln()"));
}

void CSigmaConsole::OnConsoleFunc20()
{
    InsertText(_T("exp()"));
}

void CSigmaConsole::OnConsoleFunc30()
{
    InsertText(_T("pow(,)"), 2);
}

void CSigmaConsole::OnConsoleConst01()
{
    InsertText(_T("pi"), 0);
}

void CSigmaConsole::OnConsoleConst10()
{
    InsertText(_T("_q"), 0);
}

void CSigmaConsole::OnConsoleConst11()
{
    InsertText(_T("_m"), 0);
}

void CSigmaConsole::OnConsoleConst12()
{
    InsertText(_T("_k"), 0);
}

void CSigmaConsole::OnConsoleConst13()
{
    InsertText(_T("_h"), 0);
}

void CSigmaConsole::OnConsoleConst14()
{
    InsertText(_T("_c"), 0);
}

void CSigmaConsole::OnConsoleConst15()
{
    InsertText(_T("_e"), 0);
}

void CSigmaConsole::OnConsoleConst16()
{
    InsertText(_T("_n"), 0);
}

void CSigmaConsole::OnConsoleRad()
{
    ml_angleunit = 0x00;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
}

void CSigmaConsole::OnConsoleDeg()
{
    ml_angleunit = 0x01;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
}

void CSigmaConsole::OnConsoleGrad()
{
    ml_angleunit = 0x02;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_CHECKED);
    }
}

void CSigmaConsole::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    if (m_bInitialized) {
        CRect recTT;
        GetClientRect(recTT);
        m_edtConsole.MoveWindow(recTT);
    }
}

void CSigmaConsole::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH3;
    lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT4;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CSigmaConsole::OnConsoleCut()
{
    m_edtConsole.Cut();
}

void CSigmaConsole::OnConsoleCopy()
{
    m_edtConsole.Copy();
}

void CSigmaConsole::OnConsolePaste()
{
    m_edtConsole.Paste();
}


void CSigmaConsole::OnViewInput()
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
                RECT rcT = { 0, 0, 0, 0 }, rcTT = { 0, 0, 0, 0 };
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

void CSigmaConsole::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/console.html"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CSigmaConsole::PreTranslateMessage(MSG* pMsg)
{
    // [FIX-SG200-004] All keys work now (included Suppr). Only Ctrl+S is trapped by accelerators.
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
            else if ((pMsg->wParam == VK_F1)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
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
            else if ((pMsg->wParam == VK_F8)) {
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
                if (pFrame) {
                    pFrame->OnCmdMsg(ID_VIEW_OUTPUT, 0, NULL, NULL);
                    return TRUE;
                }
            }
            else if ((pMsg->wParam == VK_F9)) {
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
                if (pFrame) {
                    pFrame->OnCmdMsg(ID_VIEW_CONSOLE, 0, NULL, NULL);
                    return TRUE;
                }
            }
            else if ((pMsg->wParam == VK_F11)) {
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
                if (pFrame) {
                    pFrame->OnCmdMsg(ID_VIEW_SCRIPT, 0, NULL, NULL);
                    return TRUE;
                }
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CSigmaConsole::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    int nLen = m_edtConsole.GetWindowTextLength();
    DWORD sel = m_edtConsole.GetSel();
    BOOL bEnable = ((nLen < 1) || ((LOWORD(sel) == 0) && (HIWORD(sel) == nLen))) ? FALSE : TRUE;
    pCmdUI->Enable(bEnable);
}

void CSigmaConsole::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    DWORD sel = m_edtConsole.GetSel();
    pCmdUI->Enable(LOWORD(sel) != HIWORD(sel));
}

void CSigmaConsole::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    DWORD sel = m_edtConsole.GetSel();
    pCmdUI->Enable(LOWORD(sel) != HIWORD(sel));
}

void CSigmaConsole::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}
void CSigmaConsole::OnUpdateConsoleTopmost(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}

void CSigmaConsole::OnViewVarlist()
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
                RECT rcT = { 0, 0, 0, 0 }, rcTT = { 0, 0, 0, 0 };
                GetWindowRect(&rcT);
                m_pVarlistDlg->GetWindowRect(&rcTT);
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

void CSigmaConsole::OnConsoleTopmost()
{
    if (m_bTopmost == TRUE) {
        ::SetWindowPos(GetSafeHwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        m_bTopmost = FALSE;
    }
    else {
        ::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        m_bTopmost = TRUE;
    }

    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_CONSOLE_TOPMOST, (m_bTopmost == TRUE) ? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED));
    }
}

void CSigmaConsole::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}