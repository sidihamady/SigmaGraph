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
#include "VarsetDlg.h"


// CVarsetDlg dialog

IMPLEMENT_DYNAMIC(CVarsetDlg, CDialog)

CVarsetDlg::CVarsetDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CVarsetDlg::IDD, pParent)
{
    m_iLimit = 0;
    m_strValue = _T("");
    m_strName = _T("");
    m_bInitialized = FALSE;
    m_bOK = FALSE;
}

CVarsetDlg::~CVarsetDlg()
{

}

void CVarsetDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_INPUT_NAME, m_edtName);
    DDX_Control(pDX, IDE_INPUT_VALUE, m_edtValue);
}


BEGIN_MESSAGE_MAP(CVarsetDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CVarsetDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CVarsetDlg::OnCancel)
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CVarsetDlg message handlers

BOOL CVarsetDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    if (m_iLimit < 2) {
        m_iLimit = 2;
    }
    if (m_iLimit >= ML_STRSIZET) {
        m_iLimit = ML_STRSIZET - 1;
    }

    ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT_NAME, EM_LIMITTEXT, (WPARAM)m_iLimit, (LPARAM)0);
    ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT_VALUE, EM_LIMITTEXT, (WPARAM)m_iLimit, (LPARAM)0);

    m_edtValue.SetWindowText((LPCTSTR)m_strValue);
    m_edtName.SetWindowText((LPCTSTR)m_strName);

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CVarsetDlg::OnOK()
{
    UpdateData(TRUE);

    int_t nlen = m_edtName.GetWindowTextLength();
    int_t nlenv = m_edtValue.GetWindowTextLength();

    if ((nlen > 0) && (nlen <= m_iLimit) && (nlenv > 0) && (nlenv <= m_iLimit)) {
        m_edtValue.GetWindowText(m_strValue);
        m_edtName.GetWindowText(m_strName);

        m_bOK = TRUE;

        CDialog::OnOK();
        return;
    }

    CDialog::OnOK();
}

void CVarsetDlg::OnCancel()
{
    CDialog::OnCancel();
}

void CVarsetDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // Do not call CDialog::OnPaint() for painting messages

    if (m_bInitialized == FALSE) {
        m_edtValue.SetSel(-1, -1);
        m_edtName.SetSel(-1, -1);
        m_bInitialized = TRUE;
    }
}
