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

#include "StdAfx.h"
#include "Sigma.h"

#include "Plotopt.h"

CPlotopt::~CPlotopt(void)
{

}
BEGIN_MESSAGE_MAP(CPlotopt, CPropertySheet)
    ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


BOOL CPlotopt::OnInitDialog()
{
    m_Curve.m_bTemplate = m_bTemplate;
    m_Axis.m_bTemplate = m_bTemplate;
    m_Scale.m_bTemplate = m_bTemplate;
    m_Line.m_bTemplate = m_bTemplate;
    m_General.m_bTemplate = m_bTemplate;

    BOOL bResult = CPropertySheet::OnInitDialog();

    HICON hIcon =     (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDR_DOCTYPE_PLOT), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    return bResult;
}

void CPlotopt::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SC_CLOSE) {
        if ((m_Curve.m_bIsModified) || (m_General.m_bIsModified)
            || (m_Axis.m_bIsModified) || (m_Line.m_bIsModified) || (m_Scale.m_bIsModified)) {

            if (this->MessageBox(_T("Apply change ?"), _T("SigmaGraph"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    m_Curve.m_bClosing = TRUE;
                    m_General.m_bClosing = TRUE;
                    m_Axis.m_bClosing = TRUE;
                    m_Line.m_bClosing = TRUE;
                    m_Scale.m_bClosing = TRUE;
                    m_Curve.OnApply();
                    m_General.OnApply();
                    m_Axis.OnApply();
                    m_Line.OnApply();
                    m_Scale.OnApply();
                    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
                }
        }
    }

    CPropertySheet::OnSysCommand(nID, lParam);
}
