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
#include "ErrorBarDlg.h"
#include "SigmaDoc.h"
#include "SigmaViewData.h"
#include "MainFrm.h"

// CErrorBarDlg dialog

IMPLEMENT_DYNAMIC(CErrorBarDlg, CDialog)

CErrorBarDlg::CErrorBarDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CErrorBarDlg::IDD, pParent)
    , m_bErrX(FALSE)
    , m_bErrY(FALSE)
    , m_bPercent(FALSE)
    , m_bFixed(FALSE)
    , m_bColumnY(FALSE)
    , m_bColumnX(FALSE)
{
    m_pDoc = NULL;
    m_pPlot = NULL;
    m_pDatasheet = NULL;

    m_nColumnCount = 0;
    m_pColumnX = NULL;
    m_pColumnY = NULL;
    m_pColumnErrX = NULL;
    m_pColumnErrY = NULL;

    m_nCurveSelected = -1;

    m_bLineSize = FALSE;
    m_bLineColor = FALSE;
    m_bLineStyle = FALSE;

    m_fPercent = 5.0;
    m_fFixed = 0.0;

    m_bInitialized = FALSE;
}

CErrorBarDlg::~CErrorBarDlg()
{

}

void CErrorBarDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDO_ERRBAR_X, m_bErrX);
    DDX_Check(pDX, IDO_ERRBAR_Y, m_bErrY);
    DDX_Check(pDX, IDO_ERRBAR_PERCENT, m_bPercent);
    DDX_Check(pDX, IDO_ERRBAR_FIXED, m_bFixed);
    DDX_Control(pDX, IDE_ERRBAR_PERCENT, m_edtPercent);
    DDX_Control(pDX, IDE_ERRBAR_FIXED, m_edtFixed);
    DDX_Text(pDX, IDE_ERRBAR_PERCENT, m_fPercent);
    DDX_Text(pDX, IDE_ERRBAR_FIXED, m_fFixed);
    DDX_Check(pDX, IDO_ERRBAR_COLUMNX, m_bColumnX);
    DDX_Check(pDX, IDO_ERRBAR_COLUMNY, m_bColumnY);
    DDX_Control(pDX, IDO_ERRBAR_COLUMNX, m_bnColumnX);
    DDX_Control(pDX, IDO_ERRBAR_COLUMNY, m_bnColumnY);
    DDX_Control(pDX, IDC_ERRBAR_COLUMNX, m_cbColumnX);
    DDX_Control(pDX, IDC_ERRBAR_COLUMNY, m_cbColumnY);
    DDX_Control(pDX, IDC_ERRBAR_LINECOLOR, m_cbLineColor);
    DDX_Control(pDX, IDC_LINE_STYLE, m_cbLineStyle);
    DDX_Control(pDX, IDC_LINE_SIZE, m_cbLineSize);
    DDX_Control(pDX, IDOK, m_bnApply);
}


BEGIN_MESSAGE_MAP(CErrorBarDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CErrorBarDlg::OnOK)
    ON_BN_CLICKED(ID_ERRBAR_REMOVE, &CErrorBarDlg::OnRemove)
    ON_BN_CLICKED(IDCANCEL, &CErrorBarDlg::OnCancel)
    ON_BN_CLICKED(IDO_ERRBAR_X, &CErrorBarDlg::OnBnClickedErrbarX)
    ON_BN_CLICKED(IDO_ERRBAR_Y, &CErrorBarDlg::OnBnClickedErrbarY)
    ON_CBN_SELCHANGE(IDC_ERRBAR_LINECOLOR, &CErrorBarDlg::OnCbnSelchangeLinecolor)
    ON_BN_CLICKED(IDO_ERRBAR_PERCENT, &CErrorBarDlg::OnBnClickedErrbarPercent)
    ON_BN_CLICKED(IDO_ERRBAR_FIXED, &CErrorBarDlg::OnBnClickedErrbarFixed)
    ON_EN_CHANGE(IDE_ERRBAR_PERCENT, &CErrorBarDlg::OnEnChangeErrbarPercent)
    ON_EN_CHANGE(IDE_ERRBAR_FIXED, &CErrorBarDlg::OnEnChangeErrbarFixed)
    ON_BN_CLICKED(IDO_ERRBAR_COLUMNX, &CErrorBarDlg::OnBnClickedErrbarColumnx)
    ON_BN_CLICKED(IDO_ERRBAR_COLUMNY, &CErrorBarDlg::OnBnClickedErrbarColumny)
    ON_CBN_SELCHANGE(IDC_ERRBAR_COLUMNX, &CErrorBarDlg::OnCbnSelchangeErrbarColumnx)
    ON_CBN_SELCHANGE(IDC_ERRBAR_COLUMNY, &CErrorBarDlg::OnCbnSelchangeErrbarColumny)
    ON_CBN_SELCHANGE(IDC_LINE_STYLE, &CErrorBarDlg::OnCbnSelchangeLinestyle)
    ON_CBN_SELCHANGE(IDC_LINE_SIZE, &CErrorBarDlg::OnCbnSelchangeLinesize)
    ON_WM_PAINT()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CErrorBarDlg message handlers

void CErrorBarDlg::VariableToControl()
{
    UpdateData(FALSE);
}

BOOL CErrorBarDlg::OnInitDialog()
{
    int_t jj, idx;
    vector_t *pColumn = NULL;

    CRect rcCombo;

    CDialog::OnInitDialog();

    ASSERT(m_pDoc);    ASSERT(m_pPlot);
    if ((m_pDoc == NULL) || (m_pPlot == NULL)) {
        SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    CSigmaViewData *pView = (CSigmaViewData*)(((CSigmaDoc*)m_pDoc)->GetDataView());
    if (pView == NULL) {
        CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
        ASSERT(pFrame != NULL);
        if (pFrame == NULL) {
            SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
        if (pFrame->CreateSigmaView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData), TRUE) == FALSE) {
            SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }

        pView = (CSigmaViewData*)(((CSigmaDoc*)m_pDoc)->GetDataView());
        if (pView == NULL) {
            SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
    }

    m_pDatasheet = pView->GetDatasheet();
    if (m_pDatasheet == NULL) {
        SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    pColumn = ((CSigmaDoc*)m_pDoc)->GetColumns(0, &m_nColumnCount);
    if ((m_nColumnCount < 2) || (m_nColumnCount > ML_MAXCOLUMNS)
        || (pColumn == NULL) || (m_pPlot->curvecount < 1)) {
        SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    m_nCurveSelected = m_pPlot->curveactive;
    if ((m_nCurveSelected < 0) || (m_nCurveSelected >= m_pPlot->curvecount)) {
        SigmaApp.Output(_T("Cannot show the error bar dialog: non active curve.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    m_pColumnX = NULL;
    m_pColumnY = NULL;
    m_cbColumnX.ResetContent();
    m_cbColumnY.ResetContent();
    for (jj = 0; jj < m_nColumnCount; jj++) {
        if ((pColumn[jj].type & 0xF0) == 0x30) {    // Err-X
            idx = m_cbColumnX.AddString((LPCTSTR)(pColumn[jj].label));
            m_cbColumnX.SetItemData(idx, jj + 1);
            if (m_pColumnX) {
                if (m_pColumnX->ide == pColumn[jj].id) {
                    m_cbColumnX.SetCurSel(idx);
                }
            }
        }
        if ((pColumn[jj].type & 0xF0) == 0x40) {    // Err-Y
            idx = m_cbColumnY.AddString((LPCTSTR)(pColumn[jj].label));
            m_cbColumnY.SetItemData(idx, jj + 1);
            if (m_pColumnY) {
                if (m_pColumnY->ide == pColumn[jj].id) {
                    m_cbColumnY.SetCurSel(idx);
                }
            }
        }
        if ((m_pPlot->curve[m_nCurveSelected].y->id == pColumn[jj].id) && ((pColumn[jj].type & 0xF0) == 0x20)) {
            m_pColumnY = &(pColumn[jj]);
        }
        if ((m_pPlot->curve[m_nCurveSelected].x->id == pColumn[jj].id) && ((pColumn[jj].type & 0xF0) == 0x10)) {
            m_pColumnX = &(pColumn[jj]);
        }
    }
    if ((m_pColumnX == NULL) || (m_pColumnY == NULL)) {
        SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if (m_pColumnX->ide > 0) {
        m_pColumnErrX = ((CSigmaDoc*)m_pDoc)->GetColumn(0, -1, m_pColumnX->ide);
        m_bColumnX = TRUE;
    }
    if (m_pColumnY->ide > 0) {
        m_pColumnErrY = ((CSigmaDoc*)m_pDoc)->GetColumn(0, -1, m_pColumnY->ide);
        m_bColumnY = TRUE;
        m_bColumnX = FALSE;
    }

    if (m_pColumnErrX != NULL) {
        if ((m_pColumnErrX->dim != m_pColumnX->dim) || (m_pColumnErrX->dim < 1) || (m_pColumnErrX->dim > ML_MAXPOINTS)) {
            SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
        m_bErrX = TRUE;
    }
    if (m_pColumnErrY != NULL) {
        if ((m_pColumnErrY->dim != m_pColumnY->dim) || (m_pColumnErrY->dim < 1) || (m_pColumnErrY->dim > ML_MAXPOINTS)) {
            SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
        m_bErrY = TRUE;
        m_bErrX = FALSE;
    }

    if ((m_pColumnErrX == NULL) && (m_pColumnErrY == NULL)) {
        if (m_nColumnCount > (ML_MAXCOLUMNS - 1)) {
            SigmaApp.Output(_T("Cannot show the error bar dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
    }

    if ((m_bColumnX == FALSE) && (m_bColumnY == FALSE)) {
        if (m_cbColumnY.GetCount() > 0) {
            m_bColumnY = TRUE;
        }
        else if (m_cbColumnX.GetCount() > 0) {
            m_bColumnX = TRUE;
        }
    }

    if ((m_cbColumnY.GetCount() <= 0) || (m_bErrY == FALSE)) {
        m_cbColumnY.EnableWindow(FALSE);
        m_bnColumnY.EnableWindow(FALSE);
        m_bColumnY = FALSE;
    }
    if ((m_cbColumnX.GetCount() <= 0) || (m_bErrX == FALSE)) {
        m_cbColumnX.EnableWindow(FALSE);
        m_bnColumnX.EnableWindow(FALSE);
        m_bColumnX = FALSE;
    }

    byte_t bStyle = (m_bErrX) ? m_pPlot->curve[m_nCurveSelected].exstyle : m_pPlot->curve[m_nCurveSelected].eystyle;
    COLORREF clrColor = (m_bErrX) ? RGB(m_pPlot->curve[m_nCurveSelected].excolor.r, m_pPlot->curve[m_nCurveSelected].excolor.g, m_pPlot->curve[m_nCurveSelected].excolor.b)
        : RGB(m_pPlot->curve[m_nCurveSelected].eycolor.r, m_pPlot->curve[m_nCurveSelected].eycolor.g, m_pPlot->curve[m_nCurveSelected].eycolor.b);

    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if ((bStyle & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }
    m_cbLineStyle.m_Color = clrColor;

    m_edtPercent.SetLimitText(6);
    m_edtFixed.SetLimitText(16);

    int isize = 1;
    if (m_bErrX == TRUE) {    // Err-X
        isize = m_pPlot->curve[m_nCurveSelected].exsize;
    }
    else if (m_bErrY == TRUE) {    // Err-Y
        isize = m_pPlot->curve[m_nCurveSelected].eysize;
    }

    // Line size
    m_cbLineSize.GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    m_cbLineSize.MoveWindow(rcCombo);

    m_cbLineSize.AddString(_T("1"));
    m_cbLineSize.AddString(_T("2"));
    m_cbLineSize.AddString(_T("3"));
    m_cbLineSize.AddString(_T("4"));
    m_cbLineSize.AddString(_T("5"));
    m_cbLineSize.AddString(_T("6"));
    m_cbLineSize.AddString(_T("7"));
    m_cbLineSize.SetCurSel(isize - 1);

    m_cbLineColor.SetSelectedColor(RGB(m_pPlot->curve[m_nCurveSelected].excolor.r, m_pPlot->curve[m_nCurveSelected].excolor.g, m_pPlot->curve[m_nCurveSelected].excolor.b));
    for (int_t jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((m_pPlot->curve[m_nCurveSelected].exstyle) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CErrorBarDlg::ControlToVariable()
{
    UpdateData(TRUE);    // Copy data to variables

}

void CErrorBarDlg::OnOK()
{
    int_t ii, jj;
    BOOL bUpdate = FALSE;

    CWaitCursor wcT;

    ControlToVariable();

    COLORREF clrTmp;
    int_t iSel = 0;

    if (m_bLineSize == TRUE) {
        int isize = m_cbLineSize.GetCurSel();
        if (isize != CB_ERR) {
            isize += 1;
        }
        else {
            isize = 1;
        }
        if (m_bErrX == TRUE) {    // Err-X
            m_pPlot->curve[m_nCurveSelected].exsize = isize;
            bUpdate = TRUE;
        }
        else if (m_bErrY == TRUE) {    // Err-Y
            m_pPlot->curve[m_nCurveSelected].eysize = isize;
            bUpdate = TRUE;
        }
    }

    if (m_bLineStyle == TRUE) {
        byte_t bStyle = m_cbLineStyle.GetSelectedStyle();
        if (m_bErrX == TRUE) {    // Err-X
            m_pPlot->curve[m_nCurveSelected].exstyle = bStyle;
            bUpdate = TRUE;
        }
        else if (m_bErrY == TRUE) {    // Err-Y
            m_pPlot->curve[m_nCurveSelected].eystyle = bStyle;
            bUpdate = TRUE;
        }
    }

    if (m_bLineColor == TRUE) {
        clrTmp = m_cbLineColor.GetSelectedColor();
        if (m_bErrX == TRUE) {    // Err-X
            m_pPlot->curve[m_nCurveSelected].excolor.r = GetRValue(clrTmp);
            m_pPlot->curve[m_nCurveSelected].excolor.g = GetGValue(clrTmp);
            m_pPlot->curve[m_nCurveSelected].excolor.b = GetBValue(clrTmp);
            m_cbLineStyle.m_Color = clrTmp;
            bUpdate = TRUE;
        }
        else if (m_bErrY == TRUE) {    // Err-Y
            m_pPlot->curve[m_nCurveSelected].eycolor.r = GetRValue(clrTmp);
            m_pPlot->curve[m_nCurveSelected].eycolor.g = GetGValue(clrTmp);
            m_pPlot->curve[m_nCurveSelected].eycolor.b = GetBValue(clrTmp);
            m_cbLineStyle.m_Color = clrTmp;
            bUpdate = TRUE;
        }
    }

    if (m_bColumnX == TRUE) {
        if (m_bErrX == FALSE) {    // Err-X
            return;
        }
        jj = m_cbColumnX.GetCurSel();
        jj = (int_t) (m_cbColumnX.GetItemData(jj));
        if ((jj < 1) || (jj > m_nColumnCount)) {
            return;
        }
        m_pColumnErrX = ((CSigmaDoc*)m_pDoc)->GetColumn(0, jj - 1, 0);
        if (m_pColumnErrX == NULL) {
            return;
        }
        if ((m_pColumnErrX->type & 0xF0) != 0x30) {    // Err-X
            return;
        }
        m_pColumnX->ide = m_pColumnErrX->id;
        m_pPlot->curve[m_nCurveSelected].ex = m_pColumnErrX;
        m_pDoc->SetModifiedFlag(TRUE);
        m_pDoc->UpdateAllViews(NULL);
        return;
    }
    else if (m_bColumnY == TRUE) {
        if (m_bErrY == FALSE) {    // Err-Y
            return;
        }
        jj = m_cbColumnY.GetCurSel();
        jj = (int_t) (m_cbColumnY.GetItemData(jj));
        if ((jj < 1) || (jj > m_nColumnCount)) {
            return;
        }
        m_pColumnErrY = ((CSigmaDoc*)m_pDoc)->GetColumn(0, jj - 1, 0);
        if (m_pColumnErrY == NULL) {
            return;
        }
        if ((m_pColumnErrY->type & 0xF0) != 0x40) {    // Err-Y
            return;
        }
        m_pColumnY->ide = m_pColumnErrY->id;
        m_pPlot->curve[m_nCurveSelected].ey = m_pColumnErrY;
        m_pDoc->SetModifiedFlag(TRUE);
        m_pDoc->UpdateAllViews(NULL);
        return;
    }

    if (m_bErrX == TRUE) {    // Err-X
        if (m_pColumnErrX == NULL) { // Create new column, if needed
            // On ne fait que des APPEND, donc les indices des colonnes existantes restent inchangés
            m_pColumnErrX = m_pDatasheet->AppendColumn();
            if (m_pColumnErrX) {
                m_pColumnErrX->type = 0x30;
                m_pColumnX->ide = m_pColumnErrX->id;
                m_pColumnErrX->status = 0x00;
                _tcsncpy(m_pColumnErrX->label, m_pColumnX->label, ML_STRSIZES - 7);
                _tcscat(m_pColumnErrX->label, _T("-Err"));
                m_pPlot->curve[m_nCurveSelected].ex = m_pColumnErrX;
                jj = m_cbColumnX.AddString((LPCTSTR)(m_pColumnErrX->label));
                m_cbColumnX.SetItemData(jj, m_nColumnCount + 1);
                m_nColumnCount += 1;
                m_cbColumnX.EnableWindow(TRUE);
                m_bnColumnX.EnableWindow(TRUE);
            }
        }
        if (m_pColumnErrX == NULL) {
            SigmaApp.Output(_T("Cannot create column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return;
        }
    }
    else if (m_bErrY == TRUE) {    // Err-Y
        if (m_pColumnErrY == NULL) { // Create new column, if needed
            // On ne fait que des APPEND, donc les indices des colonnes existantes restent inchangés
            m_pColumnErrY = m_pDatasheet->AppendColumn();
            if (m_pColumnErrY) {
                m_pColumnErrY->type = 0x40;
                m_pColumnY->ide = m_pColumnErrY->id;
                m_pColumnErrY->status = 0x00;
                _tcsncpy(m_pColumnErrY->label, m_pColumnY->label, ML_STRSIZES - 7);
                _tcscat(m_pColumnErrY->label, _T("-Err"));
                m_pPlot->curve[m_nCurveSelected].ey = m_pColumnErrY;
                jj = m_cbColumnY.AddString((LPCTSTR)(m_pColumnErrY->label));
                m_cbColumnY.SetItemData(jj, m_nColumnCount + 1);
                m_nColumnCount += 1;
                m_cbColumnY.EnableWindow(TRUE);
                m_bnColumnY.EnableWindow(TRUE);
            }
        }
        if (m_pColumnErrY == NULL) {
            SigmaApp.Output(_T("Cannot create column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return;
        }
    }
    else {
        if (bUpdate == TRUE) {
            m_cbLineStyle.Invalidate();
            m_pDoc->SetModifiedFlag(TRUE);
            m_pDoc->UpdateAllViews(NULL);
        }
        return;
    }

    if (m_bPercent == TRUE) {
        if ((m_fPercent <= 0.0) || ((m_fPercent > 100.0))) {
            SigmaApp.Output(_T("Invalid percentage.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return;
        }
        if (m_bErrX) {    // Err-X
            for (ii = 0; ii < m_pColumnErrX->dim; ii++) {
                m_pColumnErrX->dat[ii] = (real_t)fabs(m_pColumnX->dat[ii] * m_fPercent * 0.01);
                m_pColumnErrX->mask[ii] = 0x00;
            }
        }
        else {    // Err-Y
            for (ii = 0; ii < m_pColumnErrY->dim; ii++) {
                m_pColumnErrY->dat[ii] = (real_t)fabs(m_pColumnY->dat[ii] * m_fPercent * 0.01);
                m_pColumnErrY->mask[ii] = 0x00;
            }
        }
    }
    else if (m_bFixed == TRUE) {
        if (m_fFixed <= 0.0) {
            SigmaApp.Output(_T("Invalid fixed value.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return;
        }
        if (m_bErrX) {    // Err-X
            for (ii = 0; ii < m_pColumnErrX->dim; ii++) {
                m_pColumnErrX->dat[ii] = m_fFixed;
                m_pColumnErrX->mask[ii] = 0x00;
            }
        }
        else {    // Err-Y
            for (ii = 0; ii < m_pColumnErrY->dim; ii++) {
                m_pColumnErrY->dat[ii] = m_fFixed;
                m_pColumnErrY->mask[ii] = 0x00;
            }
        }
    }

    m_bLineSize = FALSE;
    m_bLineColor = FALSE;
    m_bLineStyle = FALSE;
    m_bnApply.EnableWindow(FALSE);

    m_cbLineStyle.Invalidate();
    m_pDoc->SetModifiedFlag(TRUE);
    m_pDoc->UpdateAllViews(NULL);
}

void CErrorBarDlg::OnRemove()
{
    ControlToVariable();

    int_t iSel = 0;

    if (m_bErrX == TRUE) {    // Err-X
        m_pColumnX->ide = 0;
        m_pPlot->curve[m_nCurveSelected].ex = NULL;
        m_pDoc->SetModifiedFlag(TRUE);
        m_pDoc->UpdateAllViews(NULL);
    }
    else if (m_bErrY == TRUE) {    // Err-Y
        m_pColumnY->ide = 0;
        m_pPlot->curve[m_nCurveSelected].ey = NULL;
        m_pDoc->SetModifiedFlag(TRUE);
        m_pDoc->UpdateAllViews(NULL);
    }
}

void CErrorBarDlg::OnCancel()
{
    CDialog::OnCancel();
}

void CErrorBarDlg::OnClose()
{
    CDialog::OnClose();
}

void CErrorBarDlg::OnBnClickedErrbarX()
{
    ControlToVariable();

    BOOL bEnable = (m_cbColumnX.GetCount() > 0);
    m_cbColumnX.EnableWindow(bEnable);
    m_bnColumnX.EnableWindow(bEnable);
    m_bColumnY = FALSE;
    VariableToControl();
    m_cbColumnY.EnableWindow(FALSE);
    m_bnColumnY.EnableWindow(FALSE);

    m_bnApply.EnableWindow(TRUE);

    // Line size
    int isize = 1;
    if (m_bErrX == TRUE) {    // Err-X
        isize = m_pPlot->curve[m_nCurveSelected].exsize;
    }
    else if (m_bErrY == TRUE) {    // Err-Y
        isize = m_pPlot->curve[m_nCurveSelected].eysize;
    }
    m_cbLineSize.SetCurSel(isize - 1);
}

void CErrorBarDlg::OnBnClickedErrbarY()
{
    ControlToVariable();

    BOOL bEnable = (m_cbColumnY.GetCount() > 0);
    m_cbColumnY.EnableWindow(bEnable);
    m_bnColumnY.EnableWindow(bEnable);
    m_bColumnX = FALSE;
    VariableToControl();
    m_cbColumnX.EnableWindow(FALSE);
    m_bnColumnX.EnableWindow(FALSE);

    m_bnApply.EnableWindow(TRUE);

    // Line size
    int isize = 1;
    if (m_bErrX == TRUE) {    // Err-X
        isize = m_pPlot->curve[m_nCurveSelected].exsize;
    }
    else if (m_bErrY == TRUE) {    // Err-Y
        isize = m_pPlot->curve[m_nCurveSelected].eysize;
    }
    m_cbLineSize.SetCurSel(isize - 1);

}

void CErrorBarDlg::OnBnClickedErrbarPercent()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnBnClickedErrbarFixed()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnEnChangeErrbarPercent()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnEnChangeErrbarFixed()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnBnClickedErrbarColumnx()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnBnClickedErrbarColumny()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnCbnSelchangeErrbarColumnx()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnCbnSelchangeErrbarColumny()
{
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnCbnSelchangeLinecolor()
{
    m_bLineColor = TRUE;
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnCbnSelchangeLinestyle()
{
    m_bLineStyle = TRUE;
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnCbnSelchangeLinesize()
{
    m_bLineSize = TRUE;
    m_bnApply.EnableWindow(TRUE);
}

void CErrorBarDlg::OnLineSize()
{
    m_bnApply.EnableWindow(TRUE);
}
