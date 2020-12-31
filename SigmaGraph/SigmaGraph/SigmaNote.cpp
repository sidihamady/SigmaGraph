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
#include "SigmaNote.h"


// CSigmaNote dialog

IMPLEMENT_DYNAMIC(CSigmaNote, CDialog)

CSigmaNote::CSigmaNote(CWnd* pParent /*=NULL*/)
    : CDialog(CSigmaNote::IDD, pParent)
{
    m_pDoc = NULL;
    m_pszNote = NULL;
    m_bInitialized = FALSE;
}

CSigmaNote::~CSigmaNote()
{

}

void CSigmaNote::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_NOTE, m_edtNote);
}


BEGIN_MESSAGE_MAP(CSigmaNote, CDialog)
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
    ON_WM_CHAR()
    ON_EN_CHANGE(IDE_NOTE, &CSigmaNote::OnEnChangeNote)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSigmaNote message handlers


BOOL CSigmaNote::OnInitDialog()
{
    CDialog::OnInitDialog();

    if (m_pDoc == NULL) {
        return FALSE;
    }

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_NOTE), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    m_edtNote.SetLimitText(ML_STRSIZEW - 1);
    CRect recTT;
    GetClientRect(recTT);
    m_edtNote.MoveWindow(recTT);

    m_edtNote.SetWindowText((LPCTSTR)m_pszNote);

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    m_bInitialized = TRUE;

    UpdateData(FALSE);

    return TRUE;
}

void CSigmaNote::OnClose()
{
    m_edtNote.GetWindowText((LPTSTR)m_pszNote, ML_STRSIZEW - 1);
    ShowWindow(SW_HIDE);
}

void CSigmaNote::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}

void CSigmaNote::OnOK()
{
}

void CSigmaNote::OnCancel()
{
    OnClose();
}


void CSigmaNote::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    if (m_bInitialized) {
        CRect recTT;
        GetClientRect(recTT);
        m_edtNote.MoveWindow(recTT);
    }
}

void CSigmaNote::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH3;
    lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT3;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

void CSigmaNote::OnEnChangeNote()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.


    if (m_pDoc) {
        m_pDoc->SetModifiedFlag(TRUE);
    }
}

BOOL CSigmaNote::PreTranslateMessage(MSG* pMsg)
{
    // [FIX-SG200-004] All keys work now (included Suppr). Only Ctrl+S is trapped by accelerators.
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if ((pMsg->message == WM_KEYDOWN) && CTRL_Pressed() && ((pMsg->wParam == _T('s')) || (pMsg->wParam == _T('S')))) {
            if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                return TRUE;
            }
        }
        if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_F1)) {
            if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                HWND hWnd = ::HtmlHelp(0, (LPCTSTR) (SigmaApp.m_szHelpFilename), HH_DISPLAY_TOC, NULL);
                return TRUE;
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CSigmaNote::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CDialog::OnDestroy();
}

