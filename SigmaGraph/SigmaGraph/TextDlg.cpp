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
#include "TextDlg.h"
#include "SigmaDoc.h"


// CTextDlg dialog

IMPLEMENT_DYNAMIC(CTextDlg, CDialog)

CTextDlg::CTextDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CTextDlg::IDD, pParent)
    , m_bOrientHorz(TRUE)
    , m_bOrientVert(FALSE)
    , m_bOrientVert270(FALSE)
{
    m_pPlot = NULL;
    m_pDoc = NULL;
    m_bInitialized = FALSE;
    m_bIsModified = FALSE;
    m_recFrame.bottom = m_recFrame.top = m_recFrame.left = m_recFrame.right = 0;
    m_ptInit.x = m_ptInit.y = 0;
    m_bOK = FALSE;
    m_nSelectedText = -1;
    m_bModifyText = FALSE;
    m_bReadOnly = FALSE;
}

CTextDlg::~CTextDlg()
{

}

void CTextDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDE_INPUT, m_strInput);
    DDX_Text(pDX, IDL_INPUT, m_strLabel);
    DDX_Check(pDX, IDO_TEXT_ORIENT00, m_bOrientHorz);
    DDX_Check(pDX, IDO_TEXT_ORIENT90, m_bOrientVert);
    DDX_Check(pDX, IDO_TEXT_ORIENT270, m_bOrientVert270);
    DDX_Control(pDX, IDC_INPUT_FONTCOLOR, m_cbTextColor);
    DDX_Control(pDX, IDE_INPUT, m_edtInput);
    DDX_Control(pDX, IDO_TEXT_ORIENT00, m_bnOrientHorz);
    DDX_Control(pDX, IDO_TEXT_ORIENT90, m_bnOrientVert);
    DDX_Control(pDX, IDO_TEXT_ORIENT270, m_bnOrientVert270);
}


BEGIN_MESSAGE_MAP(CTextDlg, CDialog)
    ON_EN_CHANGE(IDE_INPUT, &CTextDlg::OnEditInput)
    ON_CBN_SELCHANGE(IDC_INPUT_FONTCOLOR, &CTextDlg::OnCbnSelchangeTextcolor)
    ON_BN_CLICKED(IDO_TEXT_ORIENT00, &CTextDlg::OnOrient00)
    ON_BN_CLICKED(IDO_TEXT_ORIENT90, &CTextDlg::OnOrient90)
    ON_BN_CLICKED(IDO_TEXT_ORIENT270, &CTextDlg::OnOrient270)
    ON_BN_CLICKED(ID_INPUT_FONTCHOOSE, &CTextDlg::OnTextFont)
    ON_BN_CLICKED(IDOK, &CTextDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CTextDlg::OnCancel)
END_MESSAGE_MAP()


// CTextDlg message handlers

BOOL CTextDlg::OnInitDialog() 
{
    CRect rcCombo;

    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_TEXT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        CDialog::EndDialog(-1);
        return FALSE;
    }
    if ((m_pPlot->textcount < 0) || (m_pPlot->textcount >= PL_MAXITEMS)
        || (m_ptInit.x < m_recFrame.left) || (m_ptInit.x > m_recFrame.right)
        || (m_ptInit.y < m_recFrame.top) || (m_ptInit.y > m_recFrame.bottom)
        || (m_recFrame.bottom == m_recFrame.top)
        || (m_recFrame.left == m_recFrame.right)) {
        CDialog::EndDialog(-1);
        return FALSE;
    }
    if (m_nSelectedText >= m_pPlot->textcount) {
        CDialog::EndDialog(-1);
        return FALSE;
    }
    if (m_nSelectedText < 0) {
        m_nSelectedText = m_pPlot->textcount;
        m_strInput = _T("");
    }
    else {
        m_bModifyText = TRUE;
        // Enlever l'espace rajouté systématiquement à la fin de la chaîne de caractères
        m_strInput = m_pPlot->text[m_nSelectedText].text;
        int_t nlen = _tcslen((const char_t*)(m_pPlot->text[m_nSelectedText].text));
        if ((nlen > 0) && (nlen < (ML_STRSIZE - 1))) {
            m_strInput.TrimRight();
        }
    }

    POINT ptT;
    ptT.x = m_ptInit.x;
    ptT.y = m_ptInit.y;
    RECT rcT = {0,0,0,0};
    GetWindowRect(&rcT);
    ClientToScreen(&rcT);
    LONG ww = rcT.right - rcT.left, hh = rcT.bottom - rcT.top;
    if ((m_ptInit.x + ww) > m_recFrame.right) {
        ptT.x = m_recFrame.right - ww;
    }
    if ((m_ptInit.y + hh) > m_recFrame.bottom) {
        ptT.y = m_recFrame.bottom - hh;
    }
    SetWindowPos(NULL, ptT.x, ptT.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    CRect recTT;
    int_t nHeight, nLines = 8;
    m_cbTextColor.GetClientRect(recTT);
    nHeight = m_cbTextColor.GetItemHeight(-1);
    nHeight += m_cbTextColor.GetItemHeight(0) * nLines;
    nHeight += GetSystemMetrics(SM_CYEDGE) * 2;
    nHeight += GetSystemMetrics(SM_CYEDGE) * 2;
    m_cbTextColor.SetWindowPos(NULL, 0, 0, recTT.right, nHeight, SWP_NOMOVE | SWP_NOZORDER);

    ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT, EM_LIMITTEXT, (WPARAM)(ML_STRSIZE - 7), (LPARAM)0);

    m_bOrientHorz = (m_pPlot->text[m_nSelectedText].font.orient == 0x00);
    m_bOrientVert = (m_pPlot->text[m_nSelectedText].font.orient == 0x09);
    m_bOrientVert270 = (m_pPlot->text[m_nSelectedText].font.orient == 0x27);

    if (m_bReadOnly == TRUE) {
        m_edtInput.EnableWindow(FALSE);
        m_bnOrientHorz.EnableWindow(FALSE);
        m_bnOrientVert.EnableWindow(FALSE);
        m_bnOrientVert270.EnableWindow(FALSE);
    }

    m_cbTextColor.SetSelectedColor(RGB(m_pPlot->text[m_nSelectedText].font.color.r, m_pPlot->text[m_nSelectedText].font.color.g, m_pPlot->text[m_nSelectedText].font.color.b));

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextDlg::OnCbnSelchangeTextcolor()
{
    COLORREF clrTmp = m_cbTextColor.GetSelectedColor();
    m_pPlot->text[m_nSelectedText].font.color.r = GetRValue(clrTmp);
    m_pPlot->text[m_nSelectedText].font.color.g = GetGValue(clrTmp);
    m_pPlot->text[m_nSelectedText].font.color.b = GetBValue(clrTmp);
    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
}

void CTextDlg::OnEditInput()
{
    m_bIsModified = TRUE;
}

void CTextDlg::OnOrient00()
{
    if (m_bReadOnly == TRUE) {
        return;
    }

    UpdateData(TRUE);

    byte_t bt = m_pPlot->text[m_nSelectedText].font.orient;
    if (m_bOrientHorz == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x00;
    }
    else if (m_bOrientVert == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x09;
    }
    else if (m_bOrientVert270 == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x27;
    }
    if (bt != m_pPlot->text[m_nSelectedText].font.orient) {
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }

    m_bIsModified = TRUE;
}

void CTextDlg::OnOrient90()
{
    if (m_bReadOnly == TRUE) {
        return;
    }

    UpdateData(TRUE);

    byte_t bt = m_pPlot->text[m_nSelectedText].font.orient;
    if (m_bOrientHorz == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x00;
    }
    else if (m_bOrientVert == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x09;
    }
    else if (m_bOrientVert270 == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x27;
    }
    if (bt != m_pPlot->text[m_nSelectedText].font.orient) {
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }

    m_bIsModified = TRUE;
}

void CTextDlg::OnOrient270()
{
    if (m_bReadOnly == TRUE) {
        return;
    }

    UpdateData(TRUE);

    byte_t bt = m_pPlot->text[m_nSelectedText].font.orient;
    if (m_bOrientHorz == TRUE) {
        m_pPlot->text[m_nSelectedText].font.orient = 0x00;
    }
    else if (m_bOrientVert == TRUE) {
        m_pPlot->text[m_nSelectedText].font.orient = 0x09;
    }
    else if (m_bOrientVert270 == TRUE) {
        m_pPlot->text[m_nSelectedText].font.orient = 0x27;
    }
    if (bt != m_pPlot->text[m_nSelectedText].font.orient) {
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }

    m_bIsModified = TRUE;
}

void CTextDlg::OnTextFont()
{
    LOGFONT lft;

    UpdateData(TRUE);    // Copy data to variables

    memset(&lft, 0, sizeof(lft));

    if (SigmaApp.FontToLogfont(m_pPlot->text[m_nSelectedText].font, &lft) == FALSE) {
        return;
    }

    if (SigmaApp.GetFont(m_hWnd, &lft)) {
        SigmaApp.LogfontToFont(lft, &(m_pPlot->text[m_nSelectedText].font));
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }
}

void CTextDlg::OnOK()
{
    if (m_bReadOnly == TRUE) {
        m_bOK = TRUE;
        CDialog::OnOK();
        return;
    }

    UpdateData(TRUE);

    int_t nlen = SigmaApp.CStringToChar(m_strInput, m_pPlot->text[m_nSelectedText].text, ML_STRSIZE - 2);
    if ((nlen < 1) || (nlen >= (ML_STRSIZE - 1))) {
        if ((m_bModifyText == TRUE) && (nlen < 1)) {    // Supprimer le texte
            m_pPlot->text[m_nSelectedText].len = 0;
            memset(m_pPlot->text[m_nSelectedText].text, 0, ML_STRSIZE);
            memset(&(m_pPlot->text[m_nSelectedText].rect), 0, sizeof(rect_t));
            m_pPlot->textcount -= 1;
        }
        m_bOK = TRUE;
        CDialog::OnOK();
        return;
    }

    if (m_bOrientHorz == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x00;
    }
    else if (m_bOrientVert == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x09;
    }
    else if (m_bOrientVert270 == TRUE){
        m_pPlot->text[m_nSelectedText].font.orient = 0x27;
    }

    if (m_bModifyText == FALSE) {    // Nouveau texte
        real_t ww = m_pPlot->plint.width, hh = m_pPlot->plint.height;
        if ((ww < 100.0) || (hh < 100.0)) {
            CDialog::OnOK();
            return;
        }

        RECT recText;
        recText.left = m_ptInit.x;                            recText.top = m_ptInit.y;
        recText.right = recText.left + (nlen * 4);    recText.bottom = recText.top + 8;
        CDC* pDC = GetDC();
        if (pDC) {
            SIZE sizeText;
            if (::GetTextExtentPoint32(pDC->GetSafeHdc(), m_pPlot->text[m_nSelectedText].text, nlen, &sizeText)) {
                recText.right = recText.left + sizeText.cx;
                recText.bottom = recText.top + sizeText.cy;
            }
            ReleaseDC(pDC);
            pDC = NULL;
        }
        m_pPlot->text[m_nSelectedText].rect.xa = ((real_t)(recText.left - m_recFrame.left)) / ww;
        m_pPlot->text[m_nSelectedText].rect.xb = ((real_t)(recText.right - m_recFrame.left)) / ww;
        m_pPlot->text[m_nSelectedText].rect.ya = ((real_t)(recText.top - m_recFrame.top)) / hh;
        m_pPlot->text[m_nSelectedText].rect.yb = ((real_t)(recText.bottom - m_recFrame.top)) / hh;
        m_pPlot->text[m_nSelectedText].status &= 0xF0;
        m_pPlot->text[m_nSelectedText].id = 0;
        m_pPlot->textcount += 1;
    }

    ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE);

    m_bOK = TRUE;
    CDialog::OnOK();
}

void CTextDlg::OnCancel()
{
    CDialog::OnCancel();
}
