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

#include "EditReal.h"
#include "afxwin.h"
#include "EditInteger.h"

// CFitterMask dialog

class CFitterMask : public CPropertyPage
{
    DECLARE_DYNAMIC(CFitterMask)

public:
    CFitterMask();
    virtual ~CFitterMask();

// Dialog Data
    enum { IDD = IDD_FITTER_FILTER };

    virtual BOOL OnInitDialog();

    CDocument *m_pDoc;
    plot_t *m_pPlot;

    fit_t *m_pFit;
    vector_t *m_pColumnDataX;           // Colonne des données à fitter (X)
    vector_t *m_pColumnDataY;           // Colonne des données à fitter (Y)
    vector_t *m_pColumnFitX;            // Colonne du fit (X)
    vector_t *m_pColumnFitY;            // Colonne du fit (X)

    BOOL *m_pbIsModified;
    BOOL *m_pbRunning;

    vector_t m_ColumnUndoX;            // Colonne de sauvegarde (X)
    vector_t m_ColumnUndoY;            // Colonne de sauvegarde (Y)

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CEdit m_edtCurve;
    int_t m_nPoints;
    BOOL m_bMask;
    CEditReal m_edtMaskMin;
    CEditReal m_edtMaskMax;
    CEditReal m_edtBaseline0;
    CEditReal m_edtBaseline1;
    CEditInteger m_edtFilter;
    int_t m_nSmooth;
    BOOL m_bBaseline;
    BOOL m_bSmooth;
    CStatic m_stOutput;

    int m_iStart;
    int m_iStop;

    CFlatButton m_btnHelp;

    CButton m_bnUndo;
    BOOL m_bCanUndo;

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnProcess();
    afx_msg void OnUndo();
    afx_msg void OnHelpContents();
    virtual void OnOK();
    virtual void OnCancel();

    afx_msg void OnDestroy();
};
