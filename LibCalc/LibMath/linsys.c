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

void pivot(real_t **a, real_t *b, int_t k ,int_t n);
/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_linsys_gauss  :    résolution de système linéaire : a x = b

        a, b         :    données du système linéaire (modifiées par ml_linsys_gauss)
        x            :    résultat
        n            :    dimension du système

        Retourne 0 si OK
    -------------------------------------------------------------------- */
void ml_linsys_gauss(real_t **a, real_t *b, real_t *x, int_t n, libvar_t *pLib)
{
    int_t i, j, k;
    real_t w, m;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;

    if ((n <= 0) || (n > ML_MAXPOINTS) || (a == NULL) || (b == NULL) || (x == NULL)) {
        return;
    }

    for (k = 0; k < n-1; k++) {
        pivot(a, b, k, n);
        if (a[k][k] == 0.0) {
            return;
        }
        w = 1.0 / a[k][k];
        for (i = k+1; i < n; i++) {
            m = a[i][k] * w;
            for (j = k+1; j < n; j++) {
                a[i][j] = a[i][j] - (m * a[k][j]);
            }
            b[i] = b[i] - (m * b[k]);
        }
    }

    for (i = n-1; i >= 0; i--) {
        w = b[i];
        for (j = i+1; j < n; j++) {
            w = w - a[i][j] * x[j];
        }
        if (a[i][i] == 0.0) {
            return;
        }
        x[i] = w / a[i][i];
    }

    pLib->errcode = 0;

    return;
}

static void pivot(real_t **a, real_t *b, int_t k ,int_t n)
{
    int_t p, i;
    real_t x, w;

    p = k;
    x = fabs(a[p][k]);
    for (i = k+1; i < n; i++) {
        if ((w = fabs(a[i][k])) > x) {
            p = i;
            x = w;
        }
    }

    if (p == k) {
        return;
    }

    for (i = k; i < n; i++) {
        w = a[k][i];
        a[k][i] = a[p][i];
        a[p][i] = w;
    }

    w = b[k];
    b[k] = b[p];
    b[p] = w;
}

