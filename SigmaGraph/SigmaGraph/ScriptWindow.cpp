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
#include "InputTextDlg.h"
#include "InputRealDlg.h"

#include "ScriptWindow.h"
#include "FavoritesDlg.h"

#include "SigmaDoc.h"

#include "ScriptColorsDlg.h"

#include <afxpriv.h>

// CScriptWindow dialog

IMPLEMENT_DYNAMIC(CScriptWindow, CDialog)

CScriptWindow::CScriptWindow(CWnd* pParent /*=NULL*/)
    : CDialog(CScriptWindow::IDD, pParent)
{
    m_edtScript = NULL;
    m_edtOutput = NULL;

    m_bInitialized = FALSE;
    SetModified(FALSE);

    memset(m_szFilename, 0, ML_STRSIZE * sizeof(char_t));

    m_pFavorite = NULL;
    m_pFavorite = (favorite_t*) malloc(SIGMA_SCRIPT_MAXFAV * sizeof(favorite_t));
    if (m_pFavorite) {
        memset(m_pFavorite, 0, SIGMA_SCRIPT_MAXFAV * sizeof(favorite_t));
    }
    m_iFavorite = -1;
    m_iFavoriteCount = 0;

    g_pDoc = NULL;

    m_bRunning = FALSE;
    m_pScriptEngine = NULL;
    m_pszBufferA = NULL;
    g_bStopit = FALSE;

    m_bFirstSearch = TRUE;    // Is this the first search
}

CScriptWindow::~CScriptWindow()
{
    if (m_edtScript) {
        m_edtScript->DestroyWindow();
        delete m_edtScript;
        m_edtScript = NULL;
    }

    if (m_edtOutput) {
        m_edtOutput->DestroyWindow();
        delete m_edtOutput;
        m_edtOutput = NULL;
    }

    if (m_pFavorite) {
        free(m_pFavorite);
        m_pFavorite = NULL;
    }
    m_iFavorite = -1;
    m_iFavoriteCount = 0;

    ScriptClose();
}

void CScriptWindow::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

static UINT indicators[] =
{
    ID_INDICATOR_LINE,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM
};

const UINT CScriptWindow::SCINTILLA_MSG_FINDREPLACE = RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(CScriptWindow, CDialog)
    ON_COMMAND(ID_SCRIPT_CLOSE, &CScriptWindow::OnScriptClose)
    ON_COMMAND(ID_SCRIPT_NEW, &CScriptWindow::OnScriptNew)
    ON_COMMAND(ID_SCRIPT_FONT, &CScriptWindow::OnScriptFont)
    ON_COMMAND(ID_SCRIPT_SAVEAS, &CScriptWindow::OnScriptSaveAs)
    ON_COMMAND(ID_SCRIPT_SAVE, &CScriptWindow::OnScriptSave)
    ON_COMMAND(ID_SCRIPT_OPEN, &CScriptWindow::OnScriptOpen)
    ON_COMMAND(ID_SCRIPT_RELOAD, &CScriptWindow::OnScriptRead)
    ON_COMMAND(ID_SCRIPT_RUN, &CScriptWindow::OnScriptRun)
    ON_COMMAND(ID_EDIT_SELECTALL, &CScriptWindow::OnEditSelectAll)
    ON_COMMAND(ID_EDIT_UNDO, &CScriptWindow::OnEditUndo)
    ON_COMMAND(ID_EDIT_REDO, &CScriptWindow::OnEditRedo)
    ON_COMMAND(ID_EDIT_COPY, &CScriptWindow::OnEditCopy)
    ON_COMMAND(ID_EDIT_CUT, &CScriptWindow::OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, &CScriptWindow::OnEditPaste)
    ON_COMMAND(ID_EDIT_FIND, &CScriptWindow::OnEditFind)
    ON_COMMAND(ID_EDIT_REPLACE, &CScriptWindow::OnEditReplace)
    ON_COMMAND(ID_EDIT_GOTO, &CScriptWindow::OnEditGoto)
    ON_COMMAND(ID_EDIT_REPEAT, &CScriptWindow::OnEditRepeat)
    ON_COMMAND(ID_SCRIPT_COLORS, &CScriptWindow::OnOptionsColors)
    ON_COMMAND(ID_SCRIPT_ADDMARKER, &CScriptWindow::OnOptionsAddmarker)
    ON_COMMAND(ID_SCRIPT_NEXTMARKER, &CScriptWindow::OnOptionsFindNextmarker)
    ON_COMMAND(ID_SCRIPT_PREVMARKER, &CScriptWindow::OnOptionsFindPrevmarker)
    ON_COMMAND(ID_SCRIPT_LINENUMBERS, &CScriptWindow::OnOptionsViewLinenumbers)
    ON_COMMAND(ID_SCRIPT_AUTOCOMPLETE, &CScriptWindow::OnOptionsAutocomplete)
    ON_COMMAND(ID_SCRIPT_DELETEALLMARKERS, &CScriptWindow::OnOptionsDeleteAllMarkers)
    ON_COMMAND(ID_SCRIPT_LINESPACING_100, &CScriptWindow::OnOptionsLineSpacing100)
    ON_COMMAND(ID_SCRIPT_LINESPACING_125, &CScriptWindow::OnOptionsLineSpacing125)
    ON_COMMAND(ID_SCRIPT_LINESPACING_150, &CScriptWindow::OnOptionsLineSpacing150)
    ON_COMMAND(ID_SCRIPT_LINESPACING_175, &CScriptWindow::OnOptionsLineSpacing175)
    ON_COMMAND(ID_SCRIPT_OUTPUTSIZE_SMALL, &CScriptWindow::OnOutputSizeSmall)
    ON_COMMAND(ID_SCRIPT_OUTPUTSIZE_MEDIUM, &CScriptWindow::OnOutputSizeMedium)
    ON_COMMAND(ID_SCRIPT_OUTPUTSIZE_LARGE, &CScriptWindow::OnOutputSizeLarge)
    ON_COMMAND(ID_SCRIPT_OUTPUT_VIEW, &CScriptWindow::OnOutputView)
    ON_COMMAND(ID_SCRIPT_ZOOMIN, &CScriptWindow::OnViewZoomIn)
    ON_COMMAND(ID_SCRIPT_ZOOMOUT, &CScriptWindow::OnViewZoomOut)
    ON_COMMAND(ID_SCRIPT_ZOOMNONE, &CScriptWindow::OnViewZoomNone)
    ON_COMMAND(ID_SCRIPT_CLEAROUTPUT, &CScriptWindow::OnClearOutput)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_LINENUMBERS, &CScriptWindow::OnUpdateOptionsViewLinenumbers)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_AUTOCOMPLETE, &CScriptWindow::OnUpdateOptionsAutocomplete)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_LINESPACING_100, &CScriptWindow::OnUpdateOptionsLineSpacing100)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_LINESPACING_125, &CScriptWindow::OnUpdateOptionsLineSpacing125)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_LINESPACING_150, &CScriptWindow::OnUpdateOptionsLineSpacing150)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_LINESPACING_175, &CScriptWindow::OnUpdateOptionsLineSpacing175)
    ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &CScriptWindow::OnUpdateEditFind)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, &CScriptWindow::OnUpdateEditReplace)
    ON_UPDATE_COMMAND_UI(ID_EDIT_GOTO, &CScriptWindow::OnUpdateEditGoto)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, &CScriptWindow::OnUpdateEditSelectAll)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CScriptWindow::OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CScriptWindow::OnUpdateEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CScriptWindow::OnUpdateEditPaste)
    ON_REGISTERED_MESSAGE(SCINTILLA_MSG_FINDREPLACE, &CScriptWindow::OnFindReplaceCmd)
    ON_COMMAND(ID_HELP_CONTENTS, &CScriptWindow::OnHelpContents)
    ON_COMMAND(ID_SCRIPT_FAVORITE, &CScriptWindow::OnScriptFavorite)
    ON_COMMAND(ID_FAVORITE_ONE, &CScriptWindow::OnFavoriteOne)
    ON_COMMAND(ID_FAVORITE_TWO, &CScriptWindow::OnFavoriteTwo)
    ON_COMMAND(ID_FAVORITE_THREE, &CScriptWindow::OnFavoriteThree)
    ON_COMMAND(ID_FAVORITE_FOUR, &CScriptWindow::OnFavoriteFour)
    ON_COMMAND(ID_SCRIPT_SAMPLE01, &CScriptWindow::OnSampleOne)
    ON_COMMAND(ID_SCRIPT_SAMPLE02, &CScriptWindow::OnSampleTwo)
    ON_COMMAND(ID_SCRIPT_SAMPLE03, &CScriptWindow::OnSampleThree)
    ON_COMMAND(ID_SCRIPT_SAMPLE04, &CScriptWindow::OnSampleFour)
    ON_COMMAND(ID_SCRIPT_RESET, &CScriptWindow::OnScriptReset)
    ON_COMMAND(ID_SCRIPT_ADDFAV, &CScriptWindow::OnScriptAddFavorite)
    ON_UPDATE_COMMAND_UI(ID_SCRIPT_ADDFAV, &CScriptWindow::OnUpdateScriptAddFavorite)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
    ON_WM_SYSCOMMAND()
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
    ON_WM_INITMENUPOPUP()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CScriptWindow::ScriptInit()
{
    if (m_pScriptEngine == NULL) {
        m_pScriptEngine = luaL_newstate();            // Open Lua
        if (m_pScriptEngine == NULL) {
            return FALSE;
        }

        initScriptEngine(m_pScriptEngine);

        // >> AutoRun Scripts
        if ((m_iFavoriteCount > 0) && (m_iFavoriteCount <= SIGMA_SCRIPT_MAXFAV)) {
            int_t jj;
            for (jj = 0; jj < m_iFavoriteCount; jj++) {
                if ((m_pFavorite[jj].autorun & 0xF0) == 0x10) {
                    RunScript((LPCTSTR)(m_pFavorite[jj].path));
                }
            }
        }
        // <<
    }

    return TRUE;
}

void CScriptWindow::ScriptClose()
{
    if (m_edtScript && ::IsWindow(m_edtScript->GetSafeHwnd())) {
        if (m_edtScript->GetTextLength() < 2) {
            SetModified(FALSE);
            m_edtScript->EmptyUndoBuffer();
        }

        if (m_bIsModified) {
            int_t iT = this->MessageBox(_T("Save script before closing ?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION);
            if (iT == IDYES) {
                OnScriptSave();
            }

            SetModified(FALSE);
            m_edtScript->EmptyUndoBuffer();
        }
    }

    if (m_pScriptEngine) {
        /* cleanup Lua */
        lua_close(m_pScriptEngine);
        m_pScriptEngine = NULL;
    }

    if (m_pszBufferA) {
        free(m_pszBufferA);
        m_pszBufferA = NULL;
    }
}

void CScriptWindow::UpdateFavoriteMenu()
{
    UINT itemID;
    int_t jj;
    char_t szTT[ML_STRSIZES];
    memset(szTT, 0, ML_STRSIZES * sizeof(char_t));

    CMenu* pMenu = GetMenu();
    if (pMenu) {
        CMenu* pFavMenu = pMenu->GetSubMenu(FAVORITES_MENU_INDEX);
        if (pFavMenu != NULL) {
            if (m_iFavoriteCount >= 1) {
                for (jj = 0; jj < m_iFavoriteCount; jj++) {
                    itemID = pFavMenu->GetMenuItemID(jj);
                    pFavMenu->ModifyMenu(itemID, MF_BYCOMMAND, itemID, (const char_t*) (m_pFavorite[jj].name));
                }
                if (m_iFavoriteCount < 4) {
                    for (jj = m_iFavoriteCount; jj < 4; jj++) {
                        itemID = pFavMenu->GetMenuItemID(jj);
                        _tcsprintf(szTT, ML_STRSIZES - 1, _T("Script #%d"), jj + 1);
                        pFavMenu->ModifyMenu(itemID, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED, itemID, (const char_t*)szTT);
                    }
                }
            }
            else {
                for (jj = 0; jj < 4; jj++) {
                    itemID = pFavMenu->GetMenuItemID(jj);
                    _tcsprintf(szTT, ML_STRSIZES - 1, _T("Script #%d"), jj + 1);
                    pFavMenu->ModifyMenu(itemID, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED, itemID, (const char_t*)szTT);
                }
            }
        }
    }
}

// CScriptWindow message handlers
BOOL CScriptWindow::OnInitDialog()
{
    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_SCRIPT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    CRect rcT;
    GetClientRect(rcT);

    m_edtScript = new CScriptEdit();
    m_edtScript->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, rcT, this, IDE_SCRIPT);

    m_ScintillaPrefs.outputsize = 20;
    m_ScintillaPrefs.outputshow = true;

    m_ScintillaPrefs.common.syntaxEnable = true;
    m_ScintillaPrefs.common.foldEnable = false;
    m_ScintillaPrefs.common.indentEnable = true;
    m_ScintillaPrefs.common.overTypeInitial = false;
    m_ScintillaPrefs.common.readOnlyInitial = false;
    m_ScintillaPrefs.common.wrapModeInitial = false;
    m_ScintillaPrefs.common.displayEOLEnable = false;
    m_ScintillaPrefs.common.indentGuideEnable = false;
    m_ScintillaPrefs.common.lineNumberEnable = true;
    m_ScintillaPrefs.common.longLineOnEnable = false;
    m_ScintillaPrefs.common.whiteSpaceEnable = false;
    m_ScintillaPrefs.common.autocompleteEnable = false;
    m_ScintillaPrefs.common.lineSpacing = 1;

    for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
        _tcscpy(m_ScintillaPrefs.style[ii].name, CScriptEdit::STYLEPREFS[ii].name);
        m_ScintillaPrefs.style[ii].foreground = CScriptEdit::STYLEPREFS[ii].foreground;
        m_ScintillaPrefs.style[ii].background = CScriptEdit::STYLEPREFS[ii].background;
        _tcscpy(m_ScintillaPrefs.style[ii].fontname, SigmaApp.isWinXP() ? _T("Courier New") : CScriptEdit::STYLEPREFS[ii].fontname);
        m_ScintillaPrefs.style[ii].fontsize = CScriptEdit::STYLEPREFS[ii].fontsize;
        m_ScintillaPrefs.style[ii].fontstyle = CScriptEdit::STYLEPREFS[ii].fontstyle;
        m_ScintillaPrefs.style[ii].lettercase = CScriptEdit::STYLEPREFS[ii].lettercase;
    }

    char_t szCommonPrefs[ML_STRSIZE];
    memset(szCommonPrefs, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szCommonPrefs, (const char_t*) (SigmaApp.m_szUserDir));
    _tcscat(szCommonPrefs, _T("EditorPrefs.par"));
    long_t ilen = (long_t) fl_filesize((const char_t*) szCommonPrefs);
    if (ilen == sizeof(ScintillaPrefs)) {
        FILE *fp = NULL;
        if ((fp = _wfopen((const char_t*) szCommonPrefs, _T("rb"))) != NULL) {
            fread(&m_ScintillaPrefs, sizeof(ScintillaPrefs), 1, fp);
            fclose(fp);    fp = NULL;
            if (m_ScintillaPrefs.outputsize < 15) {
                m_ScintillaPrefs.outputsize = 15;
            }
            else if (m_ScintillaPrefs.outputsize > 50) {
                m_ScintillaPrefs.outputsize = 50;
            }
        }
    }

    m_edtScript->initPrefs(m_ScintillaPrefs);

    int_t nTabStops = 12;
    m_edtScript->SetSel(-1,-1);

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    m_bRunning = FALSE;

    memset(m_szFilename, 0, ML_STRSIZE * sizeof(char_t));

    // >> Favorites permanent
    int_t iAutoLoad = -1;
    m_iFavoriteCount = 0;
    m_iFavorite = 0;
    int_t iret = 0, jj;
    software_t softT;
    memset(&softT, 0, sizeof(software_t));
    char_t szLoc[ML_STRSIZE], szDelim[SIGMA_SSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    memset(szDelim, 0, SIGMA_SSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*) (SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("ScriptFavorites.par"));

    ilen = (long_t) fl_filesize((const char_t*) szLoc);
    if ((ilen > 1) && (ilen <= (sizeof(favorite_t) * SIGMA_SCRIPT_MAXFAV))) {
        FILE *fp = NULL;
        if ((fp = _wfopen((const char_t*)szLoc, _T("rb"))) != NULL) {
            int_t nLength, mm;
            m_iFavoriteCount = 0;

            // Lire l'entête
            iret = (int_t)fread(&softT, sizeof(software_t), 1, fp);
            if (iret != 1) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }

            // Vérifier l'entête
            if (_tcsicmp((const char_t*) (softT.name), SIGMA_SOFTWARE_NAME) != 0) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            if (_tcsicmp((const char_t*) (softT.description), SIGMA_SOFTWARE_DESCRIPTION) != 0) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            if ((softT.version & 0xF000) > (SIGMA_SOFTWARE_DOCVERSION & 0xF000)) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }

            iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
            if (iret != SIGMA_SSIZE) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_CNT")) != 0) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            iret = (int_t)fread(&m_iFavoriteCount, sizeof(int_t), 1, fp);
            if (iret != 1) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            if ((m_iFavoriteCount < 1) || (m_iFavoriteCount > SIGMA_SCRIPT_MAXFAV)) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
            if (iret != SIGMA_SSIZE) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_CNT")) != 0) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            iret = (int_t)fread(&m_iFavorite, sizeof(int_t), 1, fp);
            if (iret != 1) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            if ((m_iFavorite < 0) || (m_iFavorite >= m_iFavoriteCount)) {
                m_iFavorite = 0;
            }
            iret = (int_t)fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
            if (iret != SIGMA_SSIZE) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            if (_tcsicmp((const char_t*)szDelim, _T("SigmaDoc_FAV")) != 0) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }
            iret = (int_t)fread(m_pFavorite, sizeof(favorite_t), m_iFavoriteCount, fp);
            if (iret != m_iFavoriteCount) {
                fclose(fp);    fp = NULL;
                UpdateData(FALSE);
                return TRUE;
            }

            fclose(fp);    fp = NULL;

            iret = 0;
            FILE *fpr = NULL;
            for (jj = 0; jj < m_iFavoriteCount; jj++) {
                fpr = _wfopen((LPCTSTR)(m_pFavorite[jj].path), _T("r"));
                if (fpr != NULL) {
                    fclose(fpr);    fpr = NULL;
                    fl_getshort(m_pFavorite[jj].name, m_pFavorite[jj].path);
                    nLength = (int_t) _tcslen((const char_t*) (m_pFavorite[jj].name));
                    if ((nLength > 4) && (nLength < (ML_STRSIZE - 3))) {
                        for (mm = nLength - 1; mm > 0; mm--) {
                            if (m_pFavorite[jj].name[mm] == _T('.')) {
                                m_pFavorite[jj].name[mm] = _T('\0');
                                break;
                            }
                        }
                        iret += 1;
                        if ((iAutoLoad < 0) && ((m_pFavorite[jj].autorun & 0x0F) == 0x01)) {
                            iAutoLoad = jj;
                        }
                    }
                }
            }
            if (iret < m_iFavoriteCount) {
                for (jj = iret; jj < m_iFavoriteCount; jj++) {
                    memset(&(m_pFavorite[jj]), 0, sizeof(favorite_t));
                }
                m_iFavoriteCount = iret;
            }
            if (m_iFavorite >= m_iFavoriteCount) {
                m_iFavorite = 0;
            }
            if ((iAutoLoad >= 0) && (iAutoLoad < m_iFavoriteCount)) {
                ScriptFileOpen((LPCTSTR)(m_pFavorite[iAutoLoad].path));
            }
        }
    }
    UpdateFavoriteMenu();
    // <<

    CRect rcClientOld;
    CRect rcClientNew;
    GetClientRect(rcClientOld);

    if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
        | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED, CRect(0, 0, 0, 0), AFX_IDW_TOOLBAR + 29)) {
        ScriptClose();
        SigmaApp.Output(_T("Error initializing Script Window.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(1);
        return FALSE;
    }
    m_wndToolbar.LoadToolBar(IDR_TOOLBAR_SCRIPT);
    m_wndToolbar.LoadTrueColorToolBar(16, IDB_TOOLBAR_SCRIPT, IDB_TOOLBAR_SCRIPT, NULL);

    m_wndToolbar.ShowWindow(SW_SHOW);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNew);
    m_ptOffsetTop.x = rcClientNew.left - rcClientOld.left;
    m_ptOffsetTop.y = rcClientNew.top - rcClientOld.top;
    CRect rcChild;
    CWnd* pwndChild = GetWindow(GW_CHILD);
    while (pwndChild) {
        pwndChild->GetWindowRect(rcChild);
        ScreenToClient(rcChild);
        rcChild.top += m_ptOffsetTop.y;
        pwndChild->MoveWindow(rcChild,FALSE);
        pwndChild = pwndChild->GetNextWindow();
    }

    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.right += rcClientOld.Width() - rcClientNew.Width(); 
    rcWindow.bottom += rcClientOld.Height() - rcClientNew.Height(); 
    MoveWindow(rcWindow, FALSE);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

    if (ScriptInit() == FALSE) {
        if (m_pScriptEngine) {
            /* cleanup Lua */
            lua_close(m_pScriptEngine);
            m_pScriptEngine = NULL;
        }
        SigmaApp.Output(_T("Error initializing script engine.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(1);
        return FALSE;
    }

    int iW = m_edtScript->TextWidth(SCINTILLA_STYLE_DEFAULT, _T("for counter = 1, 10 do print (counter) end"));
    m_edtScript->setMaxLineWidth(iW);
    m_edtScript->SetScrollWidth(m_edtScript->getMaxLineWidth());

    m_bInitialized = TRUE;
    OutputView(m_ScintillaPrefs.outputshow);

    UpdateData(FALSE);
    SetModified(FALSE);
    return TRUE;
}

void CScriptWindow::OnClose()
{
    ShowWindow(SW_HIDE);
}

void CScriptWindow::PostNcDestroy()
{
    char_t szCommonPrefs[ML_STRSIZE];
    memset(szCommonPrefs, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szCommonPrefs, (const char_t*) (SigmaApp.m_szUserDir));
    _tcscat(szCommonPrefs, _T("EditorPrefs.par"));
    FILE *fp = NULL;
    if ((fp = _wfopen((const char_t*) szCommonPrefs, _T("wb"))) != NULL) {
        fwrite(&m_ScintillaPrefs, sizeof(ScintillaPrefs), 1, fp);
        fclose(fp);    fp = NULL;
    }

    char_t szLoc[ML_STRSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*) (SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("ScriptFavorites.par"));
    if ((m_iFavoriteCount > 0) && (m_iFavoriteCount <= SIGMA_SCRIPT_MAXFAV)) {
        int_t iret;
        char_t szDelim[SIGMA_SSIZE];
        memset(szDelim, 0, SIGMA_SSIZE * sizeof(char_t));
        fp = NULL;
        if ((fp = _wfopen((const char_t*)szLoc, _T("wb"))) != NULL) {
            // Ecrire l'ent�te
            iret = (int_t) fwrite(&(g_Sigmaware), sizeof(software_t), 1, fp);
            if (iret != 1) {
                fclose(fp);    fp = NULL;
                CDialog::PostNcDestroy();
                return;
            }
            _tcscpy(szDelim, _T("SigmaDoc_CNT"));
            iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
            if (iret != SIGMA_SSIZE) {
                fclose(fp);    fp = NULL;
                CDialog::PostNcDestroy();
                return;
            }
            iret = (int_t) fwrite(&m_iFavoriteCount, sizeof(int_t), 1, fp);
            if (iret != 1) {
                fclose(fp);    fp = NULL;
                CDialog::PostNcDestroy();
                return;
            }
            iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
            if (iret != SIGMA_SSIZE) {
                fclose(fp);    fp = NULL;
                CDialog::PostNcDestroy();
                return;
            }
            iret = (int_t) fwrite(&m_iFavorite, sizeof(int_t), 1, fp);
            if (iret != 1) {
                fclose(fp);    fp = NULL;
                CDialog::PostNcDestroy();
                return;
            }
            _tcscpy(szDelim, _T("SigmaDoc_FAV"));
            iret = (int_t) fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
            if (iret != SIGMA_SSIZE) {
                fclose(fp);    fp = NULL;
                CDialog::PostNcDestroy();
                return;
            }
            iret = (int_t) fwrite(m_pFavorite, sizeof(favorite_t), m_iFavoriteCount, fp);
            if (iret != m_iFavoriteCount) {
                fclose(fp);    fp = NULL;
                CDialog::PostNcDestroy();
                return;
            }
            fclose(fp);    fp = NULL;
        }
    }
    else {
        ::DeleteFile((LPCTSTR)szLoc);
    }

    CDialog::PostNcDestroy();
}

void CScriptWindow::OnScriptClose()
{
    OnClose();
}

void CScriptWindow::OnOK()
{

}

void CScriptWindow::OnCancel()
{
    OnClose();
}

void CScriptWindow::OnUpdateEditReplace(CCmdUI* pCmdUI)
{
    int_t iLen = m_edtScript->GetTextLength();
    pCmdUI->Enable((iLen >= 7) && (iLen < SCRIPT_MAXCHARS));
}

void CScriptWindow::OnEditGoto()
{
    int_t iLen = m_edtScript->GetTextLength();
    if ((iLen < 7) || (iLen > SCRIPT_MAXCHARS)) {
        return;
    }

    real_t fT = 1.0;

    CInputRealDlg *pDlgInput = new CInputRealDlg(this);
    if (pDlgInput == NULL) {
        return;
    }

    pDlgInput->m_strTitle = _T("Script Goto");
    pDlgInput->m_pfInput1 = &fT;
    pDlgInput->m_strLabel1 = _T("Go to line:");
    pDlgInput->m_pfInput2 = NULL;
    pDlgInput->m_iLimit = ML_STRSIZET - 1;
    pDlgInput->DoModal();
    if (pDlgInput->m_bOK) {
        int_t iLine = ((int_t) fT) - 1;
        if ((iLine >= 0) && (iLine < SCRIPT_MAXLINES)) {
            m_edtScript->GotoLine(iLine);
        }
    }

    delete pDlgInput; pDlgInput = NULL;
}

void CScriptWindow::OnUpdateEditGoto(CCmdUI* pCmdUI)
{
    int_t iLen = m_edtScript->GetTextLength();
    pCmdUI->Enable((iLen >= 7) && (iLen < SCRIPT_MAXCHARS));
}

void CScriptWindow::OnScriptNew()
{
    if (m_bIsModified) {
        int_t iT = this->MessageBox(_T("Save script before closing?"), _T("SigmaGraph"), MB_YESNOCANCEL | MB_ICONQUESTION);
        if (iT == IDCANCEL) {
            return;
        }
        else if (iT == IDYES) {
            OnScriptSave();
        }
    }

    m_iMarkerCount = 0;

    SetWindowText(_T("SigmaGraph Script"));
    m_edtScript->SetText(_T(""));
    int iW = m_edtScript->TextWidth(SCINTILLA_STYLE_DEFAULT, _T("for counter = 1, 10 do print (counter) end"));
    m_edtScript->setMaxLineWidth(iW);
    m_edtScript->SetScrollWidth(m_edtScript->getMaxLineWidth());
    memset(m_szFilename, 0, ML_STRSIZE * sizeof(char_t));

    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_NEXTMARKER, FALSE);
    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_PREVMARKER, FALSE);
    
    SetModified(FALSE);
    m_edtScript->EmptyUndoBuffer();
    m_edtScript->SetSel(-1, -1);
}

void CScriptWindow::OnScriptFont()
{
    LOGFONT lft;
    memset(&lft, 0, sizeof(lft));
    SigmaApp.FontToLogfont(SigmaApp.m_Options.script.font, &lft);

    if (SigmaApp.GetFont(m_hWnd, &lft)) {
        SigmaApp.LogfontToFont(lft, &(SigmaApp.m_Options.script.font));

        for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
            _tcscpy(m_ScintillaPrefs.style[ii].fontname, (const char_t*) (SigmaApp.m_Options.script.font.name));
            m_ScintillaPrefs.style[ii].fontsize = SigmaApp.m_Options.script.font.size;
            if (lft.lfWeight == FW_BOLD) {
                m_ScintillaPrefs.style[ii].fontstyle = SCRIPT_STYLE_BOLD;
            }
            else {
                m_ScintillaPrefs.style[ii].fontstyle = SCRIPT_STYLE_NORMAL;
            }
        }
        m_ScintillaPrefs.style[STYLEINFO_INDEX_LINENUM].fontsize = SigmaApp.m_Options.script.font.size;

        m_edtScript->initPrefs(m_ScintillaPrefs);

        if (m_edtOutput) {
            m_edtOutput->SetFontParameters(m_ScintillaPrefs.style[STYLEINFO_INDEX_DEFAULT].fontname, m_ScintillaPrefs.style[STYLEINFO_INDEX_DEFAULT].fontsize);
        }
    }
}

void CScriptWindow::ScriptFileSave(LPCTSTR pszFilename)
{
    int_t iLen = m_edtScript->GetTextLength();
    if ((iLen < 2) || (iLen > SCRIPT_MAXCHARS)) {
        return;
    }

    int_t nLines = m_edtScript->GetLineCount();
    if ((nLines < 1) || (nLines > SCRIPT_MAXLINES)) {
        SigmaApp.Output(_T("Cannot save script: invalid size.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
        return;
    }

    CString strT;

    FILE *fpScript = NULL;

    try {

        if ((fpScript = _wfopen((const char_t*) pszFilename, _T("w"))) == NULL) {
            SigmaApp.Output(_T("Cannot save script: create file failed\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return;
        }

        int_t ii = 0, nLength = 0;

        char_t szFilenameT[ML_STRSIZE];
        memset(szFilenameT, 0, ML_STRSIZE * sizeof(char_t));
        _tcscpy(szFilenameT, pszFilename);
        int iLen = _tcslen((const char_t*) szFilenameT);
        bool bSavePar = false;
        if ((iLen > 3) && (iLen < (ML_STRSIZE - 4))) {
            for (ii = iLen - 1; ii >= 0; ii--) {
                if (szFilenameT[ii] == _T('.')) {
                    szFilenameT[ii + 1] = _T('p');
                    szFilenameT[ii + 2] = _T('a');
                    szFilenameT[ii + 3] = _T('r');
                    szFilenameT[ii + 4] = _T('\0');
                    bSavePar = true;
                    break;
                }
            }
        }

        byte_t *pbMarker = NULL;
        if (bSavePar) {
            pbMarker = (byte_t*) malloc(nLines * sizeof(byte_t));
            if (pbMarker == NULL) {
                bSavePar = false;
            }
        }

        char_t szBuffer[ML_STRSIZE];
        int_t iCount = 0;
        for (ii = 0; ii < nLines; ii++) {
            strT = m_edtScript->GetLine(ii);
            nLength = strT.GetLength();
            if ((nLength > 0) && (nLength <= (ML_STRSIZE - 1))) {
                nLength = SigmaApp.CStringToChar(strT, (char_t*) szBuffer, nLength, TRUE);
                if (nLength == 0) {
                    fputws(_T("\n"), fpScript);
                    iCount += 1;
                }
                else if (nLength > 0) {
                    fputws(szBuffer, fpScript);
                    fputws(_T("\n"), fpScript);
                    iCount += 1;
                }
                if (pbMarker) {
                    pbMarker[ii] = (m_edtScript->MarkerGet(ii) & 0x1) ? 1 : 0;
                }
            }
        }
        fclose(fpScript); fpScript = NULL;

        FILE *fp = NULL;
        if ((fp = _wfopen((const char_t*) szFilenameT, _T("wb"))) != NULL) {

            char_t szDelim[SIGMA_SSIZE];
            memset(szDelim, 0, SIGMA_SSIZE * sizeof(char_t));
            fwrite(&g_Sigmaware, sizeof(software_t), 1, fp);

            if (pbMarker) {
                _tcscpy(szDelim, _T("SigmaScript_MRK"));
                fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
                fwrite(&iCount, sizeof(int_t), 1, fp);
                fwrite(pbMarker, sizeof(byte_t), iCount, fp);
            }

            _tcscpy(szDelim, _T("SigmaScript_CUR"));
            fwrite(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
            long_t lPos = m_edtScript->GetCurrentPos();
            fwrite(&lPos, sizeof(long_t), 1, fp);

            fclose(fp);    fp = NULL;
        }

        if (pbMarker) {
            free(pbMarker);
            pbMarker = NULL;
        }

        strT.Format(_T("Script saved: %d lines.\r\n"), iCount);
        SigmaApp.Output(strT, TRUE);

        _tcscpy(m_szFilename, (LPCTSTR) pszFilename);

        SetModified(FALSE);
        m_edtScript->EmptyUndoBuffer();

        int_t mm;
        fl_getshort(szBuffer, m_szFilename);
        nLength = (int_t) _tcslen((const char_t*)szBuffer);
        if ((nLength > 4) && (nLength < (ML_STRSIZE - 3))) {
            for (mm = nLength - 1; mm > 0; mm--) {
                if (szBuffer[mm] == _T('.')) {
                    szBuffer[mm] = _T('\0');
                    break;
                }
            }
            strT = _T("SigmaGraph Script - ");
            strT.Append((LPCTSTR)szBuffer);
            SetWindowText((LPCTSTR)strT);
        }

        SigmaApp.OutputSB(_T("Script: saved."), 0, TRUE);
        return;
    }
    catch (...) {
        if (fpScript) {
            fclose(fpScript); fpScript = NULL;
        }
        SigmaApp.Output(_T("Cannot save script.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
        return;
    }
}

void CScriptWindow::OnScriptSaveAs()
{
    int_t nLines = m_edtScript->GetLineCount();
    if ((nLines < 1) || (nLines > SCRIPT_MAXLINES)) {
        SigmaApp.Output(_T("Cannot save script: invalid size.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
    }

    char_t szFilters[] = _T("Script Files (*.lua)|*.lua||");

    CFileDialog *pFileDlg = new CFileDialog(FALSE, _T("lua"), static_cast<const char_t *>(m_szFilename), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
    if (pFileDlg->DoModal() != IDOK) {
        m_edtScript->SetFocus();
        m_edtScript->SetSel(-1, -1);
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString fileName = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    ScriptFileSave((LPCTSTR) fileName);
    m_edtScript->SetSel(-1, -1);
}

void CScriptWindow::OnScriptSave()
{
    if (m_bIsModified == FALSE) {
        return;
    }

    int_t iLen = m_edtScript->GetTextLength();
    if ((iLen < 2) || (iLen > SCRIPT_MAXCHARS)) {
        return;
    }

    CStdioFile fileT;
    CFileException excT;
    if (!fileT.Open((LPCTSTR) m_szFilename, CFile::modeRead | CFile::typeText | CFile::shareExclusive, &excT)) {
        OnScriptSaveAs();
        return;
    }
    fileT.Close();

    ScriptFileSave((LPCTSTR) m_szFilename);
}

BOOL CScriptWindow::ScriptFileOpen(LPCTSTR pszFilename, bool bStraight/* = false*/)
{
    CString strT;
    char_t szBuffer[ML_STRSIZE];
    memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
    int_t nLength, ii = 0, mm;

    m_iMarkerCount = 0;

    FILE *fpScript = NULL;

    char_t *pszBufferW = NULL;
    try {

        long_t lPos = -1L;

        long_t nFileLen = (int_t) (ScriptFileSize((const char_t*) pszFilename));
        if ((nFileLen < 5L) || (nFileLen > SCRIPT_MAXCHARS)) {
            SigmaApp.Output(_T("Cannot read script: invalid size.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
            return FALSE;
        }
        
        if ((fpScript = _wfopen((const char_t*) pszFilename, _T("r"))) == NULL) {
            SigmaApp.Output(_T("Cannot open script file\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
            return FALSE;
        }

        // Nead Confirmation
        if (bStraight == false) {
            OnScriptNew();
            if (m_bIsModified) {
                fclose(fpScript); fpScript = NULL;
                return FALSE;
            }
        }

        char_t *pszT = NULL;

        pszBufferW = (char_t*)malloc(SCRIPT_MAXCHARS * sizeof(char_t));
        if (pszBufferW == NULL) {
            fclose(fpScript); fpScript = NULL;
            SigmaApp.Output(_T("Cannot run script: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return FALSE;
        }
        memset(pszBufferW, 0, SCRIPT_MAXCHARS * sizeof(char_t));

        int maxLineIndex = 0, nLengthMax = 0;

        for (;;) {
            
            if ((nLength = ScriptFileGetLine(fpScript, szBuffer, ML_STRSIZE - 1)) < 0) {
                break;
            }
            if (nLength > 0) {
                _tcscat(pszBufferW, (const char_t*) szBuffer);
            }
            _tcscat(pszBufferW, _T("\r\n"));
            if (nLength > nLengthMax) {
                nLengthMax = nLength;
                maxLineIndex = ii;
            }
            ii += 1;
        }
        fclose(fpScript); fpScript = NULL;

        int iCount = ii;

        m_edtScript->calcLinenumberMargin(iCount);
        m_edtScript->SetText((const char_t*) pszBufferW);

        CString strT = m_edtScript->GetLine(maxLineIndex);
        strT += _T("___");
        int iW = m_edtScript->TextWidth(SCINTILLA_STYLE_DEFAULT, strT);
        m_edtScript->setMaxLineWidth(iW);
        m_edtScript->SetScrollWidth(m_edtScript->getMaxLineWidth());

        free(pszBufferW);    pszBufferW = NULL;

        strT.Format(_T("Script read: %d lines.\r\n"), iCount);
        SigmaApp.Output(strT, TRUE);

        _tcscpy(m_szFilename, (LPCTSTR)pszFilename);

        fl_getshort(szBuffer, m_szFilename);
        nLength = (int_t) _tcslen((const char_t*)szBuffer);
        if ((nLength > 4) && (nLength < (ML_STRSIZE - 3))) {
            for (mm = nLength - 1; mm > 0; mm--) {
                if (szBuffer[mm] == _T('.')) {
                    szBuffer[mm] = _T('\0');
                    break;
                }
            }
            strT = _T("SigmaGraph Script - ");
            strT.Append((LPCTSTR)szBuffer);
            SetWindowText((LPCTSTR)strT);
        }

        char_t szFilenameT[ML_STRSIZE];
        memset(szFilenameT, 0, ML_STRSIZE * sizeof(char_t));
        _tcscpy(szFilenameT, pszFilename);
        int iLen = (int) _tcslen((const char_t*) szFilenameT);
        bool bGetPar = false;
        if ((iCount >= 1) && (iLen > 3) && (iLen < (ML_STRSIZE - 4))) {
            for (ii = iLen - 1; ii >= 0; ii--) {
                if (szFilenameT[ii] == _T('.')) {
                    szFilenameT[ii + 1] = _T('p');
                    szFilenameT[ii + 2] = _T('a');
                    szFilenameT[ii + 3] = _T('r');
                    szFilenameT[ii + 4] = _T('\0');
                    bGetPar = true;
                    break;
                }
            }
        }

        byte_t *pbMarker = NULL;
        if (bGetPar) {
            pbMarker = (byte_t*) malloc(iCount * sizeof(byte_t));
        }

        if (bGetPar && pbMarker) {

            FILE *fp = NULL;
            if ((fp = _wfopen((const char_t*) szFilenameT, _T("rb"))) != NULL) {
                software_t softT;
                fread(&softT, sizeof(software_t), 1, fp);

                if ((_tcsicmp((const char_t*) (softT.name), SIGMA_SOFTWARE_NAME) == 0)
                    && (_tcsicmp((const char_t*) (softT.description), SIGMA_SOFTWARE_DESCRIPTION) == 0)) {

                    bool bFLag = false;

                    char_t szDelim[SIGMA_SSIZE];
                    memset(szDelim, 0, SIGMA_SSIZE * sizeof(char_t));
                    fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
                    if ((_tcsicmp((const char_t*) szDelim, _T("SigmaScript_MRK")) == 0) && pbMarker) {
                        int_t ict = 0;
                        fread(&ict, sizeof(int_t), 1, fp);
                        if (ict == iCount) {
                            int_t iret = (int_t) fread(pbMarker, sizeof(byte_t), iCount, fp);
                            if (iret == iCount) {
                                for (ii = 0; ii < iCount; ii++) {
                                    if (pbMarker[ii] == 1) {
                                        m_edtScript->MarkerAdd(ii, 0);
                                        m_iMarkerCount += 1;
                                    }
                                }
                            }
                        }
                        bFLag = true;
                    }

                    if (bFLag == true) {
                        fread(szDelim, sizeof(char_t), SIGMA_SSIZE, fp);
                        bFLag = false;
                    }

                    if (_tcsicmp((const char_t*) szDelim, _T("SigmaScript_CUR")) == 0) {
                        
                        fread(&lPos, sizeof(long_t), 1, fp);
                        if ((lPos < 0L) || (lPos >= m_edtScript->GetLength())) {
                            lPos = -1L;
                        }
                        bFLag = true;
                    }
                }

                fclose(fp);    fp = NULL;
            }

            if (pbMarker) {
                free(pbMarker);
                pbMarker = NULL;
            }
        }

        m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_NEXTMARKER, m_iMarkerCount > 0);
        m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_PREVMARKER, m_iMarkerCount > 0);

        SetModified(FALSE);
        m_edtScript->EmptyUndoBuffer();
        m_edtScript->SetSel(lPos, lPos);
        m_edtScript->SetFocus();

        return TRUE;
    }
    catch (...) {
        if (pszBufferW) {
            free(pszBufferW);    pszBufferW = NULL;
        }
        if (fpScript) {
            fclose(fpScript);    fpScript = NULL;
        }
        SigmaApp.Output(_T("Cannot read script file.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
        return FALSE;
    }
}

void CScriptWindow::OnScriptRead()
{
    CStdioFile fileT;
    CFileException excT;
    if (!fileT.Open((LPCTSTR)m_szFilename, CFile::modeRead | CFile::typeText | CFile::shareExclusive, &excT)) {
        OnScriptOpen();
        return;
    }
    fileT.Close();

    char_t szFilename[ML_STRSIZE];
    memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szFilename, (const char_t*) m_szFilename);

    ScriptFileOpen((LPCTSTR) szFilename);
}

void CScriptWindow::OnScriptOpen()
{
    // Nead Confirmation
    OnScriptNew();
    if (m_bIsModified) {
        m_edtScript->SetFocus();
        m_edtScript->SetSel(-1, -1);
        return;
    }

    char_t szFilters[] = _T("Script Files (*.lua)|*.lua||");

    CFileDialog *pFileDlg = new CFileDialog(FALSE, _T("lua"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, szFilters, this);
    if (pFileDlg->DoModal() != IDOK) {
        m_edtScript->SetFocus();
        m_edtScript->SetSel(-1, -1);
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString strFilename = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    ScriptFileOpen((LPCTSTR) strFilename, true);
}

void CScriptWindow::Relayout(void)
{
    if (m_bInitialized) {
        CRect recTT;
        GetClientRect(recTT);
        recTT.top += m_ptOffsetTop.y;

        CRect recEdit = recTT;
        CRect recOutput = recTT;

        LONG iHeightEdit;
        
        if (m_ScintillaPrefs.outputshow) {
            iHeightEdit = ((100 - m_ScintillaPrefs.outputsize) * recTT.Height()) / 100;
            LONG iHeightOutput = recTT.Height() - iHeightEdit;
            recEdit.bottom -= iHeightOutput;
        }
        else {
            iHeightEdit = recTT.Height();
        }

        m_edtScript->MoveWindow(recEdit);
        m_edtScript->SetSel(-1, -1);

        if (m_edtScript && m_ScintillaPrefs.outputshow) {
            recOutput.top += iHeightEdit;
            m_edtOutput->MoveWindow(recOutput);
        }
    }
}

void CScriptWindow::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    Relayout();
}

void CScriptWindow::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH2;
    lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT2;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CScriptWindow::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/scripting.html"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CScriptWindow::PreTranslateMessage(MSG* pMsg)
{
    BOOL bRet = FALSE;

    // [FIX-SG200-004] All keys work now (included Suppr). Only Ctrl+S and F12 are trapped by accelerators.
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if (CTRL_Pressed()) {
                if ((pMsg->wParam == _T('n')) || (pMsg->wParam == _T('N'))) {
                    OnScriptNew();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('s')) || (pMsg->wParam == _T('S'))) {
                    OnScriptSave();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('o')) || (pMsg->wParam == _T('O'))) {
                    OnScriptOpen();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('r')) || (pMsg->wParam == _T('R'))) {
                    char_t szFilename[ML_STRSIZE];
                    memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
                    _tcscpy(szFilename, (const char_t*) m_szFilename);
                    ScriptFileOpen((LPCTSTR) szFilename);
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('f')) || (pMsg->wParam == _T('F'))) {
                    OnEditFind();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('h')) || (pMsg->wParam == _T('H'))) {
                    OnEditReplace();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('g')) || (pMsg->wParam == _T('G'))) {
                    OnEditGoto();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('z')) || (pMsg->wParam == _T('Z'))) {
                    OnEditUndo();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('y')) || (pMsg->wParam == _T('Y'))) {
                    OnEditRedo();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('c')) || (pMsg->wParam == _T('C'))) {
                    OnEditCopy();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('v')) || (pMsg->wParam == _T('V'))) {
                    OnEditPaste();
                    return TRUE;
                }
                else if ((pMsg->wParam == _T('a')) || (pMsg->wParam == _T('A'))) {
                    if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                        return TRUE;
                    }
                }
                else if (pMsg->wParam == _T('+')) {
                    OnViewZoomIn();
                    return TRUE;
                }
                else if (pMsg->wParam == _T('-')) {
                    OnViewZoomIn();
                    return TRUE;
                }
                else if (pMsg->wParam == _T('0')) {
                    OnViewZoomNone();
                    return TRUE;
                }
                else if (pMsg->wParam == VK_F2) {
                    if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                        return TRUE;
                    }
                }
                else if (pMsg->wParam == VK_F12) {
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
            else if ((pMsg->wParam == VK_F2)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
            else if ((pMsg->wParam == VK_F3)) {
                OnEditFind();
                return TRUE;
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
            else if ((pMsg->wParam == VK_F12)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
            else if (ALT_Pressed() && (pMsg->wParam == VK_F12)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
            
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CScriptWindow::RunScript(const char_t *pszFilename/* = NULL*/)
{
    CWaitCursor wcT;

    if (m_bRunning) {
        g_bStopit = TRUE;
        return;
    }

    g_iErrline = -1;

    m_bRunning = TRUE;
    g_bStopit = FALSE;

    UpdateData(TRUE);

    if (m_pScriptEngine == NULL) {
        if (ScriptInit() == FALSE) {
            SigmaApp.Output(_T("Error initializing script engine.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_bRunning = FALSE;
            return;
        }
    }

    int_t ii = 0, nLengthA = 0;

    if (m_pszBufferA) {
        free(m_pszBufferA);
        m_pszBufferA = NULL;
    }
    m_pszBufferA = (char*)malloc(SCRIPT_MAXCHARS * sizeof(char));
    if (m_pszBufferA == NULL) {
        SigmaApp.Output(_T("Cannot run script: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        m_bRunning = FALSE;
        return;
    }

    memset(m_pszBufferA, 0, SCRIPT_MAXCHARS * sizeof(char));

    if (pszFilename == NULL) {
        nLengthA = m_edtScript->GetText(SCRIPT_MAXCHARS - 1, m_pszBufferA);

        if (nLengthA < 1) {
            free(m_pszBufferA); m_pszBufferA = NULL;
            m_bRunning = FALSE;
            return;
        }
        if (nLengthA >(SCRIPT_MAXCHARS - 1)) {
            free(m_pszBufferA); m_pszBufferA = NULL;
            SigmaApp.Output(_T("Cannot run script: invalid size (limit: 32768 characters).\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_bRunning = FALSE;
            return;
        }

        m_pszBufferA[nLengthA] = '\0';
    }
    else {
        int_t nLengthW = 0;
        char_t *pszBufferW = (char_t*)malloc(SCRIPT_MAXCHARS * sizeof(char_t));
        if (pszBufferW == NULL) {
            free(m_pszBufferA); m_pszBufferA = NULL;
            SigmaApp.Output(_T("Cannot run script: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_bRunning = FALSE;
            return;
        }
        pszBufferW[0] = _T('\0');
        CStdioFile fileT;
        if (!fileT.Open(pszFilename, CFile::modeRead | CFile::typeText | CFile::shareExclusive)) {
            free(m_pszBufferA); m_pszBufferA = NULL;
            free(pszBufferW); pszBufferW = NULL;
            m_bRunning = FALSE;
            return;
        }
        nLengthW = (int_t) (fileT.GetLength());
        if ((nLengthW  < 5) || (nLengthW > SCRIPT_MAXCHARS)) {
            fileT.Close();
            free(m_pszBufferA); m_pszBufferA = NULL;
            free(pszBufferW); pszBufferW = NULL;
            m_bRunning = FALSE;
            return;
        }
        int_t ii = 0, nLength;
        char_t *pszT = NULL;
        char_t szBuffer[ML_STRSIZE];
        memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));

        for (;;) {
            if (fileT.ReadString(szBuffer, ML_STRSIZE - 1) == NULL) {
                break;
            }
            ii += 1;
            nLength = (int_t) _tcslen((const char_t*)szBuffer);
            if ((ii > SCRIPT_MAXLINES) || (nLength < 1) || (nLength >= ML_STRSIZE)) {
                break;
            }
            if ((szBuffer[nLength - 1] == _T('\n')) || (szBuffer[nLength - 1] == _T('\r'))) {
                szBuffer[nLength - 1] = _T('\0');
            }
            if ((szBuffer[nLength - 2] == _T('\n')) || (szBuffer[nLength - 2] == _T('\r'))) {
                szBuffer[nLength - 2] = _T('\0');
            }
            _tcscat(pszBufferW, (const char_t*)szBuffer);
            _tcscat(pszBufferW, _T("\r\n"));
        }
        fileT.Close();

        if (nLengthW < 1) {
            free(m_pszBufferA); m_pszBufferA = NULL;
            free(pszBufferW); pszBufferW = NULL;
            m_bRunning = FALSE;
            return;
        }
        if (nLengthW > (SCRIPT_MAXCHARS - 1)) {
            free(m_pszBufferA); m_pszBufferA = NULL;
            free(pszBufferW); pszBufferW = NULL;
            SigmaApp.Output(_T("Cannot run script: invalid size (limit: 32768 characters).\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_bRunning = FALSE;
            return;
        }
        pszBufferW[nLengthW] = _T('\0');
        nLengthA = WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)pszBufferW, -1, (LPSTR)m_pszBufferA, SCRIPT_MAXCHARS, NULL, NULL);
        if ((nLengthA < 2) || (nLengthA > (SCRIPT_MAXCHARS - 1))) {
            free(m_pszBufferA); m_pszBufferA = NULL;
            free(pszBufferW); pszBufferW = NULL;
            SigmaApp.Output(_T("Cannot run script: invalid input.\r\n"), TRUE);
            m_bRunning = FALSE;
            return;
        }
        m_pszBufferA[nLengthA] = _T('\0');

        free(pszBufferW); pszBufferW = NULL;
    }

    try {
        int_t iRet = luaL_dostring(m_pScriptEngine, (const char*) m_pszBufferA);

        if (iRet != 0) {
            cf_error(m_pScriptEngine);
            lua_gc(m_pScriptEngine, LUA_GCCOLLECT, 0);        // Force a complete garbage collection in case of errors

            m_edtScript->highlightLine(g_iErrline);
        }
    }
    catch (...) {
        if (m_pScriptEngine) {
            /* cleanup Lua */
            lua_close(m_pScriptEngine);
            m_pScriptEngine = NULL;
        }
        ScriptInit(); // Re-init Script Engine
        m_bRunning = FALSE;
        g_bStopit = FALSE;
        return;
    }

    free(m_pszBufferA); m_pszBufferA = NULL;

    m_bRunning = FALSE;
    g_bStopit = FALSE;
}

void CScriptWindow::OnScriptRun()
{
    RunScript(NULL);
}

void CScriptWindow::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    int nLen = m_edtScript->GetTextLength();
    int_t iStart = m_edtScript->GetSelectionStart(), iStop = m_edtScript->GetSelectionEnd();
    BOOL bEnable = ((nLen < 1) || ((iStart == 0) && (iStop == nLen))) ? FALSE : TRUE;
    pCmdUI->Enable(bEnable);
}

void CScriptWindow::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    int_t iStart = m_edtScript->GetSelectionStart(), iStop = m_edtScript->GetSelectionEnd();
    pCmdUI->Enable(iStart != iStop);
}

void CScriptWindow::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    int_t iStart = m_edtScript->GetSelectionStart(), iStop = m_edtScript->GetSelectionEnd();
    pCmdUI->Enable(iStart != iStop);
}

void CScriptWindow::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}

void CScriptWindow::SetModified(BOOL bIsModified/* = TRUE*/)
{
    m_bIsModified = bIsModified;
    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_SAVE, m_bIsModified);
}
BOOL CScriptWindow::IsModified(void)
{
    return m_bIsModified;
}

void CScriptWindow::OnScriptFavorite()
{
    CFavoriteDlg *pDlgFavorite = new CFavoriteDlg(this);
    if (pDlgFavorite == NULL) {
        return;
    }

    pDlgFavorite->m_pFavorite = m_pFavorite;
    pDlgFavorite->m_piFavoriteCount = &m_iFavoriteCount;
    pDlgFavorite->m_piFavorite = &m_iFavorite;
    pDlgFavorite->DoModal();

    delete pDlgFavorite; pDlgFavorite = NULL;

    UpdateFavoriteMenu();
}

void CScriptWindow::LoadFavorite(int_t iFavorite)
{
    if (m_edtScript->GetTextLength() < 2) {
        SetModified(FALSE);
        m_edtScript->EmptyUndoBuffer();
    }

    if (m_bIsModified) {
        int_t iT = this->MessageBox(_T("Save script before closing?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION);
        if (iT == IDYES) {
            OnScriptSave();
        }

        SetModified(FALSE);
        m_edtScript->EmptyUndoBuffer();
    }

    iFavorite -= 1;

    if ((m_iFavoriteCount < 1) || (iFavorite < 0) || (iFavorite >= m_iFavoriteCount)) {
        return;
    }

    int_t nLength = (int_t) _tcslen((const char_t*) (m_pFavorite[iFavorite].path));
    if ((nLength < 7) || (nLength >= ML_STRSIZE)) {
        return;
    }

    ScriptFileOpen((LPCTSTR)(m_pFavorite[iFavorite].path));

    UpdateFavoriteMenu();
    m_iFavorite = iFavorite;
}

void CScriptWindow::OnFavoriteOne()
{
    if (m_iFavoriteCount < 1) {
        return;
    }

    LoadFavorite(1);
}

void CScriptWindow::OnFavoriteTwo()
{
    if (m_iFavoriteCount < 2) {
        return;
    }

    LoadFavorite(2);
}

void CScriptWindow::OnFavoriteThree()
{
    if (m_iFavoriteCount < 2) {
        return;
    }

    LoadFavorite(3);
}

void CScriptWindow::OnFavoriteFour()
{
    if (m_iFavoriteCount < 4) {
        return;
    }

    LoadFavorite(4);
}

void CScriptWindow::LoadSample(int indexT)
{
    if ((indexT < 0) || (indexT >= SCRIPT_SAMPLES_COUNT)) {
        return;
    }

    // Nead Confirmation
    OnScriptNew();
    if (m_bIsModified) {
        m_edtScript->SetFocus();
        m_edtScript->SetSel(-1, -1);
        return;
    }

    m_edtScript->calcLinenumberMargin(SCRIPT_SAMPLES[indexT].lines);
    m_edtScript->SetText(SCRIPT_SAMPLES[indexT].script);

    CString strT = m_edtScript->GetLine(SCRIPT_SAMPLES[indexT].linemax);
    strT += _T("___");
    int iW = m_edtScript->TextWidth(SCINTILLA_STYLE_DEFAULT, strT);
    m_edtScript->setMaxLineWidth(iW);
    m_edtScript->SetScrollWidth(m_edtScript->getMaxLineWidth());

    m_edtScript->SetFocus();
    m_edtScript->SetSel(-1, -1);

    strT = _T("SigmaGraph Script - ");
    strT.Append((LPCTSTR) (SCRIPT_SAMPLES[indexT].title));
    SetWindowText((LPCTSTR) strT);

    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_NEXTMARKER, FALSE);
    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_PREVMARKER, FALSE);

    SetModified(FALSE);
    m_edtScript->EmptyUndoBuffer();
}

void CScriptWindow::OnSampleOne()
{
    LoadSample(0);
}

void CScriptWindow::OnSampleTwo()
{
    LoadSample(1);
}

void CScriptWindow::OnSampleThree()
{
    LoadSample(2);
}

void CScriptWindow::OnSampleFour()
{
    LoadSample(3);
}

void CScriptWindow::OnScriptReset()
{
    int_t iT = this->MessageBox(_T("Reset the scripting engine ?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION);
    if (iT != IDYES) {
        return;
    }

    if (m_pScriptEngine) {
        /* cleanup Lua */
        lua_close(m_pScriptEngine);
        m_pScriptEngine = NULL;
    }

    if (m_pszBufferA) {
        free(m_pszBufferA);
        m_pszBufferA = NULL;
    }

    ScriptInit();
}

void CScriptWindow::OnScriptAddFavorite()
{
    if (m_iFavoriteCount >= SIGMA_SCRIPT_MAXFAV) {
        SigmaApp.Output(_T("Cannot add favorite: limit (32) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    int_t jj, mm, nLength = 0;

    nLength = (int_t) _tcslen((const char_t*) m_szFilename);

    if ((nLength > 7) && (nLength < ML_STRSIZE)) {
        if (m_iFavoriteCount > 0) {
            for (jj = 0; jj < m_iFavoriteCount; jj++) {
                if (_tcsicmp((const char_t*) m_szFilename, (const char_t*) (m_pFavorite[jj].path)) == 0) {
                    return;
                }
            }
        }
        _tcscpy(m_pFavorite[m_iFavoriteCount].path, (const char_t*) m_szFilename);

        fl_getshort(m_pFavorite[m_iFavoriteCount].name, m_pFavorite[m_iFavoriteCount].path);
        nLength = (int_t) _tcslen((const char_t*) (m_pFavorite[m_iFavoriteCount].name));
        if ((nLength > 4) && (nLength < (ML_STRSIZE - 4))) {
            for (mm = nLength - 1; mm > 0; mm--) {
                if (m_pFavorite[m_iFavoriteCount].name[mm] == _T('.')) {
                    m_pFavorite[m_iFavoriteCount].name[mm] = _T('\0');
                    break;
                }
            }
        }
        m_iFavoriteCount += 1;
        m_iFavorite = m_iFavoriteCount - 1;
        UpdateFavoriteMenu();
    }
}

void CScriptWindow::OnUpdateScriptAddFavorite(CCmdUI *pCmdUI)
{
    int nLen = m_edtScript->GetTextLength();
    int_t nLength = (int_t) _tcslen((const char_t*) m_szFilename);
    BOOL bEnable = ((nLen > 2) && (nLength > 4) && (nLength < (ML_STRSIZE - 4)));
    pCmdUI->Enable(bEnable);
}

BOOL CScriptWindow::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    if ((pNMHDR->code != TTN_NEEDTEXTA) && (pNMHDR->code != TTN_NEEDTEXTW)) {
        return FALSE;
    }

    // if there is a top level routing frame then let it handle the message
    if (GetRoutingFrame() != NULL) {
        return FALSE;
    }

    // to be thorough we will need to handle UNICODE versions of the message also !!
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[512];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;

    if ((pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND))
        || (pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))) {
        // idFrom is actually the HWND of the tool 
        nID = ::GetDlgCtrlID((HWND)nID);
    }

    if (nID != 0) {
        ::AfxLoadString(nID, szFullText);
        strTipText = szFullText;

#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)    {
            lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
        }
        else {
            _mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA) {
            _wcstombsz(pTTTA->szText, strTipText,sizeof(pTTTA->szText));
        }
        else {
            lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
        }
#endif

        *pResult = 0;

        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);

        return TRUE;
    }

    return FALSE;
}

BOOL CScriptWindow::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    NMHDR* pNMHdr = reinterpret_cast<NMHDR*>(lParam);
    AFXASSUME(pNMHdr != NULL);

    //Is it a notification from the embedded control
    if (pNMHdr->hwndFrom == m_edtScript->GetSafeHwnd()) {
        SCNotification* pSCNotification = reinterpret_cast<SCNotification*>(lParam);
        switch (pNMHdr->code) {
            case SCN_CHARADDED:
                m_edtScript->OnCharAdded(pSCNotification);
                break;
            case SCN_MODIFIED:
                SetModified(TRUE);
                break;
            case SCN_DWELLSTART:
                {
                    CWnd* pFocusWnd = GetFocus();
                    if (pFocusWnd) {
                        if (pFocusWnd->GetSafeHwnd() == m_edtScript->GetSafeHwnd()) {
                            m_edtScript->OnDwellStart(pSCNotification);
                        }
                    }
                }
                break;
            case SCN_DWELLEND:
                m_edtScript->OnDwellEnd(pSCNotification);
                break;
            case SCN_MARGINCLICK:
                {
                    int nLine = m_edtScript->LineFromPosition(pSCNotification->position);
                    DoAddmarker(nLine);
                }
                break;
            default:
                break;
        }
        return TRUE; // we processed the message
    }

    return CDialog::OnNotify(wParam, lParam, pResult);
}

void CScriptWindow::OnUpdateEditFind(CCmdUI* pCmdUI)
{
    int_t iLen = m_edtScript->GetLength();
    pCmdUI->Enable((iLen >= 7) && (iLen < SCRIPT_MAXCHARS));
}

void CScriptWindow::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_edtScript->CanUndo());
}

void CScriptWindow::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_edtScript->CanRedo());
}

void CScriptWindow::OnEditCut()
{
    m_edtScript->Cut();
}

void CScriptWindow::OnEditCopy()
{
    m_edtScript->Copy();
}

void CScriptWindow::OnEditPaste()
{
    long iStart = m_edtScript->GetSelectionStart();
    long iEnd = m_edtScript->GetSelectionEnd();
    long iLen = (long) (m_edtScript->GetTextLength());
    bool bSelectedAll = (iStart <= 0) && (iEnd >= (iLen - 1));

    int iLineCount = m_edtScript->GetLineCount();

    m_edtScript->Paste();

    int iLineCountNew = m_edtScript->GetLineCount();

    if (bSelectedAll || (iLineCount != iLineCountNew)) {
        int maxLineIndex = 0, nLengthMax = 0, nLength = 0;
        CString strT;
        for (int ii = 0; ii < iLineCountNew; ii++) {
            strT = m_edtScript->GetLine(ii);
            nLength = strT.GetLength();
            if (nLength > nLengthMax) {
                nLengthMax = nLength;
                maxLineIndex = ii;
            }
        }

        m_edtScript->calcLinenumberMargin(iLineCountNew);
        strT = m_edtScript->GetLine(maxLineIndex);
        strT += _T("___");
        int iW = m_edtScript->TextWidth(SCINTILLA_STYLE_DEFAULT, strT);
        m_edtScript->setMaxLineWidth(iW);
        m_edtScript->SetScrollWidth(m_edtScript->getMaxLineWidth());
    }
}

void CScriptWindow::OnEditUndo()
{
    m_edtScript->Undo();
}

void CScriptWindow::OnEditRedo()
{
    m_edtScript->Redo();
}

void CScriptWindow::OnEditSelectAll()
{
    m_edtScript->SelectAll();
}

void CScriptWindow::OnEditFind()
{
    OnEditFindReplace(TRUE);
}

void CScriptWindow::OnEditReplace()
{
    OnEditFindReplace(FALSE);
}

void CScriptWindow::OnEditRepeat()
{
    if (!FindText(m_ScintillaEditState.strFind, m_ScintillaEditState.bNext, m_ScintillaEditState.bCase, m_ScintillaEditState.bWord, m_ScintillaEditState.bRegularExpression)) {
        TextNotFound(m_ScintillaEditState.strFind, m_ScintillaEditState.bNext, m_ScintillaEditState.bCase, m_ScintillaEditState.bWord, m_ScintillaEditState.bRegularExpression, FALSE);
    }
}

void CScriptWindow::AdjustFindDialogPosition()
{
    int nStart = m_edtScript->GetSelectionStart();
    CPoint tPoint;
    tPoint.x = m_edtScript->PointXFromPosition(nStart);
    tPoint.y = m_edtScript->PointYFromPosition(nStart);
    ClientToScreen(&tPoint);
    CRect rectDlg;
    m_ScintillaEditState.pFindReplaceDlg->GetWindowRect(&rectDlg);
    if (rectDlg.PtInRect(tPoint)) {
        if (tPoint.y > rectDlg.Height()) {
            rectDlg.OffsetRect(0, tPoint.y - rectDlg.bottom - 20);
        }
        else {
            int nVertExt = GetSystemMetrics(SM_CYSCREEN);
            if ((tPoint.y + rectDlg.Height()) < nVertExt) {
                rectDlg.OffsetRect(0, 40 + tPoint.y - rectDlg.top);
            }
        }
        m_ScintillaEditState.pFindReplaceDlg->MoveWindow(&rectDlg);
    }
}

CScintillaFindReplaceDlg* CScriptWindow::CreateFindReplaceDialog()
{
    return new CScintillaFindReplaceDlg;
}

void CScriptWindow::OnEditFindReplace(BOOL bFindOnly)
{
    m_bFirstSearch = TRUE;
    if (m_ScintillaEditState.pFindReplaceDlg != NULL) {
        if (m_ScintillaEditState.bFindOnly == bFindOnly) {
            m_ScintillaEditState.pFindReplaceDlg->SetActiveWindow();
            m_ScintillaEditState.pFindReplaceDlg->ShowWindow(SW_SHOW);
            return;
        }
        else {
            ASSERT(m_ScintillaEditState.bFindOnly != bFindOnly);
            m_ScintillaEditState.pFindReplaceDlg->SendMessage(WM_CLOSE);
            ASSERT(m_ScintillaEditState.pFindReplaceDlg == NULL);
        }
    }

    CString strFind(m_edtScript->GetSelText());
    //if selection is empty or spans multiple lines use old find text
    if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1)) {
        strFind = m_ScintillaEditState.strFind;
    }

    CString strReplace(m_ScintillaEditState.strReplace);
    m_ScintillaEditState.pFindReplaceDlg = CreateFindReplaceDialog();
    AFXASSUME(m_ScintillaEditState.pFindReplaceDlg != NULL);
    if (m_ScintillaEditState.pFindReplaceDlg == NULL) {
        return;
    }

    DWORD dwFlags = NULL;
    if (m_ScintillaEditState.bNext) {
        dwFlags |= FR_DOWN;
    }
    if (m_ScintillaEditState.bCase) {
        dwFlags |= FR_MATCHCASE;
    }
    if (m_ScintillaEditState.bWord) {
        dwFlags |= FR_WHOLEWORD;
    }
    if (m_ScintillaEditState.bRegularExpression) {
        m_ScintillaEditState.pFindReplaceDlg->SetRegularExpression(TRUE);
    }

    if (!m_ScintillaEditState.pFindReplaceDlg->Create(bFindOnly, strFind, strReplace, dwFlags, this)) {
        m_ScintillaEditState.pFindReplaceDlg = NULL;
        return;
    }
    ASSERT(m_ScintillaEditState.pFindReplaceDlg != NULL);
    m_ScintillaEditState.bFindOnly = bFindOnly;
    m_ScintillaEditState.pFindReplaceDlg->SetActiveWindow();
    m_ScintillaEditState.pFindReplaceDlg->ShowWindow(SW_SHOW);
}

void CScriptWindow::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
    m_ScintillaEditState.strFind = lpszFind;
    m_ScintillaEditState.bCase = bCase;
    m_ScintillaEditState.bWord = bWord;
    m_ScintillaEditState.bNext = bNext;
    m_ScintillaEditState.bRegularExpression = bRegularExpression;

    if (!FindText(m_ScintillaEditState.strFind, bNext, bCase, bWord, bRegularExpression)) {
        TextNotFound(m_ScintillaEditState.strFind, bNext, bCase, bWord, bRegularExpression, FALSE);
    }
    else {
        AdjustFindDialogPosition();
    }
}

void CScriptWindow::OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression, LPCTSTR lpszReplace)
{
    m_ScintillaEditState.strFind = lpszFind;
    m_ScintillaEditState.strReplace = lpszReplace;
    m_ScintillaEditState.bCase = bCase;
    m_ScintillaEditState.bWord = bWord;
    m_ScintillaEditState.bNext = bNext;
    m_ScintillaEditState.bRegularExpression = bRegularExpression;

    if (!SameAsSelected(m_ScintillaEditState.strFind, bCase, bWord, bRegularExpression)) {
        if (!FindText(m_ScintillaEditState.strFind, bNext, bCase, bWord, bRegularExpression)) {
            TextNotFound(m_ScintillaEditState.strFind, bNext, bCase, bWord, bRegularExpression, TRUE);
        }
        else {
            AdjustFindDialogPosition();
        }
        return;
    }

    if (bRegularExpression) {
        m_edtScript->TargetFromSelection();
        m_edtScript->ReplaceTargetRE(m_ScintillaEditState.strReplace.GetLength(), m_ScintillaEditState.strReplace);
    }
    else {
        m_edtScript->ReplaceSel(m_ScintillaEditState.strReplace);
    }
    if (!FindText(m_ScintillaEditState.strFind, bNext, bCase, bWord, bRegularExpression)) {
        TextNotFound(m_ScintillaEditState.strFind, bNext, bCase, bWord, bRegularExpression, TRUE);
    }
    else {
        AdjustFindDialogPosition();
    }
}

void CScriptWindow::OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
    m_ScintillaEditState.strFind = lpszFind;
    m_ScintillaEditState.strReplace = lpszReplace;
    m_ScintillaEditState.bCase = bCase;
    m_ScintillaEditState.bWord = bWord;
    m_ScintillaEditState.bNext = TRUE;
    m_ScintillaEditState.bRegularExpression = bRegularExpression;

    CWaitCursor wait;

    //Set the selection to the begining of the document to ensure all text is replaced in the document
    m_edtScript->SetSel(0, 0);

    int iLen = m_ScintillaEditState.strFind.GetLength();
    if ((iLen <= 0) || (iLen > ML_STRSIZE)) {
        return;
    }

    //Do the replacments
    m_edtScript->HideSelection(TRUE, FALSE);
    BOOL bFoundSomething = FALSE;
    int iMax = 1 + (m_edtScript->GetLength() / iLen), ii = 0;
    while (FindTextSimple(m_ScintillaEditState.strFind, m_ScintillaEditState.bNext, bCase, bWord, bRegularExpression)) {
        bFoundSomething = TRUE;
        if (bRegularExpression) {
            m_edtScript->TargetFromSelection();
            m_edtScript->ReplaceTargetRE(m_ScintillaEditState.strReplace.GetLength(), m_ScintillaEditState.strReplace);
        }
        else {
            m_edtScript->ReplaceSel(m_ScintillaEditState.strReplace);
        }
        if (++ii >= iMax) {
            break;
        }
    }

    //Restore the old selection
    m_edtScript->HideSelection(FALSE, FALSE);

    //Inform the user if we could not find anything
    if (!bFoundSomething) {
        TextNotFound(m_ScintillaEditState.strFind, m_ScintillaEditState.bNext, bCase, bWord, bRegularExpression, TRUE);
    }
}

LRESULT CScriptWindow::OnFindReplaceCmd(WPARAM /*wParam*/, LPARAM lParam)
{
    CScintillaFindReplaceDlg* pDialog = static_cast<CScintillaFindReplaceDlg*>(CFindReplaceDialog::GetNotifier(lParam));
    AFXASSUME(pDialog != NULL);
    ASSERT(pDialog == m_ScintillaEditState.pFindReplaceDlg);
    if (pDialog != m_ScintillaEditState.pFindReplaceDlg) {
        return 0;
    }

    if (pDialog->IsTerminating()) {
        m_ScintillaEditState.pFindReplaceDlg = NULL;
    }
    else if (pDialog->FindNext()) {
        OnFindNext(pDialog->GetFindString(), pDialog->SearchDown(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression());
    }
    else if (pDialog->ReplaceCurrent()) {
        ASSERT(!m_ScintillaEditState.bFindOnly);
        OnReplaceSel(pDialog->GetFindString(), pDialog->SearchDown(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression(), pDialog->GetReplaceString());
    }
    else if (pDialog->ReplaceAll()) {
        ASSERT(!m_ScintillaEditState.bFindOnly);
        OnReplaceAll(pDialog->GetFindString(), pDialog->GetReplaceString(), pDialog->MatchCase(), pDialog->MatchWholeWord(), pDialog->GetRegularExpression());
    }

    return 0;
}

BOOL CScriptWindow::SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
    //check length first
    long nStartChar = m_edtScript->GetSelectionStart(); //get the selection size    
    long nEndChar = m_edtScript->GetSelectionEnd();
    size_t nLen = _tcslen(lpszCompare); //get the #chars to search for

    //Calculate the logical length of the selection. This logic handles the case where Scintilla is hosting multibyte characters
    size_t nCnt = 0;
    for (long nPos = nStartChar; nPos < nEndChar; nPos = m_edtScript->PositionAfter(nPos)) {
        nCnt++;
    }

    //if not a regular expression then sizes must match
    if (!bRegularExpression && (nLen != nCnt)) {
        return FALSE;
    }

    //Now use the advanced search functionality of scintilla to determine the result
    int nFlags = bCase ? SCFIND_MATCHCASE : 0;
    nFlags |= bWord ? SCFIND_WHOLEWORD : 0;
    nFlags |= bRegularExpression ? SCFIND_REGEXP : 0;
    m_edtScript->SetSearchFlags(nFlags);
    m_edtScript->TargetFromSelection();                     //set target
    if (m_edtScript->SearchInTarget(static_cast<int>(nLen), lpszCompare) < 0) { //see what we got
        return FALSE;                                  //no match
    }

    //If we got a match, the target is set to the found text
    return (m_edtScript->GetTargetStart() == nStartChar) && (m_edtScript->GetTargetEnd() == nEndChar);
}

BOOL CScriptWindow::FindText(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
    CWaitCursor wait;
    return FindTextSimple(lpszFind, bNext, bCase, bWord, bRegularExpression);
}

BOOL CScriptWindow::FindTextSimple(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression)
{
    TextToFind ft;
    ft.chrg.cpMin = m_edtScript->GetSelectionStart();
    ft.chrg.cpMax = m_edtScript->GetSelectionEnd();
    if (m_bFirstSearch) {
        m_bFirstSearch = FALSE;
    }

    CStringA sUTF8Text(CScintillaCtrl::W2UTF8(lpszFind, -1));
    ft.lpstrText = sUTF8Text.GetBuffer(sUTF8Text.GetLength());
    if (ft.chrg.cpMin != ft.chrg.cpMax) { // i.e. there is a selection
        if (bNext) {
            ft.chrg.cpMin++;
        }
        else {
            ft.chrg.cpMax = ft.chrg.cpMin - 1;
        }
    }

    int nLength = m_edtScript->GetLength();
    if (bNext) {
        ft.chrg.cpMax = nLength;
    }
    else {
        ft.chrg.cpMin = 0;
    }

    DWORD dwFlags = bCase ? SCFIND_MATCHCASE : 0;
    dwFlags |= bWord ? SCFIND_WHOLEWORD : 0;
    dwFlags |= bRegularExpression ? SCFIND_REGEXP : 0;

    if (!bNext) {
        // Swap the start and end positions which Scintilla uses to flag backward searches
        int ncpMinTemp = ft.chrg.cpMin;
        ft.chrg.cpMin = ft.chrg.cpMax;
        ft.chrg.cpMax = ncpMinTemp;
    }

    // if we find the text return TRUE
    BOOL bFound = (FindAndSelect(dwFlags, ft) != -1);

    sUTF8Text.ReleaseBuffer();

    return bFound;
}

long CScriptWindow::FindAndSelect(DWORD dwFlags, TextToFind& ft)
{
    long index = m_edtScript->FindText(dwFlags, &ft);
    if (index != -1) { // i.e. we found something
        m_edtScript->SetSel(ft.chrgText.cpMin, ft.chrgText.cpMax);
    }
    return index;
}

void CScriptWindow::TextNotFound(LPCTSTR /*lpszFind*/, BOOL /*bNext*/, BOOL /*bCase*/, BOOL /*bWord*/, BOOL /*bRegularExpression*/, BOOL /*bReplaced*/)
{
    m_bFirstSearch = TRUE;
    MessageBeep(MB_ICONHAND);
}

void CScriptWindow::OnOptionsColors()
{
    StyleInfo tStyleBackup[STYLEINFO_COUNT];
    for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
        _tcscpy(tStyleBackup[ii].name, m_ScintillaPrefs.style[ii].name);
        tStyleBackup[ii].foreground = m_ScintillaPrefs.style[ii].foreground;
        tStyleBackup[ii].background = m_ScintillaPrefs.style[ii].background;
        _tcscpy(tStyleBackup[ii].fontname, m_ScintillaPrefs.style[ii].fontname);
        tStyleBackup[ii].fontsize = m_ScintillaPrefs.style[ii].fontsize;
        tStyleBackup[ii].fontstyle = m_ScintillaPrefs.style[ii].fontstyle;
        tStyleBackup[ii].lettercase = m_ScintillaPrefs.style[ii].lettercase;
    }

    CScriptColorsDlg *pDlgColors = new CScriptColorsDlg(this);
    pDlgColors->m_pWndEditor = m_edtScript;
    pDlgColors->m_pScintillaPrefs = &m_ScintillaPrefs;
    INT_PTR nRet = -1;
    nRet = pDlgColors->DoModal();
    if (nRet != IDOK) {
        for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
            _tcscpy(m_ScintillaPrefs.style[ii].name, tStyleBackup[ii].name);
            m_ScintillaPrefs.style[ii].foreground = tStyleBackup[ii].foreground;
            m_ScintillaPrefs.style[ii].background = tStyleBackup[ii].background;
            _tcscpy(m_ScintillaPrefs.style[ii].fontname, tStyleBackup[ii].fontname);
            m_ScintillaPrefs.style[ii].fontsize = tStyleBackup[ii].fontsize;
            m_ScintillaPrefs.style[ii].fontstyle = tStyleBackup[ii].fontstyle;
            m_ScintillaPrefs.style[ii].lettercase = tStyleBackup[ii].lettercase;
        }
    }
    delete pDlgColors; pDlgColors = NULL;
    m_edtScript->initPrefs(m_ScintillaPrefs);
    if (m_edtScript) {
        m_edtOutput->SetFontParameters(m_ScintillaPrefs.style[STYLEINFO_INDEX_DEFAULT].fontname, m_ScintillaPrefs.style[STYLEINFO_INDEX_DEFAULT].fontsize);
    }
}

void CScriptWindow::DoAddmarker(int nLine)
{
    if (m_edtScript->MarkerGet(nLine) & 0x1) {
        m_edtScript->MarkerDelete(nLine, 0);
        m_iMarkerCount -= 1;
    }
    else {
        m_edtScript->MarkerAdd(nLine, 0);
        m_iMarkerCount += 1;
    }

    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_NEXTMARKER, m_iMarkerCount > 0);
    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_PREVMARKER, m_iMarkerCount > 0);
}


void CScriptWindow::OnOptionsAddmarker()
{
    int nPos = m_edtScript->GetCurrentPos();
    int nLine = m_edtScript->LineFromPosition(nPos);

    DoAddmarker(nLine);
}

void CScriptWindow::OnOptionsDeleteAllMarkers()
{
    m_edtScript->MarkerDeleteAll(-1);
    m_iMarkerCount = 0;
    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_NEXTMARKER, FALSE);
    m_wndToolbar.GetToolBarCtrl().EnableButton(ID_SCRIPT_PREVMARKER, FALSE);
}

void CScriptWindow::OnOptionsFindNextmarker()
{
    int nPos = m_edtScript->GetCurrentPos();
    int nLine = m_edtScript->LineFromPosition(nPos);
    int nFoundLine = m_edtScript->MarkerNext(nLine + 1, 0x1);
    if (nFoundLine >= 0) {
        m_edtScript->GotoLine(nFoundLine);
    }
    else {
        MessageBeep(MB_ICONHAND);
    }
}

void CScriptWindow::OnOptionsFindPrevmarker()
{
    int nPos = m_edtScript->GetCurrentPos();
    int nLine = m_edtScript->LineFromPosition(nPos);
    int nFoundLine = m_edtScript->MarkerPrevious(nLine - 1, 0x1);
    if (nFoundLine >= 0) {
        m_edtScript->GotoLine(nFoundLine);
    }
    else {
        MessageBeep(MB_ICONHAND);
    }
}

void CScriptWindow::OnOptionsViewLinenumbers()
{
    int nMarginWidth = m_edtScript->GetMarginWidthN(0);
    if (nMarginWidth) {
        m_edtScript->SetMarginWidthN(0, 0);
        m_ScintillaPrefs.common.lineNumberEnable = false;
    }
    else {
        m_edtScript->SetMarginWidthN(0, 32);
        m_ScintillaPrefs.common.lineNumberEnable = true;
    }
    m_edtScript->initCommonPrefs(m_ScintillaPrefs.common);
    m_edtScript->calcLinenumberMargin();
}

void CScriptWindow::OnUpdateOptionsViewLinenumbers(CCmdUI* pCmdUI)
{
    bool bL = (m_edtScript->GetMarginWidthN(0) != 0);
    pCmdUI->SetCheck(bL);
    pCmdUI->SetText(bL ? _T("  Hide &Line Numbers") : _T("  Show &Line Numbers"));
}

void CScriptWindow::OnOptionsAutocomplete()
{
    m_ScintillaPrefs.common.autocompleteEnable = (m_ScintillaPrefs.common.autocompleteEnable == false);
    m_edtScript->initCommonPrefs(m_ScintillaPrefs.common);
}

void CScriptWindow::OnUpdateOptionsAutocomplete(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_ScintillaPrefs.common.autocompleteEnable);
    pCmdUI->SetText(m_ScintillaPrefs.common.autocompleteEnable ? _T("  Disable &Autocompletion") : _T("  Enable &Autocompletion"));
}

void CScriptWindow::OnOptionsLineSpacing100()
{
    m_ScintillaPrefs.common.lineSpacing = 0;
    m_edtScript->initCaret(0);
}
void CScriptWindow::OnUpdateOptionsLineSpacing100(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_ScintillaPrefs.common.lineSpacing == 0);
}

void CScriptWindow::OnOptionsLineSpacing125()
{
    m_ScintillaPrefs.common.lineSpacing = 1;
    m_edtScript->initCaret(1);
}
void CScriptWindow::OnUpdateOptionsLineSpacing125(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_ScintillaPrefs.common.lineSpacing == 1);
}

void CScriptWindow::OnOptionsLineSpacing150()
{
    m_ScintillaPrefs.common.lineSpacing = 2;
    m_edtScript->initCaret(2);
}
void CScriptWindow::OnUpdateOptionsLineSpacing150(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_ScintillaPrefs.common.lineSpacing == 2);
}

void CScriptWindow::OnOptionsLineSpacing175()
{
    m_ScintillaPrefs.common.lineSpacing = 3;
    m_edtScript->initCaret(3);
}
void CScriptWindow::OnUpdateOptionsLineSpacing175(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_ScintillaPrefs.common.lineSpacing == 3);
}

void CScriptWindow::OnOutputSizeSmall()
{
    m_ScintillaPrefs.outputsize = 20;
    Relayout();
}

void CScriptWindow::OnOutputSizeMedium()
{
    m_ScintillaPrefs.outputsize = 35;
    Relayout();
}

void CScriptWindow::OnOutputSizeLarge()
{
    m_ScintillaPrefs.outputsize = 50;
    Relayout();
}

void CScriptWindow::OutputView(bool bView/* = true*/)
{
    if (m_edtOutput == NULL) {
        CRect rcT;
        GetClientRect(rcT);
        rcT.top += ((75 * rcT.Height()) / 100);
        m_edtOutput = new CScriptOutput();
        m_edtOutput->Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP, rcT, this, 0);
        m_edtOutput->SetFontParameters(m_ScintillaPrefs.style[STYLEINFO_INDEX_DEFAULT].fontname, m_ScintillaPrefs.style[STYLEINFO_INDEX_DEFAULT].fontsize);
    }
    if (m_edtOutput == NULL) {
        return;
    }

    bool bViewOld = m_ScintillaPrefs.outputshow;
    m_ScintillaPrefs.outputshow = bView;
    if (bViewOld != m_ScintillaPrefs.outputshow) {
        m_edtOutput->ShowWindow((m_ScintillaPrefs.outputshow == false) ? SW_HIDE : SW_SHOW);
        Relayout();
    }
}

void CScriptWindow::OnOutputView()
{
    OutputView(m_ScintillaPrefs.outputshow == false);
}

void CScriptWindow::OnViewZoomIn()
{
    m_edtScript->ZoomIn();
}

void CScriptWindow::OnViewZoomOut()
{
    m_edtScript->ZoomOut();
}

void CScriptWindow::OnViewZoomNone()
{
    m_edtScript->SetZoom(0);
}

void CScriptWindow::OnClearOutput()
{
    m_edtOutput->Reset();
}

HICON CScriptWindow::GetIconForItem(UINT itemID) const
{
    HICON hIcon = (HICON) NULL;

    if (IS_INTRESOURCE(itemID)) {
        hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(itemID), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
    }

    if (hIcon == NULL) {
        CMenu *pMenu = GetMenu();
        if (pMenu) {
            CString sItem = _T(""); // look for a named item in resources
            if (::IsMenu(pMenu->m_hMenu)) {
                pMenu->GetMenuString(itemID, sItem, MF_BYCOMMAND);
            }
            // cannot have resource items with space in name
            if (sItem.IsEmpty() == false) {
                sItem.Replace(_T(' '), _T('_'));
                hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), sItem, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
            }
        }
    }
    return hIcon;
}

void CScriptWindow::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
    if ((lpdis == NULL) || (lpdis->CtlType != ODT_MENU)) {
        CDialog::OnDrawItem(nIDCtl, lpdis);
        return; //not for a menu
    }
    if (lpdis->itemState & ODS_GRAYED) {
        CDialog::OnDrawItem(nIDCtl, lpdis);
        return;
    }

    HICON hIcon = GetIconForItem(lpdis->itemID);
    if (hIcon) {
        ICONINFO iconinfoT;
        ::GetIconInfo(hIcon, &iconinfoT);

        BITMAP bitmapT;
        ::GetObject(iconinfoT.hbmColor, sizeof(bitmapT), &bitmapT);
        ::DeleteObject(iconinfoT.hbmColor);
        ::DeleteObject(iconinfoT.hbmMask);

        ::DrawIconEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, hIcon, bitmapT.bmWidth, bitmapT.bmHeight, 0, NULL, DI_NORMAL);
    }
}

void CScriptWindow::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
    CDialog::OnInitMenuPopup(pMenu, nIndex, bSysMenu);

    if (bSysMenu) {
        pMenu = GetSystemMenu(FALSE);
    }
    MENUINFO mnfo;
    mnfo.cbSize = sizeof(mnfo);
    mnfo.fMask = MIM_STYLE;
    mnfo.dwStyle = MNS_CHECKORBMP | MNS_AUTODISMISS;
    pMenu->SetMenuInfo(&mnfo);

    MENUITEMINFO minfo;
    minfo.cbSize = sizeof(minfo);

    for (UINT pos = 0; pos < (UINT) (pMenu->GetMenuItemCount()); pos++) {
        minfo.fMask = MIIM_FTYPE | MIIM_ID;
        pMenu->GetMenuItemInfo(pos, &minfo, TRUE);

        UINT fState = MFS_ENABLED;

        if (minfo.wID == ID_SCRIPT_LINENUMBERS) {
            bool bL = (m_edtScript->GetMarginWidthN(0) != 0);
            pMenu->ModifyMenu(pos, MF_BYPOSITION, minfo.wID, (bL ? _T("  Hide &Line Numbers") : _T("  Show &Line Numbers")));
        }
        else if (minfo.wID == ID_SCRIPT_AUTOCOMPLETE) {
            pMenu->ModifyMenu(pos, MF_BYPOSITION, minfo.wID, (m_ScintillaPrefs.common.autocompleteEnable ? _T("  Disable &Autocompletion") : _T("  Enable &Autocompletion")));
        }
        else if (minfo.wID == ID_SCRIPT_SAVE) {
            fState = m_bIsModified ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_EDIT_FIND) {
            int_t iLen = m_edtScript->GetLength();
            fState = ((iLen >= 7) && (iLen < SCRIPT_MAXCHARS)) ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_EDIT_REPLACE) {
            int_t iLen = m_edtScript->GetLength();
            fState = ((iLen >= 7) && (iLen < SCRIPT_MAXCHARS)) ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_EDIT_GOTO) {
            int_t iLen = m_edtScript->GetLength();
            fState = ((iLen >= 7) && (iLen < SCRIPT_MAXCHARS)) ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_EDIT_SELECTALL) {
            int nLen = m_edtScript->GetTextLength();
            int_t iStart = m_edtScript->GetSelectionStart(), iStop = m_edtScript->GetSelectionEnd();
            BOOL bEnable = ((nLen < 1) || ((iStart == 0) && (iStop == nLen))) ? FALSE : TRUE;
            fState = bEnable ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_EDIT_COPY) {
            int_t iStart = m_edtScript->GetSelectionStart(), iStop = m_edtScript->GetSelectionEnd();
            fState = (iStart != iStop) ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_EDIT_CUT) {
            int_t iStart = m_edtScript->GetSelectionStart(), iStop = m_edtScript->GetSelectionEnd();
            fState = (iStart != iStop) ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_EDIT_PASTE) {
            fState = ::IsClipboardFormatAvailable(CF_TEXT) ? MFS_ENABLED : MFS_DISABLED;
        }
        else if (minfo.wID == ID_SCRIPT_OUTPUT_VIEW) {
            pMenu->ModifyMenu(pos, MF_BYPOSITION, minfo.wID, (m_ScintillaPrefs.outputshow ? _T("  Hide") : _T("  Show")));
        }

        HICON hIcon = GetIconForItem(minfo.wID);

        if (hIcon && !(minfo.fType & MFT_OWNERDRAW)) {
            minfo.fMask = MIIM_FTYPE | MIIM_BITMAP | MIIM_STATE;
            minfo.hbmpItem = HBMMENU_CALLBACK;
            minfo.fType = MFT_STRING;
            minfo.fState = fState;
            pMenu->SetMenuItemInfo(pos, &minfo, TRUE);
        }
    }
}

void CScriptWindow::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
    if ((lpmis == NULL) || (lpmis->CtlType != ODT_MENU)) {
        CDialog::OnMeasureItem(nIDCtl, lpmis); //not for a menu
        return;
    }

    lpmis->itemWidth = 16;
    lpmis->itemHeight = 16;

    CMenu *pMenu = GetMenu();
    ASSERT(pMenu);
    if (pMenu == NULL) {
        CDialog::OnMeasureItem(nIDCtl, lpmis);
        return;
    }

    CString sItem = _T("");
    if (::IsMenu(pMenu->m_hMenu)) {
        pMenu->GetMenuString(lpmis->itemID, sItem, MF_BYCOMMAND);
    }

    HICON hIcon = GetIconForItem(lpmis->itemID);

    if (hIcon) {
        ICONINFO iconinfoT;
        ::GetIconInfo(hIcon, &iconinfoT);

        BITMAP bitmapT;
        ::GetObject(iconinfoT.hbmColor, sizeof(bitmapT), &bitmapT);
        ::DeleteObject(iconinfoT.hbmColor);
        ::DeleteObject(iconinfoT.hbmMask);

        lpmis->itemWidth = bitmapT.bmWidth;
        lpmis->itemHeight = bitmapT.bmHeight;
    }
}

void CScriptWindow::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}
