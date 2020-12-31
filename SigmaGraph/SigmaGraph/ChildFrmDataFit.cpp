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
#include "ChildFrmDataFit.h"
#include "SigmaDoc.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrameDataFit

IMPLEMENT_DYNCREATE(CChildFrameDataFit, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrameDataFit, CMDIChildWnd)
    ON_WM_GETMINMAXINFO()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_WM_MOVE()
END_MESSAGE_MAP()

// CChildFrameDataFit construction/destruction

CChildFrameDataFit::CChildFrameDataFit()
{
    m_bSetModified = FALSE;
    m_bClose = FALSE;
}

CChildFrameDataFit::~CChildFrameDataFit()
{

}


BOOL CChildFrameDataFit::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
    if (!CMDIChildWnd::PreCreateWindow(cs)) {
        return FALSE;
    }

    cs.style &= ~(LONG)FWS_ADDTOTITLE;

    return TRUE;
}

// CChildFrameDataFit diagnostics

#ifdef _DEBUG
void CChildFrameDataFit::AssertValid() const
{
    CMDIChildWnd::AssertValid();
}

void CChildFrameDataFit::Dump(CDumpContext& dc) const
{
    CMDIChildWnd::Dump(dc);
}
#endif //_DEBUG

// CChildFrameDataFit message handlers
void CChildFrameDataFit::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH1;
    lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT1;

    CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CChildFrameDataFit::OnClose()
{
    CSigmaDoc* pDoc = (CSigmaDoc*)GetActiveDocument();
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    if (m_bClose == FALSE) {
        CView* pViewD = pDoc->GetDataView();
        CView* pViewP = pDoc->GetPlotView();
        if ((pViewD == NULL) && (pViewP == NULL)) {
            pDoc->OnCloseDocument();
            return;
        }
        
        BOOL bVisible = FALSE;

        if ((bVisible == FALSE) && pViewD) {
            if (pViewD->IsWindowVisible() == TRUE) {
                bVisible = TRUE;
            }
        }
        if ((bVisible == FALSE) && pViewP) {
            if (pViewP->IsWindowVisible() == TRUE) {
                bVisible = TRUE;
            }
        }

        if (bVisible == FALSE) {
            pDoc->OnCloseDocument();
            return;
        }

        CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
        ASSERT_VALID(pFrame);
        if (pFrame == NULL) {
            return;
        }
        pFrame->CreateSigmaView(SigmaApp.m_pTemplateDataFit, RUNTIME_CLASS(CSigmaViewDataFit), FALSE);
        pFrame->CreateSigmaView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot), TRUE);
        return;
    }
    else {
        pDoc->SetModifiedFlag(FALSE);
        CMDIChildWnd::OnClose();
        return;
    }

    CMDIChildWnd::OnClose();
}

void CChildFrameDataFit::OnSize(UINT nType, int cx, int cy)
{
    CMDIChildWnd::OnSize(nType, cx, cy);

    if (m_bSetModified) {
        CSigmaDoc* pDoc = (CSigmaDoc*)GetActiveDocument();
        if (pDoc == NULL) {
            return;
        }
        pDoc->SetModifiedFlag(TRUE);
    }
}

void CChildFrameDataFit::OnMove(int x, int y)
{
    CMDIChildWnd::OnMove(x, y);

    CSigmaDoc* pDoc = (CSigmaDoc*)GetActiveDocument();
    if (pDoc == NULL) {
        return;
    }
    pDoc->SetModifiedFlag(TRUE);
}
