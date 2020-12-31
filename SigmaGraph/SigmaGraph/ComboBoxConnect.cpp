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
#include "ComboBoxConnect.h"

#include "MainFrm.h"

#include <gdiplus.h>
using namespace Gdiplus;

const char_t* CComboBoxConnect::CONNECTSTYLE_NAME[] =
{
    _T("Straight"),
    _T("Step Horz"),
    _T("Step Vert"),
    _T("Step Horz Center"),
    _T("Step Vert Center")
};

const byte_t CComboBoxConnect::CONNECTSTYLE_VALUE[] =
{
    0x01,
    0x02,
    0x03,
    0x04,
    0x05
};

// CComboBoxConnect

IMPLEMENT_DYNAMIC(CComboBoxConnect, CComboBox)

CComboBoxConnect::CComboBoxConnect()
{
    m_bInitialized = FALSE;
}

CComboBoxConnect::~CComboBoxConnect()
{

}

BEGIN_MESSAGE_MAP(CComboBoxConnect, CComboBox)
    ON_WM_CREATE()
END_MESSAGE_MAP()

int_t CComboBoxConnect::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CComboBox::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    m_Color = RGB(0,0,0);

    InitStyles();

    return 0;
}

void CComboBoxConnect::PreSubclassWindow()
{
    ModifyStyle(0, CBS_OWNERDRAWVARIABLE);

    InitStyles();

    CComboBox::PreSubclassWindow();
}

void CComboBoxConnect::InitStyles()
{
    int_t nItem;

    if (m_bInitialized) {
        return;
    }

    // >> Increase performance
    InitStorage(CONNECTSTYLE_MAXITEMS, ML_STRSIZET * sizeof(char_t));
    // <<

    SetRedraw(FALSE);

    for (int_t nStyle = 0; nStyle < CONNECTSTYLE_MAXITEMS; nStyle++) {
        // Here we could affect the sort order at run-time
        nItem = AddString((LPCTSTR) (CONNECTSTYLE_NAME[nStyle]));
        if (nItem == CB_ERRSPACE) {
            break;
        }
        SetItemData(nItem, CONNECTSTYLE_VALUE[nStyle]);
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

void CComboBoxConnect::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if ((lpDrawItemStruct->itemID < 0) || (lpDrawItemStruct->itemID >= CONNECTSTYLE_MAXITEMS)) {
        return;
    }

    CDC dc;
    if (!dc.Attach(lpDrawItemStruct->hDC)) {
        return;
    }

    HDC hDCX = dc.GetSafeHdc();

    RECT *pRect = &(lpDrawItemStruct->rcItem);
    if (pRect == NULL) {
        dc.Detach();
        return;
    }

    int_t nx = pRect->left, ny = pRect->top, nw = pRect->right - pRect->left, nh = pRect->bottom - pRect->top;
    int_t nx0 = nx + (nw / 2), ny0 = ny + (nh / 2), nd = nw / 4, ndx = nw / 4;

    COLORREF crOldBkColor = dc.GetBkColor();
    dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
    dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_WINDOW));

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

    byte_t bStyle = CONNECTSTYLE_VALUE[lpDrawItemStruct->itemID];

    if (nw > nh) {
        nd = nh / 4;
    }

    Graphics graphicsT(hDCX);
    graphicsT.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen penT(Color(GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color)));
    penT.SetWidth(1);

    switch (bStyle) {
        case 0x01:
            graphicsT.DrawLine(&penT, nx0 + ndx, ny + nd, nx0 - ndx, ny + nh - nd);
            graphicsT.DrawRectangle(&penT, nx0 + ndx - 2, ny + nd - 2, nd, nd);
            break;
        case 0x02:
            graphicsT.DrawLine(&penT, nx0 - ndx, ny + nh - nd, nx0 + ndx, ny + nh - nd);
            graphicsT.DrawLine(&penT, nx0 + ndx, ny + nh - nd, nx0 + ndx, ny + nd);
            graphicsT.DrawRectangle(&penT, nx0 + ndx - 2, ny + nd - 2, nd, nd);
            break;
        case 0x03:
            graphicsT.DrawLine(&penT, nx0 - ndx, ny + nh - nd, nx0 - ndx, ny + nd);
            graphicsT.DrawLine(&penT, nx0 - ndx, ny + nd, nx0 + ndx, ny + nd);
            graphicsT.DrawRectangle(&penT, nx0 - ndx - 2, ny + nh - nd - 2, nd, nd);
            break;
        case 0x04:
            graphicsT.DrawLine(&penT, nx0 - ndx, ny + nh - nd, nx0 + ndx, ny + nh - nd);
            graphicsT.DrawLine(&penT, nx0 + ndx, ny + nh - nd, nx0 + ndx, ny + 2);
            graphicsT.DrawRectangle(&penT, nx0 - 2, ny + nh - nd - 2, nd, nd);
            break;
        case 0x05:
            graphicsT.DrawLine(&penT, nx0 - ndx, ny + nh - 2, nx0, ny + nh - 2);
            graphicsT.DrawLine(&penT, nx0, ny + nh - 2, nx0, ny + 1);
            graphicsT.DrawRectangle(&penT, nx0 - 2, ny0 - 2, nd, nd);
            break;
        default:
            break;
    }

    // Restore the DC state
    dc.SetBkColor(crOldBkColor);
    dc.Detach();
}

byte_t CComboBoxConnect::GetSelectedStyle()
{
    int_t iSelectedItem = GetCurSel();                     // Get Selected Item

    if (iSelectedItem == CB_ERR ) {                        // If Nothing Selected
        return 0x10;                                       // Return cont.line
    }

    return ((byte_t)GetItemData(iSelectedItem));           // Return Selected Color

}

void CComboBoxConnect::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
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
