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

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"        // main symbols

#define SIGMA_SOFTWARE_VERSION               0x26A0
#define SIGMA_SOFTWARE_NAME                 _T("SigmaConsole")
#define SIGMA_SOFTWARE_DESCRIPTION          _T("Mathematical Console.")
#define SIGMA_SOFTWARE_COPYRIGHT            _T("Copyright(C) 1997-2020  Pr. Sidi HAMADY")
#define SIGMA_SOFTWARE_DATERELEASE          _T(__DATE__)
#define SIGMA_SOFTWARE_TIMERELEASE          _T(__TIME__)
#define SIGMA_SOFTWARE_RESERVED             _T("")

#define SIGMA_MINWIDTH2                     160
#define SIGMA_MINHEIGHT2                    120

#define SIGMA_OUTPUT_INFORMATION            0
#define SIGMA_OUTPUT_WARNING                1
#define SIGMA_OUTPUT_ERROR                  2

extern software_t g_Sigmaware;            // Sigma software informations

// CSigmaConsoleApp:
// See SigmaConsole.cpp for the implementation of this class
//

class CSigmaConsoleApp : public CWinApp
{
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

    bool m_bPortable;

    bool fileExists(const char_t *pszFilename)
    {
        WIN32_FIND_DATA FindFileData;
        HANDLE handleT = ::FindFirstFile((LPCTSTR) pszFilename, &FindFileData);
        if (handleT != INVALID_HANDLE_VALUE) {
            ::FindClose(handleT);
            return true;
        }
        return false;
    }
    bool makeDir(const char_t *pszDir)
    {
        if (fileExists(pszDir)) {
            return true;
        }
        return (::CreateDirectory(pszDir, NULL) == TRUE);
    }

public:
    CSigmaConsoleApp();

    bool isPortable(void)
    {
        return m_bPortable;
    }

    console_t m_Console;                            // User preferences
    char_t m_szMainDir[ML_STRSIZE];
    char_t m_szHelpFilename[ML_STRSIZE];
    char_t m_szUserDir[ML_STRSIZE];
    BOOL getUserDir(void);
    BOOL getOptions();
    BOOL setOptions();

    libvar_t* m_pLib;                                // Interaction avec la LibMath

    char_t m_szFormatF[ML_STRSIZET];            // Forrmat Float    :    "%.6f"
    char_t m_szFormatE[ML_STRSIZET];            // Forrmat Exp        :    "%.3e"
    char_t m_szFormatI[ML_STRSIZET];            // Forrmat Int        :    "%d"
    byte_t m_bFormat;                                // Format options

    BOOL m_bRunning;

    // Utility functions extensively used by all components
    void Output(CString strT, int_t nIcon = SIGMA_OUTPUT_WARNING);
    void DoEvent(void);
    int_t CStringToChar(CString strSrc, char_t* szDest, int_t nMax);
    BOOL GetFont(HWND hWnd, LOGFONT *plf);
    BOOL GetColor(HWND hWnd, COLORREF *pcr);
    void Reformat(real_t fVal, char_t* szBuffer, int_t nCount, char_t* pszFormat = NULL, int_t *pFmt = NULL);
    BOOL LogfontToFont(LOGFONT lft, font_t *pfnt);
    BOOL FontToLogfont(font_t fnt, LOGFONT *plft);

    // Performance counter
    LARGE_INTEGER m_liPerfCount;
    ulong_t m_uiPerfCount;
    void CounterStart();
    void CounterStop();
    
    // Parser
    BOOL DoParse(const char_t* szInput, char_t* szOutput, real_t* pVal = NULL, const char_t* pszFormat = NULL);
    void ResizeParam(void);
    real_t m_fParam[_T('z') - _T('a') + 1];
    char_t m_szParam[_T('z') - _T('a') + 1][ML_STRSIZET];        // Params name : by default : a, b, c, ..., z
    BOOL m_bParam[_T('z') - _T('a') + 1];                            // Param set ?
    int_t m_nVarnum;
    real_t m_fAns;
    bool m_bAns;

// Overrides
public:
    virtual BOOL InitInstance();

// Implementation
    DECLARE_MESSAGE_MAP()
    virtual int ExitInstance();
};

extern CSigmaConsoleApp SigmaApp;