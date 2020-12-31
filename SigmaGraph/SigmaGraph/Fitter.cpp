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
#include "Fitter.h"

#include "MainFrm.h"

#include "SigmaDoc.h"

// CFitter

IMPLEMENT_DYNAMIC(CFitter, CPropertySheet)

CFitter::~CFitter()
{

}

BEGIN_MESSAGE_MAP(CFitter, CPropertySheet)
END_MESSAGE_MAP()


// CFitter message handlers

BOOL CFitter::OnInitDialog()
{
    BOOL bResult = CPropertySheet::OnInitDialog();

    if ((m_Model.m_pDoc == NULL) || (m_Model.m_pPlot == NULL)
        || (m_Model.m_pColumnDataX == NULL)
        || (m_Model.m_pColumnDataY == NULL)
        || (m_Model.m_pbIsModified == NULL)
        || (m_Model.m_pbRunning == NULL)) {
        CPropertySheet::EndDialog(-1);
        return FALSE;
    }

    if ((m_Model.m_pColumnDataX->dat == NULL) || (m_Model.m_pColumnDataY->dat == NULL)
        || (m_Model.m_pColumnDataX->dim < 2) || (m_Model.m_pColumnDataY->dim < 2)
        || (m_Model.m_pColumnDataX->dim > ML_MAXPOINTS) || (m_Model.m_pColumnDataY->dim > ML_MAXPOINTS)
        || (m_Model.m_pColumnDataY->dim != m_Model.m_pColumnDataX->dim)) {
        CPropertySheet::EndDialog(-1);
        return FALSE;
    }

    HICON hIcon = (HICON)::LoadImage(SigmaApp.m_hInstance, MAKEINTRESOURCE(IDI_FITTER), IMAGE_ICON, 16, 16, 0);
    SetIcon(hIcon, TRUE);    SetIcon(hIcon, FALSE);

    RECT rcP = { 0, 0, 0, 0 };
    CView *pView = ((CSigmaDoc*) (m_Model.m_pDoc))->GetPlotView();
    pView->GetWindowRect(&rcP);
    RECT rcM = { 0, 0, 0, 0 };
    SigmaApp.GetMainWnd()->GetWindowRect(&rcM);
    RECT rcF = { 0, 0, 0, 0 };
    this->GetWindowRect(&rcF);
    if (((rcP.right + (rcF.right - rcF.left)) <= rcM.right) && ((rcP.top + (rcF.bottom - rcF.top)) <= rcM.bottom)) {
        SetWindowPos(NULL, rcP.right + 8, rcP.top - 20, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
    else {
        SetWindowPos(NULL, rcM.right - 8 - (rcF.right - rcF.left), rcM.bottom - 28 - (rcF.bottom - rcF.top), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    return bResult;
}
