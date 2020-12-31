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

#include "SigmaDoc.h"
#include "SigmaViewPlot.h"
#include "SigmaNote.h"
#include "MainFrm.h"
#include "TextDlg.h"
#include "GeometricLineDlg.h"
#include "GeometricRectDlg.h"
#include "FitterReport.h"
#include "ErrorBarDlg.h"
#include "TemplatesDlg.h"
#include "Plotopt.h"
#include "SigmaDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DELTA_MIN            4
#define FRAME_MARGIN        4L

// CSigmaViewPlot

IMPLEMENT_DYNCREATE(CSigmaViewPlot, CView)

BEGIN_MESSAGE_MAP(CSigmaViewPlot, CView)
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONUP()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
    ON_WM_ERASEBKGND()
    ON_WM_SIZING()
    ON_WM_SETCURSOR()
    ON_WM_CANCELMODE()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_INITMENUPOPUP()
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
    ON_COMMAND(ID_PLOT_OPTIONS, &CSigmaViewPlot::OnPlotOptions)
    ON_COMMAND(ID_PLOT_AUTOSCALE, &CSigmaViewPlot::OnPlotAutoScale)
    ON_COMMAND(ID_PLOT_ZOOMIN, &CSigmaViewPlot::OnViewZoomIn)
    ON_COMMAND(ID_PLOT_ZOOMOUT, &CSigmaViewPlot::OnViewZoomOut)
    ON_COMMAND(ID_FITTER_NONLINEAR, &CSigmaViewPlot::OnFitterNonlinear)
    ON_COMMAND(ID_FITTER_LINEAR, &CSigmaViewPlot::OnFitterLinear)
    ON_COMMAND(ID_FITTER_POLYNOMIAL, &CSigmaViewPlot::OnFitterPolynomial)
    ON_COMMAND(ID_FITTER_GAUSSIAN, &CSigmaViewPlot::OnFitterGaussian)
    ON_COMMAND(ID_FITTER_LORENTZIAN, &CSigmaViewPlot::OnFitterLorentzian)
    ON_COMMAND(ID_FITTER_EXPONENTIAL, &CSigmaViewPlot::OnFitterExponetial)
    ON_COMMAND(ID_FITTER_LOGARITHMIC, &CSigmaViewPlot::OnFitterLogarithmic)
    ON_COMMAND(ID_FITTER_POWER, &CSigmaViewPlot::OnFitterPower)
    ON_COMMAND(ID_VIEW_DATA, &CSigmaViewPlot::OnViewData)
    ON_COMMAND(ID_VIEW_DATAFIT, &CSigmaViewPlot::OnViewDataFit)
    ON_COMMAND(ID_VIEW_FITREPORT, &CSigmaViewPlot::OnViewFitReport)
    ON_COMMAND(ID_PLOT_ADDTEXT, &CSigmaViewPlot::OnAddText)
    ON_COMMAND(ID_PLOT_ADDLINE, &CSigmaViewPlot::OnAddLine)
    ON_COMMAND(ID_PLOT_ADDRECT, &CSigmaViewPlot::OnAddRect)
    ON_COMMAND(ID_PLOT_ADDELLIPSE, &CSigmaViewPlot::OnAddEllipse)
    ON_COMMAND(ID_PLOT_ADDVLINE, &CSigmaViewPlot::OnAddVertLine)
    ON_COMMAND(ID_PLOT_ADDHLINE, &CSigmaViewPlot::OnAddHorzLine)
    ON_COMMAND(ID_FILE_EXPORTG, &CSigmaViewPlot::OnFileExport)
    ON_COMMAND(ID_ITEM_REMOVE, &CSigmaViewPlot::OnItemRemove)
    ON_COMMAND(ID_ITEM_COPY, &CSigmaViewPlot::OnItemCopy)
    ON_COMMAND(ID_ITEM_PASTE, &CSigmaViewPlot::OnItemPaste)
    ON_COMMAND(ID_CURVE_ACTIVE, &CSigmaViewPlot::OnCurveActive)
    ON_COMMAND(ID_CURVE_HIDE, &CSigmaViewPlot::OnCurveHide)
    ON_COMMAND(ID_CURVE_REMOVE, &CSigmaViewPlot::OnCurveRemove)
    ON_COMMAND(ID_CURVE_PROPERTIES, &CSigmaViewPlot::OnCurveProperties)
    ON_COMMAND(ID_GRAPH_ERRORBAR, &CSigmaViewPlot::OnErrorBar)
    ON_COMMAND(ID_EDIT_UNDO, &CSigmaViewPlot::OnEditUndo)
    ON_COMMAND(ID_EDIT_REDO, &CSigmaViewPlot::OnEditRedo)
    ON_COMMAND(ID_PLOT_TEMPLATES, &CSigmaViewPlot::OnPlotTemplates)
    ON_COMMAND(ID_PLOT_MASKFROM, &CSigmaViewPlot::OnPlotMaskFrom)
    ON_COMMAND(ID_PLOT_MASKTO, &CSigmaViewPlot::OnPlotMaskTo)
    ON_COMMAND(ID_PLOT_UNMASK, &CSigmaViewPlot::OnPlotUnmask)
    ON_UPDATE_COMMAND_UI(ID_PLOT_ZOOMIN, &CSigmaViewPlot::OnUpdateViewZoomIn)
    ON_UPDATE_COMMAND_UI(ID_PLOT_ZOOMOUT, &CSigmaViewPlot::OnUpdatePlotZoomout)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DATA, &CSigmaViewPlot::OnUpdateViewData)
    ON_UPDATE_COMMAND_UI(ID_VIEW_DATAFIT, &CSigmaViewPlot::OnUpdateViewFit)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FITREPORT, &CSigmaViewPlot::OnUpdateViewFitReport)
    ON_UPDATE_COMMAND_UI(ID_PLOT_AUTOSCALE, &CSigmaViewPlot::OnUpdatePlotAutoscale)
    ON_UPDATE_COMMAND_UI(ID_GRAPH_ERRORBAR, &CSigmaViewPlot::OnUpdateErrorBar)
    ON_UPDATE_COMMAND_UI(ID_FITTER_LINEAR, &CSigmaViewPlot::OnUpdateFitterLinear)
    ON_UPDATE_COMMAND_UI(ID_FITTER_GAUSSIAN, &CSigmaViewPlot::OnUpdateFitterGaussian)
    ON_UPDATE_COMMAND_UI(ID_FITTER_LORENTZIAN, &CSigmaViewPlot::OnUpdateFitterLorentzian)
    ON_UPDATE_COMMAND_UI(ID_FITTER_POLYNOMIAL, &CSigmaViewPlot::OnUpdateFitterPolynomial)
    ON_UPDATE_COMMAND_UI(ID_FITTER_EXPONENTIAL, &CSigmaViewPlot::OnUpdateFitterExponential)
    ON_UPDATE_COMMAND_UI(ID_FITTER_LOGARITHMIC, &CSigmaViewPlot::OnUpdateFitterLogarithmic)
    ON_UPDATE_COMMAND_UI(ID_FITTER_POWER, &CSigmaViewPlot::OnUpdateFitterPower)
    ON_UPDATE_COMMAND_UI(ID_FITTER_NONLINEAR, &CSigmaViewPlot::OnUpdateFitterNonlinear)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CSigmaViewPlot::OnUpdateEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CSigmaViewPlot::OnUpdateEditRedo)
    ON_UPDATE_COMMAND_UI(ID_PLOT_ADDTEXT, &CSigmaViewPlot::OnUpdatePlotAddtext)
    ON_UPDATE_COMMAND_UI(ID_PLOT_ADDLINE, &CSigmaViewPlot::OnUpdatePlotAddline)
    ON_UPDATE_COMMAND_UI(ID_PLOT_ADDRECT, &CSigmaViewPlot::OnUpdatePlotAddrect)
    ON_UPDATE_COMMAND_UI(ID_PLOT_ADDELLIPSE, &CSigmaViewPlot::OnUpdatePlotAddellipse)
END_MESSAGE_MAP()


void CSigmaViewPlot::Tic()
{
    m_ulTicToc = 0;
    m_liTicToc.HighPart = 0;
    m_liTicToc.LowPart = 0;
    m_liTicToc.QuadPart = 0;
    ::QueryPerformanceCounter(&m_liTicToc);

    return;
}
int_t CSigmaViewPlot::Toc()
{
    LARGE_INTEGER lstop, lfreq;
    real_t fT;

    if (m_liTicToc.QuadPart == 0) {
        m_liTicToc.HighPart = 0;
        m_liTicToc.LowPart = 0;
        m_ulTicToc = 0;
        return 0;
    }

    ::QueryPerformanceCounter(&lstop);
    ::QueryPerformanceFrequency(&lfreq);

    lstop.QuadPart -= m_liTicToc.QuadPart;
    lstop.QuadPart *= 1000000;
    lstop.QuadPart /= lfreq.QuadPart;

    if (lstop.HighPart != 0) {
        m_ulTicToc = 0;
    }
    else {
        m_ulTicToc = (ulong_t)(lstop.LowPart);
    }

    fT = (1e-3) * (real_t) (m_ulTicToc);

    return (int_t)ceil(fT);
}

BOOL CSigmaViewPlot::AxisLinkParse(real_t xx, real_t yy, real_t *flx, real_t *fly, char_t *szLx, char_t *szLy)
{
    if ((flx == NULL) && (fly == NULL)) {
        return FALSE;
    }
    if ((szLx == NULL) && (szLy == NULL)) {
        return FALSE;
    }
    if ((flx != NULL) && (szLx == NULL)) {
        return FALSE;
    }
    if ((fly != NULL) && (szLy == NULL)) {
        return FALSE;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return FALSE;
    }

    BOOL *pbRunning = &(pDoc->m_bRunning);
    if (pbRunning == NULL) {
        return FALSE;
    }

    if (*pbRunning == TRUE) {
        return FALSE;
    }

    real_t fval = 0.0, fbak;
    char_t szT[ML_STRSIZES];
    memset(szT, 0, ML_STRSIZES * sizeof(char_t));
    char_t szOutput[ML_STRSIZE];
    memset(szOutput, 0, ML_STRSIZE * sizeof(char_t));

    *pbRunning = TRUE;

    BOOL bOK = FALSE, bBak = FALSE;

    if (flx) {
        szLx[0] = _T('\0');

        // >>    Sauvergarder les variables du parser avant de lancer le calcul ...
        //        .. et définir les valeurs de x ou y avant d'évaluer l'expression
        int_t nLength = (int_t)_tcslen((const char_t*)(m_pPlot->linktop));
        if ((nLength > 2) && (nLength < ML_STRSIZEN)) {

            bBak = SigmaApp.DoParse(_T("x="), szOutput, &fbak);

            _tcsprintf(szT, ML_STRSIZES - 1, _T("x=%e"), xx);
            if (SigmaApp.DoParse((const char_t*)szT, szOutput)) {
                bOK = SigmaApp.DoParse((const char_t*)(m_pPlot->linktop), szLx, flx);
                if (bOK == FALSE) {
                    szLx[0] = _T('\0');
                }
            }

            if (bBak) {
                _tcsprintf(szT, ML_STRSIZES - 1, _T("x=%e"), fbak);
                SigmaApp.DoParse((const char_t*)szT, szOutput);
            }
        }
    }

    if (fly) {
        szLy[0] = _T('\0');

        // >>    Sauvergarder les variables du parser avant de lancer le calcul ...
        //        .. et définir les valeurs de x ou y avant d'évaluer l'expression
        int_t nLength = (int_t)_tcslen((const char_t*)(m_pPlot->linkright));
        if ((nLength > 2) && (nLength < ML_STRSIZEN)) {

            bBak = SigmaApp.DoParse(_T("y="), szOutput, &fbak);

            _tcsprintf(szT, ML_STRSIZES - 1, _T("y=%e"), yy);
            if (SigmaApp.DoParse((const char_t*)szT, szOutput)) {
                bOK = SigmaApp.DoParse((const char_t*)(m_pPlot->linkright), szLy, flx);
                if (bOK == FALSE) {
                    szLy[0] = _T('\0');
                }
            }

            if (bBak) {
                _tcsprintf(szT, ML_STRSIZES - 1, _T("y=%e"), fbak);
                SigmaApp.DoParse((const char_t*)szT, szOutput);
            }
        }
    }

    *pbRunning = FALSE;
    return TRUE;
}

// CSigmaViewPlot construction/destruction

void CSigmaViewPlot::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    CSigmaDoc *pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    CWnd *pWnd = this->GetOwner();
    if (pWnd == NULL) {
        pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);
        return;
    }

    int_t iTop = 0, iLeft = 0;

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);
        return;
    }

    WINDOWPLACEMENT mdiPlace;
    memset(&mdiPlace, 0, sizeof(WINDOWPLACEMENT));
    mdiPlace.length = sizeof(WINDOWPLACEMENT);
    pFrame->GetWindowPlacement(&mdiPlace);
    LONG iX0 = 0, iY0 = 0, iW = mdiPlace.rcNormalPosition.right - mdiPlace.rcNormalPosition.left,
        iH = mdiPlace.rcNormalPosition.bottom - mdiPlace.rcNormalPosition.top;

    if ((pDoc->m_PlacementPlot.rcNormalPosition.top < iY0) || (pDoc->m_PlacementPlot.rcNormalPosition.left < iX0)
        || (pDoc->m_PlacementPlot.rcNormalPosition.bottom > (iY0 + iH)) || (pDoc->m_PlacementPlot.rcNormalPosition.right > (iX0 + iW))
        || (pDoc->m_PlacementPlot.rcNormalPosition.right < (pDoc->m_PlacementPlot.rcNormalPosition.left + SIGMA_MINWIDTH3))
        || (pDoc->m_PlacementPlot.rcNormalPosition.bottom < (pDoc->m_PlacementPlot.rcNormalPosition.top + SIGMA_MINHEIGHT3))) {

        pDoc->m_PlacementPlot.length = sizeof(WINDOWPLACEMENT);
        pDoc->m_PlacementPlot.rcNormalPosition.left = iX0;
        pDoc->m_PlacementPlot.rcNormalPosition.top = iY0;
        pDoc->m_PlacementPlot.rcNormalPosition.right = iX0 + SIGMA_DEFWIDTH_PLOT;
        pDoc->m_PlacementPlot.rcNormalPosition.bottom = iY0 + SIGMA_DEFHEIGHT_PLOT;
    }

    pWnd->SetWindowPlacement(&(pDoc->m_PlacementPlot));

    CString strT = pDoc->GetTitle();
    strT.TrimRight(_T("*"));
    strT.Append(_T(" - Graph"));
    pWnd->SetWindowText((LPCTSTR)strT);

    if (m_bInitialized == FALSE) {
        PlotInit();
        if (m_pPlot == NULL) {
            pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);
        }

        pWnd->SetForegroundWindow();
        m_bInitialized = TRUE;
    }

    pDoc->SetModifiedFlag(FALSE);
    pDoc->SaveState();

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
}

void CSigmaViewPlot::PlotInit()
{
    RECT recFrame = {0,0,0,0};

    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {    // Error handling
        return;
    }

    m_pPlot = pDoc->GetPlot();

    if (m_pPlot == NULL) {    // Error handling
        return;
    }

    // Save min et max values from the Autoscale function
    axis_t *paxisX = NULL, *paxisY = NULL;

    if ((m_pPlot->curvecount > 0) && (m_pPlot->curveactive >= 0) && (m_pPlot->curveactive < m_pPlot->curvecount)) {
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0xF0) == 0x10) {        // X2-Axis
            paxisX = &(m_pPlot->axistop);
        }
        else {            // X-Axis
            paxisX = &(m_pPlot->axisbottom);
        }
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0x0F) == 0x01) {        // Y2-Axis
            paxisY = &(m_pPlot->axisright);
        }
        else {            // Y-Axis
            paxisY = &(m_pPlot->axisleft);
        }
    }
    else {            // Y-Axis
        paxisX = &(m_pPlot->axisbottom);
        paxisY = &(m_pPlot->axisleft);
    }

    m_fXmin = paxisX->min;
    m_fXmax = paxisX->max;
    m_fYmin = paxisY->min;
    m_fYmax = paxisY->max;

    m_pPlot->frame = GetSafeHwnd();
    m_pPlot->dc = NULL;

    m_hACursor = ::LoadCursor(NULL, IDC_ARROW);
    m_hZCursor = ::LoadCursor(NULL, IDC_CROSS);
    m_hMCursor = ::LoadCursor(NULL, IDC_SIZEALL);
    m_hMCursorH = ::LoadCursor(NULL, IDC_SIZEWE);
    m_hMCursorV = ::LoadCursor(NULL, IDC_SIZENS);

    return;
}

inline void CSigmaViewPlot::PlotAutoScale()
{
    m_pPlot->errcode = 0;
    pl_autoscale(m_pPlot);

    // Save min et max values from the Autoscale function
    axis_t *paxisX = NULL, *paxisY = NULL;

    if (m_pPlot == NULL) {
        return;
    }

    if ((m_pPlot->curvecount > 0) && (m_pPlot->curveactive >= 0) && (m_pPlot->curveactive < m_pPlot->curvecount)) {
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0xF0) == 0x10) {        // X2-Axis
            paxisX = &(m_pPlot->axistop);
        }
        else {            // X-Axis
            paxisX = &(m_pPlot->axisbottom);
        }
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0x0F) == 0x01) {        // Y2-Axis
            paxisY = &(m_pPlot->axisright);
        }
        else {            // Y-Axis
            paxisY = &(m_pPlot->axisleft);
        }
    }
    else {            // Y-Axis
        paxisX = &(m_pPlot->axisbottom);
        paxisY = &(m_pPlot->axisleft);
    }

    m_fXmin = paxisX->min;
    m_fXmax = paxisX->max;
    m_fYmin = paxisY->min;
    m_fYmax = paxisY->max;
}

CSigmaViewPlot::CSigmaViewPlot()
{
    // Graph
    m_hACursor = (HCURSOR)NULL;
    m_hZCursor = (HCURSOR)NULL;
    m_hMCursor = (HCURSOR)NULL;
    m_hMCursorH = (HCURSOR)NULL;
    m_hMCursorV = (HCURSOR)NULL;

    m_bInitialized = FALSE;

    m_bAutoScale = FALSE;

    m_pPlot = NULL;

    m_pCanvas = NULL;
    m_recTX.left = 0L;
    m_recTX.right = 0L;
    m_recTX.top = 0L;
    m_recTX.bottom = 0L;
    m_pointTX.x = 0L;
    m_pointTX.y = 0L;
    m_pointTXe.x = 0L;
    m_pointTXe.y = 0L;

    m_nResizingPlot = -1;

    m_bPlotSelected = FALSE;

    m_bZoomIn = FALSE;
    m_bZooming = FALSE;

    m_bAddText = FALSE;
    m_bAddLine = FALSE;
    m_bAddRect = FALSE;
    m_bAddEllipse = FALSE;

    m_bAddingText = FALSE;
    m_bAddingLine = FALSE;
    m_bAddingRect = FALSE;
    m_bAddingEllipse = FALSE;

    m_nMovingText = -1;
    m_nMovingLine = -1;
    m_nMovingRect = -1;
    m_nMovingEllipse = -1;
    m_nMovingLegend = -1;
    m_nMovingBottomTitle = -1;
    m_nMovingLeftTitle = -1;
    m_nMovingTopTitle = -1;
    m_nMovingRightTitle = -1;

    m_nResizingLine = -1;
    m_nResizingRect = -1;
    m_nResizingEllipse = -1;
    m_nResizingLineT = -1;
    m_nResizingRectT = -1;
    m_nResizingEllipseT = -1;

    m_nTextSelected = -1;
    m_nLineSelected = -1;
    m_nRectSelected = -1;
    m_nEllipseSelected = -1;
    m_nCurveSelected = -1;
    m_nLegendSelected = -1;
    m_nTitleSelected = -1;

    memset(m_recText, 0, PL_MAXITEMS * sizeof(RECT));
    memset(m_recLine, 0, PL_MAXITEMS * sizeof(RECT));
    memset(m_recRect, 0, PL_MAXITEMS * sizeof(RECT));
    memset(m_recEllipse, 0, PL_MAXITEMS * sizeof(RECT));
    memset(&m_recLegend, 0, sizeof(RECT));
    memset(&m_recBottomTitle, 0, sizeof(RECT));
    memset(&m_recLeftTitle, 0, sizeof(RECT));
    memset(&m_recTopTitle, 0, sizeof(RECT));
    memset(&m_recRightTitle, 0, sizeof(RECT));

    m_fXmin = -1.0;
    m_fXmax = 1.0;
    m_fYmin = -0.01;
    m_fYmax = 0.01;

    SetPrintMarginInfo(2, 2, 4, 4, 1, 1, 1);

    m_bDoubleBuffer = TRUE;    // Use double buffering to avoid flicker?

    memset(&m_ItemCopy, 0, sizeof(itemcopy_t));
    m_ItemCopy.isset = false;
    m_ItemCopy.type = ITEMCOPY_NONE;
}

CSigmaViewPlot::~CSigmaViewPlot()
{
    ReleaseGeo();
}

BOOL CSigmaViewPlot::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

// CSigmaViewPlot drawing

void CSigmaViewPlot::OnDraw(CDC* pDC)
{
    pDC->SetMapMode(MM_LOENGLISH);
}

void CSigmaViewPlot::OnPaint()
{
    CView::OnPaint();

    Plot();
}

void CSigmaViewPlot::Plot(CDC *pDC/* = NULL*/, long_t lwidth/* = 0*/, long_t lheight/* = 0*/, bool isPrinting/* = false*/)
{
    int_t jj;
    RECT recT, recTT;
    BOOL bReleaseDC = FALSE;
    CString strT;

    CSigmaDC sigmaDC;

    if (pDC != NULL) {                // Printing ...
        m_pPlot->frame = NULL;
        m_pPlot->dc = pDC->GetSafeHdc();
        if (m_pPlot->dc == NULL) {
            return;
        }
        if ((lwidth != 0) && (lheight != 0)) {
            real_t fwidth, fheight, fratio;

            fwidth = (real_t)lwidth;
            fheight = (real_t)lheight;
            if ((m_pPlot->plint.height != 0.0) && (m_pPlot->plint.width != 0.0)) {
                fratio = m_pPlot->plint.width / m_pPlot->plint.height;
                if (fratio < 1.0) {
                    m_pPlot->plint.height = fheight;
                    m_pPlot->plint.width = fheight * fratio;
                }
                else {
                    m_pPlot->plint.width = fwidth;
                    m_pPlot->plint.height = fwidth / fratio;
                }
            }
            else {
                m_pPlot->plint.width = fwidth;
                m_pPlot->plint.height = fheight;
            }
        }
    }
    else {
        pDC = GetDC();
        if (pDC == NULL) {
            return;
        }

        if (m_bDoubleBuffer) {
            sigmaDC.SetDC(pDC);
            m_pPlot->dc = sigmaDC.GetSafeHdc();
        }
        else {
            m_pPlot->dc = pDC->GetSafeHdc();
        }

        if (m_pPlot->dc == NULL) {
            ReleaseDC(pDC); pDC = NULL;
            return;
        }

        m_pPlot->frame = GetSafeHwnd();
        bReleaseDC = TRUE;
    }

    if ((m_pPlot->curveactive < 0) || (m_pPlot->curveactive >= m_pPlot->curvecount)) {
        m_pPlot->curveactive = 0;
    }

    if (m_bAutoScale == TRUE) {
        PlotAutoScale();
        m_bAutoScale = FALSE;
    }

    m_pPlot->errcode = 0;
    pl_validate(m_pPlot, 0);
    if (m_pPlot->errcode != 0) {    // Error
        if (bReleaseDC == TRUE) {
            sigmaDC.DrawIt(); ReleaseDC(pDC); pDC = NULL;
            m_pPlot->dc = NULL;
        }
        if (m_pPlot->curvecount != 0) {
            strT = (LPCTSTR) (m_pPlot->message);
            strT += _T("\r\n");
            SigmaApp.Output(strT, TRUE);
        }
        return;
    }

    pl_plot(m_pPlot, isPrinting);
    if (m_pPlot->errcode != 0) {    // Error
        if (bReleaseDC == TRUE) {
            sigmaDC.DrawIt(); ReleaseDC(pDC); pDC = NULL;
            m_pPlot->dc = NULL;
        }
        strT = (LPCTSTR) (m_pPlot->message);
        strT += _T("\r\n");
        SigmaApp.Output(strT, TRUE);
        return;
    }

    // Update rectangles info
    if (m_pPlot->textcount > 0) {
        for (jj = 0; jj < m_pPlot->textcount; jj++) {
            m_recText[jj].left = m_pPlot->plint.textRect[jj].left;
            m_recText[jj].right = m_pPlot->plint.textRect[jj].right;
            m_recText[jj].top = m_pPlot->plint.textRect[jj].top;
            m_recText[jj].bottom = m_pPlot->plint.textRect[jj].bottom;
        }
    }
    if (m_pPlot->linecount > 0) {
        for (jj = 0; jj < m_pPlot->linecount; jj++) {
            m_recLine[jj].left = m_pPlot->plint.lineRect[jj].left;
            m_recLine[jj].right = m_pPlot->plint.lineRect[jj].right;
            m_recLine[jj].top = m_pPlot->plint.lineRect[jj].top;
            m_recLine[jj].bottom = m_pPlot->plint.lineRect[jj].bottom;
        }
    }
    if (m_pPlot->rectanglecount > 0) {
        for (jj = 0; jj < m_pPlot->rectanglecount; jj++) {
            m_recRect[jj].left = m_pPlot->plint.rectangleRect[jj].left;
            m_recRect[jj].right = m_pPlot->plint.rectangleRect[jj].right;
            m_recRect[jj].top = m_pPlot->plint.rectangleRect[jj].top;
            m_recRect[jj].bottom = m_pPlot->plint.rectangleRect[jj].bottom;
        }
    }
    if (m_pPlot->ellipsecount > 0) {
        for (jj = 0; jj < m_pPlot->ellipsecount; jj++) {
            m_recEllipse[jj].left = m_pPlot->plint.ellipseRect[jj].left;
            m_recEllipse[jj].right = m_pPlot->plint.ellipseRect[jj].right;
            m_recEllipse[jj].top = m_pPlot->plint.ellipseRect[jj].top;
            m_recEllipse[jj].bottom = m_pPlot->plint.ellipseRect[jj].bottom;
        }
    }

    if (m_pPlot->curvecount > 0) {
        m_recLegend.left = m_pPlot->plint.legendsRect.left;
        m_recLegend.right = m_pPlot->plint.legendsRect.right;
        m_recLegend.top = m_pPlot->plint.legendsRect.top;
        m_recLegend.bottom = m_pPlot->plint.legendsRect.bottom;
    }

    m_recBottomTitle.left = m_pPlot->plint.axistRect[0].left;
    m_recBottomTitle.right = m_pPlot->plint.axistRect[0].right;
    m_recBottomTitle.top = m_pPlot->plint.axistRect[0].top;
    m_recBottomTitle.bottom = m_pPlot->plint.axistRect[0].bottom;

    m_recLeftTitle.left = m_pPlot->plint.axistRect[1].left;
    m_recLeftTitle.right = m_pPlot->plint.axistRect[1].right;
    m_recLeftTitle.top = m_pPlot->plint.axistRect[1].top;
    m_recLeftTitle.bottom = m_pPlot->plint.axistRect[1].bottom;

    m_recTopTitle.left = m_pPlot->plint.axistRect[2].left;
    m_recTopTitle.right = m_pPlot->plint.axistRect[2].right;
    m_recTopTitle.top = m_pPlot->plint.axistRect[2].top;
    m_recTopTitle.bottom = m_pPlot->plint.axistRect[2].bottom;

    m_recRightTitle.left = m_pPlot->plint.axistRect[3].left;
    m_recRightTitle.right = m_pPlot->plint.axistRect[3].right;
    m_recRightTitle.top = m_pPlot->plint.axistRect[3].top;
    m_recRightTitle.bottom = m_pPlot->plint.axistRect[3].bottom;

    PrepareGeo(m_pPlot->dc);

    if ((GetCapture() == this)
        && ((m_bZooming == TRUE) || (m_nResizingPlot >= 0)
        || (m_nMovingText >= 0) || (m_nMovingLine >= 0)
        || (m_nMovingRect >= 0) || (m_nMovingEllipse >= 0)
        || (m_nMovingLegend >= 0) || (m_nMovingBottomTitle >= 0)
        || (m_nMovingLeftTitle >= 0) || (m_nMovingTopTitle >= 0)
        || (m_nMovingRightTitle >= 0)
        || (m_nResizingLine >= 0) || (m_nResizingRect >= 0)
        || (m_nResizingEllipse >= 0)
        || (m_bAddingLine == TRUE) || (m_bAddingRect == TRUE)
        || (m_bAddingEllipse == TRUE))) {

        if (m_bAddingLine == TRUE) {
            drawLineTX(m_pointTX, m_pointTXe);
        }
        else if (m_bAddingRect == TRUE) {
            drawRectangleTX(m_recTX);
        }
        else if (m_bAddingEllipse == TRUE) {
            drawEllipseTX(m_recTX);
        }
        else if (m_nResizingLine >= 0) {
            drawLineTX(m_pointTX, m_pointTXe);
        }
        else if (m_nResizingRect >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nResizingEllipse >= 0) {
            drawEllipseTX(m_recTX);
        }
        else if (m_nMovingText >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nMovingLine >= 0) {
            drawLineTX(m_pointTX, m_pointTXe);
        }
        else if (m_nMovingRect >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nMovingEllipse >= 0) {
            drawEllipseTX(m_recTX);
        }
        else if (m_nMovingLegend >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nMovingBottomTitle >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nMovingLeftTitle >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nMovingTopTitle >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nMovingRightTitle >= 0) {
            drawRectangleTX(m_recTX);
        }
        else if (m_bZooming == TRUE) {
            drawRectangleTX(m_recTX);
        }
        else if (m_nResizingPlot >= 0) {
            drawRectangleTX(m_recTX);
            UpdateResizeLoc(&m_recTX, m_recMov);
            for (jj = 0; jj < 5; jj++) {
                drawRectangleTX(m_recMov[jj].left, m_recMov[jj].top, m_recMov[jj].right + 1, m_recMov[jj].bottom + 1);
            }
        }

        if (m_nLineSelected >= 0) {
            recT.left = m_pointTX.x - PL_RESIZE;
            recT.top = m_pointTX.y - PL_RESIZE;
            recT.right = m_pointTX.x + PL_RESIZE;
            recT.bottom = m_pointTX.y + PL_RESIZE;
            recTT.left = m_pointTXe.x - PL_RESIZE;
            recTT.top = m_pointTXe.y - PL_RESIZE;
            recTT.right = m_pointTXe.x + PL_RESIZE;
            recTT.bottom = m_pointTXe.y + PL_RESIZE;

            drawRectangleTX(recT.left, recT.top, recT.right + 1, recT.bottom + 1);
            drawRectangleTX(recTT.left, recTT.top, recTT.right + 1, recTT.bottom + 1);
        }
        else if (m_nRectSelected >= 0) {
            recT.left = m_recTX.left - PL_RESIZE;
            recT.top = m_recTX.top - PL_RESIZE;
            recT.right = m_recTX.left + PL_RESIZE;
            recT.bottom = m_recTX.top + PL_RESIZE;
            recTT.left = m_recTX.right - PL_RESIZE;
            recTT.top = m_recTX.bottom - PL_RESIZE;
            recTT.right = m_recTX.right + PL_RESIZE;
            recTT.bottom = m_recTX.bottom + PL_RESIZE;

            drawRectangleTX(recT.left, recT.top, recT.right + 1, recT.bottom + 1);
            drawRectangleTX(recTT.left, recTT.top, recTT.right + 1, recTT.bottom + 1);
        }
        else if (m_nEllipseSelected >= 0) {
            recT.left = m_recTX.left - PL_RESIZE;
            recT.top = m_recTX.top - PL_RESIZE;
            recT.right = m_recTX.left + PL_RESIZE;
            recT.bottom = m_recTX.top + PL_RESIZE;
            recTT.left = m_recTX.right - PL_RESIZE;
            recTT.top = m_recTX.bottom - PL_RESIZE;
            recTT.right = m_recTX.right + PL_RESIZE;
            recTT.bottom = m_recTX.bottom + PL_RESIZE;

            drawRectangleTX(recT.left, recT.top, recT.right + 1, recT.bottom + 1);
            drawRectangleTX(recTT.left, recTT.top, recTT.right + 1, recTT.bottom + 1);
        }
    }
    else {
        if ((m_nTextSelected >= 0) && (m_nTextSelected < m_pPlot->textcount)) {
            m_recTX = m_pPlot->plint.textRect[m_nTextSelected];
            drawRectangleTX(m_recTX);
        }
        else if (m_nLineSelected >= 0) {
            if (m_pPlot->line[m_nLineSelected].orient == 0) {
                recT.left = m_pPlot->plint.lineRect[m_nLineSelected].left - PL_RESIZE;
                recT.top = m_pPlot->plint.lineRect[m_nLineSelected].top - PL_RESIZE;
                recT.right = m_pPlot->plint.lineRect[m_nLineSelected].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].top + PL_RESIZE;
                recTT.left = m_pPlot->plint.lineRect[m_nLineSelected].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.lineRect[m_nLineSelected].bottom - PL_RESIZE;
                recTT.right = m_pPlot->plint.lineRect[m_nLineSelected].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].bottom + PL_RESIZE;
            }
            else {
                recT.left = m_pPlot->plint.lineRect[m_nLineSelected].left - PL_RESIZE;
                recT.top = m_pPlot->plint.lineRect[m_nLineSelected].bottom - PL_RESIZE;
                recT.right = m_pPlot->plint.lineRect[m_nLineSelected].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].bottom + PL_RESIZE;
                recTT.left = m_pPlot->plint.lineRect[m_nLineSelected].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.lineRect[m_nLineSelected].top - PL_RESIZE;
                recTT.right = m_pPlot->plint.lineRect[m_nLineSelected].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].top + PL_RESIZE;
            }

            drawRectangleTX(recT.left, recT.top, recT.right + 1, recT.bottom + 1);
            drawRectangleTX(recTT.left, recTT.top, recTT.right + 1, recTT.bottom + 1);
        }
        else if (m_nRectSelected >= 0) {
            recT.left = m_pPlot->plint.rectangleRect[m_nRectSelected].left - PL_RESIZE;
            recT.top = m_pPlot->plint.rectangleRect[m_nRectSelected].top - PL_RESIZE;
            recT.right = m_pPlot->plint.rectangleRect[m_nRectSelected].left + PL_RESIZE;
            recT.bottom = m_pPlot->plint.rectangleRect[m_nRectSelected].top + PL_RESIZE;
            recTT.left = m_pPlot->plint.rectangleRect[m_nRectSelected].right - PL_RESIZE;
            recTT.top = m_pPlot->plint.rectangleRect[m_nRectSelected].bottom - PL_RESIZE;
            recTT.right = m_pPlot->plint.rectangleRect[m_nRectSelected].right + PL_RESIZE;
            recTT.bottom = m_pPlot->plint.rectangleRect[m_nRectSelected].bottom + PL_RESIZE;

            drawRectangleTX(recT.left, recT.top, recT.right + 1, recT.bottom + 1);
            drawRectangleTX(recTT.left, recTT.top, recTT.right + 1, recTT.bottom + 1);
        }
        else if (m_nEllipseSelected >= 0) {
            recT.left = m_pPlot->plint.ellipseRect[m_nEllipseSelected].left - PL_RESIZE;
            recT.top = m_pPlot->plint.ellipseRect[m_nEllipseSelected].top - PL_RESIZE;
            recT.right = m_pPlot->plint.ellipseRect[m_nEllipseSelected].left + PL_RESIZE;
            recT.bottom = m_pPlot->plint.ellipseRect[m_nEllipseSelected].top + PL_RESIZE;
            recTT.left = m_pPlot->plint.ellipseRect[m_nEllipseSelected].right - PL_RESIZE;
            recTT.top = m_pPlot->plint.ellipseRect[m_nEllipseSelected].bottom - PL_RESIZE;
            recTT.right = m_pPlot->plint.ellipseRect[m_nEllipseSelected].right + PL_RESIZE;
            recTT.bottom = m_pPlot->plint.ellipseRect[m_nEllipseSelected].bottom + PL_RESIZE;

            drawRectangleTX(recT.left, recT.top, recT.right + 1, recT.bottom + 1);
            drawRectangleTX(recTT.left, recTT.top, recTT.right + 1, recTT.bottom + 1);
        }
        else if (m_nLegendSelected >= 0) {
            drawRectangleTX(m_pPlot->plint.legendsRect.left, m_pPlot->plint.legendsRect.top, m_pPlot->plint.legendsRect.right, m_pPlot->plint.legendsRect.bottom);
        }
        else if (m_nTitleSelected >= 0) {
            if (((m_nTitleSelected % 2) == 0)) {
                drawRectangleTX(m_pPlot->plint.axistRect[m_nTitleSelected].left, m_pPlot->plint.axistRect[m_nTitleSelected].top, m_pPlot->plint.axistRect[m_nTitleSelected].right, m_pPlot->plint.axistRect[m_nTitleSelected].bottom);
            }
            else {
                drawRectangleTX(m_pPlot->plint.axistRect[m_nTitleSelected].left, m_pPlot->plint.axistRect[m_nTitleSelected].top, m_pPlot->plint.axistRect[m_nTitleSelected].right, m_pPlot->plint.axistRect[m_nTitleSelected].bottom);
            }
        }
        else if ((m_bPlotSelected == TRUE) && (bReleaseDC == TRUE)) {
            UpdateResizeLoc(&(m_pPlot->plint.frameRect[1]), m_recMov);
            for (jj = 0; jj < 5; jj++) {
                drawRectangleTX(m_recMov[jj].left, m_recMov[jj].top, m_recMov[jj].right + 1, m_recMov[jj].bottom + 1);
            }
        }
    }

    ReleaseGeo();

    if (bReleaseDC == TRUE) {
        sigmaDC.DrawIt(); ReleaseDC(pDC); pDC = NULL;
        m_pPlot->dc = NULL;
    }
}

// CSigmaViewPlot diagnostics

#ifdef _DEBUG
void CSigmaViewPlot::AssertValid() const
{
    CView::AssertValid();
}

void CSigmaViewPlot::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CSigmaDoc* CSigmaViewPlot::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSigmaDoc)));
    return (CSigmaDoc*)m_pDocument;
}
#endif //_DEBUG


// CSigmaViewPlot message handlers
void CSigmaViewPlot::OnLButtonDblClk(UINT nFlags, CPoint tPoint)
{
    int_t jj;

    CView::OnLButtonDblClk(nFlags, tPoint);

    if (GetCapture() == this) {
        ReleaseCapture();
    }

    if (m_pPlot == NULL) {
        return;
    }

    // TODO: Add your message handler code here and/or call default
    if (m_pPlot->curvecount < 1) {
        return;
    }

    // Update rectangles info
    if (m_pPlot->textcount > 0) {
        for (jj = 0; jj < m_pPlot->textcount; jj++) {
            if (PtInRect(&(m_pPlot->plint.textRect[jj]), tPoint)) {    // Texte
                CTextDlg inputDlg;
                inputDlg.m_strLabel = _T("Text:");
                inputDlg.m_pPlot = m_pPlot;
                inputDlg.m_pDoc = (CDocument*)GetDocument();
                RECT rcT = {0,0,0,0};
                GetClientRect(&rcT);
                inputDlg.m_recFrame.bottom = rcT.bottom;
                inputDlg.m_recFrame.left = rcT.left;
                inputDlg.m_recFrame.top = rcT.top;
                inputDlg.m_recFrame.right = rcT.right;
                inputDlg.m_ptInit.x = m_ptInit.x;
                inputDlg.m_ptInit.y = m_ptInit.y;
                ClientToScreen(&(inputDlg.m_recFrame));
                ClientToScreen(&(inputDlg.m_ptInit));
                if ((inputDlg.m_ptInit.x < inputDlg.m_recFrame.left) || (inputDlg.m_ptInit.x > inputDlg.m_recFrame.right)
                    || (inputDlg.m_ptInit.y < inputDlg.m_recFrame.top) || (inputDlg.m_ptInit.y > inputDlg.m_recFrame.bottom)
                    || (inputDlg.m_recFrame.bottom == inputDlg.m_recFrame.top)
                    || (inputDlg.m_recFrame.left == inputDlg.m_recFrame.right)) {
                    return;
                }
                inputDlg.m_nSelectedText = jj;
                // Si le texte a un ID > 0 alors il ne peut être édité (cas de rapport de fit par exemple)
                if (m_pPlot->text[jj].id > 0) {
                    inputDlg.m_bReadOnly = TRUE;
                }
                inputDlg.DoModal();
                if (inputDlg.m_bOK) {
                    if (m_pPlot->text[jj].text[0] == _T('\0')) {
                        m_nTextSelected = -1;
                    }
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                }
                return;
            }
        }
    }

    if (m_pPlot->linecount > 0) {
        for (jj = 0; jj < m_pPlot->linecount; jj++) {
            if (SigmaApp.PtOnLine(&(m_pPlot->plint.lineRect[jj]), tPoint, m_pPlot->line[jj].orient)) {
                CGeometricLineDlg geolineDlg;
                geolineDlg.m_pPlot = m_pPlot;
                RECT rcT = {0,0,0,0};
                GetClientRect(&rcT);
                geolineDlg.m_recFrame.bottom = rcT.bottom;
                geolineDlg.m_recFrame.left = rcT.left;
                geolineDlg.m_recFrame.top = rcT.top;
                geolineDlg.m_recFrame.right = rcT.right;
                geolineDlg.m_ptInit.x = m_ptInit.x;
                geolineDlg.m_ptInit.y = m_ptInit.y;
                ClientToScreen(&(geolineDlg.m_recFrame));
                ClientToScreen(&(geolineDlg.m_ptInit));
                geolineDlg.m_nSelectedLine = jj;
                geolineDlg.DoModal();
                if (geolineDlg.m_bOK) {
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                }
                return;
            }
        }
    }
    if (m_pPlot->rectanglecount > 0) {
        for (jj = 0; jj < m_pPlot->rectanglecount; jj++) {
            if (PtInRect(&(m_pPlot->plint.rectangleRect[jj]), tPoint)) {
                CGeometricRectDlg georectDlg;
                georectDlg.m_pPlot = m_pPlot;                
                RECT rcT = {0,0,0,0};
                GetClientRect(&rcT);
                georectDlg.m_recFrame.bottom = rcT.bottom;
                georectDlg.m_recFrame.left = rcT.left;
                georectDlg.m_recFrame.top = rcT.top;
                georectDlg.m_recFrame.right = rcT.right;
                georectDlg.m_ptInit.x = m_ptInit.x;
                georectDlg.m_ptInit.y = m_ptInit.y;
                ClientToScreen(&(georectDlg.m_recFrame));
                ClientToScreen(&(georectDlg.m_ptInit));
                georectDlg.m_nSelected = jj;
                georectDlg.m_nGeo = 0;
                georectDlg.DoModal();
                if (georectDlg.m_bOK) {
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                }
                return;
            }
        }
    }
    if (m_pPlot->ellipsecount > 0) {
        for (jj = 0; jj < m_pPlot->ellipsecount; jj++) {
            if (PtInRect(&(m_pPlot->plint.ellipseRect[jj]), tPoint)) {
                CGeometricRectDlg georectDlg;
                georectDlg.m_pPlot = m_pPlot;
                RECT rcT = {0,0,0,0};
                GetClientRect(&rcT);
                georectDlg.m_recFrame.bottom = rcT.bottom;
                georectDlg.m_recFrame.left = rcT.left;
                georectDlg.m_recFrame.top = rcT.top;
                georectDlg.m_recFrame.right = rcT.right;
                georectDlg.m_ptInit.x = m_ptInit.x;
                georectDlg.m_ptInit.y = m_ptInit.y;
                ClientToScreen(&(georectDlg.m_recFrame));
                ClientToScreen(&(georectDlg.m_ptInit));
                georectDlg.m_nSelected = jj;
                georectDlg.m_nGeo = 1;
                georectDlg.DoModal();
                if (georectDlg.m_bOK) {
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                }
                return;
            }
        }
    }

    if (m_pPlot->curvecount > 0) {
        for (jj = 0; jj < m_pPlot->curvecount; jj++) {
            if (PtInRect(&(m_pPlot->plint.legendRect[jj]), tPoint)) {
                // Ne pas activer s'il s'agit d'un fit
                CSigmaDoc* pDoc = GetDocument();
                ASSERT_VALID(pDoc);
                if (pDoc) {
                    if (pDoc->GetColumn(1, -1, m_pPlot->curve[jj].y->id) == NULL) {
                        m_pPlot->curveactive = jj;
                        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    }
                }
                return;
            }
        }
    }
 
    RECT rectAxis;
    real_t fd = (real_t) ((m_pPlot->plint.frameRect[1].right - m_pPlot->plint.frameRect[1].left + m_pPlot->plint.frameRect[1].bottom - m_pPlot->plint.frameRect[1].top) / 2.0);
    fd *= 0.007;
    if (fd < 4.0) { fd = 4.0; }
    else if (fd > 8.0) { fd = 8.0; }

    for (jj = 0; jj < 4; jj++) {
        if (jj == 0) {                // Bottom
            rectAxis.left = m_pPlot->plint.frameRect[1].left;
            rectAxis.right = m_pPlot->plint.frameRect[1].right;
            rectAxis.top = m_pPlot->plint.frameRect[1].bottom - (LONG)fd;
            rectAxis.bottom = m_pPlot->plint.frameRect[1].bottom + (LONG)fd;
        }
        else if (jj == 1) {        // Left
            rectAxis.left = m_pPlot->plint.frameRect[1].left - (LONG)fd;
            rectAxis.right = m_pPlot->plint.frameRect[1].left + (LONG)fd;
            rectAxis.top = m_pPlot->plint.frameRect[1].top;
            rectAxis.bottom = m_pPlot->plint.frameRect[1].bottom;
        }
        else if (jj == 2) {        // Top
            rectAxis.left = m_pPlot->plint.frameRect[1].left;
            rectAxis.right = m_pPlot->plint.frameRect[1].right;
            rectAxis.top = m_pPlot->plint.frameRect[1].top - (LONG)fd;
            rectAxis.bottom = m_pPlot->plint.frameRect[1].top + (LONG)fd;
        }
        else if (jj == 3) {        // Right
            rectAxis.left = m_pPlot->plint.frameRect[1].right - (LONG)fd;
            rectAxis.right = m_pPlot->plint.frameRect[1].right + (LONG)fd;
            rectAxis.top = m_pPlot->plint.frameRect[1].top;
            rectAxis.bottom = m_pPlot->plint.frameRect[1].bottom;
        }

        if (PtInRect(&rectAxis, tPoint)) {                                        // Axis
            ShowProperties(1, m_pPlot->curveactive, jj);
            return;
        }
        else if (PtInRect(&(m_pPlot->plint.axislRect[jj]), tPoint)) {    // Scale
            ShowProperties(2, m_pPlot->curveactive, jj);
            return;
        }
        else if (PtInRect(&(m_pPlot->plint.axistRect[jj]), tPoint)) {    // Axis
            ShowProperties(1, m_pPlot->curveactive, jj);
            return;
        }
    }

    return;
}

void CSigmaViewPlot::OnContextMenu(CWnd* pWnd, CPoint tPoint)
{
    CMenu contextMenu; 
    CMenu *pMenu = NULL; 
    CPoint ptT = tPoint;

    if (m_pPlot == NULL) {
        return;
    }

    m_nCurveSelected = -1;

    ScreenToClient(&ptT);

    if (IsItemSelected(tPoint) == TRUE) {
        contextMenu.LoadMenu(IDM_PLOTPOP_ITEM);

        pMenu = contextMenu.GetSubMenu(0);

        if (pMenu == NULL) {
            return;
        }
        if (pMenu->m_hMenu == NULL) {
            return;
        }

        UINT itemID = pMenu->GetMenuItemID(0);
        if (m_nTextSelected >= 0) {
            pMenu->ModifyMenu(itemID, MF_BYCOMMAND, itemID, _T("  &Remove Text"));
        }
        else if (m_nLineSelected >= 0) {
            pMenu->ModifyMenu(itemID, MF_BYCOMMAND, itemID, _T("  &Remove Line"));
        }
        else if (m_nRectSelected >= 0) {
            pMenu->ModifyMenu(itemID, MF_BYCOMMAND, itemID, _T("  &Remove Rectangle"));
        }
        else if (m_nEllipseSelected >= 0) {
            pMenu->ModifyMenu(itemID, MF_BYCOMMAND, itemID, _T("  &Remove Ellipse"));
        }
        else {
            itemID = pMenu->GetMenuItemID(2);
            pMenu->EnableMenuItem(itemID, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        }
    }
    else if (PtInRect(&m_recLegend, ptT)) {
        for (int_t jj = 0; jj < m_pPlot->curvecount; jj++) {
            if (PtInRect(&(m_pPlot->plint.legendRect[jj]), ptT)) {
                m_nCurveSelected = jj;
                break;
            }
        }
        if (m_nCurveSelected >= 0) {
            contextMenu.LoadMenu(IDM_PLOTPOP_CURVE);

            pMenu = contextMenu.GetSubMenu(0);

            if (pMenu == NULL) {
                return;
            }
            if (pMenu->m_hMenu == NULL) {
                return;
            }
        }
    }
    else {
        contextMenu.LoadMenu(IDM_PLOTPOP_MAIN);

        pMenu = contextMenu.GetSubMenu(0);

        if (pMenu == NULL) {
            return;
        }
        if (pMenu->m_hMenu == NULL) {
            return;
        }

        if ((m_pPlot->curvecount > 0) && (m_pPlot->curveactive >= 0) && (m_pPlot->curveactive < m_pPlot->curvecount)) {
            pMenu->EnableMenuItem(ID_PLOT_MASKFROM, MF_BYCOMMAND|MF_ENABLED);
            pMenu->EnableMenuItem(ID_PLOT_MASKTO, MF_BYCOMMAND|MF_ENABLED);
            pMenu->EnableMenuItem(ID_PLOT_UNMASK, MF_BYCOMMAND|MF_ENABLED);
        }
        else {
            pMenu->EnableMenuItem(ID_PLOT_MASKFROM, MF_BYCOMMAND|MF_GRAYED);
            pMenu->EnableMenuItem(ID_PLOT_MASKTO, MF_BYCOMMAND|MF_GRAYED);
            pMenu->EnableMenuItem(ID_PLOT_UNMASK, MF_BYCOMMAND|MF_GRAYED);
        }

        UINT itemID = pMenu->GetMenuItemID(11);
        if (false == m_ItemCopy.isset) {
            pMenu->EnableMenuItem(itemID, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        }
        else {
            pMenu->ModifyMenu(itemID, MF_BYCOMMAND, itemID, 
                (   ITEMCOPY_TEXT      == m_ItemCopy.type) ? _T("  &Paste Text")
                : ((ITEMCOPY_LINE      == m_ItemCopy.type) ? _T("  &Paste Line")
                : ( ITEMCOPY_RECTANGLE == m_ItemCopy.type) ? _T("  &Paste Rectangle") : _T("  &Paste Ellipse")));
        }

        CSigmaDoc* pDoc = GetDocument();
        ASSERT_VALID(pDoc);
        if (pDoc) {
            if (pDoc->GetColumnCount(0) < 1) {
                pMenu->EnableMenuItem(ID_VIEW_DATA, MF_BYCOMMAND|MF_GRAYED);
            }
            else {
                pMenu->EnableMenuItem(ID_VIEW_DATA, MF_BYCOMMAND|MF_ENABLED);
            }
            if (pDoc->GetColumnCount(1) < 1) {
                pMenu->EnableMenuItem(ID_VIEW_DATAFIT, MF_BYCOMMAND|MF_GRAYED);
            }
            else {
                pMenu->EnableMenuItem(ID_VIEW_DATAFIT, MF_BYCOMMAND|MF_ENABLED);
            }
        }
    }

    if (pMenu) {
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, this, NULL);
        CSigmaViewPlot::OnRButtonDown(TPM_LEFTALIGN | TPM_RIGHTBUTTON, tPoint);
    }
}

BOOL CSigmaViewPlot::IsItemSelected(CPoint tPoint)
{
    BOOL bItemSelected = FALSE;
    RECT recT, recTT;

    if (m_pPlot == NULL) {
        return FALSE;
    }

    if (m_pPlot->curvecount < 1) {
        return FALSE;
    }

    ScreenToClient(&tPoint);

    m_nTextSelected = -1;
    m_nLineSelected = -1;
    m_nRectSelected = -1;
    m_nEllipseSelected = -1;
    m_nLegendSelected = -1;
    m_nTitleSelected = -1;

    // Move/Select Items
    if (m_pPlot->textcount > 0) {
        for (int_t jj = 0; jj < m_pPlot->textcount; jj++) {
            if (PtInRect(&(m_pPlot->plint.textRect[jj]), tPoint)) {
                bItemSelected = TRUE;
                m_nTextSelected = jj;
                break;
            }
        }
    }

    if (bItemSelected == FALSE) {
        if (m_pPlot->linecount > 0) {
            for (int_t jj = 0; jj < m_pPlot->linecount; jj++) {
                if (m_pPlot->line[jj].orient == 0) {
                    recT.left = m_pPlot->plint.lineRect[jj].left - PL_RESIZE;
                    recT.top = m_pPlot->plint.lineRect[jj].top - PL_RESIZE;
                    recT.right = m_pPlot->plint.lineRect[jj].left + PL_RESIZE;
                    recT.bottom = m_pPlot->plint.lineRect[jj].top + PL_RESIZE;
                    recTT.left = m_pPlot->plint.lineRect[jj].right - PL_RESIZE;
                    recTT.top = m_pPlot->plint.lineRect[jj].bottom - PL_RESIZE;
                    recTT.right = m_pPlot->plint.lineRect[jj].right + PL_RESIZE;
                    recTT.bottom = m_pPlot->plint.lineRect[jj].bottom + PL_RESIZE;
                }
                else {
                    recT.left = m_pPlot->plint.lineRect[jj].left - PL_RESIZE;
                    recT.top = m_pPlot->plint.lineRect[jj].bottom - PL_RESIZE;
                    recT.right = m_pPlot->plint.lineRect[jj].left + PL_RESIZE;
                    recT.bottom = m_pPlot->plint.lineRect[jj].bottom + PL_RESIZE;
                    recTT.left = m_pPlot->plint.lineRect[jj].right - PL_RESIZE;
                    recTT.top = m_pPlot->plint.lineRect[jj].top - PL_RESIZE;
                    recTT.right = m_pPlot->plint.lineRect[jj].right + PL_RESIZE;
                    recTT.bottom = m_pPlot->plint.lineRect[jj].top + PL_RESIZE;
                }
                // Resize or Move
                if (PtInRect(&recT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nLineSelected = jj;
                    break;
                }
                else if (PtInRect(&recTT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nLineSelected = jj;
                    break;
                }
                else if (SigmaApp.PtOnLine(&(m_pPlot->plint.lineRect[jj]), tPoint, m_pPlot->line[jj].orient)) {
                    bItemSelected = TRUE;
                    m_nLineSelected = jj;
                    break;
                }
            }
        }
    }

    if (bItemSelected == FALSE) {
        if (m_pPlot->rectanglecount > 0) {
            for (int_t jj = 0; jj < m_pPlot->rectanglecount; jj++) {
                recT.left = m_pPlot->plint.rectangleRect[jj].left - PL_RESIZE;
                recT.top = m_pPlot->plint.rectangleRect[jj].top - PL_RESIZE;
                recT.right = m_pPlot->plint.rectangleRect[jj].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.rectangleRect[jj].top + PL_RESIZE;
                recTT.left = m_pPlot->plint.rectangleRect[jj].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.rectangleRect[jj].bottom - PL_RESIZE;
                recTT.right = m_pPlot->plint.rectangleRect[jj].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.rectangleRect[jj].bottom + PL_RESIZE;
                // Resize or Move
                if (PtInRect(&recT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nRectSelected = jj;
                    break;
                }
                else if (PtInRect(&recTT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nRectSelected = jj;
                    break;
                }
                else if (PtInRect(&(m_pPlot->plint.rectangleRect[jj]), tPoint)) {
                    bItemSelected = TRUE;
                    m_nRectSelected = jj;
                    break;
                }
            }
        }
    }

    if (bItemSelected == FALSE) {
        if (m_pPlot->ellipsecount > 0) {
            for (int_t jj = 0; jj < m_pPlot->ellipsecount; jj++) {
                recT.left = m_pPlot->plint.ellipseRect[jj].left - PL_RESIZE;
                recT.top = m_pPlot->plint.ellipseRect[jj].top - PL_RESIZE;
                recT.right = m_pPlot->plint.ellipseRect[jj].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.ellipseRect[jj].top + PL_RESIZE;
                recTT.left = m_pPlot->plint.ellipseRect[jj].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.ellipseRect[jj].bottom - PL_RESIZE;
                recTT.right = m_pPlot->plint.ellipseRect[jj].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.ellipseRect[jj].bottom + PL_RESIZE;
                // Resize or Move
                if (PtInRect(&recT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nEllipseSelected = jj;
                    break;
                }
                else if (PtInRect(&recTT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nEllipseSelected = jj;
                    break;
                }
                else if (PtInRect(&(m_pPlot->plint.ellipseRect[jj]), tPoint)) {
                    bItemSelected = TRUE;
                    m_nEllipseSelected = jj;
                    break;
                }
            }
        }
    }

    return bItemSelected;
}

void CSigmaViewPlot::ReleaseGeo()
{
    if (m_pCanvas) {
        delete m_pCanvas;
        m_pCanvas = NULL;
    }
}

BOOL CSigmaViewPlot::PrepareGeo(HDC hDCX)
{
    if (m_pCanvas) {
        delete m_pCanvas;
        m_pCanvas = NULL;
    }

    m_pCanvas = new Gdiplus::Graphics(hDCX);
    m_pCanvas->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

    return TRUE;
}

void CSigmaViewPlot::UpdateResizeLoc(irect_t *precFrame, RECT *precMov)
{
    if ((precFrame == NULL) || (precMov == NULL)) {
        return;
    }

    real_t fd = (real_t) ((precFrame->right - precFrame->left + precFrame->bottom - precFrame->top) / 2.0);
    fd *= 0.007;
    if (fd < 4.0) { fd = 4.0; }
    else if (fd > 8.0) { fd = 8.0; }

    //    Rectangles for resizing
    // 0:X-Axis, 1:Y, 2:X2, 3:Y2
    // 4:Center
    // 5:CornerXY, 6:CornerXY2, 7:CornerYX2, 8:CornerX2Y2

    // X
    real_t ftop = (real_t) (precFrame->bottom) - fd;
    real_t fbottom = (real_t) (precFrame->bottom) + fd;
    real_t fleft = (((real_t) (precFrame->left + precFrame->right)) / 2.0) - fd;
    real_t fright = (((real_t) (precFrame->left + precFrame->right)) / 2.0) + fd;
    precMov[0].top = (LONG) ftop;
    precMov[0].bottom = (LONG) fbottom;
    precMov[0].left = (LONG) fleft;
    precMov[0].right = (LONG) fright;

    // Y
    ftop = (((real_t) (precFrame->top + precFrame->bottom)) / 2.0) - fd;
    fbottom = (((real_t) (precFrame->top + precFrame->bottom)) / 2.0) + fd;
    fleft = (real_t) (precFrame->left) - fd;
    fright = (real_t) (precFrame->left) + fd;
    precMov[1].top = (LONG) ftop;
    precMov[1].bottom = (LONG) fbottom;
    precMov[1].left = (LONG) fleft;
    precMov[1].right = (LONG) fright;

    // X2
    ftop = (real_t) (precFrame->top) - fd;
    fbottom = (real_t) (precFrame->top) + fd;
    fleft = (((real_t) (precFrame->left + precFrame->right)) / 2.0) - fd;
    fright = (((real_t) (precFrame->left + precFrame->right)) / 2.0) + fd;
    precMov[2].top = (LONG) ftop;
    precMov[2].bottom = (LONG) fbottom;
    precMov[2].left = (LONG) fleft;
    precMov[2].right = (LONG) fright;

    // Y2
    ftop = (((real_t) (precFrame->top + precFrame->bottom)) / 2.0) - fd;
    fbottom = (((real_t) (precFrame->top + precFrame->bottom)) / 2.0) + fd;
    fleft = (real_t) (precFrame->right) - fd;
    fright = (real_t) (precFrame->right) + fd;
    precMov[3].top = (LONG) ftop;
    precMov[3].bottom = (LONG) fbottom;
    precMov[3].left = (LONG) fleft;
    precMov[3].right = (LONG) fright;

    // Center
    ftop = (((real_t) (precFrame->top + precFrame->bottom)) / 2.0) - fd;
    fbottom = (((real_t) (precFrame->top + precFrame->bottom)) / 2.0) + fd;
    fleft = (((real_t) (precFrame->left + precFrame->right)) / 2.0) - fd;
    fright = (((real_t) (precFrame->left + precFrame->right)) / 2.0) + fd;
    precMov[4].top = (LONG) ftop;
    precMov[4].bottom = (LONG) fbottom;
    precMov[4].left = (LONG) fleft;
    precMov[4].right = (LONG) fright;
}

void CSigmaViewPlot::OnLButtonDown(UINT nFlags, CPoint tPoint)
{
    BOOL bItemSelected = FALSE;
    RECT recT, recTT;

    CView::OnLButtonDown(nFlags, tPoint);
    
    if (GetCapture() == this) {
        ReleaseCapture();
    }

    if (m_pPlot == NULL) {
        return;
    }

    if (m_pPlot->curvecount < 1) {
        return;
    }

    m_ptBegin.x = tPoint.x;
    m_ptBegin.y = tPoint.y;

    if (m_ItemCopy.isset) {
        m_ItemCopy.pastex = (int_t) (tPoint.x);
        m_ItemCopy.pastey = (int_t) (tPoint.y);
    }

    BOOL bPlotSelected = m_bPlotSelected;
    BOOL bItemSelectedPrev = ((m_nTextSelected >= 0) || (m_nLineSelected >= 0) || (m_nRectSelected >= 0) || (m_nEllipseSelected >= 0) || (m_nTitleSelected >= 0) || (m_nLegendSelected >= 0));

    m_bPlotSelected = FALSE;
    m_bZooming = FALSE;
    m_nResizingPlot = -1;

    m_bAddingText = FALSE;
    m_bAddingLine = FALSE;
    m_bAddingRect = FALSE;
    m_bAddingEllipse = FALSE;

    m_nTextSelected = -1;
    m_nLineSelected = -1;
    m_nRectSelected = -1;
    m_nEllipseSelected = -1;
    m_nLegendSelected = -1;
    m_nTitleSelected = -1;

    m_nMovingText = -1;
    m_nMovingLine = -1;
    m_nMovingRect = -1;
    m_nMovingEllipse = -1;

    m_nCurveSelected = -1;

    m_nMovingLegend = -1;
    m_nMovingBottomTitle = -1;
    m_nMovingLeftTitle = -1;
    m_nMovingTopTitle = -1;
    m_nMovingRightTitle = -1;

    m_nResizingLine = -1;
    m_nResizingRect = -1;
    m_nResizingEllipse = -1;
    m_nResizingLineT = -1;
    m_nResizingRectT = -1;
    m_nResizingEllipseT = -1;

    m_recTX.left = 0L;
    m_recTX.right = 0L;
    m_recTX.top = 0L;
    m_recTX.bottom = 0L;
    m_pointTX.x = 0L;
    m_pointTX.y = 0L;
    m_pointTXe.x = 0L;
    m_pointTXe.y = 0L;

    Tic();

    // Add Items
    // Text
    if (m_bAddText == TRUE) {
        if (m_pPlot->errcode == 0) { // If previously reported error don't zoom
            m_bAddingText = TRUE;
            m_ptInit.x = tPoint.x;
            m_ptInit.y = tPoint.y;
        }
        return;
    }
    // Line
    if (m_bAddLine == TRUE) {
        if (m_pPlot->errcode == 0) { // If previously reported error don't zoom
            m_bAddingLine = TRUE;
            m_ptEnd.x = m_ptBegin.x;
            m_ptEnd.y = m_ptBegin.y;
            m_pointTX = m_ptInit;
            SetCapture();
            return;
        }
        return;
    }
    // Rectangle
    if (m_bAddRect == TRUE) {
        if (m_pPlot->errcode == 0) { // If previously reported error don't zoom
            m_bAddingRect = TRUE;
            m_ptEnd.x = m_ptBegin.x;
            m_ptEnd.y = m_ptBegin.y;
            SetCapture();
            return;
        }
        return;
    }
    // Ellipse
    if (m_bAddEllipse == TRUE) {
        if (m_pPlot->errcode == 0) { // If previously reported error don't zoom
            m_bAddingEllipse = TRUE;
            m_ptEnd.x = m_ptBegin.x;
            m_ptEnd.y = m_ptBegin.y;
            SetCapture();
            return;
        }
        return;
    }

    // Move/Select Items
    if (m_pPlot->textcount > 0) {
        for (int_t jj = 0; jj < m_pPlot->textcount; jj++) {
            if (PtInRect(&(m_pPlot->plint.textRect[jj]), tPoint)) {
                bItemSelected = TRUE;
                m_nTextSelected = jj;
                m_nMovingText = jj;
                m_ptInit.x = tPoint.x;
                m_ptInit.y = tPoint.y;

                m_recTX = m_pPlot->plint.textRect[jj];

                SetCapture();
                ::SetCursor(m_hMCursor);
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                return;
            }
        }
    }
    if (bItemSelected == FALSE) {
        if (m_pPlot->linecount > 0) {
            for (int_t jj = 0; jj < m_pPlot->linecount; jj++) {
                if (m_pPlot->line[jj].orient == 0) {
                    recT.left = m_pPlot->plint.lineRect[jj].left - PL_RESIZE;
                    recT.top = m_pPlot->plint.lineRect[jj].top - PL_RESIZE;
                    recT.right = m_pPlot->plint.lineRect[jj].left + PL_RESIZE;
                    recT.bottom = m_pPlot->plint.lineRect[jj].top + PL_RESIZE;
                    recTT.left = m_pPlot->plint.lineRect[jj].right - PL_RESIZE;
                    recTT.top = m_pPlot->plint.lineRect[jj].bottom - PL_RESIZE;
                    recTT.right = m_pPlot->plint.lineRect[jj].right + PL_RESIZE;
                    recTT.bottom = m_pPlot->plint.lineRect[jj].bottom + PL_RESIZE;
                }
                else {
                    recT.left = m_pPlot->plint.lineRect[jj].left - PL_RESIZE;
                    recT.top = m_pPlot->plint.lineRect[jj].bottom - PL_RESIZE;
                    recT.right = m_pPlot->plint.lineRect[jj].left + PL_RESIZE;
                    recT.bottom = m_pPlot->plint.lineRect[jj].bottom + PL_RESIZE;
                    recTT.left = m_pPlot->plint.lineRect[jj].right - PL_RESIZE;
                    recTT.top = m_pPlot->plint.lineRect[jj].top - PL_RESIZE;
                    recTT.right = m_pPlot->plint.lineRect[jj].right + PL_RESIZE;
                    recTT.bottom = m_pPlot->plint.lineRect[jj].top + PL_RESIZE;
                }
                // Resize or Move
                if (PtInRect(&recT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nLineSelected = jj;
                    m_nResizingLine = jj;
                    m_nResizingLineT = 0;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    if (m_pPlot->line[jj].orient == 0) {
                        m_pointTXe.x = m_pPlot->plint.lineRect[jj].left;
                        m_pointTXe.y = m_pPlot->plint.lineRect[jj].top;
                        m_pointTX.x = m_pPlot->plint.lineRect[jj].right;
                        m_pointTX.y = m_pPlot->plint.lineRect[jj].bottom;
                    }
                    else {
                        m_pointTXe.x = m_pPlot->plint.lineRect[jj].left;
                        m_pointTXe.y = m_pPlot->plint.lineRect[jj].bottom;
                        m_pointTX.x = m_pPlot->plint.lineRect[jj].right;
                        m_pointTX.y = m_pPlot->plint.lineRect[jj].top;
                    }

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
                else if (PtInRect(&recTT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nLineSelected = jj;
                    m_nResizingLine = jj;
                    m_nResizingLineT = 1;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;
                    
                    if (m_pPlot->line[jj].orient == 0) {
                        m_pointTX.x = m_pPlot->plint.lineRect[jj].left;
                        m_pointTX.y = m_pPlot->plint.lineRect[jj].top;
                        m_pointTXe.x = m_pPlot->plint.lineRect[jj].right;
                        m_pointTXe.y = m_pPlot->plint.lineRect[jj].bottom;
                    }
                    else {
                        m_pointTX.x = m_pPlot->plint.lineRect[jj].left;
                        m_pointTX.y = m_pPlot->plint.lineRect[jj].bottom;
                        m_pointTXe.x = m_pPlot->plint.lineRect[jj].right;
                        m_pointTXe.y = m_pPlot->plint.lineRect[jj].top;
                    }

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
                else if (SigmaApp.PtOnLine(&(m_pPlot->plint.lineRect[jj]), tPoint, m_pPlot->line[jj].orient)) {
                    bItemSelected = TRUE;
                    m_nLineSelected = jj;
                    m_nMovingLine = jj;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    if (m_pPlot->line[jj].orient == 0) {
                        m_pointTX.x = m_pPlot->plint.lineRect[jj].left;
                        m_pointTX.y = m_pPlot->plint.lineRect[jj].top;
                        m_pointTXe.x = m_pPlot->plint.lineRect[jj].right;
                        m_pointTXe.y = m_pPlot->plint.lineRect[jj].bottom;
                    }
                    else {
                        m_pointTX.x = m_pPlot->plint.lineRect[jj].left;
                        m_pointTX.y = m_pPlot->plint.lineRect[jj].bottom;
                        m_pointTXe.x = m_pPlot->plint.lineRect[jj].right;
                        m_pointTXe.y = m_pPlot->plint.lineRect[jj].top;
                    }

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
            }
        }
    }

    if (bItemSelected == FALSE) {
        if (m_pPlot->rectanglecount > 0) {
            for (int_t jj = 0; jj < m_pPlot->rectanglecount; jj++) {
                recT.left = m_pPlot->plint.rectangleRect[jj].left - PL_RESIZE;
                recT.top = m_pPlot->plint.rectangleRect[jj].top - PL_RESIZE;
                recT.right = m_pPlot->plint.rectangleRect[jj].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.rectangleRect[jj].top + PL_RESIZE;
                recTT.left = m_pPlot->plint.rectangleRect[jj].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.rectangleRect[jj].bottom - PL_RESIZE;
                recTT.right = m_pPlot->plint.rectangleRect[jj].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.rectangleRect[jj].bottom + PL_RESIZE;
                // Resize or Move
                if (PtInRect(&recT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nRectSelected = jj;
                    m_nResizingRect = jj;
                    m_nResizingRectT = 0;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    m_recTX = m_pPlot->plint.rectangleRect[jj];

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
                else if (PtInRect(&recTT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nRectSelected = jj;
                    m_nResizingRect = jj;
                    m_nResizingRectT = 1;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    m_recTX = m_pPlot->plint.rectangleRect[jj];

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
                else if (PtInRect(&(m_pPlot->plint.rectangleRect[jj]), tPoint)) {
                    bItemSelected = TRUE;
                    m_nRectSelected = jj;
                    m_nMovingRect = jj;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    m_recTX = m_pPlot->plint.rectangleRect[jj];

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
            }
        }
    }

    if (bItemSelected == FALSE) {
        if (m_pPlot->ellipsecount > 0) {
            for (int_t jj = 0; jj < m_pPlot->ellipsecount; jj++) {
                recT.left = m_pPlot->plint.ellipseRect[jj].left - PL_RESIZE;
                recT.top = m_pPlot->plint.ellipseRect[jj].top - PL_RESIZE;
                recT.right = m_pPlot->plint.ellipseRect[jj].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.ellipseRect[jj].top + PL_RESIZE;
                recTT.left = m_pPlot->plint.ellipseRect[jj].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.ellipseRect[jj].bottom - PL_RESIZE;
                recTT.right = m_pPlot->plint.ellipseRect[jj].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.ellipseRect[jj].bottom + PL_RESIZE;
                // Resize or Move
                if (PtInRect(&recT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nEllipseSelected = jj;
                    m_nResizingEllipse = jj;
                    m_nResizingEllipseT = 0;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    m_recTX = m_pPlot->plint.ellipseRect[jj];

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
                else if (PtInRect(&recTT, tPoint)) {
                    bItemSelected = TRUE;
                    m_nEllipseSelected = jj;
                    m_nResizingEllipse = jj;
                    m_nResizingEllipseT = 1;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    m_recTX = m_pPlot->plint.ellipseRect[jj];

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
                else if (PtInRect(&(m_pPlot->plint.ellipseRect[jj]), tPoint)) {
                    bItemSelected = TRUE;
                    m_nEllipseSelected = jj;
                    m_nMovingEllipse = jj;
                    m_ptInit.x = tPoint.x;
                    m_ptInit.y = tPoint.y;

                    m_recTX = m_pPlot->plint.ellipseRect[jj];

                    SetCapture();
                    ::SetCursor(m_hMCursor);
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                    return;
                }
            }
        }
    }

    if (bItemSelectedPrev) {
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }

    // Rectangle des légendes des courbes
    if (PtInRect(&m_recLegend, tPoint)) {
        m_nMovingLegend = 0;
        m_nLegendSelected = 0;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_recLegend;

        SetCapture();
        ::SetCursor(m_hMCursor);
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        return;
    }

    // Rectangle du titre de l'axe X
    if (PtInRect(&m_recBottomTitle, tPoint) && (m_pPlot->axisbottom.otitle == 1)) {
        m_nMovingBottomTitle = 0;
        m_nTitleSelected = 0;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_recBottomTitle;

        SetCapture();
        ::SetCursor(m_hMCursor);
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        return;
    }

    // Rectangle du titre de l'axe Y
    if (PtInRect(&m_recLeftTitle, tPoint) && (m_pPlot->axisleft.otitle == 1)) {
        m_nMovingLeftTitle = 0;
        m_nTitleSelected = 1;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_recLeftTitle;

        SetCapture();
        ::SetCursor(m_hMCursor);
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        return;
    }

    // Rectangle du titre de l'axe X2
    if (PtInRect(&m_recTopTitle, tPoint) && (m_pPlot->axistop.otitle == 1)) {
        m_nMovingTopTitle = 0;
        m_nTitleSelected = 2;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_recTopTitle;

        SetCapture();
        ::SetCursor(m_hMCursor);
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        return;
    }

    // Rectangle du titre de l'axe Y2
    if (PtInRect(&m_recRightTitle, tPoint) && (m_pPlot->axisright.otitle == 1)) {
        m_nMovingRightTitle = 0;
        m_nTitleSelected = 3;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_recRightTitle;

        SetCapture();
        ::SetCursor(m_hMCursor);
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        return;
    }

    // Redimensionner ou Zoomer le tracé
    if (PtInRect(&m_recMov[0], tPoint)) {
        m_bPlotSelected = TRUE;
        m_nResizingPlot = 0;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_pPlot->plint.frameRect[1];

        SetCapture();
        if (bPlotSelected != m_bPlotSelected) {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        return;
    }
    else if (PtInRect(&m_recMov[1], tPoint)) {
        m_bPlotSelected = TRUE;
        m_nResizingPlot = 1;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_pPlot->plint.frameRect[1];

        SetCapture();
        if (bPlotSelected != m_bPlotSelected) {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        return;
    }
    else if (PtInRect(&m_recMov[2], tPoint)) {
        m_bPlotSelected = TRUE;
        m_nResizingPlot = 2;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_pPlot->plint.frameRect[1];

        SetCapture();
        if (bPlotSelected != m_bPlotSelected) {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        return;
    }
    else if (PtInRect(&m_recMov[3], tPoint)) {
        m_bPlotSelected = TRUE;
        m_nResizingPlot = 3;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_pPlot->plint.frameRect[1];

        SetCapture();
        if (bPlotSelected != m_bPlotSelected) {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        return;
    }
    else if (PtInRect(&m_recMov[4], tPoint)) {
        m_bPlotSelected = TRUE;
        m_nResizingPlot = 4;
        m_ptInit.x = tPoint.x;
        m_ptInit.y = tPoint.y;

        m_recTX = m_pPlot->plint.frameRect[1];

        SetCapture();
        if (bPlotSelected != m_bPlotSelected) {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        return;
    }
    else if (PtInRect(&(m_pPlot->plint.frameRect[1]), tPoint)) {
        if (m_bZoomIn == TRUE) {
            if (m_pPlot->errcode == 0) { // If previously reported error don't zoom
                m_bZooming = TRUE;
                m_ptEnd.x = m_ptBegin.x;
                m_ptEnd.y = m_ptBegin.y;

                m_recTX = m_pPlot->plint.frameRect[1];

                SetCapture();
                if (bPlotSelected != m_bPlotSelected) {
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                }
                return;
            }
        }
        else {
            m_bPlotSelected = TRUE;
            if (bPlotSelected != m_bPlotSelected) {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
            return;
        }
    }
    else {
        m_bPlotSelected = FALSE;
        if (bPlotSelected != m_bPlotSelected) {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        return;
    }

    return;
}

void CSigmaViewPlot::OnRButtonUp(UINT nFlags, CPoint tPoint)
{
    m_ptBegin = tPoint;

    if (m_ItemCopy.isset) {
        m_ItemCopy.pastex = (int_t) (tPoint.x);
        m_ItemCopy.pastey = (int_t) (tPoint.y);
    }

    CView::OnRButtonUp(nFlags, tPoint);
}

void CSigmaViewPlot::OnLButtonUp(UINT nFlags, CPoint tPoint)
{
    int_t jj;
    real_t fw, fh;

    CView::OnLButtonUp(nFlags, tPoint);

    if (m_pPlot == NULL) {
        if (GetCapture() == this) {
            ReleaseCapture();
        }
        return;
    }

    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        if (GetCapture() == this) {
            ReleaseCapture();
        }
        return;
    }

    // Add Items
    if ((m_bAddText == TRUE) && (m_bAddingText == TRUE)) {
        if ((m_pPlot->textcount < 0) || (m_pPlot->textcount >= PL_MAXITEMS)) {
            if (GetCapture() == this) {
                ReleaseCapture();
            }
            m_bAddText = FALSE;
            m_bAddingText = FALSE;
            return;
        }

        CTextDlg inputDlg;
        inputDlg.m_strLabel = _T("Text:");
        inputDlg.m_pPlot = m_pPlot;
        inputDlg.m_pDoc = (CDocument*)pDoc;
        RECT rcT = {0,0,0,0};
        GetClientRect(&rcT);
        inputDlg.m_recFrame.bottom = rcT.bottom;
        inputDlg.m_recFrame.left = rcT.left;
        inputDlg.m_recFrame.top = rcT.top;
        inputDlg.m_recFrame.right = rcT.right;
        inputDlg.m_ptInit.x = m_ptInit.x;
        inputDlg.m_ptInit.y = m_ptInit.y;
        ClientToScreen(&(inputDlg.m_recFrame));
        ClientToScreen(&(inputDlg.m_ptInit));
        if ((inputDlg.m_ptInit.x < inputDlg.m_recFrame.left) || (inputDlg.m_ptInit.x > inputDlg.m_recFrame.right)
            || (inputDlg.m_ptInit.y < inputDlg.m_recFrame.top) || (inputDlg.m_ptInit.y > inputDlg.m_recFrame.bottom)
            || (inputDlg.m_recFrame.bottom == inputDlg.m_recFrame.top)
            || (inputDlg.m_recFrame.left == inputDlg.m_recFrame.right)) {
            
            if (GetCapture() == this) {
                ReleaseCapture();
            }
            m_bAddText = FALSE;
            m_bAddingText = FALSE;
            return;
        }
        inputDlg.DoModal();
        m_bAddText = FALSE;
        m_bAddingText = FALSE;

        if (inputDlg.m_bOK) {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();
        }

        if (GetCapture() == this) {
            ReleaseCapture();
        }
        return;
    }
    // 

    if (GetCapture() == this) {

        ReleaseCapture();

        real_t wx, wy, dx, dy;
        rect_t *precT = NULL;

        if ((m_bAddLine == TRUE) && (m_bAddingLine == TRUE)) {
            if ((m_pPlot->linecount < 0) || (m_pPlot->linecount >= PL_MAXITEMS)) {
                m_bAddingLine = FALSE;
                return;
            }
            m_bAddLine = FALSE;
            m_bAddingLine = FALSE;
            if ((m_ptEnd.x != m_ptBegin.x) || (m_ptEnd.y != m_ptBegin.y)) {
                PlotAddGeomtric(m_ptEnd.x, m_ptEnd.y, m_ptBegin.x, m_ptBegin.y, 0);
                pDoc->SetModifiedFlag(TRUE);
                pDoc->SaveState();
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }

            return;
        }
        else if ((m_bAddRect == TRUE) && (m_bAddingRect == TRUE)) {
            if ((m_pPlot->rectanglecount < 0) || (m_pPlot->rectanglecount >= PL_MAXITEMS)) {
                m_bAddingRect = FALSE;
                return;
            }
            m_bAddRect = FALSE;
            m_bAddingRect = FALSE;
            if ((m_ptEnd.x != m_ptBegin.x) && (m_ptEnd.y != m_ptBegin.y)) {
                PlotAddGeomtric(m_ptEnd.x, m_ptEnd.y, m_ptBegin.x, m_ptBegin.y, 1);
                pDoc->SetModifiedFlag(TRUE);
                pDoc->SaveState();
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }

            return;
        }
        else if ((m_bAddEllipse == TRUE) && (m_bAddingEllipse == TRUE)) {
            if ((m_pPlot->ellipsecount < 0) || (m_pPlot->ellipsecount >= PL_MAXITEMS)) {
                m_bAddingEllipse = FALSE;
                return;
            }
            m_bAddEllipse = FALSE;
            m_bAddingEllipse = FALSE;
            if ((m_ptEnd.x != m_ptBegin.x) && (m_ptEnd.y != m_ptBegin.y)) {
                PlotAddGeomtric(m_ptEnd.x, m_ptEnd.y, m_ptBegin.x, m_ptBegin.y, 2);
                pDoc->SetModifiedFlag(TRUE);
                pDoc->SaveState();
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }

            return;
        }
        else if ((m_nResizingLine >= 0) || (m_nResizingRect >= 0) || (m_nResizingEllipse >= 0)) {
            dx = (real_t) (tPoint.x - m_ptInit.x) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
            dy = (real_t) (tPoint.y - m_ptInit.y) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);

            bool bFlag = false;
            if (m_nResizingLine >= 0) {
                bFlag = (m_nResizingLineT == 0);
                precT = &(m_pPlot->line[m_nResizingLine].rect);
            }
            else if (m_nResizingRect >= 0) {
                bFlag = (m_nResizingRectT == 0);
                precT = &(m_pPlot->rectangle[m_nResizingRect].rect);
            }
            else if (m_nResizingEllipse >= 0) {
                bFlag = (m_nResizingEllipseT == 0);
                precT = &(m_pPlot->ellipse[m_nResizingEllipse].rect);
            }
            if (bFlag) {
                precT->xa += dx;
                precT->ya += dy;
            }
            else {
                precT->xb += dx;
                precT->yb += dy;
            }
            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            m_nResizingLine = -1;
            m_nResizingLineT = -1;
            m_nResizingRect = -1;
            m_nResizingRectT = -1;
            m_nResizingEllipse = -1;
            m_nResizingEllipseT = -1;

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

            return;
        }
        else if ((m_nMovingText >= 0) || (m_nMovingLine >= 0) || (m_nMovingRect >= 0) || (m_nMovingEllipse >= 0)) {
            precT = &(m_pPlot->text[m_nMovingText].rect);
            if (m_nMovingLine >= 0) {
                precT = &(m_pPlot->line[m_nMovingLine].rect);
            }
            else if (m_nMovingRect >= 0) {
                precT = &(m_pPlot->rectangle[m_nMovingRect].rect);
            }
            else if (m_nMovingEllipse >= 0) {
                precT = &(m_pPlot->ellipse[m_nMovingEllipse].rect);
            }
            wx = precT->xb - precT->xa;
            wy = precT->yb - precT->ya;
            dx = (real_t) (tPoint.x - m_ptInit.x) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
            dy = (real_t) (tPoint.y - m_ptInit.y) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);

            precT->xa += dx;
            precT->ya += dy;
            precT->xb = precT->xa + wx;
            precT->yb = precT->ya + wy;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            m_nMovingText = -1;
            m_nMovingLine = -1;
            m_nMovingRect = -1;
            m_nMovingEllipse = -1;

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

            return;
        }
        else if (m_nMovingLegend >= 0) {
            for (jj = 0; jj < m_pPlot->curvecount; jj++) {
                wx = m_pPlot->curve[jj].lrelpos.xb - m_pPlot->curve[jj].lrelpos.xa;
                wy = m_pPlot->curve[jj].lrelpos.yb - m_pPlot->curve[jj].lrelpos.ya;
                dx = (real_t) (tPoint.x - m_ptInit.x) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
                dy = (real_t) (tPoint.y - m_ptInit.y) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);
                m_pPlot->curve[jj].lrelpos.xa += dx;
                m_pPlot->curve[jj].lrelpos.ya += dy;
                m_pPlot->curve[jj].lrelpos.xb = m_pPlot->curve[jj].lrelpos.xa + wx;
                m_pPlot->curve[jj].lrelpos.yb = m_pPlot->curve[jj].lrelpos.ya + wy;
            }

            m_nMovingLegend = -1;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

            return;
        }
        else if ((m_nMovingBottomTitle >= 0) || (m_nMovingLeftTitle >= 0) || (m_nMovingTopTitle >= 0) || (m_nMovingRightTitle >= 0)) {
            precT = &(m_pPlot->axisbottom.trelpos);
            if (m_nMovingLeftTitle >= 0) {
                precT = &(m_pPlot->axisleft.trelpos);
            }
            else if (m_nMovingTopTitle >= 0) {
                precT = &(m_pPlot->axistop.trelpos);
            }
            else if (m_nMovingRightTitle >= 0) {
                precT = &(m_pPlot->axisright.trelpos);
            }

            wx = precT->xb - precT->xa;
            wy = precT->yb - precT->ya;
            dx = (real_t) (tPoint.x - m_ptInit.x) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
            dy = (real_t) (tPoint.y - m_ptInit.y) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);

            precT->xa += dx;
            precT->ya += dy;
            precT->xb = precT->xa + wx;
            precT->yb = precT->ya + wy;

            m_nMovingBottomTitle = -1;
            m_nMovingLeftTitle = -1;
            m_nMovingTopTitle = -1;
            m_nMovingRightTitle = -1;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

            return;
        }
        else if (m_bZooming) {
            m_bZooming = FALSE;
            if ((m_ptEnd.x != m_ptBegin.x) && (m_ptEnd.y != m_ptBegin.y)) {
                PlotZoomIn(m_ptEnd.x, m_ptEnd.y, m_ptBegin.x, m_ptBegin.y);
                ::SendMessage(GetSafeHwnd(), WM_PAINT, 0, 0);    // [FIX-SG244-004]
            }
            return;
        }
        else if (m_nResizingPlot >= 0) {
            fw = (real_t) (m_recTX.right - m_recTX.left) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
            fh = (real_t) (m_recTX.bottom - m_recTX.top) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);

            switch (m_nResizingPlot) {
                case 0:
                    m_pPlot->relpos[1].yb = m_pPlot->relpos[1].ya + fh;
                    break;
                case 1:
                    m_pPlot->relpos[1].xa = m_pPlot->relpos[1].xb - fw;
                    break;
                case 2:
                    m_pPlot->relpos[1].ya = m_pPlot->relpos[1].yb - fh;
                    break;
                case 3:
                    m_pPlot->relpos[1].xb = m_pPlot->relpos[1].xa + fw;
                    break;
                case 4:
                    m_pPlot->relpos[1].xa = (real_t) (m_recTX.left) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
                    m_pPlot->relpos[1].xb = (real_t) (m_recTX.right) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
                    m_pPlot->relpos[1].ya = (real_t) (m_recTX.top) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);
                    m_pPlot->relpos[1].yb = (real_t) (m_recTX.bottom) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);
                    break;
                default:
                    m_pPlot->relpos[1].yb = m_pPlot->relpos[1].ya + fh;
                    break;
            }

            // >> Change also the "relative" position of rectangles if necessary

            // RECT XLABEL
            m_pPlot->axisbottom.lrelpos.xa = m_pPlot->relpos[1].xa;             m_pPlot->axisbottom.lrelpos.xb = m_pPlot->relpos[1].xb;
            m_pPlot->axisbottom.lrelpos.ya = m_pPlot->relpos[1].yb + 0.01;      m_pPlot->axisbottom.lrelpos.yb = m_pPlot->axisbottom.lrelpos.ya + 0.04;

            // RECT YLABEL
            m_pPlot->axisleft.lrelpos.xb = m_pPlot->relpos[1].xa - 0.01;        m_pPlot->axisleft.lrelpos.xa = m_pPlot->axisleft.lrelpos.xb - 0.04;
            m_pPlot->axisleft.lrelpos.ya = m_pPlot->relpos[1].ya;               m_pPlot->axisleft.lrelpos.yb = m_pPlot->relpos[1].yb;

            // <<

            m_nResizingPlot = -1;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            return;
        }
    }

    return;
}

void CSigmaViewPlot::OnMouseMove(UINT nFlags, CPoint tPoint)
{
    CView::OnMouseMove(nFlags, tPoint);

    real_t fx = 0.0, fy = 0.0;

    // >> Raffraichir l'affichage toutes les 50 ms
    int_t msT = Toc();
    if (msT <= 0) {
        Tic();
    }
    if (msT < 50) {
        if (GetPlotxy((int_t) (tPoint.x), (int_t) (tPoint.y), &fx, &fy, m_pPlot->curve[m_pPlot->curveactive].axisused) == FALSE) {
            SigmaApp.OutputSB((LPCTSTR)_T(""), 1);
            SigmaApp.OutputSB((LPCTSTR)_T(""), 2);
        }
        return;
    }
    if (msT >= 50) {
        Tic();
    }
    // <<

    if (m_pPlot == NULL) {
        return;
    }

    // Print x and y on the status bar
    real_t wx, wy, dx, dy, moveX, moveY;
    BOOL bLeft = TRUE, bRight = TRUE, bBottom = TRUE, bTop = TRUE;
    LONG uiDeltaX = 0, uiDeltaY = 0, uiDeltaXi = 0, uiDeltaYi = 0, uiDeltaXe = 0, uiDeltaYe = 0;

    CString strT = _T("");
    char_t szT[ML_STRSIZES];
    if (GetPlotxy((int_t) (tPoint.x), (int_t) (tPoint.y), &fx, &fy, m_pPlot->curve[m_pPlot->curveactive].axisused)) {
        real_t flx = 0.0, fly = 0.0;
        char_t szLx[ML_STRSIZE];
        char_t szLy[ML_STRSIZE];
        memset(szLx, 0, ML_STRSIZE * sizeof(char_t));
        memset(szLy, 0, ML_STRSIZE * sizeof(char_t));
        BOOL bLink = AxisLinkParse(fx, fy, &flx, &fly, szLx, szLy);

        SigmaApp.Reformat(fx, szT, ML_STRSIZES - 1);
        strT = _T("x = ");
        strT += (LPCTSTR)szT;
        if (bLink && (szLx[0] != _T('\0'))) {
            strT += _T("   [ ");
            strT += (LPCTSTR)szLx;
            strT += _T(" ] ");
        }
        else {
            strT += _T(" ");
        }

        SigmaApp.ResizePane(1, strT);

        SigmaApp.OutputSB(strT, 1);

        SigmaApp.Reformat(fy, szT, ML_STRSIZES - 1);
        strT = _T("y = ");
        strT += (LPCTSTR)szT;
        if (bLink && (szLy[0] != _T('\0'))) {
            strT += _T("   [ ");
            strT += (LPCTSTR)szLy;
            strT += _T(" ] ");
        }
        else {
            strT += _T(" ");
        }

        SigmaApp.ResizePane(2, strT);

        SigmaApp.OutputSB(strT, 2);
    }
    else {
        SigmaApp.OutputSB((LPCTSTR)_T(""), 1);
        SigmaApp.OutputSB((LPCTSTR)_T(""), 2);
    }

    RECT recTmp;
    RECT *precTmp = NULL;

    if (((nFlags & MK_LBUTTON) && (GetCapture() == this))
        && ((m_bZooming == TRUE) || (m_nResizingPlot >= 0)
        || (m_nMovingText >= 0) || (m_nMovingLine >= 0)
        || (m_nMovingRect >= 0) || (m_nMovingEllipse >= 0)
        || (m_nMovingLegend >= 0) || (m_nMovingBottomTitle >= 0)
        || (m_nMovingLeftTitle >= 0) || (m_nMovingTopTitle >= 0)
        || (m_nMovingRightTitle >= 0)
        || (m_nResizingLine >= 0) || (m_nResizingRect >= 0)
        || (m_nResizingEllipse >= 0)
        || (m_bAddingLine == TRUE) || (m_bAddingRect == TRUE)
        || (m_bAddingEllipse == TRUE))) {

        uiDeltaX = (tPoint.x > m_ptBegin.x) ? (tPoint.x - m_ptBegin.x) : (m_ptBegin.x - tPoint.x);
        uiDeltaY = (tPoint.y > m_ptBegin.y) ? (tPoint.y - m_ptBegin.y) : (m_ptBegin.y - tPoint.y);
        uiDeltaXi = (tPoint.x > m_ptInit.x) ? (tPoint.x - m_ptInit.x) : (m_ptInit.x - tPoint.x);
        uiDeltaYi = (tPoint.y > m_ptInit.y) ? (tPoint.y - m_ptInit.y) : (m_ptInit.y - tPoint.y);
        uiDeltaXe = (tPoint.x > m_ptEnd.x) ? (tPoint.x - m_ptEnd.x) : (m_ptEnd.x - tPoint.x);
        uiDeltaYe = (tPoint.y > m_ptEnd.y) ? (tPoint.y - m_ptEnd.y) : (m_ptEnd.y - tPoint.y);

        if (m_bAddingLine == TRUE) {
            m_ptEnd.x = tPoint.x;
            m_ptEnd.y = tPoint.y;

            m_pointTX.x = m_ptBegin.x;
            m_pointTX.y = m_ptBegin.y;
            m_pointTXe.x = m_ptEnd.x;
            m_pointTXe.y = m_ptEnd.y;

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        else if (m_bAddingRect == TRUE) {
            if ((uiDeltaX >= DELTA_MIN) || (uiDeltaY >= DELTA_MIN)) {
                m_ptEnd.x = tPoint.x;
                m_ptEnd.y = tPoint.y;
                recTmp.left =  m_ptBegin.x;
                recTmp.top =  m_ptBegin.y;
                recTmp.right = m_ptEnd.x;
                recTmp.bottom = m_ptEnd.y;
                if (recTmp.bottom < recTmp.top) { ML_SWAPL(recTmp.bottom, recTmp.top); }
                if (recTmp.right < recTmp.left) { ML_SWAPL(recTmp.right, recTmp.left); }

                m_recTX = recTmp;

                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
        else if (m_bAddingEllipse == TRUE) {
            if ((uiDeltaX >= DELTA_MIN) || (uiDeltaY >= DELTA_MIN)) {
                m_recTX.left = m_ptBegin.x;
                m_recTX.top = m_ptBegin.y;
                m_ptEnd.x = tPoint.x;
                m_ptEnd.y = tPoint.y;
                m_recTX.right = m_ptEnd.x;
                m_recTX.bottom = m_ptEnd.y;

                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
        else if (m_nResizingLine >= 0) {
            recTmp.left = m_recLine[m_nResizingLine].left;
            recTmp.top = m_recLine[m_nResizingLine].top;
            recTmp.right = m_recLine[m_nResizingLine].right;
            recTmp.bottom = m_recLine[m_nResizingLine].bottom;

            if (m_pPlot->line[m_nResizingLine].orient == 0) {
                if (m_nResizingLineT == 1) {
                    recTmp.right += (tPoint.x - m_ptBegin.x);
                    recTmp.bottom += (tPoint.y - m_ptBegin.y);
                }
                else {
                    recTmp.left = (tPoint.x - m_ptBegin.x);
                    recTmp.top = (tPoint.y - m_ptBegin.y);
                }
            }
            else {
                if (m_nResizingLineT == 0) {
                    recTmp.left += (tPoint.x - m_ptBegin.x);
                    recTmp.bottom += (tPoint.y - m_ptBegin.y);
                }
                else {
                    recTmp.right += (tPoint.x - m_ptBegin.x);
                    recTmp.top += (tPoint.y - m_ptBegin.y);
                }
            }

            m_ptBegin.x = tPoint.x;
            m_ptBegin.y = tPoint.y;

            m_pointTXe = tPoint;

            m_recLine[m_nResizingLine].left = recTmp.left;
            m_recLine[m_nResizingLine].right = recTmp.right;
            m_recLine[m_nResizingLine].top = recTmp.top;
            m_recLine[m_nResizingLine].bottom = recTmp.bottom;

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        else if ((m_nResizingRect >= 0) || (m_nResizingEllipse >= 0)) {
            precTmp = (m_nResizingRect >= 0) ? &(m_recRect[m_nResizingRect]) : &(m_recEllipse[m_nResizingEllipse]);

            recTmp.left = precTmp->left;
            recTmp.top = precTmp->top;
            recTmp.right = precTmp->right;
            recTmp.bottom = precTmp->bottom;

            bool bFlag = (m_nResizingRect >= 0) ? (m_nResizingRectT == 1) : (m_nResizingEllipseT == 1);
            if (bFlag) {
                recTmp.right += (tPoint.x - m_ptBegin.x);
                recTmp.bottom += (tPoint.y - m_ptBegin.y);
                m_recTX.right += (tPoint.x - m_ptBegin.x);
                m_recTX.bottom += (tPoint.y - m_ptBegin.y);
            }
            else {
                recTmp.left += (tPoint.x - m_ptBegin.x);
                recTmp.top += (tPoint.y - m_ptBegin.y);
                m_recTX.left += (tPoint.x - m_ptBegin.x);
                m_recTX.top += (tPoint.y - m_ptBegin.y);
            }

            m_ptBegin.x = tPoint.x;
            m_ptBegin.y = tPoint.y;

            precTmp->left = recTmp.left;
            precTmp->right = recTmp.right;
            precTmp->top = recTmp.top;
            precTmp->bottom = recTmp.bottom;

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
        else if (m_nMovingLine >= 0) {
            POINT pointTX, pointTXe;
            pointTX.x = m_pointTX.x + (tPoint.x - m_ptBegin.x);
            pointTX.y = m_pointTX.y + (tPoint.y - m_ptBegin.y);
            pointTXe.x = m_pointTXe.x + (tPoint.x - m_ptBegin.x);
            pointTXe.y = m_pointTXe.y + (tPoint.y - m_ptBegin.y);

            if ((pointTX.x >= (m_pPlot->plint.frameRect[0].left)) && (pointTX.x <= (m_pPlot->plint.frameRect[0].right))
                && (pointTX.y >= (m_pPlot->plint.frameRect[0].top)) && (pointTX.y <= (m_pPlot->plint.frameRect[0].bottom))
                && (pointTXe.x >= (m_pPlot->plint.frameRect[0].left)) && (pointTXe.x <= (m_pPlot->plint.frameRect[0].right))
                && (pointTXe.y >= (m_pPlot->plint.frameRect[0].top)) && (pointTXe.y <= (m_pPlot->plint.frameRect[0].bottom))) {

                m_pointTX.x = pointTX.x;
                m_pointTX.y = pointTX.y;
                m_pointTXe.x = pointTXe.x;
                m_pointTXe.y = pointTXe.y;

                m_ptBegin.x = tPoint.x;
                m_ptBegin.y = tPoint.y;

                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
        else if ((m_nMovingText >= 0) || (m_nMovingRect >= 0) || (m_nMovingEllipse >= 0)) {
            recTmp.left = m_recTX.left + (tPoint.x - m_ptBegin.x);
            recTmp.right = m_recTX.right + (tPoint.x - m_ptBegin.x);
            recTmp.top = m_recTX.top + (tPoint.y - m_ptBegin.y);
            recTmp.bottom = m_recTX.bottom + (tPoint.y - m_ptBegin.y);
            if (recTmp.bottom < recTmp.top) { ML_SWAPL(recTmp.bottom, recTmp.top); }
            if (recTmp.right < recTmp.left) { ML_SWAPL(recTmp.right, recTmp.left); }

            if ((recTmp.left >= (m_pPlot->plint.frameRect[0].left)) && (recTmp.right <= (m_pPlot->plint.frameRect[0].right))
                && (recTmp.top >= (m_pPlot->plint.frameRect[0].top)) && (recTmp.bottom <= (m_pPlot->plint.frameRect[0].bottom))) {

                m_recTX.left = recTmp.left;
                m_recTX.right = recTmp.right;
                m_recTX.top = recTmp.top;
                m_recTX.bottom = recTmp.bottom;

                m_ptBegin.x = tPoint.x;
                m_ptBegin.y = tPoint.y;

                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
        else if ((m_nMovingLegend >= 0) || (m_nMovingBottomTitle >= 0) || (m_nMovingLeftTitle >= 0) || (m_nMovingTopTitle >= 0) || (m_nMovingRightTitle >= 0)) {

            recTmp.left = m_recTX.left + (tPoint.x - m_ptBegin.x);
            recTmp.right = m_recTX.right + (tPoint.x - m_ptBegin.x);
            recTmp.top = m_recTX.top + (tPoint.y - m_ptBegin.y);
            recTmp.bottom = m_recTX.bottom + (tPoint.y - m_ptBegin.y);
            if (recTmp.bottom < recTmp.top) { ML_SWAPL(recTmp.bottom, recTmp.top); }
            if (recTmp.right < recTmp.left) { ML_SWAPL(recTmp.right, recTmp.left); }

            if ((recTmp.left >= (m_pPlot->plint.frameRect[0].left + FRAME_MARGIN)) && (recTmp.right <= (m_pPlot->plint.frameRect[0].right - FRAME_MARGIN))
                && (recTmp.top >= (m_pPlot->plint.frameRect[0].top + FRAME_MARGIN)) && (recTmp.bottom <= (m_pPlot->plint.frameRect[0].bottom - FRAME_MARGIN))) {

                m_recTX.left = recTmp.left;
                m_recTX.right = recTmp.right;
                m_recTX.top = recTmp.top;
                m_recTX.bottom = recTmp.bottom;

                m_ptBegin.x = tPoint.x;
                m_ptBegin.y = tPoint.y;

                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
        else if (m_bZooming == TRUE) {
            if ((uiDeltaXe > DELTA_MIN) || (uiDeltaYe > DELTA_MIN)) {
                recTmp.left = m_ptBegin.x;
                recTmp.top = m_ptBegin.y;
                recTmp.right = m_ptEnd.x;
                recTmp.bottom = m_ptEnd.y;

                if (recTmp.bottom < recTmp.top) { ML_SWAPL(recTmp.bottom, recTmp.top); }
                if (recTmp.right < recTmp.left) { ML_SWAPL(recTmp.right, recTmp.left); }

                m_ptEnd.x = tPoint.x;
                m_ptEnd.y = tPoint.y;

                recTmp.left = m_ptBegin.x;
                recTmp.top = m_ptBegin.y;
                recTmp.right = m_ptEnd.x;
                recTmp.bottom = m_ptEnd.y;

                if (recTmp.bottom < recTmp.top) { ML_SWAPL(recTmp.bottom, recTmp.top); }
                if (recTmp.right < recTmp.left) { ML_SWAPL(recTmp.right, recTmp.left); }
                
                m_recTX.left = recTmp.left;
                m_recTX.top = recTmp.top;
                m_recTX.right = recTmp.right;
                m_recTX.bottom = recTmp.bottom;

                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
        else if (m_nResizingPlot >= 0) {
            if ((uiDeltaXi > DELTA_MIN) || (uiDeltaYi > DELTA_MIN)) {
                real_t fhmargin, fwmargin;

                fwmargin = 0.04 * ((real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left));
                fhmargin = 0.04 * ((real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top));

                switch (m_nResizingPlot) {
                    case 0:
                        m_recTX.bottom += (tPoint.y - m_ptInit.y);
                        if (m_recTX.bottom < (m_recTX.top + SIGMA_MINHEIGHT3)) {
                            m_recTX.bottom = m_recTX.top + SIGMA_MINHEIGHT3;
                        }
                        if (m_recTX.bottom > (m_pPlot->plint.frameRect[0].bottom - (LONG) fhmargin)) {
                            m_recTX.bottom = m_pPlot->plint.frameRect[0].bottom - (LONG) fhmargin;
                        }
                        break;
                    case 1:
                        m_recTX.left += (tPoint.x - m_ptInit.x);
                        if (m_recTX.left >(m_recTX.right - SIGMA_MINWIDTH3)) {
                            m_recTX.left = m_recTX.right - SIGMA_MINWIDTH3;
                        }
                        else if (m_recTX.left < (m_pPlot->plint.frameRect[0].left + (LONG) fwmargin)) {
                            m_recTX.left = m_pPlot->plint.frameRect[0].left + (LONG) fwmargin;
                        }
                        break;
                    case 2:
                        m_recTX.top += (tPoint.y - m_ptInit.y);
                        if (m_recTX.top > (m_recTX.bottom - SIGMA_MINHEIGHT3)) {
                            m_recTX.top = m_recTX.bottom - SIGMA_MINHEIGHT3;
                        }
                        else if (m_recTX.top < (m_pPlot->plint.frameRect[0].top + (LONG) fhmargin)) {
                            m_recTX.top = m_pPlot->plint.frameRect[0].top + (LONG) fhmargin;
                        }
                        break;
                    case 3:
                        m_recTX.right += (tPoint.x - m_ptInit.x);
                        if (m_recTX.right < (m_recTX.left + SIGMA_MINWIDTH3)) {
                            m_recTX.right = m_recTX.left + SIGMA_MINWIDTH3;
                        }
                        else if (m_recTX.right > (m_pPlot->plint.frameRect[0].right - (LONG) fwmargin)) {
                            m_recTX.right = m_pPlot->plint.frameRect[0].right - (LONG) fwmargin;
                        }
                        break;
                    case 4:    // Move without resizing
                        bLeft = TRUE;        bRight = TRUE;
                        bBottom = TRUE;    bTop = TRUE;

                        m_recTX.left += (tPoint.x - m_ptInit.x);
                        if (m_recTX.left < (m_pPlot->plint.frameRect[0].left + (LONG)fwmargin)) {
                            m_recTX.left = m_pPlot->plint.frameRect[0].left + (LONG)fwmargin;
                            bLeft = FALSE;
                        }
                        m_recTX.right = m_recTX.left + (m_pPlot->plint.frameRect[1].right - m_pPlot->plint.frameRect[1].left);
                        if (m_recTX.right > (m_pPlot->plint.frameRect[0].right - (LONG)fwmargin)) {
                            m_recTX.right = m_pPlot->plint.frameRect[0].right - (LONG)fwmargin;
                            m_recTX.left = m_recTX.right - (m_pPlot->plint.frameRect[1].right - m_pPlot->plint.frameRect[1].left);
                            bRight = FALSE;
                        }
                        m_recTX.top += (tPoint.y - m_ptInit.y);
                        if (m_recTX.top < (m_pPlot->plint.frameRect[0].top + (LONG)fhmargin)) {
                            m_recTX.top = m_pPlot->plint.frameRect[0].top + (LONG)fhmargin;
                            bTop = FALSE;
                        }
                        m_recTX.bottom = m_recTX.top + (m_pPlot->plint.frameRect[1].bottom - m_pPlot->plint.frameRect[1].top);
                        if (m_recTX.bottom > (m_pPlot->plint.frameRect[0].bottom - (LONG)fhmargin)) {
                            m_recTX.bottom = m_pPlot->plint.frameRect[0].bottom - (LONG)fhmargin;
                            m_recTX.top = m_recTX.bottom - (m_pPlot->plint.frameRect[1].bottom - m_pPlot->plint.frameRect[1].top);
                            bBottom = FALSE;
                        }
                        if ((bLeft == FALSE) || (bRight == FALSE) || (bTop == FALSE) || (bBottom == FALSE)) {
                            break;
                        }

                        dx = (real_t) (tPoint.x - m_ptInit.x) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
                        dy = (real_t) (tPoint.y - m_ptInit.y) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);
                        wx = m_pPlot->axisleft.trelpos.xb - m_pPlot->axisleft.trelpos.xa;
                        wy = m_pPlot->axisleft.trelpos.yb - m_pPlot->axisleft.trelpos.ya;
                        moveX = m_pPlot->axisleft.trelpos.xa + dx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axisleft.trelpos.xa = moveX;
                        }
                        moveY = m_pPlot->axisleft.trelpos.ya + dy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axisleft.trelpos.ya = moveY;
                        }
                        moveX = m_pPlot->axisleft.trelpos.xa + wx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axisleft.trelpos.xb = moveX;
                        }
                        moveY = m_pPlot->axisleft.trelpos.ya + wy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axisleft.trelpos.yb = moveY;
                        }
                        wx = m_pPlot->axisbottom.trelpos.xb - m_pPlot->axisbottom.trelpos.xa;
                        wy = m_pPlot->axisbottom.trelpos.yb - m_pPlot->axisbottom.trelpos.ya;
                        moveX = m_pPlot->axisbottom.trelpos.xa + dx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axisbottom.trelpos.xa = moveX;
                        }
                        moveY = m_pPlot->axisbottom.trelpos.ya + dy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axisbottom.trelpos.ya = moveY;
                        }
                        moveX = m_pPlot->axisbottom.trelpos.xa + wx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axisbottom.trelpos.xb = moveX;
                        }
                        moveY = m_pPlot->axisbottom.trelpos.ya + wy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axisbottom.trelpos.yb = moveY;
                        }

                        dx = (real_t) (tPoint.x - m_ptInit.x) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
                        dy = (real_t) (tPoint.y - m_ptInit.y) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);
                        wx = m_pPlot->axisright.trelpos.xb - m_pPlot->axisright.trelpos.xa;
                        wy = m_pPlot->axisright.trelpos.yb - m_pPlot->axisright.trelpos.ya;
                        moveX = m_pPlot->axisright.trelpos.xa + dx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axisright.trelpos.xa = moveX;
                        }
                        moveY = m_pPlot->axisright.trelpos.ya + dy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axisright.trelpos.ya = moveY;
                        }
                        moveX = m_pPlot->axisright.trelpos.xa + wx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axisright.trelpos.xb = moveX;
                        }
                        moveY = m_pPlot->axisright.trelpos.ya + wy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axisright.trelpos.yb = moveY;
                        }
                        wx = m_pPlot->axistop.trelpos.xb - m_pPlot->axistop.trelpos.xa;
                        wy = m_pPlot->axistop.trelpos.yb - m_pPlot->axistop.trelpos.ya;
                        moveX = m_pPlot->axistop.trelpos.xa + dx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axistop.trelpos.xa = moveX;
                        }
                        moveY = m_pPlot->axistop.trelpos.ya + dy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axistop.trelpos.ya = moveY;
                        }
                        moveX = m_pPlot->axistop.trelpos.xa + wx;
                        if ((moveX >= 0.0) && (moveX <= 1.0)) {
                            m_pPlot->axistop.trelpos.xb = moveX;
                        }
                        moveY = m_pPlot->axistop.trelpos.ya + wy;
                        if ((moveY >= 0.0) && (moveY <= 1.0)) {
                            m_pPlot->axistop.trelpos.yb = moveY;
                        }
                        break;
                    default:
                        m_recTX.bottom += (tPoint.y - m_ptInit.y);
                        if (m_recTX.bottom < (m_recTX.top + SIGMA_MINHEIGHT3)) {
                            m_recTX.bottom = m_recTX.top + SIGMA_MINHEIGHT3;
                        }
                        else if (m_recTX.bottom > (m_pPlot->plint.frameRect[0].bottom - (LONG) fhmargin)) {
                            m_recTX.bottom = m_pPlot->plint.frameRect[0].bottom - (LONG) fhmargin;
                        }
                        break;
                }

                m_ptInit.x = tPoint.x;
                m_ptInit.y = tPoint.y;

                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
    }

    return;
}

BOOL CSigmaViewPlot::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // ZOOM!!!!!!!!!!

    return CView::OnMouseWheel(nFlags, zDelta, pt);
}

UINT CSigmaViewPlot::OnGetDlgCode()
{
    UINT nCode = DLGC_WANTARROWS | DLGC_WANTCHARS;        // DLGC_WANTALLKEYS;

    return nCode;
}

// Move graphical objects with keyboard
void CSigmaViewPlot::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CView::OnKeyDown(nChar, nRepCnt, nFlags);

    if (m_pPlot == NULL) {
        return;
    }

    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return;
    }

    if (GetCapture() == this) {
        return;
    }

    if (nChar == VK_DELETE) {
        OnItemRemove();
        return;
    }

    LONG iDelta = CTRL_Pressed() ? 1L : 5L;
    if ((nChar != VK_RIGHT) && (nChar != VK_LEFT) && (nChar != VK_DOWN) && (nChar != VK_UP)) {
        return;
    }
    if ((nChar == VK_LEFT) || (nChar == VK_UP)) {
        iDelta = -iDelta;
    }

    RECT recTmp = { 0L, 0L, 0L, 0L };
    if ((nChar == VK_RIGHT) || (nChar == VK_LEFT)) {
        recTmp.left = iDelta;
        recTmp.right = iDelta;
    }
    else if ((nChar == VK_DOWN) || (nChar == VK_UP)) {
        recTmp.top = iDelta;
        recTmp.bottom = iDelta;
    }

    irect_t *precFrame = &(m_pPlot->plint.frameRect[0]);
    real_t wx, wy, xa, xb, ya, yb,
        dx = ((real_t) iDelta) / (real_t) (precFrame->right - precFrame->left),
        dy = ((real_t) iDelta) / (real_t) (precFrame->bottom - precFrame->top);

    if (m_nTextSelected >= 0) {
        recTmp.left += m_pPlot->plint.textRect[m_nTextSelected].left;
        recTmp.top += m_pPlot->plint.textRect[m_nTextSelected].top;
        recTmp.right += m_pPlot->plint.textRect[m_nTextSelected].right;
        recTmp.bottom += m_pPlot->plint.textRect[m_nTextSelected].bottom;

        if ((recTmp.left > (precFrame->left + FRAME_MARGIN)) && (recTmp.left < (precFrame->right - FRAME_MARGIN))
            && (recTmp.top > (precFrame->top + FRAME_MARGIN)) && (recTmp.top < (precFrame->bottom - FRAME_MARGIN))) {
            wx = m_pPlot->text[m_nTextSelected].rect.xb - m_pPlot->text[m_nTextSelected].rect.xa;
            wy = m_pPlot->text[m_nTextSelected].rect.yb - m_pPlot->text[m_nTextSelected].rect.ya;

            if ((nChar == VK_RIGHT) || (nChar == VK_LEFT)) {
                m_pPlot->text[m_nTextSelected].rect.xa += dx;
                m_pPlot->text[m_nTextSelected].rect.xb = m_pPlot->text[m_nTextSelected].rect.xa + wx;
            }
            else if ((nChar == VK_DOWN) || (nChar == VK_UP)) {
                m_pPlot->text[m_nTextSelected].rect.ya += dy;
                m_pPlot->text[m_nTextSelected].rect.yb = m_pPlot->text[m_nTextSelected].rect.ya + wy;
            }

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            xa = m_pPlot->text[m_nTextSelected].rect.xa;
            xb = m_pPlot->text[m_nTextSelected].rect.xb;
            ya = m_pPlot->text[m_nTextSelected].rect.ya;
            yb = m_pPlot->text[m_nTextSelected].rect.yb;

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

            if (((nChar == VK_UP) && (m_pPlot->text[m_nTextSelected].rect.ya > ya)) || ((nChar == VK_DOWN) && (m_pPlot->text[m_nTextSelected].rect.ya < ya))) {
                m_pPlot->text[m_nTextSelected].rect.ya += (1.5 * dy);
                m_pPlot->text[m_nTextSelected].rect.yb = m_pPlot->text[m_nTextSelected].rect.ya + wy;
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
            else if (((nChar == VK_LEFT) && (m_pPlot->text[m_nTextSelected].rect.xa > xa)) || ((nChar == VK_RIGHT) && (m_pPlot->text[m_nTextSelected].rect.xa < xa))) {
                m_pPlot->text[m_nTextSelected].rect.xa += (1.5 * dx);
                m_pPlot->text[m_nTextSelected].rect.xb = m_pPlot->text[m_nTextSelected].rect.xa + wx;
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
    }
    else if (m_nLineSelected >= 0) {
        recTmp.left += m_recLine[m_nLineSelected].left;
        recTmp.top += m_recLine[m_nLineSelected].top;
        recTmp.right += m_recLine[m_nLineSelected].right;
        recTmp.bottom += m_recLine[m_nLineSelected].bottom;

        if ((recTmp.left > (precFrame->left + FRAME_MARGIN)) && (recTmp.left < (precFrame->right - FRAME_MARGIN))
            && (recTmp.top >(precFrame->top + FRAME_MARGIN)) && (recTmp.top < (precFrame->bottom - FRAME_MARGIN))) {
            wx = m_pPlot->line[m_nLineSelected].rect.xb - m_pPlot->line[m_nLineSelected].rect.xa;
            wy = m_pPlot->line[m_nLineSelected].rect.yb - m_pPlot->line[m_nLineSelected].rect.ya;

            if ((nChar == VK_RIGHT) || (nChar == VK_LEFT)) {
                m_pPlot->line[m_nLineSelected].rect.xa += dx;
            }
            else if ((nChar == VK_DOWN) || (nChar == VK_UP)) {
                m_pPlot->line[m_nLineSelected].rect.ya += dy;
            }
            m_pPlot->line[m_nLineSelected].rect.xb = m_pPlot->line[m_nLineSelected].rect.xa + wx;
            m_pPlot->line[m_nLineSelected].rect.yb = m_pPlot->line[m_nLineSelected].rect.ya + wy;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
    else if (m_nRectSelected >= 0) {
        recTmp.left += m_recRect[m_nRectSelected].left;
        recTmp.top += m_recRect[m_nRectSelected].top;
        recTmp.right += m_recRect[m_nRectSelected].right;
        recTmp.bottom += m_recRect[m_nRectSelected].bottom;

        if ((recTmp.left > (precFrame->left + FRAME_MARGIN)) && (recTmp.left < (precFrame->right - FRAME_MARGIN))
            && (recTmp.top >(precFrame->top + FRAME_MARGIN)) && (recTmp.top < (precFrame->bottom - FRAME_MARGIN))) {
            wx = m_pPlot->rectangle[m_nRectSelected].rect.xb - m_pPlot->rectangle[m_nRectSelected].rect.xa;
            wy = m_pPlot->rectangle[m_nRectSelected].rect.yb - m_pPlot->rectangle[m_nRectSelected].rect.ya;

            if ((nChar == VK_RIGHT) || (nChar == VK_LEFT)) {
                m_pPlot->rectangle[m_nRectSelected].rect.xa += dx;
            }
            else if ((nChar == VK_DOWN) || (nChar == VK_UP)) {
                m_pPlot->rectangle[m_nRectSelected].rect.ya += dy;
            }
            m_pPlot->rectangle[m_nRectSelected].rect.xb = m_pPlot->rectangle[m_nRectSelected].rect.xa + wx;
            m_pPlot->rectangle[m_nRectSelected].rect.yb = m_pPlot->rectangle[m_nRectSelected].rect.ya + wy;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
    else if (m_nEllipseSelected >= 0) {
        recTmp.left += m_recEllipse[m_nEllipseSelected].left;
        recTmp.top += m_recEllipse[m_nEllipseSelected].top;
        recTmp.right += m_recEllipse[m_nEllipseSelected].right;
        recTmp.bottom += m_recEllipse[m_nEllipseSelected].bottom;

        if ((recTmp.left > (precFrame->left + FRAME_MARGIN)) && (recTmp.left < (precFrame->right - FRAME_MARGIN))
            && (recTmp.top >(precFrame->top + FRAME_MARGIN)) && (recTmp.top < (precFrame->bottom - FRAME_MARGIN))) {
            wx = m_pPlot->ellipse[m_nEllipseSelected].rect.xb - m_pPlot->ellipse[m_nEllipseSelected].rect.xa;
            wy = m_pPlot->ellipse[m_nEllipseSelected].rect.yb - m_pPlot->ellipse[m_nEllipseSelected].rect.ya;

            if ((nChar == VK_RIGHT) || (nChar == VK_LEFT)) {
                m_pPlot->ellipse[m_nEllipseSelected].rect.xa += dx;
            }
            else if ((nChar == VK_DOWN) || (nChar == VK_UP)) {
                m_pPlot->ellipse[m_nEllipseSelected].rect.ya += dy;
            }
            m_pPlot->ellipse[m_nEllipseSelected].rect.xb = m_pPlot->ellipse[m_nEllipseSelected].rect.xa + wx;
            m_pPlot->ellipse[m_nEllipseSelected].rect.yb = m_pPlot->ellipse[m_nEllipseSelected].rect.ya + wy;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
    else if (m_nLegendSelected >= 0) {
        recTmp.left += m_pPlot->plint.legendsRect.left;
        recTmp.top += m_pPlot->plint.legendsRect.top;
        recTmp.right += m_pPlot->plint.legendsRect.right;
        recTmp.bottom += m_pPlot->plint.legendsRect.bottom;

        if ((recTmp.left > (precFrame->left + FRAME_MARGIN)) && (recTmp.left < (precFrame->right - FRAME_MARGIN))
            && (recTmp.top >(precFrame->top + FRAME_MARGIN)) && (recTmp.top < (precFrame->bottom - FRAME_MARGIN))) {

            for (int_t jj = 0; jj < m_pPlot->curvecount; jj++) {
                wx = m_pPlot->curve[jj].lrelpos.xb - m_pPlot->curve[jj].lrelpos.xa;
                wy = m_pPlot->curve[jj].lrelpos.yb - m_pPlot->curve[jj].lrelpos.ya;
                if ((nChar == VK_RIGHT) || (nChar == VK_LEFT)) {
                    m_pPlot->curve[jj].lrelpos.xa += dx;
                }
                else if ((nChar == VK_DOWN) || (nChar == VK_UP)) {
                    m_pPlot->curve[jj].lrelpos.ya += dy;
                }
                m_pPlot->curve[jj].lrelpos.xb = m_pPlot->curve[jj].lrelpos.xa + wx;
                m_pPlot->curve[jj].lrelpos.yb = m_pPlot->curve[jj].lrelpos.ya + wy;
            }

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
    else if (m_nTitleSelected >= 0) {

        axis_t *pAxis = &(m_pPlot->axisbottom);
        if (m_nTitleSelected == 1) {
            pAxis = &(m_pPlot->axisleft);
        }
        else if (m_nTitleSelected == 2) {
            pAxis = &(m_pPlot->axistop);
        }
        if (m_nTitleSelected == 3) {
            pAxis = &(m_pPlot->axisright);
        }

        xa = pAxis->trelpos.xa;
        xb = pAxis->trelpos.xb;
        ya = pAxis->trelpos.ya;
        yb = pAxis->trelpos.yb;
        wx = xb - xa;
        wy = yb - ya;

        if ((nChar == VK_RIGHT) || (nChar == VK_LEFT)) {
            xa += dx;
        }
        else if ((nChar == VK_DOWN) || (nChar == VK_UP)) {
            ya += dy;
        }
        xb = xa + wx;
        yb = ya + wy;

        if ((xa > 0.0) && (xa < 1.0) && (xb > 0.0) && (xb < 1.0)
            && (ya > 0.0) && (ya < 1.0) && (yb > 0.0) && (yb < 1.0)) {
            pAxis->trelpos.xa = xa;
            pAxis->trelpos.xb = xb;
            pAxis->trelpos.ya = ya;
            pAxis->trelpos.yb = yb;

            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();

            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
}

void CSigmaViewPlot::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CWnd::OnKeyUp(nChar, nRepCnt, nFlags);

}

void CSigmaViewPlot::OnSizing(UINT fwSide, LPRECT pRect)
{
    CView::OnSizing(fwSide, pRect);

}

void CSigmaViewPlot::OnSize(UINT nType, int_t cx, int_t cy)
{

    CView::OnSize(nType, cx, cy);
}

void CSigmaViewPlot::ShowProperties(int_t nStartupPage/* = 0*/, int_t nCurve/* = 0*/, int_t nAxis/* = -1*/)
{
    if (m_pPlot == NULL) {
        return;
    }

    CPlotopt *plotOpt = new CPlotopt();
    if (plotOpt == NULL) {
        SigmaApp.Output(_T("Cannot show the 'Plot Options' dialog: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    // Options initialization
    plotOpt->m_pPlot = m_pPlot;

    // 0. Curve
    plotOpt->m_Curve.m_pPlot = m_pPlot;            // Initializations done in m_Curve::OnInitDialog()
    plotOpt->m_Curve.m_nCurve = nCurve;
    plotOpt->m_Curve.m_pDoc = (CDocument*)GetDocument();

    // 1. Axis
    plotOpt->m_Axis.m_pPlot = m_pPlot;            // Initializations done in m_Axis::OnInitDialog()
    plotOpt->m_Axis.m_bBottomAxis = FALSE;
    plotOpt->m_Axis.m_bLeftAxis = FALSE;
    plotOpt->m_Axis.m_bTopAxis = FALSE;
    plotOpt->m_Axis.m_bRightAxis = FALSE;
    switch (nAxis) {
        case 0:
            plotOpt->m_Axis.m_bBottomAxis = TRUE;
            break;
        case 1:
            plotOpt->m_Axis.m_bLeftAxis = TRUE;
            break;
        case 2:
            plotOpt->m_Axis.m_bTopAxis = TRUE;
            break;
        case 3:
            plotOpt->m_Axis.m_bRightAxis = TRUE;
            break;
        default:
            break;
    }

    plotOpt->m_Axis.m_pDoc = (CDocument*)GetDocument();

    // 2. Line
    plotOpt->m_Line.m_pPlot = m_pPlot;            // Initializations done in m_Line::OnInitDialog()
    plotOpt->m_Line.m_bLineHorz = TRUE;
    plotOpt->m_Line.m_bLineVert = FALSE;
    plotOpt->m_Line.m_pDoc = (CDocument*)GetDocument();

    // 3. Scale
    plotOpt->m_Scale.m_pPlot = m_pPlot;            // Initializations done in m_Scale::OnInitDialog()
    plotOpt->m_Scale.m_pDoc = (CDocument*)GetDocument();
    plotOpt->m_Scale.m_bBottomAxis = plotOpt->m_Axis.m_bBottomAxis;
    plotOpt->m_Scale.m_bLeftAxis = plotOpt->m_Axis.m_bLeftAxis;
    plotOpt->m_Scale.m_bTopAxis = plotOpt->m_Axis.m_bTopAxis;
    plotOpt->m_Scale.m_bRightAxis = plotOpt->m_Axis.m_bRightAxis;

    // 4.    General
    plotOpt->m_General.m_pPlot = m_pPlot;        // Initializations done in m_General::OnInitDialog()
    plotOpt->m_General.m_pDoc = (CDocument*)GetDocument();

    plotOpt->m_Curve.m_bInitialized = FALSE;
    plotOpt->m_Axis.m_bInitialized = FALSE;
    plotOpt->m_Line.m_bInitialized = FALSE;
    plotOpt->m_Scale.m_bInitialized = FALSE;
    plotOpt->m_General.m_bInitialized = FALSE;

    plotOpt->m_Curve.m_bIsModified = FALSE;
    plotOpt->m_Axis.m_bIsModified = FALSE;
    plotOpt->m_Line.m_bIsModified = FALSE;
    plotOpt->m_Scale.m_bIsModified = FALSE;
    plotOpt->m_General.m_bIsModified = FALSE;

    if ((nStartupPage == 0) && (m_pPlot->curvecount < 1)) {
        nStartupPage = 1;
    }
    plotOpt->SetActivePage(nStartupPage);

    plotOpt->DoModal();

    delete plotOpt; plotOpt = NULL;
}

void CSigmaViewPlot::OnPlotOptions()
{
    ShowProperties(0, m_nCurveSelected);
}

void CSigmaViewPlot::OnPlotAutoScale()
{
    m_bAutoScale = TRUE;
    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}

void CSigmaViewPlot::OnViewZoomIn()
{
    if (m_bZooming == TRUE) {
        return;
    }

    if (m_bZoomIn == FALSE) {
        m_bZoomIn = TRUE;

        // Réinitialiser toutes les autres action graphiques
        m_bAddText = FALSE;
        m_bAddLine = FALSE;
        m_bAddRect = FALSE;
        m_bAddEllipse = FALSE;

        m_bAddingText = FALSE;
        m_bAddingLine = FALSE;
        m_bAddingRect = FALSE;
        m_bAddingEllipse = FALSE;

        m_nMovingText = -1;
        m_nMovingLine = -1;
        m_nMovingRect = -1;
        m_nMovingEllipse = -1;
        m_nMovingLegend = -1;
        m_nMovingBottomTitle = -1;
        m_nMovingLeftTitle = -1;
        m_nMovingTopTitle = -1;
        m_nMovingRightTitle = -1;

        m_nResizingLine = -1;
        m_nResizingRect = -1;
        m_nResizingEllipse = -1;
        m_nResizingLineT = -1;
        m_nResizingRectT = -1;
        m_nResizingEllipseT = -1;

        m_nTextSelected = -1;
        m_nLineSelected = -1;
        m_nRectSelected = -1;
        m_nEllipseSelected = -1;

        m_nCurveSelected = -1;
    }
    else {
        // Desactivate ZoomIn Button and Menu Item
        m_bZoomIn = FALSE;
    }
}

void CSigmaViewPlot::OnViewZoomOut()
{
    if (m_bZooming == TRUE) {
        return;
    }

    m_bZoomIn = FALSE;

    PlotZoomOut();
    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

    // Réinitialiser toutes les autres actions graphiques
}

//    --------------------------------------------------------------------
//    LogScale        :    Calcul de l'échelle Log à partir du range linéaire
//    --------------------------------------------------------------------
void CSigmaViewPlot::LogScale(real_t fmin, real_t fmax, real_t *fminl, real_t *fmaxl, byte_t *gridcount/* = NULL*/)
{
    real_t tmpval;

    if (fmin > 0.0) {
        *fminl = floor(log10(fmin));
    }
    else {
        if (fmax > 0.0) {
            *fminl = floor(log10(fmax)) - 1.0;
        }
        else {
            *fminl = pl_logzero - 1.0;
        }
    }

    if (fmax > 0.0) {
        *fmaxl = log10(fmax);
        tmpval = floor(*fmaxl);
        if (tmpval != *fmaxl) { *fmaxl = tmpval + 1.0; }
        else { *fmaxl = tmpval; }
    }
    else {
        *fmaxl = *fminl + 1.0;
    }

    if (*fmaxl < *fminl) {
        tmpval = *fmaxl;
        *fmaxl = *fminl;
        *fminl = tmpval;
    }
    else if (*fmaxl == *fminl) {
        *fmaxl += 1.0;
    }

    if (gridcount) {
        *gridcount = (byte_t)(*fmaxl - *fminl);
    }

    *fminl = pow(10.0, *fminl);
    *fmaxl = pow(10.0, *fmaxl);

    return;
}

BOOL CSigmaViewPlot::GetPlotxy(int_t nx, int_t ny, real_t *xx, real_t *yy, byte_t axisused/* = 0x00*/)
{
    real_t tx, ty, xr, xmin, xmax, yr, ymin, ymax, ftmp, wx, wy, fwidth, fheight;
    axis_t *paxisX = NULL, *paxisY = NULL;

    if (m_pPlot == NULL) {
        return FALSE;
    }

    if ((nx < m_pPlot->plint.frameRect[1].left) || (nx > m_pPlot->plint.frameRect[1].right)) {
        return FALSE;
    }
    if ((ny < m_pPlot->plint.frameRect[1].top) || (ny > m_pPlot->plint.frameRect[1].bottom)) {
        return FALSE;
    }

    if ((axisused & 0xF0) == 0x00) {                // Axis X
        paxisX = &(m_pPlot->axisbottom);
    }
    else if ((axisused & 0xF0) == 0x10) {        // Axis X2
        paxisX = &(m_pPlot->axistop);
    }
    if ((axisused & 0x0F) == 0x00) {                // Axis Y
        paxisY = &(m_pPlot->axisleft);
    }
    else if ((axisused & 0x0F) == 0x01) {        // Axis Y2
        paxisY = &(m_pPlot->axisright);
    }

    if ((paxisX == NULL) || (paxisY == NULL)) {
        return FALSE;
    }

    fwidth = (real_t) (m_pPlot->plint.frameRect[1].right - m_pPlot->plint.frameRect[1].left);
    fheight = (real_t) (m_pPlot->plint.frameRect[1].bottom - m_pPlot->plint.frameRect[1].top);

    tx = (real_t)nx;
    ty = (real_t)ny;
    tx -= (real_t) (m_pPlot->plint.frameRect[1].left);
    ty -= (real_t) (m_pPlot->plint.frameRect[1].top);

    if (fwidth < 1.0) { return FALSE; }
    if (fheight < 1.0) { return FALSE; }

    if ((tx < 1.0) || (tx > fwidth)) { return FALSE; }
    if ((tx < 1.0) || (ty > fheight)) { return FALSE; }

    wx = tx / fwidth;
    wy = ty / fheight;

    // X ECHELLE LOG
    if (paxisX->scale == 1) {
        if (paxisX->min != 0.0) { xmin = floor(log10(paxisX->min)); }
        else {
            if (paxisX->max != 0.0) {
                xmin = floor(log10(paxisX->max)) - 1.0;
            }
            else {
                xmin = pl_logzero - 1.0;
            }
        }

        if (paxisX->max != 0.0) {
            xmax = log10(paxisX->max);
            ftmp = floor(xmax);
            if (ftmp != xmax) { xmax = ftmp + 1.0; }
            else { xmax = ftmp; }
        }
        else {
            xmax = xmin + 1.0;
        }

        if (xmax < xmin) {
            ftmp = xmax;
            xmax = xmin;
            xmin = ftmp;
        }
        else if (xmax == xmin) {
            xmax += 1.0;
        }

        xr = xmax - xmin;
        *xx = pow(10.0, xmin + (xr * wx));
    }
    // X ECHELLE LIN
    else {
        xr = paxisX->max - paxisX->min;
        *xx = paxisX->min + (xr * wx);
    }

    // Y ECHELLE LOG
    if (paxisY->scale == 1) {
        if (paxisY->min != 0.0) { ymin = floor(log10(paxisY->min)); }
        else {
            if (paxisY->max != 0.0) {
                ymin = floor(log10(paxisY->max)) - 1.0;
            }
            else {
                ymin = pl_logzero - 1.0;
            }
        }

        if (paxisY->max != 0.0) {
            ymax = log10(paxisY->max);
            ftmp = floor(ymax);
            if (ftmp != ymax) { ymax = ftmp + 1.0; }
            else { ymax = ftmp; }
        }
        else {
            ymax = ymin + 1.0;
        }

        if (ymax < ymin) {
            ftmp = ymax;
            ymax = ymin;
            ymin = ftmp;
        }
        else if (ymax == ymin) {
            ymax += 1.0;
        }

        yr = ymax - ymin;
        *yy = pow(10.0, ymax - (yr * wy));
    }
    // Y ECHELLE LIN
    else {
        yr = paxisY->max - paxisY->min;
        *yy = paxisY->max - (yr * wy);
    }

    return TRUE;    // OK
}

void CSigmaViewPlot::PlotAddGeomtric(int_t nx, int_t ny, int_t nxPrev, int_t nyPrev, int_t nGeo)
{
    real_t xa, xb, ya, yb, fwidth, fheight;
    int_t nT;

    if (m_pPlot == NULL) {
        return;
    }
    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return;
    }
    
    fwidth = (real_t) (m_pPlot->plint.width);
    fheight = (real_t) (m_pPlot->plint.height);

    if (fwidth < 1.0) { return; }
    if (fheight < 1.0) { return; }

    if (nx < nxPrev) {
        nT = nx; nx = nxPrev; nxPrev = nT;
        nT = ny; ny = nyPrev; nyPrev = nT;
    }

    nx -= (int_t) (m_pPlot->plint.frameRect[0].left);    nxPrev -= (int_t) (m_pPlot->plint.frameRect[0].left);
    ny -= (int_t) (m_pPlot->plint.frameRect[0].top);        nyPrev -= (int_t) (m_pPlot->plint.frameRect[0].top);

    xa = ((real_t)nxPrev) / fwidth;
    ya = ((real_t)nyPrev) / fheight;
    xb = ((real_t)nx) / fwidth;
    yb = ((real_t)ny) / fheight;

    switch (nGeo) {
        case 0:        // line
            if ((m_pPlot->linecount < 0) || (m_pPlot->linecount >= PL_MAXITEMS)) {
                return;
            }
            m_pPlot->line[m_pPlot->linecount].rect.xa = xa;
            m_pPlot->line[m_pPlot->linecount].rect.xb = xb;
            m_pPlot->line[m_pPlot->linecount].rect.ya = ya;
            m_pPlot->line[m_pPlot->linecount].rect.yb = yb;
            m_pPlot->line[m_pPlot->linecount].status = 0x00;
            m_pPlot->line[m_pPlot->linecount].style = 0x10;
            m_pPlot->line[m_pPlot->linecount].size = 1;
            m_pPlot->linecount += 1;
            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();
            break;
        case 1:        // rectangle
            if ((m_pPlot->rectanglecount < 0) || (m_pPlot->rectanglecount >= PL_MAXITEMS)) {
                return;
            }
            m_pPlot->rectangle[m_pPlot->rectanglecount].rect.xa = xa;
            m_pPlot->rectangle[m_pPlot->rectanglecount].rect.xb = xb;
            m_pPlot->rectangle[m_pPlot->rectanglecount].rect.ya = ya;
            m_pPlot->rectangle[m_pPlot->rectanglecount].rect.yb = yb;
            m_pPlot->rectangle[m_pPlot->rectanglecount].status = 0x00;
            m_pPlot->rectangle[m_pPlot->rectanglecount].style = 0x10;
            m_pPlot->rectangle[m_pPlot->rectanglecount].size = 1;
            m_pPlot->rectanglecount += 1;
            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();
            break;
        case 2:        // ellipse
            if ((m_pPlot->ellipsecount < 0) || (m_pPlot->ellipsecount >= PL_MAXITEMS)) {
                return;
            }
            m_pPlot->ellipse[m_pPlot->ellipsecount].rect.xa = xa;
            m_pPlot->ellipse[m_pPlot->ellipsecount].rect.xb = xb;
            m_pPlot->ellipse[m_pPlot->ellipsecount].rect.ya = ya;
            m_pPlot->ellipse[m_pPlot->ellipsecount].rect.yb = yb;
            m_pPlot->ellipse[m_pPlot->ellipsecount].status = 0x00;
            m_pPlot->ellipse[m_pPlot->ellipsecount].style = 0x10;
            m_pPlot->ellipse[m_pPlot->ellipsecount].size = 1;
            m_pPlot->ellipsecount += 1;
            pDoc->SetModifiedFlag(TRUE);
            pDoc->SaveState();
            break;
        default:
            break;
    }
}

void CSigmaViewPlot::PlotZoomIn(int_t nx, int_t ny, int_t nxPrev, int_t nyPrev)
{
    int_t ii;
    real_t wxa, wxb, wya, wyb, xr, xmin, xmax, yr, ymin, ymax, ftmp, fwidth, fheight;
    RECT recFrame = {0,0,0,0};
    const real_t xminrange = 1e-31;
    const real_t xminrangeL = 1.0;
    const real_t yminrange = 1e-31;
    const real_t yminrangeL = 1.0;
    axis_t *paxisX = NULL, *paxisY = NULL;
    byte_t bInt = 0;

    if (m_pPlot == NULL) {
        return;
    }

    if ((m_pPlot->curvecount > 0) && (m_pPlot->curveactive >= 0) && (m_pPlot->curveactive < m_pPlot->curvecount)) {
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0xF0) == 0x10) {        // X2-Axis
            paxisX = &(m_pPlot->axistop);
        }
        else {            // X-Axis
            paxisX = &(m_pPlot->axisbottom);
        }
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0x0F) == 0x01) {        // Y2-Axis
            paxisY = &(m_pPlot->axisright);
        }
        else {            // Y-Axis
            paxisY = &(m_pPlot->axisleft);
        }
        bInt = (m_pPlot->curve[m_pPlot->curveactive].x->id == (SIGMA_MAXCOLUMNS + 99));
    }
    else {            // Y-Axis
        paxisX = &(m_pPlot->axisbottom);
        paxisY = &(m_pPlot->axisleft);
    }

    double fxmin = paxisX->min, fxmax = paxisX->max,
        fymin = paxisY->min, fymax = paxisY->max;

    if (nx < nxPrev) {
        ii = nxPrev; nxPrev = nx; nx = ii;
    }
    if (ny < nyPrev) {
        ii = nyPrev; nyPrev = ny; ny = ii;
    }

    fwidth = (real_t) (m_pPlot->plint.frameRect[1].right - m_pPlot->plint.frameRect[1].left);
    fheight = (real_t) (m_pPlot->plint.frameRect[1].bottom - m_pPlot->plint.frameRect[1].top);

    if (fwidth < 1.0) { return; }
    if (fheight < 1.0) { return; }

    nx -= (int_t) (m_pPlot->plint.frameRect[1].left);    nxPrev -= (int_t) (m_pPlot->plint.frameRect[1].left);
    ny -= (int_t) (m_pPlot->plint.frameRect[1].top);    nyPrev -= (int_t) (m_pPlot->plint.frameRect[1].top);

    wxa = (real_t)nxPrev;
    wya = (real_t)nyPrev;
    wxb = (real_t)nx;
    wyb = (real_t)ny;

    if ((wxb < (wxa + 2.0)) || (wyb < (wya + 2.0))) {
        return;
    }

    wxb = fwidth - wxb;

    ftmp = wyb; wyb = wya; wya = fheight - ftmp;

    // Recadrer
    if (wxa < 1.0) { wxa = 1.0; }
    else if (wxa > fwidth) { wxa = fwidth; }
    if (wya < 1.0) { wya = 1.0; }
    else if (wya > fheight) { wya = fheight; }
    if (wxb < 1.0) { wxb = 1.0; }
    else if (wxb > fwidth) { wxb = fwidth; }
    if (wyb < 1.0) { wyb = 1.0; }
    else if (wyb > fheight) { wyb = fheight; }

    wxa /= fwidth; wxb /= fwidth;
    wya /= fheight; wyb /= fheight;

    // X
    // ECHELLE LOG
    if (paxisX->scale == 1) {
        if (paxisX->min > 0.0) {
            xmin = floor(log10(paxisX->min));
        }
        else {
            if (paxisX->max > 0.0) {
                xmin = floor(log10(paxisX->max)) - 1.0;
            }
            else {
                xmin = pl_logzero - 1.0;
            }
        }

        if (paxisX->max > 0.0) {
            xmax = log10(paxisX->max);
            ftmp = floor(xmax);
            if (ftmp != xmax) { xmax = ftmp + 1.0; }
            else { xmax = ftmp; }
        }
        else {
            xmax = xmin + 1.0;
        }

        if (xmax < xmin) {
            ftmp = xmax;
            xmax = xmin;
            xmin = ftmp;
        }
        else if (xmax == xmin) {
            xmax += 1.0;
        }

        xr = xmax - xmin;
        if (xr < xminrangeL) { return; }
        xmin += (xr * wxa);
        xmax -= (xr * wxb);
        paxisX->min = pow(10.0, xmin);
        paxisX->max = pow(10.0, xmax);
    }
    // ECHELLE LIN
    else {
        xr = paxisX->max - paxisX->min;
        if (xr < xminrange) { return; }
        paxisX->min += (xr * wxa);
        paxisX->max -= (xr * wxb);
    }

    // Y
    // ECHELLE LOG
    if (paxisY->scale == 1) {
        if (paxisY->min > 0.0) {
            ymin = floor(log10(paxisY->min));
        }
        else {
            if (paxisY->max > 0.0) {
                ymin = floor(log10(paxisY->max)) - 1.0;
            }
            else {
                ymin = pl_logzero - 1.0;
            }
        }

        if (paxisY->max > 0.0) {
            ymax = log10(paxisY->max);
            ftmp = floor(ymax);
            if (ftmp != ymax) { ymax = ftmp + 1.0; }
            else { ymax = ftmp; }
        }
        else {
            ymax = ymin + 1.0;
        }

        if (ymax < ymin) {
            ftmp = ymax;
            ymax = ymin;
            ymin = ftmp;
        }
        else if (ymax == ymin) {
            ymax += 1.0;
        }

        yr = ymax - ymin;
        if (yr < yminrangeL) { goto PlotZoomIn_RET; }
        ymin += (yr * wya);
        ymax -= (yr * wyb);
        paxisY->min = pow(10.0, ymin);
        paxisY->max = pow(10.0, ymax);
    }
    // ECHELLE LIN
    else {
        yr = paxisY->max - paxisY->min;
        if (yr < yminrange) { goto PlotZoomIn_RET; }
        paxisY->min += (yr * wya);
        paxisY->max -= (yr * wyb);
    }

PlotZoomIn_RET:
    if (ml_isNaN(paxisX->min) || ml_isNaN(paxisX->max) || ml_isNaN(paxisY->min) || ml_isNaN(paxisY->max)
        || ml_isEqual(paxisX->min, paxisX->max) || ml_isEqual(paxisY->min, paxisY->max)) {
        paxisX->min = fxmin;
        paxisX->max = fxmax;
        paxisY->min = fymin;
        paxisY->max = fymax;
        return;
    }

    if (paxisX->scale == 0) {
        double fstep = 0.1;
        int igrid = 5;
        double fmin = paxisX->min, fmax = paxisX->max;
        pl_autotick(&fmin, &fmax, &fstep, &igrid, paxisX->scale);
        paxisX->min = fmin;
        paxisX->max = fmax;
        paxisX->increment = fstep;
        paxisX->gridcount = igrid;
        paxisX->gridmcount = 1;
    }
    if (paxisY->scale == 0) {
        double fstep = 0.1;
        int igrid = 5;
        double fmin = paxisY->min, fmax = paxisY->max;
        pl_autotick(&fmin, &fmax, &fstep, &igrid, paxisY->scale);
        paxisY->min = fmin;
        paxisY->max = fmax;
        paxisY->increment = fstep;
        paxisY->gridcount = igrid;
        paxisY->gridmcount = 1;
    }

    return;
}

void CSigmaViewPlot::PlotZoomOut(void)
{
    real_t fratio, xr, xmin, xmax, yr, ymin, ymax, ftmp;
    const real_t xminrange = DBL_EPSILON;
    const real_t xminrangeL = 1.0;
    const real_t yminrange = DBL_EPSILON;
    const real_t yminrangeL = 1.0;
    axis_t *paxisX = NULL, *paxisY = NULL;
    byte_t bInt = 0;

    if (m_pPlot == NULL) {
        return;
    }

    if ((m_pPlot->curvecount > 0) && (m_pPlot->curveactive >= 0) && (m_pPlot->curveactive < m_pPlot->curvecount)) {
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0xF0) == 0x10) {        // X2-Axis
            paxisX = &(m_pPlot->axistop);
        }
        else {            // X-Axis
            paxisX = &(m_pPlot->axisbottom);
        }
        if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0x0F) == 0x01) {        // Y2-Axis
            paxisY = &(m_pPlot->axisright);
        }
        else {            // Y-Axis
            paxisY = &(m_pPlot->axisleft);
        }

        bInt = (m_pPlot->curve[m_pPlot->curveactive].x->id == (SIGMA_MAXCOLUMNS + 99));
    }
    else {            // Y-Axis
        paxisX = &(m_pPlot->axisbottom);
        paxisY = &(m_pPlot->axisleft);
    }

    double fxmin = paxisX->min, fxmax = paxisX->max,
        fymin = paxisY->min, fymax = paxisY->max;

    // Echelle max
    if (paxisX->scale == 0) {    // Linear Scale
        if (((paxisX->max - paxisX->min) > (10.0 * (m_fXmax - m_fXmin)))) {
            return;
        }
    }
    else {    // Log Scale
        if (((paxisX->max - paxisX->min) > (1000.0 * (m_fXmax - m_fXmin)))) {
            return;
        }
    }
    if (paxisY->scale == 0) {    // Linear Scale
        if (((paxisY->max - paxisY->min) > (10.0 * (m_fYmax - m_fYmin)))) {
            return;
        }
    }
    else {    // Log Scale
        if (((paxisY->max - paxisY->min) > (1000.0 * (m_fYmax - m_fYmin)))) {
            return;
        }
    }

    fratio = 0.2;

    // X
    // ECHELLE LOG
    if (paxisX->scale == 1) {
        if (paxisX->min > 0.0) {
            xmin = floor(log10(paxisX->min));
        }
        else {
            if (paxisX->max > 0.0) {
                xmin = floor(log10(paxisX->max)) - 1.0;
            }
            else {
                xmin = pl_logzero - 1.0;
            }
        }

        if (paxisX->max > 0.0) {
            xmax = log10(paxisX->max);
            ftmp = floor(xmax);
            if (ftmp != xmax) { xmax = ftmp + 1.0; }
            else { xmax = ftmp; }
        }
        else {
            xmax = xmin + 1.0;
        }

        if (xmax < xmin) {
            ftmp = xmax;
            xmax = xmin;
            xmin = ftmp;
        }
        else if (xmax == xmin) {
            xmax += 1.0;
        }

        xr = xmax - xmin;
        if (xr < xminrangeL) { return; }
        xmin -= (xr * fratio);
        xmax += (xr * fratio);
        paxisX->min = pow(10.0, xmin);
        paxisX->max = pow(10.0, xmax);
    }
    // ECHELLE LIN
    else {
        xr = paxisX->max - paxisX->min;
        if (xr < xminrange) { return; }
        paxisX->min -= (xr * fratio);
        paxisX->max += (xr * fratio);
    }

    // Y
    // ECHELLE LOG
    if (paxisY->scale == 1) {
        if (paxisY->min > 0.0) {
            ymin = floor(log10(paxisY->min));
        }
        else {
            if (paxisY->max > 0.0) {
                ymin = floor(log10(paxisY->max)) - 1.0;
            }
            else {
                ymin = pl_logzero - 1.0;
            }
        }

        if (paxisY->max != 0.0) {
            ymax = log10(paxisY->max);
            ftmp = floor(ymax);
            if (ftmp != ymax) { ymax = ftmp + 1.0; }
            else { ymax = ftmp; }
        }
        else {
            ymax = ymin + 1.0;
        }

        if (ymax < ymin) {
            ftmp = ymax;
            ymax = ymin;
            ymin = ftmp;
        }
        else if (ymax == ymin) {
            ymax += 1.0;
        }

        yr = ymax - ymin;
        if (yr < yminrangeL) { goto PlotZoomOut_RET; }
        ymin -= (yr * fratio);
        ymax += (yr * fratio);
        paxisY->min = pow(10.0, ymin);
        paxisY->max = pow(10.0, ymax);
    }
    // ECHELLE LIN
    else {
        yr = paxisY->max - paxisY->min;
        if (yr < yminrange) { goto PlotZoomOut_RET; }
        paxisY->min -= (yr * fratio);
        paxisY->max += (yr * fratio);
    }

PlotZoomOut_RET:

    if (((paxisX->max - paxisX->min) < DBL_EPSILON) || ((paxisY->max - paxisY->min) < DBL_EPSILON)) {
        paxisX->min = fxmin;
        paxisX->max = fxmax;
        paxisY->min = fymin;
        paxisY->max = fymax;
        return;
    }

    if (paxisX->scale == 0) {
        double fstep = 0.1;
        int igrid = 5;
        double fmin = paxisX->min, fmax = paxisX->max;
        pl_autotick(&fmin, &fmax, &fstep, &igrid, paxisX->scale);
        paxisX->min = fmin;
        paxisX->max = fmax;
        paxisX->increment = fstep;
        paxisX->gridcount = igrid;
        paxisX->gridmcount = 1;
    }
    if (paxisY->scale == 0) {
        double fstep = 0.1;
        int igrid = 5;
        double fmin = paxisY->min, fmax = paxisY->max;
        pl_autotick(&fmin, &fmax, &fstep, &igrid, paxisY->scale);
        paxisY->min = fmin;
        paxisY->max = fmax;
        paxisY->increment = fstep;
        paxisY->gridcount = igrid;
        paxisY->gridmcount = 1;
    }

    return;    // OK
}

BOOL CSigmaViewPlot::OnEraseBkgnd(CDC* pDC)
{

    // Ne pas exécuter OnEraseBkgnd
    return TRUE;
}

void CSigmaViewPlot::OnViewData()
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

    if (pDoc->GetColumnCount(0) > 0) {
        CWaitCursor waitT;
        pFrame->CreateSigmaView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData), TRUE);
    }
}

void CSigmaViewPlot::OnViewDataFit()
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

void CSigmaViewPlot::ShowFitter(int_t nDefaultIndex)
{
    if (m_pPlot == NULL) {
        return;
    }

    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_pPlot->curveactive < 0) || (m_pPlot->curveactive >= m_pPlot->curvecount)) {
        return;
    }

    CSigmaDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (pDoc == NULL) {
        return;
    }

    int_t nFitCount = pDoc->GetFitCount();
    fit_t *pFit = NULL;
    vector_t *pColumnDataX = NULL;
    vector_t *pColumnDataY = NULL;
    vector_t *pColumnFitX = NULL;
    vector_t *pColumnFitY = NULL;
    BOOL *pbIsModified = NULL;
    BOOL *pbRunning = NULL;

    pColumnDataY = pDoc->GetColumn(0, -1, m_pPlot->curve[m_pPlot->curveactive].y->id);
    pColumnDataX = pDoc->GetColumn(0, -1, m_pPlot->curve[m_pPlot->curveactive].y->idx);

    // Axe X auto ? (0 1 2 3 ...)
    if ((pColumnDataX == NULL) && (pColumnDataY != NULL)) {
        pColumnDataX = pDoc->GetColumnAutoX();
        if (pColumnDataY->idx != pColumnDataX->id) {
            pColumnDataX = NULL;
        }
    }

    if ((pColumnDataX == NULL) || (pColumnDataY == NULL)) {

        // [FIX-SG200-003] La courbe active est peut-être elle-même un fit...
        pColumnDataY = pDoc->GetColumn(1, -1, m_pPlot->curve[m_pPlot->curveactive].y->id);
        pColumnDataX = pDoc->GetColumn(1, -1, m_pPlot->curve[m_pPlot->curveactive].y->idx);

        if ((pColumnDataX != NULL) && (pColumnDataY != NULL)) {
            if (m_pPlot->curvecount > 1) {
                m_pPlot->curveactive += 1;
                if (m_pPlot->curveactive >= m_pPlot->curvecount) {
                    m_pPlot->curveactive = 0;
                }
                pColumnDataY = pDoc->GetColumn(0, -1, m_pPlot->curve[m_pPlot->curveactive].y->id);
                pColumnDataX = pDoc->GetColumn(0, -1, m_pPlot->curve[m_pPlot->curveactive].y->idx);
            }
        }
    }

    if ((pColumnDataX == NULL) || (pColumnDataY == NULL)) {
        SigmaApp.Output(_T("Cannot open the fitting session: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    // Le cas ci-dessous ne devrait à priori jamais arriver
    if ((pColumnDataX->dat == NULL) || (pColumnDataY->dat == NULL)
        || (pColumnDataX->dim < 3) || (pColumnDataY->dim < 3)
        || (pColumnDataX->dim > ML_MAXPOINTS) || (pColumnDataY->dim > ML_MAXPOINTS)
        || (pColumnDataY->dim != pColumnDataX->dim)) {
        SigmaApp.Output(_T("Cannot open the fitting session: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    real_t *pWdat = (real_t*)malloc(pColumnDataX->dim * sizeof(real_t));
    if (pWdat == NULL) {
        SigmaApp.Output(_T("Cannot open the fitting session: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    // Le fit a-t-il déjà été fait ?
    // L'ID de Fit est le même que celui de la colonne Y correspondante
    pColumnFitY = pDoc->GetColumn(1, -1, m_pPlot->curve[m_pPlot->curveactive].y->idf);
    if (pColumnFitY != NULL) {
        pColumnFitX = pDoc->GetColumn(1, -1, pColumnFitY->idx);
    }
    pFit = pDoc->GetFit(-1, m_pPlot->curve[m_pPlot->curveactive].y->idf);

    if (nFitCount >= SIGMA_MAXCOLUMNS) {
        if ((pColumnFitX == NULL) || (pColumnFitY == NULL) || (pFit == NULL)) {
            SigmaApp.Output(_T("Cannot open the fitting session: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            free(pWdat); pWdat = NULL;
            return;
        }
    }

    pbIsModified = &(pDoc->m_bIsModified);
    pbRunning = pDoc->GetRunning();

    // Le cas ci-dessous ne devrait à priori jamais arriver
    if ((pbIsModified == NULL) || (pbRunning == NULL)) {
        SigmaApp.Output(_T("Cannot open the fitting session: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        free(pWdat); pWdat = NULL;
        return;
    }

    int_t nDim;
    if ((pColumnDataY->istart >= 0) && (pColumnDataY->istart < (pColumnDataY->dim - 1))
        && (pColumnDataY->istop > pColumnDataY->istart) && (pColumnDataY->istop < pColumnDataY->dim)) {
        nDim = 2 * (pColumnDataY->istop - pColumnDataY->istart + 1);
    }
    else {
        nDim = 2 * pColumnDataY->dim;
    }

    int_t iIndex = 0, ii, jj, ll;
    if ((nDim < 2) || (nDim > ML_MAXPOINTS)) {
        nDim = pColumnDataY->dim;
    }

    // If fit already done, get the corresponding number of points
    vector_t *vecT = pDoc->GetColumn(1, 0);
    if (vecT != NULL) {
        if ((vecT->dim >= 2) && (vecT->dim < ML_MAXPOINTS)) {
            nDim = vecT->dim;
        }
    }

    if (pColumnFitX == NULL) {
        iIndex = pDoc->GetColumnCount(1);
        pColumnFitX = pDoc->InsertColumn(1, iIndex - 1, nDim);
        if (pColumnFitX == NULL) {
            if (pColumnFitY != NULL) {
                pDoc->DeleteColumn(1, -1, pColumnFitY->id);
            }
            vector_t *pvecT = NULL;
            for (jj = 0; jj < iIndex; jj++) {
                pvecT = pDoc->GetColumn(1, jj);
                if (pvecT) {
                    if (pvecT->idf == 0) {
                        pColumnFitY = pvecT;
                        for (ll = 0; ll < iIndex; ll++) {
                            pvecT = pDoc->GetColumn(1, ll);
                            if (pvecT) {
                                if (pvecT->id == pColumnFitY->idx) {
                                    pColumnFitX = pvecT;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            if (pColumnFitX == NULL) {
                SigmaApp.Output(_T("Cannot open the fitting session: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                free(pWdat); pWdat = NULL;
                return;
            }
        }
        _tcscpy(pColumnFitX->format, (const char_t*)(pColumnDataX->format));
        pColumnFitX->type = 0x10;
        pDoc->SetModifiedFlag(TRUE);
        pDoc->SaveState();
    }
    if (pColumnFitY == NULL) {
        iIndex = pDoc->GetColumnCount(1);
        pColumnFitY = pDoc->InsertColumn(1, iIndex - 1, nDim);
        if (pColumnFitY == NULL) {
            if (pColumnFitX != NULL) {
                pDoc->DeleteColumn(1, -1, pColumnFitX->id);
            }
            SigmaApp.Output(_T("Cannot open the fitting session: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            free(pWdat); pWdat = NULL;
            return;
        }
        _tcscpy(pColumnFitY->format, (const char_t*)(pColumnDataY->format));
        pColumnFitY->type = 0x20;
        pDoc->SetModifiedFlag(TRUE);
        pDoc->SaveState();
    }

    if (pColumnFitY->dim != pColumnFitX->dim) {
        pDoc->DeleteColumn(1, -1, pColumnFitX->id);
        pDoc->DeleteColumn(1, -1, pColumnFitY->id);
        SigmaApp.Output(_T("Cannot open the fitting session: invalid document data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        free(pWdat); pWdat = NULL;
        pDoc->SetModifiedFlag(TRUE);
        pDoc->SaveState();
        return;
    }

    // id    :    ID, unique
    // idx :    ID d'un vecteur lié, unique - Abscisse
    // idf :    ID d'un vecteur lié, unique - Fit
    // ide :    ID d'un vecteur lié, unique - Barres d'erreur
    pColumnDataY->idf = pColumnFitY->id;
    pColumnFitY->idf = pColumnDataY->id;
    pColumnFitY->idx = pColumnFitX->id;

    pFit = pDoc->GetFit(-1, pColumnFitY->id);

    if (pFit == NULL) {
        iIndex = pDoc->GetColumnIndex(1, pColumnFitX->id) >> 1;

        if ((iIndex < 0) || (iIndex >= (SIGMA_MAXCOLUMNS >> 1))) {
            if (pColumnFitX != NULL) {
                pDoc->DeleteColumn(1, -1, pColumnFitX->id);
            }
            if (pColumnFitY != NULL) {
                pDoc->DeleteColumn(1, -1, pColumnFitY->id);
            }
            free(pWdat); pWdat = NULL;
            return;
        }

        pFit = pDoc->GetFit(iIndex);

        if (pFit == NULL) {
            if (pColumnFitX != NULL) {
                pDoc->DeleteColumn(1, -1, pColumnFitX->id);
            }
            if (pColumnFitY != NULL) {
                pDoc->DeleteColumn(1, -1, pColumnFitY->id);
            }
            free(pWdat); pWdat = NULL;
            return;
        }

        // Mettre à jour les paramètres
        pFit->status = 0x00;
        for (ll = 0; ll < ML_FIT_MAXPAR; ll++) {
            pFit->fpar[ll] = 1.0;
            pFit->mpar[ll] = 1;
        }
        //
        pFit->id = pColumnFitY->id;
        pFit->index = 0;
    }

    pFit->lambda = SigmaApp.m_Options.fit.lambda;
    pFit->iter = SigmaApp.m_Options.fit.iter;

    pDoc->UpdateAllViews(NULL);

    CFitter fitterDlg;

    fitterDlg.m_Model.m_pWdat = pWdat;
    
    for (ii = 0; ii < pColumnDataX->dim; ii++) {
        fitterDlg.m_Model.m_pWdat[ii] = 1.0;
    }

    int_t nlen = _tcslen((const char_t*)(pColumnDataY->label));
    if (nlen < (ML_STRSIZES - 6)) {
        _tcscpy(pColumnFitY->label, (const char_t*)(pColumnDataY->label));
        _tcscat(pColumnFitY->label, _T(" - Fit"));
    }
    else {
        _tcscpy(pColumnFitY->label, (const char_t*)(pColumnFitY->name));
    }
    nlen = _tcslen((const char_t*)(pColumnDataX->label));
    if (nlen < (ML_STRSIZES - 6)) {
        _tcscpy(pColumnFitX->label, (const char_t*)(pColumnDataX->label));
        _tcscat(pColumnFitX->label, _T(" - Fit"));
    }
    else {
        _tcscpy(pColumnFitX->label, (const char_t*)(pColumnFitX->name));
    }

    fitterDlg.m_Model.m_pDoc = pDoc;
    fitterDlg.m_Model.m_pPlot = m_pPlot;
    fitterDlg.m_Model.m_pColumnDataX = pColumnDataX;
    fitterDlg.m_Model.m_pColumnDataY = pColumnDataY;
    fitterDlg.m_Model.m_pColumnFitX = pColumnFitX;
    fitterDlg.m_Model.m_pColumnFitY = pColumnFitY;
    fitterDlg.m_Model.m_pFit = pFit;
    fitterDlg.m_Model.m_pbIsModified = pbIsModified;
    fitterDlg.m_Model.m_pbRunning = pbRunning;
    fitterDlg.m_Model.m_nDefaultIndex = nDefaultIndex;

    fitterDlg.m_Mask.m_pDoc = fitterDlg.m_Model.m_pDoc;
    fitterDlg.m_Mask.m_pPlot = fitterDlg.m_Model.m_pPlot;
    fitterDlg.m_Mask.m_pColumnDataX = fitterDlg.m_Model.m_pColumnDataX;
    fitterDlg.m_Mask.m_pColumnDataY = fitterDlg.m_Model.m_pColumnDataY;
    fitterDlg.m_Mask.m_pColumnFitX = fitterDlg.m_Model.m_pColumnFitX;
    fitterDlg.m_Mask.m_pColumnFitY = fitterDlg.m_Model.m_pColumnFitY;
    fitterDlg.m_Mask.m_pFit = fitterDlg.m_Model.m_pFit;
    fitterDlg.m_Mask.m_pbIsModified = fitterDlg.m_Model.m_pbIsModified;
    fitterDlg.m_Mask.m_pbRunning = fitterDlg.m_Model.m_pbRunning;

    fitterDlg.DoModal();

    free(pWdat); pWdat = NULL;

    if (*pbIsModified) {
        pDoc->SaveState();
    }
}

void CSigmaViewPlot::OnFitterNonlinear()
{
    ShowFitter(0);
}

void CSigmaViewPlot::OnFitterLinear()
{
    ShowFitter(FIT_MODEL_LINE + 1);
}

void CSigmaViewPlot::OnFitterPolynomial()
{
    ShowFitter(FIT_MODEL_POLY2 + 1);
}

void CSigmaViewPlot::OnFitterGaussian()
{
    ShowFitter(FIT_MODEL_GAUSS1 + 1);
}

void CSigmaViewPlot::OnFitterLorentzian()
{
    ShowFitter(FIT_MODEL_LORENTZ1 + 1);
}

void CSigmaViewPlot::OnFitterExponetial()
{
    ShowFitter(FIT_MODEL_EXPGR + 1);
}

void CSigmaViewPlot::OnFitterLogarithmic()
{
    ShowFitter(FIT_MODEL_LOG + 1);
}

void CSigmaViewPlot::OnFitterPower()
{
    ShowFitter(FIT_MODEL_POWER + 1);
}

void CSigmaViewPlot::OnAddText()
{
    m_bAddingText = FALSE;
    m_nTextSelected = -1;

    if (m_bAddText == TRUE) {
        m_bAddText = FALSE;
    }
    else {
        m_bAddText = TRUE;

        // Réinitialiser toutes les autres action graphiques
        m_bZoomIn = FALSE;

        m_bAddLine = FALSE;
        m_bAddRect = FALSE;
        m_bAddEllipse = FALSE;

        m_bAddingText = FALSE;
        m_bAddingLine = FALSE;
        m_bAddingRect = FALSE;
        m_bAddingEllipse = FALSE;

        m_nMovingText = -1;
        m_nMovingLine = -1;
        m_nMovingRect = -1;
        m_nMovingEllipse = -1;
        m_nMovingLegend = -1;
        m_nMovingBottomTitle = -1;
        m_nMovingLeftTitle = -1;
        m_nMovingTopTitle = -1;
        m_nMovingRightTitle = -1;

        m_nResizingLine = -1;
        m_nResizingRect = -1;
        m_nResizingEllipse = -1;
        m_nResizingLineT = -1;
        m_nResizingRectT = -1;
        m_nResizingEllipseT = -1;

        m_nTextSelected = -1;
        m_nLineSelected = -1;
        m_nRectSelected = -1;
        m_nEllipseSelected = -1;

        m_nCurveSelected = -1;
    }
}

void CSigmaViewPlot::OnAddLine()
{
m_bAddingLine = FALSE;
    m_nLineSelected = -1;

    if (m_bAddLine == TRUE) {
        m_bAddLine = FALSE;
    }
    else {
        m_bAddLine = TRUE;
        // Réinitialiser toutes les autres action graphiques
        m_bZoomIn = FALSE;

        m_bAddText = FALSE;
        m_bAddRect = FALSE;
        m_bAddEllipse = FALSE;

        m_bAddingText = FALSE;
        m_bAddingLine = FALSE;
        m_bAddingRect = FALSE;
        m_bAddingEllipse = FALSE;

        m_nMovingText = -1;
        m_nMovingLine = -1;
        m_nMovingRect = -1;
        m_nMovingEllipse = -1;
        m_nMovingLegend = -1;
        m_nMovingBottomTitle = -1;
        m_nMovingLeftTitle = -1;
        m_nMovingTopTitle = -1;
        m_nMovingRightTitle = -1;

        m_nResizingLine = -1;
        m_nResizingRect = -1;
        m_nResizingEllipse = -1;
        m_nResizingLineT = -1;
        m_nResizingRectT = -1;
        m_nResizingEllipseT = -1;

        m_nTextSelected = -1;
        m_nLineSelected = -1;
        m_nRectSelected = -1;
        m_nEllipseSelected = -1;

        m_nCurveSelected = -1;
    }
}

void CSigmaViewPlot::OnAddRect()
{
    m_bAddingRect = FALSE;
    m_nRectSelected = -1;

    if (m_bAddRect == TRUE) {
        m_bAddRect = FALSE;
    }
    else {
        m_bAddRect = TRUE;

        // Réinitialiser toutes les autres action graphiques
        m_bZoomIn = FALSE;

        m_bAddText = FALSE;
        m_bAddLine = FALSE;
        m_bAddEllipse = FALSE;

        m_bAddingText = FALSE;
        m_bAddingLine = FALSE;
        m_bAddingRect = FALSE;
        m_bAddingEllipse = FALSE;

        m_nMovingText = -1;
        m_nMovingLine = -1;
        m_nMovingRect = -1;
        m_nMovingEllipse = -1;
        m_nMovingLegend = -1;
        m_nMovingBottomTitle = -1;
        m_nMovingLeftTitle = -1;
        m_nMovingTopTitle = -1;
        m_nMovingRightTitle = -1;

        m_nResizingLine = -1;
        m_nResizingRect = -1;
        m_nResizingEllipse = -1;
        m_nResizingLineT = -1;
        m_nResizingRectT = -1;
        m_nResizingEllipseT = -1;

        m_nTextSelected = -1;
        m_nLineSelected = -1;
        m_nRectSelected = -1;
        m_nEllipseSelected = -1;

        m_nCurveSelected = -1;
    }
}

void CSigmaViewPlot::OnAddEllipse()
{
m_bAddingEllipse = FALSE;
    m_nEllipseSelected = -1;

    if (m_bAddEllipse == TRUE) {
        m_bAddEllipse = FALSE;
    }
    else {
        m_bAddEllipse = TRUE;

        // Réinitialiser toutes les autres action graphiques
        m_bZoomIn = FALSE;

        m_bAddText = FALSE;
        m_bAddLine = FALSE;
        m_bAddRect = FALSE;

        m_bAddingText = FALSE;
        m_bAddingLine = FALSE;
        m_bAddingRect = FALSE;
        m_bAddingEllipse = FALSE;

        m_nMovingText = -1;
        m_nMovingLine = -1;
        m_nMovingRect = -1;
        m_nMovingEllipse = -1;
        m_nMovingLegend = -1;
        m_nMovingBottomTitle = -1;
        m_nMovingLeftTitle = -1;
        m_nMovingTopTitle = -1;
        m_nMovingRightTitle = -1;

        m_nResizingLine = -1;
        m_nResizingRect = -1;
        m_nResizingEllipse = -1;
        m_nResizingLineT = -1;
        m_nResizingRectT = -1;
        m_nResizingEllipseT = -1;

        m_nTextSelected = -1;
        m_nLineSelected = -1;
        m_nRectSelected = -1;
        m_nEllipseSelected = -1;

        m_nCurveSelected = -1;
    }
}

void CSigmaViewPlot::OnAddVertLine()
{
    if (m_pPlot == NULL) {
        return;
    }
    if ((m_pPlot->linevertcount < 0) || (m_pPlot->linevertcount >= PL_MAXITEMS)) {
        return;
    }

    int_t jj;
    int_t iX = 0, iX2 = 0, iY = 0, iY2 = 0;
    for (jj = 0; jj < m_pPlot->curvecount; jj++) {
        if ((m_pPlot->curve[jj].axisused & 0xF0) == 0x00) {                // X-Axis
            iX += 1;
        }
        else if ((m_pPlot->curve[jj].axisused & 0xF0) == 0x10) {        // X2-Axis
            iX2 += 1;
        }
        if ((m_pPlot->curve[jj].axisused & 0x0F) == 0x00) {                // Y-Axis
            iY += 1;
        }
        else if ((m_pPlot->curve[jj].axisused & 0x0F) == 0x01) {        // Y2-Axis
            iY2 += 1;
        }
    }
    byte_t axisused = 0x00;
    if (iX > 0) {
        axisused &= 0x0F;
    }
    else if (iX2 > 0) {
        axisused &= 0x0F;
        axisused |= 0x10;
    }
    if (iY > 0) {
        axisused &= 0xF0;
    }
    else if (iY2 > 0) {
        axisused &= 0xF0;
        axisused |= 0x01;
    }

    real_t fx = 0.0, fy = 0.0;
    if (GetPlotxy((int_t) (m_ptBegin.x), (int_t) (m_ptBegin.y), &fx, &fy, axisused) == FALSE) {
        return;
    }

    if (m_pPlot->linevertcount > 0) {
        for (jj = 0; jj < m_pPlot->linevertcount; jj++) {
            if (fx == m_pPlot->linevert[jj].rect.xa) {
                return;    // Ligne déjà tracée
            }
        }
    }
    m_pPlot->linevertcount += 1;
    m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.xa = fx;
    if ((axisused & 0x0F) == 0x00) {
        if (m_pPlot->axisleft.scale == 1) {        // Log Scale
            LogScale(m_pPlot->axisleft.min, m_pPlot->axisleft.max,
                &(m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.ya),
                &(m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.yb));
        }
        else {
            m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.ya = m_pPlot->axisleft.min;
            m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.yb = m_pPlot->axisleft.max;
        }
    }
    else {
        if (m_pPlot->axisright.scale == 1) {        // Log Scale
            LogScale(m_pPlot->axisright.min, m_pPlot->axisright.max,
                &(m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.ya),
                &(m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.yb));
        }
        else {
            m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.ya = m_pPlot->axisright.min;
            m_pPlot->linevert[m_pPlot->linevertcount - 1].rect.yb = m_pPlot->axisright.max;
        }
    }
    m_pPlot->linevert[m_pPlot->linevertcount - 1].status = 0x00;
    m_pPlot->linevert[m_pPlot->linevertcount - 1].size = 1;

    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return;
    }
    pDoc->SetModifiedFlag(TRUE);
    pDoc->SaveState();

    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}

void CSigmaViewPlot::OnAddHorzLine()
{
    if (m_pPlot == NULL) {
        return;
    }
    if ((m_pPlot->linehorzcount < 0) || (m_pPlot->linehorzcount >= PL_MAXITEMS)) {
        return;
    }

    int_t jj;
    int_t iX = 0, iX2 = 0, iY = 0, iY2 = 0;
    for (jj = 0; jj < m_pPlot->curvecount; jj++) {
        if ((m_pPlot->curve[jj].axisused & 0xF0) == 0x00) {                // X-Axis
            iX += 1;
        }
        else if ((m_pPlot->curve[jj].axisused & 0xF0) == 0x10) {            // X2-Axis
            iX2 += 1;
        }
        if ((m_pPlot->curve[jj].axisused & 0x0F) == 0x00) {                // Y-Axis
            iY += 1;
        }
        else if ((m_pPlot->curve[jj].axisused & 0x0F) == 0x01) {            // Y2-Axis
            iY2 += 1;
        }
    }
    byte_t axisused = 0x00;
    if (iX > 0) {
        axisused &= 0x0F;
    }
    else if (iX2 > 0) {
        axisused &= 0x0F;
        axisused |= 0x10;
    }
    if (iY > 0) {
        axisused &= 0xF0;
    }
    else if (iY2 > 0) {
        axisused &= 0xF0;
        axisused |= 0x01;
    }

    real_t fx = 0.0, fy = 0.0;
    if (GetPlotxy((int_t) (m_ptBegin.x), (int_t) (m_ptBegin.y), &fx, &fy, axisused) == FALSE) {
        return;
    }

    if (m_pPlot->linehorzcount > 0) {
        for (jj = 0; jj < m_pPlot->linehorzcount; jj++) {
            if (fy == m_pPlot->linehorz[jj].rect.ya) {
                return;    // Ligne déjà tracée
            }
        }
    }
    m_pPlot->linehorzcount += 1;
    m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.ya = fy;
    if ((axisused & 0xF0) == 0x00) {
        if (m_pPlot->axisbottom.scale == 1) {        // Log Scale
            LogScale(m_pPlot->axisbottom.min, m_pPlot->axisbottom.max,
                &(m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xa),
                &(m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xb));
        }
        else {
            m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xa = m_pPlot->axisbottom.min;
            m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xb = m_pPlot->axisbottom.max;
        }
    }
    else {
        if (m_pPlot->axistop.scale == 1) {        // Log Scale
            LogScale(m_pPlot->axistop.min, m_pPlot->axistop.max,
                &(m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xa),
                &(m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xb));
        }
        else {
            m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xa = m_pPlot->axistop.min;
            m_pPlot->linehorz[m_pPlot->linehorzcount - 1].rect.xb = m_pPlot->axistop.max;
        }
    }
    m_pPlot->linehorz[m_pPlot->linehorzcount - 1].status = 0x00;
    m_pPlot->linehorz[m_pPlot->linehorzcount - 1].size = 1;

    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return;
    }
    pDoc->SetModifiedFlag(TRUE);
    pDoc->SaveState();

    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}

void CSigmaViewPlot::OnViewFitReport()
{
if (m_pPlot == NULL) {
        return;
    }

    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_pPlot->curveactive < 0) || (m_pPlot->curveactive >= m_pPlot->curvecount)) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    CFitterReport reportDlg;

    fit_t *pFit = pDoc->GetFit(-1, m_pPlot->curve[m_pPlot->curveactive].y->idf);
    if (pFit == NULL) {
        for (int ii = 0; ii < m_pPlot->curvecount; ii++) {
            pFit = pDoc->GetFit(-1, m_pPlot->curve[ii].y->idf);
            if (pFit != NULL) {
                m_pPlot->curveactive = ii;
                break;
            }
        }
    }

    reportDlg.m_pFit = pFit;
    reportDlg.m_pDoc = pDoc;
    reportDlg.m_pPlot = m_pPlot;

    reportDlg.DoModal();
}

//Printing
void CSigmaViewPlot::SetPrintMarginInfo(int_t nHeaderHeight, int_t nFooterHeight, int_t nLeftMargin, int_t nRightMargin,
                                                     int_t nTopMargin, int_t nBottomMargin, int_t nGap)
{
    // If any parameter is -1, keep the existing setting
    if (nHeaderHeight > -1) { m_nHeaderHeight = nHeaderHeight; }
    if (nFooterHeight > -1) { m_nFooterHeight = nFooterHeight; }
    if (nLeftMargin > -1) { m_nLeftMargin = nLeftMargin; }
    if (nRightMargin > -1) { m_nRightMargin = nRightMargin; }
    if (nTopMargin > -1) { m_nTopMargin = nTopMargin; }
    if (nBottomMargin > -1) { m_nBottomMargin = nBottomMargin; }
    if (nGap > -1) { m_nGap = nGap; }
}


BOOL CSigmaViewPlot::OnPreparePrinting(CPrintInfo* pInfo)
{
    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return FALSE;
    }

    pDoc->m_bPrinting = TRUE;

    // TODO:  call DoPreparePrinting to invoke the Print dialog box
    CPrintDialog *pDlg = new CPrintDialog(FALSE, PD_PAGENUMS);
    pInfo->m_pPD = pDlg;
    pInfo->SetMinPage(1);
    pInfo->SetMaxPage(1);

    return DoPreparePrinting(pInfo);
}

void CSigmaViewPlot::Print(CPrintDialog* pPrintDialog /*= NULL*/)
{
    CDC dc;

    if (pPrintDialog == NULL) {
        CPrintDialog printDlg(FALSE);
        if (printDlg.DoModal() != IDOK) {                    // Get printer settings from user
            return;
        }

        dc.Attach(printDlg.GetPrinterDC());                    // attach a printer DC
    }
    else {
        dc.Attach(pPrintDialog->GetPrinterDC());            // attach a printer DC
    }

    dc.m_bPrinting = TRUE;

    CString strTitle;
    strTitle.LoadString(AFX_IDS_APP_TITLE);

    if (strTitle.IsEmpty()) {
        CWnd *pParentWnd = GetParent();
        while (pParentWnd) {
            pParentWnd->GetWindowText(strTitle);
            if (strTitle.GetLength()) {                        // can happen if it is a CWnd, CChildFrm has the title
                break;
            }
            pParentWnd = pParentWnd->GetParent();
        }
    }

    DOCINFO di;                                                    // Initialise print doc details
    memset(&di, 0, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = strTitle;

    BOOL bPrintingOK = dc.StartDoc(&di);                // Begin a new print job

    CPrintInfo Info;
    Info.m_rectDraw.SetRect(0, 0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));

    OnBeginPrinting(&dc, &Info);                            // Initialise printing
    for (UINT page = Info.GetMinPage(); page <= Info.GetMaxPage() && bPrintingOK; page++) {
        dc.StartPage();                                        // begin new page
        Info.m_nCurPage = page;
        OnPrint(&dc, &Info);                                    // Print page
        bPrintingOK = (dc.EndPage() > 0);                // end page
    }
    OnEndPrinting(&dc, &Info);                                // Clean up after printing

    if (bPrintingOK) {
        dc.EndDoc();                                            // end a print job
    }
    else {
        dc.AbortDoc();                                            // abort job.
    }

    dc.Detach();                                                // detach the printer DC
}

void CSigmaViewPlot::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
    ASSERT(pDC && pInfo);
    if ((pDC == NULL) || (pInfo == NULL)) { return; }

    // Get a DC for the current window (will be a screen DC for print previewing)
    CDC *pCurrentDC = GetDC();                    // will have dimensions of the client area
    if (pCurrentDC == NULL) { return; }

    CSize PaperPixelsPerInch(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
    CSize ScreenPixelsPerInch(pCurrentDC->GetDeviceCaps(LOGPIXELSX), pCurrentDC->GetDeviceCaps(LOGPIXELSY));
    if ((PaperPixelsPerInch.cx == 0) || (PaperPixelsPerInch.cy == 0)
        || (ScreenPixelsPerInch.cx == 0) || (ScreenPixelsPerInch.cy == 0)) {

        ReleaseDC(pCurrentDC);
        pCurrentDC = NULL;
        return;
    }

    // Create the printer font
    m_PrinterFont.CreateFont(-MulDiv(7, ::GetDeviceCaps(pCurrentDC->GetSafeHdc(), LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Times New Roman"));

    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Get the average character width (in Datasheet units) and hence the margins
    m_CharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"), 52);
    m_CharSize.cx /= 52;
    int_t nMargins = (m_nLeftMargin + m_nRightMargin) * m_CharSize.cx;

    // Get the page sizes (physical and logical)
    m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

    m_LogicalPageSize.cx = ScreenPixelsPerInch.cx * m_PaperSize.cx / PaperPixelsPerInch.cx * 3 / 4;
    m_LogicalPageSize.cy = ScreenPixelsPerInch.cy * m_PaperSize.cy / PaperPixelsPerInch.cy * 3 / 4;
    m_nPageHeight = m_LogicalPageSize.cy - (m_nHeaderHeight + m_nFooterHeight + (2 * m_nGap)) * m_CharSize.cy;
    m_nPageWidth = m_LogicalPageSize.cx - nMargins;

    // Set up the print info
    pInfo->SetMaxPage(1);
    pInfo->m_nCurPage = 1;                                        // start printing at page# 1

    ReleaseDC(pCurrentDC);
    pCurrentDC = NULL;
    pDC->SelectObject(pOldFont);
}

void CSigmaViewPlot::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
    if ((pDC == NULL) || (pInfo == NULL)) {
        return;
    }

    //CRect rcPage(pInfo->m_rectDraw);
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Set the page map mode to use Datasheet units, and setup margin
    int_t iMapMode = pDC->SetMapMode(MM_ISOTROPIC);
    pDC->SetWindowExt(m_LogicalPageSize);
    pDC->SetViewportExt(m_PaperSize);
    pDC->SetWindowOrg(-m_nLeftMargin * m_CharSize.cx, 0);

    // Header
    pInfo->m_rectDraw.top = 0;
    pInfo->m_rectDraw.left = 0;
    pInfo->m_rectDraw.right = m_LogicalPageSize.cx - (m_nLeftMargin + m_nRightMargin) * m_CharSize.cx;
    pInfo->m_rectDraw.bottom = m_nHeaderHeight * m_CharSize.cy;
    PrintHeader(pDC, pInfo);
    pDC->OffsetWindowOrg(0, -m_nHeaderHeight * m_CharSize.cy);

    // Gap between header and column headings
    pDC->OffsetWindowOrg(0, -m_nGap * m_CharSize.cy);

    UINT nNumPages = 1;

    int dpiU = ::GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);
    int dpiV = ::GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSY);

    Plot(pDC, pInfo->m_rectDraw.right, m_LogicalPageSize.cy - (m_nFooterHeight * m_CharSize.cy) - (m_nGap * m_CharSize.cy * 2), (dpiU == dpiV));

    // Footer
    pInfo->m_rectDraw.bottom = m_nFooterHeight * m_CharSize.cy;
    pDC->SetWindowOrg(-m_nLeftMargin * m_CharSize.cx, -m_LogicalPageSize.cy + m_nFooterHeight * m_CharSize.cy);
    PrintFooter(pDC, pInfo);

    // SetWindowOrg back for next page
    pDC->SetWindowOrg(0,0);

    pDC->SetMapMode(iMapMode);
    pDC->SelectObject(pOldFont);
}

void CSigmaViewPlot::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    m_PrinterFont.DeleteObject();

    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return;
    }

    pDoc->m_bPrinting = FALSE;
}

void CSigmaViewPlot::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
    // print App title on top right margin
    CString strRight;
    COleDateTime tm = COleDateTime::GetCurrentTime();
    strRight = tm.Format(_T("%Y/%m/%d - %H:%M:%S"));

    // print parent window title in the centre (Gert Rijs)
    CString strCenter;
    CWnd *pParentWnd = GetParent();
    while (pParentWnd) {
        pParentWnd->GetWindowText(strCenter);
        if (strCenter.GetLength()) {            // can happen if it is a CWnd, CChildFrm has the title
            break;
        }
        pParentWnd = pParentWnd->GetParent();
    }

    CFont BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    VERIFY(m_PrinterFont.GetLogFont(&lf));
    //lf.lfWeight = FW_BOLD;
    VERIFY(BoldFont.CreateFontIndirect(&lf));

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int_t nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    CRect rc(pInfo->m_rectDraw);
    CRect recT(rc);
    recT.bottom -= 2;
    if (!strCenter.IsEmpty()) {
        pDC->DrawText(strCenter, &recT, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM);
    }
    if (!strRight.IsEmpty()) {
        pDC->DrawText(strRight, &recT, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM);
    }

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();

    // draw ruled-line across top
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.bottom);
    pDC->LineTo(rc.right, rc.bottom);
}

//print footer with a line and date, and page number
void CSigmaViewPlot::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
    // date and time on the right
    CString strRight = _T("Generated by SigmaGraph - Data Plotting and Analysis Software - http://www.hamady.org");

    CRect rc(pInfo->m_rectDraw);

    // draw ruled line on bottom
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.top);
    pDC->LineTo(rc.right, rc.top);

    CFont BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    m_PrinterFont.GetLogFont(&lf);
    //lf.lfWeight = FW_BOLD;
    BoldFont.CreateFontIndirect(&lf);

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int_t nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    // Bug fix - Force text color to black.  It doesn't always
    // get set to a printable color when it gets here.
    pDC->SetTextColor(RGB(0, 0, 0));

    if (!strRight.IsEmpty()) {
        pDC->DrawText(strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_TOP);
    }

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();
}

bool CSigmaViewPlot::DoFileExport(CString &strFilename)
{
    int iLen = SigmaApp.CStringToChar(strFilename, (char_t*) (m_pPlot->imagefilename), ML_STRSIZE - 1);
    if (iLen < 4) {
        return false;
    }
    char_t szExt[4];
    szExt[0] = m_pPlot->imagefilename[iLen - 3];
    szExt[1] = m_pPlot->imagefilename[iLen - 2];
    szExt[2] = m_pPlot->imagefilename[iLen - 1];
    szExt[3] = _T('\0');
    if (_tcsicmp(szExt, _T("emf")) == 0) {
        m_pPlot->imagetype = 1;
    }
    else if (_tcsicmp(szExt, _T("svg")) == 0) {
        m_pPlot->imagetype = 2;
    }
    else {
        return false;
    }

    SetForegroundWindow();
    SendMessage(WM_PAINT, 0, 0);
    return true;
}

void CSigmaViewPlot::OnFileExport()
{
    m_pPlot->imagetype = 0;
    memset(m_pPlot->imagefilename, 0, ML_STRSIZE * sizeof(char_t));

    char_t szFilters[] = _T("Enhanced Metafile (*.emf)|*.emf|Scalable Vector Graphics (*.svg)|*.svg||");

    CFileDialog *pFileDlg = new CFileDialog(FALSE, _T("emf"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
    if (pFileDlg->DoModal() != IDOK) {
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString strFilename = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;
    
    DoFileExport(strFilename);
    return;
}

void CSigmaViewPlot::OnItemRemove()
{
    CSigmaDoc* pDoc = GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return;
    }

    int_t ii;
    if (m_nTextSelected >= 0) {
        if (m_nTextSelected == 0) {
            if (m_pPlot->textcount > 1) {
                for (ii = 1; ii < m_pPlot->textcount; ii++) {
                    memcpy(&(m_pPlot->text[ii-1]), &(m_pPlot->text[ii]), sizeof(text_t)); 
                }
            }
        }
        else if (m_nTextSelected != (m_pPlot->textcount - 1)) {
            for (ii = m_nTextSelected; ii < m_pPlot->textcount - 1; ii++) {
                memcpy(&(m_pPlot->text[ii]), &(m_pPlot->text[ii+1]), sizeof(text_t)); 
            }
        }
        m_pPlot->textcount -= 1;
        memcpy(&(m_pPlot->line[m_pPlot->textcount]), &(SigmaApp.m_pDefaultPlot->text[m_nTextSelected]), sizeof(text_t)); 
        m_pPlot->plint.textRect[m_pPlot->textcount].left = 0;
        m_pPlot->plint.textRect[m_pPlot->textcount].top = 0;
        m_pPlot->plint.textRect[m_pPlot->textcount].right = 0;
        m_pPlot->plint.textRect[m_pPlot->textcount].bottom = 0;
        pDoc->SetModifiedFlag(TRUE);
        pDoc->SaveState();
    }
    else if (m_nLineSelected >= 0) {
        if (m_nLineSelected == 0) {
            if (m_pPlot->linecount > 1) {
                for (ii = 1; ii < m_pPlot->linecount; ii++) {
                    memcpy(&(m_pPlot->line[ii-1]), &(m_pPlot->line[ii]), sizeof(line_t)); 
                }
            }
        }
        else if (m_nLineSelected != (m_pPlot->linecount - 1)) {
            for (ii = m_nLineSelected; ii < m_pPlot->linecount - 1; ii++) {
                memcpy(&(m_pPlot->line[ii]), &(m_pPlot->line[ii+1]), sizeof(line_t)); 
            }
        }
        m_pPlot->linecount -= 1;
        memcpy(&(m_pPlot->line[m_pPlot->linecount]), &(SigmaApp.m_pDefaultPlot->line[m_nLineSelected]), sizeof(line_t)); 
        m_pPlot->plint.lineRect[m_pPlot->linecount].left = 0;
        m_pPlot->plint.lineRect[m_pPlot->linecount].top = 0;
        m_pPlot->plint.lineRect[m_pPlot->linecount].right = 0;
        m_pPlot->plint.lineRect[m_pPlot->linecount].bottom = 0;
        pDoc->SetModifiedFlag(TRUE);
        pDoc->SaveState();
    }
    else if (m_nRectSelected >= 0) {
        if (m_nRectSelected == 0) {
            if (m_pPlot->rectanglecount > 1) {
                for (ii = 1; ii < m_pPlot->rectanglecount; ii++) {
                    memcpy(&(m_pPlot->rectangle[ii-1]), &(m_pPlot->rectangle[ii]), sizeof(rectangle_t)); 
                }
            }
        }
        else if (m_nRectSelected != (m_pPlot->rectanglecount - 1)) {
            for (ii = m_nRectSelected; ii < m_pPlot->rectanglecount - 1; ii++) {
                memcpy(&(m_pPlot->rectangle[ii]), &(m_pPlot->rectangle[ii+1]), sizeof(rectangle_t)); 
            }
        }
        m_pPlot->rectanglecount -= 1;
        memcpy(&(m_pPlot->rectangle[m_pPlot->rectanglecount]), &(SigmaApp.m_pDefaultPlot->rectangle[m_nRectSelected]), sizeof(rectangle_t)); 
        m_pPlot->plint.rectangleRect[m_pPlot->rectanglecount].left = 0;
        m_pPlot->plint.rectangleRect[m_pPlot->rectanglecount].top = 0;
        m_pPlot->plint.rectangleRect[m_pPlot->rectanglecount].right = 0;
        m_pPlot->plint.rectangleRect[m_pPlot->rectanglecount].bottom = 0;
        pDoc->SetModifiedFlag(TRUE);
        pDoc->SaveState();
    }
    else if (m_nEllipseSelected >= 0) {
        if (m_nEllipseSelected == 0) {
            if (m_pPlot->ellipsecount > 1) {
                for (ii = 1; ii < m_pPlot->ellipsecount; ii++) {
                    memcpy(&(m_pPlot->ellipse[ii-1]), &(m_pPlot->ellipse[ii]), sizeof(ellipse_t)); 
                }
            }
        }
        else if (m_nEllipseSelected != (m_pPlot->ellipsecount - 1)) {
            for (ii = m_nEllipseSelected; ii < m_pPlot->ellipsecount - 1; ii++) {
                memcpy(&(m_pPlot->ellipse[ii]), &(m_pPlot->ellipse[ii+1]), sizeof(ellipse_t)); 
            }
        }
        m_pPlot->ellipsecount -= 1;
        memcpy(&(m_pPlot->ellipse[m_pPlot->ellipsecount]), &(SigmaApp.m_pDefaultPlot->ellipse[m_nEllipseSelected]), sizeof(ellipse_t)); 
        m_pPlot->plint.ellipseRect[m_pPlot->ellipsecount].left = 0;
        m_pPlot->plint.ellipseRect[m_pPlot->ellipsecount].top = 0;
        m_pPlot->plint.ellipseRect[m_pPlot->ellipsecount].right = 0;
        m_pPlot->plint.ellipseRect[m_pPlot->ellipsecount].bottom = 0;
        pDoc->SetModifiedFlag(TRUE);
        pDoc->SaveState();
    }

    m_nTextSelected = -1;
    m_nLineSelected = -1;
    m_nRectSelected = -1;
    m_nEllipseSelected = -1;
    m_bPlotSelected = FALSE;

    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}

void CSigmaViewPlot::OnItemCopy()
{
    CopyItem();
}

void CSigmaViewPlot::OnItemPaste()
{
    PasteItem();
}

void CSigmaViewPlot::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
    if (m_pPlot == NULL) {
        return;
    }

    CSigmaDoc *pDoc = GetDocument();
    if (pDoc != NULL) {
        m_bAutoScale = pDoc->GetAutoScale();
        pDoc->SetAutoScale(FALSE);
    }

    m_pPlot->frame = GetSafeHwnd();
    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

    CWnd *pWnd = GetParent();
    if (pWnd == NULL) {
        pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);
        return;
    }
    CString strT = pDoc->GetTitle();
    strT.TrimRight(_T("*"));
    strT.Append(_T(" - Graph"));
    pWnd->SetWindowText((LPCTSTR)strT);
}

void CSigmaViewPlot::OnCurveActive()
{
if ((m_nCurveSelected < 0) || (m_nCurveSelected >= m_pPlot->curvecount)) {
        return;
    }

    m_pPlot->curveactive = m_nCurveSelected;

    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}

void CSigmaViewPlot::OnCurveHide()
{
if ((m_nCurveSelected < 0) || (m_nCurveSelected >= m_pPlot->curvecount)) {
        return;
    }

    if ((m_pPlot->curve[m_nCurveSelected].status & 0x0F) == 0x0F) {
        m_pPlot->curve[m_nCurveSelected].status &= 0xF0;
    }
    else {
        m_pPlot->curve[m_nCurveSelected].status |= 0x0F;
    }

    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}

void CSigmaViewPlot::OnCurveRemove()
{
if ((m_nCurveSelected < 0) || (m_nCurveSelected >= m_pPlot->curvecount)) {
        return;
    }

    if (this->MessageBox(_T("Remove curve from graph?"), _T("SigmaGraph"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES) {
        m_pPlot->errcode = 0;
        pl_curve_remove(m_pPlot, m_nCurveSelected);

        if ((m_pPlot->curveactive < 0) || (m_pPlot->curveactive >= m_pPlot->curvecount)) {
            m_pPlot->curveactive = 0;
        }

        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }
}

void CSigmaViewPlot::OnCurveProperties()
{
    ShowProperties(0, m_nCurveSelected);
}

void CSigmaViewPlot::OnErrorBar()
{
    if (m_pPlot == NULL) {
        return;
    }

    if (m_pPlot->curvecount < 1) {
        return;
    }

    CErrorBarDlg errbarDlg;

    // Options initialization
    errbarDlg.m_pPlot = m_pPlot;
    errbarDlg.m_pDoc = (CDocument*)GetDocument();

    errbarDlg.DoModal();
}

void CSigmaViewPlot::UpdateGraphMenu(CCmdUI *pCmdUI)
{
    if (m_pPlot == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    if (m_pPlot->curvecount < 1) {
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
}

void CSigmaViewPlot::OnEditUndo()
{
    CSigmaDoc* pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    pDoc->Undo();
}

void CSigmaViewPlot::OnEditRedo()
{
    CSigmaDoc* pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    pDoc->Redo();
}


void CSigmaViewPlot::OnUpdatePlotZoomout(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateViewZoomIn(CCmdUI *pCmdUI)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*) (GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    if ((pDoc->m_pPlot == NULL) || (pDoc->m_pPlot->curvecount < 1)) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(TRUE);

    int_t iButtonIndex = pFrame->m_wndToolbarPlot.CommandToIndex(ID_PLOT_ZOOMIN);
    UINT uiStyle = pFrame->m_wndToolbarPlot.GetButtonStyle(iButtonIndex);

    if (m_bZoomIn == TRUE) {
        uiStyle |= TBBS_CHECKED;
    }
    else {
        uiStyle &= ~TBBS_CHECKED;
    }

    pFrame->m_wndToolbarPlot.SetButtonStyle(iButtonIndex, uiStyle);
}

void CSigmaViewPlot::OnUpdateErrorBar(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterLinear(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterGaussian(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterLorentzian(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterPolynomial(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterExponential(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterLogarithmic(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterPower(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateFitterNonlinear(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdatePlotAutoscale(CCmdUI *pCmdUI)
{
    UpdateGraphMenu(pCmdUI);
}

void CSigmaViewPlot::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    pCmdUI->Enable(pDoc->CanUndo());
}

void CSigmaViewPlot::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(pDoc->CanRedo());
}

void CSigmaViewPlot::OnUpdateViewData(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(pDoc->GetColumnCount(0) > 0);
}

void CSigmaViewPlot::OnUpdateViewFit(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(pDoc->canViewFit());
}

void CSigmaViewPlot::OnUpdateViewFitReport(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        pCmdUI->Enable(FALSE);
        return;
    }

    pCmdUI->Enable(pDoc->canViewFit());
}

void CSigmaViewPlot::OnUpdatePlotAddtext(CCmdUI *pCmdUI)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    if ((pDoc->m_pPlot == NULL) || (pDoc->m_pPlot->curvecount < 1)) {
        pCmdUI->Enable(FALSE);
        return;
    }

    int_t iButtonIndex = pFrame->m_wndToolbarPlot.CommandToIndex(ID_PLOT_ADDTEXT);
    UINT uiStyle = pFrame->m_wndToolbarPlot.GetButtonStyle(iButtonIndex);
    if (m_bAddText == TRUE) {
        uiStyle |= TBBS_CHECKED;
    }
    else {
        uiStyle &= ~TBBS_CHECKED;
    }

    pFrame->m_wndToolbarPlot.SetButtonStyle(iButtonIndex, uiStyle);

    if (pDoc->m_pPlot->textcount >= PL_MAXITEMS) {
        m_bAddText = FALSE;
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
    pCmdUI->SetCheck(m_bAddText);
}

void CSigmaViewPlot::OnUpdatePlotAddline(CCmdUI *pCmdUI)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    if ((pDoc->m_pPlot == NULL) || (pDoc->m_pPlot->curvecount < 1)) {
        pCmdUI->Enable(FALSE);
        return;
    }

    int_t iButtonIndex = pFrame->m_wndToolbarPlot.CommandToIndex(ID_PLOT_ADDLINE);
    UINT uiStyle = pFrame->m_wndToolbarPlot.GetButtonStyle(iButtonIndex);
    if (m_bAddLine == TRUE) {
        uiStyle |= TBBS_CHECKED;
    }
    else {
        uiStyle &= ~TBBS_CHECKED;
    }

    pFrame->m_wndToolbarPlot.SetButtonStyle(iButtonIndex, uiStyle);

    if (pDoc->m_pPlot->linecount >= PL_MAXITEMS) {
        m_bAddLine = FALSE;
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
    pCmdUI->SetCheck(m_bAddLine);
}

void CSigmaViewPlot::OnUpdatePlotAddrect(CCmdUI *pCmdUI)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    if ((pDoc->m_pPlot == NULL) || (pDoc->m_pPlot->curvecount < 1)) {
        pCmdUI->Enable(FALSE);
        return;
    }

    int_t iButtonIndex = pFrame->m_wndToolbarPlot.CommandToIndex(ID_PLOT_ADDRECT);
    UINT uiStyle = pFrame->m_wndToolbarPlot.GetButtonStyle(iButtonIndex);
    if (m_bAddRect == TRUE) {
        uiStyle |= TBBS_CHECKED;
    }
    else {
        uiStyle &= ~TBBS_CHECKED;
    }

    pFrame->m_wndToolbarPlot.SetButtonStyle(iButtonIndex, uiStyle);

    if (pDoc->m_pPlot->rectanglecount >= PL_MAXITEMS) {
        m_bAddRect = FALSE;
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
    pCmdUI->SetCheck(m_bAddRect);
}

void CSigmaViewPlot::OnUpdatePlotAddellipse(CCmdUI *pCmdUI)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    ASSERT(pDoc != NULL);
    if (pDoc == NULL) {
        return;
    }

    if ((pDoc->m_pPlot == NULL) || (pDoc->m_pPlot->curvecount < 1)) {
        pCmdUI->Enable(FALSE);
        return;
    }

    int_t iButtonIndex = pFrame->m_wndToolbarPlot.CommandToIndex(ID_PLOT_ADDELLIPSE);
    UINT uiStyle = pFrame->m_wndToolbarPlot.GetButtonStyle(iButtonIndex);
    if (m_bAddEllipse == TRUE) {
        uiStyle |= TBBS_CHECKED;
    }
    else {
        uiStyle &= ~TBBS_CHECKED;
    }

    pFrame->m_wndToolbarPlot.SetButtonStyle(iButtonIndex, uiStyle);

    if (pDoc->m_pPlot->ellipsecount >= PL_MAXITEMS) {
        m_bAddEllipse = FALSE;
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
    pCmdUI->SetCheck(m_bAddEllipse);
}

BOOL CSigmaViewPlot::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{

    if (nHitTest == HTCLIENT) {
        DWORD dwPos = ::GetMessagePos();
        CPoint tPoint(LOWORD(dwPos), HIWORD(dwPos));
        ScreenToClient(&tPoint);

        RECT recT, recTT;

        if ((m_bZooming == TRUE) ) {
            ::SetCursor(m_hZCursor);
        }
        else if (m_bPlotSelected) {
            if (PtInRect(&m_recMov[0], tPoint)) {
                ::SetCursor(m_hMCursorV);
            }
            else if (PtInRect(&m_recMov[1], tPoint)) {
                ::SetCursor(m_hMCursorH);
            }
            else if (PtInRect(&m_recMov[2], tPoint)) {
                ::SetCursor(m_hMCursorV);
            }
            else if (PtInRect(&m_recMov[3], tPoint)) {
                ::SetCursor(m_hMCursorH);
            }
            else if (PtInRect(&m_recMov[4], tPoint)) {
                ::SetCursor(m_hMCursor);
            }
            else {
                ::SetCursor(m_hACursor);
            }
        }
        else if (m_nTextSelected >= 0) {
            if (PtInRect(&(m_pPlot->plint.textRect[m_nTextSelected]), tPoint)) {
                ::SetCursor(m_hMCursor);
            }
            else {
                ::SetCursor(m_hACursor);
            }
        }
        else if (m_nLineSelected >= 0) {
            if (m_pPlot->line[m_nLineSelected].orient == 0) {
                recT.left = m_pPlot->plint.lineRect[m_nLineSelected].left - PL_RESIZE;
                recT.top = m_pPlot->plint.lineRect[m_nLineSelected].top - PL_RESIZE;
                recT.right = m_pPlot->plint.lineRect[m_nLineSelected].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].top + PL_RESIZE;
                recTT.left = m_pPlot->plint.lineRect[m_nLineSelected].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.lineRect[m_nLineSelected].bottom - PL_RESIZE;
                recTT.right = m_pPlot->plint.lineRect[m_nLineSelected].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].bottom + PL_RESIZE;
            }
            else {
                recT.left = m_pPlot->plint.lineRect[m_nLineSelected].left - PL_RESIZE;
                recT.top = m_pPlot->plint.lineRect[m_nLineSelected].bottom - PL_RESIZE;
                recT.right = m_pPlot->plint.lineRect[m_nLineSelected].left + PL_RESIZE;
                recT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].bottom + PL_RESIZE;
                recTT.left = m_pPlot->plint.lineRect[m_nLineSelected].right - PL_RESIZE;
                recTT.top = m_pPlot->plint.lineRect[m_nLineSelected].top - PL_RESIZE;
                recTT.right = m_pPlot->plint.lineRect[m_nLineSelected].right + PL_RESIZE;
                recTT.bottom = m_pPlot->plint.lineRect[m_nLineSelected].top + PL_RESIZE;
            }

            if (SigmaApp.PtOnLine(&(m_pPlot->plint.lineRect[m_nLineSelected]), tPoint, m_pPlot->line[m_nLineSelected].orient) || PtInRect(&recT, tPoint) || PtInRect(&recTT, tPoint)) {
                ::SetCursor(m_hMCursor);
            }
            else {
                ::SetCursor(m_hACursor);
            }
        }
        else if (m_nRectSelected >= 0) {
            recT.left = m_pPlot->plint.rectangleRect[m_nRectSelected].left - PL_RESIZE;
            recT.top = m_pPlot->plint.rectangleRect[m_nRectSelected].top - PL_RESIZE;
            recT.right = m_pPlot->plint.rectangleRect[m_nRectSelected].left + PL_RESIZE;
            recT.bottom = m_pPlot->plint.rectangleRect[m_nRectSelected].top + PL_RESIZE;
            recTT.left = m_pPlot->plint.rectangleRect[m_nRectSelected].right - PL_RESIZE;
            recTT.top = m_pPlot->plint.rectangleRect[m_nRectSelected].bottom - PL_RESIZE;
            recTT.right = m_pPlot->plint.rectangleRect[m_nRectSelected].right + PL_RESIZE;
            recTT.bottom = m_pPlot->plint.rectangleRect[m_nRectSelected].bottom + PL_RESIZE;

            if (PtInRect(&(m_pPlot->plint.rectangleRect[m_nRectSelected]), tPoint) || PtInRect(&recT, tPoint) || PtInRect(&recTT, tPoint)) {
                ::SetCursor(m_hMCursor);
            }
            else {
                ::SetCursor(m_hACursor);
            }
        }
        else if (m_nEllipseSelected >= 0) {
            recT.left = m_pPlot->plint.ellipseRect[m_nEllipseSelected].left - PL_RESIZE;
            recT.top = m_pPlot->plint.ellipseRect[m_nEllipseSelected].top - PL_RESIZE;
            recT.right = m_pPlot->plint.ellipseRect[m_nEllipseSelected].left + PL_RESIZE;
            recT.bottom = m_pPlot->plint.ellipseRect[m_nEllipseSelected].top + PL_RESIZE;
            recTT.left = m_pPlot->plint.ellipseRect[m_nEllipseSelected].right - PL_RESIZE;
            recTT.top = m_pPlot->plint.ellipseRect[m_nEllipseSelected].bottom - PL_RESIZE;
            recTT.right = m_pPlot->plint.ellipseRect[m_nEllipseSelected].right + PL_RESIZE;
            recTT.bottom = m_pPlot->plint.ellipseRect[m_nEllipseSelected].bottom + PL_RESIZE;

            if (PtInRect(&(m_pPlot->plint.ellipseRect[m_nEllipseSelected]), tPoint) || PtInRect(&recT, tPoint) || PtInRect(&recTT, tPoint)) {
                ::SetCursor(m_hMCursor);
            }
            else {
                ::SetCursor(m_hACursor);
            }
        }
        else if (m_nLegendSelected >= 0) {
            if (PtInRect(&(m_pPlot->plint.legendsRect), tPoint)) {
                ::SetCursor(m_hMCursor);
            }
            else {
                ::SetCursor(m_hACursor);
            }
        }
        else if (m_nTitleSelected >= 0) {
            if (PtInRect(&(m_pPlot->plint.axistRect[m_nTitleSelected]), tPoint)) {
                ::SetCursor(m_hMCursor);
            }
            else {
                ::SetCursor(m_hACursor);
            }
        }
        else {
            ::SetCursor(m_hACursor);
        }

        return TRUE;
    }

    return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CSigmaViewPlot::OnCancelMode()
{
    CView::OnCancelMode();


    if (GetCapture() == this) {
        ReleaseCapture();
    }
}

void CSigmaViewPlot::OnPlotTemplates()
{
    CTemplateDlg dlgTemplate;
    dlgTemplate.m_pPlot = m_pPlot;
    dlgTemplate.m_pDoc = (CDocument*)(GetDocument());
    dlgTemplate.DoModal();
}

void CSigmaViewPlot::OnPlotMaskFrom()
{
    if ((m_pPlot->curvecount < 1) || (m_pPlot->curveactive < 0) || (m_pPlot->curveactive >= m_pPlot->curvecount)) {
        return;
    }

    real_t fx = 0.0, fy = 0.0;

    ASSERT(m_pPlot->curve[m_pPlot->curveactive].y);
    if (m_pPlot->curve[m_pPlot->curveactive].y) {
        if (GetPlotxy((int_t) (m_ptBegin.x), (int_t) (m_ptBegin.y), &fx, &fy, m_pPlot->curve[m_pPlot->curveactive].axisused)) {
            CSigmaDoc *pDoc = GetDocument();
            if (pDoc) {
                pDoc->MaskColumnData(0, 1, fx, 0.0, -1, m_pPlot->curve[m_pPlot->curveactive].y->id, 0x01);
            }
        }
    }

    return;
}

void CSigmaViewPlot::OnPlotMaskTo()
{
    if ((m_pPlot->curvecount < 1) || (m_pPlot->curveactive < 0) || (m_pPlot->curveactive >= m_pPlot->curvecount)) {
        return;
    }

    real_t fx = 0.0, fy = 0.0;

    if (GetPlotxy((int_t) (m_ptBegin.x), (int_t) (m_ptBegin.y), &fx, &fy, m_pPlot->curve[m_pPlot->curveactive].axisused)) {
        if (m_pPlot->curve[m_pPlot->curveactive].y) {
            CSigmaDoc *pDoc = GetDocument();
            if (pDoc) {
                pDoc->MaskColumnData(0, 1, 0.0, fx, -1, m_pPlot->curve[m_pPlot->curveactive].y->id, 0x10);
            }
        }

    }

    return;
}

void CSigmaViewPlot::OnPlotUnmask()
{
    ASSERT(m_pPlot->curve[m_pPlot->curveactive].y);
    if (m_pPlot->curve[m_pPlot->curveactive].y) {
        CSigmaDoc *pDoc = GetDocument();
        if (pDoc) {
            pDoc->MaskColumnData(0, 0, 0.0, 0.0, -1, m_pPlot->curve[m_pPlot->curveactive].y->id, 0x11);
        }
    }

    return;
}

HICON CSigmaViewPlot::GetIconForItem(UINT itemID) const
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

void CSigmaViewPlot::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
    if ((lpdis == NULL) || (lpdis->CtlType != ODT_MENU)) {
        CView::OnDrawItem(nIDCtl, lpdis);
        return; //not for a menu
    }
    if (lpdis->itemState & ODS_GRAYED) {
        CView::OnDrawItem(nIDCtl, lpdis);
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

void CSigmaViewPlot::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
    CView::OnInitMenuPopup(pMenu, nIndex, bSysMenu);

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

void CSigmaViewPlot::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
    if ((lpmis == NULL) || (lpmis->CtlType != ODT_MENU)) {
        CView::OnMeasureItem(nIDCtl, lpmis); //not for a menu
        return;
    }

    lpmis->itemWidth = 16;
    lpmis->itemHeight = 16;

    CMenu *pMenu = GetMenu();
    ASSERT(pMenu);
    if (pMenu == NULL) {
        CView::OnMeasureItem(nIDCtl, lpmis);
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

BOOL CSigmaViewPlot::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if ((GetFocus() == this) && (pMsg->message == WM_KEYDOWN) && CTRL_Pressed()) {
            if ((pMsg->wParam == _T('c')) || (pMsg->wParam == _T('C'))) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    CopyItem();
                    return TRUE;
                }
            }
            else if (m_ItemCopy.isset && ((pMsg->wParam == _T('v')) || (pMsg->wParam == _T('V')))) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    PasteItem();
                    return TRUE;
                }
            }
        }
    }

    return CView::PreTranslateMessage(pMsg);
}

void CSigmaViewPlot::CopyItem(void)
{
    m_ItemCopy.pastex = m_ItemCopy.pastey = 0;

    if (m_nTextSelected >= 0) {
        if (m_pPlot->textcount < PL_MAXITEMS) {
            memcpy(&(m_ItemCopy.text), &(m_pPlot->text[m_nTextSelected]), sizeof(text_t));
            m_ItemCopy.isset = true;
            m_ItemCopy.type = ITEMCOPY_TEXT;
        }
    }
    else if (m_nLineSelected >= 0) {
        if (m_pPlot->linecount < PL_MAXITEMS) {
            memcpy(&(m_ItemCopy.line), &(m_pPlot->line[m_nLineSelected]), sizeof(line_t));
            m_ItemCopy.isset = true;
            m_ItemCopy.type = ITEMCOPY_LINE;
        }
    }
    else if (m_nRectSelected >= 0) {
        if (m_pPlot->rectanglecount < PL_MAXITEMS) {
            memcpy(&(m_ItemCopy.rectangle), &(m_pPlot->rectangle[m_nRectSelected]), sizeof(rectangle_t));
            m_ItemCopy.isset = true;
            m_ItemCopy.type = ITEMCOPY_RECTANGLE;
        }
    }
    else if (m_nEllipseSelected >= 0) {
        if (m_pPlot->ellipsecount < PL_MAXITEMS) {
            memcpy(&(m_ItemCopy.ellipse), &(m_pPlot->ellipse[m_nEllipseSelected]), sizeof(ellipse_t));
            m_ItemCopy.isset = true;
            m_ItemCopy.type = ITEMCOPY_ELLIPSE;
        }
    }
}

void CSigmaViewPlot::PasteItem(void)
{
    if (false == m_ItemCopy.isset) {
        return;
    }

    bool bRedraw = false;
    rect_t *pRect = NULL;

    if (ITEMCOPY_TEXT == m_ItemCopy.type) {
        if (m_pPlot->textcount < PL_MAXITEMS) {
            memcpy( &(m_pPlot->text[m_pPlot->textcount]), &(m_ItemCopy.text), sizeof(text_t));
            pRect = &(m_pPlot->text[m_pPlot->textcount].rect);
            m_nTextSelected = m_pPlot->textcount;
            m_pPlot->textcount += 1;
            bRedraw = true;
        }
    }
    else if (ITEMCOPY_LINE == m_ItemCopy.type) {
        if (m_pPlot->linecount < PL_MAXITEMS) {
            memcpy( &(m_pPlot->line[m_pPlot->linecount]), &(m_ItemCopy.line), sizeof(line_t));
            pRect = &(m_pPlot->line[m_pPlot->linecount].rect);
            m_nLineSelected = m_pPlot->linecount;
            m_pPlot->linecount += 1;
            bRedraw = true;
        }
    }
    else if (ITEMCOPY_RECTANGLE == m_ItemCopy.type) {
        if (m_pPlot->rectanglecount < PL_MAXITEMS) {
            memcpy( &(m_pPlot->rectangle[m_pPlot->rectanglecount]), &(m_ItemCopy.rectangle), sizeof(rectangle_t));
            pRect = &(m_pPlot->rectangle[m_pPlot->rectanglecount].rect);
            m_nRectSelected = m_pPlot->rectanglecount;
            m_pPlot->rectanglecount += 1;
            bRedraw = true;
        }
    }
    else if (ITEMCOPY_ELLIPSE == m_ItemCopy.type) {
        if (m_pPlot->ellipsecount < PL_MAXITEMS) {
            memcpy( &(m_pPlot->ellipse[m_pPlot->ellipsecount]), &(m_ItemCopy.ellipse), sizeof(ellipse_t));
            pRect = &(m_pPlot->ellipse[m_pPlot->ellipsecount].rect);
            m_nEllipseSelected = m_pPlot->ellipsecount;
            m_pPlot->ellipsecount += 1;
            bRedraw = true;
        }
    }

    if (NULL != pRect) {
        if ((m_ItemCopy.pastex > 0) && (m_ItemCopy.pastey > 0)) {
            real_t wx = pRect->xb - pRect->xa;
            real_t wy = pRect->yb - pRect->ya;
            real_t xp = (real_t) (m_ItemCopy.pastex) / (real_t) (m_pPlot->plint.frameRect[0].right - m_pPlot->plint.frameRect[0].left);
            real_t yp = (real_t) (m_ItemCopy.pastey) / (real_t) (m_pPlot->plint.frameRect[0].bottom - m_pPlot->plint.frameRect[0].top);
            pRect->xa = xp;
            pRect->ya = yp;
            pRect->xb = pRect->xa + wx;
            pRect->yb = pRect->ya + wy;
            m_ItemCopy.pastex = m_ItemCopy.pastey = 0;
        }
        else {
            real_t xoff, yoff, roff;
            roff = (0.2 * (real_t) (rand()) / (real_t) (RAND_MAX));
            xoff = (pRect->xb - pRect->xa) * roff;
            yoff = (pRect->yb - pRect->ya) * 1.1;
            pRect->xa += xoff;
            pRect->xb += xoff;
            pRect->ya += yoff;
            pRect->yb += yoff;
        }

        if (++(m_ItemCopy.count) > (PL_MAXITEMS / 2)) {
            m_ItemCopy.count = 0;
            m_ItemCopy.isset = false;
            m_ItemCopy.type = ITEMCOPY_NONE;
        }

        if (bRedraw) {
            CSigmaDoc* pDoc = GetDocument();
            ASSERT_VALID(pDoc);
            if (pDoc) {
                pDoc->SetModifiedFlag(TRUE);
                pDoc->SaveState();
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
    }
    else {
        m_ItemCopy.count = 0;
        m_ItemCopy.isset = false;
        m_ItemCopy.type = ITEMCOPY_NONE;
    }
}

void CSigmaViewPlot::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}

