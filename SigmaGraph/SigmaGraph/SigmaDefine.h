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

#define SIGMA_MAXDOC                    16                                // Nombre max de documents
#define SIGMA_MAXTPL                    SIGMA_MAXDOC                // Nombre max de templates
#define SIGMA_MAXCURVES                 ML_MAXCURVES
#define SIGMA_MAXCOLUMNS                ML_MAXCOLUMNS
#define SIGMA_MINWIDTH00                800
#define SIGMA_MINHEIGHT00               600
#define SIGMA_MINWIDTH0                 600
#define SIGMA_MINHEIGHT0                450
#define SIGMA_MINWIDTH1                 320
#define SIGMA_MINHEIGHT1                240
#define SIGMA_MINWIDTH2                 320
#define SIGMA_MINHEIGHT2                240
#define SIGMA_MINWIDTH3                 200
#define SIGMA_MINHEIGHT3                150
#define SIGMA_MINHEIGHT4                100
#define SIGMA_DEFWIDTH                  400
#define SIGMA_DEFHEIGHT                 300
#define SIGMA_DEFWIDTH_MAIN             800
#define SIGMA_DEFHEIGHT_MAIN            600
#define SIGMA_DEFWIDTH_DATA             300
#define SIGMA_DEFHEIGHT_DATA            400
#define SIGMA_DEFWIDTH_PLOT             520
#define SIGMA_DEFHEIGHT_PLOT            480
#define SIGMA_DEFWIDTH_SCRIPT           400
#define SIGMA_DEFHEIGHT_SCRIPT          600
#define SIGMA_DEFWIDTH_CONSOLE          360
#define SIGMA_DEFHEIGHT_CONSOLE         160
#define SIGMA_DELTAITER                 128

#define SIGMA_SCRIPT_MAXFAV             32

#define IDT_TIMER                       2199

#define SIGMA_SOFTWARE_DOCVERSION       0x2400
#define SIGMA_SOFTWARE_VERSION          0x26A0
// !!! SIGMA_SOFTWARE_NAME and SIGMA_SOFTWARE_DESCRIPTION should never change (to ensure file format compatibility) !!!
#define SIGMA_SOFTWARE_NAME             _T("SigmaGraph")
#define SIGMA_SOFTWARE_DESCRIPTION      _T("Data plotting and analysis software.")
#define SIGMA_SOFTWARE_COPYRIGHT        _T("Copyright(c) Sidi OULD SAAD HAMADY")
//
#define SIGMA_SOFTWARE_DATERELEASE      _T(__DATE__)
#define SIGMA_SOFTWARE_TIMERELEASE      _T(__TIME__)
#define SIGMA_SOFTWARE_RESERVED         _T("")

#define SIGMA_SSIZE                     16

#define SIGMA_OUTPUT_INFORMATION        0
#define SIGMA_OUTPUT_WARNING            1
#define SIGMA_OUTPUT_ERROR              2

#ifndef char_t
typedef TCHAR char_t;
#endif
#ifndef byte_t
typedef unsigned char byte_t;
#endif

#define SHIFT_Pressed()         ((GetKeyState(VK_SHIFT) & (1 << ((sizeof(SHORT) * 8) - 1)))   != 0)
#define CTRL_Pressed()          ((GetKeyState(VK_CONTROL) & (1 << ((sizeof(SHORT) * 8) - 1))) != 0)
#define ALT_Pressed()           ((GetKeyState(VK_MENU) & (1 << ((sizeof(SHORT) * 8) - 1)))    != 0)
