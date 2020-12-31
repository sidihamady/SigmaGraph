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

// SigmaViewPlot.h : interface of the CSigmaViewPlot class
//

// :TODO: complete doc and translate to english

#pragma once

#include "SigmaDoc.h"
#include "Fitter.h"

#define ITEMCOPY_NONE           0
#define ITEMCOPY_TEXT           1
#define ITEMCOPY_LINE           2
#define ITEMCOPY_RECTANGLE      3
#define ITEMCOPY_ELLIPSE        4
typedef struct _itemcopy_t
{
    int_t type;
    bool isset;
    int_t count;
    int_t pastex;
    int_t pastey;
    text_t text;
    line_t line;
    rectangle_t rectangle;
    ellipse_t ellipse;
} itemcopy_t;

class CSigmaViewPlot : public CView
{
protected: // create from serialization only
    CSigmaViewPlot();
    DECLARE_DYNCREATE(CSigmaViewPlot)

// Attributes
public:
    CSigmaDoc* GetDocument() const;

    BOOL m_bInitialized;

    plot_t* m_pPlot;

    // Save min et max values from the Autoscale function
    real_t m_fXmin;
    real_t m_fXmax;
    real_t m_fYmin;
    real_t m_fYmax;
    //

    LARGE_INTEGER m_liTicToc;
    ulong_t m_ulTicToc;
    void Tic();
    int_t Toc();

    plot_t* GetPlot();

    void PlotInit();
    BOOL AxisLinkParse(real_t xx, real_t yy, real_t *flx, real_t *fly, char_t *szLx, char_t *szLy);
    void LogScale(real_t fmin, real_t fmax, real_t *fminl, real_t *fmaxl, byte_t *gridcount = NULL);
    BOOL GetPlotxy(int_t nx, int_t ny, real_t *xx, real_t *yy, byte_t axisused = 0x00);
    void PlotZoomIn(int_t nx, int_t ny, int_t nxPrev, int_t nyPrev);
    void PlotZoomOut(void);
    void PlotAddGeomtric(int_t nx, int_t ny, int_t nxPrev, int_t nyPrev, int_t nGeo);

    BOOL m_bAutoScale;
    void PlotAutoScale();

    BOOL GetZoomin() { return m_bZoomIn; }
    BOOL GetAddText() { return m_bAddText; }
    BOOL GetAddLine() { return m_bAddLine; }
    BOOL GetAddRect() { return m_bAddRect; }
    BOOL GetAddEllipse() { return m_bAddEllipse; }
    void SetAddText(BOOL bAddText) {
        m_bAddText = bAddText; 
        if (m_bAddText == FALSE) {
            m_bAddingText = FALSE;
        }
    }
    void SetAddLine(BOOL bAddLine) {
        m_bAddLine = bAddLine;
        if (m_bAddLine == FALSE) {
            m_bAddingLine = FALSE;
        }
    }
    void SetAddRect(BOOL bAddRect) {
        m_bAddRect = bAddRect;
        if (m_bAddRect == FALSE) {
            m_bAddingRect = FALSE;
        }
    }
    void SetAddEllipse(BOOL bAddEllipse) {
        m_bAddEllipse = bAddEllipse;
        if (m_bAddEllipse == FALSE) {
            m_bAddingEllipse = FALSE;
        }
    }

private:
    RECT m_precPos[PL_FRAMEITEMS];              //    Position des différents rectangles
                                                // 0:WND, 1:PLOT, 2:TITLE

    RECT m_recMov[9];                           //    Rectangles for resizing
                                                // 0:X-Axis, 1:Y, 2:X2, 3:Y2
                                                // 4:Center
                                                // 5:CornerXY, 6:CornerXY2, 7:CornerYX2, 8:CornerX2Y2

    POINT m_ptInit;

    int_t m_nResizingPlot;

    BOOL m_bPlotSelected;

    BOOL m_bZoomIn;

    BOOL m_bAddText;
    BOOL m_bAddLine;
    BOOL m_bAddRect;
    BOOL m_bAddEllipse;

    BOOL m_bAddingText;
    BOOL m_bAddingLine;
    BOOL m_bAddingRect;
    BOOL m_bAddingEllipse;

    int_t m_nMovingText;
    int_t m_nMovingLine;
    int_t m_nMovingRect;
    int_t m_nMovingEllipse;
    int_t m_nMovingLegend;
    int_t m_nMovingBottomTitle;
    int_t m_nMovingLeftTitle;
    int_t m_nMovingTopTitle;
    int_t m_nMovingRightTitle;

    int_t m_nResizingLine;
    int_t m_nResizingRect;
    int_t m_nResizingEllipse;
    int_t m_nResizingLineT;
    int_t m_nResizingRectT;
    int_t m_nResizingEllipseT;

    int_t m_nTextSelected;
    int_t m_nLineSelected;
    int_t m_nRectSelected;
    int_t m_nEllipseSelected;
    int_t m_nLegendSelected;
    int_t m_nTitleSelected;

    int_t m_nCurveSelected;

    RECT m_recText[PL_MAXITEMS];
    RECT m_recLine[PL_MAXITEMS];
    RECT m_recRect[PL_MAXITEMS];
    RECT m_recEllipse[PL_MAXITEMS];
    RECT m_recLegend;
    RECT m_recBottomTitle;
    RECT m_recLeftTitle;
    RECT m_recTopTitle;
    RECT m_recRightTitle;

    BOOL m_bZooming;
    POINT m_ptBegin;
    POINT m_ptEnd;

    LOGFONT m_deffont;

    HCURSOR m_hACursor;
    HCURSOR m_hZCursor;
    HCURSOR m_hMCursor;
    HCURSOR m_hMCursorH;
    HCURSOR m_hMCursorV;

    byte_t m_nxgrid;
    byte_t m_nygrid;

    itemcopy_t m_ItemCopy;

    Gdiplus::Graphics *m_pCanvas;
    RECT m_recTX;
    POINT m_pointTX;
    POINT m_pointTXe;
    BOOL PrepareGeo(HDC hDCX);
    void ReleaseGeo();
    void UpdateResizeLoc(irect_t *precFrame, RECT *precMov);

    void drawLineTX(POINT &pointTX, POINT &pointTXe)
    {
        drawLineTX(pointTX.x, pointTX.y, pointTXe.x, pointTXe.y);
    }
    void drawLineTX(LONG x1, LONG y1, LONG x2, LONG y2)
    {
        Gdiplus::Color backcolor(0, 128, 128);
        Gdiplus::Pen *penLine = new Gdiplus::Pen(backcolor);
        penLine->SetDashStyle(Gdiplus::DashStyleDot);
        penLine->SetLineJoin(Gdiplus::LineJoinBevel);
        m_pCanvas->DrawLine(penLine, x1, y1, x2, y2);
        delete penLine; penLine = NULL;
    }

    void drawRectangleTX(RECT &recTX)
    {
        drawRectangleTX(recTX.left, recTX.top, recTX.right, recTX.bottom);
    }
    void drawRectangleTX(LONG x1, LONG y1, LONG x2, LONG y2)
    {
        Gdiplus::Color backcolor(0, 128, 128);
        Gdiplus::Pen *penLine = new Gdiplus::Pen(backcolor);
        penLine->SetDashStyle(Gdiplus::DashStyleDot);
        penLine->SetLineJoin(Gdiplus::LineJoinBevel);
        m_pCanvas->DrawRectangle(penLine, x1, y1, x2 - x1, y2 - y1);
        delete penLine; penLine = NULL;
    }

    void drawEllipseTX(RECT &recTX)
    {
        drawEllipseTX(recTX.left, recTX.top, recTX.right, recTX.bottom);
    }
    void drawEllipseTX(LONG x1, LONG y1, LONG x2, LONG y2)
    {
        Gdiplus::Color backcolor(0, 128, 128);
        Gdiplus::Pen *penLine = new Gdiplus::Pen(backcolor);
        penLine->SetDashStyle(Gdiplus::DashStyleDot);
        penLine->SetLineJoin(Gdiplus::LineJoinBevel);
        m_pCanvas->DrawEllipse(penLine, x1, y1, x2 - x1, y2 - y1);
        delete penLine; penLine = NULL;
    }

// Operations
public:

// Overrides
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    BOOL m_bDoubleBuffer;
    void SetDoubleBuffering(BOOL bBuffer = TRUE) { m_bDoubleBuffer = bBuffer; }
    BOOL GetDoubleBuffering() { return m_bDoubleBuffer; }
    void Plot(CDC *pDC = NULL, long_t lwidth = 0, long_t lheight = 0, bool isPrinting = false);

    // Printing
    CSize m_CharSize;
    int_t m_nPageHeight;
    CSize m_LogicalPageSize,                    // Page size in datasheet units.
            m_PaperSize;                            // Page size in device units.
    int_t m_nPageWidth;
    int_t m_nPrintColumn;
    int_t m_nCurrPrintRow;
    int_t m_nNumPages;
    int_t m_nPageMultiplier;
    int_t m_nHeaderHeight, m_nFooterHeight, m_nLeftMargin,
        m_nRightMargin, m_nTopMargin, m_nBottomMargin, m_nGap;
    CFont m_PrinterFont;                            // Printer font
    void SetPrintMarginInfo(int_t nHeaderHeight, int_t nFooterHeight, int_t nLeftMargin, int_t nRightMargin,
                                    int_t nTopMargin, int_t nBottomMargin, int_t nGap);
    void Print(CPrintDialog* pPrintDialog = NULL);
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
    void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
    void PrintFooter(CDC *pDC, CPrintInfo *pInfo);

    BOOL IsItemSelected(CPoint tPoint);

protected:
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint tPoint);
    afx_msg void OnPaint();

// Implementation
public:
    virtual ~CSigmaViewPlot();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

public:
    void ShowProperties(int_t nStartupPage = 0, int_t nCurve = 0, int_t nAxis = -1);

    HICON GetIconForItem(UINT itemID) const;

    bool DoFileExport(CString &strFilename);

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnPlotOptions();
    afx_msg void OnPlotAutoScale();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint tPoint);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint tPoint);
    afx_msg void OnMouseMove(UINT nFlags, CPoint tPoint);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint tPoint);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint tPoint);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSize(UINT nType, int_t cx, int_t cy);
    afx_msg void OnViewZoomIn();
    afx_msg void OnViewZoomOut();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnCancelMode();

    void ShowFitter(int_t nDefaultIndex);

    afx_msg void OnFitterNonlinear();
    afx_msg void OnFitterLinear();
    afx_msg void OnFitterPolynomial();
    afx_msg void OnFitterGaussian();
    afx_msg void OnFitterLorentzian();
    afx_msg void OnFitterExponetial();
    afx_msg void OnFitterLogarithmic();
    afx_msg void OnFitterPower();
    afx_msg void OnFitterNote();

    virtual void OnInitialUpdate();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnViewData();
    afx_msg void OnViewDataFit();
    afx_msg void OnViewFitReport();
    afx_msg void OnAddText();
    afx_msg void OnAddLine();
    afx_msg void OnAddRect();
    afx_msg void OnAddEllipse();
    afx_msg void OnAddVertLine();
    afx_msg void OnAddHorzLine();
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnFileExport();
    afx_msg void OnItemRemove();

    void CopyItem(void);
    void PasteItem(void);
    afx_msg void OnItemCopy();
    afx_msg void OnItemPaste();

protected:
    virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);

public:
    void UpdateGraphMenu(CCmdUI *pCmdUI);
    afx_msg void OnCurveActive();
    afx_msg void OnCurveHide();
    afx_msg void OnCurveRemove();
    afx_msg void OnCurveProperties();
    afx_msg void OnErrorBar();
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnPlotTemplates();
    afx_msg void OnPlotMaskFrom();
    afx_msg void OnPlotMaskTo();
    afx_msg void OnPlotUnmask();
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
    afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);

    afx_msg void OnUpdateErrorBar(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePlotAutoscale(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterLinear(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterGaussian(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterLorentzian(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterPolynomial(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterExponential(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterLogarithmic(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterPower(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterNonlinear(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePlotAddtext(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePlotAddline(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePlotAddrect(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePlotAddellipse(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePlotZoomout(CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewZoomIn(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFitterNote(CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewData(CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewFit(CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewFitReport(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);

    afx_msg void OnDestroy();
};

#ifndef _DEBUG  // debug version in SigmaView.cpp
inline CSigmaDoc* CSigmaViewPlot::GetDocument() const
   { return reinterpret_cast<CSigmaDoc*>(m_pDocument); }
#endif
