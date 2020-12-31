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

#ifndef LIBELEC_H
#define LIBELEC_H

//    ----------------------------------------------------------------------------------------- 
/* Begin Win32 */

#include <windows.h>

// Fonctionalit� Windows: sections critiques Win32
#ifndef _WINDOWS_
typedef void CRITICAL_SECTION;
#endif

/* End Win32 */
//    ----------------------------------------------------------------------------------------- 

#include "../LibMath/libmath.h"

/*  -------------------------------------------------------------------- */
/*    VARIABLE D'INTERACTION */
extern const int_t el_version;                        /* Version de libelec */
extern const int_t el_debug;                            /* Variable de d�boguage */

/*  -------------------------------------------------------------------- */

/*  --------------------------------------------------------------------
    el_reset            :    Remise � z�ro.
    -------------------------------------------------------------------- */
void el_reset(libvar_t *pLib);


/* ####################################################################
    Porteurs libres
    Fichier : carriers.c
    #################################################################### */

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    ml_fermi       :    Calcul de l'int�grale de fermi (F1/2)

    ren            :    DeltaE / kT

    Retourne la valeur estim�e de l'int�grale
    -------------------------------------------------------------------- */
real_t el_fermi(real_t ren);

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    el_conc_n         :    Calcul de la concentration d'�lectrons

    re                :    (EF - EC) / kT
    nc                :    densit� effective dans la bande de conduction

    !!! Toutes les concentrations sont donn�es en unit�s r�duites par
    !!! rapport � 1e18 cm-3 (donc 1 correspond � 1e18 cm-3)
    !!! Toutes les �nergies sont donn�es en unit�s r�duites par
    !!! rapport � kT eV (donc 1 correspond � kT eV)
    !!! Toutes les sections efficaces de capture sont donn�es en unit�s
    !!! r�duites par rapport � 1e-14 cm2 (donc 1 correspond � 1e-14 cm2)

    Retourne la concentration d'�lectrons
    -------------------------------------------------------------------- */
real_t el_conc_n(real_t re, real_t nc);

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    el_conc_p         :    Calcul de la concentration de trous

    re                :    (EV - EF) / kT
    nv                :    densit� effective dans la bande de valence

    !!! Toutes les concentrations sont donn�es en unit�s r�duites par
    !!! rapport � 1e18 cm-3 (donc 1 correspond � 1e18 cm-3)
    !!! Toutes les �nergies sont donn�es en unit�s r�duites par
    !!! rapport � kT eV (donc 1 correspond � kT eV)
    !!! Toutes les sections efficaces de capture sont donn�es en unit�s
    !!! r�duites par rapport � 1e-14 cm2 (donc 1 correspond � 1e-14 cm2)

    Retourne la concentration de trous
    -------------------------------------------------------------------- */
real_t el_conc_p(real_t re, real_t nv);

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    el_conc_nd        :    Calcul de la concentration de dopants donneurs
    ionis�s (cas de dopant discret)

    re                :    (EF - ED) / kT
    rg                :    (EC - EV) / kT
    nd                :    concentration de dopants
    nc                :    densit� effective dans la bande de conduction
    nv                :    densit� effective dans la bande de valence
    ren               :    (EF - EC) / kT
    sn                :    section efficace de capture des �lectrons
    sp                :    section efficace de capture des trous
    eqm               :    � 1 si �quilibre thermodynamique (0 sinon)

    !!! Toutes les concentrations sont donn�es en unit�s r�duites par
    !!! rapport � 1e18 cm-3 (donc 1 correspond � 1e18 cm-3)
    !!! Toutes les �nergies sont donn�es en unit�s r�duites par
    !!! rapport � kT eV (donc 1 correspond � kT eV)
    !!! Toutes les sections efficaces de capture sont donn�es en unit�s
    !!! r�duites par rapport � 1e-14 cm2 (donc 1 correspond � 1e-14 cm2)

    Retourne la concentration de dopants ionis�s
    -------------------------------------------------------------------- */
real_t el_conc_nd(real_t re, real_t rg, real_t nd, real_t nc, real_t nv,
    real_t ren, real_t sn, real_t sp, int_t eqm);

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    el_conc_na        :    Calcul de la concentration de dopants accepteurs
    ionis�s (cas de dopant discret)

    rde               :    (Ej - EV) / kT (origine en EV)
    rfe               :    (EF - EV) / kT (origine en EV)
    rg                :    (EC - EV) / kT (gap)
    na                :    concentration de dopants
    nc                :    densit� effective dans la bande de conduction
    nv                :    densit� effective dans la bande de valence
    sn                :    section efficace de capture des �lectrons
    sp                :    section efficace de capture des trous
    eqm               :    � 1 si �quilibre thermodynamique (0 sinon)

    !!! Toutes les concentrations sont donn�es en unit�s r�duites par
    !!! rapport � 1e18 cm-3 (donc 1 correspond � 1e18 cm-3)
    !!! Toutes les �nergies sont donn�es en unit�s r�duites par
    !!! rapport � kT eV (donc 1 correspond � kT eV)
    !!! Toutes les sections efficaces de capture sont donn�es en unit�s
    !!! r�duites par rapport � 1e-14 cm2 (donc 1 correspond � 1e-14 cm2)

    Retourne la concentration de dopants ionis�s
    -------------------------------------------------------------------- */
real_t el_conc_na(real_t rde, real_t rfe, real_t rg, real_t na, real_t nc, real_t nv,
    real_t sn, real_t sp, int_t eqm);


/* ####################################################################
    Caract�ristique I(V) de diode, et Spectroscopie d'admittance
    Fichier : diode.c
    #################################################################### */

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    el_diode_current        :    Calcul du courant d'une diode

    voltage                :    tension appliqu�e (V)
    temperature            :    temp�rature (K)
    ISC                    :    courant de court-circuit (A)
    IS1                    :    courant de saturation inverse (A) - Diode 1
    n1                     :    facteur d'id�alit� - Diode 1
    IS2                    :    courant de saturation inverse (A) - Diode 2
    n2                     :    facteur d'id�alit� - Diode 2
    RP                     :    r�sistance parall�le (Ohms)
    RS                     :    r�sistance s�rie (Ohms)
    I1 et I2               :    intervalle du courant (A)
    -------------------------------------------------------------------- */
real_t el_diode_current(real_t V, real_t T, real_t ISC, real_t RP, real_t IS1, real_t n1,
    real_t IS2, real_t n2, real_t RS, real_t I1, real_t I2);

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    el_capacitance           :    Calcul de capacit� (spectroscopie d'admittance)

    Freq                     :    fr�quence (Hz)
    r                        :    r�sistance s�rie (Ohms)
    L                        :    self s�rie (H)
    GLF                      :    conductance basse-fr�quence (S)
    CHF                      :    capacit� haute-fr�quence (F)
    C1                       :    capacit� (F) - niveau 1
    Tau1                     :    constante de temps (sec.) - niveau 1
    C2                       :    capacit� (F) - niveau 2
    Tau2                     :    constante de temps (sec.) - niveau 2
    -------------------------------------------------------------------- */
real_t el_capacitance(real_t Freq, real_t r, real_t L, real_t GLF, real_t CHF,
    real_t C1, real_t Tau1, real_t C2, real_t Tau2);

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english
    el_conductance           :    Calcul de conductance (spectroscopie d'admittance)

    Freq                     :    fr�quence (Hz)
    r                        :    r�sistance s�rie (Ohms)
    L                        :    self s�rie (H)
    GLF                      :    conductance basse-fr�quence (S)
    CHF                      :    capacit� haute-fr�quence (F)
    C1                       :    capacit� (F) - niveau 1
    Tau1                     :    constante de temps (sec.) - niveau 1
    C2                       :    capacit� (F) - niveau 2
    Tau2                     :    constante de temps (sec.) - niveau 2
    -------------------------------------------------------------------- */
real_t el_conductance(real_t Freq, real_t r, real_t L, real_t GLF, real_t CHF,
    real_t C1, real_t Tau1, real_t C2, real_t Tau2);

#endif