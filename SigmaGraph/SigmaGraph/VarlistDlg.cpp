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
#include "VarlistDlg.h"
#include "VarDlg.h"
#include "VarsetDlg.h"


// CVarlistDlg dialog

IMPLEMENT_DYNAMIC(CVarlistDlg, CDialog)

CVarlistDlg::CVarlistDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CVarlistDlg::IDD, pParent)
{

}

CVarlistDlg::~CVarlistDlg()
{

}

void CVarlistDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDL_VARLIST, m_lstVar);
}


BEGIN_MESSAGE_MAP(CVarlistDlg, CDialog)
    ON_NOTIFY(NM_DBLCLK, IDL_VARLIST, &CVarlistDlg::OnNMDblclkVarlist)
    ON_BN_CLICKED(ID_VARLIST_REFRESH, &CVarlistDlg::OnVarlistRefresh)
    ON_BN_CLICKED(ID_VARLIST_SET, &CVarlistDlg::OnVarSet)
    ON_BN_CLICKED(ID_VARLIST_UNSET, &CVarlistDlg::OnVarUnset)
    ON_WM_SYSCOMMAND()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CVarlistDlg::UpdateVarlist()
{
    int_t ii, jj = 0, iindex;
    char_t szBuffer[ML_STRSIZEN];
    memset(szBuffer, 0, ML_STRSIZEN * sizeof(char_t));
    if (SigmaApp.m_nVarnum > 0) {
        m_lstVar.DeleteAllItems();
        for (ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {
            if (SigmaApp.m_bParam[ii]) {
                iindex = m_lstVar.InsertItem(jj, (const char_t*)(SigmaApp.m_szParam[ii]));
                SigmaApp.Reformat(SigmaApp.m_fParam[ii], szBuffer, ML_STRSIZEN - 1);
                m_lstVar.SetItemText(iindex, 1, (const char_t*)szBuffer);
                m_lstVar.SetItemData(iindex, ii + 1);
                jj += 1;
            }
        }
    }
    UpdateData(FALSE);
}

// CVarlistDlg message handlers
BOOL CVarlistDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    RECT recT;
    GetWindowRect(&recT);
    int_t nWidthName = ((recT.right - recT.left) * 30) / 100;
    int_t nWidthValue = ((recT.right - recT.left) * 50) / 100;

    m_lstVar.InsertColumn(0, _T("Name"), LVCFMT_LEFT, nWidthName);
    m_lstVar.InsertColumn(1, _T("Value"), LVCFMT_LEFT, nWidthValue);

    DWORD dwStyleEx = m_lstVar.GetExtendedStyle();
    dwStyleEx |= LVS_EX_FULLROWSELECT;
    m_lstVar.SetExtendedStyle(dwStyleEx);

    UpdateVarlist();

    return TRUE;
}

void CVarlistDlg::OnClose()
{
    ShowWindow(SW_HIDE);
}

void CVarlistDlg::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}

void CVarlistDlg::OnOK()
{
}

void CVarlistDlg::OnCancel()
{
    OnClose();
}


void CVarlistDlg::OnVarlistRefresh()
{
    UpdateVarlist();
}

void CVarlistDlg::OnNMDblclkVarlist(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    

    CVarDlg dlgVar;
    dlgVar.m_ptInit.x = pNMItemActivate->ptAction.x;
    dlgVar.m_ptInit.y = pNMItemActivate->ptAction.y;
    ClientToScreen(&(dlgVar.m_ptInit));

    RECT rcT = {0,0,0,0};
    GetClientRect(&rcT);
    dlgVar.m_recFrame.bottom = rcT.bottom;
    dlgVar.m_recFrame.left = rcT.left;
    dlgVar.m_recFrame.top = rcT.top;
    dlgVar.m_recFrame.right = rcT.right;
    ClientToScreen(&(dlgVar.m_recFrame));

    CPoint tPoint(pNMItemActivate->ptAction);
    UINT uFlags;
    int_t nItem = m_lstVar.HitTest(tPoint, &uFlags);
    if ((nItem < 0) || (nItem >= SigmaApp.m_nVarnum)) {
        return;
    }
    int_t ii = (int_t) (m_lstVar.GetItemData(nItem)) - 1;
    if ((ii < 0) || (ii >= SigmaApp.m_nVarnum)) {
        return;
    }

    CString strVarname = m_lstVar.GetItemText(nItem, 0);
    dlgVar.m_strTitle = _T("Variable ");
    dlgVar.m_strTitle += _T("<");
    dlgVar.m_strTitle += strVarname;
    dlgVar.m_strTitle += _T(">");
    dlgVar.m_strTitle += _T(" Value");

    dlgVar.m_fInput = SigmaApp.m_fParam[ii];
    dlgVar.m_iLimit = ML_STRSIZEN - 1;

    dlgVar.DoModal();

    SigmaApp.m_fParam[ii] = dlgVar.m_fInput;

    char_t szBuffer[ML_STRSIZEN];
    memset(szBuffer, 0, ML_STRSIZEN * sizeof(char_t));
    SigmaApp.Reformat(SigmaApp.m_fParam[ii], szBuffer, ML_STRSIZEN - 1);
    m_lstVar.SetItemText(nItem, 1, (const char_t*)szBuffer);
    m_lstVar.SetItemData(nItem, ii + 1);
    UpdateData(FALSE);

    *pResult = 0;
}

void CVarlistDlg::OnVarSet()
{
    CVarsetDlg dlgVarset;

    dlgVarset.m_iLimit = ML_STRSIZEN - 1;
    dlgVarset.DoModal();

    if (dlgVarset.m_bOK) {
        CString strT = dlgVarset.m_strName;
        strT += _T(" = ");
        strT += dlgVarset.m_strValue;

        char_t szOutput[ML_STRSIZE];
        memset(szOutput, 0, ML_STRSIZE * sizeof(char_t));
        char_t szBuffer[(ML_STRSIZET * 2) + 3];
        memset(szBuffer, 0, ((ML_STRSIZET * 2) + 3) * sizeof(char_t));

        int_t nlen = (int_t) (strT.GetLength());

        if (nlen > 3) {
            SigmaApp.CStringToChar(strT, szBuffer, (ML_STRSIZET * 2) + 2);
            SigmaApp.DoParse((const char_t*)szBuffer, szOutput);
            UpdateVarlist();
        }
    }
}

void CVarlistDlg::OnVarUnset()
{
    int_t nItem = m_lstVar.GetSelectionMark();
    if ((nItem < 0) || (nItem >= SigmaApp.m_nVarnum)) {
        return;
    }

    CString strUnset = _T("unset ");
    strUnset += m_lstVar.GetItemText(nItem, 0);

    int_t nlen = (int_t) (strUnset.GetLength());

    if (nlen > 6) {
        char_t szOutput[ML_STRSIZE];
        memset(szOutput, 0, ML_STRSIZE * sizeof(char_t));
        char_t szBuffer[(ML_STRSIZET * 2) + 6];
        memset(szBuffer, 0, ((ML_STRSIZET * 2) + 6) * sizeof(char_t));
        SigmaApp.CStringToChar(strUnset, szBuffer, (ML_STRSIZET * 2) + 5);
        SigmaApp.DoParse((const char_t*)szBuffer, szOutput);
        UpdateVarlist();
    }
}
