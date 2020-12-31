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

#ifndef LIBMATH_H
#define LIBMATH_H

#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <float.h>

#ifdef WIN32
#pragma intrinsic(memcmp, memcpy, memset, _tcslen, _tcscpy, _tcscat, abs, fabs)
#endif

//    ------------------------------------------------------------------------------------------------------------- 
/*    TYPE DEF */
#ifndef _tcsprintf
#define _tcsprintf    swprintf
#endif

typedef unsigned char byte_t;

typedef TCHAR char_t;

typedef int int_t;

typedef unsigned int uint_t;

typedef long long_t;

typedef unsigned long ulong_t;

typedef double real_t;

__inline int ml_isEqual(double tx, double ty)
{
    return (fabs(tx - ty) <= (DBL_EPSILON * fabs(tx)));
}

__inline int ml_isNaN(double tx)
{
    return ((tx != tx) || ((tx == tx) && ((tx - tx) != 0.0)));
}

//    ------------------------------------------------------------------------------------------------------------- 

//    ------------------------------------------------------------------------------------------------------------- 
/*    MACROS */
#define ML_MAXMAX                262144
#define ML_MAXCURVES             16
#define ML_MAXCOLUMNS            26
#define ML_MINPOINTS             26
#define ML_MAXPOINTS             16384
#define ML_POINTS                101
#define ML_MAXCELLS              (ML_MAXPOINTS * ML_MAXCOLUMNS)
#define ML_MAXITER               2048                    // Nombre maximal d'it�rations
#define ML_PARSER_VARNUM         26                      // Number of parser variables (a, b, ..., z)

#define ML_FIT_MAXMODEL          24                      // Maximum number of models
#define FIT_MODEL_FIRST          0
#define FIT_MODEL_LINE           0
#define FIT_MODEL_POLY2          1
#define FIT_MODEL_POLY3          2
#define FIT_MODEL_POLY4          3

#define FIT_MODEL_GAUSS1         4
#define FIT_MODEL_GAUSS2         5
#define FIT_MODEL_GAUSS3         6
#define FIT_MODEL_GAUSS4         7
#define FIT_MODEL_GAUSS5         8
#define FIT_MODEL_LORENTZ1       9
#define FIT_MODEL_LORENTZ2       10
#define FIT_MODEL_LORENTZ3       11
#define FIT_MODEL_LORENTZ4       12
#define FIT_MODEL_LORENTZ5       13

#define FIT_MODEL_PEARSON        14
#define FIT_MODEL_EXPGR          15
#define FIT_MODEL_EXPGR2         16
#define FIT_MODEL_EXPDEC         17
#define FIT_MODEL_EXPDEC2        18
#define FIT_MODEL_HYPERBOLA      19
#define FIT_MODEL_LOG            20
#define FIT_MODEL_POWER          21
#define FIT_MODEL_LOGISTIC       22
#define FIT_MODEL_WAVE           23
#define FIT_MODEL_LAST           23
#define FIT_MODEL_DIODE          81
#define FIT_MODEL_DIODER         82
#define FIT_MODEL_ADMITC         84
#define FIT_MODEL_ADMITG         86
#define FIT_MODEL_USER           254
#define ML_FIT_USERINDEX        (FIT_MODEL_USER + 1)
#define ML_FIT_MAXPAR            20                     // Maximum number of parameters
#define ML_FIT_LAMBDAFAC         10.0                   // Facteur de descente
#define ML_FIT_LAMBDAMAX         1.0e+12                // Max. lambda
#define ML_FIT_LAMBDAMIN         1.0e-12                // Min. lambda

#define ML_EPSILON               1e-13
#define ML_MIN(a,b)              (((a) < (b)) ? (a) : (b))
#define ML_MAX(a,b)              (((a) > (b)) ? (a) : (b))
#define ML_SWAP(a,b)             {real_t sffs = (a);(a) = (b);(b) = sffs;}
#define ML_SWAPI(a,b)            {int_t sffs = (a);(a) = (b);(b) = sffs;}
#define ML_SWAPUI(a,b)           {uint_t sffs = (a);(a) = (b);(b) = sffs;}
#define ML_SWAPL(a,b)            {long_t sffs = (a);(a) = (b);(b) = sffs;}

#define ML_DATEFMT              _T("%d/%m/%Y %H:%M:%S")
#define ML_FORMAT_FSHORT        _T("%.9f")
#define ML_FORMAT_FLONG         _T("%.15f")
#define ML_FORMAT_ESHORT        _T("%.5e")
#define ML_FORMAT_ELONG         _T("%.9e")

/* Text size */
#define ML_STRSIZET             16        // Tiny
#define ML_STRSIZES             32        // Small
#define ML_STRSIZEN             64        // Normal
#define ML_STRSIZE              256       // Normal
#define ML_STRSIZEW             1024      // Wide
#define ML_STRSIZEL             8192      // Large

/* Constantes fondamentales */
#define ML_PI                         3.14159265358979323846264338327950
#define ML_2PI                        6.28318530717958647692528676655900
#define ML_PI2                        1.57079632679489661923132169163975
#define ML_PI4                        0.78539816339744830961566084581987
#define ML_SQRTPI2                    1.77245385090551602729816748334110
#define ML_SQRT2                      1.41421356237309504880168872420969
#define ML_SQRT22                     0.70710678118654752440084436210484

/* Constantes physiques fondamentales */
#define ML_KB                        (1.380650e-23)
#define ML_QE                        (1.602176e-19)
#define ML_HH                        (6.626068e-34)
#define ML_CC                        (299792458.0)
#define ML_EPS0                      (8.854187e-12)
#define ML_NA                        (6.022141e23)
#define ML_M0                        (9.109382e-31)
#define ML_KBT                        0.02585202874
//    ------------------------------------------------------------------------------------------------------------- 

//    ------------------------------------------------------------------------------------------------------------- 
/*    TYPE DEF */

typedef struct _date_t
{
    int_t year;
    int_t month;
    int_t day;
    int_t hour;
    int_t minute;
    int_t second;
    int_t millisecond;
} date_t;

typedef struct _rect_t
{
    real_t xa;
    real_t ya;
    real_t xb;
    real_t yb;
} rect_t;

typedef struct _point_t
{
    real_t x;
    real_t y;
} point_t;

typedef struct _vector_t
{
    long_t id;                             /* ID, unique */
    long_t idx;                            /* ID d'un vecteur li�, unique - Abscisse */
    long_t idf;                            /* ID d'un vecteur li�, unique - Fit */
    long_t ide;                            /* ID d'un vecteur li�, unique - Barres d'erreur */
    long_t idr;                            /* ID r�serv� */
    byte_t type;                           /* X: 0x1-    ;    Y: 0x2-    ;    X-Err: 0x3-    ;    Y-Err: 0x4-    ;    ... */
    char_t name[ML_STRSIZET];              /* Name, unique. Ex: A, B, ... */
    char_t label[ML_STRSIZES];             /* Label */
    char_t format[ML_STRSIZET];            /* Data format (e.g. : "%.3f") */
    char_t formula[ML_STRSIZEN];           /* Formula (optional) */
    int_t dim;                             /* Dimension */
    int_t istart;                          /* Indice de d�but (si aucun masque alors istart = 0) */
    int_t istop;                           /* Indice de fin (si aucun masque alors istop = dim - 1) */
    real_t *dat;
    byte_t *mask;                          /* 0x00    :    none,
                                              0xX0    :    not masked,        0xX1: masked,    ...
                                              0xXF    :    invalid item (to be edited) */
    byte_t status;                         /* 0x00    :    none, ..., 0xXF: invalid column */
    byte_t opt;                            /* 0x1X    :    show name    ;    0x0X : show label, ... */
} vector_t;

typedef struct _stat_t
{
    long_t id;                          /* ID, unique */
    byte_t done;                        /* Calculs faits (donn�es non modifi�es) */
    byte_t sorted;                      /* Donn�es d�j� tri�es */
    int_t points;                       /* Nombre de points */
    real_t min;                         /* Minimum */
    real_t max;                         /* Maximum */
    real_t sum;                         /* Somme */
    real_t mean;                        /* Moyenne */
    real_t median;                      /* M�diane */
    real_t rms;                         /* RMS */
    real_t var;                         /* Variance */
    real_t dev;                         /* Ecart-type */
    real_t coeff;                       /* Coefficient de variance */
    real_t skew;                        /* Skewness */
    real_t kurt;                        /* Kurtosis */
} stat_t;

typedef struct _fit_t {
    long_t id;                          /* ID, unique */
    int_t index;                        /* Function index */
    char_t name[ML_STRSIZES];           /* Name, unique */
    char_t label[ML_STRSIZES];          /* Label */
    char_t function[ML_STRSIZE];
    char_t par[ML_STRSIZES];
    int_t parcount;
    real_t fpar[ML_FIT_MAXPAR];
    real_t resd[ML_FIT_MAXPAR];
    int_t mpar[ML_FIT_MAXPAR];
    real_t fopt[ML_FIT_MAXPAR];
    int_t iter;
    real_t lambda;
    real_t chi;
    byte_t status;                       /* Status: convergence, ... */
    int_t points;                        /* Number of points */
} fit_t;

#define ML_FIT_MAXPAR_LEGACY 8
typedef struct _fit_legacy_t {
    long_t id;                            /* ID, unique */
    int_t index;                          /* Function index */
    char_t name[ML_STRSIZES];             /* Name, unique */
    char_t label[ML_STRSIZES];            /* Label */
    char_t function[ML_STRSIZE];
    char_t par[ML_STRSIZES];
    int_t parcount;
    real_t fpar[ML_FIT_MAXPAR_LEGACY];
    real_t resd[ML_FIT_MAXPAR_LEGACY];
    int_t mpar[ML_FIT_MAXPAR_LEGACY];
    real_t fopt[ML_FIT_MAXPAR_LEGACY];
    int_t iter;
    real_t lambda;
    real_t chi;
    byte_t status;                        /* Status: convergence, ... */
    int_t points;                         /* Number of points */
} fit_legacy_t;

// fonction de fit d�finie par l'utilisateur
typedef struct _userfit_t {
    long_t id;                            // ID, unique
    int_t index;                          // Function index
    char_t name[ML_STRSIZES];             // Name, unique
    int_t parcount;                       // Nombre de param�tres de fit
    char_t function[ML_STRSIZE];          // Fonction (exemple: "a + b*x")
    real_t fpar[ML_FIT_MAXPAR];           // Valeurs par d�faut des param�tres de fit
    // D�riv�e partielle par rapport au param�tre
    // num�ro jj (jj = 0 correspond � 'a', jj = 1 � 'b' ...)
    char_t derivative[ML_FIT_MAXPAR][ML_STRSIZE];
} userfit_t;

//    ------------------------------------------------------------------------------------------------------------- 

//    ------------------------------------------------------------------------------------------------------------- 
/*    VARIABLE D'INTERACTION */
extern const int_t ml_version;                        /* Version de libmath */
extern const int_t ml_debug;                          /* Variable de d�boguage */
extern char_t ml_linefmt[ML_STRSIZES];                /* Format d'une ligne de fichier ascii : "%lf\t%lf\n" , "%.6e\t%.6e\n", ... */
extern byte_t ml_angleunit;                           /* Unit� des angles : 0x00:radians, 0x01:degr�s; 0x02:grad */
extern char_t ml_format_f[ML_STRSIZET];               /* Format num�rique flottant */
extern char_t ml_format_e[ML_STRSIZET];               /* Format num�rique exponentiel */
extern char_t ml_format_i[ML_STRSIZET];               /* Format num�rique entier */

extern fit_t ml_fitmodel[ML_FIT_MAXMODEL];            /* Mod�les de fit disponibles */

typedef struct _libvar_t {
    int_t errcode;                                    /* Variable d'erreur */
    int_t running;                                    /* Variable d'ex�cution */
    int_t progress;                                   /* Variable de progression (entre 0 et 100) */
    char_t message[ML_STRSIZE];                       /* Message (erreur, info, ...) */
    byte_t interact;                                  /* Variable d'interaction */
    byte_t console;                                   /* Mode Console (pour printf) */

    int_t limitc;                                     /* Conditions aux limites (0=Dirichlet, 1=Neumann, ...) */
    int_t autoinit;                                   /* Calcul automatique des conditions initiales */

    int_t solver;                                     /* M�thode de r�solution utilis�e par ml_fzero, el_poisson, ... */
    int_t    linsolver;                               /* Algorithme utilis� pour r�soudre le syst�me lin�aire
                                                         r�sultant (0 = LU, 1 = SOR, 2 = Jacobi, 3 = Gauss) */

    int_t maxiter;                                    /* Nombre maximal d'it�rations */
    real_t tol;                                       /* Pr�cision */
    real_t reltol;                                    /* Pr�cision relative demand�e */
    real_t relerr;                                    /* Pr�cision relative trouv�e */
    real_t exptail;                                   /* Seuil de d�tection des variations exponentielles */
} libvar_t;


/* ####################################################################
    Fonctions d'interaction
    Fichier : libmath.c
    #################################################################### */

void ml_print(const char_t *pszOut);

/*  --------------------------------------------------------------------
    ml_libvar_copy    :    Copier les donn�es d'une librairie

        pfrom, pto    :    pointeur vers les couches � et vers qui copier resp.
    -------------------------------------------------------------------- */
void ml_libvar_copy(const libvar_t *pfrom, libvar_t *pto);

/*  --------------------------------------------------------------------
    ml_event            :    DoEvent
    -------------------------------------------------------------------- */
void ml_event(void);

/*  --------------------------------------------------------------------
    ml_reset            :    Remise � z�ro.
    -------------------------------------------------------------------- */
void ml_reset(libvar_t *pLib);

void ml_gettime(date_t* tm);


/* ####################################################################
    Parser math�matique
    Fichier : parser.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_parser_decode    :    d�coder une expression
    
        src             :    cha�ne � d�coder (e.g. "1 - exp(-x)")
        args            :    arguments (e.g. "x", "xy", "xyz")
        len             :    pointeur sur la variable qui contiendra la longueur
                                de l'expression d�cod�e

        Retourne l'expression d�cod�e (allou�e dynamiquement, et donc
        � lib�rer apr�s utilisation)
    -------------------------------------------------------------------- */
char_t *ml_parser_decode(char_t *src, char_t *args, int_t *len, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_parser_free       :    lib�re l'expression d�cod�e allou�e dynamiquement
                              par ml_parser_decode
        pFunction        :    expression d�cod�e par ml_parser_decode
    -------------------------------------------------------------------- */
void ml_parser_free(char_t *pFunction);

/*  --------------------------------------------------------------------
    ml_parser_evalfx    :    �valuer une expression
    
        fun             :    expression d�cod�e par ml_parser_decode
        x               :    argument

        Retourne la valeur de l'expression en x
    -------------------------------------------------------------------- */
real_t ml_parser_evalfx(char_t *fun, real_t x, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_parser_evalf    :    �valuer une expression
    
        fun            :    expression d�cod�e par ml_parser_decode
        args           :    liste des arguments (e.g. "x", "xy", "xyz")

        Retourne la valeur de l'expression
    -------------------------------------------------------------------- */
real_t ml_parser_evalf(libvar_t *pLib, char_t *fun, char_t *args, ...);

/*  --------------------------------------------------------------------
    ml_remspace        :    enl�ve tous les espaces dans une cha�ne
    
        szBuffer       :    cha�ne � traiter
        
        Retourne le nombre d'espaces enlev�s
    -------------------------------------------------------------------- */
int_t ml_remspace(char_t *szBuffer);

/*  --------------------------------------------------------------------
    ml_getformat      :    obtenir le pr�fixe et suffixe d'un format num�rique
    
        szFmt         :    cha�ne du format (exemple: %.3f, 2.3%e, ...)
        prefix        :    contiendra en sortie le pr�fixe
        suffix        :    contiendra en sortie le suffixe

        Retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_getformat(const char_t* szFmt, int_t *prefix, int_t *suffix);

/*  --------------------------------------------------------------------
    ml_reformat        :    post-formatage des nombres r�els (%f , %e)
    
        szBuffer       :    cha�ne obtenue avec _tcsprintf (%f ou %e)
        
        Retourne la longueur de la chaine reformat�e
    -------------------------------------------------------------------- */
int_t ml_reformat(char_t *szBuffer);

/*  --------------------------------------------------------------------
    ml_reformat_auto    :    formatage automatique des nombres r�els (%f , %e)

        fVal            :    nombre r�el � formater
        szBuffer        :    cha�ne obtenue avec _tcsprintf avec %f ou %e
        nCount          :    taille de la cha�ne format�e
        pszFormat       :    contiendra en sortie le format
        pFmt            :    contiendra en sortie le type de format (0 ou 1)

        Retourne la longueur de la chaine reformat�e
    -------------------------------------------------------------------- */
int_t ml_reformat_auto(real_t fVal, char_t* szBuffer, int_t nCount, char_t* pszFormat, int_t *pFmt, libvar_t *pLib);

/* ####################################################################
    Vecteurs et Matrices
    Fichier : matrix.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_vector_create       :    cr�er un vecteur
    
        pvec               :    pointera sur le vecteur cr��e
                                (pvec->dat doit �tre NULL
                                lors de l'appel � ml_vector_create)
        dim                :    dimension voulue
    -------------------------------------------------------------------- */
void ml_vector_create(vector_t *pvec, int_t dim, int_t id, const char_t* name, const vector_t *pvecd, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_dup        :    dupliquer un vecteur
    
    -------------------------------------------------------------------- */
void ml_vector_dup(vector_t *pvec, const vector_t *pvecd, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_dupstatic        :    dupliquer la partie statique d'un vecteur
    
    -------------------------------------------------------------------- */
void ml_vector_dupstatic(vector_t *pvec, const vector_t *pvecd, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_validate        :    v�rifier la partie statique d'un vecteur
        retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_vector_validate(vector_t *pvec);

/*  --------------------------------------------------------------------
    ml_vector_delete    :    d�truire un vecteur
    
        pvec                :    pointeur sur le vecteur � d�truire
    -------------------------------------------------------------------- */
void ml_vector_delete(vector_t *pvec, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_add        :    ajouter un �l�ment � un vecteur
    
        pvec             :    pointeur sur le vecteur
        ee               :    �l�ment � ajouter
    -------------------------------------------------------------------- */
void ml_vector_add(vector_t *pvec, real_t ee, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_rem        :    d�truire un �l�ment du vecteur
    
        pvec             :    pointeur sur le vecteur
        ii               :    indice de l'�l�ment � d�truire
    -------------------------------------------------------------------- */
void ml_vector_rem(vector_t *pvec, int_t istart, int_t istop, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_insert    :    ins�rer un �l�ment dans le vecteur
    
        pvec            :    pointeur sur le vecteur
        ii              :    indice de l'�l�ment apr�s lequel ins�rer
        ee              :    �l�ment � ins�rer
    -------------------------------------------------------------------- */
void ml_vector_insert(vector_t *pvec, int_t ii, real_t ee, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_copy        :    copier un �l�ment du vecteur
    
        pvec              :    pointeur sur le vecteur
        ifrom             :    indice de l'�l�ment � copier
        ito               :    indice de l'�l�ment o� copier
    -------------------------------------------------------------------- */
void ml_vector_copy(vector_t *pvec, int_t ifrom, int_t ito, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_move        :    d�placer un �l�ment du vecteur
    
        pvec              :    pointeur sur le vecteur
        ifrom             :    indice de l'�l�ment � d�placer
        ito               :    indice de l'�l�ment o� d�placer
    -------------------------------------------------------------------- */
void ml_vector_move(vector_t *pvec, int_t ifrom, int_t ito, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_sort        :    ordonner une s�rie de vecteurs
    
        pvec              :    pointeur sur les vecteurs
        icount            :    nombre de vecteurs
        ipivot            :    indice du vecteur pivot
        plink                :    tableau indiquant les colonnes li�es
                                (�l�ment � 1 si li�e, 0 sinon)
        iasc                :    1 si ascendant, 0 si descendant
    -------------------------------------------------------------------- */
void ml_vector_sort(vector_t *pvec, int_t icount, int_t ipivot, byte_t iasc, byte_t *plink,
                          int_t istart, int_t istop, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_vector_rev        :    inverser un vecteur
    
        pvec             :    pointeur sur le vecteur
        dim              :    nombre d'�l�ments du vecteur
    -------------------------------------------------------------------- */
void ml_vector_rev(vector_t *pvec, int_t dim, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_writedat          :    sauvegarde de donn�es dans un fichier ascii
    
        filename         :    nom du fichier (cr�e s'il n'existe pas et �cras� sinon)
        x, y, z          :    pointeurs sur les donn�es � sauvegarder (trois colonnes
                              x, y et z, avec comme s�parateur la tabulation
        ncol             :    nombre de colonnes (1, 2 ou 3)
        ndat             :    nombre de points
    -------------------------------------------------------------------- */
int_t ml_writedat(const char_t* filename, real_t *x, real_t *y, real_t *z,
                        int_t ncol, int_t ndat, libvar_t *plib);


/* ####################################################################
    Recherche du z�ro d'une fonction
    Fichier : fzero.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_fzguess        :    trouver un intervalle dans lequel f(x)=0

        fun           :    pointeur sur f(x)
        a             :    pointeur sur l'estimation initiale (inf�rieure)
        b             :    pointeur sur l'estimation initiale (sup�rieure)
        Retourne 1 si OK
    -------------------------------------------------------------------- */
int_t ml_fzguess(real_t (*fun)(real_t), real_t *a, real_t *b, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fzero       :    r�solution de f(x) = 0 avec la m�thode d�finie
                        par la variable pLib->solver

        fun        :    pointeur sur f(x)
        a          :    estimation initiale (inf�rieure)
        b          :    estimation initiale (sup�rieure)
        Retourne la valeur estim�e de la solution
    -------------------------------------------------------------------- */
real_t ml_fzero(real_t (*fun)(real_t), real_t a, real_t b, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_amsterdam    :    r�solution de f(x) = 0 par l'algorithme d'Amsterdam
    
        fun         :    pointeur sur f(x)
        a, c        :    intervalle contenant la solution

        Retourne la valeur estim�e de la solution
    -------------------------------------------------------------------- */
real_t ml_amsterdam(real_t (*fun)(real_t), real_t a, real_t c, libvar_t *pLib);


/* ####################################################################
    R�solution de syst�mes matriciels lin�aires
    Fichier : linsys.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_linsys_gauss     :    r�solution de syst�me lin�aire : a x = b

        a, b            :    donn�es du syst�me lin�aire (modifi�es par ml_linsys_gauss)
        x               :    r�sultat
        n               :    dimension du syst�me
    -------------------------------------------------------------------- */
void ml_linsys_gauss(real_t **a, real_t *b, real_t *x, int_t n, libvar_t *pLib);

/* ####################################################################
    Fonctions de fit, m�thode des moindres carr�s
    Fichiers : fitfun.c, fitlm.c et fitlin.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_fit_init        :    Initialisation de la session de fit

        *niter         :    Nombre d'it�rations
        *lambda        :    Param�tre de descente (nul lorsque les d�riv�es partielles
                            sont nulles). 1E-12 < *lambda < 1E12
    -------------------------------------------------------------------- */
void ml_fit_init(int_t *iter, real_t *lambda);

/*  --------------------------------------------------------------------
    ml_fit_fun          :    Fonction de fit

        ifun = type de fonction

        xx              :    Valuer de x
        fpar            :    Param�tres de la fonction
        npar            :    Nombre de param�tres de la fonction
        fopt            :    Param�tres optionnels de la fonction

        Retourne la valeur de la fonction en x
    -------------------------------------------------------------------- */
real_t ml_fit_fun(int_t ifun, real_t xx, real_t *fpar, int_t npar, real_t *fopt);

/*  --------------------------------------------------------------------
    ml_fit_dfun         :    D�riv�e de la fonction de fit

        ifun = type de fonction

        xx              :    Valuer de x
        resd            :    R�sidu
        fpar            :    Param�tres de la fonction
        dpar            :    Contiendra les d�riv�es patielles des param�tres
        npar            :    Nombre de param�tres de la fonction
        fopt            :    Param�tres optionnels de la fonction
    -------------------------------------------------------------------- */
void ml_fit_dfun(int_t ifun, real_t xx, real_t resd, real_t *fpar, real_t *dpar, int_t npar, real_t *fopt, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fit_userinit    :    Initialiser la fonction de fit d�finie
                            par l'utilisateur

        pUserFit       :    Param�tre du fit

        Retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_fit_userinit(const userfit_t *pUserFit, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fit_userend        :    Terminer la session de fit d�finie
                               par l'utilisateur
    -------------------------------------------------------------------- */
void ml_fit_userend(void);

/*  --------------------------------------------------------------------
    ml_fit_userfun      :    Fonction de fit d�finie par l'utilisateur

        x               :    Valuer de x
        fpar            :    Param�tres de la fonction

        Retourne la valeur de la fonction en x
    -------------------------------------------------------------------- */
real_t ml_fit_userfun(real_t x, real_t *fpar);

/*  --------------------------------------------------------------------
    ml_fit_userderiv    :    D�riv�es patielles de la fonction de fit d�finie
                                par l'utilisateur

        x               :    Valuer de x
        fpar            :    Param�tres de la fonction
        dpar            :    Contiendra les d�riv�es patielles des param�tres
    -------------------------------------------------------------------- */
void ml_fit_userderiv(real_t x, real_t *fpar, real_t *dpar);

/*  --------------------------------------------------------------------
    ml_fit_autopar        :    Estimer les param�tres de fit

        pvecX             :    Vecteur de donn�es x
        pvecY             :    Vecteur de donn�es y
        fpar              :    Param�tres qui seront estim�s

        Retourne 1 si une estimation a �t� trouv�e, et 0 sinon.
    -------------------------------------------------------------------- */
int_t ml_fit_autopar(vector_t *pvecX, vector_t *pvecY, int_t ifun, real_t *fpar, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fit_findpeaks    :    Recherche de tous les pics

        Entr�es:
            pvecX       :    Vecteur de donn�es x
            pvecY       :    Vecteur de donn�es y
            peaknum     :    Nombre de pics � chercher

        Sorties:
            offset      :    contiendra les valeurs estim�es des offsets des pics
                            (offset[0], offset[1], ...)
            ampl        :    contiendra les valeurs estim�es des amplitudes des pics
                            (ampl[0], ampl[1], ...)
            pos         :    contiendra les valeurs estim�es des positions des pics
                            (pos[0], pos[1], ...)
            width       :    contiendra les valeurs estim�es des largeurs des pics
                            (width[0], width[1], ...)

        Retourne le nombre de pics trouv�s
    -------------------------------------------------------------------- */
int_t ml_fit_findpeaks(vector_t *pvecX, vector_t *pvecY, int_t peaknum, real_t *offset,
                              real_t *ampl, real_t *pos, real_t *width, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fit_process        :    Ligne de base et lissage

        Entr�es:
        pvecX             :    Vecteur de donn�es (abscisses)
        pvecY             :    Vecteur de donn�es (ordonn�es)

        pxm               :    Mask des donn�es (abscisses)
        pym               :    Mask des donn�es (ordonn�es)
        fmin              :    Minimun pour le mask
        fmax              :    Maximun pour le mask

        fbaseline0        :    Ligne de base y = fbaseline0 + fbaseline1 * x
        fbaseline1        :    Ligne de base y = fbaseline0 + fbaseline1 * x
        bSmooth           :    Lissage (=1 si lissage)
        nSmooth           :    Degr� du lissage

        Retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_fit_process(vector_t *pvecX, vector_t *pvecY,
                            byte_t bMask, real_t fMin, real_t fMax,
                            byte_t bBaseline, real_t fbaseline0, real_t fbaseline1,
                            byte_t bSmooth, int_t nSmooth, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fit_levmarq      :    Fit non-lin�aire
                            (variante de l'algorithme de Levenberg-Marquardt)

        ifun = type de fonction

        xdat            :    Donn�es (abscisses)
        ydat            :    Donn�es (ordonn�es)
        wdat            :    Poids des donn�es
        ndat            :    Nombre de points
        fpar            :    Contient en entr�e une estimation des param�tres et
                             en sortie la valeur calcul�e de ces param�tres
        epar            :    Contient une estimation des erreurs
        mpar            :    Masque logique (mpar[i] = 1 -> param�tre libre, 0 = fixe)
        npar            :    Nombre de param�tres
        fopt            :    Param�tres optionnels
        niter           :    Nombre d'it�rations
        lambda0         :    Param�tre de descente (nul lorsque les d�riv�es partielles
                             sont nulles). 1E-15 < lambda0 < 1E15

        Retourne la valeur de chi2
    -------------------------------------------------------------------- */
real_t ml_fit_levmarq(int_t ifun, real_t *xdat, real_t *ydat, real_t *wdat, int_t ndat, real_t *fpar,
                             real_t *epar, int_t *mpar, int_t npar, real_t *fopt, int_t *niter,
                             real_t *lambda0, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fit_linear       :    Regression lin�aire y = a + bx

        xdat            :    Donn�es (abscisses)
        ydat            :    Donn�es (ordonn�es)
        ndat            :    Nombre de points
        a               :    Contient en sortie la valeur calcul�e de a
        b               :    Contient en sortie la valeur calcul�e de b

        Retourne la valeur du coeff. de corr�lation (id�alement = 1)
    -------------------------------------------------------------------- */
real_t ml_fit_linear(real_t *xdat, real_t *ydat, int_t ndat, real_t *a, real_t *b, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_fit_getmodel    :    Copie de mod�les de fit

        pfit           :    Tableau de mod�les � initialiser (pointeur)
        fitcount       :    Nombre d'�l�ments du tableau de mod�les
    -------------------------------------------------------------------- */
void ml_fit_getmodel(fit_t *pfit, int_t fitcount);

/*  --------------------------------------------------------------------
    ml_fit_getmodel    :    Copie d'un mod�le de fit

    pfit               :    Mod�le � initialiser (pointeur)
    pszModel           :    Nom du mod�le
    -------------------------------------------------------------------- */
int_t ml_fit_initmodel(fit_t *pfit, const char_t *pszModel);

/*  --------------------------------------------------------------------
    ml_fit_dup        :    Dupliquer un fit

        pTo           :    pointeur sur la structure source
        pFrom         :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_fit_dup(fit_t *pTo, const fit_t *pFrom);

/*  --------------------------------------------------------------------
    ml_fit_fromlegacy :    Dupliquer un fit

    pTo               :    pointeur sur la structure source
    pFrom             :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_fit_fromlegacy(fit_t *pTo, const fit_legacy_t *pFrom);

/*  --------------------------------------------------------------------
    ml_fit_tolegacy    :    Dupliquer un fit

    pTo                :    pointeur sur la structure source
    pFrom              :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_fit_tolegacy(fit_legacy_t *pTo, const fit_t *pFrom);


/* ####################################################################
    Fonctions diverses et vari�es
    Fichiers : specfun.c, poly.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_erf :    Calcul de la fonction erf(x)
                D'apr�s l'algorithme de Cody :
                W. J. Cody,
                "Rational Chebyshev approximations for the error function",
                Math. Comp., 1969, PP. 631-638.
    -------------------------------------------------------------------- */
real_t ml_erf(real_t x);

/*  --------------------------------------------------------------------
    ml_bern    :    calcul de la fonction de Bernoulli B(x) = x / (exp(x) - 1)
    
        x      :    argument de la fonction

        Retourne la valeur de B(x)
    -------------------------------------------------------------------- */
real_t ml_bern(real_t x);

/*  --------------------------------------------------------------------
    ml_dbern    :    calcul de la d�riv�e de la fonction de Bernoulli

        x       :    argument de la fonction

        Retourne la valeur de la d�riv�e de B(x)
    -------------------------------------------------------------------- */
real_t ml_dbern(real_t x, real_t delta);

/*  --------------------------------------------------------------------
    ml_gauss     :    calcul de la fonction de Gauss G(x) = exp(-(x - b)� / 2c�)
    
        x        :    argument de la fonction
        b        :    abscisse du pic
        c        :    �cart-type

        Retourne la valeur de G(x)
    -------------------------------------------------------------------- */
real_t ml_gauss(real_t x, real_t b, real_t c);

/*  --------------------------------------------------------------------
    ml_lorentz    :    calcul de la fonction de Lorentz
                        L(x) = ((1/pi) * c / ((x - b)� + c�))
    
        x         :    argument de la fonction
        b         :    abscisse du pic
        c         :    �cart-type

        Retourne la valeur de L(x)
    -------------------------------------------------------------------- */
real_t ml_lorentz(real_t x, real_t b, real_t c);

/*  --------------------------------------------------------------------
    ml_gammaln    :    calcul du logarithme de la fonction Gamma
    
        x         :    argument de la fonction

        Retourne la valeur de ln(Gamma(x))
    -------------------------------------------------------------------- */
real_t ml_gammaln(real_t x);

/*  --------------------------------------------------------------------
    ml_beta        :    calcul de la fonction Beta
                        B(z,w) = Gamma(z) * Gamma(w) / Gamma(z + w)
    
        x          :    argument de la fonction

        Retourne la valeur de ln(Gamma(x))
    -------------------------------------------------------------------- */
real_t ml_beta(real_t z, real_t w);

/*  --------------------------------------------------------------------
    ml_sort         :    ordonner une s�rie num�rique
    
        pdat        :    pointeur sur les donn�es � modifier
        ndat        :    nombre de points
        iasc        :    ordre ascendant si 1 et descendant si 0

        Retourne le pointeur sur les donn�es ordonn�es si OK ou NULL sinon
    -------------------------------------------------------------------- */
real_t* ml_sort(real_t *pdat, int_t ndat, byte_t iasc, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_reverse        :    inverser des donn�es
    
        pdat          :    pointeur sur les donn�es
        ndat          :    nombre de points
-------------------------------------------------------------------- */
void ml_reverse(real_t *pdat, int_t ndat, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_stats          :        calcul des statistiques
    
        pdat          :    pointeur sur les donn�es
        ndat          :    nombre de points
        pstats        :    pointeur sur la structure qui contiendra
                        les statistiques

        Retourne le pointeur pstats si OK ou NULL sinon
    -------------------------------------------------------------------- */
stat_t* ml_stats(const real_t *pdat, int_t ndat, stat_t* pstats, libvar_t *pLib);

/*  --------------------------------------------------------------------
    ml_stat_dup        :    Dupliquer des statistiques

        pTo            :    pointeur sur la structure source
        pFrom          :    pointeur sur la structure destination
    -------------------------------------------------------------------- */
int_t ml_stat_dup(stat_t *pTo, const stat_t *pFrom);


/* ####################################################################
    Fonctions sur les entiers
    Fichier : integer.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_fact        :    calculer la factorielle de n

        x          :    entier
        Retourne la factorielle de x
    -------------------------------------------------------------------- */
real_t ml_fact(real_t x);


/* ####################################################################
    Fonctions sur les r�els
    Fichier : real.c
    #################################################################### */

/*  --------------------------------------------------------------------
    ml_toreal         :    Convesion d'une cha�ne de caract�re en r�el

        szInput       :    Cha�ne � convertir
        pError        :    Pointeur vers l'erreur (0 si OK, 1 sinon)
        pszEnd        :    Pointeur sur la fin de la cha�ne convertie

        Retourne la valeur convertie
    -------------------------------------------------------------------- */
real_t ml_toreal(const char_t *szInput, byte_t *pError, char_t **pszEnd);

/*  --------------------------------------------------------------------
    ml_roundinf       :    Arrondir un r�el � la valeur inf�rieure
        fval          :    Pointeur vers la valeur � arrondir

        Retourne la valeur arrondie
    -------------------------------------------------------------------- */
real_t ml_roundinf(real_t *fval);

/*  --------------------------------------------------------------------
    ml_roundsup        :    Arrondir un r�el � la valeur sup�rieure
        fval           :    Pointeur vers la valeur � arrondir

        Retourne la valeur arrondie
    -------------------------------------------------------------------- */
real_t ml_roundsup(real_t *fval);

/*  -------------------------------------------------------------
    lm_round            :    Arrondir un r�el � la valeur inf�rieure
        fVal            :    Pointeur vers la valeur � arrondir

    Retourne la valeur arrondie
------------------------------------------------------------- */
real_t ml_round(double *fVal, int bUp);

/* ####################################################################
    Traitement du signal
    Fichier : signal.c
    #################################################################### */

int_t ml_fft(real_t *pdat, int_t dim, byte_t idirect);

int_t ml_autocorr(real_t *pin, real_t *pout, int_t dim);

#endif