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
#include "ColumnDlg.h"
#include "SigmaDoc.h"

// CColumnDlg dialog

IMPLEMENT_DYNAMIC(CColumnDlg, CDialog)

CColumnDlg::CColumnDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CColumnDlg::IDD, pParent)
    , m_nPrefix(0)
    , m_nSuffix(3)
    , m_bShowLabel(TRUE)
    , m_bShowName(FALSE)
    , m_bShowBoth(FALSE)
    , m_bColumnX(FALSE)
    , m_bColumnY(FALSE)
    , m_bColumnBarX(FALSE)
    , m_bColumnBarY(FALSE)
    , m_nCurrentColumn(0)
    , m_nColumnCount(0)
{
    m_pColumn = NULL;
    m_pDoc = NULL;
    m_pSelectedColumnIndex = NULL;
}

CColumnDlg::~CColumnDlg()
{

}

void CColumnDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDO_COLUMN_SHOWLABEL, m_bShowLabel);
    DDX_Check(pDX, IDO_COLUMN_SHOWNAME, m_bShowName);
    DDX_Check(pDX, IDO_COLUMN_SHOWBOTH, m_bShowBoth);
    DDX_Control(pDX, IDE_COLUMN_LABEL, m_edtLabel);
    DDX_Control(pDX, IDC_COLUMN_FORMAT, m_cbFormat);
    DDX_Text(pDX, IDE_COLUMN_PREFIX, m_nPrefix);
    DDV_MinMaxInt(pDX, m_nPrefix, 0, 9);
    DDX_Text(pDX, IDE_COLUMN_SUFFIX, m_nSuffix);
    DDV_MinMaxInt(pDX, m_nSuffix, 0, 9);
    DDX_Check(pDX, IDO_COLUMN_X, m_bColumnX);
    DDX_Check(pDX, IDO_COLUMN_Y, m_bColumnY);
    DDX_Check(pDX, IDO_COLUMN_BARX, m_bColumnBarX);
    DDX_Check(pDX, IDO_COLUMN_BARY, m_bColumnBarY);
}


BEGIN_MESSAGE_MAP(CColumnDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CColumnDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CColumnDlg::OnCancel)
    ON_NOTIFY(UDN_DELTAPOS, IDS_COLUMN_CURRENT, &CColumnDlg::OnDeltaposColumnCurrent)
    ON_BN_CLICKED(IDAPPLY, &CColumnDlg::OnApply)
END_MESSAGE_MAP()


// CColumnDlg message handlers

void CColumnDlg::VariableToControl()
{
    m_edtLabel.SetWindowText((LPTSTR)(m_pColumn[m_nCurrentColumn].label));

    CComboBox *pComboFormat = NULL;

    pComboFormat = (CComboBox*)GetDlgItem(IDC_COLUMN_FORMAT);

    if (pComboFormat == NULL) {
        return;
    }

    int_t iFmt = -1, ilen = _tcslen((const char_t*)(m_pColumn[m_nCurrentColumn].format));
    for (int ii = 0; ii < ilen; ii++) {
        if ((m_pColumn[m_nCurrentColumn].format[ii] == _T('f')) || (m_pColumn[m_nCurrentColumn].format[ii] == _T('F'))) {
            iFmt = 0;
            break;
        }
        if ((m_pColumn[m_nCurrentColumn].format[ii] == _T('e')) || (m_pColumn[m_nCurrentColumn].format[ii] == _T('E'))) {
            iFmt = 1;
            break;
        }
    }
    pComboFormat->SetCurSel(iFmt);
    ml_getformat((const char_t*)(m_pColumn[m_nCurrentColumn].format), &m_nPrefix, &m_nSuffix);

    m_bShowLabel = ((m_pColumn[m_nCurrentColumn].opt & 0xF0) == 0x00) ?  TRUE : FALSE;
    m_bShowName = ((m_pColumn[m_nCurrentColumn].opt & 0xF0) == 0x10) ?  TRUE : FALSE;
    m_bShowBoth = ((m_pColumn[m_nCurrentColumn].opt & 0xF0) == 0x20) ?  TRUE : FALSE;

    m_bColumnX = ((m_pColumn[m_nCurrentColumn].type & 0xF0) == 0x10) ?  TRUE : FALSE;
    m_bColumnY = ((m_pColumn[m_nCurrentColumn].type & 0xF0) == 0x20) ?  TRUE : FALSE;
    m_bColumnBarX = ((m_pColumn[m_nCurrentColumn].type & 0xF0) == 0x30) ?  TRUE : FALSE;
    m_bColumnBarY = ((m_pColumn[m_nCurrentColumn].type & 0xF0) == 0x40) ?  TRUE : FALSE;

    ASSERT(m_pDoc);
    if (m_pDoc != NULL) {
        m_pDoc->UpdateAllViews(NULL);
    }

    UpdateData(FALSE);
}

BOOL CColumnDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ((m_pDoc == NULL) || (m_pColumn == NULL) || (m_pSelectedColumnIndex == NULL)
        || (m_nColumnCount < 1) || (m_nCurrentColumn < 0) || (m_nCurrentColumn >= m_nColumnCount)) {
        SigmaApp.Output(_T("Cannot show the properties dialog: invalid column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }


    ::SendDlgItemMessage(GetSafeHwnd(), IDS_COLUMN_CURRENT, UDM_SETRANGE, 0, (LPARAM)MAKELONG(m_nColumnCount-1,0));

    m_edtLabel.SetLimitText(ML_STRSIZEN - 1);

    CComboBox *pComboFormat = NULL;
    CRect rcCombo;

    pComboFormat = (CComboBox*)GetDlgItem(IDC_COLUMN_FORMAT);

    if (pComboFormat == NULL) {
        return FALSE;
    }

    pComboFormat->GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    pComboFormat->MoveWindow(rcCombo);
    pComboFormat->AddString(_T("Decimal 1000"));
    pComboFormat->AddString(_T("Scientific 1E3"));

    VariableToControl();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CColumnDlg::ControlToVariable()
{
    UpdateData(TRUE);    // Copy data to variables

    byte_t typeOld = m_pColumn[m_nCurrentColumn].type;
    byte_t typeNew = typeOld;
    if (m_bColumnX == TRUE) {
        typeNew = (typeOld & 0x0F) | 0x10;
    }
    else if (m_bColumnY == TRUE) {
        typeNew = (typeOld & 0x0F) | 0x20;
    }
    else if (m_bColumnBarX == TRUE) {
        typeNew = (typeOld & 0x0F) | 0x30;
    }
    else if (m_bColumnBarY == TRUE) {
        typeNew = (typeOld & 0x0F) | 0x40;
    }
    if (typeNew != typeOld) {
        m_pColumn[m_nCurrentColumn].type = typeNew;
    }
    //

    int_t iFmt = 0;
    char_t szTmp[14], szFmt[16];
    CComboBox* pComboFormat = (CComboBox*)GetDlgItem(IDC_COLUMN_FORMAT);
    if (pComboFormat == NULL) {
        return;
    }
    iFmt = pComboFormat->GetCurSel();
    if ((iFmt == CB_ERR) || (iFmt < 0) || (iFmt > 2)) {
        return;
    }

    szFmt[0] = _T('%');
    szFmt[1] = _T('\0');

    if ((m_nPrefix != 0) && (m_nSuffix != 0)) {
        _tcsprintf(szTmp, 14, _T("%01d.%01d"), m_nPrefix, m_nSuffix);
        _tcscat(szFmt, (const char_t*)szTmp);
    }
    else if ((m_nPrefix == 0) && (m_nSuffix != 0)) {
        _tcsprintf(szTmp, 14, _T(".%01d"), m_nSuffix);
        _tcscat(szFmt, (const char_t*)szTmp);
    }
    else if ((m_nPrefix != 0) && (m_nSuffix == 0)) {
        _tcsprintf(szTmp, 14, _T("%01d.0"), m_nPrefix, m_nSuffix);
        _tcscat(szFmt, (const char_t*)szTmp);
    }

    if (iFmt == 0) {
        _tcscat(szFmt, _T("f"));
    }
    else if (iFmt == 1) {
        _tcscat(szFmt, _T("e"));
    }

    _tcscpy(m_pColumn[m_nCurrentColumn].format, (const char_t*)szFmt);

    m_edtLabel.GetWindowText((LPTSTR)(m_pColumn[m_nCurrentColumn].label), ML_STRSIZEN - 1);

    vector_t *pColumnFit = NULL;
    int_t nlen = 0;
    if ((m_pColumn[m_nCurrentColumn].type & 0xF0) == 0x10) {    // X
        pColumnFit = ((CSigmaDoc*)m_pDoc)->GetColumn(1, -1, m_pColumn[m_nCurrentColumn].idf);
        if (pColumnFit != NULL) {
            pColumnFit = ((CSigmaDoc*)m_pDoc)->GetColumn(1, -1, pColumnFit->idx);
        }
    }
    else if ((m_pColumn[m_nCurrentColumn].type & 0xF0) == 0x20) {    // Y
        pColumnFit = ((CSigmaDoc*)m_pDoc)->GetColumn(1, -1, m_pColumn[m_nCurrentColumn].idf);
    }
    if (pColumnFit != NULL) {
        nlen = _tcslen((const char_t*)(m_pColumn[m_nCurrentColumn].label));
        if (nlen < (ML_STRSIZES - 6)) {
            _tcscpy(pColumnFit->label, (const char_t*)(m_pColumn[m_nCurrentColumn].label));
            _tcscat(pColumnFit->label, _T(" - Fit"));
        }
        else {
            _tcscpy(pColumnFit->label, (const char_t*)(pColumnFit->name));
        }
    }

    m_pColumn[m_nCurrentColumn].opt &= 0x0F;
    if (m_bShowName == TRUE) {
        m_pColumn[m_nCurrentColumn].opt &= 0x0F;
        m_pColumn[m_nCurrentColumn].opt |= 0x10;
    }
    else if (m_bShowBoth == TRUE) {
        m_pColumn[m_nCurrentColumn].opt &= 0x0F;
        m_pColumn[m_nCurrentColumn].opt |= 0x20;
    }

    if (typeNew != typeOld) {
        ((CSigmaDoc*)m_pDoc)->UpdateSigmaDoc();
    }
    m_pDoc->UpdateAllViews(NULL);
    m_pDoc->SetModifiedFlag(TRUE);
}

void CColumnDlg::OnOK()
{
    ControlToVariable();

    CDialog::OnOK();
}

void CColumnDlg::OnCancel()
{
    CDialog::OnCancel();
}

void CColumnDlg::OnDeltaposColumnCurrent(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    ControlToVariable();

    if (pNMUpDown->iDelta > 0) {
        m_nCurrentColumn += 1;
        if (m_nCurrentColumn >= m_nColumnCount) {
            m_nCurrentColumn = m_nColumnCount - 1;
            pNMUpDown->iPos = m_nCurrentColumn;
        }
    }
    else {
        m_nCurrentColumn -= 1;
        if (m_nCurrentColumn < 0) {
            m_nCurrentColumn = 0;
            pNMUpDown->iPos = 0;
        }
    }

    *m_pSelectedColumnIndex = m_nCurrentColumn + 1;

    VariableToControl();

    *pResult = 0;
}

void CColumnDlg::OnApply()
{
    ControlToVariable();
}
