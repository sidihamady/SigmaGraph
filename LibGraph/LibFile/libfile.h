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

#ifndef LIBFILE_H
#define LIBFILE_H

extern "C" {
#include "../../LibCalc/LibElec/libelec.h"
}
#include "../LibPlot/libplot.h"

// Settings
typedef struct _import_t
{
    int_t iType;                                                    // default type : txt, dat, csv, ...
    char_t cSep;                                                    // Delimiter
    int_t iSkip;                                                    // Lines to skip
    int_t iTitle;                                                   // Line of Column title
    int_t iCol;                                                     // Columns count
    int_t iRow;                                                     // Rows count
    int_t iAdd;                                                     // Add to current datasheet
    int_t iNote;                                                    // Add skipped lines to datasheet note
    int_t iShow;                                                    //    Show Import Dialog
} import_t;

typedef struct _console_t
{
    byte_t angleunit;                                                // 0x00:radians, 0x01:degrees; 0x02:grad
    font_t font;                                                     // Font
    real_t tol;
    real_t reltol;
} console_t;

typedef struct _script_t
{
    font_t font;                                                    // Font
} script_t;

typedef struct _options_t
{
    byte_t winstate;                                                // 0x00:normal, 0x01:maximized; 0x02:minimized
    import_t import;
    console_t console;
    script_t script;
    fit_t fit;
    byte_t opt[32];
} options_t;

typedef struct _software_t
{
    int_t version;                                   // Sigma software version
    char_t name[ML_STRSIZEN];                        // Sigma software name
    char_t description[ML_STRSIZE];                  // Sigma software description
    char_t copyright[ML_STRSIZE];                    // Sigma software copyright
    char_t date[ML_STRSIZEN];                        // Sigma software release date (Mmm dd yyyy) 
    char_t time[ML_STRSIZEN];                        // Sigma software release time (hh:mm:ss) 
    char_t reserved[ML_STRSIZE];                     // Sigma software reserved data
} software_t;

typedef struct _document_t
{
    // Document persistent data
    software_t soft;                                 // Sigma software informations
    int_t id;                                        // ID, unique
    char_t name[ML_STRSIZE];                         // Name
    char_t note[ML_STRSIZEW];                        // Associated note
    int_t type;                                      // Type
    byte_t status;                                   // State : shown, hidden, read-only, ...
    date_t created;                                  // Document creation date
    date_t modified;                                 // Document modification date

    long_t cid;                                      // Last Column ID (each column has unique ID)
    int_t count;                                     // Number of data columns
    vector_t column[ML_MAXCOLUMNS];                  // Data columns
    stat_t stats[ML_MAXCOLUMNS];                     // Data statistics
    //
} document_t;

typedef struct _workspace_t
{
    int_t nVersion;                                  // Sigma file version

    char_t szOutput[ML_STRSIZEL];                    // Output window
    char_t szConsole[ML_STRSIZEL];                   // Console window

    char_t szFormatF[ML_STRSIZET];                   // Forrmat Float    :    "%.6f"
    char_t szFormatE[ML_STRSIZET];                   // Forrmat Exp      :    "%.3e"
    byte_t bFormat;                                  // Format options
} workspace_t;

typedef struct _favorite_t
{
    // Favorite script
    int_t id;                                        // ID, unique
    int_t type;                                      // Type
    int_t length;                                    // Script length
    char_t path[ML_STRSIZE];                         // File path
    char_t name[ML_STRSIZE];                         // Name
    byte_t status;                                   // State (0x1B = active ...)
    byte_t autorun;                                  // 0x1B = autorun, 0xA1 = autoload
} favorite_t;

/*    VARIABLE D'INTERACTION */
extern const int_t fl_version;                       /* Version de libfile */
extern const int_t fl_debug;                         /* Variable de déboguage */

/*  --------------------------------------------------------------------
    fl_reset        :    Remise à zéro.
    -------------------------------------------------------------------- */
void fl_reset(libvar_t *pLib);

/*  --------------------------------------------------------------------
    fl_wndcapt      :    capture d'une fenêtre et sauvegarde de l'image dans
                        un fichier bitmap

        hWnd        :    handle de la fenêtre
        filename    :    nom du fichier dans lequel sauvegarder l'image
    -------------------------------------------------------------------- */
void fl_wndcapt(HWND hWnd, const char_t *filename, byte_t iformat, libvar_t *pLib);

void fl_gettime(date_t* tm);

/*  --------------------------------------------------------------------
    fl_getshort          :    Accéder au nom de fichier court (sans le chemin)

        filenameShort    :    Chaîne où sera sauvegardé le nom court
        filename         :    nom du fichier long

        Retourne 0 si OK, 1 sinon.
    -------------------------------------------------------------------- */
int_t fl_getshort(char_t* filenameShort, char_t* filename);

/*  --------------------------------------------------------------------
    fl_filesize          :    Accéder à la taille (en octets) d'un fichier

        filename         :    nom du fichier

        Retourne la taille en octets.
    -------------------------------------------------------------------- */
long_t fl_filesize(const char_t* filename);

long_t fl_import_asciiv(const char_t* filename, vector_t *pColumn, int_t *piColumn, int_t *pColumnCount, int_t *pRowCount,
                        long_t *pvecCurID, char_t *pszNote, char_t cSep, int_t iSkip, int_t iTitle,
                        int_t iCreateColumn, libvar_t *pLib);

long_t fl_export_asciiv(const char_t* filename, vector_t *pColumn, int_t nColumnCount, char_t cSep,
                        libvar_t *pLib, const char_t* header);

/*  --------------------------------------------------------------------
    fl_bigendian    :    Format des données : Big Endian ?
        retourne 1 si BigEndian
                 0 sinon
    -------------------------------------------------------------------- */
int_t fl_bigendian();

void fl_byteswap(byte_t *b, int_t n);

#define fl_swapbytes(x) fl_bytesswap((byte_t*)&x, sizeof(x))

/*  -------------------------------------------------------------------- */

#endif

