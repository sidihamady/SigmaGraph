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
#include "MainFrm.h"
#include "InputTextDlg.h"

#include "ScriptWindow.h"

#include "SigmaDoc.h"

#include <afxpriv.h>

CSigmaDoc *g_pDoc = NULL;
BOOL g_bStopit = FALSE;
LARGE_INTEGER g_liTicToc;
ulong_t g_ulTicToc;
int_t g_iErrline = -1;

inline LUALIB_API int luaL_errorlevel(lua_State *pLua, int iLevel, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    luaL_where(pLua, iLevel);
    lua_pushvfstring(pLua, fmt, argp);
    va_end(argp);
    lua_concat(pLua, 2);
    return lua_error(pLua);
}

inline static bool isNaN(double tx)
{
    return ((tx != tx) || ((tx == tx) && ((tx - tx) != 0.0)));
}

inline int lua_toarrayd(lua_State *pLua, int idx, double *parr, int nn)
{
    int ii, itop, nt;

    if (!lua_istable(pLua, idx)) {
        return -1;
    }

    nt = luaL_len(pLua, idx);
    if (nt < nn) {
        return -1;
    }
    for (ii = 1; ii <= nn; ii++) {
        lua_rawgeti(pLua, idx, ii);    // push table[ii]
        itop = lua_gettop(pLua);
        parr[ii - 1] = (double) lua_tonumber(pLua, itop);
        lua_pop(pLua, 1);
        if (isNaN(parr[ii - 1])) {
            return -1;
        }
    }

    return 0;
}

inline int lua_toarrayi(lua_State *pLua, int idx, int *parr, int nn)
{
    int ii, itop, nt;

    if (!lua_istable(pLua, idx)) {
        return -1;
    }

    nt = luaL_len(pLua, idx);
    if (nt < nn) {
        return -1;
    }
    for (ii = 1; ii <= nn; ii++) {
        lua_rawgeti(pLua, idx, ii);    // push table[ii]
        itop = lua_gettop(pLua);
        parr[ii - 1] = (int) lua_tonumber(pLua, itop);
        lua_pop(pLua, 1);
        if (isNaN(parr[ii - 1])) {
            return -1;
        }
    }

    return 0;
}

static int_t cf_tic(lua_State *pLua)
{
    g_ulTicToc = 0;
    g_liTicToc.HighPart = 0;
    g_liTicToc.LowPart = 0;
    g_liTicToc.QuadPart = 0;
    ::QueryPerformanceCounter(&g_liTicToc);

    return 0;
}
static int_t cf_toc(lua_State *pLua)
{
    LARGE_INTEGER lstop, lfreq;

    if (g_liTicToc.QuadPart == 0) {
        g_liTicToc.HighPart = 0;
        g_liTicToc.LowPart = 0;
        g_ulTicToc = 0;
        lua_pushnumber(pLua, 0);
        return 1;
    }

    ::QueryPerformanceCounter(&lstop);
    ::QueryPerformanceFrequency(&lfreq);

    lstop.QuadPart -= g_liTicToc.QuadPart;
    lstop.QuadPart *= 1000000;
    lstop.QuadPart /= lfreq.QuadPart;

    if (lstop.HighPart != 0) {
        g_ulTicToc = 0;
    }
    else {
        g_ulTicToc = (ulong_t)(lstop.LowPart);
    }

    lua_pushnumber(pLua, (1e-6) * (real_t)(g_ulTicToc));
    return 1;
}

static int_t cf_stopit(lua_State *pLua)
{
    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        lua_pushboolean(pLua, 0);
        return 1;
    }

    lua_pushboolean(pLua, (g_bStopit) ? 1 : 0);
    return 1;
}

static int_t cf_read(lua_State *pLua)
{
    int_t nArgs = lua_gettop(pLua);
    char *pszArgA = NULL;
    int_t iArg = ML_STRSIZEN;

    char_t szArgW[ML_STRSIZET];
    memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));

    if (nArgs == 1) {
        if (lua_isnumber(pLua, 1)) {
            iArg = (int_t)lua_tonumber(pLua, 1);
            if (iArg < 1) {
                iArg = 1;
            }
            if (iArg > ML_STRSIZEN) {
                iArg = ML_STRSIZEN;
            }
        }
        else {
            pszArgA = (char*)lua_tostring(pLua, 1);
            if (pszArgA) {
                MultiByteToWideChar(CP_ACP, 0, pszArgA, -1, szArgW, ML_STRSIZET);
            }
        }
    }

    int_t nLength = 0;
    char szBufferA[ML_STRSIZEN];
    memset(szBufferA, 0, ML_STRSIZEN * sizeof(char));
    char_t szBufferW[ML_STRSIZEN];
    memset(szBufferW, 0, ML_STRSIZEN * sizeof(char_t));

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInputTextDlg *pDlgInput = new CInputTextDlg(pFrame->m_pwndScript);

    pDlgInput->m_strTitle = _T("Script Input");
    pDlgInput->m_pszInput1 = szBufferW;
    nLength = _tcslen((const char_t*)szArgW);
    if ((nLength > 0) && (nLength < iArg)) {
        pDlgInput->m_strLabel1.Format(_T("Input (%s):"), szArgW);
    }
    else {
        pDlgInput->m_strLabel1 = _T("Input :");
    }
    pDlgInput->m_iLimit = iArg;
    pDlgInput->DoModal();
    if (pDlgInput->m_bOK) {
        WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)szBufferW, -1, (LPSTR)szBufferA, ML_STRSIZEN, NULL, NULL);
        lua_pushstring(pLua, (const char*)szBufferA);
    }
    else {
        lua_pushstring(pLua, "");
    }

    pDlgInput->DestroyWindow();
    delete pDlgInput; pDlgInput = NULL;

    return 1;
}

static int_t doWrite(lua_State *pLua, bool bFormat)
{
    /* get number of arguments */
    int nArgCount = lua_gettop(pLua);

    int_t iErr = 0;

    char* pszMessageA = NULL;

    if ((nArgCount < 1) || (nArgCount > 32)) {
        pszMessageA = (char*) lua_tostring(pLua, 1);
        if (pszMessageA == NULL) {

            return 0;
        }
        lua_pop(pLua, 1);    // Dépiler la valeur
        iErr = 1;
    }

    int_t nLen = 0;
    char szMessageA[ML_STRSIZE], *pszT = NULL;
    memset(szMessageA, 0, ML_STRSIZE * sizeof(char));

    if (iErr == 0) {
        int_t ii, nLenT = 0;

        for (ii = 1; ii <= nArgCount; ii++) {
            pszT = (char*) lua_tostring(pLua, ii);
            if (pszT == NULL) {
                break;
            }
            nLenT = (int_t) strlen(pszT);
            nLen = (int_t) strlen(szMessageA);
            if (nLenT < (ML_STRSIZE - nLen)) {
                strncat(szMessageA, (const char*) pszT, ML_STRSIZE - nLen - 3);
                if (bFormat && (ii < nArgCount)) {
                    strncat(szMessageA, "   ", 3);
                }
            }
        }
        pszMessageA = szMessageA;
    }

    nLen = (int_t) strlen(pszMessageA);
    if ((nLen < 1) || (nLen > ML_STRSIZE)) {
        return 0;
    }

    char_t szMessageW[ML_STRSIZE];
    memset(szMessageW, 0, ML_STRSIZE * sizeof(char_t));

    int_t nLenW = MultiByteToWideChar(CP_ACP, 0, pszMessageA, -1, szMessageW, ML_STRSIZE);
    if ((nLenW < 1) || (nLenW >= ML_STRSIZE)) {
        return 0;
    }
    szMessageW[nLenW - 1] = _T('\0');

    if (iErr != 0) {
        SigmaApp.ScriptOutput(_T("! "));
    }
    SigmaApp.ScriptOutput((LPCTSTR) szMessageW);
    return 0;
}

static int_t cf_write(lua_State *pLua)
{
    return doWrite(pLua, false);
}

static int_t cf_print(lua_State *pLua)
{
    return doWrite(pLua, true);
}

static int_t cf_clear(lua_State *pLua)
{
    SigmaApp.ScriptOutputClear();

    return 0;
}

static int_t cf_pause(lua_State *pLua)
{
    int_t nArgs = lua_gettop(pLua), iDelay;

    if (nArgs == 1) {
        if (lua_isnumber(pLua, 1)) {
            iDelay = (int_t)lua_tonumber(pLua, 1);
            if ((iDelay > 1) && (iDelay < 3600000)) {
                ::Sleep((DWORD)iDelay);
            }
        }
    }

    return 0;
}

static int lineNumber(const char* pszMessageA)
{
    int iLine = -1;
    char cT;
    char *pszTA = NULL, *pszTB = NULL, *pszTTB, *pszTTA;

    if (pszMessageA == NULL) {
        return 0;
    }

    pszTB = (char*) strstr(pszMessageA, "at line");
    if (pszTB) {
        pszTB += 7;
        pszTTB = (char*) strstr(pszTB, ")");
        if (pszTTB) {
            cT = *pszTTB;
            *pszTTB = '\0';
            iLine = atoi(pszTB) - 1;
            *pszTTB = cT;
        }
    }

    if (iLine == -1) {
        pszTA = (char*) strstr(pszMessageA, "\"]:");
        if (pszTA) {
            pszTA += 3;
            pszTTA = (char*) strstr(pszTA, ":");
            if (pszTTA) {
                cT = *pszTTA;
                *pszTTA = '\0';
                iLine = atoi(pszTA) - 1;
                *pszTTA = cT;
            }
        }
    }

    return iLine;
}

int_t cf_error(lua_State *pLua)
{
    const char* pszMessageA = lua_tostring(pLua, -1);
    if (pszMessageA == NULL) {
        return 0;
    }
    lua_pop(pLua, 1);    // Dépiler la valeur

    int_t nLenA = 0;

    nLenA = (int_t) strlen(pszMessageA);
    if ((nLenA < 1) || (nLenA > ML_STRSIZE)) {
        return 0;
    }

    g_iErrline = lineNumber(pszMessageA);

    char_t szMessageW[ML_STRSIZE];
    memset(szMessageW, 0, ML_STRSIZE * sizeof(char_t));

    char *pszT = (char*)strstr(pszMessageA, "]:");
    bool bFlag = false;
    if (pszT) {
        nLenA = (int_t) strlen(pszT);
        if (nLenA > 2) {
            pszT += 2;
            nLenA -= 2;
            int_t nLenW = MultiByteToWideChar(CP_ACP, 0, pszT, nLenA, szMessageW, ML_STRSIZE - 1);
            if ((nLenW < 1) || (nLenW >= ML_STRSIZE)) {
                return 0;
            }
            szMessageW[nLenW] = _T('\0');
            bFlag = true;
        }
    }

    if (bFlag == false) {
        int_t nLenW = MultiByteToWideChar(CP_ACP, 0, pszMessageA, nLenA, szMessageW, ML_STRSIZE - 1);
        if ((nLenW < 1) || (nLenW >= ML_STRSIZE)) {
            return 0;
        }
        szMessageW[nLenW - 1] = _T('\0');
    }

    SigmaApp.ScriptOutput(_T("! "));
    SigmaApp.ScriptOutput((LPCTSTR) szMessageW);
    SigmaApp.ScriptOutput(_T("\r\n"));

    return 0;
}

static int_t cf_panic(lua_State *pLua)
{
    char_t szMessageW[ML_STRSIZE];
    memset(szMessageW, 0, ML_STRSIZE * sizeof(char_t));
    int_t iMsg = 0;
    if (lua_gettop(pLua) > 0 && lua_isstring(pLua, -1)) {
        const char* pszMessageA = lua_tostring(pLua, -1);
        if (pszMessageA != NULL) {
            lua_pop(pLua, 1);        // Dépiler la valeur
            int_t nLen = 0;
            nLen = (int_t)strlen(pszMessageA);
            if ((nLen > 0) && (nLen < ML_STRSIZE)) {
                int_t nLenW = MultiByteToWideChar(CP_ACP, 0, pszMessageA, -1, szMessageW, ML_STRSIZE);
                if ((nLenW > 0) && (nLenW < ML_STRSIZE)) {
                    szMessageW[nLenW - 1] = _T('\0');
                    SigmaApp.ScriptOutput(_T("! "));
                    SigmaApp.ScriptOutput((LPCTSTR) szMessageW);
                    SigmaApp.ScriptOutput(_T("\r\n"));

                    throw 1;
                }
            }
        }
    }

    SigmaApp.ScriptOutput(_T("! Script unrecovered error.\r\n"));

    throw 1;        // Throw an exception

    return 0;       // Never executed!
}

// Functions exported to Lua - MATH
static int_t cf_unit(lua_State *pLua)
{
    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        if (ml_angleunit == 0x00) {
            lua_pushstring(pLua, "radian");
        }
        else if (ml_angleunit == 0x01) {
            lua_pushstring(pLua, "degree");
        }
        else if (ml_angleunit == 0x02) {
            lua_pushstring(pLua, "grad");
        }

        return 1;
    }
    if (!lua_isstring(pLua, 1)) {
        lua_pushstring(pLua, "?");

        return 1;
    }

    const char* pszArgA = lua_tostring(pLua, 1);
    if (pszArgA) {
        if (_stricmp(pszArgA, "radian") == 0) {
            ml_angleunit = 0x00;
            lua_pushstring(pLua, "radian");
        }
        else if (_stricmp(pszArgA, "rad") == 0) {
            ml_angleunit = 0x00;
            lua_pushstring(pLua, "radian");
        }
        else if (_stricmp(pszArgA, "degree") == 0) {
            ml_angleunit = 0x01;
            lua_pushstring(pLua, "degree");
        }
        else if (_stricmp(pszArgA, "deg") == 0) {
            ml_angleunit = 0x01;
            lua_pushstring(pLua, "degree");
        }
        else if (_stricmp(pszArgA, "grad") == 0) {
            ml_angleunit = 0x02;
            lua_pushstring(pLua, "grad");
        }
        else {
            lua_pushstring(pLua, "?");
        }
    }
    else {
        lua_pushstring(pLua, "?");
    }

    return 1;
}

static int_t cf_exp(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)exp(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_ln(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)log(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_log(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)log10(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_log2(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)(log(fx) / log(2.0));

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_pow(lua_State *pLua)
{
    real_t fx = 0.0, fe = 1.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fe = (real_t)lua_tonumber(pLua, 2);
    fy = (real_t)pow(fx, fe);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_sin(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);

    if (ml_angleunit == 0x01) {
        int_t nx = (int_t)floor(fx);
        real_t fxx = (real_t)nx;
        if (fx == fxx) {
            if ((nx % 180) == 0) {
                lua_pushnumber(pLua, fy);
                return 1;
            }
        }
        fx *= (ML_PI / 180.0);
    }
    else if (ml_angleunit == 0x02) {
        int_t nx = (int_t)floor(fx);
        real_t fxx = (real_t)nx;
        if (fx == fxx) {
            if ((nx % 200) == 0) {
                lua_pushnumber(pLua, fy);
                return 1;
            }
        }
        fx *= (ML_PI / 200.0);
    }
    else {
        real_t ft = fx / ML_PI;
        int_t nx = (int_t)floor(ft);
        real_t fxx = (real_t)nx;
        if (ft == fxx) {
            lua_pushnumber(pLua, fy);
            return 1;
        }
    }

    fy = (real_t)sin(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_cos(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);

    if (ml_angleunit == 0x01) {
        int_t nx = (int_t)floor(fx);
        real_t fxx = (real_t)nx;
        if (fx == fxx) {
            if (((nx % 180) != 0) && ((nx % 90) == 0)) {
                lua_pushnumber(pLua, fy);
                return 1;
            }
        }
        fx *= (ML_PI / 180.0);
    }
    else if (ml_angleunit == 0x02) {
        int_t nx = (int_t)floor(fx);
        real_t fxx = (real_t)nx;
        if (fx == fxx) {
            if (((nx % 200) != 0) && ((nx % 100) == 0)) {
                lua_pushnumber(pLua, fy);
                return 1;
            }
        }
        fx *= (ML_PI / 200.0);
    }
    else {
        real_t fxx = fx / ML_PI, fxxx = fx / ML_PI2;
        int_t nx = (int_t)floor(fxx), nxx = (int_t)floor(fxxx);
        real_t ft = (real_t)nx, ftt = (real_t)nxx;
        if ((ftt == fxxx) && (ft != fxx)) {
            lua_pushnumber(pLua, fy);
            return 1;
        }
    }

    fy = (real_t)cos(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_tan(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);

    if (ml_angleunit == 0x01) {
        int_t nx = (int_t)floor(fx);
        real_t fxx = (real_t)nx;
        if (fx == fxx) {
            if ((nx % 180) == 0) {
                lua_pushnumber(pLua, fy);
                return 1;
            }
        }
        fx *= (ML_PI / 180.0);
    }
    else if (ml_angleunit == 0x02) {
        int_t nx = (int_t)floor(fx);
        real_t fxx = (real_t)nx;
        if (fx == fxx) {
            if ((nx % 200) == 0) {
                lua_pushnumber(pLua, fy);
                return 1;
            }
        }
        fx *= (ML_PI / 200.0);
    }
    else {
        real_t ft = fx / ML_PI;
        int_t nx = (int_t)floor(ft);
        real_t fxx = (real_t)nx;
        if (ft == fxx) {
            lua_pushnumber(pLua, fy);
            return 1;
        }
    }

    fy = (real_t)tan(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_asin(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)asin(fx);

    if (ml_angleunit == 0x01) {
        fy *= (180.0 / ML_PI);
    }
    else if (ml_angleunit == 0x02) {
        fy *= (200.0 / ML_PI);
    }

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_acos(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)acos(fx);

    if (ml_angleunit == 0x01) {
        fy *= (180.0 / ML_PI);
    }
    else if (ml_angleunit == 0x02) {
        fy *= (200.0 / ML_PI);
    }

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_atan(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)atan(fx);

    if (ml_angleunit == 0x01) {
        fy *= (180.0 / ML_PI);
    }
    else if (ml_angleunit == 0x02) {
        fy *= (200.0 / ML_PI);
    }

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_sinh(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)sinh(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_cosh(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)cosh(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_tanh(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)tanh(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_abs(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)fabs(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_sqrt(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)sqrt(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_ceil(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)ceil(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_floor(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)floor(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_fmod(lua_State *pLua)
{
    real_t fa = 0.0, fb = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fa = (real_t)lua_tonumber(pLua, 1);
    fb = (real_t)lua_tonumber(pLua, 2);
    fy = (real_t)fmod(fa, fb);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_j0(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)_j0(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_j1(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)_j1(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_jn(lua_State *pLua)
{
    real_t fn = 0.0, fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fn = (real_t)lua_tonumber(pLua, 1);
    fx = (real_t)lua_tonumber(pLua, 2);
    fy = (real_t)_jn((int_t)fn, fy);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_y0(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)_y0(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_y1(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fy = (real_t)_y1(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_yn(lua_State *pLua)
{
    real_t fn = 0.0, fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fn = (real_t)lua_tonumber(pLua, 1);
    fx = (real_t)lua_tonumber(pLua, 2);
    fy = (real_t)_yn((int_t)fn, fy);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_hypot(lua_State *pLua)
{
    real_t fa = 0.0, fb = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fa = (real_t)lua_tonumber(pLua, 1);
    fb = (real_t)lua_tonumber(pLua, 2);
    fy = (real_t)_hypot(fa, fb);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_pi(lua_State *pLua)
{
    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        lua_pushnumber(pLua, 0.0);
        return 1;
    }

    lua_pushnumber(pLua, ML_PI);
    return 1;
}

static int_t cf_min(lua_State *pLua)
{
    real_t fa = 0.0, fb = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fa = (real_t)lua_tonumber(pLua, 1);
    fb = (real_t)lua_tonumber(pLua, 2);

    fy = (fa < fb) ? fa : fb;

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_max(lua_State *pLua)
{
    real_t fa = 0.0, fb = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fa = (real_t)lua_tonumber(pLua, 1);
    fb = (real_t)lua_tonumber(pLua, 2);

    fy = (fa > fb) ? fa : fb;

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_rand(lua_State *pLua)
{
    real_t fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        srand((uint_t)time(NULL));
    }

    fy = ((real_t)(rand())) / ((real_t)RAND_MAX);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_time(lua_State *pLua)
{
    real_t fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fy = (real_t)time(NULL);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_sign(lua_State *pLua)
{
    real_t fx = 0.0;
    int_t iy = 0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, iy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, iy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);

    iy = (fx < 0.0) ? -1 : 1;

    lua_pushnumber(pLua, iy);
    return 1;
}

static int_t cf_erf(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);

    fy = ml_erf(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_bern(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);

    fy = ml_bern(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_dbern(lua_State *pLua)
{
    real_t fa = 0.0, fb = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fa = (real_t)lua_tonumber(pLua, 1);
    fb = (real_t)lua_tonumber(pLua, 2);

    fy = ml_dbern(fa, fb);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_fact(lua_State *pLua)
{
    real_t fx = 0.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);

    fy = ml_fact(fx);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_gauss(lua_State *pLua)
{
    real_t fx = 0.0, fb = 0.0, fc = 1.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2) || !lua_isnumber(pLua, 3)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fb = (real_t)lua_tonumber(pLua, 2);
    fc = (real_t)lua_tonumber(pLua, 3);

    fy = ml_gauss(fx, fb, fc);

    lua_pushnumber(pLua, fy);
    return 1;
}

static int_t cf_lorentz(lua_State *pLua)
{
    real_t fx = 0.0, fb = 0.0, fc = 1.0, fy = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        lua_pushnumber(pLua, fy);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2) || !lua_isnumber(pLua, 3)) {
        lua_pushnumber(pLua, fy);
        return 1;
    }

    fx = (real_t)lua_tonumber(pLua, 1);
    fb = (real_t)lua_tonumber(pLua, 2);
    fc = (real_t)lua_tonumber(pLua, 3);

    fy = ml_lorentz(fx, fb, fc);

    lua_pushnumber(pLua, fy);
    return 1;
}

// Functions exported to Lua - Physics
static int_t cf_phys_current(lua_State *pLua)
{
    real_t V, T, ISC, RP, IS1, n1, IS2, n2, RS, I1, I2, II = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 10) {
        lua_pushnumber(pLua, 0.0);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2) || !lua_isnumber(pLua, 3) || !lua_isnumber(pLua, 4)
        || !lua_isnumber(pLua, 5) || !lua_isnumber(pLua, 6) || !lua_isnumber(pLua, 7) || !lua_isnumber(pLua, 8)
        || !lua_isnumber(pLua, 9) || !lua_isnumber(pLua, 10)) {
        return luaL_errorlevel(pLua, 2, "Physics.current: invalid argument");
    }

    V = (real_t)lua_tonumber(pLua, 1);
    T = (real_t)lua_tonumber(pLua, 2);
    ISC = (real_t)lua_tonumber(pLua, 3);
    RP = (real_t)lua_tonumber(pLua, 4);
    IS1 = (real_t)lua_tonumber(pLua, 5);
    n1 = (real_t)lua_tonumber(pLua, 6);
    IS2 = (real_t)lua_tonumber(pLua, 7);
    n2 = (real_t)lua_tonumber(pLua, 8);
    RS = (real_t)lua_tonumber(pLua, 9);
    I2 = (real_t)lua_tonumber(pLua, 10);
    I1 = -I2;

    II = el_diode_current(V, T, ISC, RP, IS1, n1, IS2, n2, RS, I1, I2);

    lua_pushnumber(pLua, II);
    return 1;
}

static int_t cf_phys_capacitance(lua_State *pLua)
{
    real_t Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2, Cm = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 9) {
        lua_pushnumber(pLua, 0.0);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2) || !lua_isnumber(pLua, 3) || !lua_isnumber(pLua, 4)
        || !lua_isnumber(pLua, 5) || !lua_isnumber(pLua, 6) || !lua_isnumber(pLua, 7) || !lua_isnumber(pLua, 8)
        || !lua_isnumber(pLua, 9)) {
        return luaL_errorlevel(pLua, 2, "Physics.capacitance: invalid argument");
    }

    Freq = (real_t)lua_tonumber(pLua, 1);
    r = (real_t)lua_tonumber(pLua, 2);
    L = (real_t)lua_tonumber(pLua, 3);
    GLF = (real_t)lua_tonumber(pLua, 4);
    CHF = (real_t)lua_tonumber(pLua, 5);
    C1 = (real_t)lua_tonumber(pLua, 6);
    Tau1 = (real_t)lua_tonumber(pLua, 7);
    C2 = (real_t)lua_tonumber(pLua, 8);
    Tau2 = (real_t)lua_tonumber(pLua, 9);

    Cm = el_capacitance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2);

    lua_pushnumber(pLua, Cm);
    return 1;
}

static int_t cf_phys_conductance(lua_State *pLua)
{
    real_t Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2, Gm = 0.0;

    int_t nArgs = lua_gettop(pLua);
    if (nArgs != 9) {
        lua_pushnumber(pLua, Gm);
        return 1;
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2) || !lua_isnumber(pLua, 3) || !lua_isnumber(pLua, 4)
        || !lua_isnumber(pLua, 5) || !lua_isnumber(pLua, 6) || !lua_isnumber(pLua, 7) || !lua_isnumber(pLua, 8)
        || !lua_isnumber(pLua, 9)) {
        return luaL_errorlevel(pLua, 2, "Physics.conductance: invalid argument");
    }

    Freq = (real_t)lua_tonumber(pLua, 1);
    r = (real_t)lua_tonumber(pLua, 2);
    L = (real_t)lua_tonumber(pLua, 3);
    GLF = (real_t)lua_tonumber(pLua, 4);
    CHF = (real_t)lua_tonumber(pLua, 5);
    C1 = (real_t)lua_tonumber(pLua, 6);
    Tau1 = (real_t)lua_tonumber(pLua, 7);
    C2 = (real_t)lua_tonumber(pLua, 8);
    Tau2 = (real_t)lua_tonumber(pLua, 9);

    Gm = el_conductance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2);

    lua_pushnumber(pLua, Gm);
    return 1;
}

int lmath_kb(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.kb: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_KB);
    return 1;
}
int lmath_q(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.q: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_QE);
    return 1;
}
int lmath_h(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.h: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_HH);
    return 1;
}
int lmath_c(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.c: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_CC);
    return 1;
}
int lmath_eps0(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.eps0: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_EPS0);
    return 1;
}
int lmath_na(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.na: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_NA);
    return 1;
}
int lmath_me(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.me: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_M0);
    return 1;
}
int lmath_kbt(lua_State *pLua)
{
    int nArgs = lua_gettop(pLua);
    if (nArgs != 0) {
        return luaL_errorlevel(pLua, 2, "lmath.kbt: invalid number of argument (should be 0)");
    }

    lua_pushnumber(pLua, ML_KBT);
    return 1;
}

// -- C99
int lmath_erf(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.erf: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.erf: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double) ml_erf(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_erfc(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.erfc: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.erfc: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)erfc(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_exp2(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.exp2: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.exp2: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)exp2(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_logb(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.logb: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.logb: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)logb(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_cbrt(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.cbrt: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.cbrt: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)cbrt(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_hypot(lua_State *pLua)
{
    double at = 0.0, bt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 2) {
        return luaL_errorlevel(pLua, 2, "lmath.hypot: invalid number of argument (should be 2)");
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2)) {
        return luaL_errorlevel(pLua, 2, "lmath.hypot: invalid argument (not a number)");
    }

    at = (double) lua_tonumber(pLua, 1);
    bt = (double) lua_tonumber(pLua, 2);
    yt = (double) _hypot(at, bt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_lgamma(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.lgamma: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.lgamma: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)lgamma(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_tgamma(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.tgamma: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.tgamma: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)tgamma(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_trunc(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.trunc: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.trunc: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)trunc(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_round(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.round: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.round: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)round(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_rint(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.rint: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.rint: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)rint(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_isnan(lua_State *pLua)
{
    double xt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.isnan: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.isnan: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);

    lua_pushinteger(pLua, isNaN(xt) ? 1 : 0);
    return 1;
}

int lmath_isinf(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.isinf: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.isinf: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)isinf(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_isnormal(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.isnormal: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.isnormal: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)isnormal(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_asinh(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.asinh: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.asinh: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)asinh(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_acosh(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.acosh: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.acosh: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)acosh(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_atanh(lua_State *pLua)
{
    double xt = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        return luaL_errorlevel(pLua, 2, "lmath.atanh: invalid number of argument (should be 1)");
    }
    if (!lua_isnumber(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "lmath.atanh: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    yt = (double)atanh(xt);

    lua_pushnumber(pLua, yt);
    return 1;
}
// --

int lmath_gauss(lua_State *pLua)
{
    double xt = 0.0, bt = 0.0, ct = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        return luaL_errorlevel(pLua, 2, "lmath.gauss: invalid number of argument (should be 3)");
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2) || !lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, 2, "lmath.gauss: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    bt = (double) lua_tonumber(pLua, 2);
    ct = (double) lua_tonumber(pLua, 3);
    yt = (double) (exp(-(xt - bt) * (xt - bt) / (2.0 * ct * ct)));

    lua_pushnumber(pLua, yt);
    return 1;
}

int lmath_lorentz(lua_State *pLua)
{
    double xt = 0.0, bt = 0.0, ct = 0.0, yt = 0.0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 3) {
        return luaL_errorlevel(pLua, 2, "lmath.lorentz: invalid number of argument (should be 3)");
    }
    if (!lua_isnumber(pLua, 1) || !lua_isnumber(pLua, 2) || !lua_isnumber(pLua, 3)) {
        return luaL_errorlevel(pLua, 2, "lmath.lorentz: invalid argument (not a number)");
    }

    xt = (double) lua_tonumber(pLua, 1);
    bt = (double) lua_tonumber(pLua, 2);
    ct = (double) lua_tonumber(pLua, 3);
    yt = (double) (ct / (((xt - bt) * (xt - bt)) + (ct * ct)));

    lua_pushnumber(pLua, yt);
    return 1;
}

#define data_SWAP(a,b)            ftmp = (a);(a) = (b);(b) = ftmp;
#define data_SORT_MIN            7
#define data_SORT_STACK        50
#define data_MAXPOINTS            2048

double* data_sort(double *pdat, int ndat, int iasc);
double* data_reverse(double *pdat, int ndat);
stat_t* data_calc(const double *pdat, int ndat, stat_t* pstats);

double* data_sort(double *pdat, int ndat, int iasc)
{
    int ii, ir = ndat - 1, jj, nn, ll = 0, *istack, isec, jsec;
    int jstack = 0;
    double aa, ftmp;

    if ((pdat == NULL) || (ndat < 1) || (ndat > data_MAXPOINTS)) {
        return NULL;
    }

    if (ndat == 1) {
        return pdat;
    }
    else if (ndat == 2) {
        if (pdat[0] > pdat[1]) {
            ftmp = pdat[0];
            pdat[0] = pdat[1];
            pdat[1] = ftmp;
        }
        return pdat;
    }

    istack = (int*) malloc(data_SORT_STACK * sizeof(int));
    if (istack == NULL) {
        return NULL;
    }

    for (isec = 0; isec < data_MAXPOINTS; isec++) {
        if (ir - ll < data_SORT_MIN) {
            for (jj = ll + 1; jj <= ir; jj++) {
                aa = pdat[jj];
                for (ii = jj - 1; ii >= ll; ii--) {
                    if (pdat[ii] <= aa) { break; }
                    pdat[ii + 1] = pdat[ii];
                }
                pdat[ii + 1] = aa;
            }
            if (jstack == 0) { break; }
            ir = istack[jstack--];
            ll = istack[jstack--];
        }
        else {
            nn = (ll + ir + 1) >> 1;
            if (nn > (ndat - 1)) { free(istack); istack = NULL; return NULL; } // Never happen
            data_SWAP(pdat[nn], pdat[ll + 1])
                if (pdat[ll] > pdat[ir]) {
                    data_SWAP(pdat[ll], pdat[ir])
                }
            if (pdat[ll + 1] > pdat[ir]) {
                data_SWAP(pdat[ll + 1], pdat[ir])
            }
            if (pdat[ll] > pdat[ll + 1]) {
                data_SWAP(pdat[ll], pdat[ll + 1])
            }
            ii = ll + 1;
            jj = ir;
            aa = pdat[ll + 1];
            for (jsec = 0; jsec < data_MAXPOINTS; jsec++) {
                do { ii++; }
                while (pdat[ii] < aa);
                do { jj--; }
                while (pdat[jj] > aa);
                if (jj < ii) { break; }
                data_SWAP(pdat[ii], pdat[jj]);
            }
            pdat[ll + 1] = pdat[jj];
            pdat[jj] = aa;
            jstack += 2;
            if (jstack > data_SORT_STACK) { free(istack); istack = NULL; return NULL; }
            if (ir - ii + 1 >= jj - ll) {
                istack[jstack] = ir;
                istack[jstack - 1] = ii;
                ir = jj - 1;
            }
            else {
                istack[jstack] = jj - 1;
                istack[jstack - 1] = ll;
                ll = ii;
            }
        }
    }

    if (iasc == 0) {
        data_reverse(pdat, ndat);
    }

    free(istack); istack = NULL;
    return pdat;
}

double* data_reverse(double *pdat, int ndat)
{
    int ii, jj, isec;
    double ftmp;

    if (pdat == NULL) {
        return NULL;
    }

    if ((ndat < 1) || (ndat > data_MAXPOINTS)) {
        return NULL;
    }

    if (ndat == 1) {
        return pdat;
    }
    else if (ndat == 2) {
        ftmp = pdat[0];
        pdat[0] = pdat[1];
        pdat[1] = ftmp;
        return pdat;
    }

    ii = 0; jj = ndat - 1;
    for (isec = 0; isec < data_MAXPOINTS; isec++) {
        ftmp = pdat[ii];
        pdat[ii] = pdat[jj];
        pdat[jj] = ftmp;
        ii += 1;
        jj -= 1;
        if (ii >= jj) {
            break;
        }
    }

    return pdat;
}

stat_t* data_calc(const double *pdat, int ndat, stat_t* pstats)
{
    int ii;
    double ftmp, fa, fb, fc;
    double *ptmp = NULL;

    if ((pdat == NULL) || (pstats == NULL) || (ndat < 1) || (ndat > data_MAXPOINTS)) {
        return NULL;
    }

    if (ndat == 1) {
        pstats->points = 1;
        pstats->min = pdat[0];
        pstats->max = pdat[0];
        pstats->sum = pdat[0];
        pstats->mean = pdat[0];
        pstats->median = pdat[0];
        pstats->rms = 0.0;
        pstats->var = 0.0;
        pstats->dev = 0.0;
        pstats->coeff = 0.0;

        pstats->done = 1;

        return pstats;
    }
    else if (ndat == 2) {
        pstats->points = 2;
        pstats->min = (pdat[0] < pdat[1]) ? pdat[0] : pdat[1];
        pstats->max = (pdat[0] > pdat[1]) ? pdat[0] : pdat[1];
        pstats->sum = pdat[0] + pdat[1];
        pstats->mean = 0.5 * (pdat[0] + pdat[1]);
        pstats->median = 0.5 * (pdat[0] + pdat[1]);
        pstats->rms = (double) sqrt(((pdat[0] * pdat[0]) + (pdat[1] * pdat[1])) * 0.5);
        pstats->var = ((ptmp[0] - pstats->mean) * (ptmp[0] - pstats->mean)) + ((ptmp[1] - pstats->mean) * (ptmp[1] - pstats->mean));
        pstats->dev = (double) sqrt(pstats->var);
        if (pstats->mean != 0.0) {
            pstats->coeff = pstats->dev / pstats->mean;
        }
        else {
            pstats->coeff = 0.0;
        }

        pstats->done = 1;

        return pstats;
    }

    ptmp = (double*) malloc(ndat * sizeof(double));
    if (ptmp == NULL) {
        return NULL;
    }

    pstats->min = pstats->max = pdat[0];
    pstats->sum = 0.0;
    ftmp = 0.0;
    for (ii = 0; ii < ndat; ii++) {
        ptmp[ii] = pdat[ii];

        if (ptmp[ii] > pstats->max) {
            pstats->max = ptmp[ii];
        }
        if (ptmp[ii] < pstats->min) {
            pstats->min = ptmp[ii];
        }

        pstats->sum += ptmp[ii];
        ftmp += (ptmp[ii] * ptmp[ii]);
    }
    pstats->mean = pstats->sum / (double) ndat;
    pstats->rms = (double) sqrt(ftmp / (double) ndat);

    // Ordonner les donn�es si n�cessaire
    if (pstats->sorted == 0) {
        data_sort(ptmp, ndat, 1);
    }

    ftmp = 0.5 * (double) ndat;
    ii = ndat / 2;
    if (ftmp == (double) ii) {
        pstats->median = 0.5 * (ptmp[ii - 1] + ptmp[ii]);
    }
    else {
        pstats->median = ptmp[ii];
    }

    fa = 0.0; fb = 0.0; fc = 0.0;
    for (ii = 0; ii < ndat; ii++) {
        ftmp = (ptmp[ii] - pstats->mean) * (ptmp[ii] - pstats->mean);
        fa += ftmp;
        fb += ftmp * (ptmp[ii] - pstats->mean);
        fc += ftmp * ftmp;
    }
    pstats->var = fa / (double) (ndat - 1);
    pstats->dev = (double) sqrt(pstats->var);
    if (pstats->mean != 0.0) {
        pstats->coeff = pstats->dev / pstats->mean;
    }
    else {
        pstats->coeff = 0.0;
    }

    if (fa != 0.0) {
        pstats->skew = ((double) sqrt((double) (ndat))) * fb / ((double) pow(fa, 1.5));
        pstats->kurt = (((double) (ndat)) * fc / (fa * fa)) - 3.0;
    }
    else {
        pstats->skew = 0.0;
        pstats->kurt = 0.0;
    }

    free(ptmp); ptmp = NULL;

    return pstats;
}

double data_luacalc(lua_State *pLua, int iparam, int *ierr)
{
    double *pdat = NULL, fpar = 0.0;
    stat_t statsT;
    int ii = 0, iDim = 0;

    int nArgs = lua_gettop(pLua);
    if (nArgs != 1) {
        *ierr = -1;
        return fpar;
    }

    if (!lua_istable(pLua, 1)) {
        *ierr = -1;
        return fpar;
    }
    iDim = 0;
    lua_pushnil(pLua);
    while (lua_next(pLua, 1) != 0) {
        iDim += 1;
        lua_pop(pLua, 1);
        if (iDim > data_MAXPOINTS) {
            break;
        }
    }
    if ((iDim < 1) || (iDim > data_MAXPOINTS)) {
        *ierr = -1;
        return fpar;
    }
    pdat = (double*) malloc(iDim * sizeof(double));
    if (pdat == NULL) {
        *ierr = -1;
        return fpar;
    }
    lua_pushnil(pLua);
    ii = 0;
    while (lua_next(pLua, 1) != 0) {
        if (lua_type(pLua, -1) == LUA_TNUMBER) {
            pdat[ii++] = lua_tonumber(pLua, -1);
        }
        lua_pop(pLua, 1);
        if (ii >= iDim) {
            break;
        }
    }

    memset(&statsT, 0, sizeof(statsT));

    data_calc((const double*) pdat, iDim, &statsT);
    switch (iparam) {
        case 1:
            fpar = statsT.min;
            break;
        case 2:
            fpar = statsT.max;
            break;
        case 3:
            fpar = statsT.sum;
            break;
        case 4:
            fpar = statsT.mean;
            break;
        case 5:
            fpar = statsT.median;
            break;
        case 6:
            fpar = statsT.var;
            break;
        case 7:
            fpar = statsT.dev;
            break;
        case 8:
            fpar = statsT.rms;
            break;
        case 9:
            fpar = statsT.coeff;
            break;
        case 10:
            fpar = statsT.skew;
            break;
        case 11:
            fpar = statsT.kurt;
            break;
        default:
            break;
    }
    free(pdat); pdat = NULL;
    *ierr = 0;
    return fpar;
}

int data_min(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 1, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.min: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_max(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 2, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.max: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_sum(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 3, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.sum: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_mean(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 4, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.mean: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_median(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 5, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.median: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_var(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 6, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.var: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_dev(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 7, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.dev: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_rms(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 8, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.rms: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_coeff(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 9, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.coeff: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_skew(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 10, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.skew: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

int data_kurt(lua_State *pLua)
{
    double fpar = 0.0;
    int ierr = 0;

    fpar = data_luacalc(pLua, 11, &ierr);
    if (ierr == -1) {
        return luaL_errorlevel(pLua, 2, "data.kurt: invalid argument");
    }

    lua_pushnumber(pLua, fpar);
    return 1;
}

// Functions exported to Lua - DOC
static int_t cf_docnew(lua_State *pLua)
{
    int_t nArgs = lua_gettop(pLua);
    char *pszArgA = NULL;
    int_t iArg = ML_STRSIZEN, iLen = 0;

    char_t szArgW[ML_STRSIZEN];
    memset(szArgW, 0, ML_STRSIZEN * sizeof(char_t));

    if (nArgs == 1) {
        if (lua_isstring(pLua, 1)) {
            pszArgA = (char*)lua_tostring(pLua, 1);
            if (pszArgA) {
                MultiByteToWideChar(CP_ACP, 0, pszArgA, -1, szArgW, ML_STRSIZEN - 1);
            }
        }
        else {
            lua_pushstring(pLua, "");

            return luaL_errorlevel(pLua, 2, "\'Doc:new\': invalid name");
        }
    }

    iLen = (int_t)_tcslen((const char_t*)szArgW);
    if ((iLen < 1) || (iLen > ML_STRSIZEN)) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:new\': invalid name");
    }

    CMainFrame* pFrame = (CMainFrame*) (SigmaApp.ActivateMainframe());
    if (pFrame == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:new\': internal error");
    }

    g_pDoc = pFrame->FindDoc((const char_t*)szArgW);

    if (g_pDoc == NULL) {
        int_t iID = SigmaApp.GetID();

        SigmaApp.OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL);

        int_t iNewID = SigmaApp.GetID();

        if (iNewID == iID) {
            lua_pushstring(pLua, "");

            return luaL_errorlevel(pLua, 2, "\'Doc:new\': document not created");
        }

        g_pDoc = pFrame->FindDoc(iNewID - 1);
        if (g_pDoc == NULL) {
            lua_pushstring(pLua, "");

            return luaL_errorlevel(pLua, 2, "\'Doc:new\': document not found");
        }

        g_pDoc->SetTitle((LPCTSTR)szArgW);
    }

    CString strT = g_pDoc->GetName();
    if (strT != _T("")) {
        char szBufferA[ML_STRSIZEN];
        memset(szBufferA, 0, ML_STRSIZEN * sizeof(char));
        SigmaApp.CStringToChar(strT, szArgW, ML_STRSIZEN - 1);
        WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)szArgW, -1, (LPSTR)szBufferA, ML_STRSIZEN, NULL, NULL);
        lua_pushstring(pLua, (const char*)szBufferA);
    }

    return 1;
}

static int_t cf_docfind(lua_State *pLua)
{
    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 1) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:find\': invalid number of arguments");
    }

    char *pszArgA = NULL;

    char_t szArgW[ML_STRSIZE];
    memset(szArgW, 0, ML_STRSIZE * sizeof(char_t));

    if (lua_isstring(pLua, 1)) {
        pszArgA = (char*)lua_tostring(pLua, 1);
        if (pszArgA) {
            MultiByteToWideChar(CP_ACP, 0, pszArgA, -1, szArgW, ML_STRSIZE);
        }
    }

    if (pszArgA == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:find\': document name not valid");
    }

    CString strName = (LPCTSTR)szArgW;

    CMainFrame* pFrame = (CMainFrame*) (SigmaApp.ActivateMainframe());
    if (pFrame == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:find\': internal error");
    }

    g_pDoc = pFrame->FindDoc(strName);
    if (g_pDoc == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:find\': document not found");
    }

    CString strT = g_pDoc->GetName();
    if (strT != _T("")) {
        char szBufferA[ML_STRSIZEN];
        memset(szBufferA, 0, ML_STRSIZEN * sizeof(char));
        SigmaApp.CStringToChar(strT, szArgW, ML_STRSIZEN - 1);
        WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)szArgW, -1, (LPSTR)szBufferA, ML_STRSIZEN - 1, NULL, NULL);
        lua_pushstring(pLua, (const char*)szBufferA);
    }

    return 1;
}

static int_t cf_docopen(lua_State *pLua)
{
    int_t iID = SigmaApp.GetID();

    SigmaApp.OnCmdMsg(ID_FILE_OPEN, 0, NULL, NULL);

    int_t iNewID = SigmaApp.GetID();

    if (iNewID == iID) {
        lua_pushstring(pLua, "");

        return 1;
    }

    CMainFrame* pFrame = (CMainFrame*) (SigmaApp.ActivateMainframe());
    if (pFrame == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:open\': internal error");
    }

    g_pDoc = pFrame->FindDoc(iNewID - 1);
    if (g_pDoc == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:open\': document not found");
    }

    CString strT = g_pDoc->GetName();
    if (strT != _T("")) {
        char szBufferA[ML_STRSIZEN];
        memset(szBufferA, 0, ML_STRSIZEN * sizeof(char));
        char_t szArgW[ML_STRSIZEN];
        memset(szArgW, 0, ML_STRSIZEN * sizeof(char_t));
        SigmaApp.CStringToChar(strT, szArgW, ML_STRSIZEN - 1);
        WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)szArgW, -1, (LPSTR)szBufferA, ML_STRSIZEN - 1, NULL, NULL);
        lua_pushstring(pLua, (const char*)szBufferA);
    }
    else {
        lua_pushstring(pLua, "");
    }

    return 1;
}

static int_t cf_docname(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Doc:name\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nLength = 0;
    char szBufferA[ML_STRSIZEN];
    memset(szBufferA, 0, ML_STRSIZEN * sizeof(char));
    char_t szBufferW[ML_STRSIZEN];
    memset(szBufferW, 0, ML_STRSIZEN * sizeof(char_t));

    CString strT = g_pDoc->GetName();
    SigmaApp.CStringToChar(strT, szBufferW, ML_STRSIZEN);

    WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)szBufferW, -1, (LPSTR)szBufferA, ML_STRSIZEN, NULL, NULL);
    lua_pushstring(pLua, (const char*)szBufferA);

    return 1;
}

static int_t cf_doccount(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Doc:count\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    lua_pushnumber(pLua, g_pDoc->GetColumnCount(0));

    return 1;
}

static int_t cf_docsave(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Doc:save\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    g_pDoc->SaveSigmaDoc();

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_docupdate(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Doc:update\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    g_pDoc->UpdateSigmaDoc();
    g_pDoc->UpdateAllViews(NULL);
    g_pDoc->ActivateView(1);

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_docclose(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return 1;
    }

    int_t iID = g_pDoc->GetID();

    g_pDoc->OnCmdMsg(ID_FILE_CLOSE, 0, 0, 0);

    CMainFrame* pFrame = (CMainFrame*) (SigmaApp.ActivateMainframe());
    if (pFrame == NULL) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Doc:close\': internal error");
    }

    if ((g_pDoc = pFrame->FindDoc(iID)) != NULL) {
        lua_pushboolean(pLua, 0);

        return 1;
    }

    lua_pushboolean(pLua, 1);

    return 1;
}

// Functions exported to Lua - DATA
static int_t cf_datainsert(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:insert\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 4) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:insert\': invalid number of arguments");
    }

    int_t dtype = (int_t)lua_tonumber(pLua, 1);
    int_t icount = g_pDoc->GetColumnCount(0);
    if (icount < 0) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:insert\': columns count not valid");
    }

    int_t iafter = (int_t)lua_tonumber(pLua, 2);
    int_t idim = (int_t)lua_tonumber(pLua, 3);
    int_t ctype = (int_t)lua_tonumber(pLua, 4);

    CDatasheet *pDatasheet = g_pDoc->GetDatasheet(0);
    if (pDatasheet == NULL) {
        lua_pushstring(pLua, "");

        return luaL_errorlevel(pLua, 2, "\'Data:insert\': invalid datasheet");
    }

    if (dtype == 1) {        // Column
        int_t nLength = 0;
        char_t szArgW[ML_STRSIZET];

        if (iafter != -1) {
            memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
            char* pszArgA = (char*)lua_tostring(pLua, 2);
            if (pszArgA == NULL) {
                lua_pushstring(pLua, "");

                return 1;
            }
            MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
            int_t idcol = g_pDoc->GetColumnID(0, (const char_t*)szArgW);
            if (idcol < 0) {
                lua_pushstring(pLua, "");

                return luaL_errorlevel(pLua, 2, "\'Data:insert\': column to insert after not found");
            }
            iafter = g_pDoc->GetColumnIndex(0, idcol);
            if (iafter < 0) {
                lua_pushstring(pLua, "");

                return luaL_errorlevel(pLua, 2, "\'Data:insert\': column not found");
            }
        }

        if (iafter < 0) {
            iafter = icount - 1;
        }

        vector_t *pVec = pDatasheet->InsertColumn(iafter, idim);

        if (pVec == NULL) {
            lua_pushstring(pLua, "");

            return luaL_errorlevel(pLua, 2, "\'Data:insert\': column not created");
        }

        pVec->type = (1 == ctype) ? 0x10 : 0x20;
        char szBufferA[ML_STRSIZEN];
        memset(szBufferA, 0, ML_STRSIZEN * sizeof(char));
        char_t szBufferW[ML_STRSIZEN];
        memset(szBufferW, 0, ML_STRSIZEN * sizeof(char_t));
        g_pDoc->GetColumnNameOrLabel(0, iafter + 1, szBufferW);
        WideCharToMultiByte(CP_UTF8, 0, (LPCTSTR)szBufferW, -1, (LPSTR)szBufferA, ML_STRSIZEN - 1, NULL, NULL);

        lua_pushstring(pLua, (const char*)szBufferA);

        return 1;
    }
    else if (dtype == 0) {        // Row
        int_t irows = g_pDoc->GetColumnDim(0, 0);

        if (iafter == -1) {
            iafter = irows - 1;
        }

        int_t ir = pDatasheet->InsertRows(iafter, idim);

        if (ir < 1) {
            lua_pushnumber(pLua, 0);

            return 1;
        }

        lua_pushnumber(pLua, iafter + 1);

        return 1;
    }

    lua_pushnumber(pLua, 0);

    return 1;
}

static int_t cf_dataformat(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:format\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 2) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:format\': invalid number of arguments");
    }

    char_t szArgW[ML_STRSIZET];
    memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));

    int_t idc = 0;
    if (lua_isstring(pLua, 1)) {
        char* pszArgA = (char*)lua_tostring(pLua, 1);
        if (pszArgA == NULL) {
            lua_pushboolean(pLua, 0);

            return 1;
        }
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
        idc = g_pDoc->GetColumnID(0, (const char_t*)szArgW);
    }
    if (idc < 1) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:format\': column not found");
    }

    memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
    char* pszArgA = (char*)lua_tostring(pLua, 2);
    if (pszArgA == NULL) {
        lua_pushboolean(pLua, 0);

        return 1;
    }
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
    BOOL bRet = g_pDoc->SetColumnFormat(0, -1, szArgW, idc);

    if (bRet == FALSE) {
        lua_pushboolean(pLua, 0);

        return 1;
    }

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_datadim(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:format\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 0) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:format\': invalid number of arguments");
    }

    int_t nColCount = g_pDoc->GetColumnCount(0), nRowCount = g_pDoc->GetColumnDim(0,0);
    lua_pushnumber(pLua, nColCount);
    lua_pushnumber(pLua, nRowCount);

    return 2;
}

static int_t cf_datadelete(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:delete\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 2) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:delete\': invalid number of arguments");
    }

    CDatasheet *pDatasheet = g_pDoc->GetDatasheet(0);
    if (pDatasheet == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:delete\': invalid datasheet");
    }

    int_t dtype = (int_t)lua_tonumber(pLua, 1);

    if (dtype == 1) {            // Column
        int_t idc = 0;
        if (lua_isstring(pLua, 2)) {
            int_t nLength = 0;
            char_t szArgW[ML_STRSIZET];

            memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
            char* pszArgA = (char*)lua_tostring(pLua, 2);
            if (pszArgA == NULL) {
                lua_pushboolean(pLua, 0);

                return 1;
            }
            MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
            idc = g_pDoc->GetColumnID(0, (const char_t*)szArgW);
        }
        if (idc < 1) {
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Data:delete\': column not found");
        }

        pDatasheet->DeleteColumn(g_pDoc->GetColumnIndex(0, idc));

        lua_pushboolean(pLua, 1);

        return 1;
    }
    else if (dtype == 0) {    // Row
        int_t irow = (int_t)lua_tonumber(pLua, 2);

        pDatasheet->DeleteRow(irow - 1, TRUE);

        lua_pushboolean(pLua, 1);

        return 1;
    }

    lua_pushboolean(pLua, 0);

    return 1;
}

static int_t cf_dataset(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if ((nArgs != 2) && (nArgs != 3)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid number of arguments");
    }
    if (!lua_isstring(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid argument #1 (should be string)");
    }

    bool bSetVal = (nArgs == 3);

    if (bSetVal) {
        if (!lua_isnumber(pLua, 2)) {
            return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid index");
        }
        if (!lua_isnumber(pLua, 3)) {
            return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid value");
        }
    }
    else {
        if (!lua_istable(pLua, 2)) {
            return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid table");
        }
    }

    int_t idc = 0, nLength = 0;
    char_t szArgW[ML_STRSIZET];
    memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
    char* pszArgA = (char*) lua_tostring(pLua, 1);
    if (pszArgA == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:set\': column name not valid");
    }
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR) pszArgA, -1, (LPTSTR) szArgW, ML_STRSIZET);
    idc = g_pDoc->GetColumnID(0, (const char_t*) szArgW);
    if (idc < 1) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:set\': column not found");
    }

    if (bSetVal) {
        int_t ii = (int_t) lua_tonumber(pLua, 2) - 1;
        real_t val = (real_t) lua_tonumber(pLua, 3);

        BOOL bRet = g_pDoc->SetColumnItem(0, -1, ii, val, idc);

        if (bRet == FALSE) {
            lua_pushboolean(pLua, 0);

            return 1;
        }

        byte_t bMask = g_pDoc->GetColumnItemMask(0, -1, ii, idc);
        if ((bMask & 0x0F) == 0x0F) {
            g_pDoc->SetColumnItemMask(0, -1, ii, 0x00, idc);
        }
    }
    else {
        int iDim = luaL_len(pLua, 2);
        if ((iDim < 1) || (iDim > ML_MAXPOINTS)) {
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid argument #1 (array size)");
        }
        double *pDat = (double*) malloc(iDim * sizeof(double));
        if (pDat == NULL) {
            return luaL_errorlevel(pLua, 2, "\'Data:set\': insufficient memory");
        }
        int iret = lua_toarrayd(pLua, 2, pDat, iDim);
        if (iret != 0) {
            free(pDat); pDat = NULL;
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Data:set\': invalid argument #1");
        }

        vector_t *pColumn = g_pDoc->GetColumn(0, -1, idc);
        if (pColumn == NULL) {
            free(pDat); pDat = NULL;
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Data:set\': column not found");
        }

        int iPoints = (pColumn->dim > iDim) ? iDim : pColumn->dim;
        for (int ii = 0; ii < iPoints; ii++) {
            pColumn->dat[ii] = pDat[ii];
            if ((pColumn->mask[ii] & 0x0F) == 0x0F) {
                pColumn->mask[ii] = 0x00;
            }
        }

        free(pDat); pDat = NULL;
    }

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_dataget(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:get\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if ((nArgs != 1) && (nArgs != 2)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:get\': invalid number of arguments");
    }
    if (!lua_isstring(pLua, 1)) {
        return luaL_errorlevel(pLua, 2, "\'Data:get\': invalid argument #1 (should be string)");
    }

    bool bGetVal = (nArgs == 2);

    if (bGetVal) {
        if (!lua_isnumber(pLua, 2)) {
            return luaL_errorlevel(pLua, 2, "\'Data:get\': invalid index");
        }
    }

    int_t idc = 0;
    int_t nLength = 0;
    char_t szArgW[ML_STRSIZET];

    memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
    char* pszArgA = (char*) lua_tostring(pLua, 1);
    if (pszArgA == NULL) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:get\': column name not valid");
    }
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR) pszArgA, -1, (LPTSTR) szArgW, ML_STRSIZET);
    idc = g_pDoc->GetColumnID(0, (const char_t*) szArgW);
    if (idc < 1) {
        lua_pushnumber(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:get\': column not found");
    }

    if (bGetVal) {
        int_t ii = (int_t) lua_tonumber(pLua, 2) - 1;
        real_t val = 0.0;

        val = g_pDoc->GetColumnItem(0, -1, ii, idc);

        lua_pushnumber(pLua, val);
    }
    else {

        vector_t *pColumn = g_pDoc->GetColumn(0, -1, idc);
        if (pColumn == NULL) {
            lua_pushnumber(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Data:set\': column not found");
        }

        // Create table to be returned
        lua_newtable(pLua);
        for (int ii = 0; ii < pColumn->dim; ii++) {
            lua_pushnumber(pLua, pColumn->dat[ii]);
            lua_rawseti(pLua, -2, ii + 1);
        }
    }

    return 1;
}

static int_t cf_datamask(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:mask\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 3) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:mask\': invalid number of arguments");
    }

    int_t idc = 0;
    if (lua_isstring(pLua, 1)) {
        int_t nLength = 0;
        char_t szArgW[ML_STRSIZET];

        memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
        char* pszArgA = (char*)lua_tostring(pLua, 1);
        if (pszArgA == NULL) {
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Data:mask\': column name not valid");
        }
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
        idc = g_pDoc->GetColumnID(0, (const char_t*)szArgW);
    }
    if (idc < 1) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:mask\': column not found");
    }

    int_t ii = (int_t)lua_tonumber(pLua, 2) - 1;
    byte_t bmask = (byte_t)lua_tonumber(pLua, 3);

    g_pDoc->SetColumnItemMask(0, -1, ii, bmask, idc);

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_datasort(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:sort\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 2) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:sort\': invalid number of arguments");
    }

    int_t idc = 0;
    if (lua_isstring(pLua, 1)) {
        int_t nLength = 0;
        char_t szArgW[ML_STRSIZET];

        memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
        char* pszArgA = (char*)lua_tostring(pLua, 1);
        if (pszArgA == NULL) {
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Data:sort\': column name not valid");
        }
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
        idc = g_pDoc->GetColumnID(0, (const char_t*)szArgW);
    }
    if (idc < 1) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:sort\': column not found");
    }

    byte_t bAsc = (byte_t)lua_tonumber(pLua, 2);

    int_t jj = g_pDoc->GetColumnIndex(0, idc);
    if (jj < 0) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Data:sort\': column not found");
    }

    BOOL bRet = g_pDoc->SortColumns(0, jj, bAsc, NULL);
    g_pDoc->UpdateAllViews(NULL);

    lua_pushboolean(pLua, (int_t)bRet);

    return 1;
}

bool doFit(double *px, double *py, int idim, fit_t *pFit, libvar_t *pLib, int *piter, double *pchi)
{
    if (pLib == NULL) {
        return false;
    }
    if ((piter == NULL) || (pchi == NULL)) {
        _tcscpy(pLib->message, _T("Cannot perform operation: invalid data"));
        return false;
    }
    if (pFit == NULL) {
        _tcscpy(pLib->message, _T("Cannot perform operation: no fitting model selected"));
        return false;
    }
    if (idim < 2) {
        _tcscpy(pLib->message, _T("Cannot perform operation: fitting needs at least 2 points"));
        return false;
    }
    if (idim > ML_MAXPOINTS) {
        _tcscpy(pLib->message, _T("Cannot perform fitting: too many data points"));
        return false;
    }

    *piter = 0;
    *pchi = 10.0;

    int_t ii;

    int_t iFixed = 0;
    for (ii = 0; ii < pFit->parcount; ii++) {
        iFixed += pFit->mpar[ii];
    }

    // Tous les param�tres fix�s
    if (iFixed == 0) {
        return true;
    }

    double *pw = (double*) malloc(idim * sizeof(double));
    if (pw == NULL) {
        _tcscpy(pLib->message, _T("Cannot perform fitting: insufficient memory"));
        return false;
    }

    for (ii = 0; ii < idim; ii++) {
        pw[ii] = 1.0;
    }

    pLib->errcode = 0;
    int_t nIter = pFit->iter;
    real_t fLambda = pFit->lambda;
    pFit->chi = ml_fit_levmarq(pFit->index - 1, px, py, pw, idim, pFit->fpar, pFit->resd, pFit->mpar, pFit->parcount, NULL, &nIter, &fLambda, pLib);

    if (pLib->errcode == 0) {
        if (pFit->chi >= 1.0) {
            vector_t *pvecX = (vector_t*) malloc(sizeof(vector_t));
            vector_t *pvecY = (vector_t*) malloc(sizeof(vector_t));
            if (pvecX && pvecY) {
                memset(pvecX, 0, sizeof(vector_t));
                memset(pvecY, 0, sizeof(vector_t));
                pvecX->dat = px;
                pvecX->dim = idim;
                pvecX->istart = 0;
                pvecX->istop = idim - 1;
                pvecY->dat = py;
                pvecY->dim = idim;
                pvecY->istart = 0;
                pvecY->istop = idim - 1;
                ml_fit_autopar(pvecX, pvecY, pFit->index - 1, pFit->fpar, pLib);

                nIter = pFit->iter;
                fLambda = pFit->lambda;
                pFit->chi = ml_fit_levmarq(pFit->index - 1, px, py, pw, idim, pFit->fpar, pFit->resd, pFit->mpar, pFit->parcount, NULL, &nIter, &fLambda, pLib);
            }
            if (pvecX) {
                free(pvecX); pvecX = NULL;
            }
            if (pvecY) {
                free(pvecY); pvecY = NULL;
            }
        }
    }

    free(pw); pw = NULL;

    *piter = nIter;
    *pchi = pFit->chi;

    _tcsprintf(pLib->message, ML_STRSIZE - 1, _T("Fitting done: iters = %d   chi = %g"), *piter, *pchi);
    return (pLib->errcode == 0);
}

//chi,iters,fpar = data_fit(model, x, y, fpar, ipar, tol, iters)
int data_fit(lua_State *pLua)
{
    double *px = NULL, *py = NULL;
    fit_t *pFit = NULL;
    libvar_t *pLib = NULL;

    double ftol = 1e-6, flambda = 10.0, fchi = 1.0;
    int iters = 100;

    // Get the Lua number of arguments
    int nArgCount = lua_gettop(pLua);
    if (nArgCount != 7) {
        return luaL_errorlevel(pLua, 2, "data.fit: invalid number of arguments (should be 7)");
    }

    if (!lua_isstring(pLua, 1)) {    // func
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #1");
    }
    if (!lua_istable(pLua, 2)) {    // x
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #2");
    }
    if (!lua_istable(pLua, 3)) {    // y
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #3");
    }
    if (!lua_istable(pLua, 4)) {    // fpar
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #4");
    }
    if (!lua_istable(pLua, 5)) {    // ipar
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #5");
    }
    if (!lua_isnumber(pLua, 6)) {    // tol
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #6");
    }
    if (!lua_isnumber(pLua, 7)) {    // iters
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #7");
    }

    pLib = (libvar_t*) malloc(sizeof(libvar_t));
    if (pLib == NULL) {
        return luaL_errorlevel(pLua, 2, "data.fit: insufficient memory");
    }
    memset(pLib, 0, sizeof(libvar_t));

    char_t szModel[ML_STRSIZEN];
    memset(szModel, 0, ML_STRSIZEN * sizeof(char_t));
    const char* pszFunc = (const char*) lua_tostring(pLua, 1);

    mbstowcs(szModel, pszFunc, ML_STRSIZEN - 1);
    pFit = (fit_t*) malloc(sizeof(fit_t));
    if (pFit == NULL) {
        free(pLib); pLib = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: insufficient memory");
    }
    memset(pFit, 0, sizeof(fit_t));
    int iret = ml_fit_initmodel(pFit, (const char_t*) szModel);
    if (iret < 0) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: unknown fitting model");
    }

    // dataCount
    int dataCountx = luaL_len(pLua, 2);
    int dataCounty = luaL_len(pLua, 3);
    if ((dataCountx < 2) || (dataCountx >= ML_MAXPOINTS) || (dataCountx != dataCounty)) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid data dimension");
    }

    // parCount
    int parCount = luaL_len(pLua, 4);    // fpar
    int parCounti = luaL_len(pLua, 5);    // mpar
    if ((parCount < 2) || (parCount > ML_FIT_MAXPAR) || (parCounti < 2) || (parCounti > ML_FIT_MAXPAR) || (parCount > parCounti)) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid number of fitting parameters");
    }
    parCounti = parCount;

    px = (double*) malloc(dataCountx * sizeof(double));
    if (px == NULL) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: insufficient memory");
    }
    py = (double*) malloc(dataCounty * sizeof(double));
    if (py == NULL) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        free(px); px = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: insufficient memory");
    }

    // pX
    iret = lua_toarrayd(pLua, 2, px, dataCountx);
    if (iret != 0) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        free(px); px = NULL;
        free(py); py = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #2");
    }

    // pY
    iret = lua_toarrayd(pLua, 3, py, dataCounty);
    if (iret != 0) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        free(px); px = NULL;
        free(py); py = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #3");
    }

    // fpar
    iret = lua_toarrayd(pLua, 4, pFit->fpar, parCount);
    if (iret != 0) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        free(px); px = NULL;
        free(py); py = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #4");
    }

    // mpar
    iret = lua_toarrayi(pLua, 5, pFit->mpar, parCount);
    if (iret != 0) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        free(px); px = NULL;
        free(py); py = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #5");
    }

    // tol
    ftol = lua_tonumber(pLua, 6);
    if ((ftol < 1e-15) || (ftol > 1e-1)) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        free(px); px = NULL;
        free(py); py = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #6");
    }

    // iters
    iters = (int) lua_tonumber(pLua, 7);
    if ((iters < 1) || (iters > ML_MAXITER)) {
        free(pLib); pLib = NULL;
        free(pFit); pFit = NULL;
        free(px); px = NULL;
        free(py); py = NULL;
        return luaL_errorlevel(pLua, 2, "data.fit: invalid argument #7");
    }

    bool bret = doFit(px, py, dataCountx, pFit, pLib, &iters, &fchi);

    char szErr[ML_STRSIZE];
    memset(szErr, 0, ML_STRSIZE * sizeof(char));
    wcstombs((char*) szErr, (const char_t*) (pLib->message), ML_STRSIZE - 1);

    // Create table to be returned
    lua_newtable(pLua);
    for (int ii = 0; ii < parCount; ii++) {
        lua_pushnumber(pLua, pFit->fpar[ii]);
        lua_rawseti(pLua, -2, ii + 1);
    }

    lua_pushnumber(pLua, fchi);
    lua_pushnumber(pLua, iters);
    lua_pushstring(pLua, (const char*) szErr);

    free(pLib); pLib = NULL;
    free(pFit); pFit = NULL;
    free(px); px = NULL;
    free(py); py = NULL;

    return 4;
}

//ts = data_sort(data, asc)
int data_sort(lua_State *pLua)
{
    double *pDat = NULL;
    libvar_t *pLib = NULL;

    // Get the Lua number of arguments
    int nArgCount = lua_gettop(pLua);
    if (nArgCount != 2) {
        return luaL_errorlevel(pLua, 2, "data.sort: invalid number of arguments (should be 2)");
    }

    if (!lua_istable(pLua, 1)) {    // data
        return luaL_errorlevel(pLua, 2, "data.sort: invalid argument #1");
    }
    if (!lua_isnumber(pLua, 2)) {    // asc
        return luaL_errorlevel(pLua, 2, "data.sort: invalid argument #2");
    }

    // tol
    int iasc = lua_tonumber(pLua, 2);
    if ((iasc != 1) && (iasc != 0)) {
        return luaL_errorlevel(pLua, 2, "data.sort: invalid argument #2 (should be 1 or 0)");
    }

    pLib = (libvar_t*) malloc(sizeof(libvar_t));
    if (pLib == NULL) {
        return luaL_errorlevel(pLua, 2, "data.sort: insufficient memory");
    }
    memset(pLib, 0, sizeof(libvar_t));

    // dataCount
    int dataCount = luaL_len(pLua, 1);
    if ((dataCount < 2) || (dataCount > ML_MAXPOINTS)) {
        free(pLib); pLib = NULL;
        return luaL_errorlevel(pLua, 2, "data.sort: invalid data dimension");
    }

    pDat = (double*) malloc(dataCount * sizeof(double));
    if (pDat == NULL) {
        free(pLib); pLib = NULL;
        return luaL_errorlevel(pLua, 2, "data.sort: insufficient memory");
    }

    // pX
    int iret = lua_toarrayd(pLua, 1, pDat, dataCount);
    if (iret != 0) {
        free(pLib); pLib = NULL;
        free(pDat); pDat = NULL;
        return luaL_errorlevel(pLua, 2, "data.sort: invalid argument #1");
    }

    real_t* pRet = ml_sort(pDat, dataCount, (iasc == 1) ? 1 : 0, pLib);
    if (pRet == NULL) {
        free(pLib); pLib = NULL;
        free(pDat); pDat = NULL;
        return luaL_errorlevel(pLua, 2, "data.sort: invalid data");
    }

    // Create table to be returned
    lua_newtable(pLua);
    for (int ii = 0; ii < dataCount; ii++) {
        lua_pushnumber(pLua, pDat[ii]);
        lua_rawseti(pLua, -2, ii + 1);
    }

    free(pLib); pLib = NULL;
    free(pDat); pDat = NULL;

    return 1;
}

static int_t cf_dataupdate(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Doc:update\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    g_pDoc->UpdateSigmaDoc();
    g_pDoc->UpdateAllViews(NULL);
    g_pDoc->ActivateView(0);

    lua_pushboolean(pLua, 1);

    return 1;
}

// Functions exported to Lua - PLOT
static int_t cf_plottitle(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:title\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    plot_t *pPlot = g_pDoc->GetPlot();
    if (pPlot == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:title\': invalid plot");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 1) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:title\': invalid number of arguments");
    }

    int_t nLength = 0;
    char_t szArgW[ML_STRSIZE];
    memset(szArgW, 0, ML_STRSIZE * sizeof(char_t));

    char* pszArgA = (char*)lua_tostring(pLua, 1);
    if (pszArgA == NULL) {
        lua_pushboolean(pLua, 0);

        return 1;
    }

    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZE);

    _tcscpy(pPlot->title.text, (const char_t*) szArgW);

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_plotframe(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:frame\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    plot_t *pPlot = g_pDoc->GetPlot();
    if (pPlot == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:frame\': invalid plot");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 6) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:frame\': invalid number of arguments");
    }

    byte_t backr = (byte_t)lua_tonumber(pLua, 1);
    byte_t backg = (byte_t)lua_tonumber(pLua, 2);
    byte_t backb = (byte_t)lua_tonumber(pLua, 3);
    byte_t plotr = (byte_t)lua_tonumber(pLua, 4);
    byte_t plotg = (byte_t)lua_tonumber(pLua, 5);
    byte_t plotb = (byte_t)lua_tonumber(pLua, 6);

    pPlot->backcolor.r = backr;
    pPlot->backcolor.g = backg;
    pPlot->backcolor.b = backb;
    pPlot->pbackcolor.r = plotr;
    pPlot->pbackcolor.g = plotg;
    pPlot->pbackcolor.b = plotb;

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_plotadd(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:add\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    plot_t *pPlot = g_pDoc->GetPlot();
    if (pPlot == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:add\': invalid plot");
    }
    if (pPlot->curvecount >= SIGMA_MAXCURVES) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:add\': maximum curves count reached");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 4) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:add\': invalid number of arguments");
    }

    int_t idx = 0, idy = 0;
    if (lua_isstring(pLua, 1) && lua_isstring(pLua, 2)) {
        int_t nLength = 0;
        char_t szArgW[ML_STRSIZET];

        memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
        char* pszArgA = (char*)lua_tostring(pLua, 1);
        if (pszArgA == NULL) {
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Plot:add\': column name not valid");
        }
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
        idx = g_pDoc->GetColumnID(0, (const char_t*)szArgW);

        memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
        pszArgA = (char*)lua_tostring(pLua, 2);
        if (pszArgA == NULL) {
            lua_pushboolean(pLua, 0);

            return luaL_errorlevel(pLua, 2, "\'Plot:add\': column name not valid");
        }
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
        idy = g_pDoc->GetColumnID(0, (const char_t*)szArgW);
    }
    if ((idx < 1) || (idy < 1)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:add\': column not found");
    }

    byte_t istyle = (byte_t)lua_tonumber(pLua, 3),
        linestyle, dotstyle;

    if (istyle == 1) {
        linestyle = 0x11;
        dotstyle = 0x00;
    }
    else if (istyle == 2) {
        linestyle = 0x00;
        dotstyle = 0x21;
    }
    else if (istyle == 3) {
        linestyle = 0x11;
        dotstyle = 0x21;
    }
    else {
        linestyle = SigmaApp.m_pDefaultPlot->curve[0].linestyle;
        dotstyle = SigmaApp.m_pDefaultPlot->curve[0].dotstyle;
    }

    byte_t iaxis = (byte_t)lua_tonumber(pLua, 4), axisused = 0x00;

    if (iaxis == 1) {
        axisused = 0x00;
    }
    else if (iaxis == 2) {
        axisused = 0x01;
    }
    else if (iaxis == 3) {
        axisused = 0x10;
    }
    else if (iaxis == 4) {
        axisused = 0x11;
    }
    else {
        axisused = SigmaApp.m_pDefaultPlot->curve[0].axisused;
    }

    if (g_pDoc->CurveAdd(0, idx, idy, linestyle, dotstyle, axisused, FALSE) >= 0) {        // OK
        lua_pushboolean(pLua, 1);

        return 1;
    }

    lua_pushboolean(pLua, 0);

    return luaL_errorlevel(pLua, 2, "\'Plot:add\': cannot add curve");
}

static int_t cf_plotremove(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:remove\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    plot_t *pPlot = g_pDoc->GetPlot();
    if (pPlot == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:remove\': invalid plot");
    }
    if ((pPlot->curvecount < 1) || (pPlot->curvecount > SIGMA_MAXCURVES)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:remove\': invalid plot curves count");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 1) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:remove\': invalid number of arguments");
    }

    int_t idy = 0;
    if (lua_isstring(pLua, 1)) {
        int_t nLength = 0;
        char_t szArgW[ML_STRSIZET];

        memset(szArgW, 0, ML_STRSIZET * sizeof(char_t));
        char* pszArgA = (char*)lua_tostring(pLua, 1);
        if (pszArgA == NULL) {
            lua_pushboolean(pLua, 0);

            return 1;
        }
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZET);
        idy = g_pDoc->GetColumnID(0, (const char_t*)szArgW);
    }
    if (idy < 1) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:remove\': curve not found");
    }

    if (g_pDoc->CurveRemove(0, idy, FALSE) >= 0) {
        lua_pushboolean(pLua, 1);

        return 1;
    }

    lua_pushboolean(pLua, 0);

    return 1;
}

static int_t cf_plotaxis(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:axis\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    plot_t *pPlot = g_pDoc->GetPlot();
    if (pPlot == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:axis\': invalid plot");
    }

    int_t nArgs = lua_gettop(pLua);

    if ((nArgs < 2) || (nArgs > 7)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:axis\': invalid number of arguments");
    }

    int_t iaxis = (int_t)lua_tonumber(pLua, 1);
    byte_t iscale = (byte_t)lua_tonumber(pLua, 2);
    real_t fmin, fmax;

    if ((iscale < 1) || (iscale > 2)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:axis\': invalid axis scale (iscale = 1 or 2)");
    }

    axis_t *pAxis = NULL;
    switch (iaxis) {
    case 1:
        pAxis = &(pPlot->axisbottom);
        break;
    case 2:
        pAxis = &(pPlot->axisleft);
        break;
    case 3:
        pAxis = &(pPlot->axistop);
        break;
    case 4:
        pAxis = &(pPlot->axisright);
        break;
    default:
        break;
    }
    if (pAxis == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:axis\': invalid axis (iaxis = 1, 2, 3 or 4)");
    }

    if (nArgs > 5) {
        pAxis->otitle = (byte_t)lua_tonumber(pLua, 6);
    }
    if (nArgs > 6) {
        int_t nLength = 0;
        char_t szArgW[ML_STRSIZE];
        memset(szArgW, 0, ML_STRSIZE * sizeof(char_t));

        char* pszArgA = (char*)lua_tostring(pLua, 7);
        if (pszArgA == NULL) {
            lua_pushboolean(pLua, 0);

            return 1;
        }

        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszArgA, -1, (LPTSTR)szArgW, ML_STRSIZE);

        pAxis->otitle = (byte_t)lua_tonumber(pLua, 6);
        _tcscpy(pAxis->title.text, (const char_t*)szArgW);
    }

    pAxis->scale = iscale - 1;

    byte_t iautoscale = 0;

    if (nArgs > 2) {
        iautoscale = (byte_t)lua_tonumber(pLua, 3);
        if (iautoscale) {
            pl_autoscale(pPlot);
        }
    }
    if (nArgs > 3) {
        fmin = (real_t)lua_tonumber(pLua, 4);
        if (iautoscale == 0) {
            pAxis->min = fmin;
        }
    }
    if (nArgs > 4) {
        fmax = (real_t)lua_tonumber(pLua, 5);
        if (fmax <= fmin) {
            lua_pushboolean(pLua, 0);

            return 1;
        }
        if (iautoscale == 0) {
            pAxis->max = fmax;
        }
    }

    lua_pushboolean(pLua, 0);

    return 1;
}

static int_t cf_plotlabel(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:label\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    plot_t *pPlot = g_pDoc->GetPlot();
    if (pPlot == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:label\': invalid plot");
    }

    int_t nArgs = lua_gettop(pLua);

    if (nArgs != 2) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:label\': invalid number of arguments");
    }

    int_t iaxis = (int_t)lua_tonumber(pLua, 1);
    byte_t ilabel = (byte_t)lua_tonumber(pLua, 2);

    if ((ilabel != 0) && (ilabel != 1)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:label\': invalid axis label (ilabel = 0 or 1)");
    }

    axis_t *pAxis = NULL;
    switch (iaxis) {
    case 1:
        pAxis = &(pPlot->axisbottom);
        break;
    case 2:
        pAxis = &(pPlot->axisleft);
        break;
    case 3:
        pAxis = &(pPlot->axistop);
        break;
    case 4:
        pAxis = &(pPlot->axisright);
        break;
    default:
        break;
    }
    if (pAxis == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:label\': invalid axis (iaxis = 1, 2, 3 or 4)");
    }

    pAxis->olabel = ilabel;

    lua_pushboolean(pLua, 0);

    return 1;
}

static int_t cf_plotcolor(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:color\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    plot_t *pPlot = g_pDoc->GetPlot();
    if (pPlot == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:color\': invalid plot");
    }
    if ((pPlot->curvecount < 1) || (pPlot->curvecount > SIGMA_MAXCURVES)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:color\': invalid plot curves count");
    }

    int_t nArgs = lua_gettop(pLua);

    if ((nArgs != 4) && (nArgs != 7)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:color\': invalid number of arguments");
    }

    int_t icurve = (int_t)lua_tonumber(pLua, 1) - 1;
    byte_t lr = (byte_t)lua_tonumber(pLua, 2);
    byte_t lg = (byte_t)lua_tonumber(pLua, 3);
    byte_t lb = (byte_t)lua_tonumber(pLua, 4);

    if ((icurve < 0) || (icurve >= pPlot->curvecount)) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Plot:color\': invalid curve");
    }

    pPlot->curve[icurve].linecolor.r = lr;
    pPlot->curve[icurve].linecolor.g = lg;
    pPlot->curve[icurve].linecolor.b = lb;

    if (nArgs == 7) {
        pPlot->curve[icurve].dotcolor.r = (byte_t)lua_tonumber(pLua, 5);
        pPlot->curve[icurve].dotcolor.g = (byte_t)lua_tonumber(pLua, 6);
        pPlot->curve[icurve].dotcolor.b = (byte_t)lua_tonumber(pLua, 7);
    }

    lua_pushboolean(pLua, 1);

    return 1;
}

static int_t cf_plotupdate(lua_State *pLua)
{
    if (g_pDoc == NULL) {
        lua_pushboolean(pLua, 0);

        return luaL_errorlevel(pLua, 2, "\'Doc:update\': invalid document (use \'new\', \'open\' or \'find\')");
    }

    g_pDoc->UpdateSigmaDoc();
    g_pDoc->UpdateAllViews(NULL);
    g_pDoc->ActivateView(1);

    lua_pushboolean(pLua, 1);

    return 1;
}

BOOL initScriptEngine(lua_State *pScriptEngine)
{
    if (pScriptEngine == NULL) {
        return FALSE;        
    }

    // >> Load Lua base libraries
    luaL_openlibs(pScriptEngine);
    // <<

    // >> GENERAL FUNCTIONS
    lua_atpanic(pScriptEngine, cf_panic);
    lua_register(pScriptEngine, "clear", cf_clear);
    lua_register(pScriptEngine, "cls", cf_clear);
    lua_register(pScriptEngine, "io_print", cf_print);
    lua_register(pScriptEngine, "io_write", cf_write);
    lua_register(pScriptEngine, "print", cf_print);
    lua_register(pScriptEngine, "io_read", cf_read);
    lua_register(pScriptEngine, "io_input", cf_read);

    lua_register(pScriptEngine, "read", cf_read);
    lua_register(pScriptEngine, "error", cf_error);
    lua_register(pScriptEngine, "pause", cf_pause);
    lua_register(pScriptEngine, "sleep", cf_pause);
    lua_register(pScriptEngine, "tic", cf_tic);
    lua_register(pScriptEngine, "toc", cf_toc);
    lua_register(pScriptEngine, "stopit", cf_stopit);
    // <<

    // >> MATH
    lua_register(pScriptEngine, "unit", cf_unit);
    lua_register(pScriptEngine, "j0", cf_j0);
    lua_register(pScriptEngine, "j1", cf_j1);
    lua_register(pScriptEngine, "jn", cf_jn);
    lua_register(pScriptEngine, "y0", cf_y0);
    lua_register(pScriptEngine, "y1", cf_y1);
    lua_register(pScriptEngine, "yn", cf_yn);
    lua_register(pScriptEngine, "pi", cf_pi);
    lua_register(pScriptEngine, "min", cf_min);
    lua_register(pScriptEngine, "max", cf_max);
    lua_register(pScriptEngine, "rand", cf_rand);
    lua_register(pScriptEngine, "time", cf_time);
    lua_register(pScriptEngine, "sign", cf_sign);
    lua_register(pScriptEngine, "bern", cf_bern);
    lua_register(pScriptEngine, "dbern", cf_dbern);
    lua_register(pScriptEngine, "fact", cf_fact);
    // <<

    // MATH
    lua_register(pScriptEngine, "lmath_kb", lmath_kb);
    lua_register(pScriptEngine, "lmath_q", lmath_q);
    lua_register(pScriptEngine, "lmath_me", lmath_me);
    lua_register(pScriptEngine, "lmath_h", lmath_h);
    lua_register(pScriptEngine, "lmath_c", lmath_c);
    lua_register(pScriptEngine, "lmath_eps0", lmath_eps0);
    lua_register(pScriptEngine, "lmath_na", lmath_na);
    lua_register(pScriptEngine, "lmath_kbt", lmath_kbt);
    lua_register(pScriptEngine, "lmath_exp2", lmath_exp2);
    lua_register(pScriptEngine, "lmath_logb", lmath_logb);
    lua_register(pScriptEngine, "lmath_cbrt", lmath_cbrt);
    lua_register(pScriptEngine, "lmath_hypot", lmath_hypot);
    lua_register(pScriptEngine, "lmath_lgamma", lmath_lgamma);
    lua_register(pScriptEngine, "lmath_tgamma", lmath_tgamma);
    lua_register(pScriptEngine, "lmath_trunc", lmath_trunc);
    lua_register(pScriptEngine, "lmath_round", lmath_round);
    lua_register(pScriptEngine, "lmath_rint", lmath_rint);
    lua_register(pScriptEngine, "lmath_isinf", lmath_isinf);
    lua_register(pScriptEngine, "lmath_isnan", lmath_isnan);
    lua_register(pScriptEngine, "lmath_isnormal", lmath_isnormal);
    lua_register(pScriptEngine, "lmath_erf", lmath_erf);
    lua_register(pScriptEngine, "lmath_erfc", lmath_erfc);
    lua_register(pScriptEngine, "lmath_asinh", lmath_asinh);
    lua_register(pScriptEngine, "lmath_acosh", lmath_acosh);
    lua_register(pScriptEngine, "lmath_atanh", lmath_atanh);
    lua_register(pScriptEngine, "lmath_gauss", lmath_gauss);
    lua_register(pScriptEngine, "lmath_lorentz", lmath_lorentz);
    //

    // STATS
    lua_register(pScriptEngine, "data_min", data_min);
    lua_register(pScriptEngine, "data_max", data_max);
    lua_register(pScriptEngine, "data_sum", data_sum);
    lua_register(pScriptEngine, "data_mean", data_mean);
    lua_register(pScriptEngine, "data_median", data_median);
    lua_register(pScriptEngine, "data_var", data_var);
    lua_register(pScriptEngine, "data_dev", data_dev);
    lua_register(pScriptEngine, "data_rms", data_rms);
    lua_register(pScriptEngine, "data_coeff", data_coeff);
    lua_register(pScriptEngine, "data_skew", data_skew);
    lua_register(pScriptEngine, "data_kurt", data_kurt);
    lua_register(pScriptEngine, "data_fit", data_fit);
    lua_register(pScriptEngine, "data_sort", data_sort);
        // <<

    // >> PHYSICS
    lua_register(pScriptEngine, "physcurrent", cf_phys_current);
    lua_register(pScriptEngine, "physcapacitance", cf_phys_capacitance);
    lua_register(pScriptEngine, "physconductance", cf_phys_conductance);
    // <<

    // >> DOC
    lua_register(pScriptEngine, "docnew", cf_docnew);
    lua_register(pScriptEngine, "docname", cf_docname);
    lua_register(pScriptEngine, "doccount", cf_doccount);
    lua_register(pScriptEngine, "docfind", cf_docfind);
    lua_register(pScriptEngine, "docopen", cf_docopen);
    lua_register(pScriptEngine, "docsave", cf_docsave);
    lua_register(pScriptEngine, "docupdate", cf_docupdate);
    lua_register(pScriptEngine, "docclose", cf_docclose);
    // <<

    // >> DATA
    lua_register(pScriptEngine, "datadim", cf_datadim);
    lua_register(pScriptEngine, "datainsert", cf_datainsert);
    lua_register(pScriptEngine, "dataformat", cf_dataformat);
    lua_register(pScriptEngine, "datadelete", cf_datadelete);
    lua_register(pScriptEngine, "dataset", cf_dataset);
    lua_register(pScriptEngine, "dataget", cf_dataget);
    lua_register(pScriptEngine, "datamask", cf_datamask);
    lua_register(pScriptEngine, "datasort", cf_datasort);
    lua_register(pScriptEngine, "dataupdate", cf_dataupdate);
    // <<

    // >> PLOT
    lua_register(pScriptEngine, "plotframe", cf_plotframe);
    lua_register(pScriptEngine, "plottitle", cf_plottitle);
    lua_register(pScriptEngine, "plotadd", cf_plotadd);
    lua_register(pScriptEngine, "plotremove", cf_plotremove);
    lua_register(pScriptEngine, "plotaxis", cf_plotaxis);
    lua_register(pScriptEngine, "plotlabel", cf_plotlabel);
    lua_register(pScriptEngine, "plotcolor", cf_plotcolor);
    lua_register(pScriptEngine, "plotupdate", cf_plotupdate);
    // <<

    int_t iSize = ML_STRSIZE * ML_STRSIZE;
    char* pszSigma = (char*) malloc(iSize * sizeof(char));
    if (pszSigma == NULL) {
        return FALSE;
    }

    const char* MODULE_MATH =
        "function readOnly(builtinTable)\n"
        "    local proxy = {}\n"
        "    local mt = {\n"
        "        __index = builtinTable,\n"
        "        __newindex = function (builtinTable,k,v)\n"
        "            error(\"Cannot modify SigmaScript builtin functions\", 2)\n"
        "        end\n"
        "    }\n"
        "    setmetatable(proxy, mt)\n"
        "    return proxy\n"
        "end\n"
        "\n"
        "lmath = math or {}\n"
        "\n"
        "lmath.kb = lmath_kb()\n"
        "lmath.q = lmath_q()\n"
        "lmath.me = lmath_me()\n"
        "lmath.h = lmath_h()\n"
        "lmath.c = lmath_c()\n"
        "lmath.eps0 = lmath_eps0()\n"
        "lmath.na = lmath_na()\n"
        "lmath.kbt = lmath_kbt()\n"
        "\n"
        "function lmath.exp2(x)\n"
        "    return lmath_exp2(x)\n"
        "end\n"
        "\n"
        "function lmath.logb(x)\n"
        "    return lmath_logb(x)\n"
        "end\n"
        "\n"
        "function lmath.cbrt(x)\n"
        "    return lmath_cbrt(x)\n"
        "end\n"
        "\n"
        "function lmath.hypot(x,y)\n"
        "    return lmath_hypot(x,y)\n"
        "end\n"
        "\n"
        "function lmath.lgamma(x)\n"
        "    return lmath_lgamma(x)\n"
        "end\n"
        "\n"
        "function lmath.tgamma(x)\n"
        "    return lmath_tgamma(x)\n"
        "end\n"
        "\n"
        "function lmath.trunc(x)\n"
        "    return lmath_trunc(x)\n"
        "end\n"
        "\n"
        "function lmath.round(x)\n"
        "    return lmath_round(x)\n"
        "end\n"
        "\n"
        "function lmath.rint(x)\n"
        "    return lmath_rint(x)\n"
        "end\n"
        "\n"
        "function lmath.isinf(x)\n"
        "    return lmath_isinf(x)\n"
        "end\n"
        "\n"
        "function lmath.isnan(x)\n"
        "    return lmath_isnan(x)\n"
        "end\n"
        "\n"
        "function lmath.isnormal(x)\n"
        "    return lmath_isnormal(x)\n"
        "end\n"
        "\n"
        "function lmath.asinh(x)\n"
        "    return lmath_asinh(x)\n"
        "end\n"
        "\n"
        "function lmath.acosh(x)\n"
        "    return lmath_acosh(x)\n"
        "end\n"
        "\n"
        "function lmath.atanh(x)\n"
        "    return lmath_atanh(x)\n"
        "end\n"
        "\n"
        "function lmath.erf(x)\n"
        "    return lmath_erf(x)\n"
        "end\n"
        "\n"
        "function lmath.erfc(x)\n"
        "    return lmath_erfc(x)\n"
        "end\n"
        "\n"
        "function lmath.gauss(x, b, c)\n"
        "    return lmath_gauss(x, b, c)\n"
        "end\n"
        "\n"
        "function lmath.lorentz(x, b, c)\n"
        "    return lmath_lorentz(x, b, c)\n"
        "end\n"
        "\n"
        "lmath = readOnly(lmath)\n"
        "\n";

    int iRet = luaL_dostring(pScriptEngine, MODULE_MATH);
    if (iRet != 0) {
        free(pszSigma); pszSigma = NULL;
        cf_error(pScriptEngine);
        lua_gc(pScriptEngine, LUA_GCCOLLECT, 0);        // Force a complete garbage collection in case of errors
        return FALSE;
    }

    const char* MODULE_MATH_GLOBALS =
        "\n"
        "if abs == nil then \n"
        "    abs = function(x) \n"
        "        return math.abs(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if acos == nil then \n"
        "    acos = function(x) \n"
        "        return math.acos(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if asin == nil then \n"
        "    asin = function(x) \n"
        "        return math.asin(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if atan == nil then \n"
        "    atan = function(x) \n"
        "        return math.atan(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if atan2 == nil then \n"
        "    atan2 = function(y,x) \n"
        "        return math.atan2(y,x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if ceil == nil then \n"
        "    ceil = function(x) \n"
        "        return math.ceil(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if cos == nil then \n"
        "    cos = function(x) \n"
        "        return math.cos(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if cosh == nil then \n"
        "    cosh = function(x) \n"
        "        return math.cosh(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if deg == nil then \n"
        "    deg = function(x) \n"
        "        return math.deg(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if exp == nil then \n"
        "    exp = function(x) \n"
        "        return math.exp(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if floor == nil then \n"
        "    floor = function(x) \n"
        "        return math.floor(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if fmod == nil then \n"
        "    fmod = function(x) \n"
        "        return math.fmod(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if frexp == nil then \n"
        "    frexp = function(x) \n"
        "        return math.frexp(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if huge == nil then \n"
        "    huge = math.huge \n"
        "end \n"
        "\n"
        "if ldexp == nil then \n"
        "    ldexp = function(x,y) \n"
        "        return math.ldexp(x,y) \n"
        "    end \n"
        "end \n"
        "\n"
        "if log == nil then \n"
        "    log = function(x) \n"
        "        return math.log(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if ln == nil then \n"
        "    ln = function(x) \n"
        "        return math.log(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if log10 == nil then \n"
        "    log10 = function(x) \n"
        "        return math.log10(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if max == nil then \n"
        "    max = function(...) \n"
        "        return math.max(...) \n"
        "    end \n"
        "end \n"
        "\n"
        "if min == nil then \n"
        "    min = function(...) \n"
        "        return math.min(...) \n"
        "    end \n"
        "end \n"
        "\n"
        "if modf == nil then \n"
        "    modf = function(x) \n"
        "        return math.modf(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if pi == nil then \n"
        "    pi = math.pi \n"
        "end \n"
        "\n"
        "if pow == nil then \n"
        "    pow = function(x,y) \n"
        "        return math.pow(x,y) \n"
        "    end \n"
        "end \n"
        "\n"
        "if rad == nil then \n"
        "    rad = function(x) \n"
        "        return math.rad(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if random == nil then \n"
        "    random = function(...) \n"
        "        return math.random(...) \n"
        "    end \n"
        "end \n"
        "\n"
        "if randomseed == nil then \n"
        "    randomseed = function(x) \n"
        "        return math.randomseed(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if sin == nil then \n"
        "    sin = function(x) \n"
        "        return math.sin(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if sinh == nil then \n"
        "    sinh = function(x) \n"
        "        return math.sinh(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if sqrt == nil then \n"
        "    sqrt = function(x) \n"
        "        return math.sqrt(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if tan == nil then \n"
        "    tan = function(x) \n"
        "        return math.tan(x) \n"
        "    end \n"
        "end \n"
        "\n"
        "if tanh == nil then \n"
        "    tanh = function(x) \n"
        "        return math.tanh(x) \n"
        "    end \n"
        "end \n"
        "\n";

    iRet = luaL_dostring(pScriptEngine, MODULE_MATH_GLOBALS);
    if (iRet != 0) {
        free(pszSigma); pszSigma = NULL;
        cf_error(pScriptEngine);
        lua_gc(pScriptEngine, LUA_GCCOLLECT, 0);        // Force a complete garbage collection in case of errors
        return FALSE;
    }

    const char* MODULE_STATS =
        "\n"
        "data = data or {}\n"
        "\n"
        "function data.min(tabT)\n"
        "    return data_min(tabT)\n"
        "end\n"
        "\n"
        "function data.max(tabT)\n"
        "    return data_max(tabT)\n"
        "end\n"
        "\n"
        "function data.sum(tabT)\n"
        "    return data_sum(tabT)\n"
        "end\n"
        "\n"
        "function data.mean(tabT)\n"
        "    return data_mean(tabT)\n"
        "end\n"
        "\n"
        "function data.median(tabT)\n"
        "    return data_median(tabT)\n"
        "end\n"
        "\n"
        "function data.var(tabT)\n"
        "    return data_var(tabT)\n"
        "end\n"
        "\n"
        "function data.dev(tabT)\n"
        "    return data_dev(tabT)\n"
        "end\n"
        "\n"
        "function data.rms(tabT)\n"
        "    return data_rms(tabT)\n"
        "end\n"
        "\n"
        "function data.coeff(tabT)\n"
        "    return data_coeff(tabT)\n"
        "end\n"
        "\n"
        "function data.skew(tabT)\n"
        "    return data_skew(tabT)\n"
        "end\n"
        "\n"
        "function data.kurt(tabT)\n"
        "    return data_kurt(tabT)\n"
        "end\n"
        "\n"
        "function data.fit(model, x, y, fpar, ipar, tol, iters)\n"
        "    ipar = ipar or {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}"
        "    tol = tol or 1e-6"
        "    iters = iters or 100"
        "    return data_fit(model, x, y, fpar, ipar, tol, iters)\n"
        "end\n"
        "\n"
        "function data.sort(t, asc)\n"
        "    asc = asc or 1"
        "    return data_sort(t, asc)\n"
        "end\n"
        "\n"
        "data = readOnly(data)\n"
        "\n"
        "stats = data\n"
        "stats = readOnly(stats)\n"
        "\n"
        "stat = data\n"
        "stat = readOnly(stat)\n"
        "\n";

    iRet = luaL_dostring(pScriptEngine, MODULE_STATS);
    if (iRet != 0) {
        free(pszSigma); pszSigma = NULL;
        cf_error(pScriptEngine);
        lua_gc(pScriptEngine, LUA_GCCOLLECT, 0);        // Force a complete garbage collection in case of errors
        return FALSE;
    }

    memset(pszSigma, 0, iSize * sizeof(char));
    strcpy(pszSigma, "\n-- SigmaGraph Scripting Interface -------------------\n");
    strcat(pszSigma, "\n-- Copyright(C) 1997-2020  Pr. Sidi HAMADY -------------\n");

    strcat(pszSigma, "\n");
    strcat(pszSigma, "  io = io or {}\n");
    strcat(pszSigma, "  function io.write(...)\n");
    strcat(pszSigma, "    io_write(...)\n");
    strcat(pszSigma, "  end\n");
    strcat(pszSigma, "  \n");
    strcat(pszSigma, "  function io.read(...)\n");
    strcat(pszSigma, "    return io_read(...)\n");
    strcat(pszSigma, "  end\n");
    strcat(pszSigma, "  \n");
    strcat(pszSigma, "  function io.input(...)\n");
    strcat(pszSigma, "      io_input(...)\n");
    strcat(pszSigma, "  end\n");
    strcat(pszSigma, "\n");
    strcat(pszSigma, "io = readOnly(io)\n");
    strcat(pszSigma, "  \n");

    strcat(pszSigma, "\n-- Physics ------------------------------------------\n");
    strcat(pszSigma, "Physics =\n");
    strcat(pszSigma, "{\n");
    strcat(pszSigma, "    k = 1.380650e-23,\n");
    strcat(pszSigma, "    q = 1.602176e-19,\n");
    strcat(pszSigma, "    h = 6.626068e-34,\n");
    strcat(pszSigma, "    c = 299792458.0,\n");
    strcat(pszSigma, "    e = 8.854187e-12,\n");
    strcat(pszSigma, "    n = 6.022141e23,\n");
    strcat(pszSigma, "    m = 9.109382e-31,\n");
    strcat(pszSigma, "    kT = 0.02585202874\n");
    strcat(pszSigma, "}\n");
    strcat(pszSigma, "function Physics.current(V, T, ISC, RP, IS1, n1, IS2, n2, RS, I0)\n");
    strcat(pszSigma, "    return physcurrent(V, T, ISC, RP, IS1, n1, IS2, n2, RS, I0)\n");
    strcat(pszSigma, "end\n");
    strcat(pszSigma, "function Physics.capacitance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2)\n");
    strcat(pszSigma, "    return physcapacitance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2)\n");
    strcat(pszSigma, "end\n");
    strcat(pszSigma, "function Physics.conductance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2)\n");
    strcat(pszSigma, "    return physconductance(Freq, r, L, GLF, CHF, C1, Tau1, C2, Tau2)\n");
    strcat(pszSigma, "\n");
    strcat(pszSigma, "end\n");
    strcat(pszSigma, "Physics = readOnly(Physics)\n");

    iRet = luaL_dostring(pScriptEngine, (const char*) pszSigma);
    if (iRet != 0) {
        cf_error(pScriptEngine);
        free(pszSigma); pszSigma = NULL;
        return FALSE;
    }

    pszSigma[0] = _T('\0');
    strcpy(pszSigma, "\n-- Doc Class ----------------------------------------\n");

    strcat(pszSigma, "Doc = {}\n");
    strcat(pszSigma, "Doc.__index = Doc\n");

    strcat(pszSigma, "function Doc:create()\n");
    strcat(pszSigma, "    local self = {}                    -- our new object\n");
    strcat(pszSigma, "    setmetatable(self, Doc)                    -- make Doc handle lookup\n");
    strcat(pszSigma, "    return self\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:new(name)\n");
    strcat(pszSigma, "    self.id = docnew(name)\n");
    strcat(pszSigma, "    self.name = docname(self.id)\n");
    strcat(pszSigma, "    self.count = doccount(self.id)\n");
    strcat(pszSigma, "    return self.id\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:find(name)\n");
    strcat(pszSigma, "    self.id = docfind(name)\n");
    strcat(pszSigma, "    self.name = docname(self.id)\n");
    strcat(pszSigma, "    self.count = doccount(self.id)\n");
    strcat(pszSigma, "    return self.id\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:__tostring()\n");
    strcat(pszSigma, "    return self.name\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:print()\n");
    strcat(pszSigma, "    print(self.name)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:save()\n");
    strcat(pszSigma, "    return docsave(self.id)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:open()\n");
    strcat(pszSigma, "    self.id = docopen()\n");
    strcat(pszSigma, "    self.name = docname(self.id)\n");
    strcat(pszSigma, "    self.count = doccount(self.id)\n");
    strcat(pszSigma, "    return self.id\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:update()\n");
    strcat(pszSigma, "    return docupdate()\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Doc:close()\n");
    strcat(pszSigma, "    return docclose(self.id)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "\n");
    strcat(pszSigma, "Doc = readOnly(Doc)\n");

    iRet = luaL_dostring(pScriptEngine, (const char*) pszSigma);
    if (iRet != 0) {
        cf_error(pScriptEngine);
        free(pszSigma); pszSigma = NULL;
        return FALSE;
    }

    pszSigma[0] = _T('\0');
    strcpy(pszSigma, "\n-- Inheritance --------------------------------------\n");

    strcat(pszSigma, "function inheritsFrom(parentClass)\n");
    strcat(pszSigma, "    local childClass = {}\n");
    strcat(pszSigma, "    local mtClass = {__index = childClass}\n");
    strcat(pszSigma, "    if nil ~= parentClass then\n");
    strcat(pszSigma, "        setmetatable(childClass, {__index = parentClass})\n");
    strcat(pszSigma, "    end\n");

    strcat(pszSigma, "    function childClass:create()\n");
    strcat(pszSigma, "        local self = {}\n");
    strcat(pszSigma, "        setmetatable(self, mtClass)\n");
    strcat(pszSigma, "        return self\n");
    strcat(pszSigma, "    end\n");

    strcat(pszSigma, "    function childClass:this()\n");
    strcat(pszSigma, "        return childClass\n");
    strcat(pszSigma, "    end\n");

    strcat(pszSigma, "    -- Return the parent class object of the instance\n");
    strcat(pszSigma, "    function childClass:parent()\n");
    strcat(pszSigma, "        return parentClass\n");
    strcat(pszSigma, "    end\n");

    strcat(pszSigma, "    -- Return true if the caller is an instance of theClass\n");
    strcat(pszSigma, "    function childClass:isa(theClass)\n");
    strcat(pszSigma, "        local isx = false\n");
    strcat(pszSigma, "        local currentClass = childClass\n");
    strcat(pszSigma, "        while (nil ~= currentClass) and (false == isx) do\n");
    strcat(pszSigma, "            if currentClass == theClass then\n");
    strcat(pszSigma, "                isx = true\n");
    strcat(pszSigma, "            else\n");
    strcat(pszSigma, "                currentClass = currentClass:parent()\n");
    strcat(pszSigma, "            end\n");
    strcat(pszSigma, "        end\n");
    strcat(pszSigma, "        return isx\n");
    strcat(pszSigma, "    end\n");

    strcat(pszSigma, "    return childClass\n");
    strcat(pszSigma, "end\n");

    iRet = luaL_dostring(pScriptEngine, (const char*) pszSigma);
    if (iRet != 0) {
        cf_error(pScriptEngine);
        free(pszSigma); pszSigma = NULL;
        return FALSE;
    }

    pszSigma[0] = _T('\0');
    strcpy(pszSigma, "\n-- Data Class ---------------------------------------\n");

    strcat(pszSigma, "Data = inheritsFrom(Doc)\n");

    strcat(pszSigma, "function Data:dim()\n");
    strcat(pszSigma, "    return datadim()\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:appendcol(dim, ctype)\n");
    strcat(pszSigma, "    typecol = 1\n");
    strcat(pszSigma, "    after = -1\n");
    strcat(pszSigma, "    if nil == ctype then\n");
    strcat(pszSigma, "        ctype = 2\n");
    strcat(pszSigma, "    end\n");
    strcat(pszSigma, "    return datainsert(typecol, after, dim, ctype)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:format(name, format)\n");
    strcat(pszSigma, "    return dataformat(name, format)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:insertcol(after, dim, ctype)\n");
    strcat(pszSigma, "    typecol = 1\n");
    strcat(pszSigma, "    if nil == ctype then\n");
    strcat(pszSigma, "        ctype = 2\n");
    strcat(pszSigma, "    end\n");
    strcat(pszSigma, "    return datainsert(typecol, after, dim, ctype)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:appendrow(count)\n");
    strcat(pszSigma, "    typerow = 0\n");
    strcat(pszSigma, "    iafter = -1\n");
    strcat(pszSigma, "    if nil == count then\n");
    strcat(pszSigma, "        count = 1\n");
    strcat(pszSigma, "    end\n");
    strcat(pszSigma, "    return datainsert(typerow, iafter, count, 0)\n");
    strcat(pszSigma, "end\n");
    strcat(pszSigma, "function Data:appendrows(count)\n");
    strcat(pszSigma, "    return Data:appendrow(count)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:insertrow(iafter)\n");
    strcat(pszSigma, "    typerow = 0\n");
    strcat(pszSigma, "    return datainsert(typerow, iafter, 0, 0)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:deletecol(strname)\n");
    strcat(pszSigma, "    typecol = 1\n");
    strcat(pszSigma, "    return datadelete(typecol, strname)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:deleterow(ii)\n");
    strcat(pszSigma, "    typerow = 0\n");
    strcat(pszSigma, "    return datadelete(typerow, ii)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:set(...)\n");
    strcat(pszSigma, "    return dataset(...)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:get(...)\n");
    strcat(pszSigma, "    return dataget(...)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:mask(strname, ii)\n");
    strcat(pszSigma, "    return datamask(strname, ii, 1)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:unmask(strname, ii)\n");
    strcat(pszSigma, "    return datamask(strname, ii, 0)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:sort(strname, ascdesc)\n");
    strcat(pszSigma, "    return datasort(strname, ascdesc)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Data:update()\n");
    strcat(pszSigma, "    return dataupdate()\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "\n");
    strcat(pszSigma, "Data = readOnly(Data)\n");

    iRet = luaL_dostring(pScriptEngine, (const char*) pszSigma);
    if (iRet != 0) {
        cf_error(pScriptEngine);
        free(pszSigma); pszSigma = NULL;
        return FALSE;
    }

    pszSigma[0] = _T('\0');
    strcpy(pszSigma, "\n-- Plot Class ---------------------------------------\n");

    strcat(pszSigma, "Plot = inheritsFrom(Doc)\n");

    strcat(pszSigma, "function Plot:title(stitle)\n");
    strcat(pszSigma, "    return plottitle(stitle)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Plot:frame(backr, backg, backb, plotr, plotg, plotb)\n");
    strcat(pszSigma, "    return plotframe(backr, backg, backb, plotr, plotg, plotb)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Plot:add(namex, namey, istyle, iaxis)\n");
    strcat(pszSigma, "    return plotadd(namex, namey, istyle, iaxis)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Plot:remove(strname)\n");
    strcat(pszSigma, "    return plotremove(strname)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Plot:axis(iaxis, iscale, iautoscale, fmin, fmax, ititle, stitle)\n");
    strcat(pszSigma, "    if nil == iautoscale then\n");
    strcat(pszSigma, "        return plotaxis(iaxis, iscale)\n");
    strcat(pszSigma, "    elseif nil == fmin then\n");
    strcat(pszSigma, "        return plotaxis(iaxis, iscale, iautoscale)\n");
    strcat(pszSigma, "    elseif nil == ititle then\n");
    strcat(pszSigma, "        return plotaxis(iaxis, iscale, iautoscale, fmin, fmax)\n");
    strcat(pszSigma, "    elseif nil == stitle then\n");
    strcat(pszSigma, "        return plotaxis(iaxis, iscale, iautoscale, fmin, fmax, ititle)\n");
    strcat(pszSigma, "    end\n");
    strcat(pszSigma, "    return plotaxis(iaxis, iscale, iautoscale, fmin, fmax, ititle, stitle)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Plot:label(iaxis, ilabel)\n");
    strcat(pszSigma, "    return plotlabel(iaxis, ilabel)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Plot:color(icurve, liner, lineg, lineb, dotr, dotg, dotb)\n");
    strcat(pszSigma, "    if nil == dotr then\n");
    strcat(pszSigma, "        return plotcolor(icurve, liner, lineg, lineb)\n");
    strcat(pszSigma, "    end\n");
    strcat(pszSigma, "    return plotcolor(icurve, liner, lineg, lineb, dotr, dotg, dotb)\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "function Plot:update()\n");
    strcat(pszSigma, "    return plotupdate()\n");
    strcat(pszSigma, "end\n");

    strcat(pszSigma, "\n");
    strcat(pszSigma, "Plot = readOnly(Plot)\n");

    iRet = luaL_dostring(pScriptEngine, (const char*) pszSigma);
    if (iRet != 0) {
        cf_error(pScriptEngine);
        free(pszSigma); pszSigma = NULL;
        return FALSE;
    }

    pszSigma[0] = _T('\0');
    strcpy(pszSigma, "\n-- Modules ------------------------------------------\n");
    strcat(pszSigma, "function setpath(appendstr, sep)\n");
    strcat(pszSigma, "    ilen = string.len(appendstr)\n");
    strcat(pszSigma, "    if (ilen < 3) or (ilen > 255) or (string.find(package.path, appendstr)) then\n");
    strcat(pszSigma, "        return package.path\n");
    strcat(pszSigma, "    end\n");
    strcat(pszSigma, "    sep = sep or \";\"\n");
    strcat(pszSigma, "    return package.path .. sep .. appendstr\n");
    strcat(pszSigma, "end\n");

    iRet = luaL_dostring(pScriptEngine, (const char*) pszSigma);
    if (iRet != 0) {
        cf_error(pScriptEngine);
        free(pszSigma); pszSigma = NULL;
        return FALSE;
    }

    free(pszSigma); pszSigma = NULL;

    return TRUE;
}