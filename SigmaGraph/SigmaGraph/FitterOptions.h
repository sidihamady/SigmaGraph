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

// CFitterOptions dialog

class CFitterOptions : public CPropertyPage
{
    DECLARE_DYNAMIC(CFitterOptions)

public:
    CFitterOptions();
    virtual ~CFitterOptions();

// Dialog Data
    enum { IDD = IDD_FITTER_OPTIONS };

    int_t m_nIter;

    CEditReal m_edtLambda;
    CEditReal m_edtTolerance;
    CEditReal m_edtRelTolerance;

    CFlatButton m_btnHelp;

    real_t m_fLambda;
    real_t m_fTolerance;
    real_t m_fRelTolerance;

    BOOL m_bIsModified;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnApply();
    afx_msg void OnEnChangeFitterIter();
    afx_msg void OnEnChangeFitterLambda();
    afx_msg void OnEnChangeFitterTol();
    afx_msg void OnEnChangeFitterReltol();
    afx_msg void OnHelpContents();

    afx_msg void OnDestroy();
};
