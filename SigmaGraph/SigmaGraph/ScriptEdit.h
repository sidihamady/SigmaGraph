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

#include "../../LibGraph/LibFile/libfile.h"

#include "afxwin.h"

#include "ScintillaCtrl.h"
#include "ScintillaCommon.h"

#define SCRIPT_TYPE_DEFAULT                 0
#define SCRIPT_TYPE_KEYWORD                 1
#define SCRIPT_TYPE_KEYFUNC                 2
#define SCRIPT_TYPE_KEYSTRING               3
#define SCRIPT_TYPE_KEYMATH                 4
#define SCRIPT_TYPE_KEYIO                   5
#define SCRIPT_TYPE_KEYSYS                  6
#define SCRIPT_TYPE_COMMENT                 7
#define SCRIPT_TYPE_COMMENT_DOC             8
#define SCRIPT_TYPE_COMMENT_LINE            9
#define SCRIPT_TYPE_COMMENT_SPECIAL        10
#define SCRIPT_TYPE_CHARACTER              11
#define SCRIPT_TYPE_CHARACTER_EOL          12
#define SCRIPT_TYPE_STRING                 13
#define SCRIPT_TYPE_STRING_EOL             14
#define SCRIPT_TYPE_DELIMITER              15
#define SCRIPT_TYPE_PUNCTUATION            16
#define SCRIPT_TYPE_OPERATOR               17
#define SCRIPT_TYPE_BRACE                  18
#define SCRIPT_TYPE_COMMAND                19
#define SCRIPT_TYPE_IDENTIFIER             20
#define SCRIPT_TYPE_LABEL                  21
#define SCRIPT_TYPE_NUMBER                 22
#define SCRIPT_TYPE_PARAMETER              23
#define SCRIPT_TYPE_REGEX                  24
#define SCRIPT_TYPE_UUID                   25
#define SCRIPT_TYPE_VALUE                  26
#define SCRIPT_TYPE_PREPROCESSOR           27
#define SCRIPT_TYPE_SCRIPT                 28
#define SCRIPT_TYPE_ERROR                  29

#define SCRIPT_STYLE_NORMAL                 0
#define SCRIPT_STYLE_BOLD                   1
#define SCRIPT_STYLE_ITALIC                 2
#define SCRIPT_STYLE_UNDERL                 4
#define SCRIPT_STYLE_HIDDEN                 8
#define SCRIPT_FOLD_COMMENT                 1
#define SCRIPT_FOLD_COMPACT                 2
#define SCRIPT_FOLD_PREPROC                 4
#define SCRIPT_FOLD_HTML                   16
#define SCRIPT_FOLD_HTMLPREP               32
#define SCRIPT_FOLD_COMMENTLUA             64
#define SCRIPT_FOLD_QUOTESLUA             128
#define SCRIPT_FLAG_WRAPMODE               16

#define PAGE_COMMON                         uT("Common")
#define PAGE_LANGUAGES                      uT("Languages")
#define PAGE_STYLE_TYPES                    uT("Style types")
#define STYLE_TYPES_COUNT                   32

struct CommonInfo {
    bool syntaxEnable;
    bool foldEnable;
    bool indentEnable;
    bool readOnlyInitial;
    bool overTypeInitial;
    bool wrapModeInitial;
    bool displayEOLEnable;
    bool indentGuideEnable;
    bool lineNumberEnable;
    bool longLineOnEnable;
    bool whiteSpaceEnable;
    bool autocompleteEnable;
    int lineSpacing;
};

struct LanguageInfo {
    const char_t *name;
    const char_t *filepattern;
    const int lexer;
    struct {
        const int type;
        const char_t *words;
    } styles[STYLE_TYPES_COUNT];
    const int folds;
};

#define STYLEINFO_COUNT                        21
#define STYLEINFO_INDEX_DEFAULT                 0
#define STYLEINFO_INDEX_COMMENT_START           1
#define STYLEINFO_INDEX_COMMENT_END             3
#define STYLEINFO_INDEX_NUMBER                  4
#define STYLEINFO_INDEX_KEYWORD                 5
#define STYLEINFO_INDEX_STRING_START            6
#define STYLEINFO_INDEX_STRING_END              9
#define STYLEINFO_INDEX_OPERATOR               10
#define STYLEINFO_INDEX_IDENTIFIER             11
#define STYLEINFO_INDEX_STRINGEOL              12
#define STYLEINFO_INDEX_LIB_START              13
#define STYLEINFO_INDEX_LIB_END                19
#define STYLEINFO_INDEX_LINENUM                20

#define STYLEINFO_COLOR_BACK_DEFAULT            (RGB(0xFF, 0xFF, 0xFF))
#define STYLEINFO_COLOR_BACK_LINENUM            (RGB(0xEF, 0xF7, 0xF7))
#define STYLEINFO_COLOR_MARKER                  (RGB(0xDC, 0xB4, 0x64))
#define STYLEINFO_COLOR_INDENT                  (RGB(0xF1, 0xF4, 0xF5))
#define STYLEINFO_COLOR_BACK_SEL                (RGB(0x64, 0x8C, 0xB4))
#define STYLEINFO_COLOR_FOLDLINE                (RGB(0x96, 0xB4, 0xC8))
#define STYLEINFO_COLOR_WHITESPACE              (RGB(0x73, 0xB4, 0xD5))
#define STYLEINFO_COLOR_CARET_FORE              (RGB(0x32, 0x46, 0x78))
#define STYLEINFO_COLOR_CARET_BACK              (RGB(0xE6, 0xF0, 0xF2))

struct StyleInfo {
    char_t name[ML_STRSIZES];
    COLORREF foreground;
    COLORREF background;
    char_t fontname[ML_STRSIZES];
    int fontsize;
    int fontstyle;
    int lettercase;
};

struct StyleInfoDefault {
    const char_t *name;
    const COLORREF foreground;
    const COLORREF background;
    const char_t *fontname;
    const int fontsize;
    const int fontstyle;
    const int lettercase;
};

struct ScintillaPrefs {
    CommonInfo common;
    StyleInfo style[STYLEINFO_COUNT];
    bool outputshow;
    int outputsize;
};

#define SCINTILLA_AUTOC_MAXSUFFIX    100
struct AutoCList {
    const int len;
    const char *prefix;
    const char *words;
};

struct CallTipList {
    const int len;
    const char *prefix;
    const char *args;
};

// CScriptEdit

class CScriptEdit : public CScintillaCtrl
{
    DECLARE_DYNAMIC(CScriptEdit)

private:
    CString m_strFilename;

    LanguageInfo const* m_pLanguage;

    int m_LineNrID;
    int m_LineNrMargin;
    int m_FoldingID;
    int m_FoldingMargin;
    int m_DividerID;

    static const int AUTOC_API_COUNT;
    static const AutoCList AUTOC_API[];
    static const int CALLTIP_API_COUNT;
    static const CallTipList CALLTIP_API[];

    static const char_t* WORDLIST;
    static const char_t* WORDLIST_API;
    static const LanguageInfo LANGPREFS;

    CommonInfo m_CommonPrefs;
    StyleInfo m_StylePrefs[STYLEINFO_COUNT];

    int m_iMaxLineWidth;

    char_t m_szCurrentDir[ML_STRSIZE];

public:
    CScriptEdit();
    virtual ~CScriptEdit();
    BOOL Create(DWORD dwStyle, const RECT& rcT, CWnd* pParentWnd, UINT nID, DWORD dwExStyle = 0, LPVOID lpParam = NULL);

    static const StyleInfoDefault STYLEPREFS[];

    void highlightLine(int iLine);

    int getMaxLineWidth(void)
    {
        return m_iMaxLineWidth;
    }
    void setMaxLineWidth(int iMaxLineWidth)
    {
        m_iMaxLineWidth = iMaxLineWidth;
    }

    CString m_strSearch;
    void DoFind(CString& strSearch, int iStart = -1, int iStop = -1);
    void DoFindNext(void);

    void initCaret(const int iSpacing = -1);
    void initCommonPrefs(const CommonInfo &tCommonPrefs);
    bool initPrefs(const ScintillaPrefs &tPrefs);

    LanguageInfo const* GetLanguageInfo() {
        return m_pLanguage;
    };

    void OnCharAdded(SCNotification* pSCNotification);
    void OnDwellStart(SCNotification* pSCNotification);
    void OnDwellEnd(SCNotification* pSCNotification);

    void calcLinenumberMargin(int nLinesCount = 0);
    void SetStyleFontX(int iStyle, COLORREF foreColor, COLORREF backColor, int iSize, const char_t* faceName);

    bool DoNewFile(void);
    bool LoadFile(void);
    bool LoadFile(const CString &strFilename);
    bool SaveFile(void);
    bool SaveFile(const CString &strFilename);
    bool SaveFileAs(void);
    bool isModified(void);
    CString GetFilename() { return m_strFilename; };
    void SetFilename(const CString &strFilename)
    {
        m_strFilename = strFilename;
    };

    void DoPrint(void);
    bool ShowPrintPreviewDialog(void);

protected:

    DECLARE_MESSAGE_MAP()

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnNcPaint();
};
