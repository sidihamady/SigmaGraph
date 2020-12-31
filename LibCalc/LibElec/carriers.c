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
    el_fermi : Calcul de l'intégrale de fermi (F1/2)

    ren : DeltaE / kT

    Retourne la valeur estimée de l'intégrale
    -------------------------------------------------------------------- */
real_t el_fermi(real_t ren)
{
    real_t rr = 1.0, ren2, ren3, ren4, ren5, ren6, ren7;

    const real_t aa[6] = {
        0.764, 0.60401, 0.1891,
        0.020136, -0.002642, -0.00062917
    };
    const real_t bb[8] = {
        0.80923, 0.48869, 0.28965,
        -0.016992, 0.00088425, -2.9222e-05,
        5.3627e-07, -4.1465e-09
    };
    const real_t cc[3] = {
        -44.611, 4.4932, 0.034791
    };

    /* Non-sens */
    if (ren < -50.0) {
        rr = 1.0e-22;
    }
    /* Boltzmann */
    else if ((ren >= -50.0) && (ren < -3.0)) {
        rr = exp(ren);
    }
    /* Fermi */
    else if ((ren >= -3.0) && (ren < 1.0)) {
        ren2 = ren * ren; ren3 = ren2 * ren;
        ren4 = ren3 * ren; ren5 = ren4 * ren;

        rr = aa[0] + (aa[1] * ren) + (aa[2] * ren2) + (aa[3] * ren3)
            + (aa[4] * ren4) + (aa[5] * ren5);
    }
    /* Fermi */
    else if ((ren >= 1.0) && (ren < 30.0)) {
        ren2 = ren * ren; ren3 = ren2 * ren;
        ren4 = ren3 * ren; ren5 = ren4 * ren;
        ren6 = ren5 * ren; ren7 = ren6 * ren;

        rr = bb[0] + (bb[1] * ren) + (bb[2] * ren2) + (bb[3] * ren3)
            + (bb[4] * ren4) + (bb[5] * ren5) + (bb[6] * ren6) + (bb[7] * ren7);
    }
    /* Fermi */
    else if ((ren >= 30.0) && (ren <= 100.0)) {
        ren2 = ren * ren;

        rr = cc[0] + (cc[1] * ren) + (cc[2] * ren2);
    }
    /* Non-sens */
    else if (ren > 100.0) {
        rr = 1.0e3;
    }

    return rr;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    el_conc_n : Calcul de la concentration d'électrons

    re :    (EF - EC) / kT
    nc :    densité effective dans la bande de conduction

    !!! Toutes les concentrations sont données en unités réduites par
    !!! rapport à 1e18 cm-3 (donc 1 correspond à 1e18 cm-3)
    !!! Toutes les énergies sont données en unités réduites par
    !!! rapport à kT eV (donc 1 correspond à kT eV)
    !!! Toutes les sections efficaces de capture sont données en unités
    !!! réduites par rapport à 1e-14 cm2 (donc 1 correspond à 1e-14 cm2)

    Retourne la concentration d'électrons
    -------------------------------------------------------------------- */
real_t el_conc_n(real_t re, real_t nc)
{
    real_t n = 0.0;

    n = nc * el_fermi(re) * exp(re);

    return n;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    el_conc_p        :    Calcul de la concentration de trous

    re                :    (EV - EF) / kT
    nv                :    densité effective dans la bande de valence

    !!! Toutes les concentrations sont données en unités réduites par
    !!! rapport à 1e18 cm-3 (donc 1 correspond à 1e18 cm-3)
    !!! Toutes les énergies sont données en unités réduites par
    !!! rapport à kT eV (donc 1 correspond à kT eV)
    !!! Toutes les sections efficaces de capture sont données en unités
    !!! réduites par rapport à 1e-14 cm2 (donc 1 correspond à 1e-14 cm2)

    Retourne la concentration de trous
    -------------------------------------------------------------------- */
real_t el_conc_p(real_t re, real_t nv)
{
    real_t p = 0.0;

    p = nv * el_fermi(re) * exp(re);

    return p;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    el_conc_nd        :    Calcul de la concentration de dopants donneurs
    ionisés (cas de dopant discret)

    rde            :    (Ei - EC) / kT (origine en EC)
    rfe            :    (EF - EC) / kT (origine en EC)
    rg             :    (EC - EV) / kT (gap)
    nd             :    concentration de dopants
    nc             :    densité effective dans la bande de conduction
    nv             :    densité effective dans la bande de valence
    sn             :    section efficace de capture des électrons
    sp             :    section efficace de capture des trous
    eqm            :    à 1 si équilibre thermodynamique (0 sinon)

    !!! Toutes les concentrations sont données en unités réduites par
    !!! rapport à 1e18 cm-3 (donc 1 correspond à 1e18 cm-3)
    !!! Toutes les énergies sont données en unités réduites par
    !!! rapport à kT eV (donc 1 correspond à kT eV)
    !!! Toutes les sections efficaces de capture sont données en unités
    !!! réduites par rapport à 1e-14 cm2 (donc 1 correspond à 1e-14 cm2)

    Retourne la concentration de dopants ionisés
    -------------------------------------------------------------------- */
real_t el_conc_nd(real_t rde, real_t rfe, real_t rg, real_t nd, real_t nc, real_t nv,
    real_t sn, real_t sp, int_t eqm)
{
    real_t fd, ndx, n1, p1, n2, p2;

    /* Les vérifications des données sont faites en amont par la fonction appelante... */

    n1 = nc * el_fermi(-rfe);
    p1 = nv * el_fermi(-rg + rfe);

    if (eqm == 1) {
        fd = 1.0 / (1.0 + exp(-rfe + rde));
    }
    else {
        n2 = nc * exp(-rde) * exp(-rde + rfe) * el_fermi(rde - rfe);
        p2 = nv * exp(-rg + rde) * exp(rde - rfe) * el_fermi(-rde + rfe);
        fd = ((sp * p1) + (sn * n2)) / ((sn * (n1 + n2)) + (sp * (p1 + p2)));
    }

    ndx = nd * fd;

    return ndx;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    el_conc_na    :    Calcul de la concentration de dopants accepteurs
    ionisés (cas de dopant discret)

    rde :    (Ej - EV) / kT (origine en EV)
    rfe :    (EF - EV) / kT (origine en EV)
    rg  :    (EC - EV) / kT (gap)
    na  :    concentration de dopants
    nc  :    densité effective dans la bande de conduction
    nv  :    densité effective dans la bande de valence
    sn  :    section efficace de capture des électrons
    sp  :    section efficace de capture des trous
    eqm :    à 1 si équilibre thermodynamique (0 sinon)

    !!! Toutes les concentrations sont données en unités réduites par
    !!! rapport à 1e18 cm-3 (donc 1 correspond à 1e18 cm-3)
    !!! Toutes les énergies sont données en unités réduites par
    !!! rapport à kT eV (donc 1 correspond à kT eV)
    !!! Toutes les sections efficaces de capture sont données en unités
    !!! réduites par rapport à 1e-14 cm2 (donc 1 correspond à 1e-14 cm2)

    Retourne la concentration de dopants ionisés
    -------------------------------------------------------------------- */
real_t el_conc_na(real_t rde, real_t rfe, real_t rg, real_t na, real_t nc, real_t nv,
    real_t sn, real_t sp, int_t eqm)
{
    real_t fa, nax, n1, p1, n2, p2;

    /* Les vérifications des données sont faites en amont par la fonction appelante... */

    p1 = nv * el_fermi(-rfe);
    n1 = nc * el_fermi(-rg + rfe);

    if (eqm == 1) {
        fa = 1.0 / (1.0 + exp(-rfe + rde));
    }
    else {
        p2 = nv * exp(-rde) * exp(-rde + rfe) * el_fermi(rde - rfe);
        n2 = nc * exp(-rg + rde) * exp(rde - rfe) * el_fermi(-rde + rfe);
        fa = ((sn * n1) + (sp * p2)) / ((sp * (p1 + p2)) + (sn * (n1 + n2)));
    }

    nax = na * fa;

    return nax;
}
