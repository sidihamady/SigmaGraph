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

    ml_erf :    Calcul de la fonction erf(x)
                D'après l'algorithme de Cody :
                W. J. Cody,
                "Rational Chebyshev approximations for the error function",
                Math. Comp., 1969, PP. 631-638.
    -------------------------------------------------------------------- */

real_t ml_erf(real_t x)
{
    real_t rr = 0.0, y = 0.0, z = 0.0, xnum = 0.0, xden = 0.0;
    const real_t aa[5] = {
        3.16112374387056560e00, 1.13864154151050156e02, 3.77485237685302021e02, 3.20937758913846947e03, 1.85777706184603153e-1
    };
    const real_t bb[4] = {
        2.36012909523441209e01, 2.44024637934444173e02, 1.28261652607737228e03, 2.84423683343917062e03
    };
    const real_t cc[9] = {
        5.64188496988670089e-1, 8.88314979438837594e00, 6.61191906371416295e01, 2.98635138197400131e02, 8.81952221241769090e02,
        1.71204761263407058e03, 2.05107837782607147e03, 1.23033935479799725e03, 2.15311535474403846e-8
    };
    const real_t dd[8] = {
        1.57449261107098347e01, 1.17693950891312499e02, 5.37181101862009858e02, 1.62138957456669019e03, 3.29079923573345963e03,
        4.36261909014324716e03, 3.43936767414372164e03, 1.23033935480374942e03
    };
    const real_t ee[6] = {
        3.05326634961232344e-1, 3.60344899949804439e-1, 1.25781726111229246e-1, 1.60837851487422766e-2, 6.58749161529837803e-4, 1.63153871373020978e-2
    };
    const real_t ff[5] = {
        2.56852019228982242e00, 1.87295284992346047e00, 5.27905102951428412e-1, 6.05183413124413191e-2, 2.33520497626869185e-3
    };
    int_t ii = 0;

    y = fabs(x);

    if (y < 0.46875) {
        z = y*y;
        xnum = aa[4]*z;
        xden = y;
        for (ii = 0; ii < 3; ii++) {
            xnum = (xnum + aa[ii]) * z;
            xden = (xden + bb[ii]) * z;
        }
        rr = x * (xnum + aa[3]) / (xden + bb[3]);
    }
    else if ((y >= 0.46875) && (y <= 4.0)) {
        xnum = cc[8] * y;
        xden = y;
        for (ii = 0; ii < 7; ii++) {
            xnum = (xnum + cc[ii]) * y;
            xden = (xden + dd[ii]) * y;
        }
        rr = (xnum + cc[7]) / (xden + dd[7]);
        z = ceil(y * 16.0) / 16.0;
        rr *= exp(-(z * z) - ((y - z) * (y + z)));
    }
    else if (y > 4.0) {
        z = 1.0 / (y * y);
        xnum = ee[5] * z;
        xden = z;
        for (ii = 0; ii < 4; ii++) {
            xnum = (xnum + ee[ii]) * z;
            xden = (xden + ff[ii]) * z;
        }
        rr = z * (xnum + ee[4]) / (xden + ff[4]);
        rr = (0.56418958354775629 - rr) / y;
        z = ceil(y * 16.0) / 16.0;
        rr *= exp(-(z * z) - ((y - z) * (y + z)));
    }

    if (x > 0.46875) {
        rr = (0.5 - rr) + 0.5;
    }
    else if (x < -0.46875) {
        rr = (-0.5 + rr) - 0.5;
    }

    return rr;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_bern    :    calcul de la fonction de Bernoulli B(x) = x / (exp(x) - 1)
    
        x      :    argument de la fonction

        Retourne la valeur de B(x)
    -------------------------------------------------------------------- */
real_t ml_bern(real_t x)
{
    real_t xlim = 1e-2, ax, fi,  fp, fn, df, segno, feps = DBL_EPSILON;
    real_t bp, bn;

    ax = fabs(x);

    if (ax == 0.0) {
        bp = 1.0;
        bn = 1.0;
        return (bp);
    }

    if (ax > 80.0) {
        if (x > 0.0) {
            bp = 0.0;
            bn = x;
            return (bp);
        }
        else {
            bp = -x;
            bn = 0.0;
            return (bp);
        }
    }

    if (ax > xlim) {
        bp = x / (exp(x) - 1.0);
        bn = x + (bp);
        return (bp);
    }

    fi = 1.0;
    fp = 1.0;
    fn = 1.0;
    df = 1.0;
    segno = 1.0;
    while (fabs(df) > feps) {
        fi += 1.0;
        segno = -segno;
        df = (df * x) / fi;
        fp = fp + df;
        fn = fn + (segno * df);
        bp = 1.0 / fp;
        bn = 1.0 / fn;
    }

    return (bp);
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_dbern     :    calcul de la dérivée de la fonction de Bernoulli

        x        :    argument de la fonction

        Retourne la valeur de la dérivée de B(x)
    -------------------------------------------------------------------- */
real_t ml_dbern(real_t x, real_t delta)
{
    real_t xt, tmp, db;

    xt = x - (2.0 * delta);
    tmp = ml_bern(xt);

    xt = x - delta;
    tmp -= (8.0 * ml_bern(xt));

    xt = x + delta;
    tmp += (8.0 * ml_bern(xt));

    xt = x + (2 * delta);
    tmp -= ml_bern(xt);

    db = tmp / (12.0 * delta);

    return (db);
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_gauss     :    calcul de la fonction de Gauss G(x) = exp(-(x - b)² / 2c²)
    
        x        :    argument de la fonction
        b        :    abscisse du pic
        c        :    écart-type

        Retourne la valeur de G(x)
    -------------------------------------------------------------------- */
real_t ml_gauss(real_t x, real_t b, real_t c)
{
    real_t yt;

    if (c <= 0.0) {
        return 0.0;
    }

    yt = exp(-(x - b) * (x - b) / (2.0 * c * c));

    return yt;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_lorentz       :    calcul de la fonction de Lorentz
                          L(x) = ((1/pi) * c / ((x - b)² + c²))
    
        x            :    argument de la fonction
        b            :    abscisse du pic
        c            :    écart-type

        Retourne la valeur de L(x)
    -------------------------------------------------------------------- */
real_t ml_lorentz(real_t x, real_t b, real_t c)
{
    real_t yt;

    if (c <= 0.0) {
        return 0.0;
    }

    yt = 1 / (1.0 + (4.0 * (x - b) * (x - b) / (c * c)));

    return (yt / ML_PI);
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_gammaln    :    calcul du logarithme de la fonction Gamma
    
        x         :    argument de la fonction

        Retourne la valeur de ln(Gamma(x))
    -------------------------------------------------------------------- */
real_t ml_gammaln(real_t x)
{
    real_t xt, yt, tmp, ser;
    static real_t cof[6] = {76.18009172947146, -86.50532032941677, 24.01409824083091,
        -1.231739572450155, 0.1208650973866179e-2, -0.5395239384953e-5};
    int j;

    if (x <= 0.0) {
        return 0.0;
    }

    yt = xt = x;
    tmp = xt + 5.5;
    tmp -= (xt + 0.5) * log(tmp);
    ser = 1.000000000190015;
    for (j = 0; j <= 5; j++) {
        ser += (cof[j] / ++yt);
    }
    return (-tmp + log(2.5066282746310005 * ser / xt));
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_beta        :    calcul de la fonction Beta
                        B(z,w) = Gamma(z) * Gamma(w) / Gamma(z + w)
    
        x          :    argument de la fonction

        Retourne la valeur de ln(Gamma(x))
    -------------------------------------------------------------------- */
real_t ml_beta(real_t z, real_t w)
{
    return exp(ml_gammaln(z) + ml_gammaln(w) - ml_gammaln(z + w));
}