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

// :TODO: translate to english

static real_t    chi2old;
static real_t    chi2new;
static real_t    lambda;                                         /*    facteur de descente */
static real_t    tolerance;                                      /* précision */
static real_t    vector[ML_FIT_MAXPAR];                          /* vecteur de correction */
static real_t    matrix[ML_FIT_MAXPAR][ML_FIT_MAXPAR];           /* la matrice du système ... */
static real_t    invmatrix[ML_FIT_MAXPAR][ML_FIT_MAXPAR];        /* ... et son inverse */
static int_t     itc;                                            /* nombre d'itérations */
static int_t     found;                                          /* solution trouvée ? */
static int_t     nfree;                                          /* nombre de paramètres libres */
static int_t     nuse;                                           /* nombre de points */
static int_t     parptr[ML_FIT_MAXPAR];                          /* pointeurs sur les paramètres */

/* calcule l'inverse de la matrice invmatrix par l'algorithme de Gauss-Jordan */
static int_t invmat(void)
{
    real_t even, hv[ML_FIT_MAXPAR], mjk, rowmax;
    int_t evin, i, j, k, per[ML_FIT_MAXPAR], row;

    for (i = 0; i < nfree; i++) { per[i] = i; }
    for (j = 0; j < nfree; j++) {
        rowmax = fabs(invmatrix[j][j]);
        row = j;
        for (i = j + 1; i < nfree; i++) {
            if (fabs(invmatrix[i][j] ) > rowmax) {
                rowmax = fabs(invmatrix[i][j]);
                row = i;
            }
        }
        if (invmatrix[row][j] == 0.0) { return -6; }
        if (row > j) {
            for (k = 0; k < nfree; k++) {
                even = invmatrix[j][k];
                invmatrix[j][k] = invmatrix[row][k];
                invmatrix[row][k] = even;
            }
            evin = per[j];
            per[j] = per[row];
            per[row] = evin;
        }
        even = 1.0 / invmatrix[j][j];
        for (i = 0; i < nfree; i++) { invmatrix[i][j] *= even; }
        invmatrix[j][j] = even;
        for (k = 0; k < j; k++) {
            mjk = invmatrix[j][k];
            for (i = 0; i < j; i++) { invmatrix[i][k] -= invmatrix[i][j] * mjk; }
            for (i = j + 1; i < nfree; i++) { invmatrix[i][k] -= invmatrix[i][j] * mjk; }
            invmatrix[j][k] = -even * mjk;
        }
        for (k = j + 1; k < nfree; k++) {
            mjk = invmatrix[j][k];
            for (i = 0; i < j; i++) { invmatrix[i][k] -= invmatrix[i][j] * mjk; }
            for (i = j + 1; i < nfree; i++) { invmatrix[i][k] -= invmatrix[i][j] * mjk; }
            invmatrix[j][k] = -even * mjk;
        }
    }
    for (i = 0; i < nfree; i++) {
        for (k = 0; k < nfree; k++) {
            hv[per[k]] = invmatrix[i][k];
        }
        for (k = 0; k < nfree; k++) {
            invmatrix[i][k] = hv[k];
        }
    }

    return 0;
}

/* construction de la matrice du système */
static void getmat(int_t ifun, real_t *xdat, real_t *ydat, real_t *wdat,
                   int_t ndat, real_t *fpar, real_t *epar, int_t npar,
                   real_t *fopt, libvar_t *pLib)
{
    real_t wd, wn, yd;
    int_t i, j, n;

    for (j = 0; j < nfree; j++) {
        vector[j] = 0.0;
        for (i = 0; i <= j; i++) { matrix[j][i] = 0.0; }
    }
    chi2new = 0.0;
    for (n = 0; n < ndat; n++) {
        wn = wdat[n];
        if (wn > 0.0) {
            yd = ydat[n] - ml_fit_fun(ifun, xdat[n], fpar, npar, fopt);
            ml_fit_dfun(ifun, xdat[n], yd, fpar, epar, npar, fopt, pLib);
            chi2new += (yd * yd * wn);
            for (j = 0; j < nfree; j++) {
                wd = epar[parptr[j]] * wn;
                vector[j] += (yd * wd);
                for (i = 0; i <= j; i++) { matrix[j][i] += (epar[parptr[i]] * wd); }
            }
        }
    }

    return;
}

/* calcul du vecteur de correction : la matrice est recalculée pour que
    sa diagonale soit 1 + lambda */
static int_t getvec(int_t ifun, real_t *xdat, real_t *ydat, real_t *wdat, int_t ndat,
                    real_t *fpar, real_t *epar, int_t npar, real_t *fopt)
{
    real_t dj, dy, mii, mji, mjj, wn;
    int_t i, j, n, r;

    for (j = 0; j < nfree; j++) {
        mjj = matrix[j][j];
        if (mjj <= 0.0) { return -5; }
        mjj = sqrt(mjj);
        for (i = 0; i < j; i++) {
            mji = matrix[j][i] / mjj / sqrt(matrix[i][i]);
            invmatrix[i][j] = invmatrix[j][i] = mji;
        }
        invmatrix[j][j] = 1.0 + lambda;
    }
    r = invmat();
    if (r != 0) { return r; }
    for (i = 0; i < npar; i++) { epar[i] = fpar[i]; }
    for (j = 0; j < nfree; j++) {
        dj = 0.0;
        mjj = matrix[j][j];
        if (mjj <= 0.0) { return -7; }
        mjj = sqrt(mjj);
        for (i = 0; i < nfree; i++) {
            mii = matrix[i][i];
            if (mii <= 0.0) { return -7; }
            mii = sqrt(mii);
            dj += (vector[i] * invmatrix[j][i] / mjj / mii);
        }
        epar[parptr[j]] += dj;
    }
    chi2old = 0.0;
    for (n = 0; n < ndat; n++) {
        wn = wdat[n];
        if (wn > 0.0) {
            dy = ydat[n] - ml_fit_fun(ifun, xdat[n], epar, npar, fopt);
            chi2old += (wdat[n] * dy * dy);
        }
    }

    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_levmarq      :    Fit non-linéaire
                            (variante de l'algorithme de Levenberg-Marquardt)

        ifun = type de fonction

        xdat            :    Données (abscisses)
        ydat            :    Données (ordonnées)
        wdat            :    Poids des données
        ndat            :    Nombre de points
        fpar            :    Contient en entrée une estimation des paramètres et
                             en sortie la valeur calculée de ces paramètres
        epar            :    Contient une estimation des erreurs
        mpar            :    Masque logique (mpar[i] = 1 -> paramètre libre, 0 = fixe)
        npar            :    Nombre de paramètres
        fopt            :    Paramètres optionnels
        niter           :    Nombre d'itérations
        lambda0         :    Paramètre de descente (nul lorsque les dérivées partielles
                             sont nulles). 1e-15 < lambda0 < 1e15

        Retourne la valeur de chi2
    -------------------------------------------------------------------- */
real_t ml_fit_levmarq(int_t ifun, real_t *xdat, real_t *ydat, real_t *wdat, int_t ndat, real_t *fpar,
                      real_t *epar, int_t *mpar, int_t npar, real_t *fopt, int_t *niter,
                      real_t *lambda0, libvar_t *pLib)
{
    int_t i, n, r;
                
    if (pLib->errcode) { return 1.0; }

    if (pLib->running == 1) { return 1.0; }

    pLib->errcode = 1;
    _tcscpy(pLib->message, _T("Fitting..."));

    itc = 0; found = 0; nfree = 0; nuse = 0;                /* nombre de points valides */

    if (pLib->tol < (DBL_EPSILON * 10.0)) {
        tolerance = DBL_EPSILON * 10.0;                        /* précision par défaut */
    }
    else {
        tolerance = pLib->tol;
    }

    lambda = fabs(*lambda0) * ML_FIT_LAMBDAFAC;

    for (i = 0; i < npar; i++) {
        if (mpar[i]) {
            if (nfree > ML_FIT_MAXPAR) {
                _tcscpy(pLib->message, _T("Too many parameters"));
                return 1.0;
            }
            parptr[nfree++] = i;
        }
    }

    if (nfree == 0) {
        _tcscpy(pLib->message, _T("All fitting parameters are fixed"));
        return 1.0;
    }
    for (n = 0; n < ndat; n++) {
        if (wdat[n] > 0.0) nuse++;
    }
    if (nfree >= nuse) {        /* aucun degré de liberté */
        _tcscpy(pLib->message, _T("Insufficient number of points."));
        return 1.0;
    }
    if (lambda == 0.0) {                                                                    /* fit linéaire */
        for (i = 0; i < nfree; fpar[parptr[i++]] = 0.0) { ; }
        getmat(ifun, xdat, ydat, wdat, ndat, fpar, epar, npar, fopt, pLib);
        r = getvec(ifun, xdat, ydat, wdat, ndat, fpar, epar, npar, fopt);
        if (r != 0) {
            _tcscpy(pLib->message, _T("Parameters evaluation error"));
            return 1.0;
        }
        for (i = 0; i < npar; i++) {
            fpar[i] = epar[i];
            epar[i] = 0.0;
        }
        chi2old = sqrt(chi2old / (real_t) (nuse - nfree));
        for (i = 0; i < nfree; i++) {
            if ((matrix[i][i] <= 0.0) || (invmatrix[i][i] <= 0.0)) {
                _tcscpy(pLib->message, _T("Parameters evaluation error"));
                return 1.0;
            }
            epar[parptr[i]] = chi2old * sqrt(invmatrix[i][i]) / sqrt(matrix[i][i]);
        }
    }
    else {                                                             /* fit */
    /* l'algorithme utilise la descente profonde avec la méthode de Taylor,
        le poids de chacune des deux méthode est contrôlé par lambda. Dans
        la boucle externe, la matrice et le vecteur de correction sont calculés.
        Dans la boucle interne, on vérifie qu'une meilleure approximation que
        la précédente a été obtenue, sinon on augmente lambda, augmentant ainsi
        la poids de la descente profonde, on recalcule la vecteur de correction
        et on recommence... */
        while (!found) {
            if (itc++ >= (*niter)) {
                _tcscpy(pLib->message, _T("Too many iterations"));
                pLib->errcode = 0;
                return 1.0;
            }
            getmat(ifun, xdat, ydat, wdat, ndat, fpar, epar, npar, fopt, pLib);

            if (lambda > ML_FIT_LAMBDAMIN) { lambda /= ML_FIT_LAMBDAFAC; }
            r = getvec(ifun, xdat, ydat, wdat, ndat, fpar, epar, npar, fopt);
            if (r != 0) {
                _tcscpy(pLib->message, _T("Parameters evaluation error"));
                return 1.0;
            }
            while (chi2old >= chi2new) {
                if (lambda > ML_FIT_LAMBDAMAX) {
                    break;
                }
                lambda *= ML_FIT_LAMBDAFAC;
                r = getvec(ifun, xdat, ydat, wdat, ndat, fpar, epar, npar, fopt);
                if (r != 0) {
                    _tcscpy(pLib->message, _T("Parameters evaluation error"));
                    return 1.0;
                }
            }

            if (lambda <= ML_FIT_LAMBDAMAX) {
                for (i = 0; i < npar; i++) { fpar[i] = epar[i]; }
            }

            *lambda0 = lambda;

            if (fabs(chi2new - chi2old) <= (tolerance * chi2old) || (lambda > ML_FIT_LAMBDAMAX)) {
                /* une solution satisfaisante a été trouvée, d'où calcul de l'erreur,
                    par Taylor, et du chi2 */
                lambda = 0.0;                                        /* Taylor */
                getmat(ifun, xdat, ydat, wdat, ndat, fpar, epar, npar, fopt, pLib);
                r = getvec(ifun, xdat, ydat, wdat, ndat, fpar, epar, npar, fopt);
                if (r != 0) {
                    _tcscpy(pLib->message, _T("Parameters evaluation error"));
                    return 1.0;
                }
                for (i = 0; i < npar; i++) {
                    epar[i] = 0.0;
                }
                chi2new = sqrt(chi2new / (real_t) (nuse - nfree));
                for (i = 0; i < nfree; i++) {
                    if ((matrix[i][i] <= 0.0) || (invmatrix[i][i] <= 0.0)) {
                        _tcscpy(pLib->message, _T("Parameters evaluation error"));
                        return 1.0;
                    }
                    epar[parptr[i]] = chi2new * sqrt(invmatrix[i][i] ) / sqrt(matrix[i][i]);
                }
                pLib->errcode = 0;
                found = 1;
            }

            if (pLib->interact != 0) {
                char_t szTmp[ML_STRSIZES];
                pLib->progress = 100;
                _tcsprintf(pLib->message, (ML_STRSIZE - 1), _T("Fit: Iteration = %d ; Chi2 = "), itc + 1);
                _tcsprintf(szTmp, (ML_STRSIZES - 1), _T("%4.4e"), chi2new);
                ml_reformat(szTmp);
                _tcscat(pLib->message, (const char_t*)szTmp);
                if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_fit_levmarq]"), ML_STRSIZE - 1); }

                if (pLib->console != 0) {
                    ml_print((const char_t*)(pLib->message));
                }
            }
            //
        }
    }

    _tcscpy(pLib->message, _T("Done."));
    if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_fit_levmarq]"), ML_STRSIZE - 1); }

    *niter = itc;
    return chi2new;
}