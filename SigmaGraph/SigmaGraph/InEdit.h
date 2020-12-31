//////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//
// MFC Grid Control - inplace editing class
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
// For use with CGridCtrl v2.10+
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Modifications:
//   Copyright(C) 1998-2002 Sidi OULD SAAD HAMADY
//   http://www.hamady.org
//   sidi@hamady.org
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../LibGraph/LibFile/libfile.h"

class CInEdit : public CEdit
{
// Construction
public:
    CInEdit(CWnd* pParent, CRect& rcT, DWORD dwStyle, UINT nID, int_t nRow, int_t nColumn, CString sInitText, UINT nFirstChar);

// Attributes
public:

// Operations
public:
    void EndEdit();
    BOOL IsEditing(int_t nRow, int_t nCol) { return ((nRow == m_nRow) && (nCol == m_nCol)); }

// Overrides
public:

protected:
    virtual void PostNcDestroy();

// Implementation
public:
    virtual ~CInEdit();

    virtual BOOL PreTranslateMessage(MSG* pMsg);

// Generated message map functions
protected:
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg UINT OnGetDlgCode();

    DECLARE_MESSAGE_MAP()

private:
    CWnd *m_pOwner;
    int_t m_nRow;
    int_t m_nCol;
    CString m_strText;
    UINT m_nLastChar;
    CRect m_Rect;
};
 