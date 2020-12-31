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


// CAnalysisDlg dialog

class CAnalysisDlg : public CDialog
{
    DECLARE_DYNAMIC(CAnalysisDlg)

public:
    CAnalysisDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CAnalysisDlg();

// Dialog Data
    enum { IDD = IDD_MATH };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CDocument *m_pDoc;
    plot_t *m_pPlot;
    vector_t *m_pColumn;
    int_t m_nColumnCount;
    int_t m_nSelectedColumnIn;
    int_t m_nSelectedColumnOut;
    CListBox m_lstColumnIn;
    CListBox m_lstColumnOut;
    BOOL m_bNewColumn;
    BOOL m_bNewColumnDone;
    BOOL m_bAddCurve;
    BOOL m_bCurveAdded;

    CComboBox m_cbOperation;
    int_t m_iOperation;

    int_t m_iAverage;

    BOOL m_bInitialized;
    BOOL m_bIsModified;
    BOOL m_bShift;

    CString m_strOutput;

    virtual BOOL OnInitDialog();
    afx_msg void OnLbnSelchangeMathOutput();
    afx_msg void OnLbnSelchangeMathInput();
    afx_msg void OnCalc();
    afx_msg void OnOK();
    afx_msg void OnClose();
    afx_msg void OnCbnSelchangeMathOperation();
};
