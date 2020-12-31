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
#include "ImportDlg.h"

#define WM_SELECTALL                    (WM_USER + 0x7000)

// CImportDlg dialog

IMPLEMENT_DYNAMIC(CImportDlg, CDialog)

CImportDlg::CImportDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CImportDlg::IDD, pParent)
    , m_strFilename(_T(""))
    , m_nRowCount(0)
    , m_nSkip(0)
    , m_nSeparator(0)
    , m_nColCount(0)
    , m_bReadTitle(TRUE)
{
    m_piColumn = NULL;
    m_pColumn = NULL;
    m_pColumnCount = NULL;
    m_pCurrentID = NULL;
    m_pbIsModified = NULL;
    m_pbRunning = NULL;
    m_pszNote = NULL;
    m_bOK = FALSE;
}

CImportDlg::~CImportDlg()
{

}

void CImportDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_ASCII_CONTENT, m_edtFileContent);
    DDX_Control(pDX, IDE_ASCII_FILENAME, m_edtFilename);
    DDX_Text(pDX, IDE_ASCII_FILENAME, m_strFilename);
    DDV_MaxChars(pDX, m_strFilename, ML_STRSIZE - 1);
    DDX_Control(pDX, IDE_ASCII_FILEINFO, m_edtFileInfo);
    DDX_Control(pDX, IDC_ASCII_SEP, m_cbSeparator);
    DDX_CBIndex(pDX, IDC_ASCII_SEP, m_nSeparator);
    DDX_Text(pDX, IDE_ASCII_ROWCOUNT, m_nRowCount);
    DDX_Text(pDX, IDE_ASCII_SKIP, m_nSkip);
    DDX_Check(pDX, IDK_ASCII_READTITLE, m_bReadTitle);
    DDX_Control(pDX, IDE_ASCII_SELCOLS, m_edtSelectedColumns);
    DDX_Text(pDX, IDE_ASCII_SELCOLS, m_strSelectedColumns);
    DDV_MaxChars(pDX, m_strSelectedColumns, ML_STRSIZES - 1);
}


BEGIN_MESSAGE_MAP(CImportDlg, CDialog)
    ON_WM_DROPFILES()
    ON_BN_CLICKED(ID_ASCII_BROWSE, &CImportDlg::OnFileBrowse)
    ON_BN_CLICKED(IDOK, &CImportDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CImportDlg::OnCancel)
END_MESSAGE_MAP()


// CImportDlg message handlers

BOOL CImportDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ((m_pColumn == NULL) || (m_piColumn == NULL) || (m_pbIsModified == NULL) || (m_pbRunning == NULL) || (m_pszNote == NULL)) {
        SigmaApp.Output(_T("Cannot show the 'Import ASCII' dialog: invalid columns.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        EndDialog(-1);
        return FALSE;
    }

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_IMPORT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    HWND hWnd = GetSafeHwnd();
    if (hWnd != NULL) {
        ::SendDlgItemMessage(hWnd, IDE_ASCII_COLX, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_ASCII_COLY, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_ASCII_SKIP, CB_LIMITTEXT, (WPARAM)3, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_ASCII_SKIP, IDE_ASCII_SELCOLS, (WPARAM)(ML_STRSIZES - 1), (LPARAM)0);
    }

    if (m_strFilename != _T("")) {
        GetFileContent(m_strFilename);
    }

    if (SigmaApp.m_Options.import.cSep == _T('\t')) {
        m_nSeparator = 0;
    }
    else if (SigmaApp.m_Options.import.cSep == _T(';')) {
        m_nSeparator = 1;
    }
    else if (SigmaApp.m_Options.import.cSep == _T(' ')) {
        m_nSeparator = 2;
    }
    m_cbSeparator.AddString(_T("TAB"));
    m_cbSeparator.AddString(_T(";"));
    m_cbSeparator.AddString(_T("SPACE"));
    m_cbSeparator.SetCurSel(m_nSeparator);

    m_nRowCount = SigmaApp.m_Options.import.iRow;
    m_nSkip = SigmaApp.m_Options.import.iSkip;
    m_nColCount = SigmaApp.m_Options.import.iCol;

    m_bReadTitle = (SigmaApp.m_Options.import.iTitle == 1);

    m_strSelectedColumns = _T("*");

    DragAcceptFiles(TRUE);

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportDlg::OnDropFiles(HDROP hDropInfo)
{
    char_t szFilename[ML_STRSIZE], szExt[5];
    int_t nLength = 0, iNumFilesDroped;
    BOOL bOK = TRUE;

    //Getting the number of files been droped
    memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
    iNumFilesDroped = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, szFilename, ML_STRSIZE - 1);
    if (iNumFilesDroped != 1) {
        ::DragFinish(hDropInfo);
        return;
    }

    memset(szExt, 0, 5 * sizeof(char_t));
    nLength = ::DragQueryFile(hDropInfo, 0, szFilename, ML_STRSIZE - 1);
    if ((nLength < 5) || (nLength >= (ML_STRSIZE - 1))) {
        ::DragFinish(hDropInfo);
        return;
    }
    szExt[0] = szFilename[nLength - 4];
    szExt[1] = szFilename[nLength - 3];
    szExt[2] = szFilename[nLength - 2];
    szExt[3] = szFilename[nLength - 1];
    szExt[4] = _T('\0');
    if ((_tcsicmp((const char_t*) szExt, _T(".txt")) == 0)
        && (_tcsicmp((const char_t*) szExt, _T(".dat")) == 0)
        && (_tcsicmp((const char_t*) szExt, _T(".csv")) == 0)
        && (_tcsicmp((const char_t*) szExt, _T(".log")) == 0)) {
        ::DragFinish(hDropInfo);
        return;
    }

    CString strFilename((const char_t*) szFilename);
    GetFileContent(strFilename);

    ::DragFinish(hDropInfo);
    return;
}

void CImportDlg::OnFileBrowse()
{
    char_t szFilters[] = _T("ASCII Files (*.txt;*.dat;*.csv;*.log)|*.txt;*.dat;*.csv;*.log|All files (*.*)|*.*||");

    CFileDialog *pFileDlg = new CFileDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);
    if (pFileDlg->DoModal() != IDOK) {
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString strFilename = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    GetFileContent(strFilename);

    UpdateData(FALSE);
}

BOOL CImportDlg::GetFileContent(CString strFilename)
{
    CWaitCursor wcT;

    int_t nLength = strFilename.GetLength();
    if ((nLength < 5) || (nLength > ML_STRSIZE)) {
        return FALSE;
    }

    CString strT;
    CFileStatus fileStatus;

    m_nRowCount = 0;
    if (SigmaApp.ReadTextFile(strFilename, &m_edtFileContent, fileStatus, &m_nRowCount) == FALSE) {
        return FALSE;
    }

    m_strFilename = strFilename;
    m_edtFilename.SetWindowText((LPCTSTR)strFilename);

    CTime createdTime = fileStatus.m_ctime, modifTime = fileStatus.m_mtime;
    if ((createdTime.GetTime() == 0) || (createdTime.GetTime() > modifTime.GetTime())) {
        createdTime = modifTime;
    }
    CString strCT = createdTime.Format((LPCTSTR)ML_DATEFMT);
    CString strMT = modifTime.Format((LPCTSTR)ML_DATEFMT);
    if (fileStatus.m_size > 1024) {
        strT.Format(_T("Size: %d KB, Created: "), fileStatus.m_size / 1024);
        strT += strCT;
        strT += _T(", Modified: ");
        strT += strMT;
    }
    else {
        strT.Format(_T("Size: %d B, Created: "), fileStatus.m_size);
        strT += strCT;
        strT += _T(", Modified: ");
        strT += strMT;
    }
    m_edtFileInfo.SetWindowText((LPCTSTR)strT);

    // Solis file?
    char_t strHeader[32];
    strHeader[0] = strHeader[31] = _T('\0');
    m_edtFileContent.GetWindowText(strHeader, 31);
    if ((_T('\0') != strT[0]) && (_tcsncmp((const char_t *) strT, _T("# Solis"), _tcslen(_T("# Solis"))) == 0)) {
        SigmaApp.m_Options.import.cSep = _T('\t');
        SigmaApp.m_Options.import.iTitle = 1;
    }

    return TRUE;
}

void CImportDlg::OnOK()
{
    CWaitCursor wcT;

    UpdateData(TRUE);

    m_bOK = FALSE;

    if (m_nSeparator == 0) {
        SigmaApp.m_Options.import.cSep = _T('\t');
    }
    else if (m_nSeparator == 1) {
        SigmaApp.m_Options.import.cSep = _T(';');
    }
    else if (m_nSeparator == 2) {
        SigmaApp.m_Options.import.cSep = _T(' ');
    }

    SigmaApp.m_Options.import.iRow = m_nRowCount;
    SigmaApp.m_Options.import.iSkip = m_nSkip;
    SigmaApp.m_Options.import.iCol = m_nColCount;
    SigmaApp.m_Options.import.iTitle = (m_bReadTitle == TRUE) ? 1 : 0;

    // m_piColumn defined as array piColumn[SIGMA_MAXCOLUMNS]
    for (int jj = 0; jj < SIGMA_MAXCOLUMNS; jj++) {
        m_piColumn[jj] = 0;
    }
    bool bSelCol = false;
    int iCol;
    char_t szT[3];
    szT[2] = _T('\0');
    // Définir les colonnes à importer
    if (m_strSelectedColumns.IsEmpty() == false) {
        int iLen = m_strSelectedColumns.GetLength();
        if (m_strSelectedColumns[0] != _T('*')) {
            if (iLen <= 2) {
                if (m_strSelectedColumns[iLen - 1] == _T(';')) {
                    szT[0] = _T('0');
                    szT[1] = m_strSelectedColumns[0];
                }
                else {
                    szT[0] = m_strSelectedColumns[0];
                    szT[1] = m_strSelectedColumns[1];
                }
                iCol = _wtoi((const char_t*)szT);
                if ((iCol >= 1) && (iCol <= SIGMA_MAXCOLUMNS)) {
                    m_piColumn[iCol - 1] = 1;
                    bSelCol = true;
                }
            }
            else {
                int nTokenPos = 0;
                CString strToken = m_strSelectedColumns.Tokenize(_T(";"), nTokenPos);
                while (strToken.IsEmpty() == false) {
                    iCol = _wtoi(strToken);
                    if ((iCol >= 1) && (iCol <= SIGMA_MAXCOLUMNS)) {
                        m_piColumn[iCol - 1] = 1;
                        bSelCol = true;
                    }
                    else {
                        break;
                    }
                    strToken = m_strSelectedColumns.Tokenize(_T(";"), nTokenPos);
                }
            }
        }
    }
    if (bSelCol == false) {
        for (int jj = 0; jj < SIGMA_MAXCOLUMNS; jj++) {
            m_piColumn[jj] = 1;
        }
    }

    // Les colonnes sont nommées A, B, C, D, ... à la façon d'Excel
    m_bOK = SigmaApp.ImportData(m_strFilename, m_pColumn, m_piColumn, m_pColumnCount,
        &m_nRowCount, m_pCurrentID, m_pszNote);

    CDialog::OnOK();
}

void CImportDlg::OnCancel()
{
    CDialog::OnCancel();
}

BOOL CImportDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        if ((pMsg->wParam == 'A') && (GetKeyState(VK_CONTROL) < 0)) {
            CWnd *pWnd = NULL;
            HWND hWnd = NULL;
            if (((pWnd = GetFocus()) == NULL) || ((hWnd = pWnd->GetSafeHwnd()) == NULL)) {
                return CDialog::PreTranslateMessage(pMsg);
            }
            if (m_edtFileContent.GetSafeHwnd() == hWnd) {
                m_edtFileContent.SetSel(0, -1);
            }
            else if (m_edtFilename.GetSafeHwnd() == hWnd) {
                m_edtFilename.SetSel(0, -1);
            }
            else if (m_edtFileInfo.GetSafeHwnd() == hWnd) {
                m_edtFileInfo.SetSel(0, -1);
            }
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}