//////////////////////////////////////////////////////////////////////
// InPlaceEdit.cpp : implementation file
//
// Adapted by Chris Maunder <cmaunder@mail.com>
// Copyright(C) 1998-2002. All Rights Reserved.
//
// The code contained in this file is based on the original
// CInPlaceEdit from http://www.codeguru.com/listview/edit_subitems.shtml
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
// For use with CGridCtrl v2.10+
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Modifications:
//   Copyright(C) 1998-2002 Sidi OULD SAAD HAMADY
//   http://www.hamady.org
//   sidi@hamady.org
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "TCHAR.h"
#include "InEdit.h"

#include "Datasheet.h"

#include "Sigma.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CInEdit

CInEdit::CInEdit(CWnd* pParent, CRect& rcT, DWORD dwStyle, UINT nID,
                      int_t nRow, int_t nCol, CString strText, UINT nFirstChar)
{
    m_pOwner = pParent;
    m_strText = strText;
    m_nRow = nRow;
    m_nCol = nCol;
    m_nLastChar = 0;

    ((CDatasheet*) m_pOwner)->RedrawCell(m_nRow, m_nCol, NULL, TRUE);

    CFont *pFont = pParent->GetFont();

    CDC *pDC = m_pOwner->GetDC();
    CFont *pOldFont = pDC->SelectObject(pFont);
    CSize tSize = pDC->GetTextExtent(_T("Ky"), 2);
    if (pOldFont) {
        pDC->SelectObject(pOldFont);
    }
    m_pOwner->ReleaseDC(pDC); pDC = NULL;

    m_Rect = rcT;
    LONG yCenter = (m_Rect.bottom + m_Rect.top) >> 1;
    LONG yDelta = (tSize.cy >> 1);
    m_Rect.top = yCenter - yDelta;
    m_Rect.bottom = yCenter + yDelta + 1;

    DWORD dwEditStyle = (WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | dwStyle) & ~WS_BORDER;
    if (!CEdit::Create(dwEditStyle, m_Rect, pParent, nID)) {
        return;
    }

    SetFont(pParent->GetFont());

    SetWindowText(m_strText);
    SetFocus();

    switch (nFirstChar){
        case VK_LBUTTON: 
        case VK_RETURN:
            SetSel(m_strText.GetLength(), -1);
            return;
        case VK_BACK:
            SetSel(m_strText.GetLength(), -1);
            break;
        case VK_TAB:
        case VK_DOWN:
        case VK_UP:
        case VK_RIGHT:
        case VK_LEFT:
        case VK_NEXT:
        case VK_PRIOR:
        case VK_HOME:
        case VK_SPACE:
        case VK_END:
            SetSel(0,-1);
            return;
        default:
            SetSel(0,-1);
            break;
    }

    // When entering DBCS chars into cells the first char was being lost
    // SenMessage changed to PostMessage
    if (nFirstChar < 0x80) {
        PostMessage(WM_CHAR, nFirstChar);
    }
    else {
        PostMessage(WM_IME_CHAR, nFirstChar);
    }
}

CInEdit::~CInEdit()
{

}

BEGIN_MESSAGE_MAP(CInEdit, CEdit)
    ON_WM_CREATE()
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_GETDLGCODE()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

// CInEdit message handlers

BOOL CInEdit::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if ((pMsg->wParam == VK_DELETE)) {
                int_t iStart, iEnd;
                CEdit::GetSel(iStart, iEnd);
                if (iStart == iEnd) {
                    iEnd += 1;
                    CEdit::SetSel(iStart, iEnd);
                }
                CEdit::Cut();
                return TRUE;
            }
        }
    }

    return CEdit::PreTranslateMessage(pMsg);
}

void CInEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (CTRL_Pressed() || (nChar == VK_DOWN) || (nChar == VK_UP)) {
        m_nLastChar = nChar;
        EndEdit();
        return;
    }

    if (CTRL_Pressed()) {
        m_nLastChar = nChar;
        EndEdit();
        return;
    }

    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

// As soon as this edit loses focus, kill it.
void CInEdit::OnKillFocus(CWnd* pNewWnd)
{
    GetWindowText(m_strText);

    CEdit::OnKillFocus(pNewWnd);

    DestroyWindow();
}

void CInEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CEdit::OnChar(nChar, nRepCnt, nFlags);

    if ((nChar == VK_TAB) || (nChar == VK_RETURN)) {
        m_nLastChar = nChar;
        EndEdit();
        return;
    }
    if (nChar == VK_ESCAPE) {
        SetWindowText((LPCTSTR) m_strText);        // restore previous text
        m_nLastChar = nChar;
        EndEdit();
        return;
    }
}

UINT CInEdit::OnGetDlgCode()
{
    return (CEdit::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS);
}

// CInEdit overrides

// Auto delete
void CInEdit::PostNcDestroy() 
{
    int_t nRow = m_nRow, nCol = m_nCol;
    UINT nLastChar = m_nLastChar;
    CString strText = m_strText;

    CEdit::PostNcDestroy();

    delete this;

    ((CDatasheet*) m_pOwner)->DestroyEdit(nRow, nCol, nLastChar, (LPCTSTR) strText);
}

// CInEdit implementation

void CInEdit::EndEdit()
{
    GetWindowText(m_strText);

    DestroyWindow();
}
