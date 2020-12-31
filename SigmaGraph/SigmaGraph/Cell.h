/////////////////////////////////////////////////////////////////////////////
// GridCell.h : header file
//
// MFC Grid Control - Grid cell class header file
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
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Modifications:
//   Copyright(C) 1998-2007 Sidi OULD SAAD HAMADY
//   http://www.hamady.org
//   sidi@hamady.org
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../LibGraph/LibFile/libfile.h"

class CDatasheet;

// Cell states
#define DVIS_FOCUSED                 0x0001
#define DVIS_SELECTED                0x0002
#define DVIS_READONLY                0x0008
#define DVIS_FIXED                   0x0010
#define DVIS_FIXEDROW                0x0020
#define DVIS_FIXEDCOL                0x0040
#define DVIS_MODIFIED                0x0080

// Cell data mask
#define DVIF_TEXT                     LVIF_TEXT
#define DVIF_PARAM                    LVIF_PARAM
#define DVIF_STATE                    LVIF_STATE
#define DVIF_BKCLR                    (DVIF_STATE<<1)
#define DVIF_FGCLR                    (DVIF_STATE<<2)
#define DVIF_FORMAT                   (DVIF_STATE<<3)
#define DVIF_FONT                     (DVIF_STATE<<4)
#define DVIF_MARGIN                   (DVIF_STATE<<5)
#define DVIF_ALL                      (DVIF_TEXT|DVIF_PARAM|DVIF_STATE|DVIF_BKCLR|DVIF_FGCLR|DVIF_FORMAT|DVIF_FONT|DVIF_MARGIN)

// Used for Get/SetItem calls.
typedef struct _GV_ITEM {
    int_t col;                        // Column of item
    int_t row;                        // Row of item
    UINT mask;                        // Mask for use in getting/setting tCell data
    UINT nState;                      // tCell state (focus/hilighted etc)
    DWORD nFormat;                    // Format of tCell
    COLORREF crBkClr;                 // Background colour (or CLR_DEFAULT)
    COLORREF crFgClr;                 // Forground colour (or CLR_DEFAULT)
    LPARAM lParam;                    // 32-bit value to associate with item
    LOGFONT lfFont;                   // Cell font
    UINT nMargin;                     // Internal tCell margin
    char_t szText[ML_STRSIZEN];
} GV_ITEM;

// Each tCell contains one of these. Fields "row" and "column" are not stored since we
// will usually have acces to them in other ways, and they are an extra 8 bytes per
// tCell that is probably unnecessary.

class CCell : public CObject
{
    friend class CDatasheet;
    DECLARE_DYNCREATE(CCell)

// Construction/Destruction
public:
    CCell();
    ~CCell();

    COLORREF m_crSelectedForeColor, m_crSelectedBackColor, m_crSelectedBorderColor, m_crSelectedBackColorHeader;

    void SetState(DWORD nState) { m_nState = nState; }
    LPCTSTR GetTipText() const { return GetText(); }    // may override TitleTip return
    DWORD GetState() const { return m_nState; }

    CCell* GetDefaultCell() const;

    BOOL IsFocused() const { return (m_nState & DVIS_FOCUSED); }
    BOOL IsFixed() const { return (m_nState & DVIS_FIXED); }
    BOOL IsFixedCol() const { return (m_nState & DVIS_FIXEDCOL); }
    BOOL IsFixedRow() const { return (m_nState & DVIS_FIXEDROW); }
    BOOL IsSelected() const { return (m_nState & DVIS_SELECTED); }
    BOOL IsReadOnly() const { return (m_nState & DVIS_READONLY); }
    BOOL IsModified() const { return (m_nState & DVIS_MODIFIED); }

// Attributes
public:
    void operator=(const CCell& tCell);

    void SetText(LPCTSTR szText) { _tcscpy(m_szText, (const char_t*)szText); }
    void SetData(LPARAM lParam) { m_lParam = lParam; }
    void SetDatasheet(CDatasheet* pDatasheet) { m_pDatasheet = pDatasheet; }
    void SetFormat(DWORD nFormat) { m_nFormat = nFormat; }
    void SetTextClr(COLORREF clr) { m_crFgClr = clr; }
    void SetBackClr(COLORREF clr) { m_crBkClr = clr; }
    void SetFont(const LOGFONT* plf);
    void SetMargin(UINT nMargin) { m_nMargin = nMargin; }
    LPCTSTR GetText() const { return (LPCTSTR)m_szText; }
    int_t GetImage() const { return m_nImage; }
    LPARAM GetData() const { return m_lParam; }
    CDatasheet* GetDatasheet() const { return m_pDatasheet; }
    DWORD GetFormat() const;
    COLORREF GetTextClr() const { return m_crFgClr; } // TODO: change to use default tCell
    COLORREF GetBackClr() const { return m_crBkClr; }
    LOGFONT* GetFont() const;
    CFont* GetFontObject() const;
    UINT GetMargin() const;

    BOOL IsDefaultFont() const { return (m_plfFont == NULL); }
    void Reset();

// editing cells
public:
    BOOL Draw(CDC* pDC, int_t nRow, int_t nCol, CRect rcT, BOOL bInEdit = FALSE);

    BOOL GetTextRect(LPRECT pRect);
    CSize GetTextExtent(LPCTSTR str, CDC* pDC = NULL);
    CSize GetCellExtent(CDC* pDC);

    // Added to print cells properly
    BOOL PrintCell(CDC* pDC, int_t nRow, int_t nCol, CRect rcT);

protected:
    DWORD m_nState;    // Cell state (selected/focus etc)
    char_t m_szText[ML_STRSIZEN];                // Cell text
    LPARAM m_lParam;                                // 32-bit value to associate with item
    int_t m_nImage;                                // Index of the list view item�s icon
    DWORD m_nFormat;
    COLORREF m_crFgClr;
    COLORREF m_crBkClr;
    LOGFONT* m_plfFont;
    UINT m_nMargin;

    CDatasheet* m_pDatasheet;                    // Parent control
    BOOL OnSetCursor();
};

// This class is for storing default values. It's a little heavy weight, so
// don't use it in bulk 

class CCellDefault : public CCell
{
    DECLARE_DYNCREATE(CCellDefault)

// Construction/Destruction
public:
    CCellDefault();
    ~CCellDefault();

public:
    DWORD GetStyle() const { return m_dwStyle; }
    void SetStyle(DWORD dwStyle) { m_dwStyle = dwStyle; }
    int_t GetWidth() const { return m_Size.cx; }
    int_t GetHeight() const { return m_Size.cy; }
    void SetWidth(int_t nWidth) { m_Size.cx = nWidth; }
    void SetHeight(int_t nHeight) { m_Size.cy = nHeight; }

    // Disable these properties
    void SetData(LPARAM /*lParam*/) { ASSERT(FALSE); }
    void SetState(DWORD /*nState*/) { ASSERT(FALSE); }
    DWORD GetState() const { return CCell::GetState() | DVIS_READONLY; }
    void SetFont(const LOGFONT* /*plf*/);
    LOGFONT* GetFont() const;
    CFont* GetFontObject() const;

protected:
    CSize m_Size;                    // Default Size
    CFont m_Font;                    // Cached font
    DWORD m_dwStyle;                // Cell Style - unused
};
