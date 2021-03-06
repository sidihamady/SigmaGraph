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

#include "ConsoleEdit.h"

// CNumPadDlg dialog

class CNumPadDlg : public CDialog
{
    DECLARE_DYNAMIC(CNumPadDlg)

public:
    CNumPadDlg(CWnd* pParent = NULL);                        // standard constructor
    virtual ~CNumPadDlg();

    BOOL Create(UINT nID, CWnd *pWnd) { return CDialog::Create(nID, pWnd); }

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_NUMPAD };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
    virtual void PostNcDestroy();
    virtual void OnOK();
    virtual void OnCancel();

    DECLARE_MESSAGE_MAP()

public:
    CConsoleEdit *m_pConsoleEdit;

    void InsertText(const char_t *szText, int_t nRelPos = 0);

    afx_msg void OnInputZero();
    afx_msg void OnInputOne();
    afx_msg void OnInputTwo();
    afx_msg void OnInputThree();
    afx_msg void OnInputFour();
    afx_msg void OnInputFive();
    afx_msg void OnInputSix();
    afx_msg void OnInputSeven();
    afx_msg void OnInputEight();
    afx_msg void OnInputNine();
    afx_msg void OnInputExp();
    afx_msg void OnInputDot();
    afx_msg void OnInputPlus();
    afx_msg void OnInputMinus();
    afx_msg void OnInputMult();
    afx_msg void OnInputDiv();
    afx_msg void OnInputParenth1();
    afx_msg void OnInputParenth2();
    afx_msg void OnInputBack();
    afx_msg void OnInputEqual();
    afx_msg void OnClose();
};
