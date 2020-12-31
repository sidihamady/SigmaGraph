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

#include "SigmaViewData.h"

#include "SigmaDoc.h"

#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "../../LibGraph/LibFile/libfile.h"

#define IDT_TIMER_MONITOR    7

// :DEBUG: set to FALSE for release
#define MONITOR_ENABLED        FALSE
#include <psapi.h>

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    ON_WM_CREATE()
    ON_WM_GETMINMAXINFO()
    ON_COMMAND(ID_VIEW_OUTPUT, &CMainFrame::OnViewOutput)
    ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, &CMainFrame::OnUpdateViewOutput)
    ON_COMMAND(ID_VIEW_CONSOLE, &CMainFrame::OnViewConsole)
    ON_UPDATE_COMMAND_UI(ID_VIEW_CONSOLE, &CMainFrame::OnUpdateViewConsole)
    ON_COMMAND(ID_VIEW_SCRIPT, &CMainFrame::OnViewScript)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SCRIPT, &CMainFrame::OnUpdateViewScript)
    ON_COMMAND(ID_TOOLBAR, &CMainFrame::OnToolbar)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR, &CMainFrame::OnUpdateToolbar)
    ON_COMMAND(ID_TOOLBAR_DATA, &CMainFrame::OnToolbarData)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_DATA, &CMainFrame::OnUpdateToolbarData)
    ON_COMMAND(ID_TOOLBAR_PLOT, &CMainFrame::OnToolbarPlot)
    ON_UPDATE_COMMAND_UI(ID_TOOLBAR_PLOT, &CMainFrame::OnUpdateToolbarPlot)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_AA, &CMainFrame::OnUpdatePaneAA)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_BB, &CMainFrame::OnUpdatePaneBB)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANGLEUNIT, &CMainFrame::OnUpdatePaneAngle)
    ON_COMMAND(ID_VIEW_TOOLBAR, &CMainFrame::OnViewToolbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, &CMainFrame::OnUpdateViewToolbar)
    ON_COMMAND(ID_VIEW_TOOLBAR_DATA, &CMainFrame::OnViewToolbarData)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_DATA, &CMainFrame::OnUpdateViewToolbarData)
    ON_COMMAND(ID_VIEW_TOOLBAR_PLOT, &CMainFrame::OnViewToolbarPlot)
    ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_PLOT, &CMainFrame::OnUpdateViewToolbarPlot)
    ON_COMMAND(ID_VIEW_STATUSBAR, &CMainFrame::OnViewStatusbar)
    ON_UPDATE_COMMAND_UI(ID_VIEW_STATUSBAR, &CMainFrame::OnUpdateViewStatusbar)
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_INITMENUPOPUP()
    ON_WM_TIMER()
    ON_WM_DROPFILES()
    ON_WM_CLOSE()
    ON_WM_QUERYENDSESSION()
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,                    // status line indicator
    ID_INDICATOR_AA,
    ID_INDICATOR_BB,
    ID_INDICATOR_ANGLEUNIT,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
};

static int CalculateWSPrivate_Compare(const void * Val1, const void * Val2)
{
    if (*(PDWORD) Val1 == *(PDWORD) Val2) {
        return 0;
    }

    return (*(PDWORD) Val1 > *(PDWORD) Val2) ? 1 : -1;
}

DWORD CMainFrame::CalculateWSPrivate()
{
    if (MONITOR_ENABLED == FALSE) {
        return 0;
    }

    DWORD dSharedPages = 0;
    DWORD dPrivatePages = 0;
    DWORD dPageTablePages = 0;

    DWORD dWorkingSetPages[1024 * 128]; // hold the working set 
    // information get from QueryWorkingSet()
    DWORD dPageSize = 0x1000;

    HANDLE hProcess = ::GetCurrentProcess();

    if (!hProcess) {
        return 0;
    }

    if (!::QueryWorkingSet(hProcess, dWorkingSetPages, sizeof(dWorkingSetPages))) {
        return 0;
    }

    DWORD dPages = dWorkingSetPages[0];

    ::qsort(&dWorkingSetPages[1], dPages, sizeof(DWORD), CalculateWSPrivate_Compare);

    for (DWORD ii = 1; ii <= dPages; ii++) {
        DWORD dCurrentPageStatus = 0;
        DWORD dCurrentPageAddress;
        DWORD dNextPageAddress;
        DWORD dNextPageFlags;
        DWORD dPageAddress = dWorkingSetPages[ii] & 0xFFFFF000;
        DWORD dPageFlags = dWorkingSetPages[ii] & 0x00000FFF;

        while (ii <= dPages) {
            dCurrentPageStatus++;

            if (ii == dPages) {
                break;
            }

            dCurrentPageAddress = dWorkingSetPages[ii] & 0xFFFFF000;
            dNextPageAddress = dWorkingSetPages[ii + 1] & 0xFFFFF000;
            dNextPageFlags = dWorkingSetPages[ii + 1] & 0x00000FFF;

            if ((dNextPageAddress == (dCurrentPageAddress + dPageSize)) && (dNextPageFlags == dPageFlags)) {
                ii++;
            }
            else {
                break;
            }
        }

        if ((dPageAddress < 0xC0000000) || (dPageAddress > 0xE0000000)) {
            if (dPageFlags & 0x100) {
                dSharedPages += dCurrentPageStatus;
            }
            else {
                dPrivatePages += dCurrentPageStatus;
            }
        }
        else {
            dPageTablePages += dCurrentPageStatus;
        }
    }

    DWORD dTotal = dPages * 4;
    DWORD dShared = dSharedPages * 4;
    DWORD WSPrivate = (dTotal - dShared);

    return WSPrivate;
}

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    m_pwndConsole = NULL;
    m_pwndOutput = NULL;

    m_uiStatusTimer = 0L;
    m_lStatusTime = 0L;

    // Timer & Memory monitoring
    m_uiTimer = 0L;
    m_lTime = 0L;
    m_pfMonitor = NULL;
    memset(m_szMonitorFilename, 0, ML_STRSIZE * sizeof(char_t));
    memset(m_szMonitorLine, 0, ML_STRSIZE * sizeof(char_t));
    if (MONITOR_ENABLED) {
        int_t ii, nSize = ::GetModuleFileName(NULL, (LPTSTR) m_szMonitorFilename, ML_STRSIZE - 1);
        if ((nSize > 6) && (nSize < ML_STRSIZE - 16)) {
            for (ii = nSize - 1; ii > 0; ii--) {
                if (m_szMonitorFilename[ii] == _T('\\')) {
                    m_szMonitorFilename[ii + 1] = _T('\0');
                    break;
                }
            }
            _tcscat(m_szMonitorFilename, _T("Monitor\\Memory.txt"));
            m_pfMonitor = _wfopen((LPCTSTR) m_szMonitorFilename, _T("w"));
            if (m_pfMonitor != NULL) {
                fputws(_T("Time (sec.)\tMemory (KB)\n"), m_pfMonitor);
                _tcsprintf(m_szMonitorLine, ML_STRSIZE - 1, _T("%ld\t%ld\n"), m_lTime, CalculateWSPrivate());
                fputws((const char_t*) m_szMonitorLine, m_pfMonitor);
                m_uiTimer = SetTimer(IDT_TIMER_MONITOR, 1000, NULL);
                m_lTime += 1L;
            }
        }
    }
}

void CMainFrame::cleanup()
{
    if (m_pwndConsole) {
        m_pwndConsole->m_Font.DeleteObject();
        m_pwndConsole->DestroyWindow();
        delete m_pwndConsole;
        m_pwndConsole = NULL;
    }

    if (m_pwndScript) {
        m_pwndScript->DestroyWindow();
        delete m_pwndScript;
        m_pwndScript = NULL;
    }

    if (m_pwndOutput) {
        m_pwndOutput->DestroyWindow();
        delete m_pwndOutput;
        m_pwndOutput = NULL;
    }

    if (MONITOR_ENABLED) {
        if (m_uiTimer > 0L) {
            KillTimer(m_uiTimer);
            m_uiTimer = 0L;
        }
        if (m_pfMonitor) {
            _tcsprintf(m_szMonitorLine, ML_STRSIZE - 1, _T("%ld\t%ld\n"), m_lTime, CalculateWSPrivate());
            fputws((const char_t*) m_szMonitorLine, m_pfMonitor);
            fclose(m_pfMonitor);
            m_pfMonitor = NULL;
            m_lTime = 0L;
        }
    }

    if (m_uiStatusTimer > 0L) {
        KillTimer(m_uiStatusTimer);
        m_uiStatusTimer = 0L;
        m_lStatusTime = 0L;
    }

}

CMainFrame::~CMainFrame()
{
    cleanup();
}

void CMainFrame::WinHelp(DWORD dwData, UINT nCmd)
{
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) (SigmaApp.m_szHelpFilename), nCmd, dwData);
}

HICON CMainFrame::GetIconForItem(UINT itemID) const
{
    HICON hIcon = (HICON)NULL;

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

void CMainFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
    if ((lpdis == NULL) || (lpdis->CtlType != ODT_MENU)) {
        CFrameWnd::OnDrawItem(nIDCtl, lpdis);
        return; //not for a menu
    }
    if (lpdis->itemState & ODS_GRAYED) {
        CFrameWnd::OnDrawItem(nIDCtl, lpdis);
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

void CMainFrame::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
    CFrameWnd::OnInitMenuPopup(pMenu, nIndex, bSysMenu);

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

    for (UINT pos = 0; pos < (UINT)(pMenu->GetMenuItemCount()); pos++) {
        minfo.fMask = MIIM_FTYPE | MIIM_ID;
        pMenu->GetMenuItemInfo(pos, &minfo, TRUE);

        HICON hIcon = GetIconForItem(minfo.wID);

        if (hIcon && !(minfo.fType & MFT_OWNERDRAW)) {
            minfo.fMask = MIIM_FTYPE | MIIM_BITMAP;
            minfo.hbmpItem = HBMMENU_CALLBACK;
            minfo.fType = MFT_STRING;
            pMenu->SetMenuItemInfo(pos, &minfo, TRUE);
        }
    }
}

void CMainFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
    if ((lpmis == NULL) || (lpmis->CtlType != ODT_MENU)) {
        CFrameWnd::OnMeasureItem(nIDCtl, lpmis); //not for a menu
        return;
    }

    lpmis->itemWidth = 16;
    lpmis->itemHeight = 16;

    CMenu *pMenu = GetMenu();
    ASSERT(pMenu);
    if (pMenu == NULL) {
        CFrameWnd::OnMeasureItem(nIDCtl, lpmis);
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

void CMainFrame::DockControlBarRightOf(CToolBar* pBar, CToolBar* pLeftBar)
{
    CRect recT, recR;
    DWORD dwT;
    UINT nStyle;

    RecalcLayout(TRUE);

    dwT = pLeftBar->GetBarStyle();
    nStyle = 0;
    nStyle = (dwT & CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : nStyle;
    nStyle = ((dwT & CBRS_ALIGN_BOTTOM) && (nStyle == 0)) ? AFX_IDW_DOCKBAR_BOTTOM : nStyle;
    nStyle = ((dwT & CBRS_ALIGN_LEFT) && (nStyle == 0)) ? AFX_IDW_DOCKBAR_LEFT : nStyle;
    nStyle = ((dwT & CBRS_ALIGN_RIGHT) && (nStyle == 0)) ? AFX_IDW_DOCKBAR_RIGHT : nStyle;

    pLeftBar->GetWindowRect(&recT);
    pBar->GetWindowRect(&recR);
    CRect recTT = ((nStyle == AFX_IDW_DOCKBAR_TOP) || (nStyle == AFX_IDW_DOCKBAR_BOTTOM)) ? CRect(recT.right - 2, recT.top, recT.right - 2 + recR.Width(), recT.bottom)
        : CRect(recT.left, recT.bottom - 2, recT.right, recT.bottom - 2 + recR.Width());

    DockControlBar(pBar, nStyle, &recTT);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    if (!m_wndStatusbar.Create(this)) {
        return -1;            // fail to create
    }
    m_wndStatusbar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

    // Standard ToolBar
    if (!m_wndToolbar.CreateEx(this, 0, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_TOOLTIPS/*  | CBRS_GRIPPER*/ | CBRS_FLYBY | CBRS_SIZE_FIXED, CRect(2, 2, 2, 2), AFX_IDW_CONTROLBAR_FIRST)) {
        return -1;            // fail to create
    }
    m_wndToolbar.LoadToolBar(IDR_MAINFRAME);
    m_wndToolbar.LoadTrueColorToolBar(16, IDB_TOOLBAR_FILE, IDB_TOOLBAR_FILE, NULL);
    m_wndToolbar.SetWindowText(_T("Standard"));
    m_wndToolbar.EnableDocking(CBRS_ALIGN_TOP);
    m_wndToolbar.GetToolBarCtrl().SetPadding(5,5);
    m_wndToolbar.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);

    // Datasheet
    if (!m_wndToolbarData.CreateEx(this, 0, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_TOOLTIPS/*  | CBRS_GRIPPER*/ | CBRS_FLYBY | CBRS_SIZE_FIXED, CRect(2, 2, 2, 2), AFX_IDW_CONTROLBAR_FIRST + 1)) {
        return -1;            // fail to create
    }

    m_wndToolbarData.LoadToolBar(IDR_TOOLBAR_DATA);
    m_wndToolbarData.LoadTrueColorToolBar(16, IDB_TOOLBAR_DATA, IDB_TOOLBAR_DATA, NULL);
    m_wndToolbarData.SetWindowText(_T("Datasheet"));
    m_wndToolbarData.EnableDocking(CBRS_ALIGN_TOP);
    m_wndToolbarData.GetToolBarCtrl().SetPadding(5, 5);
    m_wndToolbarData.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);

    // Plot
    if (!m_wndToolbarPlot.CreateEx(this, 0, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_TOOLTIPS/*  | CBRS_GRIPPER*/ | CBRS_FLYBY | CBRS_SIZE_FIXED, CRect(2, 2, 2, 2), AFX_IDW_CONTROLBAR_FIRST + 2)) {
        return -1;            // fail to create
    }
    m_wndToolbarPlot.LoadToolBar(IDR_TOOLBAR_PLOT);
    m_wndToolbarPlot.LoadTrueColorToolBar(16, IDB_TOOLBAR_PLOT, IDB_TOOLBAR_PLOT, NULL);
    m_wndToolbarPlot.SetWindowText(_T("Graph"));
    m_wndToolbarPlot.EnableDocking(CBRS_ALIGN_TOP);
    m_wndToolbarPlot.GetToolBarCtrl().SetPadding(5, 5);
    m_wndToolbarPlot.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);

    EnableDocking(CBRS_ALIGN_TOP);

    DockControlBar(&m_wndToolbar, AFX_IDW_DOCKBAR_TOP);
    DockControlBarRightOf(&m_wndToolbarData, &m_wndToolbar);
    DockControlBarRightOf(&m_wndToolbarPlot, &m_wndToolbarData);

    m_wndToolbar.m_pDockBar = NULL;
    m_wndToolbarData.m_pDockBar = NULL;
    m_wndToolbarPlot.m_pDockBar = NULL;

    CMDIFrameWnd::ShowControlBar(&m_wndStatusbar, (SigmaApp.m_Options.opt[0] == 0) ? FALSE : TRUE, FALSE);
    CMDIFrameWnd::ShowControlBar(&m_wndToolbar, (SigmaApp.m_Options.opt[1] == 0) ? FALSE : TRUE, FALSE);
    CMDIFrameWnd::ShowControlBar(&m_wndToolbarData, (SigmaApp.m_Options.opt[2] == 0) ? FALSE : TRUE, FALSE);
    CMDIFrameWnd::ShowControlBar(&m_wndToolbarPlot, (SigmaApp.m_Options.opt[3] == 0) ? FALSE : TRUE, FALSE);

    // Console
    m_pwndConsole = NULL;
    m_pwndOutput = NULL;
    m_pwndScript = NULL;

    SetWindowText(_T("SigmaGraph"));
    m_strTitle = _T("SigmaGraph");

    DragAcceptFiles(TRUE);

    if (MONITOR_ENABLED) {
        m_uiTimer = SetTimer(IDT_TIMER_MONITOR, 1000, NULL);
        m_lTime += 1L;
    }

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CMDIFrameWnd::PreCreateWindow(cs)) {
        return FALSE;
    }
    // TODO: Modify the Window class or styles here by modifying
    return TRUE;
}

BOOL CMainFrame::CreateSigmaView(CDocTemplate* pTemplate, CRuntimeClass* pViewClass, BOOL bSetForeground)
{
    // Get the currently active child. 
    CMDIChildWnd* pMDIActive = MDIGetActive();
    ASSERT(pMDIActive != NULL);
    if (pMDIActive == NULL) {
        return FALSE;
    }
    // now get that child's document... you may not need to do this, but if there may
    // be multiple documents this a way to make sure that the correct set of windows
    // is being acted on.

    BOOL bShown = FALSE;

    CDocument* pDoc = pMDIActive->GetActiveDocument();
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return FALSE;
    }
    BOOL bIsModified = ((CSigmaDoc*)pDoc)->IsModified();

    CView* pView = pMDIActive->GetActiveView();
    ASSERT(pView != NULL);
    if (pView == NULL) {
        return FALSE;
    }

    UINT uiState = SW_SHOW;
    CWnd *pWnd = pView->GetParent();
    ASSERT(pWnd != NULL);
    if (pWnd == NULL) {
        return FALSE;
    }
    if (pWnd->IsZoomed()) {
        uiState = SW_SHOWMAXIMIZED;
    }

    CFrameWnd *pFrmWnd = NULL;

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL) {
        pView = pDoc->GetNextView(pos);
        if (pView == NULL) {
            break;
        }
        if (pView->IsKindOf(pViewClass)) { // We found a view for this already created.
            pFrmWnd = pView->GetParentFrame();
            ASSERT(pFrmWnd != NULL);
            if (pFrmWnd == NULL) {
                return FALSE;
            }
            pFrmWnd->ActivateFrame();
            if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewData))) {
                CChildFrameData *pChild = (CChildFrameData*)pView;
                if (pChild->IsWindowVisible() == TRUE) {
                    if (bSetForeground == FALSE) {
                        pFrmWnd->ShowWindow(SW_HIDE);
                    }
                    else {
                        pFrmWnd->ShowWindow(uiState);
                        bShown = TRUE;
                    }
                }
                else {
                    pFrmWnd->ShowWindow(uiState);
                    bShown = TRUE;
                }
            }
            else if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewPlot))) {
                CChildFramePlot *pChild = (CChildFramePlot*)pView;
                if (pChild->IsWindowVisible() == TRUE) {
                    if (bSetForeground == FALSE) {
                        pFrmWnd->ShowWindow(SW_HIDE);
                    }
                    else {
                        pFrmWnd->ShowWindow(uiState);
                        bShown = TRUE;
                    }
                }
                else {
                    pFrmWnd->ShowWindow(uiState);
                    bShown = TRUE;
                }
            }
            else if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewDataFit))) {
                CChildFrameDataFit *pChild = (CChildFrameDataFit*)pView;
                if (pChild->IsWindowVisible() == TRUE) {
                    if (bSetForeground == FALSE) {
                        pFrmWnd->ShowWindow(SW_HIDE);
                    }
                    else {
                        pFrmWnd->ShowWindow(uiState);
                        bShown = TRUE;
                    }
                }
                else {
                    pFrmWnd->ShowWindow(uiState);
                    bShown = TRUE;
                }
            }
            return bShown;
        }
    }

    // This view doesn't exist for this document, create it

    BOOL bInit = ((CSigmaDoc*)pDoc)->m_bInitialized;

    CMDIChildWnd* pNewFrame = (CMDIChildWnd*)(pTemplate->CreateNewFrame(pDoc, NULL));
    if (pNewFrame == NULL) {
        return FALSE; // not created you may want to assert here
    }
    ASSERT_KINDOF(CMDIChildWnd, pNewFrame);
    pTemplate->InitialUpdateFrame(pNewFrame, pDoc);

    pos = pDoc->GetFirstViewPosition();
    while (pos != NULL) {
        pView = pDoc->GetNextView(pos);
        if (pView == NULL) {
            break;
        }
        if (pView->IsKindOf(pViewClass)) { // We found a view for this already created.
            pFrmWnd = pView->GetParentFrame();
            ASSERT(pFrmWnd != NULL);
            if (pFrmWnd == NULL) {
                return FALSE;
            }
            pFrmWnd->ActivateFrame();

            WINDOWPLACEMENT mdiPlace;
            memset(&mdiPlace, 0, sizeof(WINDOWPLACEMENT));
            mdiPlace.length = sizeof(WINDOWPLACEMENT);
            this->GetWindowPlacement(&mdiPlace);

            if (((CSigmaDoc*)pDoc)->m_bInitializedPlot == FALSE) {

                if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewPlot))) {

                    WINDOWPLACEMENT *pPl = &(((CSigmaDoc*) pDoc)->m_PlacementPlot);
                    if ((pPl->rcNormalPosition.top > 0) && (pPl->rcNormalPosition.left > 0)) {
                        pFrmWnd->SetWindowPlacement(pPl);
                        ((CSigmaDoc*)pDoc)->m_bInitializedData = TRUE;
                    }
                }
            }

            if (((CSigmaDoc*)pDoc)->m_bInitializedData == FALSE) {

                if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewData))) {

                    WINDOWPLACEMENT *pPl = &(((CSigmaDoc*) pDoc)->m_PlacementDatasheet);
                    if ((pPl->rcNormalPosition.top > 0) && (pPl->rcNormalPosition.left > 0)) {
                        pFrmWnd->SetWindowPlacement(pPl);
                        ((CSigmaDoc*)pDoc)->m_bInitializedData = TRUE;
                    }
                }
            }

            if (((CSigmaDoc*)pDoc)->m_bInitializedDataFit == FALSE) {

                if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewDataFit))) {

                    WINDOWPLACEMENT *pPl = &(((CSigmaDoc*) pDoc)->m_PlacementDatasheetFit);
                    if ((pPl->rcNormalPosition.top > 0) && (pPl->rcNormalPosition.left > 0)) {
                        pFrmWnd->SetWindowPlacement(pPl);
                        ((CSigmaDoc*)pDoc)->m_bInitializedDataFit = TRUE;
                    }
                }
            }
            break;
        }
    }

    ((CSigmaDoc*)pDoc)->SetModifiedFlag(bIsModified);

    return TRUE;
}

BOOL CMainFrame::CloseSigmaView(CDocTemplate* pTemplate, CRuntimeClass* pViewClass)
{
    // Get the currently active child. 
    CMDIChildWnd* pMDIActive = MDIGetActive();
    if (pMDIActive == NULL) {
        return FALSE;
    }
    // now get that child's document... you may not need to do this, but if there may
    // be multiple documents this a way to make sure that the correct set of windows
    // is being acted on.

    CDocument* pDoc = pMDIActive->GetActiveDocument();
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return FALSE;
    }
    CView* pView = NULL;

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL) {
        pView = pDoc->GetNextView(pos);
        if (pView == NULL) {
            break;
        }
        if (pView->IsKindOf(pViewClass)) { // We found a view for this already created.
            if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewData))) {
                CChildFrameData *pChild = (CChildFrameData*)pView;
                pChild->m_bClose = TRUE;
            }
            else if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewPlot))) {
                CChildFramePlot *pChild = (CChildFramePlot*)pView;
                pChild->m_bClose = TRUE;
            }
            else if (pView->IsKindOf(RUNTIME_CLASS(CSigmaViewDataFit))) {
                CChildFrameDataFit *pChild = (CChildFrameDataFit*)pView;
                pChild->m_bClose = TRUE;
            }
            return TRUE;
        }
    }

    return FALSE;
}

CView* CMainFrame::GetSigmaActiveViewX(CDocTemplate* pTemplate, CRuntimeClass* pViewClass)
{
    CMDIChildWnd* pMDIActive = MDIGetActive();
    if (pMDIActive == NULL) {
        return NULL;
    }

    CDocument* pDoc = pMDIActive->GetActiveDocument();
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return NULL;
    }
    CView* pView = pMDIActive->GetActiveView();
    if (pView->IsKindOf(pViewClass)) {
        return pView;
    }

    return NULL;
}

CView* CMainFrame::GetSigmaActiveView(CDocTemplate* pTemplate, CRuntimeClass* pViewClass)
{
    CMDIChildWnd* pMDIActive = MDIGetActive();
    if (pMDIActive == NULL) {
        return NULL;
    }

    CDocument* pDoc = pMDIActive->GetActiveDocument();
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return NULL;
    }
    CView* pView = NULL;

    POSITION pos = pDoc->GetFirstViewPosition();
    while (pos != NULL) {
        pView = pDoc->GetNextView(pos);
        if (pView == NULL) {
            break;
        }
        if (pView->IsKindOf(pViewClass)) {
            return pView;
        }
    }

    return NULL;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH00;
    lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT00;

    CMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::OutputCreate(void)
{
    if (m_pwndOutput != NULL) {
        if (m_pwndOutput->GetSafeHwnd() == NULL) {
            delete m_pwndOutput;
            m_pwndOutput = NULL;
        }
    }

    if (m_pwndOutput == NULL) {
        m_pwndOutput = new CSigmaOutput();
        if (m_pwndOutput) {
            if (m_pwndOutput->Create(this)) {
                WINDOWPLACEMENT tPlacementMain;
                memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
                tPlacementMain.length = sizeof(WINDOWPLACEMENT);
                SigmaApp.GetWindowPlacement(_T("OutputPlace.par"), &tPlacementMain, SIGMA_DEFWIDTH_CONSOLE, SIGMA_DEFHEIGHT_CONSOLE, 48);
                m_pwndOutput->SetWindowPlacement(&tPlacementMain);
            }
            else {
                delete m_pwndOutput;
                m_pwndOutput = NULL;
            }
        }
    }
}

void CMainFrame::OutputView(bool bShow/* = true*/)
{
    if (m_pwndOutput == NULL) {
        OutputCreate();
        if (bShow && (m_pwndOutput != NULL)) {
            return;
        }
    }
    if (m_pwndOutput == NULL) {
        this->MessageBox(_T("Cannot show output window: internal error."), _T("SigmaGraph"), MB_ICONERROR);
        return;
    }

    if (bShow) {
        m_pwndOutput->ShowWindow(SW_SHOW);
        m_pwndOutput->SetForegroundWindow();
        m_pwndOutput->m_edtOutput.SetSel(-1, -1);
    }
    else {
        m_pwndOutput->ShowWindow(SW_HIDE);
    }
}

void CMainFrame::OnViewOutput()
{
    OutputView();
}

void CMainFrame::OnUpdateViewOutput(CCmdUI *pCmdUI)
{
    /* nothing */
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
    if (MONITOR_ENABLED && (nIDEvent == m_uiTimer)) {
        if (m_pfMonitor != NULL) {
            _tcsprintf(m_szMonitorLine, ML_STRSIZE - 1, _T("%ld\t%ld\n"), m_lTime, CalculateWSPrivate());
            fputws((const char_t*) m_szMonitorLine, m_pfMonitor);
            SetStatusbarInfo((const char_t*) m_szMonitorLine);
            m_lTime += 1L;
        }
    }

    if (nIDEvent == m_uiStatusTimer) {
        if (m_lStatusTime >= 1) {
            KillTimer(m_uiStatusTimer);
            m_uiStatusTimer = 0L;
            m_wndStatusbar.SetPaneText(0, _T(""), TRUE);
        }
        m_lStatusTime += 1;
    }

    CMDIFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::SetStatusbarInfo(CString strText/* = _T("")*/, int_t nStatusbarPane/* = 0*/, BOOL bAutoClear/* = FALSE*/)
{
    m_wndStatusbar.SetPaneText(nStatusbarPane, (LPCTSTR)strText, TRUE);

    if ((nStatusbarPane == 0) && bAutoClear) {
        if (m_uiStatusTimer > 0L) {
            KillTimer(m_uiStatusTimer);
            m_uiStatusTimer = 0L;
        }
        m_lStatusTime = 0L;
        m_uiStatusTimer = SetTimer(IDT_TIMER_MONITOR + 10, 5000, NULL);
    }
}

CFont* CMainFrame::GetStatusbarFont(void)
{
    return m_wndStatusbar.GetFont();
}

void CMainFrame::GetPaneInfo(int_t nIndex, UINT &nID, UINT &nStyle, int_t &cxWidth)
{
    m_wndStatusbar.GetPaneInfo(nIndex, nID, nStyle, cxWidth);
}
void CMainFrame::SetPaneInfo(int_t nIndex, UINT nID, UINT nStyle, int_t cxWidth)
{
    m_wndStatusbar.SetPaneInfo(nIndex, nID, nStyle, cxWidth);
}

void CMainFrame::OnViewConsole()
{
    // Console
    if (m_pwndConsole != NULL) {
        if (m_pwndConsole->GetSafeHwnd() == NULL) {
            delete m_pwndConsole;
            m_pwndConsole = NULL;
        }
    }

    if (m_pwndConsole == NULL) {
        m_pwndConsole = new CSigmaConsole();
        if (m_pwndConsole) {
            if (m_pwndConsole->Create(IDD_CONSOLE, this)) {

                WINDOWPLACEMENT tPlacementMain;
                memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
                tPlacementMain.length = sizeof(WINDOWPLACEMENT);
                SigmaApp.GetWindowPlacement(_T("ConsolePlace.par"), &tPlacementMain, SIGMA_DEFWIDTH_CONSOLE, SIGMA_DEFHEIGHT_CONSOLE, 96);
                m_pwndConsole->SetWindowPlacement(&tPlacementMain);

                m_pwndConsole->m_edtConsole.SetSel(-1,-1);
            }
            else {
                delete m_pwndConsole;
                m_pwndConsole = NULL;
                this->MessageBox(_T("Cannot show console window: internal error."), _T("SigmaGraph"), MB_ICONERROR);
            }
        }
    }

    if (m_pwndConsole) {
        m_pwndConsole->ShowWindow(SW_SHOW);
        m_pwndConsole->SetForegroundWindow();
    }
}

void CMainFrame::OnUpdateViewConsole(CCmdUI *pCmdUI)
{
    /* nothing */
}

void CMainFrame::OnViewScript()
{
    if (m_pwndScript != NULL) {
        if (m_pwndScript->GetSafeHwnd() == NULL) {
            delete m_pwndScript;
            m_pwndScript = NULL;
        }
    }
    if (m_pwndScript == NULL) {
        m_pwndScript = new CScriptWindow();
        if (m_pwndScript) {
            if (m_pwndScript->Create(IDD_SCRIPT, this)) {
                WINDOWPLACEMENT tPlacementMain;
                memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
                tPlacementMain.length = sizeof(WINDOWPLACEMENT);
                SigmaApp.GetWindowPlacement(_T("ScriptPlace.par"), &tPlacementMain, SIGMA_DEFWIDTH_SCRIPT, SIGMA_DEFHEIGHT_SCRIPT, 0);
                m_pwndScript->SetWindowPlacement(&tPlacementMain);
                m_pwndScript->m_edtScript->SetSel(-1, -1);
            }
            else {
                delete m_pwndScript;
                m_pwndScript = NULL;
                this->MessageBox(_T("Cannot show script window: internal error."), _T("SigmaGraph"), MB_ICONERROR);
            }
        }
    }

    if (m_pwndScript) {
        m_pwndScript->ShowWindow(SW_SHOW);
        m_pwndScript->SetForegroundWindow();
    }
}

void CMainFrame::OnUpdateViewScript(CCmdUI *pCmdUI)
{
    /* nothing */
}

CSigmaDoc* CMainFrame::FindDoc(CString strName)
{

    CDocTemplate* pTemplate;
    POSITION pos = SigmaApp.GetFirstDocTemplatePosition();
    while (pos != NULL) {
        pTemplate = SigmaApp.GetNextDocTemplate(pos);
        ASSERT(pTemplate);

        // tous les documents du template.
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD) {
            CSigmaDoc* pDoc = (CSigmaDoc*)(pTemplate->GetNextDoc(posD));
            if (pDoc != NULL) {
                if (pDoc->GetName() == strName) {
                    pDoc->ActivateView(0);
                    pDoc->ActivateView(1);
                    return pDoc;
                }
            }
        }
    }

    return NULL;
}

CSigmaDoc* CMainFrame::FindDoc(int_t iID)
{

    CDocTemplate* pTemplate;
    POSITION pos = SigmaApp.GetFirstDocTemplatePosition();
    while (pos != NULL) {
        pTemplate = SigmaApp.GetNextDocTemplate(pos);
        ASSERT(pTemplate);

        // tous les documents du template.
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD) {
            CSigmaDoc* pDoc = (CSigmaDoc*)(pTemplate->GetNextDoc(posD));
            if (pDoc != NULL) {
                if (pDoc->GetID() == iID) {
                    return pDoc;
                }
            }
        }
    }

    return NULL;
}

void CMainFrame::OnClose()
{
    WINDOWPLACEMENT tPlacementMain;
    memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
    tPlacementMain.length = sizeof(WINDOWPLACEMENT);
    this->GetWindowPlacement(&tPlacementMain);
    SigmaApp.SetWindowPlacement(_T("MainPlace.par"), &tPlacementMain);

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
                    if (FALSE == pDoc->ActivateView(1)) {
                        pDoc->ActivateView(0);
                    }
                    CString strT;
                    CString strTX = pDoc->GetTitle();
                    strTX = strTX.Trim(_T(" *"));
                    strT.Format(_T("Save '%s' before closing?"), strTX);
                    int_t iT = this->MessageBox(strT, _T("SigmaGraph"), MB_YESNOCANCEL | MB_ICONQUESTION);
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
            }
        }
    }

    if (IsZoomed()) {
        SigmaApp.m_Options.winstate = 0x01;
    }
    else if (IsIconic()) {
        SigmaApp.m_Options.winstate = 0x02;
    }
    else {
        SigmaApp.m_Options.winstate = 0x00;
    }

    if (m_pwndConsole) {

        WINDOWPLACEMENT tPlacementMain;
        memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
        tPlacementMain.length = sizeof(WINDOWPLACEMENT);
        m_pwndConsole->GetWindowPlacement(&tPlacementMain);
        SigmaApp.SetWindowPlacement(_T("ConsolePlace.par"), &tPlacementMain);

        m_pwndConsole->DestroyWindow();
        if (m_pwndConsole && (m_pwndConsole->m_hWnd)) {
            delete m_pwndConsole;
        }
        m_pwndConsole = NULL;
    }

    if (m_pwndScript) {

        WINDOWPLACEMENT tPlacementMain;
        memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
        tPlacementMain.length = sizeof(WINDOWPLACEMENT);
        m_pwndScript->GetWindowPlacement(&tPlacementMain);
        SigmaApp.SetWindowPlacement(_T("ScriptPlace.par"), &tPlacementMain);

        if (m_pwndScript->IsModified()) {
            m_pwndScript->ShowWindow(SW_RESTORE);
            m_pwndScript->SetForegroundWindow();
        }
        m_pwndScript->ScriptClose();
        m_pwndScript->DestroyWindow();
        if (m_pwndScript && (m_pwndScript->m_hWnd)) {
            delete m_pwndScript;
        }
        m_pwndScript = NULL;
    }

    if (m_pwndOutput) {

        WINDOWPLACEMENT tPlacementMain;
        memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
        tPlacementMain.length = sizeof(WINDOWPLACEMENT);
        m_pwndOutput->GetWindowPlacement(&tPlacementMain);
        SigmaApp.SetWindowPlacement(_T("OutputPlace.par"), &tPlacementMain);

        m_pwndOutput->DestroyWindow();
        if (m_pwndOutput && (m_pwndOutput->m_hWnd)) {
            delete m_pwndOutput;
        }
        m_pwndOutput = NULL;
    }

    cleanup();

    CMDIFrameWnd::OnClose();
}

void CMainFrame::OnToolbar()
{
    if (m_wndToolbar.IsWindowVisible()) {
        ShowControlBar(&m_wndToolbar, FALSE, FALSE);
    }
    else {
        ShowControlBar(&m_wndToolbar, TRUE, FALSE);
    }
}

void CMainFrame::OnUpdateToolbar(CCmdUI *pCmdUI)
{
    if (m_wndToolbar.IsWindowVisible()) {
        pCmdUI->SetCheck(TRUE);
    }
    else {
        pCmdUI->SetCheck(FALSE);
    }
}


void CMainFrame::OnToolbarData()
{
    if (m_wndToolbarData.IsWindowVisible()) {
        ShowControlBar(&m_wndToolbarData, FALSE, FALSE);
    }
    else {
        ShowControlBar(&m_wndToolbarData, TRUE, FALSE);
    }
}

void CMainFrame::OnUpdateToolbarData(CCmdUI *pCmdUI)
{
    if (m_wndToolbarData.IsWindowVisible()) {
        pCmdUI->SetCheck(TRUE);
    }
    else {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnToolbarPlot()
{
    if (m_wndToolbarPlot.IsWindowVisible()) {
        ShowControlBar(&m_wndToolbarPlot, FALSE, FALSE);
    }
    else {
        ShowControlBar(&m_wndToolbarPlot, TRUE, FALSE);
    }
}

void CMainFrame::OnUpdateToolbarPlot(CCmdUI *pCmdUI)
{
    if (m_wndToolbarPlot.IsWindowVisible()) {
        pCmdUI->SetCheck(TRUE);
    }
    else {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePaneAngle(CCmdUI *pCmdUI)
{
    if (ml_angleunit == 0x00) {
        pCmdUI->SetText(_T("Radian"));
    }
    else if (ml_angleunit == 0x01) {
        pCmdUI->SetText(_T("Degree"));
    }
    else if (ml_angleunit == 0x02) {
        pCmdUI->SetText(_T("Gradian"));
    }
}

void CMainFrame::OnUpdatePaneAA(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CMainFrame::OnUpdatePaneBB(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
    char_t szFilename[ML_STRSIZE], szExt[5];
    int_t iNumFilesDroped = -1, nNumFiles = SIGMA_MAXDOC - SigmaApp.m_nDocCount;
    int_t iIndex = 0, nLength = 0;
    BOOL bOK = TRUE ;

    if (nNumFiles <= 0) {
        ::DragFinish(hDropInfo);
        return;
    }

    //Getting the number of files been droped
    memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
    iNumFilesDroped = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, szFilename, ML_STRSIZE - 1);
    if (iNumFilesDroped < 1) {
        ::DragFinish(hDropInfo);
        return;
    }
    if (iNumFilesDroped > nNumFiles) {
        iNumFilesDroped = nNumFiles;
    }
    memset(szExt, 0, 5 * sizeof(char_t));
    for (iIndex = 0; iIndex < iNumFilesDroped; iIndex++) {
        nLength = ::DragQueryFile(hDropInfo, iIndex, szFilename, ML_STRSIZE - 1);
        if ((nLength < 5) || (nLength >= (ML_STRSIZE - 1))) {
            continue;
        }
        szExt[0] = szFilename[nLength - 4];
        szExt[1] = szFilename[nLength - 3];
        szExt[2] = szFilename[nLength - 2];
        szExt[3] = szFilename[nLength - 1];
        szExt[4] = _T('\0');
        if (_tcsicmp((const char_t*)szExt, _T(".lua")) == 0) {
            /* nothing */
            if ((NULL == m_pwndScript) || (m_pwndScript->IsWindowVisible() == FALSE)) {
                OnCmdMsg(ID_VIEW_SCRIPT, 0, 0, 0);
            }
            if (m_pwndScript) {
                m_pwndScript->ScriptFileOpen((LPCTSTR)szFilename);
            }
        }
        else {
            if ((_tcsicmp((const char_t*) szExt, _T(".sid")) == 0)
                || (_tcsicmp((const char_t*) szExt, _T(".txt")) == 0)
                || (_tcsicmp((const char_t*) szExt, _T(".dat")) == 0)
                || (_tcsicmp((const char_t*) szExt, _T(".csv")) == 0)
                || (_tcsicmp((const char_t*) szExt, _T(".log")) == 0)) {
                SigmaApp.m_bDropping = TRUE;
                _tcscpy(SigmaApp.m_szDropFilename, (const char_t*)szFilename);
                SigmaApp.OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL);
            }
        }
    }

    ::DragFinish(hDropInfo);
    return;
}

BOOL CMainFrame::OnQueryEndSession()
{
    // Sauvegarder le cas échéant les documents modifiés
    //CWinApp* pApp = AfxGetApp();        
    // parcourir tous les templates
    CDocTemplate* pTemplate;
    POSITION pos = SigmaApp.GetFirstDocTemplatePosition();
    while (pos != NULL) {
        pTemplate = SigmaApp.GetNextDocTemplate(pos);
        ASSERT(pTemplate);
        if (pTemplate == NULL) {
            return TRUE;
        }

        // tous les documents du template.
        POSITION posD = pTemplate->GetFirstDocPosition();
        while (posD) {
            CSigmaDoc* pDoc = (CSigmaDoc*)(pTemplate->GetNextDoc(posD));
            if (pDoc != NULL) {
                if (pDoc->IsModified()) {
                    if (FALSE == pDoc->ActivateView(1)) {
                        pDoc->ActivateView(0);
                    }
                    CString strT;
                    CString strTX = pDoc->GetTitle();
                    strTX = strTX.Trim(_T(" *"));
                    strT.Format(_T("Save '%s' before closing?"), strTX);
                    int_t iT = this->MessageBox(strT, _T("SigmaGraph"), MB_YESNOCANCEL | MB_ICONQUESTION);
                    if (iT == IDYES) {
                        // Save document
                        pDoc->SaveSigmaDoc();
                    }
                    else if (iT == IDCANCEL) {
                        return FALSE;
                    }
                    // Eviter que CMDIFrameWnd::OnQueryEndSession() ne tente de sauvegarder le document (et l'écraser!)
                    pDoc->SetModifiedFlag(FALSE);
                }
            }
        }
    }

    if (!CMDIFrameWnd::OnQueryEndSession()) {
        return FALSE;
    }

    return TRUE;
}

void CMainFrame::OnViewToolbar()
{
    SigmaApp.m_Options.opt[1] = (m_wndToolbar.IsWindowVisible()) ?  0 : 1;    // (Toolbar - Main) visible ?

    CMDIFrameWnd::ShowControlBar(&m_wndToolbar, (SigmaApp.m_Options.opt[1] == 0) ? FALSE : TRUE, FALSE);
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_wndToolbar.IsWindowVisible());
}

void CMainFrame::OnViewToolbarData()
{
    SigmaApp.m_Options.opt[2] = (m_wndToolbarData.IsWindowVisible()) ?  0 : 1;    // (Toolbar - Datasheet) visible ?

    CMDIFrameWnd::ShowControlBar(&m_wndToolbarData, (SigmaApp.m_Options.opt[2] == 0) ? FALSE : TRUE, FALSE);
}

void CMainFrame::OnUpdateViewToolbarData(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_wndToolbarData.IsWindowVisible());
}

void CMainFrame::OnViewToolbarPlot()
{
    SigmaApp.m_Options.opt[3] = (m_wndToolbarPlot.IsWindowVisible()) ?  0 : 1;    // (Toolbar - Graph) visible ?

    CMDIFrameWnd::ShowControlBar(&m_wndToolbarPlot, (SigmaApp.m_Options.opt[3] == 0) ? FALSE : TRUE, FALSE);
}

void CMainFrame::OnUpdateViewToolbarPlot(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_wndToolbarPlot.IsWindowVisible());
}

void CMainFrame::OnViewStatusbar()
{
    SigmaApp.m_Options.opt[0] = (m_wndStatusbar.IsWindowVisible()) ?  0 : 1;    // (Statusbar) visible ?

    CMDIFrameWnd::ShowControlBar(&m_wndStatusbar, (SigmaApp.m_Options.opt[0] == 0) ? FALSE : TRUE, FALSE);
}

void CMainFrame::OnUpdateViewStatusbar(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_wndStatusbar.IsWindowVisible());
}

void CMainFrame::ActivateFrame(int nCmdShow)
{
    CMDIFrameWnd::ActivateFrame(nCmdShow);
}
