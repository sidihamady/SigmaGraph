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
#include "InputTextDlg.h"


// CInputTextDlg dialog

IMPLEMENT_DYNAMIC(CInputTextDlg, CDialog)

CInputTextDlg::CInputTextDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CInputTextDlg::IDD, pParent)
{
    m_pszInput1 = NULL;
    m_strTitle = _T("Input");
    m_strLabel1 = _T("");
    m_strInput1 = _T("");
    m_pszInput2 = NULL;
    m_strLabel2 = _T("");
    m_strInput2 = _T("");
    m_bInitialized = FALSE;
    m_bIsModified = FALSE;
    m_bOK = FALSE;
    m_iLimit = 0;
}

CInputTextDlg::~CInputTextDlg()
{

}

void CInputTextDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDL_INPUT1, m_strLabel1);
    DDX_Control(pDX, IDE_INPUT1, m_edtInput1);
    DDX_Text(pDX, IDE_INPUT1, m_strInput1);
    DDX_Text(pDX, IDL_INPUT2, m_strLabel2);
    DDX_Control(pDX, IDE_INPUT2, m_edtInput2);
    DDX_Text(pDX, IDE_INPUT2, m_strInput2);
}


BEGIN_MESSAGE_MAP(CInputTextDlg, CDialog)
    ON_EN_CHANGE(IDE_INPUT, &CInputTextDlg::OnEditInput)
    ON_BN_CLICKED(IDOK, &CInputTextDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CInputTextDlg::OnCancel)
END_MESSAGE_MAP()


// CInputTextDlg message handlers

BOOL CInputTextDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_INPUT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    if (m_pszInput1 == NULL) {
        CDialog::EndDialog(-1);
        return FALSE;
    }

    SetWindowText((LPCTSTR)m_strTitle);

    if (m_iLimit < 1) {
        m_iLimit = 1;
    }
    if (m_iLimit > ML_STRSIZE - 7) {
        m_iLimit = ML_STRSIZE - 7;
    }

    ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT1, EM_LIMITTEXT, (WPARAM)m_iLimit, (LPARAM)0);
    if (m_pszInput2 != NULL) {
        ::SendDlgItemMessage(GetSafeHwnd(), IDE_INPUT2, EM_LIMITTEXT, (WPARAM)m_iLimit, (LPARAM)0);
    }

    m_strInput1 = (LPCTSTR)m_pszInput1;

    if (m_pszInput2 == NULL) {
        m_strLabel2 = _T("");
        m_edtInput2.ShowWindow(SW_HIDE);
    }
    else {
        m_strInput2 = (LPCTSTR)m_pszInput2;
    }

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputTextDlg::OnEditInput()
{
    m_bIsModified = TRUE;
}

void CInputTextDlg::OnOK()
{
    UpdateData(TRUE);

    int_t nlen = SigmaApp.CStringToChar(m_strInput1, m_pszInput1, m_iLimit);

    if ((nlen > 0) && (nlen < m_iLimit)) {
        if (m_pszInput2 != NULL) {
            nlen = SigmaApp.CStringToChar(m_strInput2, m_pszInput2, m_iLimit);
        }
        m_bOK = TRUE;
        CDialog::OnOK();
        return;
    }

    CDialog::OnOK();
}

void CInputTextDlg::OnCancel()
{
    CDialog::OnCancel();
}
