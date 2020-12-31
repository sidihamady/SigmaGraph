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
#include "AnalysisDlg.h"
#include "SigmaDoc.h"
#include "SigmaViewData.h"

static int_t isPowerOfTwo(int_t idim)
{
    if (idim < 2) {
        return 0;
    }

    return !(idim & (idim - 1));
}

// CAnalysisDlg dialog

IMPLEMENT_DYNAMIC(CAnalysisDlg, CDialog)

CAnalysisDlg::CAnalysisDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CAnalysisDlg::IDD, pParent)
    , m_strOutput(_T(""))
{
    m_pDoc = NULL;
    m_pPlot = NULL;
    m_pColumn = NULL;
    m_nColumnCount = 0;
    m_nSelectedColumnIn = -1;
    m_nSelectedColumnOut = -1;
    m_bIsModified = FALSE;
    m_bAddCurve = FALSE;
    m_bCurveAdded = FALSE;
    m_bNewColumn = FALSE;
    m_bNewColumnDone = FALSE;
    m_iOperation = 0;
    m_iAverage = 1;
    m_bInitialized = FALSE;
    m_bShift = TRUE;
}

CAnalysisDlg::~CAnalysisDlg()
{

}

void CAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDL_ANALYSIS_INPUT, m_lstColumnIn);
    DDX_Control(pDX, IDL_ANALYSIS_OUTPUT, m_lstColumnOut);
    DDX_Check(pDX, IDK_ANALYSIS_NEWCOLUMN, m_bNewColumn);
    DDX_Check(pDX, IDK_ANALYSIS_ADDCURVE, m_bAddCurve);
    DDX_Check(pDX, IDK_ANALYSIS_FFTSHIFT, m_bShift);
    DDX_Control(pDX, IDC_ANALYSIS_OPERATION, m_cbOperation);
    DDX_CBIndex(pDX, IDC_ANALYSIS_OPERATION, m_iOperation);
    DDX_Text(pDX, IDE_ANALYSIS_OUTPUT, m_strOutput);
    DDX_Text(pDX, IDL_ANALYSIS_AVG, m_iAverage);
    DDV_MinMaxInt(pDX, m_iAverage, 1, 16);
}

BEGIN_MESSAGE_MAP(CAnalysisDlg, CDialog)
    ON_LBN_SELCHANGE(IDL_ANALYSIS_OUTPUT, &CAnalysisDlg::OnLbnSelchangeMathOutput)
    ON_LBN_SELCHANGE(IDL_ANALYSIS_INPUT, &CAnalysisDlg::OnLbnSelchangeMathInput)
    ON_BN_CLICKED(ID_ANALYSIS_CALC, &CAnalysisDlg::OnCalc)
    ON_BN_CLICKED(IDOK, &CAnalysisDlg::OnOK)
    ON_CBN_SELCHANGE(IDC_ANALYSIS_OPERATION, &CAnalysisDlg::OnCbnSelchangeMathOperation)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAnalysisDlg message handlers

BOOL CAnalysisDlg::OnInitDialog()
{
    int_t jj, idc;
    char_t szBuffer[ML_STRSIZE];

    CDialog::OnInitDialog();

    if ((m_pColumn == NULL) || (m_nColumnCount < 1) || (m_nColumnCount > SIGMA_MAXCOLUMNS)) {
        SigmaApp.Output(_T("Cannot show the Mathematics dialog: invalid column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }
    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        SigmaApp.Output(_T("Cannot show the Mathematics dialog: invalid document.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }
    if ((m_pColumn[0].dim < 5) || (m_pColumn[0].dim > ML_MAXPOINTS)) {
        SigmaApp.Output(_T("Cannot show the Mathematics dialog: invalid column dimension.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
    int_t nT = 0, nSelectedColumnIn = -1, nSelectedColumnOut = -1;
    for (jj = 0; jj < m_nColumnCount; jj++) {
        if ((m_pColumn[jj].type & 0xF0) == 0x20) {    // Y
            _tcsprintf(szBuffer, ML_STRSIZE - 1, _T("[%s] %s"), m_pColumn[jj].name, m_pColumn[jj].label);
            idc = m_lstColumnIn.AddString((LPCTSTR)szBuffer);
            m_lstColumnIn.SetItemData(idc, jj + 1);
            if (jj == m_nSelectedColumnIn) {
                nSelectedColumnIn = nT;
            }
            idc = m_lstColumnOut.AddString((LPCTSTR)szBuffer);
            m_lstColumnOut.SetItemData(idc, jj + 1);
            if (jj == m_nSelectedColumnOut) {
                nSelectedColumnOut = nT;
            }
            nT += 1;
        }
    }
    if (nT < 1) {
        SigmaApp.Output(_T("Cannot show the Mathematics dialog: Y-column not found.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if (nSelectedColumnOut == nSelectedColumnIn) {
        nSelectedColumnOut = -1;
    }

    m_lstColumnIn.SetCurSel(nSelectedColumnIn);
    m_lstColumnOut.SetCurSel(nSelectedColumnOut);

    CComboBox *pComboOperation = NULL;
    CRect rcCombo;

    pComboOperation = (CComboBox*)GetDlgItem(IDC_ANALYSIS_OPERATION);
    if (pComboOperation == NULL) {
        return FALSE;
    }

    pComboOperation->GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    pComboOperation->MoveWindow(rcCombo);

    pComboOperation->AddString(_T("Integrate"));
    pComboOperation->AddString(_T("Differentiate"));
    pComboOperation->AddString(_T("Average"));
    pComboOperation->AddString(_T("Fast Fourier Transform"));
    pComboOperation->AddString(_T("Autocorrelation"));
    pComboOperation->SetCurSel(m_iOperation);

    CWnd *pWnd = this->GetDlgItem(IDK_ANALYSIS_FFTSHIFT);
    if (pWnd) {
        pWnd->EnableWindow(m_iOperation == 3);
    }
    pWnd = this->GetDlgItem(IDS_ANALYSIS_AVG);
    if (pWnd) {
        pWnd->EnableWindow(m_iOperation == 2);
    }

    ::SendDlgItemMessage(GetSafeHwnd(), IDS_ANALYSIS_AVG, UDM_SETRANGE, 0, (LPARAM)MAKELONG(28,1));

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnalysisDlg::OnCalc()
{
    CWaitCursor wcT;

    UpdateData(TRUE);

    int_t nSelectedColumnOut = -1, nSelectedColumnOutX = -1, nSelectedColumnIn = -1, ii, jj;

    nSelectedColumnIn = m_lstColumnIn.GetCurSel();
    nSelectedColumnIn = (int_t) (m_lstColumnIn.GetItemData(nSelectedColumnIn));
    if ((nSelectedColumnIn < 1) || (nSelectedColumnIn > m_nColumnCount)) {
        m_strOutput = _T("! Selected column not valid.");
        UpdateData(FALSE);
        return;
    }
    nSelectedColumnIn -= 1;

    CString strWarn = _T("");
    vector_t *pColumnX = ((CSigmaDoc*)m_pDoc)->GetColumn(0, -1, m_pColumn[nSelectedColumnIn].idx);
    if (pColumnX == NULL) {
        pColumnX = ((CSigmaDoc*)m_pDoc)->GetColumnAutoX();
        if (pColumnX) {
            if (m_pColumn[nSelectedColumnIn].idx != pColumnX->id) {
                pColumnX = NULL;
            }
        }
        if (pColumnX == NULL) {
            pColumnX = ((CSigmaDoc*)m_pDoc)->GetColumnAutoX();
            strWarn = _T("No X-column linked to the selected column. Index column selected.");
            m_strOutput = strWarn;
            UpdateData(FALSE);
        }
    }
    if (pColumnX == NULL) {
        m_strOutput = _T("! No X-column linked to the selected column.");
        UpdateData(FALSE);
        return;
    }

    if (m_iOperation == 2) {
        if ((m_iAverage < 1) || (m_iAverage > 16) || (m_iAverage > (m_pColumn[nSelectedColumnIn].dim / 8))) {
            SigmaApp.Output((m_iAverage > (m_pColumn[nSelectedColumnIn].dim / 8)) ? _T("Average order not valid.\r\n") : _T("Average order not valid (too high compared to the number of points).\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_strOutput = _T("! Average order not valid.");
            UpdateData(FALSE);
            return;
        }
    }
    else if (m_iOperation == 3) {    // FFT
        if (!isPowerOfTwo(m_pColumn[nSelectedColumnIn].dim)) {
            SigmaApp.Output(_T("For FFT calculation, the number of points should be a power of 2.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_strOutput = _T("! Number of points not valid.");
            UpdateData(FALSE);
            return;
        }
    }

    CSigmaViewData* pView = (CSigmaViewData*)(((CSigmaDoc*)m_pDoc)->GetDataView());
    if (pView == NULL) {
        SigmaApp.Output(_T("Cannot modify column: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        m_strOutput = _T("! Invalid data.");
        m_bNewColumn = FALSE;
        UpdateData(FALSE);
        return;
    }

    CDatasheet *pDatasheet = (CDatasheet*)(pView->GetDatasheet());
    if (pDatasheet == NULL) {
        SigmaApp.Output(_T("Cannot modify column: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        m_strOutput = _T("! Invalid data.");
        m_bNewColumn = FALSE;
        UpdateData(FALSE);
        return;
    }

    char_t szBuffer[ML_STRSIZE];
    memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));

    if (m_bNewColumn == FALSE) {
        nSelectedColumnOut = m_lstColumnOut.GetCurSel();
        nSelectedColumnOut = (int_t) (m_lstColumnOut.GetItemData(nSelectedColumnOut));
        if ((nSelectedColumnOut < 1) || (nSelectedColumnOut > m_nColumnCount)) {
            m_strOutput = _T("! Output column not valid. Check \'New\' or select existing column.");
            UpdateData(FALSE);
            return;
        }
        nSelectedColumnOut -= 1;
        if (m_iOperation == 3) {    // FFT
            nSelectedColumnOutX = ((CSigmaDoc*)m_pDoc)->GetColumnIndex(0, m_pColumn[nSelectedColumnOut].idx);
            if ((nSelectedColumnOutX < 0) || (nSelectedColumnOutX >= m_nColumnCount)) {
                if (m_nColumnCount >= SIGMA_MAXCOLUMNS) {
                    SigmaApp.Output(_T("Cannot create new column: maximum column count reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                    m_strOutput = _T("! Maximum column count reached.");
                    m_bNewColumn = FALSE;
                    UpdateData(FALSE);
                    return;
                }
                vector_t *pColumn = NULL;
                long_t iOutID = m_pColumn[nSelectedColumnOut].id;
                if (nSelectedColumnOut > 0) {
                    pColumn = pDatasheet->InsertColumn(nSelectedColumnOut - 1);
                }
                if (pColumn == NULL) {
                    SigmaApp.Output(_T("Cannot create new column: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                    m_strOutput = _T("! Invalid data.");
                    m_bNewColumn = FALSE;
                    UpdateData(FALSE);
                    return;
                }

                nSelectedColumnOutX = ((CSigmaDoc*)m_pDoc)->GetColumnIndex(0, pColumn->id);
                nSelectedColumnOut = ((CSigmaDoc*)m_pDoc)->GetColumnIndex(0, iOutID);
                m_nColumnCount += 1;

                pColumn->type = 0x10;    // X
                _tcscpy(pColumn->label, (const char_t*)(m_pColumn[nSelectedColumnIn].label));
                ii = (int_t)_tcslen((const char_t*)(pColumn->label));
                if ((ii > 0) && (ii < 22)) {
                    _tcscat(pColumn->label, _T(" - FFTx"));
                }
                m_pColumn[nSelectedColumnOut].idx = pColumn->id;
            }
            if ((nSelectedColumnOutX < 0) || (nSelectedColumnOutX >= m_nColumnCount)) {
                m_strOutput = _T("! Selected column not valid.");
                UpdateData(FALSE);
                return;
            }
        }
    }
    else {
        if (m_nColumnCount >= SIGMA_MAXCOLUMNS) {
            SigmaApp.Output(_T("Cannot create new column: maximum column count reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_strOutput = _T("! Maximum column count reached.");
            m_bNewColumn = FALSE;
            UpdateData(FALSE);
            return;
        }

        vector_t *pColumn = pDatasheet->AppendColumn();
        if (pColumn == NULL) {
            SigmaApp.Output(_T("Cannot create new column: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            m_strOutput = _T("! Invalid data.");
            m_bNewColumn = FALSE;
            UpdateData(FALSE);
            return;
        }

        nSelectedColumnOut = m_nColumnCount;
        m_nColumnCount += 1;

        pColumn->type = 0x20;    // Y
        _tcscpy(pColumn->label, (const char_t*)(m_pColumn[nSelectedColumnIn].label));
        ii = (int_t)_tcslen((const char_t*)(pColumn->label));
        if ((ii > 0) && (ii < 22)) {
            switch (m_iOperation) {
                case 0:
                    _tcscat(pColumn->label, _T(" - Int."));
                    break;
                case 1:
                    _tcscat(pColumn->label, _T(" - Diff."));
                    break;
                case 2:
                    _tcscat(pColumn->label, _T(" - Avg."));
                    break;
                case 3:
                    _tcscat(pColumn->label, _T(" - FFT"));
                    break;
                case 4:
                    _tcscat(pColumn->label, _T(" - Corr"));
                    break;
                default:
                    break;
            }
        }
        _tcsprintf(szBuffer, ML_STRSIZE - 1, _T("[%s] %s"), pColumn->name, pColumn->label);
        ii = m_lstColumnOut.AddString((LPCTSTR)szBuffer);
        m_lstColumnOut.SetItemData(ii, nSelectedColumnOut + 1);
        m_lstColumnOut.SetCurSel(ii);

        if (m_iOperation == 3) {    // FFT
            nSelectedColumnOutX = ((CSigmaDoc*)m_pDoc)->GetColumnIndex(0, m_pColumn[nSelectedColumnOut].idx);
            if ((nSelectedColumnOutX < 0) || (nSelectedColumnOutX >= m_nColumnCount)) {
                if (m_nColumnCount >= SIGMA_MAXCOLUMNS) {
                    SigmaApp.Output(_T("Cannot create new column: maximum column count reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                    m_strOutput = _T("! Maximum column count reached.");
                    m_bNewColumn = FALSE;
                    UpdateData(FALSE);
                    return;
                }
                vector_t *pColumnT = NULL;
                long_t iOutID = m_pColumn[nSelectedColumnOut].id;
                if (nSelectedColumnOut > 0) {
                    pColumnT = pDatasheet->InsertColumn(nSelectedColumnOut - 1);
                }
                if (pColumnT == NULL) {
                    SigmaApp.Output(_T("Cannot create new column: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                    m_strOutput = _T("! Invalid data.");
                    m_bNewColumn = FALSE;
                    UpdateData(FALSE);
                    return;
                }

                nSelectedColumnOutX = ((CSigmaDoc*)m_pDoc)->GetColumnIndex(0, pColumnT->id);
                nSelectedColumnOut = ((CSigmaDoc*)m_pDoc)->GetColumnIndex(0, iOutID);
                m_nColumnCount += 1;

                pColumnT->type = 0x10;    // X
                _tcscpy(pColumnT->label, (const char_t*)(m_pColumn[nSelectedColumnIn].label));
                ii = (int_t)_tcslen((const char_t*)(pColumnT->label));
                if ((ii > 0) && (ii < 22)) {
                    _tcscat(pColumnT->label, _T(" - FFTx"));
                }
                m_pColumn[nSelectedColumnOut].idx = pColumnT->id;
            }
            if ((nSelectedColumnOutX < 0) || (nSelectedColumnOutX >= m_nColumnCount)) {
                m_strOutput = _T("! Selected column not valid.");
                UpdateData(FALSE);
                return;
            }
        }

        m_bNewColumn = FALSE;
        CWnd *pWnd = GetDlgItem(IDK_ANALYSIS_NEWCOLUMN);
        if (pWnd) {
            pWnd->EnableWindow(FALSE);
        }
    }

    if ((nSelectedColumnIn == nSelectedColumnOut)) {
        SigmaApp.Output(_T("The input and output columns are the same.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        m_strOutput = _T("! The input and output columns are the same.");
        UpdateData(FALSE);
        return;
    }

    int_t nDim = m_pColumn[nSelectedColumnOut].dim;

    real_t Fs = 1.0;    // FFT - Sampling Frequency
    real_t Ts = 1.0;    // FFT - Sampling Period
    real_t fArea = 0.0, dx;
    m_pColumn[nSelectedColumnOut].mask[0] = 0x00;
    switch (m_iOperation) {
        case 0:
            m_pColumn[nSelectedColumnOut].dat[0] = 0.0;
            break;
        case 1:
            dx = pColumnX->dat[1] - pColumnX->dat[0];
            if (dx != 0.0) {
                m_pColumn[nSelectedColumnOut].dat[0] = (m_pColumn[nSelectedColumnIn].dat[1] - m_pColumn[nSelectedColumnIn].dat[0]) / dx;
            }
            break;
        case 2:
            m_pColumn[nSelectedColumnOut].dat[0] = m_pColumn[nSelectedColumnIn].dat[0];
            break;
        case 3:
            Ts = pColumnX->dat[1] - pColumnX->dat[0];
            if (Ts > 0.0) {
                Fs = 1.0 / Ts;
            }
            break;
        default:
            break;
    }
    if (m_iOperation == 3) {    // FFT
        m_pColumn[nSelectedColumnOutX].status = 0x00;
        for (ii = 0; ii < nDim; ii++) {
            m_pColumn[nSelectedColumnOutX].dat[ii] = Fs * ((real_t)ii) / ((real_t)nDim);
            if (m_bShift) {
                m_pColumn[nSelectedColumnOutX].dat[ii] -= (0.5 * Fs);
            }
            m_pColumn[nSelectedColumnOutX].mask[ii] = 0x00;
            m_pColumn[nSelectedColumnOut].dat[ii] = m_pColumn[nSelectedColumnIn].dat[ii];
            m_pColumn[nSelectedColumnOut].mask[ii] = 0x00;
        }
        ml_fft(m_pColumn[nSelectedColumnOut].dat, nDim, 1);

        if (m_bShift) {
            int_t nHalf = nDim >> 1;
            real_t fTmp = 0.0;
            for (ii = 0; ii < nHalf; ii++) {
                fTmp = m_pColumn[nSelectedColumnOut].dat[ii];
                m_pColumn[nSelectedColumnOut].dat[ii] = m_pColumn[nSelectedColumnOut].dat[ii + nHalf];
                m_pColumn[nSelectedColumnOut].dat[ii + nHalf] = fTmp;
            }
        }
    }
    else if (m_iOperation == 4) {    // Autocorrelation
        for (ii = 0; ii < nDim; ii++) {
            m_pColumn[nSelectedColumnOut].dat[ii] = m_pColumn[nSelectedColumnIn].dat[ii];
            m_pColumn[nSelectedColumnOut].mask[ii] = 0x00;
        }
        ml_autocorr(m_pColumn[nSelectedColumnIn].dat, m_pColumn[nSelectedColumnOut].dat, nDim);
    }
    else {
        for (ii = 1; ii < nDim - 1; ii++) {
            switch (m_iOperation) {
                case 0:
                    dx = pColumnX->dat[ii] - pColumnX->dat[ii - 1];
                    fArea = (0.5 * dx * (m_pColumn[nSelectedColumnIn].dat[ii] + m_pColumn[nSelectedColumnIn].dat[ii - 1]));
                    m_pColumn[nSelectedColumnOut].dat[ii] = m_pColumn[nSelectedColumnOut].dat[ii - 1] + fArea;
                    m_pColumn[nSelectedColumnOut].mask[ii] = 0x00;
                    break;
                case 1:
                    dx = pColumnX->dat[ii + 1] - pColumnX->dat[ii - 1];
                    if (dx != 0.0) {
                        m_pColumn[nSelectedColumnOut].dat[ii] = (m_pColumn[nSelectedColumnIn].dat[ii + 1] - m_pColumn[nSelectedColumnIn].dat[ii - 1]) / dx;
                    }
                    m_pColumn[nSelectedColumnOut].mask[ii] = 0x00;
                    break;
                case 2:
                    if ((ii < m_iAverage) || (ii > (nDim - 1 - m_iAverage))) {
                        m_pColumn[nSelectedColumnOut].dat[ii] = m_pColumn[nSelectedColumnIn].dat[ii];
                    }
                    else {
                        m_pColumn[nSelectedColumnOut].dat[ii] = 0.0;
                        for (jj = 1; jj <= m_iAverage; jj++) {
                            m_pColumn[nSelectedColumnOut].dat[ii] += (m_pColumn[nSelectedColumnIn].dat[ii - jj] + m_pColumn[nSelectedColumnIn].dat[ii + jj]);
                        }
                        m_pColumn[nSelectedColumnOut].dat[ii] /= (real_t)(2 * m_iAverage);
                    }
                    m_pColumn[nSelectedColumnOut].mask[ii] = 0x00;
                    break;
                default:
                    break;
            }
        }
    }
    switch (m_iOperation) {
        case 0:
            dx = pColumnX->dat[nDim - 1] - pColumnX->dat[nDim - 2];
            fArea = (0.5 * dx * (m_pColumn[nSelectedColumnIn].dat[nDim - 1] + m_pColumn[nSelectedColumnIn].dat[nDim - 2]));
            m_pColumn[nSelectedColumnOut].dat[nDim - 1] = m_pColumn[nSelectedColumnOut].dat[nDim - 2] + fArea;
            break;
        case 1:
            dx = pColumnX->dat[nDim - 1] - pColumnX->dat[nDim - 2];
            if (dx != 0.0) {
                m_pColumn[nSelectedColumnOut].dat[nDim - 1] = (m_pColumn[nSelectedColumnIn].dat[nDim - 1] - m_pColumn[nSelectedColumnIn].dat[nDim - 2]) / dx;
            }
            break;
        case 2:
            m_pColumn[nSelectedColumnOut].dat[nDim - 1] = m_pColumn[nSelectedColumnIn].dat[nDim - 1];
            break;
        default:
            break;
    }
    m_pColumn[nSelectedColumnOut].mask[nDim - 1] = 0x00;

    vector_t *pColumnAutoX = ((CSigmaDoc*)m_pDoc)->GetColumnAutoX();

    if (m_iOperation == 3) {    // FFT
        m_pColumn[nSelectedColumnOut].idx = m_pColumn[nSelectedColumnOutX].id;
    }
    else if (m_iOperation == 4) {        // Autocorrelation
        if (pColumnAutoX) {
            m_pColumn[nSelectedColumnOut].idx = pColumnAutoX->id;
        }
    }
    else {
        m_pColumn[nSelectedColumnOut].idx = pColumnX->id;
    }

    byte_t iplot = 0;

    if (m_bAddCurve) {
        byte_t axisused = 0x00;
        byte_t topaxis = 0x00;
        int_t icurvecount = m_pPlot->curvecount;
        int_t icurve = -1;
        if (m_pPlot->curvecount > 0) {
            for (ii = 0; ii < m_pPlot->curvecount; ii++) {
                if (m_pPlot->curve[ii].y->id == m_pColumn[nSelectedColumnIn].id) {
                    axisused = m_pPlot->curve[ii].axisused;
                }
                if (m_pPlot->curve[ii].axisused != 0x00) {
                    if (m_pPlot->curve[ii].y->id != m_pColumn[nSelectedColumnOut].id) {
                        topaxis += 1;
                    }
                }
            }
        }
        m_pPlot->errcode = 0;
        if (m_iOperation == 3) {    // FFT
            if (topaxis == 0x00) {
                icurve = pl_curve_add(m_pPlot, &(m_pColumn[nSelectedColumnOutX]), &(m_pColumn[nSelectedColumnOut]), NULL, NULL, 0x11, 0x01);
                if (m_pPlot->errcode != 0) {
                    m_strOutput = _T("! Cannot add curve.");
                    UpdateData(FALSE);
                    return;
                }
                m_pPlot->axistop.olabel = 1;
                m_pPlot->axistop.otitle = 1;
                m_pPlot->axisright.olabel = 1;
                m_pPlot->axisright.otitle = 1;
                _tcscpy(m_pPlot->axistop.title.text, _T("Frequency (Hz)"));
                _tcscpy(m_pPlot->axisright.title.text, _T("FFT"));
                m_pPlot->curve[icurve].linecolor.r = 80;
                m_pPlot->curve[icurve].linecolor.g = 140;
                m_pPlot->curve[icurve].linecolor.b = 200;
                m_pPlot->curve[icurve].dotcolor.r = 80;
                m_pPlot->curve[icurve].dotcolor.g = 140;
                m_pPlot->curve[icurve].dotcolor.b = 200;
                if (icurve >= icurvecount) {    // New curve
                    pl_autoscale(m_pPlot);
                }
            }
        }
        else if (m_iOperation == 4) {        // Autocorrelation
            if (topaxis == 0x00) {
                icurve = pl_curve_add(m_pPlot, pColumnAutoX, &(m_pColumn[nSelectedColumnOut]), NULL, NULL, 0x11, 0x01);
                if (m_pPlot->errcode != 0) {
                    m_strOutput = _T("! Cannot add curve.");
                    UpdateData(FALSE);
                    return;
                }
                m_pPlot->axistop.olabel = 1;
                m_pPlot->axistop.otitle = 1;
                m_pPlot->axisright.olabel = 1;
                m_pPlot->axisright.otitle = 1;
                _tcscpy(m_pPlot->axistop.title.text, _T("Lag"));
                _tcscpy(m_pPlot->axisright.title.text, _T("Autocorrelation"));
                m_pPlot->curve[icurve].linecolor.r = 80;
                m_pPlot->curve[icurve].linecolor.g = 140;
                m_pPlot->curve[icurve].linecolor.b = 200;
                m_pPlot->curve[icurve].dotcolor.r = 80;
                m_pPlot->curve[icurve].dotcolor.g = 140;
                m_pPlot->curve[icurve].dotcolor.b = 200;
                if (icurve >= icurvecount) {    // New curve
                    pl_autoscale(m_pPlot);
                }
            }
        }
        else {
            pl_curve_add(m_pPlot, pColumnX, &(m_pColumn[nSelectedColumnOut]), NULL, NULL, axisused, 0x01);
            if (m_pPlot->errcode != 0) {
                m_strOutput = _T("! Cannot add curve.");
                UpdateData(FALSE);
                return;
            }
            if (icurve >= icurvecount) {    // New curve
                pl_autoscale(m_pPlot);
            }
        }

        m_bAddCurve = FALSE;
        m_bCurveAdded = TRUE;

        m_pColumn[nSelectedColumnOut].status = 0x00;
    }
    else {
        if (m_pPlot->curvecount > 0) {
            for (ii = 0; ii < m_pPlot->curvecount; ii++) {
                if (m_pPlot->curve[ii].y->id == m_pColumn[nSelectedColumnOut].id) {
                    m_bCurveAdded = TRUE;
                    break;
                }
            }
        }
    }
    if (m_bCurveAdded) {
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }

    if (strWarn.IsEmpty()) {
        m_strOutput = _T("Done.");
    }
    else {
        m_strOutput = _T("Done [") + strWarn + _T("]");
    }

    UpdateData(FALSE);

    if (m_bInitialized == FALSE) {
        ((CSigmaDoc*) m_pDoc)->SetModifiedFlag(TRUE);
        m_bInitialized = TRUE;
    }

    ((CSigmaDoc*) m_pDoc)->SaveState();
}

void CAnalysisDlg::OnOK()
{
    CDialog::OnOK();
}

void CAnalysisDlg::OnClose()
{

    CDialog::OnClose();
}

void CAnalysisDlg::OnLbnSelchangeMathOutput()
{
    int_t nSelectedColumnOut = -1, nSelectedColumnIn = -1;

    nSelectedColumnIn = m_lstColumnIn.GetCurSel();
    nSelectedColumnIn = (int_t) (m_lstColumnIn.GetItemData(nSelectedColumnIn));
    if ((nSelectedColumnIn < 1) || (nSelectedColumnIn > m_nColumnCount)) {
        return;
    }
    nSelectedColumnIn -= 1;

    nSelectedColumnOut = m_lstColumnOut.GetCurSel();
    nSelectedColumnOut = (int_t) (m_lstColumnOut.GetItemData(nSelectedColumnOut));
    if ((nSelectedColumnOut < 1) || (nSelectedColumnOut > m_nColumnCount)) {
        return;
    }
    nSelectedColumnOut -= 1;

    if (nSelectedColumnIn == nSelectedColumnOut) {
         m_lstColumnOut.SetCurSel(-1);
    }
}

void CAnalysisDlg::OnLbnSelchangeMathInput()
{
    int_t nSelectedColumnOut = -1, nSelectedColumnIn = -1;

    nSelectedColumnIn = m_lstColumnIn.GetCurSel();
    nSelectedColumnIn = (int_t) (m_lstColumnIn.GetItemData(nSelectedColumnIn));
    if ((nSelectedColumnIn < 1) || (nSelectedColumnIn > m_nColumnCount)) {
        return;
    }
    nSelectedColumnIn -= 1;

    CWnd *pWnd = this->GetDlgItem(IDS_ANALYSIS_AVG);
    if (pWnd) {
        int_t nDim = m_pColumn[nSelectedColumnIn].dim;
        int_t nMax = ML_MIN(nDim / 8, 16);
        if (nMax < 1) {
            ::SendDlgItemMessage(GetSafeHwnd(), IDS_ANALYSIS_AVG, UDM_SETRANGE, 0, (LPARAM) MAKELONG(1, 1));
            pWnd->EnableWindow(FALSE);
        }
        else {
            ::SendDlgItemMessage(GetSafeHwnd(), IDS_ANALYSIS_AVG, UDM_SETRANGE, 0, (LPARAM) MAKELONG(nMax, 1));
            pWnd->EnableWindow(TRUE);
        }
    }

    nSelectedColumnOut = m_lstColumnOut.GetCurSel();
    nSelectedColumnOut = (int_t) (m_lstColumnOut.GetItemData(nSelectedColumnOut));
    if ((nSelectedColumnOut < 1) || (nSelectedColumnOut > m_nColumnCount)) {
        return;
    }
    nSelectedColumnOut -= 1;

    if (nSelectedColumnIn == nSelectedColumnOut) {
         m_lstColumnOut.SetCurSel(-1);
    }
}

void CAnalysisDlg::OnCbnSelchangeMathOperation()
{
    UpdateData(TRUE);

    CWnd *pWnd = this->GetDlgItem(IDK_ANALYSIS_FFTSHIFT);
    if (pWnd) {
        pWnd->EnableWindow(m_iOperation == 3);
    }
    pWnd = this->GetDlgItem(IDS_ANALYSIS_AVG);
    if (pWnd) {
        pWnd->EnableWindow(m_iOperation == 2);
    }
}
