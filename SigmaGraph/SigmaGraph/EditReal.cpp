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
#include "EditReal.h"


// CEditReal

IMPLEMENT_DYNAMIC(CEditReal, CEdit)

CEditReal::CEditReal()
{

}

CEditReal::~CEditReal()
{

}


BEGIN_MESSAGE_MAP(CEditReal, CEdit)
    ON_WM_CREATE()
    ON_WM_CHAR()
END_MESSAGE_MAP()



// CEditReal message handlers

BOOL CEditReal::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.style &= ~ES_MULTILINE;
    return CEdit::PreCreateWindow(cs);
}

int_t CEditReal::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CEdit::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    return 0;
}

void CEditReal::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    // TODO
    if (iswdigit(nChar)
        || (nChar == _T(',')) || (nChar == _T('.'))
        || (nChar == _T('e')) || (nChar == _T('E')) 
        || (nChar == _T('+')) || (nChar == _T('-'))) {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
    }
    else if (!_istalpha(nChar) && (nChar != _T('/')) && (nChar != _T('*'))) {
        CEdit::OnChar(nChar, nRepCnt, nFlags);
    }
}

void CEditReal::SetValue(real_t fVal, const char_t *pszFmt/* = NULL*/)
{
    char_t szBuffer[ML_STRSIZES];

    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));

    if (pszFmt == NULL) {
        SigmaApp.Reformat(fVal, szBuffer, ML_STRSIZES - 2);
    }
    else {
        _tcsprintf(szBuffer, ML_STRSIZES - 2, pszFmt, fVal);
        ml_reformat(szBuffer);
    }

    SetWindowText((LPCTSTR)szBuffer);
}

real_t CEditReal::GetValue()
{
    char_t szBuffer[ML_STRSIZES];
    real_t fVal = 0.0;

    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));

    int_t iret = GetWindowText((LPTSTR)szBuffer, ML_STRSIZES - 1);

    if ((iret > 0) && (iret < (ML_STRSIZES - 2))) {
        byte_t bErr = 0;
        fVal = ml_toreal((const char_t*)szBuffer, &bErr, NULL);
    }

    return fVal;
}
