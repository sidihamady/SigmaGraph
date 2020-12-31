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

#define _CRT_SECURE_NO_DEPRECATE 1

#include "../Target/targetver.h"

#include "libelec.h"

/*  -------------------------------------------------------------------- */
/* Begin Win32 */
#include <windows.h>
/* End Win32 */
/*  -------------------------------------------------------------------- */

/*  -------------------------------------------------------------------- */
/*    VARIABLES D'INTERACTION */
const int_t el_version = 0x26A0;                    /* Version de libelec */
#ifdef _DEBUG
const int_t el_debug = 1;                           /* Variable de déboguage */
#else
const int_t el_debug = 0;                            /* Variable de déboguage */
#endif
/*  -------------------------------------------------------------------- */

/*  --------------------------------------------------------------------
    el_reset            :    Remise à zéro.
    -------------------------------------------------------------------- */
void el_reset(libvar_t *pLib)
{
    ml_reset(pLib);
    pLib->exptail = 100.0;                           /* Seuil de détection des variations exponentielles */
}
