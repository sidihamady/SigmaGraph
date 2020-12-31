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
#include "ComboBoxDot.h"

#include "MainFrm.h"

#include <gdiplus.h>
using namespace Gdiplus;

const char_t* CComboBoxDot:: DOTSTYLE_NAME[] =
{
    _T("None"), _T("Dot"), _T("Plus"), _T("Minus"),
    _T("Bar"), _T("Circle"), _T("Circle"), _T("Square"),
    _T("Square"), _T("Diamond"), _T("Diamond"), _T("Triangle Up"),
    _T("Triangle Up"), _T("Triangle Down"), _T("Triangle Down"), _T("Triangle Left"),
    _T("Triangle Left"), _T("Triangle Right"), _T("Triangle Right"), _T("")
};

const byte_t CComboBoxDot::DOTSTYLE_VALUE[] =
{
    0x00, 0x02, 0x04, 0x06,
    0x08, 0x10, 0x11, 0x20,
    0x21, 0x30, 0x31, 0x40,
    0x41, 0x50, 0x51, 0x60,
    0x61, 0x70, 0x71, 0x00
};

// CComboBoxDot

IMPLEMENT_DYNAMIC(CComboBoxDot, CComboBox)

CComboBoxDot::CComboBoxDot()
{
    m_bInitialized = FALSE;
}

CComboBoxDot::~CComboBoxDot()
{

}

BEGIN_MESSAGE_MAP(CComboBoxDot, CComboBox)
    ON_WM_CREATE()
END_MESSAGE_MAP()

int_t CComboBoxDot::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CComboBox::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    m_Color = RGB(0,0,0);

    InitStyles();

    return 0;
}

void CComboBoxDot::PreSubclassWindow()
{
    ModifyStyle(0, CBS_OWNERDRAWVARIABLE);

    InitStyles();

    CComboBox::PreSubclassWindow();
}

void CComboBoxDot::InitStyles()
{
    int_t nItem;

    if (m_bInitialized) {
        return;
    }

    // >> Increase performance
    InitStorage(DOTSTYLE_MAXITEMS, ML_STRSIZET * sizeof(char_t));
    // <<

    SetRedraw(FALSE);

    for (int_t nStyle = 0; nStyle < DOTSTYLE_MAXITEMS; nStyle++) {
        // Here we could affect the sort order at run-time
        nItem = AddString((LPCTSTR) (DOTSTYLE_NAME[nStyle]));
        if (nItem == CB_ERRSPACE) {
            break;
        }
        SetItemData(nItem, DOTSTYLE_VALUE[nStyle]);
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

void CComboBoxDot::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if ((lpDrawItemStruct->itemID < 0) || (lpDrawItemStruct->itemID >= DOTSTYLE_MAXITEMS)) {
        return;
    }

    CDC dc;
    if (!dc.Attach(lpDrawItemStruct->hDC)) {
        return;
    }

    RECT *pRect = &(lpDrawItemStruct->rcItem);
    if (pRect == NULL) {
        dc.Detach();
        return;
    }

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

    byte_t bStyle = DOTSTYLE_VALUE[lpDrawItemStruct->itemID];

    int_t nx = pRect->left, ny = pRect->top, nw = pRect->right - pRect->left, nh = pRect->bottom - pRect->top;
    int_t nx0 = nx + (nw / 2), ny0 = ny + (nh / 2), nd = nw / 4;

    if (nw > nh) {
        nd = nh / 4;
    }

    Gdiplus::Point ptTT[4];
    Graphics graphicsT(dc.GetSafeHdc());
    graphicsT.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen penT(Gdiplus::Color(GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color)));
    penT.SetWidth(1);
    Gdiplus::SolidBrush brushT(Gdiplus::Color(GetRValue(m_Color), GetGValue(m_Color), GetBValue(m_Color)));

    switch (bStyle) {
        case 0x02:    // .
            graphicsT.FillEllipse(&brushT, nx0 - 2, ny0 - 2, 4, 4);
            break;
        case 0x04:    // +
            graphicsT.DrawLine(&penT, nx0 - nd, ny0, nx0 + nd, ny0);
            graphicsT.DrawLine(&penT, nx0, ny0 - nd, nx0, ny0 + nd);
            break;
        case 0x06:    // -
            graphicsT.DrawLine(&penT, nx0 - nd, ny0, nx0 + nd, ny0);
            break;
        case 0x08:    // |
            graphicsT.DrawLine(&penT, nx0, ny0 - nd, nx0, ny0 + nd);
            break;
        case 0x10:    // Circle
            graphicsT.DrawEllipse(&penT, nx0 - nd, ny0 - nd, 2 * nd, 2 * nd);
            break;
        case 0x11:
            graphicsT.FillEllipse(&brushT, nx0 - nd, ny0 - nd, 2 * nd, 2 * nd);
            break;
        case 0x20:    // Square
            graphicsT.DrawRectangle(&penT, nx0 - nd, ny0 - nd, 2 * nd, 2 * nd);
            break;
        case 0x21:
            graphicsT.FillRectangle(&brushT, nx0 - nd, ny0 - nd, 2 * nd, 2 * nd);
            break;
        case 0x30:    // Trapeze
        case 0x31:
            ptTT[0].X = nx0 - nd;        ptTT[0].Y = ny0;
            ptTT[1].X = nx0;             ptTT[1].Y = ny0 + nd;
            ptTT[2].X = nx0 + nd;        ptTT[2].Y = ny0;
            ptTT[3].X = nx0;             ptTT[3].Y = ny0 - nd - 1;
            if (bStyle == 0x30) {
                graphicsT.DrawPolygon(&penT, ptTT, 4);
            }
            else {
                graphicsT.FillPolygon(&brushT, ptTT, 4);
            }
            break;
        case 0x40:    // Triangle up
        case 0x41:
            ptTT[0].X = nx0 - nd;        ptTT[0].Y = ny0 + nd;
            ptTT[1].X = nx0 + nd;        ptTT[1].Y = ny0 + nd;
            ptTT[2].X = nx0;             ptTT[2].Y = ny0 - nd;
            if (bStyle == 0x40) {
                graphicsT.DrawPolygon(&penT, ptTT, 3);
            }
            else {
                graphicsT.FillPolygon(&brushT, ptTT, 3);
            }
            break;
        case 0x50:    // Triangle down
        case 0x51:
            ptTT[0].X = nx0 - nd;        ptTT[0].Y = ny0 - nd;
            ptTT[1].X = nx0 + nd;        ptTT[1].Y = ny0 - nd;
            ptTT[2].X = nx0;             ptTT[2].Y = ny0 + nd;
            if (bStyle == 0x50) {
                graphicsT.DrawPolygon(&penT, ptTT, 3);
            }
            else {
                graphicsT.FillPolygon(&brushT, ptTT, 3);
            }
            break;
        case 0x60:    // Triangle left
        case 0x61:
            ptTT[0].X = nx0 - nd;        ptTT[0].Y = ny0;
            ptTT[1].X = nx0 + nd;        ptTT[1].Y = ny0 + nd;
            ptTT[2].X = nx0 + nd;        ptTT[2].Y = ny0 - nd;
            if (bStyle == 0x60) {
                graphicsT.DrawPolygon(&penT, ptTT, 3);
            }
            else {
                graphicsT.FillPolygon(&brushT, ptTT, 3);
            }
            break;
        case 0x70:    // Triangle right
        case 0x71:
            ptTT[0].X = nx0 - nd;        ptTT[0].Y = ny0 + nd;
            ptTT[1].X = nx0 - nd;        ptTT[1].Y = ny0 - nd;
            ptTT[2].X = nx0 + nd;        ptTT[2].Y = ny0;
            if (bStyle == 0x70) {
                graphicsT.DrawPolygon(&penT, ptTT, 3);
            }
            else {
                graphicsT.FillPolygon(&brushT, ptTT, 3);
            }
            break;
        default:
            break;
    }

    // Restore the DC state
    dc.SetBkColor(crOldBkColor);
    dc.Detach();
}

byte_t CComboBoxDot::GetSelectedStyle()
{
    int_t iSelectedItem = GetCurSel();                    // Get Selected Item

    if (iSelectedItem == CB_ERR) {                        // If Nothing Selected
        return 0x00;                                      // Return no symbol
    }

    return ((byte_t)GetItemData(iSelectedItem));          // Return Selected Color
}

void CComboBoxDot::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
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
