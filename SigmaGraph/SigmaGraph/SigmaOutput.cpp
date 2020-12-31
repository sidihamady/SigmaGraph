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
#include "SigmaOutput.h"
#include "MainFrm.h"


// CSigmaOutput dialog

IMPLEMENT_DYNAMIC(CSigmaOutput, CDialog)

CSigmaOutput::CSigmaOutput(CWnd* pParent /*=NULL*/)
    : CDialog(CSigmaOutput::IDD, pParent)
    , m_strOutput(_T(""))
{
    m_bInitialized = FALSE;
}

CSigmaOutput::~CSigmaOutput()
{

}

void CSigmaOutput::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_OUTPUT, m_edtOutput);
    DDX_Text(pDX, IDE_OUTPUT, m_strOutput);
    DDV_MaxChars(pDX, m_strOutput, ML_STRSIZEL);
}


BEGIN_MESSAGE_MAP(CSigmaOutput, CDialog)
    ON_COMMAND(ID_OUTPUT_CLEAR, &CSigmaOutput::OnOutputReset)
    ON_WM_GETMINMAXINFO()
    ON_WM_NCRBUTTONDOWN()
    ON_WM_SIZE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CSigmaOutput::OnInitDialog()
{
    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_OUTPUT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    m_bInitialized = TRUE;

    m_edtOutput.SetLimitText(ML_STRSIZEL);

    CRect rcT;
    GetClientRect(rcT);
    m_edtOutput.MoveWindow(rcT);

    m_edtOutput.SetColors(::GetSysColor(COLOR_WINDOWTEXT), ::GetSysColor(COLOR_WINDOW));

    UpdateData(FALSE);

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    return TRUE;
}

void CSigmaOutput::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}

void CSigmaOutput::OnOK()
{

}

void CSigmaOutput::OnCancel()
{
    OnClose();
}

void CSigmaOutput::OnClose()
{
    ShowWindow(SW_HIDE);
}

// CSigmaOutput message handlers
void CSigmaOutput::OnSize(UINT nType, int_t cx, int_t cy) 
{
    CDialog::OnSize(nType, cx, cy);

    if (m_bInitialized) {
        CRect recTT;
        GetClientRect(recTT);
        recTT.DeflateRect(1,1);
        m_edtOutput.MoveWindow(recTT);
    }
}

void CSigmaOutput::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = SIGMA_MINWIDTH3;
    lpMMI->ptMinTrackSize.y = SIGMA_MINHEIGHT3;

    CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CSigmaOutput::PreTranslateMessage(MSG* pMsg)
{
    // [FIX-SG200-004] All keys work now (included Suppr). Only Ctrl+S is trapped by accelerators.
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if (CTRL_Pressed()) {
                if ((pMsg->wParam == _T('s')) || (pMsg->wParam == _T('S'))) {
                    if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                        return TRUE;
                    }
                }
            }
            else if ((pMsg->wParam == VK_F8)) {
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
                if (pFrame) {
                    pFrame->OnCmdMsg(ID_VIEW_OUTPUT, 0, NULL, NULL);
                    return TRUE;
                }
            }
            else if ((pMsg->wParam == VK_F9)) {
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
                if (pFrame) {
                    pFrame->OnCmdMsg(ID_VIEW_CONSOLE, 0, NULL, NULL);
                    return TRUE;
                }
            }
            else if ((pMsg->wParam == VK_F11)) {
                CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
                if (pFrame) {
                    pFrame->OnCmdMsg(ID_VIEW_SCRIPT, 0, NULL, NULL);
                    return TRUE;
                }
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CSigmaOutput::OnNcRButtonDown(UINT nHitTest, CPoint tPoint)
{
    // display ad hoc popmenu
    CMenu contextMenu; 
    CMenu *pMenu = NULL;
    contextMenu.LoadMenu(IDM_OUTPUTPOP);
    pMenu = contextMenu.GetSubMenu(0);
    //<<

    if (pMenu != NULL) {
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, this, NULL);
    }
}

void CSigmaOutput::OnOutputReset()
{
    SigmaApp.OutputClear();
}

void CSigmaOutput::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}
