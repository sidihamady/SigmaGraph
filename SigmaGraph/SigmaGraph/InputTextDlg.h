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

// CInputTextDlg dialog

class CInputTextDlg : public CDialog
{
    DECLARE_DYNAMIC(CInputTextDlg)

public:
    CInputTextDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CInputTextDlg();

    BOOL m_bIsModified;

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_INPUT_TEXT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    char_t* m_pszInput1;
    char_t* m_pszInput2;
    CEdit m_edtInput1;
    CString m_strTitle;
    CString m_strInput1;
    CString m_strLabel1;
    CEdit m_edtInput2;
    CString m_strInput2;
    CString m_strLabel2;
    BOOL m_bInitialized;
    BOOL m_bOK;
    int_t m_iLimit;

    afx_msg void OnEditInput();
    afx_msg void OnOK();
    afx_msg void OnCancel();
};
