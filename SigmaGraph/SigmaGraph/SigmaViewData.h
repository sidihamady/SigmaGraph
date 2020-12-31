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

// CSigmaViewData view

#include "Datasheet.h"
#include "SigmaDoc.h"

class CSigmaViewData : public CView
{
    DECLARE_DYNCREATE(CSigmaViewData)

protected:
    CSigmaViewData();                                // protected constructor used by dynamic creation
    virtual ~CSigmaViewData();

public:
    virtual void OnDraw(CDC* pDC);            // overridden to draw this view
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    CDatasheet m_Datasheet;

protected:
    DECLARE_MESSAGE_MAP()

public:
    CSigmaDoc* GetDocument() const;
    CDatasheet* GetDatasheet() { return &m_Datasheet; }

    afx_msg int_t OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnInitialUpdate();

protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
    afx_msg void OnSize(UINT nType, int_t cx, int_t cy);
    virtual BOOL OnCmdMsg(UINT nID, int_t nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:

    afx_msg void OnViewPlot();
    afx_msg void OnViewDataFit();
    afx_msg void OnViewFitReport();
    afx_msg void OnUpdateViewPlot(CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewFit(CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewFitReport(CCmdUI *pCmdUI);

    afx_msg void OnClose();

protected:
    virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};



#ifndef _DEBUG  // debug version in SigmaPlotView.cpp
inline CSigmaDoc* CSigmaViewData::GetDocument() const
        { return reinterpret_cast<CSigmaDoc*>(m_pDocument); }
#endif
