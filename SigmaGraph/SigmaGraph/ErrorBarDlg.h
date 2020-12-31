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

#include "EditReal.h"
#include "ComboBoxLine.h"
#include "ComboBoxColor.h"
#include "Datasheet.h"

// CErrorBarDlg dialog

class CErrorBarDlg : public CDialog
{
    DECLARE_DYNAMIC(CErrorBarDlg)

public:
    CErrorBarDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CErrorBarDlg();

// Dialog Data
    enum { IDD = IDD_ERRORBAR };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CDocument* m_pDoc;
    plot_t *m_pPlot;
    CDatasheet *m_pDatasheet;
    vector_t *m_pColumnX;
    vector_t *m_pColumnY;
    vector_t *m_pColumnErrX;
    vector_t *m_pColumnErrY;
    int_t m_nColumnCount;
    int_t m_nCurveSelected;

    BOOL m_bErrX;
    BOOL m_bErrY;
    BOOL m_bPercent;
    BOOL m_bFixed;
    CEditReal m_edtPercent;
    CEditReal m_edtFixed;
    real_t m_fPercent;
    real_t m_fFixed;
    BOOL m_bColumnX;
    BOOL m_bColumnY;
    CButton m_bnColumnX;
    CButton m_bnColumnY;
    CComboBox m_cbColumnX;
    CComboBox m_cbColumnY;

    CComboBox m_cbLineSize;
    BOOL m_bLineSize;

    CComboBoxLine m_cbLineStyle;
    BOOL m_bLineStyle;

    CComboBoxColor m_cbLineColor;
    BOOL m_bLineColor;

    void VariableToControl();
    void ControlToVariable();

    BOOL m_bInitialized;
    CButton m_bnApply;

    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedErrbarX();
    afx_msg void OnBnClickedErrbarY();
    afx_msg void OnLineSize();
    afx_msg void OnCbnSelchangeLinecolor();
    afx_msg void OnCbnSelchangeLinestyle();
    afx_msg void OnCbnSelchangeLinesize();
    afx_msg void OnBnClickedErrbarPercent();
    afx_msg void OnBnClickedErrbarFixed();
    afx_msg void OnEnChangeErrbarPercent();
    afx_msg void OnEnChangeErrbarFixed();
    afx_msg void OnBnClickedErrbarColumnx();
    afx_msg void OnBnClickedErrbarColumny();
    afx_msg void OnCbnSelchangeErrbarColumnx();
    afx_msg void OnCbnSelchangeErrbarColumny();
    afx_msg void OnOK();
    afx_msg void OnRemove();
    afx_msg void OnCancel();
    afx_msg void OnClose();
};
