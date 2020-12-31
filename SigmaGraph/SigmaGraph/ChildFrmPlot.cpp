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

#include "stdafx.h"
#include "Sigma.h"
#include "MainFrm.h"
#include "ChildFrmPlot.h"
#include "ChildFrmData.h"
#include "ChildFrmDataFit.h"
#include "SigmaDoc.h"

#include "SigmaViewPlot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFramePlot

IMPLEMENT_DYNCREATE(CChildFramePlot, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFramePlot, CMDIChildWnd)
    ON_WM_CREATE()
    ON_WM_GETMINMAXINFO()
    ON_WM_NCRBUTTONDOWN()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_WM_MOVE()
END_MESSAGE_MAP()

// CChildFramePlot construction/destruction

CChildFramePlot::CChildFramePlot()
{
    m_bClose = FALSE;
    m_bInitialized = FALSE;
}

CChildFramePlot::~CChildFramePlot()
{

}

BOOL CChildFramePlot::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
    if (!CMDIChildWnd::PreCreateWindow(cs)) {
        return FALSE;
    }

    cs.style &= ~(LONG)FWS_ADDTOTITLE;

    return TRUE;
}

int CChildFramePlot::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    m_bInitialized = FALSE;

    return 0;
}


// CChildFramePlot diagnostics

#ifdef _DEBUG
void CChildFramePlot::AssertValid() const
{
    CMDIChildWnd::AssertValid();
}

void CChildFramePlot::Dump(CDumpContext& dc) const
{
    CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

// CChildFramePlot message handlers
void CChildFramePlot::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    if (m_bInitialized == FALSE) {
        lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH0;
        lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT0;
        m_bInitialized = TRUE;
    }
    else {
        lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH1;
        lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT1;
    }

    CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CChildFramePlot::OnClose()
{
    CSigmaDoc* pDoc = (CSigmaDoc*)GetActiveDocument();
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    int_t iT = 0;

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        return;
    }

    if (m_bClose == FALSE) {
        CView* pView = pDoc->GetDataView();
        if (pView == NULL) {
            pDoc->OnCloseDocument();
            return;
        }
        if (pView->IsWindowVisible() == FALSE){
            pDoc->OnCloseDocument();
            return;
        }

        pFrame->CreateSigmaView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot), FALSE);
        pFrame->CreateSigmaView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData), TRUE);
        return;
    }
    else {
        pDoc->SetModifiedFlag(FALSE);
        CMDIChildWnd::OnClose();
        return;
    }

    CMDIChildWnd::OnClose();
}

void CChildFramePlot::OnNcRButtonDown(UINT nHitTest, CPoint tPoint)
{
    // display ad hoc popmenu
    CMenu contextMenu; 
    CMenu *pMenu = NULL;
    contextMenu.LoadMenu(IDM_PLOTPOP);
    pMenu = contextMenu.GetSubMenu(0);
    // <<

    if (pMenu != NULL) {
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, this, NULL);
    }

    CMDIChildWnd::OnNcRButtonDown(nHitTest, tPoint);
}

void CChildFramePlot::ActivateFrame(int nCmdShow)
{
    CMDIChildWnd::ActivateFrame(nCmdShow);
}

void CChildFramePlot::OnSize(UINT nType, int cx, int cy)
{
    CMDIChildWnd::OnSize(nType, cx, cy);

    CSigmaDoc* pDoc = (CSigmaDoc*)GetActiveDocument();
    if (pDoc == NULL) {
        return;
    }
    pDoc->SetModifiedFlag(TRUE);
}

void CChildFramePlot::OnMove(int x, int y)
{
    CMDIChildWnd::OnMove(x, y);

    CSigmaDoc* pDoc = (CSigmaDoc*)GetActiveDocument();
    if (pDoc == NULL) {
        return;
    }
    pDoc->SetModifiedFlag(TRUE);
}
