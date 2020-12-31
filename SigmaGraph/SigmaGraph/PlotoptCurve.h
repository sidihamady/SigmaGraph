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
#include "afxwin.h"

#include "ComboBoxColor.h"
#include "ComboBoxLine.h"
#include "ComboBoxConnect.h"
#include "ComboBoxDot.h"
#include "FlatButton.h"

// CPlotoptCurve dialog

class CPlotoptCurve : public CPropertyPage
{
    DECLARE_DYNAMIC(CPlotoptCurve)

public:
    CPlotoptCurve();
    virtual ~CPlotoptCurve();

    BOOL m_bIsModified;
    BOOL m_bClosing;

    void initializeCurve(void);
    virtual BOOL OnInitDialog();
    virtual BOOL OnApply();

    BOOL m_bInitialized;

    BOOL m_bTemplate;

// Dialog Data
    enum { IDD = IDD_PLOTOPT_CURVE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    plot_t* m_pPlot;
    CDocument* m_pDoc;

    // Current curve
    int_t m_nCurve;
    // Curve legend
    CString m_strLegend;
    BOOL m_bOLegend;

    CComboBoxColor m_cbLineColor;
    CComboBoxColor m_cbDotColor;
    CComboBoxColor m_cbDropColor;
    CComboBoxColor m_cbDropColorH;
    BOOL m_bLineColor;
    BOOL m_bDotColor;
    BOOL m_bDropColor;
    BOOL m_bDropColorH;

    CComboBoxLine m_cbLineStyle;
    BOOL m_bLineStyle;
    CComboBoxLine m_cbDropvStyle;
    BOOL m_bDropvStyle;
    CComboBoxLine m_cbDrophStyle;
    BOOL m_bDrophStyle;
    CComboBoxDot m_cbDotStyle;
    BOOL m_bDotStyle;
    CComboBoxConnect m_cbConnectStyle;
    BOOL m_bConnectStyle;

    CFlatButton m_btnHelp;

    BOOL m_bAxisXY;
    BOOL m_bAxisXY2;
    BOOL m_bAxisX2Y2;
    BOOL m_bAxisX2Y;

    CButton m_bnActive;
    CButton m_bnHide;
    CButton m_bnRemove;

    BOOL OnCurveApply(int_t iCurve);

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnEditPlotLegend();
    afx_msg void OnPlotColor();
    afx_msg void OnPlotLineStyle();
    afx_msg void OnPlotDropvStyle();
    afx_msg void OnPlotDrophStyle();
    afx_msg void OnPlotDotStyle();
    afx_msg void OnPlotConnectStyle();
    afx_msg void OnPlotDotSize();
    afx_msg void OnPlotLineSize();
    afx_msg void OnClose();
    afx_msg void OnPaint();
    afx_msg void OnCurveHide();
    afx_msg void OnCurveApplyAll();
    afx_msg void OnPlotDefault();
    afx_msg void OnCbnSelchangeLineStyle();
    afx_msg void OnCbnSelchangeDropvStyle();
    afx_msg void OnCbnSelchangeDrophStyle();
    afx_msg void OnCbnSelchangeDotStyle();
    afx_msg void OnCbnSelchangeConnectStyle();
    afx_msg void OnCbnSelchangeCurveLinesize();
    afx_msg void OnCbnSelchangeCurveDotsize();
    afx_msg void OnCbnSelchangeCurveLinecolor();
    afx_msg void OnCbnSelchangeCurveDotcolor();
    afx_msg void OnCbnSelchangeCurveDropsize();
    afx_msg void OnCbnSelchangeCurveDropcolor();
    afx_msg void OnCbnSelchangeCurveDropsizeH();
    afx_msg void OnCbnSelchangeCurveDropcolorH();
    afx_msg void OnBnClickedCurveLegend();
    afx_msg void OnCbnSelchangeCurve();
    afx_msg void OnCurveFont();
    afx_msg void OnCurveRemove();
    afx_msg void OnCurveActive();
    afx_msg void OnAxisXY();
    afx_msg void OnAxisXY2();
    afx_msg void OnAxisX2Y2();
    afx_msg void OnAxisX2Y();
    afx_msg void OnHelpContents();

    afx_msg void OnDestroy();
};
