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

#include "stdafx.h"
#include "Sigma.h"
#include "SigmaDoc.h"
#include "SigmaViewPlot.h"
#include "SigmaViewData.h"
#include "SigmaViewDataFit.h"
#include "MainFrm.h"
#include "ImportDlg.h"
#include "ImportDropDlg.h"
#include "AnalysisDlg.h"

#include "SigmaUndo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSigmaDoc

IMPLEMENT_DYNCREATE(CSigmaDoc, CDocument)

BEGIN_MESSAGE_MAP(CSigmaDoc, CDocument)
    ON_COMMAND(ID_FILE_SAVE, &CSigmaDoc::OnFileSave)
    ON_COMMAND(ID_VIEW_NOTE, &CSigmaDoc::OnViewNote)
    ON_COMMAND(ID_FILE_EXPORTD, &CSigmaDoc::OnDataExport)
    ON_COMMAND(ID_FILE_AUTOEXPORT, &CSigmaDoc::OnAutoExport)
    ON_COMMAND(ID_FILE_SAVE_AS, &CSigmaDoc::OnFileSaveAs)
    ON_COMMAND(ID_ANALYSIS_INTEGRATE, &CSigmaDoc::OnMathIntegrate)
    ON_COMMAND(ID_ANALYSIS_DIFF, &CSigmaDoc::OnMathDifferentiate)
    ON_COMMAND(ID_ANALYSIS_AVERAGE, &CSigmaDoc::OnMathAverage)
    ON_COMMAND(ID_ANALYSIS_FFT, &CSigmaDoc::OnMathFFT)
    ON_COMMAND(ID_ANALYSIS_AUTOCORR, &CSigmaDoc::OnAutocorrelation)
END_MESSAGE_MAP()

inline void CSigmaDoc::Logi(LPCTSTR pszT)
{
    SigmaApp.Output(pszT, TRUE, FALSE, SIGMA_OUTPUT_INFORMATION, -1);
}
inline void CSigmaDoc::Loge(LPCTSTR pszT)
{
    SigmaApp.Output(pszT, TRUE, TRUE, SIGMA_OUTPUT_ERROR, -1);
}

// CSigmaDoc construction/destruction

void CSigmaDoc::ResetData()
{
    if (m_pDoc == NULL) {
        return;
    }

    // >> [MOD-SG240-002]: AutoX
    if (m_pColumnAutoX) {
        if (m_pColumnAutoX->dat) {
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_delete(m_pColumnAutoX, SigmaApp.m_pLib);
        }
        memset(m_pColumnAutoX, 0, sizeof(vector_t));
    }
    // <<

    // Blank document
    for (int_t jj = 0; jj < SIGMA_MAXCOLUMNS; jj++) {
        if (m_pDoc->ColumnData[jj].dat) {
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_delete(&(m_pDoc->ColumnData[jj]), SigmaApp.m_pLib);
        }
        if (m_pDoc->ColumnDataFit[jj].dat) {
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_delete(&(m_pDoc->ColumnDataFit[jj]), SigmaApp.m_pLib);
        }
    }
    memset(m_pDoc, 0, sizeof(sigmadoc_t));

    if (m_pUndo != NULL) {
        m_pUndo->Reset();
    }

    m_bRunning = FALSE;;
    m_bIsModified = FALSE;
    m_pDoc->iColumnID = 1;

    m_bAutoScale = FALSE;

    // Drag and Drop
    m_bDropping = FALSE;
    memset(m_szDropFilename, 0, ML_STRSIZE * sizeof(char_t));

    m_strFilename.Empty();
}

CSigmaDoc::CSigmaDoc()
{
    m_pwndNote = NULL;
    m_pDoc = NULL;
    m_pPlot = NULL;
    m_strFilename.Empty();

    // >> [MOD-SG240-002]: AutoX
    m_pColumnAutoX = NULL;
    // <<

    m_bInitialized = FALSE;
    m_bInitializedData = FALSE;
    m_bInitializedPlot = FALSE;
    m_bInitializedDataFit = FALSE;

    // Undo/Redo
    m_pUndo = NULL;

    m_bPrinting = FALSE;

    // Drag and Drop
    m_bDropping = FALSE;
    memset(m_szDropFilename, 0, ML_STRSIZE * sizeof(char_t));

    memset(&m_PlacementPlot, 0, sizeof(WINDOWPLACEMENT));
    m_PlacementPlot.length = sizeof(WINDOWPLACEMENT);
    memset(&m_PlacementDatasheet, 0, sizeof(WINDOWPLACEMENT));
    m_PlacementDatasheet.length = sizeof(WINDOWPLACEMENT);
    memset(&m_PlacementDatasheetFit, 0, sizeof(WINDOWPLACEMENT));
    m_PlacementDatasheetFit.length = sizeof(WINDOWPLACEMENT);
}

CSigmaDoc::~CSigmaDoc()
{
    if (m_pwndNote) {
        m_pwndNote->DestroyWindow();
        delete m_pwndNote;
        m_pwndNote = NULL;
    }

    // Delete columns from document, etc.
    ResetData();

    if (m_pDoc != NULL) {
        delete m_pDoc;
        m_pDoc = NULL;
    }

    // >> [MOD-SG240-002]: AutoX
    if (m_pColumnAutoX) {
        delete m_pColumnAutoX;
        m_pColumnAutoX = NULL;
    }
    // <<

    // Undo/Redo
    if (m_pUndo != NULL) {
        m_pUndo->Reset();
        delete m_pUndo;
        m_pUndo = NULL;
    }

    if (m_pPlot != NULL) {
        delete m_pPlot;
        m_pPlot = NULL;
    }
}

void CSigmaDoc::SaveState()
{
    if (m_pUndo) {
        m_pUndo->SaveState(m_pDoc, m_pPlot);
    }
}
BOOL CSigmaDoc::CanUndo()
{
    if (m_pUndo) {
        return m_pUndo->CanUndo();
    }
    return FALSE;
}
BOOL CSigmaDoc::CanRedo()
{
    if (m_pUndo) {
        return m_pUndo->CanRedo();
    }
    return FALSE;
}

void CSigmaDoc::SetAutoScale(BOOL bAutoScale/* = TRUE*/)
{
    m_bAutoScale = bAutoScale;
}

void CSigmaDoc::PlotInit()
{
    RECT recFrame = {0,0,0,0};

    m_pPlot = new(std::nothrow) plot_t;
    if (m_pPlot == NULL) {    // Error handling
        return;
    }

    memset(m_pPlot, 0, sizeof(plot_t));
    pl_reset(m_pPlot, 1);
    memcpy(m_pPlot, SigmaApp.m_pDefaultPlot, sizeof(plot_t));

    m_pPlot->curvecount = 0;
    m_pPlot->curveactive = 0;

    m_pPlot->imagetype = false;
    m_pPlot->image = NULL;
    m_pPlot->metafile = NULL;
    m_pPlot->svgdoc = NULL;
    memset(m_pPlot->imagefilename, 0, ML_STRSIZE * sizeof(char_t));

    return;
}

BOOL CSigmaDoc::BlankDocument()
{
    if (m_pDoc == NULL) {
        m_pDoc = new sigmadoc_t;
        ASSERT(m_pDoc);
        if (m_pDoc == NULL) {    // Error handling
            return FALSE;
        }
    }
    memset(m_pDoc, 0, sizeof(sigmadoc_t));
    m_pDoc->nID = SigmaApp.m_nDocID;
    SigmaApp.m_nDocID += 1;
    m_bRunning = FALSE;;
    m_bIsModified = FALSE;
    m_pDoc->iColumnID = 1;

    if (m_pColumnAutoX == NULL) {
        m_pColumnAutoX = new vector_t;
        ASSERT(m_pColumnAutoX);
        if (m_pColumnAutoX == NULL) {    // Error handling
            return FALSE;
        }
        memset(m_pColumnAutoX, 0, sizeof(vector_t));
    }

    m_pDoc->Software.version = SIGMA_SOFTWARE_VERSION;
    _tcscpy(m_pDoc->Software.name, SIGMA_SOFTWARE_NAME);
    _tcscpy(m_pDoc->Software.description, SIGMA_SOFTWARE_DESCRIPTION);
    _tcscpy(m_pDoc->Software.copyright, SIGMA_SOFTWARE_COPYRIGHT);
    _tcscpy(m_pDoc->Software.date, SIGMA_SOFTWARE_DATERELEASE);
    _tcscpy(m_pDoc->Software.time, SIGMA_SOFTWARE_TIMERELEASE);
    _tcscpy(m_pDoc->Software.reserved, SIGMA_SOFTWARE_RESERVED);

    // Undo/Redo
    if (m_pUndo == NULL) {
        m_pUndo = new CSigmaUndo();
    }
    //

    int_t jj;
    for (jj = 0; jj < SIGMA_MAXCOLUMNS; jj++) {
        // Les colonnes sont nommées A, B, C, D, ... à la façon d'Excel
        m_pDoc->ColumnData[jj].name[0] = _T('A') + jj;
        m_pDoc->ColumnData[jj].name[1] = _T('\0');
        m_pDoc->ColumnData[jj].opt = 0x00;
    }

    // >> [MOD-SG240-002]: AutoX
    m_pColumnAutoX->name[0] = _T('X');
    m_pColumnAutoX->name[1] = _T('\0');
    m_pColumnAutoX->label[0] = _T('X');
    m_pColumnAutoX->label[1] = _T('\0');
    m_pColumnAutoX->type = 0x10;
    m_pColumnAutoX->opt = 0x10;
    // <<

    // Plot
    PlotInit();

    return TRUE;
}

BOOL CSigmaDoc::OnNewDocument()
{
    int_t ii, nlen;

    if (!CDocument::OnNewDocument()) {
        return FALSE;
    }

    // Blank document
    if (BlankDocument() == FALSE) {
        return FALSE;
    }

    if (m_bDropping == TRUE) {

        // Always show the import dialog
        CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
        ASSERT(pFrame != NULL);
        if (pFrame == NULL) {
            return FALSE;
        }
        pFrame->SetForegroundWindow();

        CImportDropDlg importDlg;
        importDlg.m_strFilename = m_szDropFilename;
        importDlg.m_pColumn = m_pDoc->ColumnData;
        importDlg.m_pCurrentID = &(m_pDoc->iColumnID);
        importDlg.m_pColumnCount = &(m_pDoc->iDataCount);
        importDlg.m_pbIsModified = &m_bIsModified;
        importDlg.m_pbRunning = &m_bRunning;
        importDlg.m_pszNote = m_pDoc->szNote;

        importDlg.DoModal();

        if (importDlg.m_bOK == FALSE) {
            m_bDropping = FALSE;
            memset(m_szDropFilename, 0, ML_STRSIZE * sizeof(char_t));
            return FALSE;
        }
        SigmaApp.m_Options.import.iShow = 0;
        //

        nlen = (int_t)_tcslen((const char_t*)m_szDropFilename);
        if ((nlen > 5) && (nlen < ML_STRSIZE)) {
            SetPathName((LPCTSTR) m_szDropFilename);
        }

        // >> [MOD-SG240-002]: AutoX
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_create(m_pColumnAutoX, m_pDoc->ColumnData[0].dim, SIGMA_MAXCOLUMNS + 99, _T("X"), NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            Loge(_T("Cannot create new document: insufficient memory.\r\n"));
            return FALSE;
        }
        _tcscpy(m_pColumnAutoX->label, _T("X"));
        m_pColumnAutoX->type = 0x10;
        for (ii = 0; ii < m_pColumnAutoX->dim; ii++) {
            m_pColumnAutoX->dat[ii] = (real_t) ii;
            m_pColumnAutoX->mask[ii] = 0x00;
        }
        // <<

        m_bDropping = FALSE;
        memset(m_szDropFilename, 0, ML_STRSIZE * sizeof(char_t));
    }
    else {
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_create(&(m_pDoc->ColumnData[0]), 11, m_pDoc->iColumnID, _T("A"), NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            Loge(_T("Cannot create new document: insufficient memory.\r\n"));
            return FALSE;
        }
        _tcscpy(m_pDoc->ColumnData[0].label, _T("A"));
        m_pDoc->ColumnData[0].type = 0x10;
        m_pDoc->iColumnID += 1;
        ml_vector_create(&(m_pDoc->ColumnData[1]), 11, m_pDoc->iColumnID, _T("B"), NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            // >> NB: le destructeur de CSigmaDoc se charge aussi du nettoyage...
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_delete(&(m_pDoc->ColumnData[0]), SigmaApp.m_pLib);
            memset(&(m_pDoc->ColumnData[0]), 0, sizeof(vector_t));
            // <<
            Loge(_T("Cannot create new document: insufficient memory.\r\n"));
            return FALSE;
        }
        _tcscpy(m_pDoc->ColumnData[1].label, _T("B"));
        m_pDoc->ColumnData[1].type = 0x20;
        m_pDoc->ColumnData[1].idx = m_pDoc->ColumnData[0].id;
        m_pDoc->iColumnID += 1;

        m_pDoc->iDataCount = 2;

        // >> [MOD-SG240-002]: AutoX
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_create(m_pColumnAutoX, 11, SIGMA_MAXCOLUMNS + 99, _T("X"), NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            Loge(_T("Cannot create new document: insufficient memory.\r\n"));
            return FALSE;
        }
        _tcscpy(m_pColumnAutoX->label, _T("X"));
        m_pColumnAutoX->type = 0x10;
        // <<

        for (ii = 0; ii < m_pDoc->ColumnData[0].dim; ii++) {
            m_pDoc->ColumnData[0].dat[ii] = 0.0;
            m_pDoc->ColumnData[1].dat[ii] = 0.0;
            m_pDoc->ColumnData[0].mask[ii] = 0xFF;    // Invalid item (to be edited)
            m_pDoc->ColumnData[1].mask[ii] = 0xFF;

            m_pColumnAutoX->dat[ii] = (real_t) ii;
            m_pColumnAutoX->mask[ii] = 0x00;
        }
        m_pDoc->ColumnData[0].status = 0xFF;            // Invalid column (column data is being filled)
        m_pDoc->ColumnData[1].status = 0xFF;
        m_pDoc->ColumnData[0].opt = 0x00;
        m_pDoc->ColumnData[1].opt = 0x00;
    }

    SigmaApp.m_nDocCount += 1;

    // Fitting
    memset(m_FittingModel, 0, ML_FIT_MAXMODEL * sizeof(fit_t));
    ml_fit_getmodel(m_FittingModel, ML_FIT_MAXMODEL);
    memset(&m_FittingUserModel, 0, sizeof(fit_t));
    m_FittingUserModel.iter = SigmaApp.m_Options.fit.iter;
    m_FittingUserModel.lambda = SigmaApp.m_Options.fit.lambda;

    SetModifiedFlag(FALSE);

    UpdateAllViews(NULL);

    if (m_pUndo != NULL) {
        m_pUndo->Reset();
        SaveState();
    }

    return TRUE;
}

BOOL CSigmaDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    CWaitCursor wcT;

    // Blank document
    if (BlankDocument() == FALSE) {
        return FALSE;
    }

    // Fitting
    memset(m_FittingModel, 0, ML_FIT_MAXMODEL * sizeof(fit_t));
    ml_fit_getmodel(m_FittingModel, ML_FIT_MAXMODEL);
    memset(&m_FittingUserModel, 0, sizeof(fit_t));
    m_FittingUserModel.iter = SigmaApp.m_Options.fit.iter;
    m_FittingUserModel.lambda = SigmaApp.m_Options.fit.lambda;

    if (SigmaApp.OpenSigmaDoc(lpszPathName, m_pDoc, m_pColumnAutoX, m_pPlot,
        &m_PlacementPlot, &m_PlacementDatasheet, &m_PlacementDatasheetFit) == FALSE) {
        return FALSE;
    }

    m_strFilename = (LPCTSTR)lpszPathName;

    SigmaApp.AddToRecentFileList((LPCTSTR)m_strFilename);
    SetPathName(lpszPathName);

    SigmaApp.m_nDocCount += 1;

    UpdateAllViews(NULL);

    if (m_pUndo != NULL) {
        m_pUndo->Reset();
        SaveState();
    }

    return TRUE;
}

CView* CSigmaDoc::GetPlotViewX(void)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        return NULL;
    }
    CView *pView = pFrame->GetSigmaActiveViewX(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot));
    if (pView == NULL) {
        return NULL;
    }
    return pView;
}

CView* CSigmaDoc::GetPlotView(void)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        return NULL;
    }
    CView *pView = pFrame->GetSigmaActiveView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot));
    if (pView == NULL) {
        return NULL;
    }
    return pView;
}

CView* CSigmaDoc::GetDataView(void)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        return NULL;
    }
    CView *pView = pFrame->GetSigmaActiveView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData));
    if (pView == NULL) {
        return NULL;
    }
    return pView;
}

CView* CSigmaDoc::GetDataFitView(void)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        return NULL;
    }
    CView *pView = pFrame->GetSigmaActiveView(SigmaApp.m_pTemplateDataFit, RUNTIME_CLASS(CSigmaViewDataFit));
    if (pView == NULL) {
        return NULL;
    }
    return pView;
}

CDatasheet* CSigmaDoc::GetDatasheet(int_t nGroup/* = 0*/)
{
    if (nGroup == 0) {
        CSigmaViewData* pView = (CSigmaViewData*)(GetDataView());
        if (pView != NULL) {
            return ((CDatasheet*)(pView->GetDatasheet()));
        }
    }
    else if (nGroup == 1) {
        CSigmaViewDataFit* pView = (CSigmaViewDataFit*)(GetDataFitView());
        if (pView != NULL) {
            return ((CDatasheet*)(pView->GetDatasheet()));
        }
    }

    return NULL;
}

BOOL CSigmaDoc::ActivateView(int_t iView)
{
    CFrameWnd *pFrmWnd = NULL;
    CView *pView = NULL;

    CMainFrame *pMainFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pMainFrame != NULL);
    if (pMainFrame == NULL) {
        return FALSE;
    }
    CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
    ASSERT(pMDIActive != NULL);
    if (pMDIActive == NULL) {
        return FALSE;
    }

    pView = pMDIActive->GetActiveView();
    ASSERT(pView != NULL);
    if (pView == NULL) {
        return FALSE;
    }
    UINT uiState = SW_SHOWNORMAL;
    CWnd *pWnd = pView->GetParent();
    ASSERT(pWnd != NULL);
    if (pWnd == NULL) {
        return FALSE;
    }
    if (pWnd->IsZoomed()) {
        uiState = SW_SHOWMAXIMIZED;
    }

    POSITION pos = GetFirstViewPosition();
    while (pos != NULL) {
        pView = GetNextView(pos);
        if (pView == NULL) {
            break;
        }
        if (pView->IsKindOf((iView == 0) ? RUNTIME_CLASS(CSigmaViewData) : RUNTIME_CLASS(CSigmaViewPlot))) {
            pFrmWnd = pView->GetParentFrame();
            ASSERT(pFrmWnd != NULL);
            if (pFrmWnd == NULL) {
                return FALSE;
            }
            pFrmWnd->ActivateFrame();
            pFrmWnd->ShowWindow(uiState);
            return TRUE;
        }
    }

    return FALSE;
}

// CSigmaDoc serialization

void CSigmaDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring()) {

    }
    else {

    }
}


// CSigmaDoc diagnostics

#ifdef _DEBUG
void CSigmaDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CSigmaDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CSigmaDoc commands
void CSigmaDoc::SaveSigmaDoc()
{
    BOOL bNewFile = FALSE;
    if (m_strFilename.IsEmpty()) {
         bNewFile = TRUE;
    }
    else {
        HANDLE hFile = INVALID_HANDLE_VALUE;
        hFile = ::CreateFile((LPCTSTR)m_strFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            bNewFile = TRUE;
        }
        else {
            CloseHandle(hFile);
            bNewFile = FALSE;
        }
    }

    if (m_pwndNote) {
        m_pwndNote->m_edtNote.GetWindowText((LPTSTR)(m_pwndNote->m_pszNote), ML_STRSIZEW - 1);
    }

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        return;
    }

    WINDOWPLACEMENT mdiPlace;
    memset(&mdiPlace, 0, sizeof(WINDOWPLACEMENT));
    mdiPlace.length = sizeof(WINDOWPLACEMENT);
    pFrame->GetWindowPlacement(&mdiPlace);

    CView *pView = GetPlotView();
    if (pView) {
        CWnd *pWnd = pView->GetOwner();
        if (pWnd) {
            m_PlacementPlot.length = sizeof(WINDOWPLACEMENT);
            pWnd->GetWindowPlacement(&m_PlacementPlot);
        }
    }
    pView = GetDataView();
    if (pView) {
        CWnd *pWnd = pView->GetOwner();
        if (pWnd) {
            m_PlacementDatasheet.length = sizeof(WINDOWPLACEMENT);
            pWnd->GetWindowPlacement(&m_PlacementDatasheet);
        }
    }
    pView = GetDataFitView();
    if (pView) {
        CWnd *pWnd = pView->GetOwner();
        if (pWnd) {
            m_PlacementDatasheetFit.length = sizeof(WINDOWPLACEMENT);
            pWnd->GetWindowPlacement(&m_PlacementDatasheetFit);
        }
    }

    if (bNewFile == TRUE) {
        char_t szFilters[] = _T("SigmaGraph files (*.sid)|*.sid||");

        POSITION pos = GetFirstViewPosition();
        CView *pView = GetNextView(pos);
        if (pView == NULL) {
            return;
        }

        CString strFilename = GetName();
        strFilename += _T(".sid");
        
        CFileDialog *pFileDlg = new CFileDialog(FALSE, _T("sid"), (LPCTSTR) (strFilename), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, (CWnd*) pView);
        if (pFileDlg->DoModal() != IDOK) {
            delete pFileDlg; pFileDlg = NULL;
            return;
        }
        m_strFilename = pFileDlg->GetPathName();
        CString strExt = pFileDlg->GetFileExt();
        delete pFileDlg; pFileDlg = NULL;

        strExt.MakeLower();

        if (strExt == _T("sid")) {
            HANDLE hFile = INVALID_HANDLE_VALUE;
            hFile = ::CreateFile((LPCTSTR)m_strFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE) {
                CloseHandle(hFile);
                int_t iT = pFrame->MessageBox(_T("File already exists. Overwrite it?"), _T("SigmaGraph"), MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON3);
                if (iT != IDYES) {
                    return;
                }
            }

            // save document
            CWaitCursor wait;
            if (SigmaApp.SaveSigmaDoc((LPCTSTR)m_strFilename, (const sigmadoc_t*)m_pDoc, (const vector_t*)m_pColumnAutoX,
                (const plot_t*)m_pPlot, (const WINDOWPLACEMENT*)(&m_PlacementPlot),
                (const WINDOWPLACEMENT*)(&m_PlacementDatasheet), (const WINDOWPLACEMENT*)(&m_PlacementDatasheetFit)) == TRUE) {
                CString strT = _T("Document saved to ");
                strT += m_strFilename;
                strT += _T("\r\n");
                SigmaApp.OutputSB(strT, 0, TRUE);
                Logi((LPCTSTR) strT);

                UpdateDocTitle();
            }
        }
    }
    else {
        // save document
        CWaitCursor wait;
        if (SigmaApp.SaveSigmaDoc((LPCTSTR)m_strFilename, (const sigmadoc_t*)m_pDoc, (const vector_t*)m_pColumnAutoX,
            (const plot_t*)m_pPlot, (const WINDOWPLACEMENT*)(&m_PlacementPlot), (const WINDOWPLACEMENT*)(&m_PlacementDatasheet),
            (const WINDOWPLACEMENT*)(&m_PlacementDatasheetFit)) == TRUE) {
            SigmaApp.AddToRecentFileList((LPCTSTR)m_strFilename);
            CString strT = _T("Document saved to ");
            strT += m_strFilename;
            strT += _T("\r\n");
            SigmaApp.OutputSB(strT, 0, TRUE);
            Logi((LPCTSTR) strT);
        }
    }

    SetModifiedFlag(FALSE);
}

// CSigmaDoc commands
void CSigmaDoc::OnFileSave()
{
    if (IsModified() == FALSE) {
        return;
    }

    CWaitCursor wcT;

    SaveSigmaDoc();
}

void CSigmaDoc::SetModifiedFlag(BOOL bModified/* = TRUE*/)
{
    CString strTitle = GetTitle();
    strTitle.TrimRight(_T("*"));
    if (strTitle.Right(11) == _T(" [readonly]")) {
        strTitle = strTitle.Left(strTitle.GetLength() - 11);
    }
    if (bModified) {
        SetTitle(strTitle + _T("*"));
    }
    else {
        SetTitle(strTitle);
    }
    CDocument::SetModifiedFlag(bModified);
}

void CSigmaDoc::OnViewNote()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame == NULL) {
        return;
    }

    if (m_pwndNote != NULL) {
        if (m_pwndNote->GetSafeHwnd() == NULL) {
            delete m_pwndNote;
            m_pwndNote= NULL;
        }
    }

    if (m_pwndNote == NULL) {
        m_pwndNote = new CSigmaNote();
        if (m_pwndNote) {
            m_pwndNote->m_pDoc = (CDocument*)this;
            m_pwndNote->m_pszNote = m_pDoc->szNote;
            if (m_pwndNote->Create(IDD_NOTE, pFrame)) {

                WINDOWPLACEMENT tPlacementMain;
                memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
                tPlacementMain.length = sizeof(WINDOWPLACEMENT);
                SigmaApp.GetWindowPlacement(_T("NotePlace.par"), &tPlacementMain, SIGMA_DEFWIDTH_CONSOLE, SIGMA_DEFHEIGHT_CONSOLE, 144);
                m_pwndNote->SetWindowPlacement(&tPlacementMain);

                int_t nlen = (int_t)_tcslen((const char_t*)(m_pwndNote->m_pszNote));
                m_pwndNote->m_edtNote.SetSel(nlen,nlen);
            }
            else {
                delete m_pwndNote;
                m_pwndNote = NULL;
                pFrame->MessageBox(_T("Cannot show note window: internal error."), _T("SigmaGraph"), MB_ICONERROR);
            }
        }
    }

    if (m_pwndNote) {
        CString strT = GetTitle();
        strT.TrimRight(_T("*"));
        strT.Append(_T(" - Note"));
        m_pwndNote->SetWindowText((LPCTSTR)strT);
        m_pwndNote->ShowWindow(SW_SHOW);
        m_pwndNote->SetForegroundWindow();
    }
}

void CSigmaDoc::AddToNote(const char_t *pszAppend, int_t iSize)
{
    if ((iSize < 2) || (iSize > ML_STRSIZE)) {
        return;
    }
    int_t nLength = (int_t)_tcslen(pszAppend);
    if ((nLength < 2) || (nLength > ML_STRSIZE)) {
        return;
    }
    int_t nLengthN = (int_t)_tcslen((const char_t*)(m_pDoc->szNote));
    if ((nLengthN < 0) || (nLengthN >= (ML_STRSIZEW - nLength - 2))) {
        return;
    }

    const char_t *pszT = _tcsstr((const char_t*)(m_pDoc->szNote), pszAppend);
    if (nLengthN > 0) {
        if (pszT == NULL) {
            if (m_pDoc->szNote[nLengthN - 1] != _T('\n')) {
                _tcscat(m_pDoc->szNote, _T("\r\n"));
            }
        }
    }

    if (pszT == NULL) {
        _tcscat(m_pDoc->szNote, pszAppend);
        _tcscat(m_pDoc->szNote, _T("\r\n"));
        if (m_pwndNote != NULL) {
            SetModifiedFlag(TRUE);
            CString strT = GetTitle();
            strT.TrimRight(_T("*"));
            strT.Append(_T(" - Note"));
            m_pwndNote->SetWindowText((LPCTSTR)strT);
            m_pwndNote->m_edtNote.SetWindowText((LPCTSTR)(m_pDoc->szNote));
        }
    }
}

void CSigmaDoc::OnCloseDocument()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT_VALID(pFrame);
    if (pFrame == NULL) {
        ResetData();
        SigmaApp.m_nDocCount -= 1;
        CDocument::OnCloseDocument();
        SetModifiedFlag(FALSE);
        return;
    }
    if (::IsWindow(pFrame->m_hWnd) == FALSE) {
        ResetData();
        SigmaApp.m_nDocCount -= 1;
        CDocument::OnCloseDocument();
        SetModifiedFlag(FALSE);
        return;
    }

    //    [FIX-SG200-005] Pour éviter que le message "Save document before closing?"
    //    soit affiché alors que la fenêtre principale est fermée
    if (::IsWindowVisible(pFrame->m_hWnd) == FALSE) {
        SetModifiedFlag(FALSE);
    }

    if (IsModified()) {
        int_t iT = pFrame->MessageBox(_T("Save document before closing?"), _T("SigmaGraph"), MB_YESNOCANCEL | MB_ICONQUESTION);
        if (iT == IDYES) {
            // Save document
            SaveSigmaDoc();
        }
        else if (iT == IDCANCEL) {
            return;
        }
        // Eviter que CMDIFrameWnd::OnClose() ne tente de sauvegarder le document (et l'écraser!)
        SetModifiedFlag(FALSE);
    }

    if (m_pwndNote) {

        WINDOWPLACEMENT tPlacementMain;
        memset(&tPlacementMain, 0, sizeof(WINDOWPLACEMENT));
        tPlacementMain.length = sizeof(WINDOWPLACEMENT);
        m_pwndNote->GetWindowPlacement(&tPlacementMain);
        SigmaApp.SetWindowPlacement(_T("NotePlace.par"), &tPlacementMain);

        delete m_pwndNote;
        m_pwndNote = NULL;
    }

    pFrame->CloseSigmaView(SigmaApp.m_pTemplateDataFit, RUNTIME_CLASS(CSigmaViewDataFit));
    pFrame->CloseSigmaView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot));
    pFrame->CloseSigmaView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData));

    ResetData();

    SigmaApp.m_nDocCount -= 1;

    CDocument::OnCloseDocument();
}

// Opérations sur les colonnes

void CSigmaDoc::UpdateSigmaDoc()
{
    int_t nCount = 0, nCountFit = 0, jj, ll, mm, iFound = 0, iFoundX = 0, iFoundY = 0, nDel = 0;
    long_t iDelete[ML_MAXCOLUMNS];
    vector_t *pColumn = NULL, *pColumnFit = NULL;

    nCount = m_pDoc->iDataCount;
    pColumn = m_pDoc->ColumnData;
    nCountFit = m_pDoc->iDataFitCount;
    pColumnFit = m_pDoc->ColumnDataFit;

    int nCountFitRaw = 0;
    for (ll = 0; ll < nCountFit; ll++) {
        if (pColumnFit[ll].ide < 1) {
            nCountFitRaw += 1;
        }
    }

    if ((pColumn == NULL) || (pColumnFit == NULL) || (nCount < 1) || (nCountFit < 0)) {
        return;
    }

    for (jj = 0; jj < nCount; jj++) {
        // >> Update column name (important e.g. when column deleted...)
        pColumn[jj].name[0] = _T('A') + jj;
        pColumn[jj].name[1] = _T('\0');
        // <<
        if ((pColumn[jj].type & 0xF0) == 0x10) {    // X
            if (pColumn[jj].ide > 0) {
                iFound = 0;
                for (ll = 0; ll < nCount; ll++) {
                    if ((pColumn[jj].ide == pColumn[ll].id) && ((pColumn[ll].type & 0xF0) == 0x30)) {
                        iFound = 1;
                        break;
                    }
                }
                if (iFound == 0) {
                    pColumn[jj].ide = 0;
                }
            }
            else {
                pColumn[jj].ide = 0;
            }
        }
        else if ((pColumn[jj].type & 0xF0) == 0x20) {    // Y
            if (pColumn[jj].idx > 0) {
                if (pColumn[jj].idx != m_pColumnAutoX->id) {
                    iFound = 0;
                    for (ll = 0; ll < nCount; ll++) {
                        if ((pColumn[jj].idx == pColumn[ll].id) && ((pColumn[ll].type & 0xF0) == 0x10)) {
                            iFound = 1;
                            break;
                        }
                    }
                    if (iFound == 0) {
                        pColumn[jj].idx = 0;
                    }
                }
            }
            else {
                pColumn[jj].idx = 0;
            }
            if (pColumn[jj].ide > 0) {
                iFound = 0;
                for (ll = 0; ll < nCount; ll++) {
                    if ((pColumn[jj].ide == pColumn[ll].id) && ((pColumn[ll].type & 0xF0) == 0x40)) {
                        iFound = 1;
                        break;
                    }
                }
                if (iFound == 0) {
                    pColumn[jj].ide = 0;
                }
            }
            if (pColumn[jj].idf > 0) {
                if (nCountFit > 0) {
                    iFound = 0;
                    for (ll = 0; ll < nCountFit; ll++) {
                        if (pColumn[jj].idf == pColumnFit[ll].id) {
                            if (pColumn[jj].id == pColumnFit[ll].idf) {
                                iFound = 1;
                            }
                            else {
                                
                                for (mm = 0; mm < (nCountFitRaw >> 1); mm++) {
                                    if (m_pDoc->Fit[mm].id == pColumnFit[ll].id) {
                                        m_pDoc->Fit[mm].id = 0;
                                        break;
                                    }
                                }
                                pColumnFit[ll].idf = 0;
                            }
                            break;
                        }
                    }
                    if (iFound == 0) {
                        for (mm = 0; mm < (nCountFitRaw >> 1); mm++) {
                            if (m_pDoc->Fit[mm].id == pColumnFit[ll].id) {
                                m_pDoc->Fit[mm].id = 0;
                                break;
                            }
                        }
                        pColumn[jj].idf = 0;
                    }
                }
                else {
                    for (mm = 0; mm < (nCountFitRaw >> 1); mm++) {
                        if (m_pDoc->Fit[mm].id == pColumnFit[ll].id) {
                            m_pDoc->Fit[mm].id = 0;
                            break;
                        }
                    }
                    pColumn[jj].idf = 0;
                }
            }
        }
        else if ((pColumn[jj].type & 0xF0) == 0x30) {    // Err-X
            iFound = 0;
            for (ll = 0; ll < nCount; ll++) {
                if ((pColumn[jj].id == pColumn[ll].ide) && ((pColumn[ll].type & 0xF0) == 0x10)) {
                    iFound = 1;
                    break;
                }
            }
            if (iFound == 0) {
                // Ce vecteur peut être réutilisé ou détruit
                // ...
            }
        }
        else if ((pColumn[jj].type & 0xF0) == 0x40) {    // Err-Y
            iFound = 0;
            for (ll = 0; ll < nCount; ll++) {
                if ((pColumn[jj].id == pColumn[ll].ide) && ((pColumn[ll].type & 0xF0) == 0x20)) {
                    iFound = 1;
                    break;
                }
            }
            if (iFound == 0) {
                // Ce vecteur peut être réutilisé ou détruit
                // ...
            }
        }
    }

    if (nCountFit > 0) {
        for (jj = 0; jj < nCountFit; jj++) {
            // >> Update column name (important e.g. when column deleted...)
            pColumnFit[jj].name[0] = _T('A') + jj;
            pColumnFit[jj].name[1] = _T('\0');
            // <<
            if ((pColumnFit[jj].type & 0xF0) == 0x10) {    // X
                iFound = 0;
                for (ll = 0; ll < nCountFit; ll++) {
                    if ((pColumnFit[jj].id == pColumnFit[ll].idx) && ((pColumnFit[ll].type & 0xF0) == 0x20)) {
                        iFound = 1;
                        break;
                    }
                }
                if (iFound == 0) {
                    // ...
                }
            }
            else if ((pColumnFit[jj].type & 0xF0) == 0x20) {    // Y
                if (pColumnFit[jj].idx > 0) {
                    iFound = 0;
                    for (ll = 0; ll < nCountFit; ll++) {
                        if ((pColumnFit[jj].idx == pColumnFit[ll].id) && ((pColumnFit[ll].type & 0xF0) == 0x10)) {
                            iFound = 1;
                            break;
                        }
                    }
                    if (iFound == 0) {
                        pColumnFit[jj].idx = 0;
                        pColumnFit[jj].idf = 0;
                        // ...
                    }
                }
                else {
                    pColumnFit[jj].idx = 0;
                    pColumnFit[jj].idf = 0;
                    // ...
                }
                if (pColumnFit[jj].idf > 0) {
                    if (nCount > 0) {
                        iFound = 0;
                        for (ll = 0; ll < nCount; ll++) {
                            if (pColumnFit[jj].idf == pColumn[ll].id) {
                                if (pColumnFit[jj].id == pColumn[ll].idf) {
                                    iFound = 1;
                                }
                                else {
                                    pColumn[ll].idf = 0;
                                }
                                break;
                            }
                        }
                        if (iFound == 0) {
                            pColumnFit[jj].idf = 0;
                            // ...
                        }
                    }
                    else {
                        pColumnFit[jj].idf = 0;
                        // ...
                    }
                }
            }
        }
    }
    //

    // >> [MOD-SG240-002]: AutoX
    if (m_pColumnAutoX->dim > pColumn[0].dim) {
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_rem(m_pColumnAutoX, pColumn[0].dim, m_pColumnAutoX->dim - 1, SigmaApp.m_pLib);
        for (jj = 0; jj < m_pColumnAutoX->dim; jj++) {
            m_pColumnAutoX->dat[jj] = (real_t) jj;
        }
    }
    else if (m_pColumnAutoX->dim < pColumn[0].dim) {
        SigmaApp.m_pLib->errcode = 0;
        for (jj = m_pColumnAutoX->dim; jj < pColumn[0].dim; jj++) {
            ml_vector_add(m_pColumnAutoX, (real_t) jj, SigmaApp.m_pLib);
        }
        for (jj = 0; jj < m_pColumnAutoX->dim; jj++) {
            m_pColumnAutoX->dat[jj] = (real_t) jj;
        }
    }
    if (m_pColumnAutoX->dim != pColumn[0].dim) {
        return;
    }
    // <<

    nDel = 0;
    for (jj = 0; jj < ML_MAXCOLUMNS; jj++) {
        iDelete[jj] = 0L;
    }

    if (m_pPlot->curvecount > 0) {
        vector_t *pvecX = NULL, *pvecY = NULL;
        for (jj = 0; jj < m_pPlot->curvecount; jj++) {
            pvecX = NULL;    pvecY = NULL;
            for (ll = 0; ll < nCount; ll++) {
                if (((pColumn[ll].type & 0xF0) == 0x10) && (m_pPlot->curve[jj].idx == pColumn[ll].id)) {
                    pvecX = &pColumn[ll];
                }
                if (((pColumn[ll].type & 0xF0) == 0x20) && (m_pPlot->curve[jj].idy == pColumn[ll].id)
                    && (m_pPlot->curve[jj].idx == pColumn[ll].idx) && (pColumn[ll].idx > 0)) {
                    pvecY = &pColumn[ll];
                    if (m_pPlot->curve[jj].idx == m_pColumnAutoX->id) {    // [MOD-SG240-002]: AutoX
                        pvecX = m_pColumnAutoX;
                    }
                }
                if ((pvecX) && (pvecY)) {
                    // >> [FIX-SG220-20100328]
                    // Le fit doit utiliser les mêmes axes que la courbe fittée
                    if (pvecY->idf > 0) {
                        for (mm = 0; mm < m_pPlot->curvecount; mm++) {
                            if (m_pPlot->curve[mm].y->id == pvecY->idf) {
                                m_pPlot->curve[mm].axisused = m_pPlot->curve[jj].axisused;
                                break;
                            }
                        }
                    }
                    // <<
                    break;
                }
            }
            if ((pvecX == NULL) || (pvecY == NULL)) {
                if (nCountFit > 0) {
                    for (ll = 0; ll < nCountFit; ll++) {
                        if ((m_pPlot->curve[jj].idx == pColumnFit[ll].id) && ((pColumnFit[ll].type & 0xF0) == 0x10)) {
                            pvecX = &pColumnFit[ll];
                        }
                        if ((m_pPlot->curve[jj].idy == pColumnFit[ll].id) && ((pColumnFit[ll].type & 0xF0) == 0x20)) {
                            pvecY = &pColumnFit[ll];
                        }
                        if ((pvecX) && (pvecY)) {
                            break;
                        }
                    }
                }
            }
            if ((pvecX == NULL) || (pvecY == NULL)) {
                iDelete[nDel] = m_pPlot->curve[jj].idy;
                nDel += 1;
            }
            else {
                m_pPlot->curve[jj].x = pvecX;
                m_pPlot->curve[jj].y = pvecY;
                m_pPlot->curve[jj].points = pvecX->dim;
                m_pPlot->curve[jj].idx = m_pPlot->curve[jj].x->id;
                m_pPlot->curve[jj].idy = m_pPlot->curve[jj].y->id;

                if (m_pPlot->curve[jj].ex != NULL) {
                    iFound = 0;
                    for (ll = 0; ll < nCount; ll++) {
                        if ((m_pPlot->curve[jj].x->ide == pColumn[ll].id) && ((pColumn[ll].type & 0xF0) == 0x30)) {
                            m_pPlot->curve[jj].ex = &pColumn[ll];
                            iFound = 1;
                            break;
                        }
                    }
                    if (iFound == 0) {
                        m_pPlot->curve[jj].ex = NULL;
                    }
                }
                if (m_pPlot->curve[jj].ey != NULL) {
                    iFound = 0;
                    for (ll = 0; ll < nCount; ll++) {
                        if ((m_pPlot->curve[jj].y->ide == pColumn[ll].id) && ((pColumn[ll].type & 0xF0) == 0x40)) {
                            m_pPlot->curve[jj].ey = &pColumn[ll];
                            iFound = 1;
                            break;
                        }
                    }
                    if (iFound == 0) {
                        m_pPlot->curve[jj].ey = NULL;
                    }
                }
            }
        }
    }

    if (nDel > 0) {
        for (jj = 0; jj < nDel; jj++) {
            m_pPlot->errcode = 0;
            pl_curve_removei(m_pPlot, iDelete[jj]);
        }
    }
}

BOOL* CSigmaDoc::GetRunning()
{
    return &m_bRunning;
}

void CSigmaDoc::SetRunning(BOOL bRunning/* = TRUE*/)
{
    m_bRunning = bRunning;
}

int_t CSigmaDoc::GetColumnIndex(int_t nGroup, long_t uiID)
{
    int_t nColumnCount = 0;
    vector_t *pColumn = NULL;

    if (nGroup == 1) {
        nColumnCount = m_pDoc->iDataFitCount;
        pColumn = m_pDoc->ColumnDataFit;
    }
    else if (nGroup == 0) {
        nColumnCount = m_pDoc->iDataCount;
        pColumn = m_pDoc->ColumnData;
    }
    if (pColumn == NULL) {
        return -1;
    }

    if (uiID > 0) {    // Get by ID
        if ((nColumnCount < 1) || (nColumnCount >= SIGMA_MAXCOLUMNS)) {
            return -1;
        }
        for (int_t jj = 0; jj < nColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                return jj;
            }
        }
        return -1;
    }

    return -1;
}

int_t CSigmaDoc::GetColumnID(int_t nGroup, const char_t *pszName)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return 0;
    }

    for (int_t jj = 0; jj < *pColumnCount; jj++) {
        if ((pColumn[jj].opt & 0xF0) == 0x00) {
            if (_tcsicmp((const char_t*)(pColumn[jj].label), pszName) == 0) {
                return (pColumn[jj].id);
            }
        }
        else if ((pColumn[jj].opt & 0xF0) == 0x10) {
            if (_tcsicmp((const char_t*)(pColumn[jj].name), pszName) == 0) {
                return (pColumn[jj].id);
            }
        }
        else if ((pColumn[jj].opt & 0xF0) == 0x20) {
            if (_tcsicmp((const char_t*)(pColumn[jj].name), pszName) == 0) {
                return (pColumn[jj].id);
            }
        }
    }

    return 0;
}

vector_t* CSigmaDoc::GetColumn(int_t nGroup, int_t nCol, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount > SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                return &(pColumn[jj]);
            }
        }
        return NULL;
    }
    else {
        if ((nCol < 0) || (nCol >= *pColumnCount)) {
            return NULL;
        }
        return &(pColumn[nCol]);
    }

    return NULL;
}

vector_t* CSigmaDoc::GetColumnAutoX(void)
{
    return m_pColumnAutoX;
}

vector_t* CSigmaDoc::GetColumns(int_t nGroup, int_t *pColumnCount/* = NULL*/)
{
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumn = m_pDoc->ColumnData;
        if (pColumnCount != NULL) {
            *pColumnCount = m_pDoc->iDataCount;
        }
    }
    else if (nGroup == 1) {
        pColumn = m_pDoc->ColumnDataFit;
        if (pColumnCount != NULL) {
            *pColumnCount = m_pDoc->iDataFitCount;
        }
    }
    if (pColumn == NULL) {
        if (pColumnCount != NULL) {
            *pColumnCount = 0;
        }
        return NULL;
    }

    return pColumn;
}

stat_t* CSigmaDoc::GetStats(int_t nGroup, int_t nCol, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    stat_t *pStats = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pStats = m_pDoc->Stats;
    }

    if ((pColumnCount == NULL) || (pStats == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pStats[jj].id == uiID) {
                return &(pStats[jj]);
            }
        }
        return NULL;
    }
    else {
        if ((nCol < 0) || (nCol >= *pColumnCount)) {
            return NULL;
        }
        return &(pStats[nCol]);
    }

    return NULL;
}

fit_t* CSigmaDoc::GetFit(int_t nCol, long_t uiID/* = 0*/)
{
    if ((m_pDoc->iDataFitCount < 1) || (m_pDoc->iDataFitCount >= SIGMA_MAXCOLUMNS)) {
        return NULL;
    }

    int nCountFitRaw = 0;
    for (int_t ll = 0; ll < m_pDoc->iDataFitCount; ll++) {
        if (m_pDoc->ColumnDataFit[ll].ide < 1) {
            nCountFitRaw += 1;
        }
    }

    if (uiID > 0) {    // Get by ID
        for (int_t jj = 0; jj < (nCountFitRaw >> 1); jj++) {
            if (m_pDoc->Fit[jj].id == uiID) {
                return &(m_pDoc->Fit[jj]);
            }
        }
        return NULL;
    }
    else {
        if ((nCol < 0) || (nCol >= (nCountFitRaw >> 1))) {
            return NULL;
        }
        return &(m_pDoc->Fit[nCol]);
    }

    return NULL;
}

int_t CSigmaDoc::GetFitCount()
{
    return m_pDoc->iDataFitCount;
}

real_t* CSigmaDoc::GetColumnData(int_t nGroup, int_t nCol, long_t uiID/* = 0*/)
{
    CString strTT;

    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                return pColumn[jj].dat;
            }
        }
        return NULL;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return NULL;
    }

    return pColumn[nCol].dat;
}

byte_t* CSigmaDoc::GetColumnMask(int_t nGroup, int_t nCol, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                return pColumn[jj].mask;
            }
        }
        return NULL;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return NULL;
    }

    return pColumn[nCol].mask;
}

BOOL CSigmaDoc::GetColumnNameOrLabel(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                if ((pColumn[jj].opt & 0xF0) == 0x00) {
                    _tcscpy(szBuffer, (const char_t*)(pColumn[jj].label));
                }
                else if ((pColumn[jj].opt & 0xF0) == 0x10) {
                    _tcscpy(szBuffer, (const char_t*)(pColumn[jj].name));
                }
                else if ((pColumn[jj].opt & 0xF0) == 0x20) {
                    _tcscpy(szBuffer, _T("["));
                    _tcscat(szBuffer, (const char_t*)(pColumn[jj].name));
                    _tcscat(szBuffer, _T("] "));
                    _tcscat(szBuffer, (const char_t*)(pColumn[jj].label));
                }
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    if ((pColumn[nCol].opt & 0xF0) == 0x00) {
        _tcscpy(szBuffer, (const char_t*)(pColumn[nCol].label));
    }
    else if ((pColumn[nCol].opt & 0xF0) == 0x10) {
        _tcscpy(szBuffer, (const char_t*)(pColumn[nCol].name));
    }
    else if ((pColumn[nCol].opt & 0xF0) == 0x20) {
        _tcscpy(szBuffer, _T("["));
        _tcscat(szBuffer, (const char_t*)(pColumn[nCol].name));
        _tcscat(szBuffer, _T("] "));
        _tcscat(szBuffer, (const char_t*)(pColumn[nCol].label));
    }

    return TRUE;
}

BOOL CSigmaDoc::GetColumnName(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                _tcscpy(szBuffer, (const char_t*)(pColumn[jj].name));
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    _tcscpy(szBuffer, (const char_t*)(pColumn[nCol].name));
    return TRUE;
}

BOOL CSigmaDoc::GetColumnLabel(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                _tcscpy(szBuffer, (const char_t*)(pColumn[jj].label));
                return TRUE;
            }
        }
        return 0x00;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    _tcscpy(szBuffer, (const char_t*)(pColumn[nCol].label));
    return TRUE;
}

BOOL CSigmaDoc::GetColumnFormat(int_t nGroup, int_t nCol, char_t* szBuffer, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                _tcscpy(szBuffer, (const char_t*)(pColumn[jj].format));
                return TRUE;
            }
        }
        return 0x00;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    _tcscpy(szBuffer, (const char_t*)(pColumn[nCol].format));
    return TRUE;
}

int_t CSigmaDoc::GetColumnDim(int_t nGroup, int_t nCol, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                return pColumn[jj].dim;
            }
        }
        return 0;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return 0;
    }

    return pColumn[nCol].dim;
}

BOOL CSigmaDoc::canViewFit(void)
{
    if (this->GetColumnCount(1) < 1) {
        return FALSE;
    }

    plot_t *pPlot = this->GetPlot();
    if (pPlot == NULL) {
        return FALSE;
    }

    if ((pPlot->curvecount < 1) || (pPlot->curveactive < 0) || (pPlot->curveactive >= pPlot->curvecount)) {
        return FALSE;
    }

    fit_t *pFit = this->GetFit(-1, pPlot->curve[pPlot->curveactive].y->idf);
    if (pFit == NULL) {
        for (int ii = 0; ii < pPlot->curvecount; ii++) {
            pFit = this->GetFit(-1, pPlot->curve[ii].y->idf);
            if (pFit != NULL) {
                return (pFit->parcount >= 2) ? TRUE : FALSE;
            }
        }
    }
    else {
        return (pFit->parcount >= 2) ? TRUE : FALSE;
    }

    return FALSE;
}

int_t CSigmaDoc::GetColumnCount(int_t nGroup)
{
    if (nGroup == 0) {
        return m_pDoc->iDataCount;
    }
    else if (nGroup == 1) {
        return m_pDoc->iDataFitCount;
    }
    
    return 0;
}

real_t CSigmaDoc::GetColumnItem(int_t nGroup, int_t nCol, int_t uiRow, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiRow < 0) {
        return 0.0;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return 0.0;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                if (uiRow >= pColumn[jj].dim) {
                    return 0.0;
                }
                return pColumn[jj].dat[uiRow];
            }
        }
        return 0.0;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return 0.0;
    }

    if (uiRow >= pColumn[nCol].dim) {
        return 0.0;
    }

    return pColumn[nCol].dat[uiRow];
}

byte_t CSigmaDoc::GetColumnItemMask(int_t nGroup, int_t nCol, int_t uiRow, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiRow < 0) {
        return 0x00;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                if (uiRow >= pColumn[jj].dim) {
                    return 0x00;
                }
                return pColumn[jj].mask[uiRow];
            }
        }
        return 0x00;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return 0x00;
    }

    if (uiRow >= pColumn[nCol].dim) {
        return 0x00;
    }

    return pColumn[nCol].mask[uiRow];
}

BOOL CSigmaDoc::SetColumnItem(int_t nGroup, int_t nCol, int_t uiRow, real_t fItem, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiRow < 0) {
        return FALSE;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                if (uiRow >= pColumn[jj].dim) {
                    return FALSE;
                }
                pColumn[jj].dat[uiRow] = fItem;
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    if (uiRow >= pColumn[nCol].dim) {
        return FALSE;
    }

    pColumn[nCol].dat[uiRow] = fItem;
    return TRUE;
}

BOOL CSigmaDoc::SetColumnItemMask(int_t nGroup, int_t nCol, int_t uiRow, byte_t bMask, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiRow < 0) {
        return FALSE;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                if (uiRow >= pColumn[jj].dim) {
                    return FALSE;
                }
                pColumn[jj].mask[uiRow] = bMask;
                if ((bMask & 0x0F) == 0x00) {
                    pColumn[jj].status = 0x00;
                }
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    if (uiRow >= pColumn[nCol].dim) {
        return FALSE;
    }

    pColumn[nCol].mask[uiRow] = bMask;
    if ((bMask & 0x0F) == 0x00) {
        pColumn[nCol].status = 0x00;
    }
    return TRUE;
}

BOOL CSigmaDoc::MaskColumnData(int_t nGroup, byte_t bMask, int_t iMin, int_t iMax, int_t nCol, long_t uiID/* = 0*/)
{
    CDatasheet *pDatasheet = GetDatasheet(nGroup);
    if (pDatasheet == NULL) {
        return FALSE;
    }
    // Lecture seule ?
    if (pDatasheet->IsEditable() == FALSE) {
        return FALSE;
    }

    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return FALSE;
    }

    if (uiID > 0) {
        nCol = GetColumnIndex(nGroup, uiID);
    }

    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    if ((pColumn[nCol].status & 0x0F) == 0x0F) {        // Colonne non encore éditée
        return FALSE;
    }

    if ((pColumn[nCol].dim < 2) || (pColumn[nCol].dim >= ML_MAXPOINTS)) {
        return FALSE;
    }

    if (iMin == -1) {
        iMin = 0;
        iMax = pColumn[nCol].dim - 1;
    }

    if ((iMin < 0) || (iMin >= pColumn[nCol].dim) || (iMax < 0) || (iMax >= pColumn[nCol].dim) || (iMax < iMin)) {
        return FALSE;
    }

    // ... masquer les éléments sélectionnés 
    // (!!! pour chaque colonne X, il faut faire correspondre les colonnes Y avec le mask ad hoc et vice-versa !!!)
    // (Association gérée par le tracé)
    // ...
    byte_t bT;
    int_t ii;
    for (ii = iMin; ii <= iMax; ii++) {
        bT = GetColumnItemMask(nGroup, nCol, ii, 0);
        SetColumnItemMask(nGroup, nCol, ii, (bMask == 0) ? (bT & 0xF0) : ((bT & 0xF0) | 0x01), 0);
    }
    if (iMin > 0) {
        for (ii = 0; ii < iMin; ii++) {
            bT = GetColumnItemMask(nGroup, nCol, ii, 0);
            SetColumnItemMask(nGroup, nCol, ii, (bMask == 0) ? ((bT & 0xF0) | 0x01) : (bT & 0xF0), 0);
        }
    }
    if (iMax < (pColumn[nCol].dim - 1)) {
        for (ii = iMax + 1; ii < pColumn[nCol].dim; ii++) {
            bT = GetColumnItemMask(nGroup, nCol, ii, 0);
            SetColumnItemMask(nGroup, nCol, ii, (bMask == 0) ? ((bT & 0xF0) | 0x01) : (bT & 0xF0), 0);
        }
    }

    UpdateAllViews(NULL);
    SetModifiedFlag(TRUE);

    SaveState();

    return TRUE;
}

BOOL CSigmaDoc::MaskColumnData(int_t nGroup, byte_t bMask, real_t fMinX, real_t fMaxX, int_t nCol, long_t uiID/* = 0*/, byte_t bFlag/* = 0x00*/)
{
    CDatasheet *pDatasheet = GetDatasheet(nGroup);
    if (pDatasheet) {    // Datasheet already created
        // Lecture seule ?
        if (pDatasheet->IsEditable() == FALSE) {
            return FALSE;
        }
    }

    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return FALSE;
    }

    if (uiID > 0) {
        nCol = GetColumnIndex(nGroup, uiID);
    }

    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    if ((pColumn[nCol].status & 0x0F) == 0x0F) {    // Colonne non encore éditée
        return FALSE;
    }

    if ((pColumn[nCol].type & 0xF0) != 0x20) {    // Colonne Y uniquement
        return FALSE;
    }
    if ((pColumn[nCol].dim < 2) || (pColumn[nCol].dim >= ML_MAXPOINTS)) {
        return FALSE;
    }

    if (pColumn[nCol].idx < 1) {    // Colonne X associée
        return FALSE;
    }
    int_t nColX = GetColumnIndex(nGroup, pColumn[nCol].idx);
    if ((nColX < 0) || (nColX >= *pColumnCount)) {
        return FALSE;
    }
    if (pColumn[nColX].dim != pColumn[nCol].dim) {
        return FALSE;
    }

    if ((bFlag & 0xF0) == 0x10) {
        fMinX = pColumn[nColX].dat[0];
    }
    if ((bFlag & 0x0F) == 0x01) {
        fMaxX = pColumn[nColX].dat[pColumn[nColX].dim - 1];
    }

    if ((fMinX < pColumn[nColX].dat[0]) || (fMinX > pColumn[nColX].dat[pColumn[nColX].dim - 1])) {
        return FALSE;
    }
    if ((fMaxX < pColumn[nColX].dat[0]) || (fMaxX > pColumn[nColX].dat[pColumn[nColX].dim - 1]) || (fMaxX < fMinX)) {
        return FALSE;
    }

    // ... masquer les éléments sélectionnés 
    // (!!! pour chaque colonne X, il faut faire correspondre les colonnes Y avec le mask ad hoc et vice-versa !!!)
    // (Association gérée par le tracé)
    // ...
    byte_t bT;
    int_t ii;
    int_t istart = -1, istop = -1;
    for (ii = 0; ii < pColumn[nColX].dim; ii++) {
        if ((pColumn[nColX].dat[ii] >= fMinX) && (pColumn[nColX].dat[ii] <= fMaxX)) {
            bT = GetColumnItemMask(nGroup, nCol, ii, 0);
            SetColumnItemMask(nGroup, nCol, ii, (bMask == 0) ? (bT & 0xF0) : ((bT & 0xF0) | 0x01), 0);
        }

        // Update start/stop (mainly for fitting module)
        if (((pColumn[nCol].mask[ii] & 0x0F) == 0x00) && (istart < 0)) {
            istart = ii;
        }
        else if (((pColumn[nCol].mask[ii] & 0x0F) == 0x01) && (istart >= 0) && (istop < 0)) {
            istop = ii - 1;
        }
    }
    if (istart >= 0) {
        if (istop <= istart) {
            istop = pColumn[nColX].dim - 1;
        }
        pColumn[nCol].istart = istart;
        pColumn[nColX].istart = istart;
        pColumn[nCol].istop = istop;
        pColumn[nColX].istop = istop;
    }
    else {
        pColumn[nCol].istart = 0;
        pColumn[nColX].istart = 0;
        pColumn[nCol].istop = pColumn[nCol].dim - 1;
        pColumn[nColX].istop = pColumn[nColX].dim - 1;
    }

    UpdateAllViews(NULL);
    SetModifiedFlag(TRUE);

    SaveState();

    return TRUE;
}

BOOL CSigmaDoc::SetColumnStatus(int_t nGroup, int_t nCol, byte_t bStatus, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                pColumn[jj].status = bStatus;
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    pColumn[nCol].status = bStatus;
    return TRUE;
}

BOOL CSigmaDoc::SetColumnName(int_t nGroup, int_t nCol, const char_t* szBuffer, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return NULL;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                _tcscpy(pColumn[jj].name, (const char_t*)szBuffer);
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    _tcscpy(pColumn[nCol].name, (const char_t*)szBuffer);
    return TRUE;
}

BOOL CSigmaDoc::SetColumnLabel(int_t nGroup, int_t nCol, const char_t* szBuffer, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return FALSE;
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return FALSE;
        }
        for (int_t jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                _tcscpy(pColumn[jj].label, (const char_t*)szBuffer);
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    _tcscpy(pColumn[nCol].label, (const char_t*)szBuffer);
    return TRUE;
}

BOOL CSigmaDoc::SetColumnFormat(int_t nGroup, int_t nCol, const char_t* szFormat, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;
    int_t jj, ll, nlen;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return FALSE;
    }

    nlen = (int_t)_tcslen(szFormat);
    if ((nlen < 2) || (nlen >= ML_STRSIZET)) {
        return FALSE;
    }
    if (szFormat[0] != _T('%')) {
        return FALSE;
    }
    if ((szFormat[nlen - 1] != _T('f')) && (szFormat[nlen - 1] != _T('e'))
        && (szFormat[nlen - 1] != _T('g')) && (szFormat[nlen - 1] != _T('d'))) {
        return FALSE;
    }
    if (nlen > 2) {
        if ((szFormat[nlen - 2] < _T('0')) || (szFormat[nlen - 2] > _T('9'))) {
            return FALSE;
        }
        for (ll = 1; ll < nlen - 1; ll++) {
            if ((szFormat[ll] != _T('.')) && ((szFormat[ll] < _T('0')) || (szFormat[ll] > _T('9')))) {
                return FALSE;
            }
        }
    }

    if (uiID > 0) {    // Get by ID
        if ((*pColumnCount < 1) || (*pColumnCount >= SIGMA_MAXCOLUMNS)) {
            return FALSE;
        }
        for (jj = 0; jj < *pColumnCount; jj++) {
            if (pColumn[jj].id == uiID) {
                _tcscpy(pColumn[jj].format, szFormat);
                return TRUE;
            }
        }
        return FALSE;
    }

    // Get by index (only used by owned datasheet
    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    _tcscpy(pColumn[nCol].format, (const char_t*)szFormat);
    return TRUE;
}

// Functions only accessible to the attached datasheet (then use index and not ID)

BOOL CSigmaDoc::SortColumns(int_t nGroup, int_t nPivot, byte_t bAsc, byte_t *bLink)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return FALSE;
    }

    if ((nPivot < 0) || (nPivot >= *pColumnCount)) {
        return FALSE;
    }

    // [FIX-SG244-007]: ignore invalid items (not yet edited for example)
    int_t ii, jj, iStart = 0, iStop = pColumn[nPivot].dim - 1;
    if (pColumn[nPivot].mask[iStart] == 0xFF) {
        for (ii = iStart; ii < iStop; ii++) {
            if (pColumn[nPivot].mask[ii] != 0xFF) {
                iStart = ii;
                break;
            }
        }
    }
    if (pColumn[nPivot].mask[iStop] == 0xFF) {
        for (ii = iStop; ii > iStart; ii--) {
            if (pColumn[nPivot].mask[ii] != 0xFF) {
                iStop = ii;
                break;
            }
        }
    }
    // [FIX-SG244-008]: When sorting X-Column, sort also the columns that use this X-Column
    byte_t ilink[ML_MAXCOLUMNS];
    memset(ilink, 0, ML_MAXCOLUMNS * sizeof(byte_t));
    if (bLink == NULL) {
        bLink = ilink;
    }
    if ((pColumn[nPivot].type & 0xF0) == 0x10) {
        for (jj = 0; jj < *pColumnCount; jj++) {
            if (((pColumn[jj].type & 0xF0) == 0x20) && (pColumn[jj].idx == pColumn[nPivot].id)) {
                bLink[jj] = 1;
            }
        }
    }
    //

    SigmaApp.m_pLib->errcode = 0;
    ml_vector_sort(pColumn, *pColumnCount, nPivot, bAsc, bLink, iStart, iStop, SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode == 0) {
        Logi(_T("Column sorting done.\r\n"));
        return TRUE;
    }

    CString strT = _T("Column sorting error: ");
    strT += (LPCTSTR) (SigmaApp.m_pLib->message);
    strT += _T("\r\n");
    Logi(strT);
    return FALSE;
}

BOOL CSigmaDoc::DeleteColumn(int_t nGroup, int_t nCol, long_t uiID/* = 0*/)
{
    int_t *pColumnCount = NULL, jj, ll;
    vector_t *pColumn = NULL, *pColumnX = NULL, *pColumnY = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return FALSE;
    }

    if (uiID > 0) {
        nCol = GetColumnIndex(nGroup, uiID);
    }

    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return FALSE;
    }

    if (uiID <= 0) {
        uiID = pColumn[nCol].id;
    }

    int nCountFitRaw = 0;
    for (int_t ll = 0; ll < m_pDoc->iDataFitCount; ll++) {
        if (m_pDoc->ColumnDataFit[ll].ide < 1) {
            nCountFitRaw += 1;
        }
    }

    bool bMP = ((nGroup == 1) && (pColumn[nCol].ide > 0));

    // Si la colonne est un fit, il faut mettre à jour la colonne de données
    // et supprimer le fit
    if ((nGroup == 1) && ((pColumn[nCol].type & 0xF0) == 0x20) && (bMP == false)) {
        for (jj = 0; jj < m_pDoc->iDataCount; jj++) {
            pColumnY = GetColumn(0, jj);
            if (pColumnY) {
                if (pColumnY->idf == uiID) {
                    pColumnY->idf = 0;
                    break;
                }
            }
        }

        bool bDelX = true;
        if (bMP == false) {
            for (jj = 0; jj < m_pDoc->iDataFitCount; jj++) {
                pColumnY = GetColumn(1, jj);
                if (pColumnY) {
                    if (pColumnY->ide == uiID) {
                        pColumnY->ide = 0;
                        pColumnY->idf = 0;
                        bDelX = false;
                    }
                }
            }
        }
        else {
            bDelX = false;
        }

        // Mettre à jour les données du fit
        if ((bMP == false) && (nCol < (m_pDoc->iDataFitCount - 1))) {
            for (jj = 0; jj < ((nCountFitRaw >> 1) - 1); jj++) {
                if (m_pDoc->Fit[jj].id == uiID) {
                    for (ll = jj; ll < ((nCountFitRaw >> 1) - 1); ll++) {
                        memcpy(&(m_pDoc->Fit[ll]), &(m_pDoc->Fit[ll + 1]), sizeof(fit_t));
                    }
                }
            }
        }

        // Supprimer la colonne X du fit
        if (bDelX) {
            pColumnX = GetColumn(1, -1, pColumn[nCol].idx);
            if (pColumnX) {
                int_t nColX = GetColumnIndex(1, pColumn[nCol].idx);
                if (nColX < (m_pDoc->iDataFitCount - 1)) {
                    for (jj = nColX; jj < m_pDoc->iDataFitCount - 1; jj++) {
                        SigmaApp.m_pLib->errcode = 0;
                        ml_vector_dup(&(m_pDoc->ColumnDataFit[jj]), (const vector_t*) (&(m_pDoc->ColumnDataFit[jj + 1])), SigmaApp.m_pLib);
                        if (SigmaApp.m_pLib->errcode != 0) {
                            return FALSE;
                        }
                    }
                }
                SigmaApp.m_pLib->errcode = 0;
                ml_vector_delete(&(m_pDoc->ColumnDataFit[m_pDoc->iDataFitCount - 1]), SigmaApp.m_pLib);
                if (SigmaApp.m_pLib->errcode == 0) {
                    m_pDoc->iDataFitCount -= 1;
                }
            }
        }

        // Supprimer la colonne Y du fit
        pColumnY = GetColumn(1, -1, uiID);
        if (pColumnY) {
            int_t nColY = GetColumnIndex(1, uiID);
            if (nColY < (m_pDoc->iDataFitCount - 1)) {
                for (jj = nColY; jj < m_pDoc->iDataFitCount - 1; jj++) {
                    SigmaApp.m_pLib->errcode = 0;
                    ml_vector_dup(&(m_pDoc->ColumnDataFit[jj]), (const vector_t*)(&(m_pDoc->ColumnDataFit[jj + 1])), SigmaApp.m_pLib);
                    if (SigmaApp.m_pLib->errcode != 0) {
                        return FALSE;
                    }
                }
            }
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_delete(&(m_pDoc->ColumnDataFit[m_pDoc->iDataFitCount - 1]), SigmaApp.m_pLib);
            if (SigmaApp.m_pLib->errcode == 0) {
                m_pDoc->iDataFitCount -= 1;
            }
        }

        if (SigmaApp.m_pLib->errcode == 0) {
            UpdateSigmaDoc();
            UpdateAllViews(NULL);
            return TRUE;
        }
    }
    else if ((nGroup == 1) && ((pColumn[nCol].type & 0xF0) == 0x10)) {
        long_t uiIDy = 0;
        int_t iColy = 0;
        for (jj = 0; jj < m_pDoc->iDataFitCount; jj++) {
            if (m_pDoc->ColumnDataFit[jj].idx == uiID) {
                uiIDy = m_pDoc->ColumnDataFit[jj].id;
                iColy = jj;
                break;
            }
        }
        if (uiIDy <= 0) {
            return FALSE;
        }

        // if X used by more than one Y-Column, then do not delete it
        bool bDelX = true;
        for (jj = 0; jj < m_pDoc->iDataFitCount; jj++) {
            pColumnY = GetColumn(1, jj);
            if (pColumnY) {
                if (pColumnY->ide == uiIDy) {
                    bDelX = false;
                    break;
                }
            }
        }
        if (bDelX == false) {
            return FALSE;
        }

        for (jj = 0; jj < m_pDoc->iDataCount; jj++) {
            pColumnY = GetColumn(0, jj);
            if (pColumnY) {
                if (pColumnY->idf == uiIDy) {
                    pColumnY->idf = 0;
                    break;
                }
            }
        }

        if (nCol < (m_pDoc->iDataFitCount - 1)) {
            for (jj = 0; jj < ((nCountFitRaw >> 1) - 1); jj++) {
                if (m_pDoc->Fit[jj].id == uiIDy) {
                    for (ll = jj; ll < ((nCountFitRaw >> 1) - 1); ll++) {
                        memcpy(&(m_pDoc->Fit[ll]), &(m_pDoc->Fit[ll + 1]), sizeof(fit_t));
                    }
                }
            }
        }

        // Supprimer la colonne Y du fit
        pColumnY = GetColumn(1, -1, uiIDy);
        if (pColumnY) {
            if (iColy < (m_pDoc->iDataFitCount - 1)) {
                for (jj = iColy; jj < m_pDoc->iDataFitCount - 1; jj++) {
                    SigmaApp.m_pLib->errcode = 0;
                    ml_vector_dup(&(m_pDoc->ColumnDataFit[jj]), (const vector_t*)(&(m_pDoc->ColumnDataFit[jj + 1])), SigmaApp.m_pLib);
                    if (SigmaApp.m_pLib->errcode != 0) {
                        return FALSE;
                    }
                }
            }
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_delete(&(m_pDoc->ColumnDataFit[m_pDoc->iDataFitCount - 1]), SigmaApp.m_pLib);
            if (SigmaApp.m_pLib->errcode == 0) {
                m_pDoc->iDataFitCount -= 1;
            }
        }

        // Supprimer la colonne X du fit
        int_t nColX = GetColumnIndex(1, uiID);
        if (nColX < (m_pDoc->iDataFitCount - 1)) {
            for (jj = nColX; jj < m_pDoc->iDataFitCount - 1; jj++) {
                SigmaApp.m_pLib->errcode = 0;
                ml_vector_dup(&(m_pDoc->ColumnDataFit[jj]), (const vector_t*)(&(m_pDoc->ColumnDataFit[jj + 1])), SigmaApp.m_pLib);
                if (SigmaApp.m_pLib->errcode != 0) {
                    return FALSE;
                }
            }
        }
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_delete(&(m_pDoc->ColumnDataFit[m_pDoc->iDataFitCount - 1]), SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            m_pDoc->iDataFitCount -= 1;
        }

        if (SigmaApp.m_pLib->errcode == 0) {
            UpdateSigmaDoc();
            UpdateAllViews(NULL);
            return TRUE;
        }
    }
    // Si la colonne a un fit, il faut le supprimer
    else if ((nGroup == 0) && ((pColumn[nCol].type & 0xF0) == 0x20) && (pColumn[nCol].idf > 0)) {
        long_t uiIDf = pColumn[nCol].idf;
        pColumnY = GetColumn(1, -1, uiIDf);
        pColumn[nCol].idf = 0;

        // Supprimer le fit
        if (pColumnY) {

            int_t nColY = GetColumnIndex(1, pColumnY->id);
            pColumnX = GetColumn(1, -1, pColumnY->idx);

            if (nColY < (m_pDoc->iDataFitCount - 1)) {
                for (jj = 0; jj < ((nCountFitRaw >> 1) - 1); jj++) {
                    if (m_pDoc->Fit[jj].id == uiIDf) {
                        for (ll = jj; ll < ((nCountFitRaw >> 1) - 1); ll++) {
                            memcpy(&(m_pDoc->Fit[ll]), &(m_pDoc->Fit[ll + 1]), sizeof(fit_t));
                        }
                    }
                }
            }

            // Supprimer la colonne Y du fit
            if (nColY < (m_pDoc->iDataFitCount - 1)) {
                for (jj = nColY; jj < m_pDoc->iDataFitCount - 1; jj++) {
                    SigmaApp.m_pLib->errcode = 0;
                    ml_vector_dup(&(m_pDoc->ColumnDataFit[jj]), (const vector_t*)(&(m_pDoc->ColumnDataFit[jj + 1])), SigmaApp.m_pLib);
                    if (SigmaApp.m_pLib->errcode != 0) {
                        return FALSE;
                    }
                }
            }
            SigmaApp.m_pLib->errcode = 0;
            ml_vector_delete(&(m_pDoc->ColumnDataFit[m_pDoc->iDataFitCount - 1]), SigmaApp.m_pLib);
            if (SigmaApp.m_pLib->errcode == 0) {
                m_pDoc->iDataFitCount -= 1;
            }

            // Supprimer la colonne X du fit
            if (pColumnX) {
                int_t nColX = GetColumnIndex(1, pColumnX->id);
                if (nColX < (m_pDoc->iDataFitCount - 1)) {
                    for (jj = nColX; jj < m_pDoc->iDataFitCount - 1; jj++) {
                        SigmaApp.m_pLib->errcode = 0;
                        ml_vector_dup(&(m_pDoc->ColumnDataFit[jj]), (const vector_t*)(&(m_pDoc->ColumnDataFit[jj + 1])), SigmaApp.m_pLib);
                        if (SigmaApp.m_pLib->errcode != 0) {
                            return FALSE;
                        }
                    }
                }
                SigmaApp.m_pLib->errcode = 0;
                ml_vector_delete(&(m_pDoc->ColumnDataFit[m_pDoc->iDataFitCount - 1]), SigmaApp.m_pLib);
                if (SigmaApp.m_pLib->errcode == 0) {
                    m_pDoc->iDataFitCount -= 1;
                }
            }
        }

        // Supprimer la colonne
        if (nCol < (m_pDoc->iDataCount - 1)) {
            for (jj = nCol; jj < m_pDoc->iDataCount - 1; jj++) {
                SigmaApp.m_pLib->errcode = 0;
                ml_vector_dup(&(m_pDoc->ColumnData[jj]), (const vector_t*)(&(m_pDoc->ColumnData[jj + 1])), SigmaApp.m_pLib);
                if (SigmaApp.m_pLib->errcode != 0) {
                    return FALSE;
                }
            }
        }
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_delete(&(m_pDoc->ColumnData[m_pDoc->iDataCount - 1]), SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            m_pDoc->iDataCount -= 1;
            UpdateSigmaDoc();
            UpdateAllViews(NULL);
            Logi(_T("Column deleted\r\n"));
            SigmaApp.OutputSB(_T("Column deleted"), 0, TRUE);
            return TRUE;
        }
    }
    else {
        if (nCol < ((*pColumnCount) - 1)) {
            for (jj = nCol; jj < *pColumnCount - 1; jj++) {
                SigmaApp.m_pLib->errcode = 0;
                ml_vector_dup(&pColumn[jj], (const vector_t*)(&pColumn[jj + 1]), SigmaApp.m_pLib);
                if (SigmaApp.m_pLib->errcode != 0) {
                    return FALSE;
                }
            }
        }
        SigmaApp.m_pLib->errcode = 0;
        bool isY = ((pColumn[*pColumnCount - 1].type & 0xF0) == 0x20);
        ml_vector_delete(&pColumn[*pColumnCount - 1], SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            *pColumnCount -= 1;
            UpdateSigmaDoc();
            UpdateAllViews(NULL);
            Logi(_T("Column deleted\r\n"));
            SigmaApp.OutputSB(_T("Column deleted"), 0, TRUE);
            return TRUE;
        }
    }

    UpdateSigmaDoc();
    UpdateAllViews(NULL);
    return FALSE;
}

vector_t* CSigmaDoc::InsertColumn(int_t nGroup, int_t nCol, int_t nColumnDim /* = 0*/)
{
    int_t *pColumnCount = NULL, ii, jj;
    vector_t *pColumn = NULL, *pT = NULL;
    char_t szName[16];

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return NULL;
    }

    if (m_pColumnAutoX->id == m_pDoc->iColumnID) {
        m_pDoc->iColumnID += 1;
    }

    // Create the first column...
    if ((nCol < 0) && (*pColumnCount == 0)) {
        if ((nColumnDim < 1) || (nColumnDim >= ML_MAXPOINTS)) {
            return NULL;
        }
        SigmaApp.m_pLib->errcode = 0;
        memset(&(pColumn[*pColumnCount]), 0, sizeof(vector_t));
        szName[0] = _T('A') + *pColumnCount;
        szName[1] = _T('\0');
        ml_vector_create(&(pColumn[*pColumnCount]), nColumnDim, m_pDoc->iColumnID, szName, NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            for (ii = 0; ii < pColumn[*pColumnCount].dim; ii++) {
                pColumn[*pColumnCount].dat[ii] = 0.0;
                pColumn[*pColumnCount].mask[ii] = 0xFF;    // Invalid item (to be edited)
            }
            _tcscpy(pColumn[*pColumnCount].label, (const char_t*)(pColumn[*pColumnCount].name));
            pColumn[*pColumnCount].opt = 0x00;
            pColumn[*pColumnCount].status = 0xFF; // La colonne ne fait pas encore partie du tracé
            *pColumnCount += 1;
            m_pDoc->iColumnID += 1;
            return &(pColumn[*pColumnCount - 1]);
        }
        return NULL;
    }

    if ((nCol < 0) || (nCol >= *pColumnCount)) {
        return NULL;
    }

    if (*pColumnCount >= SIGMA_MAXCOLUMNS) {
        return NULL;
    }

    // Append...
    if (nCol == (*pColumnCount - 1)) {
        SigmaApp.m_pLib->errcode = 0;
        memset(&(pColumn[*pColumnCount]), 0, sizeof(vector_t));
        szName[0] = _T('A') + *pColumnCount;
        szName[1] = _T('\0');
        ml_vector_create(&(pColumn[*pColumnCount]), pColumn[0].dim, m_pDoc->iColumnID, szName, NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode == 0) {
            for (ii = 0; ii < pColumn[nCol].dim; ii++) {
                pColumn[*pColumnCount].dat[ii] = 0.0;
                pColumn[*pColumnCount].mask[ii] = 0xFF;    // Invalid item (to be edited)
            }
            _tcscpy(pColumn[*pColumnCount].label, (const char_t*)(pColumn[*pColumnCount].name));
            pColumn[*pColumnCount].opt = pColumn[nCol].opt;
            pColumn[*pColumnCount].status = 0xFF; // La colonne ne fait pas encore partie du tracé
            _tcscpy(pColumn[*pColumnCount].format, (const char_t*)(pColumn[*pColumnCount - 1].format));
            *pColumnCount += 1;
            m_pDoc->iColumnID += 1;

            Logi(_T("Column created\r\n"));
            SigmaApp.OutputSB(_T("Column created"), 0, TRUE);

            return &(pColumn[*pColumnCount - 1]);
        }
        return NULL;
    }

    // Insert...
    memset(&pColumn[*pColumnCount], 0, sizeof(vector_t));
    szName[0] = _T('A') + *pColumnCount;
    szName[1] = _T('\0');
    SigmaApp.m_pLib->errcode = 0;
    ml_vector_create(&(pColumn[*pColumnCount]), pColumn[0].dim, m_pDoc->iColumnID, szName, NULL, SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode == 0) {
        *pColumnCount += 1;
        m_pDoc->iColumnID += 1;
        if (*pColumnCount > 2) {
            for (jj = *pColumnCount - 2; jj > nCol; jj--) { // TODEBUG
                ml_vector_dup(&(pColumn[jj + 1]), (const vector_t*)(&(pColumn[jj])), SigmaApp.m_pLib);
                pColumn[jj + 1].name[0] = _T('A') + jj + 1;
                pColumn[jj + 1].name[1] = _T('\0');
                if (SigmaApp.m_pLib->errcode != 0) {
                    CString strT = (LPCTSTR) (this->GetDoc()->szName);
                    strT += _T(": ");
                    strT += (LPCTSTR) (SigmaApp.m_pLib->message);
                    strT += _T("\r\n");
                    Logi((LPCTSTR) strT);
                    UpdateSigmaDoc();
                    return NULL;
                }
            }
        }

        for (ii = 0; ii < pColumn[nCol].dim; ii++) {
            pColumn[nCol + 1].dat[ii] = 0.0;
            pColumn[nCol + 1].mask[ii] = 0xFF;    // Invalid item (to be edited)
        }
        pColumn[nCol + 1].status = 0xFF;            // La colonne ne fait pas encore partie du tracé
        pColumn[nCol + 1].id = m_pDoc->iColumnID - 1;
        pColumn[nCol + 1].name[0] = _T('A') + nCol + 1;
        pColumn[nCol + 1].name[1] = _T('\0');
        _tcscpy(pColumn[nCol + 1].label, (const char_t*)(pColumn[nCol + 1].name));
        pColumn[nCol + 1].opt = pColumn[nCol].opt;

        UpdateSigmaDoc();

        Logi(_T("Column created\r\n"));
        SigmaApp.OutputSB(_T("Column created"), 0, TRUE);

        return &(pColumn[nCol + 1]);
    }

    UpdateSigmaDoc();
    return NULL;
}

int_t CSigmaDoc::DeleteRow(int_t nGroup, int_t uiRow)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return -1;
    }

    if (*pColumnCount < 1) {
        return -1;
    }

    if ((uiRow < 0) || (uiRow >= pColumn[0].dim)) {
        return -1;
    }

    SigmaApp.m_pLib->errcode = 0;
    for (int_t jj = 0; jj < *pColumnCount; jj++) {
        ml_vector_rem(&pColumn[jj], uiRow, uiRow, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            return -1;
        }
    }

    // >> [MOD-SG240-002]: AutoX
    ml_vector_rem(m_pColumnAutoX, m_pColumnAutoX->dim - 1, m_pColumnAutoX->dim - 1, SigmaApp.m_pLib);
    // <<

    if (SigmaApp.m_pLib->errcode != 0) {
        return -1;
    }

    return uiRow;
}

int_t CSigmaDoc::InsertRow(int_t nGroup, int_t uiRow)
{
    int_t *pColumnCount = NULL;
    vector_t *pColumn = NULL;

    if (nGroup == 0) {
        pColumnCount = &(m_pDoc->iDataCount);
        pColumn = m_pDoc->ColumnData;
    }
    else if (nGroup == 1) {
        pColumnCount = &(m_pDoc->iDataFitCount);
        pColumn = m_pDoc->ColumnDataFit;
    }
    if ((pColumnCount == NULL) || (pColumn == NULL)) {
        return -1;
    }

    if (*pColumnCount < 1) {
        return -1;
    }

    if ((uiRow < 0) || (uiRow >= pColumn[0].dim)) {
        return -1;
    }

    SigmaApp.m_pLib->errcode = 0;
    for (int_t jj = 0; jj < *pColumnCount; jj++) {
        ml_vector_insert(&(pColumn[jj]), uiRow, 0.0, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            return -1;
        }
        pColumn[jj].mask[uiRow + 1] = 0xFF;    // Invalid item (to be edited)
    }

    // >> [MOD-SG240-002]: AutoX
    ml_vector_insert(m_pColumnAutoX, m_pColumnAutoX->dim - 1, (real_t) (m_pColumnAutoX->dim), SigmaApp.m_pLib);
    if (SigmaApp.m_pLib->errcode != 0) {
        return -1;
    }
    m_pColumnAutoX->mask[m_pColumnAutoX->dim - 1] = 0x00;
    // <<

    return (uiRow + 1);
}

int_t CSigmaDoc::CurveAdd(int_t nGroup, int_t idx, int_t idy, byte_t linestyle, byte_t dotstyle,
                                  byte_t axisused/* = 0x00*/, BOOL bUpdate/* = TRUE*/)
{
    int_t nColumnCount = 0;
    vector_t *pColumn = GetColumns(0, &nColumnCount);
    if ((pColumn == NULL) || (nColumnCount < 1) || (nColumnCount > SIGMA_MAXCOLUMNS)) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot add curve: invalid columns."));
        return -1;
    }

    int_t ix = GetColumnIndex(nGroup, idx);
    int_t iy = GetColumnIndex(nGroup, idy);

    if ((iy < 0) || (iy >= nColumnCount) || (ix < 0) || (ix >= nColumnCount)) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot add curve: invalid columns."));
        return -1;
    }

    // Show the plot window before showing the 'add curve' dialog
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return -1;
    }
    if (pFrame->CreateSigmaView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot), TRUE) == FALSE) {
        return -1;
    }

    int_t iCurveCount = m_pPlot->curvecount;

    if ((iCurveCount < 0) || (iCurveCount >= ML_MAXCURVES)) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot add curve: invalid curves count."));
        return -1;
    }

    pColumn[ix].status = 0x00;
    pColumn[iy].status = 0x00;
    pColumn[iy].idx = pColumn[ix].id;

    // [FIX-SG244-007]: ignore invalid items (not yet edited for example)
    int_t ii, jj, iStart = 0, iStop = pColumn[ix].dim - 1;
    if (pColumn[ix].mask[iStart] == 0xFF) {
        for (ii = iStart; ii < iStop; ii++) {
            if ((pColumn[ix].mask[ii] & 0x0F) == 0x00) {
                iStart = ii;
                break;
            }
        }
    }
    if (pColumn[ix].mask[iStop] == 0xFF) {
        for (ii = iStop; ii > iStart; ii--) {
            if ((pColumn[ix].mask[ii] & 0x0F) == 0x00) {
                iStop = ii;
                break;
            }
        }
    }
    //
    // Ordonner la colonne X si nécessaire
    if (pColumn[ix].dat[0] > pColumn[ix].dat[pColumn[ix].dim - 1]) {
        byte_t ilink[ML_MAXCOLUMNS];
        memset(ilink, 0, ML_MAXCOLUMNS * sizeof(byte_t));
        // [FIX-SG244-008]: Sort columns that use the X-Column
        for (jj = 0; jj < nColumnCount; jj++) {
            if (((pColumn[jj].type & 0xF0) == 0x20) && (pColumn[jj].idx == pColumn[ix].id)) {
                ilink[jj] = 1;
            }
        }
        //
        ilink[iy] = 1;
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_sort(pColumn, nColumnCount, ix, 1, ilink, iStart, iStop, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            _tcscpy(SigmaApp.m_pLib->message, _T("Cannot add curve: cannot sort columns."));
            return -1;
        }
    }

    m_pPlot->errcode = 0;
    int_t iCurve = pl_curve_add(m_pPlot, &(pColumn[ix]), &(pColumn[iy]), NULL, NULL, axisused, 0x01);
    if (iCurveCount == 0) {
        pl_autoscale(m_pPlot);
    }

    if ((m_pPlot->errcode != 0) || (iCurve < 0) || (iCurve >= ML_MAXCURVES)) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot add curve: plot internal error."));
        return -1;
    }

    m_pPlot->curve[iCurve].linestyle = linestyle;
    m_pPlot->curve[iCurve].dotstyle = dotstyle;
    m_pPlot->curve[iCurve].axisused = axisused;

    if (bUpdate) {
        UpdateAllViews(NULL);
    }

    SetModifiedFlag(TRUE);

    _tcscpy(SigmaApp.m_pLib->message, _T("Done."));

    Logi(_T("Curve added to plot\r\n"));

    SaveState();

    return iCurveCount;
}

int_t CSigmaDoc::CurveRemove(int_t nGroup, int_t idy, const char_t *pszLegend/* = NULL*/,
                                      BOOL bUpdate/* = TRUE*/)
{
    if ((idy < 1) && (pszLegend == NULL)) {
        return -1;
    }

    if ((m_pPlot->curvecount < 1) || (m_pPlot->curvecount > SIGMA_MAXCURVES)) {
        return -1;
    }

    int_t nColumnCount = GetColumnCount(0);
    if ((nColumnCount < 1) || (nColumnCount > SIGMA_MAXCOLUMNS)) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot remove curve: invalid columns count."));
        return -1;
    }

    int_t nCurve = -1, jj;
    if (idy > 0) {        // Search by ID
        for (jj = 0; jj < m_pPlot->curvecount; jj++) {
            if (m_pPlot->curve[jj].idy == idy) {
                nCurve = jj;
                break;
            }
        }
    }
    else {    // Search by name
        for (jj = 0; jj < m_pPlot->curvecount; jj++) {
            if (_tcsicmp(pszLegend, (const char_t*)(m_pPlot->curve[jj].legend.text)) == 0) {
                idy = m_pPlot->curve[jj].idy;
                nCurve = jj;
                break;
            }
        }
    }
    if ((nCurve < 0) || (nCurve >= m_pPlot->curvecount)) {
        return -1;
    }

    int_t iy = GetColumnIndex(nGroup, idy);

    if ((iy < 0) || (iy >= nColumnCount)) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot remove curve: invalid columns."));
        return -1;
    }

    vector_t *pVec = GetColumn(nGroup, -1, idy);
    if (pVec == NULL) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot remove curve: invalid column."));
        return -1;
    }
    int_t idf = pVec->idf;

    // Ne pas supprimer s'il s'agit d'un fit
    if (GetColumn(1, -1, m_pPlot->curve[nCurve].y->id) != NULL) {
        return -1;
    }

    m_pPlot->errcode = 0;
    pl_curve_remove(m_pPlot, nCurve);
    if (m_pPlot->errcode != 0) {
        _tcscpy(SigmaApp.m_pLib->message, _T("Cannot add curve: plot internal error."));
        return -1;
    }

    if (m_pPlot->curveactive == nCurve) {
        m_pPlot->curveactive = 0;
    }

    // >> Supprimer aussi le fit
    if ((m_pPlot->curvecount) > 0 && (idf > 0)) {
        for (jj = 0; jj < m_pPlot->curvecount; jj++) {
            if (m_pPlot->curve[jj].idy == idf) {
                nCurve = jj;
                break;
            }
        }
        pl_curve_remove(m_pPlot, nCurve);
        if (m_pPlot->errcode != 0) {
            _tcscpy(SigmaApp.m_pLib->message, _T("Cannot add curve: plot internal error."));
            return -1;
        }
    }
    // <<

    if (m_pPlot->curveactive == nCurve) {
        m_pPlot->curveactive = 0;
    }

    if (bUpdate) {
        ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
    }

    Logi(_T("Curve removed\r\n"));
    SigmaApp.OutputSB(_T("Curve removed"), 0, TRUE);

    SaveState();

    return (m_pPlot->curveactive);
}

int_t CSigmaDoc::CurveIndex(const char_t *pszLegend)
{
    if (pszLegend == NULL) {
        return -1;
    }

    if ((m_pPlot->curvecount < 1) || (m_pPlot->curvecount > SIGMA_MAXCURVES)) {
        return -1;
    }

    int_t nCurve = -1, jj;
    for (jj = 0; jj < m_pPlot->curvecount; jj++) {
        if (_tcsicmp(pszLegend, (const char_t*)(m_pPlot->curve[jj].legend.text)) == 0) {
            nCurve = jj;
            break;
        }
    }

    return nCurve;
}

vector_t* CSigmaDoc::AppendFitColumn(vector_t *pColumnFitY, fit_t *pFit, int_t iPos)
{
    vector_t* pColumn = NULL;

    if ((pColumnFitY == NULL) || (pFit == NULL) || (iPos < 0) || (iPos >= 5)) {
        return NULL;
    }

    int ifun = pFit->index - 1;
    bool bPeak = ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5));
    bool bPeakM = bPeak && (ifun != FIT_MODEL_GAUSS1) && (ifun != FIT_MODEL_LORENTZ1);

    if (bPeakM == false) {
        return NULL;
    }
    int_t iFitColumnCount = 0;
    if ((ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_LORENTZ2)) {
        iFitColumnCount = 2;
    }
    else if ((ifun == FIT_MODEL_GAUSS3) || (ifun == FIT_MODEL_LORENTZ3)) {
        iFitColumnCount = 3;
    }
    else if ((ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_LORENTZ4)) {
        iFitColumnCount = 4;
    }
    else if ((ifun == FIT_MODEL_GAUSS5) || (ifun == FIT_MODEL_LORENTZ5)) {
        iFitColumnCount = 5;
    }
    if (iPos >= iFitColumnCount) {
        return NULL;
    }

    int_t nColCount = GetColumnCount(1);
    if ((nColCount < 2) || (nColCount > ML_MAXCOLUMNS)) {
        return NULL;
    }
    
    int_t nCol = nColCount - 1, jj, iFound = 0, tIndex[ML_MAXCOLUMNS];
    bool bFound = false;

    memset(tIndex, 0, ML_MAXCOLUMNS * sizeof(int_t));

    for (jj = 0; jj < nColCount; jj++) {
        if (pColumnFitY->id == m_pDoc->ColumnDataFit[jj].id) {
            bFound = true;
            break;
        }
    }
    if (bFound == false) {
        return NULL;
    }

    for (jj = 0; jj < nColCount; jj++) {
        if (m_pDoc->ColumnDataFit[jj].ide == pColumnFitY->id) {
            tIndex[iFound] = jj;
            iFound += 1;
        }
    }
    if ((iFound == iFitColumnCount) || (iPos < iFound)) {
        return &(m_pDoc->ColumnDataFit[tIndex[iPos]]);
    }

    if ((pColumn = InsertColumn(1, nCol)) != NULL) {

        pColumn->ide = pColumnFitY->id;

        CSigmaViewDataFit* pView = (CSigmaViewDataFit*) (GetDataFitView());

        if (pView == NULL) {
            CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
            ASSERT(pFrame != NULL);
            if (pFrame == NULL) {
                return NULL;
            }
            if (pFrame->CreateSigmaView(SigmaApp.m_pTemplateDataFit, RUNTIME_CLASS(CSigmaViewDataFit), TRUE) == FALSE) {
                return NULL;
            }
            pView = (CSigmaViewDataFit*) (GetDataFitView());
            if (pView == NULL) {
                return NULL;
            }
        }

        CDatasheet *pDatasheet = (CDatasheet*) (pView->GetDatasheet());
        pDatasheet->SetColumnCount(1 + GetColumnCount(1));
        nColCount = GetColumnCount(1);
        if (nColCount >= 2) {
            pDatasheet->SetColumnWidth(nColCount, pDatasheet->GetColumnWidth(nColCount - 1));
        }
        else if (nColCount >= 1) {
            pDatasheet->SetColumnWidth(nColCount, pDatasheet->GetColumnWidth(0));
        }

        return pColumn;
    }

    return NULL;
}

void CSigmaDoc::DoDataImport(vector_t *pColumnT, int_t *piColumnT, int_t iDataCount)
{
    int_t jj;

    CSigmaViewData* pView = (CSigmaViewData*) (GetDataView());
    if (pView == NULL) {
        return;
    }
    CDatasheet *pDatasheet = (CDatasheet*) (pView->GetDatasheet());
    if (pDatasheet == NULL) {
        return;
    }

    int_t iColumnT[ML_MAXCOLUMNS];
    for (jj = 0; jj < ML_MAXCOLUMNS; jj++) {
        iColumnT[jj] = 1;
    }
    if (piColumnT == NULL) {
        piColumnT = iColumnT;
    }

    int ic = 0, iColWidth = this->GetDatasheet()->GetColumnWidth(), iColCount = m_pDoc->iDataCount;

    // :DEBUG: modifier cette partie pour ne prendre en compte que les colonnes sélectionnées...
    // Si le datasheet est nouveau le détruire, sinon append
    if ((m_pDoc->iDataCount == 2) && (m_pDoc->ColumnData[0].status == 0xFF) && (m_pDoc->ColumnData[1].status == 0xFF)) {
        SigmaApp.m_pLib->errcode = 0;
        for (jj = 0; jj < iDataCount; jj++) {
            if (piColumnT[jj] == 0) {
                continue;
            }
            if (m_pDoc->ColumnData[ic].dat != NULL) {
                ml_vector_delete(&(m_pDoc->ColumnData[ic]), SigmaApp.m_pLib);
            }
            ml_vector_create(&(m_pDoc->ColumnData[ic]), pColumnT[jj].dim, pColumnT[jj].id, (const char_t*) (pColumnT[jj].name), NULL, SigmaApp.m_pLib);
            ml_vector_dup(&(m_pDoc->ColumnData[ic]), (const vector_t*) (&(pColumnT[jj])), SigmaApp.m_pLib);
            ml_vector_delete(&(pColumnT[jj]), SigmaApp.m_pLib);

            ic += 1;
        }
        if (ic > 2) {
            m_pDoc->iDataCount = ic;
        }
        else if (ic == 1) {
            if (m_pDoc->ColumnData[1].dat != NULL) {
                ml_vector_delete(&(m_pDoc->ColumnData[1]), SigmaApp.m_pLib);
            }
            m_pDoc->iDataCount = ic;
        }
    }
    else {
        vector_t* pColumn = NULL;
        int nColCount = 0, ii;
        ic = m_pDoc->iDataCount - 1;
        SigmaApp.m_pLib->errcode = 0;

        int iDim;

        for (jj = 0; jj < iDataCount; jj++) {
            if (piColumnT[jj] == 0) {
                // Column not created if flag is 0
                continue;
            }
            if ((pColumn = InsertColumn(0, ic)) == NULL) {
                for (int ll = 0; ll < iDataCount; ll++) {
                    ml_vector_delete(&(pColumnT[ll]), SigmaApp.m_pLib);
                }
                return;
            }
            iDim = ML_MIN(pColumn->dim, pColumnT[jj].dim);
            if (pColumn->dim == pColumnT[jj].dim) {
                ml_vector_dup(pColumn, (const vector_t*) (&(pColumnT[jj])), SigmaApp.m_pLib);
            }
            else {
                for (ii = 0; ii < iDim; ii++) {
                    pColumn->dat[ii] = pColumnT[jj].dat[ii];
                    pColumn->mask[ii] = pColumnT[jj].mask[ii];
                }
                pColumn->id = pColumnT[jj].id;
                _tcscpy(pColumn->name, (const char_t*) (pColumnT[jj].name));
                _tcscpy(pColumn->label, (const char_t*) (pColumnT[jj].label));
                _tcscpy(pColumn->format, (const char_t*) (pColumnT[jj].format));
                pColumn->status = pColumnT[jj].status;
                pColumn->type = pColumnT[jj].type;        // Vecteur X ou Y
            }
            // Delete created column, when done. 
            ml_vector_delete(&(pColumnT[jj]), SigmaApp.m_pLib);

            ic += 1;
        }
    }

    Logi(_T("Data imported\r\n"));
    SigmaApp.OutputSB(_T("Data imported"), 0, TRUE);
    this->GetDatasheet()->SetColumnCount(1 + GetColumnCount(0));
    if (m_pwndNote) {
        m_pwndNote->m_edtNote.SetWindowText((LPCTSTR) (m_pDoc->szNote));
    }
    UpdateAllViews(NULL);
    if (m_pDoc->iDataCount > iColCount) {
        for (jj = iColCount; jj < m_pDoc->iDataCount; jj++) {
            this->GetDatasheet()->SetColumnWidth(jj + 1, iColWidth);
        }
    }

    SaveState();

    return;
}

void CSigmaDoc::OnDataImport()
{
    char_t szFilters[] = _T("ASCII Files (*.txt;*.dat;*.csv;*.log)|*.txt;*.dat;*.csv;*.log|All files (*.*)|*.*||");

    POSITION pos = GetFirstViewPosition();
    CView *pView = GetNextView(pos);
    if (pView == NULL) {
        return;
    }

    CFileDialog *pFileDlg = new CFileDialog(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, (CWnd*) pView);
    if (pFileDlg->DoModal() != IDOK) {
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString strFilename = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    int_t jj, nColCount = 0, nRowCount = 0;

    vector_t ColumnT[SIGMA_MAXCOLUMNS];
    memset(ColumnT, 0, SIGMA_MAXCOLUMNS * sizeof(vector_t));
    int_t iDataCount = 0;
    for (jj = 0; jj < SIGMA_MAXCOLUMNS; jj++) {
        // Les colonnes sont nommées A, B, C, D, ... à la façon d'Excel
        ColumnT[jj].name[0] = _T('A') + jj;
        ColumnT[jj].name[1] = _T('\0');
    }

    if (SigmaApp.ImportData(strFilename, ColumnT, NULL, &iDataCount,
        &nRowCount, &(m_pDoc->iColumnID), m_pDoc->szNote) == FALSE) {

        if (iDataCount > 0) {
            SigmaApp.m_pLib->errcode = 0;
            for (int ll = 0; ll < iDataCount; ll++) {
                ml_vector_delete(&(ColumnT[ll]), SigmaApp.m_pLib);
            }
        }

        return;
    }

    DoDataImport((vector_t*) ColumnT, NULL, iDataCount);
}

void CSigmaDoc::OnDataImportDlg()
{
    int_t jj;

    vector_t ColumnT[SIGMA_MAXCOLUMNS];
    memset(ColumnT, 0, SIGMA_MAXCOLUMNS * sizeof(vector_t));
    int_t iDataCount = 0;
    for (jj = 0; jj < SIGMA_MAXCOLUMNS; jj++) {
        // Les colonnes sont nommées A, B, C, D, ... à la façon d'Excel
        ColumnT[jj].name[0] = _T('A') + jj;
        ColumnT[jj].name[1] = _T('\0');
    }
    int_t iColumnT[SIGMA_MAXCOLUMNS];
    memset(iColumnT, 0, SIGMA_MAXCOLUMNS * sizeof(int_t));

    CImportDlg importDlg;
    importDlg.m_pColumn = ColumnT;
    importDlg.m_piColumn = iColumnT;
    importDlg.m_pCurrentID = &(m_pDoc->iColumnID);
    importDlg.m_pColumnCount = &iDataCount;
    importDlg.m_pbIsModified = &m_bIsModified;
    importDlg.m_pbRunning = &m_bRunning;
    importDlg.m_pszNote = m_pDoc->szNote;

    importDlg.DoModal();

    if (importDlg.m_bOK == FALSE) {
        if (iDataCount > 0) {
            SigmaApp.m_pLib->errcode = 0;
            for (int ll = 0; ll < iDataCount; ll++) {
                ml_vector_delete(&(ColumnT[ll]), SigmaApp.m_pLib);
            }
        }
        return;
    }

    DoDataImport((vector_t*) ColumnT, (int_t*) iColumnT, iDataCount);
}

void CSigmaDoc::UpdateDocTitle()
{
    char_t szFilename[ML_STRSIZE];
    memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
    SigmaApp.CStringToChar(m_strFilename, szFilename, ML_STRSIZE - 1);
    int_t nlen = (int_t) _tcslen((const char_t*) szFilename), ii;
    if ((nlen > 5) && (nlen < ML_STRSIZE)) {
        char_t *psz = (char_t*) (&szFilename[nlen - 1]);
        for (ii = nlen - 1; ii >= 0; ii--, psz--) {
            if (*psz == _T('.')) {
                *psz = _T('\0');
            }
            else if (*psz == _T('\\')) {
                psz += 1;
                break;
            }
        }

        SetTitle((LPCTSTR) psz);

        CString strT = GetTitle();
        CString strTT = strT, strTTT = strT;
        CWnd *pWnd = NULL;
        CView *pView = GetDataView();
        if (pView) {
            pWnd = pView->GetParent();
            if (pWnd) {
                strT.Append(_T(" - Datasheet"));
                pWnd->SetWindowText((LPCTSTR) strT);
            }
        }
        pView = GetDataFitView();
        if (pView) {
            pWnd = pView->GetParent();
            if (pWnd) {
                strTT.Append(_T(" - Datasheet (Fit)"));
                pWnd->SetWindowText((LPCTSTR) strTT);
            }
        }
        pView = GetPlotView();
        if (pView) {
            pWnd = pView->GetParent();
            if (pWnd) {
                strTTT.Append(_T(" - Graph"));
                pWnd->SetWindowText((LPCTSTR) strTTT);
            }
        }
    }
}

void CSigmaDoc::SetTitle(LPCTSTR lpszTitle)
{
    int_t nlen = (int_t)_tcslen((const char_t*)lpszTitle);
    if ((nlen < 1) || (nlen >= ML_STRSIZE)) {
        CDocument::SetTitle(_T("Untitled"));
    }
    else {
        char_t szFilename[ML_STRSIZE];
        memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
        SigmaApp.CStringToChar(m_strFilename, szFilename, ML_STRSIZE - 1);
        int_t nlen = (int_t) _tcslen((const char_t*) szFilename);
        if ((nlen > 5) && (nlen < ML_STRSIZE) && (CSigmaApp::fileExists((const char_t*) szFilename)) && (CSigmaApp::isFileReadonly((const char_t*) szFilename))) {
            char_t *psz = (char_t*) (&szFilename[nlen - 1]);
            for (int ii = nlen - 1; ii >= 0; ii--, psz--) {
                if (*psz == _T('.')) {
                    *psz = _T('\0');
                }
                else if (*psz == _T('\\')) {
                    psz += 1;
                    break;
                }
            }
            char_t szTitle[ML_STRSIZE];
            _tcscpy(szTitle, (const char_t*) psz);
            _tcscat(szTitle, _T(" [readonly]"));
            CDocument::SetTitle((LPCTSTR) szTitle);
        }
        else {
            CDocument::SetTitle(lpszTitle);
        }
    }
}

CString CSigmaDoc::GetName()
{
    CString strTitle = GetTitle();
    strTitle.TrimRight(_T("*"));
    if (strTitle.Right(11) == _T(" [readonly]")) {
        strTitle = strTitle.Left(strTitle.GetLength() - 11);
    }

    return strTitle;
}

void CSigmaDoc::OnDataExport()
{
    char_t szFilters[] = _T("ASCII Files (*.txt;*.dat;*.csv;*.log)|*.txt;*.dat;*.csv;*.log|All files (*.*)|*.*||");

    POSITION pos = GetFirstViewPosition();
    CView *pView = GetNextView(pos);
    if (pView == NULL) {
        return;
    }

    CFileDialog *pFileDlg = new CFileDialog(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, (CWnd*) pView);
    if (pFileDlg->DoModal() != IDOK) {
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString strFilename = pFileDlg->GetPathName();
    delete pFileDlg; pFileDlg = NULL;

    CString strExt = strFilename.Mid(strFilename.GetLength() - 4, 4);
    strExt.MakeLower();

    if (strExt.GetAt(0) != _T('.')) {
        strFilename += _T(".txt");
    }

    if (SigmaApp.ExportData(strFilename, m_pDoc->ColumnData, m_pDoc->iDataCount) == TRUE) {
        Logi(_T("Data exported\r\n"));
        SigmaApp.OutputSB(_T("Data exported"), 0, TRUE);
    }
}

void CSigmaDoc::OnAutoExport()
{
    SigmaApp.m_Options.opt[AUTOEXPORT_INDEX] = (1 == SigmaApp.m_Options.opt[AUTOEXPORT_INDEX]) ? 0 : 1;
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    if (pFrame) {
        CMenu *pMenu = pFrame->GetMenu();
        if (pMenu) {
            CMenu *pSubMenu = pMenu->GetSubMenu(0);
            if (pSubMenu) {
                pSubMenu->CheckMenuItem(ID_FILE_AUTOEXPORT, (1 == SigmaApp.m_Options.opt[AUTOEXPORT_INDEX]) ? (MF_BYCOMMAND | MF_CHECKED) : (MF_BYCOMMAND | MF_UNCHECKED));
            }
        }
    }
}

void CSigmaDoc::OnFileSaveAs()
{
    CSigmaViewPlot* plotView = (CSigmaViewPlot*) (GetPlotViewX());
    bool bPlotActive = (plotView != NULL);

    char_t szFiltersN[] = _T("SigmaGraph files (*.sid)|*.sid|ASCII files (*.txt)|*.txt||");
    char_t szFiltersP[] = _T("SigmaGraph files (*.sid)|*.sid|ASCII files (*.txt)|*.txt|Enhanced Metafile (*.emf)|*.emf|Scalable Vector Graphics (*.svg)|*.svg||");

    POSITION pos = GetFirstViewPosition();
    CView *pView = GetNextView(pos);
    if (pView == NULL) {
        return;
    }

    const char_t *pszFilename = static_cast<const char_t *>(m_strFilename);
    CFileDialog *pFileDlg = new CFileDialog(FALSE, _T("sid"), pszFilename, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, bPlotActive ? szFiltersP : szFiltersN, (CWnd*) pView);
    if (pFileDlg->DoModal() != IDOK) {
        delete pFileDlg; pFileDlg = NULL;
        return;
    }
    CString strFilename = pFileDlg->GetPathName();
    CString strExt = pFileDlg->GetFileExt();
    int indexT = (int) (pFileDlg->GetOFN().nFilterIndex);
    delete pFileDlg; pFileDlg = NULL; 

    strExt.MakeLower();

    if (strExt == _T("sid")) {
        m_strFilename = strFilename;
        HANDLE hFile = INVALID_HANDLE_VALUE;
        hFile = ::CreateFile((LPCTSTR)m_strFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
            ASSERT(pFrame != NULL);
            int_t iT = pFrame->MessageBox(_T("File already exists. Overwrite it?"), _T("SigmaGraph"), MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON3);
            if (iT != IDYES) {
                return;
            }
        }

        // save document
        CWaitCursor wait;
        if (SigmaApp.SaveSigmaDoc((LPCTSTR)m_strFilename, (const sigmadoc_t*)m_pDoc, (const vector_t*)m_pColumnAutoX,
            (const plot_t*)m_pPlot, (const WINDOWPLACEMENT*)(&m_PlacementPlot), (const WINDOWPLACEMENT*)(&m_PlacementDatasheet),
            (const WINDOWPLACEMENT*)(&m_PlacementDatasheetFit)) == TRUE) {
            SigmaApp.OutputSB(_T("SigmaGraph document saved."), 0, TRUE);
            SigmaApp.AddToRecentFileList((LPCTSTR)m_strFilename);
            SetPathName((LPCTSTR) m_strFilename);
            UpdateDocTitle();
            SetModifiedFlag(FALSE);
        }
    }
    else if ((strExt == _T("txt")) || (indexT == 1)) {
        if (strExt != _T("txt")) {
            strFilename += _T(".txt");
        }

        if (SigmaApp.ExportData(strFilename, m_pDoc->ColumnData, m_pDoc->iDataCount) == TRUE) {
            Logi(_T("Data exported\r\n"));
            SigmaApp.OutputSB(_T("Data exported"), 0, TRUE);
        }
    }
    else if (bPlotActive) {
        if ((strExt == _T("emf")) || (strExt == _T("svg"))) {
            if (plotView->DoFileExport(strFilename)) {
                Logi(_T("Plot exported\r\n"));
                SigmaApp.OutputSB(_T("Plot exported"), 0, TRUE);
            }
        }
    }
}

void CSigmaDoc::Undo()
{
    if (m_pUndo->CanUndo() == FALSE) {
        return;
    }

    m_pUndo->Undo(m_pDoc, m_pPlot);
    UpdateSigmaDoc();
    UpdateAllViews(NULL);
}

void CSigmaDoc::Redo()
{
    if (m_pUndo->CanRedo() == FALSE) {
        return;
    }

    m_pUndo->Redo(m_pDoc, m_pPlot);
    UpdateSigmaDoc();
    UpdateAllViews(NULL);
}

void CSigmaDoc::ShowMathDialog(int_t iOperation)
{
    CAnalysisDlg dlgMath;

    CSigmaViewData* pView = (CSigmaViewData*)(GetDataView());

    if (pView == NULL) {
        CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
        ASSERT(pFrame != NULL);
        if (pFrame == NULL) {
            Loge(_T("Cannot show the analysis dialog: invalid data.\r\n"));
            return;
        }
        if (pFrame->CreateSigmaView(SigmaApp.m_pTemplateData, RUNTIME_CLASS(CSigmaViewData), TRUE) == FALSE) {
            Loge(_T("Cannot show the analysis dialog: invalid data.\r\n"));
            return;
        }

        pView = (CSigmaViewData*)(GetDataView());
        if (pView == NULL) {
            Loge(_T("Cannot show the analysis dialog: invalid data.\r\n"));
            return;
        }
    }

    int_t nSelectedColumnIn = -1;
    CDatasheet *pDatasheet = (CDatasheet*)(pView->GetDatasheet());
    if (pDatasheet != NULL) {
        vector_t *pColumn = pDatasheet->GetSelectedColumn(&nSelectedColumnIn);
        if (pColumn != NULL) {
            if ((pColumn->type & 0xF0) != 0x20) {    // Y
                nSelectedColumnIn = -1;
            }
        }
    }

    dlgMath.m_iOperation = iOperation;
    dlgMath.m_pDoc = this;
    dlgMath.m_pPlot = GetPlot();
    dlgMath.m_pColumn = GetColumns(0, &(dlgMath.m_nColumnCount));
    dlgMath.m_nSelectedColumnIn = nSelectedColumnIn;
    dlgMath.DoModal();
}

void CSigmaDoc::OnMathIntegrate()
{
    ShowMathDialog(0);
}

void CSigmaDoc::OnMathDifferentiate()
{
    ShowMathDialog(1);
}

void CSigmaDoc::OnMathAverage()
{
    ShowMathDialog(2);
}

void CSigmaDoc::OnMathFFT()
{
    ShowMathDialog(3);
}

void CSigmaDoc::OnAutocorrelation()
{
    ShowMathDialog(4);
}