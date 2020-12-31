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
#include "ComboBoxLine.h"

#include "MainFrm.h"


const char_t* CComboBoxLine::LINESTYLE_NAME[] =
{
    _T("Solid"),
    _T("Dash"),
    _T("Dot"),
    _T("Dash Dot"),
    _T("Dash Dot Dot"),
    _T("")
};

const byte_t CComboBoxLine::LINESTYLE_VALUE[] =
{
    0x10,
    0x20,
    0x30,
    0x40,
    0x50,
    0x00
};

// CComboBoxLine

IMPLEMENT_DYNAMIC(CComboBoxLine, CComboBox)

CComboBoxLine::CComboBoxLine()
{
    m_bInitialized = FALSE;
}

CComboBoxLine::~CComboBoxLine()
{

}


BEGIN_MESSAGE_MAP(CComboBoxLine, CComboBox)
    ON_WM_CREATE()
END_MESSAGE_MAP()

int_t CComboBoxLine::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CComboBox::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    m_Color = RGB(0,0,0);

    InitStyles();

    return 0;
}

void CComboBoxLine::PreSubclassWindow()
{
    ModifyStyle(0, CBS_OWNERDRAWVARIABLE);

    InitStyles();

    CComboBox::PreSubclassWindow();
}

void CComboBoxLine::InitStyles()
{
    int_t nItem;

    if (m_bInitialized) {
        return;
    }

    // >> Increase performance
    InitStorage(LINESTYLE_MAXITEMS, ML_STRSIZET * sizeof(char_t));
    // <<

    SetRedraw(FALSE);

    for (int_t nStyle = 0; nStyle < LINESTYLE_MAXITEMS; nStyle++) {
        // Here we could affect the sort order at run-time
        nItem = AddString((LPCTSTR) (LINESTYLE_NAME[nStyle]));
        if (nItem == CB_ERRSPACE) {
            break;
        }
        SetItemData(nItem, CComboBoxLine::LINESTYLE_VALUE[nStyle]);
    }

    CRect recTT;
    int_t nHeight, nLines = 8;

    GetClientRect(recTT);
    nHeight = GetItemHeight(-1);
    nHeight += GetItemHeight(0) * nLines;
    nHeight += GetSystemMetrics(SM_CYEDGE) * 2;
    nHeight += GetSystemMetrics(SM_CYEDGE) * 2;
    SetWindowPos(NULL, 0, 0, recTT.right, nHeight, SWP_NOMOVE | SWP_NOZORDER);

    SetRedraw(TRUE);

    m_bInitialized = TRUE;
}

void CComboBoxLine::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if ((lpDrawItemStruct->itemID < 0) || (lpDrawItemStruct->itemID >= LINESTYLE_MAXITEMS)) {
        return;
    }

    CDC dc;
    if (!dc.Attach(lpDrawItemStruct->hDC)) {
        return;
    }

    COLORREF crOldBkColor = dc.GetBkColor();
    dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
    dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_WINDOW));

    Gdiplus::Graphics graphicsT(dc.GetSafeHdc());
    graphicsT.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen penT(Gdiplus::Color(GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color)));
    penT.SetWidth(2);

    byte_t bStyle = CComboBoxLine::LINESTYLE_VALUE[lpDrawItemStruct->itemID];
    switch (bStyle) {
        case 0x10:
            penT.SetDashStyle(Gdiplus::DashStyleSolid);
            break;
        case 0x20:
            penT.SetDashStyle(Gdiplus::DashStyleDash);
            break;
        case 0x30:
            penT.SetDashStyle(Gdiplus::DashStyleDot);
            break;
        case 0x40:
            penT.SetDashStyle(Gdiplus::DashStyleDashDot);
            break;
        case 0x50:
            penT.SetDashStyle(Gdiplus::DashStyleDashDotDot);
            break;
        default:
            penT.SetDashStyle(Gdiplus::DashStyleCustom);
            break;
    }

    RECT *pRect = &(lpDrawItemStruct->rcItem);
    if (pRect == NULL) {
        dc.Detach();
        return;
    }

    if (lpDrawItemStruct->itemAction & ODA_FOCUS) {
        dc.DrawFocusRect(pRect);
    }
    else if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE) {
        if (lpDrawItemStruct->itemState & ODS_FOCUS) {
            dc.DrawFocusRect(pRect);
        }
        else {
            dc.ExtTextOut(0, 0, ETO_OPAQUE, pRect, _T(""), 0, NULL);
        }
    }

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

    if ((bStyle != 0x00)  && ((int_t) (lpDrawItemStruct->itemID) >= 0) && ((int_t) (lpDrawItemStruct->itemID) < LINESTYLE_MAXITEMS)) {    // Any item selected?
        graphicsT.DrawLine(&penT, pRect->left + 2, (pRect->top + pRect->bottom) / 2, pRect->right - 2, (pRect->top + pRect->bottom) / 2);
    }

    // Restore the DC state
    dc.SetBkColor(crOldBkColor);
    dc.Detach();
}

byte_t CComboBoxLine::GetSelectedStyle()
{
    int_t iSelectedItem = GetCurSel();                     // Get Selected Item

    if (iSelectedItem == CB_ERR ) {                        // If Nothing Selected
        SetCurSel(0);
        return 0x10;                                       // Return cont.line
    }

    return ((byte_t)GetItemData(iSelectedItem));           // Return Selected Color

}

void CComboBoxLine::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    ASSERT(lpMeasureItemStruct->CtlType == ODT_COMBOBOX);

    if (lpMeasureItemStruct->CtlType == ODT_COMBOBOX) {
        return;
    }

    lpMeasureItemStruct->itemHeight = 20;

    if (lpMeasureItemStruct->itemID != (UINT)-1) {
        LPCTSTR lpszText = (LPCTSTR)(lpMeasureItemStruct->itemData);
        ASSERT(lpszText != NULL);
        if (lpszText) {
            CSize szT;
            CDC* pDC = GetDC();
            if (pDC) {
                szT = pDC->GetTextExtent(lpszText);
                ReleaseDC(pDC);
                pDC = NULL;
                if (szT.cy > 10) {
                    lpMeasureItemStruct->itemHeight = szT.cy;
                }
            }
        }
   }
}
