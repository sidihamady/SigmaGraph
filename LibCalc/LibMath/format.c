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

    ml_remspace         :    enlève tous les espaces dans une chaîne
    
        szBuffer        :    chaîne à traiter
        
        Retourne le nombre d'espaces enlevés
    -------------------------------------------------------------------- */
int_t ml_remspace(char_t *szBuffer)
{
    int_t nlen, ii, nspace;
    char_t *psz = NULL;
    char_t szTmp[ML_STRSIZE];

    nlen = (int_t)_tcslen((const char_t *)szBuffer);
    if ((nlen < 1) || (nlen > 4096)) {
        return 0;
    }

    if (nlen == 1) {
        if (_istspace(szBuffer[0])) {
            szBuffer[0] = _T('\0');
            return 1;
        }
        return 0;
    }

    memset(szTmp, 0, sizeof(char_t) * ML_STRSIZE);

    // ------------------------------------------------------------------------------------
    ii = 0;    nspace = 0;
    while(1) {
        if ((ii >= nlen) || (nspace >= nlen)) {
            return nspace;
        }
        if (szBuffer[ii] == _T('\0')) {
            return nspace;
        }
        if (_istspace(szBuffer[ii])) {
            nspace += 1;
            psz = &szBuffer[ii+1];
            _tcscpy(szTmp, psz);
            szBuffer[ii] = _T('\0');
            _tcscat(szBuffer, szTmp);
        }
        else {
            ii += 1;
        }

    }
    // ------------------------------------------------------------------------------------

    return nspace;
}

/*  --------------------------------------------------------------------
    :TODO: translate to english

    ml_getformat    :    obtenir le préfixe et suffixe d'un format numérique
    
        szFmt       :    chaîne du format (exemple: %.3f, 2.3%e, ...)
        prefix      :    contiendra en sortie le préfixe
        suffix      :    contiendra en sortie le suffixe

        Retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_getformat(const char_t* szFmt, int_t *prefix, int_t *suffix)
{
    int_t ii, jj, ilen = 0;
    char_t szT[16], *pz = NULL;
    byte_t bErr = 0x00;

    *prefix = 0;    *suffix = 0;
    if (szFmt == NULL) {
        return -1;
    }

    ilen = _tcslen(szFmt);

    if ((ilen < 3) || (ilen > 16)) {
        return -1;
    }

    if (szFmt[0] != _T('%')) {
        return -1;
    }

    jj = 0;
    _tcscpy(szT, szFmt);
    for (ii = 0; ii < ilen; ii++) {
        if (szT[ii] == _T('.')) {
            jj = ii;
            break;
        }
    }

    if (jj != 0) {
        if (jj == 1) {
            szT[ilen-1]  = _T('\0');
            pz = &szT[jj+1];
            *suffix = (int_t)ml_toreal(pz, &bErr, NULL);
        }
        else {
            szT[jj] = _T('\0');
            pz = &szT[1];
            *prefix = (int_t)ml_toreal(pz, &bErr, NULL);
            szT[jj] = szFmt[jj];
            szT[ilen-1]  = _T('\0');
            pz = &szT[jj+1];
            *suffix = (int_t)ml_toreal(pz, &bErr, NULL);
        }
    }
    else{
        pz = &szT[1];
        szT[ilen-1]  = _T('\0');
        *prefix = (int_t)ml_toreal(pz, &bErr, NULL);
    }

    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: translate to english

    ml_reformat         :    post-formatage des nombres réels (%f , %e)
    
        szBuffer        :    chaîne obtenue avec _tcsprintf avec %f ou %e
        
        Retourne la longueur de la chaine reformatée
    -------------------------------------------------------------------- */
int_t ml_reformat(char_t *szBuffer)
{
    int_t ii, nlen, itmp, istart, ifmt;
    real_t fval;
    char_t szTmp[ML_STRSIZE], *psz = NULL;

    nlen = (int_t)_tcslen((const char_t *)szBuffer);
    if ((nlen < 2) || (nlen > ML_STRSIZEN)) {
        return 0;
    }

    psz = (char_t*)_tcsstr((const char_t*)szBuffer, _T("#INF"));
    if (psz) {
        //_tcscpy(szBuffer, _T("Infinite"));
        szBuffer[0] = 0x221E;
        szBuffer[1] = _T('\0');
        return 0;
    }
    psz = (char_t*)_tcsstr((const char_t*)szBuffer, _T("#IND"));
    if (psz) {
        //_tcscpy(szBuffer, _T("Infinite"));
        szBuffer[0] = _T('?');
        szBuffer[1] = _T('\0');
        return 0;
    }
    psz = (char_t*)_tcsstr((const char_t*)szBuffer, _T("#NAN"));
    if (psz) {
        //_tcscpy(szBuffer, _T("Infinite"));
        szBuffer[0] = _T('?');
        szBuffer[1] = _T('\0');
        return 0;
    }

    memset((char_t*)szTmp, 0, sizeof(char_t) * ML_STRSIZE);

    errno = 0;
    fval = ml_toreal(szBuffer, NULL, NULL);
    if ((fval == -HUGE_VAL) || (fval == HUGE_VAL) || (errno == ERANGE)) {
        return 0;
    }
    if (fval == 0.0) {
        _tcscpy(szBuffer, _T("0"));
        return 0;
    }

    ifmt = -1;
    istart = nlen - 1;
    for (ii = istart; ii >= 0; ii--) {
        if ((szBuffer[ii] == _T('e')) || (szBuffer[ii] == _T('E'))) {    // Format %e
            if (ii > 1) {
                ifmt = 1;
            }
            break;
        }
        else if (szBuffer[ii] == _T('.')) {    // Format %f
            ifmt = 0;
            break;
        }
    }

    if (ifmt == -1) {
        return 0;
    }

    if (ifmt == 1) {    // Format %e
        psz = &szBuffer[ii];
        itmp = (int_t)_tcslen((const char_t *)psz);
        if (itmp == 3) {
            if (*(psz+1) == _T('0')) {    //e0x
                *(psz+1) = *(psz+2);
                *(psz+2) = _T('\0');
            }
            else if (((*(psz+1) == _T('+')) || (*(psz+1) == _T('-'))) && (*(psz+2) == _T('0'))) {    //e+0
                *psz = _T('\0');
            }
            _tcscpy(szTmp, (const char_t*)psz);
        }
        else if ((itmp >= 4) && (itmp <= 5)) {
            if (((*(psz+1) == _T('+')) || (*(psz+1) == _T('-'))) && (*(psz+2) == _T('0'))) {
                if (itmp == 4) {    //e+0x
                    if (*(psz+3) == _T('0')) {    //e+00
                        *psz = _T('\0');
                    }
                    else {
                        *(psz+2) = *(psz+3);
                        *(psz+3) = _T('\0');
                    }
                }
                else if (itmp == 5) {
                    if (*(psz+3) == _T('0')) {    //e+00x
                        if (*(psz+4) == _T('0')) {    //e+000
                            *psz = _T('\0');
                        }
                        else {
                            *(psz+2) = *(psz+4);
                            *(psz+3) = _T('\0');
                        }
                    }
                    else {    //e+0xy
                        *(psz+2) = *(psz+3);
                        *(psz+3) = *(psz+4);
                        *(psz+4) = _T('\0');
                    }
                }
            }
            else if ((*(psz+1) == _T('0')) && (*(psz+2) == _T('0'))) {
                if (itmp == 4) {    //e00x
                    if (*(psz+3) == _T('0')) {    //e+000
                        *psz = _T('\0');
                    }
                    else {
                        *(psz+1) = *(psz+3);
                        *(psz+2) = _T('\0');
                    }
                }
                else if (itmp == 5) {    //e00xy
                    *(psz+1) = *(psz+3);
                    *(psz+2) = *(psz+4);
                    *(psz+3) = _T('\0');
                }
            }
            _tcscpy(szTmp, (const char_t*)psz);
        }
        istart = ii - 1;
    }

    for (ii = istart; ii >= 0; ii--) {
        if (szBuffer[ii] != _T('0')) {
            break;
        }
    }
    if (szBuffer[ii] == _T('.')) {
        szBuffer[ii] = _T('\0');
    }
    else {
        szBuffer[ii+1] = _T('\0');
    }

    if ((ifmt == 1) && (szTmp[0] != _T('\0'))) {
        _tcscat(szBuffer, (const char_t*)szTmp);
    }

    return (int_t)_tcslen(szBuffer);
}

/*  --------------------------------------------------------------------
    :TODO: translate to english

    ml_reformat_auto        :    formatage automatique des nombres réels (%f , %e)

        fVal                :    nombre réel à formater
        szBuffer            :    chaîne obtenue avec _tcsprintf avec %f ou %e
        nCount              :    taille de la chaîne formatée
        pszFormat           :    contiendra en sortie le format
        pFmt                :    contiendra en sortie le type de format (0 ou 1)

        Retourne la longueur de la chaine reformatée
    -------------------------------------------------------------------- */
int_t ml_reformat_auto(real_t fVal, char_t* szBuffer, int_t nCount, char_t* pszFormat, int_t *pFmt, libvar_t *pLib)
{
    real_t ftmp = fabs(fVal);
    int_t iFmt = 0;

    if ((nCount < 1) || (nCount > ML_STRSIZE)) {
        return 0;
    }

    if ((ftmp > 1e-4) && (ftmp < 1e4)) {
        _tcsprintf(szBuffer, nCount, (const char_t*)ml_format_f, fVal);
        if (pszFormat != NULL) {
            _tcscpy(pszFormat, (const char_t*)ml_format_f);
            if (pFmt != NULL) {
                *pFmt = 0;
            }
        }
    }
    else if (ftmp == 0.0) {
        _tcsprintf(szBuffer, nCount, (const char_t*)ml_format_f, fVal);
        if (pszFormat != NULL) {
            _tcscpy(pszFormat, (const char_t*)ml_format_f);
            if (pFmt != NULL) {
                *pFmt = 0;
            }
        }
    }
    else {
        _tcsprintf(szBuffer, nCount, (const char_t*)ml_format_e, fVal);
        if (pszFormat != NULL) {
            _tcscpy(pszFormat, (const char_t*)ml_format_e);
            if (pFmt != NULL) {
                *pFmt = 1;
            }
        }
    }

    pLib->errcode = 0;
    ml_reformat(szBuffer);

    if ((pszFormat != NULL) && (pFmt != NULL) && (fVal != 0.0)) {
        if (*pFmt == 0) {
            ftmp = ml_toreal(szBuffer, NULL, NULL);
            if (ftmp == 0.0) {
                _tcscpy(pszFormat, (const char_t*)ml_format_e);
                *pFmt = 1;
            }
        }
    }

    return (int_t)_tcslen(szBuffer);
}
