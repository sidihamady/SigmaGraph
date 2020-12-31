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

#include "ComboBoxDot.h"
#include "ComboBoxLine.h"
#include "ComboBoxColor.h"

// CAddCurveDlg dialog

class CAddCurveDlg : public CDialog
{
    DECLARE_DYNAMIC(CAddCurveDlg)

public:
    CAddCurveDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CAddCurveDlg();

// Dialog Data
    enum { IDD = IDD_ADDCURVE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CDocument* m_pDoc;
    vector_t *m_pColumn;
    vector_t *m_pColumnAutoX;
    int_t m_nColumnCount;
    int_t m_nSelectedColumnX;
    int_t m_nSelectedColumnY;
    CListBox m_lstColumnX;
    CListBox m_lstColumnY;
    int_t m_nCurve;

    plot_t *m_pPlot;

    CComboBoxLine m_cbLineStyle;
    BOOL m_bLineStyle;

    CComboBoxDot m_cbDotStyle;
    BOOL m_bDotStyle;

    CComboBoxColor m_cbLineColor;
    CComboBoxColor m_cbDotColor;
    BOOL m_bLineColor;
    BOOL m_bDotColor;

    BOOL m_bInitialized;
    BOOL m_bIsModified;

    BOOL m_bAxisXY;
    BOOL m_bAxisXY2;
    BOOL m_bAxisX2Y2;
    BOOL m_bAxisX2Y;
    CButton m_bnAdd;
    CButton m_bnRemove;
    CString m_strOutput;

    afx_msg void OnPlotLineSize();
    afx_msg void OnPlotDotSize();
    afx_msg void OnCbnSelchangeCurveLinesize();
    afx_msg void OnCbnSelchangeCurveDotsize();
    afx_msg void OnCbnSelchangeCurveLinecolor();
    afx_msg void OnCbnSelchangeCurveDotcolor();
    afx_msg void OnCbnSelchangeCurveDotstyle();
    afx_msg void OnCbnSelchangeCurveLinestyle();

    virtual BOOL OnInitDialog();
    afx_msg void OnCurveAdd();
    afx_msg void OnCurveRemove();
    afx_msg void OnPaint();
    afx_msg void OnLbnSelchangeColumnY();
    afx_msg void OnOK();
    afx_msg void OnClose();
};
