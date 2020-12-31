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

#pragma once

#include "SigmaOutput.h"
#include "ProgressBar.h"
#include "SigmaToolBar.h"
#include "ScriptWindow.h"

class CMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

    BOOL CreateSigmaView(CDocTemplate* pTemplate,CRuntimeClass* pViewClass, BOOL bSetForeground);
    BOOL CloseSigmaView(CDocTemplate* pTemplate, CRuntimeClass* pViewClass);
    CView* GetSigmaActiveView(CDocTemplate* pTemplate, CRuntimeClass* pViewClass);
    CView* GetSigmaActiveViewX(CDocTemplate* pTemplate, CRuntimeClass* pViewClass);

    // Timer & Memory monitoring
    DWORD CalculateWSPrivate();
    UINT_PTR m_uiTimer;
    long_t m_lTime;
    char_t m_szMonitorFilename[ML_STRSIZE];
    char_t m_szMonitorLine[ML_STRSIZE];
    FILE *m_pfMonitor;

    // Status bar timer
    UINT_PTR m_uiStatusTimer;
    long_t m_lStatusTime;

    // Misc
    void SetStatusbarInfo(CString strText = _T(""), int_t nPane = 0, BOOL bAutoClear = FALSE);
    CFont *GetStatusbarFont(void);
    void GetPaneInfo(int_t nIndex, UINT &nID, UINT &nStyle, int_t &cxWidth);
    void SetPaneInfo(int_t nIndex, UINT nID, UINT nStyle, int_t cxWidth);

    // Console window
    CSigmaConsole* m_pwndConsole;

    // Script window
    CScriptWindow* m_pwndScript;

    // Output window
    CSigmaOutput* m_pwndOutput;

    CSigmaDoc* FindDoc(CString strName);
    CSigmaDoc* FindDoc(int_t iID);

// Attributes
public:
    CSigmaToolBar m_wndToolbarData;
    CSigmaToolBar m_wndToolbarPlot;

// Operations
public:

    HICON GetIconForItem(UINT itemID) const;

    void cleanup();

    void OutputCreate(void);
    void OutputView(bool bShow = true);

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void WinHelp(DWORD dwData, UINT nCmd);

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    void DockControlBarRightOf(CToolBar* pBar, CToolBar* pLeftBar);

protected:  // control bar embedded members
    CSigmaToolBar m_wndToolbar;
    CStatusBar m_wndStatusbar;

// Generated message map functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnViewOutput();
    afx_msg void OnUpdateViewOutput(CCmdUI *pCmdUI);
    afx_msg void OnViewConsole();
    afx_msg void OnUpdateViewConsole(CCmdUI *pCmdUI);
    afx_msg void OnViewScript();
    afx_msg void OnUpdateViewScript(CCmdUI *pCmdUI);
    afx_msg void OnToolbar();
    afx_msg void OnUpdateToolbar(CCmdUI *pCmdUI);
    afx_msg void OnToolbarData();
    afx_msg void OnUpdateToolbarData(CCmdUI *pCmdUI);
    afx_msg void OnToolbarPlot();
    afx_msg void OnUpdateToolbarPlot(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePaneAA(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePaneBB(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePaneAngle(CCmdUI *pCmdUI);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
    afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);
    afx_msg void OnViewToolbar();
    afx_msg void OnUpdateViewToolbar(CCmdUI *pCmdUI);
    afx_msg void OnViewToolbarData();
    afx_msg void OnUpdateViewToolbarData(CCmdUI *pCmdUI);
    afx_msg void OnViewToolbarPlot();
    afx_msg void OnUpdateViewToolbarPlot(CCmdUI *pCmdUI);
    afx_msg void OnViewStatusbar();
    afx_msg void OnUpdateViewStatusbar(CCmdUI *pCmdUI);
    afx_msg BOOL OnQueryEndSession();
    afx_msg void OnClose();
    virtual void ActivateFrame(int nCmdShow = -1);
};

