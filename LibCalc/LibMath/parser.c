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

static int_t fp_finditem(char_t *name);
static real_t fp_value(char_t *fun, libvar_t *pLib);
static int_t fp_isop(char_t c);
static int_t fp_iscode(char_t c);
static int_t fp_isreal(char_t c);
static size_t fp_maxsize(char_t *src);
static char_t *fp_strtok(char_t *s);
static char_t *fp_decode(char_t *fun, char_t *begin, char_t *end);

static real_t const_k(void);
static real_t const_q(void);
static real_t const_h(void);
static real_t const_c(void);
static real_t const_e(void);
static real_t const_n(void);
static real_t const_m(void);

static real_t fp_pi(void);

static real_t fp_sin(real_t x);
static real_t fp_cos(real_t x);
static real_t fp_tan(real_t x);
static real_t fp_asin(real_t x);
static real_t fp_acos(real_t x);
static real_t fp_atan(real_t x);

static real_t fp_min(real_t a, real_t b);
static real_t fp_max(real_t a, real_t b);
static real_t fp_rand(real_t fsrand);
static real_t fp_time(void);
static real_t fp_sign(real_t x);
static real_t fp_log2(real_t x);

static real_t fp_jn(real_t n, real_t x);
static real_t fp_yn(real_t n, real_t x);

static real_t fp_param[_T('z') - _T('a') + 1];

typedef struct _item_t {
    char_t *name;                /* Nom de la fonction */
    void *fun;                    /* Pointeur sur la fonction ... */
    int_t npar;                    /* ... et son nombre de paramètres */
} item_t;

static struct _item_t fp_table[] = {

    // Librairie mathématique C standard
    {_T("exp"),             (void*)exp,                1},
    {_T("ln"),              (void*)log,                1},
    {_T("log"),             (void*)log10,              1},
    {_T("log2"),            (void*)fp_log2,            1},
    {_T("pow"),             (void*)pow,                2},
    {_T("sin"),             (void*)fp_sin,             1},
    {_T("cos"),             (void*)fp_cos,             1},
    {_T("tan"),             (void*)fp_tan,             1},
    {_T("asin"),            (void*)fp_asin,            1},
    {_T("acos"),            (void*)fp_acos,            1},
    {_T("atan"),            (void*)fp_atan,            1},
    {_T("sinh"),            (void*)sinh,               1},
    {_T("cosh"),            (void*)cosh,               1},
    {_T("tanh"),            (void*)tanh,               1},
    {_T("abs"),             (void*)fabs,               1},
    {_T("sqrt"),            (void*)sqrt,               1},
    {_T("ceil"),            (void*)ceil,               1},
    {_T("floor"),           (void*)floor,              1},
    {_T("int"),             (void*)floor,              1},
    {_T("fmod"),            (void*)fmod,               2},
    {_T("j0"),              (void*)_j0,                1},
    {_T("j1"),              (void*)_j1,                1},
    {_T("jn"),              (void*)fp_jn,              2},
    {_T("y0"),              (void*)_y0,                1},
    {_T("y1"),              (void*)_y1,                1},
    {_T("yn"),              (void*)fp_yn,              2},
    {_T("hypot"),           (void*)_hypot,             2},
    {_T("pi"),              (void*)fp_pi,              0},
    {_T("min"),             (void*)fp_min,             2},
    {_T("max"),             (void*)fp_max,             2},
    {_T("rand"),            (void*)fp_rand,            1},
    {_T("time"),            (void*)fp_time,            0},
    {_T("sign"),            (void*)fp_sign,            1},
    {_T("exp2"),            (void*)exp2,               1},
    {_T("log2"),            (void*)log2,               1},
    {_T("cbrt"),            (void*)cbrt,               1},
    {_T("lgamma"),          (void*)lgamma,             1},
    {_T("tgamma"),          (void*)tgamma,             1},
    {_T("trunc"),           (void*)trunc,              1},
    {_T("round"),           (void*)round,              1},
    {_T("rint"),            (void*)rint,               1},
    {_T("asinh"),           (void*)asinh,              1},
    {_T("acosh"),           (void*)acosh,              1},
    {_T("atanh"),           (void*)atanh,              1},
    {_T("erf"),             (void*)erf,                1},
    {_T("erfc"),            (void*)erfc,               1},

    // Physical constants
    {_T("_k"),               (void*)const_k,           0},
    {_T("_q"),               (void*)const_q,           0},
    {_T("_h"),               (void*)const_h,           0},
    {_T("_c"),               (void*)const_c,           0},
    {_T("_e"),               (void*)const_e,           0},
    {_T("_n"),               (void*)const_n,           0},
    {_T("_m"),               (void*)const_m,           0},
    {_T("kB"),               (void*)const_k,           0},
    {_T("e0"),               (void*)const_e,           0},
    {_T("NA"),               (void*)const_n,           0},
    {_T("me"),               (void*)const_m,           0},

    // Special functions
    {_T("erf"),              (void*)ml_erf,            1},
    {_T("bern"),             (void*)ml_bern,           1},
    {_T("dbern"),            (void*)ml_dbern,          2},
    {_T("fact"),             (void*)ml_fact,           1},
    {_T("gauss"),            (void*)ml_gauss,          3},
    {_T("lorentz"),          (void*)ml_lorentz,        3},
    {_T("gammaln"),          (void*)ml_gammaln,        1},
    {_T("beta"),             (void*)ml_beta,           2},

    {NULL,                   NULL,                     0}
};

/*  -------------------------------------------------------------------- */
/* Fonctions exportées */

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_parser_decode    :    décoder une expression
    
        src             :    chaîne à décoder (e.g. "1 - exp(-x)")
        args            :    arguments (e.g. "x", "xy", "xyz")
        len             :    pointeur sur la variable qui contiendra la longueur
                             de l'expression décodée

        Retourne l'expression décodée (allouée dynamiquement, et donc
            à libérer après utilisation)
    -------------------------------------------------------------------- */
char_t *ml_parser_decode(char_t *src, char_t *args, int_t *len, libvar_t *pLib)
{
    char_t *result, *scan, *scarg, *fun, *nfunc;
    size_t fsize;

    if (pLib->errcode) { return NULL; }

    pLib->errcode = 1;

    for (scan = src; *scan != _T('\0'); scan++) {
        if (iswlower(*scan) && !iswalpha(*(scan + 1)) && ((scan == src) || !iswalpha(*(scan - 1))) ) {
            for (scarg = args; (*scarg != _T('\0')) && (*scarg != *scan); scarg++) { ; }
            if (*scarg == _T('\0')) {
                *len = 0;
                return NULL;
            }
        }
    }
    fsize = fp_maxsize(src);
    if (!(fun = (char_t*)malloc(fsize * sizeof(char_t)))) {
        *len = 0;
        return NULL;
    }

    result = fp_decode(fun, src, src + _tcslen(src));

    if (!result) {
        free(fun);
        *len = 0;
        return NULL;
    }
    else {
        *result = _T('\0');
        *len = (int_t)(result - fun);

        if (((*len) + 1) < (int_t)fsize) {
            nfunc = (char_t *)malloc(((*len) + 1) * sizeof(char_t));
            if (nfunc) {
                wmemcpy(nfunc, fun, ((*len) + 1));
                free(fun);
                fun = nfunc;
            }
            pLib->errcode = 0;
            return fun;
        }
    }

    *len = 0;
    return NULL;
}

void ml_parser_free(char_t *pFunction)
{
    if (pFunction) {
        free(pFunction); pFunction = NULL;
    }
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_parser_evalfx    :    évaluer une expression
    
        fun             :    expression décodée par ml_parser_decode
        x               :    argument

        Retourne la valeur de l'expression en x
    -------------------------------------------------------------------- */
real_t ml_parser_evalfx(char_t *fun, real_t x, libvar_t *pLib)
{
    fp_param[_T('x')-_T('a')] = x;

    return fp_value(fun, pLib);
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_parser_evalf    :    évaluer une expression
    
        fun            :    expression décodée par ml_parser_decode
        args           :    liste des arguments (e.g. "x", "xy", "xyz")

        Retourne la valeur de l'expression
    -------------------------------------------------------------------- */
real_t ml_parser_evalf(libvar_t *pLib, char_t *fun, char_t *args, ...)
{
    va_list ap;
    real_t result;

    va_start(ap, args);
    while (*args) {
        fp_param[(*args++) - _T('a')] = va_arg(ap, real_t);
    }
    va_end(ap);
    result = fp_value(fun, pLib);

    return result;
}
/*  -------------------------------------------------------------------- */

int_t fp_finditem(char_t *name)
{
    item_t *table_p;

    for (table_p = fp_table; (table_p->fun != NULL) && _tcscmp(name,table_p->name); table_p++) { ; }
    if (table_p->fun == NULL) {
        return -1;
    }

    return (int_t)(table_p - fp_table);
}

real_t fp_value(char_t *fun, libvar_t *pLib)
{
    real_t buffer[1024];
    real_t *bufp = buffer;
    real_t x, y, z;
    real_t result;
    int_t ires;

    if (pLib->errcode) { return 0.0; }

    pLib->errcode = 1;

    if (!fun) return 0.0;

    for (;;) {
        switch (*fun++) {
            case _T('\0'):
                goto fp_value_RET;
            case _T('D'):
                *(bufp++) = *((real_t *) fun)++;
                break;
            case _T('V'):
                *(bufp++) = fp_param[(*(fun++)) - _T('a')];
                break;
            case _T('M'):
                result = -(*(--bufp));
                *(bufp++) = result;
                break;
            case _T('+'):
                y = *(--bufp);
                result = y + *(--bufp);
                *(bufp++) = result;
                break;
            case _T('-'):
                y = *(--bufp);
                result = *(--bufp) - y;
                *(bufp++) = result;
                break;
            case _T('*'):
                y = *(--bufp);
                result = *(--bufp) * y;
                *(bufp++) = result;
                break;
            case _T('/'):
                y = *(--bufp);
                result = *(--bufp) / y;
                *(bufp++) = result;
                break;
            case _T('%'):
                y = *(--bufp);
                ires = (int_t)(*(--bufp)) % (int_t)y;
                *(bufp++) = (real_t)ires;
                break;
            case _T('^'):
                y = *(--bufp);
                result = pow(*(--bufp),y);
                *(bufp++) = result;
                break;
            case _T('F'):
                switch (fp_table[*fun].npar) {
                    case 0:
                        *(bufp++) = (*((real_t (*)(void))fp_table[*fun++].fun))();
                        break;
                    case 1:
                        x = *(--bufp);
                        *(bufp++) = (*((real_t (*)(real_t))fp_table[*fun++].fun))(x);
                        break;
                    case 2:
                        y = *(--bufp);
                        x = *(--bufp);
                        *(bufp++) = (*((real_t (*)(real_t,real_t))fp_table[*fun++].fun))(x,y);
                        break;
                    case 3:
                        z = *(--bufp);
                        y = *(--bufp);
                        x = *(--bufp);
                        *(bufp++) = (*((real_t (*)(real_t, real_t, real_t))fp_table[*fun++].fun))(x,y,z);
                        break;
                    default:
                        return 0.0;
                }
                break;
            default:
                return 0.0;
        }
    }

fp_value_RET:
    pLib->errcode = 0;
    return buffer[0];
}

static int_t fp_isop(char_t c)
{
    return ((c == _T('+')) || (c == _T('-')) || (c == _T('*')) || (c == _T('/')) || (c == _T('%')) || (c == _T('^')));
}

static int_t fp_iscode(char_t c)
{
    return ((c == _T('+')) || (c == _T('-')) || (c == _T('*')) || (c == _T('/')) || (c == _T('%')) || (c == _T('^')) || (c == _T('M')));
}

static int_t fp_isreal(char_t c)
{
    return (iswdigit(c) || (c == _T('.')) || (c == _T('E')));
}

size_t fp_maxsize(char_t *src)
{
    int_t numbers = 0;
    int_t functions = 0;
    int_t operators = 0;
    int_t variables = 0;

    const size_t funsize = 2;
    const size_t varsize = 2;
    const size_t numsize = sizeof(char_t) + sizeof(real_t);
    const size_t opsize = sizeof(char_t);
    const size_t endsize = sizeof(_T('\0'));

    char_t *scan;

    for (scan = src; *scan; scan++) {
        if (iswalpha(*scan) && (*scan != _T('E'))) {
            if (iswalpha(*(scan + 1))) { ; }
            else if (*(scan + 1) == _T('('))  { functions++; }
            else { variables++; }
        }
    }

    if (fp_isop(*src)) { operators++; }
    if (*src != _T('\0')) {
        for (scan = src + 1; *scan; scan++) {
            if (fp_isop(*scan) && (*(scan - 1) != _T('E'))) { operators++; }
        }
    }

    scan = src;
    while (*scan) {
        if (fp_isreal(*scan) || (((*scan == _T('+')) || (*scan == _T('-')))
            && (scan > src) && (*(scan - 1) == _T('E')))) {
                numbers++;
                scan++;
                while (fp_isreal(*scan) || (((*scan == _T('+')) || (*scan == _T('-')))
                    && (scan > src) && (*(scan - 1) == _T('E')))) {
                    scan++;
                }
        }
        else scan++;
    }

    return ((numbers * numsize) + (operators * opsize) + (functions * numsize)
        + (variables * varsize) + endsize);
}

static char_t *fp_strtok(char_t *s)
{
    int_t pars;
    static char_t *token = NULL;
    char_t *nexttoken;

    if (s != NULL) { token = s; }
    else if (token != NULL) { s = token; }
    else return NULL;

    for (pars = 0; (*s != _T('\0')) && ((*s != _T(',')) || (pars != 0)); s++) {
        if (*s == _T('(')) { ++pars; }
        if (*s == _T(')')) { --pars; }
    }
    if (*s == _T('\0')) {
        nexttoken = NULL;
        s = token;

        token = nexttoken;
        return s;
    }
    else {
        *s = _T('\0');
        nexttoken = s+1;
        s = token;

        token=nexttoken;
        return s;
    }
}

#define FP_TWOOP {                                                      \
    if ((tempu = fp_decode(fun, begin, scan))                           \
        &&    (tempv = fp_decode(tempu, scan+1, end))) {                \
        *tempv++ = *scan;                                               \
        return tempv;                                                   \
    } else { return NULL; }                                             \
}

static char_t *fp_decode(char_t *fun, char_t *begin, char_t *end)
{
    int_t i, pars, nfun, space;
    char_t *scan, *temps, tempch, *endf, *paramstr[3], *parbuf;
    char_t *tempu, *tempv;
    real_t tempd;

    if (begin >= end) {
        return NULL;
    }

    for (pars = 0, scan = begin; (scan < end) && (pars >= 0); scan++) {
        if (*scan == _T('(')) { pars++; }
        else if (*scan == _T(')')) { pars--; }
    }
    if (pars != 0) {
        return NULL;
    }

    for (pars = 0, scan = end - 1; scan >= begin; scan--) {
        if (*scan == _T('(')) { pars++; }
        else if (*scan == _T(')')) { pars--; }
        else if (!pars && ((*scan == _T('+')) || ((*scan == _T('-')) && (scan != begin)))
            && ((scan == begin) || (*(scan-1) != _T('E')))) {
            break;
        }
    }

    if (scan >= begin) FP_TWOOP

    for (pars = 0, scan = end - 1; scan >= begin; scan--) {
        if (*scan == _T('(')) { pars++; }
        else if (*scan == _T(')')) { pars--; }
        else if (!pars && ((*scan == _T('*')) || (*scan == _T('/')) || (*scan == _T('%')))) {
            break;
        }
    }

    if (scan >= begin) FP_TWOOP

    for (pars = 0, scan = end-1; scan >= begin; scan--) {
        if (*scan == _T('(')) { pars++; }
        else if (*scan == _T(')')) { pars--; }
        else if (!pars && (*scan == _T('^'))) {
            break;
        }
    }

    if (scan >= begin) FP_TWOOP

    if (*begin == _T('-')) {
        tempu = fp_decode(fun, begin + 1, end);
        if (tempu) {
            *tempu++ = _T('M');
            return tempu;
        } else { return NULL; }
    }

    while (_istspace(*begin)) { begin++; }
    while (_istspace(*(end - 1))) { end--; }

    if ((*begin == _T('(')) && (*(end - 1) == _T(')'))) {
        return fp_decode(fun, begin+1, end-1);
    }

    if ((end == (begin + 1)) && iswlower(*begin)) {
        *fun++ = _T('V');
        *fun++ = *begin;
        return fun;
    }

    tempch = *end;
    *end = _T('\0');
    tempd = ml_toreal(begin, NULL, (char_t**)&tempu);
    *end = tempch;
    if ((char_t*) tempu == end) {
        *fun++ = _T('D');
        *((real_t*)fun)++ = tempd;
        return fun;
    }

    if (!iswalpha(*begin) && (*begin != _T('_'))) {
        return NULL;
    }
    for (endf = begin + 1; (endf < end) && (_istalnum(*endf) || (*endf == _T('_'))); endf++) { ; }
    tempch = *endf;
    *endf = _T('\0');
    if ((nfun = fp_finditem(begin)) == -1) {
        *endf = tempch;
        return NULL;
    }
    *endf = tempch;
    if ((*endf != _T('(')) || (*(end-1) != _T(')'))) {
        return NULL;
    }
    if (fp_table[nfun].npar == 0) {
        space = 1;
        for (scan = endf + 1; scan < (end - 1); scan++) {
            if (!_istspace(*scan)) { space = 0; }
        }
        if (space) {
            *fun++ = _T('F');
            *fun++ = nfun;
            return fun;
        }
        else {
            return NULL;
        }
    }
    else {
        tempch = *(end - 1);
        *(end - 1) = _T('\0');
        parbuf = (char_t *) malloc((_tcslen(endf + 1) + 1) * sizeof(char_t));
        if (!parbuf) { return NULL; }
        _tcscpy(parbuf, endf + 1);
        *(end - 1) = tempch;
        for (i = 0; i < fp_table[nfun].npar; i++) {
            if ((temps = fp_strtok((i == 0) ? parbuf : NULL)) == NULL) {
                break;
            }
            paramstr[i] = temps;
        }
        if (temps == NULL) {
            free(parbuf);
            return NULL;
        }
        if ((temps = fp_strtok(NULL)) != NULL) {
            free(parbuf);
            return NULL;
        }

        tempu = fun;
        for (i = 0; i < fp_table[nfun].npar; i++) {
            if (!(tempu = fp_decode(tempu, paramstr[i], paramstr[i] + _tcslen(paramstr[i])))) {
                free(parbuf);
                return NULL;
            }
        }
        free(parbuf);
        *(tempu++) = _T('F');
        *(tempu++) = nfun;
        return tempu;
    }
}

static real_t fp_pi(void)
{
    return ML_PI;
}

static real_t fp_sin(real_t x)
{
    if (ml_angleunit == 0x01) {
        int_t nx = (int_t)floor(x);
        real_t fx = (real_t)nx;
        if (x == fx) {
            if ((nx % 180) == 0) {
                return 0.0;
            }
        }
        x *= (ML_PI / 180.0);
    }
    else if (ml_angleunit == 0x02) {
        int_t nx = (int_t)floor(x);
        real_t fx = (real_t)nx;
        if (x == fx) {
            if ((nx % 200) == 0) {
                return 0.0;
            }
        }
        x *= (ML_PI / 200.0);
    }
    else {
        real_t ft = x / ML_PI;
        int_t nx = (int_t)floor(ft);
        real_t fx = (real_t)nx;
        if (ft == fx) {
            return 0.0;
        }
    }

    return sin(x);
}

static real_t fp_cos(real_t x)
{
    if (ml_angleunit == 0x01) {
        int_t nx = (int_t)floor(x);
        real_t fx = (real_t)nx;
        if (x == fx) {
            if (((nx % 180) != 0) && ((nx % 90) == 0)) {
                return 0.0;
            }
        }
        x *= (ML_PI / 180.0);
    }
    else if (ml_angleunit == 0x02) {
        int_t nx = (int_t)floor(x);
        real_t fx = (real_t)nx;
        if (x == fx) {
            if (((nx % 200) != 0) && ((nx % 100) == 0)) {
                return 0.0;
            }
        }
        x *= (ML_PI / 200.0);
    }
    else {
        real_t fx = x / ML_PI, fxx = x / ML_PI2;
        int_t nx = (int_t)floor(fx), nxx = (int_t)floor(fxx);
        real_t ft = (real_t)nx, ftt = (real_t)nxx;
        if ((ftt == fxx) && (ft != fx)) {
            return 0.0;
        }
    }

    return cos(x);
}

static real_t fp_tan(real_t x)
{
    if (ml_angleunit == 0x01) {
        int_t nx = (int_t)floor(x);
        real_t fx = (real_t)nx;
        if (x == fx) {
            if ((nx % 180) == 0) {
                return 0.0;
            }
        }
        x *= (ML_PI / 180.0);
    }
    else if (ml_angleunit == 0x02) {
        int_t nx = (int_t)floor(x);
        real_t fx = (real_t)nx;
        if (x == fx) {
            if ((nx % 200) == 0) {
                return 0.0;
            }
        }
        x *= (ML_PI / 200.0);
    }
    else {
        real_t ft = x / ML_PI;
        int_t nx = (int_t)floor(ft);
        real_t fx = (real_t)nx;
        if (ft == fx) {
            return 0.0;
        }
    }

    return tan(x);
}

static real_t fp_asin(real_t x)
{
    real_t y = asin(x);

    if (ml_angleunit == 0x01) {
        y *= (180.0 / ML_PI);
    }
    else if (ml_angleunit == 0x02) {
        y *= (200.0 / ML_PI);
    }

    return y;
}

static real_t fp_acos(real_t x)
{
    real_t y = acos(x);

    if (ml_angleunit == 0x01) {
        y *= (180.0 / ML_PI);
    }
    else if (ml_angleunit == 0x02) {
        y *= (200.0 / ML_PI);
    }

    return y;
}

static real_t fp_atan(real_t x)
{
    real_t y = atan(x);

    if (ml_angleunit == 0x01) {
        y *= (180.0 / ML_PI);
    }
    else if (ml_angleunit == 0x02) {
        y *= (200.0 / ML_PI);
    }

    return y;
}

static real_t const_k(void)
{
    return ML_KB;
}

static real_t const_q(void)
{
    return ML_QE;
}

static real_t const_h(void)
{
    return ML_HH;
}

static real_t const_c(void)
{
    return ML_CC;
}

static real_t const_e(void)
{
    return ML_EPS0;
}

static real_t const_n(void)
{
    return ML_NA;
}

static real_t const_m(void)
{
    return ML_M0;
}

static real_t fp_min(real_t a, real_t b)
{
    return ((a > b) ? b : a);
}

static real_t fp_max(real_t a, real_t b)
{
    return ((a < b) ? b : a);
}

static real_t fp_rand(real_t fsrand)
{
    if (fsrand != 0.0) {
        srand((uint_t)time(NULL));
    }

    return ((real_t)(rand())) / ((real_t)RAND_MAX);
}

static real_t fp_time(void)
{
    return (real_t)time(NULL);
}

static real_t fp_sign(real_t x)
{
    if (x == 0.0) {
        return 0.0;
    }
    if (x < 0.0) {
        return -1.0;
    }

    return 1.0;
}

static real_t fp_log2(real_t x)
{
    return log(x) / log(2.0);
}

static real_t fp_jn(real_t n, real_t x)
{
    return _jn((int_t)n, x);
}

static real_t fp_yn(real_t n, real_t x)
{
    return _yn((int_t)n, x);
}
