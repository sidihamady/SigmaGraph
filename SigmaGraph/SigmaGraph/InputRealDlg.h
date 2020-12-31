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

// CInputRealDlg dialog

class CInputRealDlg : public CDialog
{
    DECLARE_DYNAMIC(CInputRealDlg)

public:
    CInputRealDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CInputRealDlg();

    BOOL m_bIsModified;

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_INPUT_REAL };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CString m_strTitle;
    real_t* m_pfInput1;
    real_t* m_pfInput2;
    CEditReal m_edtInput1;
    real_t m_fInput1;
    CString m_strLabel1;
    CEditReal m_edtInput2;
    real_t m_fInput2;
    CString m_strLabel2;
    BOOL m_bInitialized;
    BOOL m_bOK;
    int_t m_iLimit;

    afx_msg void OnEditInput();
    afx_msg void OnOK();
    afx_msg void OnCancel();
};
