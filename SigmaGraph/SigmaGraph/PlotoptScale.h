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

#include "EditReal.h"

// CPlotoptScale dialog

class CPlotoptScale : public CPropertyPage
{
    DECLARE_DYNAMIC(CPlotoptScale)

public:
    CPlotoptScale();
    virtual ~CPlotoptScale();

    BOOL m_bIsModified;
    BOOL m_bClosing;

    virtual BOOL OnInitDialog();
    virtual BOOL OnApply();

    BOOL m_bInitialized;
    BOOL Initialize();

    BOOL m_bTemplate;

// Dialog Data
    enum { IDD = IDD_PLOTOPT_SCALE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    plot_t* m_pPlot;
    CDocument* m_pDoc;

    BOOL m_bBottomAxis;
    BOOL m_bLeftAxis;
    BOOL m_bTopAxis;
    BOOL m_bRightAxis;

    real_t m_fAxisMin;
    real_t m_fAxisMax;
    int_t m_nPrefix;
    int_t m_nSuffix;
    BOOL m_bAxisLinear;
    BOOL m_bAxisLog;
    BOOL m_bAxisAbs;

    // >>
    CEditReal m_edtAxisMin;
    CEditReal m_edtAxisMax;

    CFlatButton m_btnHelp;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void VariableToControl();
    void ControlToVariable();
    // <<

    afx_msg void OnAxisBottom();
    afx_msg void OnAxisLeft();
    afx_msg void OnAxisTop();
    afx_msg void OnAxisRight();
    afx_msg void OnCbnSelchangeAxisformat();
    afx_msg void OnEnChangeAxismin();
    afx_msg void OnEnChangeAxismax();
    afx_msg void OnEnChangeAxisprefix();
    afx_msg void OnEnChangeAxissuffix();
    afx_msg void OnPlotDefault();
    afx_msg void OnBnClickedAxisLinear();
    afx_msg void OnBnClickedAxisLog();
    afx_msg void OnBnClickedAxisAbs();
    afx_msg void OnBnClickedScaleAuto();
    afx_msg void OnPaint();
    afx_msg void OnHelpContents();

    afx_msg void OnDestroy();
};
