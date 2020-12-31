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

#include "libmath.h"

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_linear       :    Regression linéaire y = a + bx

        xdat            :    Données (abscisses)
        ydat            :    Données (ordonnées)
        ndat            :    Nombre de points
        a               :    Contient en sortie la valeur calculée de a
        b               :    Contient en sortie la valeur calculée de b

        Retourne la valeur du coeff. de corrélation (idéalement = 1)
    -------------------------------------------------------------------- */
real_t ml_fit_linear(real_t *xdat, real_t *ydat, int_t ndat, real_t *a, real_t *b, libvar_t *pLib)
{
    int_t i;
    real_t sumx = 0.0, sumy = 0.0, sumxx = 0.0, sumyy = 0.0, sumxy = 0.0;
    real_t sxx, syy, sxy, rr;

    if (pLib->errcode) { return 0.0; }

    pLib->errcode = 1;

    *a = 0.0;
    *b = 0.0;

    if (ndat < 2) { return 0.0; }

    // Deux points ? Fitter quand même ? :-) OK :
    if (ndat == 2) {
        if (xdat[0] == xdat[1]) {
            return 0.0;
        }
        *b = (ydat[1] - ydat[0]) / (xdat[1] - xdat[0]);
        *a = ydat[0] - ((*b) * xdat[0]);
        pLib->errcode = 0;
        return 1.0;
    }

    for (i = 0; i < ndat; i++) {
        sumx += xdat[i];
        sumy += ydat[i];
        sumxx += (xdat[i] * xdat[i]);
        sumyy += (ydat[i] * ydat[i]);
        sumxy += (xdat[i] * ydat[i]);
    }
    sxx = sumxx - (sumx * sumx / ((real_t)ndat));
    syy = sumyy - (sumy * sumy / ((real_t)ndat));
    sxy = sumxy - (sumx * sumy / ((real_t)ndat));

    if (fabs(sxx) == 0.0) {
        return 0.0;
    }

    *b = sxy / sxx;
    *a = (sumy / ((real_t)ndat)) - ((*b) * sumx / ((real_t)ndat));

    if (fabs(syy) == 0.0) {
        rr = 1.0;
    }
    else {
        rr = sxy / sqrt(sxx * syy);
    }

    pLib->errcode = 0;

    return rr;
}