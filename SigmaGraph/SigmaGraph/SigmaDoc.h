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

// :TODO: complete doc and translate to english

#pragma once

#include "../../LibGraph/LibFile/libfile.h"

#include "SigmaNote.h"
#include "Datasheet.h"

class CSigmaUndo;

class CSigmaDoc : public CDocument
{
protected: // create from serialization only
    CSigmaDoc();
    DECLARE_DYNCREATE(CSigmaDoc)

// Attributes
public:
    BOOL m_bInitialized;
    BOOL m_bInitializedData;
    BOOL m_bInitializedPlot;
    BOOL m_bInitializedDataFit;

// Operations
public:

// Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

// Implementation
public:
    virtual ~CSigmaDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

public:
    sigmadoc_t *m_pDoc;
    plot_t *m_pPlot;
    CString m_strFilename;
    bool canClose(void)
    {
        return (m_strFilename.IsEmpty()) && (IsModified() == FALSE) && ((NULL == m_pPlot) || (m_pPlot->curvecount < 1));
    }

    WINDOWPLACEMENT m_PlacementPlot;
    WINDOWPLACEMENT m_PlacementDatasheet;
    WINDOWPLACEMENT m_PlacementDatasheetFit;

    vector_t *m_pColumnAutoX;    // Auto-X column

    BOOL m_bRunning;
    BOOL m_bIsModified;

    void SaveSigmaDoc();
    void SetModifiedFlag(BOOL bModified = TRUE);

    // Note window
    CSigmaNote* m_pwndNote;

    BOOL m_bAutoScale;
    BOOL GetAutoScale(void) { return m_bAutoScale; }
    void SetAutoScale(BOOL bAutoScale = TRUE);
    CView* GetPlotView(void);
    CView* GetPlotViewX(void);
    CView* GetDataView(void);
    CView* GetDataFitView(void);
    CDatasheet* GetDatasheet(int_t nGroup = 0);
    CString GetFilename(void) { return m_strFilename; }
    BOOL ActivateView(int_t iView);

    BOOL m_bPrinting;

    void Logi(LPCTSTR pszT);
    void Loge(LPCTSTR pszT);

public:
    afx_msg void OnFileSave();

    sigmadoc_t* GetDoc() { return m_pDoc; }

    void PlotInit();
    plot_t* GetPlot() { return m_pPlot; }

    void ResetData();

    int_t GetID() const { return m_pDoc->nID; }

    BOOL BlankDocument();

    // Operations on Columns and Rows
    void UpdateSigmaDoc();
    BOOL* GetRunning();
    void SetRunning(BOOL bRunning = TRUE);
    int_t GetColumnIndex(int_t nGroup, long_t uiID);
    vector_t* GetColumn(int_t nGroup, int_t nCol, long_t uiID = 0);
    vector_t* GetColumns(int_t nGroup, int_t *pColumnCount = NULL);
    vector_t* GetColumnAutoX(void);
    stat_t* GetStats(int_t nGroup, int_t nCol, long_t uiID = 0);
    fit_t* GetFit(int_t nCol, long_t uiID = 0);
    int_t GetFitCount();
    int_t GetColumnID(int_t nGroup, const char_t *pszName);
    BOOL GetColumnNameOrLabel(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID = 0);
    BOOL GetColumnName(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID = 0);
    BOOL GetColumnLabel(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID = 0);
    BOOL GetColumnFormat(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID = 0);
    real_t* GetColumnData(int_t nGroup, int_t nCol, long_t uiID = 0);
    byte_t* GetColumnMask(int_t nGroup, int_t nCol, long_t uiID = 0);
    int_t GetColumnCount(int_t nGroup);

    BOOL canViewFit(void);

    int_t GetColumnDim(int_t nGroup, int_t nCol, long_t uiID = 0);
    real_t GetColumnItem(int_t nGroup, int_t nCol, int_t uiRow, long_t uiID = 0);
    BOOL SetColumnItem(int_t nGroup, int_t nCol, int_t uiRow, real_t fItem, long_t uiID = 0);
    byte_t GetColumnItemMask(int_t nGroup, int_t nCol, int_t uiRow, long_t uiID = 0);
    BOOL SetColumnItemMask(int_t nGroup, int_t nCol, int_t uiRow, byte_t bMask, long_t uiID = 0);
    BOOL SetColumnStatus(int_t nGroup, int_t nCol, byte_t bStatus, long_t uiID = 0);
    BOOL SetColumnName(int_t nGroup, int_t nCol, const char_t* szBuffer, long_t uiID = 0);
    BOOL SetColumnLabel(int_t nGroup, int_t nCol, const char_t* szBuffer, long_t uiID = 0);
    BOOL SetColumnFormat(int_t nGroup, int_t nCol, const char_t* szFormat, long_t uiID = 0);
    BOOL MaskColumnData(int_t nGroup, byte_t bMask, int_t iMin, int_t iMax, int_t nCol, long_t uiID = 0);
    BOOL MaskColumnData(int_t nGroup, byte_t bMask, real_t fMin, real_t fMax, int_t nCol, long_t uiID = 0, byte_t bFlag = 0x00);

    // Functions only accessible to the attached datasheet (then use index and not ID)
    BOOL SortColumns(int_t nGroup, int_t nPivot, byte_t bAsc, byte_t *bLink);
    BOOL DeleteColumn(int_t nGroup, int_t nCol, long_t uiID = 0);
    vector_t* InsertColumn(int_t nGroup, int_t nCol, int_t nColumnDim = 0);
    int_t DeleteRow(int_t nGroup, int_t uiRow);
    int_t InsertRow(int_t nGroup, int_t uiRow);

    int_t CurveAdd(int_t nGroup, int_t idx, int_t idy, byte_t linestyle, byte_t dotstyle,
        byte_t axisused = 0x00, BOOL bUpdate = TRUE);
    int_t CurveRemove(int_t nGroup, int_t idy, const char_t *pszLegend = NULL, BOOL bUpdate = TRUE);
    int_t CurveIndex(const char_t *pszLegend);

    // Drag and Drop
    BOOL m_bDropping;
    char_t m_szDropFilename[ML_STRSIZE];

    // Fitting
    fit_t m_FittingModel[ML_FIT_MAXMODEL];
    fit_t m_FittingUserModel;

    // Opérations d'Undo/Redo
    CSigmaUndo *m_pUndo;
    BOOL CanUndo();
    BOOL CanRedo();
    void SaveState();
    void Undo();
    void Redo();

    // Opérations d'importation de données ASCII
    void DoDataImport(vector_t *pColumnT, int_t *piColumnT, int_t iDataCount);
    void OnDataImport();
    void OnDataImportDlg();

    vector_t* AppendFitColumn(vector_t *pColumnFitY, fit_t *pFit, int_t iPos);

    CString GetName();

    void AddToNote(const char_t *pszAppend, int_t iSize);

    void ShowMathDialog(int_t iOperation);

    void UpdateDocTitle();

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

public:
    virtual void OnCloseDocument();
    virtual void SetTitle(LPCTSTR lpszTitle);
    afx_msg void OnViewNote();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    afx_msg void OnDataExport();
    afx_msg void OnAutoExport();
    afx_msg void OnFileSaveAs();
    afx_msg void OnMathIntegrate();
    afx_msg void OnMathDifferentiate();
    afx_msg void OnMathAverage();
    afx_msg void OnMathFFT();
    afx_msg void OnAutocorrelation();

};


