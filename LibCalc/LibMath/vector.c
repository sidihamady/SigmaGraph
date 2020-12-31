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

#define SORT_MIN        7
#define SORT_STACK    50

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_create       :    créer un vecteur
    
        pvec               :    pointera sur le vecteur créée
                               (pvec->dat doit être NULL
                                lors de l'appel à ml_vector_create)
        dim                :    dimension voulue
    -------------------------------------------------------------------- */
void ml_vector_create(vector_t *pvec, int_t dim, int_t id, const char_t* name, const vector_t *pvecd, libvar_t *pLib)
{
    if (pLib->errcode) {
        return;
    }

    pLib->errcode = 1;

    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_create]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pvec->dat != NULL) || (dim < 1) || (dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_create]"), ML_STRSIZE - 1); }
        return;
    }

    memset(pvec, 0, sizeof(vector_t));

    pvec->dim = 0;
    if ((pvec->dat = (real_t*)malloc(dim * sizeof(real_t))) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_create]"), ML_STRSIZE - 1); }
        return;
    }
    if ((pvec->mask = (byte_t*)malloc(dim * sizeof(byte_t))) == NULL) {
        free(pvec->dat); pvec->dat = NULL;
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_create]"), ML_STRSIZE - 1); }
        return;
    }
    pvec->dim = dim;
    pvec->istart = 0;
    pvec->istop = pvec->dim - 1;
    pvec->type = 0x20;    // Vecteur Y par défaut

    if (pvecd != NULL) {
        if ((pvecd->dat != NULL) && (pvecd->dim > 0) && (pvecd->dim <= ML_MAXPOINTS)) {
            int_t ii, idim = ML_MIN(pvec->dim, pvecd->dim);
            for (ii = 0; ii < idim; ii++) {
                pvec->dat[ii] = pvecd->dat[ii];
                pvec->mask[ii] = pvecd->mask[ii];
            }
        }
        if (id == 0) {
            pvec->id = pvecd->id;
        }
        else {
            pvec->id = id;
        }
        if (name == NULL) {
            _tcscpy(pvec->name, (const char_t*)(pvecd->name));
        }
        else {
            _tcscpy(pvec->name, (const char_t*)(name));
        }
        _tcscpy(pvec->label, (const char_t*)(pvecd->label));
        _tcscpy(pvec->formula, (const char_t*)(pvecd->formula));
        pvec->status = pvecd->status;
        pvec->type = pvecd->type;    // Vecteur X ou Y
    }
    else {
        pvec->id = id;
        if (name != NULL) {
            _tcscpy(pvec->name, (const char_t*)name);
        }
    }

    pvec->idx = 0;            /* ID d'un vecteur lié, unique - Abscisse */
    pvec->idf = 0;            /* ID d'un vecteur lié, unique - Fit */
    pvec->ide = 0;            /* ID d'un vecteur lié, unique - Barres d'erreur */
    pvec->idr = 0;            /* ID réservé */

    _tcscpy(pvec->format, _T("%.3f"));
    pLib->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_dup        :    dupliquer un vecteur
    -------------------------------------------------------------------- */
void ml_vector_dup(vector_t *pvec, const vector_t *pvecd, libvar_t *pLib)
{
    int_t ii, dim;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;

    if ((pvec == NULL) || (pvecd == NULL)) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_dup]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pvec->dat == NULL) || (pvec->dim < 1) || (pvec->dim > ML_MAXPOINTS)
        || (pvecd->dat == NULL) || (pvecd->dim < 1) || (pvecd->dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_dup]"), ML_STRSIZE - 1); }
        return;
    }

    dim = ML_MIN(pvec->dim, pvecd->dim);

    for (ii = 0; ii < dim; ii++) {
        pvec->dat[ii] = pvecd->dat[ii];
        pvec->mask[ii] = pvecd->mask[ii];
    }
    //    Si la taille du vecteur destination est différente de cette du vecteur source
    //    alors le re-dimensionner le vecteur destination
    if (pvec->dim > dim) {
        pLib->errcode = 0;
        ml_vector_rem(pvec, dim, pvec->dim - 1, pLib);
    }
    else if (pvec->dim < dim) {
        int_t ndim = pvec->dim;
        pLib->errcode = 0;
        for (ii = ndim; ii < dim; ii++) {
            ml_vector_add(pvec, pvecd->dat[ii], pLib);
            if (pLib->errcode != 0) {
                break;
            }
            pvec->mask[ii] = pvecd->mask[ii];
        }
    }

    pLib->errcode = 0;
    ml_vector_dupstatic(pvec, pvecd, pLib);
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_dupstatic        :    dupliquer la partie statique d'un vecteur
    
    -------------------------------------------------------------------- */
void ml_vector_dupstatic(vector_t *pvec, const vector_t *pvecd, libvar_t *pLib)
{
    if (pLib->errcode) { return; }

    pLib->errcode = 1;

    if ((pvec == NULL) || (pvecd == NULL)) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_dup]"), ML_STRSIZE - 1); }
        return;
    }

    pvec->id = pvecd->id;
    pvec->idx = pvecd->idx;
    pvec->idf = pvecd->idf;            /* ID d'un vecteur lié, unique - Fit */
    pvec->ide = pvecd->ide;            /* ID d'un vecteur lié, unique - Barres d'erreur */
    pvec->idr = pvecd->idr;            /* ID réservé */
    _tcscpy(pvec->name, (const char_t*)(pvecd->name));
    _tcscpy(pvec->label, (const char_t*)(pvecd->label));
    _tcscpy(pvec->format, (const char_t*)(pvecd->format));
    pvec->status = pvecd->status;
    pvec->type = pvecd->type;        // Vecteur X ou Y
    pvec->istart = pvecd->istart;
    pvec->istop = pvecd->istop;
    pvec->opt = pvecd->opt;

    pLib->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_validate        :    vérifier la partie statique d'un vecteur
        retourne 0 si OK
    -------------------------------------------------------------------- */
int_t ml_vector_validate(vector_t *pvec)
{
    int_t ipre = 0, isuf = 0;

    if (pvec == NULL) {
        return -1;
    }

    if ((pvec->id < 1) || (pvec->idx < 0) || (pvec->idf < 0) || (pvec->ide < 0) || (pvec->idr < 0)) {
        return -1;
    }

    if (ml_getformat((const char_t*)(pvec->format), &ipre, &isuf) != 0) {
        return -1;
    }

    if ((pvec->dim < 1) || (pvec->dim > ML_MAXPOINTS)) {
        return -1;
    }

    if ((pvec->istart < 0) || (pvec->istart >= pvec->dim)
        || (pvec->istop < 0) || (pvec->istop >= pvec->dim)
        || (pvec->istop <= pvec->istart)) {
        pvec->istart = 0;
        pvec->istop = pvec->dim - 1;
    }

    return 0;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_delete        :    détruire un vecteur
    
        pvec                :    pointeur sur le vecteur à détruire
    -------------------------------------------------------------------- */
void ml_vector_delete(vector_t *pvec, libvar_t *pLib)
{
    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_delete]"), ML_STRSIZE - 1); }
        return;
    }

    if (pvec->dat == NULL) {
        pvec->dim = 0;
        pLib->errcode = 0;
        return;
    }

    free(pvec->dat);
    pvec->dat = NULL;
    free(pvec->mask);
    pvec->mask = NULL;
    pvec->dim = 0;
    pvec->istart = 0;
    pvec->istop = 0;
    pvec->idx = 0;            /* ID d'un vecteur lié, unique - Abscisse */
    pvec->idf = 0;            /* ID d'un vecteur lié, unique - Fit */
    pvec->ide = 0;            /* ID d'un vecteur lié, unique - Barres d'erreur */
    pvec->idr = 0;            /* ID réservé */

    memset(pvec, 0, sizeof(vector_t));
    pLib->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_add        :    ajouter un élément à un vecteur
    
        pvec             :    pointeur sur le vecteur
        ee               :    élément à ajouter
    -------------------------------------------------------------------- */
void ml_vector_add(vector_t *pvec, real_t ee, libvar_t *pLib)
{
    real_t *pdat = NULL;
    byte_t *pmask = NULL;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;

    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_add]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pvec->dat == NULL) || (pvec->mask == NULL) || (pvec->dim < 0) || (pvec->dim >= ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_add]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim + 1) * sizeof(real_t))) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_add]"), ML_STRSIZE - 1); }
        return;
    }
    if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim + 1) * sizeof(byte_t))) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_add]"), ML_STRSIZE - 1); }
        return;
    }
    pvec->dat = pdat;
    pvec->mask = pmask;
    pvec->dat[pvec->dim] = ee;
    pvec->mask[pvec->dim] = 0x00;
    pvec->dim += 1;
    pvec->istart = 0;
    pvec->istop = pvec->dim - 1;

    pLib->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_rem        :    détruire un élément du vecteur
    
        pvec             :    pointeur sur le vecteur
        ii               :    indice de l'élément à détruire
    -------------------------------------------------------------------- */
void ml_vector_rem(vector_t *pvec, int_t istart, int_t istop, libvar_t *pLib)
{
    int_t jj, idat;

    real_t *pdat = NULL;
    byte_t *pmask = NULL;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pvec->dat == NULL) || (pvec->dim < 1) || (pvec->dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
        return;
    }

    if ((istart < 0) || (istart >= pvec->dim) || (istop < 0) || (istop >= pvec->dim)) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
        return;
    }

    if (istart > istop) {
        ML_SWAPUI(istart, istop);
    }

    if (istart == istop) {
        if (istart == pvec->dim - 1) {
            if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim - 1) * sizeof(real_t))) == NULL) {
                _tcscpy(pLib->message, _T("Insufficient memory"));
                if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
                return;
            }
            if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim - 1) * sizeof(byte_t))) == NULL) {
                _tcscpy(pLib->message, _T("Insufficient memory"));
                if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
                return;
            }
            pvec->dat = pdat;
            pvec->mask = pmask;
            pvec->dim -= 1;
            pvec->istart = 0;
            pvec->istop = pvec->dim - 1;

            pLib->errcode = 0;
            _tcscpy(pLib->message, _T("Done."));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
            return;
        }

        idat = 0;
        for (jj = istart; jj < pvec->dim - 1; jj++) {
            pvec->dat[jj] = pvec->dat[jj + 1];
            pvec->mask[jj] = pvec->mask[jj + 1];
        }

        if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim - 1) * sizeof(real_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
            return;
        }
        if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim - 1) * sizeof(byte_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
            return;
        }
        pvec->dat = pdat;
        pvec->mask = pmask;
        pvec->dim -= 1;
        pvec->istart = 0;
        pvec->istop = pvec->dim - 1;
    }
    else if ((istart == 0) && (istop == pvec->dim - 1)) {
        free(pvec->dat);
        free(pvec->mask);
        pvec->dat = NULL;
        pvec->mask = NULL;
        pvec->dim = 0;
        pvec->istart = 0;
        pvec->istop = 0;
        memset(pvec, 0, sizeof(vector_t));
    }
    else {
        idat = 0;
        for (jj = istart; jj < pvec->dim - (istop - istart + 1); jj++) {
            pvec->dat[jj] = pvec->dat[jj + (istop - istart + 1)];
        }

        if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim - (istop - istart + 1)) * sizeof(real_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
            return;
        }
        if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim - (istop - istart + 1)) * sizeof(byte_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rem]"), ML_STRSIZE - 1); }
            return;
        }
        pvec->dat = pdat;
        pvec->mask = pmask;
        pvec->dim -= (istop - istart + 1);
        pvec->istart = 0;
        pvec->istop = pvec->dim - 1;
    }

    pLib->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_insert    :    insérer un élément dans le vecteur
    
        pvec            :    pointeur sur le vecteur
        ii              :    indice de l'élément après lequel insérer
        ee              :    élément à insérer
    -------------------------------------------------------------------- */
void ml_vector_insert(vector_t *pvec, int_t ii, real_t ee, libvar_t *pLib)
{
    int_t jj, idat;

    real_t *pdat = NULL;
    byte_t *pmask = NULL;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pvec->dat == NULL) || (pvec->dim < 1) || (pvec->dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
        return;
    }

    if ((ii < -1) || (ii >= pvec->dim)) {
        _tcscpy(pLib->message, _T("Invalid column index"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
        return;
    }

    if (ii == pvec->dim - 1) {
        if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim + 1) * sizeof(real_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
            return;
        }
        if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim + 1) * sizeof(byte_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
            return;
        }
        pvec->dat = pdat;
        pvec->mask = pmask;
        pvec->dat[pvec->dim] = ee;
        pvec->mask[pvec->dim] = 0x00;
        pvec->dim += 1;
        pvec->istart = 0;
        pvec->istop = pvec->dim - 1;

        pLib->errcode = 0;
        _tcscpy(pLib->message, _T("Done."));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim + 1) * sizeof(real_t))) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
        return;
    }
    if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim + 1) * sizeof(byte_t))) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_insert]"), ML_STRSIZE - 1); }
        return;
    }
    pvec->dat = pdat;
    pvec->mask = pmask;
    pvec->dim += 1;
    pvec->istart = 0;
    pvec->istop = pvec->dim - 1;

    idat = 0;
    for (jj = pvec->dim - 1; jj > ii + 1; jj--) {
        pvec->dat[jj] = pvec->dat[jj - 1];
        pvec->mask[jj] = pvec->mask[jj - 1];
    }
    pvec->dat[ii + 1] = ee;
    pvec->mask[ii + 1] = 0x00;

    pLib->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_copy        :    copier un élément du vecteur

        pvec              :    pointeur sur le vecteur
        ifrom             :    indice de l'élément à copier
        ito               :    indice de l'élément où copier
    -------------------------------------------------------------------- */
void ml_vector_copy(vector_t *pvec, int_t ifrom, int_t ito, libvar_t *pLib)
{
    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_copy]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pvec->dat == NULL) || (pvec->dim < 1) || (pvec->dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_copy]"), ML_STRSIZE - 1); }
        return;
    }

    if ((ifrom < 0) || (ifrom >= pvec->dim) || (ito < 0) || (ito >= pvec->dim)) {
        _tcscpy(pLib->message, _T("Invalid column index"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_copy]"), ML_STRSIZE - 1); }
        return;
    }

    pvec->dat[ito] = pvec->dat[ifrom];

    pLib->errcode = 0;
    return;
}


/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_move        :    déplacer un élément du vecteur
    
        pvec                :    pointeur sur le vecteur
        ifrom                :    indice de l'élément à déplacer
        ito                :    indice de l'élément où déplacer
    -------------------------------------------------------------------- */
void ml_vector_move(vector_t *pvec, int_t ifrom, int_t ito, libvar_t *pLib)
{
    int_t jj, idat;
    real_t *pdat = NULL;
    byte_t *pmask = NULL;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
        return;
    }

    if ((pvec->dat == NULL) || (pvec->dim < 1) || (pvec->dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
        return;
    }

    if ((ifrom < 0) || (ifrom >= pvec->dim) || (ito < 0) || (ito >= pvec->dim)) {
        _tcscpy(pLib->message, _T("Invalid column index"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
        return;
    }

    pvec->dat[ito] = pvec->dat[ifrom];

    if (ifrom == pvec->dim - 1) {
        if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim - 1) * sizeof(real_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
            return;
        }
        if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim - 1) * sizeof(byte_t))) == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
            return;
        }
        pvec->dat = pdat;
        pvec->mask = pmask;
        pvec->dim -= 1;
        pvec->istart = 0;
        pvec->istop = pvec->dim - 1;

        pLib->errcode = 0;
        _tcscpy(pLib->message, _T("Done."));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
        return;
    }

    idat = 0;
    for (jj = ifrom; jj < pvec->dim - 1; jj++) {
        pvec->dat[jj] = pvec->dat[jj + 1];
        pvec->mask[jj] = pvec->mask[jj + 1];
    }

    if ((pdat = (real_t*)realloc(pvec->dat, (pvec->dim - 1) * sizeof(real_t))) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
        return;
    }
    if ((pmask = (byte_t*)realloc(pvec->mask, (pvec->dim - 1) * sizeof(byte_t))) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_move]"), ML_STRSIZE - 1); }
        return;
    }
    pvec->dat = pdat;
    pvec->mask = pmask;
    pvec->dim -= 1;
    pvec->istart = 0;
    pvec->istop = pvec->dim - 1;

    pLib->errcode = 0;
    return;
}


/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_sort        :    ordonner une série de vecteurs
    
        pvec              :    pointeur sur les vecteurs
        icount            :    nombre de vecteurs
        ipivot            :    indice du vecteur pivot
        plink             :    tableau indiquant les colonnes liées
                              (élément à 1 si liée, 0 sinon)
        istop             :    indice du vecteur de fin
        iasc              :    1 si ascendant, 0 si descendant
    -------------------------------------------------------------------- */
void ml_vector_sort(vector_t *pvec, int_t icount, int_t ipivot, byte_t iasc, byte_t *plink,
                          int_t istart, int_t istop, libvar_t *pLib)
{
    int_t ii, ir, dim, jj, nn, ll = 0, *istack, isec, jsec;
    int_t jstack = 0, icol;
    real_t aa, ftmp, ca[ML_MAXCOLUMNS];

    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        return;
    }

    if ((icount < 1) || (icount > ML_MAXCOLUMNS)) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        return;
    }

    if ((ipivot < 0) || (ipivot >= icount)) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        return;
    }

    dim = pvec[ipivot].dim;
    if ((dim < 1) || (dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid column size."));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        return;
    }
    if (plink != NULL) {
        for (icol = 0; icol <= icount; icol++) {
            if ((icol != ipivot) && (plink[icol] == 1)) {
                if ((pvec[icol].dim < 1) || (pvec[icol].dim > ML_MAXPOINTS) || (pvec[icol].dim != dim)) {
                    _tcscpy(pLib->message, _T("Invalid column size."));
                    if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
                    return;
                }
            }
        }
    }
    if ((istart < 0) || (istart >= dim) || (istop < 0) || (istop >= dim) || (istop <= istart)) {
        istart = 0;
        istop = dim - 1;
    }
    ir = istop;

    if (dim == 1) {
        pLib->errcode = 0;
        return;
    }
    else if (dim == 2) {
        if (pvec[ipivot].dat[0] > pvec[ipivot].dat[1]) {
            ftmp = pvec[ipivot].dat[0];
            pvec[ipivot].dat[0] = pvec[ipivot].dat[1];
            pvec[ipivot].dat[1] = ftmp;

            // >>
            if (plink != NULL) {
                for (icol = 0; icol < icount; icol++) {
                    if ((icol != ipivot) && (plink[icol] == 1)) {
                        ftmp = pvec[icol].dat[0];
                        pvec[icol].dat[0] = pvec[icol].dat[1];
                        pvec[icol].dat[1] = ftmp;
                    }
                }
            }
            // <<
        }
        pLib->errcode = 0;
        if (iasc == 0) {
            ml_vector_rev(&(pvec[ipivot]), dim, pLib);
            if (plink != NULL) {
                for (icol = 0; icol < icount; icol++) {
                    if ((icol != ipivot) && (plink[icol] == 1)) {
                        ml_vector_rev(&(pvec[icol]), dim, pLib);
                    }
                }
            }
        }
        if (pLib->errcode == 0) {
            _tcscpy(pLib->message, _T("Done."));
            if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        }
        return;
    }

    istack = (int_t*)malloc(SORT_STACK * sizeof(int_t));
    if (istack == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        return;
    }

    for (isec = istart; isec < ML_MAXPOINTS; isec++) {
        if (ir - ll < SORT_MIN) {
            for (jj = ll + 1; jj <= ir; jj++) {
                aa = pvec[ipivot].dat[jj];
                // >>
                if (plink != NULL) {
                    for (icol = 0; icol < icount; icol++) {
                        if ((icol != ipivot) && (plink[icol] == 1)) {
                            ca[icol] = pvec[icol].dat[jj];
                        }
                    }
                }
                // <<
                for (ii = jj - 1; ii >= ll; ii--) {
                    if (pvec[ipivot].dat[ii] <= aa) { break; }
                    pvec[ipivot].dat[ii + 1] = pvec[ipivot].dat[ii];
                    // >>
                    if (plink != NULL) {
                        for (icol = 0; icol < icount; icol++) {
                            if ((icol != ipivot) && (plink[icol] == 1)) {
                                pvec[icol].dat[ii + 1] = pvec[icol].dat[ii];
                            }
                        }
                    }
                    // <<
                }
                pvec[ipivot].dat[ii + 1] = aa;
                // >>
                if (plink != NULL) {
                    for (icol = 0; icol < icount; icol++) {
                        if ((icol != ipivot) && (plink[icol] == 1)) {
                            pvec[icol].dat[ii + 1] = ca[icol];
                        }
                    }
                }
                // <<
            }
            if (jstack == 0) { break; }
            ir = istack[jstack--];
            ll = istack[jstack--];
        }
        else {
            nn = (ll + ir + 1) >> 1;
            //if (nn > dim - 1) { // Never happen
            if (nn > (istop + 1)) { // Never happen
                _tcscpy(pLib->message, _T("Invalid column index"));
                if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
                free(istack); istack = NULL;
                return;
            }
            ML_SWAP(pvec[ipivot].dat[nn], pvec[ipivot].dat[ll+1]);
            // >>
            if (plink != NULL) {
                for (icol = 0; icol < icount; icol++) {
                    if ((icol != ipivot) && (plink[icol] == 1)) {
                        ML_SWAP(pvec[icol].dat[nn], pvec[icol].dat[ll+1]);
                    }
                }
            }
            // <<
            if (pvec[ipivot].dat[ll] > pvec[ipivot].dat[ir]) {
                ML_SWAP(pvec[ipivot].dat[ll], pvec[ipivot].dat[ir]);
                // >>
                if (plink != NULL) {
                    for (icol = 0; icol < icount; icol++) {
                        if ((icol != ipivot) && (plink[icol] == 1)) {
                            ML_SWAP(pvec[icol].dat[ll], pvec[icol].dat[ir]);
                        }
                    }
                }
                // <<
            }
            if (pvec[ipivot].dat[ll + 1] > pvec[ipivot].dat[ir]) {
                ML_SWAP(pvec[ipivot].dat[ll + 1], pvec[ipivot].dat[ir]);
                // >>
                if (plink != NULL) {
                    for (icol = 0; icol < icount; icol++) {
                        if ((icol != ipivot) && (plink[icol] == 1)) {
                            ML_SWAP(pvec[icol].dat[ll + 1], pvec[icol].dat[ir]);
                        }
                    }
                }
                // <<
            }
            if (pvec[ipivot].dat[ll] > pvec[ipivot].dat[ll + 1]) {
                ML_SWAP(pvec[ipivot].dat[ll], pvec[ipivot].dat[ll + 1]);
                // >>
                if (plink != NULL) {
                    for (icol = 0; icol < icount; icol++) {
                        if ((icol != ipivot) && (plink[icol] == 1)) {
                            ML_SWAP(pvec[icol].dat[ll], pvec[icol].dat[ll + 1]);
                        }
                    }
                }
                // <<
            }
            ii = ll + 1;
            jj = ir;
            aa = pvec[ipivot].dat[ll + 1];
            // >>
            if (plink != NULL) {
                for (icol = 0; icol < icount; icol++) {
                    if ((icol != ipivot) && (plink[icol] == 1)) {
                        ca[icol] = pvec[icol].dat[ll + 1];
                    }
                }
            }
            // <<
            for (jsec = istart; jsec < ML_MAXPOINTS; jsec++) {
                do { ii++; } while (pvec[ipivot].dat[ii] < aa);
                do { jj--; } while (pvec[ipivot].dat[jj] > aa);
                if (jj < ii) { break; }
                ML_SWAP(pvec[ipivot].dat[ii], pvec[ipivot].dat[jj]);

                // >>
                if (plink != NULL) {
                    for (icol = 0; icol < icount; icol++) {
                        if ((icol != ipivot) && (plink[icol] == 1)) {
                            ML_SWAP(pvec[icol].dat[ii], pvec[icol].dat[jj]);
                        }
                    }
                }
                // <<
            }
            pvec[ipivot].dat[ll + 1] = pvec[ipivot].dat[jj];
            pvec[ipivot].dat[jj] = aa;
            // >>
            if (plink != NULL) {
                for (icol = 0; icol < icount; icol++) {
                    if ((icol != ipivot) && (plink[icol] == 1)) {
                        pvec[icol].dat[ll + 1] = pvec[icol].dat[jj];
                        pvec[icol].dat[jj] = ca[icol];
                    }
                }
            }
            // <<
            jstack += 2;
            if (jstack > SORT_STACK) {
                _tcscpy(pLib->message, _T("Vector sorting limit reached"));
                if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
                free(istack); istack = NULL;
                return;
            }
            if (ir - ii + 1 >= jj - ll) {
                istack[jstack] = ir;
                istack[jstack - 1] = ii;
                ir = jj - 1;
            }
            else {
                istack[jstack] = jj - 1;
                istack[jstack - 1] = ll;
                ll = ii;
            }
        }
    }

    pLib->errcode = 0;
    if (iasc == 0) {
        ml_vector_rev(&pvec[ipivot], dim, pLib);
        if (plink != NULL) {
            for (icol = 0; icol < icount; icol++) {
                if ((icol != ipivot) && (plink[icol] == 1)) {
                    ml_vector_rev(&pvec[icol], dim, pLib);
                }
            }
        }
    }

    free(istack); istack = NULL;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_vector_rev          :    inverser un vecteur
    
        pvec               :    pointeur sur le vecteur
        dim                :    nombre d'éléments du vecteur
    -------------------------------------------------------------------- */
void ml_vector_rev(vector_t *pvec, int_t dim, libvar_t *pLib)
{
    int_t ii, jj, isec, istart, istop;
    real_t ftmp;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;
    
    if (pvec == NULL) {
        _tcscpy(pLib->message, _T("Invalid column"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        return;
    }

    if ((dim < 1) || (dim > ML_MAXPOINTS)) {
        _tcscpy(pLib->message, _T("Invalid data"));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_sort]"), ML_STRSIZE - 1); }
        return;
    }

    if (dim == 1) {
        pLib->errcode = 0;
        return;
    }
    else if (dim == 2) {
        ftmp = pvec->dat[0];
        pvec->dat[0] = pvec->dat[1];
        pvec->dat[1] = ftmp;
        pLib->errcode = 0;
        _tcscpy(pLib->message, _T("Done."));
        if  (ml_debug == 1) { _tcsncat(pLib->message, _T("   [ml_vector_rev]"), ML_STRSIZE - 1); }
        return;
    }

    // ignore invalid items (not yet edited for example)
    istart = 0;
    istop = dim - 1;
    if (pvec->mask[istart] == 0xFF) {
        for (ii = istart; ii < istop; ii++) {
            if (pvec->mask[ii] != 0xFF) {
                istart = ii;
                break;
            }
        }
    }
    if (pvec->mask[istop] == 0xFF) {
        for (ii = istop; ii > istart; ii--) {
            if (pvec->mask[ii] != 0xFF) {
                istop = ii;
                break;
            }
        }
    }

    ii = istart; jj = istop;
    for (isec = istart; isec < ML_MAXPOINTS; isec++) {
        ftmp = pvec->dat[ii];
        pvec->dat[ii] = pvec->dat[jj];
        pvec->dat[jj] = ftmp;
        ii += 1;
        jj -= 1;
        if (ii >= jj) {
            break;
        }
    }

    pLib->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    :TODO: complete doc and translate to english

    ml_writedat         :    sauvegarde de données dans un fichier ascii
    
        filename        :    nom du fichier (crée s'il n'existe pas et écrasé sinon)
        x, y, z         :    pointeurs sur les données à sauvegarder (trois colonnes
                             x, y et z, avec comme séparateur la tabulation
        ncol            :    nombre de colonnes (1, 2 ou 3)
        ndat            :    nombre de points
    -------------------------------------------------------------------- */
int_t ml_writedat(const char_t* filename, real_t *x, real_t *y, real_t *z,
                        int_t ncol, int_t ndat, libvar_t *plib)
{
    FILE* pf = NULL;
    int_t i = 0;
    char_t line[ML_STRSIZEW], linefmt[ML_STRSIZES];

    if (plib->errcode) { return 0; }

    plib->errcode = 1;

    if ((ncol < 1) || (ncol > 3)) {
        _tcscpy(plib->message, _T("Invalid data"));
        if (ml_debug == 1) { _tcsncat(plib->message, _T("   [ml_writedat]"), ML_STRSIZE - 1); }
        return 0;    
    }

    if ((ndat < 2) || (ndat > ML_MAXPOINTS)) {
        _tcscpy(plib->message, _T("Invalid data"));
        if (ml_debug == 1) { _tcsncat(plib->message, _T("   [ml_writedat]"), ML_STRSIZE - 1); }
        return 0;    
    }

    pf = _wfopen(filename, _T("w"));
    if (pf == NULL) {
        _tcscpy(plib->message, _T("Cannot open file"));
        if (ml_debug == 1) { _tcsncat(plib->message, _T("   [ml_writedat]"), ML_STRSIZE - 1); }
        return 0;    
    }

    memset((char_t*)line, 0, sizeof(char_t) * ML_STRSIZEW);
    memset((char_t*)linefmt, 0, sizeof(char_t) * ML_STRSIZES);

    if (ml_linefmt[0] == _T('\0')) { 
        switch(ncol) {
            case 1:
                _tcscpy(linefmt, _T("%.6e\n"));
                break;
            case 2:
                _tcscpy(linefmt, _T("%.6e\t%.6e\n"));
                break;
            case 3:
                _tcscpy(linefmt, _T("%.6e\t%.6e\t%.6e\n"));
                break;
            default:
                _tcscpy(linefmt, _T("%.6e\t%.6e\n"));
                break;
        }
    }
    else { 
        _tcscpy(linefmt, (const char_t*)ml_linefmt);
    }

    plib->progress = 0;

    for (i = 0; i < ndat; i++) {
        switch(ncol) {
            case 1:
                _tcsprintf(line, (ML_STRSIZEW - 1), (const char_t*)linefmt, x[i]);
                break;
            case 2:
                _tcsprintf(line, (ML_STRSIZEW - 1), (const char_t*)linefmt, x[i], y[i]);
                break;
            case 3:
                _tcsprintf(line, (ML_STRSIZEW - 1), (const char_t*)linefmt, x[i], y[i], z[i]);
                break;
            default:
                _tcsprintf(line, (ML_STRSIZEW - 1), (const char_t*)linefmt, x[i], y[i]);
                break;
        }        
        if (fputws((const char_t*)line, pf) == EOF) {
            break;
        }
    }

    fclose(pf); pf = NULL;
    plib->errcode = 0;
    return (i+1);
}
