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

#pragma once

#include "../../LibGraph/LibFile/libfile.h"

#define CONSOLE_MAXLINE        1024
#define CONSOLE_HISTORY        16

// CConsoleEdit

class CConsoleEdit : public CEdit
{
    DECLARE_DYNAMIC(CConsoleEdit)

public:
    CConsoleEdit();
    virtual ~CConsoleEdit();

    void SetPromptText(LPCTSTR lpszPromptText) {
        m_strPromptText = lpszPromptText;
        AppendText(lpszPromptText);
    }
    void SetHistorySize(int_t nMaxCommand) {
        if ((nMaxCommand > 0) && (nMaxCommand <= CONSOLE_MAXLINE)) {
            m_aCommandHistory.SetSize(nMaxCommand);
        }
    }

    void CarryReturn(BOOL bLineFeed = TRUE);
    void AppendTextT(LPCTSTR szOut);
    void DoParse(int_t nBegin);

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

    afx_msg int_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
    char_t m_szCommand[ML_STRSIZE];
    CString m_strPromptText;

    BOOL m_bInitialized;

    CStringArray m_aCommandHistory;
    int_t m_nCurCommand, m_nTotalCommand;

    void AppendText(LPCTSTR lpszText = NULL);
    void SetCaretPos(int_t pos = -1);
    void GetCaretPos(int_t& nLine, int_t& nPos);

    void SetCurCommand(LPCTSTR lpszCommand);
    BOOL IsEditingLine();
    BOOL IsPromptArea(int_t nSpace);
    void GetSelectedText(char_t* szSel, int_t iLength);

    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    void Return();
    void Back();
};


