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
#include "ImportDropDlg.h"


// CImportDropDlg dialog

IMPLEMENT_DYNAMIC(CImportDropDlg, CDialog)

CImportDropDlg::CImportDropDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CImportDropDlg::IDD, pParent)
    , m_strFilename(_T(""))
    , m_nSkip(0)
    , m_nSeparator(0)
    , m_nColCount(0)
    , m_bReadTitle(TRUE)
{
    m_pColumn = NULL;
    m_pColumnCount = NULL;
    m_pCurrentID = NULL;
    m_pbIsModified = NULL;
    m_pbRunning = NULL;
    m_pszNote = NULL;
    m_bOK = FALSE;
}

CImportDropDlg::~CImportDropDlg()
{

}

void CImportDropDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_ASCII_FILENAME, m_edtFilename);
    DDX_Text(pDX, IDE_ASCII_FILENAME, m_strFilename);
    DDV_MaxChars(pDX, m_strFilename, ML_STRSIZE);
    DDX_Control(pDX, IDE_ASCII_FILEINFO, m_edtFileInfo);
    DDX_Control(pDX, IDC_ASCII_SEP, m_cbSeparator);
    DDX_CBIndex(pDX, IDC_ASCII_SEP, m_nSeparator);
    DDX_Text(pDX, IDE_ASCII_SKIP, m_nSkip);
    DDX_Check(pDX, IDK_ASCII_READTITLE, m_bReadTitle);
}


BEGIN_MESSAGE_MAP(CImportDropDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CImportDropDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CImportDropDlg::OnCancel)
END_MESSAGE_MAP()


// CImportDropDlg message handlers

BOOL CImportDropDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ((m_pColumn == NULL) || (m_pbIsModified == NULL) || (m_pbRunning == NULL)
        || (m_pszNote == NULL) || (m_strFilename.IsEmpty())) {
        //SigmaApp.Output(_T("Cannot show the 'Import ASCII' dialog: invalid columns.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        EndDialog(-1);
        return FALSE;
    }

    HICON hIcon =     (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_IMPORT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    HWND hWnd = GetSafeHwnd();
    if (hWnd != NULL) {
        ::SendDlgItemMessage(hWnd, IDE_ASCII_COLX, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_ASCII_COLY, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_ASCII_SKIP, CB_LIMITTEXT, (WPARAM)3, (LPARAM)0);
    }

    if (GetFileInfo(m_strFilename) == FALSE) {
        EndDialog(-1);
        return FALSE;
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

    m_nSkip = SigmaApp.m_Options.import.iSkip;
    m_nColCount = SigmaApp.m_Options.import.iCol;

    m_bReadTitle = (SigmaApp.m_Options.import.iTitle == 1);

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CImportDropDlg::GetFileInfo(CString strFilename)
{
    int_t nLength = strFilename.GetLength();
    if ((nLength < 5) || (nLength > ML_STRSIZE)) {
        return FALSE;
    }

    CString strT;
    CFileStatus fileStatus;

    char_t strHeader[32];
    strHeader[0] = strHeader[31] = _T('\0');

    m_strFilename = strFilename;
    m_edtFilename.SetWindowText((LPCTSTR)strFilename);

    CStdioFile fileT;
    CFileException excT;
    if (!fileT.Open((LPCTSTR)m_strFilename, CFile::modeRead | CFile::typeText | CFile::shareExclusive, &excT)) {
        char_t szError[ML_STRSIZEW];
        excT.GetErrorMessage(szError, ML_STRSIZEW);
        // >> OUTPUT
        strT.Format(_T("Cannot read file: %s\r\n"), szError);
        SigmaApp.Output(strT, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        // <<
        return FALSE;
    }
    if (fileT.GetStatus(fileStatus) == FALSE) {
        fileT.Close();
        // >> OUTPUT
        SigmaApp.Output(_T("Cannot read file: access denied.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        // <<
        return FALSE;
    }
    fileT.ReadString(strHeader, 31);
    fileT.Close();

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
    if ((_T('\0') != strHeader[0]) && (_tcsncmp((const char_t *) strHeader, _T("# Solis"), _tcslen(_T("# Solis"))) == 0)) {
        SigmaApp.m_Options.import.cSep = _T('\t');
        SigmaApp.m_Options.import.iTitle = 1;
    }

    return TRUE;
}

void CImportDropDlg::OnOK()
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

    int_t nRowCount = SigmaApp.m_Options.import.iRow;
    SigmaApp.m_Options.import.iSkip = m_nSkip;
    SigmaApp.m_Options.import.iCol = m_nColCount;
    SigmaApp.m_Options.import.iTitle = (m_bReadTitle == TRUE) ? 1 : 0;

    // Les colonnes sont nommées A, B, C, D, ... à la façon d'Excel
    m_bOK = SigmaApp.ImportData(m_strFilename, m_pColumn, NULL, m_pColumnCount,
        &nRowCount, m_pCurrentID, m_pszNote);

    CDialog::OnOK();
}

void CImportDropDlg::OnCancel()
{
    CDialog::OnCancel();
}

BOOL CImportDropDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        if ((pMsg->wParam == 'A') && (GetKeyState(VK_CONTROL) < 0)) {
            CWnd *pWnd = NULL;
            HWND hWnd = NULL;
            if (((pWnd = GetFocus()) == NULL) || ((hWnd = pWnd->GetSafeHwnd()) == NULL)) {
                return CDialog::PreTranslateMessage(pMsg);
            }
            if (m_edtFilename.GetSafeHwnd() == hWnd) {
                m_edtFilename.SetSel(0, -1);
            }
            else if (m_edtFileInfo.GetSafeHwnd() == hWnd) {
                m_edtFileInfo.SetSel(0, -1);
            }
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}
