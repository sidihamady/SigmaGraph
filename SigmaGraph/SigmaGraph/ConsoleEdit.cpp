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
#include "ConsoleEdit.h"

#define CRLF    _T("\r\n")

// CConsoleEdit

IMPLEMENT_DYNAMIC(CConsoleEdit, CEdit)

CConsoleEdit::CConsoleEdit()
{
    m_bInitialized = FALSE;
    m_nCurCommand = m_nTotalCommand = 0;
}

CConsoleEdit::~CConsoleEdit()
{
    m_nCurCommand = m_nTotalCommand = 0;
}

BEGIN_MESSAGE_MAP(CConsoleEdit, CEdit)
    ON_WM_CREATE()
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


void CConsoleEdit::CarryReturn(BOOL bLineFeed)
{
    SetCaretPos();
    if (!bLineFeed) {
        SetSel(LineIndex(), GetWindowTextLength());
        ReplaceSel(_T(""));
    }
    else {
        AppendText(CRLF);
    }
    return;
}

void CConsoleEdit::AppendTextT(LPCTSTR szOut)
{
    CString strT = szOut;
    strT.Replace(_T("\n"), CRLF);
    AppendText(strT);
    return;
}

void CConsoleEdit::SetCaretPos(int_t pos)
{
    AppendText();
    if (pos >= 0) {
        int_t li = LineIndex() + pos;
        SetSel(li, li);
        ReplaceSel(_T(""));
    }
    return;
}

void CConsoleEdit::GetCaretPos(int_t& nLine, int_t& nPos)
{
    int_t nStart, nEnd;
    GetSel(nStart, nEnd);
    nLine = LineFromChar(nEnd);
    nPos = nEnd - LineIndex(nLine);
}

void CConsoleEdit::SetCurCommand(LPCTSTR lpszCommand)
{
    SetSel(LineIndex() + m_strPromptText.GetLength(), GetWindowTextLength());
    ReplaceSel(lpszCommand);
    SetSel(-1, -1);
}

void CConsoleEdit::AppendText(LPCTSTR lpszText)
{
    int_t len = GetWindowTextLength();
    SetSel(len,len);
    ReplaceSel(lpszText ? lpszText : _T(""));
    SetSel(-1, -1);
}

BOOL CConsoleEdit::IsEditingLine()
{
    return (LineFromChar() == (GetLineCount() - 1));
}

BOOL CConsoleEdit::IsPromptArea(int_t nSpace)
{
    int_t nLine, nPos;
    GetCaretPos(nLine, nPos);
    return (IsEditingLine() && (nPos < (m_strPromptText.GetLength() + nSpace)));
}

void CConsoleEdit::GetSelectedText(char_t* szSel, int_t iLength)
{
    int_t ii, iStart, iEnd, iLimit;

    iLimit = (int_t)GetLimitText();

    szSel[0] = _T('\0');

    if ((iLength < 1) || (iLength > iLimit)) {
        return;
    }

    GetSel(iStart, iEnd);
    if ((iStart < 0) || (iStart > iLimit) || (iEnd < 0)  || (iEnd > iLimit) || (iStart >= iEnd)) {
        return;
    }

    if ((iEnd - iStart) > iLength) {
        return;
    }

    int_t nLengthL = GetLine(LineFromChar(), (char_t*)szSel, iLength);
    if ((nLengthL <= 0) || (nLengthL >= ML_STRSIZE)) {
        return;
    }
    szSel[nLengthL] = _T('\0');
    for (ii = iStart; ii < iEnd; ii++) {
        szSel[ii - iStart] = szSel[ii];
    }
    szSel[iEnd - iStart] = _T('\0');
    return;
}

// CConsoleEdit message handlers

BOOL CConsoleEdit::PreCreateWindow(CREATESTRUCT& cs) 
{
    // TODO
    cs.style |= (ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL);
    return CEdit::PreCreateWindow(cs);
}

int_t CConsoleEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CEdit::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    // TODO
    m_strPromptText = _T("$ ");
    m_aCommandHistory.RemoveAll();
    m_aCommandHistory.SetSize(CONSOLE_HISTORY);
    SetSel(-1,-1);

    return 0;
}

void CConsoleEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_DELETE) {
        if (!IsEditingLine() || IsPromptArea(0)) {
            return;
        }
    }
    else if (nChar == VK_LEFT) {
        if (IsPromptArea(0)) {
            return;
        }
    }
    else if (nChar == VK_UP) {
        if (m_nTotalCommand < 1) {
            return;
        }
        if (m_aCommandHistory.GetUpperBound() >= 0) {
            m_nCurCommand -= 1;
            if (m_nCurCommand < 0) {
                m_nCurCommand = m_nTotalCommand - 1;
            }
            SetCurCommand(m_aCommandHistory[m_nCurCommand]);
        }
        return;
    }
    else if (nChar == VK_DOWN) {
        if (m_nTotalCommand < 1) {
            return;
        }
        if (m_aCommandHistory.GetUpperBound() >= 0) {
            m_nCurCommand += 1;
            if (m_nCurCommand >= m_nTotalCommand) {
                m_nCurCommand = 0;
            }
            SetCurCommand(m_aCommandHistory[m_nCurCommand]);
        }
        return;
    }

    CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

    if (IsEditingLine()) {
        int_t nStart, nEnd;
        GetSel(nStart, nEnd);
        if (nStart != nEnd) {
            if (nStart - LineIndex() < m_strPromptText.GetLength()) {
                SetSel(LineIndex() + m_strPromptText.GetLength(), nEnd);
            }
        }
        else {
            if (IsPromptArea(1)) {
                SetCaretPos(m_strPromptText.GetLength());
            }
        }
    }
}

void CConsoleEdit::DoParse(int_t nBegin)
{
    if ((nBegin < 0) || (nBegin >= (ML_STRSIZE - 1))) {
        return;
    }

    BOOL bPrintOutput = TRUE;

    SetCaretPos();
    LPTSTR pszT = &m_szCommand[nBegin];
    int_t nLength = (int_t)_tcslen((const char_t*)pszT);
    if ((nLength < 1) || (nLength >= ML_STRSIZE)) {
        return;
    }
    if (pszT[nLength - 1] == _T(';')) {
        pszT[nLength - 1] = _T('\0');
        bPrintOutput = FALSE;
    }

    char_t szOutput[ML_STRSIZE];
    SigmaApp.DoParse((const char_t*)pszT, szOutput);
    if (bPrintOutput && (szOutput[0] != _T('\0'))) {
        CarryReturn();
        AppendTextT((LPCTSTR)szOutput);
    }

    if (pszT[0] != _T('\0')) {
        int_t nHistorySize = (int_t) (m_aCommandHistory.GetSize());
        if (m_nTotalCommand >= nHistorySize) {
            for (int_t ii = 0; ii < nHistorySize - 1; ii++) {
                m_aCommandHistory[ii] = m_aCommandHistory[ii + 1];
            }
            m_nTotalCommand -= 1;
        }
        if (bPrintOutput == FALSE) {
            pszT[nLength - 1] = _T(';');
            pszT[nLength] = _T('\0');
        }
        m_aCommandHistory[m_nTotalCommand++] = pszT;
        m_nCurCommand = m_nTotalCommand;
    }

    return;
}

void CConsoleEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    // TODO
    if (nChar == VK_RETURN) {
        int_t nLineNo = GetLineCount() - 1;
        int_t nLength = LineLength(), nLengthL;
        if ((nLength > 0) && (nLength < (ML_STRSIZE - 1))) {
            nLengthL = GetLine(nLineNo, (char_t*)m_szCommand, nLength + 1);
            if ((nLengthL < 2) || (nLengthL > nLength)) {
                CEdit::OnChar(nChar, nRepCnt, nFlags);
                return;
            }
            m_szCommand[nLengthL] = _T('\0');

            DoParse(m_strPromptText.GetLength());
        }
        AppendText(CRLF);
        AppendText(m_strPromptText);
        return;
    }
    else if (nChar == VK_DELETE) {
        if (!IsEditingLine() || IsPromptArea(0)) {
            return;
        }
    }
    else if (nChar == VK_BACK) {
        if (!IsEditingLine() || IsPromptArea(0)) {
            return;
        }
        if (IsPromptArea(1)) {
            int_t nStart, nEnd;
            GetSel(nStart, nEnd);
            if (nStart < nEnd) {
                if ((nStart - LineIndex()) < m_strPromptText.GetLength()) {
                    return;
                }
            }
            else {
                return;
            }
        }
    }
    else {
        if ((::GetKeyState(VK_SHIFT) >= 0) && (::GetKeyState(VK_CONTROL)) >= 0 && !IsEditingLine()) {
            SetCaretPos();
        }
    }

    CEdit::OnChar(nChar, nRepCnt, nFlags);
}

LRESULT CConsoleEdit::DefWindowProc(UINT messageT, WPARAM wParam, LPARAM lParam)
{
    // TODO
    if ((messageT == WM_CLEAR || messageT == WM_CUT) && (!IsEditingLine() || IsPromptArea(0))) {
        return 0;
    }

    return CEdit::DefWindowProc(messageT, wParam, lParam);
}

HBRUSH CConsoleEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    // Deselect text
    if (m_bInitialized == FALSE) {
        SetSel(-1, -1);
        m_bInitialized = TRUE;
    }

    // TODO:  Return a non-NULL brush if the parent's handler should not be called
    return NULL;
}

void CConsoleEdit::OnContextMenu(CWnd* /*pWnd*/, CPoint tPoint)
{
    CMenu contextMenu; 
    CMenu *pMenu; 

    // which popmenu to display ?
    contextMenu.LoadMenu(IDM_CONSOLE_POP);

    pMenu = contextMenu.GetSubMenu(0);

    if (pMenu == NULL) {
        return;
    }

    if (ml_angleunit == 0x00) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
    else if (ml_angleunit == 0x01) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
    else if (ml_angleunit == 0x02) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_CHECKED);
    }

    int_t nStart, nEnd;
    GetSel(nStart, nEnd);
    if (nStart >= nEnd) {
        pMenu->EnableMenuItem(ID_CONSOLE_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }
    else {
        pMenu->EnableMenuItem(ID_CONSOLE_COPY, MF_BYCOMMAND | MF_ENABLED);
    }

    if (!IsEditingLine() || IsPromptArea(0)) {
            pMenu->EnableMenuItem(ID_CONSOLE_CUT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            pMenu->EnableMenuItem(ID_CONSOLE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }
    else {
        if (nStart >= nEnd) {
            pMenu->EnableMenuItem(ID_CONSOLE_CUT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        }
        else {
            pMenu->EnableMenuItem(ID_CONSOLE_CUT, MF_BYCOMMAND | MF_ENABLED);
        }
        if (IsClipboardFormatAvailable(CF_TEXT)) {
            pMenu->EnableMenuItem(ID_CONSOLE_PASTE, MF_BYCOMMAND | MF_ENABLED);
        }
        else {
            pMenu->EnableMenuItem(ID_CONSOLE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        }
    }

    pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, GetParent(), NULL);
}

void CConsoleEdit::Return()
{
    SendMessage(WM_CHAR, (WPARAM)VK_RETURN, (LPARAM)1);
}

void CConsoleEdit::Back()
{
    SendMessage(WM_CHAR, (WPARAM)VK_BACK, (LPARAM)1);
}
