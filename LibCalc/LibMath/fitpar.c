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

fit_t ml_fitmodel[ML_FIT_MAXMODEL];    /* Modèles de fit disponible */

char_t const*const MODELNAME_SHORT[] = {
    _T("Linear"),             // FIT_MODEL_LINE
    _T("Poly2"),              // FIT_MODEL_POLY2
    _T("Poly3"),              // FIT_MODEL_POLY3
    _T("Poly4"),              // FIT_MODEL_POLY4
    _T("Gauss1"),             // FIT_MODEL_GAUSS1
    _T("Gauss2"),             // FIT_MODEL_GAUSS2
    _T("Gauss3"),             // FIT_MODEL_GAUSS3
    _T("Gauss4"),             // FIT_MODEL_GAUSS4
    _T("Gauss5"),             // FIT_MODEL_GAUSS5
    _T("Lorentz1"),           // FIT_MODEL_LORENTZ1
    _T("Lorentz2"),           // FIT_MODEL_LORENTZ2
    _T("Lorentz3"),           // FIT_MODEL_LORENTZ3
    _T("Lorentz4"),           // FIT_MODEL_LORENTZ4
    _T("Lorentz5"),           // FIT_MODEL_LORENTZ5
    _T("Pearson"),            // FIT_MODEL_PEARSON
    _T("Exp"),                // FIT_MODEL_EXPGR
    _T("Exp2"),               // FIT_MODEL_EXPGR2
    _T("ExpDec"),             // FIT_MODEL_EXPDEC
    _T("ExpDec2"),            // FIT_MODEL_EXPDEC2
    _T("Hyperbola"),          // FIT_MODEL_HYPERBOLA
    _T("Log"),                // FIT_MODEL_LOG
    _T("Power"),              // FIT_MODEL_POWER
    _T("Logistic"),           // FIT_MODEL_LOGISTIC
    _T("Wave")                // FIT_MODEL_WAVE
};

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_init         :    Initialisation de la session de fit

        pfit            :    Tableau de modèles à initialiser (pointeur)
        fitcount        :    Nombre d'éléments du tableau de modèles
    -------------------------------------------------------------------- */
void ml_fit_init(int_t *iter, real_t *lambda)
{
    int_t ii, ll;

    if (*lambda < ML_FIT_LAMBDAMIN) { *lambda = ML_FIT_LAMBDAMIN; }
    if (*lambda > ML_FIT_LAMBDAMAX) { *lambda = ML_FIT_LAMBDAMAX; }
    if (*iter < 1) { *iter = 1; }
    if (*iter > ML_MAXITER) { *iter = ML_MAXITER; }

    memset(ml_fitmodel, 0, ML_FIT_MAXMODEL * sizeof(fit_t));

    _tcscpy(ml_fitmodel[FIT_MODEL_LINE].name, _T("Linear"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LINE].function, _T("a + b*x"));
    ml_fitmodel[FIT_MODEL_LINE].parcount = 2;
    ml_fitmodel[FIT_MODEL_LINE].index = FIT_MODEL_LINE + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_POLY2].name, _T("Polynomial 2"));
    _tcscpy(ml_fitmodel[FIT_MODEL_POLY2].function, _T("a + b*x + c*x*x"));
    ml_fitmodel[FIT_MODEL_POLY2].parcount = 3;
    ml_fitmodel[FIT_MODEL_POLY2].index = FIT_MODEL_POLY2 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_POLY3].name, _T("Polynomial 3"));
    _tcscpy(ml_fitmodel[FIT_MODEL_POLY3].function, _T("a + b*x + c*x*x + d*x*x*x"));
    ml_fitmodel[FIT_MODEL_POLY3].parcount = 4;
    ml_fitmodel[FIT_MODEL_POLY3].index = FIT_MODEL_POLY3 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_POLY4].name, _T("Polynomial 4"));
    _tcscpy(ml_fitmodel[FIT_MODEL_POLY4].function, _T("a + b*x + c*x*x + d*x*x*x + e*x*x*x*x"));
    ml_fitmodel[FIT_MODEL_POLY4].parcount = 5;
    ml_fitmodel[FIT_MODEL_POLY4].index = FIT_MODEL_POLY4 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS1].name, _T("Gaussian"));
    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS1].function, _T("a + (b*exp(-0.5*((x-d)/c)*((x-d)/c))"));
    ml_fitmodel[FIT_MODEL_GAUSS1].parcount = 4;
    ml_fitmodel[FIT_MODEL_GAUSS1].index = FIT_MODEL_GAUSS1 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS2].name, _T("Gaussian 2"));
    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS2].function, _T("a + (b*exp(-0.5*((x-d)/c)*((x-d)/c))"));
    ml_fitmodel[FIT_MODEL_GAUSS2].parcount = 7;
    ml_fitmodel[FIT_MODEL_GAUSS2].index = FIT_MODEL_GAUSS2 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS3].name, _T("Gaussian 3"));
    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS3].function, _T("a + (b*exp(-0.5*((x-d)/c)*((x-d)/c))"));
    ml_fitmodel[FIT_MODEL_GAUSS3].parcount = 10;
    ml_fitmodel[FIT_MODEL_GAUSS3].index = FIT_MODEL_GAUSS3 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS4].name, _T("Gaussian 4"));
    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS4].function, _T("a + (b*exp(-0.5*((x-d)/c)*((x-d)/c))"));
    ml_fitmodel[FIT_MODEL_GAUSS4].parcount = 13;
    ml_fitmodel[FIT_MODEL_GAUSS4].index = FIT_MODEL_GAUSS4 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS5].name, _T("Gaussian 5"));
    _tcscpy(ml_fitmodel[FIT_MODEL_GAUSS5].function, _T("a + (b*exp(-0.5*((x-d)/c)*((x-d)/c))"));
    ml_fitmodel[FIT_MODEL_GAUSS5].parcount = 16;
    ml_fitmodel[FIT_MODEL_GAUSS5].index = FIT_MODEL_GAUSS5 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ1].name, _T("Lorentzian"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ1].function, _T("a + b/(1.0 + (4.0*(x-d)*(x-d)/c*c))"));
    ml_fitmodel[FIT_MODEL_LORENTZ1].parcount = 4;
    ml_fitmodel[FIT_MODEL_LORENTZ1].index = FIT_MODEL_LORENTZ1 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ2].name, _T("Lorentzian 2"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ2].function, _T("a + b/(1.0 + (4.0*(x-d)*(x-d)/c*c))"));
    ml_fitmodel[FIT_MODEL_LORENTZ2].parcount = 7;
    ml_fitmodel[FIT_MODEL_LORENTZ2].index = FIT_MODEL_LORENTZ2 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ3].name, _T("Lorentzian 3"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ3].function, _T("a + b/(1.0 + (4.0*(x-d)*(x-d)/c*c))"));
    ml_fitmodel[FIT_MODEL_LORENTZ3].parcount = 10;
    ml_fitmodel[FIT_MODEL_LORENTZ3].index = FIT_MODEL_LORENTZ3 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ4].name, _T("Lorentzian 4"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ4].function, _T("a + b/(1.0 + (4.0*(x-d)*(x-d)/c*c))"));
    ml_fitmodel[FIT_MODEL_LORENTZ4].parcount = 13;
    ml_fitmodel[FIT_MODEL_LORENTZ4].index = FIT_MODEL_LORENTZ4 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ5].name, _T("Lorentzian 5"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LORENTZ5].function, _T("a + b/(1.0 + (4.0*(x-d)*(x-d)/c*c))"));
    ml_fitmodel[FIT_MODEL_LORENTZ5].parcount = 16;
    ml_fitmodel[FIT_MODEL_LORENTZ5].index = FIT_MODEL_LORENTZ5 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_PEARSON].name, _T("Pearson VII"));
    _tcscpy(ml_fitmodel[FIT_MODEL_PEARSON].function, _T("a + b/pow((1.0 + 4.0*(pow(2.0, 1/e)-1.0)*((x-d)/c)*((x-d)/c)),e)"));
    ml_fitmodel[FIT_MODEL_PEARSON].parcount = 5;
    ml_fitmodel[FIT_MODEL_PEARSON].index = FIT_MODEL_PEARSON + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_EXPGR].name, _T("Exponential"));
    _tcscpy(ml_fitmodel[FIT_MODEL_EXPGR].function, _T("a + b*exp(c*x)"));
    ml_fitmodel[FIT_MODEL_EXPGR].parcount = 3;
    ml_fitmodel[FIT_MODEL_EXPGR].index = FIT_MODEL_EXPGR + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_EXPGR2].name, _T("Exponential 2"));
    _tcscpy(ml_fitmodel[FIT_MODEL_EXPGR2].function, _T("a + b*exp(c*x) + d*exp(e*x)"));
    ml_fitmodel[FIT_MODEL_EXPGR2].parcount = 5;
    ml_fitmodel[FIT_MODEL_EXPGR2].index = FIT_MODEL_EXPGR2 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_EXPDEC].name, _T("Exponential Decay"));
    _tcscpy(ml_fitmodel[FIT_MODEL_EXPDEC].function, _T("a + b*exp(-c*x)"));
    ml_fitmodel[FIT_MODEL_EXPDEC].parcount = 3;
    ml_fitmodel[FIT_MODEL_EXPDEC].index = FIT_MODEL_EXPDEC + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_EXPDEC2].name, _T("Exponential Decay 2"));
    _tcscpy(ml_fitmodel[FIT_MODEL_EXPDEC2].function, _T("a + b*exp(-c*x) + d*exp(-e*x)"));
    ml_fitmodel[FIT_MODEL_EXPDEC2].parcount = 5;
    ml_fitmodel[FIT_MODEL_EXPDEC2].index = FIT_MODEL_EXPDEC2 + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_HYPERBOLA].name, _T("Hyperbola"));
    _tcscpy(ml_fitmodel[FIT_MODEL_HYPERBOLA].function, _T("a + b*x/(c + d*x)"));
    ml_fitmodel[FIT_MODEL_HYPERBOLA].parcount = 4;
    ml_fitmodel[FIT_MODEL_HYPERBOLA].index = FIT_MODEL_HYPERBOLA + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_LOG].name, _T("Logarithm"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LOG].function, _T("a + b*log(c + d*x)"));
    ml_fitmodel[FIT_MODEL_LOG].parcount = 4;
    ml_fitmodel[FIT_MODEL_LOG].index = FIT_MODEL_LOG + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_POWER].name, _T("Power"));
    _tcscpy(ml_fitmodel[FIT_MODEL_POWER].function, _T("a + b*pow(x,c)"));
    ml_fitmodel[FIT_MODEL_POWER].parcount = 3;
    ml_fitmodel[FIT_MODEL_POWER].index = FIT_MODEL_POWER + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_LOGISTIC].name, _T("Logistic"));
    _tcscpy(ml_fitmodel[FIT_MODEL_LOGISTIC].function, _T("a + b/(1.0 + pow(x/c,d))"));
    ml_fitmodel[FIT_MODEL_LOGISTIC].parcount = 4;
    ml_fitmodel[FIT_MODEL_LOGISTIC].index = FIT_MODEL_LOGISTIC + 1;

    _tcscpy(ml_fitmodel[FIT_MODEL_WAVE].name, _T("Wave"));
    _tcscpy(ml_fitmodel[FIT_MODEL_WAVE].function, _T("a + b*sin(pi*(x-c)/d)"));
    ml_fitmodel[FIT_MODEL_WAVE].parcount = 4;
    ml_fitmodel[FIT_MODEL_WAVE].index = FIT_MODEL_WAVE + 1;

    for (ii = 0; ii < ML_FIT_MAXMODEL; ii++) {
        memset(ml_fitmodel[ii].par, 0, ML_STRSIZES);
        _tcsprintf(ml_fitmodel[ii].par, (ML_STRSIZES - 1), _T("%d"),  ml_fitmodel[ii].parcount);
        // Paramètres par défaut
        for (ll = 0; ll < ml_fitmodel[ii].parcount; ll++) {
            ml_fitmodel[ii].fpar[ll] = 1.0;
            ml_fitmodel[ii].mpar[ll] = 1;
        }
        ml_fitmodel[ii].iter = *iter;
        ml_fitmodel[ii].lambda = *lambda;
        //
    }
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_getmodel    :    Copie de modèles de fit

        pfit           :    Tableau de modèles à initialiser (pointeur)
        fitcount       :    Nombre d'éléments du tableau de modèles
    -------------------------------------------------------------------- */
void ml_fit_getmodel(fit_t *pfit, int_t fitcount)
{
    int_t ii;

    if ((pfit == NULL) || (fitcount < 1) || (fitcount > ML_FIT_MAXMODEL)) {
        return;
    }

    for (ii = 0; ii < fitcount; ii++) {
        ml_fit_dup(&(pfit[ii]), (const fit_t*)(&(ml_fitmodel[ii])));
    }
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_getmodel    :    Copie d'un modèle de fit

    pfit               :    Modèles à initialiser (pointeur)
    pszModel           :    Nom du modèle
    -------------------------------------------------------------------- */
int_t ml_fit_initmodel(fit_t *pfit, const char_t *pszModel)
{
    int_t ii;

    for (ii = 0; ii < ML_FIT_MAXMODEL; ii++) {
        if (_tcsicmp(pszModel, (const char_t*) (ml_fitmodel[ii].name)) == 0) {
            ml_fit_dup(pfit, (const fit_t*) (&(ml_fitmodel[ii])));
            return ii;
        }
    }

    return -1;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_dup         :    Dupliquer un fit

        pTo            :    pointeur sur la structure source
        pFrom          :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_fit_dup(fit_t *pTo, const fit_t *pFrom)
{
    int_t ii;

    if ((pTo == NULL) || (pFrom == NULL)) {
        return -1;
    }

    _tcscpy(pTo->function, (const char_t*)(pFrom->function));
    _tcscpy(pTo->name, (const char_t*)(pFrom->name));
    _tcscpy(pTo->label, (const char_t*)(pFrom->label));
    _tcscpy(pTo->par, (const char_t*)(pFrom->par));
    pTo->index = pFrom->index;
    pTo->id = pFrom->id;
    pTo->iter = pFrom->iter;
    pTo->lambda = pFrom->lambda;
    pTo->chi = pFrom->chi;
    pTo->parcount = pFrom->parcount;
    pTo->status = pFrom->status;
    for (ii = 0; ii < ML_FIT_MAXPAR; ii++) {
        pTo->fpar[ii] = pFrom->fpar[ii];
        pTo->mpar[ii] = pFrom->mpar[ii];
        pTo->resd[ii] = pFrom->resd[ii];
        pTo->fopt[ii] = pFrom->fopt[ii];
    }
    pTo->points = pFrom->points;

    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_fromlegacy    :    Dupliquer un fit

    pTo                  :    pointeur sur la structure source
    pFrom                :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_fit_fromlegacy(fit_t *pTo, const fit_legacy_t *pFrom)
{
    int_t ii;

    if ((pTo == NULL) || (pFrom == NULL)) {
        return -1;
    }

    _tcscpy(pTo->function, (const char_t*) (pFrom->function));
    _tcscpy(pTo->name, (const char_t*) (pFrom->name));
    _tcscpy(pTo->label, (const char_t*) (pFrom->label));
    _tcscpy(pTo->par, (const char_t*) (pFrom->par));
    int ifun = pFrom->index;
    if (ifun <= 5) {
        pTo->index = ifun;
    }
    else if (ifun == 6) {
        pTo->index = ifun + 4;
    }
    else {
        pTo->index = ifun + 8;
    }
    pTo->id = pFrom->id;
    pTo->iter = pFrom->iter;
    pTo->lambda = pFrom->lambda;
    pTo->chi = pFrom->chi;
    pTo->parcount = pFrom->parcount;
    pTo->status = pFrom->status;
    for (ii = 0; ii < ML_FIT_MAXPAR_LEGACY; ii++) {
        pTo->fpar[ii] = pFrom->fpar[ii];
        pTo->mpar[ii] = pFrom->mpar[ii];
        pTo->resd[ii] = pFrom->resd[ii];
        pTo->fopt[ii] = pFrom->fopt[ii];
    }
    for (ii = ML_FIT_MAXPAR_LEGACY; ii < ML_FIT_MAXPAR; ii++) {
        pTo->fpar[ii] = 0.0;
        pTo->mpar[ii] = 0;
        pTo->resd[ii] = 0.0;
        pTo->fopt[ii] = 0.0;
    }
    pTo->points = pFrom->points;

    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_tolegacy    :    Dupliquer un fit

    pTo                :    pointeur sur la structure source
    pFrom              :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_fit_tolegacy(fit_legacy_t *pTo, const fit_t *pFrom)
{
    int_t ii;

    if ((pTo == NULL) || (pFrom == NULL)) {
        return -1;
    }

    _tcscpy(pTo->function, (const char_t*) (pFrom->function));
    _tcscpy(pTo->name, (const char_t*) (pFrom->name));
    _tcscpy(pTo->label, (const char_t*) (pFrom->label));
    _tcscpy(pTo->par, (const char_t*) (pFrom->par));
    int ifun = pFrom->index;
    if (ifun <= 5) {
        pTo->index = ifun;
    }
    else if (ifun == (FIT_MODEL_LORENTZ1 + 1)) {
        pTo->index = ifun - 4;
    }
    else if (ifun >= (FIT_MODEL_PEARSON + 1)) {
        pTo->index = ifun - 8;
    }
    else {
        pTo->index = 0;
    }
    pTo->id = pFrom->id;
    pTo->iter = pFrom->iter;
    pTo->lambda = pFrom->lambda;
    pTo->chi = pFrom->chi;
    if (pFrom->parcount > ML_FIT_MAXPAR_LEGACY) {
        pTo->parcount = ML_FIT_MAXPAR_LEGACY;
    }
    else {
        pTo->parcount = pFrom->parcount;
    }
    pTo->status = pFrom->status;
    for (ii = 0; ii < ML_FIT_MAXPAR_LEGACY; ii++) {
        pTo->fpar[ii] = pFrom->fpar[ii];
        pTo->mpar[ii] = pFrom->mpar[ii];
        pTo->resd[ii] = pFrom->resd[ii];
        pTo->fopt[ii] = pFrom->fopt[ii];
    }
    pTo->points = pFrom->points;

    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_autopar          :    Estimer les paramètres de fit

        ifun = type de fonction

        pvecX               :    Vecteur de données x
        pvecY               :    Vecteur de données y
        fpar                :    Paramètres qui seront estimés

        Retourne 1 si une estimation a été trouvée, et 0 sinon.
    -------------------------------------------------------------------- */
int_t ml_fit_autopar(vector_t *pvecX, vector_t *pvecY, int_t ifun, real_t *fpar, libvar_t *pLib)
{
    int ii, iflag = 0, ipeaknum, ivis = 0;
    real_t ampl, *foffset = NULL, *fampl = NULL, *fpos = NULL, *fwidth = NULL;

    if ((pvecX == NULL) || (pvecY == NULL)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        return -1;
    }

    if ((pvecX->dat == NULL) || (pvecY->dat == NULL)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        return -1;
    }

    // Il faut au moins 12 points pour le traitement
    if ((pvecX->dim < 12) || (pvecX->dim >= ML_MAXPOINTS)) {
        _tcsprintf(pLib->message, ML_STRSIZEN - 1,
            (pvecX->dim < 12) ? _T("Insufficient number of points (%d, min: %d).") : _T("High number of points (%d, max: %d)."),
            (pvecX->dim < 12) ? 12 : ML_MAXPOINTS);
        return -1;
    }
    if (pvecY->dim != pvecX->dim) {
        _tcsprintf(pLib->message, ML_STRSIZEN - 1,
            _T("Inconsistent X,Y dimensions (%d ; %d)."),
            pvecX->dim, pvecY->dim);
        return -1;
    }

    for (int ii = pvecX->istart; ii <= pvecX->istop; ii++) {
        if (pvecX->mask[ii] != 0xFF) {
            break;
        }
        pvecX->istart++;
    }
    for (int ii = pvecX->istop; ii >= 0; ii--) {
        if (pvecX->mask[ii] != 0xFF) {
            break;
        }
        pvecX->istop--;
    }

    if ((pvecX->istart < 0) || (pvecX->istart >= pvecX->dim)
        || (pvecX->istop < 0) || (pvecX->istop >= pvecX->dim)
        || (pvecX->istop != pvecY->istop)
        || (pvecX->istart != pvecY->istart)
        || ((pvecX->istop - pvecX->istart) < 12)) {
        _tcsprintf(pLib->message, ML_STRSIZEN - 1,
            _T("Insufficient number of points (%d, min: 12)."),
            pvecX->istop - pvecX->istart + 1);
        return -1;
    }

    foffset = (real_t*)malloc(pvecX->dim * sizeof(real_t));
    if (foffset == NULL) { return 0; }
    fampl = (real_t*)malloc(pvecX->dim * sizeof(real_t));
    if (fampl == NULL) { free(foffset); return 0; }
    fpos = (real_t*)malloc(pvecX->dim * sizeof(real_t));
    if (fpos == NULL) { free(foffset); free(fampl); return 0; }
    fwidth = (real_t*)malloc(pvecX->dim * sizeof(real_t));
    if (fwidth == NULL) { free(foffset); free(fampl);  free(fpos); return 0; }

    // Pics
    if ((ifun == FIT_MODEL_GAUSS1) || (ifun == FIT_MODEL_GAUSS2) || (ifun == FIT_MODEL_GAUSS3)
        || (ifun == FIT_MODEL_GAUSS4) || (ifun == FIT_MODEL_GAUSS5)
        || (ifun == FIT_MODEL_LORENTZ1) || (ifun == FIT_MODEL_LORENTZ2) || (ifun == FIT_MODEL_LORENTZ3)
        || (ifun == FIT_MODEL_LORENTZ4) || (ifun == FIT_MODEL_LORENTZ5)
        || (ifun == FIT_MODEL_PEARSON)) {

        pLib->errcode = 0;

        switch (ifun) {
            case FIT_MODEL_GAUSS1:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 1, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];
                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_GAUSS2:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 2, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_GAUSS3:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 3, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[7] = fampl[2];
                fpar[8] = fwidth[2];
                fpar[9] = fpos[2];

                fpar[8] = 10.0 * fabs(fpar[8]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_GAUSS4:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 4, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[7] = fampl[2];
                fpar[8] = fwidth[2];
                fpar[9] = fpos[2];

                fpar[8] = 10.0 * fabs(fpar[8]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[10] = fampl[3];
                fpar[11] = fwidth[3];
                fpar[12] = fpos[3];

                fpar[11] = 10.0 * fabs(fpar[11]);       // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_GAUSS5:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 5, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[7] = fampl[2];
                fpar[8] = fwidth[2];
                fpar[9] = fpos[2];

                fpar[8] = 10.0 * fabs(fpar[8]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[10] = fampl[3];
                fpar[11] = fwidth[3];
                fpar[12] = fpos[3];

                fpar[11] = 10.0 * fabs(fpar[11]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[13] = fampl[4];
                fpar[14] = fwidth[4];
                fpar[15] = fpos[4];

                fpar[14] = 10.0 * fabs(fpar[15]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_LORENTZ1:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 1, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_LORENTZ2:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 2, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];
                
                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_LORENTZ3:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 3, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }
                fpar[0] = foffset[0];
                
                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[7] = fampl[2];
                fpar[8] = fwidth[2];
                fpar[9] = fpos[2];

                fpar[8] = 10.0 * fabs(fpar[8]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_LORENTZ4:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 4, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[7] = fampl[2];
                fpar[8] = fwidth[2];
                fpar[9] = fpos[2];

                fpar[8] = 10.0 * fabs(fpar[8]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[10] = fampl[3];
                fpar[11] = fwidth[3];
                fpar[12] = fpos[3];

                fpar[11] = 10.0 * fabs(fpar[11]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_LORENTZ5:

                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 5, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];

                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[4] = fampl[1];
                fpar[5] = fwidth[1];
                fpar[6] = fpos[1];

                fpar[5] = 10.0 * fabs(fpar[5]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[7] = fampl[2];
                fpar[8] = fwidth[2];
                fpar[9] = fpos[2];

                fpar[8] = 10.0 * fabs(fpar[8]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[10] = fampl[3];
                fpar[11] = fwidth[3];
                fpar[12] = fpos[3];

                fpar[11] = 10.0 * fabs(fpar[11]);        // Un facteur 10 pour la FWHM par rapport au pas

                fpar[13] = fampl[4];
                fpar[14] = fwidth[4];
                fpar[15] = fpos[4];

                fpar[14] = 10.0 * fabs(fpar[15]);        // Un facteur 10 pour la FWHM par rapport au pas

                break;

            case FIT_MODEL_PEARSON:
                ipeaknum = ml_fit_findpeaks(pvecX, pvecY, 1, foffset, fampl, fpos, fwidth, pLib);
                if ((pLib->errcode != 0) || (ipeaknum < 1)) {
                    free(foffset); free(fampl);  free(fpos);  free(fwidth);
                    _tcscpy(pLib->message, _T("Unable to find peaks"));
                    return -1;
                }

                fpar[0] = foffset[0];

                fpar[1] = fampl[0];
                fpar[2] = fwidth[0];
                fpar[3] = fpos[0];
                fpar[2] = 10.0 * fabs(fpar[2]);        // Un facteur 10 pour la FWHM par rapport au pas
                fpar[1] = fampl[0] - fpar[0];
                fpar[4] = 1.0;
                break;
        }

        iflag = 1;
    }
    else {
        for (ii = pvecX->istart; ii <= pvecX->istop; ii++) {
            // ------------------------------------------------------------------------------------
            if (ifun == FIT_MODEL_EXPGR) {                   // Exp. Growth
                if (ii == pvecX->istart) {
                    fpar[0] = pvecY->dat[pvecX->istart];     // a
                    ampl = pvecY->dat[pvecX->istart];        // f
                }
                else {
                    if (pvecY->dat[ii] > ampl) {
                        ampl = pvecY->dat[ii];
                        iflag = 1;
                    }
                    if ((pvecY->dat[ii] > pvecY->dat[0]) && (pvecY->dat[ii-1] > pvecY->dat[0])
                        && (pvecY->dat[ii] > pvecY->dat[ii-1]) && (pvecX->dat[ii] > pvecX->dat[ii-1])) {
                        fpar[2] = log((pvecY->dat[ii] - pvecY->dat[0]) / (pvecY->dat[ii-1] - pvecY->dat[0])) / (pvecX->dat[ii] - pvecX->dat[ii-1]);
                    }
                }
            }
            else if (ifun == FIT_MODEL_EXPGR2) {             // Exp. Growth
                if (ii == pvecX->istart) {
                    fpar[0] = pvecY->dat[pvecX->istart];     // a
                    ampl = pvecY->dat[pvecX->istart];        // f
                }
                else {
                    if (pvecY->dat[ii] > ampl) {
                        ampl = pvecY->dat[ii];
                        iflag = 1;
                    }
                    if ((pvecY->dat[ii] > pvecY->dat[0]) && (pvecY->dat[ii-1] > pvecY->dat[0]) && (pvecY->dat[ii] > pvecY->dat[ii-1]) && (pvecX->dat[ii] > pvecX->dat[ii-1])) {
                        fpar[2] = log((pvecY->dat[ii] - pvecY->dat[0]) / (pvecY->dat[ii-1] - pvecY->dat[0])) / (pvecX->dat[ii] - pvecX->dat[ii-1]);
                        fpar[4] = fpar[2];
                    }
                }
            }
            else if (ifun == FIT_MODEL_EXPDEC) {            // Exp. Decay
                if (ii == pvecX->istart) {
                    fpar[0] = pvecY->dat[pvecX->istart];    // a
                    ampl = pvecY->dat[pvecX->istart];       // f
                }
                else {
                    if (pvecY->dat[ii] < ampl) {
                        ampl = pvecY->dat[ii];
                        iflag = 1;
                    }
                    if ((pvecY->dat[ii] < pvecY->dat[0]) && (pvecY->dat[ii-1] < pvecY->dat[0]) && (pvecY->dat[ii] < pvecY->dat[ii-1]) && (pvecX->dat[ii] > pvecX->dat[ii-1])) {
                        fpar[2] = log((pvecY->dat[ii] - pvecY->dat[0]) / (pvecY->dat[ii-1] - pvecY->dat[0])) / (pvecX->dat[ii] - pvecX->dat[ii-1]);
                    }
                }
            }
            else if (ifun == FIT_MODEL_EXPDEC2) {            // Exp. Decay
                if (ii == pvecX->istart) {
                    fpar[0] = pvecY->dat[pvecX->istart];     // a
                    ampl = pvecY->dat[pvecX->istart];        // f
                }
                else {
                    if (pvecY->dat[ii] < ampl) {
                        ampl = pvecY->dat[ii];
                        iflag = 1;
                    }
                    if ((pvecY->dat[ii] < pvecY->dat[0]) && (pvecY->dat[ii-1] < pvecY->dat[0]) && (pvecY->dat[ii] < pvecY->dat[ii-1]) && (pvecX->dat[ii] > pvecX->dat[ii-1])) {
                        fpar[2] = log((pvecY->dat[ii] - pvecY->dat[0]) / (pvecY->dat[ii-1] - pvecY->dat[0])) / (pvecX->dat[ii] - pvecX->dat[ii-1]);
                        fpar[4] = fpar[2];
                    }
                }
            }
        }
        if (ifun == FIT_MODEL_EXPGR) {
            fpar[1] = (ampl - fpar[0]) / (exp(fpar[2] * pvecX->dat[pvecX->istop]));
        }
        else if (ifun == FIT_MODEL_EXPGR2) {
            fpar[1] = (ampl - fpar[0]) / (exp(fpar[2] * pvecX->dat[pvecX->istop]));
            fpar[3] = fpar[1];
        }
        else if (ifun == FIT_MODEL_EXPDEC) {
            fpar[1] = (fpar[0] - ampl) / (exp(-fpar[2] * pvecX->dat[pvecX->istop]));
            fpar[0] = ampl;
        }
        else if (ifun == FIT_MODEL_EXPDEC2) {
            fpar[1] = (fpar[0] - ampl) / (exp(-fpar[2] * pvecX->dat[pvecX->istop]));
            fpar[0] = ampl;
            fpar[3] = fpar[1];
            fpar[2] = fpar[0];
        }
    }

    free(foffset); free(fampl);  free(fpos);  free(fwidth);
    return iflag;
}

static int calcFilterCoeffs(int nl, int nr, int idegree, double *pCoeffs, libvar_t *pLib)
{
    double **ppMat = NULL, *pB = NULL, *pX = NULL;
    double fSum = 0.0;
    int ii, jj, ll, nn;

    if (pCoeffs == NULL) {
        return -1;
    }

    if ((nl < 1) || (nr < 1) || ((nl + nr) < idegree)) {
        return -1;
    }

    ppMat = (double**) malloc((idegree + 1) * sizeof(double*));
    if (ppMat == NULL) {
        return -1;
    }
    for (ii = 0; ii <= idegree; ii++) {
        ppMat[ii] = (double*) malloc((idegree + 1) * sizeof(double));
        if (ppMat[ii] == NULL) {
            if (ii > 0) {
                for (jj = 0; jj < ii; jj++) {
                    free(ppMat[jj]);
                    ppMat[jj] = NULL;
                }
            }
            free(ppMat);
            ppMat = NULL;
            return -1;
        }
    }
    pB = (double*) malloc((idegree + 1) * sizeof(double));
    if (pB == NULL) {
        for (jj = 0; jj <= idegree; jj++) {
            free(ppMat[jj]);
            ppMat[jj] = NULL;
        }
        free(ppMat);
        ppMat = NULL;
        return -1;
    }
    pX = (double*) malloc((idegree + 1) * sizeof(double));
    if (pX == NULL) {
        for (jj = 0; jj <= idegree; jj++) {
            free(ppMat[jj]);
            ppMat[jj] = NULL;
        }
        free(ppMat);
        ppMat = NULL;
        free(pB);
        pB = NULL;
        return -1;
    }

    for (ii = 0; ii <= idegree; ii++) {
        for (jj = 0; jj <= idegree; jj++) {
            fSum = ((ii == 0) && (jj == 0)) ? 1.0 : 0.0;
            for (ll = 1; ll <= nr; ll++) {
                fSum += pow((double) ll, (double) (ii + jj));
            }
            for (ll = 1; ll <= nl; ll++) {
                fSum += pow((double) (-ll), (double) (ii + jj));
            }
            ppMat[ii][jj] = fSum;
        }
        pB[ii] = 0.0;
        pX[ii] = 0.0;
    }
    pB[0] = 1.0;

    ml_linsys_gauss(ppMat, pB, pX, idegree + 1, pLib);

    for (nn = -nl; nn <= nr; nn++) {
        fSum = pX[0];
        for (ll = 1; ll <= idegree; ll++) {
            fSum += pX[ll] * pow((double) nn, (double) ll);
        }
        pCoeffs[nn + nl] = fSum;
    }

    for (jj = 0; jj <= idegree; jj++) {
        free(ppMat[jj]);
        ppMat[jj] = NULL;
    }
    free(ppMat);
    ppMat = NULL;
    free(pB);
    pB = NULL;
    free(pX);
    pX = NULL;
    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_process        :    Ligne de base et lissage

        Entrées:
        pvecX             :    Vecteur de données (abscisses)
        pvecY             :    Vecteur de données (ordonnées)

        pxm               :    Mask des données (abscisses)
        pym               :    Mask des données (ordonnées)
        fmin              :    Minimum pour le mask
        fmax              :    Maximum pour le mask

        fbaseline0        :    Ligne de base y = fbaseline0 + fbaseline1 * x
        fbaseline1        :    Ligne de base y = fbaseline0 + fbaseline1 * x
        bSmooth           :    Lissage (=1 si lissage)
        nSmooth           :    Degré du lissage

        Retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_fit_process(vector_t *pvecX, vector_t *pvecY,
                     byte_t bMask, real_t fMin, real_t fMax,
                     byte_t bBaseline, real_t fbaseline0, real_t fbaseline1,
                     byte_t bSmooth, int_t nSmooth, libvar_t *pLib)
{
    int ii, jj, istart = -1, istop = -1, ii0, iin;
    const int pointsmin = (bSmooth == 0) ? 8 : 12;

    if ((bMask == 0) && (bBaseline == 0) && (bSmooth == 0)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        return -1;
    }

    if ((pvecX == NULL) || (pvecY == NULL)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        return -1;
    }

    if ((pvecX->dat == NULL) || (pvecY->dat == NULL)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        return -1;
    }

    // Il faut au moins pointsmin points pour le traitement
    if ((pvecX->dim < pointsmin) || (pvecX->dim >= ML_MAXPOINTS)) {
        _tcsprintf(pLib->message, ML_STRSIZEN - 1,
            (pvecX->dim < pointsmin) ? _T("Insufficient number of points (%d, min: %d).") : _T("Number of points exceeds the limit (%d, max: %d)."),
            pvecX->dim, (pvecX->dim < pointsmin) ? pointsmin : ML_MAXPOINTS);
        return -1;
    }
    if (pvecY->dim != pvecX->dim) {
        _tcsprintf(pLib->message, ML_STRSIZEN - 1,
            _T("Inconsistent X,Y dimensions (%d ; %d)."),
            pvecX->dim, pvecY->dim);
        return -1;
    }

    ii0 = 0;
    iin = pvecX->dim - 1;
    for (int ii = 0; ii < pvecX->dim; ii++) {
        if (pvecX->mask[ii] != 0xFF) {
            break;
        }
        ii0++;
    }
    for (int ii = pvecX->dim - 1; ii >= 0; ii--) {
        if (pvecX->mask[ii] != 0xFF) {
            break;
        }
        iin--;
    }
    if (pvecX->istart < ii0) {
        pvecX->istart = ii0;
    }
    if (pvecX->istop > iin) {
        pvecX->istop = iin;
    }

    // Il faut au moins pointsmin points pour le traitement
    if ((iin - ii0 + 1) < pointsmin) {
        _tcsprintf(pLib->message, ML_STRSIZEN - 1,
            _T("Insufficient number of points (%d, min: %d)."),
            iin - ii0 + 1, pointsmin);
        return -1;
    }

    if (bMask == 1) {
        // Les données X sont supposées être ordonnées
        if ((fMin >= fMax) || (fMin < pvecX->dat[ii0]) || (fMax > pvecX->dat[iin])
            || ((fMax - fMin) < (0.01 * (pvecX->dat[iin] - pvecX->dat[ii0])))) {
            bMask = 0;
        }
        else if ((fMin <= pvecX->dat[ii0]) && (fMax >= pvecX->dat[iin])) {
            bMask = 0;
        }
        else {
            istart = -1;
            istop = -1;
        }
    }

    if (bSmooth == 1) {
        if ((nSmooth < 1) || (nSmooth > ((iin - ii0) / 5))) {
            bSmooth = 0;
        }
    }

    // 1. Ligne de base et mask
    if ((bMask == 1) || (bBaseline == 1)) {
        for (ii = ii0; ii <= iin; ii++) {
            if (pvecX->mask[ii] == 0xFF) {
                continue;
            }
            if (bBaseline == 1) {
                pvecY->dat[ii] -= (fbaseline0 + (fbaseline1 * pvecX->dat[ii]));
            }
            if (bMask == 1) {
                // Masquer tous les points en dehors de l'intervalle [fMin,fMax]
                if ((pvecX->dat[ii] < fMin) || (pvecX->dat[ii] > fMax)) {
                    pvecY->mask[ii] = (pvecY->mask[ii] & 0xF0) | 0x01;
                    if ((pvecX->dat[ii] > fMax) && (istop == -1)) {
                        istop = ii - 1;
                        if ((istop > istart) && (istop >= 0)) {
                            pvecX->istop = istop;
                            pvecY->istop = istop;
                        }
                    }
                }
                else {
                    pvecY->mask[ii] &= 0xF0;
                    if (istart == -1) {
                        istart = ii;
                        pvecX->istart = istart;
                        pvecY->istart = istart;
                    }
                }
            }
        }
    }

    // 2. Lissage
    if (bSmooth == 1) {
        real_t fT = 0.0;
        int_t ndat = (2 * nSmooth) + 1;
        real_t *pCoeffs = NULL;

        if (nSmooth == 1) {
            for (ii = (ii0 + nSmooth); ii < (iin + 1 - nSmooth); ii++) {
                if (pvecX->mask[ii] == 0xFF) {
                    continue;
                }
                fT = 0.0;
                for (jj = (ii - nSmooth); jj <= (ii + nSmooth); jj++) {
                    if (pvecX->mask[jj] == 0xFF) {
                        continue;
                    }
                    fT += pvecY->dat[jj];
                }
                pvecY->dat[ii] = fT / 3.0;
            }
            _tcscpy(pLib->message, _T("Done."));
            return 0;
        }

        pCoeffs = (real_t*) malloc(ndat * sizeof(real_t));
        if (pCoeffs == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            return -1;
        }
        if (calcFilterCoeffs(nSmooth, nSmooth, 3, pCoeffs, pLib) != 0) {
            free(pCoeffs);
            pCoeffs = NULL;
            return -1;
        }

        for (ii = ii0 + 1; ii < (ii0 + nSmooth); ii++) {
            if (pvecX->mask[ii] == 0xFF) {
                continue;
            }
            pvecY->dat[ii] = (pvecY->dat[ii - 1] + pvecY->dat[ii] + pvecY->dat[ii + 1]) / 3.0;
        }
        pvecY->dat[ii0] = 0.5 * (pvecY->dat[ii0] + pvecY->dat[ii0 + 1]);
        pvecY->dat[iin] = 0.5 * (pvecY->dat[iin - 1] + pvecY->dat[iin]);
        for (ii = (iin + 1 - nSmooth); ii < (iin + 1 - 1); ii++) {
            if (pvecX->mask[ii] == 0xFF) {
                continue;
            }
            pvecY->dat[ii] = (pvecY->dat[ii - 1] + pvecY->dat[ii] + pvecY->dat[ii + 1]) / 3.0;
        }

        for (ii = (ii0 + nSmooth); ii < (iin + 1 - nSmooth); ii++) {
            if (pvecX->mask[ii] == 0xFF) {
                continue;
            }
            fT = 0.0;
            for (jj = (ii - nSmooth); jj <= (ii + nSmooth); jj++) {
                if (pvecX->mask[jj] == 0xFF) {
                    continue;
                }
                fT += pvecY->dat[jj] * pCoeffs[jj - (ii - nSmooth)];
            }
            pvecY->dat[ii] = fT;
        }
        free(pCoeffs);
        pCoeffs = NULL;
    }

    _tcscpy(pLib->message, _T("Done."));
    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_fit_findpeaks    :    Recherche de tous les pics

        Entrées:
        pvecX           :    Vecteur de données x
        pvecY           :    Vecteur de données y
        peaknum         :    Nombre de pics à chercher

        Sorties:
        offset          :    contiendra les valeurs estimées des offsets des pics
                            (offset[0], offset[1], ...)
        ampl            :    contiendra les valeurs estimées des amplitudes des pics
                            (ampl[0], ampl[1], ...)
        pos             :    contiendra les valeurs estimées des positions des pics
                            (pos[0], pos[1], ...)
        width           :    contiendra les valeurs estimées des largeurs des pics
                            (width[0], width[1], ...)

        Retourne le nombre de pics trouvés
    -------------------------------------------------------------------- */
int_t ml_fit_findpeaks(vector_t *pvecX, vector_t *pvecY, int_t peaknum, real_t *offset,
                              real_t *ampl, real_t *pos, real_t *width, libvar_t *pLib)
{
    int_t ii, jj, nn, ipeaknum, nstart, nstop, iflag = 0;
    real_t *dy = NULL, dx, ftmp,
        *foffset = NULL, *fampl = NULL, *fpos = NULL, *fwidth = NULL;
    byte_t peakfound = 0, peakfoundbasic = 0, shoulderfoundp = 0, shoulderfoundn = 0;
    const int pointsmin = 8;

    if (pLib->errcode) { return 0; }

    pLib->errcode = 1;

    if ((pvecX == NULL) || (pvecY == NULL)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        return -1;
    }

    if ((pvecX->dat == NULL) || (pvecY->dat == NULL)) {
        _tcscpy(pLib->message, _T("Parameters not valid"));
        return -1;
    }

    // Il faut au moins pointsmin points pour le traitement
    if ((pvecX->dim < pointsmin) || (pvecX->dim >= ML_MAXPOINTS)) {
        return -1;
    }
    if (pvecY->dim != pvecX->dim) {
        return -1;
    }

    for (int ii = pvecX->istart; ii <= pvecX->istop; ii++) {
        if (pvecX->mask[ii] != 0xFF) {
            break;
        }
        pvecX->istart++;
    }
    for (int ii = pvecX->istop; ii >= 0; ii--) {
        if (pvecX->mask[ii] != 0xFF) {
            break;
        }
        pvecX->istop--;
    }

    if ((pvecX->istart < 0) || (pvecX->istart >= pvecX->dim)
        || (pvecX->istop < 0) || (pvecX->istop >= pvecX->dim)
        || (pvecX->istop != pvecY->istop)
        || (pvecX->istart != pvecY->istart)
        || ((pvecX->istop - pvecX->istart) < pointsmin)) {
        _tcscpy(pLib->message, _T("Invalid column mask"));
        return -1;
    }

    // Nombre de pic à chercher
    if ((peaknum < 1) || (peaknum > ML_MAXPOINTS)) {
        return 0;
    }

    foffset = (real_t*) malloc(pvecX->dim * sizeof(real_t));
    if (foffset == NULL) { return 0; }
    fampl = (real_t*) malloc(pvecX->dim * sizeof(real_t));
    if (fampl == NULL) { free(foffset); return 0; }
    fpos = (real_t*) malloc(pvecX->dim * sizeof(real_t));
    if (fpos == NULL) { free(foffset); free(fampl); return 0; }
    fwidth = (real_t*) malloc(pvecX->dim * sizeof(real_t));
    if (fwidth == NULL) { free(foffset); free(fampl); free(fpos); return 0; }
    dy = (real_t*) malloc(pvecX->dim * sizeof(real_t));
    if (dy == NULL) { free(foffset); free(fampl); free(fpos); free(fwidth); return 0; }

    nstart = pvecX->istart + 4;    nstop = pvecX->istop - 4;

    // Données ordonnées et uniformes : dx > 0 et constante
    ipeaknum = 0;
    iflag = 0;
    for (ii = nstart; ii < nstop; ii++) {
        if (pvecX->mask[ii] == 0xFF) {
            continue;
        }
        dx = 0.5 * (pvecX->dat[ii+1] - pvecX->dat[ii-1]);
        if (dx <= 0.0) {
            free(foffset); free(fampl); free(fpos); free(fwidth); free(dy);
            _tcsprintf(pLib->message, ML_STRSIZE - 1, _T("X-data not in ascending order [ at index %d ]"), ii + 1);
            return 0;
        }

        dy[ii-2] = (pvecY->dat[ii-4] - (8.0 * pvecY->dat[ii-3]) + (8.0 * pvecY->dat[ii-1]) - pvecY->dat[ii]) / (12.0 * dx);
        dy[ii-1] = (pvecY->dat[ii-3] - (8.0 * pvecY->dat[ii-2]) + (8.0 * pvecY->dat[ii]) - pvecY->dat[ii+1]) / (12.0 * dx);
        dy[ii] = (pvecY->dat[ii-2] - (8.0 * pvecY->dat[ii-1]) + (8.0 * pvecY->dat[ii+1]) - pvecY->dat[ii+2]) / (12.0 * dx);
        dy[ii+1] = (pvecY->dat[ii-1] - (8.0 * pvecY->dat[ii]) + (8.0 * pvecY->dat[ii+2]) - pvecY->dat[ii+3]) / (12.0 * dx);
        dy[ii+2] = (pvecY->dat[ii] - (8.0 * pvecY->dat[ii+1]) + (8.0 * pvecY->dat[ii+3]) - pvecY->dat[ii+4]) / (12.0 * dx);

        // Pic - Recherche avancée
        if (((dy[ii-2] > 0.0) && (dy[ii-1] > 0.0) && (dy[ii+1] < 0.0) && (dy[ii+2] < 0.0))
            && ((pvecY->dat[ii] > pvecY->dat[ii-1]) && (pvecY->dat[ii-1] > pvecY->dat[ii-2]) && (pvecY->dat[ii] > pvecY->dat[ii+1]) && (pvecY->dat[ii+1] > pvecY->dat[ii+2]))) {
            peakfound = 1;
        }
        else {
            peakfound = 0;
        }

        // Pic - Recherche basique
        if ((pvecY->dat[ii] > pvecY->dat[ii-1]) && (pvecY->dat[ii-1] > pvecY->dat[ii-2]) && (pvecY->dat[ii] > pvecY->dat[ii+1]) && (pvecY->dat[ii+1] > pvecY->dat[ii+2])) {
            peakfoundbasic = 1;
        }
        else {
            peakfoundbasic = 0;
        }

        // Epaulement croissant
        if ((dy[ii+2] > 0.0) && (dy[ii+1] > 0.0) && (dy[ii] > 0.0)
            && (dy[ii-1] > 0.0) && (dy[ii-2] > 0.0)
            && (dy[ii] < dy[ii-2]) && (dy[ii] < dy[ii-1])
            && (dy[ii] < dy[ii+1]) && (dy[ii] < dy[ii+2])
            && (dy[ii-1] < dy[ii-2]) && (dy[ii+1] < dy[ii+2])) {
            shoulderfoundp = 1;
        }
        else {
            shoulderfoundp = 0;
        }

        // Epaulement décroissant
        if ((dy[ii+2] < 0.0) && (dy[ii+1] < 0.0) && (dy[ii] < 0.0)
            && (dy[ii-1] < 0.0) && (dy[ii-2] < 0.0)
            && (dy[ii] > dy[ii-2]) && (dy[ii] > dy[ii-1])
            && (dy[ii] > dy[ii+1]) && (dy[ii] > dy[ii+2])
            && (dy[ii-1] > dy[ii-2]) && (dy[ii+1] > dy[ii+2])) {
            shoulderfoundn = 1;
        }
        else {
            shoulderfoundn = 0;
        }

        if (ii == nstart) {
            foffset[0] = pvecY->dat[pvecX->istart];
            fpos[0] = pvecX->dat[pvecX->istart];
            fampl[0] = pvecY->dat[pvecX->istart];
        }
        else if (peakfound || peakfoundbasic) {        // Pic
            foffset[ipeaknum] = pvecY->dat[pvecX->istart];
            fampl[ipeaknum] = pvecY->dat[ii];
            fwidth[ipeaknum] = pvecX->dat[ii] - pvecX->dat[ii-1];
            fpos[ipeaknum] = pvecX->dat[ii];
            ipeaknum += 1;
        }
        else if (shoulderfoundn) {                            // Epaulement décroissant
            foffset[ipeaknum] = pvecY->dat[pvecX->istart];
            fampl[ipeaknum] = pvecY->dat[ii];
            fwidth[ipeaknum] = pvecX->dat[ii] - pvecX->dat[ii-1];
            fpos[ipeaknum] = pvecX->dat[ii];
            ipeaknum += 1;
        }
        else if (shoulderfoundp) {                            // Epaulement croissant
            foffset[ipeaknum] = pvecY->dat[pvecX->istart];
            fampl[ipeaknum] = pvecY->dat[ii];
            fwidth[ipeaknum] = pvecX->dat[ii] - pvecX->dat[ii-1];
            fpos[ipeaknum] = pvecX->dat[ii];
            ipeaknum += 1;
        }
    }

    if (ipeaknum < 1) {
        free(foffset); free(fampl); free(fpos); free(fwidth); free(dy);
        _tcscpy(pLib->message, _T("Peaks not found"));
        return 0;
    }

    if (ipeaknum == 1) {
        offset[0] = foffset[0];
        ampl[0] = fampl[0];
        width[0] = fwidth[0];
        pos[0] = fpos[0];
        pLib->errcode = 0;
        free(foffset); free(fampl); free(fpos); free(fwidth); free(dy);
        return 1;
    }

    // Ordonner selon l'amplitude (décroissante)
    for (ii = 0; ii < ipeaknum - 1; ii++) {
        for (jj = (ii + 1); jj < ipeaknum; jj++) {
            if (fampl[jj] > fampl[ii]) {
                ftmp = fpos[ii];
                fpos[ii] = fpos[jj];
                fpos[jj] = ftmp;

                ftmp = fampl[ii];
                fampl[ii] = fampl[jj];
                fampl[jj] = ftmp;

                ftmp = fwidth[ii];
                fwidth[ii] = fwidth[jj];
                fwidth[jj] = ftmp;

                ftmp = foffset[ii];
                foffset[ii] = foffset[jj];
                foffset[jj] = ftmp;
            }
        }
    }

    if (peaknum == 1) {
        offset[0] = foffset[0];
        ampl[0] = fampl[0];
        width[0] = fwidth[0];
        pos[0] = fpos[0];
        pLib->errcode = 0;
        free(foffset); free(fampl); free(fpos); free(fwidth); free(dy);
        return 1;
    }

    if (ipeaknum > peaknum) {
        nn = peaknum;
    }
    else {
        nn = ipeaknum;
    }

    for (ii = 0; ii < nn; ii++) {
        pos[ii] = fpos[ii];
        ampl[ii] = fampl[ii];
        offset[ii] = foffset[ii];
        width[ii] = fwidth[ii];
    }

    // Ordonner selon la position
    for (ii = 0; ii < nn - 1; ii++) {
        for (jj = (ii + 1); jj < nn; jj++) {
            if (pos[jj] < pos[ii]) {
                ftmp = pos[ii];
                pos[ii] = pos[jj];
                pos[jj] = ftmp;

                ftmp = ampl[ii];
                ampl[ii] = ampl[jj];
                ampl[jj] = ftmp;

                ftmp = width[ii];
                width[ii] = width[jj];
                width[jj] = ftmp;

                ftmp = offset[ii];
                offset[ii] = offset[jj];
                offset[jj] = ftmp;
            }
        }
    }

    // If less peaks found than the expected ones, then complete the list
    if ((ipeaknum >= 1) && (ipeaknum < peaknum)) {
        for (jj = ipeaknum; jj < peaknum; jj++) {
            width[jj] = width[ipeaknum - 1];
            pos[jj] = pos[ipeaknum - 1];
            ampl[jj] = ampl[ipeaknum - 1];
            offset[jj] = offset[ipeaknum - 1];
        }
    }

    pLib->errcode = 0;
    free(foffset); free(fampl); free(fpos); free(fwidth); free(dy);
    return nn;
}