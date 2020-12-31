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
#include "FavoritesDlg.h"
#include "ScriptWindow.h"


// CFavoriteDlg dialog

IMPLEMENT_DYNAMIC(CFavoriteDlg, CDialog)

CFavoriteDlg::CFavoriteDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFavoriteDlg::IDD, pParent)
    , m_strOutput(_T(""))
{
    m_pFavorite = NULL;
    m_piFavorite = NULL;
    m_piFavoriteCount = NULL;
    m_bAutoLoad = FALSE;
    m_bAutoRun = FALSE;
}

CFavoriteDlg::~CFavoriteDlg()
{

}

void CFavoriteDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDL_SCRIPT_FAVORITE, m_lstFavorite);
    DDX_Control(pDX, ID_SCRIPT_ADDFAV, m_bnAdd);
    DDX_Control(pDX, ID_SCRIPT_REMFAV, m_bnRemove);
    DDX_Control(pDX, ID_SCRIPT_LOADFAV, m_bnLoad);
    DDX_Control(pDX, ID_SCRIPT_UPFAV, m_bnUp);
    DDX_Control(pDX, ID_SCRIPT_DOWNFAV, m_bnDown);
    DDX_Control(pDX, IDK_SCRIPT_AUTOLOAD, m_bnAutoLoad);
    DDX_Control(pDX, IDK_SCRIPT_AUTORUN, m_bnAutoRun);
    DDX_Check(pDX, IDK_SCRIPT_AUTOLOAD, m_bAutoLoad);
    DDX_Check(pDX, IDK_SCRIPT_AUTORUN, m_bAutoRun);
    DDX_Text(pDX, IDL_FAV_OUTPUT, m_strOutput);
}


BEGIN_MESSAGE_MAP(CFavoriteDlg, CDialog)
    ON_LBN_SELCHANGE(IDL_SCRIPT_FAVORITE, &CFavoriteDlg::OnLbnSelchangeFavorite)
    ON_BN_CLICKED(ID_SCRIPT_ADDFAV, &CFavoriteDlg::OnFavoriteAdd)
    ON_BN_CLICKED(ID_SCRIPT_REMFAV, &CFavoriteDlg::OnFavoriteRemove)
    ON_BN_CLICKED(ID_SCRIPT_LOADFAV, &CFavoriteDlg::OnFavoriteLoad)
    ON_BN_CLICKED(ID_SCRIPT_UPFAV, &CFavoriteDlg::OnFavoriteUp)
    ON_BN_CLICKED(ID_SCRIPT_DOWNFAV, &CFavoriteDlg::OnFavoriteDown)
    ON_BN_CLICKED(IDK_SCRIPT_AUTORUN, &CFavoriteDlg::OnBnClickedScriptAutorun)
    ON_BN_CLICKED(IDK_SCRIPT_AUTOLOAD, &CFavoriteDlg::OnBnClickedScriptAutoload)
    ON_BN_CLICKED(IDOK, &CFavoriteDlg::OnOK)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CFavoriteDlg::UpdateFavoriteList()
{
    m_lstFavorite.ResetContent();

    if (*m_piFavoriteCount > 0) {
        int_t jj, idl;
        for (jj = 0; jj < *m_piFavoriteCount; jj++) {
            idl = m_lstFavorite.AddString((LPCTSTR)(m_pFavorite[jj].name));
            m_lstFavorite.SetItemData(idl, jj + 1);
        }
    }

    m_bnUp.EnableWindow((*m_piFavorite > 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnDown.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < (*m_piFavoriteCount - 1)));
    m_bnAdd.EnableWindow(*m_piFavoriteCount < SIGMA_SCRIPT_MAXFAV);
    m_bnRemove.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnLoad.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnAutoLoad.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnAutoRun.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
}

// CFavoriteDlg message handlers

BOOL CFavoriteDlg::OnInitDialog()
{
    CDialog::OnInitDialog();


    if ((m_pFavorite == NULL) || (m_piFavoriteCount == NULL) || (m_piFavorite == NULL)) {
        SigmaApp.Output(_T("Cannot show the 'Favorites' dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if ((*m_piFavoriteCount < 0) || (*m_piFavoriteCount > SIGMA_SCRIPT_MAXFAV)) {
        SigmaApp.Output(_T("Cannot show the 'Favorites' dialog: invalid favorites count.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    // Vérifier les favoris
    if (*m_piFavoriteCount > 0) {
        FILE *pf = NULL;
        int_t iFav = 0, jj, mm, nLength, idl;
        for (jj = 0; jj < *m_piFavoriteCount; jj++) {
            pf = _wfopen(m_pFavorite[jj].path, _T("r"));
            if (pf != NULL) {
                fl_getshort(m_pFavorite[jj].name, m_pFavorite[jj].path);
                nLength = (int_t)_tcslen((const char_t*)(m_pFavorite[jj].name));
                for (mm = nLength - 1; mm > 0; mm--) {
                    if (m_pFavorite[jj].name[mm] == _T('.')) {
                        m_pFavorite[jj].name[mm] = _T('\0');
                        break;
                    }
                }
                idl = m_lstFavorite.AddString((LPCTSTR)(m_pFavorite[jj].name));
                m_lstFavorite.SetItemData(idl, jj + 1);
                fclose(pf);    pf = NULL;
                iFav += 1;
            }
        }
        *m_piFavoriteCount = iFav;
    }

    if ((*m_piFavorite < 0) || (*m_piFavorite >= *m_piFavoriteCount) || (*m_piFavoriteCount < 1)) {
        *m_piFavorite = -1;
    }
    else {
        m_bAutoRun = ((m_pFavorite[*m_piFavorite].autorun & 0xF0) == 0x10);
        m_bAutoLoad = ((m_pFavorite[*m_piFavorite].autorun & 0x0F) == 0x01);
    }

    m_lstFavorite.SetCurSel(*m_piFavorite);

    m_bnUp.EnableWindow((*m_piFavorite > 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnDown.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < (*m_piFavoriteCount - 1)));
    m_bnAdd.EnableWindow(*m_piFavoriteCount < SIGMA_SCRIPT_MAXFAV);
    m_bnRemove.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnLoad.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnAutoLoad.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnAutoRun.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CFavoriteDlg::OnFavoriteAdd()
{
    UpdateData(TRUE);

    if (*m_piFavoriteCount >= SIGMA_SCRIPT_MAXFAV) {
        SigmaApp.Output(_T("Cannot add favorite: limit (32) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        m_strOutput = _T("Favorites count limit (32) reached.");
        UpdateData(FALSE);
        return;
    }

    char_t szFilters[] = _T("Script Files (*.lua)|*.lua||");

    CFileDialog *pFileDlg = new CFileDialog(TRUE, _T("lua"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);
    if (pFileDlg->DoModal() != IDOK) {
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString fileName = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    int_t jj, mm, nLength = fileName.GetLength();

    if ((nLength > 7) && (nLength < ML_STRSIZE)) {
        if (*m_piFavoriteCount > 0) {
            char_t szBuffer[ML_STRSIZE];
            memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
            SigmaApp.CStringToChar(fileName, szBuffer, ML_STRSIZE - 1);
            for (jj = 0; jj < *m_piFavoriteCount; jj++) {
                if (_tcsicmp((const char_t*)szBuffer, (const char_t*)(m_pFavorite[jj].path)) == 0) {
                    m_strOutput = _T("Script already in the list.");
                    UpdateData(FALSE);
                    return;
                }
            }
            _tcscpy(m_pFavorite[*m_piFavoriteCount].path, (const char_t*)szBuffer);
        }
        else {
            SigmaApp.CStringToChar(fileName, m_pFavorite[*m_piFavoriteCount].path, ML_STRSIZE - 1);
        }

        fl_getshort(m_pFavorite[*m_piFavoriteCount].name, m_pFavorite[*m_piFavoriteCount].path);
        nLength = (int_t)_tcslen((const char_t*)(m_pFavorite[*m_piFavoriteCount].name));
        if ((nLength > 4) && (nLength < (ML_STRSIZE - 4))) {
            for (mm = nLength - 1; mm > 0; mm--) {
                if (m_pFavorite[*m_piFavoriteCount].name[mm] == _T('.')) {
                    m_pFavorite[*m_piFavoriteCount].name[mm] = _T('\0');
                    break;
                }
            }
        }
        *m_piFavoriteCount += 1;
        *m_piFavorite = *m_piFavoriteCount - 1;
        UpdateFavoriteList();
        m_lstFavorite.SetCurSel(*m_piFavorite);

        m_strOutput = _T("Done.");
    }
    else {
        m_strOutput = _T("Filename not valid.");
    }

    UpdateData(FALSE);
}

void CFavoriteDlg::OnFavoriteRemove()
{
    UpdateData(TRUE);

    if (*m_piFavoriteCount < 1) {
        m_strOutput = _T("Empty list.");
        UpdateData(FALSE);
        return;
    }

    int_t idl = m_lstFavorite.GetCurSel();
    int_t jj = (int_t) (m_lstFavorite.GetItemData(idl)), ll;

    if ((jj < 1) || (jj > *m_piFavoriteCount)) {
        m_strOutput = _T("Selected item not valid.");
        UpdateData(FALSE);
        return;
    }
    jj -= 1;

    if (*m_piFavoriteCount == 1) {
        memset(&(m_pFavorite[0]), 0, sizeof(favorite_t));
        *m_piFavoriteCount = 0;
        *m_piFavorite = -1;
        m_lstFavorite.ResetContent();
        m_lstFavorite.SetCurSel(-1);
        UpdateFavoriteList();
        m_strOutput = _T("Done.");
        UpdateData(FALSE);
        return;
    }
    if (jj == (*m_piFavoriteCount - 1)) {
        memset(&(m_pFavorite[jj]), 0, sizeof(favorite_t));
        *m_piFavoriteCount -= 1;
        *m_piFavorite = 0;
        UpdateFavoriteList();
        m_lstFavorite.SetCurSel(*m_piFavorite);
        m_strOutput = _T("Done.");
        UpdateData(FALSE);
        return;
    }

    for (ll = jj; ll < (*m_piFavoriteCount - 1); ll++) {
        _tcscpy(m_pFavorite[ll].path, (const char_t*)(m_pFavorite[ll + 1].path));
        _tcscpy(m_pFavorite[ll].name, (const char_t*)(m_pFavorite[ll + 1].name));
        m_pFavorite[ll].id = m_pFavorite[ll + 1].id;
        m_pFavorite[ll].type = m_pFavorite[ll + 1].type;
        m_pFavorite[ll].status = m_pFavorite[ll + 1].status;
        m_pFavorite[ll].autorun = m_pFavorite[ll + 1].autorun;
    }
    memset(&(m_pFavorite[*m_piFavoriteCount - 1]), 0, sizeof(favorite_t));
    *m_piFavoriteCount -= 1;

    UpdateFavoriteList();
    m_lstFavorite.SetCurSel(*m_piFavorite);

    m_strOutput = _T("Done.");
    UpdateData(FALSE);
}

void CFavoriteDlg::OnFavoriteUp()
{
    UpdateData(TRUE);

    if (*m_piFavoriteCount < 2) {
        return;
    }

    int_t idl = m_lstFavorite.GetCurSel();
    int_t jj = (int_t) (m_lstFavorite.GetItemData(idl));

    if ((jj <= 1) || (jj > *m_piFavoriteCount)) {
        m_strOutput = _T("Selected item not valid.");
        UpdateData(FALSE);
        return;
    }
    jj -= 1;

    int_t iT;
    byte_t bT;
    char_t szBuffer[ML_STRSIZE];
    memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szBuffer, (const char_t*)(m_pFavorite[jj].path));
    _tcscpy(m_pFavorite[jj].path, (const char_t*)(m_pFavorite[jj - 1].path));
    _tcscpy(m_pFavorite[jj - 1].path, (const char_t*)szBuffer);
    _tcscpy(szBuffer, (const char_t*)(m_pFavorite[jj].name));
    _tcscpy(m_pFavorite[jj].name, (const char_t*)(m_pFavorite[jj - 1].name));
    _tcscpy(m_pFavorite[jj - 1].name, (const char_t*)szBuffer);
    iT = m_pFavorite[jj].id;
    m_pFavorite[jj].id = m_pFavorite[jj - 1].id;
    m_pFavorite[jj - 1].id = iT;
    iT = m_pFavorite[jj].type;
    m_pFavorite[jj].type = m_pFavorite[jj - 1].type;
    m_pFavorite[jj - 1].type = iT;
    bT = m_pFavorite[jj].status;
    m_pFavorite[jj].status = m_pFavorite[jj - 1].status;
    m_pFavorite[jj - 1].status = bT;
    bT = m_pFavorite[jj].autorun;
    m_pFavorite[jj].autorun = m_pFavorite[jj - 1].autorun;
    m_pFavorite[jj - 1].autorun = bT;

    *m_piFavorite -= 1;

    UpdateFavoriteList();
    m_lstFavorite.SetCurSel(*m_piFavorite);

    m_strOutput = _T("Done.");
    UpdateData(FALSE);
}


void CFavoriteDlg::OnFavoriteDown()
{
    UpdateData(TRUE);

    if (*m_piFavoriteCount < 2) {
        return;
    }

    int_t idl = m_lstFavorite.GetCurSel();
    int_t jj = (int_t) (m_lstFavorite.GetItemData(idl));

    if ((jj < 1) || (jj >= *m_piFavoriteCount)) {
        m_strOutput = _T("Selected item not valid.");
        UpdateData(FALSE);
        return;
    }
    jj -= 1;

    int_t iT;
    byte_t bT;
    char_t szBuffer[ML_STRSIZE];
    memset(szBuffer, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szBuffer, (const char_t*)(m_pFavorite[jj].path));
    _tcscpy(m_pFavorite[jj].path, (const char_t*)(m_pFavorite[jj + 1].path));
    _tcscpy(m_pFavorite[jj + 1].path, (const char_t*)szBuffer);
    _tcscpy(szBuffer, (const char_t*)(m_pFavorite[jj].name));
    _tcscpy(m_pFavorite[jj].name, (const char_t*)(m_pFavorite[jj + 1].name));
    _tcscpy(m_pFavorite[jj + 1].name, (const char_t*)szBuffer);
    iT = m_pFavorite[jj].id;
    m_pFavorite[jj].id = m_pFavorite[jj + 1].id;
    m_pFavorite[jj + 1].id = iT;
    iT = m_pFavorite[jj].type;
    m_pFavorite[jj].type = m_pFavorite[jj + 1].type;
    m_pFavorite[jj + 1].type = iT;
    bT = m_pFavorite[jj].status;
    m_pFavorite[jj].status = m_pFavorite[jj + 1].status;
    m_pFavorite[jj + 1].status = bT;
    bT = m_pFavorite[jj].autorun;
    m_pFavorite[jj].autorun = m_pFavorite[jj + 1].autorun;
    m_pFavorite[jj + 1].autorun = bT;

    *m_piFavorite += 1;

    UpdateFavoriteList();
    m_lstFavorite.SetCurSel(*m_piFavorite);

    m_strOutput = _T("Done.");
    UpdateData(FALSE);
}

void CFavoriteDlg::OnFavoriteLoad()
{
    UpdateData(TRUE);

    if (*m_piFavoriteCount < 1) {
        return;
    }

    int_t idl = m_lstFavorite.GetCurSel();
    int_t jj = (int_t) (m_lstFavorite.GetItemData(idl));

    if ((jj < 1) || (jj > *m_piFavoriteCount)) {
        m_strOutput = _T("Selected item not valid.");
        UpdateData(FALSE);
        return;
    }
    jj -= 1;

    CScriptWindow *pwndScript = (CScriptWindow*)GetParent();
    if (pwndScript == NULL) {
        return;
    }

    pwndScript->ScriptFileOpen((LPCTSTR)(m_pFavorite[jj].path));

    m_strOutput = _T("Done.");
    UpdateData(FALSE);
}

void CFavoriteDlg::OnLbnSelchangeFavorite()
{
    int_t jj, idl;

    UpdateData(TRUE);

    idl = m_lstFavorite.GetCurSel();
    jj = (int_t) (m_lstFavorite.GetItemData(idl));

    *m_piFavorite = jj - 1;

    m_bnUp.EnableWindow((*m_piFavorite > 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnDown.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < (*m_piFavoriteCount - 1)));
    m_bnAdd.EnableWindow(*m_piFavoriteCount < SIGMA_SCRIPT_MAXFAV);
    m_bnRemove.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnLoad.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnAutoLoad.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));
    m_bnAutoRun.EnableWindow((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount));

    if ((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount)) {
        m_bAutoRun = ((m_pFavorite[*m_piFavorite].autorun & 0xF0) == 0x10);
        m_bAutoLoad = ((m_pFavorite[*m_piFavorite].autorun & 0x0F) == 0x01);
    }
    else {
        m_bAutoRun = FALSE;
        m_bAutoLoad = FALSE;
    }

    UpdateData(FALSE);
}

void CFavoriteDlg::OnOK()
{
    UpdateData(TRUE);

    if ((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount)) {
        m_pFavorite[*m_piFavorite].autorun &= 0x0F;
        if (m_bAutoRun) {
            m_pFavorite[*m_piFavorite].autorun |= 0x10;
        }
        m_pFavorite[*m_piFavorite].autorun &= 0xF0;
        if (m_bAutoLoad) {
            m_pFavorite[*m_piFavorite].autorun |= 0x01;
        }
    }

    CDialog::OnOK();
}

void CFavoriteDlg::OnClose()
{

    UpdateData(TRUE);

    if ((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount)) {
        m_pFavorite[*m_piFavorite].autorun &= 0x0F;
        if (m_bAutoRun) {
            m_pFavorite[*m_piFavorite].autorun |= 0x10;
        }
        m_pFavorite[*m_piFavorite].autorun &= 0xF0;
        if (m_bAutoLoad) {
            m_pFavorite[*m_piFavorite].autorun |= 0x01;
        }
    }

    CDialog::OnClose();
}

void CFavoriteDlg::OnBnClickedScriptAutorun()
{
    UpdateData(TRUE);

    if ((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount)) {
        m_pFavorite[*m_piFavorite].autorun &= 0x0F;
        if (m_bAutoRun) {
            m_pFavorite[*m_piFavorite].autorun |= 0x10;
        }
    }
}

void CFavoriteDlg::OnBnClickedScriptAutoload()
{
    UpdateData(TRUE);

    if ((*m_piFavorite >= 0) && (*m_piFavorite < *m_piFavoriteCount)) {
        m_pFavorite[*m_piFavorite].autorun &= 0xF0;
        if (m_bAutoLoad) {
            m_pFavorite[*m_piFavorite].autorun |= 0x01;
        }
    }
}
