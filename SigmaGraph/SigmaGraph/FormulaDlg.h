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

#pragma once
#include "afxwin.h"

#include "SigmaDefine.h"

// CFormulaDlg dialog

class CFormulaDlg : public CDialog
{
    DECLARE_DYNAMIC(CFormulaDlg)

public:
    CFormulaDlg(CWnd* pParent = NULL);                        // standard constructor
    virtual ~CFormulaDlg();

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_FORMULA };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    int_t m_nFromRow;
    int_t m_nToRow;
    CString m_strLabel;
    CEdit m_edtFormula;
    BOOL m_bInitialized;
    vector_t *m_pColumn[SIGMA_MAXCOLUMNS];
    int_t m_nColumnCount;
    int_t m_nSelectedColumn;
    BOOL *m_pbRunning;
    BOOL m_bOK;
    BOOL m_bIsModified;
    CButton m_bnApply;

    void AppendText(LPCTSTR lpszText = NULL);
    void SetEditCaretPos(int_t pos = -1);
    void GetEditCaretPos(int_t& nLine, int_t& nPos);

    void InsertText(const char_t *szText, int_t nRelPos = 1);

    afx_msg void OnFormulaFunc80();
    afx_msg void OnFormulaFunc81();
    afx_msg void OnFormulaFunc83();
    afx_msg void OnFormulaFunc84();
    afx_msg void OnFormulaFunc01();
    afx_msg void OnFormulaFunc02();
    afx_msg void OnFormulaFunc03();
    afx_msg void OnFormulaFunc04();
    afx_msg void OnFormulaFunc05();
    afx_msg void OnFormulaFunc06();
    afx_msg void OnFormulaFunc10();
    afx_msg void OnFormulaFunc11();
    afx_msg void OnFormulaFunc20();
    afx_msg void OnFormulaFunc30();
    afx_msg void OnFormulaConst01();
    afx_msg void OnFormulaConst10();
    afx_msg void OnFormulaConst11();
    afx_msg void OnFormulaConst12();
    afx_msg void OnFormulaConst13();
    afx_msg void OnFormulaConst14();
    afx_msg void OnFormulaConst15();
    afx_msg void OnFormulaConst16();
    afx_msg void OnFormulaRad();
    afx_msg void OnFormulaDeg();
    afx_msg void OnFormulaGrad();
    afx_msg void OnUpdateFormulaRad(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFormulaDeg(CCmdUI *pCmdUI);
    afx_msg void OnUpdateFormulaGrad(CCmdUI *pCmdUI);
    afx_msg void OnApply();
    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnEnChangeFormula();
    afx_msg void OnEnChangeRowFrom();
    afx_msg void OnEnChangeRowTo();
};
