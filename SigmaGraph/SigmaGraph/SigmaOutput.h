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

#include "EditOutput.h"

// CSigmaOutput dialog

class CSigmaOutput : public CDialog
{
    DECLARE_DYNAMIC(CSigmaOutput)

public:
    CSigmaOutput(CWnd* pParent = NULL);                    // standard constructor
    virtual ~CSigmaOutput();

    BOOL Create(CWnd *pWnd) { return CDialog::Create(CSigmaOutput::IDD, pWnd); }

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_OUTPUT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    virtual void OnOK();
    virtual void OnCancel();

    BOOL m_bInitialized;

    DECLARE_MESSAGE_MAP()

public:
    CString m_strOutput;
    CEditOutput m_edtOutput;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint ptT);
    afx_msg void OnSize(UINT nType, int_t cx, int_t cy);
    afx_msg void OnOutputReset();
    afx_msg void OnClose();

    afx_msg void OnDestroy();
};
