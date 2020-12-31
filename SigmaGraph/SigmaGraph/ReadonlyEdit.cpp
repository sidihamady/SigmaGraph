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
#include "ReadonlyEdit.h"


// CReadonlyEdit

IMPLEMENT_DYNAMIC(CReadonlyEdit, CEdit)

CReadonlyEdit::CReadonlyEdit()
{
    m_bInitialized = FALSE;
    m_clrText = RGB(0,0,0);
    m_clrBackground = RGB(255,255,255);
    m_brBackground = NULL;
}

CReadonlyEdit::CReadonlyEdit(COLORREF clrText, COLORREF clrBackground)
{
    m_bInitialized = FALSE;
    m_clrText = clrText;
    m_clrBackground = clrBackground;
    m_brBackground = NULL;
}

CReadonlyEdit::~CReadonlyEdit()
{
    if (m_brBackground) {
        ::DeleteObject(m_brBackground);
        m_brBackground = NULL;
    }
    m_Font.DeleteObject();
}

void CReadonlyEdit::SetColors(COLORREF clrText, COLORREF clrBackground)
{
    m_clrText = clrText;
    m_clrBackground = clrBackground;
}

BEGIN_MESSAGE_MAP(CReadonlyEdit, CEdit)
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void CReadonlyEdit::SetFontParameters(const char_t *pszName, int iSize)
{
    LOGFONT lft;
    memset(&lft, 0, sizeof(lft));
    m_Font.GetLogFont(&lft);
    m_Font.DeleteObject();
    _tcscpy(lft.lfFaceName, pszName);
    HDC hDCX = ::GetDC(NULL);
    if (hDCX) {
        lft.lfHeight = -MulDiv(iSize, ::GetDeviceCaps(hDCX, LOGPIXELSY), 72);
        ::ReleaseDC(NULL, hDCX);
        hDCX = NULL;
    }
    m_Font.CreateFontIndirect(&lft);
    SetFont(&m_Font);
}

// CReadonlyEdit message handlers

HBRUSH CReadonlyEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    // TODO:  Change any attributes of the DC here
    // Deselect text
    if (m_bInitialized == FALSE) {
        SetSel(-1, -1);
        m_bInitialized = TRUE;
    }

    //Set text color
    pDC->SetTextColor(m_clrText);
    //Set the text's background color
    pDC->SetBkColor(m_clrBackground);

    if (m_brBackground == NULL) {
        m_brBackground = ::CreateSolidBrush(m_clrBackground);
    }

    // TODO:  Return a non-NULL brush if the parent's handler should not be called
    return m_brBackground;
}
