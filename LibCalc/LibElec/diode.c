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

#include "libelec.h"


/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    el_diode_current       :    Calcul du courant d'une diode

    voltage                :    tension appliquée (V)
    temperature            :    température (K)
    ISC                    :    courant de court-circuit (A)
    IS1                    :    courant de saturation inverse (A) - Diode 1
    n1                     :    facteur d'idéalité - Diode 1
    IS2                    :    courant de saturation inverse (A) - Diode 2
    n2                     :    facteur d'idéalité - Diode 2
    RP                     :    résistance parallèle (Ohms)
    RS                     :    résistance série (Ohms)
    I1 et I2               :    intervalle du courant (A)
    -------------------------------------------------------------------- */
real_t el_diode_current(real_t V, real_t T, real_t ISC, real_t RP, real_t IS1, real_t n1,
    real_t IS2, real_t n2, real_t RS, real_t I1, real_t I2)
{
    real_t fpar[ML_FIT_MAXPAR], fopt[ML_FIT_MAXPAR];
    real_t II;

    if ((V == 0.0) && (ISC == 0.0)) {
        return 0.0;
    }

    //    fpar[0]        ISC
    //    fpar[1]        RP
    //    fpar[2]        IS1
    //    fpar[3]        n1
    //    fpar[4]        IS2
    //    fpar[5]        n2
    //    fpar[6]        RS
    fpar[0] = ISC;
    fpar[1] = RP;
    fpar[2] = IS1;
    fpar[3] = n1;
    fpar[4] = IS2;
    fpar[5] = n2;
    fpar[6] = RS;
    fpar[7] = 0.0;

    //    fopt[0]        V
    //    fopt[1]        T
    //    fopt[2]        I1
    //    fopt[3]        I2
    //    fopt[4]        Eg
    fopt[0] = V;
    fopt[1] = T;
    fopt[2] = I1;
    fopt[3] = I2;
    fopt[4] = 0.0;
    fopt[5] = 0.0;
    fopt[6] = 0.0;
    fopt[7] = 0.0;

    II = ml_fit_fun(FIT_MODEL_DIODER, V, fpar, 7, fopt);

    return II;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    el_capacitance         :    Calcul de capacité (spectroscopie d'admittance)

    Freq                   :    fréquence (Hz)
    r                      :    résistance série (Ohms)
    L                      :    self série (H)
    GLF                    :    conductance basse-fréquence (S)
    CHF                    :    capacité haute-fréquence (F)
    C1                     :    capacité (F) - niveau 1
    Tau1                   :    constante de temps (sec.) - niveau 1
    C2                     :    capacité (F) - niveau 2
    Tau2                   :    constante de temps (sec.) - niveau 2
    -------------------------------------------------------------------- */
real_t el_capacitance(real_t Freq, real_t r, real_t L, real_t GLF, real_t CHF,
    real_t C1, real_t Tau1, real_t C2, real_t Tau2)
{
    real_t fpar[ML_FIT_MAXPAR];
    real_t cm;

    //    fpar[0]        r
    //    fpar[1]        L
    //    fpar[2]        GLF
    //    fpar[3]        CHF
    //    fpar[4]        C1
    //    fpar[5]        Tau1
    //    fpar[6]        C2
    //    fpar[7]        Tau2
    fpar[0] = r;
    fpar[1] = L;
    fpar[2] = GLF;
    fpar[3] = CHF;
    fpar[4] = C1;
    fpar[5] = Tau1;
    fpar[6] = C2;
    fpar[7] = Tau2;

    cm = ml_fit_fun(FIT_MODEL_ADMITC, Freq, fpar, 8, NULL);

    return cm;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    el_conductance         :    Calcul de conductance (spectroscopie d'admittance)

    Freq                   :    fréquence (Hz)
    r                      :    résistance série (Ohms)
    L                      :    self série (H)
    GLF                    :    conductance basse-fréquence (S)
    CHF                    :    capacité haute-fréquence (F)
    C1                     :    capacité (F) - niveau 1
    Tau1                   :    constante de temps (sec.) - niveau 1
    C2                     :    capacité (F) - niveau 2
    Tau2                   :    constante de temps (sec.) - niveau 2
    -------------------------------------------------------------------- */
real_t el_conductance(real_t Freq, real_t r, real_t L, real_t GLF, real_t CHF,
    real_t C1, real_t Tau1, real_t C2, real_t Tau2)
{
    real_t fpar[ML_FIT_MAXPAR];
    real_t gm;

    //    fpar[0]        r
    //    fpar[1]        L
    //    fpar[2]        GLF
    //    fpar[3]        CHF
    //    fpar[4]        C1
    //    fpar[5]        Tau1
    //    fpar[6]        C2
    //    fpar[7]        Tau2
    fpar[0] = r;
    fpar[1] = L;
    fpar[2] = GLF;
    fpar[3] = CHF;
    fpar[4] = C1;
    fpar[5] = Tau1;
    fpar[6] = C2;
    fpar[7] = Tau2;

    gm = ml_fit_fun(FIT_MODEL_ADMITG, Freq, fpar, 8, NULL);

    return gm;
}
