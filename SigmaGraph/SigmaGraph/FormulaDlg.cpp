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

#include "Datasheet.h"
#include "Sigma.h"
#include "FormulaDlg.h"

// CFormulaDlg dialog

IMPLEMENT_DYNAMIC(CFormulaDlg, CDialog)

CFormulaDlg::CFormulaDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFormulaDlg::IDD, pParent)
    , m_strLabel(_T(""))
{
    m_bInitialized = FALSE;
    for (int_t ii = 0; ii < SIGMA_MAXCOLUMNS; ii++) {
        m_pColumn[ii] = NULL;
    }
    m_nColumnCount = 0;
    m_nSelectedColumn = 0;
    m_pbRunning = NULL;
    m_nToRow = 0;
    m_nFromRow = 0;
    m_bOK = FALSE;
    m_bIsModified = FALSE;
}

CFormulaDlg::~CFormulaDlg()
{

}

void CFormulaDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDL_FORMULA, m_strLabel);
    DDX_Control(pDX, IDE_FORMULA, m_edtFormula);
    DDX_Text(pDX, IDE_ROW_FROM, m_nFromRow);
    DDX_Text(pDX, IDE_ROW_TO, m_nToRow);
    DDX_Control(pDX, IDAPPLY, m_bnApply);
}


BEGIN_MESSAGE_MAP(CFormulaDlg, CDialog)
    ON_COMMAND(ID_PARSER_FUNC80, &CFormulaDlg::OnFormulaFunc80)
    ON_COMMAND(ID_PARSER_FUNC81, &CFormulaDlg::OnFormulaFunc81)
    ON_COMMAND(ID_PARSER_FUNC83, &CFormulaDlg::OnFormulaFunc83)
    ON_COMMAND(ID_PARSER_FUNC84, &CFormulaDlg::OnFormulaFunc84)
    ON_COMMAND(ID_PARSER_FUNC01, &CFormulaDlg::OnFormulaFunc01)
    ON_COMMAND(ID_PARSER_FUNC02, &CFormulaDlg::OnFormulaFunc02)
    ON_COMMAND(ID_PARSER_FUNC03, &CFormulaDlg::OnFormulaFunc03)
    ON_COMMAND(ID_PARSER_FUNC04, &CFormulaDlg::OnFormulaFunc04)
    ON_COMMAND(ID_PARSER_FUNC05, &CFormulaDlg::OnFormulaFunc05)
    ON_COMMAND(ID_PARSER_FUNC06, &CFormulaDlg::OnFormulaFunc06)
    ON_COMMAND(ID_PARSER_FUNC10, &CFormulaDlg::OnFormulaFunc10)
    ON_COMMAND(ID_PARSER_FUNC20, &CFormulaDlg::OnFormulaFunc20)
    ON_COMMAND(ID_PARSER_FUNC30, &CFormulaDlg::OnFormulaFunc30)
    ON_COMMAND(ID_PARSER_CONST01, &CFormulaDlg::OnFormulaConst01)
    ON_COMMAND(ID_PARSER_CONST10, &CFormulaDlg::OnFormulaConst10)
    ON_COMMAND(ID_PARSER_CONST11, &CFormulaDlg::OnFormulaConst11)
    ON_COMMAND(ID_PARSER_CONST12, &CFormulaDlg::OnFormulaConst12)
    ON_COMMAND(ID_PARSER_CONST13, &CFormulaDlg::OnFormulaConst13)
    ON_COMMAND(ID_PARSER_CONST14, &CFormulaDlg::OnFormulaConst14)
    ON_COMMAND(ID_PARSER_CONST15, &CFormulaDlg::OnFormulaConst15)
    ON_COMMAND(ID_PARSER_CONST16, &CFormulaDlg::OnFormulaConst16)
    ON_COMMAND(ID_PARSER_RAD, &CFormulaDlg::OnFormulaRad)
    ON_COMMAND(ID_PARSER_DEG, &CFormulaDlg::OnFormulaDeg)
    ON_COMMAND(ID_PARSER_GRAD, &CFormulaDlg::OnFormulaGrad)
    ON_BN_CLICKED(IDAPPLY, &CFormulaDlg::OnApply)
    ON_EN_CHANGE(IDE_FORMULA, &CFormulaDlg::OnEnChangeFormula)
    ON_EN_CHANGE(IDE_ROW_FROM, &CFormulaDlg::OnEnChangeRowFrom)
    ON_EN_CHANGE(IDE_ROW_TO, &CFormulaDlg::OnEnChangeRowTo)
    ON_WM_SYSCOMMAND()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// CFormulaDlg message handlers
BOOL CFormulaDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_FORMULA), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    // Vérification des données
    if ((m_nColumnCount < 1) || (m_nColumnCount > SIGMA_MAXCOLUMNS)
        || (m_nSelectedColumn < 0) || (m_nSelectedColumn >= SIGMA_MAXCOLUMNS)
        || (m_nSelectedColumn >= m_nColumnCount) || (m_pbRunning == NULL)) {
        SigmaApp.Output(_T("Cannot show the 'Set Column Values' dialog: invalid column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }
    for (int_t ii = 0; ii < m_nColumnCount; ii++) {
        if (m_pColumn[ii] == NULL) {
            SigmaApp.Output(_T("Cannot show the 'Set Column Values' dialog: invalid column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
        if ((m_pColumn[ii]->dim < 1) || (m_pColumn[ii]->dim > ML_MAXPOINTS)) {
            SigmaApp.Output(_T("Cannot show the 'Set Column Values' dialog: invalid column dimension.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
        // Toutes les colonnes doivent avoir la m�me dimension.
        if (m_pColumn[ii]->dim != m_pColumn[0]->dim) {
            SigmaApp.Output(_T("Cannot show the 'Set Column Values' dialog: invalid column dimension.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            CDialog::EndDialog(-1);
            return FALSE;
        }
    }

    m_edtFormula.SetLimitText(ML_STRSIZE - 1);
    m_edtFormula.SetWindowText((LPCTSTR)(m_pColumn[m_nSelectedColumn]->formula));
    m_edtFormula.SetSel(0,-1);
    m_strLabel = m_pColumn[m_nSelectedColumn]->name;
    m_strLabel += _T(" = ");
    m_nFromRow = 0;
    m_nToRow = m_pColumn[m_nSelectedColumn]->dim - 1;

    CMenu* pMenu = GetMenu();
    if (pMenu) {
        if (ml_angleunit == 0x00) {
            pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_CHECKED);
            pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
        }
        else if (ml_angleunit == 0x01) {
            pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_CHECKED);
            pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
        }
        else if (ml_angleunit == 0x02) {
            pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
            pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_CHECKED);
        }
    }

    m_bInitialized = TRUE;

    UpdateData(FALSE);

    return TRUE;
}

void CFormulaDlg::OnOK()
{
    if (*m_pbRunning == TRUE) {
        this->MessageBox(_T("Calculation in progress. Please wait."), _T("SigmaGraph"), MB_ICONERROR);
        return;
    }
    OnApply();
    if (m_bOK == TRUE) {
        CDialog::OnOK();
    }
}

void CFormulaDlg::OnCancel()
{
    if (*m_pbRunning == TRUE) {
        this->MessageBox(_T("Calculation in progress. Please wait."), _T("SigmaGraph"), MB_ICONERROR);
        return;
    }
    CDialog::OnCancel();
}

inline void CFormulaDlg::AppendText(LPCTSTR lpszText)
{
    int_t len = m_edtFormula.GetWindowTextLength();
    m_edtFormula.SetSel(len,len);
    m_edtFormula.ReplaceSel(lpszText ? lpszText : _T(""));
    m_edtFormula.SetSel(-1, -1);
}

inline void CFormulaDlg::SetEditCaretPos(int_t pos)
{
    AppendText();
    if (pos >= 0) {
        int_t li = m_edtFormula.LineIndex() + pos;
        m_edtFormula.SetSel(li, li);
        m_edtFormula.ReplaceSel(_T(""));
    }
    return;
}

inline void CFormulaDlg::GetEditCaretPos(int_t& nLine, int_t& nPos)
{
    int_t nStart, nEnd;
    m_edtFormula.GetSel(nStart, nEnd);
    nLine = m_edtFormula.LineFromChar(nEnd);
    nPos = nEnd - m_edtFormula.LineIndex(nLine);
}

inline void CFormulaDlg::InsertText(const char_t *szText, int_t nRelPos)
{
    int_t nLine = 0, nPos = 0;
    m_edtFormula.ReplaceSel(szText);
    if (nRelPos != 0) {
        GetEditCaretPos(nLine, nPos);
        SetEditCaretPos(nPos - nRelPos);
    }
}

void CFormulaDlg::OnFormulaFunc80()
{
    InsertText(_T("sqrt()"));
}

void CFormulaDlg::OnFormulaFunc81()
{
    InsertText(_T("abs()"));
}

void CFormulaDlg::OnFormulaFunc83()
{
    InsertText(_T("ceil()"));
}

void CFormulaDlg::OnFormulaFunc84()
{
    InsertText(_T("int()"));
}

void CFormulaDlg::OnFormulaFunc01()
{
    InsertText(_T("sin()"));
}

void CFormulaDlg::OnFormulaFunc02()
{
    InsertText(_T("cos()"));
}

void CFormulaDlg::OnFormulaFunc03()
{
    InsertText(_T("tan()"));
}

void CFormulaDlg::OnFormulaFunc04()
{
    InsertText(_T("asin()"));
}

void CFormulaDlg::OnFormulaFunc05()
{
    InsertText(_T("acos()"));
}

void CFormulaDlg::OnFormulaFunc06()
{
    InsertText(_T("atan()"));
}

void CFormulaDlg::OnFormulaFunc10()
{
    InsertText(_T("ln()"));
}

void CFormulaDlg::OnFormulaFunc20()
{
    InsertText(_T("exp()"));
}

void CFormulaDlg::OnFormulaFunc30()
{
    InsertText(_T("pow(,)"), 2);
}

void CFormulaDlg::OnFormulaConst01()
{
    InsertText(_T("pi"), 0);
}

void CFormulaDlg::OnFormulaConst10()
{
    InsertText(_T("_q"), 0);
}

void CFormulaDlg::OnFormulaConst11()
{
    InsertText(_T("_m"), 0);
}

void CFormulaDlg::OnFormulaConst12()
{
    InsertText(_T("_k"), 0);
}

void CFormulaDlg::OnFormulaConst13()
{
    InsertText(_T("_h"), 0);
}

void CFormulaDlg::OnFormulaConst14()
{
    InsertText(_T("_c"), 0);
}

void CFormulaDlg::OnFormulaConst15()
{
    InsertText(_T("_e"), 0);
}

void CFormulaDlg::OnFormulaConst16()
{
    InsertText(_T("_n"), 0);
}

void CFormulaDlg::OnFormulaRad()
{
ml_angleunit = 0x00;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
}

void CFormulaDlg::OnFormulaDeg()
{
ml_angleunit = 0x01;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_CHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_UNCHECKED);
    }
}

void CFormulaDlg::OnFormulaGrad()
{
ml_angleunit = 0x02;
    CMenu* pMenu = GetMenu();
    if (pMenu) {
        pMenu->CheckMenuItem(ID_PARSER_RAD, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_DEG, MF_BYCOMMAND | MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_PARSER_GRAD, MF_BYCOMMAND | MF_CHECKED);
    }
}

void CFormulaDlg::OnApply()
{
    if (m_bIsModified == FALSE) {
        m_bOK = TRUE;
        return;
    }

    if (*m_pbRunning == TRUE) {
        this->MessageBox(_T("Another operation is in progress.\nPlease wait before starting new operation."), _T("SigmaGraph"), MB_ICONERROR);
        m_bOK = FALSE;
        return;
    }

    CWaitCursor wcT;

    UpdateData(TRUE);

    char_t szFormula[ML_STRSIZE];
    memset(szFormula, 0, ML_STRSIZE * sizeof(char_t));
    m_edtFormula.GetWindowText((LPTSTR)szFormula, ML_STRSIZE - 1);
    int_t nLength = (int_t)_tcslen((const char_t*)szFormula);
    if (nLength < 1) {
        m_bOK = FALSE;
        return;
    }
    if (nLength >= ML_STRSIZE) {
        this->MessageBox(_T("Invalid expression."), _T("SigmaGraph"), MB_ICONERROR);
        m_bOK = FALSE;
        return;
    }

    int_t ii, jj, nn = m_pColumn[m_nSelectedColumn]->dim;
    real_t fval = 0.0;
    char_t szOutput[ML_STRSIZE], szT[ML_STRSIZE];

    if ((m_nFromRow < 0) || (m_nFromRow >= nn)
        || (m_nToRow < 0) || (m_nToRow >= nn)
        || (m_nToRow <= m_nFromRow)) {
        this->MessageBox(_T("Invalid row indices."), _T("SigmaGraph"), MB_ICONERROR);
        m_bOK = FALSE;
        return;
    }

    m_bOK = TRUE;

    // Simple affectation
    if (nLength == 1) {
        char_t cT = _totlower(szFormula[0]);
        jj = (int_t) (cT - _T('a'));
        if ((jj >= 0) && (jj < m_nColumnCount)) {
            for (ii = m_nFromRow; ii <= m_nToRow; ii++) {
                m_pColumn[m_nSelectedColumn]->dat[ii] = m_pColumn[jj]->dat[ii];
                m_pColumn[m_nSelectedColumn]->mask[ii] = m_pColumn[jj]->mask[ii];
            }
        }
        else if (_istdigit(cT)) {
            fval = (real_t)(cT - _T('0'));
            for (ii = m_nFromRow; ii <= m_nToRow; ii++) {
                m_pColumn[m_nSelectedColumn]->dat[ii] = fval;
                m_pColumn[m_nSelectedColumn]->mask[ii] = 0x00;
            }
        }
        else {
            this->MessageBox(_T("Invalid expression."), _T("SigmaGraph"), MB_ICONERROR);
            m_bOK = FALSE;
        }
        if (m_bOK == TRUE) {
            CDatasheet *pDatasheet = (CDatasheet*)GetOwner();
            if (pDatasheet != NULL) {
                CDocument *pDoc = pDatasheet->GetDocument();
                if (pDoc != NULL) {
                    pDoc->UpdateAllViews(NULL);
                }
            }
        }
        if (nLength < ML_STRSIZEN) {    // Column formula size limited to 64 characters
            m_edtFormula.GetWindowText((LPTSTR)(m_pColumn[m_nSelectedColumn]->formula), ML_STRSIZEN - 1);
        }    
        m_bIsModified = FALSE;
        m_bnApply.EnableWindow(m_bIsModified);
        return;
    }
    //

    *m_pbRunning = TRUE;

    memset(szT, 0, ML_STRSIZE * sizeof(char_t));

    // Sauvergarder les variables du parser avant de lancer le calcul
    char_t szParam[ML_PARSER_VARNUM][ML_STRSIZET];    // Params name : by default : a, b, c, ..., z
    real_t fParam[ML_PARSER_VARNUM];
    byte_t bParam[ML_PARSER_VARNUM];
    memset(szParam, 0, (ML_PARSER_VARNUM * ML_STRSIZET) * sizeof(char_t));
    for (jj = 0; jj < ML_PARSER_VARNUM; jj++) {
        bParam[jj] = SigmaApp.m_bParam[jj];
        fParam[jj] = SigmaApp.m_fParam[jj];
        _tcscpy(szParam[jj], (const char_t*)(SigmaApp.m_szParam[jj]));
        SigmaApp.m_bParam[jj] = FALSE;
        SigmaApp.m_fParam[jj] = 0.0;
        memset(SigmaApp.m_szParam[jj], 0, ML_STRSIZET * sizeof(char_t));
    }
    SigmaApp.m_nVarnum = 0;
    //

    BOOL bEnd = FALSE;
    byte_t bMask = 0x00;
    byte_t bStatus = 0x00;
    char_t* pszT = NULL;

    for (ii = m_nFromRow; ii <= m_nToRow; ii++) {

        // D�finir les valeurs de a, b, c,... avant d'�valuer la colonne
        for (jj = 0; jj < m_nColumnCount; jj++) {

            // :FIX:20160425: s'arr�ter si l'une des colonnes utilis�es a des données non valid�es
            pszT = _tcschr((char_t*) szFormula, _T('a') + jj);
            if (pszT == NULL) {
                pszT = _tcschr((char_t*) szFormula, _T('A') + jj);
            }
            if (pszT != NULL) {
                if (pszT == ((char_t*) szFormula)) {
                    if (!isalnum(*(pszT + 1))) {
                        bMask = m_pColumn[jj]->mask[ii];
                        bEnd = (bMask == 0xFF) ? TRUE : FALSE;
                        bStatus = m_pColumn[jj]->status;
                    }
                }
                else {
                    if (!isalnum(*(pszT - 1)) && !isalnum(*(pszT + 1))) {
                        bMask = m_pColumn[jj]->mask[ii];
                        bEnd = (bMask == 0xFF) ? TRUE : FALSE;
                        bStatus = m_pColumn[jj]->status;
                    }
                }
            }
            if (bEnd == TRUE) {
                break;
            }
            //

            _tcsprintf(szT, ML_STRSIZE - 1, _T("%c=%e"), _T('a') + jj, m_pColumn[jj]->dat[ii]);
            m_bOK = SigmaApp.DoParse((const char_t*)szT, szOutput);
            if (m_bOK == FALSE) {
                if (jj != m_nSelectedColumn) {
                    CString strT = _T("Cannot evaluate expression: ");
                    strT += szOutput;
                    strT += _T("\r\n");
                    SigmaApp.Output(strT, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
                    break;
                }
                else {
                    m_bOK = TRUE;
                }
            }
        }

        if ((m_bOK == FALSE) || (bEnd == TRUE)) {
            break;
        }

        m_bOK = SigmaApp.DoParse((const char_t*)szFormula, szOutput, &fval);
        if (m_bOK == FALSE) {
            CString strT = _T("Cannot evaluate expression: ");
            strT += szOutput;
            strT += _T("\r\n");
            SigmaApp.Output(strT, TRUE, TRUE, SIGMA_OUTPUT_ERROR);
            break;
        }
        m_pColumn[m_nSelectedColumn]->dat[ii] = fval;
        m_pColumn[m_nSelectedColumn]->mask[ii] = bMask;
        m_pColumn[m_nSelectedColumn]->status = bStatus;
    }

    // Restaurer les variables du parser
    SigmaApp.m_nVarnum = 0;
    for (jj = 0; jj < ML_PARSER_VARNUM; jj++) {
        SigmaApp.m_bParam[jj] = bParam[jj];
        SigmaApp.m_fParam[jj] = fParam[jj];
        _tcscpy(SigmaApp.m_szParam[jj], (const char_t*)(szParam[jj]));
        if (SigmaApp.m_bParam[jj]) {
            SigmaApp.m_nVarnum += 1;
        }
    }
    //

    *m_pbRunning = FALSE;

    CDocument *pDoc = NULL;
    CDatasheet *pDatasheet = (CDatasheet*)GetOwner();
    if (pDatasheet != NULL) {
        pDoc = pDatasheet->GetDocument();
    }

    if (m_bOK == TRUE) {
        if (pDoc != NULL) {
            pDoc->UpdateAllViews(NULL);
        }
    }

    if (nLength < ML_STRSIZEN) {            // Column formula size limited to 64 characters
        m_edtFormula.GetWindowText((LPTSTR)(m_pColumn[m_nSelectedColumn]->formula), ML_STRSIZEN - 1);
    }
    else if (nLength < ML_STRSIZE) {
        if (pDoc != NULL) {
            ((CSigmaDoc*)pDoc)->AddToNote((const char_t*)szFormula, nLength);
        }
    }

    m_bIsModified = FALSE;
    m_bnApply.EnableWindow(m_bIsModified);
}

void CFormulaDlg::OnClose()
{
    if (*m_pbRunning == TRUE) {
        this->MessageBox(_T("Calculation in progress. Please wait."), _T("SigmaGraph"), MB_ICONERROR);
        return;
    }

    CDialog::OnClose();
}

void CFormulaDlg::OnEnChangeFormula()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    m_bIsModified = TRUE;
    m_bnApply.EnableWindow(m_bIsModified);
}

void CFormulaDlg::OnEnChangeRowFrom()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    m_bIsModified = TRUE;
    m_bnApply.EnableWindow(m_bIsModified);
}

void CFormulaDlg::OnEnChangeRowTo()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    m_bIsModified = TRUE;
    m_bnApply.EnableWindow(m_bIsModified);
}
