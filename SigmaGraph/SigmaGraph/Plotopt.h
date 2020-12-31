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
#include "afxdlgs.h"

#include "PlotoptCurve.h"
#include "PlotoptAxis.h"
#include "PlotoptLine.h"
#include "PlotoptScale.h"
#include "PlotoptGeneral.h"

class CPlotopt : public CPropertySheet
{
public:
    plot_t* m_pPlot;

    BOOL m_bTemplate;

    CPlotoptCurve m_Curve;
    CPlotoptAxis m_Axis;
    CPlotoptLine m_Line;
    CPlotoptScale m_Scale;
    CPlotoptGeneral m_General;

    CPlotopt(CWnd* pParentWnd = NULL) : CPropertySheet (_T("Graph Options"), pParentWnd, 0)
    {
        m_pPlot = NULL;

        m_bTemplate = FALSE;

        AddPage(&m_Curve);
        AddPage(&m_Axis);
        AddPage(&m_Scale);
        AddPage(&m_Line);
        AddPage(&m_General);
    }

    ~CPlotopt(void);

DECLARE_MESSAGE_MAP()

    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

public:
    virtual BOOL OnInitDialog();
};
