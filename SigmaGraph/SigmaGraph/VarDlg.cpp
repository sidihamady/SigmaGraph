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
#include "VarDlg.h"


// CVarDlg dialog

IMPLEMENT_DYNAMIC(CVarDlg, CDialog)

CVarDlg::CVarDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CVarDlg::IDD, pParent)
{
    m_iLimit = 0;
    m_fInput = 0.0;
    m_bInitialized = FALSE;
    m_ptInit.x = m_ptInit.y = 0;
}

CVarDlg::~CVarDlg()
{

}

void CVarDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_INPUT, m_edtInput);
}


BEGIN_MESSAGE_MAP(CVarDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CVarDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CVarDlg::OnCancel)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CVarDlg message handlers

BOOL CVarDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    if (m_iLimit < 2) {
        m_iLimit = 2;
    }
    if (m_iLimit >= ML_STRSIZEN) {
        m_iLimit = ML_STRSIZEN - 1;
    }

    RECT rcT = {0,0,0,0};
    GetWindowRect(&rcT);
    ClientToScreen(&rcT);
    LONG ww = rcT.right - rcT.left, hh = rcT.bottom - rcT.top;
    if ((m_ptInit.x + ww) > m_recFrame.right) {
        m_ptInit.x = m_recFrame.right - ww;
    }
    if ((m_ptInit.y + hh) > m_recFrame.bottom) {
        m_ptInit.y = m_recFrame.bottom - hh;
    }

    ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT, EM_LIMITTEXT, (WPARAM)m_iLimit, (LPARAM)0);

    SetWindowPos(NULL, m_ptInit.x, m_ptInit.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    if (m_strTitle.GetLength() > 1) {
        SetWindowText((LPCTSTR)m_strTitle);
    }

    char_t szBuffer[ML_STRSIZEN];
    memset(szBuffer, 0, ML_STRSIZEN * sizeof(char_t));
    SigmaApp.Reformat(m_fInput, szBuffer, ML_STRSIZEN - 1);
    m_edtInput.SetWindowText((LPCTSTR)szBuffer);

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CVarDlg::OnOK()
{
    UpdateData(TRUE);

    int_t nlen = m_edtInput.GetWindowTextLength();

    if ((nlen > 0) && (nlen <= m_iLimit)) {
        char_t szOutput[ML_STRSIZE];
        memset(szOutput, 0, ML_STRSIZE * sizeof(char_t));
        char_t szBuffer[ML_STRSIZEN];
        memset(szBuffer, 0, ML_STRSIZEN * sizeof(char_t));
        m_edtInput.GetWindowText(szBuffer, ML_STRSIZEN - 1);
        nlen = (int_t)_tcslen(szBuffer);

        if (nlen < 2) {
            m_fInput = ml_toreal((const char_t*)szBuffer, NULL, NULL);
        }
        else {
            SigmaApp.DoParse((const char_t*)szBuffer, szOutput, &m_fInput);
        }

        CDialog::OnOK();
        return;
    }

    CDialog::OnOK();
}

void CVarDlg::OnCancel()
{
    CDialog::OnCancel();
}

void CVarDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // Do not call CDialog::OnPaint() for painting messages

    if (m_bInitialized == FALSE) {
        m_edtInput.SetSel(-1, -1);
        m_bInitialized = TRUE;
    }
}
