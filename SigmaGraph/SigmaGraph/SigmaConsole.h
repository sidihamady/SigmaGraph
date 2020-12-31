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
#include "ConsoleEdit.h"

#include "NumPadDlg.h"
#include "VarlistDlg.h"

// CSigmaConsole dialog

class CSigmaConsole : public CDialog
{
    DECLARE_DYNAMIC(CSigmaConsole)

public:
    CSigmaConsole(CWnd* pParent = NULL);                        // standard constructor
    virtual ~CSigmaConsole();

    BOOL Create(UINT nID, CWnd *pWnd) { return CDialog::Create(nID, pWnd); }

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_CONSOLE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
    virtual void PostNcDestroy();
    virtual void OnOK();
    virtual void OnCancel();

    DECLARE_MESSAGE_MAP()

private:
    BOOL m_bTopmost;

public:
    CConsoleEdit m_edtConsole;
    CRect m_Rect;
    BOOL m_bInitialized;
    CFont m_Font;
    COLORREF m_Color;
    CNumPadDlg *m_pNumPadDlg;
    CVarlistDlg *m_pVarlistDlg;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void InsertText(const char_t *szText, int_t nRelPos = 1);

    afx_msg void OnConsoleSaveAs();
    afx_msg void OnConsoleOpen();
    afx_msg void OnConsoleCut();
    afx_msg void OnConsoleCopy();
    afx_msg void OnConsolePaste();
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg void OnConsoleFunc80();
    afx_msg void OnConsoleFunc81();
    afx_msg void OnConsoleFunc83();
    afx_msg void OnConsoleFunc84();
    afx_msg void OnConsoleFunc01();
    afx_msg void OnConsoleFunc02();
    afx_msg void OnConsoleFunc03();
    afx_msg void OnConsoleFunc04();
    afx_msg void OnConsoleFunc05();
    afx_msg void OnConsoleFunc06();
    afx_msg void OnConsoleFunc10();
    afx_msg void OnConsoleFunc11();
    afx_msg void OnConsoleFunc20();
    afx_msg void OnConsoleFunc30();
    afx_msg void OnConsoleConst01();
    afx_msg void OnConsoleConst10();
    afx_msg void OnConsoleConst11();
    afx_msg void OnConsoleConst12();
    afx_msg void OnConsoleConst13();
    afx_msg void OnConsoleConst14();
    afx_msg void OnConsoleConst15();
    afx_msg void OnConsoleConst16();
    afx_msg void OnConsoleRad();
    afx_msg void OnConsoleDeg();
    afx_msg void OnConsoleGrad();
    afx_msg void OnConsoleTopmost();
    afx_msg void OnUpdateConsoleRad(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConsoleDeg(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConsoleGrad(CCmdUI *pCmdUI);
    afx_msg void OnUpdateConsoleTopmost(CCmdUI* pCmdUI);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnConsoleReset();
    afx_msg void OnConsoleFont();
    afx_msg void OnConsoleFontColor();
    afx_msg void OnConsoleClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnViewInput();
    afx_msg void OnViewVarlist();
    afx_msg void OnHelpContents();
    afx_msg void OnClose();

    afx_msg void OnDestroy();
};
