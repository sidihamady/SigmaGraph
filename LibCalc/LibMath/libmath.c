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
const int_t ml_debug = 1;                    /* Variable de d�boguage */
#else
const int_t ml_debug = 0;                    /* Variable de d�boguage */
#endif
byte_t ml_angleunit = 0x00;                  /* Unit� des angles : 0x00:radians, 0x01:degr�s; 0x02:grad */
char_t ml_linefmt[ML_STRSIZES];              /* "%lf\t%lf\n" , "%.6e\t%.6e\n" */
char_t ml_format_f[ML_STRSIZET];             /* Format num�rique flottant */
char_t ml_format_e[ML_STRSIZET];             /* Format num�rique exponentiel */
/*  -------------------------------------------------------------------- */

void ml_print(const char_t *pszOut)
{
    _tprintf(pszOut);
}

/*  --------------------------------------------------------------------
    ml_reset            :    Remise � z�ro.
    -------------------------------------------------------------------- */
void ml_reset(libvar_t *pLib)
{
    memset(ml_linefmt, 0,
        ML_STRSIZES * sizeof(char_t));                /* "%lf\t%lf\n" , "%.6e\t%.6e\n" */
    _tcscpy(ml_format_f, ML_FORMAT_FSHORT);
    _tcscpy(ml_format_e, ML_FORMAT_ESHORT);

    pLib->running = 0;                                /* Variable d'ex�cution */
    pLib->errcode = 0;                                /* Variable d'erreur */
    pLib->progress = 0;                               /* Variable de progression (entre 0 et 100) */
    pLib->interact = 1;                               /* Variable d'interaction */
    pLib->console = 0;                                /* Mode Console (pour printf) */
    memset(pLib->message, 0,
        ML_STRSIZE * sizeof(char_t));                 /* Message (erreur, info, ...) */

    pLib->limitc = 1;                                 /* Conditions aux limites (0=Dirichlet, 1=Neumann, ...) */
    pLib->autoinit = 1;                               /* Calcul automatique des conditions initiales */

    pLib->solver = 0;                                 /* M�thode de r�solution utilis�e par ml_fzero */
    pLib->linsolver = 0;                              /* Algorithme utilis� pour r�soudre le syst�me lin�aire
                                                         r�sultant (0 = LU, 1 = SOR, 2 = Jacobi, 3 = Gauss) */

    pLib->maxiter = 256;                              /* Nombre maximal d'it�rations */
    pLib->tol = 1e-9;                                 /* Pr�cision */
    pLib->reltol = 1e-3;                              /* Pr�cision relative */
    pLib->relerr = 1.0;                               /* Pr�cision relative trouv�e */
    pLib->exptail = 100.0;                            /* Seuil de d�tection des variations exponentielles */
}

/*  --------------------------------------------------------------------
    ml_libvar_copy    :    Copier les donn�es d'une librairie

        pfrom, pto    :    pointeur vers les couches � et vers qui copier resp.
    -------------------------------------------------------------------- */
void ml_libvar_copy(const libvar_t *pfrom, libvar_t *pto)
{
    if ((pfrom == NULL) || (pto == NULL)) {
        return;
    }

    pto->errcode = pfrom->errcode;                        /* Variable d'erreur */
    pto->running = pfrom->running;                        /* Variable d'ex�cution */
    pto->progress = pfrom->progress;                      /* Variable de progression (entre 0 et 100) */
    _tcscpy(pto->message, (const char_t*)(pfrom->message));
    pto->interact = pfrom->interact;                      /* Variable d'interaction */
    pto->console = pfrom->console;                        /* Mode Console (pour printf) */
    pto->solver = pfrom->solver;                          /* M�thode de r�solution utilis�e par ml_fzero */
    pto->linsolver = pfrom->linsolver;                    /* Algorithme utilis� pour r�soudre le syst�me lin�aire
                                                             r�sultant (0 = LU, 1 = SOR, 2 = Jacobi, 3 = Gauss) */
    pto->limitc = pfrom->limitc;                          /* Conditions aux limites (0=Dirichlet, 1=Neumann, ...) */
    pto->autoinit = pfrom->autoinit;                      /* Calcul automatique des conditions initiales */
    pto->maxiter = pfrom->maxiter;                        /* Nombre maximal d'it�rations */
    pto->tol = pfrom->tol;                                /* Pr�cision */
    pto->reltol = pfrom->reltol;                          /* Pr�cision relative demand�e */
    pto->relerr = pfrom->relerr;                          /* Pr�cision relative trouv�e */
    pto->exptail = pfrom->exptail;                        /* Seuil de d�tection des variations exponentielles */
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