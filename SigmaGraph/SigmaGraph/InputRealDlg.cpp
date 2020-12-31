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
#include "InputRealDlg.h"


// CInputRealDlg dialog

IMPLEMENT_DYNAMIC(CInputRealDlg, CDialog)

CInputRealDlg::CInputRealDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CInputRealDlg::IDD, pParent)
{
    m_pfInput1 = NULL;
    m_pfInput2 = NULL;
    m_fInput1 = 0.0;
    m_fInput2 = 0.0;
    m_strTitle = _T("");
    m_strLabel1 = _T("");
    m_strLabel2 = _T("");
    m_bInitialized = FALSE;
    m_bIsModified = FALSE;
    m_bOK = FALSE;
    m_iLimit = 0;
}

CInputRealDlg::~CInputRealDlg()
{

}

void CInputRealDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDL_INPUT1, m_strLabel1);
    DDX_Control(pDX, IDE_INPUT1, m_edtInput1);
    DDX_Text(pDX, IDE_INPUT1, m_fInput1);
    DDX_Text(pDX, IDL_INPUT2, m_strLabel2);
    DDX_Control(pDX, IDE_INPUT2, m_edtInput2);
    DDX_Text(pDX, IDE_INPUT2, m_fInput2);
}


BEGIN_MESSAGE_MAP(CInputRealDlg, CDialog)
    ON_EN_CHANGE(IDE_INPUT, &CInputRealDlg::OnEditInput)
    ON_BN_CLICKED(IDOK, &CInputRealDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CInputRealDlg::OnCancel)
END_MESSAGE_MAP()


// CInputRealDlg message handlers

BOOL CInputRealDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_INPUT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    if (m_pfInput1 == NULL) {
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if (m_iLimit < 2) {
        m_iLimit = 2;
    }
    if (m_iLimit > ML_STRSIZE - 7) {
        m_iLimit = ML_STRSIZE - 7;
    }

    ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT1, EM_LIMITTEXT, (WPARAM)m_iLimit, (LPARAM)0);
    if (m_pfInput2 != NULL) {
        ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT2, EM_LIMITTEXT, (WPARAM)m_iLimit, (LPARAM)0);
    }

    if (m_pfInput2 == NULL) {
        m_strLabel2 = _T("");
        m_edtInput2.ShowWindow(SW_HIDE);
    }
    else {
        m_fInput2 = *m_pfInput2;
    }
    m_fInput1 = *m_pfInput1;

    if (m_strTitle.GetLength() > 1) {
        SetWindowText((LPCTSTR)m_strTitle);
    }

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputRealDlg::OnEditInput()
{
    m_bIsModified = TRUE;
}

void CInputRealDlg::OnOK()
{
    UpdateData(TRUE);

    int_t nlen = m_edtInput1.GetWindowTextLength();

    if ((nlen > 0) && (nlen <= m_iLimit)) {
        *m_pfInput1 = m_fInput1;
        if (m_pfInput2 != NULL) {
            nlen = m_edtInput2.GetWindowTextLength();
            if ((nlen > 0) && (nlen < m_iLimit)) {
                *m_pfInput2 = m_fInput2;
            }
        }
        m_bOK = TRUE;
        CDialog::OnOK();
        return;
    }

    CDialog::OnOK();
}

void CInputRealDlg::OnCancel()
{
    CDialog::OnCancel();
}
