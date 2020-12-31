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

#include "EditReal.h"
#include "afxwin.h"

// CSeriesDlg dialog

class CSeriesDlg : public CDialog
{
    DECLARE_DYNAMIC(CSeriesDlg)

public:
    CSeriesDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CSeriesDlg();

// Dialog Data
    enum { IDD = IDD_SERIES };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    BOOL m_bOK;
    CDocument *m_pDoc;
    vector_t *m_pColumn;
    CEditReal m_edtFrom;
    CEditReal m_edtTo;
    CEditReal m_edtIncrement;
    CEditReal m_edtMultiplier;
    real_t m_fFrom;
    BOOL m_bTo;
    real_t m_fTo;
    real_t m_fIncrement;
    real_t m_fMultiplier;

    afx_msg void OnOK();
    afx_msg void OnCancel();
    virtual BOOL OnInitDialog();
};
