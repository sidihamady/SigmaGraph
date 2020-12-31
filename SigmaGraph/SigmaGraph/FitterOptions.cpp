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

// :TODO: complete doc and translate to english

#include "stdafx.h"
#include "Sigma.h"
#include "FitterOptions.h"


// CFitterOptions dialog

IMPLEMENT_DYNAMIC(CFitterOptions, CPropertyPage)

CFitterOptions::CFitterOptions()
    : CPropertyPage(CFitterOptions::IDD)
{
    m_nIter = SigmaApp.m_Options.fit.iter;
    m_fLambda = SigmaApp.m_Options.fit.lambda;
    m_fTolerance = SigmaApp.m_Options.console.tol;
    m_fRelTolerance = SigmaApp.m_Options.console.reltol;
    m_bIsModified = FALSE;
}

CFitterOptions::~CFitterOptions()
{

}

void CFitterOptions::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_FITTER_LAMBDA, m_edtLambda);
    DDX_Control(pDX, IDE_FITTER_TOL, m_edtTolerance);
    DDX_Control(pDX, IDE_FITTER_RELTOL, m_edtRelTolerance);
    DDX_Text(pDX, IDE_FITTER_LAMBDA, m_fLambda);
    DDX_Text(pDX, IDE_FITTER_TOL, m_fTolerance);
    DDX_Text(pDX, IDE_FITTER_RELTOL, m_fRelTolerance);
    DDX_Text(pDX, IDE_FITTER_ITER, m_nIter);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}


BEGIN_MESSAGE_MAP(CFitterOptions, CPropertyPage)
    ON_EN_CHANGE(IDE_FITTER_ITER, &CFitterOptions::OnEnChangeFitterIter)
    ON_EN_CHANGE(IDE_FITTER_LAMBDA, &CFitterOptions::OnEnChangeFitterLambda)
    ON_EN_CHANGE(IDE_FITTER_TOL, &CFitterOptions::OnEnChangeFitterTol)
    ON_EN_CHANGE(IDE_FITTER_RELTOL, &CFitterOptions::OnEnChangeFitterReltol)
    ON_COMMAND(ID_HELP_CONTENTS, &CFitterOptions::OnHelpContents)
END_MESSAGE_MAP()


// CFitterOptions message handlers

BOOL CFitterOptions::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    m_edtLambda.SetLimitText(32);
    m_edtTolerance.SetLimitText(32);
    m_edtRelTolerance.SetLimitText(32);
    ::SendDlgItemMessage(GetSafeHwnd(), IDE_FITTER_ITER, EM_LIMITTEXT, (WPARAM)4, (LPARAM)0);

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    UpdateData(TRUE);    // Copy variables to data

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFitterOptions::OnApply()
{
    if (m_bIsModified) {
        UpdateData(TRUE);    // Copy data to variables

        // Quelques vérifications
        if ((m_fTolerance < (DBL_EPSILON * 10.0)) || (m_fTolerance > 1.0)) {
            m_fTolerance = SigmaApp.m_pLib->tol;
            UpdateData(FALSE);
        }
        if ((m_fRelTolerance < (DBL_EPSILON * 10.0)) || (m_fRelTolerance > 1.0)) {
            m_fRelTolerance = SigmaApp.m_pLib->reltol;
            UpdateData(FALSE);
        }
        if ((m_fLambda < ML_FIT_LAMBDAMIN) || (m_fLambda > ML_FIT_LAMBDAMAX)) {
            m_fLambda = SigmaApp.m_Options.fit.lambda;
            UpdateData(FALSE);
        }
        if ((m_nIter < 1) || (m_nIter > ML_MAXITER)) {
            m_nIter = SigmaApp.m_pLib->maxiter;
            UpdateData(FALSE);
        }
        //
        SigmaApp.m_Options.console.tol = m_fTolerance;
        SigmaApp.m_Options.console.reltol = m_fRelTolerance;
        SigmaApp.m_Options.fit.lambda = m_fLambda;
        SigmaApp.m_Options.fit.iter = m_nIter;
        SigmaApp.m_pLib->tol = SigmaApp.m_Options.console.tol;
        SigmaApp.m_pLib->reltol = SigmaApp.m_Options.console.reltol;
        SigmaApp.m_pLib->maxiter = m_nIter;
        ml_fit_init(&m_nIter, &m_fLambda);
    }

    CPropertyPage::OnApply();

    m_bIsModified = FALSE;

    return TRUE;
}

void CFitterOptions::OnEnChangeFitterIter()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CFitterOptions::OnEnChangeFitterLambda()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CFitterOptions::OnEnChangeFitterTol()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CFitterOptions::OnEnChangeFitterReltol()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CFitterOptions::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/fitting.html#fitting_options"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CFitterOptions::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if ((pMsg->wParam == VK_F1)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
        }
    }

    return CPropertyPage::PreTranslateMessage(pMsg);
}

void CFitterOptions::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}


