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
#include "SigmaViewData.h"
#include "FitterReport.h"

// CSigmaViewData

IMPLEMENT_DYNCREATE(CSigmaViewData, CView)

CSigmaViewData::CSigmaViewData()
{

}

CSigmaViewData::~CSigmaViewData()
{

}

BEGIN_MESSAGE_MAP(CSigmaViewData, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_COMMAND(ID_VIEW_PLOT, &CSigmaViewData::OnViewPlot)
    ON_COMMAND(ID_VIEW_DATAFIT, &CSigmaViewData::OnViewDataFit)
    ON_COMMAND(ID_VIEW_FITREPORT, &CSigmaViewData::OnViewFitReport)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PLOT, &CSigmaViewData::OnUpdateViewPlot)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DATAFIT, &CSigmaViewData::OnUpdateViewFit)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FITREPORT, &CSigmaViewData::OnUpdateViewFitReport)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSigmaViewData drawing

void CSigmaViewData::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: add draw code here
}


// CSigmaViewData diagnostics

#ifdef _DEBUG
void CSigmaViewData::AssertValid() const
{
    CView::AssertValid();
}

void CSigmaViewData::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CSigmaDoc* CSigmaViewData::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSigmaDoc)));
    return (CSigmaDoc*)m_pDocument;
}
#endif //_DEBUG


// CSigmaViewData message handlers

int_t CSigmaViewData::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    // TODO:  Add your specialized creation code here

    return 0;
}

void CSigmaViewData::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    CWaitCursor wcT;

    int_t nWidth = 0, nHeight = 0, row, col;
    real_t fItem = 0.0;
    char_t szBuffer[ML_STRSIZE];

    _tcsnset(szBuffer, 0, ML_STRSIZE);

    // Create the Datasheet
    CRect rcT;
    GetClientRect(rcT);

    m_Datasheet.Create(rcT, this, ML_STRSIZE - 1);

    m_Datasheet.m_nGroup = 0;

    CSigmaDoc *pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    BOOL bIsModified = pDoc->IsModified();

    m_Datasheet.SetEditable(TRUE);

    try {
        m_Datasheet.SetRowCount(1 + pDoc->GetColumnDim(m_Datasheet.m_nGroup, 0, 0));
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
            m_Datasheet.SetColumnWidth(nCol, sizeTT.cx, false);
        }
    }

    for (int_t uiRow = 0; uiRow < (int_t) (m_Datasheet.GetRowCount()); uiRow++) {
        sizeTT = m_Datasheet.GetTextExtent(uiRow, 0, _T("123456.123456"));
        if (sizeTT.cy > 10) {
            m_Datasheet.SetRowHeight(uiRow, 5 + sizeTT.cy, false);
        }
    }

    m_Datasheet.ResetScrollBars();

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

    WINDOWPLACEMENT mdiPlace;
    memset(&mdiPlace, 0, sizeof(WINDOWPLACEMENT));
    mdiPlace.length = sizeof(WINDOWPLACEMENT);
    pFrame->GetWindowPlacement(&mdiPlace);
    LONG iX0 = 0, iY0 = 0, iW = mdiPlace.rcNormalPosition.right - mdiPlace.rcNormalPosition.left,
        iH = mdiPlace.rcNormalPosition.bottom - mdiPlace.rcNormalPosition.top;

    if ((pDoc->m_PlacementDatasheet.rcNormalPosition.top < iY0) || (pDoc->m_PlacementDatasheet.rcNormalPosition.left < iX0)
        || (pDoc->m_PlacementDatasheet.rcNormalPosition.bottom > (iY0 + iH)) || (pDoc->m_PlacementDatasheet.rcNormalPosition.right > (iX0 + iW))
        || (pDoc->m_PlacementDatasheet.rcNormalPosition.right < (pDoc->m_PlacementDatasheet.rcNormalPosition.left + SIGMA_MINWIDTH3))
        || (pDoc->m_PlacementDatasheet.rcNormalPosition.bottom < (pDoc->m_PlacementDatasheet.rcNormalPosition.top + SIGMA_MINHEIGHT3))) {

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
        pDoc->m_PlacementDatasheet.length = sizeof(WINDOWPLACEMENT);
        pDoc->m_PlacementDatasheet.rcNormalPosition.left = iX0;
        pDoc->m_PlacementDatasheet.rcNormalPosition.top = iY0;
        pDoc->m_PlacementDatasheet.rcNormalPosition.right = iX0 + iWm;
        pDoc->m_PlacementDatasheet.rcNormalPosition.bottom = iY0 + iHm;
    }

    const LONG posmax = 160;
    LONG ix1 = SigmaApp.m_LastPlacement.rcNormalPosition.left,
        iy1 = SigmaApp.m_LastPlacement.rcNormalPosition.top,
        ix2 = SigmaApp.m_LastPlacement.rcNormalPosition.right,
        iy2 = SigmaApp.m_LastPlacement.rcNormalPosition.bottom;
    if ((ix2 > 0) && (iy2 > 0) && (ix1 < posmax) && (iy1 < posmax)) {
        pDoc->m_PlacementDatasheet.rcNormalPosition.left = ix1 + 32;
        pDoc->m_PlacementDatasheet.rcNormalPosition.top = iy1 + 32;
        pDoc->m_PlacementDatasheet.rcNormalPosition.right = ix2 + 32;
        pDoc->m_PlacementDatasheet.rcNormalPosition.bottom = iy2 + 32;
    }
    memcpy(&(SigmaApp.m_LastPlacement), &(pDoc->m_PlacementDatasheet), sizeof(WINDOWPLACEMENT));
    if ((SigmaApp.m_LastPlacement.rcNormalPosition.left > posmax) || (SigmaApp.m_LastPlacement.rcNormalPosition.top > posmax)) {
        SigmaApp.m_LastPlacement.rcNormalPosition.right = SigmaApp.m_LastPlacement.rcNormalPosition.right - SigmaApp.m_LastPlacement.rcNormalPosition.left;
        SigmaApp.m_LastPlacement.rcNormalPosition.bottom = SigmaApp.m_LastPlacement.rcNormalPosition.bottom - SigmaApp.m_LastPlacement.rcNormalPosition.top;
        SigmaApp.m_LastPlacement.rcNormalPosition.left = 0;
        SigmaApp.m_LastPlacement.rcNormalPosition.top = 0;
    }

    pWnd->SetWindowPlacement(&(pDoc->m_PlacementDatasheet));

    CString strT = pDoc->GetTitle();
    strT.TrimRight(_T("*"));
    strT.Append(_T(" - Datasheet"));
    pWnd->SetWindowText((LPCTSTR)strT);

    ::SendMessage(GetSafeHwnd(), WM_PAINT, 0, 0);

    pDoc->SetModifiedFlag(bIsModified);

}

BOOL CSigmaViewData::OnPreparePrinting(CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void CSigmaViewData::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
    m_Datasheet.OnPrint(pDC, pInfo);
}

void CSigmaViewData::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
    m_Datasheet.OnBeginPrinting(pDC, pInfo);
}

void CSigmaViewData::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
    m_Datasheet.OnEndPrinting(pDC, pInfo);
}

void CSigmaViewData::OnSize(UINT nType, int_t cx, int_t cy)
{
    CView::OnSize(nType, cx, cy);

    if (m_Datasheet.GetSafeHwnd()) {
        m_Datasheet.EndEdit();
        CRect rcT;
        GetClientRect(rcT);
        m_Datasheet.MoveWindow(rcT);
    }
}

BOOL CSigmaViewData::OnCmdMsg(UINT nID, int_t nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (::IsWindow(m_Datasheet.m_hWnd)) {
        if (m_Datasheet.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
            return TRUE;
        }
    }

    return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CSigmaViewData::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

BOOL CSigmaViewData::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

void CSigmaViewData::OnClose()
{
    m_Datasheet.EndEdit();

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }
    pDoc->OnCloseDocument();
}

void CSigmaViewData::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
    CSigmaDoc *pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    int_t nRowCount = 1 + pDoc->GetColumnDim(m_Datasheet.m_nGroup, 0, 0), nRowCountPrev = m_Datasheet.GetRowCount();
    int_t nColumnCount = 1 + pDoc->GetColumnCount(m_Datasheet.m_nGroup), nColumnCountPrev = m_Datasheet.GetColumnCount();

    // Si des colonnes ont été supprimées
    if (nColumnCount < nColumnCountPrev) {
        for (int_t uiCol = nColumnCount; uiCol < nColumnCountPrev; uiCol++) {
            m_Datasheet.DeleteColumn(uiCol);
        }
    }

    m_Datasheet.SetRowCount(nRowCount);
    m_Datasheet.SetColumnCount(nColumnCount);

    // Si des lignes ont été ajoutées
    int_t iHeight = m_Datasheet.GetRowHeight(1);
    if ((iHeight > 0) && (nRowCount > nRowCountPrev)) {
        for (int_t uiRow = nRowCountPrev; uiRow < nRowCount; uiRow++) {
            m_Datasheet.SetRowHeight(uiRow, iHeight);
        }
    }

    // Si des colonnes ont été ajoutées
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
        CCellID cellID;
        cellID.col = m_Datasheet.m_nSelectedColumn;
        cellID.row = 0;
        m_Datasheet.SetFocusCell(cellID);
    }
    m_Datasheet.Refresh();

    CWnd *pWnd = GetParent();
    if (pWnd == NULL) {
        pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);
        return;
    }
    CString strT = pDoc->GetTitle();
    strT.TrimRight(_T("*"));
    strT.Append(_T(" - Datasheet"));
    pWnd->SetWindowText((LPCTSTR)strT);
}

void CSigmaViewData::OnViewPlot()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    if (pFrame->CreateSigmaView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot), TRUE) == FALSE) {
        pFrame->CreateSigmaView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData), TRUE);
    }
}

void CSigmaViewData::OnViewDataFit()
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        return;
    }

    if (pDoc->GetColumnCount(1) > 0) {
        CWaitCursor waitT;
        pFrame->CreateSigmaView(SigmaApp.m_pTemplateDataFit, RUNTIME_CLASS(CSigmaViewDataFit), TRUE);
    }
}

void CSigmaViewData::OnViewFitReport()
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

    CFitterReport reportDlg;

    reportDlg.m_pFit = pFit;
    reportDlg.m_pDoc = pDoc;
    reportDlg.m_pPlot = pPlot;

    reportDlg.DoModal();
}

void CSigmaViewData::OnUpdateViewPlot(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(pDoc->GetColumnCount(0) > 0);
}

void CSigmaViewData::OnUpdateViewFit(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*) (GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(pDoc->canViewFit());
}

void CSigmaViewData::OnUpdateViewFitReport(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*) (GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(pDoc->canViewFit());
}
