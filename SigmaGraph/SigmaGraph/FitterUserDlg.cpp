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
#include "FitterUserDlg.h"

// CFitterUserDlg dialog

IMPLEMENT_DYNAMIC(CFitterUserDlg, CDialog)

CFitterUserDlg::CFitterUserDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFitterUserDlg::IDD, pParent)
{
    m_nModelSelected = 0;
    m_nDerivSelected = -1;
    m_bOK = FALSE;
    m_fVal = 0.0;
}

CFitterUserDlg::~CFitterUserDlg()
{

}

void CFitterUserDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_FITTER_PARA, m_edtParA);
    DDX_Control(pDX, IDE_FITTER_PARB, m_edtParB);
    DDX_Control(pDX, IDE_FITTER_PARC, m_edtParC);
    DDX_Control(pDX, IDE_FITTER_PARD, m_edtParD);
    DDX_Control(pDX, IDE_FITTER_PARE, m_edtParE);
    DDX_Control(pDX, IDE_FITTER_PARF, m_edtParF);
}


BEGIN_MESSAGE_MAP(CFitterUserDlg, CDialog)
    ON_NOTIFY(UDN_DELTAPOS, IDS_USERMODEL_PARAMCOUNT, &CFitterUserDlg::OnDeltaposUsermodelParamcount)
    ON_BN_CLICKED(ID_USERMODEL_UPDATE, &CFitterUserDlg::OnUpdate)
    ON_BN_CLICKED(ID_USERMODEL_ADD, &CFitterUserDlg::OnAdd)
    ON_BN_CLICKED(ID_USERMODEL_REMOVE, &CFitterUserDlg::OnRemove)
    ON_CBN_SELCHANGE(IDC_USERMODEL_LIST, &CFitterUserDlg::OnCbnSelchangeModel)
    ON_CBN_SELCHANGE(IDC_USERMODEL_DERIVATIVE, &CFitterUserDlg::OnCbnSelchangeDerivative)
    ON_BN_CLICKED(IDOK, &CFitterUserDlg::OnOK)
END_MESSAGE_MAP()


// CFitterUserDlg message handlers

void CFitterUserDlg::VariableToControl()
{
    CEdit *pEditName = NULL, *pEditFunction = NULL, *pEditDerivative = NULL;
    CSpinButtonCtrl* pSpinParamcount = NULL;

    pEditName = (CEdit*)GetDlgItem(IDE_USERMODEL_NAME);
    pEditFunction = (CEdit*)GetDlgItem(IDE_USERMODEL_FUNCTION);
    pEditDerivative = (CEdit*)GetDlgItem(IDE_USERMODEL_DERIVATIVE);
    pSpinParamcount = (CSpinButtonCtrl*)GetDlgItem(IDS_USERMODEL_PARAMCOUNT);
    CComboBox *pModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
    CComboBox *pDerivative = (CComboBox*)GetDlgItem(IDC_USERMODEL_DERIVATIVE);

    if ((pEditName == NULL) || (pEditFunction == NULL) || (pEditDerivative == NULL)
        || (pSpinParamcount == NULL) || (pModel == NULL) || (pDerivative == NULL)) {
        return;
    }

    m_nModelSelected = pModel->GetCurSel();
    if ((m_nModelSelected < 0) || (m_nModelSelected >= SigmaApp.m_iUserFitCount)) {
        m_nModelSelected = 0;
        pModel->SetCurSel(m_nModelSelected);
        UpdateData(TRUE);
    }

    int iParcount = SigmaApp.m_pUserFit[m_nModelSelected]->parcount;

    if (SigmaApp.m_iUserFitCount < 1) {
        iParcount = 3;
        pModel->ResetContent();
        pSpinParamcount->SetPos(3);
        SetDlgItemInt(IDL_USERMODEL_PARAMCOUNT, 3, FALSE);
    }
    else {
        pModel->ResetContent();
        int_t jj;
        for (jj = 0; jj < SigmaApp.m_iUserFitCount; jj++) {
            pModel->AddString((LPCTSTR) (SigmaApp.m_pUserFit[jj]->name));
        }
    }
    pModel->SetCurSel(m_nModelSelected);

    pDerivative->AddString(_T("a"));
    pDerivative->AddString(_T("b"));
    if (iParcount > 2) {
        pDerivative->AddString(_T("c"));
    }
    if (iParcount > 3) {
        pDerivative->AddString(_T("d"));
    }
    if (iParcount > 4) {
        pDerivative->AddString(_T("e"));
    }
    if (iParcount > 5) {
        pDerivative->AddString(_T("f"));
    }
    pDerivative->SetCurSel(0);

    m_nDerivSelected = pDerivative->GetCurSel();
    if ((m_nDerivSelected < 0) || (m_nDerivSelected >= ML_FIT_MAXPAR)) {
        m_nDerivSelected = 0;
        pDerivative->SetCurSel(m_nDerivSelected);
        UpdateData(TRUE);
    }

    pEditName->SetWindowText((LPCTSTR) (SigmaApp.m_pUserFit[m_nModelSelected]->name));
    pEditFunction->SetWindowText((LPCTSTR) (SigmaApp.m_pUserFit[m_nModelSelected]->function));
    pEditDerivative->SetWindowText((LPCTSTR) (SigmaApp.m_pUserFit[m_nModelSelected]->derivative[m_nDerivSelected]));
    if (iParcount > 1) {
        pSpinParamcount->SetPos(iParcount);
        SetDlgItemInt(IDL_USERMODEL_PARAMCOUNT, iParcount, FALSE);
    }

    m_edtParA.SetValue(SigmaApp.m_pUserFit[m_nModelSelected]->fpar[0]);
    m_edtParB.SetValue(SigmaApp.m_pUserFit[m_nModelSelected]->fpar[1]);

    if (iParcount > 2) {
        m_edtParC.SetValue(SigmaApp.m_pUserFit[m_nModelSelected]->fpar[2]);
        m_edtParC.EnableWindow(TRUE);
    }
    else {
        m_edtParC.SetWindowText(_T(""));
        m_edtParC.EnableWindow(FALSE);
    }

    if (iParcount > 3) {
        m_edtParD.SetValue(SigmaApp.m_pUserFit[m_nModelSelected]->fpar[3]);
        m_edtParD.EnableWindow(TRUE);
    }
    else {
        m_edtParD.SetWindowText(_T(""));
        m_edtParD.EnableWindow(FALSE);
    }

    if (iParcount > 4) {
        m_edtParE.SetValue(SigmaApp.m_pUserFit[m_nModelSelected]->fpar[4]);
        m_edtParE.EnableWindow(TRUE);
    }
    else {
        m_edtParE.SetWindowText(_T(""));
        m_edtParE.EnableWindow(FALSE);
    }

    if (iParcount > 5) {
        m_edtParF.SetValue(SigmaApp.m_pUserFit[m_nModelSelected]->fpar[5]);
        m_edtParF.EnableWindow(TRUE);
    }
    else {
        m_edtParF.SetWindowText(_T(""));
        m_edtParF.EnableWindow(FALSE);
    }

    UpdateData(FALSE);
}

BOOL CFitterUserDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bOK = FALSE;

    CEdit *pEdit = (CEdit*)GetDlgItem(IDE_USERMODEL_NAME);
    if (pEdit == NULL) {
        return FALSE;
    }
    pEdit->SetLimitText(ML_STRSIZES - 1);

    pEdit = (CEdit*)GetDlgItem(IDE_USERMODEL_FUNCTION);
    if (pEdit == NULL) {
        return FALSE;
    }
    pEdit->SetLimitText(ML_STRSIZE - 1);

    pEdit = (CEdit*)GetDlgItem(IDE_USERMODEL_DERIVATIVE);
    if (pEdit == NULL) {
        return FALSE;
    }
    pEdit->SetLimitText(ML_STRSIZE - 1);

    ::SendDlgItemMessage(GetSafeHwnd(), IDS_USERMODEL_PARAMCOUNT, UDM_SETRANGE, 0, (LPARAM)MAKELONG(6,2));

    m_edtParA.SetLimitText(ML_STRSIZES - 1);
    m_edtParB.SetLimitText(ML_STRSIZES - 1);
    m_edtParC.SetLimitText(ML_STRSIZES - 1);
    m_edtParD.SetLimitText(ML_STRSIZES - 1);
    m_edtParE.SetLimitText(ML_STRSIZES - 1);
    m_edtParF.SetLimitText(ML_STRSIZES - 1);

    VariableToControl();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_FITTER), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFitterUserDlg::ControlToVariable()
{
    CEdit *pEditName = NULL, *pEditFunction = NULL, *pEditDerivative = NULL;
    CSpinButtonCtrl* pSpinParamcount = NULL;

    pEditName = (CEdit*)GetDlgItem(IDE_USERMODEL_NAME);
    pEditFunction = (CEdit*)GetDlgItem(IDE_USERMODEL_FUNCTION);
    pEditDerivative = (CEdit*)GetDlgItem(IDE_USERMODEL_DERIVATIVE);
    pSpinParamcount = (CSpinButtonCtrl*)GetDlgItem(IDS_USERMODEL_PARAMCOUNT);
    CComboBox *pModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
    CComboBox *pDerivative = (CComboBox*)GetDlgItem(IDC_USERMODEL_DERIVATIVE);

    if ((pEditName == NULL) || (pEditFunction == NULL) || (pEditDerivative == NULL)
        || (pSpinParamcount == NULL) || (pModel == NULL) || (pDerivative == NULL)) {
        return FALSE;
    }

    if (SigmaApp.m_iUserFitCount < 1) {
        pModel->ResetContent();
        return FALSE;
    }

    m_nModelSelected = pModel->GetCurSel();
    if ((m_nModelSelected < 0) || (m_nModelSelected >= SigmaApp.m_iUserFitCount)) {
        return FALSE;
    }

    m_nDerivSelected = pDerivative->GetCurSel();
    if ((m_nDerivSelected < 0) || (m_nDerivSelected >= ML_FIT_MAXPAR)) {
        m_nDerivSelected = 0;
        pDerivative->SetCurSel(m_nDerivSelected);
        UpdateData(FALSE);
    }

    int_t iParamcount = pSpinParamcount->GetPos();
    int_t jj;

    real_t fA = 0.0, fB = 0.0, fC = 0.0, fD = 0.0, fE = 0.0, fF = 0.0;

    UpdateData(TRUE);

    fA = m_edtParA.GetValue();
    fB = m_edtParB.GetValue();
    if (iParamcount > 2){
        fC = m_edtParC.GetValue();
    }
    if (iParamcount > 3){
        fD = m_edtParD.GetValue();
    }
    if (iParamcount > 4){
        fE = m_edtParE.GetValue();
    }
    if (iParamcount > 5){
        fF = m_edtParF.GetValue();
    }

    char_t szName[ML_STRSIZES], szFunction[ML_STRSIZE], szDerivative[ML_STRSIZE];
    int_t iLenA = 0, iLenB = 0, iLenC = 0;
    memset(szName, 0, ML_STRSIZES * sizeof(char_t));
    memset(szFunction, 0, ML_STRSIZE * sizeof(char_t));
    memset(szDerivative, 0, ML_STRSIZE * sizeof(char_t));

    pEditName->GetWindowText((LPTSTR)szName, ML_STRSIZES - 1);
    pEditFunction->GetWindowText((LPTSTR)szFunction, ML_STRSIZE - 1);
    pEditDerivative->GetWindowText((LPTSTR)szDerivative, ML_STRSIZE - 1);
    iLenA = (int_t)_tcslen((const char_t*)szName);
    iLenB = (int_t)_tcslen((const char_t*)szFunction);
    iLenC = (int_t)_tcslen((const char_t*)(szDerivative));
    if ((iParamcount < 2) || (iParamcount > ML_FIT_MAXPAR)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid number of parameters.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    if ((iLenA < 3) || (iLenA >= ML_STRSIZES)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid model name.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    if ((iLenB < 3) || (iLenB >= ML_STRSIZE)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    if ((iLenC < 1) || (iLenC >= ML_STRSIZE)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid derivative.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }

    // >> Verifs
    if (_tcschr(szFunction, _T('x')) == NULL) {
        SigmaApp.Output(_T("Cannot update fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    for (jj = 0; jj < iParamcount; jj++) {
        if (_tcschr(szFunction, _T('a') + jj) == NULL) {
            SigmaApp.Output(_T("Cannot update fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return FALSE;
        }
    }
    SigmaApp.m_pLib->errcode = 0;
    char_t *pszFunction = ml_parser_decode(szFunction, _T("abcdefx"), &iLenB, SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszFunction);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot update fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    real_t fT = ml_parser_evalf(SigmaApp.m_pLib, pszFunction, _T("abcdefx"), fA, fB, fC, fD, fE, fF);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszFunction);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot update fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    ml_parser_free(pszFunction);

    SigmaApp.m_pLib->errcode = 0;
    char_t *pszDerivative = ml_parser_decode(szDerivative, _T("abcdefx"), &iLenC, SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszDerivative);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot update fitting model: invalid derivative.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    fT = ml_parser_evalf(SigmaApp.m_pLib, pszDerivative, _T("abcdefx"), fA, fB, fC, fD, fE, fF);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszDerivative);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot update fitting model: invalid derivative.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return FALSE;
    }
    ml_parser_free(pszDerivative);
    // <<

    if (_tcsicmp((LPCTSTR)(SigmaApp.m_pUserFit[m_nModelSelected]->name), (const char_t*)szName) != 0) {
        _tcscpy(SigmaApp.m_pUserFit[m_nModelSelected]->name, (const char_t*)szName);
        pModel->ResetContent();
        if (SigmaApp.m_iUserFitCount > 0) {
            for (jj = 0; jj < SigmaApp.m_iUserFitCount; jj++) {
                pModel->AddString((LPCTSTR)(SigmaApp.m_pUserFit[jj]->name));
            }
        }
        pModel->SetCurSel(m_nModelSelected);
    }

    _tcscpy(SigmaApp.m_pUserFit[m_nModelSelected]->function, (const char_t*)szFunction);
    _tcscpy(SigmaApp.m_pUserFit[m_nModelSelected]->derivative[m_nDerivSelected], (const char_t*) szDerivative);

    SigmaApp.m_pUserFit[m_nModelSelected]->parcount = (int_t)GetDlgItemInt(IDL_USERMODEL_PARAMCOUNT, NULL, FALSE);

    SigmaApp.m_pUserFit[m_nModelSelected]->fpar[0] = fA;
    SigmaApp.m_pUserFit[m_nModelSelected]->fpar[1] = fB;
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 2){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[2] = fC;
    }
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 3){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[3] = fD;
    }
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 4){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[4] = fE;
    }
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 5){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[5] = fF;
    }

    return TRUE;
}

void CFitterUserDlg::OnOK()
{
    CString strT = _T("");
    int_t jj, iLen = 0;
    if (ControlToVariable() == TRUE) {
        for (jj = 0; jj < SigmaApp.m_pUserFit[m_nModelSelected]->parcount; jj++) {
            iLen = (int_t)_tcslen((const char_t*)(SigmaApp.m_pUserFit[m_nModelSelected]->derivative[jj]));
            if ((iLen < 1) || (iLen >= ML_STRSIZE)) {
                strT.Format(_T("Cannot add fitting model: invalid model derivative (%c).\r\n"), _T('a') + jj);
                SigmaApp.Output(strT, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                return;
            }
        }

        SigmaApp.setUserFit();
        m_bOK = TRUE;
        CDialog::OnOK();
        return;
    }

    m_bOK = FALSE;
    CDialog::OnCancel();
    return;
}

void CFitterUserDlg::OnUpdate()
{
    ControlToVariable();
}

void CFitterUserDlg::OnAdd()
{
    CEdit *pEditName = NULL, *pEditFunction = NULL, *pEditDerivative = NULL;
    CSpinButtonCtrl* pSpinParamcount = NULL;

    pEditName = (CEdit*)GetDlgItem(IDE_USERMODEL_NAME);
    pEditFunction = (CEdit*)GetDlgItem(IDE_USERMODEL_FUNCTION);
    pEditDerivative = (CEdit*)GetDlgItem(IDE_USERMODEL_DERIVATIVE);
    pSpinParamcount = (CSpinButtonCtrl*)GetDlgItem(IDS_USERMODEL_PARAMCOUNT);
    CComboBox *pModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
    CComboBox *pDerivative = (CComboBox*)GetDlgItem(IDC_USERMODEL_DERIVATIVE);

    if ((pEditName == NULL) || (pEditFunction == NULL) || (pEditDerivative == NULL)
        || (pSpinParamcount == NULL) || (pModel == NULL) || (pDerivative == NULL)) {
        return;
    }

    if (SigmaApp.m_iUserFitCount < 0) {
        SigmaApp.m_iUserFitCount = 0;
    }

    if (SigmaApp.m_iUserFitCount >= ML_FIT_MAXMODEL) {
        SigmaApp.Output(_T("Cannot add fitting model: limit (16) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    char_t szName[ML_STRSIZES], szFunction[ML_STRSIZE], szDerivative[ML_STRSIZE];
    int_t iParamcount = 0, iLenA = 0, iLenB = 0, iLenC = 0;
    memset(szName, 0, ML_STRSIZES * sizeof(char_t));
    memset(szFunction, 0, ML_STRSIZE * sizeof(char_t));
    memset(szDerivative, 0, ML_STRSIZE * sizeof(char_t));

    pEditName->GetWindowText((LPTSTR)szName, ML_STRSIZES - 1);
    pEditFunction->GetWindowText((LPTSTR)szFunction, ML_STRSIZE - 1);
    pEditDerivative->GetWindowText((LPTSTR)szDerivative, ML_STRSIZE - 1);
    iParamcount = pSpinParamcount->GetPos();
    iLenA = (int_t)_tcslen((const char_t*)szName);
    iLenB = (int_t)_tcslen((const char_t*)szFunction);
    iLenC = (int_t)_tcslen((const char_t*)szDerivative);
    if ((iParamcount < 2) || (iParamcount > ML_FIT_MAXPAR)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid number of parameters.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    if ((iLenA < 3) || (iLenA >= ML_STRSIZES)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid model name.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    if ((iLenB < 3) || (iLenB >= ML_STRSIZE)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    if ((iLenC < 1) || (iLenC >= ML_STRSIZE)) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid derivative.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    int_t jj;

    if (SigmaApp.m_iUserFitCount > 0) {
        for (jj = 0; jj < SigmaApp.m_iUserFitCount; jj++) {
            if (_tcsicmp((const char_t*)szName, (const char_t*)(SigmaApp.m_pUserFit[jj]->name)) == 0) {
                SigmaApp.Output(_T("A fitting model with the same name already exists.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
                return;
            }
            if (_tcsicmp((const char_t*)szFunction, (const char_t*)(SigmaApp.m_pUserFit[jj]->function)) == 0) {
                SigmaApp.Output(_T("A fitting model with the same function already exists.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_WARNING);
                return;
            }
        }
    }

    real_t fA = 0.0, fB = 0.0, fC = 0.0, fD = 0.0, fE = 0.0, fF = 0.0;

    UpdateData(TRUE);

    fA = m_edtParA.GetValue();
    fB = m_edtParB.GetValue();
    if (iParamcount > 2){
        fC = m_edtParC.GetValue();
    }
    if (iParamcount > 3){
        fD = m_edtParD.GetValue();
    }
    if (iParamcount > 4){
        fE = m_edtParE.GetValue();
    }
    if (iParamcount > 5){
        fF = m_edtParF.GetValue();
    }

    // >> Verifs
    if (_tcschr(szFunction, _T('x')) == NULL) {
        SigmaApp.Output(_T("Cannot add fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    for (jj = 0; jj < iParamcount; jj++) {
        if (_tcschr(szFunction, _T('a') + jj) == NULL) {
            SigmaApp.Output(_T("Cannot add fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            return;
        }
    }
    SigmaApp.m_pLib->errcode = 0;
    char_t *pszFunction = ml_parser_decode(szFunction, _T("abcdefx"), &iLenB, SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszFunction);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot add fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    real_t fT = ml_parser_evalf(SigmaApp.m_pLib, pszFunction, _T("abcdefx"), fA, fB, fC, fD, fE, fF, m_fVal);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszFunction);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot add fitting model: invalid function.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    ml_parser_free(pszFunction);

    SigmaApp.m_pLib->errcode = 0;
    char_t *pszDerivative = ml_parser_decode(szDerivative, _T("abcdefx"), &iLenC, SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszDerivative);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot update fitting model: invalid derivative.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    fT = ml_parser_evalf(SigmaApp.m_pLib, pszDerivative, _T("abcdefx"), fA, fB, fC, fD, fE, fF, m_fVal);
    if (SigmaApp.m_pLib->errcode != 0) {
        ml_parser_free(pszDerivative);
        SigmaApp.m_pLib->errcode = 0;
        SigmaApp.Output(_T("Cannot update fitting model: invalid derivative.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }
    ml_parser_free(pszDerivative);
    // <<

    m_nModelSelected = SigmaApp.m_iUserFitCount;
    SigmaApp.m_iUserFitCount += 1;

    int_t iDerivative = pDerivative->GetCurSel();
    if ((iDerivative < 0) || (iDerivative >= ML_FIT_MAXPAR)) {
        iDerivative = 0;
        pDerivative->SetCurSel(iDerivative);
        UpdateData(TRUE);
    }

    _tcscpy(SigmaApp.m_pUserFit[m_nModelSelected]->name, (const char_t*)szName);
    _tcscpy(SigmaApp.m_pUserFit[m_nModelSelected]->function, (const char_t*)szFunction);
    for (jj = 0; jj < SigmaApp.m_pUserFit[m_nModelSelected]->parcount; jj++) {
        memset(SigmaApp.m_pUserFit[m_nModelSelected]->derivative[jj], 0, ML_STRSIZE * sizeof(char_t));
    }
    _tcscpy(SigmaApp.m_pUserFit[m_nModelSelected]->derivative[iDerivative], (const char_t*)szDerivative);
    SigmaApp.m_pUserFit[m_nModelSelected]->parcount = iParamcount;
    SigmaApp.m_pUserFit[m_nModelSelected]->index = ML_FIT_USERINDEX;

    SigmaApp.m_pUserFit[m_nModelSelected]->fpar[0] = fA;
    SigmaApp.m_pUserFit[m_nModelSelected]->fpar[1] = fB;
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 2){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[2] = fC;
    }
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 3){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[3] = fD;
    }
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 4){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[4] = fE;
    }
    if (SigmaApp.m_pUserFit[m_nModelSelected]->parcount > 5){
        SigmaApp.m_pUserFit[m_nModelSelected]->fpar[5] = fF;
    }

    pModel->AddString((LPCTSTR)(SigmaApp.m_pUserFit[m_nModelSelected]->name));
    pModel->SetCurSel(m_nModelSelected);
}

void CFitterUserDlg::OnRemove()
{
    CEdit *pEditName = NULL, *pEditFunction = NULL, *pEditDerivative = NULL;
    CSpinButtonCtrl* pSpinParamcount = NULL;

    pEditName = (CEdit*)GetDlgItem(IDE_USERMODEL_NAME);
    pEditFunction = (CEdit*)GetDlgItem(IDE_USERMODEL_FUNCTION);
    pEditDerivative = (CEdit*)GetDlgItem(IDE_USERMODEL_DERIVATIVE);
    pSpinParamcount = (CSpinButtonCtrl*)GetDlgItem(IDS_USERMODEL_PARAMCOUNT);
    CComboBox *pModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
    CComboBox *pDerivative = (CComboBox*)GetDlgItem(IDC_USERMODEL_DERIVATIVE);

    if ((pEditName == NULL) || (pEditFunction == NULL) || (pEditDerivative == NULL)
        || (pSpinParamcount == NULL) || (pModel == NULL) || (pDerivative == NULL)) {
        return;
    }

    if (SigmaApp.m_iUserFitCount <= 0) {
        SigmaApp.m_iUserFitCount = 0;
        return;
    }

    int_t jj, mm;

    m_nModelSelected = pModel->GetCurSel();
    if ((m_nModelSelected < 0) || (m_nModelSelected >= SigmaApp.m_iUserFitCount)) {
        m_nModelSelected = 0;
        pModel->SetCurSel(m_nModelSelected);
        VariableToControl();
        return;
    }
    if (m_nModelSelected == (SigmaApp.m_iUserFitCount - 1)) {
        SigmaApp.m_iUserFitCount -= 1;
        m_nModelSelected = SigmaApp.m_iUserFitCount - 1;
        if (m_nModelSelected < 0) {
            m_nModelSelected = 0;
        }
        pModel->ResetContent();
        if (SigmaApp.m_iUserFitCount > 0) {
            for (jj = 0; jj < SigmaApp.m_iUserFitCount; jj++) {
                pModel->AddString((LPCTSTR)(SigmaApp.m_pUserFit[jj]->name));
            }
        }
        pModel->SetCurSel(m_nModelSelected);
        pDerivative->SetCurSel(0);
        VariableToControl();
        return;
    }

    for (jj = m_nModelSelected; jj < (SigmaApp.m_iUserFitCount - 1); jj++) {
        _tcscpy(SigmaApp.m_pUserFit[jj]->name, (const char_t*)(SigmaApp.m_pUserFit[jj + 1]->name));
        _tcscpy(SigmaApp.m_pUserFit[jj]->function, (const char_t*)(SigmaApp.m_pUserFit[jj + 1]->function));
        for (mm = 0; mm < SigmaApp.m_pUserFit[jj]->parcount; mm++) {
            SigmaApp.m_pUserFit[jj]->fpar[mm] = SigmaApp.m_pUserFit[jj + 1]->fpar[mm];
            _tcscpy(SigmaApp.m_pUserFit[jj]->derivative[mm], (const char_t*)(SigmaApp.m_pUserFit[jj + 1]->derivative[mm]));
        }
        SigmaApp.m_pUserFit[jj]->parcount = SigmaApp.m_pUserFit[jj + 1]->parcount;
        SigmaApp.m_pUserFit[jj]->id = SigmaApp.m_pUserFit[jj + 1]->id;
        SigmaApp.m_pUserFit[jj]->index = SigmaApp.m_pUserFit[jj + 1]->index;
    }
    SigmaApp.m_iUserFitCount -= 1;
    if ((m_nModelSelected < 0) || (m_nModelSelected >= SigmaApp.m_iUserFitCount)) {
        m_nModelSelected = 0;
    }
    pEditName->SetWindowText((LPCTSTR)(SigmaApp.m_pUserFit[m_nModelSelected]->name));
    pEditFunction->SetWindowText((LPCTSTR)(SigmaApp.m_pUserFit[m_nModelSelected]->function));
    pEditDerivative->SetWindowText((LPCTSTR)(SigmaApp.m_pUserFit[m_nModelSelected]->derivative[0]));
    pSpinParamcount->SetPos(SigmaApp.m_pUserFit[m_nModelSelected]->parcount);

    pModel->ResetContent();
    if (SigmaApp.m_iUserFitCount > 0) {
        for (jj = 0; jj < SigmaApp.m_iUserFitCount; jj++) {
            pModel->AddString((LPCTSTR)(SigmaApp.m_pUserFit[jj]->name));
        }
    }

    pDerivative->SetCurSel(0);
    pModel->SetCurSel(m_nModelSelected);
    VariableToControl();
    return;
}

void CFitterUserDlg::OnCbnSelchangeModel()
{    
    VariableToControl();
}

void CFitterUserDlg::OnCbnSelchangeDerivative()
{
    CEdit *pEditDerivative = (CEdit*) GetDlgItem(IDE_USERMODEL_DERIVATIVE);
    CComboBox *pDerivative = (CComboBox*) GetDlgItem(IDC_USERMODEL_DERIVATIVE);

    if ((m_nModelSelected < 0) || (m_nModelSelected >= SigmaApp.m_iUserFitCount)) {
        m_nModelSelected = 0;
    }

    // Sauvegarder la dérivée avant de changer de rang
    if (pEditDerivative && pDerivative && (m_nDerivSelected >= 0) && (m_nDerivSelected < ML_FIT_MAXPAR)) {

        char_t szDerivative[ML_STRSIZE];
        memset(szDerivative, 0, ML_STRSIZE * sizeof(char_t));

        pEditDerivative->GetWindowText((LPTSTR) szDerivative, ML_STRSIZE - 1);
        int_t iLen = (int_t) _tcslen((const char_t*) szDerivative);

        SigmaApp.m_pLib->errcode = 0;
        char_t *pszDerivative = ml_parser_decode(szDerivative, _T("abcdefx"), &iLen, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            _tcscpy(SigmaApp.m_pUserFit[m_nModelSelected]->derivative[m_nDerivSelected], (const char_t*) szDerivative);

        }
        ml_parser_free(pszDerivative);
    }

    m_nDerivSelected = pDerivative->GetCurSel();
    if ((m_nDerivSelected < 0) || (m_nDerivSelected >= ML_FIT_MAXPAR)) {
        m_nDerivSelected = 0;
        pDerivative->SetCurSel(m_nDerivSelected);
        UpdateData(TRUE);
    }

    pEditDerivative->SetWindowText((LPCTSTR) (SigmaApp.m_pUserFit[m_nModelSelected]->derivative[m_nDerivSelected]));

}

void CFitterUserDlg::OnDeltaposUsermodelParamcount(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    UpdateData(TRUE);

    int_t iParcount = pNMUpDown->iPos;
    if (pNMUpDown->iDelta > 0) {
        iParcount += 1;
        if (iParcount > ML_FIT_MAXPAR) {
            iParcount = ML_FIT_MAXPAR;
            pNMUpDown->iPos = iParcount;
        }
    }
    else {
        iParcount -= 1;
        if (iParcount < 2) {
            iParcount = 2;
            pNMUpDown->iPos = iParcount;
        }
    }
    SetDlgItemInt(IDL_USERMODEL_PARAMCOUNT, iParcount, FALSE);

    m_edtParA.EnableWindow(TRUE);
    m_edtParB.EnableWindow(TRUE);
    m_edtParC.EnableWindow(TRUE);
    m_edtParD.EnableWindow(TRUE);
    m_edtParE.EnableWindow(TRUE);
    m_edtParF.EnableWindow(TRUE);

    if (iParcount == 2) {
        m_edtParC.EnableWindow(FALSE);
        m_edtParD.EnableWindow(FALSE);
        m_edtParE.EnableWindow(FALSE);
        m_edtParF.EnableWindow(FALSE);
    }
    if (iParcount == 3) {
        m_edtParD.EnableWindow(FALSE);
        m_edtParE.EnableWindow(FALSE);
        m_edtParF.EnableWindow(FALSE);
    }
    if (iParcount == 4) {
        m_edtParE.EnableWindow(FALSE);
        m_edtParF.EnableWindow(FALSE);
    }
    if (iParcount == 5) {
        m_edtParF.EnableWindow(FALSE);
    }

    CComboBox *pDerivative = (CComboBox*)GetDlgItem(IDC_USERMODEL_DERIVATIVE);
    if (pDerivative != NULL) {
        pDerivative->ResetContent();
        pDerivative->AddString(_T("a"));
        pDerivative->AddString(_T("b"));
        if (iParcount > 2) {
            pDerivative->AddString(_T("c"));
        }
        if (iParcount > 3) {
            pDerivative->AddString(_T("d"));
        }
        if (iParcount > 4) {
            pDerivative->AddString(_T("e"));
        }
        if (iParcount > 5) {
            pDerivative->AddString(_T("f"));
        }
        pDerivative->SetCurSel(0);
    }

    UpdateData(FALSE);

    *pResult = 0;
}
