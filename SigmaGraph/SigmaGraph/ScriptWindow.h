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

#ifndef __LUA_INC_H__
#define __LUA_INC_H__
extern "C"
{
#include "../../LibCalc/Lua/lua.h"
#include "../../LibCalc/Lua/lualib.h"
#include "../../LibCalc/Lua/lauxlib.h"
}
#endif // __LUA_INC_H__

#include "SigmaToolBar.h"

#include "ScriptEdit.h"
#include "ScriptOutput.h"

#define SCRIPT_MAXLINES            8192
#define SCRIPT_MAXCHARS            524288

#define FAVORITES_MENU_INDEX    4

extern CSigmaDoc *g_pDoc;
extern BOOL g_bStopit;
extern LARGE_INTEGER g_liTicToc;
extern ulong_t g_ulTicToc;
extern int_t g_iErrline;

BOOL initScriptEngine(lua_State *pScriptEngine);
int_t cf_error(lua_State *pLua);

// CScriptWindow dialog

typedef struct _lexword {
    int count;
    char_t *words[10];
} lexword;

#define SCRIPT_SAMPLES_COUNT    4
typedef struct _scriptsample_t {
    const char_t *title;        // Sample title
    const int lines;            // Total number of line (including empty lines)
    const int linemax;          // Index of the longest line
    const char_t *script;       // Sample
} scriptsample_t;

class CScriptWindow : public CDialog
{
    DECLARE_DYNAMIC(CScriptWindow)

public:

    ScintillaPrefs m_ScintillaPrefs;

    char_t * trim(const char_t *pszT, long *iLen)
    {
        char_t *pszTrimmed = (char_t *) pszT;
        long ii;
        int iStart = 0, iEnd = *iLen;
        if (iEnd <= 0) {
            return NULL;
        }
        for (ii = 0; ii < *iLen; ii++) {
            if ((pszT[ii] != '\r') && (pszT[ii] != '\n') && (pszT[ii] != '\t') && (pszT[ii] != ' ')) {
                iStart = ii;
                break;
            }
        }
        for (ii = *iLen - 1; ii >= 0; ii--) {
            if ((pszT[ii] != '\r') && (pszT[ii] != '\n') && (pszT[ii] != '\t') && (pszT[ii] != ' ')) {
                iEnd = ii + 1;
                break;
            }
        }
        if ((iStart != 0) || (iEnd != *iLen)) {
            pszTrimmed += iStart;
            *(pszTrimmed + iEnd) = _T('\0');
            *iLen = iEnd - iStart;
        }
        return pszTrimmed;
    }

    static const scriptsample_t SCRIPT_SAMPLES[];

public:
    CScriptWindow(CWnd* pParent = NULL);                        // standard constructor
    virtual ~CScriptWindow();

    BOOL Create(UINT nID, CWnd *pWnd) { return CDialog::Create(nID, pWnd); }

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_SCRIPT };

    lua_State *m_pScriptEngine;
    char *m_pszBufferA;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
    virtual void PostNcDestroy();
    virtual void OnOK();
    virtual void OnCancel();

    CSigmaToolBar m_wndToolbar;
    CPoint m_ptOffsetTop;

    DECLARE_MESSAGE_MAP()

    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    CScintillaEditState m_ScintillaEditState;

private:

    int m_iMarkerCount;
    void DoAddmarker(int nLine);

    void RunScript(const char_t *pszFilename = NULL);

public:
    CScriptEdit *m_edtScript;
    CScriptOutput *m_edtOutput;
    CRect m_Rect;
    BOOL m_bInitialized;

    static const UINT SCINTILLA_MSG_FINDREPLACE;

    char_t m_szFilename[ML_STRSIZE];

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void DoResize(int_t delta);

    BOOL m_bRunning;
    BOOL m_bIsModified;
    BOOL ScriptInit();
    void ScriptClose();
    void ScriptFileSave(LPCTSTR pszFilename);
    BOOL ScriptFileOpen(LPCTSTR pszFilename, bool bStraight = false);

    long_t ScriptFileSize(const char_t* pszFilename)
    {
        HANDLE hFile = INVALID_HANDLE_VALUE;
        DWORD dwSizeHigh = 0L, dwSizeLow = 0L;

        hFile = ::CreateFile((LPCTSTR)pszFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return 0L;
        }
        dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
        ::CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;

        return (long_t) ((dwSizeHigh * (2 ^ 32)) + dwSizeLow);
    }

    int_t ScriptFileGetLine(FILE* pf, char_t* pszBuffer, int_t nCount)
    {
        int ii, iLen = 0;

        if (feof(pf) != 0) { // EOF
            return -1;
        }

        if (_fgetts(pszBuffer, nCount, pf) == NULL) {
            return -1;
        }

        if (feof(pf) != 0) { // EOF
            return -1;
        }

        iLen = (int) _tcslen(pszBuffer);

        if ((iLen < 0) || (iLen > nCount)) {
            return -1;
        }
        if (iLen == 0) {
            pszBuffer[0] = _T('\0');
            return 0;
        }

        for (ii = iLen - 1; ii >= 0; ii--) {
            if ((pszBuffer[ii] != _T('\r')) && (pszBuffer[ii] != _T('\n'))) { // EOL
                if (ii > 0) {
                    pszBuffer[ii + 1] = _T('\0');
                    return ii + 1;
                }
            }
        }

        pszBuffer[0] = _T('\0');
        return 0;
    }

    // >> Favorite
    favorite_t *m_pFavorite;
    int_t m_iFavoriteCount;
    int_t m_iFavorite;
    void LoadFavorite(int_t iFavorite);
    void UpdateFavoriteMenu();
    // >>

    int lineNumber(const char* pszMessageA);

    void SetModified(BOOL bIsModified = TRUE);
    BOOL IsModified(void);

    BOOL m_bFirstSearch;    // Is this the first search

    virtual void OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
    virtual void TextNotFound(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression, BOOL bReplaced);
    virtual BOOL FindText(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
    virtual void OnEditFindReplace(BOOL bFindOnly);
    virtual BOOL FindTextSimple(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
    virtual void OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord, BOOL bRegularExpression, LPCTSTR lpszReplace);
    virtual void OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
    virtual BOOL SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL bWord, BOOL bRegularExpression);
    virtual long FindAndSelect(DWORD dwFlags, TextToFind& ft);
    virtual void AdjustFindDialogPosition();
    virtual CScintillaFindReplaceDlg* CreateFindReplaceDialog();

    HICON GetIconForItem(UINT itemID) const;
    
    void OutputView(bool bView = true);

    void Relayout(void);

    void LoadSample(int indexT);

    afx_msg void OnScriptSaveAs();
    afx_msg void OnScriptSave();
    afx_msg void OnScriptOpen();
    afx_msg void OnScriptRead();
    afx_msg void OnScriptNew();
    afx_msg void OnScriptFont();
    afx_msg void OnScriptRun();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnHelpContents();
    afx_msg void OnEditSelectAll();
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnEditCopy();
    afx_msg void OnEditCut();
    afx_msg void OnEditPaste();
    afx_msg void OnEditFind();
    afx_msg void OnEditRepeat();
    afx_msg void OnEditReplace();
    afx_msg void OnEditGoto();
    afx_msg void OnOptionsAddmarker();
    afx_msg void OnOptionsColors();
    afx_msg void OnOptionsDeleteAllMarkers();
    afx_msg void OnOptionsFindNextmarker();
    afx_msg void OnOptionsFindPrevmarker();
    afx_msg void OnOptionsViewLinenumbers();
    afx_msg void OnOptionsAutocomplete();
    afx_msg void OnOptionsLineSpacing100();
    afx_msg void OnOptionsLineSpacing125();
    afx_msg void OnOptionsLineSpacing150();
    afx_msg void OnOptionsLineSpacing175();
    afx_msg void OnOutputSizeSmall();
    afx_msg void OnOutputSizeMedium();
    afx_msg void OnOutputSizeLarge();
    afx_msg void OnOutputView();
    afx_msg void OnViewZoomIn();
    afx_msg void OnViewZoomOut();
    afx_msg void OnViewZoomNone();
    afx_msg void OnClearOutput();
    afx_msg LRESULT OnFindReplaceCmd(WPARAM, LPARAM lParam);
    afx_msg void OnUpdateOptionsViewLinenumbers(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsAutocomplete(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsLineSpacing100(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsLineSpacing125(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsLineSpacing150(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOptionsLineSpacing175(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditReplace(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditGoto(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
    afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);
    afx_msg void OnScriptFavorite();
    afx_msg void OnFavoriteOne();
    afx_msg void OnFavoriteTwo();
    afx_msg void OnFavoriteThree();
    afx_msg void OnFavoriteFour();
    afx_msg void OnSampleOne();
    afx_msg void OnSampleTwo();
    afx_msg void OnSampleThree();
    afx_msg void OnSampleFour();
    afx_msg void OnScriptClose();
    afx_msg void OnClose();
    afx_msg void OnScriptReset();
    afx_msg void OnScriptAddFavorite();
    afx_msg void OnUpdateScriptAddFavorite(CCmdUI *pCmdUI);
    afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg void OnDestroy();
};
