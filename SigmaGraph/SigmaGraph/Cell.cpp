//////////////////////////////////////////////////////////////////////
// GridCell.cpp : implementation file
//
// MFC Grid Control - Main grid cell class
//
// Provides the implementation for the "default" cell type of the
// grid control. Adds in cell editing.
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright(C) 1998-2005. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.20+
//
// History:
// Eric Woodruff - 20 Feb 2000 - Added PrintCell() plus other minor changes
// Ken Bertelson - 12 Apr 2000 - Split CGridCell into CGridCell and CGridCellBase
// <kenbertelson@hotmail.com>
// C Maunder     - 17 Jun 2000 - Font handling optimsed, Added CGridDefaultCell
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Modifications:
//   Copyright(C) 1998-2007 Sidi OULD SAAD HAMADY
//   http://www.hamady.org
//   sidi@hamady.org
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Cell.h"
#include "InEdit.h"
#include "Datasheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCell, CObject)
IMPLEMENT_DYNCREATE(CCellDefault, CCell)

// Cell

CCell::CCell()
{
    m_plfFont = NULL;
    CCell::Reset();
}

CCell::~CCell()
{
    if (m_plfFont) { delete m_plfFont; }
}

// Cell Attributes

void CCell::operator=(const CCell& tCell)
{
    if (this == &tCell) { return; }

    SetDatasheet(tCell.GetDatasheet());                // do first in case of dependencies

    SetText(tCell.GetText());
    SetData(tCell.GetData());
    SetState(tCell.GetState());
    SetFormat(tCell.GetFormat());
    SetTextClr(tCell.GetTextClr());
    SetBackClr(tCell.GetBackClr());
    SetFont(tCell.IsDefaultFont() ? NULL : tCell.GetFont());
    SetMargin(tCell.GetMargin());
}

void CCell::Reset()
{
    m_nState = 0;

    m_crSelectedForeColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    m_crSelectedBackColor = ::GetSysColor(COLOR_HIGHLIGHT);
    m_crSelectedBorderColor = ::GetSysColor(COLOR_GRADIENTACTIVECAPTION);
    m_crSelectedBackColorHeader = ::GetSysColor(COLOR_3DLIGHT);

    memset(m_szText, 0, ML_STRSIZEN * sizeof(char_t));
    m_nImage = -1;
    m_lParam = NULL;
    m_pDatasheet = NULL;

    m_nFormat = DT_RIGHT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
    m_crBkClr = CLR_DEFAULT;                // Background colour (or CLR_DEFAULT)
    m_crFgClr = CLR_DEFAULT;                // Forground colour (or CLR_DEFAULT)
    m_nMargin = (UINT)-1;

    if (m_plfFont) {
        delete m_plfFont;
        m_plfFont = NULL;                        // Cell font
    }
}

void CCell::SetFont(const LOGFONT* plf)
{
    if (plf == NULL) {
        if (m_plfFont) { delete m_plfFont; }
        m_plfFont = NULL;
    }
    else {
        if (!m_plfFont) {
            m_plfFont = new LOGFONT;
        }
        if (m_plfFont) {
            memcpy(m_plfFont, plf, sizeof(LOGFONT));
        }
    }
}

LOGFONT* CCell::GetFont() const
{
    if (m_plfFont == NULL) {
        CCellDefault *pDefaultCell = (CCellDefault*) GetDefaultCell();
        if (!pDefaultCell) {
            return NULL;
        }
        return pDefaultCell->GetFont();
    }

    return m_plfFont; 
}

CFont* CCell::GetFontObject() const
{
    // If the default font is specified, use the default tCell implementation
    if (m_plfFont == NULL) {
        CCellDefault *pDefaultCell = (CCellDefault*) GetDefaultCell();
        if (!pDefaultCell) {
            return NULL;
        }
        return pDefaultCell->GetFontObject();
    }
    else {
        static CFont tFont;
        tFont.DeleteObject();
        tFont.CreateFontIndirect(m_plfFont);
        return &tFont;
    }
}

DWORD CCell::GetFormat() const
{
    if (m_nFormat == (DWORD)-1) {
        CCellDefault *pDefaultCell = (CCellDefault*) GetDefaultCell();
        if (!pDefaultCell) {
            return 0;
        }
        return pDefaultCell->GetFormat();
    }

    return m_nFormat; 
}

UINT CCell::GetMargin() const
{
    if (m_nMargin == (UINT)-1) {
        CCellDefault *pDefaultCell = (CCellDefault*) GetDefaultCell();
        if (!pDefaultCell) {
            return 0;
        }
        return pDefaultCell->GetMargin();
    }

    return m_nMargin; 
}

// Returns a pointer to a tCell that holds default values for this particular type of tCell
CCell* CCell::GetDefaultCell() const
{
    if (GetDatasheet()) {
        return GetDatasheet()->GetDefaultCell(IsFixedRow(), IsFixedCol());
    }
    return NULL;
}

// Return TRUE if you set the cursor
BOOL CCell::OnSetCursor()
{
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
    return TRUE;
}

BOOL CCell::GetTextRect(LPRECT pRect)  // i/o:  i=dims of tCell rcT; o=dims of text rcT
{
    return TRUE;
}

// By default this uses the selected font (which is a bigger font)
CSize CCell::GetTextExtent(LPCTSTR szText, CDC* pDC /*= NULL*/)
{
    CDatasheet* pDatasheet = GetDatasheet();
    ASSERT(pDatasheet);

    BOOL bReleaseDC = FALSE;
    if ((pDC == NULL) || (szText == NULL)) {
        if (szText) {
            pDC = pDatasheet->GetDC();
        }
        if ((pDC == NULL) || (szText == NULL)) {
            if (pDC) {
                pDatasheet->ReleaseDC(pDC); pDC = NULL;
            }
            CCellDefault* pDefCell = (CCellDefault*) GetDefaultCell();
            ASSERT(pDefCell);
            return CSize(pDefCell->GetWidth(), pDefCell->GetHeight());
        }
        bReleaseDC = TRUE;
    }

    LOGFONT* plf = NULL;
    LONG lfW = 0;
    if (IsFixed()) {
        plf = GetFont();
        lfW = plf->lfWeight;
        plf->lfWeight = FW_BOLD;
        SetFont(plf);
    }

    CFont *pOldFont = NULL,
        *pFont = GetFontObject();
    if (pFont) {
        pOldFont = pDC->SelectObject(pFont);
    }

    int_t nFormat = GetFormat();

    CSize tSize = pDC->GetTextExtent(szText, (int_t) _tcslen(szText));
    CSize sizeDef = tSize;

    if (IsFixed() == FALSE) {
        sizeDef = pDC->GetTextExtent(_T("12345"), (int_t) _tcslen(_T("12345")));
    }

    if (IsFixed()) {
        plf->lfWeight = lfW;
        SetFont(plf);
    }

    if (pOldFont) {
        pDC->SelectObject(pOldFont);
    }
    tSize += CSize(2 * GetMargin(), 2 * GetMargin());
    sizeDef += CSize(2 * GetMargin(), 2 * GetMargin());

    // Kludge for vertical text
    LOGFONT *pLF = GetFont();
    if ((pLF->lfEscapement == 900) || (pLF->lfEscapement == -900)) {
        UINT uiMargin = GetMargin();
        int_t nTemp = tSize.cx;
        tSize.cx = tSize.cy;
        tSize.cy = nTemp;
        tSize += CSize(0, 4 * uiMargin);

        nTemp = sizeDef.cx;
        sizeDef.cx = sizeDef.cy;
        sizeDef.cy = nTemp;
        sizeDef += CSize(0, 4 * uiMargin);
    }

    if (bReleaseDC) {
        pDatasheet->ReleaseDC(pDC); pDC = NULL;
    }

    if ((tSize.cx < sizeDef.cx) || (tSize.cy < sizeDef.cy)) {
        return sizeDef;
    }

    return tSize;
}

CSize CCell::GetCellExtent(CDC* pDC)
{
    CSize tSize = GetTextExtent(GetText(), pDC);
    CSize sizeT = GetTextExtent(_T("a"), pDC);
    if (tSize.cx > (8 * sizeT.cx)) {
        tSize.cx += sizeT.cx + (sizeT.cx >> 1);
    }
    else {
        tSize.cx += sizeT.cx;
    }

    return tSize;
}

BOOL CCell::PrintCell(CDC* pDC, int_t nRow, int_t nCol, CRect rcT)
{
    COLORREF crFG, crBG;

    CDatasheet* pDatasheet = GetDatasheet();
    if (!pDatasheet || !pDC) {
        return FALSE;
    }

    if ((rcT.Width() <= 0) || (rcT.Height() <= 0)) {    // prevents imagelist item from drawing even though tCell is hidden
        return FALSE;
    }

    int_t nSavedDC = pDC->SaveDC();

    pDC->SetBkMode(TRANSPARENT);

    // Get the default tCell implementation for this kind of tCell. We use it if this tCell
    // has anything marked as "default"
    CCellDefault *pDefaultCell = (CCellDefault*) GetDefaultCell();
    if (!pDefaultCell) {
        return FALSE;
    }

    BOOL bMasked = pDatasheet->IsCellMasked(nCol, nRow);

    int xL = rcT.left, yT = rcT.top, xR = rcT.right, yB = rcT.bottom;

    COLORREF TextClr, TextClrMask, TextBkClr, TextBkClrMask;

    TextClrMask = RGB(255, 0, 0);
    TextBkClrMask = RGB(240, 242, 242);

    TextClr = bMasked ? TextClrMask : ((GetTextClr() == CLR_DEFAULT) ? pDefaultCell->GetTextClr() : GetTextClr());
    if (GetBackClr() == CLR_DEFAULT) {
        TextBkClr = bMasked ? TextBkClrMask : pDefaultCell->GetBackClr();
    }
    else {
        TextBkClr = bMasked ? TextBkClrMask : GetBackClr();
    }

    // Use custom color if it doesn't match the default color and the
    // default grid background color.  If not, leave it alone.
    if (IsFixed()) {
        crBG = (GetBackClr() != CLR_DEFAULT) ? GetBackClr() : pDefaultCell->GetBackClr();
    }
    else {
        crBG = bMasked ? TextBkClrMask : ((GetBackClr() != CLR_DEFAULT && GetBackClr() != pDefaultCell->GetBackClr()) ? GetBackClr() : CLR_DEFAULT);
    }

    // Use custom color if the background is different or if it doesn't
    // match the default color and the default grid text color.  
    if (IsFixed()) {
        crFG = (GetBackClr() != CLR_DEFAULT) ? GetTextClr() : pDefaultCell->GetTextClr();
    }
    else {
        crFG = (GetBackClr() != CLR_DEFAULT) ? GetTextClr() : pDefaultCell->GetTextClr();
    }

    if (pDC->GetDeviceCaps(NUMCOLORS) == 2 && crBG == CLR_DEFAULT) {
        crFG = RGB(GetRValue(crFG) * 0.30, GetGValue(crFG) * 0.59, GetBValue(crFG) * 0.11);
    }

    // Only erase the background if the color is not the default
    // grid background color.
    if (crBG != CLR_DEFAULT) {
        CBrush brushT(crBG);
        rcT.right++; rcT.bottom++;
        pDC->FillRect(rcT, &brushT);
        rcT.right--; rcT.bottom--;
    }

    pDC->SetTextColor(crFG);

    CFont *pFont = GetFontObject();
    if (pFont) {
        pDC->SelectObject(pFont);
    }

    // Draw lines only when wanted on fixed cells.  Normal tCell grid lines
    // are handled in OnPrint.
    if ((pDatasheet->GetGridLines() != DVL_NONE) && IsFixed()) {
        CPen lightpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHIGHLIGHT)),
            darkpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW)),
            *pOldPen = pDC->GetCurrentPen();

        pDC->SelectObject(&lightpen);
        pDC->MoveTo(rcT.right, rcT.top);
        pDC->LineTo(rcT.left, rcT.top);
        pDC->LineTo(rcT.left, rcT.bottom);

        pDC->SelectObject(&darkpen);
        pDC->MoveTo(rcT.right, rcT.top);
        pDC->LineTo(rcT.right, rcT.bottom);
        pDC->LineTo(rcT.left, rcT.bottom);

        rcT.DeflateRect(1, 1);
        pDC->SelectObject(pOldPen);
    }

    rcT.DeflateRect(GetMargin(), 0);

    HDC hDCX = pDC->GetSafeHdc();

    BOOL bHeader = ((nRow == 0) || (nCol == 0));

    if (bHeader) {

        if ((nRow == 0) && (nCol > 0)) {

            LOGFONT* plf = GetFont();
            LONG lfW = plf->lfWeight;
            plf->lfWeight = FW_NORMAL;
            SetFont(plf);

            CFont *pFont = GetFontObject();
            ASSERT(pFont);
            if (pFont) {
                pDC->SelectObject(pFont);
            }

            char_t szT[2];
            szT[1] = _T('\0');
            CRect rcL;

            szT[0] = pDatasheet->GetColumnType(nCol);
            CSize sizeT = pDC->GetTextExtent(szT);
            rcL.top = yT;
            rcL.bottom = rcL.top + sizeT.cy;
            rcL.left = xL + (sizeT.cx >> 1);
            rcL.right = rcL.left + sizeT.cx;
            pDC->SetTextColor(RGB(140, 110, 150));
            DrawText(hDCX, szT, -1, rcL, DT_LEFT | DT_NOPREFIX | DT_TOP);

            rcT.left = rcL.right;

            szT[0] = pDatasheet->GetColumnName(nCol);
            sizeT = pDC->GetTextExtent(szT);
            rcL.bottom = yB;
            rcL.top = rcL.bottom - sizeT.cy;
            rcL.left = xR - sizeT.cx - (sizeT.cx >> 1);
            rcL.right = rcL.left + sizeT.cx;
            pDC->SetTextColor(RGB(80, 130, 140));
            DrawText(hDCX, szT, -1, rcL, DT_RIGHT | DT_NOPREFIX | DT_BOTTOM);

            rcT.right = rcL.left;

            plf->lfWeight = lfW;
            SetFont(plf);

            pFont = GetFontObject();
            ASSERT(pFont);
            if (pFont) {
                pDC->SelectObject(pFont);
            }

            pDC->SetTextColor(TextClr);
        }

        LPCTSTR pszT = GetText();
        CSize sizeT = pDC->GetTextExtent(pszT);
        CRect rcX = rcT;
        rcX.top = ((rcT.bottom + rcT.top) >> 1) - (sizeT.cy >> 1);
        rcX.bottom = rcX.top + sizeT.cy;

        DrawText(hDCX, pszT, -1, rcX, DT_NOPREFIX | DT_CENTER | DT_TOP);
    }
    else {
        LPCTSTR pszT = GetText();
        CSize sizeT = pDC->GetTextExtent(pszT);
        CRect rcX = rcT;
        rcX.top = ((rcT.bottom + rcT.top) >> 1) - (sizeT.cy >> 1);
        rcX.bottom = rcX.top + sizeT.cy;

        DrawText(hDCX, pszT, -1, rcX, DT_NOPREFIX | DT_RIGHT | DT_TOP);
    }

    pDC->RestoreDC(nSavedDC);

    return TRUE;
}

// Various changes to make it draw cells better when using alternate
// color schemes.  Also removed printing references as that's now done
// by PrintCell() and fixed the sort marker so that it doesn't draw out
// of bounds.
BOOL CCell::Draw(CDC* pDC, int_t nRow, int_t nCol, CRect rcT, BOOL bInEdit/* = FALSE*/)
{
    BOOL bHeader = ((nRow == 0) || (nCol == 0));

    CDatasheet* pDatasheet = GetDatasheet();
    ASSERT(pDatasheet);

    if (!pDatasheet || !pDC) {
        return FALSE;
    }

    int_t nGridLines = pDatasheet->GetGridLines();

    if ((rcT.Width() <= 0) || (rcT.Height() <= 0)) {            // prevents imagelist item from drawing even
        return FALSE;                                                        // though tCell is hidden
    }

    int_t nSavedDC = pDC->SaveDC();
    if (nSavedDC < 1) {
        return FALSE;
    }

    int_t nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    // Get the default tCell implementation for this kind of tCell. We use it if this tCell
    // has anything marked as "default"
    CCellDefault *pDefaultCell = (CCellDefault*) GetDefaultCell();
    if (!pDefaultCell) {
        pDC->SetBkMode(nPrevBkMode);
        return FALSE;
    }

    BOOL bMasked = pDatasheet->IsCellMasked(nCol, nRow);

    int xL = rcT.left, yT = rcT.top, xR = rcT.right, yB = rcT.bottom;

    COLORREF TextClr, TextClrMask, TextBkClr, TextBkClrMask;

    TextClrMask = ((GetState() & DVIS_SELECTED)) ? RGB(255, 195, 195) : RGB(255, 0, 0);
    TextBkClrMask = RGB(247, 244, 243);

    TextClr = bMasked ? TextClrMask : ((GetTextClr() == CLR_DEFAULT) ? pDefaultCell->GetTextClr() : GetTextClr());
    if (GetBackClr() == CLR_DEFAULT) {
        TextBkClr = bMasked ? TextBkClrMask : pDefaultCell->GetBackClr();
    }
    else {
        TextBkClr = bMasked ? TextBkClrMask : GetBackClr();
    }

    if (IsFocused()) {
        if (bInEdit || pDatasheet->IsEditing(nRow, nCol)) {
            TextBkClr = ::GetSysColor(COLOR_WINDOW);
            rcT.right++; rcT.bottom++;                // FillRect doesn't draw RHS or bottom
            pDC->FillSolidRect(rcT, TextBkClr);
            rcT.right--; rcT.bottom--;
            pDC->SetTextColor(TextClr);
        }
        else if ((GetState() & DVIS_SELECTED)) {
            rcT.right++; rcT.bottom++;                // FillRect doesn't draw RHS or bottom
            pDC->FillSolidRect(rcT, m_crSelectedBackColor);
            rcT.right--; rcT.bottom--;
            pDC->SetTextColor(bMasked ? TextClrMask : m_crSelectedForeColor);
        }
        else {
            rcT.right++; rcT.bottom++;            // FillRect doesn't draw RHS or bottom
            CBrush brushT(TextBkClr);
            pDC->FillRect(rcT, &brushT);
            rcT.right--; rcT.bottom--;
            pDC->SetTextColor(TextClr);
        }
    }
    else if ((GetState() & DVIS_SELECTED)) {
        rcT.right++; rcT.bottom++;                // FillRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rcT, m_crSelectedBackColor);
        rcT.right--; rcT.bottom--;
        pDC->SetTextColor(bMasked ? TextClrMask : m_crSelectedForeColor);
    }
    else {
        rcT.right++; rcT.bottom++;            // FillRect doesn't draw RHS or bottom
        CBrush brushT(TextBkClr);
        pDC->FillRect(rcT, &brushT);
        rcT.right--; rcT.bottom--;
        pDC->SetTextColor(TextClr);
    }

    // Draw lines only when wanted
    if (IsFixed() && pDatasheet->GetGridLines() != DVL_NONE) {
        CCellID FocusCell = pDatasheet->GetFocusCell();

        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        BOOL bHiliteFixed = pDatasheet->GetTrackFocusCell() && pDatasheet->isValid(FocusCell)
            && ((FocusCell.row == nRow && nRow > 0) || (FocusCell.col == nCol && nCol > 0)
            || ((FocusCell.row == nRow && nRow == 0) && (FocusCell.col == nCol && nCol == 0)));

        CPen penHeader(PS_SOLID, 1, m_crSelectedBorderColor);
        CPen *pOldPen = pDC->SelectObject(&penHeader);
        pDC->MoveTo(rcT.right, rcT.top);
        pDC->LineTo(rcT.right, rcT.bottom);
        pDC->LineTo(rcT.left, rcT.bottom);
        pDC->LineTo(rcT.left, rcT.top);
        penHeader.DeleteObject();
        pDC->SelectObject(pOldPen);

        // If this fixed tCell is on the same row/col as the focus tCell,
        // highlight it.
        if (bHiliteFixed && ((nRow > 0) || (nCol > 0) || pDatasheet->IsAllCellsSelected())) {

            TextClr = GetTextClr();

            CBrush brushT(m_crSelectedBackColorHeader);
            CPen *penSel = NULL;
            if ((0 == nRow) || (0 == nCol)) {
                rcT.left++;
                try {
                    penSel = new CPen(PS_NULL, 0, m_crSelectedBorderColor);
                }
                catch (...) {
                    penSel = NULL;
                }
            }
            else {
                try {
                    penSel = new CPen(PS_SOLID, 1, m_crSelectedBorderColor);
                }
                catch (...) {
                    penSel = NULL;
                }
            }
            CBrush *pBrushOld = pDC->SelectObject(&brushT);
            if (penSel) {
                pOldPen = pDC->SelectObject(penSel);
            }
            pDC->Rectangle(rcT);
            brushT.DeleteObject();
            if (penSel) {
                penSel->DeleteObject();
                delete penSel;
                penSel = NULL;
                pDC->SelectObject(pOldPen);
            }
            pDC->SelectObject(pBrushOld);

            LOGFONT* plf = GetFont();
            if (plf->lfWeight != FW_BOLD) {
                plf->lfWeight = FW_BOLD;
                SetFont(plf);
            }
        }
        else {
            LOGFONT* plf = GetFont();
            if (plf->lfWeight != FW_NORMAL) {
                plf->lfWeight = FW_NORMAL;
                SetFont(plf);
            }
        }
    }

    // Draw Text
    if (!pDC->m_bPrinting) {
        CFont *pFont = GetFontObject();
        ASSERT(pFont);
        if (pFont) {
            pDC->SelectObject(pFont);
        }
    }

    rcT.DeflateRect(GetMargin(), GetMargin());

    // We want to see '&' characters so use DT_NOPREFIX
    GetTextRect(rcT);

    HDC hDCX = pDC->GetSafeHdc();
    if (bHeader) {

        if ((nRow == 0) && (nCol > 0)) {

            LOGFONT* plf = GetFont();
            LONG lfW = plf->lfWeight;
            plf->lfWeight = FW_NORMAL;
            SetFont(plf);

            CFont *pFont = GetFontObject();
            ASSERT(pFont);
            if (pFont) {
                pDC->SelectObject(pFont);
            }

            char_t szT[2];
            szT[1] = _T('\0');
            CRect rcL;

            szT[0] = pDatasheet->GetColumnType(nCol);
            CSize sizeT = pDC->GetTextExtent(szT);
            rcL.top = yT;
            rcL.bottom = rcL.top + sizeT.cy;
            rcL.left = xL + (sizeT.cx >> 1);
            rcL.right = rcL.left + sizeT.cx;
            pDC->SetTextColor(RGB(140, 110, 150));
            DrawText(hDCX, szT, -1, rcL, DT_LEFT | DT_NOPREFIX | DT_TOP);

            rcT.left = rcL.right;

            szT[0] = pDatasheet->GetColumnName(nCol);
            sizeT = pDC->GetTextExtent(szT);
            rcL.bottom = yB;
            rcL.top = rcL.bottom - sizeT.cy;
            rcL.left = xR - sizeT.cx - (sizeT.cx >> 1);
            rcL.right = rcL.left + sizeT.cx;
            pDC->SetTextColor(RGB(80, 130, 140));
            DrawText(hDCX, szT, -1, rcL, DT_RIGHT | DT_NOPREFIX | DT_BOTTOM);

            rcT.right = rcL.left;

            plf->lfWeight = lfW;
            SetFont(plf);

            pFont = GetFontObject();
            ASSERT(pFont);
            if (pFont) {
                pDC->SelectObject(pFont);
            }

            pDC->SetTextColor(TextClr);
        }

        LPCTSTR pszT = GetText();
        CSize sizeT = pDC->GetTextExtent(pszT);
        CRect rcX = rcT;
        rcX.top = ((rcT.bottom + rcT.top) >> 1) - (sizeT.cy >> 1);
        rcX.bottom = rcX.top + sizeT.cy;

        DrawText(hDCX, pszT, -1, rcX, DT_NOPREFIX | DT_CENTER | DT_TOP);
    }
    else {
        LPCTSTR pszT = GetText();
        CSize sizeT = pDC->GetTextExtent(pszT);
        CRect rcX = rcT;
        rcX.top = ((rcT.bottom + rcT.top) >> 1) - (sizeT.cy >> 1);
        rcX.bottom = rcX.top + sizeT.cy;

        DrawText(hDCX, pszT, -1, rcX, DT_NOPREFIX | DT_RIGHT | DT_TOP);
    }

    pDC->SetBkMode(nPrevBkMode);
    pDC->RestoreDC(nSavedDC);

    return TRUE;
}

// CCellDefault

CCellDefault::CCellDefault() 
{
    m_nFormat = DT_RIGHT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
    m_crFgClr = CLR_DEFAULT;
    m_crBkClr = CLR_DEFAULT;
    m_Size = CSize(30,10);
    m_dwStyle = 0;

    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    SetFont(&(ncm.lfMessageFont));
}

CCellDefault::~CCellDefault()
{
    m_Font.DeleteObject(); 
}

void CCellDefault::SetFont(const LOGFONT* plf)
{
    ASSERT(plf);

    if (!plf) { return; }

    m_Font.DeleteObject();
    m_Font.CreateFontIndirect(plf);

    CCell::SetFont(plf);

    // Get the font size and hence the default tCell size
    CDC* pDC = CDC::FromHandle(::GetDC(NULL));
    if (pDC) {
        CFont* pOldFont = pDC->SelectObject(&m_Font);

        SetMargin(pDC->GetTextExtent(_T(" "), 1).cx);
        m_Size = pDC->GetTextExtent(_T(" WWWWWWWWW "), 14);
        m_Size.cy = (m_Size.cy * 3) / 2;

        pDC->SelectObject(pOldFont);
        ::ReleaseDC(NULL, pDC->GetSafeHdc());
        pDC = NULL;
    }
    else {
        SetMargin(3);
        m_Size = CSize(40,16);
    }
}

LOGFONT* CCellDefault::GetFont() const
{
    ASSERT(m_plfFont);            // This is the default - it CAN'T be NULL!
    return m_plfFont;
}

CFont* CCellDefault::GetFontObject() const
{
    ASSERT(m_Font.GetSafeHandle());
    return (CFont*) &m_Font; 
}
