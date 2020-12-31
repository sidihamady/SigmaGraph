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


// CColumnDlg dialog

class CColumnDlg : public CDialog
{
    DECLARE_DYNAMIC(CColumnDlg)

public:
    CColumnDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CColumnDlg();

// Dialog Data
    enum { IDD = IDD_COLUMN };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CDocument* m_pDoc;
    vector_t *m_pColumn;
    int_t m_nColumnCount;
    int_t m_nCurrentColumn;
    int_t *m_pSelectedColumnIndex;
    CEdit m_edtLabel;
    BOOL m_bShowLabel;
    BOOL m_bShowName;
    BOOL m_bShowBoth;
    CComboBox m_cbFormat;
    int_t m_nPrefix;
    int_t m_nSuffix;
    BOOL m_bColumnX;
    BOOL m_bColumnY;
    BOOL m_bColumnBarX;
    BOOL m_bColumnBarY;

    void VariableToControl();
    void ControlToVariable();

    virtual BOOL OnInitDialog();
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnDeltaposColumnCurrent(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnApply();
};
