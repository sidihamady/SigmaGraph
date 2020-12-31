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

#include "ReadonlyEdit.h"

// CSigmaStats dialog

class CSigmaStats : public CDialog
{
    DECLARE_DYNAMIC(CSigmaStats)

public:
    CSigmaStats(CWnd* pParent = NULL);   // standard constructor
    virtual ~CSigmaStats();

// Dialog Data
    enum { IDD = IDD_STATS };

    CReadonlyEdit m_edtPoints;
    CReadonlyEdit m_edtMin;
    CReadonlyEdit m_edtMax;
    CReadonlyEdit m_edtSum;
    CReadonlyEdit m_edtMean;
    CReadonlyEdit m_edtMedian;
    CReadonlyEdit m_edtVar;
    CReadonlyEdit m_edtDev;
    CReadonlyEdit m_edtCoeff;
    CReadonlyEdit m_edtRMS;
    CReadonlyEdit m_edtSkew;
    CReadonlyEdit m_edtKurt;

    vector_t *m_pColumn;
    stat_t *m_pStats;
    BOOL *m_pbRunning;

    int_t m_nPoints;
    char_t m_szMin[ML_STRSIZES];
    char_t m_szMax[ML_STRSIZES];
    char_t m_szSum[ML_STRSIZES];
    char_t m_szMean[ML_STRSIZES];
    char_t m_szMedian[ML_STRSIZES];
    char_t m_szVar[ML_STRSIZES];
    char_t m_szDev[ML_STRSIZES];
    char_t m_szCoeff[ML_STRSIZES];
    char_t m_szRMS[ML_STRSIZES];
    char_t m_szSkew[ML_STRSIZES];
    char_t m_szKurt[ML_STRSIZES];

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnCopy();
};
