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

/*  -------------------------------------------------------------------- */
/* Begin Win32 (For ml_interact function) */
#include <windows.h>
/* End Win32 */
/*  -------------------------------------------------------------------- */

/*  -------------------------------------------------------------------- */
/*    VARIABLES D'INTERACTION */
const int_t ml_version = 0x26A0;             /* Version de libmath */
#ifdef _DEBUG
const int_t ml_debug = 1;                    /* Variable de déboguage */
#else
const int_t ml_debug = 0;                    /* Variable de déboguage */
#endif
byte_t ml_angleunit = 0x00;                  /* Unité des angles : 0x00:radians, 0x01:degrés; 0x02:grad */
char_t ml_linefmt[ML_STRSIZES];              /* "%lf\t%lf\n" , "%.6e\t%.6e\n" */
char_t ml_format_f[ML_STRSIZET];             /* Format numérique flottant */
char_t ml_format_e[ML_STRSIZET];             /* Format numérique exponentiel */
/*  -------------------------------------------------------------------- */

void ml_print(const char_t *pszOut)
{
    _tprintf(pszOut);
}

/*  --------------------------------------------------------------------
    ml_reset            :    Remise à zéro.
    -------------------------------------------------------------------- */
void ml_reset(libvar_t *pLib)
{
    memset(ml_linefmt, 0,
        ML_STRSIZES * sizeof(char_t));                /* "%lf\t%lf\n" , "%.6e\t%.6e\n" */
    _tcscpy(ml_format_f, ML_FORMAT_FSHORT);
    _tcscpy(ml_format_e, ML_FORMAT_ESHORT);

    pLib->running = 0;                                /* Variable d'exécution */
    pLib->errcode = 0;                                /* Variable d'erreur */
    pLib->progress = 0;                               /* Variable de progression (entre 0 et 100) */
    pLib->interact = 1;                               /* Variable d'interaction */
    pLib->console = 0;                                /* Mode Console (pour printf) */
    memset(pLib->message, 0,
        ML_STRSIZE * sizeof(char_t));                 /* Message (erreur, info, ...) */

    pLib->limitc = 1;                                 /* Conditions aux limites (0=Dirichlet, 1=Neumann, ...) */
    pLib->autoinit = 1;                               /* Calcul automatique des conditions initiales */

    pLib->solver = 0;                                 /* Méthode de résolution utilisée par ml_fzero */
    pLib->linsolver = 0;                              /* Algorithme utilisé pour résoudre le système linéaire
                                                         résultant (0 = LU, 1 = SOR, 2 = Jacobi, 3 = Gauss) */

    pLib->maxiter = 256;                              /* Nombre maximal d'itérations */
    pLib->tol = 1e-9;                                 /* Précision */
    pLib->reltol = 1e-3;                              /* Précision relative */
    pLib->relerr = 1.0;                               /* Précision relative trouvée */
    pLib->exptail = 100.0;                            /* Seuil de détection des variations exponentielles */
}

/*  --------------------------------------------------------------------
    ml_libvar_copy    :    Copier les données d'une librairie

        pfrom, pto    :    pointeur vers les couches à et vers qui copier resp.
    -------------------------------------------------------------------- */
void ml_libvar_copy(const libvar_t *pfrom, libvar_t *pto)
{
    if ((pfrom == NULL) || (pto == NULL)) {
        return;
    }

    pto->errcode = pfrom->errcode;                        /* Variable d'erreur */
    pto->running = pfrom->running;                        /* Variable d'exécution */
    pto->progress = pfrom->progress;                      /* Variable de progression (entre 0 et 100) */
    _tcscpy(pto->message, (const char_t*)(pfrom->message));
    pto->interact = pfrom->interact;                      /* Variable d'interaction */
    pto->console = pfrom->console;                        /* Mode Console (pour printf) */
    pto->solver = pfrom->solver;                          /* Méthode de résolution utilisée par ml_fzero */
    pto->linsolver = pfrom->linsolver;                    /* Algorithme utilisé pour résoudre le système linéaire
                                                             résultant (0 = LU, 1 = SOR, 2 = Jacobi, 3 = Gauss) */
    pto->limitc = pfrom->limitc;                          /* Conditions aux limites (0=Dirichlet, 1=Neumann, ...) */
    pto->autoinit = pfrom->autoinit;                      /* Calcul automatique des conditions initiales */
    pto->maxiter = pfrom->maxiter;                        /* Nombre maximal d'itérations */
    pto->tol = pfrom->tol;                                /* Précision */
    pto->reltol = pfrom->reltol;                          /* Précision relative demandée */
    pto->relerr = pfrom->relerr;                          /* Précision relative trouvée */
    pto->exptail = pfrom->exptail;                        /* Seuil de détection des variations exponentielles */
}
void ml_event(void)
{
    MSG msgFree;
    // Traiter les messages Windows en attente
    while(PeekMessage(&msgFree, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msgFree);
        DispatchMessage(&msgFree);
    }
}