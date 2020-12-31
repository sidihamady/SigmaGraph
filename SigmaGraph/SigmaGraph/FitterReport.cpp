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
#include "FitterReport.h"
#include "MainFrm.h"
#include "SigmaDoc.h"

// CFitterReport dialog

IMPLEMENT_DYNAMIC(CFitterReport, CDialog)

CFitterReport::CFitterReport()
    : CDialog(CFitterReport::IDD)
{
    m_bInit = FALSE;
    m_pFit = NULL;
    m_pPlot = NULL;
    m_pDoc = NULL;
    m_nCurrentCurve = 0;
}

CFitterReport::~CFitterReport()
{

}

void CFitterReport::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDF_FITTER_MODEL, m_bmpFunction);
    DDX_Control(pDX, IDE_CURVE, m_edtCurve);
    DDX_Control(pDX, IDE_FITREPORT, m_edtFitReport);
}

BEGIN_MESSAGE_MAP(CFitterReport, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDS_FITREPORT_CURVE, &CFitterReport::OnDeltaposCurve)
    ON_WM_PAINT()
END_MESSAGE_MAP()


void CFitterReport::VariableToControl()
{
    m_edtCurve.SetWindowText((LPCTSTR)m_pPlot->curve[m_nCurrentCurve].legend.text);

    CString strReport = CSigmaApp::getFitReport(m_pFit);

    m_edtFitReport.SetWindowText((LPCTSTR)strReport);
    m_edtFitReport.SetSel(-1, 0, FALSE);

    if (m_bInit == TRUE) {
        UINT uiBmp[ML_FIT_MAXMODEL] = {
            IDB_FITTER_MODEL01, IDB_FITTER_MODEL02, IDB_FITTER_MODEL03,
            IDB_FITTER_MODEL04, IDB_FITTER_MODEL05, IDB_FITTER_MODEL06,
            IDB_FITTER_MODEL07, IDB_FITTER_MODEL08, IDB_FITTER_MODEL09,
            IDB_FITTER_MODEL10, IDB_FITTER_MODEL11, IDB_FITTER_MODEL12,
            IDB_FITTER_MODEL13, IDB_FITTER_MODEL14, IDB_FITTER_MODEL15,
            IDB_FITTER_MODEL16, IDB_FITTER_MODEL17, IDB_FITTER_MODEL18,
            IDB_FITTER_MODEL19, IDB_FITTER_MODEL20, IDB_FITTER_MODEL21,
            IDB_FITTER_MODEL22, IDB_FITTER_MODEL23, IDB_FITTER_MODEL24
        };
        m_Bitmap.DeleteObject();
        if ((m_pFit->index >= 0) && (m_pFit->index < ML_FIT_MAXMODEL)) {
            if (m_Bitmap.LoadBitmap(uiBmp[m_pFit->index - 1])) {
                m_bmpFunction.SetBitmap(m_Bitmap);
            }
        }
        else {
            if (m_pFit->index == ML_FIT_USERINDEX) {
                if (m_Bitmap.LoadBitmap(IDB_FITTER_MODEL255)) {
                    m_bmpFunction.SetBitmap(m_Bitmap);
                }
            }
            else {
                if (m_Bitmap.LoadBitmap(IDB_FITTER_MODEL99)) {
                    m_bmpFunction.SetBitmap(m_Bitmap);
                }

            }
        }
    }

    UpdateData(FALSE);
}

// CFitterReport message handlers
BOOL CFitterReport::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ((m_pFit == NULL) || (m_pPlot == NULL) || (m_pDoc == NULL)) {
        SigmaApp.Output(_T("Cannot show the 'Fitting Report' dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    m_edtCurve.LimitText(ML_STRSIZE);
    m_edtFitReport.LimitText(ML_STRSIZEW);
    m_nCurrentCurve = m_pPlot->curveactive;

    int_t nColumnCount = ((CSigmaDoc*)m_pDoc)->GetColumnCount(1);
    if (nColumnCount <= 2) {
        CWnd *pWnd = GetDlgItem(IDS_FITREPORT_CURVE);
        if (pWnd != NULL) {
            pWnd->EnableWindow(FALSE);
        }
    }

    VariableToControl();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CFitterReport::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // Do not call CDialog::OnPaint() for painting messages

    if (IsWindowVisible()) {
        if (m_bInit == FALSE) {
            UINT uiBmp[ML_FIT_MAXMODEL] = {
                IDB_FITTER_MODEL01, IDB_FITTER_MODEL02, IDB_FITTER_MODEL03,
                IDB_FITTER_MODEL04, IDB_FITTER_MODEL05, IDB_FITTER_MODEL06,
                IDB_FITTER_MODEL07, IDB_FITTER_MODEL08, IDB_FITTER_MODEL09,
                IDB_FITTER_MODEL10, IDB_FITTER_MODEL11, IDB_FITTER_MODEL12,
                IDB_FITTER_MODEL13, IDB_FITTER_MODEL14, IDB_FITTER_MODEL15,
                IDB_FITTER_MODEL16, IDB_FITTER_MODEL17, IDB_FITTER_MODEL18,
                IDB_FITTER_MODEL19, IDB_FITTER_MODEL20, IDB_FITTER_MODEL21,
                IDB_FITTER_MODEL22, IDB_FITTER_MODEL23, IDB_FITTER_MODEL24
            };
            m_Bitmap.DeleteObject();

            if ((m_pFit->index >= 0) && (m_pFit->index < ML_FIT_MAXMODEL)) {
                if (m_Bitmap.LoadBitmap(uiBmp[m_pFit->index - 1])) {
                    m_bmpFunction.SetBitmap(m_Bitmap);
                }
            }
            else {
                if (m_pFit->index == ML_FIT_USERINDEX) {
                    if (m_Bitmap.LoadBitmap(IDB_FITTER_MODEL255)) {
                        m_bmpFunction.SetBitmap(m_Bitmap);
                    }
                }
                else {
                    if (m_Bitmap.LoadBitmap(IDB_FITTER_MODEL99)) {
                        m_bmpFunction.SetBitmap(m_Bitmap);
                    }

                }
            }

            m_bInit = TRUE;
        }
    }
}

void CFitterReport::OnDeltaposCurve(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    
    UpdateData(TRUE);

    int_t iCurve = m_nCurrentCurve;
    if (pNMUpDown->iDelta > 0) {
        iCurve -= 1;
        if (iCurve < 0) {
            iCurve = 0;
            pNMUpDown->iPos = 0;
        }
    }
    else {
        iCurve += 1;
        if (iCurve >= m_pPlot->curvecount) {
            iCurve = m_pPlot->curvecount - 1;
            pNMUpDown->iPos = iCurve;
        }
    }

    fit_t *pFit = ((CSigmaDoc*)m_pDoc)->GetFit(-1, m_pPlot->curve[iCurve].y->idf);
    if (pFit != NULL) {
        m_pFit = pFit;
        m_nCurrentCurve = iCurve;
        VariableToControl();
    }

    *pResult = 0;
}
