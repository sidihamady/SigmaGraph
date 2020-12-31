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

#include "PlotFrame.h"

// CTemplateDlg dialog

class CTemplateDlg : public CDialog
{
    DECLARE_DYNAMIC(CTemplateDlg)

private:
    char_t m_szBuffer[ML_STRSIZE];

public:
    CTemplateDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CTemplateDlg();

// Dialog Data
    enum { IDD = IDD_TEMPLATES };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CListBox m_lstTemplate;
    template_t m_Template;
    plot_t **m_ppPlot;
    plot_t *m_pPlot;
    int_t m_iPlot;
    vector_t *m_pColumnDataX;
    vector_t *m_pColumnDataY;
    vector_t *m_pColumnDataYi;
    HWND m_hWndPlot;
    CDocument* m_pDoc;
    CPlotFrame m_PlotFrame;

    CButton m_bnAdd;
    CButton m_bnRemove;
    CButton m_bnApply;
    CButton m_bnEdit;

    BOOL m_bDefault;

    void PlotCreateFont(plot_t *pPlot);
    void PlotDeleteFont(plot_t *pPlot);

    void UpdateTemplateList();

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnTemplateAdd();
    afx_msg void OnTemplateRemove();
    afx_msg void OnTemplateApply();
    afx_msg void OnTemplateEdit();
    afx_msg void OnLbnSelchangeTemplate();
    afx_msg void OnOK();
    afx_msg void OnClose();
    afx_msg void OnLbnDblclkTemplateList();
};
