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
#include "FlatButton.h"

#include "afxwin.h"
#include "afxcmn.h"

typedef struct _ListSortInfoM {
    CListCtrl* pListControl;
    int_t nColumnNo;
    bool nAscendingSortOrder;
} ListSortInfoM;

// CFitterModel dialog

class CFitterModel : public CPropertyPage
{
    DECLARE_DYNAMIC(CFitterModel)

public:
    CFitterModel();
    virtual ~CFitterModel();

// Dialog Data
    enum { IDD = IDD_FITTER_MODEL };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

    CDocument *m_pDoc;
    plot_t *m_pPlot;

    fit_t *m_pFit;
    vector_t *m_pColumnDataX;           // Colonne des données à fitter (X)
    vector_t *m_pColumnDataY;           // Colonne des données à fitter (Y)
    vector_t *m_pColumnFitX;            // Colonne du fit (X)
    vector_t *m_pColumnFitY;            // Colonne du fit (Y)
    vector_t *m_pColumnFitYp1;          // Colonne du fit (Y) - Peak 1
    vector_t *m_pColumnFitYp2;          // Colonne du fit (Y) - Peak 2
    vector_t *m_pColumnFitYp3;          // Colonne du fit (Y) - Peak 3
    vector_t *m_pColumnFitYp4;          // Colonne du fit (Y) - Peak 4
    vector_t *m_pColumnFitYp5;          // Colonne du fit (Y) - Peak 5

    long_t m_arrColumnFitId[5];         // Id des colonnes du fit -multi-peaks)

    BOOL *m_pbIsModified;
    BOOL *m_pbRunning;

    BOOL m_bOK;

    BOOL m_bInit;
    CListCtrl m_lstModel;
    CStatic m_bmpFunction;
    CBitmap m_Bitmap;

    static int_t CALLBACK SortList(LPARAM lpOne, LPARAM lpTwo, LPARAM lpArg);

    ListSortInfoM m_SortInfo;

    typedef struct _ITEMDATA {
        LPTSTR pszName;
        LPTSTR pszFunction;
        LPTSTR pszPar;
    } ITEMDATA, *PITEMDATA;

    ITEMDATA m_Data[ML_FIT_MAXMODEL];

    fit_t m_AvailableModel[ML_FIT_MAXMODEL];
    fit_t *m_pSelectedModel;
    int_t m_nDefaultIndex;

    real_t *m_pWdat;

    int m_iPage;

    CEdit m_edtCurve;
    CEditReal m_edtParA;
    CEditReal m_edtParB;
    CEditReal m_edtParC;
    CEditReal m_edtParD;

    CButton m_cbFixedParA;
    CButton m_cbFixedParB;
    CButton m_cbFixedParC;
    CButton m_cbFixedParD;

    CButton m_cbPlotPeaks;

    BOOL m_bFixedParA;
    BOOL m_bFixedParB;
    BOOL m_bFixedParC;
    BOOL m_bFixedParD;

    BOOL m_bPlotPeaks;

    BOOL m_bReport;

    int_t m_nPoints;
    CEditReal m_edtXmin;
    CEditReal m_edtXmax;
    CEdit m_edtIter;
    CEdit m_edtChi2;

    CFlatButton m_btnHelp;

    int_t AutoParam(real_t *px, real_t *py, int_t ndat, int_t ftype, real_t *fpar);

    void UpdateModel(int_t nDefaultIndex);
    void UpdatePar(fit_t *pModel);

    void PrintReport(int_t iCurve);
    void ActivateFitterModel(void);
    void ActivateFitterUserModel(void);

    void updateNextPar(void);
    void ParamsToControls(void);
    void ControlsToParams(void);

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void doAutoPar(bool bVerbose);

    afx_msg void OnFitterNextPar();
    afx_msg void OnFitterUser();
    afx_msg void OnFitterFit();
    afx_msg void OnAutoPar();
    afx_msg void OnChangedFitterModel(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnItemClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnFitterReset();
    afx_msg void OnCbnSelchangeUsermodelList();
    afx_msg void OnNMClickFitterModel(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedUsermodel();
    afx_msg void OnPaint();
    afx_msg void OnHelpContents();
    virtual void OnOK();
    virtual void OnCancel();

    afx_msg void OnDestroy();
};
