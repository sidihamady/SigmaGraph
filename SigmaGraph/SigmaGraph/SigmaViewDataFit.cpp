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
#include "SigmaViewDataFit.h"
#include "ChildFrmDataFit.h"
#include "FitterReport.h"

// CSigmaViewDataFit

IMPLEMENT_DYNCREATE(CSigmaViewDataFit, CView)

CSigmaViewDataFit::CSigmaViewDataFit()
{

}

CSigmaViewDataFit::~CSigmaViewDataFit()
{

}

BEGIN_MESSAGE_MAP(CSigmaViewDataFit, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_COMMAND(ID_VIEW_PLOT, &CSigmaViewDataFit::OnViewPlot)
    ON_COMMAND(ID_VIEW_DATA, &CSigmaViewDataFit::OnViewData)
    ON_COMMAND(ID_VIEW_FITREPORT, &CSigmaViewDataFit::OnViewFitReport)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PLOT, &CSigmaViewDataFit::OnUpdateViewPlot)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DATA, &CSigmaViewDataFit::OnUpdateViewData)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FITREPORT, &CSigmaViewDataFit::OnUpdateViewFitReport)
END_MESSAGE_MAP()


// CSigmaViewDataFit drawing

void CSigmaViewDataFit::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
}


// CSigmaViewDataFit diagnostics

#ifdef _DEBUG
void CSigmaViewDataFit::AssertValid() const
{
    CView::AssertValid();
}

void CSigmaViewDataFit::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CSigmaDoc* CSigmaViewDataFit::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSigmaDoc)));
    return (CSigmaDoc*)m_pDocument;
}
#endif //_DEBUG


// CSigmaViewDataFit message handlers

int_t CSigmaViewDataFit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    return 0;
}

void CSigmaViewDataFit::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    int_t nWidth = 0, nHeight = 0, row, col;
    real_t fItem = 0.0;
    char_t szBuffer[ML_STRSIZE];

    _tcsnset(szBuffer, 0, ML_STRSIZE);

    // Create the Datasheet
    CRect rcT;
    GetClientRect(rcT);
    m_Datasheet.Create(rcT, this, ML_STRSIZE - 1);

    m_Datasheet.m_nGroup = 1;

    CSigmaDoc *pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    m_Datasheet.SetEditable(FALSE);

    try {
        m_Datasheet.SetRowCount(1 + pDoc->GetColumnDim(m_Datasheet.m_nGroup, 0,0));
        m_Datasheet.SetColumnCount(1 + pDoc->GetColumnCount(m_Datasheet.m_nGroup));
        m_Datasheet.SetFixedRowCount(1);
        m_Datasheet.SetFixedColumnCount(1);
    }
    catch (CMemoryException* excT) {
        excT->ReportError();
        excT->Delete();
        return;
    }

    CSize sizeTT = m_Datasheet.GetTextExtent(1, 0, _T("1234"));
    if (sizeTT.cx > 10) {
        m_Datasheet.SetColumnWidth(0, sizeTT.cx);
    }
    for (int_t nCol = 1; nCol < m_Datasheet.GetColumnCount(); nCol++) {
        sizeTT = m_Datasheet.GetTextExtent(1, nCol, _T("123456.123456"));
        if (sizeTT.cx > 10) {
            m_Datasheet.SetColumnWidth(nCol, sizeTT.cx);
        }
    }

    for (int_t uiRow = 0; uiRow < (int_t) (m_Datasheet.GetRowCount()); uiRow++) {
        sizeTT = m_Datasheet.GetTextExtent(uiRow, 0, _T("123456.123456"));
        if (sizeTT.cy > 10) {
            m_Datasheet.SetRowHeight(uiRow, 5+sizeTT.cy);
        }
    }

    // Set initial window position

    int_t nRowCount = m_Datasheet.GetRowCount();
    if (nRowCount > 14) {
        nRowCount = 14;
    }
    int_t nColCount = m_Datasheet.GetColumnCount();
    if (nColCount > 5) {
        nColCount = 5;
    }

    nWidth = 0;    nHeight = 0;
    for (row = 0; row < nRowCount; row++) {
        nHeight += m_Datasheet.GetRowHeight(row);
    }
    for (col = 0; col < nColCount; col++) { 
        nWidth += m_Datasheet.GetColumnWidth(col);
    }

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        return;
    }
    CWnd *pWnd = GetParent();
    if (pWnd == NULL) {
        pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);
        return;
    }

    CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
    WINDOWPLACEMENT mdiPlace;
    memset(&mdiPlace, 0, sizeof(WINDOWPLACEMENT));
    mdiPlace.length = sizeof(WINDOWPLACEMENT);
    pMainFrame->GetWindowPlacement(&mdiPlace);
    LONG iX0 = 0L, iY0 = 0L, iW = mdiPlace.rcNormalPosition.right - mdiPlace.rcNormalPosition.left,
        iH = mdiPlace.rcNormalPosition.bottom - mdiPlace.rcNormalPosition.top;

    if ((pDoc->m_PlacementDatasheetFit.rcNormalPosition.top < iY0) || (pDoc->m_PlacementDatasheetFit.rcNormalPosition.left < iX0)
        || (pDoc->m_PlacementDatasheetFit.rcNormalPosition.bottom > (iY0 + iH)) || (pDoc->m_PlacementDatasheetFit.rcNormalPosition.right > (iX0 + iW))
        || (pDoc->m_PlacementDatasheetFit.rcNormalPosition.right < (pDoc->m_PlacementDatasheetFit.rcNormalPosition.left + SIGMA_MINWIDTH3))
        || (pDoc->m_PlacementDatasheetFit.rcNormalPosition.bottom < (pDoc->m_PlacementDatasheetFit.rcNormalPosition.top + SIGMA_MINHEIGHT3))) {

        LONG iWm = SIGMA_DEFWIDTH_DATA;
        LONG iHm = SIGMA_DEFHEIGHT_DATA;
        LONG iWD = m_Datasheet.GetColumnWidth(0) + ((m_Datasheet.GetColumnCount() - 1) * m_Datasheet.GetColumnWidth(1));
        LONG iHD = m_Datasheet.GetRowHeight(0) + (m_Datasheet.GetRowCount() * m_Datasheet.GetRowHeight(1))
            + (LONG) ::GetSystemMetrics(SM_CYCAPTION);
        if (iWD > SIGMA_MINWIDTH3) {
            iWm = __min(iWD, SIGMA_DEFWIDTH_DATA);
        }
        if (iHD > SIGMA_MINHEIGHT3) {
            iHm = __min(iHD, SIGMA_DEFHEIGHT_DATA);
        }
        pDoc->m_PlacementDatasheetFit.length = sizeof(WINDOWPLACEMENT);
        pDoc->m_PlacementDatasheetFit.rcNormalPosition.left = iX0;
        pDoc->m_PlacementDatasheetFit.rcNormalPosition.top = iY0;
        pDoc->m_PlacementDatasheetFit.rcNormalPosition.right = iX0 + iWm;
        pDoc->m_PlacementDatasheetFit.rcNormalPosition.bottom = iY0 + iHm;
    }

    pWnd->SetWindowPlacement(&(pDoc->m_PlacementDatasheetFit));

    CString strT = pDoc->GetTitle();
    strT.TrimRight(_T("*"));
    strT.Append(_T(" - Datasheet (Fit)"));
    pWnd->SetWindowText((LPCTSTR)strT);

    ::SendMessage(pWnd->m_hWnd, WM_PAINT, 0, 0);

    ((CChildFrameDataFit*)pWnd)->m_bSetModified = TRUE;
}

BOOL CSigmaViewDataFit::OnPreparePrinting(CPrintInfo* pInfo)
{
    // TODO:  call DoPreparePrinting to invoke the Print dialog box

    return DoPreparePrinting(pInfo);
}

void CSigmaViewDataFit::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
    m_Datasheet.OnPrint(pDC, pInfo);
}

void CSigmaViewDataFit::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
    m_Datasheet.OnBeginPrinting(pDC, pInfo);
}

void CSigmaViewDataFit::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
    m_Datasheet.OnEndPrinting(pDC, pInfo);
}

void CSigmaViewDataFit::OnSize(UINT nType, int_t cx, int_t cy)
{
    CView::OnSize(nType, cx, cy);

    if (m_Datasheet.GetSafeHwnd()) {
        CRect rect;
        GetClientRect(rect);
        m_Datasheet.MoveWindow(rect);
    }
}

BOOL CSigmaViewDataFit::OnCmdMsg(UINT nID, int_t nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (m_Datasheet && ::IsWindow(m_Datasheet.m_hWnd)) {
        if (m_Datasheet.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
            return TRUE;
        }
    }

    return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CSigmaViewDataFit::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

BOOL CSigmaViewDataFit::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CSigmaViewDataFit::OnViewPlot()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    if (pFrame->CreateSigmaView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot), TRUE) == FALSE) {
        pFrame->CreateSigmaView(SigmaApp.m_pTemplateDataFit, RUNTIME_CLASS(CSigmaViewDataFit), TRUE);
    }
}


void CSigmaViewDataFit::OnViewData()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    if (pFrame->CreateSigmaView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData), TRUE) == FALSE) {
        CWaitCursor waitT;
        pFrame->CreateSigmaView(SigmaApp.m_pTemplateDataFit, RUNTIME_CLASS(CSigmaViewDataFit), TRUE);
    }
}

void CSigmaViewDataFit::OnUpdateViewPlot(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    pCmdUI->Enable(pDoc->GetColumnCount(1) > 0);
}

void CSigmaViewDataFit::OnUpdateViewData(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    pCmdUI->Enable(pDoc->GetColumnCount(0) > 0);
}

void CSigmaViewDataFit::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
    CSigmaDoc *pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    int_t nRowCount = 1 + pDoc->GetColumnDim(m_Datasheet.m_nGroup, 0, 0),
        nRowCountPrev = m_Datasheet.GetRowCount();
    int_t nColumnCount = 1 + pDoc->GetColumnCount(m_Datasheet.m_nGroup),
        nColumnCountPrev = m_Datasheet.GetColumnCount();

    m_Datasheet.SetRowCount(nRowCount);
    m_Datasheet.SetColumnCount(nColumnCount);

    int_t iHeight = m_Datasheet.GetRowHeight(1);
    if ((iHeight > 0) && (nRowCount > nRowCountPrev)) {
        for (int_t uiRow = nRowCountPrev; uiRow < nRowCount; uiRow++) {
            m_Datasheet.SetRowHeight(uiRow, iHeight);
        }
    }
    int_t iWidth = m_Datasheet.GetColumnWidth(1);
    if ((iWidth > 0) && (nColumnCount > nColumnCountPrev)) {
        for (int_t uiCol = nColumnCountPrev; uiCol < nColumnCount; uiCol++) {
            m_Datasheet.SetColumnWidth(uiCol, iWidth);
        }
    }

    if ((m_Datasheet.m_nSelectedColumn >= 1) && (m_Datasheet.m_nSelectedColumn < nColumnCount)) {
        CCellRange cellRange = m_Datasheet.GetSelectedCellRange();
        m_Datasheet.ResetSelectedRange();
        cellRange.SetMinCol(m_Datasheet.m_nSelectedColumn);
        cellRange.SetMaxCol(m_Datasheet.m_nSelectedColumn);
        m_Datasheet.SetSelectedRange(cellRange);
    }
    m_Datasheet.Refresh();

    CWnd *pWnd = GetParent();
    if (pWnd == NULL) {
        pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);
        return;
    }
    CString strT = pDoc->GetTitle();
    strT.TrimRight(_T("*"));
    strT.Append(_T(" - Datasheet (Fit)"));
    pWnd->SetWindowText((LPCTSTR)strT);
}

void CSigmaViewDataFit::OnViewFitReport()
{
    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }
    plot_t *pPlot = pDoc->GetPlot();
    if (pPlot == NULL) {
        return;
    }
    if (pPlot->curvecount < 1) {
        return;
    }
    if ((pPlot->curveactive < 0) || (pPlot->curveactive >= pPlot->curvecount)) {
        return;
    }

    CFitterReport reportDlg;
    fit_t *pFit = pDoc->GetFit(-1, pPlot->curve[pPlot->curveactive].y->idf);
    if (pFit == NULL) {
        for (int ii = 0; ii < pPlot->curvecount; ii++) {
            pFit = pDoc->GetFit(-1, pPlot->curve[ii].y->idf);
            if (pFit != NULL) {
                if (pFit->parcount < 2) {
                    return;
                }
                pPlot->curveactive = ii;
                break;
            }
        }
    }
    reportDlg.m_pFit = pFit;
    reportDlg.m_pDoc = pDoc;
    reportDlg.m_pPlot = pPlot;

    reportDlg.DoModal();
}

void CSigmaViewDataFit::OnUpdateViewFitReport(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }
    plot_t *pPlot = pDoc->GetPlot();
    if (pPlot == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    if (pPlot->curvecount < 1) {
        pCmdUI->Enable(FALSE);
        return;
    }

    if (pDoc->GetColumnCount(1) < 1) {
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
}