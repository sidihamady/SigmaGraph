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
#include "SigmaConsole.h"
#include "SigmaConsoleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSigmaConsoleApp

BEGIN_MESSAGE_MAP(CSigmaConsoleApp, CWinApp)
END_MESSAGE_MAP()


// CSigmaConsoleApp construction

CSigmaConsoleApp::CSigmaConsoleApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
    m_pLib = NULL;
}


// The one and only CSigmaConsoleApp object

CSigmaConsoleApp SigmaApp;

software_t g_Sigmaware;                                // Sigma software informations

void CSigmaConsoleApp::DoEvent(void)
{
    MSG msgFree;
    // Traiter les messages Windows en attente
    while(::PeekMessage(&msgFree, NULL, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msgFree);
        ::DispatchMessage(&msgFree);
    }
}

// CSigmaApp initialization
BOOL CSigmaConsoleApp::getUserDir(void)
{
    memset(m_szMainDir, 0, ML_STRSIZE * sizeof(char_t));
    memset(m_szHelpFilename, 0, ML_STRSIZE * sizeof(char_t));
    memset(m_szUserDir, 0, ML_STRSIZE * sizeof(char_t));

    DWORD dwSize = ::GetModuleFileName(NULL, m_szMainDir, ML_STRSIZE - 1);
    if ((dwSize > 6) && (dwSize < (ML_STRSIZE - 30))) {
        for (int_t ii = dwSize - 1; ii > 0; ii--) {
            if (m_szMainDir[ii] == _T('\\')) {
                m_szMainDir[ii + 1] = _T('\0');
                break;
            }
        }
    }
    else {
        return FALSE;
    }
    char_t *pszT = _tcsstr((char_t*) m_szMainDir, _T("\\bin"));
    if (pszT) {
        *(pszT + 1) = _T('\0');
    }

    _tcscpy(m_szHelpFilename, (const char_t*) m_szMainDir);
    _tcscat(m_szHelpFilename, _T("help\\sigmaconsole.chm"));

    char_t szFilename[ML_STRSIZE];
    memset(szFilename, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szFilename, (const char_t*) m_szMainDir);
    _tcscat(szFilename, _T("setup.txt"));
    m_bPortable = (fileExists((const char_t*) szFilename) == false);

    if (isPortable()) { // Portable distribution
        _tcscpy((char_t*) m_szUserDir, (const char_t*) m_szMainDir);
        _tcscat(m_szUserDir, _T("config\\"));
        makeDir((const char_t*) m_szUserDir);
        if (m_pszProfileName) {
            free((void*) m_pszProfileName);
            m_pszProfileName = NULL;
        }
        _tcscpy((char_t*) szFilename, (const char_t*) m_szUserDir);
        _tcscat((char_t*) szFilename, _T("settings.ini"));
        m_pszProfileName = ::_tcsdup((const char_t*) szFilename);
        return TRUE;
    }
    else if (::SHGetSpecialFolderPath(NULL, m_szUserDir, CSIDL_APPDATA, FALSE) == TRUE) {    // Get AppData Directory
        int_t nret = (int_t) _tcslen(m_szUserDir);
        if ((nret > 6) && (nret < (ML_STRSIZE - 30))) {
            if (m_szUserDir[nret - 1] != _T('\\')) {
                m_szUserDir[nret] = _T('\\');
                m_szUserDir[nret + 1] = _T('\0');
            }
            _tcscat(m_szUserDir, _T("SigmaGraph\\"));
            makeDir((const char_t*) m_szUserDir);
            if (m_pszProfileName) {
                free((void*) m_pszProfileName);
                m_pszProfileName = NULL;
            }
            _tcscpy((char_t*) szFilename, (const char_t*) m_szUserDir);
            _tcscat((char_t*) szFilename, _T("settings.ini"));
            m_pszProfileName = ::_tcsdup((const char_t*) szFilename);
            return TRUE;
        }
    }

    return FALSE;
}


BOOL CSigmaConsoleApp::getOptions()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    console_t *pp = NULL;
    char_t szLoc[ML_STRSIZE];
    DWORD dwT = ML_STRSIZE;

    memset(&m_Console, 0, sizeof(console_t));

    m_Console.angleunit = 0x00;
    m_Console.font.color.r = 0;
    m_Console.font.color.g = 0;
    m_Console.font.color.b = 0;
    _tcscpy(m_Console.font.name, _T("Lucida Sans Unicode"));
    m_Console.font.size = 9;
    m_Console.font.style = 0x00;
    m_Console.tol = m_pLib->tol;
    m_Console.reltol = m_pLib->reltol;

    _tcscpy(szLoc, (const char_t*)m_szMainDir);
    _tcscat(szLoc, _T("ConsoleOptions.par"));

    ilen = (long_t)fl_filesize((const char_t*)szLoc);
    // Le fichier ne correspond pas � une donn�e valide
    if (ilen != (long_t)sizeof(console_t)) {
        return FALSE;
    }

    if ((fp = _wfopen((const char_t*)szLoc, _T("rb"))) == NULL) {
        return FALSE;
    }

    itemlen = (int_t)sizeof(console_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    ilen = (long_t)fread(pitem, itemlen, 1, fp);
    if (ilen != 1) {
        free(pitem); pitem = NULL;
        fclose(fp); fp = NULL;
        return FALSE;
    }

    pp = (console_t*)pitem;

    memcpy(&m_Console, pp, itemlen);

    // Quelques v�rifications
    if ((m_Console.tol < (DBL_EPSILON * 10.0)) || (m_Console.tol > 1.0)) {
        m_Console.tol = m_pLib->tol;
    }
    else {
        m_pLib->tol = m_Console.tol;
    }
    if ((m_Console.reltol < (DBL_EPSILON * 10.0)) || (m_Console.reltol > 1.0)) {
        m_Console.reltol = m_pLib->reltol;
    }
    else {
        m_pLib->reltol = m_Console.reltol;
    }
    //

    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    return TRUE;
}

BOOL CSigmaConsoleApp::setOptions()
{
    FILE *fp = NULL;
    char* pitem = NULL;
    long_t itemlen = 0, ilen = 0;
    char_t szLoc[ML_STRSIZE];
    DWORD dwT = ML_STRSIZE;

    _tcscpy(szLoc, (const char_t*)m_szMainDir);
    _tcscat(szLoc, _T("ConsoleOptions.par"));

    if ((fp = _wfopen((const char_t*)szLoc, _T("wb"))) == NULL) {
        return FALSE;
    }

    itemlen = (long_t)sizeof(console_t);
    pitem = (char*)malloc(itemlen);
    if (pitem == NULL) {
        fclose(fp); fp = NULL;
        return FALSE;
    }
    memcpy(pitem, &m_Console, itemlen);
    fwrite(pitem, itemlen, 1, fp);

    fclose(fp); fp = NULL;
    free(pitem); pitem = NULL;

    return TRUE;
}

// CSigmaConsoleApp initialization

BOOL CSigmaConsoleApp::InitInstance()
{
    // -----------------------------------------------------------------------------
    // Information sur le logiciel
    g_Sigmaware.version = SIGMA_SOFTWARE_VERSION;
    _tcscpy(g_Sigmaware.name, SIGMA_SOFTWARE_NAME);
    _tcscpy(g_Sigmaware.description, SIGMA_SOFTWARE_DESCRIPTION);
    _tcscpy(g_Sigmaware.copyright, SIGMA_SOFTWARE_COPYRIGHT);
    _tcscpy(g_Sigmaware.date, SIGMA_SOFTWARE_DATERELEASE);
    _tcscpy(g_Sigmaware.time, SIGMA_SOFTWARE_TIMERELEASE);
    _tcscpy(g_Sigmaware.reserved, SIGMA_SOFTWARE_RESERVED);
    // -----------------------------------------------------------------------------

    if (m_pszAppName) {
        free((void*)m_pszAppName);
        m_pszAppName = NULL;
    }
    m_pszAppName = _tcsdup(_T("SigmaConsole"));

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // Interaction with Lib
    if (m_pLib == NULL) {
        m_pLib = new libvar_t;
        if (m_pLib == NULL) {    // Error handling
            return FALSE;
        }
    }
    ASSERT(m_pLib);
    ml_reset(m_pLib);

    // Numeric format
    m_bFormat = 0x00;
    _tcscpy(m_szFormatF, ML_FORMAT_FSHORT);
    _tcscpy(m_szFormatE, ML_FORMAT_ESHORT);
    memset(ml_format_f, 0, ML_STRSIZET * sizeof(char_t));
    memset(ml_format_e, 0, ML_STRSIZET * sizeof(char_t));
    _tcscpy(ml_format_f, ML_FORMAT_FSHORT);
    _tcscpy(ml_format_e, ML_FORMAT_ESHORT);
    
    m_bRunning = FALSE;

    // User preferences
    getUserDir();
    getOptions();
    ml_angleunit = m_Console.angleunit;
    
    // Performance counter
    m_liPerfCount.HighPart = 0;
    m_liPerfCount.LowPart = 0;
    m_liPerfCount.QuadPart = 0;
    m_uiPerfCount = 0;
    
    // Parser
    m_nVarnum = 0;
    const char_t *szAZ[_T('z') - _T('a') + 1] = {_T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("h"), _T("i"), _T("j"), _T("k"), _T("l"), _T("m"),
        _T("n"), _T("o"), _T("p"), _T("q"), _T("r"), _T("s"), _T("t"), _T("u"), _T("v"), _T("w"), _T("x"), _T("y"), _T("z")};

    for (int_t ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {    
        m_fParam[ii] = 0.0;
        _tcscpy(m_szParam[ii], szAZ[ii]);        // Params name : by default : a, b, c, ..., z
        m_bParam[ii] = FALSE;                        // Param set ?
    }

    m_fAns = 0.0;
    m_bAns = false;
    
    CSigmaConsoleDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}


int_t CSigmaConsoleApp::CStringToChar(CString strSrc, char_t* szDest, int_t nMax)
{
    int_t nlen = 0;

    if ((nMax < 1) || (nMax >= ML_STRSIZE)) {
        return -1;
    }

    nlen = strSrc.GetLength();
    if ((nlen < 0) || (nlen >= ML_STRSIZE)) {
        return -1;
    }
    if (nlen == 0) {
        szDest[0] = _T('\0');
        return 0;
    }

    for (int_t ii = 0; ii < nlen; ii++) {
        szDest[ii] = (char_t)strSrc[ii];
    }
    szDest[nlen] = _T('\0');

    return nlen;
}

BOOL CSigmaConsoleApp::GetFont(HWND hWnd, LOGFONT *plf)
{ 
    CHOOSEFONT cf; 
 
    cf.lStructSize = sizeof(CHOOSEFONT); 
    cf.hwndOwner = hWnd; 
    cf.hDC = (HDC)NULL; 
    cf.lpLogFont = plf; 
    cf.iPointSize = 0; 
    cf.Flags = CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS; 
    cf.rgbColors = RGB(0,0,0); 
    cf.lCustData = 0L; 
    cf.lpfnHook = (LPCFHOOKPROC)NULL; 
    cf.lpTemplateName = NULL; 
    cf.hInstance = (HINSTANCE) NULL; 
    cf.lpszStyle = NULL; 
    cf.nFontType = SCREEN_FONTTYPE; 
    cf.nSizeMin = 0; 
    cf.nSizeMax = 0; 

    if (::ChooseFont(&cf)) {
        return TRUE;
    }

    return FALSE;
}

BOOL CSigmaConsoleApp::GetColor(HWND hWnd, COLORREF *pcr)
{
    CHOOSECOLOR cc;
    COLORREF acrCustClr[16];

    ::ZeroMemory(&cc, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hWnd;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = *pcr;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (::ChooseColor(&cc)) {
        *pcr = cc.rgbResult;
        return TRUE;
    }

    return FALSE;
}

void CSigmaConsoleApp::Reformat(real_t fVal, char_t* szBuffer, int_t nCount, char_t* pszFormat/* = NULL*/, int_t *pFmt/* = NULL*/)
{
    real_t ftmp = fabs(fVal);
    int_t iFmt = 0;

    if ((nCount < 1) || (nCount > ML_STRSIZE)) {
        return;
    }

    if (m_bFormat == 0x09) {
        _tcsprintf(szBuffer, nCount, _T("%.0f"), fVal);
        return;
    }

    if ((ftmp > 1e-9) && (ftmp < 1e9)) {
        _tcsprintf(szBuffer, nCount, (const char_t*)(m_szFormatF), fVal);
        if (pszFormat != NULL) {
            _tcscpy(pszFormat, (const char_t*)(m_szFormatF));
            if (pFmt != NULL) {
                *pFmt = 0;
            }
        }
    }
    else if (ftmp == 0.0) {
        _tcsprintf(szBuffer, nCount, (const char_t*)(m_szFormatF), fVal);
        if (pszFormat != NULL) {
            _tcscpy(pszFormat, (const char_t*)(m_szFormatF));
            if (pFmt != NULL) {
                *pFmt = 0;
            }
        }
    }
    else {
        _tcsprintf(szBuffer, nCount, (const char_t*)(m_szFormatE), fVal);
        if (pszFormat != NULL) {
            _tcscpy(pszFormat, (const char_t*)(m_szFormatE));
            if (pFmt != NULL) {
                *pFmt = 1;
            }
        }
    }

    SigmaApp.m_pLib->errcode = 0;
    ml_reformat(szBuffer);

    if ((pszFormat != NULL) && (pFmt != NULL) && (fVal != 0.0)) {
        if (*pFmt == 0) {
            ftmp = ml_toreal(szBuffer, NULL, NULL);
            if (ftmp == 0.0) {
                _tcscpy(pszFormat, (const char_t*)(m_szFormatE));
                *pFmt = 1;
            }
        }
    }
}

BOOL CSigmaConsoleApp::LogfontToFont(LOGFONT lft, font_t *pfnt)
{
    if (pfnt == NULL) {
        return FALSE;
    }

    HDC hDCX = ::GetDC(NULL);
    if (hDCX) {
        pfnt->size = -MulDiv(lft.lfHeight, 72, ::GetDeviceCaps(hDCX, LOGPIXELSY));
        ::ReleaseDC(NULL, hDCX);
    }

    _tcscpy(pfnt->name, (const char_t*)(lft.lfFaceName));
    if (lft.lfWeight == FW_BOLD) {
        pfnt->style = 0x10;
    }
    else {
        pfnt->style = 0x00;
    }
    if (lft.lfItalic) {
        pfnt->style |= 0x01;
    }

    return TRUE;
}

BOOL CSigmaConsoleApp::FontToLogfont(font_t fnt, LOGFONT *plft)
{
    if (plft == NULL) {
        return FALSE;
    }

    HDC hDCX = ::GetDC(NULL);
    if (hDCX) {
        plft->lfHeight = -MulDiv(fnt.size, ::GetDeviceCaps(hDCX, LOGPIXELSY), 72);
        ::ReleaseDC(NULL, hDCX);
    }

    _tcscpy(plft->lfFaceName, (const char_t*)(fnt.name));
    if (fnt.style & 0x10) {
        plft->lfWeight = FW_BOLD;
    }
    else {
        plft->lfWeight = FW_NORMAL;
    }
    if (fnt.style & 0x01) {
        plft->lfItalic = 1;
    }

    return TRUE;
}

// Performance counter
void CSigmaConsoleApp::CounterStart()
{
    m_uiPerfCount = 0;
    m_liPerfCount.HighPart = 0;
    m_liPerfCount.LowPart = 0;
    m_liPerfCount.QuadPart = 0;
    ::QueryPerformanceCounter(&m_liPerfCount);
}
void CSigmaConsoleApp::CounterStop()
{
    LARGE_INTEGER lstop, lfreq;

    if (m_liPerfCount.QuadPart == 0) {
        m_liPerfCount.HighPart = 0;
        m_liPerfCount.LowPart = 0;
        m_uiPerfCount = 0;
        return;
    }

    ::QueryPerformanceCounter(&lstop);
    ::QueryPerformanceFrequency(&lfreq);

    lstop.QuadPart -= m_liPerfCount.QuadPart;
    lstop.QuadPart *= 1000000;
    lstop.QuadPart /= lfreq.QuadPart;

    if (lstop.HighPart != 0) {
        m_uiPerfCount = 0;
    }
    else {
        m_uiPerfCount = (ulong_t)(lstop.LowPart);
    }
}

// >> GLOBAL C-STYLE FUNCTIONS
char_t* g_pszFunction = NULL;
real_t funfx(real_t x)
{
    if (g_pszFunction == NULL) {
        return 0.0;
    }
    return ml_parser_evalfx(g_pszFunction, x, SigmaApp.m_pLib);
}
// <<


void CSigmaConsoleApp::ResizeParam(void)
{
    const char_t *szAZ[_T('z') - _T('a') + 1] = {_T("a"), _T("b"), _T("c"), _T("d"), _T("e"), _T("f"), _T("g"), _T("h"), _T("i"), _T("j"), _T("k"), _T("l"), _T("m"),
        _T("n"), _T("o"), _T("p"), _T("q"), _T("r"), _T("s"), _T("t"), _T("u"), _T("v"), _T("w"), _T("x"), _T("y"), _T("z")};

    if (m_nVarnum != (_T('z') - _T('a') + 1)) {
        return;
    }

    for (int_t ii = 0; ii < (_T('z') - _T('a')); ii++) {    
        m_fParam[ii] = m_fParam[ii + 1];
        _tcscpy(m_szParam[ii], m_szParam[ii + 1]);            // Params name : by default : a, b, c, ..., z
        m_bParam[ii] = m_bParam[ii + 1];                            // Param set ?
    }
    m_fParam[_T('z') - _T('a')] = 0.0;
    _tcscpy(m_szParam[_T('z') - _T('a')], szAZ[_T('z') - _T('a')]);        // Params name : by default : a, b, c, ..., z
    m_bParam[_T('z') - _T('a')] = FALSE;
    m_nVarnum -= 1;
}

BOOL CSigmaConsoleApp::DoParse(const char_t* szInput, char_t* szOutput, real_t* pVal/* = NULL*/, const char_t* pszFormat/* = NULL*/)
{
    CString strExpr;
    int_t nlen, nslen, nplen, nvlen = 0, ii, jj, ll, ne, ns;
    char_t szExpression[ML_STRSIZE], szFunction[ML_STRSIZE], szTmp[ML_STRSIZE],
        szVar[ML_STRSIZET], *pszFunction = NULL, *pszTT = NULL;
    char_t szBuffer[ML_STRSIZE], *pszT = NULL;
    real_t ftmp;
    enum _iParse {MATH = 0, COMMAND} iParse;
    enum _iOp {EXPR = 0, ASSIGN, PRINT, SOLVE} iOp;
    char_t cVar;
    BOOL bFound = FALSE, bRep = FALSE, bMathParse = FALSE;

    // Verifications
    if (szOutput == NULL) {
        return FALSE;
    }

    szOutput[0] = _T('\0');

    if (pVal) { *pVal = 0.0; }

    nlen = (int_t)_tcslen((const char_t*)szInput);
    if (nlen < 1) {
        _tcscpy(szOutput, _T("! Empty expression."));
        return FALSE;
    }

    if (nlen > 250) {
        _tcscpy(szOutput, _T("! Too long expression."));
        return FALSE;
    }

    if ((char_t*)_tcsstr((const char_t*)szInput, _T("\"")) != NULL) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    _tcsncpy(szExpression, szInput, nlen);
    szExpression[nlen] = _T('\0');
    nlen = (int_t)_tcslen((const char_t*)szExpression);
    if (nlen < 1) {
        _tcscpy(szOutput, _T("! Empty expression."));
        return FALSE;
    }

    int_t ispace = ml_remspace(szExpression);
    if ((ispace > 0) && (ispace < nlen)) {
        nlen -= ispace;
        if (nlen < 1) {
            _tcscpy(szOutput, _T("! Empty expression."));
            return FALSE;
        }
    }

    _tcscpy(szOutput, _T("! Invalid expression."));

    // Command parsing
    iParse = COMMAND;
    if (_tcsicmp((const char_t*)szExpression, _T("author")) == 0) {
        _tcscpy(szOutput, _T("Sidi OULD SAAD HAMADY\r\nsidi@hamady.org"));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("help")) == 0) {
        HWND hWnd = ::HtmlHelp(0, (LPCTSTR) m_szHelpFilename, HH_DISPLAY_TOC, NULL);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("exit")) == 0) {
        OnCmdMsg(ID_APP_EXIT, 0, 0, 0);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("quit")) == 0) {
        OnCmdMsg(ID_APP_EXIT, 0, 0, 0);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format long")) == 0) {
        _tcscpy(m_szFormatF, ML_FORMAT_FLONG);
        _tcscpy(m_szFormatE, ML_FORMAT_ELONG);
        m_bFormat = 0x00;
        _tcscpy(ml_format_f, ML_FORMAT_FLONG);
        _tcscpy(ml_format_e, ML_FORMAT_ELONG);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format short")) == 0) {
        _tcscpy(m_szFormatF, ML_FORMAT_FSHORT);
        _tcscpy(m_szFormatE, ML_FORMAT_ESHORT);
        m_bFormat = 0x00;
        _tcscpy(ml_format_f, ML_FORMAT_FSHORT);
        _tcscpy(ml_format_e, ML_FORMAT_ESHORT);
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format int")) == 0) {
        m_bFormat = 0x09;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("format auto")) == 0) {
        m_bFormat = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("tol ?")) == 0) {
        Reformat(m_pLib->tol, szOutput, ML_STRSIZE - 1);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("iter ?")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d"), SigmaApp.m_pLib->maxiter);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("radian")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("rad")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit radian")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit rad")) == 0) {
        ml_angleunit = 0x00;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("degree")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("deg")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit degree")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit deg")) == 0) {
        ml_angleunit = 0x01;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("grad")) == 0) {
        ml_angleunit = 0x02;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit grad")) == 0) {
        ml_angleunit = 0x02;
        _tcscpy(szOutput, _T("done."));
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("unit ?")) == 0) {
        if (ml_angleunit == 0x00) {
            _tcscpy(szOutput, _T("angle unit: radian."));
        }
        else if (ml_angleunit == 0x01) {
            _tcscpy(szOutput, _T("angle unit: degree."));
        }
        else if (ml_angleunit == 0x02) {
            _tcscpy(szOutput, _T("angle unit: grad."));
        }
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("var"))) != NULL) {
        ((CSigmaConsoleDlg*)m_pMainWnd)->OnViewVarlist();
        _tcscpy(szOutput, _T("done."));
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("unset all"))) != NULL) {
        for (char_t cc = _T('a'); cc <= _T('z'); cc++) {
            m_bParam[cc - _T('a')] = FALSE;
            m_szParam[cc - _T('a')][0] = cc;
            m_szParam[cc - _T('a')][1] = _T('\0');
        }
        m_nVarnum = 0;
        _tcscpy(szOutput, _T("done."));
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("unset "))) != NULL) {
        if ((pszT += _tcslen(_T("unset "))) != NULL) {
            for (char_t cc = _T('a'); cc <= _T('z'); cc++) {
                if (_tcsicmp((const char_t*)pszT, (const char_t*)m_szParam[cc - _T('a')]) == 0) {
                    m_bParam[cc - _T('a')] = FALSE;
                    m_szParam[cc - _T('a')][0] = cc;
                    m_szParam[cc - _T('a')][1] = _T('\0');
                    m_nVarnum -= 1;
                    _tcscpy(szOutput, _T("done."));
                    break;
                }
            }
        }
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("set ?")) == 0) {
        szOutput[0] = _T('\0');
        ii = 0;
        int_t nT;
        for (char_t cc = _T('a'); cc <= _T('z'); cc++) {
            if (m_bParam[cc - _T('a')] == TRUE) {    // Var set
                nT = _tcslen((const char_t*)szOutput);
                if (nT > (ML_STRSIZE - ML_STRSIZES - 3)) {
                    _tcscat(szOutput, _T("...,"));
                    break;
                }
                _tcscat(szOutput, (const char_t*)m_szParam[cc - _T('a')]);
                _tcscat(szOutput, _T(","));
            }
        }
        nT = _tcslen((const char_t*)szOutput);
        if (nT < 2) {
            _tcscpy(szOutput, _T("variable not set."));
        }
        else {
            szOutput[nT - 1] = _T('\0');
        }
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("reset")) == 0) {
        for (jj = 0; jj < ML_PARSER_VARNUM; jj++) {
            m_bParam[jj] = FALSE;
            m_fParam[jj] = 0.0;
            memset(m_szParam[jj], 0, ML_STRSIZET * sizeof(char_t));
        }
        m_nVarnum = 0;
        _tcscpy(szOutput, _T("done."));
        //
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("name")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.name);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("version")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d.%d.%d"), (g_Sigmaware.version >> 12) & 0x000F, (g_Sigmaware.version >> 8) & 0x000F, (g_Sigmaware.version >> 4) & 0x000F);
#ifdef SIGMA_SOFTWARE_VERSION_B
        int_t ilenx = (int_t)_tcslen(SIGMA_SOFTWARE_VERSION_B);
        if ((ilenx > 0) && (ilenx < ML_STRSIZET)) {
            _tcscat(szOutput, _T("-"));
            _tcscat(szOutput, SIGMA_SOFTWARE_VERSION_B);
#endif
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("release")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.date);
        _tcscat(szOutput, _T(" "));
        _tcscat(szOutput, g_Sigmaware.time);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("copyright")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.copyright);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("description")) == 0) {
        _tcscpy(szOutput, g_Sigmaware.description);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("libmath version")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d.%d.%d"), (ml_version >> 12) & 0x000F, (ml_version >> 8) & 0x000F, (ml_version >> 4) & 0x000F);
    }
    else if (_tcsicmp((const char_t*)szExpression, _T("libfile version")) == 0) {
        _tcsprintf(szOutput, ML_STRSIZE - 1, _T("%d.%d.%d"), (fl_version >> 12) & 0x000F, (fl_version >> 8) & 0x000F, (fl_version >> 4) & 0x000F);
    }
    else {
        iParse = MATH;
    }
    // Valid command found
    if (iParse != MATH) {
        return TRUE;
    }

    // Mathematical parsing

    memset((char_t*)szVar, 0, sizeof(char_t) * ML_STRSIZET);
    memset((char_t*)szFunction, 0, sizeof(char_t) * ML_STRSIZE);
    jj = 0;    ne = 0;    ns = 0;
    int_t nVarNotSet = 0;
    for (ii = 0; ii < nlen; ii++) {
        if ((szExpression[ii] != _T('\r')) && (szExpression[ii] != _T('\n'))
             && (szExpression[ii] != _T('\t')) && (szExpression[ii] != _T(' '))) {
            szFunction[jj] = _totlower(szExpression[ii]);
            if ((szFunction[jj] == _T('=')) && (ne == 0)) {
                ne = jj;
            }
            if ((jj >= 2) && (jj < nlen) && (ns == 0)) {
                if ((szFunction[jj - 2] == _T('-')) && (szFunction[jj - 1] == _T('>')) && (szFunction[jj] == _T('0'))) {
                    ns = jj - 2;
                }
            }
            jj += 1;
        }

        // V�rification que les variables de l'expression sont d�finies
        // !!! � revoir (cf. les cas particuliers tels que 'e'...)
        if (ii == 0) {
            if ((szExpression[ii] >= _T('a')) && (szExpression[ii] <= _T('z'))
                && (_istalpha(szExpression[ii + 1]) == FALSE) && (_istdigit(szExpression[ii + 1]) == FALSE)
                && (szExpression[ii + 1] != _T('='))) {
                ll = (int_t)(szExpression[ii] - _T('a'));
                if (m_bParam[ll] == FALSE) {    // Var not set
                    if (szExpression[ii] != _T('x')) {
                        _tcscpy(szOutput, _T("! Variable not set."));
                        return FALSE;
                    }
                    nVarNotSet = 1;
                }
            }
        }
        else if (ii == (nlen - 1)) {
            if ((szExpression[ii] >= _T('a')) && (szExpression[ii] <= _T('z'))
                && (_istalpha(szExpression[ii - 1]) == FALSE) && (_istdigit(szExpression[ii - 1]) == FALSE)
                && (szExpression[ii - 1] != _T('_'))) {
                ll = (int_t)(szExpression[ii] - _T('a'));
                if (m_bParam[ll] == FALSE) {    // Var not set
                    _tcscpy(szOutput, _T("! Variable not set."));
                    return FALSE;
                }
            }
        }
        else {
            if ((szExpression[ii] >= _T('a')) && (szExpression[ii] <= _T('z'))
                && (_istalpha(szExpression[ii - 1]) == FALSE) && (_istdigit(szExpression[ii - 1]) == FALSE)
                && (_istalpha(szExpression[ii + 1]) == FALSE) && (_istdigit(szExpression[ii + 1]) == FALSE)
                && (szExpression[ii-1] != _T('_')) && (szExpression[ii+1] != _T('='))) {
                ll = (int_t)(szExpression[ii] - _T('a'));
                if (m_bParam[ll] == FALSE) {    // Var not set
                    if (szExpression[ii] != _T('x')) {
                        _tcscpy(szOutput, _T("! Variable not set."));
                        return FALSE;
                    }
                    nVarNotSet = 1;
                }
            }
        }
    }

    nlen = jj;
    szFunction[nlen] = _T('\0');
    if ((nlen < 1) || (nlen > ML_STRSIZE - 7)) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    memset((char_t*)szExpression, 0, sizeof(char_t) * ML_STRSIZE);
    _tcscpy((char_t*)szExpression, (const char_t*)szFunction);

    iOp = EXPR;

    if ((nVarNotSet == 1) && (ns == 0)) {
        _tcscpy(szOutput, _T("! Variable not set."));
        return FALSE;
    }

    if (ns > 0) {                   // SOLVE
        szExpression[ns] = _T('\0');
        szFunction[ns] = _T('\0');
        iOp = SOLVE;
    }
    else if ((pszT = (char_t*)_tcsstr((const char_t*)szExpression, _T("solve("))) != NULL) {
        int ill = (int)_tcslen((const char_t*)szExpression);
        if (ill > 6) {
            char_t szTTT[ML_STRSIZE];
            szExpression[ill - 1] = _T('\0');
            pszT = szExpression;
            pszT += 6;
            memset((char_t*)szTTT, 0, sizeof(char_t) * ML_STRSIZE);
            _tcscpy((char_t*)szTTT, (const char_t*)pszT);
            _tcscpy((char_t*)szExpression, (const char_t*)szTTT);
            _tcscpy((char_t*)szFunction, (const char_t*)szExpression);
            iOp = SOLVE;
        }
    }
    else if ((ne > 0) && (ne < ML_STRSIZE)) {                                            // '=' found
        if (ne >= ML_STRSIZET) {    
            _tcscpy(szOutput, _T("! Invalid variable name (too long)."));
            return FALSE;
        }
        if (ne < (nlen - 1)) {                                                           // ASSIGN
            for (ii = 0; ii < ne; ii++) {                                                // Var name.
                szVar[ii] = szExpression[ii];
            }
            szVar[ne] = _T('\0');
            // Reserved names
            if (_tcsicmp((const char_t*)szVar, _T("solve")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("rad")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("radian")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("deg")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("degree")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("gradian")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("grad")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("unit")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("reset")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("set")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("unset")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("name")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("release")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("copyright")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("version")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("description")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("var")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("format")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("new")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("save")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("quit")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("exit")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("tol")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("iter")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("author")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("reserved")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            else if (_tcsicmp((const char_t*)szVar, _T("sidi")) == 0) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }

            if ((szVar[0] == _T('_')) && (ne == 2)) {
                _tcscpy(szOutput, _T("! Name reserved."));
                return FALSE;
            }
            if (ne == 2) {
                if ((szVar[0] == _T('p')) && (szVar[1] == _T('i'))) {
                    _tcscpy(szOutput, _T("! Name reserved."));
                    return FALSE;
                }
            }
            // Verify varname validity
            if (_istalpha(szVar[0]) == 0) {
                _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                return FALSE;
            }
            for (ii = 0; ii < ne; ii++) {
                if ((_istalpha(szVar[ii]) == 0) && (_istdigit(szVar[ii]) == 0)) {
                    _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                    return FALSE;
                }
            }
            //
            for (ii = ne + 1; ii < nlen; ii++) {                                        // Expr.
                szFunction[ii - (ne + 1)] = szExpression[ii];
            }
            szFunction[nlen - (ne + 1)] = _T('\0');
            iOp = ASSIGN;
        }
        else if (ne == (nlen - 1)) {                                                        // PRINT
            for (ii = 0; ii < ne; ii++) {                                                    // Var name.
                szVar[ii] = szExpression[ii];
            }
            szVar[ne] = _T('\0');
            iOp = PRINT;
        }
    }

    // Update nlen
    nlen = (int_t)_tcslen((const char_t*)szFunction);
    if ((nlen < 1) || (nlen > 250)) {
        _tcscpy(szOutput, _T("! Invalid expression."));
        return FALSE;
    }

    if ((iOp == ASSIGN) && (m_nVarnum == (_T('z') - _T('a') + 1))) {
        ResizeParam();
    }

    cVar = _T('\0');

    if (iOp != SOLVE) {
        // Search for variable name
        nvlen = (int_t)_tcslen((const char_t*)szVar);
        if (nvlen > 1) {
            if (m_nVarnum > 0) {
                for (ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {    
                    if (_tcscmp((const char_t*)m_szParam[ii], (const char_t*)szVar) == 0) {
                        if (m_bParam[ii]) {
                            cVar = _T('a') + ii;
                            bFound = TRUE;
                            break;
                        }
                        else {
                            m_szParam[ii][0] = _T('a') + ii;
                            m_szParam[ii][1] = _T('\0');
                        }
                    }
                }
            }
        }
        else if ((nvlen == 1) && (szVar[0] >= _T('a')) && (szVar[0] <= _T('z'))) {
            if (m_bParam[szVar[0] - _T('a')]) {
                cVar = szVar[0];
                bFound = TRUE;
            }
        }

        // Replace variables name in expression
        char_t cLeft, cRight;
        memset((char_t*)szExpression, 0, sizeof(char_t) * ML_STRSIZE);
        _tcscpy((char_t*)szExpression, (const char_t*)szFunction);
        for (ii = 0; ii < (_T('z') - _T('a') + 1); ii++) {
            jj = 0;
            nslen = (int_t)_tcslen((const char_t*)m_szParam[ii]);
            if (nslen > 1) {
                pszTT = (char_t*)_tcsstr((const char_t*)szExpression, (const char_t*)m_szParam[ii]);
                if (pszTT) {

                    // Word?
                    cLeft = cRight = 0;
                    if (pszTT > (wchar_t*) szExpression) {
                        cLeft = *(pszTT - 1);
                    }
                    if ((pszTT + nslen) <= (wchar_t*) (szExpression + nlen - 1)) {
                        cRight = *(pszTT + nslen);
                    }
                    if ((_istalpha(cLeft) != 0) || (_istalpha(cRight) != 0)) {
                        continue;
                    }
                    //

                    // Replace
                    nplen = (int_t)_tcslen((const char_t*)pszTT);
                    *pszTT = _T('a') + ii;
                    for (ll = 1; ll < nplen - nslen + 1; ll++) {
                        *(pszTT + ll) = *(pszTT + ll + nslen - 1);
                    }
                    *(pszTT + nplen - nslen + 1) = _T('\0');
                    pszTT += 1;

                    jj = 1;
                    while(1) {
                        pszTT = (char_t*)_tcsstr((const char_t*)pszTT, (const char_t*)m_szParam[ii]);
                        if ((pszTT == NULL) || (jj >= nlen)) {
                            bRep = TRUE;
                            break;
                        }

                        // Word?
                        cLeft = cRight = _T('a');
                        if (pszTT > (wchar_t*) szExpression) {
                            cLeft = *(pszTT - 1);
                        }
                        if ((pszTT + nslen) <= (wchar_t*) (szExpression + nlen - 1)) {
                            cRight = *(pszTT + nslen);
                        }
                        if ((_istalpha(cLeft) != 0) || (_istalpha(cRight) != 0)) {
                            continue;
                        }
                        //

                        // Replace
                        nplen = (int_t)_tcslen((const char_t*)pszTT);
                        *pszTT = _T('a') + ii;
                        for (ll = 1; ll < nplen - nslen + 1; ll++) {
                            *(pszTT + ll) = *(pszTT + ll + nslen - 1);
                        }
                        *(pszTT + nplen - nslen + 1) = _T('\0');
                        pszTT += 1;
                        //

                        jj += 1;
                    }
                    //
                }
            }
        }

        if (bRep) {
            memset((char_t*)szFunction, 0, sizeof(char_t) * ML_STRSIZE);
            _tcscpy((char_t*)szFunction, (const char_t*)szExpression);
            // Update nlen
            nlen = (int_t)_tcslen((const char_t*)szFunction);
            if ((nlen < 1) || (nlen > 250)) {
                _tcscpy(szOutput, _T("! Invalid expression."));
                return FALSE;
            }
        }
    }

    else {    // SOLVE
        // Search for 'x'
        if (_tcschr((const char_t*)szExpression, _T('x')) == NULL) {
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }
    }

    // Treatement before calling parser
    ii = 0;

    bool bSepLeft = true;
    bool bNumLeft = true;

    while (1) {

        nlen = (int_t)_tcslen((const char_t*)szFunction);
        if ((ii >= nlen) || (ii >= ML_STRSIZE)) { break; }
        if (szFunction[ii] == _T('\0')) { break; }

        if (bNumLeft && szFunction[ii] == _T('e')) {
            if (iswdigit(szFunction[ii+1]) || (szFunction[ii+1] == _T('+')) || (szFunction[ii+1] == _T('-'))) {
                szFunction[ii] = _T('E');
            }
        }

        else if (bSepLeft && (ii <= (nlen - 3)) && (ii < (ML_STRSIZE - ML_STRSIZES)) && ((nlen == 3) || isSeparator(szFunction[ii + 3]))) {
            if (m_bAns && (szFunction[ii] == _T('a')) && (szFunction[ii + 1] == _T('n')) && (szFunction[ii + 2] == _T('s'))) {
                pszTT = &szFunction[ii + 3];
                _tcscpy(szTmp, pszTT);
                pszTT = &szFunction[ii];
                _tcsprintf(pszTT, ML_STRSIZES - 1, _T("%g"), m_fAns);
                _tcscat(szFunction, szTmp);
            }
        }

        else if (bSepLeft && (ii <= (nlen - 2)) && (ii <= (ML_STRSIZE - 5)) && ((nlen == 2) || isSeparator(szFunction[ii + 2]))) {
            if ((szFunction[ii] == _T('p')) && (szFunction[ii + 1] == _T('i')) && (szFunction[ii + 2] != _T('('))) {
                pszTT = &szFunction[ii + 2];
                _tcscpy(szTmp, pszTT);
                szFunction[ii + 2] = _T('(');
                szFunction[ii + 3] = _T(')');
                szFunction[ii + 4] = _T('\0');
                _tcscat(szFunction, szTmp);
            }
            else if (szFunction[ii] == _T('_')) {    // Constants
                if ((szFunction[ii + 1] == _T('k')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('q')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('h')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('c')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('e')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('n')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
                else if ((szFunction[ii + 1] == _T('m')) && (szFunction[ii + 2] != _T('('))) {
                    pszTT = &szFunction[ii + 2];
                    _tcscpy(szTmp, pszTT);
                    szFunction[ii + 2] = _T('(');
                    szFunction[ii + 3] = _T(')');
                    szFunction[ii + 4] = _T('\0');
                    _tcscat(szFunction, szTmp);
                }
            }
        }

        bSepLeft = isSeparatorLeft(szFunction[ii]);
        bNumLeft = (_istdigit(szFunction[ii]) != 0);

        ii += 1;
    }

    // Print variable value
    if (iOp == PRINT) {
        if (cVar == _T('\0')) {
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        if (m_bParam[cVar - _T('a')] == FALSE) {    // Var not set
            _tcscpy(szOutput, _T("! Variable not set."));
            return FALSE;
        }
        //

        if (pVal) { *pVal = m_fParam[cVar - _T('a')]; }

        //ftmp = fabs(m_fParam[cVar - _T('a')]);
        if (pszFormat == NULL) {
            Reformat(m_fParam[cVar - _T('a')], szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, m_fParam[cVar - _T('a')]);
            SigmaApp.m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        _tcscpy(szOutput, (const char_t*)szBuffer);
        return TRUE;
    }

    // Assign
    else if (iOp == ASSIGN) {

        if ((nvlen == 3) && (szVar[0] == _T('a')) && (szVar[1] == _T('n')) && (szVar[2] == _T('s'))) {
            _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
            return FALSE;
        }

        // Search for variable name
        if (bFound == FALSE) {
            // Verify varname validity
            if (_istalpha(szVar[0]) == 0) {
                _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                return FALSE;
            }
            int_t varlen = (int_t)_tcslen(szVar);
            for (ii = 0; ii < varlen; ii++) {
                if ((_istalpha(szVar[ii]) == 0) && (_istdigit(szVar[ii]) == 0) && (szVar[ii] != _T('_'))) {
                    _tcscpy(szOutput, _T("! Variable name not valid. Choose another variable name."));
                    return FALSE;
                }
            }
            //
            if (varlen == 1) {
                cVar = szVar[0];
                m_nVarnum += 1;
            }
            else if (varlen > 1) {
                char_t cc;
                cVar = _T('a') + m_nVarnum;
                for (cc = _T('a'); cc < _T('z'); cc++) {
                    if (m_bParam[cc - _T('a')] == FALSE) {
                        cVar = cc;
                        _tcscpy(m_szParam[cVar - _T('a')], (const char_t*)szVar);
                        m_nVarnum += 1;
                        break;
                    }
                }
            }
        }
        else if (nvlen == 1) {
            if ((m_bParam[szVar[0] - _T('a')] == TRUE) && (m_szParam[szVar[0] - _T('a')][1] != _T('\0'))) {
                _tcscpy(szOutput, _T("! Variable not set. Choose another variable name."));
                return FALSE;
            }
        }
        //

        if (cVar == _T('\0')) {
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        SigmaApp.m_pLib->errcode = 0;
        pszFunction = ml_parser_decode(szFunction, _T("abcdefghijklmnopqrstuvwxyz"), &nlen, SigmaApp.m_pLib);

        if ((SigmaApp.m_pLib->errcode != 0) || (pszFunction == NULL)) {
            ml_parser_free(pszFunction);
            SigmaApp.m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        m_fParam[cVar - _T('a')] = ml_parser_evalf(SigmaApp.m_pLib, pszFunction, _T("abcdefghijklmnopqrstuvwxyz"), m_fParam[0], m_fParam[1], m_fParam[2], m_fParam[3], m_fParam[4], m_fParam[5],
            m_fParam[6], m_fParam[7], m_fParam[8], m_fParam[9], m_fParam[10], m_fParam[11], m_fParam[12], m_fParam[13], m_fParam[14], m_fParam[15],
            m_fParam[16], m_fParam[17], m_fParam[18], m_fParam[19], m_fParam[20], m_fParam[21], m_fParam[22], m_fParam[23], m_fParam[24], m_fParam[25]);

        if (SigmaApp.m_pLib->errcode != 0) {
            ml_parser_free(pszFunction);
            SigmaApp.m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        ml_parser_free(pszFunction);

        if (pVal) { *pVal = m_fParam[cVar - _T('a')]; }

        if (pszFormat == NULL) {
            Reformat(m_fParam[cVar - _T('a')], szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, m_fParam[cVar - _T('a')]);
            SigmaApp.m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        // Variable set
        m_bParam[cVar - _T('a')] = FALSE;
        if (_tcsicmp((const char_t*)m_szParam[cVar - _T('a')], _T("tol")) == 0) {            // Special set
            ftmp = m_fParam[cVar - _T('a')];
            if ((ftmp > 1e-15) && (ftmp < 1e3)) {
                m_pLib->tol = ftmp;
            }
            m_szParam[cVar - _T('a')][0] = cVar;
            m_szParam[cVar - _T('a')][1] = _T('\0');
        }
        if (_tcsicmp((const char_t*)m_szParam[cVar - _T('a')], _T("reltol")) == 0) {        // Special set
            ftmp = m_fParam[cVar - _T('a')];
            if ((ftmp > 1e-15) && (ftmp < 1e3)) {
                m_pLib->reltol = ftmp;
            }
            m_szParam[cVar - _T('a')][0] = cVar;
            m_szParam[cVar - _T('a')][1] = _T('\0');
        }
        if (_tcsicmp((const char_t*)m_szParam[cVar - _T('a')], _T("maxiter")) == 0) {        // Special set
            ftmp = m_fParam[cVar - _T('a')];
            if ((ftmp > 1) && (ftmp < ML_MAXITER)) {
                SigmaApp.m_pLib->maxiter = (int_t)ftmp;
            }
            m_szParam[cVar - _T('a')][0] = cVar;
            m_szParam[cVar - _T('a')][1] = _T('\0');
        }
        else {
            m_bParam[cVar - _T('a')] = TRUE;
        }

        _tcscpy(szOutput, (const char_t*)szBuffer);
        return TRUE;
    }

    // Calculation
    else if (iOp == EXPR) {

        if ((nlen == 3) && (szFunction[0] == _T('a')) && (szFunction[1] == _T('n')) && (szFunction[2] == _T('s')) && (m_bAns == false)) {
            _tcscpy(szOutput, _T("! ans not set."));
            return FALSE;
        }

        real_t yval;

        SigmaApp.m_pLib->errcode = 0;
        pszFunction = ml_parser_decode(szFunction, _T("abcdefghijklmnopqrstuvwxyz"), &nlen, SigmaApp.m_pLib);

        if ((SigmaApp.m_pLib->errcode != 0) || (pszFunction == NULL)) {
            ml_parser_free(pszFunction);
            SigmaApp.m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        yval = ml_parser_evalf(SigmaApp.m_pLib, pszFunction, _T("abcdefghijklmnopqrstuvwxyz"), m_fParam[0], m_fParam[1], m_fParam[2], m_fParam[3], m_fParam[4], m_fParam[5],
            m_fParam[6], m_fParam[7], m_fParam[8], m_fParam[9], m_fParam[10], m_fParam[11], m_fParam[12], m_fParam[13], m_fParam[14], m_fParam[15],
            m_fParam[16], m_fParam[17], m_fParam[18], m_fParam[19], m_fParam[20], m_fParam[21], m_fParam[22], m_fParam[23], m_fParam[24], m_fParam[25]);

        if (SigmaApp.m_pLib->errcode != 0) {
            ml_parser_free(pszFunction);
            SigmaApp.m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        ml_parser_free(pszFunction);

        if (pVal) { *pVal = yval; }

        m_fAns = yval;
        m_bAns = true;

        if (pszFormat == NULL) {
            Reformat(yval, szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, yval);
            SigmaApp.m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        _tcscpy(szOutput, (const char_t*)szBuffer);
        return TRUE;
    }

    // Solve equation
    else if (iOp == SOLVE) {
        real_t xval, fval, aa, bb;

        SigmaApp.m_pLib->errcode = 0;
        g_pszFunction = ml_parser_decode(szFunction, _T("x"), &nlen, SigmaApp.m_pLib);

        if ((SigmaApp.m_pLib->errcode != 0) || (g_pszFunction == NULL)) {
            ml_parser_free(g_pszFunction);
            SigmaApp.m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Invalid expression."));
            return FALSE;
        }

        aa = m_fParam[0];        bb = m_fParam[1];
        ml_fzguess(funfx, &aa, &bb, SigmaApp.m_pLib);

        xval = ml_fzero(funfx, aa, bb, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            ml_parser_free(g_pszFunction);
            SigmaApp.m_pLib->errcode = 0;
            _tcscpy(szOutput, _T("! Cannot solve equation. Try to change the initial guess (a and b)."));
            return FALSE;
        }

        fval = funfx(xval);

        ml_parser_free(g_pszFunction);

        if (pVal) { *pVal = xval; }

        if (pszFormat == NULL) {
            Reformat(xval, szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, xval);
            SigmaApp.m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        m_fParam[_T('x') - _T('a')] = xval;
        m_bParam[_T('x') - _T('a')] = TRUE;

        _tcsprintf(szTmp, ML_STRSIZE - 1, _T("x = %s"), szBuffer);
        _tcscpy(szOutput, (const char_t*)szTmp);

        if (pszFormat == NULL) {
            Reformat(fval, szBuffer, ML_STRSIZE - 1);
        }
        else {    // pszFormat != NULL
            _tcsprintf(szBuffer, ML_STRSIZE - 1, pszFormat, fval);
            SigmaApp.m_pLib->errcode = 0;
            ml_reformat(szBuffer);
        }

        m_fParam[_T('y') - _T('a')] = fval;
        m_bParam[_T('y') - _T('a')] = TRUE;

        _tcsprintf(szTmp, (ML_STRSIZE - 1), _T("   y = %s"), szBuffer);
        _tcscat(szOutput, (const char_t*)szTmp);
    }

    return TRUE;
}


void CSigmaConsoleApp::Output(CString strT, int_t nIcon /* = 1*/)
{
    HWND hwndParent = ::GetForegroundWindow();

    int_t nLength = (int_t)(strT.GetLength());
    if ((nLength < 2) || (nLength > ML_STRSIZE)) {
        return;
    }

    if (strT[nLength - 2] == _T('\r') || strT[nLength - 2] == _T('\n')) {
        strT.SetAt(nLength - 2, _T('\0'));
    }
    else if (strT[nLength - 1] == _T('\r') || strT[nLength - 1] == _T('\n')) {
        strT.SetAt(nLength - 1, _T('\0'));
    }
    if (strT.GetLength() > 1) {
        UINT uiIcon = MB_ICONINFORMATION;
        switch(nIcon) {
            case SIGMA_OUTPUT_INFORMATION:
                uiIcon = MB_ICONINFORMATION;
                break;
            case SIGMA_OUTPUT_WARNING:
                uiIcon = MB_ICONWARNING;
                break;
            case SIGMA_OUTPUT_ERROR:
                uiIcon = MB_ICONERROR;
                break;
            default:
                uiIcon = MB_ICONINFORMATION;
                break;
        }
        MessageBox(hwndParent, (LPCTSTR)strT, _T("SigmaConsole"), MB_OK|uiIcon);
    }
}

int CSigmaConsoleApp::ExitInstance()
{
    if (m_pLib != NULL) {
        delete m_pLib;
        m_pLib = NULL;
    }

    return CWinApp::ExitInstance();
}
