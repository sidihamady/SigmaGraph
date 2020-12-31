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
#include "MainFrm.h"

#include "ChildFrmPlot.h"
#include "ChildFrmData.h"
#include "ChildFrmDataFit.h"
#include "SigmaDoc.h"
#include "SigmaViewPlot.h"
#include "SigmaViewData.h"
#include "SigmaViewDataFit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName, _Out_writes_(nMax) LPTSTR lpszTitle, UINT nMax);
UINT AFXAPI AfxGetFileName(LPCTSTR lpszPathName, _Out_writes_opt_(nMax) LPTSTR lpszTitle, UINT nMax);
BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);
AFX_STATIC void AFXAPI _AfxAbbreviateName(_Inout_z_ LPTSTR lpszCanon, int cchMax, BOOL bAtLeastName);

BOOL SigmaRecentFileList::GetDisplayName(CString& strName, int nIndex,
    LPCTSTR lpszCurDir, int nCurDir, BOOL bAtLeastName/* = TRUE*/) const
{
    ENSURE_ARG(lpszCurDir == NULL || AfxIsValidString(lpszCurDir, nCurDir));

    ASSERT(m_arrNames != NULL);
    ENSURE_ARG(nIndex < m_nSize);
    if (lpszCurDir == NULL || m_arrNames[nIndex].IsEmpty())
        return FALSE;

    int nLenName = m_arrNames[nIndex].GetLength();
    LPTSTR lpch = strName.GetBuffer(nLenName + 1);
    if (lpch == NULL) {
        AfxThrowMemoryException();
    }
    Checked::tcsncpy_s(lpch, nLenName + 1, m_arrNames[nIndex], _TRUNCATE);
    // nLenDir is the length of the directory part of the full path
    int nLenDir = nLenName - (AfxGetFileName(lpch, NULL, 0) - 1);
    BOOL bSameDir = FALSE;
    if (nLenDir == nCurDir) {
        TCHAR chSave = lpch[nLenDir];
        lpch[nCurDir] = 0;  // terminate at same location as current dir
        bSameDir = ::AfxComparePath(lpszCurDir, lpch);
        lpch[nLenDir] = chSave;
    }
    // copy the full path, otherwise abbreviate the name
    if (bSameDir) {
        // copy file name only since directories are same
        TCHAR szTemp[_MAX_PATH];
        AfxGetFileTitle(lpch + nCurDir, szTemp, _countof(szTemp));
        Checked::tcsncpy_s(lpch, nLenName + 1, szTemp, _TRUNCATE);
    }
    else if (m_nMaxDisplayLength != -1) {
        // abbreviate name based on what will fit in limited space
        _AfxAbbreviateName(lpch, m_nMaxDisplayLength, bAtLeastName);
    }
    strName.ReleaseBuffer();
    return TRUE;
}

void SigmaRecentFileList::UpdateMenu(CCmdUI* pCmdUI)
{
    ENSURE_ARG(pCmdUI != NULL);
    ASSERT(m_arrNames != NULL);

    if (m_strOriginal.IsEmpty())
        m_strOriginal = _T("Recent File");

    if (m_arrNames[0].IsEmpty()) {
        // no MRU files
        if (pCmdUI->m_pMenu != NULL) {
            int iMRU;
            for (iMRU = 1; iMRU < (m_nSize + 1); iMRU++)
                pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);
            m_uiClearID = 0;
        }
        pCmdUI->SetText(m_strOriginal);
        pCmdUI->Enable(FALSE);
        return;
    }

    if (pCmdUI->m_pMenu == NULL)
        return;

    int iMRU;
    for (iMRU = 0; iMRU < m_nSize; iMRU++)
        pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID + iMRU, MF_BYCOMMAND);

    if (m_uiClearID > ID_FILE_MRU_FILE1)
        pCmdUI->m_pMenu->DeleteMenu(m_uiClearID, MF_BYCOMMAND);

    TCHAR szCurDir[_MAX_PATH];
    DWORD dwDirLen = GetCurrentDirectory(_MAX_PATH, szCurDir);
    if (dwDirLen == 0 || dwDirLen >= _MAX_PATH)
        return;    // Path too long

    int nCurDir = AtlStrLen(szCurDir);
    ASSERT(nCurDir >= 0);
    szCurDir[nCurDir] = '\\';
    szCurDir[++nCurDir] = '\0';

    CString strName;
    CString strTemp;
    for (iMRU = 0; iMRU < m_nSize; iMRU++) {
        if (!GetDisplayName(strName, iMRU, szCurDir, nCurDir))
            break;

        // double up any '&' characters so they are not underlined
        LPCTSTR lpszSrc = strName;
        LPTSTR lpszDest = strTemp.GetBuffer(strName.GetLength() * 2);
        while (*lpszSrc != 0) {
            if (*lpszSrc == '&')
                *lpszDest++ = '&';
            if (_istlead(*lpszSrc))
                *lpszDest++ = *lpszSrc++;
            *lpszDest++ = *lpszSrc++;
        }
        *lpszDest = 0;
        strTemp.ReleaseBuffer();

        // insert mnemonic + the file name
        TCHAR buf[10];
        int nItem = ((iMRU + m_nStart) % _AFX_MRU_MAX_COUNT) + 1;


        // number &1 thru &9, then 1&0, then 11 thru ...
        if (nItem > 10)
            _stprintf_s(buf, _countof(buf), _T("%d "), nItem);
        else if (nItem == 10)
            Checked::tcscpy_s(buf, _countof(buf), _T("1&0 "));
        else
            _stprintf_s(buf, _countof(buf), _T("&%d "), nItem);

        pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++,
            MF_STRING | MF_BYPOSITION, pCmdUI->m_nID++,
            CString(buf) + strTemp);
    }

    // update end menu count
    pCmdUI->m_nIndex--; // point to last menu added
    pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();

    m_uiClearID = pCmdUI->m_nID;
    pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex + 1,
        MF_STRING | MF_BYPOSITION, m_uiClearID,
        _T("Clear Recent List"));
    pCmdUI->m_pMenu->EnableMenuItem(m_uiClearID, MF_BYCOMMAND | MF_ENABLED);


    pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
}

void CSigmaApp::LoadStdProfileSettings(UINT nMaxMRU)
{
    if (m_pRecentFileList != NULL) {
        delete m_pRecentFileList;
        m_pRecentFileList = NULL;
    }

    if (nMaxMRU != 0) {
        m_pRecentFileList = new SigmaRecentFileList(0, _T("Recent File List"), _T("File%d"), nMaxMRU, 47);
        m_pRecentFileList->ReadList();
    }

    m_nNumPreviewPages = GetProfileInt(_T("Settings"), _T("PreviewPages"), 0);
}

// CSigmaApp

BEGIN_MESSAGE_MAP(CSigmaApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CSigmaApp::OnAppAbout)
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, &CSigmaApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CSigmaApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
    ON_COMMAND(ID_APP_EXIT, &CSigmaApp::OnAppExit)
    ON_COMMAND_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_LAST, &CSigmaApp::OnOpenMRUFile)
    ON_COMMAND(ID_WINDOW_DUP, &CSigmaApp::OnWindowDup)
    ON_COMMAND(ID_WINDOW_COPYFORMAT, &CSigmaApp::OnWindowCopyFormat)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_COPYFORMAT, &CSigmaApp::OnUpdateWindowCopyFormat)
    ON_COMMAND(ID_WINDOW_PASTEFORMAT, &CSigmaApp::OnWindowPasteFormat)
    ON_UPDATE_COMMAND_UI(ID_WINDOW_PASTEFORMAT, &CSigmaApp::OnUpdateWindowPasteFormat)
    ON_COMMAND(ID_WINDOW_CLOSEALL, &CSigmaApp::OnWindowCloseAll)
    ON_COMMAND(ID_HELP_INDEX, &CSigmaApp::OnHelpIndex)
    ON_COMMAND(ID_HELP_CONTENTS, &CSigmaApp::OnHelpContents)
    ON_COMMAND(ID_HELP_CONTENTS_PDF, &CSigmaApp::OnHelpContentsPDF)
    ON_COMMAND(ID_CONTEXT_HELP, &CSigmaApp::OnHelpContext)
END_MESSAGE_MAP()

// CSigmaApp construction

CSigmaApp::CSigmaApp()
{
    m_pLib = NULL;
    m_pPlotCopy = NULL;

    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    m_iWinVersionMin = osvi.dwMinorVersion;
    m_iWinVersionMax = osvi.dwMajorVersion;

    int_t jj;
    m_iUserFitCount = 0;
    for (jj = 0; jj < ML_FIT_MAXMODEL; jj++) {
        m_pUserFit[jj] = NULL;
    }

    char m_szLocaleA[ML_STRSIZE];
    memset(m_szLocaleA, 0, ML_STRSIZE * sizeof(char));
    char *pszz = setlocale(LC_ALL, NULL);
    if (pszz) {
        strcpy(m_szLocaleA, (const char*) pszz);
        setlocale(LC_ALL, "C");
    }
}

// The one and only CSigmaApp object

CSigmaApp SigmaApp;

software_t g_Sigmaware;                                // Sigma software informations

void CSigmaApp::OnOpenMRUFile(UINT uiID)
{
    CWaitCursor wcT;

    ASSERT_VALID(this);
    ASSERT(m_pRecentFileList != NULL);
    if (m_pRecentFileList == NULL) {
        return;
    }

    int_t recentCount = m_pRecentFileList->GetSize();

    UINT uiClearID = ((SigmaRecentFileList*) m_pRecentFileList)->GetClearID();
    if (uiClearID == uiID) {
        CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
        if (pMainFrame) {
            if (pMainFrame->MessageBox(_T("Clear the recent files list?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
                delete m_pRecentFileList;
                m_pRecentFileList = NULL;
                m_pRecentFileList = new SigmaRecentFileList(0, _T("Recent File List"), _T("File%d"), 8, 47);
            }
        }
        return;
    }

    ASSERT(uiID >= ID_FILE_MRU_FILE1);
    ASSERT(uiID < ID_FILE_MRU_FILE1 + (UINT)recentCount);
    if ((uiID < ID_FILE_MRU_FILE1) || (uiID > ID_FILE_MRU_FILE1 + (UINT)recentCount)) {
        return;
    }

    int nIndex = (int_t) (uiID - ID_FILE_MRU_FILE1);
    ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);
    if ((*m_pRecentFileList)[nIndex].GetLength() == 0) {
        return;
    }

    CString strFilename = (LPCTSTR)((*m_pRecentFileList)[nIndex]);

    CString strExt = strFilename.Mid(strFilename.GetLength() - 4, 4);
    strExt.MakeLower();

    if ((strExt != _T(".sid")) && (strExt != _T(".lua"))
        && (strExt != _T(".txt")) && (strExt != _T(".dat"))
        && (strExt != _T(".csv")) && (strExt != _T(".log"))) {
        m_pRecentFileList->Remove(nIndex);
        return;
    }

    if (strExt == _T(".lua")) {
        CWaitCursor wait;
        CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
        if (pMainFrame) {
            if ((pMainFrame->m_pwndScript == NULL) || (pMainFrame->m_pwndScript->IsWindowVisible() == FALSE)) {
                pMainFrame->OnCmdMsg(ID_VIEW_SCRIPT, 0, 0, 0);
            }
            if (pMainFrame->m_pwndScript) {
                if (pMainFrame->m_pwndScript->ScriptFileOpen((LPCTSTR)strFilename) == TRUE) {
                    AddToRecentFileList((LPCTSTR)strFilename);
                }
            }
        }
        return;
    }

    CSigmaDoc* pDocToClose = NULL;

    // Vérifier que le fichier n'est pas déjà ouvert
    // parcourir tous les templates
    CDocTemplate* pTemplate;
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        pTemplate = GetNextDocTemplate(pos);
        ASSERT(pTemplate);
        // tous les documents du template.
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD) {
            CSigmaDoc* pDoc = (CSigmaDoc*)(pTemplate->GetNextDoc(posD));
            if (pDoc != NULL) {
                if ((1 == m_nDocCount) && (NULL == pDocToClose)) {
                    pDocToClose = pDoc;
                }
                if (pDoc->GetFilename() == strFilename) {
                    CView* pView = NULL;
                    POSITION posV = pDoc->GetFirstViewPosition();
                    while (posV != NULL) {
                        pView = pDoc->GetNextView(posV);
                        if (pView == NULL) {
                            break;
                        }
                        if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewPlot))) {
                            pView->GetParentFrame()->ActivateFrame(SW_RESTORE);
                            return;
                        }
                        else if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewData))) {
                            pView->GetParentFrame()->ActivateFrame(SW_RESTORE);
                            return;
                        }
                    }
                    return;
                }
            }
        }
    }

    // Ouvrir le document
    if (m_nDocCount >= SIGMA_MAXDOC) {
        Output(_T("Cannot open document.\r\nMaximum number of documents (16) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CDocTemplate* pDocTemplate = (CDocTemplate*)m_pTemplatePlot;
    ASSERT_VALID(pDocTemplate);
    if (pDocTemplate == NULL) {
        Output(_T("Cannot open document.\r\nInvalid document template.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CDocument* pDocument = pDocTemplate->CreateNewDocument();
    ASSERT_VALID(pDocument);
    if (pDocument == NULL) {
        Output(_T("Cannot open document.\r\nInvalid document resource.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CSigmaDoc* pSigmaDocument = (CSigmaDoc*)pDocument;

    BOOL bAutoDelete = pDocument->m_bAutoDelete;
    pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
    CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDocument, NULL);
    pDocument->m_bAutoDelete = bAutoDelete;
    if (pFrame == NULL) {
        Output(_T("Cannot open document.\r\nInvalid frame.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        delete pDocument;       // explicit delete on error
        return;
    }
    ASSERT_VALID(pFrame);

    if ((strExt == _T(".txt")) || (strExt == _T(".dat")) || (strExt == _T(".csv")) || (strExt == _T(".log"))) {
        pSigmaDocument->m_bDropping = TRUE;
        CStringToChar(strFilename, (char_t*)(pSigmaDocument->m_szDropFilename), ML_STRSIZE - 1);
        if (!pDocument->OnNewDocument()) {
            // user has be alerted to what failed in OnNewDocument
            pFrame->DestroyWindow();
            pSigmaDocument->m_bDropping = FALSE;
            delete pDocument;       // explicit delete on error
            return;
        }
    }
    else if (strExt == _T(".sid")) {
        // open an existing document
        CWaitCursor wait;
        if (!pDocument->OnOpenDocument((LPCTSTR)strFilename)) {
            // user has be alerted to what failed in OnOpenDocument
            pDocument->m_bAutoDelete = TRUE;
            pFrame->DestroyWindow();
            return;
        }
        pDocument->SetPathName((LPCTSTR)strFilename);
    }
    else {
        pDocument->m_bAutoDelete = TRUE;
        pFrame->DestroyWindow();
        return;
    }

    pDocTemplate->InitialUpdateFrame(pFrame, pDocument, TRUE);

    if (pDocToClose && (pDocToClose->canClose())) {
        pDocToClose->OnCloseDocument();
    }
}

// CSigmaApp initialization
BOOL CSigmaApp::getUserDir(void)
{
    memset(m_szMainDir, 0, ML_STRSIZE * sizeof(char_t));
    memset(m_szHelpFilename, 0, ML_STRSIZE * sizeof(char_t));
    memset(m_szHelpFilenamePDF, 0, ML_STRSIZE * sizeof(char_t));
    memset(m_szUserDir, 0, ML_STRSIZE * sizeof(char_t));

    DWORD dwSize = ::GetModuleFileName(NULL, m_szMainDir, ML_STRSIZE - 1);
    if ((dwSize > 6) && (dwSize < (ML_STRSIZE - 30))) {
        for (int_t ii = dwSize - 1; ii > 0; ii--) {
            if (m_szMainDir[ii] == _T('\\')) {
                m_szMainDir[ii + 1] = _T('\0');
                break;
            }
        }
    }
    else {
        return FALSE;
    }
    char_t *pszT = _tcsstr((char_t*) m_szMainDir, _T("\\bin"));
    if (pszT) {
        *(pszT + 1) = _T('\0');
    }

    _tcscpy(m_szHelpFilename, (const char_t*) m_szMainDir);
    _tcscat(m_szHelpFilename, _T("help\\sigmagraph.chm"));
    _tcscpy(m_szHelpFilenamePDF, (const char_t*) m_szMainDir);
    _tcscat(m_szHelpFilenamePDF, _T("help\\sigmagraph.pdf"));

    char_t szFilename[ML_STRSIZE];
    memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szFilename, (const char_t*) m_szMainDir);
    _tcscat(szFilename, _T("setup.txt"));
    m_bPortable = (CSigmaApp::fileExists((const char_t*) szFilename) == false);

    if (isPortable()) { // Portable distribution
        _tcscpy((char_t*) m_szUserDir, (const char_t*) m_szMainDir);
        _tcscat(m_szUserDir, _T("config\\"));
        CSigmaApp::makeDir((const char_t*) m_szUserDir);
        if (m_pszProfileName) {
            free((void*) m_pszProfileName);
            m_pszProfileName = NULL;
        }
        _tcscpy((char_t*) szFilename, (const char_t*) m_szUserDir);
        _tcscat((char_t*) szFilename, _T("settings.ini"));
        m_pszProfileName = ::_tcsdup((const char_t*) szFilename);
        return TRUE;
    }
    else if (::SHGetSpecialFolderPath(NULL, m_szUserDir, CSIDL_APPDATA, FALSE) == TRUE) {    // Get AppData Directory
        int_t nret = (int_t) _tcslen(m_szUserDir);
        if ((nret > 6) && (nret < (ML_STRSIZE - 30))) {
            if (m_szUserDir[nret - 1] != _T('\\')) {
                m_szUserDir[nret] = _T('\\');
                m_szUserDir[nret + 1] = _T('\0');
            }
            _tcscat(m_szUserDir, _T("SigmaGraph\\"));
            CSigmaApp::makeDir((const char_t*) m_szUserDir);
            if (m_pszProfileName) {
                free((void*) m_pszProfileName);
                m_pszProfileName = NULL;
            }
            _tcscpy((char_t*) szFilename, (const char_t*) m_szUserDir);
            _tcscat((char_t*) szFilename, _T("settings.ini"));
            m_pszProfileName = ::_tcsdup((const char_t*) szFilename);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL CSigmaApp::getPlotSettings()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    plot_t *pp = NULL;
    char_t szLoc[ML_STRSIZE];
    DWORD dwT = ML_STRSIZE;

    m_pDefaultPlot = new(std::nothrow) plot_t;    
    if (m_pDefaultPlot == NULL) {    // Error handling
        return FALSE;
    }

    memset(m_pDefaultPlot, 0, sizeof(plot_t));
    pl_reset(m_pDefaultPlot, 1);

    _tcscpy(szLoc, (const char_t*)m_szUserDir);
    _tcscat(szLoc, _T("PlotDefault.par"));

    ilen = (long_t)fl_filesize((const char_t*)szLoc);
    // Le fichier ne correspond pas à une donnée valide
    if (ilen != (long_t)sizeof(plot_v24_t)) {
        return FALSE;
    }

    if ((fp = _wfopen((const char_t*)szLoc, _T("rb"))) == NULL) {
        return FALSE;
    }

    itemlen = (int_t)sizeof(plot_v24_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    ilen = (long_t)fread(pitem, itemlen, 1, fp);
    if (ilen != 1) {
        free(pitem); pitem = NULL;
        fclose(fp); fp = NULL;
        return FALSE;
    }

    memcpy(m_pDefaultPlot, pitem, itemlen);
    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    pl_validate(m_pDefaultPlot, 1);
    if (m_pDefaultPlot->errcode != 0) {
        return FALSE;
    }

    return TRUE;
}

BOOL CSigmaApp::setPlotSettings()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    char_t szLoc[ML_STRSIZE];
    DWORD dwT = ML_STRSIZE;
    int_t jj;

    // Enlever tous les éléments dynamiques
    m_pDefaultPlot->dc = NULL;
    m_pDefaultPlot->frame = NULL;
    m_pDefaultPlot->title.font.ptr = NULL;
    m_pDefaultPlot->axisbottom.title.font.ptr = NULL;
    m_pDefaultPlot->axisleft.title.font.ptr = NULL;
    m_pDefaultPlot->axistop.title.font.ptr = NULL;
    m_pDefaultPlot->axisright.title.font.ptr = NULL;
    m_pDefaultPlot->axisbottom.lfont.ptr = NULL;
    m_pDefaultPlot->axisleft.lfont.ptr = NULL;
    m_pDefaultPlot->axistop.lfont.ptr = NULL;
    m_pDefaultPlot->axisright.lfont.ptr = NULL;
    m_pDefaultPlot->curvecount = 0;
    m_pDefaultPlot->curveactive = 0;
    m_pDefaultPlot->linevertcount = 0;
    m_pDefaultPlot->linehorzcount = 0;
    for (jj = 0; jj < SIGMA_MAXCURVES; jj++) {
        m_pDefaultPlot->curve[jj].points = 0;
        m_pDefaultPlot->curve[jj].x = NULL;
        m_pDefaultPlot->curve[jj].y = NULL;
        m_pDefaultPlot->curve[jj].ex = NULL;
        m_pDefaultPlot->curve[jj].ey = NULL;
        m_pDefaultPlot->curve[jj].legend.font.ptr = NULL;
    }
    if (m_pDefaultPlot->textcount > 0) {
        for (jj = 0; jj < m_pDefaultPlot->textcount; jj++) {
            m_pDefaultPlot->text[jj].font.ptr = NULL;
            memset(m_pDefaultPlot->text[jj].text, 0, ML_STRSIZE * sizeof(char_t));
        }
        m_pDefaultPlot->textcount = 0;
    }

    _tcscpy(szLoc, (const char_t*)m_szUserDir);
    _tcscat(szLoc, _T("PlotDefault.par"));

    if ((fp = _wfopen((const char_t*)szLoc, _T("wb"))) == NULL) {
        return FALSE;
    }

    itemlen = (long_t)sizeof(plot_v24_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    memcpy(pitem, m_pDefaultPlot, itemlen);
    fwrite(pitem, itemlen, 1, fp);
    
    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    return TRUE;
}

BOOL CSigmaApp::getUserFit()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    char_t szLoc[ML_STRSIZE];

    // User-defined fitting functions
    _tcscpy(szLoc, (const char_t*)m_szUserDir);
    _tcscat(szLoc, _T("UserFitModel.par"));

    ilen = (long_t)fl_filesize((const char_t*)szLoc);
    // Le fichier ne correspond pas à une donnée valide
    if ((ilen < (long_t)(sizeof(int_t) + sizeof(userfit_t))) || (ilen > (long_t)(sizeof(int_t) + (ML_FIT_MAXMODEL * sizeof(userfit_t))))) {
        return FALSE;
    }

    if ((fp = _wfopen((const char_t*)szLoc, _T("rb"))) == NULL) {
        return FALSE;
    }

    int_t itemcount = 0, jj;
    ilen = (long_t)fread(&itemcount, sizeof(int_t), 1, fp);
    if ((ilen != 1) || (itemcount < 1) || (itemcount > ML_FIT_MAXMODEL)) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    m_iUserFitCount = itemcount;

    itemlen = (int_t)sizeof(userfit_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }

    userfit_t *puf = NULL;
    for (jj = 0; jj < itemcount; jj++) {
        if (m_pUserFit[jj] == NULL) {
            free(pitem); pitem = NULL;
            fclose(fp); fp = NULL;
            return FALSE;
        }
        ilen = (long_t)fread(pitem, itemlen, 1, fp);
        if (ilen != 1) {
            free(pitem); pitem = NULL;
            fclose(fp); fp = NULL;
            return FALSE;
        }

        puf = (userfit_t*)pitem;

        memcpy(m_pUserFit[jj], puf, itemlen);
        m_pUserFit[jj]->index = ML_FIT_USERINDEX;
    }

    free(pitem); pitem = NULL;
    fclose(fp); fp = NULL;

    return TRUE;
}

BOOL CSigmaApp::setUserFit()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0;
    char_t szLoc[ML_STRSIZE];


    _tcscpy(szLoc, (const char_t*)m_szUserDir);
    _tcscat(szLoc, _T("UserFitModel.par"));
    if (m_iUserFitCount < 1) {
        ::DeleteFile((LPCTSTR)szLoc);
        return TRUE;
    }

    if ((fp = _wfopen((const char_t*)szLoc, _T("wb"))) == NULL) {
        return FALSE;
    }

    itemlen = (long_t)sizeof(int_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    memcpy(pitem, &m_iUserFitCount, itemlen);
    fwrite(pitem, itemlen, 1, fp);
    free(pitem); pitem = NULL;

    itemlen = (int_t)sizeof(userfit_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    int_t jj;
    for (jj = 0; jj < m_iUserFitCount; jj++) {
        memcpy(pitem, m_pUserFit[jj], itemlen);
        fwrite(pitem, itemlen, 1, fp);
    }
    free(pitem); pitem = NULL;

    fclose(fp); fp = NULL;

    return TRUE;
}

BOOL CSigmaApp::getOptions()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    options_t *pp = NULL;
    char_t szLoc[ML_STRSIZE];
    DWORD dwT = ML_STRSIZE;

    memset(&m_Options, 0, sizeof(options_t));

    m_Options.winstate = 0x00;

    m_Options.console.angleunit = 0x00;
    m_Options.console.font.color.r = 0;
    m_Options.console.font.color.g = 0;
    m_Options.console.font.color.b = 0;
    _tcscpy(m_Options.console.font.name, _T("Lucida Sans Unicode"));
    m_Options.console.font.size = 10;
    m_Options.console.font.style = 0x00;
    m_Options.console.tol = m_pLib->tol;
    m_Options.console.reltol = m_pLib->reltol;

    m_Options.script.font.color.r = 0;
    m_Options.script.font.color.g = 0;
    m_Options.console.font.color.b = 0;
    _tcscpy(m_Options.script.font.name, SigmaApp.isWinXP() ? _T("Courier New") : _T("Consolas"));
    m_Options.script.font.size = 10;
    m_Options.script.font.style = 0x00;

    m_Options.import.cSep = _T('\t');
    m_Options.import.iAdd = 0;
    m_Options.import.iCol = 2;
    m_Options.import.iRow = 32;
    m_Options.import.iSkip = 0;
    m_Options.import.iTitle = 1;
    m_Options.import.iType = 0;
    m_Options.import.iShow = 1;

    memset(&(m_Options.fit), 0, sizeof(fit_t));
    m_Options.fit.iter = m_pLib->maxiter;
    m_Options.fit.lambda = 10.0;

    memset(m_Options.opt, 0, 32 * sizeof(byte_t));
    m_Options.opt[0] = 1;                   // (Statusbar) visible ?
    m_Options.opt[1] = 1;                   // (Toolbar - Main) visible ?
    m_Options.opt[2] = 1;                   // (Toolbar - Datasheet) visible ?
    m_Options.opt[3] = 1;                   // (Toolbar - Graph) visible ?
    m_Options.opt[AUTOEXPORT_INDEX] = 1;    // AutoExport ?

    _tcscpy(szLoc, (const char_t*)m_szUserDir);
    _tcscat(szLoc, _T("MainOptions.par"));

    ilen = (long_t)fl_filesize((const char_t*)szLoc);
    // Le fichier ne correspond pas à une donnée valide
    if (ilen != (long_t)sizeof(options_t)) {
        return FALSE;
    }

    if ((fp = _wfopen((const char_t*)szLoc, _T("rb"))) == NULL) {
        return FALSE;
    }

    itemlen = (int_t)sizeof(options_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    ilen = (long_t)fread(pitem, itemlen, 1, fp);
    if (ilen != 1) {
        free(pitem); pitem = NULL;
        fclose(fp); fp = NULL;
        return FALSE;
    }

    pp = (options_t*)pitem;

    memcpy(&m_Options, pp, itemlen);

    if (m_Options.import.iRow == 0) {
        m_Options.import.cSep = _T('\t');
        m_Options.import.iAdd = 0;
        m_Options.import.iCol = 2;
        m_Options.import.iRow = 32;
        m_Options.import.iTitle = 1;
        m_Options.import.iType = 0;
    }
    m_Options.import.iSkip = 0;    // Always iSkip initialize to zero
    m_Options.import.iShow = 1;

    // Quelques vérifications
    if ((m_Options.console.tol < (DBL_EPSILON * 10.0)) || (m_Options.console.tol > 1.0)) {
        m_Options.console.tol = m_pLib->tol;
    }
    else {
        m_pLib->tol = m_Options.console.tol;
    }
    if ((m_Options.console.reltol < (DBL_EPSILON * 10.0)) || (m_Options.console.reltol > 1.0)) {
        m_Options.console.reltol = m_pLib->reltol;
    }
    else {
        m_pLib->reltol = m_Options.console.reltol;
    }
    if ((m_Options.fit.lambda < ML_FIT_LAMBDAMIN) || (m_Options.fit.lambda > ML_FIT_LAMBDAMAX)) {
        m_Options.fit.lambda = 10.0;
    }
    if ((m_Options.fit.iter < 1) || (m_Options.fit.iter > ML_MAXITER)) {
        m_Options.fit.iter = m_pLib->maxiter;
    }
    else {
        m_pLib->maxiter = m_Options.fit.iter;
    }
    //

    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    return TRUE;
}

BOOL CSigmaApp::setOptions()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0;
    char_t szLoc[ML_STRSIZE];

    _tcscpy(szLoc, (const char_t*)m_szUserDir);
    _tcscat(szLoc, _T("MainOptions.par"));

    if ((fp = _wfopen((const char_t*)szLoc, _T("wb"))) == NULL) {
        return FALSE;
    }

    itemlen = (long_t)sizeof(options_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    memcpy(pitem, &m_Options, itemlen);
    fwrite(pitem, itemlen, 1, fp);
    
    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    return TRUE;
}

BOOL CSigmaApp::InitInstance()
{
    m_bPortable = true;

    // Information sur le logiciel
    g_Sigmaware.version = SIGMA_SOFTWARE_VERSION;
    _tcscpy(g_Sigmaware.name, SIGMA_SOFTWARE_NAME);
    _tcscpy(g_Sigmaware.description, SIGMA_SOFTWARE_DESCRIPTION);
    _tcscpy(g_Sigmaware.copyright, SIGMA_SOFTWARE_COPYRIGHT);
    _tcscpy(g_Sigmaware.date, SIGMA_SOFTWARE_DATERELEASE);
    _tcscpy(g_Sigmaware.time, SIGMA_SOFTWARE_TIMERELEASE);
    _tcscpy(g_Sigmaware.reserved, SIGMA_SOFTWARE_RESERVED);

    if (m_pszAppName) {
        free((void*)m_pszAppName);
        m_pszAppName = NULL;
    }
    m_pszAppName = _tcsdup(_T("SigmaGraph"));

    m_pDefaultPlot = NULL;

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    if (!AfxOleInit()) {
        return FALSE;
    }

    Scintilla_RegisterClasses(GetModuleHandle(NULL));
    Scintilla_LinkLexers();

    // Interaction with Lib
    if (m_pLib == NULL) {
        m_pLib = new(std::nothrow) libvar_t;
        if (m_pLib == NULL) {    // Error handling
            return FALSE;
        }
    }
    ASSERT(m_pLib);
    ml_reset(m_pLib);

    // User preferences
    getUserDir();
    getOptions();
    ml_angleunit = m_Options.console.angleunit;
    getPlotSettings();
    if (m_pDefaultPlot == NULL) {
        return FALSE;
    }
    ::SetCurrentDirectory(m_szUserDir);

    LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

    // Fitting
    int_t iter = 10, jj;
    real_t lambda = 10.0;
    ml_fit_init(&iter, &lambda);
    m_iUserFitCount = 0;
    for (jj = 0; jj < ML_FIT_MAXMODEL; jj++) {
        m_pUserFit[jj] = NULL;
        m_pUserFit[jj] = (userfit_t*)malloc(sizeof(userfit_t));
        if (m_pUserFit[jj] != NULL) {
            memset(m_pUserFit[jj], 0, sizeof(userfit_t));
        }
    }
    getUserFit();

    int_t nSize = 0;
    if (m_pRecentFileList != NULL) {
        nSize = m_pRecentFileList->GetSize();
    }
    if (nSize > 8) {
        m_pRecentFileList->m_arrNames->Empty();
        m_pRecentFileList->m_nSize = 0;
    }
    if (nSize > 0) {
        int_t ii = 0, jj;
        HANDLE hFile = INVALID_HANDLE_VALUE;
        for (jj = 0; jj < nSize; jj++) {
            hFile = ::CreateFile((LPCTSTR)((*m_pRecentFileList)[ii]), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE) {
                CloseHandle(hFile);
                ii += 1;
            }
            else {
                m_pRecentFileList->Remove(ii);
            }
        }
    }

    m_pTemplatePlot = new CMultiDocTemplate(
        IDR_DOCTYPE_PLOT,
        RUNTIME_CLASS(CSigmaDoc),
        RUNTIME_CLASS(CChildFramePlot),        // custom MDI child frame
        RUNTIME_CLASS(CSigmaViewPlot));
    if (m_pTemplatePlot == NULL) {
        return FALSE;
    }
    AddDocTemplate(m_pTemplatePlot);

    m_pTemplateData = new CMultiDocTemplate(
        IDR_DOCTYPE_DATA,
        RUNTIME_CLASS(CSigmaDoc),
        RUNTIME_CLASS(CChildFrameData),        // custom MDI child frame
        RUNTIME_CLASS(CSigmaViewData));
    if (m_pTemplateData == NULL) {
        return FALSE;
    }
    AddDocTemplate(m_pTemplateData);

    m_pTemplateDataFit = new CMultiDocTemplate(
        IDR_DOCTYPE_DATA,
        RUNTIME_CLASS(CSigmaDoc),
        RUNTIME_CLASS(CChildFrameDataFit),        // custom MDI child frame
        RUNTIME_CLASS(CSigmaViewDataFit));
    if (m_pTemplateDataFit == NULL) {
        return FALSE;
    }
    AddDocTemplate(m_pTemplateDataFit);

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME)) {
        delete pMainFrame;
        return FALSE;
    }
    m_pMainWnd = pMainFrame;

    // Numeric format
    m_Workspace.bFormat = 0x00;
    _tcscpy(m_Workspace.szFormatF, ML_FORMAT_FSHORT);
    _tcscpy(m_Workspace.szFormatE, ML_FORMAT_ESHORT);
    memset(ml_format_f, 0, ML_STRSIZET * sizeof(char_t));
    memset(ml_format_e, 0, ML_STRSIZET * sizeof(char_t));
    _tcscpy(ml_format_f, ML_FORMAT_FSHORT);
    _tcscpy(ml_format_e, ML_FORMAT_ESHORT);

    // Persistent Data
    m_nDocCount = 0;                        // Current document count. To be incremented as new document is created.
    m_nDocID = 1;

    // Performance counter
    m_liPerfCount.HighPart = 0;
    m_liPerfCount.LowPart = 0;
    m_liPerfCount.QuadPart = 0;
    m_uiPerfCount = 0;

    // Parser
    m_nVarnum = 0;
    const char_t *szAZ[_T('z') - _T('a') + 1] = {_T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("h"), _T("i"), _T("j"), _T("k"), _T("l"), _T("m"),
        _T("n"), _T("o"), _T("p"), _T("q"), _T("r"), _T("s"), _T("t"), _T("u"), _T("v"), _T("w"), _T("x"), _T("y"), _T("z")};

    for (int_t ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {    
        _tcscpy(m_szParam[ii], szAZ[ii]);        // Params name : by default : a, b, c, ..., z
        m_fParam[ii] = 0.0;
        m_bParam[ii] = FALSE;                        // Param set ?
    }

    m_fAns = 0.0;
    m_bAns = false;

    // Copy Plot Style
    m_pPlotCopy = NULL;
    m_pPlotCopy = new(std::nothrow) plot_t;
    memset(m_pPlotCopy, 0, sizeof(plot_t));
    m_bCanPastePlot = FALSE;

    // Drag and Drop
    m_bDropping = FALSE;

    BOOL bDragShowContent = FALSE;
    ::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bDragShowContent, 0);

    // Enable DDE Execute open
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    CString strFilename = cmdInfo.m_strFileName;

    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo)) {
        return FALSE;
    }

    m_nCmdShow = SW_RESTORE;
    if (0x01 == SigmaApp.m_Options.winstate) {
        m_nCmdShow = SW_SHOWMAXIMIZED;
    }
    else {
        // The main window has been initialized, so show and update it
        WINDOWPLACEMENT tPlacementMain;
        memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
        tPlacementMain.length = sizeof(WINDOWPLACEMENT);
        SigmaApp.GetWindowPlacement(_T("MainPlace.par"), &tPlacementMain, SIGMA_DEFWIDTH_MAIN, SIGMA_DEFHEIGHT_MAIN);
        pMainFrame->SetWindowPlacement(&tPlacementMain);
    }

    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();

    // Initialize GDI+.
    GdiplusStartup(&m_GdiplusToken, &m_GdiplusStart, NULL);

    BOOL bNew = TRUE;

    memset(&m_LastPlacement, 0, sizeof(WINDOWPLACEMENT));
    m_LastPlacement.length = sizeof(WINDOWPLACEMENT);

    int_t nlen = strFilename.GetLength();
    if ((nlen > 5) && (nlen < (ML_STRSIZE - 7))) {
        char_t szExt[5];
        memset(szExt, 0, 5 * sizeof(char_t));
        szExt[0] = strFilename[nlen - 4];
        szExt[1] = strFilename[nlen - 3];
        szExt[2] = strFilename[nlen - 2];
        szExt[3] = strFilename[nlen - 1];
        szExt[4] = _T('\0');
        if ((_tcsicmp((const char_t*)szExt, _T(".sid")) == 0)
            || (_tcsicmp((const char_t*)szExt, _T(".txt")) == 0)
            || (_tcsicmp((const char_t*)szExt, _T(".dat")) == 0)
            || (_tcsicmp((const char_t*) szExt, _T(".csv")) == 0)
            || (_tcsicmp((const char_t*) szExt, _T(".log")) == 0)) {
            bNew = FALSE;
            m_bDropping = TRUE;
            CStringToChar(strFilename, m_szDropFilename, ML_STRSIZE - 7);
            OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL);
        }
        else if (_tcsicmp((const char_t*)szExt, _T(".lua")) == 0) {
            bNew = FALSE;
            if ((pMainFrame->m_pwndScript == NULL) || (pMainFrame->m_pwndScript->IsWindowVisible() == FALSE)) {
                pMainFrame->OnCmdMsg(ID_VIEW_SCRIPT, 0, 0, 0);
            }
            if (pMainFrame->m_pwndScript) {
                pMainFrame->m_pwndScript->ScriptFileOpen((LPCTSTR)strFilename);
            }
        }
    }

    if (bNew) {
        OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL);
    }

    CMenu *pMenu = pMainFrame->GetMenu();
    if (pMenu) {
        CMenu *pSubMenu = pMenu->GetSubMenu(0);
        if (pSubMenu) {
            pSubMenu->CheckMenuItem(ID_FILE_AUTOEXPORT, (1 == SigmaApp.m_Options.opt[AUTOEXPORT_INDEX]) ? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED));
        }
    }

    return TRUE;
}

CMDIFrameWnd *CSigmaApp::ActivateMainframe(void)
{
    CMDIFrameWnd* pFrame = (CMDIFrameWnd*) AfxGetMainWnd();

    if (pFrame == NULL) {
        return NULL;
    }

    WINDOWPLACEMENT tPlacementMain;
    memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
    tPlacementMain.length = sizeof(WINDOWPLACEMENT);
    pFrame->GetWindowPlacement(&tPlacementMain);

    if (tPlacementMain.showCmd == SW_SHOWMINIMIZED) {
        pFrame->ShowWindow(SW_RESTORE);
    }

    pFrame->SetForegroundWindow();

    return pFrame;
}

BOOL CSigmaApp::PtOnLine(const irect_t *precT, CPoint ptT, byte_t orient/* = 0*/)
{
    const int_t nn = 20;
    int_t ii;
    real_t ptx, pty, ft, fx, fx0, fxmax, fxmin, fymax, fymin, fy, fyy, fy0, dx;
    POINT ptTT;

    ptTT.x = ptT.x;
    ptTT.y = ptT.y;

    if (precT == NULL) {
        return FALSE;
    }

    ptx = (real_t)(ptT.x);
    pty = (real_t)(ptT.y);

    fx = (real_t)(precT->right - precT->left);
    fy = (real_t)(precT->bottom - precT->top);

    fxmin = (real_t)(precT->left);
    fxmax = (real_t)(precT->right);
    fymin = (real_t)(precT->top);
    fymax = (real_t)(precT->bottom);

    if (fx < PL_RESIZE) {
        if ((ptx >= (fxmin - PL_RESIZE)) && (ptx <= (fxmin + PL_RESIZE)) && (pty >= fymin) && (pty <= fymax)) {
            return TRUE;
        }
        return FALSE;
    }
    if (fy < PL_RESIZE) {
        if ((pty >= (fymin - PL_RESIZE)) && (pty <= (fymin + PL_RESIZE)) && (ptx >= fxmin) && (ptx <= fxmax)) {
            return TRUE;
        }
        return FALSE;
    }

    if (::PtInRect(precT, ptTT) == FALSE) {
        return FALSE;
    }

    if ((ptT.x < precT->left) || (ptT.x > precT->right) || (ptT.y < precT->top) || (ptT.y > precT->bottom)) {
        return FALSE;
    }

    ft = fy / fx;
    dx = fx / ((real_t)(nn - 1));

    fx0 = (real_t)(precT->left);
    
    if (orient == 0) {
        fy0 = (real_t)(precT->top);
    }
    else {
        fy0 = (real_t)(precT->bottom);
    }

    fx = fx0;

    ii = 0;
    for (ii = 0; ii < nn; ii++) {
        if (orient == 0) {
            fy = fy0 + ft * (fx - dx - fx0);
            fyy = fy0 + ft * (fx + dx - fx0);
        }
        else {
            fy = fy0 - ft * (fx + dx - fx0);
            fyy = fy0 - ft * (fx - dx - fx0);
        }
        if ((ptx >= (fx - dx)) && (ptx <= (fx + dx)) && (pty >= fy) && (pty <= fyy)) {
            return TRUE;
        }
        fx += dx;
    }
    return FALSE;
}

int_t CSigmaApp::CStringToChar(CString strSrc, char_t* szDest, int_t nMax, BOOL bDelCRLF/* = FALSE*/)
{
    int_t ii, nlen = 0;

    if ((nMax < 1) || (nMax >= ML_STRSIZE)) {
        return -1;
    }

    nlen = strSrc.GetLength();
    if ((nlen < 0) || (nlen > nMax)) {
        return -1;
    }
    if (nlen == 0) {
        szDest[0] = _T('\0');
        return 0;
    }
    if (nlen == 1) {
        if (bDelCRLF) {
            if ((strSrc[0] == _T('\r')) || (strSrc[0] == _T('\n'))) {
                szDest[0] = _T('\0');
                return 0;
            }
        }
        szDest[0] = (char_t) strSrc[0];
        szDest[1] = _T('\0');
        return 1;
    }

    for (ii = 0; ii < nlen; ii++) {
        szDest[ii] = (char_t) strSrc[ii];
    }
    szDest[nlen] = _T('\0');

    if (bDelCRLF) {
        char_t cT;
        int ilen = nlen;
        for (ii = (nlen - 1); ii >= 0; ii--) {
            cT = szDest[ii];
            if ((cT != _T('\r')) && (cT != _T('\n')) && (cT != _T(' ')) && (cT != _T('\t'))) {
                ilen = ii + 1;
                szDest[ilen] = _T('\0');
                break;
            }
        }
        if (ii <= 0) {
            szDest[0] = _T('\0');
            ilen = 0;
        }
        nlen = ilen;
    }

    return nlen;
}

int_t CSigmaApp::CStringReverseFind(CString strT, LPCTSTR szFind)
{
    char_t *pszBuffer = (char_t*)(strT.GetBuffer());
    if (pszBuffer == NULL) {
        return -1;
    }

    char_t *pszNext = NULL, *pszBuffer0;
    int_t iLen = (int_t) _tcslen((const char_t*)pszBuffer),
        iLenFind = (int_t) _tcslen((const char_t*)szFind);
    int_t iPos = -1;

    if ((iLen < 2) || (iLen > (ML_STRSIZEL * 8))
        || (iLenFind < 1) || (iLenFind > ML_STRSIZE)) {
        return -1;
    }

    pszBuffer0 = pszBuffer;
    pszNext = _tcsstr(pszBuffer, (const char_t*)szFind);
    while (pszNext) {
        iPos = pszNext - pszBuffer0;
        pszBuffer = pszNext + iLenFind;
        pszNext = _tcsstr(pszBuffer, (const char_t*)szFind);
    }
    
    return iPos;
}

BOOL CSigmaApp::GetFont(HWND hWnd, LOGFONT *plf)
{ 
    CHOOSEFONT cf; 
 
    cf.lStructSize = sizeof(CHOOSEFONT); 
    cf.hwndOwner = hWnd; 
    cf.hDC = (HDC)NULL; 
    cf.lpLogFont = plf; 
    cf.iPointSize = 0; 
    cf.Flags = CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS; 
    cf.rgbColors = RGB(0,0,0); 
    cf.lCustData = 0L; 
    cf.lpfnHook = (LPCFHOOKPROC)NULL; 
    cf.lpTemplateName = NULL; 
    cf.hInstance = (HINSTANCE) NULL; 
    cf.lpszStyle = NULL; 
    cf.nFontType = SCREEN_FONTTYPE; 
    cf.nSizeMin = 0; 
    cf.nSizeMax = 0; 

    if (::ChooseFont(&cf)) {
        return TRUE;
    }

    return FALSE;
}

BOOL CSigmaApp::GetColor(HWND hWnd, COLORREF *pcr)
{
    CHOOSECOLOR cc;
    COLORREF acrCustClr[16];

    ::ZeroMemory(&cc, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hWnd;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = *pcr;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (::ChooseColor(&cc)) {
        *pcr = cc.rgbResult;
        return TRUE;
    }

    return FALSE;
}

void CSigmaApp::Reformat(real_t fVal, char_t* szBuffer, int_t nCount, char_t* pszFormat/* = NULL*/, int_t *pFmt/* = NULL*/)
{
    if (m_Workspace.bFormat == 0x09) {
        _tcsprintf(szBuffer, nCount, _T("%.0f"), fVal);
        return;
    }

    m_pLib->errcode = 0;
    ml_reformat_auto(fVal, szBuffer, nCount, pszFormat, pFmt, m_pLib);
}

// MOD-20100412
void CSigmaApp::ResizePane(int_t nPane, CString strT)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

    if (pFrame == NULL) {
        return;
    }

    if (::IsWindow(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (::IsWindowVisible(pFrame->m_hWnd) == FALSE) {
        return;
    }

    int_t nLenT = (int_t) (strT.GetLength());
    if ((nLenT < 1) || (nLenT > ML_STRSIZE)) {
        return;
    }

    CDC *pDC = pFrame->GetDC();    // GetDC:003
    if (pDC) {
        CFont *pFont = pFrame->GetStatusbarFont();
        if (pFont) {
            int_t cxWidth;
            UINT nID, nStyle;

            CFont *pFontOld = pDC->SelectObject(pFont);

            CSize size = pDC->GetTextExtent(strT);
            pFrame->GetPaneInfo(nPane, nID, nStyle, cxWidth);
            pFrame->SetPaneInfo(nPane, nID, nStyle, size.cx);

            pDC->SelectObject(pFontOld);
        }
        pFrame->ReleaseDC(pDC);        // ReleaseDC:003
        pDC = NULL;
    }
}

void CSigmaApp::OutputSB(CString strT, int_t nStatusbarPane/* = 0*/, BOOL bAutoClear/* = FALSE*/)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

    if (pFrame == NULL) {
        return;
    }

    if (::IsWindow(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (::IsWindowVisible(pFrame->m_hWnd) == FALSE) {
        return;
    }

    int_t nLength = (int_t) (strT.GetLength());
    if (nLength > ML_STRSIZE) {
        return;
    }
    if (nLength < 2) {
        pFrame->SetStatusbarInfo(_T(""), nStatusbarPane);
        return;
    }

    if (strT[nLength - 2] == _T('\r') || strT[nLength - 2] == _T('\n')) {
        strT.SetAt(nLength - 2, _T('\0'));
    }
    else if (strT[nLength - 1] == _T('\r') || strT[nLength - 1] == _T('\n')) {
        strT.SetAt(nLength - 1, _T('\0'));
    }
    pFrame->SetStatusbarInfo(strT, nStatusbarPane, bAutoClear);
}

BOOL CSigmaApp::LogfontToFont(LOGFONT lft, font_t *pfnt)
{
    if (pfnt == NULL) {
        return FALSE;
    }

    HDC hDCX = ::GetDC(NULL);
    if (hDCX) {
        pfnt->size = -MulDiv(lft.lfHeight, 72, ::GetDeviceCaps(hDCX, LOGPIXELSY));
        ::ReleaseDC(NULL, hDCX);
        hDCX = NULL;
    }

    _tcscpy(pfnt->name, (const char_t*)(lft.lfFaceName));
    if (lft.lfWeight == FW_BOLD) {
        pfnt->style = 0x10;
    }
    else {
        pfnt->style = 0x00;
    }
    pfnt->style &= 0xF0;
    if (lft.lfItalic) {
        pfnt->style |= 0x01;
    }
    if (lft.lfUnderline) {
        pfnt->style |= 0x02;
    }

    return TRUE;
}

BOOL CSigmaApp::FontToLogfont(font_t fnt, LOGFONT *plft)
{
    if (plft == NULL) {
        return FALSE;
    }

    HDC hDCX = ::GetDC(NULL);
    if (hDCX) {
        plft->lfHeight = -MulDiv(fnt.size, ::GetDeviceCaps(hDCX, LOGPIXELSY), 72);
        ::ReleaseDC(NULL, hDCX);
        hDCX = NULL;
    }

    _tcscpy(plft->lfFaceName, (const char_t*)(fnt.name));
    if (fnt.style & 0x10) {
        plft->lfWeight = FW_BOLD;
    }
    else {
        plft->lfWeight = FW_NORMAL;
    }
    if (fnt.style & 0x01) {
        plft->lfItalic = 1;
    }
    if (fnt.style & 0x02) {
        plft->lfUnderline = 1;
    }

    return TRUE;
}

// Performance counter
void CSigmaApp::CounterStart()
{
    m_uiPerfCount = 0;
    m_liPerfCount.HighPart = 0;
    m_liPerfCount.LowPart = 0;
    m_liPerfCount.QuadPart = 0;
    ::QueryPerformanceCounter(&m_liPerfCount);
}
void CSigmaApp::CounterStop()
{
    LARGE_INTEGER lstop, lfreq;

    if (m_liPerfCount.QuadPart == 0) {
        m_liPerfCount.HighPart = 0;
        m_liPerfCount.LowPart = 0;
        m_uiPerfCount = 0;
        return;
    }

    ::QueryPerformanceCounter(&lstop);
    ::QueryPerformanceFrequency(&lfreq);

    lstop.QuadPart -= m_liPerfCount.QuadPart;
    lstop.QuadPart *= 1000000;
    lstop.QuadPart /= lfreq.QuadPart;

    if (lstop.HighPart != 0) {
        m_uiPerfCount = 0;
    }
    else {
        m_uiPerfCount = (ulong_t)(lstop.LowPart);
    }
}

// >> GLOBAL C-STYLE FUNCTIONS
char_t* g_pszFunction = NULL;
real_t funfx(real_t x)
{
    if (g_pszFunction == NULL) {
        return 0.0;
    }
    return ml_parser_evalfx(g_pszFunction, x, SigmaApp.m_pLib);
}
// <<


void CSigmaApp::ResizeParam(void)
{
    const char_t *szAZ[_T('z') - _T('a') + 1] = {_T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("h"), _T("i"), _T("j"), _T("k"), _T("l"), _T("m"),
        _T("n"), _T("o"), _T("p"), _T("q"), _T("r"), _T("s"), _T("t"), _T("u"), _T("v"), _T("w"), _T("x"), _T("y"), _T("z")};

    if (m_nVarnum != (_T('z') - _T('a') + 1)) {
        return;
    }

    for (int_t ii = 0; ii < (_T('z') - _T('a')); ii++) {    
        _tcscpy(m_szParam[ii], m_szParam[ii + 1]);            // Params name : by default : a, b, c, ..., z
        m_fParam[ii] = m_fParam[ii + 1];
        m_bParam[ii] = m_bParam[ii + 1];                            // Param set ?
    }
    m_fParam[_T('z') - _T('a')] = 0.0;
    _tcscpy(m_szParam[_T('z') - _T('a')], szAZ[_T('z') - _T('a')]);        // Params name : by default : a, b, c, ..., z
    m_bParam[_T('z') - _T('a')] = FALSE;
    m_nVarnum -= 1;
}

BOOL CSigmaApp::DoParse(const char_t* szInput, char_t* szOutput, real_t* pVal/* = NULL*/, const char_t* pszFormat/* = NULL*/)
{
    CString strExpr;
    int_t nlen, nslen, nplen, nvlen, ii, jj, ll, ne, ns;
    char_t szExpression[ML_STRSIZE], szFunction[ML_STRSIZE], szTmp[ML_STRSIZE],
        szVar[ML_STRSIZET], *pszFunction = NULL, *pszTT = NULL;
    char_t szBuffer[ML_STRSIZE], *pszT = NULL;
    real_t ftmp;
    enum _iParse {MATH = 0, COMMAND} iParse;
    enum _iOp {EXPR = 0, ASSIGN, PRINT, SOLVE} iOp;
    char_t cVar;
    BOOL bFound = FALSE, bRep = FALSE, bMathParse = FALSE;

    // Verifications
    if (szOutput == NULL) {
        return FALSE;
    }

    szOutput[0] = _T('\0');

    if (pVal) { *pVal = 0.0; }

    nlen = (int_t)_tcslen((const char_t*)szInput);
    if (nlen < 2) {
        if ((nlen == 1) && (_istdigit(szInput[0]))) {
            szOutput[0] = szInput[0];
            szOutput[1] = _T('\0');
            return TRUE;
        }
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    if (nlen > 250) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    if ((char_t*)_tcsstr((const char_t*)szInput, _T("\"")) != NULL) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    _tcsncpy(szExpression, szInput, nlen);
    szExpression[nlen] = _T('\0');
    nlen = (int_t)_tcslen((const char_t*)szExpression);
    if (nlen < 2) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    _tcscpy(szOutput, _T("! Invalid expression."));

    if (nlen > 250) {
        return FALSE;
    }

    // Command parsing
    iParse = COMMAND;
    if (_tcsicmp((const char_t*)szExpression, _T("author")) == 0) {
        _tcscpy(szOutput, _T("Sidi OULD SAAD HAMADY\r\nsidi@hamady.org"));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("stop")) == 0) {
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("help")) == 0) {
        _tcscpy(szOutput, _T("For help, please click on the Help menu."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("exit")) == 0) {
        OnCmdMsg(ID_APP_EXIT, 0, 0, 0);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("quit")) == 0) {
        OnCmdMsg(ID_APP_EXIT, 0, 0, 0);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("save")) == 0) {
        OnCmdMsg(ID_FILE_SAVE, 0, 0, 0);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("new")) == 0) {
        OnCmdMsg(ID_FILE_NEW, 0, 0, 0);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format long")) == 0) {
        _tcscpy(m_Workspace.szFormatF, ML_FORMAT_FLONG);
        _tcscpy(m_Workspace.szFormatE, ML_FORMAT_ELONG);
        m_Workspace.bFormat = 0x00;
        _tcscpy(ml_format_f, ML_FORMAT_FLONG);
        _tcscpy(ml_format_e, ML_FORMAT_ELONG);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format short")) == 0) {
        _tcscpy(m_Workspace.szFormatF, ML_FORMAT_FSHORT);
        _tcscpy(m_Workspace.szFormatE, ML_FORMAT_ESHORT);
        m_Workspace.bFormat = 0x00;
        _tcscpy(ml_format_f, ML_FORMAT_FSHORT);
        _tcscpy(ml_format_e, ML_FORMAT_ESHORT);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format int")) == 0) {
        m_Workspace.bFormat = 0x09;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format auto")) == 0) {
        m_Workspace.bFormat = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("tol ?")) == 0) {
        Reformat(m_pLib->tol, szOutput, ML_STRSIZE - 1);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("iter ?")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d"), m_pLib->maxiter);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("radian")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("rad")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit radian")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit rad")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("degree")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("deg")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit degree")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit deg")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("grad")) == 0) {
        ml_angleunit = 0x02;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit grad")) == 0) {
        ml_angleunit = 0x02;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit ?")) == 0) {
        if (ml_angleunit == 0x00) {
            _tcscpy(szOutput, _T("angle unit: radian."));
        }
        else if (ml_angleunit == 0x01) {
            _tcscpy(szOutput, _T("angle unit: degree."));
        }
        else if (ml_angleunit == 0x02) {
            _tcscpy(szOutput, _T("angle unit: grad."));
        }
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("var"))) != NULL) {
        if (((CMainFrame*)m_pMainWnd)->m_pwndConsole) {
            ((CMainFrame*)m_pMainWnd)->m_pwndConsole->OnViewVarlist();
        }
        _tcscpy(szOutput, _T("done."));
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("unset all"))) != NULL) {
        for (char_t cc = _T('a'); cc <= _T('z'); cc++) {
            m_bParam[cc - _T('a')] = FALSE;
            m_szParam[cc - _T('a')][0] = cc;
            m_szParam[cc - _T('a')][1] = _T('\0');
        }
        m_nVarnum = 0;
        _tcscpy(szOutput, _T("done."));
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("unset "))) != NULL) {
        if ((pszT += _tcslen(_T("unset "))) != NULL) {
            for (char_t cc = _T('a'); cc <= _T('z'); cc++) {
                if (_tcsicmp((const char_t*)pszT, (const char_t*)m_szParam[cc - _T('a')]) == 0) {
                    m_bParam[cc - _T('a')] = FALSE;
                    m_szParam[cc - _T('a')][0] = cc;
                    m_szParam[cc - _T('a')][1] = _T('\0');
                    m_nVarnum -= 1;
                    _tcscpy(szOutput, _T("done."));
                    break;
                }
            }
        }
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("set ?")) == 0) {
        szOutput[0] = _T('\0');
        ii = 0;
        int_t nT;
        for (char_t cc = _T('a'); cc <= _T('z'); cc++) {
            if (m_bParam[cc - _T('a')] == TRUE) {    // Var set
                nT = _tcslen((const char_t*)szOutput);
                if (nT > (ML_STRSIZE - ML_STRSIZES - 3)) {
                    _tcscat(szOutput, _T("...,"));
                    break;
                }
                _tcscat(szOutput, (const char_t*)m_szParam[cc - _T('a')]);
                _tcscat(szOutput, _T(","));
            }
        }
        nT = _tcslen((const char_t*)szOutput);
        if (nT < 2) {
            _tcscpy(szOutput, _T("variable not set."));
        }
        else {
            szOutput[nT - 1] = _T('\0');
        }
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("reset")) == 0) {
        for (jj = 0; jj < ML_PARSER_VARNUM; jj++) {
            m_bParam[jj] = FALSE;
            m_fParam[jj] = 0.0;
            memset(m_szParam[jj], 0, ML_STRSIZET * sizeof(char_t));
        }
        m_nVarnum = 0;
        _tcscpy(szOutput, _T("done."));
        //
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("name")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.name);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("version")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d.%d.%d"), (g_Sigmaware.version >> 12) & 0x000F, (g_Sigmaware.version >> 8) & 0x000F, (g_Sigmaware.version >> 4) & 0x000F);
#ifdef SIGMA_SOFTWARE_VERSION_B
        int_t ilenx = (int_t)_tcslen(SIGMA_SOFTWARE_VERSION_B);
        if ((ilenx > 0) && (ilenx < ML_STRSIZET)) {
            _tcscat(szOutput, _T("-"));
            _tcscat(szOutput, SIGMA_SOFTWARE_VERSION_B);
#endif
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("release")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.date);
        _tcscat(szOutput, _T(" "));
        _tcscat(szOutput, g_Sigmaware.time);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("copyright")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.copyright);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("description")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.description);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("libmath version")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d.%d.%d"), (ml_version >> 12) & 0x000F, (ml_version >> 8) & 0x000F, (ml_version >> 4) & 0x000F);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("libplot version")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d.%d.%d"), (pl_version >> 12) & 0x000F, (pl_version >> 8) & 0x000F, (pl_version >> 4) & 0x000F);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("libfile version")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d.%d.%d"), (fl_version >> 12) & 0x000F, (fl_version >> 8) & 0x000F, (fl_version >> 4) & 0x000F);
    }
    else {
        iParse = MATH;
    }
    // Valid command found
    if (iParse != MATH) {
        return TRUE;
    }

    // Mathematical parsing
    int_t ispace = ml_remspace(szExpression);
    if ((ispace > 0) && (ispace < nlen)) {
        nlen -= ispace;
    }

    memset((char_t*)szVar, 0, sizeof(char_t) * ML_STRSIZET);
    memset((char_t*)szFunction, 0, sizeof(char_t) * ML_STRSIZE);
    jj = 0;    ne = 0;    ns = 0;
    int_t nVarNotSet = 0;
    for (ii = 0; ii < nlen; ii++) {
        if ((szExpression[ii] != _T('\r')) && (szExpression[ii] != _T('\n'))
             && (szExpression[ii] != _T('\t')) && (szExpression[ii] != _T(' '))) {
            szFunction[jj] = _totlower(szExpression[ii]);
            if ((szFunction[jj] == _T('=')) && (ne == 0)) {
                ne = jj;
            }
            if ((jj >= 2) && (jj < nlen) && (ns == 0)) {
                if ((szFunction[jj - 2] == _T('-')) && (szFunction[jj - 1] == _T('>')) && (szFunction[jj] == _T('0'))) {
                    ns = jj - 2;
                }
            }
            jj += 1;
        }

        // Vérification que les variables de l'expression sont définies
        // !!! à revoir (cf. les cas particuliers tels que 'e'...)
        if (ii == 0) {
            if ((szExpression[ii] >= _T('a')) && (szExpression[ii] <= _T('z'))
                && (_istalpha(szExpression[ii + 1]) == FALSE) && (_istdigit(szExpression[ii + 1]) == FALSE)
                && (szExpression[ii + 1] != _T('='))) {
                ll = (int_t) (szExpression[ii] - _T('a'));
                if (m_bParam[ll] == FALSE) {    // Var not set
                    if (szExpression[ii] != _T('x')) {
                        _tcscpy(szOutput, _T("! Variable not set."));
                        return FALSE;
                    }
                    nVarNotSet = 1;
                }
            }
        }
        else if (ii == (nlen - 1)) {
            if ((szExpression[ii] >= _T('a')) && (szExpression[ii] <= _T('z'))
                && (_istalpha(szExpression[ii-1]) == FALSE) && (_istdigit(szExpression[ii-1]) == FALSE)
                && (szExpression[ii-1] != _T('_'))) {
                ll = (int_t) (szExpression[ii] - _T('a'));
                if (m_bParam[ll] == FALSE) {    // Var not set
                    _tcscpy(szOutput, _T("! Variable not set."));
                    return FALSE;
                }
            }
        }
        else {
            if ((szExpression[ii] >= _T('a')) && (szExpression[ii] <= _T('z'))
                && (_istalpha(szExpression[ii - 1]) == FALSE) && (_istdigit(szExpression[ii - 1]) == FALSE)
                && (_istalpha(szExpression[ii + 1]) == FALSE) && (_istdigit(szExpression[ii + 1]) == FALSE)
                && (szExpression[ii - 1] != _T('_')) && (szExpression[ii + 1] != _T('='))) {
                ll = (int_t) (szExpression[ii] - _T('a'));
                if (m_bParam[ll] == FALSE) {    // Var not set
                    if (szExpression[ii] != _T('x')) {
                        _tcscpy(szOutput, _T("! Variable not set."));
                        return FALSE;
                    }
                    nVarNotSet = 1;
                }
            }
        }
    }

    nlen = jj;
    szFunction[nlen] = _T('\0');
    if ((nlen < 2) || (nlen > ML_STRSIZE - 7)) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    memset((char_t*)szExpression, 0, sizeof(char_t) * ML_STRSIZE);
    _tcscpy((char_t*)szExpression, (const char_t*)szFunction);

    iOp = EXPR;

    if ((nVarNotSet == 1) && (ns == 0)) {
        _tcscpy(szOutput, _T("! Variable not set."));
        return FALSE;
    }

    if (ns > 0) {                                                                                // SOLVE
        szExpression[ns] = _T('\0');
        szFunction[ns] = _T('\0');
        iOp = SOLVE;
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("solve("))) != NULL) {
        int ill = (int)_tcslen((const char_t*)szExpression);
        if (ill > 6) {
            char_t szTTT[ML_STRSIZE];
            szExpression[ill - 1] = '\0';
            pszT = szExpression;
            pszT += 6;
            memset((char_t*)szTTT, 0, sizeof(char_t) * ML_STRSIZE);
            _tcscpy((char_t*)szTTT, (const char_t*)pszT);
            _tcscpy((char_t*)szExpression, (const char_t*)szTTT);
            _tcscpy((char_t*)szFunction, (const char_t*)szExpression);
            iOp = SOLVE;
        }
    }
    else if ((ne > 0) && (ne < ML_STRSIZE)) {                                            // '=' found
        if (ne >= ML_STRSIZET) {    
            _tcscpy(szOutput, _T("! Invalid variable name (too long)."));
            return FALSE;
        }
        if (ne < (nlen - 1)) {                                // ASSIGN
            for (ii = 0; ii < ne; ii++) {                                                    // Var name.
                szVar[ii] = szExpression[ii];
            }
            szVar[ne] = _T('\0');
            // Reserved names
            if (_tcsicmp((const char_t*)szVar, _T("solve")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("rad")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("radian")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("deg")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("degree")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("gradian")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("grad")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("unit")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("reset")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("set")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("unset")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("name")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("release")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("copyright")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("version")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("description")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("var")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("format")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("new")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("save")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("quit")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("exit")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("tol")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("iter")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("author")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("reserved")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("sidi")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }

            if ((szVar[0] == _T('_')) && (ne == 2)) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            if (ne == 2) {
                if ((szVar[0] == _T('p')) && (szVar[1] == _T('i'))) {
                    _tcscpy(szOutput, _T("! Name reserved."));
                    return FALSE;
                }
            }
            // Verify varname validity
            if (_istalpha(szVar[0]) == 0) {
                _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                return FALSE;
            }
            for (ii = 0; ii < ne; ii++) {
                if ((_istalpha(szVar[ii]) == 0) && (_istdigit(szVar[ii]) == 0)) {
                    _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                    return FALSE;
                }
            }
            //
            for (ii = ne + 1; ii < nlen; ii++) {                                        // Expr.
                szFunction[ii - (ne + 1)] = szExpression[ii];
            }
            szFunction[nlen - (ne + 1)] = _T('\0');
            iOp = ASSIGN;
        }
        else if (ne == (nlen - 1)) {                                                        // PRINT
            for (ii = 0; ii < ne; ii++) {                                                    // Var name.
                szVar[ii] = szExpression[ii];
            }
            szVar[ne] = _T('\0');
            iOp = PRINT;
        }
    }

    // Update nlen
    nlen = (int_t)_tcslen((const char_t*)szFunction);
    if ((nlen < 1) || (nlen > 250)) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    if ((iOp == ASSIGN) && (m_nVarnum == (_T('z') - _T('a') + 1))) {
        ResizeParam();
    }

    cVar = _T('\0');

    if (iOp != SOLVE) {
        // Search for variable name
        nvlen = (int_t)_tcslen((const char_t*)szVar);
        if (nvlen > 1) {
            if (m_nVarnum > 0) {
                for (ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {    
                    if (_tcscmp((const char_t*)m_szParam[ii], (const char_t*)szVar) == 0) {
                        if (m_bParam[ii]) {
                            cVar = _T('a') + ii;
                            bFound = TRUE;
                            break;
                        }
                        else {
                            m_szParam[ii][0] = _T('a') + ii;
                            m_szParam[ii][1] = _T('\0');
                        }
                    }
                }
            }
        }
        else if ((nvlen == 1) && (szVar[0] >= _T('a')) && (szVar[0] <= _T('z'))) {
            if (m_bParam[szVar[0] - _T('a')]) {
                cVar = szVar[0];
                bFound = TRUE;
            }
        }
        // Replace variables name in expression
        char_t cLeft, cRight;
        memset((char_t*) szExpression, 0, sizeof(char_t) * ML_STRSIZE);
        _tcscpy((char_t*)szExpression, (const char_t*)szFunction);
        for (ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {
            jj = 0;
            nslen = (int_t)_tcslen((const char_t*)m_szParam[ii]);
            if (nslen > 1) {
                pszTT = (char_t*)_tcsstr((const char_t*)szExpression, (const char_t*)m_szParam[ii]);
                if (pszTT) {

                    // Word?
                    cLeft = cRight = _T('a');
                    if (pszTT > (wchar_t*) szExpression) {
                        cLeft = *(pszTT - 1);
                    }
                    if ((pszTT + nslen) <= (wchar_t*) (szExpression + nlen - 1)) {
                        cRight = *(pszTT + nslen);
                    }
                    if ((_istalpha(cLeft) != 0) || (_istalpha(cRight) != 0)) {
                        continue;
                    }
                    //

                    // Replace
                    nplen = (int_t)_tcslen((const char_t*)pszTT);
                    *pszTT = _T('a') + ii;
                    for (ll = 1; ll < nplen - nslen + 1; ll++) {
                        *(pszTT + ll) = *(pszTT + ll + nslen - 1);
                    }
                    *(pszTT + nplen - nslen + 1) = _T('\0');
                    pszTT += 1;

                    jj = 1;
                    while(1) {
                        pszTT = (char_t*)_tcsstr((const char_t*)pszTT, (const char_t*)m_szParam[ii]);
                        if ((pszTT == NULL) || (jj >= nlen)) {
                            bRep = TRUE;
                            break;
                        }

                        // Word?
                        cLeft = cRight = _T('a');
                        if (pszTT > (wchar_t*) szExpression) {
                            cLeft = *(pszTT - 1);
                        }
                        if ((pszTT + nslen) <= (wchar_t*) (szExpression + nlen - 1)) {
                            cRight = *(pszTT + nslen);
                        }
                        if ((_istalpha(cLeft) != 0) || (_istalpha(cRight) != 0)) {
                            continue;
                        }
                        //

                        // Replace
                        nplen = (int_t)_tcslen((const char_t*)pszTT);
                        *pszTT = _T('a') + ii;
                        for (ll = 1; ll < nplen - nslen + 1; ll++) {
                            *(pszTT + ll) = *(pszTT + ll + nslen - 1);
                        }
                        *(pszTT + nplen - nslen + 1) = _T('\0');
                        pszTT += 1;
                        //

                        jj += 1;
                    }
                }
            }
        }

        if (bRep) {
            memset((char_t*)szFunction, 0, sizeof(char_t) * ML_STRSIZE);
            _tcscpy((char_t*)szFunction, (const char_t*)szExpression);
            // Update nlen
            nlen = (int_t)_tcslen((const char_t*)szFunction);
            if ((nlen < 1) || (nlen > 250)) {
                _tcscpy(szOutput, _T("! Invalid expression."));
                return FALSE;
            }
        }
    }

    else {    // SOLVE
        // Search for 'x'
        if (_tcschr((const char_t*)szExpression, _T('x')) == NULL) {
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }
    }

    // Treatment before calling parser
    ii = 0;

    bool bSepLeft = true;
    bool bNumLeft = true;

    while (1) {

        nlen = (int_t) _tcslen((const char_t*) szFunction);
        if ((ii >= nlen) || (ii >= ML_STRSIZE)) { break; }
        if (szFunction[ii] == _T('\0')) { break; }

        if (bNumLeft && szFunction[ii] == _T('e')) {
            if (iswdigit(szFunction[ii + 1]) || (szFunction[ii + 1] == _T('+')) || (szFunction[ii + 1] == _T('-'))) {
                szFunction[ii] = _T('E');
            }
        }

        else if (bSepLeft && (ii <= (nlen - 3)) && (ii < (ML_STRSIZE - ML_STRSIZES)) && ((nlen == 3) || isSeparator(szFunction[ii + 3]))) {
            if (m_bAns && (szFunction[ii] == _T('a')) && (szFunction[ii + 1] == _T('n')) && (szFunction[ii + 2] == _T('s'))) {
                pszTT = &szFunction[ii + 3];
                _tcscpy(szTmp, pszTT);
                pszTT = &szFunction[ii];
                _tcsprintf(pszTT, ML_STRSIZES - 1, _T("%g"), m_fAns);
                _tcscat(szFunction, szTmp);
            }
        }

        else if (bSepLeft && (ii <= (nlen - 2)) && (ii <= (ML_STRSIZE - 5)) && ((nlen == 2) || isSeparator(szFunction[ii + 2]))) {
            if ((szFunction[ii] == _T('p')) && (szFunction[ii + 1] == _T('i')) && (szFunction[ii + 2] != _T('('))) {
                pszTT = &szFunction[ii + 2];
                _tcscpy(szTmp, pszTT);
                szFunction[ii + 2] = _T('(');
                szFunction[ii + 3] = _T(')');
                szFunction[ii + 4] = _T('\0');
                _tcscat(szFunction, szTmp);
            }
            else if (szFunction[ii] == _T('_')) {    // Constants
                if ((szFunction[ii + 1] == _T('k')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('q')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('h')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('c')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('e')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('n')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('m')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
            }
        }

        bSepLeft = isSeparatorLeft(szFunction[ii]);
        bNumLeft = (_istdigit(szFunction[ii]) != 0);

        ii += 1;
    }

    // Print variable value
    if (iOp == PRINT) {
        if (cVar == _T('\0')) {
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        if (m_bParam[cVar - _T('a')] == FALSE) {    // Var not set
            _tcscpy(szOutput, _T("! Variable not set."));
            return FALSE;
        }
        //

        if (pVal) { *pVal = m_fParam[cVar - _T('a')]; }

        //ftmp = fabs(m_fParam[cVar - _T('a')]);
        if (pszFormat == NULL) {
            Reformat(m_fParam[cVar - _T('a')], szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, m_fParam[cVar - _T('a')]);
            m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        _tcscpy(szOutput, (const char_t*)szBuffer);
        return TRUE;
    }

    // Assign
    else if (iOp == ASSIGN) {

        if ((nvlen == 3) && (szVar[0] == _T('a')) && (szVar[1] == _T('n')) && (szVar[2] == _T('s'))) {
            _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
            return FALSE;
        }

        // Search for variable name
        if (bFound == FALSE) {
            // Verify varname validity
            if (_istalpha(szVar[0]) == 0) {
                _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                return FALSE;
            }
            int_t varlen = (int_t)_tcslen(szVar);
            for (ii = 0; ii < varlen; ii++) {
                if ((_istalpha(szVar[ii]) == 0) && (_istdigit(szVar[ii]) == 0) && (szVar[ii] != _T('_'))) {
                    _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                    return FALSE;
                }
            }
            //
            if (varlen == 1) {
                cVar = szVar[0];
                m_nVarnum += 1;
            }
            else if (varlen > 1) {
                char_t cc;
                cVar = _T('a') + m_nVarnum;
                for (cc = _T('a'); cc < _T('z'); cc++) {
                    if (m_bParam[cc - _T('a')] == FALSE) {
                        cVar = cc;
                        _tcscpy(m_szParam[cVar - _T('a')], (const char_t*)szVar);
                        m_nVarnum += 1;
                        break;
                    }
                }
            }
        }
        else if (nvlen == 1) {
            if ((m_bParam[szVar[0] - _T('a')] == TRUE) && (m_szParam[szVar[0] - _T('a')][1] != _T('\0'))) {
                _tcscpy(szOutput, _T("! Variable not set. Choose another variable name."));
                return FALSE;
            }
        }
        //

        if (cVar == _T('\0')) {
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        m_pLib->errcode = 0;
        pszFunction = ml_parser_decode(szFunction, _T("abcdefghijklmnopqrstuvwxyz"), &nlen, m_pLib);

        if ((m_pLib->errcode != 0) || (pszFunction == NULL)) {
            ml_parser_free(pszFunction);
            m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        m_fParam[cVar - _T('a')] = ml_parser_evalf(m_pLib, pszFunction, _T("abcdefghijklmnopqrstuvwxyz"), m_fParam[0], m_fParam[1], m_fParam[2], m_fParam[3], m_fParam[4], m_fParam[5],
            m_fParam[6], m_fParam[7], m_fParam[8], m_fParam[9], m_fParam[10], m_fParam[11], m_fParam[12], m_fParam[13], m_fParam[14], m_fParam[15],
            m_fParam[16], m_fParam[17], m_fParam[18], m_fParam[19], m_fParam[20], m_fParam[21], m_fParam[22], m_fParam[23], m_fParam[24], m_fParam[25]);

        if (m_pLib->errcode != 0) {
            ml_parser_free(pszFunction);
            m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        ml_parser_free(pszFunction);

        if (pVal) { *pVal = m_fParam[cVar - _T('a')]; }

        if (pszFormat == NULL) {
            Reformat(m_fParam[cVar - _T('a')], szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, m_fParam[cVar - _T('a')]);
            m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        // Variable set
        m_bParam[cVar - _T('a')] = FALSE;
        if (_tcsicmp((const char_t*)m_szParam[cVar - _T('a')], _T("tol")) == 0) {            // Special set
            ftmp = m_fParam[cVar - _T('a')];
            if ((ftmp > 1e-15) && (ftmp < 1e3)) {
                m_pLib->tol = ftmp;
            }
            m_szParam[cVar - _T('a')][0] = cVar;
            m_szParam[cVar - _T('a')][1] = _T('\0');
        }
        if (_tcsicmp((const char_t*)m_szParam[cVar - _T('a')], _T("reltol")) == 0) {        // Special set
            ftmp = m_fParam[cVar - _T('a')];
            if ((ftmp > 1e-15) && (ftmp < 1e3)) {
                m_pLib->reltol = ftmp;
            }
            m_szParam[cVar - _T('a')][0] = cVar;
            m_szParam[cVar - _T('a')][1] = _T('\0');
        }
        if (_tcsicmp((const char_t*)m_szParam[cVar - _T('a')], _T("maxiter")) == 0) {        // Special set
            ftmp = m_fParam[cVar - _T('a')];
            if ((ftmp > 1) && (ftmp < ML_MAXITER)) {
                m_pLib->maxiter = (int_t)ftmp;
            }
            m_szParam[cVar - _T('a')][0] = cVar;
            m_szParam[cVar - _T('a')][1] = _T('\0');
        }
        else {
            m_bParam[cVar - _T('a')] = TRUE;
        }

        _tcscpy(szOutput, (const char_t*)szBuffer);
        return TRUE;
    }

    // Calculation
    else if (iOp == EXPR) {

        if ((nlen == 3) && (szFunction[0] == _T('a')) && (szFunction[1] == _T('n')) && (szFunction[2] == _T('s')) && (m_bAns == false)) {
            _tcscpy(szOutput, _T("! ans not set."));
            return FALSE;
        }

        real_t yval;

        m_pLib->errcode = 0;
        pszFunction = ml_parser_decode(szFunction, _T("abcdefghijklmnopqrstuvwxyz"), &nlen, m_pLib);

        if ((m_pLib->errcode != 0) || (pszFunction == NULL)) {
            ml_parser_free(pszFunction);
            m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        yval = ml_parser_evalf(m_pLib, pszFunction, _T("abcdefghijklmnopqrstuvwxyz"), m_fParam[0], m_fParam[1], m_fParam[2], m_fParam[3], m_fParam[4], m_fParam[5],
            m_fParam[6], m_fParam[7], m_fParam[8], m_fParam[9], m_fParam[10], m_fParam[11], m_fParam[12], m_fParam[13], m_fParam[14], m_fParam[15],
            m_fParam[16], m_fParam[17], m_fParam[18], m_fParam[19], m_fParam[20], m_fParam[21], m_fParam[22], m_fParam[23], m_fParam[24], m_fParam[25]);

        if (m_pLib->errcode != 0) {
            ml_parser_free(pszFunction);
            m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        ml_parser_free(pszFunction);

        if (pVal) { *pVal = yval; }

        m_fAns = yval;
        m_bAns = true;

        if (pszFormat == NULL) {
            Reformat(yval, szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, yval);
            m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        _tcscpy(szOutput, (const char_t*)szBuffer);
        return TRUE;
    }

    // Solve equation
    else if (iOp == SOLVE) {
        real_t xval, fval, aa, bb;

        m_pLib->errcode = 0;
        g_pszFunction = ml_parser_decode(szFunction, _T("x"), &nlen, m_pLib);

        if ((m_pLib->errcode != 0) || (g_pszFunction == NULL)) {
            ml_parser_free(g_pszFunction);
            m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        aa = m_fParam[0];        bb = m_fParam[1];
        ml_fzguess(funfx, &aa, &bb, m_pLib);

        xval = ml_fzero(funfx, aa, bb, m_pLib);
        if (m_pLib->errcode != 0) {
            ml_parser_free(g_pszFunction);
            m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Cannot solve equation. Try to change the initial guess (a and b)."));
            return FALSE;
        }

        fval = funfx(xval);

        ml_parser_free(g_pszFunction);

        if (pVal) { *pVal = xval; }

        int_t nLenT = (ML_STRSIZE >> 1) - 7;

        if (pszFormat == NULL) {
            Reformat(xval, szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, nLenT, pszFormat, xval);
            m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        m_fParam[_T('x') - _T('a')] = xval;
        m_bParam[_T('x') - _T('a')] = TRUE;

        _tcsprintf(szTmp, nLenT, _T("x = %s"), szBuffer);
        _tcscpy(szOutput, (const char_t*)szTmp);

        if (pszFormat == NULL) {
            Reformat(fval, szBuffer, nLenT);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, nLenT, pszFormat, fval);
            m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        m_fParam[_T('y') - _T('a')] = fval;
        m_bParam[_T('y') - _T('a')] = TRUE;

        _tcsprintf(szTmp, nLenT, _T("   y = %s"), szBuffer);
        _tcsncat(szOutput, (const char_t*)szTmp, nLenT);
    }

    return TRUE;
}

BOOL CSigmaApp::ReadTextFile(CString fileName, CEdit *pEdit, CFileStatus &fileStatus,
                                      int_t *nLineCount, byte_t bCRLF/* = 1*/)
{
    char_t cSep = m_Options.import.cSep;

    Output(_T("Data reading...\r\n"), TRUE);

    if (pEdit == NULL) {
        Output(_T("Cannot read data file: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    *nLineCount = 0;

    int_t nLength = fileName.GetLength();
    if ((nLength < 5) || (nLength > ML_STRSIZE)) {
        Output(_T("Cannot read data file: invalid file name.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    CString strT;
    CStdioFile fileT;
    char_t *pszBuffer = NULL, *pszData = NULL;

    try {
        CFileException excT;
        if (!fileT.Open((LPCTSTR)fileName, CFile::modeRead | CFile::typeText | CFile::shareExclusive, &excT)) {
            char_t szError[ML_STRSIZEW];
            excT.GetErrorMessage(szError, ML_STRSIZEW);
            // >> OUTPUT
            strT.Format(_T("Cannot read data file: %s\r\n"), szError);
            Output(strT, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            // <<
            return FALSE;
        }
        if (fileT.GetStatus(fileStatus) == FALSE) {
            fileT.Close();
            // >> OUTPUT
            Output(_T("Cannot read data file: access denied.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            // <<
            return FALSE;
        }
        // Maximum file size = 128 MB
        int_t nFileLen = (int_t) (fileT.GetLength());
        if ((nFileLen < 5) || (nFileLen >= (sizeof(char_t) * (ML_STRSIZES - 1) * SIGMA_MAXCOLUMNS * ML_MAXPOINTS))) {
            fileT.Close();
            // >> OUTPUT
            Output(_T("Cannot read data file: invalid size.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            // <<
            return FALSE;
        }

        pszData = (char_t *) malloc((nFileLen + ML_MAXPOINTS) * sizeof(char_t));
        if (pszData == NULL) {
            fileT.Close();
            // >> OUTPUT
            Output(_T("Cannot read data file: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            // <<
            return FALSE;
        }
        memset(pszData, 0, (nFileLen + ML_MAXPOINTS) * sizeof(char_t));

        pszBuffer = (char_t *) malloc((ML_STRSIZES * SIGMA_MAXCOLUMNS) * sizeof(char_t));
        if (pszBuffer == NULL) {
            free(pszData); pszData = NULL;
            fileT.Close();
            // >> OUTPUT
            Output(_T("Cannot read data file: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            // <<
            return FALSE;
        }
        pEdit->SetWindowText(_T(""));
        int nLength, ii;
        for (ii = 0; ii < ML_MAXPOINTS; ii++) {
            if (fileT.ReadString(pszBuffer, (ML_STRSIZES * SIGMA_MAXCOLUMNS) - 2) == NULL) {
                break;
            }
            nLength = (int_t)_tcslen((const char_t*)pszBuffer);
            if (nLength < 1) {
                continue;
            }
            if (nLength >= ((ML_STRSIZES * SIGMA_MAXCOLUMNS) - 2)) {
                break;
            }

            // Append CRLF or LF (ReadString eventually already appended LF)
            if (pszBuffer[nLength - 1] == _T('\n')) {
                pszBuffer[nLength - 1] = _T('\0');
            }
            if (bCRLF) {
                _tcscat(pszBuffer, _T("\r"));
            }
            _tcscat(pszBuffer, _T("\n"));
            _tcscat(pszData, (const char_t*)pszBuffer);
        }
        pEdit->SetWindowText((LPCTSTR)pszData);
        free(pszData); pszData = NULL;
        free(pszBuffer); pszBuffer = NULL;
        fileT.Close();

        // >> OUTPUT
        strT.Format(_T("Data file read: %d lines.\r\n"), ii);
        Output(strT, TRUE, FALSE, -1, 0);
        // <<
        *nLineCount = ii;
        return TRUE;
    }
    catch (...) {
        if (pszData) { free(pszData); pszData = NULL; }
        if (pszBuffer) { free(pszBuffer); pszBuffer = NULL; }
        fileT.Close();
        // >> OUTPUT
        Output(_T("Cannot read data file.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        // <<
        return FALSE;
    }
}

BOOL CSigmaApp::ImportData(CString fileName, vector_t *pColumn, int_t *piColumn, int_t *pColumnCount, int_t *pRowCount,
                                    long_t *pvecCurID, char_t *pszNote/* = NULL*/)
{
    char_t cSep = m_Options.import.cSep;
    int_t iSkip = m_Options.import.iSkip;
    int_t iTitle = m_Options.import.iTitle;

    char_t szFilename[ML_STRSIZE];
    CStringToChar(fileName, szFilename, ML_STRSIZE - 1);
    m_pLib->errcode = 0;
    fl_import_asciiv((const char_t*)szFilename, pColumn, piColumn, pColumnCount, pRowCount,
        pvecCurID, pszNote, cSep, iSkip, iTitle, 1, m_pLib);

    if (m_pLib->errcode != 0) {
        Output(m_pLib->message, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        Output(_T("\r\n"));
        return FALSE;
    }

    Output(m_pLib->message, TRUE);
    Output(_T("\r\n"));
    return TRUE;
}

BOOL CSigmaApp::ExportData(CString fileName, vector_t *pColumn, int_t nColumnCount)
{
    char_t szFilename[ML_STRSIZE];
    CStringToChar(fileName, szFilename, ML_STRSIZE - 1);
    m_pLib->errcode = 0;
    fl_export_asciiv((const char_t*)szFilename, pColumn, nColumnCount, m_Options.import.cSep, m_pLib, NULL);

    if (m_pLib->errcode != 0){
        Output(m_pLib->message, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        Output(_T("\r\n"));
        return FALSE;
    }

    Output(m_pLib->message, TRUE);
    Output(_T("\r\n"));
    return TRUE;
}

BOOL CSigmaApp::CopyDocument(sigmadoc_t *pDocTo, const sigmadoc_t *pDocFrom)
{
    if ((pDocTo == NULL) || (pDocFrom == NULL)) {
        return FALSE;
    }

    if ((pDocFrom->iDataCount < 1) || (pDocFrom->iDataCount > SIGMA_MAXCOLUMNS)) {
        return FALSE;
    }
    if ((pDocFrom->iDataFitCount < 0) || (pDocFrom->iDataFitCount > SIGMA_MAXCOLUMNS)) {
        return FALSE;
    }

    int_t jj;

    if (pDocTo->iDataCount > 0) {
        for (jj = 0; jj < pDocTo->iDataCount; jj++) {
            if (pDocTo->ColumnData[jj].dat) {
                m_pLib->errcode = 0;
                ml_vector_delete(&(pDocTo->ColumnData[jj]), m_pLib);
            }
        }
        pDocTo->iDataCount = 0;
        memset(pDocTo->ColumnData, 0, SIGMA_MAXCOLUMNS * sizeof(vector_t));
    }
    if (pDocTo->iDataFitCount > 0) {
        for (jj = 0; jj < pDocTo->iDataFitCount; jj++) {
            if (pDocTo->ColumnDataFit[jj].dat) {
                m_pLib->errcode = 0;
                ml_vector_delete(&(pDocTo->ColumnDataFit[jj]), m_pLib);
            }
        }
        pDocTo->iDataFitCount = 0;
        memset(pDocTo->ColumnDataFit, 0, SIGMA_MAXCOLUMNS * sizeof(vector_t));
    }

    pDocTo->Software.version = pDocFrom->Software.version;
    _tcscpy(pDocTo->Software.name, (const char_t*)(pDocFrom->Software.name));
    _tcscpy(pDocTo->Software.description, (const char_t*)(pDocFrom->Software.description));
    _tcscpy(pDocTo->Software.copyright, (const char_t*)(pDocFrom->Software.copyright));
    _tcscpy(pDocTo->Software.date, (const char_t*)(pDocFrom->Software.date));
    _tcscpy(pDocTo->Software.time, (const char_t*)(pDocFrom->Software.time));
    _tcscpy(pDocTo->Software.reserved, (const char_t*)(pDocFrom->Software.reserved));

    _tcscpy(pDocTo->szName, (const char_t*)(pDocFrom->szName));
    _tcscpy(pDocTo->szNote, (const char_t*)(pDocFrom->szNote));
    pDocTo->nID = pDocFrom->nID;
    pDocTo->iType = pDocFrom->iType;
    pDocTo->bStatus = pDocFrom->bStatus;
    pDocTo->tmCreated.year = pDocFrom->tmCreated.year;
    pDocTo->tmCreated.month = pDocFrom->tmCreated.month;
    pDocTo->tmCreated.day = pDocFrom->tmCreated.day;
    pDocTo->tmCreated.hour = pDocFrom->tmCreated.hour;
    pDocTo->tmCreated.minute = pDocFrom->tmCreated.minute;
    pDocTo->tmCreated.second = pDocFrom->tmCreated.second;
    pDocTo->tmCreated.millisecond = pDocFrom->tmCreated.millisecond;
    pDocTo->tmModified.year = pDocFrom->tmModified.year;
    pDocTo->tmModified.month = pDocFrom->tmModified.month;
    pDocTo->tmModified.day = pDocFrom->tmModified.day;
    pDocTo->tmModified.hour = pDocFrom->tmModified.hour;
    pDocTo->tmModified.minute = pDocFrom->tmModified.minute;
    pDocTo->tmModified.second = pDocFrom->tmModified.second;
    pDocTo->tmModified.millisecond = pDocFrom->tmModified.millisecond;
    pDocTo->iColumnID = pDocFrom->iColumnID;

    //
    for (jj = 0; jj < pDocFrom->iDataCount; jj++) {
        m_pLib->errcode = 0;
        ml_vector_create(&(pDocTo->ColumnData[jj]), pDocFrom->ColumnData[jj].dim,
            pDocFrom->ColumnData[jj].id, (const char_t*)(pDocFrom->ColumnData[jj].name), NULL, m_pLib);
        ml_vector_dup(&(pDocTo->ColumnData[jj]), (const vector_t*)(&(pDocFrom->ColumnData[jj])), m_pLib);
        ml_stat_dup(&(pDocTo->Stats[jj]), (const stat_t*)(&(pDocFrom->Stats[jj])));
    }
    pDocTo->iDataCount = pDocFrom->iDataCount;
    if (pDocFrom->iDataFitCount > 1) {
        for (jj = 0; jj < pDocFrom->iDataFitCount; jj++) {
            m_pLib->errcode = 0;
            ml_vector_create(&(pDocTo->ColumnDataFit[jj]), pDocFrom->ColumnDataFit[jj].dim,
                pDocFrom->ColumnDataFit[jj].id, (const char_t*)(pDocFrom->ColumnDataFit[jj].name), NULL, m_pLib);
            ml_vector_dup(&(pDocTo->ColumnDataFit[jj]), (const vector_t*)(&(pDocFrom->ColumnDataFit[jj])), m_pLib);
        }
        for (jj = 0; jj < (pDocFrom->iDataCount >> 1); jj++) {
            m_pLib->errcode = 0;
            ml_fit_dup(&(pDocTo->Fit[jj]), (const fit_t*) (&(pDocFrom->Fit[jj])));
        }
        pDocTo->iDataFitCount = pDocFrom->iDataFitCount;
    }

    return TRUE;
}

BOOL CSigmaApp::CopyPlot(plot_t *pPlotTo, const plot_t *pPlotFrom)
{
    if ((pPlotTo == NULL) || (pPlotFrom == NULL)) {
        return FALSE;        
    }

    pPlotTo->errcode = 0;
    pl_plot_dupstyle(pPlotTo, pPlotFrom);
    if (pPlotTo->errcode != 0) {
        return FALSE;
    }

    return TRUE;
}


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

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CSigmaApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CSigmaApp message handlers

void CSigmaApp::Output(CString strT, BOOL bAddTime /*= FALSE*/, BOOL bShowMessage /*= FALSE*/, int_t nIcon /* = 1*/, int_t nStatusbarPane/* = -1*/, BOOL bAutoClear/* = FALSE*/)
{
    HWND hwndParent = ::GetForegroundWindow();

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        return;
    }

    if (::IsWindow(pFrame->m_hWnd) == FALSE) {
        return;
    }
    
    if (::IsWindowVisible(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (hwndParent == NULL) {
        hwndParent = pFrame->m_hWnd;
    }

    // dont automatically create output, if no error
    if ((SIGMA_OUTPUT_ERROR != nIcon) && (pFrame->m_pwndOutput == NULL)) {
        return;
    }

    pFrame->OutputCreate();

    if (pFrame->m_pwndOutput == NULL) {
        return;
    }

    int_t nlen = (int_t) (strT.GetLength()), nLength;
    if ((nlen < 1) || (nlen > ML_STRSIZE)) {
        return;
    }
    nLength = nlen;

    int_t ii;
    nlen = (int_t) (m_strOutput.GetLength());
    if (nlen > ML_STRSIZEL) {
        m_strOutput.Delete(0, ML_STRSIZE);
        nlen = (int_t) (m_strOutput.GetLength());
        for (ii = 0; ii < nlen; ii++) {
            if ((char_t)(m_strOutput[ii]) == _T('\n')) {
                break;
            }
        }
        m_strOutput.Delete(0, ii + 1);
    }

    if (bAddTime == TRUE) {
        CString strTT;
        date_t tm;
        m_pLib->errcode = 0;
        fl_gettime(&tm);
        strTT.Format(_T("[%02d:%02d:%02d] "), tm.hour, tm.minute, tm.second);
        m_strOutput += strTT;
    }
    m_strOutput += strT;

    pFrame->m_pwndOutput->m_edtOutput.SetWindowText(_T(""));
    pFrame->m_pwndOutput->m_edtOutput.SetSel(-1, -1);                    //select position after last char_t in editbox
    pFrame->m_pwndOutput->m_edtOutput.ReplaceSel(m_strOutput);        // replace selection with new text

    if (bShowMessage == TRUE) {
        if (strT[nLength - 2] == _T('\r') || strT[nLength - 2] == _T('\n')) {
            strT.SetAt(nLength - 2, _T('\0'));
        }
        else if (strT[nLength - 1] == _T('\r') || strT[nLength - 1] == _T('\n')) {
            strT.SetAt(nLength - 1, _T('\0'));
        }
        if (strT.GetLength() > 1) {
            UINT uiIcon = MB_ICONINFORMATION;
            switch (nIcon) {
                case SIGMA_OUTPUT_INFORMATION:
                    uiIcon = MB_ICONINFORMATION;
                    break;
                case SIGMA_OUTPUT_WARNING:
                    uiIcon = MB_ICONWARNING;
                    break;
                case SIGMA_OUTPUT_ERROR:
                    uiIcon = MB_ICONERROR;
                    break;
                default:
                    uiIcon = MB_ICONINFORMATION;
                    break;
            }
            MessageBox(hwndParent, (LPCTSTR)strT, (LPCTSTR)(pFrame->GetTitle()), MB_OK|uiIcon);
        }
    }

    if (pFrame && (nStatusbarPane >= 0)) {
        if (strT[nLength - 2] == _T('\r') || strT[nLength - 2] == _T('\n')) {
            strT.SetAt(nLength - 2, _T('\0'));
        }
        else if (strT[nLength - 1] == _T('\r') || strT[nLength - 1] == _T('\n')) {
            strT.SetAt(nLength - 1, _T('\0'));
        }
        pFrame->SetStatusbarInfo(strT, nStatusbarPane, bAutoClear);
    }
}

void CSigmaApp::ScriptOutput(const char_t *pszText)
{
    HWND hwndParent = ::GetForegroundWindow();

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        return;
    }

    if (::IsWindow(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (::IsWindowVisible(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (hwndParent == NULL) {
        hwndParent = pFrame->m_hWnd;
    }

    if (pFrame->m_pwndScript == NULL) {
        pFrame->OnViewScript();
    }
    if (pFrame->m_pwndScript == NULL) {
        return;
    }

    pFrame->m_pwndScript->OutputView(true);
    if (pFrame->m_pwndScript->m_edtOutput == NULL) {
        return;
    }

    pFrame->m_pwndScript->m_edtOutput->AppendText(pszText);
}

void CSigmaApp::ScriptOutputClear(void)
{
    HWND hwndParent = ::GetForegroundWindow();

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        return;
    }

    if (::IsWindow(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (::IsWindowVisible(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (hwndParent == NULL) {
        hwndParent = pFrame->m_hWnd;
    }

    if (pFrame->m_pwndScript == NULL) {
        pFrame->OnViewScript();
    }
    if (pFrame->m_pwndScript == NULL) {
        return;
    }

    pFrame->m_pwndScript->OutputView(true);
    if (pFrame->m_pwndScript->m_edtOutput == NULL) {
        return;
    }

    pFrame->m_pwndScript->m_edtOutput->Reset();
}

void CSigmaApp::OutputView(void)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        return;
    }
    pFrame->OutputView();
}

void CSigmaApp::OutputClear(void)
{
    m_strOutput.Empty();

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        return;
    }

    if (::IsWindow(pFrame->m_hWnd) == FALSE) {
        return;
    }
    
    if (::IsWindowVisible(pFrame->m_hWnd) == FALSE) {
        return;
    }

    if (pFrame->m_pwndOutput != NULL) {
        if (pFrame->m_pwndOutput->GetSafeHwnd() == NULL) {
            delete pFrame->m_pwndOutput;
            pFrame->m_pwndOutput = NULL;
            return;
        }
    }

    if (pFrame->m_pwndOutput == NULL) {
        return;
    }

    pFrame->m_pwndOutput->m_edtOutput.SetWindowText(_T(""));
    pFrame->m_pwndOutput->m_edtOutput.SetSel(-1, -1);                    //select position after last char_t in editbox
}

void CSigmaApp::OnFileNew()
{
    if (m_nDocCount >= SIGMA_MAXDOC) {
        m_bDropping = FALSE;
        this->Output(_T("Cannot create a new document.\r\nMaximum number of documents (16) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CSigmaDoc* pDocToClose = NULL;

    BOOL bOpenSigmaDoc = FALSE;
    if (m_bDropping == TRUE) {
        int_t nlen = (int_t) _tcslen((const char_t*) m_szDropFilename);
        char_t szExt[5];
        if (nlen > 5) {
            szExt[0] = m_szDropFilename[nlen - 4];
            szExt[1] = m_szDropFilename[nlen - 3];
            szExt[2] = m_szDropFilename[nlen - 2];
            szExt[3] = m_szDropFilename[nlen - 1];
            szExt[4] = _T('\0');
            if (_tcsicmp((const char_t*) szExt, _T(".sid")) == 0) {
                bOpenSigmaDoc = TRUE;

                CString strFilename = (const char_t*) m_szDropFilename;
                // Vérifier que le fichier n'est pas déjà ouvert
                // parcourir tous les templates
                CDocTemplate* pTemplate;
                POSITION pos = GetFirstDocTemplatePosition();
                while (pos != NULL) {
                    pTemplate = GetNextDocTemplate(pos);
                    ASSERT(pTemplate);
                    // tous les documents du template.
                    POSITION posD = pTemplate->GetFirstDocPosition();
                    while (posD) {
                        CSigmaDoc* pDoc = (CSigmaDoc*) (pTemplate->GetNextDoc(posD));
                        if (pDoc != NULL) {
                            if ((1 == m_nDocCount) && (NULL == pDocToClose)) {
                                pDocToClose = pDoc;
                            }
                            if (pDoc->GetFilename() == strFilename) {
                                CView* pView = NULL;
                                POSITION posV = pDoc->GetFirstViewPosition();
                                while (posV != NULL) {
                                    pView = pDoc->GetNextView(posV);
                                    if (pView == NULL) {
                                        break;
                                    }
                                    if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewPlot))) {
                                        pView->GetParentFrame()->ActivateFrame(SW_RESTORE);
                                        return;
                                    }
                                    else if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewData))) {
                                        pView->GetParentFrame()->ActivateFrame(SW_RESTORE);
                                        return;
                                    }
                                }
                                return;
                            }
                        }
                    }
                }

            }
        }
    }

    CDocTemplate* pDocTemplate = NULL;
    if (m_bDropping == TRUE) {
        if (bOpenSigmaDoc) {
            pDocTemplate = (CDocTemplate*) m_pTemplatePlot;
        }
        else {
            pDocTemplate = (CDocTemplate*) m_pTemplateData;
        }
    }
    else {
        pDocTemplate = (CDocTemplate*) m_pTemplateData;
    }
    if (pDocTemplate == NULL) {
        m_bDropping = FALSE;
        this->Output(_T("Cannot create a new document.\r\nInvalid document template.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CDocument* pDocument = pDocTemplate->CreateNewDocument();
    if (pDocument == NULL) {
        m_bDropping = FALSE;
        this->Output(_T("Cannot create a new document.\r\nInvalid document resource.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CSigmaDoc* pSigmaDocument = (CSigmaDoc*) pDocument;

    // Vérifier le type de fichier pour le drop (m_szDropFilename)
    pSigmaDocument->m_bDropping = m_bDropping;
    if (m_bDropping == TRUE) {
        _tcscpy(pSigmaDocument->m_szDropFilename, (const char_t*) m_szDropFilename);
    }

    BOOL bAutoDelete = pDocument->m_bAutoDelete;
    pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
    CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDocument, NULL);
    pDocument->m_bAutoDelete = bAutoDelete;
    if (pFrame == NULL) {
        m_bDropping = FALSE;
        delete pDocument;       // explicit delete on error
        this->Output(_T("Cannot create a new document.\r\nInvalid frame.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    if (bOpenSigmaDoc == FALSE) {
        if (!pDocument->OnNewDocument()) {
            // user has be alerted to what failed in OnNewDocument
            // document destroyed by the frame
            pDocument->m_bAutoDelete = TRUE;
            pFrame->DestroyWindow();
            m_bDropping = FALSE;
            return;
        }
    }
    else {
        // open an existing document
        CWaitCursor wait;
        if (!pDocument->OnOpenDocument((LPCTSTR) m_szDropFilename)) {
            // user has be alerted to what failed in OnOpenDocument
            // document destroyed by the frame
            pDocument->m_bAutoDelete = TRUE;
            pFrame->DestroyWindow();
            m_bDropping = FALSE;
            return;
        }
    }

    pDocTemplate->InitialUpdateFrame(pFrame, pDocument, TRUE);

    if (m_bDropping == TRUE) {
        m_bDropping = FALSE;
        memset(m_szDropFilename, 0, ML_STRSIZE * sizeof(char_t));
        if (pDocToClose && (pDocToClose->canClose())) {
            pDocToClose->OnCloseDocument();
        }
    }
}

void CSigmaApp::OnFileOpen()
{
    CString strFilename(_T(""));
    CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pMainFrame != NULL);
    if (pMainFrame) {
        CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
        if (pMDIActive) {
            CSigmaDoc* pDocument = (CSigmaDoc*) (pMDIActive->GetActiveDocument());
            if (pDocument) {
                strFilename = pDocument->GetFilename();
            }
        }
    }

    char_t szFilters[] = _T("SigmaGraph files (*.sid)|*.sid|ASCII files (*.txt;*.dat;*.csv;*.log)|*.txt;*.dat;*.csv;*.log|Lua files (*.lua)|*.lua||");

    CFileDialog *pFileDlg = new CFileDialog(TRUE, NULL, (const char_t *) (strFilename), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, m_pMainWnd);
    if (pFileDlg->DoModal() != IDOK) {
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    strFilename = pFileDlg->GetPathName();
    CString strExt = pFileDlg->GetFileExt();
    strExt.MakeLower();
    delete pFileDlg; pFileDlg = NULL;

    if (strExt == _T("lua")) {
        CWaitCursor wait;
        CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
        if (pMainFrame) {
            if ((pMainFrame->m_pwndScript == NULL) || (pMainFrame->m_pwndScript->IsWindowVisible() == FALSE)) {
                pMainFrame->OnCmdMsg(ID_VIEW_SCRIPT, 0, 0, 0);
            }
            if (pMainFrame->m_pwndScript) {
                if (pMainFrame->m_pwndScript->ScriptFileOpen((LPCTSTR)strFilename) == TRUE) {
                    AddToRecentFileList((LPCTSTR)strFilename);
                }
            }
        }
        return;
    }
    if ((strExt != _T("sid")) && (strExt != _T("txt")) && (strExt != _T("dat")) && (strExt != _T("csv"))) {
        return;
    }

    CSigmaDoc* pDocToClose = NULL;

    // Vérifier que le fichier n'est pas déjà ouvert
    // parcourir tous les templates
    CDocTemplate* pTemplate;
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        pTemplate = GetNextDocTemplate(pos);
        ASSERT(pTemplate);
        // tous les documents du template.
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD) {
            CSigmaDoc* pDoc = (CSigmaDoc*)(pTemplate->GetNextDoc(posD));
            if (pDoc != NULL) {
                if ((1 == m_nDocCount) && (NULL == pDocToClose)) {
                    pDocToClose = pDoc;
                }
                if (pDoc->GetFilename() == strFilename) {
                    CView* pView = NULL;
                    POSITION posV = pDoc->GetFirstViewPosition();
                    while (posV != NULL) {
                        pView = pDoc->GetNextView(posV);
                        if (pView == NULL) {
                            break;
                        }
                        if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewPlot))) {
                            pView->GetParentFrame()->ActivateFrame(SW_RESTORE);
                            return;
                        }
                        else if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewData))) {
                            pView->GetParentFrame()->ActivateFrame(SW_RESTORE);
                            return;
                        }
                    }
                    return;
                }
            }
        }
    }

    if (m_nDocCount >= SIGMA_MAXDOC) {
        Output(_T("Cannot open document.\r\nMaximum number of documents (16) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CDocTemplate* pDocTemplate = NULL;
    if (strExt == _T("sid")) {
        pDocTemplate = (CDocTemplate*)m_pTemplatePlot;
    }
    else {
        pDocTemplate = (CDocTemplate*)m_pTemplateData;
    }
    ASSERT_VALID(pDocTemplate);
    if (pDocTemplate == NULL) {
        Output(_T("Cannot open document.\r\nInvalid document template.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CDocument* pDocument = pDocTemplate->CreateNewDocument();
    ASSERT_VALID(pDocument);
    if (pDocument == NULL) {
        Output(_T("Cannot open document.\r\nInvalid document resource.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CSigmaDoc* pSigmaDocument = (CSigmaDoc*)pDocument;

    BOOL bAutoDelete = pDocument->m_bAutoDelete;
    pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
    CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDocument, NULL);
    pDocument->m_bAutoDelete = bAutoDelete;
    if (pFrame == NULL) {
        Output(_T("Cannot open document.\r\nInvalid frame.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        delete pDocument;       // explicit delete on error
        return;
    }
    ASSERT_VALID(pFrame);

    if ((strExt == _T("txt")) || (strExt == _T("dat")) || (strExt == _T("csv"))) {
        pSigmaDocument->m_bDropping = TRUE;
        CStringToChar(strFilename, (char_t*)(pSigmaDocument->m_szDropFilename), ML_STRSIZE - 1);
        if (!pDocument->OnNewDocument()) {
            // user has be alerted to what failed in OnNewDocument
            // document destroyed by the frame
            pDocument->m_bAutoDelete = TRUE;
            pFrame->DestroyWindow();
            pSigmaDocument->m_bDropping = FALSE;
            return;
        }
    }
    else if (strExt == _T("sid")) {
        // open an existing document
        CWaitCursor wait;
        if (!pSigmaDocument->OnOpenDocument((LPCTSTR)strFilename)) {
            // user has be alerted to what failed in OnOpenDocument
            // document destroyed by the frame
            pDocument->m_bAutoDelete = TRUE;
            pFrame->DestroyWindow();
            return;
        }
        pSigmaDocument->SetPathName((LPCTSTR) strFilename);
    }
    else {
        // document destroyed by the frame
        pDocument->m_bAutoDelete = TRUE;
        pFrame->DestroyWindow();
        return;
    }

    pDocTemplate->InitialUpdateFrame(pFrame, pDocument, TRUE);

    if (pDocToClose && (pDocToClose->canClose())) {
        pDocToClose->OnCloseDocument();
    }
}

void CSigmaApp::OnAppExit()
{
    if (m_szLocaleA[0] != '\0') {
        setlocale(LC_ALL, (const char*) (m_szLocaleA));
    }

    setOptions();
    CWinApp::OnAppExit();
}

CDocument* CSigmaApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
    return NULL;
}

bool CSigmaApp::fileExists(const char_t *pszFilename)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE handleT = ::FindFirstFile((LPCTSTR) pszFilename, &FindFileData);
    if (handleT != INVALID_HANDLE_VALUE) {
        ::FindClose(handleT);
        return true;
    }
    return false;
}

bool CSigmaApp::makeDir(const char_t *pszDir)
{
    if (CSigmaApp::fileExists(pszDir)) {
        return true;
    }
    return (::CreateDirectory(pszDir, NULL) == TRUE);
}

bool CSigmaApp::isFileReadonly(LPCTSTR pszFilename)
{
    DWORD dwAttrs = ::GetFileAttributes(pszFilename);
    if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return (dwAttrs & FILE_ATTRIBUTE_READONLY);
}

bool CSigmaApp::setFileReadWrite(LPCTSTR pszFilename)
{
    DWORD dwAttrs = ::GetFileAttributes(pszFilename);
    if (dwAttrs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    if (dwAttrs & FILE_ATTRIBUTE_READONLY) {
        dwAttrs &= ~FILE_ATTRIBUTE_READONLY;
        ::SetFileAttributes(pszFilename, dwAttrs);
        dwAttrs = ::GetFileAttributes(pszFilename);
    }

    return ((dwAttrs & FILE_ATTRIBUTE_READONLY) == 0);
}

int CSigmaApp::getShortFilename(char_t* szFilenameShort, const char_t* filename)
{
    int ii, nlen;
    char_t* psz = NULL;
    byte_t bret = 0;

    nlen = static_cast<int>(_tcslen(filename));

    if ((nlen < 6) || (nlen > ML_STRSIZE)) {
        return 1;
    }

    for (ii = nlen - 1; ii > 0; ii--) {
        if ((filename[ii] == uT('\\')) || (filename[ii] == uT('/'))) {
            psz = const_cast<char_t*>(&filename[ii + 1]);
            bret = 1;
            break;
        }
    }

    if (bret == 1) {
        _tcscpy(szFilenameShort, static_cast<const char_t*>(psz));
        return 0;
    }

    return 1;
}

BOOL CSigmaApp::AccessSigmaDoc(FILE* fp, sigmadoc_t *pDoc, int_t iOp)
{
    BOOL bRet = FALSE;
    char_t szMessage[ML_STRSIZE];

    if ((pDoc == NULL) || (fp == NULL)) {
        return FALSE;
    }

    // Garder l'ID donné dans BlankDocument()
    int_t nDocID = pDoc->nID;

    memset(szMessage, 0, ML_STRSIZE * sizeof(char_t));
    if (iOp == 0) {
        _tcscpy(szMessage, _T("Cannot open document: invalid file format.\r\n"));
    }
    else {
        _tcscpy(szMessage, _T("Cannot save document.\r\n"));
    }

    char_t szDelim[SIGMA_SSIZE];
    int_t iret = 0;

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->Software), sizeof(software_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->Software), sizeof(software_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    // Vérifier l'entête (en cas de lecture)
    if (iOp == 0) {
        if (_tcsicmp((const char_t*)(pDoc->Software.name), SIGMA_SOFTWARE_NAME) != 0) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
        if (_tcsicmp((const char_t*)(pDoc->Software.description), SIGMA_SOFTWARE_DESCRIPTION) != 0) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
        if ((pDoc->Software.version & 0xF000) > (SIGMA_SOFTWARE_DOCVERSION & 0xF000)) {
            _tcscpy(szMessage, _T("Cannot open document: invalid file version.\r\nYou can download newer SigmaGraph version here: www.hamady.org.\r\n"));
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
    }

    if (iOp == 0) { iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp); }
    else {
        _tcscpy(szDelim, _T("SigmaDoc_SID"));
        iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    }
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_SID")) != 0) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->nID), sizeof(int_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->nID), sizeof(int_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if (pDoc->nID < 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
        // Garder l'ID donné dans BlankDocument()
        pDoc->nID = nDocID;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->szName), ML_STRSIZE * sizeof(char_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->szName), ML_STRSIZE * sizeof(char_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->szNote), ML_STRSIZEW * sizeof(char_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->szNote), ML_STRSIZEW * sizeof(char_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->iType), sizeof(int_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->iType), sizeof(int_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if (pDoc->iType < 0) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->bStatus), sizeof(byte_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->bStatus), sizeof(byte_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->tmCreated), sizeof(date_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->tmCreated), sizeof(date_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->tmModified), sizeof(date_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->tmModified), sizeof(date_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->iColumnID), sizeof(long_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->iColumnID), sizeof(long_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if (pDoc->iColumnID < 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->iDataCount), sizeof(int_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->iDataCount), sizeof(int_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if ((pDoc->iDataCount < 1) || (pDoc->iDataCount > ML_MAXCOLUMNS)) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
    }

    if (iOp == 0) { iret = (int_t)fread(&(pDoc->iDataFitCount), sizeof(int_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pDoc->iDataFitCount), sizeof(int_t), 1, fp); }
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto AccessSigmaDoc_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if ((pDoc->iDataFitCount < 0) || (pDoc->iDataFitCount > ML_MAXCOLUMNS)) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto AccessSigmaDoc_RET;
        }
    }

    bRet = TRUE;

AccessSigmaDoc_RET:
    return bRet;
}

BOOL CSigmaApp::OpenSigmaDoc(LPCTSTR lpszFileName, sigmadoc_t *pDoc, vector_t *pColumnAutoX, plot_t *pPlot,
                                      WINDOWPLACEMENT *pPlacementPlot, WINDOWPLACEMENT *pPlacementDatasheet,
                                      WINDOWPLACEMENT *pPlacementDatasheetFit)
{
    BOOL bRet = FALSE;
    CString strT;

    if ((lpszFileName == NULL) || (pDoc == NULL) || (pColumnAutoX == NULL) || (pPlot == NULL)) {
        return FALSE;
    }

    // Vérifier que le fichier n'est pas déjà ouvert
    // parcourir tous les templates
    CDocTemplate* pTemplate;
    POSITION pos = GetFirstDocTemplatePosition();
    while (pos != NULL) {
        pTemplate = GetNextDocTemplate(pos);
        ASSERT(pTemplate);
        // tous les documents du template.
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD) {
            CSigmaDoc* pDocT = (CSigmaDoc*)(pTemplate->GetNextDoc(posD));
            if (pDocT != NULL) {
                if (pDocT->GetFilename() == (LPCTSTR)lpszFileName) {
                    CView* pView = NULL;
                    POSITION posV = pDocT->GetFirstViewPosition();
                    while (posV != NULL) {
                        pView = pDocT->GetNextView(posV);
                        if (pView == NULL) {
                            break;
                        }
                        if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewData))) {
                            pView->GetParentFrame()->ActivateFrame(SW_RESTORE);
                            return FALSE;
                        }
                    }
                }
            }
        }
    }

    char_t szMessage[ML_STRSIZE];
    memset(szMessage, 0, ML_STRSIZE * sizeof(char_t));

    char_t szShortFilename[ML_STRSIZE];
    memset(szShortFilename, 0, ML_STRSIZE * sizeof(char_t));
    CSigmaApp::getShortFilename(szShortFilename, (const char_t*) lpszFileName);

    _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot open '%s': invalid file format.\r\n"), (const char_t*) szShortFilename);

    // Vérifier la taille du fichier avant lecture
    long_t iFileLength = fl_filesize((const char_t*)lpszFileName);
    long_t iMinLength = (long_t)(sizeof(software_t) + sizeof(plot_v24_t));
    long_t iMaxLength = (long_t)(sizeof(sigmadoc_t) + sizeof(plot_v24_t) + (SIGMA_SSIZE * 77 * sizeof(char_t)) + (2 * (ML_MAXPOINTS * ML_MAXCOLUMNS * (sizeof(real_t) + sizeof(byte_t)))));
    iMaxLength += (3 * (long_t)(sizeof(WINDOWPLACEMENT)));
    if ((iFileLength < iMinLength) || (iFileLength > iMaxLength)) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    FILE *fp = NULL;

    if ((fp = _wfopen((const char_t*)lpszFileName, _T("rb"))) == NULL) {
        _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot open '%s'.\r\n"), (const char_t*) szShortFilename);
        Output((const char_t*) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    char_t szDelim[SIGMA_SSIZE];
    int_t iret = 0;

    char_t szBuffer[ML_STRSIZES];
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    int_t jj, ll, iCurveCount = 0;
    vector_t vecT;
    memset(&vecT, 0, sizeof(vector_t));

    // Lire les données statiques
    if (AccessSigmaDoc(fp, pDoc, 0) == FALSE) {
        goto OpenSigmaDoc_RET;
    }

    if (pDoc->iColumnID != (pDoc->iDataCount + pDoc->iDataFitCount + 1)) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    // Vérifier
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_DATA")) != 0) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    // Lire les colonnes
    for (jj = 0; jj < pDoc->iDataCount; jj++) {
        iret = (int_t)fread(&(pDoc->ColumnData[jj]), sizeof(vector_t), 1, fp);
        if (iret != 1) {
            if (jj > 0) {
                for (ll = 0; ll < jj; ll++) {
                    m_pLib->errcode = 0;
                    ml_vector_delete(&(pDoc->ColumnData[ll]), m_pLib);
                }
            }
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
    
        // Vérifier
        if (ml_vector_validate(&(pDoc->ColumnData[jj])) != 0) {
            if (jj > 0) {
                for (ll = 0; ll < jj; ll++) {
                    m_pLib->errcode = 0;
                    ml_vector_delete(&(pDoc->ColumnData[ll]), m_pLib);
                }
            }
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
        pDoc->ColumnData[jj].dat = NULL;
        pDoc->ColumnData[jj].mask = NULL;
        _tcscpy(szBuffer, (const char_t*)(pDoc->ColumnData[jj].label));
        m_pLib->errcode = 0;
        ml_vector_dupstatic(&vecT, (const vector_t*)(&(pDoc->ColumnData[jj])), m_pLib);
        ml_vector_create(&(pDoc->ColumnData[jj]), pDoc->ColumnData[jj].dim, pDoc->ColumnData[jj].id, (const char_t*)(pDoc->ColumnData[jj].name), NULL, m_pLib);
        ml_vector_dupstatic(&(pDoc->ColumnData[jj]), (const vector_t*)(&vecT), m_pLib);
        _tcscpy(pDoc->ColumnData[jj].label, (const char_t*)szBuffer);
        iret = (int_t)fread(pDoc->ColumnData[jj].dat, sizeof(real_t), pDoc->ColumnData[jj].dim, fp);
        if (iret != pDoc->ColumnData[jj].dim) {
            for (ll = 0; ll <= jj; ll++) {
                m_pLib->errcode = 0;
                ml_vector_delete(&(pDoc->ColumnData[ll]), m_pLib);
            }
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
        iret = (int_t)fread(pDoc->ColumnData[jj].mask, sizeof(byte_t), pDoc->ColumnData[jj].dim, fp);
        if (iret != pDoc->ColumnData[jj].dim) {
            for (ll = 0; ll <= jj; ll++) {
                m_pLib->errcode = 0;
                ml_vector_delete(&(pDoc->ColumnData[ll]), m_pLib);
            }
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
        // Colonne Y à tracer ?
        if (pDoc->ColumnData[jj].idx > 0) { iCurveCount += 1; }
    }

    // >> [MOD-SG240-002]: AutoX
    m_pLib->errcode = 0;
    ml_vector_create(pColumnAutoX, pDoc->ColumnData[0].dim, SIGMA_MAXCOLUMNS + 99, _T("X"), NULL, m_pLib);
    if (m_pLib->errcode != 0) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    _tcscpy(pColumnAutoX->label, _T("X"));
    pColumnAutoX->type = 0x10;
    for (int_t ii = 0; ii < pColumnAutoX->dim; ii++) {
        pColumnAutoX->dat[ii] = (real_t)ii;
        pColumnAutoX->mask[ii] = 0x00;
    }
    // <<

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_FitCol")) != 0) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    // Lire les colonnes de fit, le cas échéant
    if (pDoc->iDataFitCount > 0) {
        for (jj = 0; jj < pDoc->iDataFitCount; jj++) {
            iret = (int_t)fread(&(pDoc->ColumnDataFit[jj]), sizeof(vector_t), 1, fp);
            if (iret != 1) {
                if (jj > 0) {
                    for (ll = 0; ll < jj; ll++) {
                        m_pLib->errcode = 0;
                        ml_vector_delete(&(pDoc->ColumnDataFit[ll]), m_pLib);
                    }
                }
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto OpenSigmaDoc_RET;
            }
            pDoc->ColumnDataFit[jj].dat = NULL;
            pDoc->ColumnDataFit[jj].mask = NULL;
            m_pLib->errcode = 0;
            ml_vector_dupstatic(&vecT, (const vector_t*)(&(pDoc->ColumnDataFit[jj])), m_pLib);
            ml_vector_create(&(pDoc->ColumnDataFit[jj]), pDoc->ColumnDataFit[jj].dim, pDoc->ColumnDataFit[jj].id, (const char_t*)(pDoc->ColumnDataFit[jj].name), NULL, m_pLib);
            ml_vector_dupstatic(&(pDoc->ColumnDataFit[jj]), (const vector_t*)(&vecT), m_pLib);
            iret = (int_t)fread(pDoc->ColumnDataFit[jj].dat, sizeof(real_t), pDoc->ColumnDataFit[jj].dim, fp);
            if (iret != pDoc->ColumnDataFit[jj].dim) {
                for (ll = 0; ll <= jj; ll++) {
                    m_pLib->errcode = 0;
                    ml_vector_delete(&(pDoc->ColumnDataFit[ll]), m_pLib);
                }
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto OpenSigmaDoc_RET;
            }
            iret = (int_t)fread(pDoc->ColumnDataFit[jj].mask, sizeof(byte_t), pDoc->ColumnDataFit[jj].dim, fp);
            if (iret != pDoc->ColumnDataFit[jj].dim) {
                for (ll = 0; ll <= jj; ll++) {
                    m_pLib->errcode = 0;
                    ml_vector_delete(&(pDoc->ColumnDataFit[ll]), m_pLib);
                }
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto OpenSigmaDoc_RET;
            }
            // Colonne Y à tracer (tous les fits seront tracés...)
            if (pDoc->ColumnDataFit[jj].idx > 0) { iCurveCount += 1; }
        }
    }
    else {
        int_t iNothing = 0;
        iret = (int_t)fread(&iNothing, sizeof(int_t), 1, fp);
        if (iret != 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_StatsC")) != 0) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    // Lire les statistiques
    for (jj = 0; jj < pDoc->iDataCount; jj++) {
        iret = (int_t)fread(&(pDoc->Stats[jj]), sizeof(stat_t), 1, fp);
        if (iret != 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_FitRep")) != 0) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    // Save file position, if need to read again if document contains fit data with an older SigmaGraph version (ensuring compatibility)
    long ifpos = ftell(fp);

    // Lire les données de Fit
    if (pDoc->iDataFitCount > 1) {
        for (jj = 0; jj < (pDoc->iDataCount >> 1); jj++) {
            iret = (int_t) fread(&(pDoc->Fit[jj]), sizeof(fit_t), 1, fp);
            if (iret != 1) {
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto OpenSigmaDoc_RET;
            }
        }
    }
    else {
        int_t iNothing = 0;
        iret = (int_t)fread(&iNothing, sizeof(int_t), 1, fp);
        if (iret != 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    if (_tcsicmp((const char_t*) szDelim, _T("SigmaDoc_PlotId")) != 0) {

        if (pDoc->iDataFitCount <= 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }

        // Read again if document contains fit data with an older SigmaGraph version (ensuring compatibility)
        if (fseek(fp, ifpos, SEEK_SET) != 0) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }

        fit_legacy_t tFit;
        for (jj = 0; jj < pDoc->iDataCount >> 1; jj++) {
            iret = (int_t) fread(&tFit, sizeof(fit_legacy_t), 1, fp);
            if (iret != 1) {
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto OpenSigmaDoc_RET;
            }
            ml_fit_fromlegacy(&(pDoc->Fit[jj]), (const fit_legacy_t*) (&tFit));
        }

        iret = (int_t) fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
        if (iret != SIGMA_SSIZE) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
        if (_tcsicmp((const char_t*) szDelim, _T("SigmaDoc_PlotId")) != 0) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto OpenSigmaDoc_RET;
        }
    }

    int_t iX[ML_MAXCURVES], iY[ML_MAXCURVES],
        iFitX[ML_MAXCURVES], iFitY[ML_MAXCURVES];
    iret = (int_t)fread(iX, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    iret = (int_t)fread(iY, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    iret = (int_t)fread(iFitX, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    iret = (int_t)fread(iFitY, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_PLOT")) != 0) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    // Lire le graphe

    iret = (int_t) fread(pPlot, sizeof(plot_v24_t), 1, fp);
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    pPlot->errcode = 0;
    memset(pPlot->message, 0, ML_STRSIZE * sizeof(char_t));
    memset(&(pPlot->plint), 0, sizeof(plint_t));

    pl_validate(pPlot, 1);
    if (pPlot->errcode != 0) {    // Error
        Output((LPCTSTR) (pPlot->message), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    if (iCurveCount > 0) {
        vector_t *pvecT = NULL, *pvecTT = NULL;
        vector_t *pVec = NULL;
        pPlot->errcode = 0;
        byte_t axisused = 0x00;
        int_t iCurveIndex = 0;
        byte_t curvestatus;
        for (jj = 0; jj < ML_MAXCURVES; jj++) {
            if (iX[jj] == pColumnAutoX->id) {
                if ((iY[jj] < 1) || (iY[jj] > pDoc->iDataCount)) {
                    iY[jj] = 0;
                }
                if (iY[jj] > 0) {
                    // >> Error Bars
                    pvecT = NULL;    pvecTT = NULL;
                    if (pDoc->ColumnData[iY[jj] - 1].ide > 0) {
                        for (ll = 0; ll < pDoc->iDataCount; ll++) {
                            if ((pDoc->ColumnData[ll].id == pDoc->ColumnData[iY[jj] - 1].ide) && ((pDoc->ColumnData[ll].type & 0xF0) == 0x40)) {
                                pvecTT = &(pDoc->ColumnData[ll]);
                                break;
                            }
                        }
                    }
                    // <<
                    curvestatus = pPlot->curve[jj].status;        // [FIX-SG240-003]
                    pPlot->errcode = 0;
                    pl_curve_add(pPlot, pColumnAutoX, &(pDoc->ColumnData[iY[jj] - 1]), pvecT, pvecTT, pPlot->curve[jj].axisused, 0x01);
                    pPlot->curve[jj].status = curvestatus;
                }
            }
            else {
                if ((iX[jj] < 1) || (iX[jj] > pDoc->iDataCount)
                    || (iY[jj] < 1) || (iY[jj] > pDoc->iDataCount)) {
                    iX[jj] = 0;
                    iY[jj] = 0;
                }
                if ((iX[jj] > 0) && (iY[jj] > 0)) {
                    // >> Error Bars
                    pvecT = NULL;    pvecTT = NULL;
                    if (pDoc->ColumnData[iX[jj] - 1].ide > 0) {
                        for (ll = 0; ll < pDoc->iDataCount; ll++) {
                            if ((pDoc->ColumnData[ll].id == pDoc->ColumnData[iX[jj] - 1].ide) && ((pDoc->ColumnData[ll].type & 0xF0) == 0x30)) {
                                pvecT = &(pDoc->ColumnData[ll]);
                                break;
                            }
                        }
                    }
                    if (pDoc->ColumnData[iY[jj] - 1].ide > 0) {
                        for (ll = 0; ll < pDoc->iDataCount; ll++) {
                            if ((pDoc->ColumnData[ll].id == pDoc->ColumnData[iY[jj] - 1].ide) && ((pDoc->ColumnData[ll].type & 0xF0) == 0x40)) {
                                pvecTT = &(pDoc->ColumnData[ll]);
                                break;
                            }
                        }
                    }
                    // <<
                    curvestatus = pPlot->curve[jj].status;        // [FIX-SG240-003]
                    pPlot->errcode = 0;
                    pl_curve_add(pPlot, &(pDoc->ColumnData[iX[jj] - 1]), &(pDoc->ColumnData[iY[jj] - 1]), pvecT, pvecTT, pPlot->curve[jj].axisused, 0x00);
                    pPlot->curve[jj].status = curvestatus;
                }
            }
        }

        iCurveIndex = pPlot->curvecount;
        for (jj = 0; jj < ML_MAXCURVES; jj++) {
            if ((iFitX[jj] < 1) || (iFitX[jj] > pDoc->iDataFitCount)
                || (iFitY[jj] < 1) || (iFitY[jj] > pDoc->iDataFitCount)) {
                iFitX[jj] = 0;
                iFitY[jj] = 0;
            }
            if ((iFitX[jj] > 0) && (iFitY[jj] > 0)) {
                // >> [FIX-SG220-20100328]
                // Le fit doit utiliser les mêmes axes que la courbe fittée
                for (ll = 0; ll < pPlot->curvecount; ll++) {
                    if (pPlot->curve[ll].y->idf == pDoc->ColumnDataFit[iFitY[jj] - 1].id) {
                        axisused = pPlot->curve[ll].axisused;
                        break;
                    }
                }
                if (ll >= pPlot->curvecount) {
                    if (iCurveIndex < pPlot->curvecount) {
                        axisused = pPlot->curve[iCurveIndex].axisused;
                    }
                }
                // <<
                pPlot->errcode = 0;
                pl_curve_add(pPlot, &(pDoc->ColumnDataFit[iFitX[jj] - 1]), &(pDoc->ColumnDataFit[iFitY[jj] - 1]), NULL, NULL, axisused, 0x00);
                iCurveIndex += 1;
            }
        }
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_END")) != 0) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto OpenSigmaDoc_RET;
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret == SIGMA_SSIZE) {
        if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_OPTW")) == 0) {

            CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
            WINDOWPLACEMENT mdiPlace;
            memset(&mdiPlace, 0, sizeof(WINDOWPLACEMENT));
            mdiPlace.length = sizeof(WINDOWPLACEMENT);
            pMainFrame->GetWindowPlacement(&mdiPlace);
            LONG iX0 = 0, iY0 = 0, iW = mdiPlace.rcNormalPosition.right - mdiPlace.rcNormalPosition.left,
                iH = mdiPlace.rcNormalPosition.bottom - mdiPlace.rcNormalPosition.top;

            iret = (int_t)fread(pPlacementPlot, sizeof(WINDOWPLACEMENT), 1, fp);
            if (iret == 1) {

                if ((pPlacementPlot->rcNormalPosition.top < iY0) || (pPlacementPlot->rcNormalPosition.left < iX0)
                    || (pPlacementPlot->rcNormalPosition.bottom > (iY0 + iH)) || (pPlacementPlot->rcNormalPosition.right > (iX0 + iW))
                    || (pPlacementPlot->rcNormalPosition.right < (pPlacementPlot->rcNormalPosition.left + SIGMA_MINWIDTH3))
                    || (pPlacementPlot->rcNormalPosition.bottom < (pPlacementPlot->rcNormalPosition.top + SIGMA_MINHEIGHT3))) {

                    LONG iWp = pPlacementPlot->rcNormalPosition.right - pPlacementPlot->rcNormalPosition.left,
                        iHp = pPlacementPlot->rcNormalPosition.bottom - pPlacementPlot->rcNormalPosition.top;

                    pPlacementPlot->length = sizeof(WINDOWPLACEMENT);
                    pPlacementPlot->rcNormalPosition.left = iX0;
                    pPlacementPlot->rcNormalPosition.top = iY0;
                    if (iWp > (iW - 16)) {
                        iWp = iW - 16;
                    }
                    if (iHp > (iH - 96)) {
                        iHp = iH - 96;
                    }
                    pPlacementPlot->rcNormalPosition.right = iX0 + iWp;
                    pPlacementPlot->rcNormalPosition.bottom = iY0 + iHp;
                }
            }
            else {
                pPlacementPlot->length = sizeof(WINDOWPLACEMENT);
                pPlacementPlot->rcNormalPosition.left = iX0;
                pPlacementPlot->rcNormalPosition.top = iY0;
                pPlacementPlot->rcNormalPosition.right = iX0 + SIGMA_DEFWIDTH_PLOT;
                pPlacementPlot->rcNormalPosition.bottom = iY0 + SIGMA_DEFHEIGHT_PLOT;
            }

            iret = (int_t)fread(pPlacementDatasheet, sizeof(WINDOWPLACEMENT), 1, fp);
            if (iret == 1) {

                if ((pPlacementDatasheet->rcNormalPosition.top < iY0) || (pPlacementDatasheet->rcNormalPosition.left < iX0)
                    || (pPlacementDatasheet->rcNormalPosition.bottom > (iY0 + iH)) || (pPlacementDatasheet->rcNormalPosition.right > (iX0 + iW))
                    || (pPlacementDatasheet->rcNormalPosition.right < (pPlacementDatasheet->rcNormalPosition.left + SIGMA_MINWIDTH3))
                    || (pPlacementDatasheet->rcNormalPosition.bottom < (pPlacementDatasheet->rcNormalPosition.top + SIGMA_MINHEIGHT3))) {

                    LONG iWd = pPlacementDatasheet->rcNormalPosition.right - pPlacementDatasheet->rcNormalPosition.left,
                        iHd = pPlacementDatasheet->rcNormalPosition.bottom - pPlacementDatasheet->rcNormalPosition.top;

                    pPlacementDatasheet->length = sizeof(WINDOWPLACEMENT);
                    pPlacementDatasheet->rcNormalPosition.left = iX0;
                    pPlacementDatasheet->rcNormalPosition.top = iY0;
                    if (iWd > (iW - 16)) {
                        iWd = iW - 16;
                    }
                    if (iHd > (iH - 96)) {
                        iHd = iH - 96;
                    }
                    pPlacementDatasheet->rcNormalPosition.right = iX0 + iWd;
                    pPlacementDatasheet->rcNormalPosition.bottom = iY0 + iHd;
                }
            }
            else {
                pPlacementDatasheet->length = sizeof(WINDOWPLACEMENT);
                pPlacementDatasheet->rcNormalPosition.left = iX0;
                pPlacementDatasheet->rcNormalPosition.top = iY0;
                pPlacementDatasheet->rcNormalPosition.right = iX0 + SIGMA_DEFWIDTH_DATA;
                pPlacementDatasheet->rcNormalPosition.bottom = iY0 + SIGMA_DEFHEIGHT_DATA;
            }

            iret = (int_t)fread(pPlacementDatasheetFit, sizeof(WINDOWPLACEMENT), 1, fp);
            if (iret == 1) {

                if ((pPlacementDatasheetFit->rcNormalPosition.top < iY0) || (pPlacementDatasheetFit->rcNormalPosition.left < iX0)
                    || (pPlacementDatasheetFit->rcNormalPosition.bottom > (iY0 + iH)) || (pPlacementDatasheetFit->rcNormalPosition.right > (iX0 + iW))
                    || (pPlacementDatasheetFit->rcNormalPosition.right < (pPlacementDatasheetFit->rcNormalPosition.left + SIGMA_MINWIDTH3))
                    || (pPlacementDatasheetFit->rcNormalPosition.bottom < (pPlacementDatasheetFit->rcNormalPosition.top + SIGMA_MINHEIGHT3))) {

                    LONG iWdf = pPlacementDatasheetFit->rcNormalPosition.right - pPlacementDatasheetFit->rcNormalPosition.left,
                        iHdf = pPlacementDatasheetFit->rcNormalPosition.bottom - pPlacementDatasheetFit->rcNormalPosition.top;

                    pPlacementDatasheetFit->length = sizeof(WINDOWPLACEMENT);
                    pPlacementDatasheetFit->rcNormalPosition.left = iX0;
                    pPlacementDatasheetFit->rcNormalPosition.top = iY0;
                    if (iWdf > (iW - 16)) {
                        iWdf = iW - 16;
                    }
                    if (iHdf > (iH - 96)) {
                        iHdf = iH - 96;
                    }
                    pPlacementDatasheetFit->rcNormalPosition.right = iX0 + iWdf;
                    pPlacementDatasheetFit->rcNormalPosition.bottom = iY0 + iHdf;
                }
            }
            else {
                pPlacementDatasheetFit->length = sizeof(WINDOWPLACEMENT);
                pPlacementDatasheetFit->rcNormalPosition.left = iX0;
                pPlacementDatasheetFit->rcNormalPosition.top = iY0;
                pPlacementDatasheetFit->rcNormalPosition.right = iX0 + SIGMA_DEFWIDTH_DATA;
                pPlacementDatasheetFit->rcNormalPosition.bottom = iY0 + SIGMA_DEFHEIGHT_DATA;
            }
        }
    }

    bRet = TRUE;
    strT = lpszFileName;
    strT += _T(" opened successfully.\r\n");
    Output(strT, TRUE, FALSE, -1, 0);

OpenSigmaDoc_RET:
    fclose(fp); fp = NULL;
    return bRet;
}

CString CSigmaApp::getFitReport(fit_t *pFit)
{
    CString strReport = _T("");

    if (pFit->parcount < 2) {
        return strReport;
    }

    int ifun = pFit->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    char_t szBuffer[ML_STRSIZES];
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    if (pFit->index == ML_FIT_USERINDEX) {
        strReport.Format(_T("# Model: %s\r\n# Function: %s\r\n# Parameters: %d"), pFit->name, pFit->function, pFit->parcount);
    }
    else {
        strReport.Format(_T("# Model: %s\r\n# Parameters: %d"), pFit->name, pFit->parcount);
    }

    if (bPeak) {
        strReport.Append(_T("\r\n# a = "));
        SigmaApp.Reformat(pFit->fpar[0], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (pFit->mpar[0] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(bPeakM ? _T("\r\n# b1 = ") : _T("\r\n# b = "));
        SigmaApp.Reformat(pFit->fpar[1], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (pFit->mpar[1] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(bPeakM ? _T(" ; c1 = ") : _T(" ; c = "));
        SigmaApp.Reformat(pFit->fpar[2], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (pFit->mpar[2] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(bPeakM ? _T(" ; d1 = ") : _T(" ; d = "));
        SigmaApp.Reformat(pFit->fpar[3], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (pFit->mpar[3] == 0) {
            strReport.Append(_T(" (fixed)"));
        }

        if (pFit->parcount > 4) {
            strReport.Append(_T("\r\n# b2 = "));
            SigmaApp.Reformat(pFit->fpar[4], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[4] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 5) {
            strReport.Append(_T(" ; c2 = "));
            SigmaApp.Reformat(pFit->fpar[5], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[5] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 6) {
            strReport.Append(_T(" ; d2 = "));
            SigmaApp.Reformat(pFit->fpar[6], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[6] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 7) {
            strReport.Append(_T("\r\n# b3 = "));
            SigmaApp.Reformat(pFit->fpar[7], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[7] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (pFit->parcount > 8) {
            strReport.Append(_T(" ; c3 = "));
            SigmaApp.Reformat(pFit->fpar[8], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[8] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 9) {
            strReport.Append(_T(" ; d3 = "));
            SigmaApp.Reformat(pFit->fpar[9], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[9] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 10) {
            strReport.Append(_T("\r\n# b4 = "));
            SigmaApp.Reformat(pFit->fpar[10], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[10] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 11) {
            strReport.Append(_T(" ; c4 = "));
            SigmaApp.Reformat(pFit->fpar[11], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[11] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 12) {
            strReport.Append(_T(" ; d4 = "));
            SigmaApp.Reformat(pFit->fpar[12], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[12] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 13) {
            strReport.Append(_T("\r\n# b5 = "));
            SigmaApp.Reformat(pFit->fpar[13], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[13] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 14) {
            strReport.Append(_T(" ; c5 = "));
            SigmaApp.Reformat(pFit->fpar[14], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[14] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 15) {
            strReport.Append(_T(" ; d5 = "));
            SigmaApp.Reformat(pFit->fpar[15], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[15] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
    }
    else {
        strReport.Append(_T("\r\n# a = "));
        SigmaApp.Reformat(pFit->fpar[0], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (pFit->mpar[0] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(_T(" ; b = "));
        SigmaApp.Reformat(pFit->fpar[1], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (pFit->mpar[1] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        if (pFit->parcount > 2) {
            strReport.Append(_T(" ; c = "));
            SigmaApp.Reformat(pFit->fpar[2], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[2] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 3) {
            strReport.Append(_T("\r\n# d = "));
            SigmaApp.Reformat(pFit->fpar[3], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[3] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 4) {
            strReport.Append(_T(" ; e = "));
            SigmaApp.Reformat(pFit->fpar[4], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[4] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 5) {
            strReport.Append(_T(" ; f = "));
            SigmaApp.Reformat(pFit->fpar[5], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[5] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 6) {
            strReport.Append(_T("\r\n# g = "));
            SigmaApp.Reformat(pFit->fpar[6], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[6] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 7) {
            strReport.Append(_T(" ; h = "));
            SigmaApp.Reformat(pFit->fpar[7], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[7] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (pFit->parcount > 8) {
            strReport.Append(_T(" ; i = "));
            SigmaApp.Reformat(pFit->fpar[8], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[8] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 9) {
            strReport.Append(_T("\r\n# j = "));
            SigmaApp.Reformat(pFit->fpar[9], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[9] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 10) {
            strReport.Append(_T(" ; k = "));
            SigmaApp.Reformat(pFit->fpar[10], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[10] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 11) {
            strReport.Append(_T(" ; l = "));
            SigmaApp.Reformat(pFit->fpar[11], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[11] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 12) {
            strReport.Append(_T("\r\n# m = "));
            SigmaApp.Reformat(pFit->fpar[12], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[12] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 13) {
            strReport.Append(_T(" ; n = "));
            SigmaApp.Reformat(pFit->fpar[13], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[13] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 14) {
            strReport.Append(_T(" ; o = "));
            SigmaApp.Reformat(pFit->fpar[14], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[14] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (pFit->parcount > 15) {
            strReport.Append(_T("\r\n# p = "));
            SigmaApp.Reformat(pFit->fpar[15], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (pFit->mpar[15] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
    }

    strReport.Append(_T("\r\n# Chi² = "));
    SigmaApp.Reformat(pFit->chi, szBuffer, ML_STRSIZES - 1);
    strReport.Append((LPCTSTR) szBuffer);
    strReport.Append(_T("\r\n"));

    return strReport;
}

bool CSigmaApp::isAutoExportChecked()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame) {
        CMenu *pMenu = pFrame->GetMenu();
        if (pMenu) {
            CMenu *pSubMenu = pMenu->GetSubMenu(0);
            if (pSubMenu) {
                bool bChecked = (pSubMenu->GetMenuState(ID_FILE_AUTOEXPORT, MF_BYCOMMAND) & MF_CHECKED) != 0;
                SigmaApp.m_Options.opt[AUTOEXPORT_INDEX] = bChecked ? 1 : 0;
                return bChecked;
            }
        }
    }
    return true;
}

BOOL CSigmaApp::SaveSigmaDoc(LPCTSTR lpszFileName, const sigmadoc_t *pDoc, const vector_t *pColumnAutoX, const plot_t *pPlot,
                                      const WINDOWPLACEMENT *pPlacementPlot, const WINDOWPLACEMENT *pPlacementDatasheet, const WINDOWPLACEMENT *pPlacementDatasheetFit)
{
    BOOL bRet = FALSE;

    if ((lpszFileName == NULL) || (pDoc == NULL) || (pColumnAutoX == NULL) || (pPlot == NULL)) {
        return FALSE;
    }

    if ((pDoc->iDataCount < 1) || (pDoc->iDataCount > ML_MAXCOLUMNS)) {
        return FALSE;
    }

    bool bExport = isAutoExportChecked();
    char_t szFilenameData[ML_STRSIZE];
    char_t szFilenamePlot[ML_STRSIZE];
    char_t szFilenameFit[ML_STRSIZE];
    if (bExport) {
        char_t *pszT = NULL;
        memset(szFilenameData, 0, ML_STRSIZE * sizeof(char_t));
        memset(szFilenamePlot, 0, ML_STRSIZE * sizeof(char_t));
        memset(szFilenameFit, 0, ML_STRSIZE * sizeof(char_t));
        _tcscpy(szFilenameData, lpszFileName);
        if ((pszT = _tcsstr(szFilenameData, _T(".sid"))) != NULL) {
            *pszT = _T('\0');
            _tcscpy(szFilenamePlot, (const char_t*) szFilenameData);
            _tcscpy(szFilenameFit, (const char_t*) szFilenameData);
            _tcscat(szFilenameData, _T("_data.txt"));
            _tcscat(szFilenamePlot, _T("_plot.svg"));
            _tcscat(szFilenameFit, _T("_fit.txt"));
        }
        bExport = (pszT != NULL);
    }

    char_t szMessage[ML_STRSIZE * 2];
    memset(szMessage, 0, ML_STRSIZE * 2 * sizeof(char_t));
    _tcscpy(szMessage, _T("Cannot save document.\r\n"));

    char_t szShortFilename[ML_STRSIZE];
    memset(szShortFilename, 0, ML_STRSIZE * sizeof(char_t));
    CSigmaApp::getShortFilename(szShortFilename, (const char_t*) lpszFileName);

    _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot save '%s'.\r\n"), (const char_t*) szShortFilename);

    sigmadoc_t *pDocT = new(std::nothrow) sigmadoc_t;
    if (pDocT == NULL) {
        _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot save '%s': insufficient memory.\r\n"), (const char_t*) szShortFilename);
        Output((const char_t*) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    memset(pDocT, 0, sizeof(sigmadoc_t));
    if (CopyDocument(pDocT, pDoc) == FALSE) {
        _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot save '%s': invalid data.\r\n"), (const char_t*) szShortFilename);
        Output((const char_t*) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    plot_t *pPlotT = new(std::nothrow) plot_t;
    if (pPlotT == NULL) {
        if (pDocT) {
            delete pDocT; pDocT = NULL;
        }
        _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot save '%s': insufficient memory.\r\n"), (const char_t*) szShortFilename);
        Output((const char_t*) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    pPlotT->errcode = 0;
    pl_plot_dupstatic(pPlotT, pPlot);
    if (pPlotT->errcode != 0) {
        if (pDocT) {
            delete pDocT; pDocT = NULL;
        }
        if (pPlotT) {
            delete pPlotT; pPlotT = NULL;
        }
        _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot save '%s': invalid data.\r\n"), (const char_t*) szShortFilename);
        Output((const char_t*) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    if (CSigmaApp::fileExists(lpszFileName) && CSigmaApp::isFileReadonly((const char_t*) lpszFileName)) {
        if (m_pMainWnd->MessageBox(_T("Read-only file. Set to read-write?"), _T("SigmaGraph"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
            if (pDocT) {
                delete pDocT; pDocT = NULL;
            }
            if (pPlotT) {
                delete pPlotT; pPlotT = NULL;
            }
            _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot save '%s': read-only file.\r\n"), (const char_t*) szShortFilename);
            Output((const char_t*) szMessage, TRUE, FALSE, SIGMA_OUTPUT_ERROR);
            return FALSE;
        }
        if (CSigmaApp::setFileReadWrite((const char_t*) lpszFileName) == false) {
            if (pDocT) {
                delete pDocT; pDocT = NULL;
            }
            if (pPlotT) {
                delete pPlotT; pPlotT = NULL;
            }
            _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot set '%s' to read-write.\r\n"), (const char_t*) szShortFilename);
            Output((const char_t*) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return FALSE;
        }
    }

    FILE *fp = NULL;

    if ((fp = _wfopen((const char_t*)lpszFileName, _T("wb"))) == NULL) {
        if (pDocT) {
            delete pDocT; pDocT = NULL;
        }
        if (pPlotT) {
            delete pPlotT; pPlotT = NULL;
        }
        _tcsprintf(szMessage, ML_STRSIZE - 1, _T("Cannot save '%s': file writing failed.\r\n"), (const char_t*) szShortFilename);
        Output((const char_t*) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    char_t szDelim[SIGMA_SSIZE];
    int_t jj, iret = 0;

    // >> [FIX-SG220-20100328]
    // Puisque je modifie les ID des colonnes (nouveaux IDs : de 1 à iDataCount + iDataFitCount),
    // il faut en tenir compte ici
    pDocT->iColumnID = pDocT->iDataCount + pDocT->iDataFitCount + 1;
    // <<

    int_t iDataCount = pDocT->iDataCount;

    real_t *pdat = NULL;
    byte_t *pmask = NULL;
    int_t ll, mm, idC = 1, idCF = pDocT->iDataCount + 1, idO, idxp;
    int_t iX[ML_MAXCURVES], iY[ML_MAXCURVES], iFitX[ML_MAXCURVES], iFitY[ML_MAXCURVES];
    memset(iX, 0, ML_MAXCURVES * sizeof(int_t));
    memset(iY, 0, ML_MAXCURVES * sizeof(int_t));
    memset(iFitX, 0, ML_MAXCURVES * sizeof(int_t));
    memset(iFitY, 0, ML_MAXCURVES * sizeof(int_t));

    int_t ifound = 0, ifoundF = 0, nn = 0, rr, tt, uu;
    mm = 0;

    if (AccessSigmaDoc(fp, pDocT, 1) == FALSE) {
        goto SaveSigmaDoc_RET;
    }

    _tcscpy(szDelim, _T("SigmaDoc_DATA"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    for (jj = 0; jj < iDataCount; jj++) {
        pDocT->ColumnData[jj].idr = 0;
    }
    if (pDocT->iDataFitCount > 0) {
        for (jj = 0; jj < pDocT->iDataFitCount; jj++) {
            pDocT->ColumnDataFit[jj].idr = 0;
        }
    }

    for (jj = 0; jj < iDataCount; jj++) {
        pdat = pDocT->ColumnData[jj].dat;
        pmask = pDocT->ColumnData[jj].mask;
        pDocT->ColumnData[jj].dat = NULL;
        pDocT->ColumnData[jj].mask = NULL;

        // Modifier les ID des colonnes (nouveaux IDs : de 1 à iDataCount)
        // Colonne X
        if (pDocT->ColumnData[jj].idr == 0) {
            if ((pDocT->ColumnData[jj].type & 0xF0) == 0x10) {
                idO = pDocT->ColumnData[jj].id;
                pDocT->ColumnData[jj].id = idC;
                pDocT->ColumnData[jj].idr = 1;
                idC += 1;
                for (ll = 0; ll < iDataCount; ll++) {
                    if (((pDocT->ColumnData[ll].type & 0xF0) == 0x20) && (pDocT->ColumnData[ll].idx == idO) && (pDocT->ColumnData[ll].idr == 0)) {
                        pDocT->ColumnData[ll].idx = pDocT->ColumnData[jj].id;
                    }
                }
                if (pDocT->ColumnData[jj].ide > 0) {
                    for (ll = 0; ll < pDocT->iDataCount; ll++) {
                        if (((pDocT->ColumnData[ll].type & 0xF0) == 0x30) && (pDocT->ColumnData[jj].ide == pDocT->ColumnData[ll].id) && (pDocT->ColumnData[ll].idr == 0)) {
                            idO = pDocT->ColumnData[ll].id;
                            pDocT->ColumnData[ll].id = idC;
                            pDocT->ColumnData[ll].idr = 1;
                            pDocT->ColumnData[jj].ide = pDocT->ColumnData[ll].id;
                            idC += 1;
                            for (rr = 0; rr < pDocT->iDataCount; rr++) {
                                if (((pDocT->ColumnData[rr].type & 0xF0) == 0x10) && (pDocT->ColumnData[rr].ide == idO)) {
                                    pDocT->ColumnData[rr].ide = pDocT->ColumnData[ll].id;
                                }
                            }
                            break;
                        }
                    }
                }

            }
            // Colonne Y
            else if ((pDocT->ColumnData[jj].type & 0xF0) == 0x20) {
                idO = pDocT->ColumnData[jj].id;
                pDocT->ColumnData[jj].id = idC;
                pDocT->ColumnData[jj].idr = 1;
                idC += 1;
                // >> [FIX-SG240-004]
                if (pPlot->curvecount > 0) {
                    for (ll = 0; ll < pPlot->curvecount; ll++) {
                        if (pPlot->curve[ll].y->id == idO) {
                            iY[mm] = jj + 1;
                            if (pDocT->ColumnData[jj].idx == pColumnAutoX->id) {
                                iX[mm] = pColumnAutoX->id;
                                mm += 1;
                                break;
                            }
                            else {
                                for (rr = 0; rr < iDataCount; rr++) {
                                    if (pDocT->ColumnData[jj].idx == pDocT->ColumnData[rr].id) {
                                        iX[mm] = rr + 1;
                                        mm += 1;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
                // >>
                if (pDocT->ColumnData[jj].ide > 0) {
                    for (ll = 0; ll < pDocT->iDataCount; ll++) {
                        if (((pDocT->ColumnData[ll].type & 0xF0) == 0x40) && (pDocT->ColumnData[jj].ide == pDocT->ColumnData[ll].id) && (pDocT->ColumnData[ll].idr == 0)) {
                            idO = pDocT->ColumnData[ll].id;
                            pDocT->ColumnData[ll].id = idC;
                            pDocT->ColumnData[ll].idr = 1;
                            pDocT->ColumnData[jj].ide = pDocT->ColumnData[ll].id;
                            idC += 1;
                            for (rr = 0; rr < pDocT->iDataCount; rr++) {
                                if (((pDocT->ColumnData[rr].type & 0xF0) == 0x20) && (pDocT->ColumnData[rr].ide == idO)) {
                                    pDocT->ColumnData[rr].ide = pDocT->ColumnData[ll].id;
                                }
                            }
                            break;
                        }
                    }
                }

                if (pDocT->iDataFitCount < 1) {
                    pDocT->ColumnData[jj].idf = 0;
                }

                if ((pDocT->iDataFitCount > 0) && (pDocT->ColumnData[jj].idf > 0)) {

                    for (ll = 0; ll < pDocT->iDataFitCount; ll++) {

                        if ((pDocT->ColumnData[jj].idf == pDocT->ColumnDataFit[ll].id) && (pDocT->ColumnDataFit[ll].idr == 0)) {
                        
                            idO = pDocT->ColumnDataFit[ll].id;
                            pDocT->ColumnDataFit[ll].id = idCF;
                            pDocT->ColumnDataFit[ll].idr = 1;
                            pDocT->ColumnData[jj].idf = pDocT->ColumnDataFit[ll].id;
                            idCF += 1;

                            idxp = 0;

                            // >> [FIX-SG240-004]
                            if (pPlot->curvecount > 0) {
                                for (rr = 0; rr < pPlot->curvecount; rr++) {
                                    if (pPlot->curve[rr].y->id == idO) {
                                        iFitY[nn] = ll + 1;
                                        for (tt = 0; tt < pDocT->iDataFitCount; tt++) {
                                            if (pDocT->ColumnDataFit[ll].idx == pDocT->ColumnDataFit[tt].id) {
                                                iFitX[nn] = tt + 1;
                                                idxp = iFitX[nn];
                                                nn += 1;
                                                break;
                                            }
                                        }
                                        break;
                                    }
                                }
                            }
                            // <<

                            for (uu = 0; uu < pDocT->iDataFitCount; uu++) {
                                if ((pDocT->ColumnDataFit[ll].idx == pDocT->ColumnDataFit[uu].id) && (pDocT->ColumnDataFit[uu].idr == 0)) {
                                    pDocT->ColumnDataFit[uu].id = idCF;
                                    pDocT->ColumnDataFit[uu].idr = 1;
                                    pDocT->ColumnDataFit[ll].idx = pDocT->ColumnDataFit[uu].id;
                                    idCF += 1;
                                    break;
                                }
                            }


                            // >> if (ide > 0) for a fit column, then there are more than one curve (e.g. mutli-peak fitting)
                            if (pDocT->iDataFitCount > 2) {
                                for (rr = 0; rr < pDocT->iDataFitCount; rr++) {
                                    if ((pDocT->ColumnDataFit[rr].ide == idO) && (pDocT->ColumnDataFit[rr].idr == 0)) {
                                        pDocT->ColumnDataFit[rr].ide = pDocT->ColumnDataFit[ll].id;
                                        pDocT->ColumnDataFit[rr].id = idCF;
                                        pDocT->ColumnDataFit[rr].idr = 1;
                                        pDocT->ColumnDataFit[rr].idf = 0;
                                        if (pDocT->ColumnDataFit[rr].idx > 0) {
                                            pDocT->ColumnDataFit[rr].idx = pDocT->ColumnDataFit[ll].idx;
                                            iFitX[nn] = idxp;
                                            iFitY[nn] = rr + 1;
                                            nn += 1;
                                        }
                                        idCF += 1;
                                    }
                                }
                            }
                            // <<

                            for (uu = 0; uu < pDocT->iDataFitCount >> 1; uu++) {
                                if (idO == pDocT->Fit[uu].id) {
                                    pDocT->Fit[uu].id = pDocT->ColumnDataFit[ll].id;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }

        iret = (int_t) fwrite(&(pDocT->ColumnData[jj]), sizeof(vector_t), 1, fp);
        if (iret != 1) {
            for (ll = jj; ll < iDataCount; ll++) {
                m_pLib->errcode = 0;
                ml_vector_delete(&(pDocT->ColumnData[ll]), m_pLib);
            }
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto SaveSigmaDoc_RET;
        }
        iret = (int_t) fwrite(pdat, sizeof(real_t), pDocT->ColumnData[jj].dim, fp);
        if (iret != pDocT->ColumnData[jj].dim) {
            for (ll = jj; ll < iDataCount; ll++) {
                m_pLib->errcode = 0;
                ml_vector_delete(&(pDocT->ColumnData[ll]), m_pLib);
            }
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto SaveSigmaDoc_RET;
        }
        iret = (int_t) fwrite(pmask, sizeof(byte_t), pDocT->ColumnData[jj].dim, fp);
        if (iret != pDocT->ColumnData[jj].dim) {
            for (ll = jj; ll < iDataCount; ll++) {
                m_pLib->errcode = 0;
                ml_vector_delete(&(pDocT->ColumnData[ll]), m_pLib);
            }
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto SaveSigmaDoc_RET;
        }
        m_pLib->errcode = 0;
        ml_vector_delete(&(pDocT->ColumnData[jj]), m_pLib);
        pdat = NULL;
        pmask = NULL;
    }

    _tcscpy(szDelim, _T("SigmaDoc_FitCol"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    pdat = NULL;
    pmask = NULL;
    if (pDocT->iDataFitCount > 0) {
        for (jj = 0; jj < pDocT->iDataFitCount; jj++) {
            pdat = pDocT->ColumnDataFit[jj].dat;
            pmask = pDocT->ColumnDataFit[jj].mask;
            pDocT->ColumnDataFit[jj].dat = NULL;
            pDocT->ColumnDataFit[jj].mask = NULL;

            iret = (int_t) fwrite(&(pDocT->ColumnDataFit[jj]), sizeof(vector_t), 1, fp);
            if (iret != 1) {
                for (ll = jj; ll < pDocT->iDataFitCount; ll++) {
                    m_pLib->errcode = 0;
                    ml_vector_delete(&(pDocT->ColumnDataFit[ll]), m_pLib);
                }
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto SaveSigmaDoc_RET;
            }
            iret = (int_t) fwrite(pdat, sizeof(real_t), pDocT->ColumnDataFit[jj].dim, fp);
            if (iret != pDocT->ColumnDataFit[jj].dim) {
                for (ll = jj; ll < pDocT->iDataFitCount; ll++) {
                    m_pLib->errcode = 0;
                    ml_vector_delete(&(pDocT->ColumnDataFit[ll]), m_pLib);
                }
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto SaveSigmaDoc_RET;
            }
            iret = (int_t) fwrite(pmask, sizeof(byte_t), pDocT->ColumnDataFit[jj].dim, fp);
            if (iret != pDocT->ColumnDataFit[jj].dim) {
                for (ll = jj; ll < pDocT->iDataFitCount; ll++) {
                    m_pLib->errcode = 0;
                    ml_vector_delete(&(pDocT->ColumnDataFit[ll]), m_pLib);
                }
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto SaveSigmaDoc_RET;
            }
            m_pLib->errcode = 0;
            ml_vector_delete(&(pDocT->ColumnDataFit[jj]), m_pLib);
            pdat = NULL;
            pmask = NULL;
        }
    }
    else {
        int_t iNothing = 0;
        iret = (int_t) fwrite(&iNothing, sizeof(int_t), 1, fp);
        if (iret != 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto SaveSigmaDoc_RET;
        }
    }

    _tcscpy(szDelim, _T("SigmaDoc_StatsC"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    int ifun;
    bool bPeak, bPeakM;
    int iPeakM = 0;

    for (jj = 0; jj < pDocT->iDataCount; jj++) {
        iret = (int_t) fwrite(&(pDocT->Stats[jj]), sizeof(stat_t), 1, fp);
        if (iret != 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto SaveSigmaDoc_RET;
        }
        if (jj < (pDocT->iDataCount >> 1)) {
            ifun = pDocT->Fit[jj].index - 1;
            bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
                || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
                || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
                || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
            bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);
            if ((bPeakM == true) || (pDocT->Fit[jj].parcount > ML_FIT_MAXPAR_LEGACY)) {
                ++iPeakM;
            }
        }
    }

    _tcscpy(szDelim, _T("SigmaDoc_FitRep"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    if (pDocT->iDataFitCount > 1) {
        fit_legacy_t tFit;
        for (jj = 0; jj < (pDocT->iDataCount >> 1); jj++) {
            if (iPeakM <= 0) {
                ml_fit_tolegacy(&tFit, (const fit_t*) (&pDocT->Fit[jj]));
                iret = (int_t) fwrite(&tFit, sizeof(fit_legacy_t), 1, fp);
            }
            else {
                iret = (int_t) fwrite(&(pDocT->Fit[jj]), sizeof(fit_t), 1, fp);
            }
            if (iret != 1) {
                Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                goto SaveSigmaDoc_RET;
            }
        }
    }
    else {
        int_t iNothing = 0;
        iret = (int_t) fwrite(&iNothing, sizeof(int_t), 1, fp);
        if (iret != 1) {
            Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            goto SaveSigmaDoc_RET;
        }
    }

    _tcscpy(szDelim, _T("SigmaDoc_PlotId"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    iret = (int_t) fwrite(iX, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }
    iret = (int_t) fwrite(iY, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }
    iret = (int_t) fwrite(iFitX, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }
    iret = (int_t) fwrite(iFitY, sizeof(int_t), ML_MAXCURVES, fp);
    if (iret != ML_MAXCURVES) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    _tcscpy(szDelim, _T("SigmaDoc_PLOT"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    pl_validate(pPlotT, 1);
    iret = (int_t) fwrite(pPlotT, sizeof(plot_v24_t), 1, fp);
    if (iret != 1) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    _tcscpy(szDelim, _T("SigmaDoc_END"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }

    _tcscpy(szDelim, _T("SigmaDoc_OPTW"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        Output((LPCTSTR) szMessage, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        goto SaveSigmaDoc_RET;
    }
    iret = (int_t) fwrite(pPlacementPlot, sizeof(WINDOWPLACEMENT), 1, fp);
    iret = (int_t) fwrite(pPlacementDatasheet, sizeof(WINDOWPLACEMENT), 1, fp);
    iret = (int_t) fwrite(pPlacementDatasheetFit, sizeof(WINDOWPLACEMENT), 1, fp);

    if (bExport) {
        m_pLib->errcode = 0;
        char_t* pszHeader = (char_t*) malloc(ML_STRSIZEW * ML_STRSIZEN * sizeof(char_t));
        if (pszHeader) {
            memset(pszHeader, 0, ML_STRSIZEW * ML_STRSIZEN * sizeof(char_t));
            const int iHeaderLen = (int) _tcslen((const char_t*) (pDoc->szNote));
            if (iHeaderLen > 1) {
                CString strNote = CString((const char_t*) (pDoc->szNote));
                if (strNote.Find(_T("#")) < 0) {
                    strNote = CString(_T("# ")) + strNote;
                    strNote.Replace(_T("\n"), _T("\n# "));
                }
                _tcscpy(pszHeader, strNote.GetBuffer());
                _tcscat(pszHeader, _T("\r\n"));
                if (pDoc->szNote[iHeaderLen - 1] != _T('\n')) {
                    _tcscat(pszHeader, _T("\r\n"));
                }
            }
            fl_export_asciiv((const char_t*) szFilenameData, (vector_t *) (pDoc->ColumnData), pDoc->iDataCount, m_Options.import.cSep, m_pLib, pszHeader);

            if (pDoc->iDataFitCount > 1) {
                CString strReport = _T("");
                fit_t *pFit = NULL;
                for (jj = 0; jj < (pDoc->iDataCount >> 1); jj++) {
                    strReport += CSigmaApp::getFitReport((fit_t *) (&(pDoc->Fit[jj])));
                }
                const int iLen = strReport.GetLength();
                if (iLen > 1) {
                    _tcsncat(pszHeader, strReport.GetBuffer(), ML_STRSIZEW * ML_STRSIZES);
                    _tcscat(pszHeader, _T("\r\n"));
                    if (strReport[iLen - 1] != _T('\n')) {
                        _tcscat(pszHeader, _T("\r\n"));
                    }
                }
                fl_export_asciiv((const char_t*) szFilenameFit, (vector_t *) (pDoc->ColumnDataFit), pDoc->iDataFitCount, m_Options.import.cSep, m_pLib, pszHeader);
            }
            else {
                _wunlink((const char_t*) szFilenameFit);
            }

            free(pszHeader);
            pszHeader = NULL;

            if (pPlot->curvecount >= 1) {
                _tcscpy((char_t*) (pPlot->imagefilename), (const char_t*) (szFilenamePlot));
                ((plot_t *) (pPlot))->imagetype = 2;
                ::SetForegroundWindow(pPlot->frame);
                ::SendMessage(pPlot->frame, WM_PAINT, 0, 0);
            }
        }
    }

    bRet = TRUE;

SaveSigmaDoc_RET:
    if (pDocT) {
        delete pDocT; pDocT = NULL;
    }
    if (pPlotT) {
        delete pPlotT; pPlotT = NULL;
    }
    fclose(fp); fp = NULL;

    return bRet;
}


BOOL CSigmaApp::AccessSigmaTemplate(FILE* fp, template_t *pTemplate, int_t iOp)
{
    BOOL bRet = FALSE;

    if ((pTemplate == NULL) || (fp == NULL)) {
        return FALSE;
    }

    char_t szDelim[SIGMA_SSIZE];
    int_t iret = 0;

    if (iOp == 0) { iret = (int_t)fread(&(pTemplate->Software), sizeof(software_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pTemplate->Software), sizeof(software_t), 1, fp); }
    if (iret != 1) {
        goto AccessSigmaTemplate_RET;
    }

    // Vérifier l'entête (en cas de lecture)
    if (iOp == 0) {
        if (_tcsicmp((const char_t*)(pTemplate->Software.name), SIGMA_SOFTWARE_NAME) != 0) {
            goto AccessSigmaTemplate_RET;
        }
        if (_tcsicmp((const char_t*)(pTemplate->Software.description), SIGMA_SOFTWARE_DESCRIPTION) != 0) {
            goto AccessSigmaTemplate_RET;
        }
        if ((pTemplate->Software.version & 0xF000) > (SIGMA_SOFTWARE_DOCVERSION & 0xF000)) {
            goto AccessSigmaTemplate_RET;
        }
    }

    if (iOp == 0) { iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp); }
    else {
        _tcscpy(szDelim, _T("SigmaTpl_SID"));
        iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    }
    if (iret != SIGMA_SSIZE) {
        goto AccessSigmaTemplate_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if (_tcsicmp((const char_t*)szDelim, _T("SigmaTpl_SID")) != 0) {
            goto AccessSigmaTemplate_RET;
        }
    }

    if (iOp == 0) { iret = (int_t)fread(&(pTemplate->tmCreated), sizeof(date_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pTemplate->tmCreated), sizeof(date_t), 1, fp); }
    if (iret != 1) {
        goto AccessSigmaTemplate_RET;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pTemplate->tmModified), sizeof(date_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pTemplate->tmModified), sizeof(date_t), 1, fp); }
    if (iret != 1) {
        goto AccessSigmaTemplate_RET;
    }

    if (iOp == 0) { iret = (int_t)fread(&(pTemplate->iCount), sizeof(int_t), 1, fp); }
    else { iret = (int_t) fwrite(&(pTemplate->iCount), sizeof(int_t), 1, fp); }
    if (iret != 1) {
        goto AccessSigmaTemplate_RET;
    }

    // Vérifier (en cas de lecture)
    if (iOp == 0) {
        if ((pTemplate->iCount < 1) || (pTemplate->iCount > SIGMA_MAXTPL)) {
            goto AccessSigmaTemplate_RET;
        }
    }

    bRet = TRUE;

AccessSigmaTemplate_RET:
    return bRet;
}

BOOL CSigmaApp::OpenSigmaTemplate(LPCTSTR lpszFileName, template_t *pTemplate, plot_t **ppPlot)
{
    BOOL bRet = FALSE;

    if ((lpszFileName == NULL) || (pTemplate == NULL) || (ppPlot == NULL)) {
        return FALSE;
    }
    if (*ppPlot == NULL) {
        return FALSE;
    }

    // Vérifier la taille du fichier avant lecture
    long_t iFileLength = fl_filesize((const char_t*)lpszFileName);
    long_t iMinLength = (long_t)(sizeof(template_t) + sizeof(plot_v24_t));
    long_t iMaxLength = (long_t)(sizeof(template_t) + (SIGMA_MAXTPL * sizeof(plot_v24_t)));
    if ((iFileLength < iMinLength) || (iFileLength > iMaxLength)) {
        return FALSE;
    }

    FILE *fp = NULL;

    if ((fp = _wfopen((const char_t*)lpszFileName, _T("rb"))) == NULL) {
        return FALSE;
    }

    char_t szDelim[SIGMA_SSIZE];
    int_t iret = 0;

    int_t jj;

    // Lire les données statiques
    if (AccessSigmaTemplate(fp, pTemplate, 0) == FALSE) {
        goto OpenSigmaTemplate_RET;
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        goto OpenSigmaTemplate_RET;
    }
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaTpl_PLOT")) != 0) {
        goto OpenSigmaTemplate_RET;
    }

    // Lire le graphe
    for (jj = 0; jj < pTemplate->iCount; jj++) {
        iret = (int_t)fread(ppPlot[jj], sizeof(plot_v24_t), 1, fp);
        if (iret != 1) {
            goto OpenSigmaTemplate_RET;
        }
        pl_validate(ppPlot[jj], 1);
        if (ppPlot[jj]->errcode != 0) {    // Error
            memset(ppPlot[jj], 0, sizeof(plot_v24_t));
            pl_reset(ppPlot[jj], 2);
            goto OpenSigmaTemplate_RET;
        }
    }

    iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        goto OpenSigmaTemplate_RET;
    }
    if (_tcsicmp((const char_t*)szDelim, _T("SigmaTpl_END")) != 0) {
        goto OpenSigmaTemplate_RET;
    }

    bRet = TRUE;

OpenSigmaTemplate_RET:
    fclose(fp); fp = NULL;
    return bRet;
}

BOOL CSigmaApp::SaveSigmaTemplate(LPCTSTR lpszFileName, template_t *pTemplate, const plot_t **ppPlot)
{
    BOOL bRet = FALSE;

    if ((lpszFileName == NULL) || (pTemplate == NULL) || (ppPlot == NULL)) {
        return FALSE;
    }
    if (*ppPlot == NULL) {
        return FALSE;
    }
    if ((pTemplate->iCount < 1) || (pTemplate->iCount > SIGMA_MAXTPL)) {
        return FALSE;
    }

    plot_t *pPlotT = new(std::nothrow) plot_t;
    ASSERT(pPlotT);
    if (pPlotT == NULL) {
        return FALSE;
    }
    pl_reset(pPlotT, 1);

    FILE *fp = NULL;

    if ((fp = _wfopen((const char_t*)lpszFileName, _T("wb"))) == NULL) {
        if (pPlotT) {
            delete pPlotT; pPlotT = NULL;
        }
        return FALSE;
    }

    char_t szDelim[SIGMA_SSIZE];
    int_t jj, iret = 0;

    if (AccessSigmaTemplate(fp, pTemplate, 1) == FALSE) {
        goto SaveSigmaTemplate_RET;
    }

    _tcscpy(szDelim, _T("SigmaTpl_PLOT"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        goto SaveSigmaTemplate_RET;
    }

    // Ecrire les graphes
    for (jj = 0; jj < pTemplate->iCount; jj++) {
        pPlotT->errcode = 0;
        pl_plot_dupstatic(pPlotT, ppPlot[jj]);
        if (pPlotT->errcode != 0) {
            if (pPlotT) {
                delete pPlotT; pPlotT = NULL;
            }
            return FALSE;
        }

        pl_validate(pPlotT, 1);
        iret = (int_t) fwrite(pPlotT, sizeof(plot_v24_t), 1, fp);
        if (iret != 1) {
            goto SaveSigmaTemplate_RET;
        }
    }

    _tcscpy(szDelim, _T("SigmaTpl_END"));
    iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
    if (iret != SIGMA_SSIZE) {
        goto SaveSigmaTemplate_RET;
    }

    bRet = TRUE;

SaveSigmaTemplate_RET:
    if (pPlotT) {
        delete pPlotT; pPlotT = NULL;
    }
    fclose(fp); fp = NULL;

    return bRet;
}

int CSigmaApp::ExitInstance()
{
    if (m_pPlotCopy != NULL) {
        delete m_pPlotCopy;
        m_pPlotCopy = NULL;
    }

    setOptions();

    if (m_pLib != NULL) {
        delete m_pLib;
        m_pLib = NULL;
    }
    if (m_pDefaultPlot) {
        delete m_pDefaultPlot;
        m_pDefaultPlot = NULL;
    }

    // Fitting
    int_t jj;
    m_iUserFitCount = 0;
    for (jj = 0; jj < ML_FIT_MAXPAR; jj++) {
        if (m_pUserFit[jj] != NULL) {
            free(m_pUserFit[jj]);
            m_pUserFit[jj] = NULL;
        }
    }

    Gdiplus::GdiplusShutdown(m_GdiplusToken);

    return CWinApp::ExitInstance();
}

void CSigmaApp::OnWindowDup()
{
    if (m_nDocCount >= SIGMA_MAXDOC) {
        Output(_T("Cannot duplicate document.\r\nMaximum number of documents (16) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pMainFrame != NULL);
    if (pMainFrame == NULL) {
        return;
    }

    CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
    ASSERT(pMDIActive != NULL);
    if (pMDIActive == NULL) {
        return;
    }

    CSigmaDoc* pDocumentSource = (CSigmaDoc*)(pMDIActive->GetActiveDocument());
    ASSERT(pDocumentSource != NULL);
    if (pDocumentSource == NULL) {
        return;
    }

    sigmadoc_t *pDocSource = pDocumentSource->GetDoc();

    if ((pDocSource == NULL) || (pDocumentSource->GetColumnCount(0) < 1)) {
        return;
    }

    CDocTemplate* pDocTemplate = (CDocTemplate*)m_pTemplateData;
    ASSERT_VALID(pDocTemplate);
    if (pDocTemplate == NULL) {
        Output(_T("Cannot duplicate document.\r\nInvalid document template.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CDocument* pDocument = pDocTemplate->CreateNewDocument();
    ASSERT_VALID(pDocument);
    if (pDocument == NULL) {
        Output(_T("Cannot duplicate document.\r\nInvalid document resource.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    CSigmaDoc* pSigmaDocument = (CSigmaDoc*)pDocument;

    BOOL bAutoDelete = pDocument->m_bAutoDelete;
    pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
    CFrameWnd* pFrame = pDocTemplate->CreateNewFrame(pDocument, NULL);
    pDocument->m_bAutoDelete = bAutoDelete;
    if (pFrame == NULL) {
        Output(_T("Cannot duplicate document.\r\nInvalid frame.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        delete pDocument;       // explicit delete on error
        return;
    }
    ASSERT_VALID(pFrame);

    if (!pDocument->OnNewDocument()) {
        // user has be alerted to what failed in OnNewDocument
        pFrame->DestroyWindow();
        return;
    }

    sigmadoc_t *pDocT = pSigmaDocument->GetDoc();
    if (pDocT == NULL) {
        Output(_T("Cannot duplicate document: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        pFrame->DestroyWindow();
        return;
    }
    memset(pDocT, 0, sizeof(sigmadoc_t));
    if (CopyDocument(pDocT, pDocSource) == FALSE) {
        Output(_T("Cannot duplicate document: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        pFrame->DestroyWindow();
        return;
    }

    // >> [MOD-SG240-002]: AutoX
    if (pSigmaDocument->m_pColumnAutoX->dat) {
        m_pLib->errcode = 0;
        ml_vector_delete(pSigmaDocument->m_pColumnAutoX, m_pLib);
    }
    ml_vector_create(pSigmaDocument->m_pColumnAutoX, pDocumentSource->m_pColumnAutoX->dim,
        pDocumentSource->m_pColumnAutoX->id, (const char_t*)(pDocumentSource->m_pColumnAutoX->name), NULL, m_pLib);
    ml_vector_dup(pSigmaDocument->m_pColumnAutoX, (const vector_t*)(pDocumentSource->m_pColumnAutoX), m_pLib);
    // <<

    pSigmaDocument->SetModifiedFlag(TRUE);

    pDocTemplate->InitialUpdateFrame(pFrame, pDocument, TRUE);
}

void CSigmaApp::OnWindowCopyFormat()
{
    if (m_pPlotCopy == NULL) {
        return;
    }

    CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pMainFrame != NULL);
    if (pMainFrame == NULL) {
        return;
    }

    CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
    ASSERT(pMDIActive != NULL);
    if (pMDIActive == NULL) {
        return;
    }

    CSigmaDoc* pDocumentSource = (CSigmaDoc*)(pMDIActive->GetActiveDocument());
    ASSERT(pDocumentSource != NULL);
    if (pDocumentSource == NULL) {
        return;
    }

    plot_t *pPlotSource = pDocumentSource->GetPlot();
    if (pPlotSource == NULL) {
        return;
    }

    m_pPlotCopy->errcode = 0;
    pl_plot_dupstatic(m_pPlotCopy, pPlotSource);
    if (m_pPlotCopy->errcode != 0) {
        Output(_T("Cannot copy plot format: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    m_bCanPastePlot = TRUE;
}

void CSigmaApp::OnUpdateWindowCopyFormat(CCmdUI *pCmdUI)
{
    pCmdUI->Enable((m_pPlotCopy != NULL));
}

void CSigmaApp::OnWindowPasteFormat()
{
    if (m_pPlotCopy == NULL) {
        return;
    }

    CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pMainFrame != NULL);
    if (pMainFrame == NULL) {
        return;
    }

    CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
    ASSERT(pMDIActive != NULL);
    if (pMDIActive == NULL) {
        return;
    }

    CSigmaDoc* pDocumentDest = (CSigmaDoc*)(pMDIActive->GetActiveDocument());
    ASSERT(pDocumentDest != NULL);
    if (pDocumentDest == NULL) {
        return;
    }

    plot_t *pPlotDest = pDocumentDest->GetPlot();
    if (pPlotDest == NULL) {
        return;
    }

    pDocumentDest->SaveState();

    CopyPlot(pPlotDest, m_pPlotCopy);

    pDocumentDest->SetModifiedFlag(TRUE);
    pDocumentDest->UpdateAllViews(NULL);
}

void CSigmaApp::OnUpdateWindowPasteFormat(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_bCanPastePlot);
}

void CSigmaApp::OnWindowCloseAll()
{
    // Sauvegarder le cas échéant les documents modifiés
    // parcourir tous les templates
    CDocTemplate* pTemplate;
    POSITION pos = SigmaApp.GetFirstDocTemplatePosition();
    CView *pView = NULL;
    while (pos != NULL) {
        pTemplate = SigmaApp.GetNextDocTemplate(pos);
        ASSERT(pTemplate);
        // tous les documents du template.
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD) {
            CSigmaDoc* pDoc = (CSigmaDoc*)(pTemplate->GetNextDoc(posD));
            if (pDoc != NULL) {
                if (pDoc->IsModified()) {
                    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
                    ASSERT(pFrame != NULL);
                    if (FALSE == pDoc->ActivateView(1)) {
                        pDoc->ActivateView(0);
                    }
                    CString strT;
                    CString strTX = pDoc->GetTitle();
                    strTX = strTX.Trim(_T(" *"));
                    strT.Format(_T("Save '%s' before closing?"), strTX);
                    int_t iT = pFrame->MessageBox(strT, _T("SigmaGraph"), MB_YESNOCANCEL | MB_ICONQUESTION);
                    if (iT == IDYES) {
                        // Save document
                        pDoc->SaveSigmaDoc();
                    }
                    else if (iT == IDCANCEL) {
                        return;
                    }
                    pView = pDoc->GetDataView();
                    if (pView) {
                        ((CSigmaViewData*)pView)->GetDatasheet()->EndEdit();
                    }
                    // Eviter que CMDIFrameWnd::OnClose() ne tente de sauvegarder le document (et l'écraser!)
                    pDoc->SetModifiedFlag(FALSE);
                }
                pDoc->OnCloseDocument();
            }
        }
    }
}

void CSigmaApp::OnHelpIndex()
{
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) (m_szHelpFilename), HH_DISPLAY_INDEX, NULL);
}

void CSigmaApp::OnHelpContents()
{
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) (m_szHelpFilename), HH_DISPLAY_TOC, NULL);
}

void CSigmaApp::OnHelpContentsPDF()
{
    ::ShellExecute(NULL, _T("open"), (LPCTSTR) (m_szHelpFilenamePDF), NULL, NULL, SW_SHOW);
}

void CSigmaApp::WinHelp(DWORD dwData, UINT nCmd)
{
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) (m_szHelpFilename), nCmd, dwData);
}

void CSigmaApp::OnHelpContext()
{
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) (m_szHelpFilename), HH_DISPLAY_INDEX, NULL);
}

void CSigmaApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
    int_t ilen = (int_t)_tcslen((const char_t*)lpszPathName);
    if ((ilen < 7) || (ilen >= (ML_STRSIZE >> 1))) {
        return;
    }

    if (((lpszPathName[ilen - 4] != _T('.')) || (lpszPathName[ilen - 3] != _T('s'))
        || (lpszPathName[ilen - 2] != _T('i')) || (lpszPathName[ilen - 1] != _T('d')))
        && ((lpszPathName[ilen - 4] != _T('.')) || (lpszPathName[ilen - 3] != _T('l'))
        || (lpszPathName[ilen - 2] != _T('u')) || (lpszPathName[ilen - 1] != _T('a')))) {
        return;
    }

    int_t iFound = 0;
    if (m_pRecentFileList != NULL) {
        int_t nSize = m_pRecentFileList->GetSize();
        if (nSize > 8) {
            m_pRecentFileList->m_arrNames->Empty();
            m_pRecentFileList->m_nSize = 0;
        }
        if (nSize > 0) {
            int_t jj;
            for (jj = 0; jj < nSize; jj++) {
                if ((*(SigmaApp.m_pRecentFileList))[jj] == lpszPathName) {
                    iFound = 1;
                    break;
                }
            }
        }
    }
    if (iFound) {
        return;
    }

    CWinApp::AddToRecentFileList(lpszPathName);
}

BOOL CSigmaApp::GetWindowPlacement(const char_t *pszFilenameShort, WINDOWPLACEMENT *pPlacementMain,
    int iDefWidth/* = SIGMA_DEFWIDTH*/, int iDefHeight/* = SIGMA_DEFHEIGHT*/,
    int iNextToMain/* = -1*/)
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    char_t szLoc[ML_STRSIZE];

    if (pPlacementMain == NULL) {
        return FALSE;
    }

    pPlacementMain->length = sizeof(WINDOWPLACEMENT);
    pPlacementMain->showCmd = SW_RESTORE;

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return FALSE;
    }

    LONG iX0 = 0L, iY0 = 0L, iW = 0L, iH = 0L;

    RECT desktopRect;
    const HWND hDesktop = ::GetDesktopWindow();
    ::GetWindowRect(hDesktop, &desktopRect);
    iX0 = desktopRect.left;
    iW = desktopRect.right - desktopRect.left;
    iY0 = desktopRect.top;
    iH = desktopRect.bottom - desktopRect.top;

    RECT defRect;
    if (iNextToMain >= 0) {
        LONG iX0m = 0L, iY0m = 0L, iWm = 0L, iHm = 0L;
        WINDOWPLACEMENT mainPlace;
        memset(&mainPlace, 0, sizeof(WINDOWPLACEMENT));
        mainPlace.length = sizeof(WINDOWPLACEMENT);
        pFrame->GetWindowPlacement(&mainPlace);
        iX0m = 0L;
        iY0m = 0L;
        iWm = mainPlace.rcNormalPosition.right - mainPlace.rcNormalPosition.left;
        iHm = mainPlace.rcNormalPosition.bottom - mainPlace.rcNormalPosition.top;
        if (iDefWidth > (iWm - 100)) {
            iDefWidth = iWm - 100;
        }
        if (iDefHeight > (iHm - 100)) {
            iDefHeight = iHm - 100;
        }
        defRect.left = mainPlace.rcNormalPosition.right - iNextToMain - iDefWidth;
        defRect.right = mainPlace.rcNormalPosition.right - iNextToMain;
        defRect.top = mainPlace.rcNormalPosition.bottom - iNextToMain - iDefHeight;
        defRect.bottom = mainPlace.rcNormalPosition.bottom - iNextToMain;
    }
    else {
        if (iDefWidth > (iW - 100)) {
            iDefWidth = iW - 100;
        }
        if (iDefHeight > (iH - 100)) {
            iDefHeight = iH - 100;
        }
        defRect.left = iX0 + 100;
        defRect.right = iX0 + 100 + iDefWidth;
        defRect.top = iY0 + 100;
        defRect.bottom = iY0 + 100 + iDefHeight;
    }
    pPlacementMain->rcNormalPosition = defRect;

    _tcscpy(szLoc, (const char_t*) m_szUserDir);
    _tcscat(szLoc, pszFilenameShort);

    ilen = (long_t) fl_filesize((const char_t*) szLoc);
    // Le fichier ne correspond pas à une donnée valide
    if (ilen != (long_t)sizeof(WINDOWPLACEMENT)) {
        return FALSE;
    }

    if ((fp = _wfopen((const char_t*) szLoc, _T("rb"))) == NULL) {
        return FALSE;
    }

    itemlen = (int_t)sizeof(WINDOWPLACEMENT);
    pitem = (char*) malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    ilen = (long_t) fread(pitem, itemlen, 1, fp);
    if (ilen != 1) {
        free(pitem); pitem = NULL;
        fclose(fp); fp = NULL;
        return FALSE;
    }

    memcpy(pPlacementMain, pitem, itemlen);
    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    pPlacementMain->showCmd = SW_RESTORE;

    bool bFlag = ((pPlacementMain->rcNormalPosition.left >= iX0) && (pPlacementMain->rcNormalPosition.top >= iY0)
        && (pPlacementMain->rcNormalPosition.right <= (iX0 + iW)) && (pPlacementMain->rcNormalPosition.bottom <= (iY0 + iH))
        && (pPlacementMain->rcNormalPosition.right >= (pPlacementMain->rcNormalPosition.left + SIGMA_MINWIDTH3))
        && (pPlacementMain->rcNormalPosition.bottom >= (pPlacementMain->rcNormalPosition.top + SIGMA_MINHEIGHT3)));

    if (bFlag == false) {
        pPlacementMain->rcNormalPosition = defRect;
    }

    return TRUE;
}

BOOL CSigmaApp::SetWindowPlacement(const char_t *pszFilenameShort, WINDOWPLACEMENT *pPlacementMain)
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    char_t szLoc[ML_STRSIZE];

    if (pPlacementMain == NULL) {
        return FALSE;
    }

    pPlacementMain->length = sizeof(WINDOWPLACEMENT);

    _tcscpy(szLoc, (const char_t*) m_szUserDir);
    _tcscat(szLoc, pszFilenameShort);

    if ((fp = _wfopen((const char_t*) szLoc, _T("wb"))) == NULL) {
        return FALSE;
    }

    itemlen = (long_t)sizeof(WINDOWPLACEMENT);
    pitem = (char*) malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    memcpy(pitem, pPlacementMain, itemlen);
    fwrite(pitem, itemlen, 1, fp);

    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    return TRUE;
}

