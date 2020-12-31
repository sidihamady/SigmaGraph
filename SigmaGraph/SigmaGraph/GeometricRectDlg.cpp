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
#include "GeometricRectDlg.h"


// CGeometricRectDlg dialog

IMPLEMENT_DYNAMIC(CGeometricRectDlg, CDialog)

CGeometricRectDlg::CGeometricRectDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CGeometricRectDlg::IDD, pParent)
{
    m_nGeo = 0;    // Rectangle

    m_bLineColor = FALSE;
    m_bFillColor = FALSE;
    m_bLineStyle = FALSE;

    m_pPlot = NULL;
    m_bInitialized = FALSE;
    m_ptInit.x = m_ptInit.y = 0;
    m_bOK = FALSE;
    m_nSelected = -1;
    m_bStyle = FALSE;
}

CGeometricRectDlg::~CGeometricRectDlg()
{

}

void CGeometricRectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LINE_COLOR, m_cbLineColor);
    DDX_Control(pDX, IDC_FILL_COLOR, m_cbFillColor);
    DDX_Control(pDX, IDC_LINE_STYLE, m_cbLineStyle);
    DDX_Check(pDX, IDK_FILL, m_bStyle);
}


BEGIN_MESSAGE_MAP(CGeometricRectDlg, CDialog)
    ON_WM_PAINT()
    ON_CBN_SELCHANGE(IDC_LINE_COLOR, &CGeometricRectDlg::OnLineColor)
    ON_CBN_SELCHANGE(IDC_LINE_STYLE, &CGeometricRectDlg::OnLineStyle)
    ON_CBN_SELCHANGE(IDC_FILL_COLOR, &CGeometricRectDlg::OnFillColor)
    ON_BN_CLICKED(IDAPPLY, &CGeometricRectDlg::OnApply)
END_MESSAGE_MAP()


// CGeometricRectDlg message handlers
BOOL CGeometricRectDlg::OnInitDialog() 
{
    CComboBox *pComboLineSize;
    CRect rcCombo;

    CDialog::OnInitDialog();

    HICON hIcon = (m_nGeo == 0) ? (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_RECT), IMAGE_ICON, 16, 16, 0)
        : (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(ID_PLOT_ADDELLIPSE), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    if (m_pPlot == NULL) {
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if (((m_pPlot->rectanglecount < 1) || (m_pPlot->rectanglecount >= PL_MAXITEMS))
        && ((m_pPlot->ellipsecount < 1) || (m_pPlot->ellipsecount >= PL_MAXITEMS))) {

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

    if (m_nGeo == 0) {
        SetWindowText(_T("Rectangle Properties"));
    }
    else {
        SetWindowText(_T("Ellipse Properties"));
    }

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
    if (m_nGeo == 0) {
        pComboLineSize->SetCurSel((int_t) (m_pPlot->rectangle[m_nSelected].size) - 1);
    }
    else {
        pComboLineSize->SetCurSel((int_t) (m_pPlot->ellipse[m_nSelected].size) - 1);
    }

    m_bStyle = FALSE;
    byte_t bStyle;

    if (m_nGeo == 0) {
        if (((m_pPlot->rectangle[m_nSelected].style) & 0x01) == 0x01) {
            m_bStyle = TRUE;
        }
        bStyle = m_pPlot->rectangle[m_nSelected].status;
        m_cbLineStyle.m_Color = RGB(m_pPlot->rectangle[m_nSelected].color.r, m_pPlot->rectangle[m_nSelected].color.g, m_pPlot->rectangle[m_nSelected].color.b);
    }
    else {
        if (((m_pPlot->ellipse[m_nSelected].style) & 0x01) == 0x01) {
            m_bStyle = TRUE;
        }
        bStyle = m_pPlot->ellipse[m_nSelected].status;
        m_cbLineStyle.m_Color = RGB(m_pPlot->ellipse[m_nSelected].color.r, m_pPlot->ellipse[m_nSelected].color.g, m_pPlot->ellipse[m_nSelected].color.b);
    }

    int_t jj;
    for (jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
        if ((bStyle & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
            m_cbLineStyle.SetCurSel(jj);
            break;
        }
    }

    UpdateData(FALSE);

    return TRUE;
}

void CGeometricRectDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting


    if (m_bInitialized == FALSE) {
        CComboBox *pComboLineSize;

        pComboLineSize = (CComboBox*)GetDlgItem(IDC_LINE_SIZE);

        if (pComboLineSize == NULL) {
            return;
        }
        if (m_nGeo == 0) {
            pComboLineSize->SetCurSel((int_t) (m_pPlot->rectangle[m_nSelected].size) - 1);
            m_cbLineColor.SetSelectedColor(RGB(m_pPlot->rectangle[m_nSelected].color.r, m_pPlot->rectangle[m_nSelected].color.g, m_pPlot->rectangle[m_nSelected].color.b));
            m_cbFillColor.SetSelectedColor(RGB(m_pPlot->rectangle[m_nSelected].fcolor.r, m_pPlot->rectangle[m_nSelected].fcolor.g, m_pPlot->rectangle[m_nSelected].fcolor.b));
            BOOL bFound = FALSE;
            for (int_t jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
                if (((m_pPlot->rectangle[m_nSelected].style) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
                    m_cbLineStyle.SetCurSel(jj);
                    bFound = TRUE;
                    break;
                }
            }
            if (FALSE == bFound) {
                m_pPlot->rectangle[m_nSelected].style = CComboBoxLine::LINESTYLE_VALUE[0];
                m_cbLineStyle.SetCurSel(0);
            }
        }
        else {
            pComboLineSize->SetCurSel((int_t) (m_pPlot->ellipse[m_nSelected].size) - 1);
            m_cbLineColor.SetSelectedColor(RGB(m_pPlot->ellipse[m_nSelected].color.r, m_pPlot->ellipse[m_nSelected].color.g, m_pPlot->ellipse[m_nSelected].color.b));
            m_cbFillColor.SetSelectedColor(RGB(m_pPlot->ellipse[m_nSelected].fcolor.r, m_pPlot->ellipse[m_nSelected].fcolor.g, m_pPlot->ellipse[m_nSelected].fcolor.b));
            BOOL bFound = FALSE;
            for (int_t jj = 0; jj < LINESTYLE_MAXITEMS; jj++) {
                if (((m_pPlot->ellipse[m_nSelected].style) & 0xF0) == CComboBoxLine::LINESTYLE_VALUE[jj]) {
                    m_cbLineStyle.SetCurSel(jj);
                    bFound = TRUE;
                    break;
                }
            }
            if (FALSE == bFound) {
                m_pPlot->ellipse[m_nSelected].style = CComboBoxLine::LINESTYLE_VALUE[0];
                m_cbLineStyle.SetCurSel(0);
            }
        }

        m_bInitialized = TRUE;
    }
}

void CGeometricRectDlg::OnLineColor()
{
    m_bLineColor = TRUE;
}

void CGeometricRectDlg::OnLineStyle()
{
    m_bLineStyle = TRUE;
}


void CGeometricRectDlg::OnFillColor()
{
    m_bFillColor = TRUE;
}

void CGeometricRectDlg::OnOK()
{
    OnApply();

    m_bOK = TRUE;
    CDialog::OnOK();
}

void CGeometricRectDlg::OnCancel()
{
    CDialog::OnCancel();
}

void CGeometricRectDlg::OnApply()
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
        if (m_nGeo == 0) {
            m_pPlot->rectangle[m_nSelected].size = iSel + 1;
        }
        else {
            m_pPlot->ellipse[m_nSelected].size = iSel + 1;
        }
    }
    if (m_bLineColor == TRUE) {
        clrTmp = m_cbLineColor.GetSelectedColor();
        if (m_nGeo == 0) {
            m_pPlot->rectangle[m_nSelected].color.r = GetRValue(clrTmp);
            m_pPlot->rectangle[m_nSelected].color.g = GetGValue(clrTmp);
            m_pPlot->rectangle[m_nSelected].color.b = GetBValue(clrTmp);
        }
        else {
            m_pPlot->ellipse[m_nSelected].color.r = GetRValue(clrTmp);
            m_pPlot->ellipse[m_nSelected].color.g = GetGValue(clrTmp);
            m_pPlot->ellipse[m_nSelected].color.b = GetBValue(clrTmp);
        }
        m_cbLineStyle.m_Color = clrTmp;
        m_cbLineStyle.Invalidate();
        m_bLineColor = FALSE;
    }
    if (m_bFillColor == TRUE) {
        clrTmp = m_cbFillColor.GetSelectedColor();
        if (m_nGeo == 0) {
            m_pPlot->rectangle[m_nSelected].fcolor.r = GetRValue(clrTmp);
            m_pPlot->rectangle[m_nSelected].fcolor.g = GetGValue(clrTmp);
            m_pPlot->rectangle[m_nSelected].fcolor.b = GetBValue(clrTmp);
        }
        else {
            m_pPlot->ellipse[m_nSelected].fcolor.r = GetRValue(clrTmp);
            m_pPlot->ellipse[m_nSelected].fcolor.g = GetGValue(clrTmp);
            m_pPlot->ellipse[m_nSelected].fcolor.b = GetBValue(clrTmp);
        }
        m_bFillColor = FALSE;
    }
    if (m_bLineStyle == TRUE) {
        if (m_nGeo == 0) {
            m_pPlot->rectangle[m_nSelected].style &= 0x0F;
            m_pPlot->rectangle[m_nSelected].style |= (m_cbLineStyle.GetSelectedStyle() & 0xF0);
        }
        else {
            m_pPlot->ellipse[m_nSelected].style &= 0x0F;
            m_pPlot->ellipse[m_nSelected].style |= (m_cbLineStyle.GetSelectedStyle() & 0xF0);
        }
        m_bLineStyle = FALSE;
    }

    UpdateData(TRUE);
    if (m_nGeo == 0) {
        if (m_bStyle == TRUE) {
            m_pPlot->rectangle[m_nSelected].style |= 0x01;
        }
        else {
            m_pPlot->rectangle[m_nSelected].style &= 0xF0;
        }
    }
    else {
        if (m_bStyle == TRUE) {
            m_pPlot->ellipse[m_nSelected].style |= 0x01;
        }
        else {
            m_pPlot->ellipse[m_nSelected].style &= 0xF0;
        }
    }

    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}
