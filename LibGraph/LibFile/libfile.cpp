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

#include "libfile.h"

/*    VARIABLES D'INTERACTION */
const int_t fl_version = 0x26A0;                    /* Version de libfile */
#ifdef _DEBUG
const int_t fl_debug = 1;                            /* Variable de déboguage */
#else
const int_t fl_debug = 0;                            /* Variable de déboguage */
#endif

/*  --------------------------------------------------------------------
    fl_reset            :    Remise à zéro.
    -------------------------------------------------------------------- */
void fl_reset(libvar_t *pLib)
{
    ml_reset(pLib);
}

/*  --------------------------------------------------------------------
    fl_interact        :    permet de laisser la main à la fonction appelante
                            lors de l'éxécution d'une fonction (libmath, ...)
                            particulièrement gourmande en temps processeur
                            et permet d'afficher la progession du calcul.
    -------------------------------------------------------------------- */
void fl_interact(void)
{
    MSG msgFree;
    // ... et traiter les messages Windows en attente
    while(PeekMessage(&msgFree, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msgFree);
        DispatchMessage(&msgFree);
    }
}

/*  --------------------------------------------------------------------
    fl_bigendian    :    Format des données : Big Endian ?
        retourne 1 si BigEndian
                 0 sinon
    -------------------------------------------------------------------- */
int_t fl_bigendian()
{
    short word = 0x4321;
    if ((*(char_t*)&word) != 0x21) {
        return 1;
    }
    else {
        return 0;
    }
}

void fl_bytesswap(byte_t *b, int_t n)
{
    register int_t i = 0;
    register int_t j = n - 1;
    byte_t tmp;

    if ((n <= 1) || (n > 64)) { return; }

    while (i < j) {
        tmp = b[i];
        b[i] = b[j];
        b[j] = tmp;
        i++; j--;
    }
}

