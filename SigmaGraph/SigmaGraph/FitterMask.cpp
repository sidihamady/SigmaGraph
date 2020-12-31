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

#include "stdafx.h"
#include "Sigma.h"
#include "FitterMask.h"


// CFitterMask dialog

IMPLEMENT_DYNAMIC(CFitterMask, CPropertyPage)

CFitterMask::CFitterMask()
    : CPropertyPage(CFitterMask::IDD)
{
    m_pDoc = NULL;
    m_pPlot = NULL;
    m_pFit = NULL;
    m_pColumnDataX = NULL;
    m_pColumnDataY = NULL;
    m_pColumnFitX = NULL;
    m_pColumnFitY = NULL;

    m_pbIsModified = NULL;

    m_bCanUndo = FALSE;

    m_bMask = FALSE;
    m_bSmooth = FALSE;
    m_bBaseline = FALSE;

    memset(&m_ColumnUndoX, 0, sizeof(vector_t));
    memset(&m_ColumnUndoY, 0, sizeof(vector_t));
}

CFitterMask::~CFitterMask()
{
    SigmaApp.m_pLib->errcode = 0;
    ml_vector_delete(&m_ColumnUndoX, SigmaApp.m_pLib);
    ml_vector_delete(&m_ColumnUndoY, SigmaApp.m_pLib);
}

void CFitterMask::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDE_FITTER_NPOINTS, m_nPoints);
    DDX_Text(pDX, IDE_FITTER_FILTER, m_nSmooth);
    DDX_Control(pDX, IDE_CURVE, m_edtCurve);
    DDX_Control(pDX, IDE_FITTER_FILTER, m_edtFilter);
    DDX_Control(pDX, IDE_FITTER_MINX, m_edtMaskMin);
    DDX_Control(pDX, IDE_FITTER_MAXX, m_edtMaskMax);
    DDX_Control(pDX, IDE_FITTER_BASELINE0, m_edtBaseline0);
    DDX_Control(pDX, IDE_FITTER_BASELINE1, m_edtBaseline1);
    DDX_Check(pDX, IDK_FITTER_MASK, m_bMask);
    DDX_Check(pDX, IDK_FITTER_FILTER, m_bSmooth);
    DDX_Check(pDX, IDK_FITTER_BASELINE, m_bBaseline);
    DDX_Control(pDX, IDL_FITTER_OUTPUT, m_stOutput);
    DDX_Control(pDX, ID_UNDO, m_bnUndo);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}


BEGIN_MESSAGE_MAP(CFitterMask, CPropertyPage)
    ON_COMMAND(ID_FITTER_PROCESS, &CFitterMask::OnProcess)
    ON_COMMAND(ID_UNDO, &CFitterMask::OnUndo)
    ON_COMMAND(ID_HELP_CONTENTS, &CFitterMask::OnHelpContents)
END_MESSAGE_MAP()


// CFitterMask message handlers

BOOL CFitterMask::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // Data Initialization
    m_edtMaskMin.SetLimitText(32);
    m_edtMaskMax.SetLimitText(32);
    m_edtBaseline0.SetLimitText(32);
    m_edtBaseline1.SetLimitText(32);
    m_edtFilter.SetRange(1, 9, 1);

    real_t fMaskMin, fMaskMax;
    m_edtCurve.SetWindowText((LPCTSTR) (m_pPlot->curve[m_pPlot->curveactive].legend.text));

    m_nPoints = m_pColumnDataX->dim;
    m_iStart = 0;
    m_iStop = m_nPoints - 1;

    for (int ii = 0; ii < m_pColumnDataX->dim; ii++) {
        if (m_pColumnDataX->mask[ii] != 0xFF) {
            break;
        }
        m_iStart++;
    }
    for (int ii = m_pColumnDataX->dim - 1; ii >= 0; ii--) {
        if (m_pColumnDataX->mask[ii] != 0xFF) {
            break;
        }
        m_iStop--;
    }
    if (m_pColumnDataX->istart < m_iStart) {
        m_pColumnDataX->istart = m_iStart;
    }
    if (m_pColumnDataX->istop > m_iStop) {
        m_pColumnDataX->istop = m_iStop;
    }
    fMaskMin = m_pColumnDataX->dat[m_pColumnDataX->istart];
    fMaskMax = m_pColumnDataX->dat[m_pColumnDataX->istop];

    m_nPoints = m_iStop - m_iStart + 1;
    m_bMask = (m_iStart > 0) || (m_iStop < (m_pColumnDataX->dim - 1));

    m_edtMaskMin.SetValue(fMaskMin);
    m_edtMaskMax.SetValue(fMaskMax);

    m_nSmooth = 2;

    m_edtBaseline0.SetValue(0.0);
    m_edtBaseline1.SetValue(0.0);

    m_bCanUndo = FALSE;
    SigmaApp.m_pLib->errcode = 0;
    ml_vector_create(&m_ColumnUndoX, m_pColumnDataX->dim, 0, _T("UndoX"), NULL, SigmaApp.m_pLib);
    ml_vector_create(&m_ColumnUndoY, m_pColumnDataY->dim, 0, _T("UndoY"), NULL, SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode != 0) {
        CWnd *pWnd = GetDlgItem(ID_UNDO);
        if (pWnd != NULL) {
            pWnd->EnableWindow(TRUE);
        }
    }
    else {
        ml_vector_dup(&m_ColumnUndoX, (const vector_t*)m_pColumnDataX, SigmaApp.m_pLib);
        ml_vector_dup(&m_ColumnUndoY, (const vector_t*)m_pColumnDataY, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            m_bCanUndo = TRUE;
        }
    }

    m_bnUndo.EnableWindow(*m_pbIsModified);

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CFitterMask::OnProcess()
{
    if (*m_pbRunning == TRUE) {
        return;
    }

    *m_pbRunning = TRUE;

    m_stOutput.SetWindowText(_T(""));

    CWaitCursor wcT;

    UpdateData(TRUE);

    real_t fMaskMin = m_edtMaskMin.GetValue();
    real_t fMaskMax = m_edtMaskMax.GetValue();

    if ((m_bSmooth == TRUE) && ((m_nSmooth < 1) || (m_nSmooth >((m_iStop - m_iStart + 1) / 5)))) {
        m_bSmooth = FALSE;
        UpdateData(FALSE);
    }
    if (m_bMask == TRUE) {
        if ((fMaskMin < m_pColumnDataX->dat[m_iStart]) && (fMaskMax > m_pColumnDataX->dat[m_iStop])) {
            fMaskMin = m_pColumnDataX->dat[m_iStart];
            fMaskMax = m_pColumnDataX->dat[m_iStop];
            m_edtMaskMin.SetValue(fMaskMin);
            m_edtMaskMin.SetValue(fMaskMax);
            m_stOutput.SetWindowText(_T("Mask value outside range. Fixed."));
            UpdateData(FALSE);
        }
        if (fMaskMin < m_pColumnDataX->dat[m_iStart]) {
            fMaskMin = m_pColumnDataX->dat[m_iStart];
            m_edtMaskMin.SetValue(fMaskMin);
            m_stOutput.SetWindowText(_T("Mask value outside range. Fixed."));
            UpdateData(FALSE);
        }
        if (fMaskMax > m_pColumnDataX->dat[m_iStop]) {
            fMaskMax = m_pColumnDataX->dat[m_iStop];
            m_edtMaskMin.SetValue(fMaskMax);
            m_stOutput.SetWindowText(_T("Mask value outside range. Fixed."));
            UpdateData(FALSE);
        }
        if (fMaskMax <= fMaskMin) {
            fMaskMin = m_pColumnDataX->dat[m_iStart];
            fMaskMax = m_pColumnDataX->dat[m_iStop];
            m_edtMaskMin.SetValue(fMaskMin);
            m_edtMaskMin.SetValue(fMaskMax);
            m_stOutput.SetWindowText(_T("Invalid mask values. Fixed."));
            UpdateData(FALSE);
        }
        if ((fMaskMax - fMaskMin) < (0.01 * (m_pColumnDataX->dat[m_iStop] - m_pColumnDataX->dat[m_iStart]))) {
            fMaskMin = m_pColumnDataX->dat[m_iStart];
            fMaskMax = m_pColumnDataX->dat[m_iStop];
            m_edtMaskMin.SetValue(fMaskMin);
            m_edtMaskMin.SetValue(fMaskMax);
            m_stOutput.SetWindowText(_T("Invalid mask values. Fixed."));
            UpdateData(FALSE);
        }
    }

    real_t fBaseline0 = 0.0,
        fBaseline1 = 0.0;
    if (m_bBaseline == TRUE) {
        fBaseline0 = m_edtBaseline0.GetValue(),
            fBaseline1 = m_edtBaseline1.GetValue();
        if ((fBaseline0 == 0.0) && (fBaseline1 == 0.0)) {
            m_bBaseline = FALSE;
            UpdateData(FALSE);
        }
    }

    if ((m_bMask == FALSE) && (m_bSmooth == FALSE) && (m_bBaseline == FALSE)) {
        *m_pbRunning = FALSE;
        return;
    }

    SigmaApp.m_pLib->errcode = 0;
    int_t iRet = ml_fit_process(m_pColumnDataX, m_pColumnDataY,
        (m_bMask == TRUE) ? 1 : 0, fMaskMin, fMaskMax,
        (m_bBaseline == TRUE) ? 1 : 0, fBaseline0, fBaseline1,
        (m_bSmooth == TRUE) ? 1 : 0, m_nSmooth, SigmaApp.m_pLib);

    m_stOutput.SetWindowText(SigmaApp.m_pLib->message);

    if (m_bBaseline == TRUE) {
        ((CSigmaDoc*)m_pDoc)->SetAutoScale(TRUE);
    }

    m_pDoc->SetModifiedFlag(TRUE);
    m_pDoc->UpdateAllViews(NULL);

    *m_pbIsModified = TRUE;
    m_bnUndo.EnableWindow(*m_pbIsModified);

    m_bMask = FALSE;
    m_bSmooth = FALSE;
    m_bBaseline = FALSE;
    UpdateData(FALSE);

    *m_pbRunning = FALSE;
}

void CFitterMask::OnUndo()
{
    if (*m_pbRunning == TRUE) {
        return;
    }

    *m_pbRunning = TRUE;

    m_stOutput.SetWindowText(_T(""));

    if ((m_bCanUndo == TRUE) && (*m_pbIsModified == TRUE)) {
        ml_vector_dup(m_pColumnDataX, (const vector_t*)(&m_ColumnUndoX), SigmaApp.m_pLib);
        ml_vector_dup(m_pColumnDataY, (const vector_t*)(&m_ColumnUndoY), SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            ((CSigmaDoc*)m_pDoc)->SetAutoScale(TRUE);
            m_pDoc->UpdateAllViews(NULL);
            m_stOutput.SetWindowText(_T("Done."));
            *m_pbIsModified = FALSE;
            m_bnUndo.EnableWindow(*m_pbIsModified);
        }
    }

    *m_pbRunning = FALSE;
}

void CFitterMask::OnOK()
{
    if (*m_pbRunning == TRUE) {
        if (this->MessageBox(_T("Stop calculations and end the fitting session?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
            return;
        }
    }
    UpdateData(TRUE);

    CPropertyPage::OnOK();
}

void CFitterMask::OnCancel()
{
    if (*m_pbRunning == TRUE) {
        if (this->MessageBox(_T("Stop calculations and end the fitting session?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
            return;
        }
    }

    CPropertyPage::OnCancel();
}

void CFitterMask::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/fitting.html#fitting_mask"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CFitterMask::PreTranslateMessage(MSG* pMsg)
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

void CFitterMask::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}
