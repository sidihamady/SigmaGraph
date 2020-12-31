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
#include "FitterModel.h"
#include "MainFrm.h"
#include "SigmaDoc.h"
#include "FitterUserDlg.h"


// CFitterModel dialog

IMPLEMENT_DYNAMIC(CFitterModel, CPropertyPage)

CFitterModel::CFitterModel()
    : CPropertyPage(CFitterModel::IDD)
    , m_nPoints(0)
    , m_bReport(FALSE)
{
    m_iPage = 0;

    m_bInit = FALSE;
    m_bFixedParA = FALSE;
    m_bFixedParB = FALSE;
    m_bFixedParC = FALSE;
    m_bFixedParD = FALSE;
    m_nDefaultIndex = 0;

    m_bPlotPeaks = FALSE;

    m_pDoc = NULL;
    m_pPlot = NULL;
    m_pFit = NULL;
    m_pColumnDataX = NULL;
    m_pColumnDataY = NULL;
    m_pColumnFitX = NULL;
    m_pColumnFitY = NULL;
    m_pColumnFitYp1 = NULL;
    m_pColumnFitYp2 = NULL;
    m_pColumnFitYp3 = NULL;
    m_pColumnFitYp4 = NULL;
    m_pColumnFitYp5 = NULL;

    m_arrColumnFitId[0] = 0L;
    m_arrColumnFitId[1] = 0L;
    m_arrColumnFitId[2] = 0L;
    m_arrColumnFitId[3] = 0L;
    m_arrColumnFitId[4] = 0L;

    m_pSelectedModel = NULL;

    m_pbIsModified = NULL;

    m_pWdat = NULL;

    m_bOK = FALSE;
}

CFitterModel::~CFitterModel()
{

}

void CFitterModel::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDE_FITTER_NPOINTS, m_nPoints);
    DDX_Control(pDX, IDL_FITTER_MODEL, m_lstModel);
    DDX_Control(pDX, IDF_FITTER_MODEL, m_bmpFunction);
    DDX_Control(pDX, IDE_CURVE, m_edtCurve);
    DDX_Control(pDX, IDE_FITTER_MINX, m_edtXmin);
    DDX_Control(pDX, IDE_FITTER_MAXX, m_edtXmax);
    DDX_Control(pDX, IDE_FITTER_ITER, m_edtIter);
    DDX_Control(pDX, IDE_FITTER_CHI2, m_edtChi2);
    DDX_Control(pDX, IDE_FITTER_PARA, m_edtParA);
    DDX_Control(pDX, IDE_FITTER_PARB, m_edtParB);
    DDX_Control(pDX, IDE_FITTER_PARC, m_edtParC);
    DDX_Control(pDX, IDE_FITTER_PARD, m_edtParD);
    DDX_Control(pDX, IDK_FITTER_FIXEDA, m_cbFixedParA);
    DDX_Control(pDX, IDK_FITTER_FIXEDB, m_cbFixedParB);
    DDX_Control(pDX, IDK_FITTER_FIXEDC, m_cbFixedParC);
    DDX_Control(pDX, IDK_FITTER_FIXEDD, m_cbFixedParD);
    DDX_Control(pDX, IDK_FITTER_SHOWPEAKS, m_cbPlotPeaks);
    DDX_Check(pDX, IDK_FITTER_FIXEDA, m_bFixedParA);
    DDX_Check(pDX, IDK_FITTER_FIXEDB, m_bFixedParB);
    DDX_Check(pDX, IDK_FITTER_FIXEDC, m_bFixedParC);
    DDX_Check(pDX, IDK_FITTER_FIXEDD, m_bFixedParD);
    DDX_Check(pDX, IDK_FITTER_SHOWPEAKS, m_bPlotPeaks);
    DDX_Check(pDX, IDK_FITTER_REPORT, m_bReport);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}

BEGIN_MESSAGE_MAP(CFitterModel, CPropertyPage)
    ON_BN_CLICKED(ID_FITTER_NEXTPAR, &CFitterModel::OnFitterNextPar)
    ON_COMMAND(ID_FITTER_FIT, &CFitterModel::OnFitterFit)
    ON_BN_CLICKED(ID_USERMODEL_EDIT, &CFitterModel::OnFitterUser)
    ON_BN_CLICKED(ID_FITTER_AUTOPAR, &CFitterModel::OnAutoPar)
    ON_NOTIFY(LVN_ITEMCHANGED, IDL_FITTER_MODEL, &CFitterModel::OnChangedFitterModel)
    ON_NOTIFY(LVN_COLUMNCLICK, IDL_FITTER_MODEL, &CFitterModel::OnColumnClick)
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CFitterModel::OnItemClick)
    ON_BN_CLICKED(ID_FITTER_RESET, &CFitterModel::OnFitterReset)
    ON_CBN_SELCHANGE(IDC_USERMODEL_LIST, &CFitterModel::OnCbnSelchangeUsermodelList)
    ON_NOTIFY(NM_CLICK, IDL_FITTER_MODEL, &CFitterModel::OnNMClickFitterModel)
    ON_BN_CLICKED(IDK_USERMODEL, &CFitterModel::OnBnClickedUsermodel)
    ON_COMMAND(ID_HELP_CONTENTS, &CFitterModel::OnHelpContents)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CFitterModel::UpdateModel(int_t nDefaultIndex)
{
    if ((nDefaultIndex <= 0) || (nDefaultIndex > ML_FIT_MAXMODEL)) {
        return;
    }

    int_t ii, jj = -1;
    for (ii = 0; ii < ML_FIT_MAXMODEL; ii++) {
        if (nDefaultIndex == ((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].index) {
            m_pSelectedModel = &(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii]);
            jj = ii;
            break;
        }
    }

    if (jj >= 0) {
        UINT uiBmp[ML_FIT_MAXMODEL] = {
            IDB_FITTER_MODEL01, IDB_FITTER_MODEL02, IDB_FITTER_MODEL03,
            IDB_FITTER_MODEL04, IDB_FITTER_MODEL05, IDB_FITTER_MODEL06,
            IDB_FITTER_MODEL07, IDB_FITTER_MODEL08, IDB_FITTER_MODEL09,
            IDB_FITTER_MODEL10, IDB_FITTER_MODEL11, IDB_FITTER_MODEL12,
            IDB_FITTER_MODEL13, IDB_FITTER_MODEL14, IDB_FITTER_MODEL15,
            IDB_FITTER_MODEL16, IDB_FITTER_MODEL17, IDB_FITTER_MODEL18,
            IDB_FITTER_MODEL19, IDB_FITTER_MODEL20, IDB_FITTER_MODEL21,
            IDB_FITTER_MODEL22, IDB_FITTER_MODEL23, IDB_FITTER_MODEL24
        };
        m_nDefaultIndex = nDefaultIndex;
        m_lstModel.SetItemState(jj, LVIS_SELECTED, LVIS_SELECTED);
        m_Bitmap.DeleteObject();
        if (m_Bitmap.LoadBitmap(uiBmp[m_nDefaultIndex - 1])) {
            m_bmpFunction.SetBitmap(m_Bitmap);
        }
    }
}

// CFitterModel message handlers
BOOL CFitterModel::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // Data Initialization
    m_edtXmin.SetLimitText(32);
    m_edtXmax.SetLimitText(32);
    m_edtParA.SetLimitText(32);
    m_edtParB.SetLimitText(32);
    m_edtParC.SetLimitText(32);
    m_edtParD.SetLimitText(32);

    m_iPage = 0;

    m_pSelectedModel = NULL;

    CRect recTT;
    m_lstModel.GetClientRect(recTT);
    recTT.DeflateRect(1,1);

    m_lstModel.SetExtendedStyle((LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP) & ~LVS_EX_HEADERDRAGDROP);

    int_t nWidth = recTT.Width(), nW0, nW1;
    nW0 = (3 * nWidth) / 5;
    nW1 = (2 * nWidth) / 5;

    SIZE sizeText;
    CDC * pDC = GetDC();
    if (pDC) {
        if (::GetTextExtentPoint32(pDC->GetSafeHdc(), _T("Exponential   "), 14, &sizeText)) {
            nW0 = ML_MAX(nW0, sizeText.cx);
            nW1 = ML_MAX(nW1, sizeText.cx / 2);
        }
        ReleaseDC(pDC);
        pDC = NULL;
    }

    m_lstModel.InsertColumn(0, _T("Model"), LVCFMT_LEFT, nW0);
    m_lstModel.InsertColumn(1, _T("Parameters"), LVCFMT_LEFT, nW1);

    int_t ii, jj = -1;
    for (ii = 0; ii < ML_FIT_MAXMODEL; ii++) {
        m_Data[ii].pszName = (LPTSTR)(LPCTSTR)(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].name);
        m_Data[ii].pszFunction = (LPTSTR)(LPCTSTR)(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].function);
        m_Data[ii].pszPar = (LPTSTR)(LPCTSTR)(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].par);

        m_lstModel.InsertItem(ii, (LPCTSTR)(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].name));
        m_lstModel.SetItemText(ii, 1, (LPCTSTR)(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].par));
        m_lstModel.SetItemData(ii, (LPARAM)&m_Data[ii]);

        if (m_nDefaultIndex > 0) {
            if (m_nDefaultIndex == ((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].index) {
                m_pSelectedModel = &(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii]);
                jj = ii;
            }
        }
        else {
            if (m_pFit->index == ((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].index) {
                m_pSelectedModel = &(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii]);
                m_nDefaultIndex = ((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].index;
                jj = ii;
            }
        }
    }

    if (jj >= 0) {
        UINT uiBmp[ML_FIT_MAXMODEL] = {
            IDB_FITTER_MODEL01, IDB_FITTER_MODEL02, IDB_FITTER_MODEL03,
            IDB_FITTER_MODEL04, IDB_FITTER_MODEL05, IDB_FITTER_MODEL06,
            IDB_FITTER_MODEL07, IDB_FITTER_MODEL08, IDB_FITTER_MODEL09,
            IDB_FITTER_MODEL10, IDB_FITTER_MODEL11, IDB_FITTER_MODEL12,
            IDB_FITTER_MODEL13, IDB_FITTER_MODEL14, IDB_FITTER_MODEL15,
            IDB_FITTER_MODEL16, IDB_FITTER_MODEL17, IDB_FITTER_MODEL18,
            IDB_FITTER_MODEL19, IDB_FITTER_MODEL20, IDB_FITTER_MODEL21,
            IDB_FITTER_MODEL22, IDB_FITTER_MODEL23, IDB_FITTER_MODEL24
        };
        m_lstModel.SetItemState(jj, LVIS_SELECTED, LVIS_SELECTED);
        m_lstModel.EnsureVisible(jj, TRUE);
        m_Bitmap.DeleteObject();
        if (m_Bitmap.LoadBitmap(uiBmp[m_nDefaultIndex - 1])) {
            m_bmpFunction.SetBitmap(m_Bitmap);
        }
        m_bInit = TRUE;
    }
    else {
        if (m_pFit->index == ML_FIT_USERINDEX) {
            m_Bitmap.DeleteObject();
            if (m_Bitmap.LoadBitmap(IDB_FITTER_MODEL255)) {
                m_bmpFunction.SetBitmap(m_Bitmap);
            }
            m_bInit = TRUE;
            m_pSelectedModel = &(((CSigmaDoc*)m_pDoc)->m_FittingUserModel);
            SigmaApp.m_pLib->errcode = 0;
            ml_fit_dup(m_pSelectedModel, (const fit_t*)(m_pFit));
        }
    }

    m_edtCurve.SetWindowText((LPCTSTR)(m_pPlot->curve[m_pPlot->curveactive].legend.text));

    m_nPoints = m_pColumnFitX->dim;

    axis_t *paxisX = NULL;
    if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0xF0) == 0x10) {
        paxisX = &(m_pPlot->axistop);
    }
    else {
        paxisX = &(m_pPlot->axisbottom);
    }

    char_t szBuffer[ML_STRSIZES];
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    SigmaApp.Reformat(paxisX->min, szBuffer, ML_STRSIZES - 1);
    m_edtXmin.SetWindowText((LPCTSTR)szBuffer);
    SigmaApp.Reformat(paxisX->max, szBuffer, ML_STRSIZES - 1);
    m_edtXmax.SetWindowText((LPCTSTR)szBuffer);

    m_SortInfo.nAscendingSortOrder = TRUE;

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    if (m_pSelectedModel != NULL) {
        for (ii = 0; ii < m_pFit->parcount; ii++) {
            m_pSelectedModel->fpar[ii] = m_pFit->fpar[ii];
            m_pSelectedModel->mpar[ii] = m_pFit->mpar[ii];
            m_pSelectedModel->resd[ii] = m_pFit->resd[ii];
            m_pSelectedModel->fopt[ii] = m_pFit->fopt[ii];
        }
    }

    // Le texte existe déjà ?
    if ((m_bReport == FALSE) && (m_pPlot->textcount > 0)) {
        for (ii = 0; ii < m_pPlot->textcount; ii++) {
            if (m_pPlot->text[ii].id == m_pFit->id) {
                m_bReport = TRUE;
                break;
            }
        }
    }
    //

    ParamsToControls();

    CComboBox *pUserModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
    if (pUserModel == NULL) {
        return FALSE;
    }

    ii = 0;
    if (SigmaApp.m_iUserFitCount > 0) {
        int_t jj;
        char_t szUM[ML_STRSIZEN + ML_STRSIZES + 12];
        szUM[0] = szUM[(ML_STRSIZEN + ML_STRSIZES + 12) - 1] = _T('\0');
        for (jj = 0; jj < SigmaApp.m_iUserFitCount; jj++) {
            _tcscpy(szUM, (LPCTSTR) (SigmaApp.m_pUserFit[jj]->name));
            _tcscat(szUM, _T(" { "));
            _tcsncat(szUM, (LPCTSTR) (SigmaApp.m_pUserFit[jj]->function), ML_STRSIZEN);
            if (_tcslen((LPCTSTR) (SigmaApp.m_pUserFit[jj]->function)) > ML_STRSIZEN) {
                _tcscat(szUM, _T("..."));
            }
            _tcscat(szUM, _T(" }"));
            pUserModel->AddString((LPCTSTR) (szUM));
            if ((m_pSelectedModel != NULL) && (ii == 0)) {
                if (_tcsicmp((const char_t*)(m_pSelectedModel->name), (const char_t*)(SigmaApp.m_pUserFit[jj]->name)) == 0) {
                    ii = jj;
                    CheckDlgButton(IDK_USERMODEL, BST_CHECKED);
                }
            }
        }
    }
    pUserModel->SetCurSel(ii);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

int_t CALLBACK CFitterModel::SortList(LPARAM lpOne, LPARAM lpTwo, LPARAM lpArg)
{
    int_t nResult = 0;
    ListSortInfoM* pSortInfo = (ListSortInfoM*)lpArg;

    int_t nColumnNo = pSortInfo->nColumnNo;
    CListCtrl* pListControl = pSortInfo->pListControl;
    bool nAscendingSortOrder = pSortInfo->nAscendingSortOrder;

    int_t lFirstData = -1, lSecondData = -1;

    LV_FINDINFO FindInfo;
    // use LVFI_WRAP for cases where lpTwo represents a row before lpOne
    FindInfo.flags = LVFI_PARAM | LVFI_WRAP;
    FindInfo.lParam = lpOne;
    lFirstData = pListControl->FindItem(&FindInfo);

    FindInfo.lParam = lpTwo;
    // reduce searching time by setting the start row as lFirstData
    lSecondData = pListControl->FindItem(&FindInfo,lFirstData);
    // because we are searching for LPARAM sent to us, FindItem() on
    // these values should always be successful
    ASSERT(lFirstData != -1); ASSERT(lSecondData != -1);

    CString FirstText = pListControl->GetItemText(lFirstData, nColumnNo);
    CString SecondText = pListControl->GetItemText(lSecondData, nColumnNo);

    int_t nCompareValue;

    nCompareValue = FirstText.Compare(SecondText);

    nResult = nCompareValue * ((nAscendingSortOrder)? 1 : -1);

    return nResult;
}

void CFitterModel::OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    *pResult = 0;
}

void CFitterModel::OnItemClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

    HD_NOTIFY *phdn = (HD_NOTIFY*) pNMHDR;
    if(phdn->iButton == 0) {
        if (phdn->iItem == m_SortInfo.nColumnNo) {
            m_SortInfo.nAscendingSortOrder = !(m_SortInfo.nAscendingSortOrder);
        }
        else {
            m_SortInfo.nAscendingSortOrder = TRUE;
        }

        m_SortInfo.nColumnNo = phdn->iItem;
        m_SortInfo.pListControl = &m_lstModel;

        m_lstModel.SortItems(SortList, (DWORD)&m_SortInfo);
    }

    *pResult = 0;
}

void CFitterModel::OnFitterUser()
{
    CFitterUserDlg dlgFitterUser;

    dlgFitterUser.m_nModelSelected = 0;
    CComboBox *pUserModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
    if (pUserModel == NULL) {
        return;
    }
    if (SigmaApp.m_iUserFitCount > 0) {
        dlgFitterUser.m_nModelSelected = pUserModel->GetCurSel();
    }

    char_t szBuffer[ML_STRSIZES];
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    axis_t *paxisX = NULL;
    if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0xF0) == 0x10) {
        paxisX = &(m_pPlot->axistop);
    }
    else {
        paxisX = &(m_pPlot->axisbottom);
    }
    dlgFitterUser.m_fVal = paxisX->min;

    dlgFitterUser.DoModal();

    pUserModel->ResetContent();
    int_t jj;
    if (SigmaApp.m_iUserFitCount > 0) {
        char_t szUM[ML_STRSIZEN + ML_STRSIZES + 12];
        szUM[0] = szUM[(ML_STRSIZEN + ML_STRSIZES + 12) - 1] = _T('\0');
        for (jj = 0; jj < SigmaApp.m_iUserFitCount; jj++) {
            _tcscpy(szUM, (LPCTSTR) (SigmaApp.m_pUserFit[jj]->name));
            _tcscat(szUM, _T(" { "));
            _tcsncat(szUM, (LPCTSTR) (SigmaApp.m_pUserFit[jj]->function), ML_STRSIZEN);
            if (_tcslen((LPCTSTR) (SigmaApp.m_pUserFit[jj]->function)) > ML_STRSIZEN) {
                _tcscat(szUM, _T("..."));
            }
            _tcscat(szUM, _T(" }"));
            pUserModel->AddString((LPCTSTR) (szUM));
        }
    }
    pUserModel->SetCurSel(dlgFitterUser.m_nModelSelected);

    if ((dlgFitterUser.m_bOK == TRUE) && (dlgFitterUser.m_nModelSelected >= 0)
        && (dlgFitterUser.m_nModelSelected < SigmaApp.m_iUserFitCount)
        && (SigmaApp.m_iUserFitCount > 0)) {
        CheckDlgButton(IDK_USERMODEL, BST_CHECKED);
        ActivateFitterUserModel();
    }
}

void CFitterModel::OnFitterFit()
{
    if (*m_pbRunning == TRUE) {
        return;
    }

    BOOL bUserModel = (IsDlgButtonChecked(IDK_USERMODEL) == BST_CHECKED);

    if (m_pSelectedModel == NULL) {
        if (bUserModel == FALSE) {
            this->MessageBox(_T("Cannot perform operation: no fitting model selected."), _T("SigmaGraph"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
    }
    if (bUserModel == TRUE) {
        m_pSelectedModel = &(((CSigmaDoc*) m_pDoc)->m_FittingUserModel);
    }

    *m_pbRunning = TRUE;

    CWaitCursor wcT;

    UpdateData(TRUE);

    int_t ii, jj;

    // Le texte existe déjà ? l'effacer.
    if ((m_bReport == FALSE) && (m_pPlot->textcount > 0)) {
        for (ii = 0; ii < m_pPlot->textcount; ii++) {
            if (m_pPlot->text[ii].id == m_pFit->id) {
                //m_bReport = TRUE;
                m_pPlot->text[ii].len = 0;
                memset(m_pPlot->text[ii].text, 0, ML_STRSIZE);
                memset(&(m_pPlot->text[ii].rect), 0, sizeof(rect_t));
                m_pPlot->textcount -= 1;
                break;
            }
        }
    }

    // >> UserFit
    if (bUserModel == TRUE) {
        CComboBox *pUserModel = (CComboBox*) GetDlgItem(IDC_USERMODEL_LIST);
        if (pUserModel == NULL) {
            SigmaApp.Output(_T("User-defined fitting model not valid.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            *m_pbRunning = FALSE;
            return;
        }
        int_t iSelectedModel = pUserModel->GetCurSel();
        if ((iSelectedModel < 0) || (iSelectedModel >= SigmaApp.m_iUserFitCount)) {
            SigmaApp.Output(_T("User-defined fitting model not valid.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            *m_pbRunning = FALSE;
            return;
        }
        SigmaApp.m_pLib->errcode = 0;
        if (ml_fit_userinit((const userfit_t*) (SigmaApp.m_pUserFit[iSelectedModel]), SigmaApp.m_pLib) != 0) {
            SigmaApp.Output(_T("User-defined fitting model not valid.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            *m_pbRunning = FALSE;
            return;
        }

        m_pSelectedModel->id = SigmaApp.m_pUserFit[iSelectedModel]->id;
        m_pSelectedModel->index = SigmaApp.m_pUserFit[iSelectedModel]->index;
        m_pSelectedModel->parcount = SigmaApp.m_pUserFit[iSelectedModel]->parcount;
        _tcsncpy(m_pSelectedModel->name, (const char_t*) (SigmaApp.m_pUserFit[iSelectedModel]->name), ML_STRSIZES - 1);
        _tcsncpy(m_pSelectedModel->function, (const char_t*) (SigmaApp.m_pUserFit[iSelectedModel]->function), ML_STRSIZE - 1);
    }
    // <<

    CWnd *pWnd = GetDlgItem(ID_FITTER_NEXTPAR);
    if (m_pSelectedModel->parcount <= 4) {
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Page 1/1"));
            pWnd->EnableWindow(FALSE);
        }
        m_iPage = 0;
    }
    else {
        if (pWnd != NULL) {
            pWnd->EnableWindow(TRUE);
        }
    }

    ControlsToParams();

    int_t iFixed = 0;
    for (ii = 0; ii < m_pSelectedModel->parcount; ii++) {
        iFixed += m_pSelectedModel->mpar[ii];
    }

    // Redimensionner les colonnes selon la nouvelle valeur du nombre de points
    int_t nColumnCount = 0;
    vector_t *pColumn = ((CSigmaDoc*) m_pDoc)->GetColumns(1, &nColumnCount);
    if ((pColumn == NULL) || (nColumnCount < 1) || (nColumnCount > SIGMA_MAXCOLUMNS)) {
        // >> UserFit
        if (bUserModel == TRUE) { ml_fit_userend(); }
        // <<
        SigmaApp.Output(_T("Fitting error: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        *m_pbRunning = FALSE;
        return;
    }
    int_t nPrevDim = m_pColumnFitX->dim;
    if (m_nPoints < nPrevDim) {
        if (nColumnCount == 2) {    // Dans le cas d'un fit unique, supprimer les points excédentaires
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_rem(m_pColumnFitX, m_nPoints, nPrevDim - 1, SigmaApp.m_pLib);
            ml_vector_rem(m_pColumnFitY, m_nPoints, nPrevDim - 1, SigmaApp.m_pLib);
        }
        else {
            for (ii = m_nPoints; ii < nPrevDim; ii++) {
                m_pColumnFitX->dat[ii] = 0.0;
                m_pColumnFitX->mask[ii] = 0xFF;    // Item invalide
                m_pColumnFitY->dat[ii] = 0.0;
                m_pColumnFitY->mask[ii] = 0xFF;    // Item invalide
            }
        }
    }
    else if (m_nPoints > nPrevDim) {
        for (jj = 0; jj < nColumnCount; jj++) {
            for (ii = nPrevDim; ii < m_nPoints; ii++) {
                SigmaApp.m_pLib->errcode = 0;
                ml_vector_add(&(pColumn[jj]), 0.0, SigmaApp.m_pLib);
                if (SigmaApp.m_pLib->errcode != 0) {
                    // >> UserFit
                    if (bUserModel == TRUE) { ml_fit_userend(); }
                    // <<
                    CString strT = _T("Fitting error: \r\n");
                    strT.Append((LPCTSTR) (SigmaApp.m_pLib->message));
                    SigmaApp.Output((LPCTSTR) strT, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                    SigmaApp.Output(_T("\r\n"));
                    *m_pbRunning = FALSE;
                    return;
                }
                pColumn[jj].mask[ii] = 0xFF;    // Item invalide
            }
        }
    }

    m_bOK = FALSE;

    real_t fmin, fmax, ft, ftm;
    real_t fStep;

    axis_t *paxisX = NULL;
    if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0xF0) == 0x10) {
        paxisX = &(m_pPlot->axistop);
    }
    else {
        paxisX = &(m_pPlot->axisbottom);
    }

    // Tous les paramètres fixés
    if (iFixed == 0) {
        // Redéfinir le pas, le cas échéant
        fmin = m_edtXmin.GetValue();
        fmax = m_edtXmax.GetValue();
        ft = fmax - fmin;
        ftm = paxisX->max - paxisX->min;
        if ((ft <= 0) || (ft > ftm) || (ft < (ftm * 0.1))) {
            fmin = paxisX->min;
            fmax = paxisX->max;
            ft = fmax - fmin;
            m_edtXmin.SetValue(paxisX->min);
            m_edtXmax.SetValue(paxisX->max);
            UpdateData(FALSE);
        }

        fStep = ft / (real_t) (m_nPoints - 1);
        m_pColumnFitX->dat[0] = fmin;
        m_pColumnFitY->dat[0] = ml_fit_fun(m_pSelectedModel->index - 1, m_pColumnFitX->dat[0],
            m_pSelectedModel->fpar, m_pSelectedModel->parcount, NULL);
        m_pColumnFitX->mask[0] = 0x00;
        m_pColumnFitY->mask[0] = 0x00;
        for (ii = 1; ii < m_nPoints - 1; ii++) {
            m_pColumnFitX->dat[ii] = m_pColumnFitX->dat[ii - 1] + fStep;
            m_pColumnFitY->dat[ii] = ml_fit_fun(m_pSelectedModel->index - 1, m_pColumnFitX->dat[ii],
                m_pSelectedModel->fpar, m_pSelectedModel->parcount, NULL);
            m_pColumnFitX->mask[ii] = 0x00;
            m_pColumnFitY->mask[ii] = 0x00;
        }
        m_pColumnFitX->dat[m_nPoints - 1] = fmax;
        m_pColumnFitY->dat[m_nPoints - 1] = ml_fit_fun(m_pSelectedModel->index - 1, m_pColumnFitX->dat[m_nPoints - 1],
            m_pSelectedModel->fpar, m_pSelectedModel->parcount, NULL);
        m_pColumnFitX->mask[m_nPoints - 1] = 0x00;
        m_pColumnFitY->mask[m_nPoints - 1] = 0x00;

        m_pColumnFitX->status = 0x00;
        m_pColumnFitY->status = 0x00;

        m_pPlot->errcode = 0;
        int iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitY, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
        if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
            m_pPlot->curve[iCurve].dotstyle = 0x00;
            if (m_pPlot->curve[iCurve].linesize > 2) {
                m_pPlot->curve[iCurve].linesize = 2;
            }
        }

        m_bOK = TRUE;

        m_edtChi2.SetWindowText(_T(""));
        m_edtIter.SetWindowText(_T(""));

        ParamsToControls();

        if (m_bReport == TRUE) {
            PrintReport(iCurve);
        }

        m_pDoc->SetModifiedFlag(TRUE);
        m_pDoc->UpdateAllViews(NULL);

        // >> UserFit
        if (bUserModel == TRUE) { ml_fit_userend(); }
        // <<

        *m_pbRunning = FALSE;

        return;
    }

    // Allouer de la mémoire pour les données à fitter
    real_t *px = NULL, *py = NULL;
    int_t nn = m_pColumnDataX->dim, idat = 0;
    px = (real_t*) malloc(nn * sizeof(real_t));
    if (px == NULL) {
        // >> UserFit
        if (bUserModel == TRUE) { ml_fit_userend(); }
        // <<
        SigmaApp.Output(_T("Fitting error: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        *m_pbRunning = FALSE;
        return;
    }
    py = (real_t*) malloc(nn * sizeof(real_t));
    if (py == NULL) {
        free(px); px = NULL;
        // >> UserFit
        if (bUserModel == TRUE) { ml_fit_userend(); }
        // <<
        SigmaApp.Output(_T("Fitting error: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        *m_pbRunning = FALSE;
        return;
    }
    idat = 0;
    for (ii = 0; ii < nn; ii++) {
        // Ne pas tenir compte des éléments masqués...
        if (((m_pColumnDataX->mask[ii] & 0x0F) == 0x01) || ((m_pColumnDataY->mask[ii] & 0x0F) == 0x01)) {
            continue;
        }
        // ...et des éléments non valides.
        if (((m_pColumnDataX->mask[ii] & 0x0F) == 0x0F) || ((m_pColumnDataY->mask[ii] & 0x0F) == 0x0F)) {
            continue;
        }

        px[idat] = m_pColumnDataX->dat[ii];
        py[idat] = m_pColumnDataY->dat[ii];
        idat += 1;
    }
    if (idat < 3) {
        free(px); px = NULL;
        free(py); py = NULL;
        // >> UserFit
        if (bUserModel == TRUE) { ml_fit_userend(); }
        // <<
        SigmaApp.Output(_T("Fitting error: too many masked points.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        *m_pbRunning = FALSE;
        return;
    }

    SigmaApp.m_pLib->errcode = 0;
    int_t nIter = m_pSelectedModel->iter;
    real_t fLambda = m_pSelectedModel->lambda;
    m_pSelectedModel->chi = ml_fit_levmarq(m_pSelectedModel->index - 1, px, py, m_pWdat,
        idat, m_pSelectedModel->fpar, m_pSelectedModel->resd, m_pSelectedModel->mpar,
        m_pSelectedModel->parcount, NULL, &nIter, &fLambda, SigmaApp.m_pLib);

    if (SigmaApp.m_pLib->errcode == 0) {
        if (m_pSelectedModel->chi >= 1.0) {
            real_t fparBak[ML_FIT_MAXPAR];
            if (iFixed < m_pSelectedModel->parcount) {
                for (int ii = 0; ii < m_pSelectedModel->parcount; ii++) {
                    fparBak[ii] = m_pSelectedModel->fpar[ii];
                }
            }
            ml_fit_autopar(m_pColumnDataX, m_pColumnDataY, m_pSelectedModel->index - 1, m_pSelectedModel->fpar, SigmaApp.m_pLib);
            if (iFixed < m_pSelectedModel->parcount) {
                for (int ii = 0; ii < m_pSelectedModel->parcount; ii++) {
                    if (m_pSelectedModel->mpar[ii] == 0) {
                        m_pSelectedModel->fpar[ii] = fparBak[ii];
                    }
                }
            }

            ParamsToControls();

            nIter = m_pSelectedModel->iter;
            fLambda = m_pSelectedModel->lambda;
            m_pSelectedModel->chi = ml_fit_levmarq(m_pSelectedModel->index - 1, px, py, m_pWdat,
                idat, m_pSelectedModel->fpar, m_pSelectedModel->resd, m_pSelectedModel->mpar,
                m_pSelectedModel->parcount, NULL, &nIter, &fLambda, SigmaApp.m_pLib);
        }
    }

    if (SigmaApp.m_pLib->errcode != 0) {
        *m_pbRunning = FALSE;
        SigmaApp.m_pLib->errcode = 0;
        doAutoPar(false);
        *m_pbRunning = TRUE;
        nIter = m_pSelectedModel->iter;
        fLambda = m_pSelectedModel->lambda;
        m_pSelectedModel->chi = ml_fit_levmarq(m_pSelectedModel->index - 1, px, py, m_pWdat,
            idat, m_pSelectedModel->fpar, m_pSelectedModel->resd, m_pSelectedModel->mpar,
            m_pSelectedModel->parcount, NULL, &nIter, &fLambda, SigmaApp.m_pLib);

        if (SigmaApp.m_pLib->errcode != 0) {
            free(px); px = NULL;
            free(py); py = NULL;
            // >> UserFit
            if (bUserModel == TRUE) { ml_fit_userend(); }
            // <<
            CString strT = _T("Fitting calculations do not converge: \r\n");
            strT.Append((LPCTSTR) (SigmaApp.m_pLib->message));
            SigmaApp.Output((LPCTSTR) strT, TRUE, TRUE, SIGMA_OUTPUT_WARNING);
            SigmaApp.Output(_T("\r\n"));
            *m_pbRunning = FALSE;
            return;
        }
    }

    free(px); px = NULL;
    free(py); py = NULL;

    // Redéfinir le pas, le cas échéant
    fmin = m_edtXmin.GetValue();
    fmax = m_edtXmax.GetValue();
    ft = fmax - fmin;
    ftm = paxisX->max - paxisX->min;
    if ((ft <= 0) || (ft > ftm) || (ft < (ftm * 0.1))) {
        fmin = paxisX->min;
        fmax = paxisX->max;
        ft = fmax - fmin;
        m_edtXmin.SetValue(paxisX->min);
        m_edtXmax.SetValue(paxisX->max);
        UpdateData(FALSE);
    }

    bool bPlotPeaks = false;
    int ifun = m_pSelectedModel->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    real_t fparP1[ML_FIT_MAXPAR], fparP2[ML_FIT_MAXPAR],
        fparP3[ML_FIT_MAXPAR], fparP4[ML_FIT_MAXPAR],
        fparP5[ML_FIT_MAXPAR];
    int_t parcountP = 4;
    int_t modelBase = 0;

    CSigmaDoc *pDoc = (CSigmaDoc*) (m_pDoc);

    for (jj = 0; jj < 5; jj++) {
        pl_curve_removei(m_pPlot, m_arrColumnFitId[jj]);
        pDoc->DeleteColumn(1, -1, m_arrColumnFitId[jj]);
    }

    int_t nlen = _tcslen((const char_t*) (m_pColumnDataY->label));

    int_t iFitColumnCount = 0;
    if (bPeakM) {
        modelBase = ((ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
            || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)) ? (FIT_MODEL_GAUSS1 + 1) : (FIT_MODEL_LORENTZ1 + 1);

        if ((ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_LORENTZ2)) {
            iFitColumnCount = 2;
            m_pColumnFitYp1 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 0);
            m_pColumnFitYp2 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 1);

            bPlotPeaks = ((m_pColumnFitYp1 != NULL) && (m_pColumnFitYp2 != NULL));
            if (bPlotPeaks) {
                m_arrColumnFitId[0] = m_pColumnFitYp1->id;
                m_arrColumnFitId[1] = m_pColumnFitYp2->id;
            }
            fparP1[0] = 0.0;
            fparP1[1] = m_pSelectedModel->fpar[1];
            fparP1[2] = m_pSelectedModel->fpar[2];
            fparP1[3] = m_pSelectedModel->fpar[3];
            fparP2[0] = 0.0;
            fparP2[1] = m_pSelectedModel->fpar[4];
            fparP2[2] = m_pSelectedModel->fpar[5];
            fparP2[3] = m_pSelectedModel->fpar[6];
        }
        else if ((ifun == FIT_MODEL_GAUSS3) || (ifun == FIT_MODEL_LORENTZ3)) {
            iFitColumnCount = 3;
            m_pColumnFitYp1 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 0);
            m_pColumnFitYp2 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 1);
            m_pColumnFitYp3 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 2);
            bPlotPeaks = ((m_pColumnFitYp1 != NULL) && (m_pColumnFitYp2 != NULL) && (m_pColumnFitYp3 != NULL));
            if (bPlotPeaks) {
                m_arrColumnFitId[0] = m_pColumnFitYp1->id;
                m_arrColumnFitId[1] = m_pColumnFitYp2->id;
                m_arrColumnFitId[2] = m_pColumnFitYp3->id;
            }
            fparP1[0] = 0.0;
            fparP1[1] = m_pSelectedModel->fpar[1];
            fparP1[2] = m_pSelectedModel->fpar[2];
            fparP1[3] = m_pSelectedModel->fpar[3];
            fparP2[0] = 0.0;
            fparP2[1] = m_pSelectedModel->fpar[4];
            fparP2[2] = m_pSelectedModel->fpar[5];
            fparP2[3] = m_pSelectedModel->fpar[6];
            fparP3[0] = 0.0;
            fparP3[1] = m_pSelectedModel->fpar[7];
            fparP3[2] = m_pSelectedModel->fpar[8];
            fparP3[3] = m_pSelectedModel->fpar[9];
        }
        else if ((ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_LORENTZ4)) {
            iFitColumnCount = 4;
            m_pColumnFitYp1 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 0);
            m_pColumnFitYp2 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 1);
            m_pColumnFitYp3 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 2);
            m_pColumnFitYp4 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 3);
            bPlotPeaks = ((m_pColumnFitYp1 != NULL) && (m_pColumnFitYp2 != NULL) && (m_pColumnFitYp3 != NULL) && (m_pColumnFitYp4 != NULL));
            if (bPlotPeaks) {
                m_arrColumnFitId[0] = m_pColumnFitYp1->id;
                m_arrColumnFitId[1] = m_pColumnFitYp2->id;
                m_arrColumnFitId[2] = m_pColumnFitYp3->id;
                m_arrColumnFitId[3] = m_pColumnFitYp4->id;
            }
            fparP1[0] = 0.0;
            fparP1[1] = m_pSelectedModel->fpar[1];
            fparP1[2] = m_pSelectedModel->fpar[2];
            fparP1[3] = m_pSelectedModel->fpar[3];
            fparP2[0] = 0.0;
            fparP2[1] = m_pSelectedModel->fpar[4];
            fparP2[2] = m_pSelectedModel->fpar[5];
            fparP2[3] = m_pSelectedModel->fpar[6];
            fparP3[0] = 0.0;
            fparP3[1] = m_pSelectedModel->fpar[7];
            fparP3[2] = m_pSelectedModel->fpar[8];
            fparP3[3] = m_pSelectedModel->fpar[9];
            fparP4[0] = 0.0;
            fparP4[1] = m_pSelectedModel->fpar[10];
            fparP4[2] = m_pSelectedModel->fpar[11];
            fparP4[3] = m_pSelectedModel->fpar[12];
        }
        else if ((ifun == FIT_MODEL_GAUSS5) || (ifun == FIT_MODEL_LORENTZ5)) {
            iFitColumnCount = 5;
            m_pColumnFitYp1 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 0);
            m_pColumnFitYp2 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 1);
            m_pColumnFitYp3 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 2);
            m_pColumnFitYp4 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 3);
            m_pColumnFitYp5 = pDoc->AppendFitColumn(m_pColumnFitY, m_pSelectedModel, 4);
            bPlotPeaks = ((m_pColumnFitYp1 != NULL) && (m_pColumnFitYp2 != NULL) && (m_pColumnFitYp3 != NULL) && (m_pColumnFitYp4 != NULL) && (m_pColumnFitYp5 != NULL));
            if (bPlotPeaks) {
                m_arrColumnFitId[0] = m_pColumnFitYp1->id;
                m_arrColumnFitId[1] = m_pColumnFitYp2->id;
                m_arrColumnFitId[2] = m_pColumnFitYp3->id;
                m_arrColumnFitId[3] = m_pColumnFitYp4->id;
                m_arrColumnFitId[4] = m_pColumnFitYp5->id;
            }
            fparP1[0] = 0.0;
            fparP1[1] = m_pSelectedModel->fpar[1];
            fparP1[2] = m_pSelectedModel->fpar[2];
            fparP1[3] = m_pSelectedModel->fpar[3];
            fparP2[0] = 0.0;
            fparP2[1] = m_pSelectedModel->fpar[4];
            fparP2[2] = m_pSelectedModel->fpar[5];
            fparP2[3] = m_pSelectedModel->fpar[6];
            fparP3[0] = 0.0;
            fparP3[1] = m_pSelectedModel->fpar[7];
            fparP3[2] = m_pSelectedModel->fpar[8];
            fparP3[3] = m_pSelectedModel->fpar[9];
            fparP4[0] = 0.0;
            fparP4[1] = m_pSelectedModel->fpar[10];
            fparP4[2] = m_pSelectedModel->fpar[11];
            fparP4[3] = m_pSelectedModel->fpar[12];
            fparP5[0] = 0.0;
            fparP5[1] = m_pSelectedModel->fpar[13];
            fparP5[2] = m_pSelectedModel->fpar[14];
            fparP5[3] = m_pSelectedModel->fpar[15];
        }
    }

    if (bPlotPeaks) {

        m_pColumnFitYp1->opt = 0x00;
        m_pColumnFitYp2->opt = 0x00;
        if (nlen < (ML_STRSIZES - 10)) {
            _tcscpy(m_pColumnFitYp1->label, (const char_t*) (m_pColumnDataY->label));
            _tcscat(m_pColumnFitYp1->label, _T(" - Peak1"));
            _tcscpy(m_pColumnFitYp2->label, (const char_t*) (m_pColumnDataY->label));
            _tcscat(m_pColumnFitYp2->label, _T(" - Peak2"));
        }
        else {
            _tcscpy(m_pColumnFitYp1->label, (const char_t*) (m_pColumnFitYp1->name));
            _tcscpy(m_pColumnFitYp2->label, (const char_t*) (m_pColumnFitYp2->name));
        }
        m_pColumnFitYp1->dat[0] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[0], fparP1, parcountP, NULL);
        m_pColumnFitYp2->dat[0] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[0], fparP2, parcountP, NULL);
        m_pColumnFitYp1->mask[0] = 0x00;
        m_pColumnFitYp2->mask[0] = 0x00;

        if (iFitColumnCount > 2) {
            m_pColumnFitYp3->opt = 0x00;
            if (nlen < (ML_STRSIZES - 10)) {
                _tcscpy(m_pColumnFitYp3->label, (const char_t*) (m_pColumnDataY->label));
                _tcscat(m_pColumnFitYp3->label, _T(" - Peak3"));
            }
            else {
                _tcscpy(m_pColumnFitYp3->label, (const char_t*) (m_pColumnFitYp3->name));
            }
            m_pColumnFitYp3->dat[0] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[0], fparP3, parcountP, NULL);
            m_pColumnFitYp3->mask[0] = 0x00;
        }

        if (iFitColumnCount > 3) {
            m_pColumnFitYp4->opt = 0x00;
            if (nlen < (ML_STRSIZES - 10)) {
                _tcscpy(m_pColumnFitYp4->label, (const char_t*) (m_pColumnDataY->label));
                _tcscat(m_pColumnFitYp4->label, _T(" - Peak4"));
            }
            else {
                _tcscpy(m_pColumnFitYp4->label, (const char_t*) (m_pColumnFitYp4->name));
            }
            m_pColumnFitYp4->dat[0] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[0], fparP4, parcountP, NULL);
            m_pColumnFitYp4->mask[0] = 0x00;
        }

        if (iFitColumnCount > 4) {
            m_pColumnFitYp5->opt = 0x00;
            if (nlen < (ML_STRSIZES - 10)) {
                _tcscpy(m_pColumnFitYp5->label, (const char_t*) (m_pColumnDataY->label));
                _tcscat(m_pColumnFitYp5->label, _T(" - Peak5"));
            }
            else {
                _tcscpy(m_pColumnFitYp5->label, (const char_t*) (m_pColumnFitYp5->name));
            }
            m_pColumnFitYp5->dat[0] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[0], fparP5, parcountP, NULL);
            m_pColumnFitYp5->mask[0] = 0x00;
        }
    }

    fStep = ft / (real_t)(m_nPoints - 1);
    m_pColumnFitX->dat[0] = fmin;
    m_pColumnFitY->dat[0] = ml_fit_fun(m_pSelectedModel->index - 1, m_pColumnFitX->dat[0],
        m_pSelectedModel->fpar, m_pSelectedModel->parcount, NULL);
    m_pColumnFitX->mask[0] = 0x00;
    m_pColumnFitY->mask[0] = 0x00;
    for (ii = 1; ii < m_nPoints - 1; ii++) {
        m_pColumnFitX->dat[ii] = m_pColumnFitX->dat[ii - 1] + fStep;
        m_pColumnFitY->dat[ii] = ml_fit_fun(m_pSelectedModel->index - 1, m_pColumnFitX->dat[ii],
            m_pSelectedModel->fpar, m_pSelectedModel->parcount, NULL);
        m_pColumnFitX->mask[ii] = 0x00;
        m_pColumnFitY->mask[ii] = 0x00;

        if (bPlotPeaks) {
            if (iFitColumnCount == 2) {
                m_pColumnFitYp1->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP1, parcountP, NULL);
                m_pColumnFitYp2->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP2, parcountP, NULL);
                m_pColumnFitYp1->mask[ii] = 0x00;
                m_pColumnFitYp2->mask[ii] = 0x00;
            }
            else if (iFitColumnCount == 3) {
                m_pColumnFitYp1->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP1, parcountP, NULL);
                m_pColumnFitYp2->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP2, parcountP, NULL);
                m_pColumnFitYp3->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP3, parcountP, NULL);
                m_pColumnFitYp1->mask[ii] = 0x00;
                m_pColumnFitYp2->mask[ii] = 0x00;
                m_pColumnFitYp3->mask[ii] = 0x00;
            }
            else if (iFitColumnCount == 4) {
                m_pColumnFitYp1->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP1, parcountP, NULL);
                m_pColumnFitYp2->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP2, parcountP, NULL);
                m_pColumnFitYp3->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP3, parcountP, NULL);
                m_pColumnFitYp4->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP4, parcountP, NULL);
                m_pColumnFitYp1->mask[ii] = 0x00;
                m_pColumnFitYp2->mask[ii] = 0x00;
                m_pColumnFitYp3->mask[ii] = 0x00;
                m_pColumnFitYp4->mask[ii] = 0x00;
            }
            else if (iFitColumnCount == 5) {
                m_pColumnFitYp1->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP1, parcountP, NULL);
                m_pColumnFitYp2->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP2, parcountP, NULL);
                m_pColumnFitYp3->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP3, parcountP, NULL);
                m_pColumnFitYp4->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP4, parcountP, NULL);
                m_pColumnFitYp5->dat[ii] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[ii], fparP5, parcountP, NULL);
                m_pColumnFitYp1->mask[ii] = 0x00;
                m_pColumnFitYp2->mask[ii] = 0x00;
                m_pColumnFitYp3->mask[ii] = 0x00;
                m_pColumnFitYp4->mask[ii] = 0x00;
                m_pColumnFitYp5->mask[ii] = 0x00;
            }
        }
    }
    m_pColumnFitX->dat[m_nPoints - 1] = fmax;
    m_pColumnFitY->dat[m_nPoints - 1] = ml_fit_fun(m_pSelectedModel->index - 1, m_pColumnFitX->dat[m_nPoints - 1],
        m_pSelectedModel->fpar, m_pSelectedModel->parcount, NULL);
    m_pColumnFitX->mask[m_nPoints - 1] = 0x00;
    m_pColumnFitY->mask[m_nPoints - 1] = 0x00;

    m_pColumnFitX->status = 0x00;
    m_pColumnFitY->status = 0x00;

    if (bPlotPeaks) {
        if (iFitColumnCount == 2) {
            m_pColumnFitYp1->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP1, parcountP, NULL);
            m_pColumnFitYp2->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP2, parcountP, NULL);
            m_pColumnFitYp1->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp2->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp1->status = 0x00;
            m_pColumnFitYp2->status = 0x00;
        }
        else if (iFitColumnCount == 3) {
            m_pColumnFitYp1->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP1, parcountP, NULL);
            m_pColumnFitYp2->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP2, parcountP, NULL);
            m_pColumnFitYp3->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP3, parcountP, NULL);
            m_pColumnFitYp1->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp2->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp3->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp1->status = 0x00;
            m_pColumnFitYp2->status = 0x00;
            m_pColumnFitYp3->status = 0x00;
        }
        else if (iFitColumnCount == 4) {
            m_pColumnFitYp1->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP1, parcountP, NULL);
            m_pColumnFitYp2->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP2, parcountP, NULL);
            m_pColumnFitYp3->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP3, parcountP, NULL);
            m_pColumnFitYp4->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP4, parcountP, NULL);
            m_pColumnFitYp1->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp2->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp3->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp4->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp1->status = 0x00;
            m_pColumnFitYp2->status = 0x00;
            m_pColumnFitYp3->status = 0x00;
            m_pColumnFitYp4->status = 0x00;
        }
        else if (iFitColumnCount == 5) {
            m_pColumnFitYp1->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP1, parcountP, NULL);
            m_pColumnFitYp2->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP2, parcountP, NULL);
            m_pColumnFitYp3->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP3, parcountP, NULL);
            m_pColumnFitYp4->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP4, parcountP, NULL);
            m_pColumnFitYp5->dat[m_nPoints - 1] = ml_fit_fun(modelBase - 1, m_pColumnFitX->dat[m_nPoints - 1], fparP5, parcountP, NULL);
            m_pColumnFitYp1->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp2->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp3->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp4->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp5->mask[m_nPoints - 1] = 0x00;
            m_pColumnFitYp1->status = 0x00;
            m_pColumnFitYp2->status = 0x00;
            m_pColumnFitYp3->status = 0x00;
            m_pColumnFitYp4->status = 0x00;
            m_pColumnFitYp5->status = 0x00;
        }
    }

    byte_t iLineSize = 2;
    int iCurveActive = m_pPlot->curveactive;
    if ((iCurveActive >= 0) && (iCurveActive < m_pPlot->curvecount)) {
        iLineSize = m_pPlot->curve[iCurveActive].linesize + 1;
    }
    if (iLineSize > 2) {
        iLineSize = 2;
    }

    m_pPlot->errcode = 0;

    for (jj = 0; jj < 5; jj++) {
        pl_curve_removei(m_pPlot, m_arrColumnFitId[jj]);
    }

    int iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitY, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
    if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
        m_pPlot->curve[iCurve].dotstyle = 0x00;
        m_pPlot->curve[iCurve].linesize = iLineSize;
        m_pPlot->curve[iCurve].linecolor.r = 30;
        m_pPlot->curve[iCurve].linecolor.g = 90;
        m_pPlot->curve[iCurve].linecolor.b = 190;
    }

    if (m_bPlotPeaks && bPlotPeaks) {
        if (iFitColumnCount == 2) {
            m_pPlot->errcode = 0;
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp1, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 20;
                m_pPlot->curve[iCurve].linecolor.b = 150;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp2, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 70;
                m_pPlot->curve[iCurve].linecolor.b = 20;
            }
        }
        else if (iFitColumnCount == 3) {
            m_pPlot->errcode = 0;
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp1, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 20;
                m_pPlot->curve[iCurve].linecolor.b = 150;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp2, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 70;
                m_pPlot->curve[iCurve].linecolor.b = 20;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp3, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 150;
                m_pPlot->curve[iCurve].linecolor.g = 140;
                m_pPlot->curve[iCurve].linecolor.b = 16;
            }
        }
        else if (iFitColumnCount == 4) {
            m_pPlot->errcode = 0;
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp1, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 20;
                m_pPlot->curve[iCurve].linecolor.b = 150;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp2, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 70;
                m_pPlot->curve[iCurve].linecolor.b = 20;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp3, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 150;
                m_pPlot->curve[iCurve].linecolor.g = 140;
                m_pPlot->curve[iCurve].linecolor.b = 16;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp4, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 16;
                m_pPlot->curve[iCurve].linecolor.g = 154;
                m_pPlot->curve[iCurve].linecolor.b = 80;
            }
        }
        else if (iFitColumnCount == 5) {
            m_pPlot->errcode = 0;
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp1, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 20;
                m_pPlot->curve[iCurve].linecolor.b = 150;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp2, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 200;
                m_pPlot->curve[iCurve].linecolor.g = 70;
                m_pPlot->curve[iCurve].linecolor.b = 20;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp3, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 150;
                m_pPlot->curve[iCurve].linecolor.g = 140;
                m_pPlot->curve[iCurve].linecolor.b = 16;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp4, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 16;
                m_pPlot->curve[iCurve].linecolor.g = 154;
                m_pPlot->curve[iCurve].linecolor.b = 80;
            }
            iCurve = pl_curve_add(m_pPlot, m_pColumnFitX, m_pColumnFitYp5, NULL, NULL, m_pPlot->curve[m_pPlot->curveactive].axisused, 0x01);
            if ((iCurve >= 0) && (iCurve < m_pPlot->curvecount)) {
                m_pPlot->curve[iCurve].dotstyle = 0x00;
                m_pPlot->curve[iCurve].linesize = iLineSize;
                m_pPlot->curve[iCurve].linecolor.r = 16;
                m_pPlot->curve[iCurve].linecolor.g = 130;
                m_pPlot->curve[iCurve].linecolor.b = 184;
            }
        }
    }

    m_bOK = TRUE;

    char_t szBuffer[ML_STRSIZES];
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    SigmaApp.Reformat(m_pSelectedModel->chi, szBuffer, ML_STRSIZES - 1);
    m_edtChi2.SetWindowText((LPCTSTR)szBuffer);
    _tcsprintf(szBuffer, ML_STRSIZES - 1, _T("%d"), nIter);
    m_edtIter.SetWindowText((LPCTSTR)szBuffer);

    ParamsToControls();

    // >> UserFit
    if (bUserModel == TRUE) { ml_fit_userend(); }
    // <<

    if (m_bReport == TRUE) {
        PrintReport(iCurve);
    }

    m_pDoc->SetModifiedFlag(TRUE);
    m_pDoc->UpdateAllViews(NULL);
    ((CSigmaDoc*) m_pDoc)->ActivateView(1);

    *m_pbRunning = FALSE;

    *m_pbIsModified = TRUE;
}

void CFitterModel::doAutoPar(bool bVerbose)
{

    if (*m_pbRunning == TRUE) {
        return;
    }
    if (m_pSelectedModel == NULL) {
        if (bVerbose) {
            this->MessageBox(_T("Cannot perform operation: no fitting model selected."), _T("SigmaGraph"), MB_OK | MB_ICONEXCLAMATION);
        }
        return;
    }

    *m_pbRunning = TRUE;

    UpdateData(TRUE);
    ControlsToParams();

    SigmaApp.m_pLib->errcode = 0;

    real_t fparBak[ML_FIT_MAXPAR];
    for (int ii = 0; ii < m_pSelectedModel->parcount; ii++) {
        fparBak[ii] = m_pSelectedModel->fpar[ii];
    }
    int iret = ml_fit_autopar(m_pColumnDataX, m_pColumnDataY, m_pSelectedModel->index - 1, m_pSelectedModel->fpar, SigmaApp.m_pLib);
    for (int ii = 0; ii < m_pSelectedModel->parcount; ii++) {
        if (m_pSelectedModel->mpar[ii] == 0) {
            m_pSelectedModel->fpar[ii] = fparBak[ii];
        }
    }
    if ((iret < 0) && (_tcslen(SigmaApp.m_pLib->message) > 5)) {
        if (bVerbose) {
            this->MessageBox(SigmaApp.m_pLib->message, _T("SigmaGraph"), MB_OK | MB_ICONEXCLAMATION);
        }
    }

    ParamsToControls();

    *m_pbRunning = FALSE;
}

void CFitterModel::OnAutoPar()
{
    doAutoPar(true);
}

void CFitterModel::OnChangedFitterModel(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    ActivateFitterModel();

    *pResult = 0;
}

void CFitterModel::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // Do not call CDialog::OnPaint() for painting messages

    if (IsWindowVisible()) {
        if (m_bInit == FALSE) {
            m_Bitmap.DeleteObject();
            if (m_Bitmap.LoadBitmap(IDB_FITTER_MODEL99)) {
                m_bmpFunction.SetBitmap(m_Bitmap);
            }
            ParamsToControls();
            m_bInit = TRUE;
        }
    }
}

void CFitterModel::updateNextPar(void)
{
    CWnd *pWnd = GetDlgItem(ID_FITTER_NEXTPAR);

    if (m_pSelectedModel == NULL) {
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Page 1/1"));
            pWnd->EnableWindow(FALSE);
        }

        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Fitting Parameters"));
        }
        return;
    }

    int ifun = m_pSelectedModel->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    if (m_pSelectedModel->parcount <= 4) {
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Page 1/1"));
            pWnd->EnableWindow(FALSE);
        }

        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Fitting Parameters"));
        }
        m_iPage = 0;
    }
    else {
        CString strT;
        int iPageMax = 1;
        if (bPeakM) {
            if ((m_pSelectedModel->parcount > 4) && (m_pSelectedModel->parcount <= 7)) {
                iPageMax = 2;
            }
            else if ((m_pSelectedModel->parcount > 7) && (m_pSelectedModel->parcount <= 10)) {
                iPageMax = 3;
            }
            else if ((m_pSelectedModel->parcount > 10) && (m_pSelectedModel->parcount <= 13)) {
                iPageMax = 4;
            }
            else if ((m_pSelectedModel->parcount > 13) && (m_pSelectedModel->parcount <= 16)) {
                iPageMax = 5;
            }
        }
        else {
            if ((m_pSelectedModel->parcount > 4) && (m_pSelectedModel->parcount <= 8)) {
                iPageMax = 2;
            }
            else if ((m_pSelectedModel->parcount > 8) && (m_pSelectedModel->parcount <= 12)) {
                iPageMax = 3;
            }
            else if ((m_pSelectedModel->parcount > 12) && (m_pSelectedModel->parcount <= 16)) {
                iPageMax = 4;
            }
            else if ((m_pSelectedModel->parcount > 16) && (m_pSelectedModel->parcount <= 20)) {
                iPageMax = 5;
            }
        }
        if (pWnd != NULL) {
            strT.Format(_T("Page %d/%d >>"), m_iPage + 1, iPageMax);
            pWnd->SetWindowText(strT);
            pWnd->EnableWindow(TRUE);
        }
        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            strT.Format(_T("Fitting Parameters (Page %d/%d)"), m_iPage + 1, iPageMax);
            pWnd->SetWindowText(strT);
        }
    }

    CString strT;

    pWnd = GetDlgItem(IDL_FITTER_PARA);
    if (pWnd != NULL) {
        pWnd->SetWindowText(_T("a"));
    }
    pWnd = GetDlgItem(IDL_FITTER_PARB);
    if (pWnd != NULL) {
        if (bPeakM) {
            strT.Format(_T("b%d"), m_iPage + 1);
            pWnd->SetWindowText(strT);
        }
        else {
            pWnd->SetWindowText(_T("b"));
        }
    }
    pWnd = GetDlgItem(IDL_FITTER_PARC);
    if (pWnd != NULL) {
        if (bPeakM) {
            strT.Format(_T("c%d"), m_iPage + 1);
            pWnd->SetWindowText(strT);
        }
        else {
            pWnd->SetWindowText(_T("c"));
        }
    }
    pWnd = GetDlgItem(IDL_FITTER_PARD);
    if (pWnd != NULL) {
        if (bPeakM) {
            strT.Format(_T("d%d"), m_iPage + 1);
            pWnd->SetWindowText(strT);
        }
        else {
            pWnd->SetWindowText(_T("d"));
        }
    }
}

void CFitterModel::ControlsToParams()
{
    if (m_pSelectedModel == NULL) {
        return;
    }

    int ifun = m_pSelectedModel->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    int iT = (m_iPage << 2);

    // Save data before changing page
    if (bPeakM && (iT > 0)) {
        if (m_iPage == 2) {
            iT = 7;
        }
        else if (m_iPage == 3) {
            iT = 10;
        }
        else if (m_iPage == 4) {
            iT = 13;
        }
        m_pSelectedModel->fpar[iT] = m_edtParB.GetValue();
        m_pSelectedModel->fpar[iT + 1] = m_edtParC.GetValue();
        m_pSelectedModel->fpar[iT + 2] = m_edtParD.GetValue();
        m_pSelectedModel->mpar[iT] = (m_bFixedParB == TRUE) ? 0 : 1;
        m_pSelectedModel->mpar[iT + 1] = (m_bFixedParC == TRUE) ? 0 : 1;
        m_pSelectedModel->mpar[iT + 2] = (m_bFixedParD == TRUE) ? 0 : 1;
    }
    else {
        m_pSelectedModel->fpar[iT] = m_edtParA.GetValue();
        m_pSelectedModel->fpar[iT + 1] = m_edtParB.GetValue();
        m_pSelectedModel->mpar[iT] = (m_bFixedParA == TRUE) ? 0 : 1;
        m_pSelectedModel->mpar[iT + 1] = (m_bFixedParB == TRUE) ? 0 : 1;

        if (m_pSelectedModel->parcount > (iT + 2)) {
            m_pSelectedModel->fpar[iT + 2] = m_edtParC.GetValue();
            m_pSelectedModel->mpar[iT + 2] = (m_bFixedParC == TRUE) ? 0 : 1;
        }
        if (m_pSelectedModel->parcount > (iT + 3)) {
            m_pSelectedModel->fpar[iT + 3] = m_edtParD.GetValue();
            m_pSelectedModel->mpar[iT + 3] = (m_bFixedParD == TRUE) ? 0 : 1;
        }
    }
}

void CFitterModel::ParamsToControls()
{
    if ((m_pSelectedModel == NULL) || (m_pSelectedModel->index <= 0)) {
        m_edtParA.SetWindowText(_T(""));
        m_edtParB.SetWindowText(_T(""));
        m_edtParC.SetWindowText(_T(""));
        m_edtParD.SetWindowText(_T(""));
        m_bFixedParA = FALSE;
        m_bFixedParB = FALSE;
        m_bFixedParC = FALSE;
        m_bFixedParD = FALSE;
        m_bPlotPeaks = FALSE;
        m_cbPlotPeaks.ShowWindow(SW_HIDE);
        CWnd *pWnd = GetDlgItem(ID_FITTER_NEXTPAR);
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Page 1/1"));
            pWnd->EnableWindow(FALSE);
        }
        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Fitting Parameters"));
        }
        UpdateData(FALSE);
        return;
    }

    int ifun = m_pSelectedModel->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    int iT = (m_iPage << 2);

    m_cbPlotPeaks.ShowWindow(bPeakM ? SW_SHOW : SW_HIDE);

    if ((bPeakM == false) || (iT < 1)) {
        m_edtParA.SetValue(m_pSelectedModel->fpar[iT]);
        m_edtParB.SetValue(m_pSelectedModel->fpar[iT + 1]);
        m_bFixedParA = (m_pSelectedModel->mpar[iT] == 0) ? TRUE : FALSE;
        m_bFixedParB = (m_pSelectedModel->mpar[iT + 1] == 0) ? TRUE : FALSE;

        if (m_pSelectedModel->parcount > (iT + 1)) {
            m_edtParB.SetValue(m_pSelectedModel->fpar[iT + 1]);
        }
        else {
            m_edtParB.SetWindowText(_T(""));
        }

        if (m_pSelectedModel->parcount > (iT + 2)) {
            m_edtParC.SetValue(m_pSelectedModel->fpar[iT + 2]);
            m_bFixedParC = (m_pSelectedModel->mpar[iT + 2] == 0) ? TRUE : FALSE;
        }
        else {
            m_edtParC.SetWindowText(_T(""));
            m_bFixedParC = FALSE;
        }

        if (m_pSelectedModel->parcount > (iT + 3)) {
            m_edtParD.SetValue(m_pSelectedModel->fpar[iT + 3]);
            m_bFixedParD = (m_pSelectedModel->mpar[iT + 3] == 0) ? TRUE : FALSE;
        }
        else {
            m_edtParD.SetWindowText(_T(""));
            m_bFixedParD = FALSE;
        }
    }
    else {
        if (m_iPage == 2) {
            iT = 7;
        }
        else if (m_iPage == 3) {
            iT = 10;
        }
        else if (m_iPage == 4) {
            iT = 13;
        }

        m_edtParA.SetValue(m_pSelectedModel->fpar[0]);
        m_edtParB.SetValue(m_pSelectedModel->fpar[iT]);
        m_edtParC.SetValue(m_pSelectedModel->fpar[iT + 1]);
        m_edtParD.SetValue(m_pSelectedModel->fpar[iT + 2]);

        m_bFixedParA = (m_pSelectedModel->mpar[0] == 0) ? TRUE : FALSE;
        m_bFixedParB = (m_pSelectedModel->mpar[iT] == 0) ? TRUE : FALSE;
        m_bFixedParC = (m_pSelectedModel->mpar[iT + 1] == 0) ? TRUE : FALSE;
        m_bFixedParD = (m_pSelectedModel->mpar[iT + 2] == 0) ? TRUE : FALSE;
    }

    CWnd *pWnd = GetDlgItem(ID_FITTER_NEXTPAR);

    if (m_pSelectedModel == NULL) {
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Page 1/1"));
            pWnd->EnableWindow(FALSE);
        }

        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Fitting Parameters"));
        }
        return;
    }

    if (m_pSelectedModel->parcount <= 4) {
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Page 1/1"));
            pWnd->EnableWindow(FALSE);
        }

        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Fitting Parameters"));
        }
        m_iPage = 0;
    }
    else {
        CString strT;
        int iPageMax = 1;
        if (bPeakM) {
            if ((m_pSelectedModel->parcount > 4) && (m_pSelectedModel->parcount <= 7)) {
                iPageMax = 2;
            }
            else if ((m_pSelectedModel->parcount > 7) && (m_pSelectedModel->parcount <= 10)) {
                iPageMax = 3;
            }
            else if ((m_pSelectedModel->parcount > 10) && (m_pSelectedModel->parcount <= 13)) {
                iPageMax = 4;
            }
            else if ((m_pSelectedModel->parcount > 13) && (m_pSelectedModel->parcount <= 16)) {
                iPageMax = 5;
            }
        }
        else {
            if ((m_pSelectedModel->parcount > 4) && (m_pSelectedModel->parcount <= 8)) {
                iPageMax = 2;
            }
            else if ((m_pSelectedModel->parcount > 8) && (m_pSelectedModel->parcount <= 12)) {
                iPageMax = 3;
            }
            else if ((m_pSelectedModel->parcount > 12) && (m_pSelectedModel->parcount <= 16)) {
                iPageMax = 4;
            }
            else if ((m_pSelectedModel->parcount > 16) && (m_pSelectedModel->parcount <= 20)) {
                iPageMax = 5;
            }
        }
        if (pWnd != NULL) {
            strT.Format(_T("Page %d/%d >>"), m_iPage + 1, iPageMax);
            pWnd->SetWindowText(strT);
            pWnd->EnableWindow(TRUE);
        }
        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            strT.Format(_T("Fitting Parameters (Page %d/%d)"), m_iPage + 1, iPageMax);
            pWnd->SetWindowText(strT);
        }
    }

    CString strT;
    char_t szT[2];
    szT[1] = _T('\0');

    pWnd = GetDlgItem(IDL_FITTER_PARA);
    if (pWnd != NULL) {
        szT[0] = (bPeakM) ? _T('a') : (_T('a') + iT);
        pWnd->SetWindowText(szT);
    }
    pWnd = GetDlgItem(IDL_FITTER_PARB);
    if (pWnd != NULL) {
        if (bPeakM) {
            strT.Format(_T("b%d"), m_iPage + 1);
            pWnd->SetWindowText(strT);
        }
        else {
            szT[0] = _T('b') + iT;
            pWnd->SetWindowText(szT);
            m_cbFixedParB.EnableWindow((m_pSelectedModel->parcount > (iT + 1)));
            m_edtParB.EnableWindow((m_pSelectedModel->parcount > (iT + 1)));
        }
    }
    pWnd = GetDlgItem(IDL_FITTER_PARC);
    if (pWnd != NULL) {
        if (bPeakM) {
            strT.Format(_T("c%d"), m_iPage + 1);
            pWnd->SetWindowText(strT);
        }
        else {
            szT[0] = _T('c') + iT;
            pWnd->SetWindowText(szT);
            m_cbFixedParC.EnableWindow((m_pSelectedModel->parcount > (iT + 2)));
            m_edtParC.EnableWindow((m_pSelectedModel->parcount > (iT + 2)));
        }
    }
    pWnd = GetDlgItem(IDL_FITTER_PARD);
    if (pWnd != NULL) {
        if (bPeakM) {
            strT.Format(_T("d%d"), m_iPage + 1);
            pWnd->SetWindowText(strT);
        }
        else {
            szT[0] = _T('d') + iT;
            pWnd->SetWindowText(szT);
            m_cbFixedParD.EnableWindow((m_pSelectedModel->parcount > (iT + 3)));
            m_edtParD.EnableWindow((m_pSelectedModel->parcount > (iT + 3)));
        }
    }

    UpdateData(FALSE);
}

void CFitterModel::OnFitterNextPar()
{
    if (m_pSelectedModel == NULL) {
        return;
    }

    CWnd *pWnd = GetDlgItem(ID_FITTER_NEXTPAR);
    if (pWnd == NULL) {
        return;
    }

    UpdateData(TRUE);

    if (m_pSelectedModel->parcount <= 4) {
        m_iPage = 0;
        pWnd->SetWindowText(_T("Parameters"));
        pWnd->EnableWindow(FALSE);

        pWnd = GetDlgItem(IDL_FITTER_PARTITLE);
        if (pWnd != NULL) {
            pWnd->SetWindowText(_T("Fitting Parameters"));
        }

        m_pSelectedModel->fpar[0] = m_edtParA.GetValue();
        m_pSelectedModel->fpar[1] = m_edtParB.GetValue();
        m_pSelectedModel->mpar[0] = (m_bFixedParA == TRUE) ? 0 : 1;
        m_pSelectedModel->mpar[1] = (m_bFixedParB == TRUE) ? 0 : 1;

        if (m_pSelectedModel->parcount > 2){
            m_pSelectedModel->fpar[2] = m_edtParC.GetValue();
            m_pSelectedModel->mpar[2] = (m_bFixedParC == TRUE) ? 0 : 1;
        }
        if (m_pSelectedModel->parcount > 3){
            m_pSelectedModel->fpar[3] = m_edtParD.GetValue();
            m_pSelectedModel->mpar[3] = (m_bFixedParD == TRUE) ? 0 : 1;
        }

        return;
    }

    int ifun = m_pSelectedModel->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    int iPageMax = 1;
    if (bPeakM) {
        if ((m_pSelectedModel->parcount > 4) && (m_pSelectedModel->parcount <= 7)) {
            iPageMax = 2;
        }
        else if ((m_pSelectedModel->parcount > 7) && (m_pSelectedModel->parcount <= 10)) {
            iPageMax = 3;
        }
        else if ((m_pSelectedModel->parcount > 10) && (m_pSelectedModel->parcount <= 13)) {
            iPageMax = 4;
        }
        else if ((m_pSelectedModel->parcount > 13) && (m_pSelectedModel->parcount <= 16)) {
            iPageMax = 5;
        }
    }
    else {
        if ((m_pSelectedModel->parcount > 4) && (m_pSelectedModel->parcount <= 8)) {
            iPageMax = 2;
        }
        else if ((m_pSelectedModel->parcount > 8) && (m_pSelectedModel->parcount <= 12)) {
            iPageMax = 3;
        }
        else if ((m_pSelectedModel->parcount > 12) && (m_pSelectedModel->parcount <= 16)) {
            iPageMax = 4;
        }
        else if ((m_pSelectedModel->parcount > 16) && (m_pSelectedModel->parcount <= 20)) {
            iPageMax = 5;
        }
    }

    ControlsToParams();

    m_iPage += 1;
    if (m_iPage >= iPageMax) {
        m_iPage = 0;
    }

    ParamsToControls();
}

void CFitterModel::OnOK()
{
    if (*(m_pbRunning) == TRUE) {
        if (this->MessageBox(_T("Stop calculations and end the fitting session?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
            return;
        }
    }
    if ((m_bOK == TRUE) && (m_pSelectedModel != NULL)) {
        long_t uiID = m_pFit->id;
        SigmaApp.m_pLib->errcode = 0;
        ml_fit_dup(m_pFit, (const fit_t*)(m_pSelectedModel));
        m_pFit->id = uiID;
        m_pFit->points = m_pColumnFitY->dim;
    }

    CPropertyPage::OnOK();
}

void CFitterModel::OnCancel()
{
    if (*(m_pbRunning) == TRUE) {
        if (this->MessageBox(_T("Stop calculations and end the fitting session?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION) != IDYES) {
            return;
        }
    }

    CPropertyPage::OnCancel();
}

void CFitterModel::OnFitterReset()
{
    CSigmaDoc *pDoc = (CSigmaDoc*)(m_pDoc);

    if (IsDlgButtonChecked(IDK_USERMODEL) == BST_CHECKED) {
        memset(&(pDoc->m_FittingUserModel), 0, sizeof(fit_t));
        pDoc->m_FittingUserModel.iter = SigmaApp.m_Options.fit.iter;
        pDoc->m_FittingUserModel.lambda = SigmaApp.m_Options.fit.lambda;

        CComboBox *pUserModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
        if (pUserModel != NULL) {
            int_t jj;
            int_t iSel = pUserModel->GetCurSel();
            if ((iSel >= 0) && (iSel < SigmaApp.m_iUserFitCount)) {
                if (_tcslen((const char_t*)(SigmaApp.m_pUserFit[iSel]->name)) > 0) {
                    m_pSelectedModel = &(((CSigmaDoc*)m_pDoc)->m_FittingUserModel);
                    m_pSelectedModel->id = SigmaApp.m_pUserFit[iSel]->id;
                    m_pSelectedModel->index = SigmaApp.m_pUserFit[iSel]->index;
                    m_pSelectedModel->parcount = SigmaApp.m_pUserFit[iSel]->parcount;
                    _tcsncpy(m_pSelectedModel->name, (const char_t*)(SigmaApp.m_pUserFit[iSel]->name), ML_STRSIZES - 1);
                    _tcsncpy(m_pSelectedModel->function, (const char_t*)(SigmaApp.m_pUserFit[iSel]->function), ML_STRSIZE - 1);
                    for (jj = 0; jj < m_pSelectedModel->parcount; jj++) {
                        m_pSelectedModel->fpar[jj] = SigmaApp.m_pUserFit[iSel]->fpar[jj];
                        m_pSelectedModel->mpar[jj] = 1;
                    }
                }
            }
        }
    }
    else {
        memset(pDoc->m_FittingModel, 0, ML_FIT_MAXMODEL * sizeof(fit_t));
        ml_fit_getmodel(pDoc->m_FittingModel, ML_FIT_MAXMODEL);
        m_pFit->lambda = SigmaApp.m_Options.fit.lambda;
        m_pFit->iter = SigmaApp.m_Options.fit.iter;
    }

    ParamsToControls();
}

void CFitterModel::PrintReport(int_t iCurve)
{
    int ii, iText = -1;

    if (m_pSelectedModel == NULL) {
        return;
    }

    if (m_pPlot->textcount >= PL_MAXITEMS) {
        return;
    }

    BOOL bModifyText = FALSE;

    // Le texte existe déjà ?
    if (m_pPlot->textcount > 0) {
        for (ii = 0; ii < m_pPlot->textcount; ii++) {
            if (m_pPlot->text[ii].id == m_pFit->id) {
                iText = ii;
                bModifyText = TRUE;
                break;
            }
        }
    }
    if (iText < 0) {
        iText = m_pPlot->textcount;
    }
    if (iCurve < 0) {
        iCurve = iText;
    }

    CString strReport = _T("");

    int ifun = m_pSelectedModel->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    char_t szBuffer[ML_STRSIZES];
    memset(szBuffer, 0, ML_STRSIZES * sizeof(char_t));
    strReport.Format(_T("Model: %s"), m_pSelectedModel->name);
    if (bPeak) {
        strReport.Append(_T("\r\na = "));
        SigmaApp.Reformat(m_pSelectedModel->fpar[0], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (m_pSelectedModel->mpar[0] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(bPeakM ? _T("\r\nb1 = ") : _T("\r\nb = "));
        SigmaApp.Reformat(m_pSelectedModel->fpar[1], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (m_pSelectedModel->mpar[1] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(bPeakM ? _T(" ; c1 = ") : _T(" ; c = "));
        SigmaApp.Reformat(m_pSelectedModel->fpar[2], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (m_pSelectedModel->mpar[2] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(bPeakM ? _T(" ; d1 = ") : _T(" ; d = "));
        SigmaApp.Reformat(m_pSelectedModel->fpar[3], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (m_pSelectedModel->mpar[3] == 0) {
            strReport.Append(_T(" (fixed)"));
        }

        if (m_pSelectedModel->parcount > 4) {
            strReport.Append(_T("\r\nb2 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[4], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[4] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 5) {
            strReport.Append(_T(" ; c2 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[5], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[5] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 6) {
            strReport.Append(_T(" ; d2 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[6], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[6] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 7) {
            strReport.Append(_T("\r\nb3 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[7], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[7] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 8) {
            strReport.Append(_T(" ; c3 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[8], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[8] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 9) {
            strReport.Append(_T(" ; d3 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[9], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[9] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 10) {
            strReport.Append(_T("\r\nb4 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[10], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[10] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 11) {
            strReport.Append(_T(" ; c4 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[11], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[11] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 12) {
            strReport.Append(_T(" ; d4 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[12], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[12] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 13) {
            strReport.Append(_T("\r\nb5 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[13], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[13] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 14) {
            strReport.Append(_T("\r\nc5 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[14], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[14] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
        if (m_pSelectedModel->parcount > 15) {
            strReport.Append(_T("\r\nd5 = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[15], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[15] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
    }
    else {
        strReport.Append(_T("\r\na = "));
        SigmaApp.Reformat(m_pSelectedModel->fpar[0], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (m_pSelectedModel->mpar[0] == 0) {
            strReport.Append(_T(" (fixed)"));
        }
        strReport.Append(_T(" ; b = "));
        SigmaApp.Reformat(m_pSelectedModel->fpar[1], szBuffer, ML_STRSIZES - 1);
        strReport.Append((LPCTSTR) szBuffer);
        if (m_pSelectedModel->mpar[1] == 0) {
            strReport.Append(_T(" (fixed)"));
        }

        if (m_pSelectedModel->parcount > 2) {
            strReport.Append(_T(" ; c = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[2], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[2] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 3) {
            strReport.Append(_T("\r\nd = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[3], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[3] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 4) {
            strReport.Append(_T(" ; e = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[4], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[4] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 5) {
            strReport.Append(_T(" ; f = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[5], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[5] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 6) {
            strReport.Append(_T("\r\ng = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[6], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[6] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 7) {
            strReport.Append(_T(" ; h = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[7], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[7] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 8) {
            strReport.Append(_T(" ; i = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[8], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[8] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 9) {
            strReport.Append(_T("\r\nj = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[9], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[9] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 10) {
            strReport.Append(_T(" ; k = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[10], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[10] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 11) {
            strReport.Append(_T(" ; l = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[11], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[11] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 12) {
            strReport.Append(_T("\r\n m = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[12], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[12] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 13) {
            strReport.Append(_T(" ; n = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[13], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[13] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 14) {
            strReport.Append(_T(" ; o = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[14], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[14] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }

        if (m_pSelectedModel->parcount > 15) {
            strReport.Append(_T("\r\np = "));
            SigmaApp.Reformat(m_pSelectedModel->fpar[15], szBuffer, ML_STRSIZES - 1);
            strReport.Append((LPCTSTR) szBuffer);
            if (m_pSelectedModel->mpar[15] == 0) {
                strReport.Append(_T(" (fixed)"));
            }
        }
    }

    int_t nlen = SigmaApp.CStringToChar(strReport, m_pPlot->text[iText].text, ML_STRSIZE - 1);
    if ((nlen < 7) || (nlen > (ML_STRSIZE - 1))) {
        if (bModifyText == TRUE) {    // Supprimer le texte
            m_pPlot->text[iText].len = 0;
            memset(m_pPlot->text[iText].text, 0, ML_STRSIZE);
            memset(&(m_pPlot->text[iText].rect), 0, sizeof(rect_t));
            m_pPlot->textcount -= 1;
        }
        return;
    }

    if (bModifyText == FALSE) {    // Nouveau texte
        real_t ww = m_pPlot->plint.width, hh = m_pPlot->plint.height;
        if ((ww < 100.0) || (hh < 100.0)) {
            return;
        }

        if (m_pPlot->curvecount > 0) {
            m_pPlot->text[iText].rect.xa = m_pPlot->curve[m_pPlot->curvecount - 1].lrelpos.xa;
            m_pPlot->text[iText].rect.ya = m_pPlot->curve[m_pPlot->curvecount - 1].lrelpos.yb + 0.01;
        }
        else {
            m_pPlot->text[iText].rect.xa = m_pPlot->relpos[1].xa + 0.01;
            m_pPlot->text[iText].rect.ya = m_pPlot->relpos[1].ya + 0.01;
        }
        m_pPlot->text[iText].rect.xb = m_pPlot->text[iText].rect.xa + 0.1;
        m_pPlot->text[iText].rect.yb = m_pPlot->text[iText].rect.ya + 0.1;

        m_pPlot->text[iText].status &= 0xF0;
        m_pPlot->text[iText].font.orient = 0x00;
        m_pPlot->text[iText].font.color.r = m_pPlot->curve[iCurve].linecolor.r;
        m_pPlot->text[iText].font.color.g = m_pPlot->curve[iCurve].linecolor.g;
        m_pPlot->text[iText].font.color.b = m_pPlot->curve[iCurve].linecolor.b;
        m_pPlot->text[iText].id = m_pFit->id;
        m_pPlot->text[iText].type = 0x01;
        m_pPlot->text[iText].border = 0x01;
        m_pPlot->textcount += 1;
    }

    return;
}

void CFitterModel::ActivateFitterUserModel()
{
    if (SigmaApp.m_iUserFitCount < 1) {
        return;
    }

    CComboBox *pUserModel = (CComboBox*)GetDlgItem(IDC_USERMODEL_LIST);
    if (pUserModel == NULL) {
        return;
    }

    int_t jj;
    int_t iSel = pUserModel->GetCurSel();
    if ((iSel >= 0) && (iSel < SigmaApp.m_iUserFitCount)) {
        if (_tcslen((const char_t*)(SigmaApp.m_pUserFit[iSel]->name)) > 0) {
            m_Bitmap.DeleteObject();
            if (m_Bitmap.LoadBitmap(IDB_FITTER_MODEL255)) {
                m_bmpFunction.SetBitmap(m_Bitmap);
            }
            m_pSelectedModel = &(((CSigmaDoc*)m_pDoc)->m_FittingUserModel);
            m_pSelectedModel->id = SigmaApp.m_pUserFit[iSel]->id;
            m_pSelectedModel->index = SigmaApp.m_pUserFit[iSel]->index;
            m_pSelectedModel->parcount = SigmaApp.m_pUserFit[iSel]->parcount;
            _tcsncpy(m_pSelectedModel->name, (const char_t*)(SigmaApp.m_pUserFit[iSel]->name), ML_STRSIZES - 1);
            _tcsncpy(m_pSelectedModel->function, (const char_t*)(SigmaApp.m_pUserFit[iSel]->function), ML_STRSIZE - 1);
            for (jj = 0; jj < m_pSelectedModel->parcount; jj++) {
                m_pSelectedModel->fpar[jj] = SigmaApp.m_pUserFit[iSel]->fpar[jj];
                m_pSelectedModel->mpar[jj] = 1;
            }

            updateNextPar();

            m_edtParA.SetValue(m_pSelectedModel->fpar[0]);
            m_edtParB.SetValue(m_pSelectedModel->fpar[1]);

            if (m_pSelectedModel->parcount > 2) {
                m_edtParC.SetValue(m_pSelectedModel->fpar[2]);

            }
            else {
                m_edtParC.SetWindowText(_T(""));
            }

            if (m_pSelectedModel->parcount > 3) {
                m_edtParD.SetValue(m_pSelectedModel->fpar[3]);

            }
            else {
                m_edtParD.SetWindowText(_T(""));
            }

            m_bFixedParA = (m_pSelectedModel->mpar[0] == 0) ? TRUE : FALSE;
            m_bFixedParB = (m_pSelectedModel->mpar[1] == 0) ? TRUE : FALSE;
            m_bFixedParC = (m_pSelectedModel->mpar[2] == 0) ? TRUE : FALSE;
            m_bFixedParD = (m_pSelectedModel->mpar[3] == 0) ? TRUE : FALSE;

            m_cbFixedParA.EnableWindow(TRUE);
            m_cbFixedParB.EnableWindow(TRUE);
            m_cbFixedParC.EnableWindow(TRUE);
            m_cbFixedParD.EnableWindow(TRUE);

            m_edtParA.EnableWindow(TRUE);
            m_edtParB.EnableWindow(TRUE);
            m_edtParC.EnableWindow(TRUE);
            m_edtParD.EnableWindow(TRUE);

            if (m_pSelectedModel->parcount == 2) {
                m_cbFixedParC.EnableWindow(FALSE);
                m_cbFixedParD.EnableWindow(FALSE);
                m_edtParC.EnableWindow(FALSE);
                m_edtParD.EnableWindow(FALSE);
            }
            if (m_pSelectedModel->parcount == 3) {
                m_cbFixedParD.EnableWindow(FALSE);
                m_edtParD.EnableWindow(FALSE);
            }

            int_t ifun = m_pSelectedModel->index - 1;
            BOOL bAutopar = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
                || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
                || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
                || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5)
                || (ifun == FIT_MODEL_PEARSON)
                || (ifun == FIT_MODEL_EXPGR) || (ifun == FIT_MODEL_EXPGR2) || (ifun == FIT_MODEL_EXPDEC)
                || (ifun == FIT_MODEL_EXPDEC2));
            CWnd *pWnd = GetDlgItem(ID_FITTER_AUTOPAR);
            if (pWnd != NULL) {
                pWnd->EnableWindow(bAutopar);
            }

            UpdateData(FALSE);
        }
    }
}

void CFitterModel::OnCbnSelchangeUsermodelList()
{
    ActivateFitterUserModel();
}

void CFitterModel::ActivateFitterModel(void)
{
    UINT uiBmp[ML_FIT_MAXMODEL] = {
        IDB_FITTER_MODEL01, IDB_FITTER_MODEL02, IDB_FITTER_MODEL03,
        IDB_FITTER_MODEL04, IDB_FITTER_MODEL05, IDB_FITTER_MODEL06,
        IDB_FITTER_MODEL07, IDB_FITTER_MODEL08, IDB_FITTER_MODEL09,
        IDB_FITTER_MODEL10, IDB_FITTER_MODEL11, IDB_FITTER_MODEL12,
        IDB_FITTER_MODEL13, IDB_FITTER_MODEL14, IDB_FITTER_MODEL15,
        IDB_FITTER_MODEL16, IDB_FITTER_MODEL17, IDB_FITTER_MODEL18,
        IDB_FITTER_MODEL19, IDB_FITTER_MODEL20, IDB_FITTER_MODEL21,
        IDB_FITTER_MODEL22, IDB_FITTER_MODEL23, IDB_FITTER_MODEL24
    };

    char_t szBuffer[ML_STRSIZE];
    DWORD cchBuf(ML_STRSIZE);
    LVITEM lvi;
    lvi.iSubItem = 0;
    lvi.mask = LVIF_TEXT;
    lvi.pszText = szBuffer;
    lvi.cchTextMax = cchBuf;

    POSITION tPos = m_lstModel.GetFirstSelectedItemPosition();
    int_t nSel = m_lstModel.GetNextSelectedItem(tPos);

    if ((nSel < 0) || (nSel >= ML_FIT_MAXMODEL)) {
        if (IsWindowVisible()) {
            m_Bitmap.DeleteObject();
            m_Bitmap.LoadBitmap(IDB_FITTER_MODEL99);
            m_bmpFunction.SetBitmap(m_Bitmap);
        }
        return;
    }
    lvi.iItem = nSel;

    m_iPage = 0;

    nSel = 0;
    if (m_lstModel.GetItem(&lvi)) {
        for (int_t ii = 0; ii < ML_FIT_MAXMODEL; ii++) {
            if (_tcsicmp((const char_t*)(lvi.pszText), (const char_t*)(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].name)) == 0) {
                nSel = ((CSigmaDoc*)m_pDoc)->m_FittingModel[ii].index;
                m_pSelectedModel = &(((CSigmaDoc*)m_pDoc)->m_FittingModel[ii]);
                //
                ParamsToControls();
                break;
            }
        }
    }

    if (m_pSelectedModel == NULL) {
        if (IsWindowVisible()) {
            m_Bitmap.DeleteObject();
            m_Bitmap.LoadBitmap(IDB_FITTER_MODEL99);
            m_bmpFunction.SetBitmap(m_Bitmap);
        }
        return;
    }

    if (IsWindowVisible()) {
        if (nSel > 0) {
            m_Bitmap.DeleteObject();
            m_Bitmap.LoadBitmap(uiBmp[nSel - 1]);
        }
        else {
            m_Bitmap.DeleteObject();
            m_Bitmap.LoadBitmap(IDB_FITTER_MODEL99);
        }
        m_bmpFunction.SetBitmap(m_Bitmap);
    }

    if (nSel >= 0) {
        m_cbFixedParA.EnableWindow(TRUE);
        m_cbFixedParB.EnableWindow(TRUE);
        m_cbFixedParC.EnableWindow(TRUE);
        m_cbFixedParD.EnableWindow(TRUE);

        m_edtParA.EnableWindow(TRUE);
        m_edtParB.EnableWindow(TRUE);
        m_edtParC.EnableWindow(TRUE);
        m_edtParD.EnableWindow(TRUE);

        if (m_pSelectedModel->parcount == 2) {
            m_cbFixedParC.EnableWindow(FALSE);
            m_cbFixedParD.EnableWindow(FALSE);
            m_edtParC.EnableWindow(FALSE);
            m_edtParD.EnableWindow(FALSE);
        }
        if (m_pSelectedModel->parcount == 3) {
            m_cbFixedParD.EnableWindow(FALSE);
            m_edtParD.EnableWindow(FALSE);
        }
    }

    int_t ifun = m_pSelectedModel->index - 1;
    BOOL bAutopar = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5)
        || (ifun == FIT_MODEL_PEARSON)
        || (ifun == FIT_MODEL_EXPGR) || (ifun == FIT_MODEL_EXPGR2) || (ifun == FIT_MODEL_EXPDEC)
        || (ifun == FIT_MODEL_EXPDEC2));

    CWnd *pWnd = GetDlgItem(ID_FITTER_AUTOPAR);
    if (pWnd != NULL) {
        pWnd->EnableWindow(bAutopar);
    }

    CheckDlgButton(IDK_USERMODEL, BST_UNCHECKED);

    UpdateData(FALSE);
}

void CFitterModel::OnNMClickFitterModel(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    ActivateFitterModel();

    *pResult = 0;
}

void CFitterModel::OnBnClickedUsermodel()
{
    ActivateFitterUserModel();
}

void CFitterModel::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/fitting.html#fitting"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CFitterModel::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if ((pMsg->wParam == VK_F1)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
        }
    }

    return CPropertyPage::PreTranslateMessage(pMsg);
}

void CFitterModel::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}

