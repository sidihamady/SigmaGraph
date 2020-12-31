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

#include "libfile.h"

// Fonction interne: lire une ligne d'un fichier. Attention: données à vérifier avant appel à cette fonction.
__inline static int_t fl_getline(FILE* pf, char_t* pszBuffer, int_t nCount);


void fl_gettime(date_t* tm)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    tm->year = (int_t)(st.wYear);
    tm->month = (int_t)(st.wMonth);
    tm->day = (int_t)(st.wDay);
    tm->hour = (int_t)(st.wHour);
    tm->minute = (int_t)(st.wMinute);
    tm->second = (int_t)(st.wSecond);
    tm->millisecond = (int_t)(st.wMilliseconds);

    return;
}

/*  --------------------------------------------------------------------
    fl_getshort            :    Accéder au nom de fichier court (sans le chemin)

        filenameShort      :    Chaîne où sera sauvegardé le nom court
        filename           :    nom du fichier long

        Retourne 0 si OK, 1 sinon.
    -------------------------------------------------------------------- */
int_t fl_getshort(char_t* filenameShort, char_t* filename)
{
    int_t ii, nlen;
    char_t* psz = NULL;
    byte_t bret = 0;

    nlen = (int)_tcslen(filename);

    if ((nlen < 6) || (nlen > ML_STRSIZE)) {
        return 1;
    }

    for (ii = nlen - 1; ii > 0; ii--) {
        if ((filename[ii] == _T('\\')) || (filename[ii] == _T('/'))) {
            psz = &filename[ii+1];
            bret = 1;
            break;
        }
    }

    if (bret == 1) {
        _tcscpy(filenameShort, (const char_t*)psz);
        return 0;
    }

    return 1;
}

/*  --------------------------------------------------------------------
    fl_filesize            :    Accéder à la taille (en octets) d'un fichier

        filename           :    nom du fichier

        Retourne la taille en octets.
    -------------------------------------------------------------------- */
long_t fl_filesize(const char_t* filename)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwSizeHigh = 0L, dwSizeLow = 0L;

    hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return 0L;
    }
    dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    return (long_t)((dwSizeHigh * (2^32)) + dwSizeLow);
}

__inline static int_t fl_getline(FILE* pf, char_t* pszBuffer, int_t nCount)
{
    int_t ii;
    char_t cr;

    for (ii = 0; ii < nCount; ii++) {
        cr = _fgettc(pf);

        if (feof(pf) != 0) {
            if (ii > 0) {
                if (pszBuffer[ii - 1] != _T('\n')) {
                    pszBuffer[ii] = _T('\n');
                    pszBuffer[ii + 1] = _T('\0');
                    return (ii + 1);
                }
            }
            pszBuffer[ii] = _T('\0');
            return ii;
        }
        pszBuffer[ii] = (char_t)cr;
        if (pszBuffer[ii] == _T('\n')) {
            if (ii > 0) {
                if (pszBuffer[ii - 1] == _T('\r')) {
                    pszBuffer[ii - 1] = _T('\n');
                    pszBuffer[ii] = _T('\0');
                    return ii;
                }
            }
            pszBuffer[ii + 1] = _T('\0');
            return (ii + 1);
        }
    }

    return 0;
}

long_t fl_import_asciiv(const char_t* filename, vector_t *pColumn, int_t *piColumn, int_t *pColumnCount, int_t *pRowCount,
                        long_t *pvecCurID, char_t *pszNote, char_t cSep, int_t iSkip, int_t iTitle,
                        int_t iCreateColumn, libvar_t *pLib)
{
    FILE *pf = NULL;
    int_t iOffset, iCol, nRowInit, iTrim;
    int_t ii, jj, ll, mm, nLength, nLineLen, nLineSizeMax, nLineLenMax,
        iMaxColCount, nTT, iFmt[ML_MAXCOLUMNS];
    long_t iLength;
    char_t *pszBuffer = NULL, szBuffer[ML_STRSIZES], szBufferT[ML_STRSIZES];
    real_t fval;
    char_t *pszT = NULL;

    if (pLib->errcode) {
        return 0;
    }

    pLib->errcode = 1;

    int_t iColumnT[ML_MAXCOLUMNS];
    for (ii = 0; ii < ML_MAXCOLUMNS; ii++) {
        iColumnT[ii] = 1;
    }
    if (piColumn == NULL) {
        piColumn = iColumnT;
    }

    // Verifications
    if ((pColumn == NULL) || (piColumn == NULL) || (pvecCurID == NULL) || (pColumnCount == NULL) || (pRowCount == NULL)) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
        return 0;
    }

    long_t uiCurID = *pvecCurID;

    if (iCreateColumn == 0) {
        if ((*pColumnCount < 1) || (*pColumnCount > ML_MAXCOLUMNS)) {
            _tcscpy(pLib->message, _T("Invalid data"));
            if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
            return 0;
        }
        iMaxColCount = (*pColumnCount);
    }
    else {
        iMaxColCount = ML_MAXCOLUMNS;
    }

    _tcscpy(pLib->message, _T("Data reading..."));

    iLength = fl_filesize(filename);
    if ((iLength < 5) || (iLength >= (sizeof(char_t) * (ML_STRSIZES - 1) * ML_MAXCOLUMNS * ML_MAXPOINTS))) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
        return 0;
    }

    pf = _wfopen(filename, _T("r"));
    if (pf == NULL) {
        _tcscpy(pLib->message, _T("Cannot open file"));
        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
        return 0;
    }

    pLib->progress = 0;

    nLineLenMax = (ML_STRSIZES * ML_MAXCOLUMNS) - 1;
    nLineSizeMax = (nLineLenMax + 1) * sizeof(char_t);
    pszBuffer = (char_t*)malloc(nLineSizeMax);
    if (pszBuffer == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        fclose(pf); pf = NULL;
        return 0;
    }

    memset(pszBuffer, 0, nLineSizeMax);
    memset(iFmt, 0, ML_MAXCOLUMNS * sizeof(int_t));

    // Skip some lines...
    iOffset = 0;
    nLength = 0;
    if (pszNote != NULL) {
        date_t tm;
        fl_gettime(&tm);
        _tcsprintf(pszNote, ML_STRSIZEW - 1, _T("[%02d/%02d/%04d %02d:%02d:%02d]\r\n%s\r\n"), tm.day, tm.month, tm.year,
            tm.hour, tm.minute, tm.second, filename);
        nLength = (int_t)_tcslen((const char_t*)pszNote);
    }

    if (iSkip > 0) {
        for (ii = 0; ii < iSkip; ii++) {
            fl_getline(pf, pszBuffer, nLineLenMax);
            nTT = (int_t)_tcslen((const char_t*)pszBuffer);
            if ((pszNote != NULL) && (nLength < (ML_STRSIZEW - nTT))) {
                _tcscat(pszNote, (const char_t*)pszBuffer);
                nLength = (int_t)_tcslen((const char_t*)pszNote);
                if (nLength > 1) {
                    if ((pszNote[nLength - 1] == _T('\r')) || (pszNote[nLength - 1] == _T('\n'))) {
                        pszNote[nLength - 1] = _T('\0');
                    }
                }
                _tcscat(pszNote, _T("\r\n"));
            }
            if (ii == iSkip) {
                iOffset = ii + 1; // First data line
                break;
            }
            if (pLib->interact != 0) {
                if ((ii % ML_POINTS) == 0) {
                    ml_event();
                }
            }
        }
    }

    // Read Title
    if (iTitle > 0) {
        while (true) {
            nLineLen = fl_getline(pf, pszBuffer, nLineLenMax);
            if ((_T('#') == *pszBuffer) || (_T(';') == *pszBuffer)) {
                // skip comment
                continue;
            }

            // remove trailing comment
            pszT = _tcsrchr(pszBuffer, _T('#'));
            if (pszT) {
                *pszT = _T('\0');
            }
            if (cSep != _T(';')) {
                pszT = _tcsrchr(pszBuffer, _T(';'));
                if (pszT) {
                    *pszT = _T('\0');
                }
            }

            if ((nLineLen >= 4) && (nLineLen < nLineLenMax)) {
                ll = 0;    iCol = 0;
                iTrim = 0;
                jj = 0;
                while (jj < nLineLen) {
                    if (iTrim == 0) {
                        if ((pszBuffer[jj] == _T(' ')) || (pszBuffer[jj] == _T('\t'))) {
                            jj += 1;
                            continue;
                        }
                    }
                    iTrim = 1;

                    if (pszBuffer[jj] == cSep) {
                        if (ll < 1) {
                            break;
                        }
                        szBuffer[ll] = _T('\0');
                        ll = 0;
                        _tcscpy(pColumn[iCol].label, (const char_t*) szBuffer);
                        iCol += 1;

                        for (mm = jj; mm < nLineLen; mm++) {
                            if (pszBuffer[mm] != cSep) {
                                break;
                            }
                        }
                        jj = mm;
                    }
                    else if (pszBuffer[jj] == _T('\n')) {
                        if (ll < 1) {
                            break;
                        }
                        szBuffer[ll] = _T('\0');
                        ll = 0;
                        _tcscpy(pColumn[iCol].label, (const char_t*) szBuffer);
                        iCol += 1;
                        break;
                    }
                    else {
                        szBuffer[ll] = pszBuffer[jj];
                        jj += 1;
                        ll += 1;
                        if (ll >= ML_STRSIZES) {
                            break;
                        }
                    }
                }
            }

            break;
        }
    }

    while (true) {

        // First data line
        nLineLen = fl_getline(pf, pszBuffer, nLineLenMax);
        if ((_T('#') == *pszBuffer) || (_T(';') == *pszBuffer)) {
            // skip comment
            continue;
        }

        // remove trailing comment
        pszT = _tcsrchr(pszBuffer, _T('#'));
        if (pszT) {
            *pszT = _T('\0');
        }
        if (cSep != _T(';')) {
            pszT = _tcsrchr(pszBuffer, _T(';'));
            if (pszT) {
                *pszT = _T('\0');
            }
        }

        if ((nLineLen < 4) || (nLineLen > nLineLenMax)) {
            _tcscpy(pLib->message, _T("Invalid data"));
            if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
            fclose(pf); pf = NULL;
            free(pszBuffer); pszBuffer = NULL;
            return 0;
        }
        iCol = 0;
        iTrim = 0;
        // >> Trim
        for (mm = nLineLen - 1; mm > 0; mm--) {
            if ((pszBuffer[mm] == _T(' ')) || (pszBuffer[mm] == _T('\t'))
                || (pszBuffer[mm] == _T('\r')) || (pszBuffer[mm] == _T('\n'))) {
                continue;
            }
            else {
                if ((mm > 0) && (mm < (nLineLen - 1))) {
                    pszBuffer[mm + 1] = _T('\n');
                    nLineLen = mm + 2;
                }
                break;
            }
        }
        // <<
        jj = 0;
        while (jj < nLineLen) {
            if (iTrim == 0) {
                if ((pszBuffer[jj] == _T(' ')) || (pszBuffer[jj] == _T('\t'))) {
                    jj += 1;
                    continue;
                }
            }
            iTrim = 1;
            if (pszBuffer[jj] == cSep) {
                iCol += 1;
                for (mm = jj; mm < nLineLen; mm++) {
                    if (pszBuffer[mm] != cSep) {
                        break;
                    }
                }
                jj = mm;
            }
            else if (pszBuffer[jj] == _T('\n')) {
                iCol += 1;
                break;
            }
            else {
                jj += 1;
            }
        }

        break;
    }

    if ((iCol < 1) || (iCol > iMaxColCount)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
        free(pszBuffer); pszBuffer = NULL;
        fclose(pf); pf = NULL;
        return 0;
    }

    // Create Vectors if needed
    *pRowCount = 1;

    memset(szBufferT, 0, ML_STRSIZES * sizeof(char_t));

    if (iCreateColumn) {

        *pColumnCount = iCol;

        for (jj = 0; jj < *pColumnCount; jj++) {
            if (piColumn[jj] == 0) {
                continue;
            }
            if ((pColumn[jj].dim < 1) || (pColumn[jj].dat == NULL)) {
                _tcscpy(szBuffer, pColumn[jj].name);
                if ((iTitle > 0) && (pColumn[jj].label[0] != _T('\0'))) {
                    _tcscpy(szBufferT, pColumn[jj].label);
                }
                pLib->errcode = 0;
                ml_vector_create(&(pColumn[jj]), *pRowCount, *pvecCurID, (const char_t*)szBuffer, NULL, pLib);
                if (pLib->errcode != 0) {
                    *pvecCurID = uiCurID;
                    *pColumnCount = 0;    *pRowCount = 0;
                    _tcscpy(pszBuffer, _T("Error: "));
                    _tcscat(pszBuffer, (const char_t*)(pLib->message));
                    _tcscpy(pLib->message, (const char_t*)pszBuffer);
                    if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                    free(pszBuffer); pszBuffer = NULL;
                    fclose(pf); pf = NULL;
                    return 0;
                }
                if ((iTitle > 0) && (szBufferT[0] != _T('\0'))) {
                    _tcscpy(pColumn[jj].label, (const char_t*)szBufferT);
                }
                else {
                    _tcscpy(pColumn[jj].label, (const char_t*)(pColumn[jj].name));
                    //_tcscat(pColumn[jj].label, _T("."));
                }
                if (jj == 0) {
                    pColumn[jj].type = 0x10;    // X-Column
                }
                else {
                    pColumn[jj].type = 0x20;    // Y-Column
                    pColumn[jj].idx = 0;
                }
                *pvecCurID += 1;
            }
        }
    }

    for (jj = 0; jj < iCol; jj++) {
        if (piColumn[jj] == 0) {
            continue;
        }
        if ((pColumn[jj].dim < 1) || (pColumn[jj].dat == NULL)) {
            _tcscpy(pLib->message, _T("Invalid column"));
            if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
            free(pszBuffer); pszBuffer = NULL;
            fclose(pf); pf = NULL;
            return 0;
        }
    }

    nRowInit = pColumn[0].dim;

    // First line
    ll = 0;    iCol = 0;
    errno = 0;
    iTrim = 0;
    // >> Trim
    for (mm = nLineLen - 1; mm > 0; mm--) {
        if ((pszBuffer[mm] == _T(' ')) || (pszBuffer[mm] == _T('\t'))
            || (pszBuffer[mm] == _T('\r')) || (pszBuffer[mm] == _T('\n'))) {
                continue;
        }
        else {
            if ((mm > 0) && (mm < (nLineLen - 1))) {
                pszBuffer[mm + 1] = _T('\n');
                nLineLen = mm + 2;
            }
            break;
        }
    }
    // <<
    jj = 0;
    while (jj < nLineLen) {
        if (iTrim == 0) {
            if ((pszBuffer[jj] == _T(' ')) || (pszBuffer[jj] == _T('\t'))) {
                jj += 1;
                continue;
            }
        }
        iTrim = 1;

        if (pszBuffer[jj] == cSep) {
            if (ll < 1) {
                _tcscpy(pLib->message, _T("Data format not valid"));
                if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                free(pszBuffer); pszBuffer = NULL;
                fclose(pf); pf = NULL;
                return 0;
            }
            szBuffer[ll] = _T('\0');
            ll = 0;
            if (piColumn[iCol] != 0) {
                fval = ml_toreal(szBuffer, NULL, NULL);
                if ((fval == -HUGE_VAL) || (fval == HUGE_VAL) || (errno == ERANGE)) {
                    _tcscpy(pLib->message, _T("Data format not valid"));
                    if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                    free(pszBuffer); pszBuffer = NULL;
                    fclose(pf); pf = NULL;
                    return 0;
                }
                pColumn[iCol].dat[0] = fval;
                pColumn[iCol].mask[0] = 0x00;
                // Adapter le format num�rique de la colonne
                pLib->errcode = 0;
                ml_reformat_auto(fval, szBuffer, ML_STRSIZES - 1, pColumn[iCol].format, &iFmt[iCol], pLib);
                //
            }
            iCol += 1;
            if (iCol >= *pColumnCount) {
                break;
            }

            for (mm = jj; mm < nLineLen; mm++) {
                if (pszBuffer[mm] != cSep) {
                    break;
                }
            }
            jj = mm;
        }
        else if (pszBuffer[jj] == _T('\n')) {
            if (ll < 1) {
                _tcscpy(pLib->message, _T("Data format not valid"));
                if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                free(pszBuffer); pszBuffer = NULL;
                fclose(pf); pf = NULL;
                return 0;
            }
            szBuffer[ll] = _T('\0');
            ll = 0;
            if (piColumn[iCol] != 0) {
                fval = ml_toreal(szBuffer, NULL, NULL);
                if ((fval == -HUGE_VAL) || (fval == HUGE_VAL) || (errno == ERANGE)) {
                    _tcscpy(pLib->message, _T("Data format not valid"));
                    if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                    free(pszBuffer); pszBuffer = NULL;
                    fclose(pf); pf = NULL;
                    return 0;
                }
                pColumn[iCol].dat[0] = fval;
                pColumn[iCol].mask[0] = 0x00;
                // Adapter le format num�rique de la colonne
                pLib->errcode = 0;
                ml_reformat_auto(fval, szBuffer, ML_STRSIZES - 1, pColumn[iCol].format, &iFmt[iCol], pLib);
                //
            }
            iCol += 1;
            break;
        }
        else {
            szBuffer[ll] = pszBuffer[jj];
            jj += 1;
            ll += 1;
            if (ll >= ML_STRSIZES) {
                _tcscpy(pLib->message, _T("Data format not valid"));
                if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                free(pszBuffer); pszBuffer = NULL;
                fclose(pf); pf = NULL;
                return 0;
            }
        }
    }

    // Get lines
    *pRowCount = 1;
    for (ii = 1; ii < ML_MAXPOINTS; ii++) {
        // Data line
        nLineLen = fl_getline(pf, pszBuffer, nLineLenMax);

        if ((_T('#') == *pszBuffer) || (_T(';') == *pszBuffer)) {
            // skip comment
            continue;
        }

        // remove trailing comment
        pszT = _tcsrchr(pszBuffer, _T('#'));
        if (pszT) {
            *pszT = _T('\0');
        }
        if (cSep != _T(';')) {
            pszT = _tcsrchr(pszBuffer, _T(';'));
            if (pszT) {
                *pszT = _T('\0');
            }
        }

        if ((nLineLen < 4) || (nLineLen > nLineLenMax)) {
            _tcsprintf(pLib->message, ML_STRSIZE  - 1, _T("Data read from file: %d lines"), ii);
            if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
            free(pszBuffer); pszBuffer = NULL;
            fclose(pf); pf = NULL;

            pLib->errcode = 0;

            *pRowCount = ii;
            for (jj = 0; jj < *pColumnCount; jj++) {
                pColumn[jj].status = 0x00;
                if ((*pRowCount < nRowInit) && (*pRowCount > 2)) {
                    ml_vector_rem(&(pColumn[jj]), *pRowCount, nRowInit - 1, pLib);
                }
            }
            return (*pRowCount);
        }

        ll = 0;    iCol = 0;
        iTrim = 0;
        // >> Trim
        for (mm = nLineLen - 1; mm > 0; mm--) {
            if ((pszBuffer[mm] == _T(' ')) || (pszBuffer[mm] == _T('\t'))
                || (pszBuffer[mm] == _T('\r')) || (pszBuffer[mm] == _T('\n'))) {
                continue;
            }
            else {
                if ((mm > 0) && (mm < (nLineLen - 1))) {
                    pszBuffer[mm + 1] = _T('\n');
                    nLineLen = mm + 2;
                }
                break;
            }
        }
        // <<
        jj = 0;
        while (jj < nLineLen) {
            if (iTrim == 0) {
                if ((pszBuffer[jj] == _T(' ')) || (pszBuffer[jj] == _T('\t'))) {
                    jj += 1;
                    continue;
                }
            }
            iTrim = 1;
            if (pszBuffer[jj] == cSep) {
                if (ll < 1) {
                    _tcscpy(pLib->message, _T("Data format not valid"));
                    if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                    free(pszBuffer); pszBuffer = NULL;
                    fclose(pf); pf = NULL;
                    return 0;
                }
                szBuffer[ll] = _T('\0');
                ll = 0;
                if (piColumn[iCol] != 0) {
                    fval = ml_toreal(szBuffer, NULL, NULL);
                    if ((fval == -HUGE_VAL) || (fval == HUGE_VAL) || (errno == ERANGE)) {
                        _tcscpy(pLib->message, _T("Data format not valid"));
                        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                        free(pszBuffer); pszBuffer = NULL;
                        fclose(pf); pf = NULL;
                        return 0;
                    }
                    if (ii < pColumn[iCol].dim) {
                        pColumn[iCol].dat[ii] = fval;
                        pColumn[iCol].mask[ii] = 0x00;
                    }
                    else {
                        ml_vector_add(&(pColumn[iCol]), fval, pLib);
                        pColumn[iCol].mask[ii] = 0x00;
                    }
                    if (pLib->errcode != 0) {
                        _tcscpy(pLib->message, _T("Data format not valid"));
                        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                        free(pszBuffer); pszBuffer = NULL;
                        fclose(pf); pf = NULL;
                        return 0;
                    }
                    // Adapter le format num�rique de la colonne
                    if (iFmt[iCol] == 0) {
                        pLib->errcode = 0;
                        ml_reformat_auto(fval, szBuffer, ML_STRSIZES - 1, pColumn[iCol].format, &iFmt[iCol], pLib);
                    }
                    //
                }

                for (mm = jj; mm < nLineLen; mm++) {
                    if (pszBuffer[mm] != cSep) {
                        break;
                    }
                }
                jj = mm;

                iCol += 1;
                if (iCol >= *pColumnCount) {
                    break;
                }
            }
            else if (pszBuffer[jj] == _T('\n')) {
                if (ll < 1) {
                    break;
                }
                szBuffer[ll] = _T('\0');
                ll = 0;
                if (piColumn[iCol] != 0) {
                    fval = ml_toreal(szBuffer, NULL, NULL);
                    if ((fval == -HUGE_VAL) || (fval == HUGE_VAL) || (errno == ERANGE)) {
                        _tcscpy(pLib->message, _T("Data format not valid"));
                        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                        free(pszBuffer); pszBuffer = NULL;
                        fclose(pf); pf = NULL;
                        return 0;
                    }
                    if (ii < pColumn[iCol].dim) {
                        pColumn[iCol].dat[ii] = fval;
                        pColumn[iCol].mask[ii] = 0x00;
                    }
                    else {
                        ml_vector_add(&(pColumn[iCol]), fval, pLib);
                        pColumn[iCol].mask[ii] = 0x00;
                    }
                    if (pLib->errcode != 0) {
                        _tcscpy(pLib->message, _T("Data format not valid"));
                        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                        free(pszBuffer); pszBuffer = NULL;
                        fclose(pf); pf = NULL;
                        return 0;
                    }
                    // Adapter le format num�rique de la colonne
                    if (iFmt[iCol] == 0) {
                        pLib->errcode = 0;
                        ml_reformat_auto(fval, szBuffer, ML_STRSIZES - 1, pColumn[iCol].format, &iFmt[iCol], pLib);
                    }
                    //
                }
                iCol += 1;
                *pRowCount += 1;
                break;
            }
            else {
                szBuffer[ll] = pszBuffer[jj];
                jj += 1;
                ll += 1;
                if (ll >= ML_STRSIZES) {
                    _tcscpy(pLib->message, _T("Data format not valid"));
                    if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_import_asciiv]"), ML_STRSIZE - 1); }
                    free(pszBuffer); pszBuffer = NULL;
                    fclose(pf); pf = NULL;
                    return 0;
                }
            }
        }

        if (pLib->interact != 0) {
            if ((ii % ML_POINTS) == 0) {
                ml_event();
            }
        }
    }

    pLib->errcode = 0;

    for (jj = 0; jj < *pColumnCount; jj++) {
        pColumn[jj].status = 0x00;
        if ((*pRowCount < nRowInit) && (*pRowCount > 2)) {
            ml_vector_rem(&(pColumn[jj]), *pRowCount, nRowInit - 1, pLib);
        }
    }

    free(pszBuffer); pszBuffer = NULL;
    fclose(pf); pf = NULL;

    return (*pRowCount);
}

long_t fl_export_asciiv(const char_t* filename, vector_t *pColumn, int_t nColumnCount, char_t cSep, libvar_t *pLib, const char_t* header)
{
    FILE *pf = NULL;
    int_t ii, jj, nLength, nLineLenMax, idat;
    char_t *pszBuffer = NULL, szSep[2];
    int_t iMaxDim = 0, iCol = 0;

    if (pLib->errcode) {
        return 0;
    }

    pLib->errcode = 1;

    // Verifications
    if (pColumn == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_export_asciiv]"), ML_STRSIZE - 1); }
        return 0;
    }
    if ((nColumnCount < 1) || (nColumnCount > ML_MAXCOLUMNS)) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_export_asciiv]"), ML_STRSIZE - 1); }
        return 0;
    }
    for (jj = 0; jj < nColumnCount; jj++) {
        if (pColumn[jj].dim > iMaxDim) {
            iMaxDim = pColumn[jj].dim;
        }
        if ((pColumn[jj].dim < 1) || (pColumn[jj].dim > ML_MAXPOINTS)) {
            _tcscpy(pLib->message, _T("Invalid column"));
            if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_export_asciiv]"), ML_STRSIZE - 1); }
            return 0;
        }
    }

    _tcscpy(pLib->message, _T("Data writing..."));

    pf = _wfopen(filename, _T("w"));
    if (pf == NULL) {
        _tcscpy(pLib->message, _T("Cannot open file"));
        if (fl_debug == 1) { _tcsncat(pLib->message, _T("   [fl_export_asciiv]"), ML_STRSIZE - 1); }
        return 0;
    }

    pLib->progress = 0;

    if (header) {
        fputws(header, pf);
    }

    nLineLenMax = ML_STRSIZES * ML_MAXCOLUMNS;
    pszBuffer = (char_t*)malloc(nLineLenMax * sizeof(char_t));
    if (pszBuffer == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        fclose(pf); pf = NULL;
        return 0;
    }

    memset(pszBuffer, 0, nLineLenMax * sizeof(char_t));

    nLength = 0;

    szSep[0] = cSep;
    szSep[1] = _T('\0');

    idat = 0;

    if (fputws(_T("# "), pf) == EOF) {
        free(pszBuffer); pszBuffer = NULL;
        fclose(pf); pf = NULL;
        _tcscpy(pLib->message, _T("Cannot save data to file"));
        return 0;
    }

    for (jj = 0; jj < nColumnCount; jj++) {
        if (fputws((const char_t*)(pColumn[jj].label), pf) == EOF) {
            break;
        }
        if (jj < (nColumnCount - 1)) { fputws((const char_t*)szSep, pf); }
        else { fputws(_T("\n"), pf); }
        if (pLib->interact != 0) {
            idat += 1;
            if ((idat % ML_POINTS) == 0) {
                ml_event();
            }
        }
    }

    idat = 0;
    for (ii = 0; ii < iMaxDim; ii++) {

        iCol = 0;
        for (jj = 0; jj < nColumnCount; jj++) {
            // if all colmuns are non edited, skip
            if ((pColumn[jj].mask[ii] & 0x0F) == 0x0F) {
                iCol += 1;
            }
        }
        if (iCol == nColumnCount) {
            continue;
        }

        for (jj = 0; jj < nColumnCount; jj++) {
            if (ii >= pColumn[jj].dim) {
                if (fputws(_T(" "), pf) == EOF) {
                    break;
                }
            }
            else {
                if ((pColumn[jj].mask[ii] & 0x0F) != 0x0F) {
                    _tcsprintf(pszBuffer, nLineLenMax - 1, (const char_t*) (pColumn[jj].format), pColumn[jj].dat[ii]);
                    if (fputws((const char_t*) pszBuffer, pf) == EOF) {
                        break;
                    }
                }
                else {
                    if (fputws(_T(" "), pf) == EOF) {
                        break;
                    }
                }
            }
            if (jj < (nColumnCount - 1)) { fputws((const char_t*)szSep, pf); }
        }
        if (ii < (iMaxDim - 1)) { fputws(_T("\n"), pf); }

        if (pLib->interact != 0) {
            idat += 1;
            if ((idat % ML_POINTS) == 0) {
                ml_event();
            }
        }
    }

    free(pszBuffer); pszBuffer = NULL;
    fclose(pf); pf = NULL;
    pLib->errcode = 0;
    return ii;
}
