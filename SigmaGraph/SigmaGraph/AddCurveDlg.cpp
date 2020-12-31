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
#include "AddCurveDlg.h"


// CAddCurveDlg dialog

IMPLEMENT_DYNAMIC(CAddCurveDlg, CDialog)

CAddCurveDlg::CAddCurveDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CAddCurveDlg::IDD, pParent)
    , m_strOutput(_T(""))
{
    m_pDoc = NULL;
    m_pPlot = NULL;
    m_pColumn = NULL;
    m_nColumnCount = 0;
    m_nSelectedColumnX = 0;
    m_nSelectedColumnY = 0;
    m_nCurve = -1;
    m_bIsModified = FALSE;

    m_bAxisXY = TRUE;
    m_bAxisXY2 = FALSE;
    m_bAxisX2Y2 = FALSE;
    m_bAxisXY2 = FALSE;

    m_pColumnAutoX = NULL;

    m_bLineColor = FALSE;
    m_bDotColor = FALSE;
    m_bLineStyle = FALSE;
    m_bDotStyle = FALSE;

    m_bInitialized = FALSE;
}

CAddCurveDlg::~CAddCurveDlg()
{

}

void CAddCurveDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDL_COLUMN_X, m_lstColumnX);
    DDX_Control(pDX, IDL_COLUMN_Y, m_lstColumnY);
    DDX_Text(pDX, IDL_ADDCURVE_OUTPUT, m_strOutput);
    DDX_Check(pDX, IDO_CURVE_AXISXY, m_bAxisXY);
    DDX_Check(pDX, IDO_CURVE_AXISXY2, m_bAxisXY2);
    DDX_Check(pDX, IDO_CURVE_AXISX2Y2, m_bAxisX2Y2);
    DDX_Check(pDX, IDO_CURVE_AXISX2Y, m_bAxisX2Y);
    DDX_Control(pDX, IDC_CURVE_LINECOLOR, m_cbLineColor);
    DDX_Control(pDX, IDC_CURVE_DOTCOLOR, m_cbDotColor);
    DDX_Control(pDX, IDC_DOT_STYLE, m_cbDotStyle);
    DDX_Control(pDX, IDC_LINE_STYLE, m_cbLineStyle);
    DDX_Control(pDX, ID_CURVE_ADD, m_bnAdd);
    DDX_Control(pDX, ID_CURVE_REMOVE, m_bnRemove);
}


BEGIN_MESSAGE_MAP(CAddCurveDlg, CDialog)
    ON_BN_CLICKED(ID_CURVE_ADD, &CAddCurveDlg::OnCurveAdd)
    ON_BN_CLICKED(ID_CURVE_REMOVE, &CAddCurveDlg::OnCurveRemove)
    ON_LBN_SELCHANGE(IDL_COLUMN_Y, &CAddCurveDlg::OnLbnSelchangeColumnY)
    ON_CBN_SELENDOK(IDC_CURVE_LINESIZE, &CAddCurveDlg::OnPlotLineSize)
    ON_CBN_SELENDOK(IDC_CURVE_DOTSIZE, &CAddCurveDlg::OnPlotDotSize)
    ON_CBN_SELCHANGE(IDC_CURVE_LINESIZE, &CAddCurveDlg::OnCbnSelchangeCurveLinesize)
    ON_CBN_SELCHANGE(IDC_CURVE_DOTSIZE, &CAddCurveDlg::OnCbnSelchangeCurveDotsize)
    ON_CBN_SELCHANGE(IDC_CURVE_LINECOLOR, &CAddCurveDlg::OnCbnSelchangeCurveLinecolor)
    ON_CBN_SELCHANGE(IDC_CURVE_DOTCOLOR, &CAddCurveDlg::OnCbnSelchangeCurveDotcolor)
    ON_CBN_SELCHANGE(IDC_CURVE_DOTCOLOR, &CAddCurveDlg::OnCbnSelchangeCurveDotcolor)
    ON_CBN_SELCHANGE(IDC_DOT_STYLE, &CAddCurveDlg::OnCbnSelchangeCurveDotstyle)
    ON_CBN_SELCHANGE(IDC_LINE_STYLE, &CAddCurveDlg::OnCbnSelchangeCurveLinestyle)
    ON_BN_CLICKED(IDOK, &CAddCurveDlg::OnOK)
    ON_WM_PAINT()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CAddCurveDlg message handlers

BOOL CAddCurveDlg::OnInitDialog()
{
    CComboBox *pComboLineSize, *pComboDotSize;
    CRect rcCombo;

    int_t jj, idx;
    char_t szBuffer[ML_STRSIZE];

    CDialog::OnInitDialog();

    if ((m_pPlot == NULL) || (m_pColumn == NULL) || (m_pColumnAutoX == NULL) || (m_nColumnCount < 1) || (m_nColumnCount > SIGMA_MAXCOLUMNS)) {
        SigmaApp.Output(_T("Cannot show the 'Add Curve' dialog: invalid column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }
    if (m_pDoc == NULL) {
        SigmaApp.Output(_T("Cannot show the 'Add Curve' dialog: invalid document.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    BOOL bAutoX = FALSE;
    memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
    int_t nT = 0, nTT = 0, nSelectedColumnX = -1, nSelectedColumnY = -1;
    for (jj = 0; jj < m_nColumnCount; jj++) {
        if ((m_pColumn[jj].type & 0xF0) == 0x10) {
            _tcsprintf(szBuffer, ML_STRSIZE - 1, _T("[%s] %s"), m_pColumn[jj].name, m_pColumn[jj].label);
            idx = m_lstColumnX.AddString((LPCTSTR)szBuffer);
            m_lstColumnX.SetItemData(idx, jj + 1);
            if (jj == m_nSelectedColumnX) {
                nSelectedColumnX = nT;
            }
            nT += 1;
        }
        else if ((m_pColumn[jj].type & 0xF0) == 0x20) {
            _tcsprintf(szBuffer, ML_STRSIZE - 1, _T("[%s] %s"), m_pColumn[jj].name, m_pColumn[jj].label);
            idx = m_lstColumnY.AddString((LPCTSTR)szBuffer);
            m_lstColumnY.SetItemData(idx, jj + 1);
            if (jj == m_nSelectedColumnY) {
                nSelectedColumnY = nTT;
                if (m_pColumn[jj].idx == m_pColumnAutoX->id) {
                    bAutoX = TRUE;
                }
            }
            nTT += 1;
        }
    }

    idx = m_lstColumnX.AddString(_T("AutoX [0 1 2 3 ...]"));
    m_lstColumnX.SetItemData(idx, (int_t) (m_pColumnAutoX->id));
    nT += 1;

    // Si une colonne Y est sélectionnée seule alors lui associer la 1ère colonne X
    if ((nSelectedColumnY >= 0) && (nSelectedColumnX < 0) ) {
        nSelectedColumnX = 0;
    }
    if (nT <= 1) {
        m_lstColumnX.SetCurSel(0);
    }
    else {
        if (bAutoX) {
            m_lstColumnX.SetCurSel(nT - 1);
        }
        else {
            m_lstColumnX.SetCurSel(nSelectedColumnX);
        }
    }
    m_lstColumnY.SetCurSel(nSelectedColumnY);

    m_nCurve = -1;
    if ((m_nSelectedColumnY >= 0) && (m_nSelectedColumnY < m_nColumnCount)) {
        if (m_pPlot->curvecount < 1) {
            m_nCurve = 0;
            m_bAxisXY = TRUE;
            m_bAxisXY2 = FALSE;
            m_bAxisX2Y = FALSE;
            m_bAxisX2Y2 = FALSE;
            m_pPlot->axistop.olabel = 0;
            m_pPlot->axistop.otitle = 0;
            m_pPlot->axisright.olabel = 0;
            m_pPlot->axisright.otitle = 0;
        }
        else {
            for (jj = 0; jj < m_pPlot->curvecount; jj++) {
                if (m_pPlot->curve[jj].y->id == m_pColumn[m_nSelectedColumnY].id) {
                    m_nCurve = jj;
                    m_bnAdd.SetWindowText(_T("&Modify"));
                    break;
                }
            }
            if (m_nCurve < 0) {
                if (m_pPlot->curvecount < ML_MAXCURVES) {
                    m_nCurve = m_pPlot->curvecount;
                    m_bnRemove.EnableWindow(FALSE);
                    m_bAxisXY = TRUE;
                    m_bAxisXY2 = FALSE;
                    m_bAxisX2Y = FALSE;
                    m_bAxisX2Y2 = FALSE;
                }
            }
            else {
                m_bAxisXY = FALSE;
                m_bAxisXY2 = FALSE;
                m_bAxisX2Y = FALSE;
                m_bAxisX2Y2 = FALSE;
                if (m_nCurve >= 0) {
                    if (((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x00)
                        && ((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x00)) {
                        m_bAxisXY = TRUE;
                    }
                    else if (((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x10)
                        && ((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x00)) {
                        m_bAxisX2Y = TRUE;
                    }
                    else if (((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x10)
                        && ((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x01)) {
                        m_bAxisX2Y2 = TRUE;
                    }
                    else if (((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x00)
                        && ((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x01)) {
                        m_bAxisXY2 = TRUE;
                    }
                    else {
                        m_bAxisXY = TRUE;
                    }
                }
            }
        }
    }

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_CURVE_LINESIZE);
    pComboDotSize = (CComboBox*)GetDlgItem(IDC_CURVE_DOTSIZE);

    if ((pComboLineSize == NULL) || (pComboDotSize == NULL)) {
        return FALSE;
    }

    int_t iCurve = m_pPlot->curveactive;
    if ((iCurve < 0) || (iCurve >= m_pPlot->curvecount)) {
        iCurve = 0;
    }

    // Line size
    pComboLineSize->GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    pComboLineSize->MoveWindow(rcCombo);

    pComboLineSize->AddString(_T("1"));
    pComboLineSize->AddString(_T("2"));
    pComboLineSize->AddString(_T("3"));
    pComboLineSize->AddString(_T("4"));
    pComboLineSize->AddString(_T("5"));
    pComboLineSize->AddString(_T("6"));
    pComboLineSize->AddString(_T("7"));
    pComboLineSize->SetCurSel((int_t) (m_pPlot->curve[iCurve].linesize) - 1);

    // Dot size
    pComboDotSize->GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    pComboDotSize->MoveWindow(rcCombo);

    pComboDotSize->AddString(_T("1"));
    pComboDotSize->AddString(_T("2"));
    pComboDotSize->AddString(_T("3"));
    pComboDotSize->AddString(_T("4"));
    pComboDotSize->AddString(_T("5"));
    pComboDotSize->AddString(_T("6"));
    pComboDotSize->AddString(_T("7"));
    pComboDotSize->AddString(_T("8"));
    int_t iSel = (int_t) (m_pPlot->curve[iCurve].dotsize) - 1;
    pComboDotSize->SetCurSel(iSel);

    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((m_pPlot->curve[iCurve].linestyle) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }
    for (jj = 0; jj < DOTSTYLE_MAXITEMS; jj++) {
        if (m_pPlot->curve[iCurve].dotstyle == CComboBoxDot::DOTSTYLE_VALUE[jj]) {
            m_cbDotStyle.SetCurSel(jj);
            break;
        }
    }
    m_cbLineStyle.m_Color = RGB(m_pPlot->curve[iCurve].linecolor.r, m_pPlot->curve[iCurve].linecolor.g, m_pPlot->curve[iCurve].linecolor.b);
    m_cbDotStyle.m_Color = RGB(m_pPlot->curve[iCurve].dotcolor.r, m_pPlot->curve[iCurve].dotcolor.g, m_pPlot->curve[iCurve].dotcolor.b);

    m_cbLineColor.setColor(m_cbLineStyle.m_Color);
    m_cbDotColor.setColor(m_cbDotStyle.m_Color);

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddCurveDlg::OnPlotLineSize()
{
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnPlotDotSize()
{
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnCurveAdd()
{
    UpdateData(TRUE);

    int_t iy, ix;
    vector_t *pColumnX = NULL;

    if ((m_pPlot->curvecount < 0) || (m_pPlot->curvecount >= ML_MAXCURVES)) {
        m_strOutput = _T("! Cannot add curve.");
        UpdateData(FALSE);
        return;
    }

    iy = m_lstColumnY.GetCurSel();
    iy = (int_t) (m_lstColumnY.GetItemData(iy));
    ix = m_lstColumnX.GetCurSel();
    ix = (int_t) (m_lstColumnX.GetItemData(ix));
    if ((iy < 1) || (iy > m_nColumnCount)) {
        return;
    }
    if ((ix != (int_t) (m_pColumnAutoX->id)) && ((ix < 1) || (ix > m_nColumnCount))) {
        return;
    }

    BOOL bAutoX = FALSE;

    if (ix == (int_t) (m_pColumnAutoX->id)) {
        bAutoX = TRUE;
    }
    pColumnX = (bAutoX == TRUE) ? m_pColumnAutoX : &(m_pColumn[ix - 1]);

    if (pColumnX == NULL) {
        return;
    }

    if ((m_nCurve < 0) || (m_nCurve > m_pPlot->curvecount)) {
        m_strOutput = _T("! Cannot add curve.");
        UpdateData(FALSE);
        return;
    }

    m_pColumn[iy - 1].status = 0x00;
    pColumnX->status = 0x00;
    m_pColumn[iy - 1].idx = pColumnX->id;

    byte_t dotstyle = 0x00, linestyle = 0x00;
    dotstyle = m_cbDotStyle.GetSelectedStyle();
    linestyle = m_cbLineStyle.GetSelectedStyle() | 0x01;
    if ((dotstyle == 0x00) && (linestyle == 0x00)) {
        dotstyle = 0x00;
        linestyle = 0x11;
    }

    // Ordonner la colonne X si nécessaire
    if (bAutoX == FALSE) {
        // [FIX-SG244-007]: ignore invalid items (not yet edited for example)
        int_t ii, jj, iStart = 0, iStop = pColumnX->dim - 1;
        if (pColumnX->mask[iStart] == 0xFF) {
            for (ii = iStart; ii < iStop; ii++) {
                if ((pColumnX->mask[ii] & 0x0F) == 0x00) {
                    iStart = ii;
                    break;
                }
            }
        }
        if (pColumnX->mask[iStop] == 0xFF) {
            for (ii = iStop; ii > iStart; ii--) {
                if ((pColumnX->mask[ii] & 0x0F) == 0x00) {
                    iStop = ii;
                    break;
                }
            }
        }
        if (pColumnX->dat[iStart] > pColumnX->dat[iStop]) {
            if (this->MessageBox(_T("The X column should be sorted in ascending order before adding curve.\nDo you want to proceed?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
                m_strOutput = _T("! Cannot add curve: X-Column nor sorted in ascending order.");
                UpdateData(FALSE);
                return;
            }
            byte_t ilink[ML_MAXCOLUMNS];
            memset(ilink, 0, ML_MAXCOLUMNS * sizeof(byte_t));
            // [FIX-SG244-008]: Sort columns that use the X-Column
            for (jj = 0; jj < m_nColumnCount; jj++) {
                if (((m_pColumn[jj].type & 0xF0) == 0x20) && (m_pColumn[jj].idx == pColumnX->id)) {
                    ilink[jj] = 1;
                }
            }
            //
            ilink[iy - 1] = 1;
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_sort(m_pColumn, m_nColumnCount, ix - 1, 1, ilink, iStart, iStop, SigmaApp.m_pLib);
            if (SigmaApp.m_pLib->errcode != 0) {
                m_strOutput = _T("! Cannot add curve: ");
                m_strOutput += (LPCTSTR)(SigmaApp.m_pLib->message);
                UpdateData(FALSE);
                return;
            }
        }
    }

    byte_t axisused = 0x00;
    if (m_bAxisXY == TRUE) {
        axisused = 0x00;
        if (m_pPlot->curvecount < 1) {
            m_pPlot->axistop.olabel = 0;
            m_pPlot->axistop.otitle = 0;
            m_pPlot->axisright.olabel = 0;
            m_pPlot->axisright.otitle = 0;
        }
    }
    else if (m_bAxisXY2 == TRUE) {
        axisused = 0x01;
        m_pPlot->axisright.olabel = 1;
        m_pPlot->axisright.otitle = 1;
    }
    else if (m_bAxisX2Y2 == TRUE) {
        axisused = 0x11;
        m_pPlot->axistop.olabel = 1;
        m_pPlot->axistop.otitle = 1;
        m_pPlot->axisright.olabel = 1;
        m_pPlot->axisright.otitle = 1;
    }
    else if (m_bAxisX2Y == TRUE) {
        axisused = 0x10;
        m_pPlot->axistop.olabel = 1;
        m_pPlot->axistop.otitle = 1;
    }
    else {
        m_bAxisXY = TRUE;
        axisused = 0x00;
        if (m_pPlot->curvecount < 1) {
            m_pPlot->axistop.olabel = 0;
            m_pPlot->axistop.otitle = 0;
            m_pPlot->axisright.olabel = 0;
            m_pPlot->axisright.otitle = 0;
        }
    }

    // >> Par défault les échelles sont linéaires
    if (m_pPlot->curvecount < 1) {
        m_pPlot->axisbottom.scale = 0;
        m_pPlot->axisleft.scale = 0;
        m_pPlot->axistop.scale = 0;
        m_pPlot->axisright.scale = 0;
    }
    // <<

    int iCurveCount = m_pPlot->curvecount;
    m_pPlot->errcode = 0;
    int iCurve = pl_curve_add(m_pPlot, pColumnX, &(m_pColumn[iy - 1]), NULL, NULL, axisused, 0x01);

    if ((m_pPlot->errcode != 0) || (iCurve < 0) || (iCurve >= ML_MAXCURVES)) {
        m_strOutput = _T("! Cannot add curve.");
        UpdateData(FALSE);
        return;
    }

    m_nCurve = iCurve;

    m_pPlot->curve[m_nCurve].dotstyle = dotstyle;
    m_pPlot->curve[m_nCurve].linestyle = linestyle;

    // Appliquer le même style de grid lorsqu'on change d'axe (à condition que cet axe ait été inutilisé)
    if (axisused != m_pPlot->curve[m_nCurve].axisused) {
        int_t jj, iXt = 0, iX2t = 0, iYt = 0, iY2t = 0;
        for (jj = 0; jj < m_pPlot->curvecount; jj++) {
            if ((m_pPlot->curve[jj].axisused & 0xF0) == 0x00) {                // X-Axis
                iXt += 1;
            }
            else if ((m_pPlot->curve[jj].axisused & 0xF0) == 0x10) {            // X2-Axis
                iX2t += 1;
            }
            if ((m_pPlot->curve[jj].axisused & 0x0F) == 0x00) {                // Y-Axis
                iYt += 1;
            }
            else if ((m_pPlot->curve[jj].axisused & 0x0F) == 0x01) {            // Y2-Axis
                iY2t += 1;
            }
        }
        if (((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x00) && ((axisused & 0xF0) == 0x10) && (iX2t < 1)) {    // X to X2
            m_pPlot->axistop.ogrid = m_pPlot->axisbottom.ogrid;
            m_pPlot->axistop.gridcolor.r = m_pPlot->axisbottom.gridcolor.r;
            m_pPlot->axistop.gridcolor.g = m_pPlot->axisbottom.gridcolor.g;
            m_pPlot->axistop.gridcolor.b = m_pPlot->axisbottom.gridcolor.b;
            m_pPlot->axistop.gridcount = m_pPlot->axisbottom.gridcount;
            m_pPlot->axistop.gridsize = m_pPlot->axisbottom.gridsize;
            m_pPlot->axistop.gridstyle = m_pPlot->axisbottom.gridstyle;
            m_pPlot->axistop.ogridm = m_pPlot->axisbottom.ogridm;
            m_pPlot->axistop.gridmcolor.r = m_pPlot->axisbottom.gridmcolor.r;
            m_pPlot->axistop.gridmcolor.g = m_pPlot->axisbottom.gridmcolor.g;
            m_pPlot->axistop.gridmcolor.b = m_pPlot->axisbottom.gridmcolor.b;
            m_pPlot->axistop.gridmcount = m_pPlot->axisbottom.gridmcount;
            m_pPlot->axistop.gridmsize = m_pPlot->axisbottom.gridmsize;
            m_pPlot->axistop.gridmstyle = m_pPlot->axisbottom.gridmstyle;
        }
        else if (((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x10) && ((axisused & 0xF0) == 0x00) && (iXt < 1)) {    // X2 to X
            m_pPlot->axisbottom.ogrid = m_pPlot->axistop.ogrid;
            m_pPlot->axisbottom.gridcolor.r = m_pPlot->axistop.gridcolor.r;
            m_pPlot->axisbottom.gridcolor.g = m_pPlot->axistop.gridcolor.g;
            m_pPlot->axisbottom.gridcolor.b = m_pPlot->axistop.gridcolor.b;
            m_pPlot->axisbottom.gridcount = m_pPlot->axistop.gridcount;
            m_pPlot->axisbottom.gridsize = m_pPlot->axistop.gridsize;
            m_pPlot->axisbottom.gridstyle = m_pPlot->axistop.gridstyle;
            m_pPlot->axisbottom.ogridm = m_pPlot->axistop.ogridm;
            m_pPlot->axisbottom.gridmcolor.r = m_pPlot->axistop.gridmcolor.r;
            m_pPlot->axisbottom.gridmcolor.g = m_pPlot->axistop.gridmcolor.g;
            m_pPlot->axisbottom.gridmcolor.b = m_pPlot->axistop.gridmcolor.b;
            m_pPlot->axisbottom.gridmcount = m_pPlot->axistop.gridmcount;
            m_pPlot->axisbottom.gridmsize = m_pPlot->axistop.gridmsize;
            m_pPlot->axisbottom.gridmstyle = m_pPlot->axistop.gridmstyle;
        }
        if (((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x00) && ((axisused & 0x0F) == 0x01) && (iY2t < 1)) {    // Y to Y2
            m_pPlot->axisright.ogrid = m_pPlot->axisleft.ogrid;
            m_pPlot->axisright.gridcolor.r = m_pPlot->axisleft.gridcolor.r;
            m_pPlot->axisright.gridcolor.g = m_pPlot->axisleft.gridcolor.g;
            m_pPlot->axisright.gridcolor.b = m_pPlot->axisleft.gridcolor.b;
            m_pPlot->axisright.gridcount = m_pPlot->axisleft.gridcount;
            m_pPlot->axisright.gridsize = m_pPlot->axisleft.gridsize;
            m_pPlot->axisright.gridstyle = m_pPlot->axisleft.gridstyle;
            m_pPlot->axisright.ogridm = m_pPlot->axisleft.ogridm;
            m_pPlot->axisright.gridmcolor.r = m_pPlot->axisleft.gridmcolor.r;
            m_pPlot->axisright.gridmcolor.g = m_pPlot->axisleft.gridmcolor.g;
            m_pPlot->axisright.gridmcolor.b = m_pPlot->axisleft.gridmcolor.b;
            m_pPlot->axisright.gridmcount = m_pPlot->axisleft.gridmcount;
            m_pPlot->axisright.gridmsize = m_pPlot->axisleft.gridmsize;
            m_pPlot->axisright.gridmstyle = m_pPlot->axisleft.gridmstyle;
        }
        if (((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x01) && ((axisused & 0x0F) == 0x00) && (iYt < 1)) {    // Y2 to Y
            m_pPlot->axisleft.ogrid = m_pPlot->axisright.ogrid;
            m_pPlot->axisleft.gridcolor.r = m_pPlot->axisright.gridcolor.r;
            m_pPlot->axisleft.gridcolor.g = m_pPlot->axisright.gridcolor.g;
            m_pPlot->axisleft.gridcolor.b = m_pPlot->axisright.gridcolor.b;
            m_pPlot->axisleft.gridcount = m_pPlot->axisright.gridcount;
            m_pPlot->axisleft.gridsize = m_pPlot->axisright.gridsize;
            m_pPlot->axisleft.gridstyle = m_pPlot->axisright.gridstyle;
            m_pPlot->axisleft.ogridm = m_pPlot->axisright.ogridm;
            m_pPlot->axisleft.gridmcolor.r = m_pPlot->axisright.gridmcolor.r;
            m_pPlot->axisleft.gridmcolor.g = m_pPlot->axisright.gridmcolor.g;
            m_pPlot->axisleft.gridmcolor.b = m_pPlot->axisright.gridmcolor.b;
            m_pPlot->axisleft.gridmcount = m_pPlot->axisright.gridmcount;
            m_pPlot->axisleft.gridmsize = m_pPlot->axisright.gridmsize;
            m_pPlot->axisleft.gridmstyle = m_pPlot->axisright.gridmstyle;
        }
    }

    m_pPlot->curve[m_nCurve].axisused = axisused;

    CComboBox *pComboLineSize, *pComboDotSize;
    COLORREF clrTmp;
    int_t iSel = 0;

    m_bnAdd.SetWindowText(_T("&Modify"));
    m_bnRemove.EnableWindow(TRUE);

    // >>
    pComboLineSize = (CComboBox*)GetDlgItem(IDC_CURVE_LINESIZE);
    pComboDotSize = (CComboBox*)GetDlgItem(IDC_CURVE_DOTSIZE);
    if ((pComboLineSize == NULL) || (pComboDotSize == NULL)) {
        m_strOutput = _T("! Invalid size.");
        UpdateData(FALSE);
        return;
    }

    iSel = pComboLineSize->GetCurSel();
    if (iSel != CB_ERR) {
        m_pPlot->curve[m_nCurve].linesize = iSel + 1;
    }

    clrTmp = m_cbLineColor.GetSelectedColor();
    m_pPlot->curve[m_nCurve].linecolor.r = GetRValue(clrTmp);
    m_pPlot->curve[m_nCurve].linecolor.g = GetGValue(clrTmp);
    m_pPlot->curve[m_nCurve].linecolor.b = GetBValue(clrTmp);
    if ((m_pPlot->curve[m_nCurve].linestyle & 0xF0) != 0x00) {
        m_pPlot->curve[m_nCurve].legend.font.color.r = m_pPlot->curve[m_nCurve].linecolor.r;
        m_pPlot->curve[m_nCurve].legend.font.color.g = m_pPlot->curve[m_nCurve].linecolor.g;
        m_pPlot->curve[m_nCurve].legend.font.color.b = m_pPlot->curve[m_nCurve].linecolor.b;
    }
    else if (m_pPlot->curve[m_nCurve].dotstyle != 0x00) {
        m_pPlot->curve[m_nCurve].legend.font.color.r = m_pPlot->curve[m_nCurve].dotcolor.r;
        m_pPlot->curve[m_nCurve].legend.font.color.g = m_pPlot->curve[m_nCurve].dotcolor.g;
        m_pPlot->curve[m_nCurve].legend.font.color.b = m_pPlot->curve[m_nCurve].dotcolor.b;
    }

    iSel = pComboDotSize->GetCurSel();
    if (iSel != CB_ERR) {
        m_pPlot->curve[m_nCurve].dotsize = (byte_t)(iSel + 1);
    }
    clrTmp = m_cbDotColor.GetSelectedColor();
    m_pPlot->curve[m_nCurve].dotcolor.r = GetRValue(clrTmp);
    m_pPlot->curve[m_nCurve].dotcolor.g = GetGValue(clrTmp);
    m_pPlot->curve[m_nCurve].dotcolor.b = GetBValue(clrTmp);
    if ((m_pPlot->curve[m_nCurve].dotstyle != 0x00) && ((m_pPlot->curve[m_nCurve].linestyle & 0xF0) == 0x00)) {
        m_pPlot->curve[m_nCurve].legend.font.color.r = m_pPlot->curve[m_nCurve].dotcolor.r;
        m_pPlot->curve[m_nCurve].legend.font.color.g = m_pPlot->curve[m_nCurve].dotcolor.g;
        m_pPlot->curve[m_nCurve].legend.font.color.b = m_pPlot->curve[m_nCurve].dotcolor.b;
    }

    m_pPlot->curve[m_nCurve].axisused = axisused;
    if (m_nCurve == 0) {
        if ((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x00) {
            _tcscpy(m_pPlot->axisbottom.title.text, (const char_t*)(m_pPlot->curve[m_nCurve].x->label));
        }
        else if ((m_pPlot->curve[m_nCurve].axisused & 0xF0) == 0x10) {
            _tcscpy(m_pPlot->axistop.title.text, (const char_t*)(m_pPlot->curve[m_nCurve].x->label));
        }
        if ((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x00) {
            _tcscpy(m_pPlot->axisleft.title.text, (const char_t*)(m_pPlot->curve[m_nCurve].y->label));
        }
        else if ((m_pPlot->curve[m_nCurve].axisused & 0x0F) == 0x01) {
            _tcscpy(m_pPlot->axisright.title.text, (const char_t*)(m_pPlot->curve[m_nCurve].y->label));
        }
    }

    m_cbLineStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbDotStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b);
    m_cbLineStyle.Invalidate();
    m_cbDotStyle.Invalidate();

    ((CSigmaDoc*) m_pDoc)->SetAutoScale(TRUE);
    m_pDoc->UpdateAllViews(NULL);
    m_pDoc->SetModifiedFlag(TRUE);

    m_strOutput = _T("Done.");

    UpdateData(FALSE);
}

void CAddCurveDlg::OnCurveRemove()
{
    UpdateData(TRUE);

    int_t iy, ix;

    iy = m_lstColumnY.GetCurSel();
    iy = (int_t) (m_lstColumnY.GetItemData(iy));
    ix = m_lstColumnX.GetCurSel();
    ix = (int_t) (m_lstColumnX.GetItemData(ix));
    if ((iy < 1) || (iy > m_nColumnCount)) {
        return;
    }
    if ((ix != (int_t) (m_pColumnAutoX->id)) && ((ix < 1) || (ix > m_nColumnCount))) {
        return;
    }

    if (m_pPlot->curvecount < 1) {
        return;
    }

    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_strOutput = _T("");

    int iCurveCount = m_pPlot->curvecount;

    m_pPlot->errcode = 0;
    pl_curve_remove(m_pPlot, m_nCurve);

    bool bCurveRemoved = (iCurveCount > m_pPlot->curvecount);

    if (m_pPlot->curveactive == m_nCurve) {
        m_pPlot->curveactive = 0;
    }

    if (m_pPlot->errcode != 0) {
        m_strOutput = _T("! Cannot remove curve.");
    }
    else {
        m_strOutput = _T("Done.");
    }

    if (m_pPlot->curvecount < ML_MAXCURVES) {
        m_nCurve = m_pPlot->curvecount;
        m_bnRemove.EnableWindow(FALSE);
    }

    int_t jj;
    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((m_pPlot->curve[m_nCurve].linestyle) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }
    for (jj = 0; jj < DOTSTYLE_MAXITEMS; jj++) {
        if (m_pPlot->curve[m_nCurve].dotstyle == CComboBoxDot::DOTSTYLE_VALUE[jj]) {
            m_cbDotStyle.SetCurSel(jj);
            break;
        }
    }
    m_cbLineStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbDotStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b);
    m_cbLineStyle.Invalidate();
    m_cbDotStyle.Invalidate();

    m_bnAdd.SetWindowText(_T("&Add"));
    m_bnRemove.EnableWindow(FALSE);

    if (bCurveRemoved) {
        ((CSigmaDoc*)m_pDoc)->SetAutoScale(TRUE);
        m_pDoc->UpdateAllViews(NULL);
        m_pDoc->SetModifiedFlag(TRUE);
    }

    UpdateData(FALSE);
}

void CAddCurveDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting


    if (m_bInitialized == FALSE) {
        CComboBox *pComboLineSize, *pComboDotSize;

        pComboLineSize = (CComboBox*)GetDlgItem(IDC_CURVE_LINESIZE);
        pComboDotSize = (CComboBox*)GetDlgItem(IDC_CURVE_DOTSIZE);

        if ((pComboLineSize == NULL) || (pComboDotSize == NULL)) {
            return;
        }
        pComboLineSize->SetCurSel((int_t) (m_pPlot->curve[0].linesize) - 1);
        int_t iSel = (int_t) (m_pPlot->curve[0].dotsize) - 1;
        pComboDotSize->SetCurSel(iSel);

        m_bInitialized = TRUE;
    }
    // Do not call CDialog::OnPaint() for painting messages
}

void CAddCurveDlg::OnCbnSelchangeCurveLinesize()
{
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnCbnSelchangeCurveDotsize()
{
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnCbnSelchangeCurveLinecolor()
{
    m_bLineColor = TRUE;
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnCbnSelchangeCurveLinestyle()
{
    m_bLineStyle = TRUE;
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnCbnSelchangeCurveDotcolor()
{
    m_bDotColor = TRUE;
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnCbnSelchangeCurveDotstyle()
{
    m_bDotStyle = TRUE;
    m_bIsModified = TRUE;
}

void CAddCurveDlg::OnOK()
{
    CDialog::OnOK();
}

void CAddCurveDlg::OnLbnSelchangeColumnY()
{
    int_t jj, ll, iy;

    UpdateData(TRUE);

    iy = m_lstColumnY.GetCurSel();
    iy = (int_t) (m_lstColumnY.GetItemData(iy));

    if ((iy < 1) || (iy > m_nColumnCount)) {
        m_nCurve = -1;
        return;
    }

    BOOL bFound = FALSE;
    if (m_pColumn[iy - 1].idx > 0) {
        if (m_pColumn[iy - 1].idx != m_pColumnAutoX->id) {
            for (jj = 0; jj < m_nColumnCount; jj++) {
                if (m_pColumn[jj].id == m_pColumn[iy - 1].idx) {
                    for (ll = 0; ll < m_lstColumnX.GetCount(); ll++) {
                        if (m_lstColumnX.GetItemData(ll) == (jj + 1)) {
                            m_lstColumnX.SetCurSel(ll);
                            break;
                        }
                    }
                    break;
                }
            }
        }
        else {
            m_lstColumnX.SetCurSel(m_lstColumnX.GetCount() - 1);
        }
    }

    m_bnAdd.SetWindowText(_T("&Add"));
    m_bnRemove.EnableWindow(TRUE);

    m_nCurve = -1;
    if (m_pPlot->curvecount < 1) {
        m_nCurve = 0;
        m_bAxisXY = TRUE;
        m_bAxisXY2 = FALSE;
        m_bAxisX2Y = FALSE;
        m_bAxisX2Y2 = FALSE;
        UpdateData(FALSE);
    }
    else {
        for (jj = 0; jj < m_pPlot->curvecount; jj++) {
            if (m_pPlot->curve[jj].y->id == m_pColumn[iy - 1].id) {
                m_nCurve = jj;
                m_bnAdd.SetWindowText(_T("&Modify"));
                break;
            }
        }
        if (m_nCurve < 0) {
            if (m_pPlot->curvecount < ML_MAXCURVES) {
                m_nCurve = m_pPlot->curvecount;
                m_bnRemove.EnableWindow(FALSE);
                m_bAxisXY = TRUE;
                m_bAxisXY2 = FALSE;
                m_bAxisX2Y = FALSE;
                m_bAxisX2Y2 = FALSE;
            }
            else {
                return;
            }
        }

        if ((m_nCurve >= 0) && (m_nCurve < m_pPlot->curvecount)) {
            m_bAxisXY = FALSE;
            m_bAxisXY2 = FALSE;
            m_bAxisX2Y = FALSE;
            m_bAxisX2Y2 = FALSE;
            switch (m_pPlot->curve[m_nCurve].axisused) {
                case 0x00:
                    m_bAxisXY = TRUE;
                    break;
                case 0x01:
                    m_bAxisXY2 = TRUE;
                    break;
                case 0x10:
                    m_bAxisX2Y = TRUE;
                    break;
                case 0x11:
                    m_bAxisX2Y2 = TRUE;
                    break;
                default:
                    m_bAxisXY = TRUE;
                    break;
            }

            int_t jj;
            for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
                if (((m_pPlot->curve[m_nCurve].linestyle) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
                    m_cbLineStyle.SetCurSel(jj);
                    break;
                }
            }
            for (jj = 0; jj < DOTSTYLE_MAXITEMS; jj++) {
                if (m_pPlot->curve[m_nCurve].dotstyle == CComboBoxDot::DOTSTYLE_VALUE[jj]) {
                    m_cbDotStyle.SetCurSel(jj);
                    break;
                }
            }
            m_cbLineStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
            m_cbDotStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b);
            m_cbLineStyle.Invalidate();
            m_cbDotStyle.Invalidate();
            m_cbLineColor.setColor(m_cbLineStyle.m_Color);
            m_cbDotColor.setColor(m_cbDotStyle.m_Color);
        }
        UpdateData(FALSE);
    }
}

void CAddCurveDlg::OnClose()
{

    CDialog::OnClose();
}
