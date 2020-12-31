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
#include "ScriptEdit.h"
#include "res/markersmall.xpm"

// Lua
const char_t* CScriptEdit::WORDLIST =
    _T("and break do else elseif end for ")
    _T("function if local nil not or repeat return then ")
    _T("until while");

const char_t* CScriptEdit::WORDLIST_API =
    _T("_G _VERSION cls clear ")
    _T("print ")
    _T("assert collectgarbage dofile error getfenv getmetatable ipairs ")
    _T("load loadfile loadstring module next pairs pcall print rawequal ")
    _T("rawget rawset require select setfenv setmetatable tonumber tostring ")
    _T("type unpack xpcall ")

    _T("coroutine.create coroutine.resume coroutine.running coroutine.status coroutine.wrap coroutine.yield ")

    _T("debug.getfenv debug.gethook debug.getinfo debug.getlocal debug.getmetatable debug.getregistry debug.getupvalue ")
    _T("debug.setfenv debug.sethook debug.setlocal debug.setmetatable debug.setupvalue debug.traceback ")

    _T("file:close file:flush file:lines file:read file:seek file:setvbuf file:write ")

    _T("io.close io.flush io.input io.lines io.open io.output io.popen io.read io.stderr ")
    _T("io.stdin io.stdout io.tmpfile io.type io.write ")

    _T("math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.cosh math.deg math.exp ")
    _T("math.floor math.fmod math.frexp math.huge math.ldexp math.log math.log10 math.max math.min ")
    _T("math.modf math.pi math.pow math.rad math.random math.randomseed ")
    _T("math.sin math.sinh math.sqrt math.tan math.tanh ")

    _T("abs acos asin atan atan2 ceil cos cosh deg exp ")
    _T("floor fmod frexp huge ldexp log log10 max min ")
    _T("modf pi pow rad random randomseed ")
    _T("sin sinh sqrt tan tanh ")

    _T("new dim set get doc Doc data Data plot Plot open mask sort axis add color ")
    _T("print close title frame label create update format unmask ")
    _T("remove appendcol insertcol appendrow insertrow deletecol deleterow ")

    _T("tic toc ")

    _T("lapack ")

    _T("os.clock os.date os.difftime os.execute os.exit os.getenv ")
    _T("os.remove os.rename os.setlocale os.time os.tmpname ")

    _T("package.cpath package.loaded package.loaders package.loadlib ")
    _T("package.path package.preload package.seeall ")

    _T("string.byte string.char string.dump string.find string.format string.gmatch ")
    _T("string.gsub len lower match rep reverse sub upper ")

    _T("table.concat table.insert table.pack table.remove table.sort table.unpack ");

const LanguageInfo CScriptEdit::LANGPREFS =
{
    _T("Lua"),
    _T("*.lua"),
    SCINTILLA_LEX_LUA,
    {   { SCINTILLA_LUA_DEFAULT, NULL },
        { SCINTILLA_LUA_COMMENT, NULL },
        { SCINTILLA_LUA_COMMENTLINE, NULL },
        { SCINTILLA_LUA_COMMENTDOC, NULL },
        { SCINTILLA_LUA_NUMBER, NULL },
        { SCINTILLA_LUA_WORD, CScriptEdit::WORDLIST },
        { SCINTILLA_LUA_STRING, NULL },
        { SCINTILLA_LUA_CHARACTER, NULL },
        { SCINTILLA_LUA_LITERALSTRING, NULL },
        { SCINTILLA_LUA_PREPROCESSOR, NULL },
        { SCINTILLA_LUA_OPERATOR, NULL },
        { SCINTILLA_LUA_IDENTIFIER, NULL },
        { SCINTILLA_LUA_STRINGEOL, NULL },
        { SCINTILLA_LUA_WORD2, NULL },
        { SCINTILLA_LUA_WORD3, NULL },
        { SCINTILLA_LUA_WORD4, NULL },
        { SCINTILLA_LUA_WORD5, WORDLIST_API },
        { SCINTILLA_LUA_WORD6, NULL },
        { SCINTILLA_LUA_WORD7, NULL },
        { SCINTILLA_LUA_WORD8, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL },
        { -1, NULL }
    },
    0
};

const int CScriptEdit::AUTOC_API_COUNT = 12;
const AutoCList CScriptEdit::AUTOC_API[] = {
    { 3, "io.", "close\nflush\ninput\nlines\nopen\noutput\npopen\nread\nstderr\nstdin\nstdout\ntmpfile\ntype\nwrite" },
    { 3, "os.", "clock\ndate\ndifftime\nexecute\nexit\ngetenv\nremove\nrename\nsetlocale\ntime\ntmpname" },
    { 5, "math.", "abs\nacos\nasin\natan\natan2\nceil\ncos\ncosh\ndeg\nexp\nfloor\nfmod\nfrexp\nhuge\nldexp\nlog\nlog10\nmax\nmin\nmath.modf\npi\npow\nrad\nrandom\nrandomseed\nmath.sin\nsinh\nsqrt\ntan\ntanh" },
    { 5, "file:", "close\nflush\nlines\nread\nseek\nsetvbuf\nwrite" },
    { 6, "table.", "concat\ninsert\npack\nremove\nsort\nunpack" },
    { 7, "string.", "byte\nchar\ndump\nfind\nformat\ngmatch\ngsub\nlen\nlower\nmatch\nrep\nreverse\nsub\nupper" },
    { 10, "coroutine.", "create\nresume\nrunning\nstatus\nwrap\nyield" },
    { 8, "package.", "cpath\nloaded\nloaders\nloadlib\npath\npreload\nseeall" },
    { 4, "doc:", "create\nnew\nfind\nprint\nsave\nopen\nupdate\nclose" },
    { 5, "data:", "create\nnew\ndim\nappendcol\ninsertcol\nformat\nappendrow\ninsertrow\ndeletecol\ndeleterow\nset\nget\nmask\nunmask\nsort" },
    { 5, "plot:", "create\nnew\ntitle\nframe\nadd\nremove\naxis\nlabel\ncolor" },
    { 5, "data.", "min\nmax\nsum\nmean\nmedian\nvar\ndev\ncoeff\nrms\nskew\nkurt\nsort\nfit" }
};

const int CScriptEdit::CALLTIP_API_COUNT = 28;
const CallTipList CScriptEdit::CALLTIP_API[] = {
    { 5, ":new(", "Doc:new(name)\nCreate a new datasheet" },
    { 6, ":find(", "Doc:find(name)\nFind an existing document" },
    { 7, ":print(", "Doc:print()\nPrint the current document" },
    { 6, ":save(", "Doc:save()\nSave the current document" },
    { 6, ":open(", "Doc:open()\nOpen a document" },
    { 8, ":update(", "Doc:update()\nUpdate the current document" },
    { 7, ":close(", "Doc:close()\nClose the current document" },
    { 5, ":dim(", "Data:dim()\nGet the number of columns and rows" },
    { 11, ":appendcol(", "Data:appendcol(dim)\nAppend a column to the datasheet" },
    { 11, ":insertcol(", "Data:insertcol(after, dim)\nInsert a column into the datasheet" },
    { 8, ":format(", "Data:format(name, fmt)\nSet the column numeric format (ex: format(\"A\", \"%.3f\"))" },
    { 11, ":appendrow(", "Data:appendrow(count)\nAppend rows to the datasheet" },
    { 11, ":insertrow(", "Data:insertrow(iafter)\nInsert a row into the datasheet" },
    { 11, ":deletecol(", "Data:deletecol(name)\nDelete column" },
    { 11, ":deleterow(", "Data:deleterow(index)\nDelete row" },
    { 5, ":set(", "Data:set(name, index, value)\nSet cell value in the named column\nData:set(name, tab)\nSet column values with table" },
    { 5, ":get(", "Data:get(name, index)\nGet cell value from the named column (returns value)\nData:get(name)\nGet the column (returns table)" },
    { 6, ":mask(", "Data:mask(name, index)\nMask cell value in the named column" },
    { 8, ":unmask(", "Data:unmask(name, index)\nUnmask cell value in the named column" },
    { 6, ":sort(", "Data:sort(name, asc)\nSort column in as(des)cending order (asc = 1 for ascending)" },
    { 7, ":title(", "Plot:title(text)\nSet the graph title" },
    { 7, ":frame(", "Plot:title(backred, backgreen, backblue, plotred, plotgreen, plotblue)\nSet the graph colors" },
    { 5, ":add(", "Plot:add(namex, namey, istyle, iaxis)\nAdd curve to the graph:\nnamex, namey --> name of the X and Y columns;\nistyle = 1 for line, 2 for scatter and 3 for both;\niaxis = 1 for XY axis, 2 for XY2, 3 for X2Y and 4 for X2Y2" },
    { 8, ":remove(", "Plot:remove(name)\nRemove curve from the graph" },
    { 6, ":axis(", "Plot:axis(iaxis, iscale, iautoscale, fmin, fmax, ititle, stitle)\nSet the axis properties:\niaxis = 1 for X, 2 for Y, 3 for X2 and 4 for Y2;\niscale = 1 for linear and 2 for log; iautoscale = 1 if AutoScale and 0 otherwise;\nfmin, fmax = axis min and max; ititle = 1 if title visible and 0 otherwise;\nstitle = axis title" },
    { 7, ":label(", "Plot:remove(iaxis, ilabel)\nActivate the axis label:\niaxis = 1 for X, 2 for Y, 3 for X2 and 4 for Y2;\nilabel = 1 to activate and 0 to hide" },
    { 10, "data.sort(", "ts = data.sort(t, asc)\nSort table t in as(des)cending order (asc = 1 for ascending)\nReturns the sorted table" },
    { 9, "data.fit(", "pars, chi, iters, msg = data.fit(model, x, y, fpar, ipar, tol, iters)\nWhere:\nmodel: name (as \"Linear\", \"Gauss1\", ...)\nx, y: data to fit\nfpar: initial fitting parameters table\nipar(optional): table with value 1 for varying parameter or 0 for fixed one\ntol(optional): relative tolerance\niters(optional): maximum number of iterations\nReturns parameters pars, chi value, iterations iters and message msg" }
};

const StyleInfoDefault CScriptEdit::STYLEPREFS[] = {

    // SCRIPT_TYPE_DEFAULT
    {
        _T("Default"),
        RGB(0x00, 0x00, 0x00), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_COMMENT
    {
        _T("Comment"),
        RGB(0x3C, 0x8C, 0x32), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_COMMENTLINE
    {
        _T("CommentLine"),
        RGB(0x3C, 0x8C, 0x32), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_COMMENTDOC
    {
        _T("CommentDoc"),
        RGB(0x3C, 0x8C, 0x32), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_NUMBER
    {
        _T("Number"),
        RGB(0x00, 0x80, 0x80), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD
    {
        _T("Keyword"),
        RGB(0x00, 0x00, 0xC0), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, SCRIPT_STYLE_BOLD, 0
    },

    // SCINTILLA_LUA_STRING
    {
        _T("String"),
        RGB(0xDC, 0x0C, 0xC8), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_CHARACTER
    {
        _T("Character"),
        RGB(0xDC, 0x0C, 0xC8), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_LITERALSTRING
    {
        _T("LitteralString"),
        RGB(0xDC, 0x0C, 0xC8), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_PREPROCESSOR
    {
        _T("Preprocessor"),
        RGB(0x00, 0x00, 0x00), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },
    
    // SCINTILLA_LUA_OPERATOR
    {
        _T("Operator"),
        RGB(0x00, 0x00, 0x00), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_IDENTIFIER
    {
        _T("Identifier"),
        RGB(0x00, 0x00, 0x00), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_STRINGEOL
    {
        _T("CharacterEOL"),
        RGB(0xDC, 0x0C, 0xC8), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD2
    {
        _T("Word2"),
        RGB(0x78, 0x28, 0xE6), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD3
    {
        _T("Word3"),
        RGB(0x78, 0x28, 0xE6), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD4
    {
        _T("Word4"),
        RGB(0x78, 0x28, 0xE6), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD5
    {
        _T("Word5"),
        RGB(0x78, 0x28, 0xE6), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD6
    {
        _T("Word6"),
        RGB(0x78, 0x28, 0xE6), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD7
    {
        _T("Word7"),
        RGB(0x78, 0x28, 0xE6), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_WORD8
    {
        _T("Word8"),
        RGB(0x78, 0x28, 0xE6), RGB(0xFF, 0xFF, 0xFF),
        _T("Consolas"), 10, 0, 0
    },

    // SCINTILLA_LUA_LINENUMBER
    {
        _T("LineNumber"),
        RGB(0x3D, 0x8F, 0xB9), RGB(0xEF, 0xF7, 0xF7),
        _T("Consolas"), 10, 0, 0
    }
};

// CScriptEdit

IMPLEMENT_DYNAMIC(CScriptEdit, CScintillaCtrl)

CScriptEdit::CScriptEdit() : CScintillaCtrl()
{
    m_strFilename = _T("");

    for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
        _tcscpy(m_StylePrefs[ii].name, STYLEPREFS[ii].name);
        m_StylePrefs[ii].foreground = STYLEPREFS[ii].foreground;
        m_StylePrefs[ii].background = STYLEPREFS[ii].background;
        _tcscpy(m_StylePrefs[ii].fontname, SigmaApp.isWinXP() ? _T("Courier New") : STYLEPREFS[ii].fontname);
        m_StylePrefs[ii].fontsize = STYLEPREFS[ii].fontsize;
        m_StylePrefs[ii].fontstyle = STYLEPREFS[ii].fontstyle;
        m_StylePrefs[ii].lettercase = STYLEPREFS[ii].lettercase;
    }

    m_LineNrID = 0;
    m_DividerID = 1;
    m_FoldingID = 2;

    // initialize language
    m_pLanguage = NULL;

    m_strSearch = _T("");

    m_iMaxLineWidth = 0;

}

CScriptEdit::~CScriptEdit()
{

}

BOOL CScriptEdit::Create(DWORD dwStyle, const RECT& rcT, CWnd* pParentWnd, UINT nID, DWORD dwExStyle, LPVOID lpParam)
{
    BOOL bRet = CScintillaCtrl::Create(dwStyle, rcT, pParentWnd, nID, dwExStyle, lpParam);

    SetModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT | SC_MOD_CHANGEMARKER);

    ModifyStyleEx(0, WS_EX_STATICEDGE, 0); // Necessary for OnNcPaint to draw borders

    return bRet;
}

// Always call after initCommonPrefs to update line spacing
void CScriptEdit::initCaret(const int iSpacing/* = -1*/)
{
    if (iSpacing >= 0) {
        m_CommonPrefs.lineSpacing = iSpacing;
    }

    int iH = 16;
    CDC* pDC = GetDC();
    if (pDC) {
        CSize sizeT = pDC->GetTextExtent(_T("Qy"), 2);

        ReleaseDC(pDC);

        iH = (int) (sizeT.cy);
        if (iH < 1) {
            iH = 8;
        }
        if (iH > 64) {
            iH = 64;
        }
    }

    int iAscent = 1;
    switch (m_CommonPrefs.lineSpacing) {
        case 1:
            iAscent = 1 + (iH >> 3);
            break;
        case 2:
            iAscent = 1 + (iH >> 2);
            break;
        case 3:
            iAscent = 1 + ((3 * iH) >> 3);
            break;
        default:
            break;
    }

    SetExtraAscent(iAscent);
    SetExtraDescent(iAscent);
}

void CScriptEdit::initCommonPrefs(const CommonInfo &tCommonPrefs)
{
    m_CommonPrefs = tCommonPrefs;

    // default font for all styles
    SetViewEOL(m_CommonPrefs.displayEOLEnable);
    SetIndentationGuides(m_CommonPrefs.indentGuideEnable);
    SetEdgeMode(m_CommonPrefs.longLineOnEnable ? SCINTILLA_EDGE_LINE : SCINTILLA_EDGE_NONE);
    SetViewWS(m_CommonPrefs.whiteSpaceEnable ? SCINTILLA_WS_VISIBLEALWAYS : SCINTILLA_WS_INVISIBLE);
    SetOvertype(m_CommonPrefs.overTypeInitial);
    SetReadOnly(m_CommonPrefs.readOnlyInitial);
    SetWrapMode(m_CommonPrefs.wrapModeInitial ? SCINTILLA_WRAP_WORD : SCINTILLA_WRAP_NONE);
}

bool CScriptEdit::initPrefs(const ScintillaPrefs &tPrefs)
{
    StyleClearAll();

    for (int ii = 0; ii < STYLEINFO_COUNT; ii++) {
        _tcscpy(m_StylePrefs[ii].name, tPrefs.style[ii].name);
        m_StylePrefs[ii].foreground = tPrefs.style[ii].foreground;
        m_StylePrefs[ii].background = tPrefs.style[ii].background;
        _tcscpy(m_StylePrefs[ii].fontname, tPrefs.style[ii].fontname);
        m_StylePrefs[ii].fontsize = tPrefs.style[ii].fontsize;
        m_StylePrefs[ii].fontstyle = tPrefs.style[ii].fontstyle;
        m_StylePrefs[ii].lettercase = tPrefs.style[ii].lettercase;
    }

    // set visibility
    SetVisiblePolicy(SCINTILLA_VISIBLE_STRICT | SCINTILLA_VISIBLE_SLOP, 1);
    SetXCaretPolicy(SCINTILLA_CARET_EVEN | SCINTILLA_VISIBLE_STRICT | SCINTILLA_CARET_SLOP, 1);
    SetYCaretPolicy(SCINTILLA_CARET_EVEN | SCINTILLA_VISIBLE_STRICT | SCINTILLA_CARET_SLOP, 1);

    // markers
    CallScintilla(SCI_MARKERDEFINEPIXMAP, static_cast<WPARAM>(SCINTILLA_MARGIN_SYMBOL), reinterpret_cast<LPARAM>(bookmark_small_xpm), true);

    CallScintilla(SCI_SETMARGINLEFT, 0, static_cast<LPARAM>(4), true);
    CallScintilla(SCI_SETMARGINRIGHT, 0, static_cast<LPARAM>(4), true);

    LanguageInfo const* curInfo = NULL;

    curInfo = &LANGPREFS;

    SetLexer(curInfo->lexer);
    m_pLanguage = curInfo;

    COLORREF clrBack = STYLEINFO_COLOR_BACK_DEFAULT;
    CDC* pDC = GetDC();
    if (NULL != pDC) {
        clrBack = pDC->GetBkColor();
        ReleaseDC(pDC);
    }
    int jj;
    for (jj = 0; jj < SCINTILLA_STYLE_LASTPREDEFINED; jj++) {
        SetStyleFontX(jj, m_StylePrefs[0].foreground, m_StylePrefs[0].background, m_StylePrefs[0].fontsize, (const char_t*) (m_StylePrefs[0].fontname));
    }

    SetStyleFontX(SCINTILLA_STYLE_LINENUMBER, m_StylePrefs[STYLEINFO_INDEX_LINENUM].foreground, m_StylePrefs[STYLEINFO_INDEX_LINENUM].background,
        m_StylePrefs[STYLEINFO_INDEX_LINENUM].fontsize, (const char_t*) (m_StylePrefs[STYLEINFO_INDEX_LINENUM].fontname));
    StyleSetFore(SCINTILLA_STYLE_LINENUMBER, m_StylePrefs[STYLEINFO_INDEX_LINENUM].foreground);
    StyleSetBack(SCINTILLA_STYLE_LINENUMBER, m_StylePrefs[STYLEINFO_INDEX_LINENUM].background);
    StyleSetBold(SCINTILLA_STYLE_LINENUMBER, (m_StylePrefs[STYLEINFO_INDEX_LINENUM].fontstyle & SCRIPT_STYLE_BOLD) > 0);
    StyleSetItalic(SCINTILLA_STYLE_LINENUMBER, (m_StylePrefs[STYLEINFO_INDEX_LINENUM].fontstyle & SCRIPT_STYLE_ITALIC) > 0);
    StyleSetUnderline(SCINTILLA_STYLE_LINENUMBER, (m_StylePrefs[STYLEINFO_INDEX_LINENUM].fontstyle & SCRIPT_STYLE_UNDERL) > 0);
    StyleSetVisible(SCINTILLA_STYLE_LINENUMBER, (m_StylePrefs[STYLEINFO_INDEX_LINENUM].fontstyle & SCRIPT_STYLE_HIDDEN) == 0);
    StyleSetCase(SCINTILLA_STYLE_LINENUMBER, m_StylePrefs[STYLEINFO_INDEX_LINENUM].lettercase);

    MarkerSetFore(SCINTILLA_MARGIN_SYMBOL, STYLEINFO_COLOR_MARKER);
    StyleSetFore(SCINTILLA_STYLE_DEFAULT, m_StylePrefs[STYLEINFO_INDEX_DEFAULT].foreground);
    StyleSetBack(SCINTILLA_STYLE_DEFAULT, clrBack);
    StyleSetFore(SCINTILLA_STYLE_INDENTGUIDE, STYLEINFO_COLOR_INDENT);
    SetSelFore(TRUE, clrBack);

    StyleSetFore(SCINTILLA_STYLE_FOLDINGLINE, STYLEINFO_COLOR_FOLDLINE);
    StyleSetBack(SCINTILLA_STYLE_FOLDINGLINE, clrBack);
    SetWhitespaceFore(TRUE, STYLEINFO_COLOR_WHITESPACE);
    SetWhitespaceBack(FALSE, clrBack);

    SetSelBack(TRUE, ::GetSysColor(COLOR_HIGHLIGHT));

    initCommonPrefs(tPrefs.common);

    if (m_CommonPrefs.syntaxEnable) {
        int keywordnr = 0;
        for (int jj = 0; jj < STYLE_TYPES_COUNT; jj++) {
            if (curInfo->styles[jj].type == -1) {
                continue;
            }
            StyleInfo &curType = m_StylePrefs[curInfo->styles[jj].type];
            SetStyleFontX(jj, curType.foreground, curType.background, curType.fontsize, (const char_t*) (curType.fontname));
            StyleSetFore(curInfo->styles[jj].type, curType.foreground);
            StyleSetBack(curInfo->styles[jj].type, curType.background);
            StyleSetBold(curInfo->styles[jj].type, (curType.fontstyle & SCRIPT_STYLE_BOLD) > 0);
            StyleSetItalic(curInfo->styles[jj].type, (curType.fontstyle & SCRIPT_STYLE_ITALIC) > 0);
            StyleSetUnderline(curInfo->styles[jj].type, (curType.fontstyle & SCRIPT_STYLE_UNDERL) > 0);
            StyleSetVisible(curInfo->styles[jj].type, (curType.fontstyle & SCRIPT_STYLE_HIDDEN) == 0);
            StyleSetCase(curInfo->styles[jj].type, curType.lettercase);
            const char_t *pwords = curInfo->styles[jj].words;
            if (pwords) {
                SetKeyWords(keywordnr, pwords);
                keywordnr += 1;
            }
        }
    }

    m_FoldingMargin = TextWidth(SCINTILLA_STYLE_LINENUMBER, _T("l"));
    SetLayoutCache(SCINTILLA_CACHE_PAGE);

    SetEOLMode(SCINTILLA_EOL_CRLF);
    SetMarginTypeN(m_DividerID, SCINTILLA_MARGIN_SYMBOL);
    SetMarginWidthN(m_DividerID, TextWidth(SCINTILLA_MARGIN_SYMBOL, _T("oo")));
    SetMarginSensitiveN(m_DividerID, TRUE);
    StyleSetBack(m_DividerID, STYLEINFO_COLOR_BACK_LINENUM);

    SetMarginTypeN(m_FoldingID, SCINTILLA_MARGIN_SYMBOL);
    SetMarginMaskN(m_FoldingID, SCINTILLA_MASK_FOLDERS);
    StyleSetBack(m_FoldingID, clrBack);
    SetMarginWidthN(m_FoldingID, 0);
    SetMarginSensitiveN(m_FoldingID, FALSE);
    SetProperty("fold", "0");
    SetProperty("fold.comment", "0");
    SetProperty("fold.compact", "0");
    SetProperty("fold.preprocessor", "0");
    SetProperty("fold.html", "0");
    SetProperty("fold.html.preprocessor", "0");
    SetProperty("fold.comment.lua", "0");
    SetProperty("fold.quotes.lua", "0");

    SetCaretFore(STYLEINFO_COLOR_CARET_FORE);
    SetCaretLineBack(STYLEINFO_COLOR_CARET_BACK);
    SetCaretLineVisible(TRUE);
    SetCaretLineVisibleAlways(TRUE);
    SetCaretStyle(CARETSTYLE_LINE);

    initCaret(tPrefs.common.lineSpacing);

    SetTabWidth(3);
    SetUseTabs(TRUE);
    SetTabIndents(TRUE);
    SetBackSpaceUnIndents(TRUE);
    SetIndent(m_CommonPrefs.indentEnable ? 3 : 0);

    SetEOLMode(SCINTILLA_EOL_CRLF);
    SetEdgeColumn(80);

    AutoCSetSeparator('\n');
    SetMouseDwellTime(1000);

    calcLinenumberMargin();

    StyleSetBack(SCINTILLA_LUA_COMMENT, clrBack);

    return true;
}

BEGIN_MESSAGE_MAP(CScriptEdit, CScintillaCtrl)
    ON_WM_SIZE()
    ON_WM_NCPAINT()
END_MESSAGE_MAP()


// CScriptEdit message handlers

void CScriptEdit::highlightLine(int iLine)
{
    if (iLine == GetLineCount()) {
        iLine -= 1;
    }

    if ((iLine < 0) || (iLine >= GetLineCount())) {
        SetCaretLineVisible(false);
        return;
    }

    GotoLine(iLine);
    SetCaretLineVisible(true);
}

void CScriptEdit::OnSize(UINT nType, int cx, int cy)
{
    CScintillaCtrl::OnSize(nType, cx, cy);
}

void CScriptEdit::OnNcPaint()
{
    CDC* pDC = GetWindowDC();

    CRect rcT;
    GetWindowRect(&rcT);
    rcT.OffsetRect(-rcT.left, -rcT.top);

    CBrush brushT(::GetSysColor(COLOR_3DDKSHADOW));
    pDC->FrameRect(&rcT, &brushT);
    ReleaseDC(pDC);
}

void CScriptEdit::calcLinenumberMargin(int nLinesCount/* = 0*/)
{
    if (m_CommonPrefs.lineNumberEnable == false) {
        return;
    }

    if (nLinesCount < 1) {
        nLinesCount = GetLineCount();
    }
    char_t szBuffer[ML_STRSIZET];
    memset(szBuffer, 0, ML_STRSIZET * sizeof(char_t));
    _tcsprintf(szBuffer, ML_STRSIZET - 1, _T("0%d"), nLinesCount);
    m_LineNrMargin = TextWidth(SCINTILLA_STYLE_LINENUMBER, (const char_t*) szBuffer);
    SetMarginWidthN(m_LineNrID, m_LineNrMargin);
}

void CScriptEdit::OnCharAdded(SCNotification* pSCNotification)
{

    if (m_CommonPrefs.autocompleteEnable) {
        // Get the previous N characters and display the ad hoc autocomplete list
        int nStartSel = GetSelectionStart();
        int nEndSel = GetSelectionEnd();
        if ((nStartSel == nEndSel) && (nStartSel >= 2)) {
            TextRange tr;
            char sText[12];
            sText[0] = '\0';
            tr.lpstrText = sText;
            bool bAutoShow = false;
            int nStart = 0;
            for (int ii = 0; ii < AUTOC_API_COUNT; ii++) {
                nStart = nStartSel - AUTOC_API[ii].len;
                if (nStart < 0) {
                    continue;
                }
                tr.chrg.cpMin = nStart;
                tr.chrg.cpMax = nEndSel;
                GetTextRange(&tr);

                //Display the auto completion list
                if (_strcmpi(sText, AUTOC_API[ii].prefix) == 0) {
                    AutoCSetOrder(SC_ORDER_PERFORMSORT);
                    AutoCShow(0, AUTOC_API[ii].words);
                    bAutoShow = true;
                    break;
                }
            }

            if (bAutoShow == false) {
                for (int ii = 0; ii < CALLTIP_API_COUNT; ii++) {
                    nStart = nStartSel - CALLTIP_API[ii].len;
                    if (nStart < 0) {
                        continue;
                    }
                    tr.chrg.cpMin = nStart;
                    tr.chrg.cpMax = nEndSel;
                    GetTextRange(&tr);

                    //Display the call tips
                    if (_strcmpi(sText, CALLTIP_API[ii].prefix) == 0) {
                        CallTipShow(nEndSel, CALLTIP_API[ii].args);
                        break;
                    }
                }
            }
        }
    }

    int currentLine = GetCurLineIndex();

    CString strT = GetCurLine();
    int iW = TextWidth(SCINTILLA_STYLE_DEFAULT, strT);
    if (iW > m_iMaxLineWidth) {
        m_iMaxLineWidth = iW;
        SetScrollWidth(m_iMaxLineWidth);
    }

    if (pSCNotification->ch == '\n') {
        calcLinenumberMargin();

        int lineInd = 0;
        if (currentLine > 0) {
            lineInd = GetLineIndentation(currentLine - 1);
        }
        if (lineInd == 0) { return; }
        SetLineIndentation(currentLine, lineInd);
        GotoPos(GetLineIndentPosition(currentLine));
    }
}

void CScriptEdit::SetStyleFontX(int iStyle, COLORREF foreColor, COLORREF backColor, int iSize, const char_t* faceName)
{
    StyleSetFore(iStyle, foreColor);
    StyleSetBack(iStyle, backColor);
    if (iSize >= 1) {
        StyleSetSize(iStyle, iSize);
    }
    if (faceName) {
        StyleSetFont(iStyle, faceName);
    }
}

// Call tips
void CScriptEdit::OnDwellStart(SCNotification* pSCNotification)
{
    TextRange tr;
    tr.chrg.cpMin = max(0, pSCNotification->position - 7);
    tr.chrg.cpMax = min(pSCNotification->position + 7, GetLength());
    char sText[15];
    sText[0] = '\0';
    tr.lpstrText = sText;
    GetTextRange(&tr);

    _strupr_s(sText, sizeof(sText));
    if (strstr(sText, "AUTHOR ")) {
        CallTipShow(pSCNotification->position, _T("Sidi OULD SAAD HAMADY"));
    }
}

void CScriptEdit::OnDwellEnd(SCNotification* /*pSCNotification*/)
{
    // Cancel any outstanding call tip
    if (CallTipActive()) {
        CallTipCancel();
    }
}
