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

#include "EditReal.h"

// CPlotoptLine dialog

class CPlotoptLine : public CPropertyPage
{
    DECLARE_DYNAMIC(CPlotoptLine)

public:
    CPlotoptLine();
    virtual ~CPlotoptLine();

    BOOL m_bIsModified;
    BOOL m_bClosing;

    virtual BOOL OnInitDialog();
    virtual BOOL OnApply();

    BOOL m_bInitialized;
    BOOL Initialize();

    BOOL m_bTemplate;

// Dialog Data
    enum { IDD = IDD_PLOTOPT_LINE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    plot_t* m_pPlot;
    CDocument* m_pDoc;

    CComboBoxLine m_cbLineStyle;
    BOOL m_bLineStyle;

    CComboBoxColor m_cbLineColor;
    BOOL m_bLineColor;

    // Current line
    int_t m_nLine;
    int_t m_nLineCount;

    CEditReal m_edtPos;
    CEditReal m_edtMin;
    CEditReal m_edtMax;

    BOOL m_bLineVert;
    BOOL m_bLineHorz;

    CButton m_bnAdd;
    CButton m_bnRemove;

    void DrawLine(CDC *pDC);

    BOOL OnLineApply(int_t iLine);

    void UpdateLine(BOOL bIsModified = FALSE);

    BOOL LinkParse(int_t iLink);

    CFlatButton m_btnHelp;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnEditPlotLegend();
    afx_msg void OnLineStyle();
    afx_msg void OnLineColor();
    afx_msg void OnLineSize();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint tPoint);
    afx_msg void OnClose();
    afx_msg void OnPaint();
    afx_msg void OnLineApplyAll();
    afx_msg void OnPlotDefault();
    afx_msg void OnCbnSelchangeLinestyle();
    afx_msg void OnCbnSelchangeLinesize();
    afx_msg void OnCbnSelchangeLinecolor();
    afx_msg void OnCbnSelchangeLine();
    afx_msg void OnLineRemove();
    afx_msg void OnLineAdd();
    afx_msg void OnLineHorz();
    afx_msg void OnLineVert();
    afx_msg void OnEnChangeLinePos();
    afx_msg void OnEnChangeLineMin();
    afx_msg void OnEnChangeLineMax();
    afx_msg void OnHelpContents();

    afx_msg void OnDestroy();
};
