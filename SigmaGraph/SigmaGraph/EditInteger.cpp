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
#include "EditInteger.h"


// CEditInteger

IMPLEMENT_DYNAMIC(CEditInteger, CEdit)

CEditInteger::CEditInteger()
{

}

CEditInteger::~CEditInteger()
{

}


BEGIN_MESSAGE_MAP(CEditInteger, CEdit)
    ON_WM_CREATE()
    ON_WM_CHAR()
END_MESSAGE_MAP()



// CEditInteger message handlers

BOOL CEditInteger::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.style &= ~ES_MULTILINE;
    cs.style |= ES_NUMBER;
    return CEdit::PreCreateWindow(cs);
}

int_t CEditInteger::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CEdit::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    m_iMin = 1;
    m_iMax = 9;
    SetLimitText(1);

    return 0;
}

void CEditInteger::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if (iswdigit(nChar)) {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
        int_t iVal = GetValue();
        if (iVal < m_iMin) {
            SetValue(m_iMin);
            SetSel(GetWindowTextLength(), GetWindowTextLength());
        }
        if (iVal > m_iMax) {
            SetValue(m_iMax);
            SetSel(GetWindowTextLength(), GetWindowTextLength());
        }
    }
    else if (!_istalpha(nChar) && (nChar != _T('/')) && (nChar != _T('*'))) {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
    }
}

void CEditInteger::SetRange(int_t iMin, int_t iMax, int_t iMaxDigits)
{
    m_iMin = iMin;
    m_iMax = iMax;
    SetLimitText(iMaxDigits);
}

void CEditInteger::SetValue(int_t iVal)
{
    char_t szBuffer[ML_STRSIZES];
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    _tcsprintf(szBuffer, ML_STRSIZES - 2, _T("%d"), iVal);
    SetWindowText((LPCTSTR) szBuffer);
}

int_t CEditInteger::GetValue(void)
{
    char_t szBuffer[ML_STRSIZES];
    int_t iVal = 0;
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    int_t iret = GetWindowText((LPTSTR)szBuffer, ML_STRSIZES - 1);

    if ((iret > 0) && (iret < (ML_STRSIZES - 2))) {
        iVal = (int_t) _wtol((const char_t*) szBuffer);
    }

    return iVal;
}
