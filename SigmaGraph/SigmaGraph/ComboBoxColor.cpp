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
#include "ComboBoxColor.h"

#include "MainFrm.h"


// CComboBoxColor

IMPLEMENT_DYNAMIC(CComboBoxColor, CComboBox)

const char_t* CComboBoxColor::COLOR_NAME[] =
{
    _T("Black"),
    _T("White"),
    _T("Red"),
    _T("Green"),
    _T("Blue"),
    _T("Yellow"),
    _T("Navy"),
    _T("Maroon"),
    _T("Olive"),
    _T("Purple"),
    _T("Teal"),
    _T("Grey"),
    _T("Silver"),
    _T("Lime"),
    _T("Magenta"),
    _T("Aqua"),
    _T("Custom")
};

const COLORREF CComboBoxColor::COLOR_VALUE[] =
{
    RGB(0x00, 0x00, 0x00),
    RGB(0xFF, 0xFF, 0xFF),
    RGB(0xFF, 0x00, 0x00),
    RGB(0x00, 0x80, 0x00),
    RGB(0x00, 0x00, 0xFF),
    RGB(0xFF, 0xFF, 0x00),
    RGB(0x00, 0x00, 0x80),
    RGB(0x80, 0x00, 0x00),
    RGB(0x00, 0x80, 0x80),
    RGB(0x80, 0x00, 0x80),
    RGB(0x80, 0x80, 0x00),
    RGB(0x80, 0x80, 0x80),
    RGB(0xC0, 0xC0, 0xC0),
    RGB(0x00, 0xFF, 0x00),
    RGB(0xFF, 0x00, 0xFF),
    RGB(0x00, 0xFF, 0xFF),
    RGB(0x40, 0x40, 0x40),
    RGB(0x40, 0x40, 0x40)
};

CComboBoxColor::CComboBoxColor()
{
    int_t jj;
    for (jj = 0; jj < (COLORS_MAXITEMS + 2); jj++) {
        m_Colors[jj] = COLOR_VALUE[jj];
    }

    m_bInitialized = FALSE;
}

CComboBoxColor::~CComboBoxColor()
{

}

BEGIN_MESSAGE_MAP(CComboBoxColor, CComboBox)
    ON_WM_CREATE()
    ON_CONTROL_REFLECT(CBN_SELENDOK, &CComboBoxColor::OnCbnSelendok)
END_MESSAGE_MAP()

int_t CComboBoxColor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CComboBox::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    InitColors();

    return 0;
}

void CComboBoxColor::PreSubclassWindow()
{
    ModifyStyle(0, CBS_OWNERDRAWVARIABLE);

    InitColors();

    CComboBox::PreSubclassWindow();
}

void CComboBoxColor::InitColors()
{
    int_t nItem;

    if (m_bInitialized) {
        return;
    }

    // >> Increase performance
    InitStorage(COLORS_MAXITEMS + 2, (COLORS_MAXITEMS + 1) * sizeof(char_t));
    // <<

    SetRedraw(FALSE);

    for (int_t nColor = 0; nColor < (COLORS_MAXITEMS + 1); nColor++) {
        // Here we could affect the sort order at run-time
        nItem = AddString((LPCTSTR) (COLOR_NAME[nColor]));
        if (nItem == CB_ERRSPACE) {
            break;
        }
        SetItemData(nItem, m_Colors[nColor]);
    }

    nItem = AddString(_T("..."));
    if (nItem != CB_ERRSPACE) {
        SetItemData(nItem, RGB(0x00, 0x00, 0x00));
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

void CComboBoxColor::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC dc;
    CBrush brushBlack;
    brushBlack.CreateStockObject(GRAY_BRUSH);

    if (!dc.Attach(lpDrawItemStruct->hDC)) {
        brushBlack.DeleteObject();
        return;
    }

    COLORREF rgbTextColor = dc.GetTextColor();
    COLORREF rgbBkColor = dc.GetBkColor();

    RECT *pRect = &(lpDrawItemStruct->rcItem);
    if (pRect == NULL) {
        dc.SetTextColor(rgbTextColor);
        dc.SetBkColor(rgbBkColor);
        dc.Detach();
        brushBlack.DeleteObject();
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

    if ((int_t) (lpDrawItemStruct->itemID) >= 0 && (int_t) (lpDrawItemStruct->itemID) <= COLORS_MAXITEMS) {    // Any item selected?
        ::InflateRect(pRect, -2, -2);
        dc.FillSolidRect(pRect, (COLORREF)lpDrawItemStruct->itemData);
        dc.FrameRect(pRect, &brushBlack);
        CString strTT = _T("Color: ");
        strTT += COLOR_NAME[lpDrawItemStruct->itemID];
        pFrame->SetMessageText(strTT);
    }
    else if ((int_t) (lpDrawItemStruct->itemID) == (COLORS_MAXITEMS + 1)) {    // Any item selected?
        ::InflateRect(pRect, -2, -2);
        dc.DrawText(_T("..."), pRect, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
        pFrame->SetMessageText(_T(""));
    }
    else {
        pFrame->SetMessageText(_T(""));
    }

    // Restore the DC state
    dc.SetTextColor(rgbTextColor);
    dc.SetBkColor(rgbBkColor);
    dc.Detach();
    brushBlack.DeleteObject();
}

COLORREF CComboBoxColor::GetSelectedColor()
{
    int_t iSelectedItem = GetCurSel();                    // Get Selected Item

    if (iSelectedItem == CB_ERR) {                        // If Nothing Selected
        return (RGB(0, 0, 0));                            // Return Black
    }

    return (GetItemData(iSelectedItem));                  // Return Selected Color

}
void CComboBoxColor::OnCbnSelendok()
{
    int_t iSelectedItem = GetCurSel();                    // Get Selected Item

    if (iSelectedItem != (COLORS_MAXITEMS + 1)) {         // If Custom Not Selected
        return;                                           // Return
    }

    COLORREF clr = GetItemData(iSelectedItem - 1);
    if (SigmaApp.GetColor(m_hWnd, &clr)) {
        m_Colors[iSelectedItem - 1] = clr;
        SetItemData(iSelectedItem - 1, clr);
        SetCurSel(iSelectedItem - 1);
    }
}

void CComboBoxColor::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
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
