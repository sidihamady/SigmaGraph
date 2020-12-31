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

// CTextDlg dialog

class CTextDlg : public CDialog
{
    DECLARE_DYNAMIC(CTextDlg)

public:
    CTextDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CTextDlg();

    BOOL m_bIsModified;

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_TEXT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    plot_t* m_pPlot;
    CDocument* m_pDoc;
    CString m_strInput;
    CString m_strLabel;
    BOOL m_bOrientHorz;
    BOOL m_bOrientVert;
    BOOL m_bOrientVert270;
    CComboBoxColor m_cbTextColor;
    BOOL m_bInitialized;
    RECT m_recFrame;
    POINT m_ptInit;
    BOOL m_bOK;
    int_t m_nSelectedText;
    BOOL m_bModifyText;
    BOOL m_bReadOnly;

    CButton m_bnOrientHorz;
    CButton m_bnOrientVert;
    CButton m_bnOrientVert270;
    CEdit m_edtInput;

    afx_msg void OnEditInput();
    afx_msg void OnOrient00();
    afx_msg void OnOrient90();
    afx_msg void OnOrient270();
    afx_msg void OnCbnSelchangeTextcolor();
    afx_msg void OnTextFont();
    afx_msg void OnOK();
    afx_msg void OnCancel();
};
