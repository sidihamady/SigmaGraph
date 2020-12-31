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
#include "EditOutput.h"

#define OUTPUT_MSG_COPY                 _T("&Copy")
#define OUTPUT_MSG_CLEAR                _T("&Clear")
#define OUTPUT_MSG_SELECTALL            _T("Select &All")
#define WM_SELECTALL                    (WM_USER + 0x7000)

// CEditOutput

IMPLEMENT_DYNAMIC(CEditOutput, CReadonlyEdit)

CEditOutput::CEditOutput() : CReadonlyEdit()
{
    SetColors(::GetSysColor(COLOR_WINDOWTEXT), ::GetSysColor(COLOR_WINDOW));
}


CEditOutput::CEditOutput(COLORREF clrText, COLORREF clrBackground) : CReadonlyEdit(clrText, clrBackground)
{
    SetColors(::GetSysColor(COLOR_WINDOWTEXT), ::GetSysColor(COLOR_WINDOW));
}

CEditOutput::~CEditOutput()
{

}


BEGIN_MESSAGE_MAP(CEditOutput, CReadonlyEdit)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CEditOutput message handlers
void CEditOutput::OnContextMenu(CWnd* pWnd, CPoint tPoint)
{
    SetFocus();

    CMenu popMenu;
    if (popMenu.CreatePopupMenu() == FALSE) {
        return;
    }

    BOOL bReadOnly = GetStyle() & ES_READONLY;

    DWORD sel = GetSel();
    DWORD flags = LOWORD(sel) == HIWORD(sel) ? MF_GRAYED : MF_ENABLED;
    popMenu.InsertMenu(0, MF_BYPOSITION | flags, WM_COPY, OUTPUT_MSG_COPY);

    int nLen = GetWindowTextLength();
    flags = ((nLen < 1) || ((LOWORD(sel) == 0) && (HIWORD(sel) == nLen))) ? MF_GRAYED : MF_ENABLED;
    popMenu.InsertMenu(1, MF_BYPOSITION | flags, WM_SELECTALL, OUTPUT_MSG_SELECTALL);

    flags = (nLen < 1) ? MF_GRAYED : MF_ENABLED;
    popMenu.InsertMenu(2, MF_BYPOSITION | flags, WM_CLEAR, OUTPUT_MSG_CLEAR);

    popMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, this);
}

BOOL CEditOutput::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam)) {
        case EM_UNDO:
        case WM_CUT:
        case WM_PASTE:
            return TRUE;
        case WM_COPY:
            return SendMessage(LOWORD(wParam));
        case WM_CLEAR:
            SigmaApp.OutputClear();
            return TRUE;
        case WM_SELECTALL:
            return SendMessage (EM_SETSEL, 0, -1);
        default:
            return CReadonlyEdit::OnCommand(wParam, lParam);
    }

     return CReadonlyEdit::OnCommand(wParam, lParam);
}
