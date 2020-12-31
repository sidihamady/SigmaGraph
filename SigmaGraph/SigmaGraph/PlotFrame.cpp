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
#include "PlotFrame.h"

#include "MainFrm.h"

#include "SigmaDC.h"


// CPlotFrame

IMPLEMENT_DYNAMIC(CPlotFrame, CStatic)

CPlotFrame::CPlotFrame()
{
    m_bInitialized = FALSE;
    m_bDoubleBuffer = TRUE;            // Use double buffering to avoid flicker?
}

CPlotFrame::~CPlotFrame()
{

}

BEGIN_MESSAGE_MAP(CPlotFrame, CStatic)

END_MESSAGE_MAP()


void CPlotFrame::PreSubclassWindow()
{
    CStatic::PreSubclassWindow();

    ModifyStyle(0, SS_OWNERDRAW);
}

void CPlotFrame::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (m_pPlot == NULL) {
        return;        
    }

    CSigmaDC sigmaDC;

    CDC dc;
    if (!dc.Attach(lpDrawItemStruct->hDC)) {
        return;
    }


    if (m_bDoubleBuffer) {
        sigmaDC.SetDC(&dc);
        m_pPlot->dc = sigmaDC.GetSafeHdc();
    }
    else {
        m_pPlot->dc = dc.GetSafeHdc();
    }

    if (m_pPlot->dc == NULL) {
        dc.Detach();
        return;
    }

    m_pPlot->frame = GetSafeHwnd();


    //--------------------------------------------------------------------------------
    m_pPlot->errcode = 0;
    pl_validate(m_pPlot, 0);
    if (m_pPlot->errcode != 0) {    // Error
        dc.Detach();
        m_pPlot->dc = NULL;
        return;
    }
    //---------------------------------------------------------------------------------

    pl_plot(m_pPlot);
    if (m_pPlot->errcode != 0) {    // Error
        sigmaDC.DrawIt();
        dc.Detach();
        m_pPlot->dc = NULL;
        return;
    }

    sigmaDC.DrawIt();

    dc.Detach();
    m_pPlot->dc = NULL;
}
