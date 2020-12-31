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

static int_t ispowtwo(int_t dim)
{
    if (dim < 2) {
        return 0;
    }

    return !(dim & (dim - 1));
}


int_t ml_fft(real_t *pdat, int_t dim, byte_t idirect)
{
    int_t mt, mtt, m, i, i1, i2, j, k, l, l1, l2;
    real_t c1, c2, tx, ty, t1, t2, u1, u2, z, ftmp, *pimag = NULL;

    if (!ispowtwo(dim)) {
        return -1;
    }
    if ((dim < 2) || (dim > ML_MAXPOINTS) || (pdat == NULL)) {
        return -1;
    }

    pimag = (real_t*)calloc(dim, sizeof(real_t));
    if (pimag == NULL) {
        return -1;
    }

    ftmp = log((real_t)dim) / log(2.0);
    mt = (int_t)ftmp;
    m = mt;
    mtt = 1;
    for (i = 1; i <= (mt + 1); i++) {
        mtt *= 2;
        if (mtt == dim) {
            m = i;
            break;
        }
    }

    i2 = dim >> 1;
    j = 0;
    pimag[dim - 1] = 0.0;
    for (i = 0; i < (dim - 1); i++) {
        pimag[i] = 0.0;
        if (i < j) {
            tx = pdat[i];
            ty = pimag[i];
            pdat[i] = pdat[j];
            pimag[i] = pimag[j];
            pdat[j] = tx;
            pimag[j] = ty;
        }
        k = i2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    c1 = -1.0; 
    c2 = 0.0;
    l2 = 1;
    for (l = 0; l < m; l++) {
        l1 = l2;
        l2 <<= 1;
        u1 = 1.0; 
        u2 = 0.0;
        for (j = 0; j < l1; j++) {
            for (i = j; i < dim; i += l2) {
                i1 = i + l1;
                t1 = u1 * pdat[i1] - u2 * pimag[i1];
                t2 = u1 * pimag[i1] + u2 * pdat[i1];
                pdat[i1] = pdat[i] - t1;
                pimag[i1] = pimag[i] - t2;
                pdat[i] += t1;
                pimag[i] += t2;
            }
            z =  u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }
        c2 = sqrt((1.0 - c1) / 2.0);
        if (idirect == 1) { c2 = -c2; }
        c1 = sqrt((1.0 + c1) / 2.0);
    }

    if (idirect == 1) {
        for (i = 0; i < dim; i++) {
            pdat[i] = sqrt(pdat[i] * pdat[i] + pimag[i] * pimag[i]) / (real_t)dim;
        }
    }

    free(pimag);
    pimag = NULL;
    return 0;
}

int_t ml_autocorr(real_t *pin, real_t *pout, int_t dim)
{
    int_t ii, jj;
    real_t ymean, yvar;

    if ((dim < 2) || (dim > ML_MAXPOINTS) || (pin == NULL) || (pout == NULL)) {
        return -1;
    }

    ymean = pin[0];
    for (ii = 1; ii < dim; ii++) {
        ymean += pin[ii];
    }
    ymean /= (real_t)dim;
    yvar = (pin[0] - ymean) * (pin[0] - ymean);
    for (ii = 1; ii < dim; ii++) {
        yvar += (pin[ii] - ymean) * (pin[ii] - ymean);
    }
    if (yvar == 0.0) {
        return -1;
    }

    for (jj = 0; jj < dim; jj++) {
        pout[jj] = 0.0;
        for (ii = 0; ii < (dim - jj); ii++) {
            pout[jj] += (pin[ii] - ymean) * (pin[ii + jj] - ymean);
        }
        pout[jj] /= yvar;
    }

    return 0;
}
