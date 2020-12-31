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
#include "PlotoptLine.h"
#include "SigmaDoc.h"

// CPlotoptLine dialog

IMPLEMENT_DYNAMIC(CPlotoptLine, CPropertyPage)

CPlotoptLine::CPlotoptLine()
    : CPropertyPage(CPlotoptLine::IDD)
    , m_nLine(0)
{
    m_pPlot = NULL;
    m_pDoc = NULL;
    m_bIsModified = FALSE;
    m_bClosing = FALSE;

    m_bLineColor = FALSE;

    m_bLineVert = FALSE;
    m_bLineHorz = FALSE;

    m_nLineCount = 0;

    m_bLineStyle = FALSE;

    m_bInitialized = FALSE;

    m_bTemplate = FALSE;
}

CPlotoptLine::~CPlotoptLine()
{

}

void CPlotoptLine::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);

    DDX_CBIndex(pDX, IDC_LINE, m_nLine);
    DDX_Check(pDX, IDO_LINE_HORZ, m_bLineHorz);
    DDX_Check(pDX, IDO_LINE_VERT, m_bLineVert);
    DDX_Control(pDX, IDC_LINE_STYLE, m_cbLineStyle);
    DDX_Control(pDX, IDC_LINE_COLOR, m_cbLineColor);
    DDX_Control(pDX, IDE_LINE_POS, m_edtPos);
    DDX_Control(pDX, IDE_LINE_MIN, m_edtMin);
    DDX_Control(pDX, IDE_LINE_MAX, m_edtMax);
    DDX_Control(pDX, ID_LINE_ADD, m_bnAdd);
    DDX_Control(pDX, ID_LINE_REMOVE, m_bnRemove);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}

BEGIN_MESSAGE_MAP(CPlotoptLine, CPropertyPage)
    ON_CBN_SELCHANGE(IDC_LINE, &CPlotoptLine::OnCbnSelchangeLine)
    ON_STN_CLICKED(IDC_LINE_COLOR, &CPlotoptLine::OnLineColor)
    ON_CBN_SELENDOK(IDC_LINE_STYLE, &CPlotoptLine::OnLineStyle)
    ON_CBN_SELENDOK(IDC_LINE_SIZE, &CPlotoptLine::OnLineSize)
    ON_BN_CLICKED(ID_LINE_APPLYALL, &CPlotoptLine::OnLineApplyAll)
    ON_BN_CLICKED(ID_LINE_DEFAULT, &CPlotoptLine::OnPlotDefault)
    ON_CBN_SELCHANGE(IDC_LINE_STYLE, &CPlotoptLine::OnCbnSelchangeLinestyle)
    ON_CBN_SELCHANGE(IDC_LINE_SIZE, &CPlotoptLine::OnCbnSelchangeLinesize)
    ON_CBN_SELCHANGE(IDC_LINE_COLOR, &CPlotoptLine::OnCbnSelchangeLinecolor)
    ON_BN_CLICKED(ID_LINE_ADD, &CPlotoptLine::OnLineAdd)
    ON_BN_CLICKED(ID_LINE_REMOVE, &CPlotoptLine::OnLineRemove)
    ON_BN_CLICKED(IDO_LINE_HORZ, &CPlotoptLine::OnLineHorz)
    ON_BN_CLICKED(IDO_LINE_VERT, &CPlotoptLine::OnLineVert)
    ON_EN_CHANGE(IDE_LINE_POS, &CPlotoptLine::OnEnChangeLinePos)
    ON_EN_CHANGE(IDE_LINE_MIN, &CPlotoptLine::OnEnChangeLineMin)
    ON_EN_CHANGE(IDE_LINE_MAX, &CPlotoptLine::OnEnChangeLineMax)
    ON_COMMAND(ID_HELP_CONTENTS, &CPlotoptLine::OnHelpContents)
    ON_WM_PAINT()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CPlotoptLine::Initialize()
{
    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        return FALSE;
    }

    // >> Rendre toutes les lignes visibles
    int_t jj;
    if (m_pPlot->linevertcount > 0) {
        for (jj = 0; jj < m_pPlot->linevertcount; jj++) {
            m_pPlot->linevert[jj].status = 0x00;        // Line visible
        }
    }
    if (m_pPlot->linehorzcount > 0) {
        for (jj = 0; jj < m_pPlot->linehorzcount; jj++) {
            m_pPlot->linehorz[jj].status = 0x00;        // Line visible
        }
    }
    // <<

    CComboBox *pComboLineSize;

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);

    if (pComboLineSize == NULL) {
        return FALSE;
    }

    if ((m_nLine < 0) || (m_nLine >= m_nLineCount)) {
        m_nLine = 0;
    }

    line_t *pLine = NULL;
    if (m_bLineHorz) {
        pLine = &(m_pPlot->linehorz[m_nLine]);
    }
    else if (m_bLineVert) {
        pLine = &(m_pPlot->linevert[m_nLine]);
    }
    else {
        return FALSE;
    }

    pComboLineSize->SetCurSel((int_t) (pLine->size) - 1);

    UpdateLine(FALSE);

    m_bInitialized = TRUE;

    return TRUE;
}

BOOL CPlotoptLine::OnInitDialog() 
{
    CComboBox *pComboLine;
    CComboBox *pComboLineSize;
    CRect rcCombo;
    CString strTmp;

    CPropertyPage::OnInitDialog();

    HWND hWnd = GetSafeHwnd();
    if (hWnd != NULL) {
        ::SendDlgItemMessage(hWnd, IDC_LINE, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDC_LINE_SIZE, CB_LIMITTEXT, (WPARAM)2, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_LINE_POS, EM_LIMITTEXT, (WPARAM)ML_STRSIZET, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_LINE_MIN, EM_LIMITTEXT, (WPARAM)ML_STRSIZET, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDE_LINE_MAX, EM_LIMITTEXT, (WPARAM)ML_STRSIZET, (LPARAM)0);
    }

    pComboLine = (CComboBox*)GetDlgItem(IDC_LINE);
    if (pComboLine == NULL) {
        return FALSE;
    }

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);

    if (pComboLineSize == NULL) {
        return FALSE;
    }

    // Line
    pComboLine->GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    pComboLine->MoveWindow(rcCombo);

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

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    return TRUE;
}

// CPlotoptAxis message handlers

void CPlotoptLine::OnLineColor()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnLineStyle()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnLineSize()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

BOOL CPlotoptLine::OnLineApply(int_t iLine)
{
    CComboBox *pComboLineSize;
    COLORREF clrTmp;
    int_t iSel = 0;
    line_t *pLine = NULL;

    // Processing
    if ((iLine < 0) || (iLine >= m_nLineCount)) {
        return FALSE;
    }

    if (m_bLineHorz) {
        pLine = &(m_pPlot->linehorz[iLine]);
        m_pPlot->linehorz[iLine].rect.ya = m_edtPos.GetValue();
        m_pPlot->linehorz[iLine].rect.xa = m_edtMin.GetValue();
        m_pPlot->linehorz[iLine].rect.xb = m_edtMax.GetValue();
    }
    else if (m_bLineVert) {
        pLine = &(m_pPlot->linevert[iLine]);
        m_pPlot->linevert[iLine].rect.xa = m_edtPos.GetValue();
        m_pPlot->linevert[iLine].rect.ya = m_edtMin.GetValue();
        m_pPlot->linevert[iLine].rect.yb = m_edtMax.GetValue();
    }
    else {
        return FALSE;        
    }

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);
    if (pComboLineSize == NULL) {
        return FALSE;
    }
    iSel = pComboLineSize->GetCurSel();
    if (iSel != CB_ERR) {
        pLine->size = iSel + 1;
    }
    if (m_bLineColor == TRUE) {
        clrTmp = m_cbLineColor.GetSelectedColor();
        pLine->color.r = GetRValue(clrTmp);
        pLine->color.g = GetGValue(clrTmp);
        pLine->color.b = GetBValue(clrTmp);
    }
    if (m_bLineStyle == TRUE) {
        pLine->style = m_cbLineStyle.GetSelectedStyle();
    }

    return TRUE;
}

BOOL CPlotoptLine::OnApply()
{
    if (m_bIsModified) {
        UpdateData(TRUE);    // Copy data to variables

        // Processing
        OnLineApply(m_nLine);

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

    return TRUE;
}

void CPlotoptLine::OnClose()
{

    BOOL *pbRunning = &(((CSigmaDoc*)m_pDoc)->m_bRunning);
    if (pbRunning) {
        *pbRunning = FALSE;
    }

    CPropertyPage::OnClose();
}

void CPlotoptLine::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    if (m_bInitialized == FALSE) {
        Initialize();
    }

    // Do not call CPropertyPage::OnPaint() for painting messages
}

void CPlotoptLine::OnLineApplyAll()
{
    if (m_bIsModified) {
        UpdateData(TRUE);    // Copy data to variables

        if (m_bLineHorz) {
            m_nLineCount = m_pPlot->linehorzcount;
        }
        else if (m_bLineVert) {
            m_nLineCount = m_pPlot->linevertcount;
        }
        else {
            CPropertyPage::OnApply();
            return;        
        }

        // Processing
        for (int_t ii = 0; ii < m_nLineCount; ii++) {
            OnLineApply(ii);
        }

        m_pDoc->SetModifiedFlag(TRUE);

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

    CPropertyPage::OnApply();

    m_bIsModified = FALSE;
    m_bLineColor = FALSE;
}

void CPlotoptLine::OnPlotDefault()
{
    OnApply();

    SigmaApp.m_pDefaultPlot->errcode = 0;
    pl_plot_dupstatic(SigmaApp.m_pDefaultPlot, m_pPlot);

    SigmaApp.setPlotSettings();
}

void CPlotoptLine::OnCbnSelchangeLinestyle()
{
    m_bLineStyle = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnCbnSelchangeLinesize()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnCbnSelchangeLinecolor()
{
    m_bLineColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnCbnSelchangeLine()
{
    UpdateData(TRUE);

    CComboBox *pComboLine;
    CComboBox *pComboLineSize;

    pComboLine = (CComboBox*)GetDlgItem(IDC_LINE);
    if (pComboLine == NULL) {
        return;
    }

    m_nLine = (int_t)pComboLine->GetCurSel();

    if ((m_nLine < 0) || (m_nLine >= m_nLineCount)) {
        return;
    }
    
    line_t *pLine = NULL;
    if (m_bLineHorz) {
        pLine = &(m_pPlot->linehorz[m_nLine]);
        m_edtPos.SetValue(pLine->rect.ya);
        m_edtMin.SetValue(pLine->rect.xa);
        m_edtMax.SetValue(pLine->rect.xb);
    }
    else if (m_bLineVert) {
        pLine = &(m_pPlot->linevert[m_nLine]);
        m_edtPos.SetValue(pLine->rect.xa);
        m_edtMin.SetValue(pLine->rect.ya);
        m_edtMax.SetValue(pLine->rect.yb);
    }
    else {
        return;
    }

    m_bnAdd.EnableWindow(m_nLineCount < PL_MAXITEMS);
    m_bnRemove.EnableWindow((m_nLine >= 0) && (m_nLine < m_nLineCount));

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);

    if (pComboLineSize == NULL) {
        return;
    }

    m_cbLineColor.SetSelectedColor(RGB(pLine->color.r, pLine->color.g, pLine->color.b));

    BOOL bFound = FALSE;
    for (int_t jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((pLine->style) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            bFound = TRUE;
            break;
        }
    }
    if (FALSE == bFound) {
        pLine->style = CComboBoxLine::LINESTYLE_VALUE[0];
        m_cbLineStyle.SetCurSel(0);
    }

    // Line size
    pComboLineSize->SetCurSel((int_t) (pLine->size) - 1);

    UpdateData(FALSE);
}

void CPlotoptLine::OnLineAdd()
{
    UpdateData(TRUE);    // Copy data to variables

    if ((m_nLineCount < 0) || (m_nLineCount >= PL_MAXITEMS)) {
        return;
    }

    CComboBox *pComboLineSize;
    COLORREF clrTmp;
    int_t iSel = 0;

    int_t jj;
    if (m_bLineHorz) {
        if (m_pPlot->linehorzcount > 0) {
            for (jj = 0; jj < m_pPlot->linehorzcount; jj++) {
                if (m_edtPos.GetValue() == m_pPlot->linehorz[jj].rect.ya) {
                    return;    // Ligne déjà tracée
                }
            }
        }
        m_nLine = m_nLineCount;
        m_nLineCount += 1;
        m_pPlot->linehorzcount += 1;
        m_pPlot->linehorz[m_nLine].rect.ya = m_edtPos.GetValue();
        m_pPlot->linehorz[m_nLine].rect.xa = m_edtMin.GetValue();
        m_pPlot->linehorz[m_nLine].rect.xb = m_edtMax.GetValue();
        m_pPlot->linehorz[m_nLine].status = 0x00;
        m_pPlot->linehorz[m_nLine].style = m_cbLineStyle.GetSelectedStyle();
        if (0x00 == m_pPlot->linehorz[m_nLine].style) {
            m_pPlot->linehorz[m_nLine].style = CComboBoxLine::LINESTYLE_VALUE[0];
        }
        if (RGB(m_pPlot->axisleft.color.r, m_pPlot->axisleft.color.g, m_pPlot->axisleft.color.b) == RGB(m_pPlot->linehorz[m_nLine].color.r, m_pPlot->linehorz[m_nLine].color.g, m_pPlot->linehorz[m_nLine].color.b)) {
            m_pPlot->linehorz[m_nLine].color.r = 255;
            m_pPlot->linehorz[m_nLine].color.g = 0;
            m_pPlot->linehorz[m_nLine].color.b = 0;
            m_cbLineColor.SetSelectedColor(RGB(m_pPlot->linehorz[m_nLine].color.r, m_pPlot->linehorz[m_nLine].color.g, m_pPlot->linehorz[m_nLine].color.b));
            UpdateData(FALSE);
        }

        pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);
        if (pComboLineSize == NULL) {
            return;
        }
        iSel = pComboLineSize->GetCurSel();
        if (iSel != CB_ERR) {
            m_pPlot->linehorz[m_nLine].size = iSel + 1;
        }
        if (m_bLineColor == TRUE) {
            clrTmp = m_cbLineColor.GetSelectedColor();
            m_pPlot->linehorz[m_nLine].color.r = GetRValue(clrTmp);
            m_pPlot->linehorz[m_nLine].color.g = GetGValue(clrTmp);
            m_pPlot->linehorz[m_nLine].color.b = GetBValue(clrTmp);
        }
    }
    else if (m_bLineVert) {
        if (m_pPlot->linevertcount > 0) {
            for (jj = 0; jj < m_pPlot->linevertcount; jj++) {
                if (m_edtPos.GetValue() == m_pPlot->linevert[jj].rect.xa) {
                    return;    // Ligne déjà tracée
                }
            }
        }
        m_nLine = m_nLineCount;
        m_nLineCount += 1;
        m_pPlot->linevertcount += 1;
        m_pPlot->linevert[m_nLine].rect.xa = m_edtPos.GetValue();
        m_pPlot->linevert[m_nLine].rect.ya = m_edtMin.GetValue();
        m_pPlot->linevert[m_nLine].rect.yb = m_edtMax.GetValue();
        m_pPlot->linevert[m_nLine].status = 0x00;
        m_pPlot->linevert[m_nLine].style = m_cbLineStyle.GetSelectedStyle();
        if (0x00 == m_pPlot->linevert[m_nLine].style) {
            m_pPlot->linevert[m_nLine].style = CComboBoxLine::LINESTYLE_VALUE[0];
        }
        if (RGB(m_pPlot->axisbottom.color.r, m_pPlot->axisbottom.color.g, m_pPlot->axisbottom.color.b) == RGB(m_pPlot->linevert[m_nLine].color.r, m_pPlot->linevert[m_nLine].color.g, m_pPlot->linevert[m_nLine].color.b)) {
            m_pPlot->linevert[m_nLine].color.r = 255;
            m_pPlot->linevert[m_nLine].color.g = 0;
            m_pPlot->linevert[m_nLine].color.b = 0;
            m_cbLineColor.SetSelectedColor(RGB(m_pPlot->linevert[m_nLine].color.r, m_pPlot->linevert[m_nLine].color.g, m_pPlot->linevert[m_nLine].color.b));
            UpdateData(FALSE);
        }

        pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);
        if (pComboLineSize == NULL) {
            return;
        }
        iSel = pComboLineSize->GetCurSel();
        if (iSel != CB_ERR) {
            m_pPlot->linevert[m_nLine].size = iSel + 1;
        }
        if (m_bLineColor == TRUE) {
            clrTmp = m_cbLineColor.GetSelectedColor();
            m_pPlot->linevert[m_nLine].color.r = GetRValue(clrTmp);
            m_pPlot->linevert[m_nLine].color.g = GetGValue(clrTmp);
            m_pPlot->linevert[m_nLine].color.b = GetBValue(clrTmp);
        }
    }
    else {
        return;
    }

    // Update window plot
    if (m_bClosing == FALSE) {
        if (m_bTemplate == TRUE) {
            GetParent()->GetParent()->Invalidate();
        }
        else {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }

    UpdateLine(FALSE);
}

void CPlotoptLine::OnLineRemove()
{
    UpdateData(TRUE);    // Copy data to variables

    if ((m_nLineCount < 1) || (m_nLineCount > PL_MAXITEMS)) {
        return;
    }
    if ((m_nLine < 0) || (m_nLine >= m_nLineCount)) {
        return;
    }

    int_t jj;

    if (m_bLineHorz) {
        if ((m_pPlot->linehorzcount > 1) && (m_nLine < (m_pPlot->linehorzcount - 1))) {
            for (jj = m_nLine; jj < m_pPlot->linehorzcount - 1; jj++) {
                m_pPlot->linehorz[jj].arrlength = m_pPlot->linehorz[jj + 1].arrlength;
                m_pPlot->linehorz[jj].arrstyle = m_pPlot->linehorz[jj + 1].arrstyle;
                m_pPlot->linehorz[jj].arrwidth = m_pPlot->linehorz[jj + 1].arrwidth;
                m_pPlot->linehorz[jj].color.r = m_pPlot->linehorz[jj + 1].color.r;
                m_pPlot->linehorz[jj].color.g = m_pPlot->linehorz[jj + 1].color.g;
                m_pPlot->linehorz[jj].color.b = m_pPlot->linehorz[jj + 1].color.b;
                m_pPlot->linehorz[jj].id = m_pPlot->linehorz[jj + 1].id;
                m_pPlot->linehorz[jj].orient = m_pPlot->linehorz[jj + 1].orient;
                m_pPlot->linehorz[jj].rect.xa = m_pPlot->linehorz[jj + 1].rect.xa;
                m_pPlot->linehorz[jj].rect.xb = m_pPlot->linehorz[jj + 1].rect.xb;
                m_pPlot->linehorz[jj].rect.ya = m_pPlot->linehorz[jj + 1].rect.ya;
                m_pPlot->linehorz[jj].rect.yb = m_pPlot->linehorz[jj + 1].rect.yb;
                m_pPlot->linehorz[jj].size = m_pPlot->linehorz[jj + 1].size;
                m_pPlot->linehorz[jj].status = m_pPlot->linehorz[jj + 1].status;
                m_pPlot->linehorz[jj].style = m_pPlot->linehorz[jj + 1].style;
            }
        }
        m_nLineCount -= 1;
        m_pPlot->linehorzcount -= 1;
    }
    else if (m_bLineVert) {
        if ((m_pPlot->linevertcount > 1) && (m_nLine < (m_pPlot->linevertcount - 1))) {
            for (jj = m_nLine; jj < m_pPlot->linevertcount - 1; jj++) {
                m_pPlot->linevert[jj].arrlength = m_pPlot->linevert[jj + 1].arrlength;
                m_pPlot->linevert[jj].arrstyle = m_pPlot->linevert[jj + 1].arrstyle;
                m_pPlot->linevert[jj].arrwidth = m_pPlot->linevert[jj + 1].arrwidth;
                m_pPlot->linevert[jj].color.r = m_pPlot->linevert[jj + 1].color.r;
                m_pPlot->linevert[jj].color.g = m_pPlot->linevert[jj + 1].color.g;
                m_pPlot->linevert[jj].color.b = m_pPlot->linevert[jj + 1].color.b;
                m_pPlot->linevert[jj].id = m_pPlot->linevert[jj + 1].id;
                m_pPlot->linevert[jj].orient = m_pPlot->linevert[jj + 1].orient;
                m_pPlot->linevert[jj].rect.xa = m_pPlot->linevert[jj + 1].rect.xa;
                m_pPlot->linevert[jj].rect.xb = m_pPlot->linevert[jj + 1].rect.xb;
                m_pPlot->linevert[jj].rect.ya = m_pPlot->linevert[jj + 1].rect.ya;
                m_pPlot->linevert[jj].rect.yb = m_pPlot->linevert[jj + 1].rect.yb;
                m_pPlot->linevert[jj].size = m_pPlot->linevert[jj + 1].size;
                m_pPlot->linevert[jj].status = m_pPlot->linevert[jj + 1].status;
                m_pPlot->linevert[jj].style = m_pPlot->linevert[jj + 1].style;
            }
        }
        m_nLineCount -= 1;
        m_pPlot->linevertcount -= 1;
    }
    else {
        return;
    }

    if ((m_nLine < 0) || (m_nLine >= m_nLineCount)) {
        m_nLine = 0;
    }

    // Update window plot
    if (m_bClosing == FALSE) {
        if (m_bTemplate == TRUE) {
            GetParent()->GetParent()->Invalidate();
        }
        else {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }

    UpdateLine(FALSE);
}

void CPlotoptLine::UpdateLine(BOOL bIsModified/* = FALSE*/)
{
    line_t *pLine = NULL;
    if (m_bLineHorz) {
        m_nLineCount = m_pPlot->linehorzcount;
        if ((m_nLine < 0) || (m_nLine >= m_nLineCount)) {
            m_nLine = 0;
        }
        pLine = &(m_pPlot->linehorz[m_nLine]);
        m_edtPos.SetValue(pLine->rect.ya);
        m_edtMin.SetValue(pLine->rect.xa);
        m_edtMax.SetValue(pLine->rect.xb);
    }
    else if (m_bLineVert) {
        m_nLineCount = m_pPlot->linevertcount;
        if ((m_nLine < 0) || (m_nLine >= m_nLineCount)) {
            m_nLine = 0;
        }
        pLine = &(m_pPlot->linevert[m_nLine]);
        m_edtPos.SetValue(pLine->rect.xa);
        m_edtMin.SetValue(pLine->rect.ya);
        m_edtMax.SetValue(pLine->rect.yb);
    }
    else {
        m_bLineHorz = TRUE;
        m_nLineCount = m_pPlot->linehorzcount;
        if ((m_nLine < 0) || (m_nLine >= m_nLineCount)) {
            m_nLine = 0;
        }
        pLine = &(m_pPlot->linehorz[m_nLine]);
        m_edtPos.SetValue(pLine->rect.ya);
        m_edtMin.SetValue(pLine->rect.xa);
        m_edtMax.SetValue(pLine->rect.xb);
    }

    CComboBox* pComboLine = (CComboBox*)GetDlgItem(IDC_LINE);
    if (pComboLine == NULL) {
        return;
    }

    CComboBox* pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);
    if (pComboLineSize == NULL) {
        return;
    }

    // Line
    pComboLine->ResetContent();
    if (m_nLineCount > 0) {
        char_t szT[16];
        for (int_t ii = 0; ii < m_nLineCount; ii++) {
            _tcsprintf(szT, 15, _T("%d"), ii + 1);
            pComboLine->AddString((const char_t*)szT);
        }
        pComboLine->SetCurSel(m_nLine);
    }

    m_cbLineColor.SetSelectedColor(RGB(pLine->color.r, pLine->color.g, pLine->color.b));

    // Line size
    pComboLineSize->SetCurSel((int_t) (pLine->size) - 1);

    BOOL bFound = FALSE;
    for (int_t jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if (((pLine->style) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            bFound = TRUE;
            break;
        }
    }
    if (FALSE == bFound) {
        pLine->style = CComboBoxLine::LINESTYLE_VALUE[0];
        m_cbLineStyle.SetCurSel(0);
    }

    m_bnAdd.EnableWindow(m_nLineCount < PL_MAXITEMS);
    m_bnRemove.EnableWindow((m_nLine >= 0) && (m_nLine < m_nLineCount));

    SetModified(bIsModified);    m_bIsModified = bIsModified;

    UpdateData(FALSE);
}

void CPlotoptLine::OnLineHorz()
{
    UpdateData(TRUE);

    UpdateLine(FALSE);
}

void CPlotoptLine::OnLineVert()
{
    UpdateData(TRUE);

    UpdateLine(FALSE);
}

void CPlotoptLine::OnEnChangeLinePos()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.


    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnEnChangeLineMin()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.


    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnEnChangeLineMax()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptLine::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/graph.html#graph_options_lines"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CPlotoptLine::PreTranslateMessage(MSG* pMsg)
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

void CPlotoptLine::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}