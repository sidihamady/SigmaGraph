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

    ml_toreal         :    Convesion d'une chaîne de caractère en réel

        szInput       :    Chaîne à convertir
        pError        :    Pointeur vers l'erreur (0 si OK, 1 sinon)
        pszEnd        :    Pointeur sur la fin de la chaîne convertie

        Retourne la valeur convertie
    -------------------------------------------------------------------- */
real_t ml_toreal(const char_t *szInput, byte_t *pError, char_t **pszEnd)
{
    real_t fval = 0.0;

    errno = 0;
    if (pError) {
        *pError = 0;
    }

    if (szInput[0] == _T('\0')) {
        if (pszEnd) {
            *pszEnd = NULL;
        }
        return 0.0;
    }
    
    if (szInput[1] == _T('\0')) {
        if (pszEnd) {
            *pszEnd = (char_t *) (&szInput[1]);
        }
        return (real_t)(szInput[0] - _T('0'));
    }
    else {
        fval = (real_t)_tcstod(szInput, pszEnd);
    }

    if (pError) {
        if ((errno == ERANGE) || (fval == -HUGE_VAL) || (fval == HUGE_VAL)) {
            *pError = 1;
        }
    }

    return fval;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_roundinf         :    Arrondir un réel à la valeur inférieure
        fval            :    Pointeur vers la valeur à arrondir

        Retourne la valeur arrondie
    -------------------------------------------------------------------- */
real_t ml_roundinf(real_t *fval)
{
    real_t fT;
    int_t ii;
    const real_t fdelta = 10.0;
    const int_t imax = 32;

    if (*fval == 0.0) {
        return 0.0;
    }

    fT = *fval - floor(*fval);
    if (fT < (0.000001 * (*fval))) {
        *fval = floor(*fval);
        return *fval;
    }

    fT = (*fval < 0.0) ? -1.0 : 1.0;
    for (ii = 0; ii > -imax; ii -= 1) {
        if (*fval > 0.0) {
            if ((*fval > fT) && (*fval < (fT * fdelta))) {
                *fval = (real_t)(floor(*fval / fT)) * fT;
                return *fval;
            }
        }
        else {
            if ((*fval > fT) && (*fval < (fT / fdelta))) {
                fT /= fdelta;
                *fval = (real_t)(ceil(*fval / fT)) * fT;
                return *fval;
            }
        }
        fT /= fdelta;
    }

    fT = (*fval < 0.0) ? -1.0 : 1.0;
    for (ii = 0; ii < imax; ii += 1) {
        if (*fval > 0.0) {
            if ((*fval > fT) && (*fval < (fT * fdelta))) {
                *fval = (real_t)(floor(*fval));
                return *fval;
            }
        }
        else {
            if ((*fval > fT) && (*fval < (fT / fdelta))) {
                fT /= fdelta;
                *fval = (real_t)(ceil(*fval));
                return *fval;
            }
        }
        fT *= fdelta;
    }

    return *fval;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_roundsup         :    Arrondir un réel à la valeur supérieure
        fval            :    Pointeur vers la valeur à arrondir

        Retourne la valeur arrondie
    -------------------------------------------------------------------- */
real_t ml_roundsup(real_t *fval)
{
    real_t fT;
    int_t ii;
    const real_t fdelta = 10.0;
    const int_t imax = 32;

    if (*fval == 0.0) {
        return 0.0;
    }

    fT = *fval - floor(*fval);
    if (fT < (0.000001 * (*fval))) {
        *fval = floor(*fval);
        return *fval;
    }

    fT = (*fval < 0.0) ? -1.0 : 1.0;
    for (ii = 0; ii > -imax; ii -= 1) {
        if (*fval > 0.0) {
            if ((*fval > fT) && (*fval < (fT * fdelta))) {
                *fval = (real_t)(ceil(*fval / fT)) * fT;
                return *fval;
            }
        }
        else {
            if ((*fval > fT) && (*fval < (fT / fdelta))) {
                fT /= fdelta;
                *fval = (real_t)(floor(*fval / fT)) * fT;
                return *fval;
            }
        }
        fT /= fdelta;
    }

    fT = (*fval < 0.0) ? -1.0 : 1.0;
    for (ii = 0; ii < imax; ii += 1) {
        if (*fval > 0.0) {
            if ((*fval > fT) && (*fval < (fT * fdelta))) {
                *fval = (real_t)(ceil(*fval));
                return *fval;
            }
        }
        else {
            if ((*fval > fT) && (*fval < (fT / fdelta))) {
                fT /= fdelta;
                *fval = (real_t)(floor(*fval));
                return *fval;
            }
        }
        fT *= fdelta;
    }

    return *fval;
}

/*  -------------------------------------------------------------
    :TODO: complete doc and translate to english

    lm_round            :    Arrondir un réel à la valeur inférieure
        fVal            :    Pointeur vers la valeur à arrondir

    Retourne la valeur arrondie
------------------------------------------------------------- */
real_t ml_round(double *fVal, int bUp)
{
    double ft, fa, fb;

    if (*fVal == 0.0) {
        return *fVal;
    }

    // Calculate nPlaces: if log10(x) < 0 --> nPlaces = (int) ceil(fabs(log10(x)))
    int nPlaces = 0;
    double fPlaces = fabs(*fVal);
    if (fPlaces > DBL_EPSILON) {
        fPlaces = log10(fPlaces);
        if (fPlaces < 0.0) {
            nPlaces = (int) ceil(fabs(fPlaces));
            if (nPlaces > 12) {
                nPlaces = 12;
            }
        }
    }

    ft = *fVal;
    if (ft < 0.0) {
        ft = fabs(ft);
    }
    fa = (real_t)(pow(10.0, (real_t)(nPlaces)));
    fb = ft * fa;
    fb = (real_t)(bUp ? ceil(fb) : floor(fb));
    ft = (fb / fa);

    *fVal = (*fVal >= 0.0) ? ft : -ft;

    return *fVal;
}
