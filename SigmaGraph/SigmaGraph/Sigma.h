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

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "SigmaDefine.h"

#include "resource.h"                                               // main symbols
#include "SigmaConsole.h"

#include "htmlhelp.h"

extern software_t g_Sigmaware;                                      // Sigma software informations

typedef struct _sigmadoc_t
{
    // Document persistent data
    software_t Software;                                            // Sigma software informations
    int_t nID;                                                      // ID, unique
    char_t szName[ML_STRSIZE];                                      // Name
    char_t szNote[ML_STRSIZEW];                                     // Associated note
    int_t iType;                                                    // Type
    byte_t bStatus;                                                 // State : shown, hidden, read-only, ...
    date_t tmCreated;                                               // Document creation date
    date_t tmModified;                                              // Document modification date

    long_t iColumnID;                                               // Last Column ID (each column has unique ID)
    int_t iDataCount;                                               // Number of data columns
    int_t iDataFitCount;                                            // Number of fit columns
    vector_t ColumnData[SIGMA_MAXCOLUMNS];                          // Data columns
    vector_t ColumnDataFit[SIGMA_MAXCOLUMNS];                       // Fit columns
    stat_t Stats[SIGMA_MAXCOLUMNS];                                 // Data statistics
    fit_t Fit[SIGMA_MAXCOLUMNS >> 1];                               // Fit
    //
} sigmadoc_t;

typedef struct _template_t
{
    // Template persistent data
    software_t Software;                                            // Sigma software informations

    date_t tmCreated;                                               // Template creation date
    date_t tmModified;                                              // Template modification date

    int_t iCount;                                                   // Number of templates
} template_t;

#define AUTOEXPORT_INDEX   4

#include "SigmaViewPlot.h"
#include "SigmaViewData.h"
#include "SigmaViewDataFit.h"

#include <afxadv.h>

// CSigmaApp:
// See Sigma.cpp for the implementation of this class
//

class SigmaRecentFileList : public CRecentFileList
{
private:
    UINT m_uiClearID;

public:
    SigmaRecentFileList(UINT nStart, LPCTSTR lpszSection,
        LPCTSTR lpszEntryFormat, int nSize, int nMaxDispLen) : CRecentFileList(nStart, lpszSection,
        lpszEntryFormat, nSize, nMaxDispLen)
    {
        m_uiClearID = 0;
    }

    BOOL GetDisplayName(CString& strName, int nIndex,
        LPCTSTR lpszCurDir, int nCurDir, BOOL bAtLeastName = TRUE) const;

    void UpdateMenu(CCmdUI* pCmdUI);

    UINT GetClearID(void)
    {
        return m_uiClearID;
    }
};

class CSigmaApp : public CWinApp
{
private:
    char m_szLocaleA[ML_STRSIZE];

    Gdiplus::GdiplusStartupInput m_GdiplusStart;
    ULONG_PTR m_GdiplusToken;

    bool m_bPortable;

    int_t m_iWinVersionMin;
    int_t m_iWinVersionMax;

private:
    bool isSeparator(char_t cT)
    {
        if ((cT == _T(' ')) || (cT == _T('\t')) || (cT == _T('\r')) || (cT == _T('\n'))
            || (cT == _T('+')) || (cT == _T('-')) || (cT == _T('/')) || (cT == _T('*'))
            || (cT == _T('%')) || (cT == _T('^'))
            || (cT == _T('\0'))) {
            return true;
        }
        return false;
    }

    bool isSeparatorLeft(char_t cT)
    {
        return isSeparator(cT) || (cT == _T('('));
    }

    bool isSeparatorRight(char_t cT)
    {
        return isSeparator(cT) || (cT == _T(')'));
    }

public:
    CSigmaApp();

    bool isPortable(void)
    {
        return m_bPortable;
    }

    void LoadStdProfileSettings(UINT nMaxMRU);

    BOOL isWinXP(void)
    {
        return (m_iWinVersionMax == 5);
    }

    CMultiDocTemplate *m_pTemplatePlot;
    CMultiDocTemplate *m_pTemplateData;
    CMultiDocTemplate *m_pTemplateDataFit;

    // User-defined functions
    int_t m_iUserFitCount;
    userfit_t *m_pUserFit[ML_FIT_MAXMODEL];
    //

    options_t m_Options;                // User preferences
    workspace_t m_Workspace;        // Project Workspace
    int_t m_nDocCount;
    int_t m_nDocID;

    int_t GetID() const { return m_nDocID; }

    libvar_t* m_pLib;                    // Interaction avec les Lib

    plot_t *m_pDefaultPlot;
    char_t m_szUserDir[ML_STRSIZE];
    BOOL getUserDir(void);
    BOOL getPlotSettings();
    BOOL setPlotSettings();
    BOOL getOptions();
    BOOL setOptions();
    BOOL getUserFit();
    BOOL setUserFit();

    // Utility functions extensively used by all components
    void ScriptOutput(const char_t *pszText);
    void ScriptOutputClear(void);
    void Output(CString strT, BOOL bAddTime = FALSE, BOOL bShowMessage = FALSE, int_t nIcon = 1, int_t nStatusbarPane = -1, BOOL bAutoClear = FALSE);
    void OutputSB(CString strT, int_t nStatusbarPane = 0, BOOL bAutoClear = FALSE);        //Print text on status bar pane
    CMDIFrameWnd *ActivateMainframe(void);
    BOOL PtOnLine(const irect_t *precT, CPoint ptT, byte_t orient = 0);
    int_t CStringToChar(CString strSrc, char_t* szDest, int_t nMax, BOOL bDelCRLF = FALSE);
    int_t CStringReverseFind(CString strT, LPCTSTR szFind);
    BOOL GetFont(HWND hWnd, LOGFONT *plf);
    BOOL GetColor(HWND hWnd, COLORREF *pcr);
    void Reformat(real_t fVal, char_t* szBuffer, int_t nCount, char_t* pszFormat = NULL, int_t *pFmt = NULL);
    void ResizePane(int_t nPane, CString strT);
    BOOL LogfontToFont(LOGFONT lft, font_t *pfnt);
    BOOL FontToLogfont(font_t fnt, LOGFONT *plft);
    BOOL ReadTextFile(CString fileName, CEdit *pEdit, CFileStatus &fileStatus, int_t *nLineCount, byte_t bCRLF = 1);
    BOOL GetDataFromFile(CString fileName, vector_t *pVector, int_t *uiVectorCurID,
                                int_t *nCol, char_t *szNote = NULL, byte_t bTranspose = FALSE);
    BOOL ImportData(CString fileName, vector_t *pColumn, int_t *piColumn, int_t *nColumnCount, int_t *nRowCount,
                         long_t *pvecCurID, char_t *pszNote = NULL);
    BOOL ExportData(CString fileName, vector_t *pColumn, int_t nColumnCount);
    BOOL CopyDocument(sigmadoc_t *pDocTo, const sigmadoc_t *pDocFrom);
    BOOL CopyPlot(plot_t *pPlotTo, const plot_t *pPlotFrom);

    CString m_strOutput;
    void OutputClear(void);
    void OutputView(void);

    // Performance counter
    LARGE_INTEGER m_liPerfCount;
    ulong_t m_uiPerfCount;
    void CounterStart();
    void CounterStop();

    // Parser
    BOOL DoParse(const char_t* szInput, char_t* szOutput, real_t* pVal = NULL, const char_t* pszFormat = NULL);
    void ResizeParam(void);
    real_t m_fParam[_T('z') - _T('a') + 1];
    char_t m_szParam[_T('z') - _T('a') + 1][ML_STRSIZET];           // Params name : by default : a, b, c, ..., z
    BOOL m_bParam[_T('z') - _T('a') + 1];                           // Param set ?
    int_t m_nVarnum;
    real_t m_fAns;
    bool m_bAns;

    // Drag and Drop
    BOOL m_bDropping;
    char_t m_szDropFilename[ML_STRSIZE];

    // Frame position
    WINDOWPLACEMENT m_LastPlacement;

    // File Format
    static bool fileExists(const char_t *pszFilename);
    static bool makeDir(const char_t *pszDir);
    static bool isFileReadonly(LPCTSTR pszFilename);
    static bool setFileReadWrite(LPCTSTR pszFilename);
    static int getShortFilename(char_t* szFilenameShort, const char_t* filename);
    BOOL AccessSigmaDoc(FILE* fp, sigmadoc_t *pDoc, int_t iOp);
    BOOL OpenSigmaDoc(LPCTSTR lpszFileName, sigmadoc_t *pDoc, vector_t *pColumnAutoX, plot_t *pPlot,
        WINDOWPLACEMENT *pPlacementPlot, WINDOWPLACEMENT *pPlacementDatasheet, WINDOWPLACEMENT *pPlacementDatasheetFit);
    static CString getFitReport(fit_t *pFit);
    bool isAutoExportChecked();
    BOOL SaveSigmaDoc(LPCTSTR lpszFileName, const sigmadoc_t *pDoc, const vector_t *pColumnAutoX, const plot_t *pPlot,
        const WINDOWPLACEMENT *pPlacementPlot, const WINDOWPLACEMENT *pPlacementDatasheet, const WINDOWPLACEMENT *pPlacementDatasheetFit);
    BOOL AccessSigmaTemplate(FILE* fp, template_t *pTemplate, int_t iOp);
    BOOL OpenSigmaTemplate(LPCTSTR lpszFileName, template_t *pTemplate, plot_t **ppPlot);
    BOOL SaveSigmaTemplate(LPCTSTR lpszFileName, template_t *pTemplate, const plot_t **ppPlot);

    // Copy Plot Style
    plot_t *m_pPlotCopy;
    BOOL m_bCanPastePlot;

    // Help
    char_t m_szMainDir[ML_STRSIZE];
    char_t m_szHelpFilename[ML_STRSIZE];
    char_t m_szHelpFilenamePDF[ML_STRSIZE];

    void FileNew();

    BOOL GetWindowPlacement(const char_t *pszFilenameShort, WINDOWPLACEMENT *pPlacementMain,
        int iDefWidth = SIGMA_DEFWIDTH, int iDefHeight = SIGMA_DEFHEIGHT,
        int iNextToMain = -1);
    BOOL SetWindowPlacement(const char_t *pszFilenameShort, WINDOWPLACEMENT *pPlacementMain);

// Overrides
public:
    virtual BOOL InitInstance();
    virtual void AddToRecentFileList(LPCTSTR lpszPathName);
    virtual void WinHelp(DWORD dwData, UINT nCmd);

    afx_msg void OnAppAbout();

    DECLARE_MESSAGE_MAP()

    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnAppExit();
    afx_msg void OnOpenMRUFile(UINT uiID);
    virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
    virtual int ExitInstance();
    afx_msg void OnWindowDup();
    afx_msg void OnWindowCopyFormat();
    afx_msg void OnUpdateWindowCopyFormat(CCmdUI *pCmdUI);
    afx_msg void OnWindowPasteFormat();
    afx_msg void OnUpdateWindowPasteFormat(CCmdUI *pCmdUI);
    afx_msg void OnWindowCloseAll();
    afx_msg void OnHelpIndex();
    afx_msg void OnHelpContents();
    afx_msg void OnHelpContentsPDF();
    afx_msg void OnHelpContext();
};

extern CSigmaApp SigmaApp;
