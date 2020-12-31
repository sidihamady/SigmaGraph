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

// :TODO: complete doc and translate to english

#include "stdafx.h"
#include "Sigma.h"
#include "ScriptOutput.h"

#define CRLF                            _T("\r\n")
#define SCRIPTOUTPUT_MAXCHARS        32768

// CScriptOutput

IMPLEMENT_DYNAMIC(CScriptOutput, CReadonlyEdit)

CScriptOutput::CScriptOutput() : CReadonlyEdit()
{
    m_strPromptText = _T("> ");
    m_bInitialized = FALSE;
    SetColors(::GetSysColor(COLOR_WINDOWTEXT), ::GetSysColor(COLOR_WINDOW));
}

CScriptOutput::CScriptOutput(COLORREF clrText, COLORREF clrBackground) : CReadonlyEdit(clrText, clrBackground)
{
    m_strPromptText = _T("> ");
    m_bInitialized = FALSE;
    SetColors(::GetSysColor(COLOR_WINDOWTEXT), ::GetSysColor(COLOR_WINDOW));
}

CScriptOutput::~CScriptOutput()
{

}

BEGIN_MESSAGE_MAP(CScriptOutput, CReadonlyEdit)
    ON_WM_CREATE()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CScriptOutput::AppendText(const char_t *pszText)
{
    int iLenText = (int) (_tcslen(pszText));
    if (iLenText < 1) {
        return;
    }

    CString strU;
    GetWindowText(strU);

    int ii, iPos, iMaxLen = SCRIPTOUTPUT_MAXCHARS;
    int iLen = (int) (strU.GetLength());
    int iLenEdit = iLen;
    if (iLen >= iMaxLen) {
        iPos = iMaxLen >> 1;
        for (ii = iPos; ii < (iLen - 2); ii++) {
            if ((strU[ii] == _T('\r')) || (strU[ii] == _T('\n'))) {
                break;
            }
        }
        if ((strU[ii + 1] == _T('\r')) || (strU[ii + 1] == _T('\n'))) {
            iPos = ii + 2;
        }
        else if ((strU[ii] == _T('\r')) || (strU[ii] == _T('\n'))) {
            iPos = ii + 1;
        }
        strU = strU.Mid(iPos, iLen - iPos);
        SetWindowText(strU);
        iLenEdit = (int) (strU.GetLength());
    }

    int iLenPrompt = m_strPromptText.GetLength();
    CString strT = _T("");

    ii = 0;
    iLen = iLenText;
    while (true) {
        if ((pszText[ii] == _T('\r')) || (pszText[ii] == _T('\n'))) {
            ii += 1;
            if (ii < iLen) {
                if ((pszText[ii] == _T('\r')) || (pszText[ii] == _T('\n'))) {
                    ii += 1;
                }
            }
            strT += CRLF;
            strT += m_strPromptText;
        }
        else {
            strT += pszText[ii];
            ii += 1;
        }
        if (ii >= iLen) {
            break;
        }
    }

    SetSel(iLenEdit, iLenEdit);        // select position after last char_t in editbox
    ReplaceSel(strT);        // replace selection with new text
}

// CScriptOutput message handlers

BOOL CScriptOutput::PreCreateWindow(CREATESTRUCT& cs) 
{
    cs.style |= (ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL | ES_READONLY | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL);

    return CReadonlyEdit::PreCreateWindow(cs);
}

int_t CScriptOutput::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CReadonlyEdit::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    SetLimitText(SCRIPTOUTPUT_MAXCHARS);

    SetWindowText(m_strPromptText);
    int iLenPrompt = m_strPromptText.GetLength();
    SetSel(iLenPrompt, iLenPrompt); //select position after last char_t in editbox

    return 0;
}

void CScriptOutput::OnContextMenu(CWnd* /*pWnd*/, CPoint tPoint)
{
    SetFocus();

    CMenu popMenu;
    if (popMenu.CreatePopupMenu() == FALSE) {
        return;
    }

    BOOL bReadOnly = GetStyle() & ES_READONLY;

    UINT indexT = 0;

    DWORD sel = GetSel();
    DWORD flags = (LOWORD(sel) == HIWORD(sel)) ? MF_GRAYED : MF_ENABLED;
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | flags, WM_COPY, _T("Copy"));

    int nLen = GetWindowTextLength();

    flags = (nLen < 1) ? MF_GRAYED : MF_ENABLED;
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | flags, WM_CLEAR, _T("Clear"));
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCTSTR) NULL);
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | MF_ENABLED, ID_SCRIPT_OUTPUTSIZE_SMALL, _T("Small"));
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | MF_ENABLED, ID_SCRIPT_OUTPUTSIZE_MEDIUM, _T("Medium"));
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | MF_ENABLED, ID_SCRIPT_OUTPUTSIZE_LARGE, _T("Large"));
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | MF_SEPARATOR, 0, (LPCTSTR) NULL);
    popMenu.InsertMenu(indexT++, MF_BYPOSITION | MF_ENABLED, ID_SCRIPT_OUTPUT_VIEW, _T("Hide"));

    popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, this);
}

BOOL CScriptOutput::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam)) {
        case EM_UNDO:
        case WM_CUT:
        case WM_PASTE:
            return TRUE;
        case WM_COPY:
            return SendMessage(LOWORD(wParam));
        case WM_CLEAR:
            Reset();
            return TRUE;
        case ID_SCRIPT_OUTPUTSIZE_SMALL:
        case ID_SCRIPT_OUTPUTSIZE_MEDIUM:
        case ID_SCRIPT_OUTPUTSIZE_LARGE:
        case ID_SCRIPT_OUTPUT_VIEW:
            {
                CWnd* pParent = GetParent();
                pParent->SendMessage(WM_COMMAND, wParam, lParam);
            }
            return TRUE;
        default:
            return CReadonlyEdit::OnCommand(wParam, lParam);
    }

    return CReadonlyEdit::OnCommand(wParam, lParam);
}

void CScriptOutput::Reset(void)
{
    SetWindowText(m_strPromptText);
    int iLenPrompt = m_strPromptText.GetLength();
    SetSel(iLenPrompt, iLenPrompt); //select position after last char_t in editbox
}
