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
#include "PlotoptCurve.h"
#include "SigmaDoc.h"

// CPlotoptCurve dialog

IMPLEMENT_DYNAMIC(CPlotoptCurve, CPropertyPage)

CPlotoptCurve::CPlotoptCurve()
    : CPropertyPage(CPlotoptCurve::IDD)
    , m_nCurve(0)
    , m_bOLegend(FALSE)
    , m_strLegend(_T(""))
{
    m_pPlot = NULL;
    m_pDoc = NULL;
    m_bIsModified = FALSE;
    m_bClosing = FALSE;

    m_bLineColor = FALSE;
    m_bDotColor = FALSE;
    m_bDropColor = FALSE;
    m_bDropColorH = FALSE;

    m_bLineStyle = FALSE;
    m_bDropvStyle = FALSE;
    m_bDrophStyle = FALSE;
    m_bDotStyle = FALSE;
    m_bConnectStyle = FALSE;

    m_bAxisXY = FALSE;
    m_bAxisXY2 = FALSE;
    m_bAxisX2Y = FALSE;
    m_bAxisX2Y2 = FALSE;

    m_bInitialized = FALSE;

    m_bTemplate = FALSE;
}

CPlotoptCurve::~CPlotoptCurve()
{

}

void CPlotoptCurve::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);

    DDX_Text(pDX, IDE_CURVE_LEGEND, m_strLegend);
    DDX_Check(pDX, IDK_CURVE_LEGEND, m_bOLegend);
    DDX_Control(pDX, IDC_CURVE_LINESTYLE, m_cbLineStyle);
    DDX_Control(pDX, IDC_CURVE_DROPVSTYLE, m_cbDropvStyle);
    DDX_Control(pDX, IDC_CURVE_DROPHSTYLE, m_cbDrophStyle);
    DDX_Control(pDX, IDC_CURVE_DOTSTYLE, m_cbDotStyle);
    DDX_Control(pDX, IDC_CURVE_CONNECTSTYLE, m_cbConnectStyle);
    DDX_Control(pDX, IDC_CURVE_LINECOLOR, m_cbLineColor);
    DDX_Control(pDX, IDC_CURVE_DOTCOLOR, m_cbDotColor);
    DDX_Control(pDX, IDC_CURVE_DROPCOLOR, m_cbDropColor);
    DDX_Control(pDX, IDC_CURVE_DROPCOLORH, m_cbDropColorH);
    DDX_Control(pDX, ID_CURVE_ACTIVE, m_bnActive);
    DDX_Control(pDX, ID_CURVE_HIDE, m_bnHide);
    DDX_Control(pDX, ID_CURVE_REMOVE, m_bnRemove);
    DDX_Check(pDX, IDO_CURVE_AXISXY, m_bAxisXY);
    DDX_Check(pDX, IDO_CURVE_AXISXY2, m_bAxisXY2);
    DDX_Check(pDX, IDO_CURVE_AXISX2Y2, m_bAxisX2Y2);
    DDX_Check(pDX, IDO_CURVE_AXISX2Y, m_bAxisX2Y);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}

BEGIN_MESSAGE_MAP(CPlotoptCurve, CPropertyPage)
    ON_WM_PAINT()
    ON_EN_CHANGE(IDE_CURVE_LEGEND, &CPlotoptCurve::OnEditPlotLegend)
    ON_STN_CLICKED(IDC_CURVE_COLOR, &CPlotoptCurve::OnPlotColor)
    ON_CBN_SELENDOK(IDC_CURVE_LINESTYLE, &CPlotoptCurve::OnPlotLineStyle)
    ON_CBN_SELENDOK(IDC_CURVE_DROPVSTYLE, &CPlotoptCurve::OnPlotDropvStyle)
    ON_CBN_SELENDOK(IDC_CURVE_DROPHSTYLE, &CPlotoptCurve::OnPlotDrophStyle)
    ON_CBN_SELENDOK(IDC_CURVE_DOTSTYLE, &CPlotoptCurve::OnPlotDotStyle)
    ON_CBN_SELENDOK(IDC_CURVE_CONNECTSTYLE, &CPlotoptCurve::OnPlotConnectStyle)
    ON_CBN_SELENDOK(IDC_CURVE_LINESIZE, &CPlotoptCurve::OnPlotLineSize)
    ON_CBN_SELENDOK(IDC_CURVE_DOTSIZE, &CPlotoptCurve::OnPlotDotSize)
    ON_BN_CLICKED(ID_CURVE_HIDE, &CPlotoptCurve::OnCurveHide)
    ON_BN_CLICKED(ID_CURVE_APPLYALL, &CPlotoptCurve::OnCurveApplyAll)
    ON_BN_CLICKED(ID_CURVE_DEFAULT, &CPlotoptCurve::OnPlotDefault)
    ON_CBN_SELCHANGE(IDC_CURVE_LINESTYLE, &CPlotoptCurve::OnCbnSelchangeLineStyle)
    ON_CBN_SELCHANGE(IDC_CURVE_DROPVSTYLE, &CPlotoptCurve::OnCbnSelchangeDropvStyle)
    ON_CBN_SELCHANGE(IDC_CURVE_DROPHSTYLE, &CPlotoptCurve::OnCbnSelchangeDrophStyle)
    ON_CBN_SELCHANGE(IDC_CURVE_CONNECTSTYLE, &CPlotoptCurve::OnCbnSelchangeConnectStyle)
    ON_CBN_SELCHANGE(IDC_CURVE_DOTSTYLE, &CPlotoptCurve::OnCbnSelchangeDotStyle)
    ON_CBN_SELCHANGE(IDC_CURVE_LINESIZE, &CPlotoptCurve::OnCbnSelchangeCurveLinesize)
    ON_CBN_SELCHANGE(IDC_CURVE_DOTSIZE, &CPlotoptCurve::OnCbnSelchangeCurveDotsize)
    ON_CBN_SELCHANGE(IDC_CURVE_LINECOLOR, &CPlotoptCurve::OnCbnSelchangeCurveLinecolor)
    ON_CBN_SELCHANGE(IDC_CURVE_DOTCOLOR, &CPlotoptCurve::OnCbnSelchangeCurveDotcolor)
    ON_CBN_SELCHANGE(IDC_CURVE_DROPSIZE, &CPlotoptCurve::OnCbnSelchangeCurveDropsize)
    ON_CBN_SELCHANGE(IDC_CURVE_DROPCOLOR, &CPlotoptCurve::OnCbnSelchangeCurveDropcolor)
    ON_CBN_SELCHANGE(IDC_CURVE_DROPSIZEH, &CPlotoptCurve::OnCbnSelchangeCurveDropsizeH)
    ON_CBN_SELCHANGE(IDC_CURVE_DROPCOLORH, &CPlotoptCurve::OnCbnSelchangeCurveDropcolorH)
    ON_BN_CLICKED(IDK_CURVE_LEGEND, &CPlotoptCurve::OnBnClickedCurveLegend)
    ON_CBN_SELCHANGE(IDC_CURVE, &CPlotoptCurve::OnCbnSelchangeCurve)
    ON_BN_CLICKED(ID_CURVE_FONT, &CPlotoptCurve::OnCurveFont)
    ON_BN_CLICKED(ID_CURVE_REMOVE, &CPlotoptCurve::OnCurveRemove)
    ON_BN_CLICKED(ID_CURVE_ACTIVE, &CPlotoptCurve::OnCurveActive)
    ON_BN_CLICKED(IDO_CURVE_AXISXY, &CPlotoptCurve::OnAxisXY)
    ON_BN_CLICKED(IDO_CURVE_AXISXY2, &CPlotoptCurve::OnAxisXY2)
    ON_BN_CLICKED(IDO_CURVE_AXISX2Y2, &CPlotoptCurve::OnAxisX2Y2)
    ON_BN_CLICKED(IDO_CURVE_AXISX2Y, &CPlotoptCurve::OnAxisX2Y)
    ON_COMMAND(ID_HELP_CONTENTS, &CPlotoptCurve::OnHelpContents)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CPlotoptCurve::OnInitDialog() 
{
    CString strTmp;

    CPropertyPage::OnInitDialog();

    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        return FALSE;
    }

    CComboBox *pComboCurve;
    CComboBox *pComboLineSize, *pComboDotSize, *pComboDropSize, *pComboDropSizeH;

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_CURVE_LINESIZE);
    pComboDotSize = (CComboBox*)GetDlgItem(IDC_CURVE_DOTSIZE);
    pComboDropSize = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZE);
    pComboDropSizeH = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZEH);

    if ((pComboLineSize == NULL) || (pComboDotSize == NULL)
        || (pComboDropSize == NULL) || (pComboDropSizeH == NULL)) {
        return FALSE;
    }

    pComboCurve = (CComboBox*)GetDlgItem(IDC_CURVE);
    if (pComboCurve == NULL) {
        return FALSE;
    }

    // >> Increase performance
    pComboCurve->InitStorage(ML_MAXCURVES, ML_STRSIZET * sizeof(char_t));
    pComboCurve->SetRedraw(FALSE);
    pComboLineSize->InitStorage(8, ML_STRSIZET * sizeof(char_t));
    pComboLineSize->SetRedraw(FALSE);
    pComboDotSize->InitStorage(8, ML_STRSIZET * sizeof(char_t));
    pComboDotSize->SetRedraw(FALSE);
    pComboDropSize->InitStorage(8, ML_STRSIZET * sizeof(char_t));
    pComboDropSize->SetRedraw(FALSE);
    pComboDropSizeH->InitStorage(8, ML_STRSIZET * sizeof(char_t));
    pComboDropSizeH->SetRedraw(FALSE);
    // <<

    CRect rcCombo;

    // Curve
    pComboCurve->GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    pComboCurve->MoveWindow(rcCombo);

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

    pComboDropSize->AddString(_T("1"));
    pComboDropSize->AddString(_T("2"));
    pComboDropSize->AddString(_T("3"));
    pComboDropSize->AddString(_T("4"));
    pComboDropSize->AddString(_T("5"));
    pComboDropSize->AddString(_T("6"));
    pComboDropSize->AddString(_T("7"));

    pComboDropSizeH->AddString(_T("1"));
    pComboDropSizeH->AddString(_T("2"));
    pComboDropSizeH->AddString(_T("3"));
    pComboDropSizeH->AddString(_T("4"));
    pComboDropSizeH->AddString(_T("5"));
    pComboDropSizeH->AddString(_T("6"));
    pComboDropSizeH->AddString(_T("7"));

    // >> Increase performance
    pComboCurve->SetRedraw(TRUE);
    pComboLineSize->SetRedraw(TRUE);
    pComboDotSize->SetRedraw(TRUE);
    pComboDropSize->SetRedraw(TRUE);
    pComboDropSizeH->SetRedraw(TRUE);
    pComboCurve->Invalidate(TRUE);
    pComboLineSize->Invalidate(TRUE);
    pComboDotSize->Invalidate(TRUE);
    pComboDropSize->Invalidate(TRUE);
    pComboDropSizeH->Invalidate(TRUE);
    // <<

    HWND hWnd = GetSafeHwnd();
    if (hWnd != NULL) {
        ::SendDlgItemMessage(hWnd, IDE_CURVE, EM_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_CURVE_LEGEND, EM_LIMITTEXT, (WPARAM)60, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDC_CURVE_LINESIZE, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDC_CURVE_DOTSIZE, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDC_CURVE, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
    }

    initializeCurve();

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    m_bInitialized = TRUE;

    return TRUE;
}

void CPlotoptCurve::initializeCurve(void) 
{
    CString strTmp;

    CComboBox *pComboCurve;

    pComboCurve = (CComboBox*)GetDlgItem(IDC_CURVE);
    if (pComboCurve == NULL) {
        return;
    }
        
    CComboBox *pComboLineSize, *pComboDotSize, *pComboDropSize, *pComboDropSizeH;

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_CURVE_LINESIZE);
    pComboDotSize = (CComboBox*)GetDlgItem(IDC_CURVE_DOTSIZE);
    pComboDropSize = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZE);
    pComboDropSizeH = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZEH);

    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        m_nCurve = 0;
    }

    if (m_pPlot->curvecount < 1) {
        m_bnActive.EnableWindow(FALSE);
        m_bnHide.EnableWindow(FALSE);
        m_bnRemove.EnableWindow(FALSE);
        pComboCurve->ResetContent();
        pComboCurve->EnableWindow(FALSE);
        m_cbLineStyle.EnableWindow(FALSE);
        m_cbDropvStyle.EnableWindow(FALSE);
        m_cbDrophStyle.EnableWindow(FALSE);
        m_cbDotStyle.EnableWindow(FALSE);
        m_cbConnectStyle.EnableWindow(FALSE);
        m_cbDotColor.EnableWindow(FALSE);
        m_cbLineColor.EnableWindow(FALSE);
        m_cbDropColor.EnableWindow(FALSE);
        m_cbDropColorH.EnableWindow(FALSE);
    }
    else {
        m_cbLineStyle.EnableWindow(TRUE);
        m_cbDropvStyle.EnableWindow(TRUE);
        m_cbDrophStyle.EnableWindow(TRUE);
        m_cbDotStyle.EnableWindow(TRUE);
        m_cbConnectStyle.EnableWindow(TRUE);
        m_cbDotColor.EnableWindow(TRUE);
        m_cbLineColor.EnableWindow(TRUE);
        m_cbDropColor.EnableWindow(TRUE);
        m_cbDropColorH.EnableWindow(TRUE);
        pComboCurve->EnableWindow(TRUE);
        if (m_bTemplate == FALSE) {
            m_bnActive.EnableWindow(TRUE);
            m_bnHide.EnableWindow(TRUE);
            m_bnRemove.EnableWindow(TRUE);
        }
        else {
            m_bnActive.EnableWindow(FALSE);
            m_bnHide.EnableWindow(FALSE);
            m_bnRemove.EnableWindow(FALSE);
        }
    }

    m_strLegend = m_pPlot->curve[m_nCurve].legend.text;
    m_bOLegend = (m_pPlot->curve[m_nCurve].olegend == 1) ? TRUE : FALSE;

    m_bAxisXY = FALSE;
    m_bAxisXY2 = FALSE;
    m_bAxisX2Y = FALSE;
    m_bAxisX2Y2 = FALSE;
    if (m_nCurve >= 0) {
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
    }

    // Curve
    pComboCurve->ResetContent();
    char_t szT[ML_STRSIZET];
    for (int_t ii = 0; ii < m_pPlot->curvecount; ii++) {
        _tcsprintf(szT, ML_STRSIZET - 1, _T("%d"), ii + 1);
        pComboCurve->AddString((const char_t*)szT);
    }
    pComboCurve->SetCurSel(m_nCurve);

    int_t jj;
    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((m_pPlot->curve[m_nCurve].linestyle) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }
    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((m_pPlot->curve[m_nCurve].linestyle) & 0x0F) == CComboBoxConnect::CONNECTSTYLE_VALUE[jj]) {
            m_cbConnectStyle.SetCurSel(jj);
            break;
        }
    }
    for (jj = 0; jj < DOTSTYLE_MAXITEMS; jj++) {
        if (m_pPlot->curve[m_nCurve].dotstyle == CComboBoxDot::DOTSTYLE_VALUE[jj]) {
            m_cbDotStyle.SetCurSel(jj);
            break;
        }
    }

    m_cbLineColor.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b));
    m_cbDotColor.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b));
    m_cbDropColor.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].dropcolorv.r, m_pPlot->curve[m_nCurve].dropcolorv.g, m_pPlot->curve[m_nCurve].dropcolorv.b));
    m_cbDropColorH.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].dropcolorh.r, m_pPlot->curve[m_nCurve].dropcolorh.g, m_pPlot->curve[m_nCurve].dropcolorh.b));

    m_cbLineStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbConnectStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbDropvStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dropcolorv.r, m_pPlot->curve[m_nCurve].dropcolorv.g, m_pPlot->curve[m_nCurve].dropcolorv.b);
    m_cbDrophStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dropcolorh.r, m_pPlot->curve[m_nCurve].dropcolorh.g, m_pPlot->curve[m_nCurve].dropcolorh.b);
    m_cbDotStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b);

    // Line size
    pComboLineSize->SetCurSel((int_t) (m_pPlot->curve[m_nCurve].linesize) - 1);

    // Dot size
    int_t iSel = (int_t) (m_pPlot->curve[m_nCurve].dotsize) - 1;
    pComboDotSize->SetCurSel(iSel);

    pComboDropSize->SetCurSel((int_t) (m_pPlot->curve[m_nCurve].dropsizev) - 1);

    pComboDropSizeH->SetCurSel((int_t) (m_pPlot->curve[m_nCurve].dropsizeh) - 1);

    UpdateData(FALSE);

    return;
}

// CPlotoptAxis message handlers

void CPlotoptCurve::OnEditPlotLegend()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.


    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotColor()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotLineStyle()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotDropvStyle()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotDrophStyle()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotDotStyle()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotConnectStyle()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotLineSize()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnPlotDotSize()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

BOOL CPlotoptCurve::OnCurveApply(int_t iCurve)
{
    CComboBox *pComboLineSize, *pComboDotSize, *pComboDropSize, *pComboDropSizeH;
    COLORREF clrTmp;
    int_t iSel = 0;

    // Processing
    if (m_pPlot->curvecount < 1) {
        return FALSE;
    }
    if ((iCurve < 0) || (iCurve >= m_pPlot->curvecount)) {
        return FALSE;
    }
    SigmaApp.CStringToChar(m_strLegend, m_pPlot->curve[iCurve].legend.text, (int_t) (m_strLegend.GetLength()));
    m_pPlot->curve[iCurve].olegend = (m_bOLegend == TRUE) ? 1 : 0;

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_CURVE_LINESIZE);
    pComboDotSize = (CComboBox*)GetDlgItem(IDC_CURVE_DOTSIZE);
    pComboDropSize = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZE);
    pComboDropSizeH = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZEH);
    if ((pComboLineSize == NULL) || (pComboDotSize == NULL)
        || (pComboDropSize == NULL) || (pComboDropSizeH == NULL)) {
        return FALSE;
    }

    iSel = pComboLineSize->GetCurSel();
    if (iSel != CB_ERR) {
        m_pPlot->curve[iCurve].linesize = iSel + 1;
    }

    byte_t styleTmp;
    if (m_bLineStyle == TRUE) {
        styleTmp = m_cbLineStyle.GetSelectedStyle();
        m_pPlot->curve[iCurve].linestyle &= 0x0F;
        m_pPlot->curve[iCurve].linestyle |= styleTmp;
    }
    if (m_bConnectStyle == TRUE) {
        styleTmp = m_cbConnectStyle.GetSelectedStyle();
        m_pPlot->curve[iCurve].linestyle &= 0xF0;
        m_pPlot->curve[iCurve].linestyle |= styleTmp;
    }
    if (m_bDotStyle == TRUE) {
        styleTmp = m_cbDotStyle.GetSelectedStyle();
        m_pPlot->curve[iCurve].dotstyle = styleTmp;
    }
    if (m_bDropvStyle == TRUE) {
        styleTmp = m_cbDropvStyle.GetSelectedStyle();
        m_pPlot->curve[iCurve].dropstylev = styleTmp;
    }
    if (m_bDrophStyle == TRUE) {
        styleTmp = m_cbDrophStyle.GetSelectedStyle();
        m_pPlot->curve[iCurve].dropstyleh = styleTmp;
    }

    if (m_bLineColor == TRUE) {
        clrTmp = m_cbLineColor.GetSelectedColor();
        m_pPlot->curve[iCurve].linecolor.r = GetRValue(clrTmp);
        m_pPlot->curve[iCurve].linecolor.g = GetGValue(clrTmp);
        m_pPlot->curve[iCurve].linecolor.b = GetBValue(clrTmp);
        if ((m_pPlot->curve[iCurve].linestyle & 0xF0) != 0x00) {
            m_pPlot->curve[iCurve].legend.font.color.r = m_pPlot->curve[iCurve].linecolor.r;
            m_pPlot->curve[iCurve].legend.font.color.g = m_pPlot->curve[iCurve].linecolor.g;
            m_pPlot->curve[iCurve].legend.font.color.b = m_pPlot->curve[iCurve].linecolor.b;
        }
        else if (m_pPlot->curve[iCurve].dotstyle != 0x00) {
            m_pPlot->curve[iCurve].legend.font.color.r = m_pPlot->curve[iCurve].dotcolor.r;
            m_pPlot->curve[iCurve].legend.font.color.g = m_pPlot->curve[iCurve].dotcolor.g;
            m_pPlot->curve[iCurve].legend.font.color.b = m_pPlot->curve[iCurve].dotcolor.b;
        }
    }

    iSel = pComboDotSize->GetCurSel();
    if (iSel != CB_ERR) {
        m_pPlot->curve[iCurve].dotsize = (byte_t)(iSel + 1);
    }
    if (m_bDotColor == TRUE) {
        clrTmp = m_cbDotColor.GetSelectedColor();
        m_pPlot->curve[iCurve].dotcolor.r = GetRValue(clrTmp);
        m_pPlot->curve[iCurve].dotcolor.g = GetGValue(clrTmp);
        m_pPlot->curve[iCurve].dotcolor.b = GetBValue(clrTmp);
        if ((m_pPlot->curve[iCurve].dotstyle != 0x00) && ((m_pPlot->curve[iCurve].linestyle & 0xF0) == 0x00)) {
            m_pPlot->curve[iCurve].legend.font.color.r = m_pPlot->curve[iCurve].dotcolor.r;
            m_pPlot->curve[iCurve].legend.font.color.g = m_pPlot->curve[iCurve].dotcolor.g;
            m_pPlot->curve[iCurve].legend.font.color.b = m_pPlot->curve[iCurve].dotcolor.b;
        }
    }

    iSel = pComboDropSize->GetCurSel();
    if (iSel != CB_ERR) {
        m_pPlot->curve[iCurve].dropsizev = iSel + 1;
    }
    if (m_bDropColor == TRUE) {
        clrTmp = m_cbDropColor.GetSelectedColor();
        m_pPlot->curve[iCurve].dropcolorv.r = GetRValue(clrTmp);
        m_pPlot->curve[iCurve].dropcolorv.g = GetGValue(clrTmp);
        m_pPlot->curve[iCurve].dropcolorv.b = GetBValue(clrTmp);
    }

    iSel = pComboDropSizeH->GetCurSel();
    if (iSel != CB_ERR) {
        m_pPlot->curve[iCurve].dropsizeh = iSel + 1;
    }
    if (m_bDropColorH == TRUE) {
        clrTmp = m_cbDropColorH.GetSelectedColor();
        m_pPlot->curve[iCurve].dropcolorh.r = GetRValue(clrTmp);
        m_pPlot->curve[iCurve].dropcolorh.g = GetGValue(clrTmp);
        m_pPlot->curve[iCurve].dropcolorh.b = GetBValue(clrTmp);
    }

    m_cbLineStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbConnectStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbDropvStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dropcolorv.r, m_pPlot->curve[m_nCurve].dropcolorv.g, m_pPlot->curve[m_nCurve].dropcolorv.b);
    m_cbDrophStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dropcolorh.r, m_pPlot->curve[m_nCurve].dropcolorh.g, m_pPlot->curve[m_nCurve].dropcolorh.b);
    m_cbDotStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b);

    m_cbLineStyle.Invalidate();
    m_cbConnectStyle.Invalidate();
    m_cbDropvStyle.Invalidate();
    m_cbDrophStyle.Invalidate();
    m_cbDotStyle.Invalidate();

    // Used Axis
    byte_t axisused = 0x00;
    BOOL bAutoScale = FALSE;
    if (m_bAxisXY == TRUE) {
        axisused = 0x00;
        if (axisused != m_pPlot->curve[iCurve].axisused) {
            m_pPlot->axisbottom.olabel = 1;
            m_pPlot->axisbottom.otitle = 1;
            m_pPlot->axisleft.olabel = 1;
            m_pPlot->axisleft.otitle = 1;
            bAutoScale = TRUE;
        }
    }
    else if (m_bAxisXY2 == TRUE) {
        axisused = 0x01;
        if (axisused != m_pPlot->curve[iCurve].axisused) {
            m_pPlot->axisbottom.olabel = 1;
            m_pPlot->axisbottom.otitle = 1;
            m_pPlot->axisright.olabel = 1;
            m_pPlot->axisright.otitle = 1;
            bAutoScale = TRUE;
        }
    }
    else if (m_bAxisX2Y == TRUE) {
        axisused = 0x10;
        if (axisused != m_pPlot->curve[iCurve].axisused) {
            m_pPlot->axistop.olabel = 1;
            m_pPlot->axistop.otitle = 1;
            m_pPlot->axisleft.olabel = 1;
            m_pPlot->axisleft.otitle = 1;
            bAutoScale = TRUE;
        }
    }
    else if (m_bAxisX2Y2 == TRUE) {
        axisused = 0x11;
        if (axisused != m_pPlot->curve[iCurve].axisused) {
            m_pPlot->axistop.olabel = 1;
            m_pPlot->axistop.otitle = 1;
            m_pPlot->axisright.olabel = 1;
            m_pPlot->axisright.otitle = 1;
            bAutoScale = TRUE;
        }
    }

    // Appliquer le même style de grid lorsqu'on change d'axe (à condition que cet axe ait été inutilisée)
    if (axisused != m_pPlot->curve[iCurve].axisused) {
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
        if (((m_pPlot->curve[iCurve].axisused & 0xF0) == 0x00) && ((axisused & 0xF0) == 0x10) && (iX2t < 1)) {    // X to X2
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
        else if (((m_pPlot->curve[iCurve].axisused & 0xF0) == 0x10) && ((axisused & 0xF0) == 0x00) && (iXt < 1)) {    // X2 to X
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
        if (((m_pPlot->curve[iCurve].axisused & 0x0F) == 0x00) && ((axisused & 0x0F) == 0x01) && (iY2t < 1)) {    // Y to Y2
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
        if (((m_pPlot->curve[iCurve].axisused & 0x0F) == 0x01) && ((axisused & 0x0F) == 0x00) && (iYt < 1)) {    // Y2 to Y
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

        m_pPlot->curve[iCurve].axisused = axisused;

        // Modifier aussi le Fit
        if (m_bTemplate == FALSE) {
            if (m_pPlot->curve[iCurve].y->idf > 0) {
                if (axisused != m_pPlot->curve[iCurve].axisused) {
                    for (jj = 0; jj < m_pPlot->curvecount; jj++) {
                        if (m_pPlot->curve[jj].y->id == m_pPlot->curve[iCurve].y->idf) {
                            m_pPlot->curve[jj].axisused = m_pPlot->curve[iCurve].axisused;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (bAutoScale) {
        pl_autoscale(m_pPlot);
    }

    return TRUE;
}

BOOL CPlotoptCurve::OnApply()
{
    if (m_bIsModified) {
        UpdateData(TRUE);    // Copy data to variables

        // Processing
        OnCurveApply(m_nCurve);

        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }

        // Update window plot
        if (m_bClosing == FALSE) { 
            if (m_bTemplate == TRUE) {
                GetParent()->GetParent()->Invalidate();
            }
            else {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }

        if (m_bTemplate == FALSE) {
            ((CSigmaDoc*) m_pDoc)->SaveState();
        }
    }

    CPropertyPage::OnApply();

    m_bIsModified = FALSE;
    m_bLineColor = FALSE;
    m_bDotColor = FALSE;
    m_bDropColor = FALSE;
    m_bDropColorH = FALSE;
    m_bLineStyle = FALSE;
    m_bDotStyle = FALSE;
    m_bConnectStyle = FALSE;

    return TRUE;
}

void CPlotoptCurve::OnClose()
{

    CPropertyPage::OnClose();
}

void CPlotoptCurve::OnPaint()
{
    CPaintDC dc(this); // device context for painting


    m_bInitialized = TRUE;

    // Do not call CPropertyPage::OnPaint() for painting messages
}

void CPlotoptCurve::OnCurveHide()
{
    if (m_bTemplate) { return; }

    if (m_pPlot->curvecount < 1) {
        return;
    }

    UpdateData(TRUE);    // Copy data to variables

    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    if ((m_pPlot->curve[m_nCurve].status & 0x0F) == 0x0F) {    // Hidden curve
        m_pPlot->curve[m_nCurve].status &= 0xF0;
    }
    else {    // Shown curve
        m_pPlot->curve[m_nCurve].status |= 0x0F;
    }

    // [FIX-SG240-002] The document state is saved when the curve is modified.
    if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }

    // Update window plot
    if (m_bClosing == FALSE) {
        if (m_bTemplate == TRUE) {
            GetParent()->GetParent()->Invalidate();
        }
        else {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
}

void CPlotoptCurve::OnCurveApplyAll()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }

    UpdateData(TRUE);    // Copy data to variables

    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    if (m_bIsModified && (m_pPlot->curvecount > 0)) {
        // Processing
        for (int_t ii = 0; ii < m_pPlot->curvecount; ii++) {
            OnCurveApply(ii);
        }

        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }

        // Update window plot
        if (m_bClosing == FALSE) {
            if (m_bTemplate == TRUE) {
                GetParent()->GetParent()->Invalidate();
            }
            else {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }

        if (m_bTemplate == FALSE) {
            ((CSigmaDoc*) m_pDoc)->SaveState();
        }
    }

    CPropertyPage::OnApply();

    m_bIsModified = FALSE;
    m_bLineColor = FALSE;
    m_bDotColor = FALSE;
    m_bDropColor = FALSE;
    m_bDropColorH = FALSE;
    m_bLineStyle = FALSE;
    m_bDotStyle = FALSE;
    m_bConnectStyle = FALSE;
}

void CPlotoptCurve::OnPlotDefault()
{
    OnApply();

    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SigmaApp.m_pDefaultPlot->errcode = 0;
    pl_plot_dupstatic(SigmaApp.m_pDefaultPlot, m_pPlot);

    SigmaApp.setPlotSettings();
}

void CPlotoptCurve::OnCbnSelchangeCurveLinesize()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurveDotsize()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurveDropsize()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeLineStyle()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bLineStyle = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeDropvStyle()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bDropvStyle = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeDrophStyle()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bDrophStyle = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeDotStyle()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bDotStyle = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeConnectStyle()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bConnectStyle = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurveLinecolor()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bLineColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurveDotcolor()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bDotColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurveDropcolor()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bDropColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurveDropcolorH()
{
    
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_bDropColorH = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnBnClickedCurveLegend()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurveDropsizeH()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnCbnSelchangeCurve()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }

    CComboBox *pComboCurve;
    CComboBox *pComboLineSize, *pComboDotSize, *pComboDropSize, *pComboDropSizeH;

    pComboCurve = (CComboBox*)GetDlgItem(IDC_CURVE);
    if (pComboCurve == NULL) {
        return;
    }

    m_nCurve = (int_t)pComboCurve->GetCurSel();

    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    if (m_bTemplate == FALSE) {
        m_bnActive.EnableWindow(TRUE);
        m_bnHide.EnableWindow(TRUE);
        m_bnRemove.EnableWindow(TRUE);
    }
    else {
        m_bnActive.EnableWindow(FALSE);
        m_bnHide.EnableWindow(FALSE);
        m_bnRemove.EnableWindow(FALSE);
    }

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_CURVE_LINESIZE);
    pComboDotSize = (CComboBox*)GetDlgItem(IDC_CURVE_DOTSIZE);
    pComboDropSize = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZE);
    pComboDropSizeH = (CComboBox*)GetDlgItem(IDC_CURVE_DROPSIZEH);

    if ((pComboLineSize == NULL) || (pComboDotSize == NULL)
        || (pComboDropSize == NULL) || (pComboDropSizeH == NULL)) {
        return;
    }

    int_t jj;
    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((m_pPlot->curve[m_nCurve].linestyle) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }
    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((m_pPlot->curve[m_nCurve].linestyle) & 0x0F) == CComboBoxConnect::CONNECTSTYLE_VALUE[jj]) {
            m_cbConnectStyle.SetCurSel(jj);
            break;
        }
    }
    for (jj = 0; jj < DOTSTYLE_MAXITEMS; jj++) {
        if (m_pPlot->curve[m_nCurve].dotstyle == CComboBoxDot::DOTSTYLE_VALUE[jj]) {
            m_cbDotStyle.SetCurSel(jj);
            break;
        }
    }

    m_cbLineColor.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b));
    m_cbDotColor.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b));
    m_cbDropColor.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].dropcolorv.r, m_pPlot->curve[m_nCurve].dropcolorv.g, m_pPlot->curve[m_nCurve].dropcolorv.b));
    m_cbDropColorH.SetSelectedColor(RGB(m_pPlot->curve[m_nCurve].dropcolorh.r, m_pPlot->curve[m_nCurve].dropcolorh.g, m_pPlot->curve[m_nCurve].dropcolorh.b));

    m_cbLineStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbConnectStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].linecolor.r, m_pPlot->curve[m_nCurve].linecolor.g, m_pPlot->curve[m_nCurve].linecolor.b);
    m_cbDropvStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dropcolorv.r, m_pPlot->curve[m_nCurve].dropcolorv.g, m_pPlot->curve[m_nCurve].dropcolorv.b);
    m_cbDrophStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dropcolorh.r, m_pPlot->curve[m_nCurve].dropcolorh.g, m_pPlot->curve[m_nCurve].dropcolorh.b);
    m_cbDotStyle.m_Color = RGB(m_pPlot->curve[m_nCurve].dotcolor.r, m_pPlot->curve[m_nCurve].dotcolor.g, m_pPlot->curve[m_nCurve].dotcolor.b);

    m_cbLineStyle.Invalidate();
    m_cbConnectStyle.Invalidate();
    m_cbDropvStyle.Invalidate();
    m_cbDrophStyle.Invalidate();
    m_cbDotStyle.Invalidate();

    m_strLegend = m_pPlot->curve[m_nCurve].legend.text;
    m_bOLegend = (m_pPlot->curve[m_nCurve].olegend == 1) ? TRUE : FALSE;

    // Line size
    pComboLineSize->SetCurSel((int_t) (m_pPlot->curve[m_nCurve].linesize) - 1);

    // Symbol size
    int_t iSel = (int_t) (m_pPlot->curve[m_nCurve].dotsize) - 1;
    pComboDotSize->SetCurSel(iSel);

    // Drop size
    pComboDropSize->SetCurSel((int_t) (m_pPlot->curve[m_nCurve].dropsizev) - 1);
    pComboDropSizeH->SetCurSel((int_t) (m_pPlot->curve[m_nCurve].dropsizeh) - 1);

    // Used Axis
    m_bAxisXY = FALSE;
    m_bAxisXY2 = FALSE;
    m_bAxisX2Y = FALSE;
    m_bAxisX2Y2 = FALSE;
    if (m_nCurve >= 0) {
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
    }

    UpdateData(FALSE);
}

void CPlotoptCurve::OnCurveFont()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    
    LOGFONT lft;

    memset(&lft, 0, sizeof(lft));

    CComboBox *pComboCurve;
    pComboCurve = (CComboBox*)GetDlgItem(IDC_CURVE);
    if (pComboCurve == NULL) {
        return;
    }
    m_nCurve = (int_t)pComboCurve->GetCurSel();

    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    if (SigmaApp.FontToLogfont(m_pPlot->curve[m_nCurve].legend.font, &lft) == FALSE) {
        return;
    }

    if (SigmaApp.GetFont(m_hWnd, &lft)) {
        SigmaApp.LogfontToFont(lft, &(m_pPlot->curve[m_nCurve].legend.font));

        // Update window plot
        if (m_bClosing == FALSE) {
            if (m_bTemplate == TRUE) {
                GetParent()->GetParent()->Invalidate();
            }
            else {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }

        // [FIX-SG200-002] The document state is saved when the graph fonts change.
        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }
    }
}

void CPlotoptCurve::OnCurveRemove()
{
    if (m_bTemplate) { return; }

    if (m_pPlot->curvecount < 1) {
        return;
    }

    UpdateData(TRUE);    // Copy data to variables

    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    long_t uiIDf = m_pPlot->curve[m_nCurve].y->idf;
    long_t uiID = m_pPlot->curve[m_nCurve].y->id;

    // Supprimer aussi s'il s'agit d'un fit
    if (((CSigmaDoc*)m_pDoc)->GetColumn(1, -1, uiID) != NULL) {

        // supprimer du tracé
        pl_curve_remove(m_pPlot, m_nCurve);

        // supprimer le fit
        ((CSigmaDoc*)m_pDoc)->DeleteColumn(1, -1, uiID);

        initializeCurve();

        return;
    }

    m_pPlot->curve[m_nCurve].y->idf = 0;
    m_pPlot->errcode = 0;
    pl_curve_remove(m_pPlot, m_nCurve);
    // Supprimer aussi la courbe du fit
    if (uiIDf > 0) {
        if (m_pPlot->curvecount > 1) {
            for (int_t jj = 0; jj < m_pPlot->curvecount; jj++) {
                if (m_pPlot->curve[jj].y->id == uiIDf) {
                    m_pPlot->errcode = 0;
                    pl_curve_remove(m_pPlot, jj);
                    // supprimer aussi la colonne du fit
                    ((CSigmaDoc*)m_pDoc)->DeleteColumn(1, -1, uiIDf);
                    break;
                }
            }
        }
    }

    if ((m_pPlot->curveactive < 0) || (m_pPlot->curveactive >= m_pPlot->curvecount)) {
        m_pPlot->curveactive = 0;
    }
    m_nCurve = m_pPlot->curveactive;

    initializeCurve();

    m_bInitialized = FALSE;

    // [FIX-SG240-002] The document state is saved when the curve is modified.
    if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }

    // Update window plot
    if (m_bClosing == FALSE) {
        if (m_bTemplate == TRUE) {
            GetParent()->GetParent()->Invalidate();
        }
        else {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
}

void CPlotoptCurve::OnCurveActive()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }

    UpdateData(TRUE);    // Copy data to variables
    
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    m_pPlot->curveactive = m_nCurve;

    // [FIX-SG240-002] The document state is saved when the curve is modified.
    if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }

    // Update window plot
    if (m_bClosing == FALSE) {
        if (m_bTemplate == TRUE) {
            GetParent()->GetParent()->Invalidate();
        }
        else {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }
}

void CPlotoptCurve::OnAxisXY()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnAxisXY2()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnAxisX2Y2()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnAxisX2Y()
{
    if (m_pPlot->curvecount < 1) {
        return;
    }
    if ((m_nCurve < 0) || (m_nCurve >= m_pPlot->curvecount)) {
        return;
    }

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptCurve::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/graph.html#graph_options_curves"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CPlotoptCurve::PreTranslateMessage(MSG* pMsg)
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

void CPlotoptCurve::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}
