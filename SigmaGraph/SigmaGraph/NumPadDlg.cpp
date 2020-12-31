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
#include "NumPadDlg.h"


// CNumPadDlg dialog

IMPLEMENT_DYNAMIC(CNumPadDlg, CDialog)

CNumPadDlg::CNumPadDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNumPadDlg::IDD, pParent)
{
    m_pConsoleEdit = NULL;
}

CNumPadDlg::~CNumPadDlg()
{

}

void CNumPadDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNumPadDlg, CDialog)
    ON_COMMAND(ID_INPUT_ZERO, &CNumPadDlg::OnInputZero)
    ON_COMMAND(ID_INPUT_ONE, &CNumPadDlg::OnInputOne)
    ON_COMMAND(ID_INPUT_TWO, &CNumPadDlg::OnInputTwo)
    ON_COMMAND(ID_INPUT_THREE, &CNumPadDlg::OnInputThree)
    ON_COMMAND(ID_INPUT_FOUR, &CNumPadDlg::OnInputFour)
    ON_COMMAND(ID_INPUT_FIVE, &CNumPadDlg::OnInputFive)
    ON_COMMAND(ID_INPUT_SIX, &CNumPadDlg::OnInputSix)
    ON_COMMAND(ID_INPUT_SEVEN, &CNumPadDlg::OnInputSeven)
    ON_COMMAND(ID_INPUT_EIGHT, &CNumPadDlg::OnInputEight)
    ON_COMMAND(ID_INPUT_NINE, &CNumPadDlg::OnInputNine)
    ON_COMMAND(ID_INPUT_EXP, &CNumPadDlg::OnInputExp)
    ON_COMMAND(ID_INPUT_DOT, &CNumPadDlg::OnInputDot)
    ON_COMMAND(ID_INPUT_PLUS, &CNumPadDlg::OnInputPlus)
    ON_COMMAND(ID_INPUT_MINUS, &CNumPadDlg::OnInputMinus)
    ON_COMMAND(ID_INPUT_MULT, &CNumPadDlg::OnInputMult)
    ON_COMMAND(ID_INPUT_DIV, &CNumPadDlg::OnInputDiv)
    ON_COMMAND(ID_INPUT_PARENTH1, &CNumPadDlg::OnInputParenth1)
    ON_COMMAND(ID_INPUT_PARENTH2, &CNumPadDlg::OnInputParenth2)
    ON_COMMAND(ID_INPUT_BACK, &CNumPadDlg::OnInputBack)
    ON_COMMAND(ID_INPUT_EQUAL, &CNumPadDlg::OnInputEqual)
    ON_WM_SYSCOMMAND()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// CNumPadDlg message handlers
BOOL CNumPadDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if (m_pConsoleEdit == NULL) {
        CDialog::EndDialog(-1);
        return FALSE;
    }

    UpdateData(FALSE);

    return TRUE;
}

void CNumPadDlg::OnClose()
{
    ShowWindow(SW_HIDE);
}

void CNumPadDlg::PostNcDestroy()
{
    CDialog::PostNcDestroy();
}

void CNumPadDlg::OnOK()
{
}

void CNumPadDlg::OnCancel()
{
    OnClose();
}

inline void CNumPadDlg::InsertText(const char_t *szText, int_t nRelPos/* = 0*/)
{
    if (!(m_pConsoleEdit->IsEditingLine()) || m_pConsoleEdit->IsPromptArea(0)) {
        return;
    }
    int_t nLine = 0, nPos = 0;
    m_pConsoleEdit->ReplaceSel(szText);
    if (nRelPos != 0) {
        m_pConsoleEdit->GetCaretPos(nLine, nPos);
        m_pConsoleEdit->SetCaretPos(nPos - nRelPos);
    }
}

void CNumPadDlg::OnInputZero()
{
    InsertText(_T("0"));
}

void CNumPadDlg::OnInputOne()
{
    InsertText(_T("1"));
}

void CNumPadDlg::OnInputTwo()
{
    InsertText(_T("2"));
}

void CNumPadDlg::OnInputThree()
{
    InsertText(_T("3"));
}

void CNumPadDlg::OnInputFour()
{
    InsertText(_T("4"));
}

void CNumPadDlg::OnInputFive()
{
    InsertText(_T("5"));
}

void CNumPadDlg::OnInputSix()
{
    InsertText(_T("6"));
}

void CNumPadDlg::OnInputSeven()
{
    InsertText(_T("7"));
}

void CNumPadDlg::OnInputEight()
{
    InsertText(_T("8"));
}

void CNumPadDlg::OnInputNine()
{
    InsertText(_T("9"));
}

void CNumPadDlg::OnInputExp()
{
    InsertText(_T("e"));
}

void CNumPadDlg::OnInputDot()
{
    InsertText(_T("."));
}

void CNumPadDlg::OnInputPlus()
{
    InsertText(_T("+"));
}

void CNumPadDlg::OnInputMinus()
{
    InsertText(_T("-"));
}

void CNumPadDlg::OnInputMult()
{
    InsertText(_T("*"));
}

void CNumPadDlg::OnInputDiv()
{
    InsertText(_T("/"));
}

void CNumPadDlg::OnInputParenth1()
{
    InsertText(_T("("));
}

void CNumPadDlg::OnInputParenth2()
{
    InsertText(_T(")"));
}

void CNumPadDlg::OnInputBack()
{
    m_pConsoleEdit->Back();
}

void CNumPadDlg::OnInputEqual()
{
    m_pConsoleEdit->Return();
}
