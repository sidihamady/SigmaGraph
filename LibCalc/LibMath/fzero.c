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
    :TODO: translate to english

    ml_fzguess    :    trouver un intervalle dans lequel f(x)=0

        fun       :    pointeur sur f(x)
        a         :    pointeur sur l'estimation initiale (inférieure)
        b         :    pointeur sur l'estimation initiale (supérieure)
        Retourne 1 si OK
    -------------------------------------------------------------------- */
int_t ml_fzguess(real_t (*fun)(real_t), real_t *a, real_t *b, libvar_t *pLib)
{
    int_t ii;
    real_t fa, fb;
    if (*a == *b) {
        return 0;
    }
    fa = (*fun)(*a);
    fb = (*fun)(*b);
    for (ii = 0; ii < pLib->maxiter; ii++) {
        if (fa * fb <= 0.0) { return 1; }
        if (fabs(fa) < fabs(fb)) {
            fa = (*fun)(*a += 1.6 * (*a - *b));
        }
        else {
            fb = (*fun)(*b += 1.6 * (*b - *a));
        }
    }
    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: translate to english

    ml_fzero        :    résolution de f(x) = 0 avec la méthode définie
                         par la variable pLib->solver

        fun        :    pointeur sur f(x)
        a          :    estimation initiale (inférieure)
        b          :    estimation initiale (supérieure)
        Retourne la valeur estimée de la solution
    -------------------------------------------------------------------- */
real_t ml_fzero(real_t (*fun)(real_t), real_t a, real_t b, libvar_t *pLib)
{
    return ml_amsterdam(fun, a, b, pLib);
}

/*  --------------------------------------------------------------------
    :TODO: translate to english

    ml_brent            :    résolution de f(x) = 0 par l'algorithme de Brent
    
        fun             :    pointeur sur f(x)
        a, b            :    intervalle contenant la solution

        Retourne la valeur estimée de la solution
    -------------------------------------------------------------------- */
real_t ml_brent(real_t (*fun)(real_t), real_t a, real_t b, libvar_t *pLib)
{
    int_t i;
    real_t c = b, d, e, min1, min2, fa = (*fun)(a), fb = (*fun)(b),
        fc, p, q, r, s, xm, tol = pLib->tol;

    if (pLib->errcode) { return 0.0; }

    pLib->errcode = 1;

    if (fa == 0.0) { pLib->errcode = 0; return fa; }
    if (fb == 0.0) { pLib->errcode = 0; return fb; }
    if (((fa > 0.0) && (fb > 0.0)) || ((fa < 0.0) && (fb < 0.0))) {
        return 0.0;
    }

    if ((tol <= 0.0) || (tol > 1.0)) {
        return 0.0;
    }

    fc = fb;

    for (i = 0; i < pLib->maxiter; i++) {
        if (((fb > 0.0) && (fc > 0.0)) || ((fb < 0.0) && (fc < 0.0))) {
            c = a;
            fc = fa;
            e = d = b - a;
        }
        if (fabs(fc) < fabs(fb)) {
            a = b; b = c; c = a;
            fa = fb; fb = fc; fc = fa;
        }

        xm = 0.5 * (c - b);

        if ((fabs(xm) <= tol) || (fb == 0.0)) {
            pLib->errcode = 0;
            return b;
        }

        if ((fabs(e) >= tol) && (fabs(fa) > fabs(fb))) {
            s = fb / fa;
            if (a == c) {
                p = 2.0 * xm * s;
                q = 1.0 - s;
            }
            else {
                q = fa / fc;
                r = fb / fc;
                p = s * (2.0 * xm * q * (q - r) - (b - a) * (r - 1.0));
                q = (q - 1.0) * (r - 1.0) * (s - 1.0);
            }
            if (p > 0.0) q = -q;
            p = fabs(p);
            min1 = 3.0 * xm * q - fabs(tol * q);
            min2 = fabs(e * q);
            if (2.0 * p < (min1 < min2 ? min1 : min2)) {
                e = d;
                d = p / q;
            }
            else {
                d = xm;
                e = d;
            }
        }
        else {
            d = xm;
            e = d;
        }
        a = b;
        fa = fb;
        if (fabs(d) > tol) { b += d; }
        else { b += (tol > fabs(xm)) ? tol : xm; }
        fb = (*fun)(b);
    }

    pLib->errcode = 0;
    return b;
}

#define NUMERATOR(dab,dcb,fa,fb,fc)    (fb*(dab*fc*(fc-fb)-fa*dcb*(fa-fb)))
#define DENOMINATOR(fa,fb,fc)            ((fc-fb)*(fa-fb)*(fa-fc))
/*  --------------------------------------------------------------------
    :TODO: translate to english

    ml_amsterdam    :    résolution de f(x) = 0 par l'algorithme d'Amsterdam
    
        fun         :    pointeur sur f(x)
        a, c        :    intervalle contenant la solution

        Retourne la valeur estimée de la solution
    -------------------------------------------------------------------- */
real_t ml_amsterdam(real_t (*fun)(real_t), real_t a, real_t c, libvar_t *pLib)
{
    real_t fa = (*fun)(a), b = 0.5 * (a + c), fc = (*fun)(c), fb = fa * fc;
    real_t delta, dab, dcb, tolerance = pLib->tol;
    int_t i;

    if (pLib->errcode) { return 0.0; }

    pLib->errcode = 0;
    if (fb >= 0.0) {
        if (fb > 0.0)  { pLib->errcode = 1; return 0.0; }
        else { return (fa == 0.0) ? a : c; }
    }

    if (a > c) {
        delta = a; a = c; c = delta; delta = fa; fa = fc; fc = delta;
    }

    if (fa > 0.0) {
        for (i = 0; i < pLib->maxiter; i++) {
            if ((c - a) < tolerance ) { return 0.5 * (a + c); }

            fb = (*fun)(b);

            if ((c - a) < tolerance) { return 0.5 * (a + c); }

            if ((b - a) < tolerance) {
                if ( fb > 0 ) {
                    a = b; fa = fb; b = 0.5 * (a + c); continue;
                }
                else { return b; }
            }

            if ((c - b) < tolerance) {
                if (fb < 0) {
                    c = b; fc = fb; b = 0.5 * (a + c); continue;
                }
                else { return b; }
            }

            if ((fa > fb) && (fb > fc)) {
                delta = DENOMINATOR(fa,fb,fc);
                if (delta != 0.0) {
                    dab = a - b;
                    dcb = c - b;
                    delta = NUMERATOR(dab,dcb,fa,fb,fc) / delta;
                    if (delta > dab && delta < dcb) {
                        if (fb > 0.0) { a = b; fa = fb; }
                        else if (fb < 0.0)  { c = b; fc = fb; } 
                        else { return b; }
                        b += delta;
                        continue;
                    }
                }   
            }
            (fb > 0) ? ( a = b, fa = fb ) : ( c = b, fc = fb );
            b = 0.5 * ( a + c );
        }
    }
    else { 
        for (i = 0; i < pLib->maxiter; i++) {
            if ((c - a) < tolerance) { return 0.5 * ( a + c); }

            fb = (*fun)(b);

            if ((b - a) < tolerance) {
                if (fb < 0) {
                    a = b; fa = fb; b = 0.5 * (a + c); continue;
                }
                else { return b; }
            }

            if ((c - b) < tolerance) {
                if (fb > 0) {
                    c = b; fc = fb; b = 0.5 * ( a + c ); continue;
                }
                else { return b; }
            }

            if ((fa < fb) && (fb < fc)) {
                delta = DENOMINATOR(fa,fb,fc);
                if (delta != 0.0) {
                    dab = a - b;
                    dcb = c - b;
                    delta = NUMERATOR(dab,dcb,fa,fb,fc) / delta;
                    if (delta > dab && delta < dcb) {
                        if (fb < 0.0) { a = b; fa = fb; }
                        else if (fb > 0.0)  { c = b; fc = fb; } 
                        else { return b; }
                        b += delta;
                        continue;
                    }
                }
            }

            (fb < 0) ? ( a = b, fa = fb ) : ( c = b, fc = fb );
            b = 0.5 * ( a + c );
        }
    }

    pLib->errcode = 0;
    return  b;
}
