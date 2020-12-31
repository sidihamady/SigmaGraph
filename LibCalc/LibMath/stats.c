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

#define SWAP(a,b)        ftmp = (a);(a) = (b);(b) = ftmp;
#define SORT_MIN        7
#define SORT_STACK    50

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_sort         :    ordonner une série numérique
    
        pdat        :    pointeur sur les données à modifier
        ndat        :    nombre de points
        iasc        :    ordre ascendant si 1 et descendant si 0

        Retourne le pointeur sur les données ordonnées si OK ou NULL sinon
    -------------------------------------------------------------------- */
real_t* ml_sort(real_t *pdat, int_t ndat, byte_t iasc, libvar_t *pLib)
{
    int_t ii, ir = ndat - 1, jj, nn, ll = 0, *istack, isec, jsec;
    int_t jstack = 0;
    real_t aa, ftmp;

    if (pLib->errcode) { return NULL; }

    pLib->errcode = 1;

    if ((pdat == NULL) || (ndat < 1) || (ndat > ML_MAXPOINTS)) {
        return NULL;
    }

    if (ndat == 1) {
        pLib->errcode = 0;
        return pdat;
    }
    else if (ndat == 2) {
        if (pdat[0] > pdat[1]) {
            ftmp = pdat[0];
            pdat[0] = pdat[1];
            pdat[1] = ftmp;
        }
        pLib->errcode = 0;
        return pdat;
    }

    istack = (int_t*)malloc(SORT_STACK * sizeof(int_t));
    if (istack == NULL) {
        return NULL;
    }

    for (isec = 0; isec < ML_MAXPOINTS; isec++) {
        if (ir - ll < SORT_MIN) {
            for (jj = ll + 1; jj <= ir; jj++) {
                aa = pdat[jj];
                for (ii = jj - 1; ii >= ll; ii--) {
                    if (pdat[ii] <= aa) { break; }
                    pdat[ii+1] = pdat[ii];
                }
                pdat[ii+1] = aa;
            }
            if (jstack == 0) { break; }
            ir = istack[jstack--];
            ll = istack[jstack--];
        }
        else {
            nn = (ll + ir + 1) >> 1;
            if (nn > ndat - 1) { free(istack); istack = NULL; return NULL; } // Never happen
            SWAP(pdat[nn], pdat[ll+1])
            if (pdat[ll] > pdat[ir]) {
                SWAP(pdat[ll], pdat[ir])
            }
            if (pdat[ll+1] > pdat[ir]) {
                SWAP(pdat[ll + 1], pdat[ir])
            }
            if (pdat[ll] > pdat[ll+1]) {
                SWAP(pdat[ll], pdat[ll + 1])
            }
            ii = ll + 1;
            jj = ir;
            aa = pdat[ll + 1];
            for (jsec = 0; jsec < ML_MAXPOINTS; jsec++) {
                do { ii++; } while (pdat[ii] < aa);
                do { jj--; } while (pdat[jj] > aa);
                if (jj < ii) { break; }
                SWAP(pdat[ii], pdat[jj]);
            }
            pdat[ll + 1] = pdat[jj];
            pdat[jj] = aa;
            jstack += 2;
            if (jstack > SORT_STACK) { free(istack); istack = NULL; return NULL; }
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

    pLib->errcode = 0;
    if (iasc == 0) {
        ml_reverse(pdat, ndat, pLib);
    }

    if (pLib->errcode == 0) {
        _tcscpy(pLib->message, _T("Done."));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_sort]"), ML_STRSIZE - 1); }
    }

    free(istack); istack = NULL;
    return pdat;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_reverse          :    inverser des données
    
        pdat            :    pointeur sur les données
        ndat            :    nombre de points
-------------------------------------------------------------------- */
void ml_reverse(real_t *pdat, int_t ndat, libvar_t *pLib)
{
    int_t ii, jj, isec;
    real_t ftmp;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pdat == NULL) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_reverse]"), ML_STRSIZE - 1); }
        return;
    }

    if ((ndat < 1) || (ndat > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_reverse]"), ML_STRSIZE - 1); }
        return;
    }

    if (ndat == 1) {
        pLib->errcode = 0;
        return;
    }
    else if (ndat == 2) {
        ftmp = pdat[0];
        pdat[0] = pdat[1];
        pdat[1] = ftmp;
        pLib->errcode = 0;
        _tcscpy(pLib->message, _T("Done."));
        if (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_reverse]"), ML_STRSIZE - 1); }
        return;
    }

    ii = 0; jj = ndat - 1;
    for (isec = 0; isec < ML_MAXPOINTS; isec++) {
        ftmp = pdat[ii];
        pdat[ii] = pdat[jj];
        pdat[jj] = ftmp;
        ii += 1;
        jj -= 1;
        if (ii >= jj) {
            break;
        }
    }

    pLib->errcode = 0;
    _tcscpy(pLib->message, _T("Done."));
    if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_reverse]"), ML_STRSIZE - 1); }
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_stats        :    calcul des statistiques
    
        pdat        :    pointeur sur les données
        ndat        :    nombre de points
        pstats      :    pointeur sur la structure qui contiendra
                        les statistiques

        Retourne le pointeur pstats si OK ou NULL sinon
    -------------------------------------------------------------------- */
stat_t* ml_stats(const real_t *pdat, int_t ndat, stat_t* pstats, libvar_t *pLib)
{
    int_t ii;
    real_t ftmp, fa, fb, fc;

    real_t *ptmp = NULL;

    if (pLib->errcode) { return NULL; }

    pLib->errcode = 1;

    if ((pdat == NULL) || (pstats == NULL) || (ndat < 1) || (ndat > ML_MAXPOINTS)) {
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

        pLib->errcode = 0;
        return pstats;
    }
    else if (ndat == 2) {
        pstats->points = 2;
        pstats->min = (pdat[0] < pdat[1]) ? pdat[0] : pdat[1];
        pstats->max = (pdat[0] > pdat[1]) ? pdat[0] : pdat[1];
        pstats->sum = pdat[0] + pdat[1];
        pstats->mean = 0.5 * (pdat[0] + pdat[1]);
        pstats->median = 0.5 * (pdat[0] + pdat[1]);
        pstats->rms = (real_t)sqrt(((pdat[0] * pdat[0]) + (pdat[1] * pdat[1])) * 0.5);
        pstats->var = ((pdat[0] - pstats->mean) * (pdat[0] - pstats->mean)) + ((pdat[1] - pstats->mean) * (pdat[1] - pstats->mean));
        pstats->dev = (real_t)sqrt(pstats->var);
        if (pstats->mean != 0.0) {
            pstats->coeff = pstats->dev / pstats->mean;
        }
        else {
            pstats->coeff = 0.0;
        }

        pstats->done = 1;

        pLib->errcode = 0;
        return pstats;
    }

    ptmp = (real_t*)malloc(ndat * sizeof(real_t));
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
    pstats->mean = pstats->sum / (real_t)ndat;
    pstats->rms = (real_t)sqrt(ftmp / (real_t)ndat);

    // Ordonner les données si nécessaire
    if (pstats->sorted == 0) {
        pLib->errcode = 0;
        ml_sort(ptmp, ndat, 1, pLib);
        if (pLib->errcode) {
            free(ptmp); ptmp = NULL;
            return NULL;
        }
    }

    ftmp = 0.5 * (real_t)ndat;
    ii = ndat / 2;
    if (ftmp == (real_t)ii) {
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
    pstats->var = fa / (real_t)(ndat - 1);
    pstats->dev = (real_t)sqrt(pstats->var);
    if (pstats->mean != 0.0) {
        pstats->coeff = pstats->dev / pstats->mean;
    }
    else {
        pstats->coeff = 0.0;
    }

    if (fa != 0.0) {
        pstats->skew = ((real_t)sqrt((real_t)(ndat))) * fb / ((real_t)pow(fa, 1.5));
        pstats->kurt = (((real_t)(ndat)) * fc / (fa * fa)) - 3.0;
    }
    else {
        pstats->skew = 0.0;
        pstats->kurt = 0.0;
    }

    pLib->errcode = 0;
    free(ptmp); ptmp = NULL;

    return pstats;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_stat_dup    :    Dupliquer des statistiques

        pTo        :    pointeur sur la structure source
        pFrom      :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_stat_dup(stat_t *pTo, const stat_t *pFrom)
{
    if ((pTo == NULL) || (pFrom == NULL)) {
        return -1;
    }

    pTo->id = pFrom->id;
    pTo->points = pFrom->points;
    pTo->sum = pFrom->sum;
    pTo->min = pFrom->min;
    pTo->max = pFrom->max;
    pTo->mean = pFrom->mean;
    pTo->median = pFrom->median;
    pTo->var = pFrom->var;
    pTo->dev = pFrom->dev;
    pTo->rms = pFrom->rms;
    pTo->coeff = pFrom->coeff;
    pTo->kurt = pFrom->kurt;
    pTo->skew = pFrom->skew;
    pTo->sorted = pFrom->sorted;
    pTo->done = pFrom->done;

    return 0;
}

