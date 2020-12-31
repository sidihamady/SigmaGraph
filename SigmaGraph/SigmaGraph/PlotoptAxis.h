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

// CPlotoptAxis dialog

#include "afxwin.h"

#include "ComboBoxColor.h"

class CPlotoptAxis : public CPropertyPage
{
    DECLARE_DYNAMIC(CPlotoptAxis)

public:
    CPlotoptAxis();
    virtual ~CPlotoptAxis();

    BOOL m_bIsModified;
    BOOL m_bClosing;

    virtual BOOL OnInitDialog();
    virtual BOOL OnApply();

    BOOL m_bInitialized;
    BOOL Initialize();

    BOOL m_bTemplate;

// Dialog Data
    enum { IDD = IDD_PLOTOPT_AXES };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    plot_t* m_pPlot;
    CDocument* m_pDoc;

    BOOL m_bOTitle;
    BOOL m_bAxis;
    BOOL m_bOGrid;
    BOOL m_bOTick;
    BOOL m_bOGridM;
    BOOL m_bOTickM;
    BOOL m_bOLabel;
    BOOL m_bBottomAxis;
    BOOL m_bLeftAxis;
    BOOL m_bTopAxis;
    BOOL m_bRightAxis;
    CString m_strTitle;
    int_t m_nAxisSize;
    int_t m_nGridCount;
    int_t m_nGridMCount;

    BOOL m_bFontColor;
    BOOL m_bAxisColor;
    BOOL m_bLabelColor;
    BOOL m_bGridColor;
    BOOL m_bGridMColor;
    CComboBoxColor m_cbFontColor;
    CComboBoxColor m_cbAxisColor;
    CComboBoxColor m_cbLabelColor;
    CComboBoxColor m_cbGridColor;
    CComboBoxColor m_cbGridMColor;

    CEdit m_edtLink;
    CString m_strLink;
    CString m_strLinkOutput;

    BOOL AxisLinkParse();

    CFlatButton m_btnHelp;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnAxisTitleFont();
    afx_msg void OnAxisLabelFont();
    afx_msg void OnAxisBottom();
    afx_msg void OnAxisLeft();
    afx_msg void OnAxisTop();
    afx_msg void OnAxisRight();
    afx_msg void OnAxisApply();
    afx_msg void OnAxisApplyAll();
    afx_msg void OnEnChangeAxisSize();
    afx_msg void OnBnClickedAxisOtitle();
    afx_msg void OnBnClickedAxisStatus();
    afx_msg void OnCbnSelchangeFontcolor();
    afx_msg void OnCbnSelchangeAxiscolor();
    afx_msg void OnCbnSelchangeLabelcolor();
    afx_msg void OnBnClickedAxisOgrid();
    afx_msg void OnBnClickedAxisOtick();
    afx_msg void OnBnClickedAxisOgridM();
    afx_msg void OnBnClickedAxisOtickM();
    afx_msg void OnBnClickedAxisOlabel();
    afx_msg void OnCbnSelchangeGridcolor();
    afx_msg void OnCbnSelchangeGridMcolor();
    afx_msg void OnEnChangeAxisGridcount();
    afx_msg void OnEnChangeAxisGridMcount();
    afx_msg void OnPlotDefault();
    afx_msg void OnEnChangeAxisTitle();
    afx_msg void OnEnChangeAxisLink();
    afx_msg void OnPaint();
    afx_msg void OnHelpContents();

    afx_msg void OnDestroy();
};
