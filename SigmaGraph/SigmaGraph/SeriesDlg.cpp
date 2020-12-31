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
#include "SeriesDlg.h"

#include "SigmaDoc.h"

// CSeriesDlg dialog

IMPLEMENT_DYNAMIC(CSeriesDlg, CDialog)

CSeriesDlg::CSeriesDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSeriesDlg::IDD, pParent)
    , m_bTo(FALSE)
{
    m_bOK = FALSE;
    m_pDoc = NULL;
    m_pColumn = NULL;
    m_fFrom = 0.0;
    m_bTo = FALSE;
    m_fTo = 1.0;
    m_fIncrement = 0.1;
    m_fMultiplier = 1.0;
}

CSeriesDlg::~CSeriesDlg()
{

}

void CSeriesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_SERIES_FROM, m_edtFrom);
    DDX_Control(pDX, IDE_SERIES_TO, m_edtTo);
    DDX_Control(pDX, IDE_SERIES_INCREMENT, m_edtIncrement);
    DDX_Control(pDX, IDE_SERIES_MULTIPLIER, m_edtMultiplier);
    DDX_Text(pDX, IDE_SERIES_FROM, m_fFrom);
    DDX_Check(pDX, IDK_SERIES_TO, m_bTo);
    DDX_Text(pDX, IDE_SERIES_TO, m_fTo);
    DDX_Text(pDX, IDE_SERIES_INCREMENT, m_fIncrement);
    DDX_Text(pDX, IDE_SERIES_MULTIPLIER, m_fMultiplier);
}


BEGIN_MESSAGE_MAP(CSeriesDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CSeriesDlg::OnOK)
    ON_BN_CLICKED(IDCANCEL, &CSeriesDlg::OnCancel)
END_MESSAGE_MAP()


// CSeriesDlg message handlers

void CSeriesDlg::OnOK()
{
    int_t nDim = 0, nPoints = 0, ii;

    CWaitCursor wcT;

    UpdateData(TRUE);

    if (m_bTo == TRUE) {
        if ((m_fFrom >= m_fTo) || (m_fIncrement <= 0.0)) {
            this->MessageBox(_T("Invalid From/To values."), _T("SigmaGraph"), MB_ICONERROR);
            return;
        }
        if (m_fIncrement >= (m_fTo - m_fFrom)) {
            this->MessageBox(_T("Invalid Increment value."), _T("SigmaGraph"), MB_ICONERROR);
            return;
        }
        nPoints = 1 + (int_t)ceil((m_fTo - m_fFrom) / m_fIncrement);
        if ((nPoints < 1) || (nPoints > ML_MAXPOINTS)) {
            this->MessageBox(_T("Invalid number of points."), _T("SigmaGraph"), MB_ICONERROR);
            return;
        }
    }

    nDim = m_pColumn->dim;
    if ((nDim < 1) || (nDim > ML_MAXPOINTS)) {
        this->MessageBox(_T("Invalid column."), _T("SigmaGraph"), MB_ICONERROR);
        CDialog::OnOK();
        return;
    }
    if (m_bTo == FALSE) {
        nPoints = nDim;
    }

    if (nPoints > nDim) {
        nPoints = nDim;
    }

    m_pColumn->dat[0] = m_fFrom;
    m_pColumn->mask[0] = 0x00;
    for (ii = 1; ii < nPoints; ii++) {
        m_pColumn->dat[ii] = (m_pColumn->dat[ii - 1] * m_fMultiplier) + m_fIncrement;
        m_pColumn->mask[ii] = 0x00;
    }

    m_bOK = TRUE;

    CDialog::OnOK();
}

void CSeriesDlg::OnCancel()
{
    CDialog::OnCancel();
}

BOOL CSeriesDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    if (m_pColumn == NULL) {
        SigmaApp.Output(_T("Cannot show the 'Create series' dialog: invalid column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
