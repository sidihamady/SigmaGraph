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

static userfit_t *g_pUserFit = NULL;
static char_t *g_pszFunction = NULL;
static char_t *g_pszDeriv[ML_FIT_MAXPAR] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static int_t g_iInit = 0;

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_userinit    :    Initialiser la fonction de fit définie
    par l'utilisateur

    pUserFit            :    Paramètre du fit

    Retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_fit_userinit(const userfit_t *pUserFit, libvar_t *pLib)
{
    int_t jj, iLen = 0;

    if (pUserFit == NULL) {
        pLib->errcode = 1;
        return -1;
    }

    if (g_iInit == 0) {
        g_pszFunction = NULL;
        for (jj = 0; jj < ML_FIT_MAXPAR; jj++) {
            g_pszDeriv[jj] = NULL;
        }
        g_pUserFit = NULL;
        g_iInit = 1;
    }

    if ((pUserFit->parcount < 2) || (pUserFit->parcount > ML_FIT_MAXPAR)) {
        _tcscpy(pLib->message, _T("Invalid number of fitting parameters"));
        pLib->errcode = 1;
        return -1;
    }
    iLen = (int_t) _tcslen((const char_t*) (pUserFit->name));
    if ((iLen < 1) || (iLen >= ML_STRSIZES)) {
        _tcscpy(pLib->message, _T("Invalid fitting model name"));
        pLib->errcode = 1;
        return -1;
    }
    iLen = (int_t) _tcslen((const char_t*) (pUserFit->function));
    if ((iLen < 1) || (iLen >= ML_STRSIZE)) {
        _tcscpy(pLib->message, _T("Invalid fitting function"));
        pLib->errcode = 1;
        return -1;
    }
    if (_tcschr(pUserFit->function, _T('x')) == NULL) {
        _tcscpy(pLib->message, _T("Invalid fitting function"));
        return -1;
    }
    for (jj = 0; jj < pUserFit->parcount; jj++) {
        iLen = (int_t) _tcslen((const char_t*) (pUserFit->derivative[jj]));
        if ((iLen < 1) || (iLen >= ML_STRSIZE)) {
            _tcscpy(pLib->message, _T("Invalid fitting function derivative"));
            pLib->errcode = 1;
            return -1;
        }
        if (_tcschr(pUserFit->function, _T('a') + jj) == NULL) {
            _tcscpy(pLib->message, _T("Invalid fitting function"));
            pLib->errcode = 1;
            return -1;
        }
    }

    ml_fit_userend();

    g_pUserFit = (userfit_t*) malloc(sizeof(userfit_t));
    if (g_pUserFit == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        pLib->errcode = 1;
        return -1;
    }
    memset(g_pUserFit, 0, sizeof(userfit_t));

    g_pUserFit->id = pUserFit->id;
    g_pUserFit->index = pUserFit->index;
    g_pUserFit->parcount = pUserFit->parcount;
    _tcscpy(g_pUserFit->name, (const char_t*) (pUserFit->name));
    _tcscpy(g_pUserFit->function, (const char_t*) (pUserFit->function));
    for (jj = 0; jj < pUserFit->parcount; jj++) {
        g_pUserFit->fpar[jj] = pUserFit->fpar[jj];
        _tcscpy(g_pUserFit->derivative[jj], (const char_t*) (pUserFit->derivative[jj]));
    }

    pLib->errcode = 0;
    iLen = (int_t) _tcslen((const char_t*) (g_pUserFit->function));
    g_pszFunction = ml_parser_decode(g_pUserFit->function, _T("abcdefghx"), &iLen, pLib);
    if ((pLib->errcode != 0) || (g_pszFunction == NULL) || (iLen < 1) || (iLen >= ML_STRSIZE)) {
        ml_parser_free(g_pszFunction);
        pLib->errcode = 1;
        _tcscpy(pLib->message, _T("Invalid fitting function"));
        return -1;
    }

    for (jj = 0; jj < g_pUserFit->parcount; jj++) {
        pLib->errcode = 0;
        iLen = (int_t) _tcslen((const char_t*) (g_pUserFit->derivative[jj]));
        g_pszDeriv[jj] = ml_parser_decode(g_pUserFit->derivative[jj], _T("abcdefghx"), &iLen, pLib);
        if ((pLib->errcode != 0) || (g_pszDeriv[jj] == NULL) || (iLen < 1) || (iLen >= ML_STRSIZE)) {
            ml_parser_free(g_pszDeriv[jj]);
            pLib->errcode = 1;
            _tcscpy(pLib->message, _T("Invalid fitting function"));
            return -1;
        }
    }

    pLib->errcode = 0;
    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_userend        :    Terminer la session de fit définie
    par l'utilisateur
    -------------------------------------------------------------------- */
void ml_fit_userend(void)
{
    int_t jj;

    if (g_pszFunction != NULL) {
        ml_parser_free(g_pszFunction);
        g_pszFunction = NULL;
    }
    for (jj = 0; jj < ML_FIT_MAXPAR; jj++) {
        if (g_pszDeriv[jj] != NULL) {
            ml_parser_free(g_pszDeriv[jj]);
            g_pszDeriv[jj] = NULL;
        }
    }

    if (g_pUserFit != NULL) {
        free(g_pUserFit);
        g_pUserFit = NULL;
    }

    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_userfun    :    Fonction de fit définie par l'utilisateur

    x                :    Valuer de x
    fpar            :    Paramètres de la fonction

    Retourne la valeur de la fonction en x
    -------------------------------------------------------------------- */
real_t ml_fit_userfun(real_t x, real_t *fpar)
{
    int_t jj;
    real_t y = 0.0;
    libvar_t libT;

    if (g_pUserFit == NULL) {
        ml_fit_userend();
        return 0.0;
    }

    if ((g_pUserFit->parcount < 2) || (g_pUserFit->parcount > ML_FIT_MAXPAR) || (g_pszFunction == NULL)) {
        ml_fit_userend();
        return 0.0;
    }
    for (jj = 0; jj < g_pUserFit->parcount; jj++) {
        if (g_pszDeriv[jj] == NULL) {
            ml_fit_userend();
            return 0.0;
        }
    }

    ml_reset(&libT);
    switch (g_pUserFit->parcount) {
        case 2:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abx"), fpar[0], fpar[1], x);
            break;
        case 3:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abcx"), fpar[0], fpar[1], fpar[2], x);
            break;
        case 4:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abcdx"), fpar[0], fpar[1], fpar[2],
                fpar[3], x);
            break;
        case 5:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abcdex"), fpar[0], fpar[1], fpar[2],
                fpar[3], fpar[4], x);
            break;
        case 6:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abcdefx"), fpar[0], fpar[1], fpar[2],
                fpar[3], fpar[4], fpar[5], x);
            break;
        case 7:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abcdefgx"), fpar[0], fpar[1], fpar[2],
                fpar[3], fpar[4], fpar[5], fpar[6], x);
            break;
        case 8:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abcdefghx"), fpar[0], fpar[1], fpar[2],
                fpar[3], fpar[4], fpar[5], fpar[6], fpar[7], x);
            break;
        default:
            y = ml_parser_evalf(&libT, g_pszFunction, _T("abcdefghx"), fpar[0], fpar[1], fpar[2],
                fpar[3], fpar[4], fpar[5], fpar[6], fpar[7], x);
            break;
    }

    return y;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_userderiv    :    Dérivées patielles de la fonction de fit définie
    par l'utilisateur

    x                   :    Valuer de x
    fpar                :    Paramètres de la fonction
    dpar                :    Contiendra les dérivées patielles des paramètres
    -------------------------------------------------------------------- */
void ml_fit_userderiv(real_t x, real_t *fpar, real_t *dpar)
{
    int_t jj;
    libvar_t libT;

    if (g_pUserFit == NULL) {
        ml_fit_userend();
        return;
    }

    if ((g_pUserFit->parcount < 2) || (g_pUserFit->parcount > ML_FIT_MAXPAR) || (g_pszFunction == NULL)) {
        ml_fit_userend();
        return;
    }
    for (jj = 0; jj < g_pUserFit->parcount; jj++) {
        if (g_pszDeriv[jj] == NULL) {
            ml_fit_userend();
            return;
        }
    }

    ml_reset(&libT);
    switch (g_pUserFit->parcount) {
        case 2:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abx"), fpar[0], fpar[1], x);
            }
            break;
        case 3:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abcx"), fpar[0], fpar[1], fpar[2], x);
            }
            break;
        case 4:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abcdx"), fpar[0], fpar[1], fpar[2],
                    fpar[3], x);
            }
            break;
        case 5:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abcdex"), fpar[0], fpar[1], fpar[2],
                    fpar[3], fpar[4], x);
            }
            break;
        case 6:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abcdefx"), fpar[0], fpar[1], fpar[2],
                    fpar[3], fpar[4], fpar[5], x);
            }
            break;
        case 7:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abcdefgx"), fpar[0], fpar[1], fpar[2],
                    fpar[3], fpar[4], fpar[5], fpar[6], x);
            }
            break;
        case 8:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abcdefghx"), fpar[0], fpar[1], fpar[2],
                    fpar[3], fpar[4], fpar[5], fpar[6], fpar[7], x);
            }
            break;
        default:
            for (jj = 0; jj < g_pUserFit->parcount; jj++) {
                dpar[jj] = ml_parser_evalf(&libT, g_pszDeriv[jj], _T("abcdefghx"), fpar[0], fpar[1], fpar[2],
                    fpar[3], fpar[4], fpar[5], fpar[6], fpar[7], x);
            }
            break;
    }

    return;
}

// Diode
static real_t g_ISC;
static real_t g_RP;
static real_t g_IS1;
static real_t g_n1;
static real_t g_IS2;
static real_t g_n2;
static real_t g_RS;
static real_t g_V;
static real_t g_T;
static real_t g_VT;

real_t diode_fun(real_t I)
{
    real_t Iph, val, VP;

    //>> Calcul du photocourant (charge en court-circuit -> V = 0 et I = ISC)
    // Courants de D1 et D2
    Iph = (g_IS1 * (exp(g_RS * g_ISC / (g_n1 * g_VT)) - 1.0));
    Iph += (g_IS2 * (exp(g_RS * g_ISC / (g_n2 * g_VT)) - 1.0));

    // Courant dans RP
    Iph += ((g_RS / g_RP) * g_ISC);

    // Courant débité
    Iph += g_ISC;
    //<<

    //>> Calcul de f(V,I)
    VP = g_V + (g_RS * I);
    val = Iph;

    // Courants de D1 et D2
    val -= (g_IS1 * (exp(VP / (g_n1 * g_VT)) - 1.0));
    val -= (g_IS2 * (exp(VP / (g_n2 * g_VT)) - 1.0));

    // Courant dans RP
    val -= (VP / g_RP);

    // Courant débité
    val -= I;
    //<<

    return val;
}

real_t diode_currentr(real_t volt, real_t *fpar, real_t *fopt)
{
    real_t II, I1, I2, val;
    libvar_t libT;

    //    fpar[0]        ISC
    //    fpar[1]        RP
    //    fpar[2]        IS1
    //    fpar[3]        n1
    //    fpar[4]        IS2
    //    fpar[5]        n2
    //    fpar[6]        RS

    //    fopt[0]        V
    //    fopt[1]        T
    //    fopt[2]        I1
    //    fopt[3]        I2
    //    fopt[4]        Eg

    if ((fpar[0] < 0.0) || (fpar[0] > 10.0)) {
        fpar[0] = 0.0;
    }
    if ((fpar[1] < 1e-3) || (fpar[1] > 1e15)) {
        fpar[1] = 1e6;
    }
    if (fpar[2] < 1e-42) {
        fpar[2] = 1e-42;
    }
    if (fpar[2] > 1.0) {
        fpar[2] = 1.0;
    }
    if ((fpar[3] < 1.0) || (fpar[3] > 20.0)) {
        fpar[3] = 1.0;
    }
    if (fpar[4] < 1e-42) {
        fpar[4] = 1e-42;
    }
    if (fpar[4] > 1.0) {
        fpar[4] = 1.0;
    }
    if ((fpar[5] < 1.0) || (fpar[5] > 20.0)) {
        fpar[5] = 1.0;
    }
    if ((fpar[6] < 0.0) || (fpar[6] > 1e13)) {
        fpar[6] = 1.0;
    }

    g_ISC = fpar[0];
    g_RP = fpar[1];
    g_IS1 = fpar[2];
    g_n1 = fpar[3];
    g_IS2 = fpar[4];
    g_n2 = fpar[5];
    g_RS = fpar[6];

    g_V = volt;
    g_T = fopt[1];
    g_VT = g_T * (0.000086173429);

    I1 = fopt[2];
    I2 = fopt[3];

    if ((g_V < -100.0) || (g_V > 100.0) || (g_T < 1.0) || (g_T > 1300.0)
        || (I1 < -10.0) || (I2 < -10.0)
        || (I1 > 10.0) || (I2 > 10.0)) {

        return 0.0;
    }

    if (I2 < I1) {
        val = I1;
        I1 = I2;
        I2 = val;
    }

    if (g_RS < 0.01) {
        II = -g_ISC;
        II += (g_IS1 * (exp(g_V / (g_n1 * g_VT)) - 1.0));
        II += (g_IS2 * (exp(g_V / (g_n2 * g_VT)) - 1.0));
        II += (g_V / g_RP);

        return II;
    }

    ml_reset(&libT);

    II = ml_fzero(diode_fun, I1, I2, &libT);

    return (-II);
}

void diode_dcurrentr(real_t volt, real_t *fpar, real_t *dpar, real_t *fopt, real_t feps)
{
    real_t fdd, fdr, fdl, fparBak, I1, I2;
    int_t jj, npar = 7;

    //    fpar[0]        ISC
    //    fpar[1]        RP
    //    fpar[2]        IS1
    //    fpar[3]        n1
    //    fpar[4]        IS2
    //    fpar[5]        n2
    //    fpar[6]        RS

    //    fopt[0]        V
    //    fopt[1]        T
    //    fopt[2]        I1
    //    fopt[3]        I2
    //    fopt[4]        Eg

    if ((fpar[0] < 0.0) || (fpar[0] > 10.0)) {
        fpar[0] = 0.0;
    }
    if ((fpar[1] < 1e-3) || (fpar[1] > 1e15)) {
        fpar[1] = 1e6;
    }
    if (fpar[2] < 1e-42) {
        fpar[2] = 1e-42;
    }
    if (fpar[2] > 1.0) {
        fpar[2] = 1.0;
    }
    if ((fpar[3] < 1.0) || (fpar[3] > 20.0)) {
        fpar[3] = 1.0;
    }
    if (fpar[4] < 1e-42) {
        fpar[4] = 1e-42;
    }
    if (fpar[4] > 1.0) {
        fpar[4] = 1.0;
    }
    if ((fpar[5] < 1.0) || (fpar[5] > 20.0)) {
        fpar[5] = 1.0;
    }
    if ((fpar[6] < 0.0) || (fpar[6] > 1e13)) {
        fpar[6] = 1.0;
    }

    g_ISC = fpar[0];
    g_RP = fpar[1];
    g_IS1 = fpar[2];
    g_n1 = fpar[3];
    g_IS2 = fpar[4];
    g_n2 = fpar[5];
    g_RS = fpar[6];

    g_V = volt;
    g_T = fopt[1];
    g_VT = g_T * (0.000086173429);

    I1 = fopt[2];
    I2 = fopt[3];

    if ((g_V < -100.0) || (g_V > 100.0) || (g_T < 1.0) || (g_T > 1300.0)
        || (I1 < -10.0) || (I2 < -10.0)
        || (I1 > 10.0) || (I2 > 10.0)) {

        return;
    }

    if (g_RS < 0.01) {
        dpar[0] = -1.0;
        dpar[1] = -(g_V / (g_RP * g_RP));
        dpar[2] = (exp(g_V / (g_n1 * g_VT)) - 1.0);
        dpar[3] = -(g_V / (g_n1 * g_n1 * g_VT)) * g_IS1 * exp(g_V / (g_n1 * g_VT));
        dpar[4] = (exp(g_V / (g_n2 * g_VT)) - 1.0);
        dpar[5] = -(g_V / (g_n2 * g_n2 * g_VT)) * g_IS2 * exp(g_V / (g_n2 * g_VT));
        dpar[6] = 0.0;
        return;
    }

    if ((feps < DBL_EPSILON) || (feps > 0.1)) {
        feps = DBL_EPSILON * 10.0;
    }

    for (jj = 0; jj < npar; jj++) {
        fparBak = fpar[jj];
        fdd = feps * fpar[jj];
        if (fdd < DBL_EPSILON) {
            fdd = DBL_EPSILON;
        }
        fpar[jj] = fparBak - fdd;
        fdl = diode_currentr(g_V, fpar, fopt);
        fpar[jj] = fparBak + fdd;
        fdr = diode_currentr(g_V, fpar, fopt);
        dpar[jj] = (fdr - fdl) / (2.0 * fdd);
        fpar[jj] = fparBak;
    }

    return;
}
// ----------------------------------------------------------------------------

real_t diode_current(real_t volt, real_t *fpar, real_t *fopt)
{
    real_t II;

    real_t ISC, IS, n, RP, T, VT;

    //    fpar[0]        ISC
    //    fpar[1]        RP
    //    fpar[2]        IS
    //    fpar[3]        n

    if ((fpar[0] < 0.0) || (fpar[0] > 10.0)) {
        fpar[0] = 0.0;
    }
    if ((fpar[1] < 1e-3) || (fpar[1] > 1e15)) {
        fpar[1] = 1e6;
    }
    if (fpar[2] < 1e-42) {
        fpar[2] = 1e-42;
    }
    if (fpar[2] > 1.0) {
        fpar[2] = 1.0;
    }
    if ((fpar[3] < 1.0) || (fpar[3] > 20.0)) {
        fpar[3] = 1.0;
    }

    ISC = fpar[0];
    RP = fpar[1];
    IS = fpar[2];
    n = fpar[3];

    T = fopt[1];
    VT = T * (0.000086173429);

    if ((volt < -100.0) || (volt > 100.0)) {
        return 0.0;
    }

    II = -ISC;
    II += (volt / RP);
    II += (IS * (exp(volt / (n * VT)) - 1.0));

    return II;
}

void diode_dcurrent(real_t volt, real_t *fpar, real_t *dpar, real_t *fopt)
{
    real_t ISC, IS, n, RP, T, VT;

    //    fpar[0]        ISC
    //    fpar[1]        RP
    //    fpar[2]        IS
    //    fpar[3]        n

    if ((fpar[0] < 0.0) || (fpar[0] > 10.0)) {
        fpar[0] = 0.0;
    }
    if ((fpar[1] < 1e-3) || (fpar[1] > 1e15)) {
        fpar[1] = 1e6;
    }
    if (fpar[2] < 1e-42) {
        fpar[2] = 1e-42;
    }
    if (fpar[2] > 1.0) {
        fpar[2] = 1.0;
    }
    if ((fpar[3] < 1.0) || (fpar[3] > 20.0)) {
        fpar[3] = 1.0;
    }

    ISC = fpar[0];
    RP = fpar[1];
    IS = fpar[2];
    n = fpar[3];

    T = fopt[1];
    VT = T * (0.000086173429);

    if ((volt < -100.0) || (volt > 100.0)) {
        return;
    }

    dpar[0] = -1.0;
    dpar[1] = -(volt / (RP * RP));
    dpar[2] = (exp(volt / (n * VT)) - 1.0);
    dpar[3] = -(volt / (n * n * VT)) * IS * exp(volt / (n * VT));

    return;
}

real_t admittance_cm(real_t freq, real_t *fpar)
{
    real_t w, d1, d2, C, G, Cm,
        r, L, GLF, CHF, C1, t1, C2, t2,
        XX, Y, Z;

    w = 2.0 * ML_PI * freq;

    //    fpar[0]        r
    //    fpar[1]        L
    //    fpar[2]        GLF
    //    fpar[3]        CHF
    //    fpar[4]        C1
    //    fpar[5]        Tau1
    //    fpar[6]        C2
    //    fpar[7]        Tau2

    r = fpar[0];
    L = fpar[1];
    GLF = fpar[2];
    CHF = fpar[3];
    C1 = fpar[4];
    t1 = fpar[5];
    C2 = fpar[6];
    t2 = fpar[7];

    d1 = 1.0 + (w * w * t1 * t1);
    d2 = 1.0 + (w * w * t2 * t2);

    C = CHF + (C1 / d1) + (C2 / d2);
    G = GLF + (w * w * t1 * C1 / d1) + (w * w * t2 * C2 / d2);

    XX = 1.0 - ((L * G * G / C) + (w * w * L * C));
    Y = 1.0 + (r * G) - (w * w * L * C);
    Z = w * ((r * C) + (L * G));

    Cm = C * XX / ((Y * Y) + (Z * Z));

    return Cm;
}

void admittance_dcm(real_t freq, real_t *fpar, real_t *dpar)
{
    real_t w, d1, d2, C, G,
        r, L, GLF, CHF, C1, t1, C2, t2,
        XX, Y, Z, U,
        dCC1, dCC2, dGC1, dGC2,
        dCT1, dCT2, dGT1, dGT2,
        dXXr, dXXL, dXXGL, dXXCH,
        dXXC1, dXXT1, dXXC2, dXXT2,
        dYr, dYL, dYGL, dYCH,
        dYC1, dYT1, dYC2, dYT2,
        dZr, dZL, dZGL, dZCH,
        dZC1, dZT1, dZC2, dZT2;

    w = 2.0 * ML_PI * freq;

    //    fpar[0]        r
    //    fpar[1]        L
    //    fpar[2]        GLF
    //    fpar[3]        CHF
    //    fpar[4]        C1
    //    fpar[5]        Tau1
    //    fpar[6]        C2
    //    fpar[7]        Tau2

    r = fpar[0];
    L = fpar[1];
    GLF = fpar[2];
    CHF = fpar[3];
    C1 = fpar[4];
    t1 = fpar[5];
    C2 = fpar[6];
    t2 = fpar[7];

    d1 = 1.0 + (w * w * t1 * t1);
    d2 = 1.0 + (w * w * t2 * t2);

    C = CHF + (C1 / d1) + (C2 / d2);
    G = GLF + (w * w * t1 * C1 / d1) + (w * w * t2 * C2 / d2);

    XX = 1.0 - ((L * G * G / C) + (w * w * L * C));
    Y = 1.0 + (r * G) - (w * w * L * C);
    Z = w * ((r * C) + (L * G));
    U = (Y * Y) + (Z * Z);

    dCC1 = 1.0 / d1;
    dCT1 = -2.0 * C1 * w * w * t1 / (d1 * d1);
    dGC1 = w * w * t1 / d1;
    dGT1 = C1 * w * w * (1.0 - (w * w * t1 * t1)) / (d1 * d1);
    dCC2 = 1.0 / d2;
    dCT2 = -2.0 * C2 * w * w * t2 / (d2 * d2);
    dGC2 = w * w * t2 / d2;
    dGT2 = C2 * w * w * (1.0 - (w * w * t2 * t2)) / (d2 * d2);

    dXXr = 0.0;
    dXXL = -((G * G / C) + (w * w * C));
    dXXGL = -2.0 * L * G / C;
    dXXCH = -((-L * G * G / (C * C)) + (w * w * L));
    dXXC1 = -(((2.0 * L * dGC1 * C) - (L * G * G * dCC1)) / (C * C)) - (w * w * L * dCC1);
    dXXT1 = -(((2.0 * L * dGT1 * C) - (L * G * G * dCT1)) / (C * C)) - (w * w * L * dCT1);
    dXXC2 = -(((2.0 * L * dGC2 * C) - (L * G * G * dCC2)) / (C * C)) - (w * w * L * dCC2);
    dXXT2 = -(((2.0 * L * dGT2 * C) - (L * G * G * dCT2)) / (C * C)) - (w * w * L * dCT2);

    dYr = G;
    dYL = -(w * w * C);
    dYGL = r;
    dYCH = -(w * w * L);
    dYC1 = (r * dGC1) - (w * w * L * dCC1);
    dYT1 = (r * dGT1) - (w * w * L * dCT1);
    dYC2 = (r * dGC2) - (w * w * L * dCC2);
    dYT2 = (r * dGT2) - (w * w * L * dCT2);

    dZr = w * C;
    dZL = w * G;
    dZGL = w * L;
    dZCH = w * r;

    dZC1 = w * ((r * dCC1) + (L * dGC1));
    dZT1 = w * ((r * dCT1) + (L * dGT1));
    dZC2 = w * ((r * dCC2) + (L * dGC2));
    dZT2 = w * ((r * dCT2) + (L * dGT2));

    dpar[0] = ((C * dXXr * U) - (C * XX * ((2.0 * Y * dYr) + (2.0 * Z * dZr)))) / (U * U);
    dpar[1] = ((C * dXXL * U) - (C * XX * ((2.0 * Y * dYL) + (2.0 * Z * dZL)))) / (U * U);
    dpar[2] = ((C * dXXGL * U) - (C * XX * ((2.0 * Y * dYGL) + (2.0 * Z * dZGL)))) / (U * U);
    dpar[3] = (((C * dXXCH * U) + (XX * U)) - (C * XX * ((2.0 * Y * dYCH) + (2.0 * Z * dZCH)))) / (U * U);
    dpar[4] = (((C * dXXC1 * U) + (XX * dCC1 * U)) - (C * XX * ((2.0 * Y * dYC1) + (2.0 * Z * dZC1)))) / (U * U);
    dpar[5] = (((C * dXXT1 * U) + (XX * dCT1 * U)) - (C * XX * ((2.0 * Y * dYT1) + (2.0 * Z * dZT1)))) / (U * U);
    dpar[6] = (((C * dXXC2 * U) + (XX * dCC2 * U)) - (C * XX * ((2.0 * Y * dYC2) + (2.0 * Z * dZC2)))) / (U * U);
    dpar[7] = (((C * dXXT2 * U) + (XX * dCT2 * U)) - (C * XX * ((2.0 * Y * dYT2) + (2.0 * Z * dZT2)))) / (U * U);
}

real_t admittance_gm(real_t freq, real_t *fpar)
{
    real_t w, d1, d2, C, G, Gm,
        r, L, GLF, CHF, C1, t1, C2, t2,
        X, Y, Z;

    w = 2.0 * ML_PI * freq;

    //    fpar[0]        r
    //    fpar[1]        L
    //    fpar[2]        GLF
    //    fpar[3]        CHF
    //    fpar[4]        C1
    //    fpar[5]        Tau1
    //    fpar[6]        C2
    //    fpar[7]        Tau2

    r = fpar[0];
    L = fpar[1];
    GLF = fpar[2];
    CHF = fpar[3];
    C1 = fpar[4];
    t1 = fpar[5];
    C2 = fpar[6];
    t2 = fpar[7];

    d1 = 1.0 + (w * w * t1 * t1);
    d2 = 1.0 + (w * w * t2 * t2);

    C = CHF + (C1 / d1) + (C2 / d2);
    G = GLF + (w * w * t1 * C1 / d1) + (w * w * t2 * C2 / d2);

    X = 1.0 + (r * G) + (w * w * r * C * C / G);
    Y = 1.0 + (r * G) - (w * w * L * C);
    Z = w * ((r * C) + (L * G));

    Gm = G * X / ((Y * Y) + (Z * Z));

    return (Gm / w);
}

void admittance_dgm(real_t freq, real_t *fpar, real_t *dpar)
{
    real_t w, d1, d2, C, G,
        r, L, GLF, CHF, C1, t1, C2, t2,
        X, Y, Z, U,
        dCC1, dCC2, dGC1, dGC2,
        dCT1, dCT2, dGT1, dGT2,
        dXr, dXL, dXGL, dXCH,
        dXC1, dXT1, dXC2, dXT2,
        dYr, dYL, dYGL, dYCH,
        dYC1, dYT1, dYC2, dYT2,
        dZr, dZL, dZGL, dZCH,
        dZC1, dZT1, dZC2, dZT2;

    w = 2.0 * ML_PI * freq;

    //    fpar[0]        r
    //    fpar[1]        L
    //    fpar[2]        GLF
    //    fpar[3]        CHF
    //    fpar[4]        C1
    //    fpar[5]        Tau1
    //    fpar[6]        C2
    //    fpar[7]        Tau2

    r = fpar[0];
    L = fpar[1];
    GLF = fpar[2];
    CHF = fpar[3];
    C1 = fpar[4];
    t1 = fpar[5];
    C2 = fpar[6];
    t2 = fpar[7];

    d1 = 1.0 + (w * w * t1 * t1);
    d2 = 1.0 + (w * w * t2 * t2);

    C = CHF + (C1 / d1) + (C2 / d2);
    G = GLF + (w * w * t1 * C1 / d1) + (w * w * t2 * C2 / d2);

    X = 1.0 + (r * G) + (w * w * r * C * C / G);
    Y = 1.0 + (r * G) - (w * w * L * C);
    Z = w * ((r * C) + (L * G));
    U = (Y * Y) + (Z * Z);

    dCC1 = 1.0 / d1;
    dCT1 = -2.0 * C1 * w * w * t1 / (d1 * d1);
    dGC1 = w * w * t1 / d1;
    dGT1 = C1 * w * w * (1.0 - (w * w * t1 * t1)) / (d1 * d1);
    dCC2 = 1.0 / d2;
    dCT2 = -2.0 * C2 * w * w * t2 / (d2 * d2);
    dGC2 = w * w * t2 / d2;
    dGT2 = C2 * w * w * (1.0 - (w * w * t2 * t2)) / (d2 * d2);

    dXr = G + (w * w * C * C / G);
    dXL = 0.0;
    dXGL = r - (w * w * r * C * C / (G * G));
    dXCH = 2.0 * w * w * r * C / G;

    dXC1 = (r * dGC1) + (w * w * r * ((2.0 * dCC1 * C * G) - (C * C * dGC1)) / (G * G));
    dXT1 = (r * dGT1) + (w * w * r * ((2.0 * dCT1 * C * G) - (C * C * dGT1)) / (G * G));
    dXC2 = (r * dGC2) + (w * w * r * ((2.0 * dCC2 * C * G) - (C * C * dGC2)) / (G * G));
    dXT2 = (r * dGT2) + (w * w * r * ((2.0 * dCT2 * C * G) - (C * C * dGT2)) / (G * G));

    dYr = G;
    dYL = -(w * w * C);
    dYGL = r;
    dYCH = -(w * w * L);

    dYC1 = (r * dGC1) - (w * w * L * dCC1);
    dYT1 = (r * dGT1) - (w * w * L * dCT1);
    dYC2 = (r * dGC2) - (w * w * L * dCC2);
    dYT2 = (r * dGT2) - (w * w * L * dCT2);

    dZr = w * C;
    dZL = w * G;
    dZGL = w * L;
    dZCH = w * r;

    dZC1 = w * ((r * dCC1) + (L * dGC1));
    dZT1 = w * ((r * dCT1) + (L * dGT1));
    dZC2 = w * ((r * dCC2) + (L * dGC2));
    dZT2 = w * ((r * dCT2) + (L * dGT2));

    dpar[0] = ((G * dXr * U) - (G * X * ((2.0 * Y * dYr) + (2.0 * Z * dZr)))) / (U * U * w);
    dpar[1] = ((G * dXL * U) - (G * X * ((2.0 * Y * dYL) + (2.0 * Z * dZL)))) / (U * U * w);
    dpar[2] = ((G * dXGL * U) - (G * X * ((2.0 * Y * dYGL) + (2.0 * Z * dZGL)))) / (U * U * w);
    dpar[3] = (((G * dXCH * U) + (X * U)) - (G * X * ((2.0 * Y * dYCH) + (2.0 * Z * dZCH)))) / (U * U * w);
    dpar[4] = (((G * dXC1 * U) + (X * dCC1 * U)) - (G * X * ((2.0 * Y * dYC1) + (2.0 * Z * dZC1)))) / (U * U * w);
    dpar[5] = (((G * dXT1 * U) + (X * dCT1 * U)) - (G * X * ((2.0 * Y * dYT1) + (2.0 * Z * dZT1)))) / (U * U * w);
    dpar[6] = (((G * dXC2 * U) + (X * dCC2 * U)) - (G * X * ((2.0 * Y * dYC2) + (2.0 * Z * dZC2)))) / (U * U * w);
    dpar[7] = (((G * dXT2 * U) + (X * dCT2 * U)) - (G * X * ((2.0 * Y * dYT2) + (2.0 * Z * dZT2)))) / (U * U * w);
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    ml_fit_fun        :    Fonction de fit

    ifun = type de fonction

    xx                :    Valuer de x
    fpar            :    Paramètres de la fonction
    npar            :    Nombre de paramètres de la fonction
    fopt            :    Paramètres optionnels de la fonction

    Retourne la valeur de la fonction en x
    -------------------------------------------------------------------- */
real_t ml_fit_fun(int_t ifun, real_t xx, real_t *fpar, int_t npar, real_t *fopt)
{
    real_t aa, bb, cc, dd, ee, ftmp, val = 0.0;
    real_t b1, c1, d1, b2, c2, d2, b3, c3, d3, b4, c4, d4, b5, c5, d5;

    if ((npar <= 0) || (npar > ML_FIT_MAXPAR)) {
        return 0.0;
    }

    // User-defined function
    if (ifun == FIT_MODEL_USER) {
        val = ml_fit_userfun(xx, fpar);
        return val;
    }

    // >> Fonctions électroniques
    if (ifun == FIT_MODEL_DIODE) {                // Diode
        if (fopt) {
            fopt[0] = xx;
            val = diode_current(xx, fpar, fopt);
        }
        return val;
    }
    else if (ifun == FIT_MODEL_DIODER) {        // Diode
        if (fopt) {
            fopt[0] = xx;
            val = diode_currentr(xx, fpar, fopt);
        }
        return val;
    }
    else if (ifun == FIT_MODEL_ADMITC) {        // Admittance Cm
        val = admittance_cm(xx, fpar);
        return val;
    }
    else if (ifun == FIT_MODEL_ADMITG) {        // Admittance Gm
        val = admittance_gm(xx, fpar);
        return val;
    }
    // <<

    if ((ifun < FIT_MODEL_FIRST) || (ifun > FIT_MODEL_LAST)) {
        return 0.0;
    }

    aa = fpar[0];    bb = fpar[1];

    switch (ifun) {

        case FIT_MODEL_LINE:        // Line
            val = aa + (bb * xx);
            break;

        case FIT_MODEL_POLY2:    // Poly2
            cc = fpar[2];
            val = aa + (bb * xx) + (cc * xx * xx);
            break;

        case FIT_MODEL_POLY3:    // Poly3
            cc = fpar[2];    dd = fpar[3];
            val = aa + (bb * xx) + (cc * xx * xx) + (dd * xx * xx * xx);
            break;

        case FIT_MODEL_POLY4:    // Poly4
            cc = fpar[2];    dd = fpar[3];    ee = fpar[4];
            val = aa + (bb * xx) + (cc * xx * xx) + (dd * xx * xx * xx) + (ee * xx * xx * xx * xx);
            break;

        case FIT_MODEL_GAUSS1:    // Gauss1
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            aa = fpar[0];    b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            val = aa + (b1 * exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1)));
            break;

        case FIT_MODEL_GAUSS2:    // Gauss2
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            val = aa + (b1 * exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1)));
            val += (b2 * exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2)));
            break;

        case FIT_MODEL_GAUSS3:    // Gauss3
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];

            val = aa + (b1 * exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1)));
            val += (b2 * exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2)));
            val += (b3 * exp(-0.5 * ((xx - d3) / c3) * ((xx - d3) / c3)));
            break;

        case FIT_MODEL_GAUSS4:    // Gauss4
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];

            val = aa + (b1 * exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1)));
            val += (b2 * exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2)));
            val += (b3 * exp(-0.5 * ((xx - d3) / c3) * ((xx - d3) / c3)));
            val += (b4 * exp(-0.5 * ((xx - d4) / c4) * ((xx - d4) / c4)));
            break;

        case FIT_MODEL_GAUSS5:    // Gauss5
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }
            if (fpar[14] <= 0.0) {
                fpar[14] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];
            b5 = fpar[13];    c5 = fpar[14];    d5 = fpar[15];

            val = aa + (b1 * exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1)));
            val += (b2 * exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2)));
            val += (b3 * exp(-0.5 * ((xx - d3) / c3) * ((xx - d3) / c3)));
            val += (b4 * exp(-0.5 * ((xx - d4) / c4) * ((xx - d4) / c4)));
            val += (b5 * exp(-0.5 * ((xx - d5) / c5) * ((xx - d5) / c5)));
            break;

        case FIT_MODEL_LORENTZ1:    // Lorentz1
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            val = aa + (b1 / (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1))));
            break;

        case FIT_MODEL_LORENTZ2:    // Lorentz2
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            val = aa + (b1 / (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1))));
            val += (b2 / (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2))));
            break;

        case FIT_MODEL_LORENTZ3:    // Lorentz3
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            val = aa + (b1 / (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1))));
            val += (b2 / (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2))));
            val += (b3 / (1.0 + (4.0 * (xx - d3) * (xx - d3) / (c3 * c3))));
            break;

        case FIT_MODEL_LORENTZ4:    // Lorentz4
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];
            val = aa + (b1 / (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1))));
            val += (b2 / (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2))));
            val += (b3 / (1.0 + (4.0 * (xx - d3) * (xx - d3) / (c3 * c3))));
            val += (b4 / (1.0 + (4.0 * (xx - d4) * (xx - d4) / (c4 * c4))));
            break;

        case FIT_MODEL_LORENTZ5:    // Lorentz5
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }
            if (fpar[14] <= 0.0) {
                fpar[14] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];
            b5 = fpar[13];    c5 = fpar[14];    d5 = fpar[15];
            val = aa + (b1 / (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1))));
            val += (b2 / (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2))));
            val += (b3 / (1.0 + (4.0 * (xx - d3) * (xx - d3) / (c3 * c3))));
            val += (b4 / (1.0 + (4.0 * (xx - d4) * (xx - d4) / (c4 * c4))));
            val += (b5 / (1.0 + (4.0 * (xx - d5) * (xx - d5) / (c3 * c5))));
            break;

        case FIT_MODEL_PEARSON:    // Pearson VII
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[4] <= 0.0) {
                fpar[4] = 1.0;
            }
            cc = fpar[2];    dd = fpar[3];    ee = fpar[4];
            val = aa + (bb * pow(1.0 + (4.0 * (pow(2.0, 1.0 / ee) - 1.0) * (xx - dd) * (xx - dd) / (cc * cc)), -ee));
            break;

        case FIT_MODEL_EXPGR:    // Exp. Growth
            val = aa;
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            cc = fpar[2];
            val += (bb * exp(cc * xx));
            break;

        case FIT_MODEL_EXPGR2:    // Exp. Growth 2
            val = aa;
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[4] < 0.0) {
                fpar[4] = 1.0;
            }
            cc = fpar[2]; dd = fpar[3]; ee = fpar[4];
            val += (bb * exp(cc * xx));
            val += (dd * exp(ee * xx));
            break;

        case FIT_MODEL_EXPDEC:    // Exp. Decay
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            cc = fpar[2];
            val = aa + (bb * exp(-cc * xx));
            break;

        case FIT_MODEL_EXPDEC2:    // Exp. Decay 2
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[4] < 0.0) {
                fpar[4] = 1.0;
            }
            cc = fpar[2]; dd = fpar[3]; ee = fpar[4];
            val = aa;
            val += (bb * exp(-cc * xx));
            val += (dd * exp(-ee * xx));
            break;

        case FIT_MODEL_HYPERBOLA:    // Hyperbola
            cc = fpar[2]; dd = fpar[3];
            val = aa;
            ftmp = cc + (dd * xx);
            if (ftmp != 0.0) {
                val += (bb * xx / ftmp);
            }
            break;

        case FIT_MODEL_LOG:    // Log
            cc = fpar[2]; dd = fpar[3];
            val = aa;
            ftmp = cc + (dd * xx);
            if (ftmp > 0.0) {
                val += (bb * log(ftmp));
            }
            break;

        case FIT_MODEL_POWER:    // Power
            cc = fpar[2];
            val = aa;
            if (xx > 0.0) {
                val += (bb * pow(xx, cc));
            }
            break;

        case FIT_MODEL_LOGISTIC:    // Logistic
            cc = fpar[2]; dd = fpar[3];
            val = aa;
            if (cc != 0.0) {
                ftmp = xx / cc;
                if (ftmp > 0.0) {
                    val += (bb / (1.0 + pow(ftmp, dd)));
                }
                else if (ftmp == 0.0) {
                    val += bb;
                }
            }
            break;

        case FIT_MODEL_WAVE:    // Wave
            cc = fpar[2]; dd = fpar[3];
            val = aa;
            if (dd != 0.0) {
                val += (bb * sin(ML_PI * (xx - cc) / dd));
            }
            break;

        default:
            val = aa + (bb * xx);
            break;
    }

    return val;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    ml_fit_dfun        :    Dérivée de la fonction de fit

    ifun = type de fonction

    xx                :    Valuer de x
    resd            :    Résidu
    fpar            :    Paramètres de la fonction
    dpar            :    Contiendra les dérivées patielles des paramètres
    npar            :    Nombre de paramètres de la fonction
    fopt            :    Paramètres optionnels de la fonction
    -------------------------------------------------------------------- */
void ml_fit_dfun(int_t ifun, real_t xx, real_t resd, real_t *fpar, real_t *dpar, int_t npar, real_t *fopt, libvar_t *pLib)
{
    real_t aa, bb, cc, dd, ee, ftmp = 0.0, yy;
    real_t b1, c1, d1, b2, c2, d2, b3, c3, d3, b4, c4, d4, b5, c5, d5;

    const real_t ln2 = 0.693147180559945290;

    if ((npar <= 0) || (npar > ML_FIT_MAXPAR)) {
        return;
    }

    // User-defined function
    if (ifun == FIT_MODEL_USER) {
        ml_fit_userderiv(xx, fpar, dpar);
        return;
    }

    // >> Fonctions spéciales
    if (ifun == FIT_MODEL_DIODE) {                // Diode
        if (fopt) {
            fopt[0] = xx;
            diode_dcurrent(xx, fpar, dpar, fopt);
        }
        return;
    }
    else if (ifun == FIT_MODEL_DIODER) {        // Diode
        if (fopt) {
            fopt[0] = xx;
            diode_dcurrent(xx, fpar, dpar, fopt);
        }
        return;
    }
    else if (ifun == FIT_MODEL_ADMITC) {        // Admittance Cm
        admittance_dcm(xx, fpar, dpar);
        return;
    }
    else if (ifun == FIT_MODEL_ADMITG) {        // Admittance Gm
        admittance_dgm(xx, fpar, dpar);
        return;
    }
    // <<
    // --------------------------------------------------------------------------------------

    if ((ifun < FIT_MODEL_FIRST) || (ifun > FIT_MODEL_LAST)) {
        return;
    }

    aa = fpar[0];    bb = fpar[1];

    switch (ifun) {

        case FIT_MODEL_LINE:        // Line
            dpar[0] = 1.0;
            dpar[1] = xx;
            break;

        case FIT_MODEL_POLY2:    // Poly2
            cc = fpar[2];
            dpar[0] = 1.0;
            dpar[1] = xx;
            dpar[2] = xx * xx;
            break;

        case FIT_MODEL_POLY3:    // Poly3
            cc = fpar[2];    dd = fpar[3];
            dpar[0] = 1.0;
            dpar[1] = xx;
            dpar[2] = xx * xx;
            dpar[3] = xx * xx * xx;
            break;

        case FIT_MODEL_POLY4:    // Poly4
            cc = fpar[2];    dd = fpar[3];    ee = fpar[4];
            dpar[0] = 1.0;
            dpar[1] = xx;
            dpar[2] = xx * xx;
            dpar[3] = xx * xx * xx;
            dpar[4] = xx * xx * xx * xx;
            break;

        case FIT_MODEL_GAUSS1:    // Gauss1
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];

            ftmp = exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1));
            dpar[0] = 1.0;
            dpar[1] = ftmp;
            dpar[2] = (b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) * ftmp;
            dpar[3] = (b1 * (xx - d1) / (c1 * c1)) * ftmp;
            break;

        case FIT_MODEL_GAUSS2:    // Gauss2
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];

            dpar[0] = 1.0;

            ftmp = exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1));
            dpar[1] = ftmp;
            dpar[2] = (b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) * ftmp;
            dpar[3] = (b1 * (xx - d1) / (c1 * c1)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2));
            dpar[4] = ftmp;
            dpar[5] = (b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) * ftmp;
            dpar[6] = (b2 * (xx - d2) / (c2 * c2)) * ftmp;
            break;

        case FIT_MODEL_GAUSS3:    // Gauss3
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];

            dpar[0] = 1.0;

            ftmp = exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1));
            dpar[1] = ftmp;
            dpar[2] = (b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) * ftmp;
            dpar[3] = (b1 * (xx - d1) / (c1 * c1)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2));
            dpar[4] = ftmp;
            dpar[5] = (b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) * ftmp;
            dpar[6] = (b2 * (xx - d2) / (c2 * c2)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d3) / c3) * ((xx - d3) / c3));
            dpar[7] = ftmp;
            dpar[8] = (b3 * (xx - d3) * (xx - d3) / (c3 * c3 * c3)) * ftmp;
            dpar[9] = (b3 * (xx - d3) / (c3 * c3)) * ftmp;
            break;

        case FIT_MODEL_GAUSS4:    // Gauss4
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }

            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];

            dpar[0] = 1.0;

            ftmp = exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1));
            dpar[1] = ftmp;
            dpar[2] = (b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) * ftmp;
            dpar[3] = (b1 * (xx - d1) / (c1 * c1)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2));
            dpar[4] = ftmp;
            dpar[5] = (b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) * ftmp;
            dpar[6] = (b2 * (xx - d2) / (c2 * c2)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d3) / c3) * ((xx - d3) / c3));
            dpar[7] = ftmp;
            dpar[8] = (b3 * (xx - d3) * (xx - d3) / (c3 * c3 * c3)) * ftmp;
            dpar[9] = (b3 * (xx - d3) / (c3 * c3)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d4) / c4) * ((xx - d4) / c4));
            dpar[10] = ftmp;
            dpar[11] = (b4 * (xx - d4) * (xx - d4) / (c4 * c4 * c4)) * ftmp;
            dpar[12] = (b4 * (xx - d4) / (c4 * c4)) * ftmp;

            break;

        case FIT_MODEL_GAUSS5:    // Gauss5
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }
            if (fpar[14] <= 0.0) {
                fpar[14] = 1.0;
            }

            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];
            b5 = fpar[13];    c5 = fpar[14];    d5 = fpar[15];

            dpar[0] = 1.0;

            ftmp = exp(-0.5 * ((xx - d1) / c1) * ((xx - d1) / c1));
            dpar[1] = ftmp;
            dpar[2] = (b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) * ftmp;
            dpar[3] = (b1 * (xx - d1) / (c1 * c1)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d2) / c2) * ((xx - d2) / c2));
            dpar[4] = ftmp;
            dpar[5] = (b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) * ftmp;
            dpar[6] = (b2 * (xx - d2) / (c2 * c2)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d3) / c3) * ((xx - d3) / c3));
            dpar[7] = ftmp;
            dpar[8] = (b3 * (xx - d3) * (xx - d3) / (c3 * c3 * c3)) * ftmp;
            dpar[9] = (b3 * (xx - d3) / (c3 * c3)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d4) / c4) * ((xx - d4) / c4));
            dpar[10] = ftmp;
            dpar[11] = (b4 * (xx - d4) * (xx - d4) / (c4 * c4 * c4)) * ftmp;
            dpar[12] = (b4 * (xx - d4) / (c4 * c4)) * ftmp;

            ftmp = exp(-0.5 * ((xx - d5) / c5) * ((xx - d5) / c5));
            dpar[13] = ftmp;
            dpar[14] = (b5 * (xx - d5) * (xx - d5) / (c5 * c5 * c5)) * ftmp;
            dpar[15] = (b5 * (xx - d5) / (c5 * c5)) * ftmp;

            break;

        case FIT_MODEL_LORENTZ1:    // Lorentz1
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }

            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];

            ftmp = (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1)));
            dpar[0] = 1.0;
            dpar[1] = 1 / ftmp;
            dpar[2] = (8.0 * b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) / (ftmp * ftmp);
            dpar[3] = (8.0 * b1 * (xx - d1) / (c1 * c1)) / (ftmp * ftmp);
            break;

        case FIT_MODEL_LORENTZ2:    // Lorentz2
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];

            dpar[0] = 1.0;

            ftmp = (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1)));
            dpar[1] = 1 / ftmp;
            dpar[2] = (8.0 * b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) / (ftmp * ftmp);
            dpar[3] = (8.0 * b1 * (xx - d1) / (c1 * c1)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2)));
            dpar[4] = 1 / ftmp;
            dpar[5] = (8.0 * b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) / (ftmp * ftmp);
            dpar[6] = (8.0 * b2 * (xx - d2) / (c2 * c2)) / (ftmp * ftmp);
            break;

        case FIT_MODEL_LORENTZ3:    // Lorentz3
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];

            dpar[0] = 1.0;

            ftmp = (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1)));
            dpar[1] = 1 / ftmp;
            dpar[2] = (8.0 * b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) / (ftmp * ftmp);
            dpar[3] = (8.0 * b1 * (xx - d1) / (c1 * c1)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2)));
            dpar[4] = 1 / ftmp;
            dpar[5] = (8.0 * b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) / (ftmp * ftmp);
            dpar[6] = (8.0 * b2 * (xx - d2) / (c2 * c2)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d3) * (xx - d3) / (c3 * c3)));
            dpar[7] = 1 / ftmp;
            dpar[8] = (8.0 * b3 * (xx - d3) * (xx - d3) / (c3 * c3 * c3)) / (ftmp * ftmp);
            dpar[9] = (8.0 * b3 * (xx - d3) / (c3 * c3)) / (ftmp * ftmp);

            break;

        case FIT_MODEL_LORENTZ4:    // Lorentz4

            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];

            dpar[0] = 1.0;

            ftmp = (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1)));
            dpar[1] = 1 / ftmp;
            dpar[2] = (8.0 * b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) / (ftmp * ftmp);
            dpar[3] = (8.0 * b1 * (xx - d1) / (c1 * c1)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2)));
            dpar[4] = 1 / ftmp;
            dpar[5] = (8.0 * b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) / (ftmp * ftmp);
            dpar[6] = (8.0 * b2 * (xx - d2) / (c2 * c2)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d3) * (xx - d3) / (c3 * c3)));
            dpar[7] = 1 / ftmp;
            dpar[8] = (8.0 * b3 * (xx - d3) * (xx - d3) / (c3 * c3 * c3)) / (ftmp * ftmp);
            dpar[9] = (8.0 * b3 * (xx - d3) / (c3 * c3)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d4) * (xx - d4) / (c4 * c4)));
            dpar[10] = 1 / ftmp;
            dpar[11] = (8.0 * b4 * (xx - d4) * (xx - d4) / (c4 * c4 * c4)) / (ftmp * ftmp);
            dpar[12] = (8.0 * b4 * (xx - d4) / (c4 * c4)) / (ftmp * ftmp);

            break;

        case FIT_MODEL_LORENTZ5:    // Lorentz5

            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[5] <= 0.0) {
                fpar[5] = 1.0;
            }
            if (fpar[8] <= 0.0) {
                fpar[8] = 1.0;
            }
            if (fpar[11] <= 0.0) {
                fpar[11] = 1.0;
            }
            if (fpar[14] <= 0.0) {
                fpar[14] = 1.0;
            }
            aa = fpar[0];
            b1 = fpar[1];    c1 = fpar[2];    d1 = fpar[3];
            b2 = fpar[4];    c2 = fpar[5];    d2 = fpar[6];
            b3 = fpar[7];    c3 = fpar[8];    d3 = fpar[9];
            b4 = fpar[10];    c4 = fpar[11];    d4 = fpar[12];
            b5 = fpar[13];    c5 = fpar[14];    d5 = fpar[15];

            dpar[0] = 1.0;

            ftmp = (1.0 + (4.0 * (xx - d1) * (xx - d1) / (c1 * c1)));
            dpar[1] = 1 / ftmp;
            dpar[2] = (8.0 * b1 * (xx - d1) * (xx - d1) / (c1 * c1 * c1)) / (ftmp * ftmp);
            dpar[3] = (8.0 * b1 * (xx - d1) / (c1 * c1)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d2) * (xx - d2) / (c2 * c2)));
            dpar[4] = 1 / ftmp;
            dpar[5] = (8.0 * b2 * (xx - d2) * (xx - d2) / (c2 * c2 * c2)) / (ftmp * ftmp);
            dpar[6] = (8.0 * b2 * (xx - d2) / (c2 * c2)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d3) * (xx - d3) / (c3 * c3)));
            dpar[7] = 1 / ftmp;
            dpar[8] = (8.0 * b3 * (xx - d3) * (xx - d3) / (c3 * c3 * c3)) / (ftmp * ftmp);
            dpar[9] = (8.0 * b3 * (xx - d3) / (c3 * c3)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d4) * (xx - d4) / (c4 * c4)));
            dpar[10] = 1 / ftmp;
            dpar[11] = (8.0 * b4 * (xx - d4) * (xx - d4) / (c4 * c4 * c4)) / (ftmp * ftmp);
            dpar[12] = (8.0 * b4 * (xx - d4) / (c4 * c4)) / (ftmp * ftmp);

            ftmp = (1.0 + (4.0 * (xx - d5) * (xx - d5) / (c5 * c5)));
            dpar[13] = 1 / ftmp;
            dpar[14] = (8.0 * b5 * (xx - d5) * (xx - d5) / (c5 * c5 * c5)) / (ftmp * ftmp);
            dpar[15] = (8.0 * b5 * (xx - d5) / (c5 * c5)) / (ftmp * ftmp);

            break;

        case FIT_MODEL_PEARSON:    // Pearson VII
            if (fpar[2] <= 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[4] <= 0.0) {
                fpar[4] = 1.0;
            }
            cc = fpar[2];    dd = fpar[3];    ee = fpar[4];

            ftmp = 1.0 + (4.0 * (pow(2.0, 1.0 / ee) - 1.0) * (xx - dd) * (xx - dd) / (cc * cc));
            dpar[0] = 1.0;
            dpar[1] = pow(ftmp, -ee);
            dpar[2] = (8.0 * bb * ee * (pow(2.0, 1.0 / ee) - 1.0) * (xx - dd) * (xx - dd) / (cc * cc * cc)) * pow(ftmp, -ee - 1.0);
            dpar[3] = (8.0 * bb * ee * (pow(2.0, 1.0 / ee) - 1.0) * (xx - dd) / (cc * cc)) * pow(ftmp, -ee - 1.0);
            yy = bb * pow(ftmp, -ee);
            dpar[4] = yy * (-log(ftmp) + ((4.0 * ln2 * (xx - dd) * (xx - dd) * (pow(2.0, 1.0 / ee)) / (cc * cc * ee)) / ftmp));
            break;

        case FIT_MODEL_EXPGR:    // Exp. Growth
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            cc = fpar[2];
            dpar[0] = 1.0;
            dpar[1] = exp(cc * xx);
            dpar[2] = (xx * bb * exp(cc * xx));
            break;

        case FIT_MODEL_EXPGR2:    // Exp. Growth 2
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[4] < 0.0) {
                fpar[4] = 1.0;
            }
            cc = fpar[2]; dd = fpar[3]; ee = fpar[4];
            dpar[0] = 1.0;
            dpar[1] = exp(cc * xx);
            dpar[2] = (xx * bb * exp(cc * xx));
            dpar[3] = exp(ee * xx);
            dpar[4] = (xx * dd * exp(ee * xx));
            break;

        case FIT_MODEL_EXPDEC:    // Exp. Decay
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            cc = fpar[2];
            dpar[0] = 1.0;
            dpar[1] = exp(-cc * xx);
            dpar[2] = (-xx * bb * exp(-cc * xx));
            break;

        case FIT_MODEL_EXPDEC2:    // Exp. Decay 2
            if (fpar[2] < 0.0) {
                fpar[2] = 1.0;
            }
            if (fpar[4] < 0.0) {
                fpar[4] = 1.0;
            }
            cc = fpar[2]; dd = fpar[3]; ee = fpar[4];
            dpar[0] = 1.0;
            dpar[1] = exp(-cc * xx);
            dpar[2] = (-xx * bb * exp(-cc * xx));
            dpar[3] = exp(ee * xx);
            dpar[4] = (-xx * dd * exp(-ee * xx));
            break;

        case FIT_MODEL_HYPERBOLA:    // Hyperbola
            cc = fpar[2]; dd = fpar[3];
            dpar[0] = 1.0;
            ftmp = cc + (dd * xx);
            if (ftmp != 0.0) {
                dpar[1] = xx / ftmp;
                dpar[2] = -bb * xx / (ftmp * ftmp);
                dpar[3] = -bb * xx * xx / (ftmp * ftmp);
            }
            break;

        case FIT_MODEL_LOG:    // Log
            cc = fpar[2]; dd = fpar[3];
            ftmp = cc + (dd * xx);
            if (ftmp > 0.0) {
                dpar[0] = 1.0;
                dpar[1] = log(ftmp);
                dpar[2] = 1.0 / ftmp;
                dpar[3] = xx / ftmp;
            }
            break;

        case FIT_MODEL_POWER:    // Power
            cc = fpar[2];
            if (xx > 0.0) {
                dpar[0] = 1.0;
                dpar[1] = pow(xx, cc);
                dpar[2] = bb * log(xx) * pow(xx, cc);
            }
            break;

        case FIT_MODEL_LOGISTIC:    // Logistic
            cc = fpar[2]; dd = fpar[3];
            if (cc != 0.0) {
                ftmp = xx / cc;
                if (ftmp > 0.0) {
                    yy = 1.0 + pow(ftmp, dd);
                    dpar[0] = 1.0;
                    dpar[1] = 1.0 / yy;
                    dpar[2] = -bb * (-dd * pow(ftmp, dd) / cc) / (yy * yy);
                    dpar[3] = -bb * (log(ftmp) * pow(ftmp, dd)) / (yy * yy);
                }
                else if (ftmp == 0.0) {
                    dpar[0] = 1.0;
                    dpar[1] = 1.0;
                }
            }
            break;

        case FIT_MODEL_WAVE:    // Wave
            cc = fpar[2]; dd = fpar[3];
            if (dd != 0.0) {
                dpar[0] = 1.0;
                dpar[1] = sin(ML_PI * (xx - cc) / dd);
                dpar[2] = (ML_PI / dd) * cos(ML_PI * (xx - cc) / dd);
                dpar[3] = (ML_PI * (xx - cc) / (dd * dd)) * cos(ML_PI * (xx - cc) / dd);
            }
            break;

        default:
            dpar[0] = 1.0;
            dpar[1] = xx;
            break;
    }
}
