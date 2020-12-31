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

#include "../../LibGraph/LibFile/libfile.h"

#include "ComboBoxColor.h"

// CPlotoptGeneral dialog

class CPlotoptGeneral : public CPropertyPage
{
    DECLARE_DYNAMIC(CPlotoptGeneral)

public:
    CPlotoptGeneral();
    virtual ~CPlotoptGeneral();

    BOOL m_bIsModified;
    BOOL m_bClosing;

    virtual BOOL OnInitDialog();
    virtual BOOL OnApply();

    BOOL m_bInitialized;
    BOOL Initialize();

    BOOL m_bTemplate;

// Dialog Data
    enum { IDD = IDD_PLOTOPT_GENERAL };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    plot_t* m_pPlot;
    CDocument* m_pDoc;

    CComboBoxColor m_cbTitleColor;
    BOOL m_bTitleColor;
    CComboBoxColor m_cbPlotColor;
    BOOL m_bPlotColor;
    CComboBoxColor m_cbPlotColorB;
    BOOL m_bPlotColorB;

    CString m_strTitle;
    BOOL m_bOTitle;

    int_t m_iModPoints;

    CFlatButton m_btnHelp;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnPlotoptFont();
    afx_msg void OnFontChoose();
    afx_msg void OnPlotDefault();
    afx_msg void OnCbnSelchangeFontcolor();
    afx_msg void OnCbnSelchangePlotcolor();
    afx_msg void OnCbnSelchangePlotcolorB();
    afx_msg void OnBnClickedOtitle();
    afx_msg void OnEnChangeTitle();
    afx_msg void OnEnChangeModPoints();
    afx_msg void OnPaint();
    afx_msg void OnHelpContents();

    afx_msg void OnDestroy();
};
