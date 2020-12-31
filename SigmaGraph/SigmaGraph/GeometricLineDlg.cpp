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
#include "GeometricLineDlg.h"


// CGeometricLineDlg dialog

IMPLEMENT_DYNAMIC(CGeometricLineDlg, CDialog)

CGeometricLineDlg::CGeometricLineDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CGeometricLineDlg::IDD, pParent)
{
    m_bLineColor = FALSE;
    m_bLineStyle = FALSE;

    m_bArrowBegin = FALSE;
    m_bArrowEnd = FALSE;
    m_bArrowBoth = FALSE;
    m_bArrowNone = FALSE;
    m_bArrowStyle00 = FALSE;
    m_bArrowStyle01 = FALSE;

    m_pPlot = NULL;
    m_bInitialized = FALSE;
    m_ptInit.x = m_ptInit.y = 0;
    m_bOK = FALSE;
    m_nSelectedLine = -1;
}

CGeometricLineDlg::~CGeometricLineDlg()
{

}

void CGeometricLineDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDO_ARROW_BEGIN, m_bArrowBegin);
    DDX_Check(pDX, IDO_ARROW_END, m_bArrowEnd);
    DDX_Check(pDX, IDO_ARROW_BOTH, m_bArrowBoth);
    DDX_Check(pDX, IDO_ARROW_NONE, m_bArrowNone);
    DDX_Check(pDX, IDO_ARROW_STYLE00, m_bArrowStyle00);
    DDX_Check(pDX, IDO_ARROW_STYLE01, m_bArrowStyle01);
    DDX_Control(pDX, IDC_LINE_COLOR, m_cbLineColor);
    DDX_Control(pDX, IDC_LINE_STYLE, m_cbLineStyle);
}


BEGIN_MESSAGE_MAP(CGeometricLineDlg, CDialog)
    ON_WM_PAINT()
    ON_CBN_SELCHANGE(IDC_LINE_COLOR, &CGeometricLineDlg::OnLineColor)
    ON_CBN_SELCHANGE(IDC_LINE_STYLE, &CGeometricLineDlg::OnLineStyle)
    ON_BN_CLICKED(IDAPPLY, &CGeometricLineDlg::OnApply)
END_MESSAGE_MAP()


// CGeometricLineDlg message handlers
BOOL CGeometricLineDlg::OnInitDialog() 
{
    CComboBox *pComboLineSize;
    CRect rcCombo;

    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_LINE), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    if (m_pPlot == NULL) {
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if ((m_pPlot->linecount < 1) || (m_pPlot->linecount >= PL_MAXITEMS)) {
        CDialog::EndDialog(-1);
        return FALSE;
    }

    RECT rcT = {0,0,0,0};
    GetWindowRect(&rcT);
    ClientToScreen(&rcT);
    LONG ww = rcT.right - rcT.left, hh = rcT.bottom - rcT.top;
    if ((m_ptInit.x + ww) > m_recFrame.right) {
        m_ptInit.x = m_recFrame.right - ww;
    }
    if ((m_ptInit.y + hh) > m_recFrame.bottom) {
        m_ptInit.y = m_recFrame.bottom - hh;
    }
    SetWindowPos(NULL, m_ptInit.x, m_ptInit.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);
    if (pComboLineSize == NULL) {
        return FALSE;
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
    pComboLineSize->SetCurSel((int_t) (m_pPlot->line[m_nSelectedLine].size) - 1);

    if ((m_pPlot->line[m_nSelectedLine].arrstyle & 0xF0) == 0x10) {
        m_bArrowBegin = TRUE;
    }
    else if ((m_pPlot->line[m_nSelectedLine].arrstyle & 0xF0) == 0x20) {
        m_bArrowEnd = TRUE;
    }
    else if ((m_pPlot->line[m_nSelectedLine].arrstyle & 0xF0) == 0x30) {
        m_bArrowBoth = TRUE;
    }
    else {
        m_bArrowNone = TRUE;
    }
    if ((m_pPlot->line[m_nSelectedLine].arrstyle & 0x0F) == 0x01) {
        m_bArrowStyle00 = TRUE;
    }
    else {
        m_bArrowStyle01 = TRUE;
    }

    int_t jj;
    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if ((m_pPlot->line[m_nSelectedLine].style & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }
    m_cbLineStyle.m_Color = RGB(m_pPlot->line[m_nSelectedLine].color.r, m_pPlot->line[m_nSelectedLine].color.g, m_pPlot->line[m_nSelectedLine].color.b);

    m_cbLineColor.SetSelectedColor(RGB(m_pPlot->line[m_nSelectedLine].color.r, m_pPlot->line[m_nSelectedLine].color.g, m_pPlot->line[m_nSelectedLine].color.b));

    UpdateData(FALSE);

    return TRUE;
}

void CGeometricLineDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting


    if (m_bInitialized == FALSE) {
        CComboBox *pComboLineSize;

        pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);

        if (pComboLineSize == NULL) {
            return;
        }
        pComboLineSize->SetCurSel((int_t) (m_pPlot->line[m_nSelectedLine].size) - 1);

        m_bInitialized = TRUE;
    }
}

void CGeometricLineDlg::OnLineColor()
{
    m_bLineColor = TRUE;    
}

void CGeometricLineDlg::OnLineStyle()
{
    m_bLineStyle = TRUE;
}

void CGeometricLineDlg::OnOK()
{
    OnApply();

    m_bOK = TRUE;
    CDialog::OnOK();
}

void CGeometricLineDlg::OnCancel()
{
    CDialog::OnCancel();
}

void CGeometricLineDlg::OnApply()
{
    CComboBox *pComboLineSize;
    COLORREF clrTmp;
    int_t iSel = 0;

    // Processing
    pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);
    if (pComboLineSize == NULL) {
        return;
    }

    iSel = pComboLineSize->GetCurSel();
    if (iSel != CB_ERR) {
        m_pPlot->line[m_nSelectedLine].size = iSel + 1;
    }
    if (m_bLineColor == TRUE) {
        clrTmp = m_cbLineColor.GetSelectedColor();
        m_pPlot->line[m_nSelectedLine].color.r = GetRValue(clrTmp);
        m_pPlot->line[m_nSelectedLine].color.g = GetGValue(clrTmp);
        m_pPlot->line[m_nSelectedLine].color.b = GetBValue(clrTmp);
        m_cbLineStyle.m_Color = clrTmp;
        m_cbLineStyle.Invalidate();
        m_bLineColor = FALSE;
    }
    if (m_bLineStyle == TRUE) {
        m_pPlot->line[m_nSelectedLine].style = m_cbLineStyle.GetSelectedStyle();
        m_bLineStyle = FALSE;
    }

    UpdateData(TRUE);
    if (m_bArrowBegin == TRUE) {
        m_pPlot->line[m_nSelectedLine].arrstyle = 0x10;
    }
    else if (m_bArrowEnd == TRUE) {
        m_pPlot->line[m_nSelectedLine].arrstyle = 0x20;
    }
    else if (m_bArrowBoth == TRUE) {
        m_pPlot->line[m_nSelectedLine].arrstyle = 0x30;
    }
    else if (m_bArrowNone == TRUE) {
        m_pPlot->line[m_nSelectedLine].arrstyle = 0x00;
    }
    if (m_bArrowStyle00 == TRUE) {
        m_pPlot->line[m_nSelectedLine].arrstyle |= 0x01;
    }
    else if (m_bArrowStyle01 == TRUE) {
        m_pPlot->line[m_nSelectedLine].arrstyle |= 0x02;
    }
    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}
