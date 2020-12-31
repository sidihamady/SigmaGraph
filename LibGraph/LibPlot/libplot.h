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

#ifndef LIBPLOT_H
#define LIBPLOT_H

extern "C" {
#include "../../LibCalc/LibMath/libmath.h"
}

#include <windows.h>
#include <gdiplus.h>

#include "svg.h"

// MACROS
#define PL_FRAMEITEMS             3
// Nombre max de lignes verticales et horizontales
#define PL_MAXITEMS               16
// Taille max des lignes
#define PL_MAXLINESIZE            8

#define PL_MINLMARGIN            64.0
#define PL_MINTMARGIN            48.0
#define PL_MINWIDTH              128.0
#define PL_MINHEIGHT             96.0
#define PL_MAXNGRID              20
#define PL_MAXNGRIDM             10
#define PL_DELTA                 4
#define PL_RESIZE                4
#define PL_MODPOINTS_MIN         32
#define PL_MODPOINTS_MAX         4096
#define PL_MODPOINTS_DEF         2048

// Plot Type
#define    PLOTTYPE_XY           0x10

#define PL_BLOCK_START          _T('{')
#define PL_BLOCK_END            _T('}')

// TYPE DEF
typedef HWND                    frame_t;
typedef HDC                     dc_t;
typedef RECT                    irect_t;
typedef POINT                   ipoint_t;
typedef HFONT                   fontptr_t;

typedef struct _color_t
{
    byte_t r, g, b;
} color_t;

typedef struct _font_t {
    char_t name[ML_STRSIZES];            // Font face name
    color_t color;                       // Color
    int_t size;                          // Size
    int_t orient;                        // Orientation
    byte_t style;                        // Normal, Bold or Underline
    fontptr_t ptr;                       // Dynamic
} font_t;

typedef struct _text_t {
    long_t id;                           // ID, unique
    byte_t type;                         // 0x00 = Simple ligne, 0x02 = Multiligne
    byte_t status;                       // Statut
    char_t text[ML_STRSIZE];
    int_t len;
    rect_t rect;
    font_t font;
    int_t border;
} text_t;

typedef struct _line_t {
    long_t id;                           // ID, unique
    byte_t status;                       // Statut
    rect_t rect;
    color_t color;
    byte_t style;                        // 0x00 : line, 0xAB : arrow
    int_t size;
    int_t arrwidth;                      // Arrow width
    int_t arrlength;                     // Arrow length
    byte_t arrstyle;                     // Arrow style
    byte_t orient;                       // 0x00: oriented like \.    0x01: oriented like /.
} line_t;

typedef struct _rectangle_t {
    long_t id;                           // ID, unique
    byte_t status;                       // Statut
    rect_t rect;
    color_t color;
    color_t fcolor;
    byte_t style;
    int_t size;
} rectangle_t;

typedef struct _ellipse_t {
    long_t id;                           // ID, unique
    byte_t status;                       // Statut
    rect_t rect;
    color_t color;
    color_t fcolor;
    byte_t style;
    int_t size;
} ellipse_t;

// PLOT 2D
typedef struct _plint_t
{
    byte_t running;

    real_t dbottom;
    real_t dmbottom;
    real_t dtop;
    real_t dmtop;
    real_t dleft;
    real_t dmleft;
    real_t dright;
    real_t dmright;

    real_t width;
    real_t height;

    irect_t frameRect[PL_FRAMEITEMS];                        // Position des différents rectangles
                                                             // 0:WND, 1:PLOT, 2:TITLE

    irect_t axistRect[4];                                    // Position des différents rectangles
                                                             // 0:TITLE-BOTTOM        1:TITLE-LEFT
                                                             // 2:TITLE-TOP            3:TITLE-RIGHT

    irect_t axislRect[4];                                    // Position des différents rectangles
                                                             // 0:LABEL-BOTTOM        1:LABEL-LEFT
                                                             // 2:LABEL-TOP            3:LABEL-RIGHT

    irect_t legendsRect;                                            // Position des légendes des courbes
    irect_t legendRect[ML_MAXCURVES];                        // Position des légendes des courbes

    irect_t linevertRect[PL_MAXITEMS];                       // Position des ligne verticales
    irect_t linehorzRect[PL_MAXITEMS];                       // Position des ligne horizontales

    irect_t textRect[PL_MAXITEMS];                           // Position des zones de texte
    irect_t lineRect[PL_MAXITEMS];                           // Position des lignes
    irect_t rectangleRect[PL_MAXITEMS];                      // Position des rectangles
    irect_t ellipseRect[PL_MAXITEMS];                        // Position des ellipses

    ipoint_t *curvePointA[ML_MAXCURVES];                     // Position des points de chaque courbe - 4 pixels
    ipoint_t *curvePointB[ML_MAXCURVES];                     // Position des points de chaque courbe + 4 pixels
} plint_t;

typedef struct _axis_t
{
    byte_t status;                                           // ScaleAuto:0x00, ScaleManual:0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, ..., 0xF0, ..., NonVisible:0x_F
    real_t min;                                              // Min
    real_t max;                                              // Max
    byte_t op;                                               // Opération sur l'axe (abs, ...)

    text_t title;                                            // Titre

    char_t format[ML_STRSIZES];                              // Format
    byte_t scale;                                            // Echelle
    byte_t size;                                             // Line size
    color_t color;                                           // Couleur de l'axe

    real_t increment;                                        // Incrément de l'échelle

    byte_t gridstyle;                                        // Largeur du quadrillage (majeur)
    byte_t gridcount;                                        // Nombre de divisions verticales (majeures)
    color_t gridcolor;                                       // Couleur du quadrillage (majeur) - RGB
    byte_t gridsize;                                         // Style du quadrillage (majeur)

    byte_t gridmstyle;                                       // Largeur du quadrillage (mineur)
    byte_t gridmcount;                                       // Nombre de divisions verticales (mineures)
    color_t gridmcolor;                                      // Couleur du quadrillage (mineur) - RGB
    byte_t gridmsize;                                        // Largeur du quadrillage (mineur)

    byte_t tickstyle;                                        // Style du tick (majeur)
    byte_t ticksize;                                         // Largeur du tick (majeur)
    byte_t ticklen;                                          // Longueur du tick (majeur)
    color_t tickcolor;                                       // Couleur des ticks (majeur) - RGB

    byte_t tickmstyle;                                       // Style du tick (mineur)
    byte_t tickmsize;                                        // Largeur du tick (mineur)
    byte_t tickmlen;                                         // Longueur du tick (mineur)
    color_t tickmcolor;                                      // Couleur des ticks (mineur) - RGB

    font_t lfont;                                            // Police label
    rect_t lrelpos;                                          // Position label

    rect_t trelpos;                                          // Position titre

    byte_t ogrid, ogridm;                                    // Options Grid (majeur et mineur)
    byte_t otick, otickm;                                    // Options Tick (majeur et mineur)
    byte_t otitle, olabel;                                   // Options titre et label
} axis_t;

typedef struct _curve_t
{
    byte_t status;                                           // Tracé visible, ...
    int_t points;                                            // Nombre de points

    vector_t *x;                                             // Pointeur vers les x | Dynamic
    vector_t *y;                                             // Pointeur vers les y | Dynamic
    vector_t *ex;                                            // Pointeur vers les e | Dynamic
    vector_t *ey;                                            // Pointeur vers les e | Dynamic
    long_t idx;                                              // ID de X
    long_t idy;                                              // ID de Y

    byte_t axisused;                                         // Axis used : 0x00 (XY), 0x01 (XY2), 0x10 (X2Y), 0x11 (X2Y2)

    text_t legend;                                           // Légende du tracé

    rect_t lrelpos;                                          // Position légende

    byte_t linestyle;                                        // Ligne:    0xAB avec A = ligne et B = connect
                                                             //             0xA1 = droite
                                                             //             0xA2 = step horizontal, 0xA3 = step horizontal centré,
                                                             //             0xA4 = step vertical, 0xA5 = step vertical centré
    byte_t linesize;                                         // Taille de ligne
    color_t linecolor;                                       // Couleur de ligne - RGB

    byte_t exstyle;                                          // Ligne:    0xAB avec A = ligne et B = 0 (barres d'erreur X)
    byte_t exsize;                                           // Taille de ligne (barres d'erreur X)
    color_t excolor;                                         // Couleur de ligne (barres d'erreur X) - RGB
    byte_t eystyle;                                          // Ligne:    0xAB avec A = ligne et B = 0 (barres d'erreur Y)
    byte_t eysize;                                           // Taille de ligne (barres d'erreur X)
    color_t eycolor;                                         // Couleur de ligne (barres d'erreur X) - RGB

    byte_t dotstyle;                                         // Symbole
    byte_t dotsize;                                          // Taille du symbole
    color_t dotcolor;                                        // Couleur de symbole - RGB

    byte_t dropstyleh;                                       // Horizontal Line drop:    0x00 = sans, 0x1A = horizontal, 0xA1 = vertical
    byte_t dropsizeh;                                        // Epaisseur des line drop (1 = fin, ..., 5 = trés épais)
    color_t dropcolorh;                                      // Couleur des line drop - RGB

    byte_t dropstylev;                                       // Vertical Line drop:    0x00 = sans, 0x1A = horizontal, 0xA1 = vertical
    byte_t dropsizev;                                        // Epaisseur des line drop (1 = fin, ..., 5 = trés épais)
    color_t dropcolorv;                                      // Couleur des line drop - RGB

    byte_t olegend;                                          // Options légende
} curve_t;

#include "libplot_v24.h"

// PLOT STRUCT
typedef struct _plot_t
{
    // plot_v24_t -------------------------------------------------------------------------------------------
    // Type
    byte_t type;                                             // Type de tracé :    0x1B:xy
                                                             //                         0x2B:Barres
                                                             //                         0x3B:Colonnes
                                                             //                         0x4B:Histogramme
                                                             //                         0x5B:Proba
                                                             //                         0x6B:Polaire

    // Cadre
    frame_t frame;                                           // Fenêtre du graphe
    dc_t dc;                                                 // Device Context du graphe
    color_t backcolor, pbackcolor;                           // Couleurs des fonds de la fenêtre et du tracé - RGB

    // Positions
    rect_t relpos[PL_FRAMEITEMS];                            // Position des différents rectangles
                                                             // 0:WND, 1:PLOT, 2:TITLE

    // Titre
    text_t title;                                            // Titre du graphe
    byte_t otitle;                                           // Option Titre

    // Axes
    axis_t axisbottom, axisleft, axistop, axisright;
    char_t linktop[ML_STRSIZE];                              // Formule d'axe lié
    char_t linkright[ML_STRSIZE];                            // Formule d'axe lié

    // Courbes
    int_t curvecount;                                        // Nombre de tracés
    int_t curveactive;                                       // Tracé actif
    curve_t curve[ML_MAXCURVES];                             // Tracés

    // Lignes
    int_t linevertcount;                                     // Nombre de lignes verticales
    int_t linehorzcount;                                     // Nombre de lignes horizontales
    line_t linevert[PL_MAXITEMS];                            // Lignes verticales 
    line_t linehorz[PL_MAXITEMS];                            // Lignes horizontales

    // Text
    int_t textcount;                                         // Nombre de zones de texte
    text_t text[PL_MAXITEMS];                                // Zones de texte

    // Lignes GEO
    int_t linecount;                                         // Nombre de lignes
    line_t line[PL_MAXITEMS];                                // Lignes

    // Rectangles GEO
    int_t rectanglecount;                                    // Nombre de zones de texte
    rectangle_t rectangle[PL_MAXITEMS];                      // Rectangle

    // Ellipses GEO
    int_t ellipsecount;                                      // Nombre d'ellipses
    ellipse_t ellipse[PL_MAXITEMS];                          // Ellipses

    plint_t plint;                                           // Structure interne au plot

    int_t errcode;                                           // Variable d'erreur (0 si OK)
    char_t message[ML_STRSIZE];                              // Message (erreur, info, ...)

    int_t modpoints;                                         // Nombre max. de points à tracer

    int_t ireserved;                                         // Variable int_t réservée
    byte_t breserved;                                        // Variable byte_t réservée
    // ------------------------------------------------------------------------------------------------------

    // GDI+
    int imagetype;                                           // 0=None, 1=EMF, 2=SVG
    char_t imagefilename[ML_STRSIZE];                        // Image filename
    svgDocument *svgdoc;                                     // SVG document
    bool imagesaved;                                         // SVG successfully saved?
    Gdiplus::Metafile *metafile;                             // GDI+ metafile class for image saving
    Gdiplus::Graphics *image;                                // GDI+ graphics class for image saving
    Gdiplus::Graphics *canvas;                               // GDI+ graphics class
    Gdiplus::Pen *pen;                                       // GDI+ graphics pen
    Gdiplus::Brush *brush;                                   // GDI+ graphics brush
    Gdiplus::Font *font;                                     // GDI+ graphics font
} plot_t;

// EXTERN DECL

extern const int_t pl_version;                               // Version de libplot
extern const int_t pl_debug;                                 // Variable de déboguage
extern const real_t pl_logzero;                              // Valeur prise pour log(x) en x~0
extern const real_t pl_loginf;                               // Valeur prise pour log(x) pour x 'très grand'
extern const ulong_t pl_perfcount;                           // Compteur de perf. : durée (microsec.)


/* ####################################################################
    Fonctions exportées
    Fichiers : libplot.c, plot.c
    #################################################################### */

/*  --------------------------------------------------------------------
    pl_reset         :    ré-initialisation des données internes

        istatic      :    valider uniquement les données statiques (et supprimer
                        les données dynamiques)
        pplot        :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
void pl_reset(plot_t* pplot, byte_t istatic);

/*  --------------------------------------------------------------------
    pl_validate      :    validation les données

        pplot        :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
void pl_validate(plot_t* pplot, byte_t istatic);

/*  --------------------------------------------------------------------
    pl_autoscale    :    Calcul automatique de l'échelle

        pplot       :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
void pl_autoscale(plot_t *pplot);

// >> 20160122: auto label tick improvement
void pl_autotick(double *pfMin, double *pfMax, double *pfStep, int *piGrid, byte_t iscale);
//

/*  --------------------------------------------------------------------
    pl_plot        :    Tracé du graphe

        pplot      :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
void pl_plot(plot_t* pplot, bool IsPrinting = false);

/*  --------------------------------------------------------------------
    pl_curve_add          :    Ajouter une courbe au tracé

        pplot             :    pointeur sur la structure du tracé
        pvecX, pvecY      :    pointeurs sur les vecteurs X et Y
        pvecEX, pvecEY    :    pointeurs sur les vecteurs barres d'erreur X et Y
        axisused          :    axes : 0x00 (XY), 0x01 (XY2), 0x10 (X2Y), 0x11 (X2Y2)

        retourne l'indice de la courbe
    -------------------------------------------------------------------- */
int_t pl_curve_add(plot_t* pplot, vector_t *pvecX, vector_t *pvecY,
                         vector_t *pvecEX, vector_t *pvecEY, byte_t axisused,
                         byte_t autoformat);

/*  --------------------------------------------------------------------
    pl_curve_modify     :    Modifier les éléments dynamiques d'une courbe

        pplot           :    pointeur sur la structure du tracé
        pxOld, pyOld    :    pointeurs sur les anciens vecteurs X et Y
        pxNew, pyNew    :    pointeurs sur les nouveaux vecteurs X et Y
        pEX, pEY        :    pointeurs sur les nouveaux vecteurs barres
                             d'erreur X et Y
    -------------------------------------------------------------------- */
void pl_curve_modify(plot_t* pplot, vector_t *pxOld, vector_t *pyOld,
                            vector_t *pxNew, vector_t *pyNew,
                            vector_t *pEX, vector_t *pEY);

/*  --------------------------------------------------------------------
    pl_curve_removei    :    Enlever une courbe du tracé

        pplot           :    pointeur sur la structure du tracé
        id                    :    ID (unique) de la colonne Y
    -------------------------------------------------------------------- */
void pl_curve_removei(plot_t* pplot, long_t id);

/*  --------------------------------------------------------------------
    pl_curve_remove    :    Enlever une courbe du tracé

        pplot                :    pointeur sur la structure du tracé
        iCurve            :    indice de la courbe à enlever
    -------------------------------------------------------------------- */
void pl_curve_remove(plot_t* pplot, int_t iCurve);

/*  --------------------------------------------------------------------
    pl_curve_dup        :    Dupliquer une courbe du tracé

        pplot                :    pointeur sur la structure du tracé
        iFrom                :    indice de la courbe source
        iTo                :    indice de la courbe destination
    -------------------------------------------------------------------- */
void pl_curve_dup(plot_t* pplot, int_t iTo, int_t iFrom);

/*  --------------------------------------------------------------------
    pl_plot_dupstyle    :    Dupliquer le style d'un tracé

        pFrom                :    pointeur sur le tracé source
        pTo                :    pointeur sur le tracé destination
    -------------------------------------------------------------------- */
void pl_plot_dupstyle(plot_t* pTo, const plot_t* pFrom);

/*  --------------------------------------------------------------------
    pl_plot_dupstylei    :    Dupliquer le style items [texte, géo] d'un tracé

        pFrom                :    pointeur sur le tracé source
        pTo                :    pointeur sur le tracé destination
    -------------------------------------------------------------------- */
void pl_plot_dupstylei(plot_t* pTo, const plot_t* pFrom);

/*  --------------------------------------------------------------------
    pl_plot_dupstatic    :    Dupliquer les éléments statiques d'un tracé

        pFrom                :    pointeur sur la structure du tracé source
        pTo                :    pointeur sur la structure du tracé destination
    -------------------------------------------------------------------- */
void pl_plot_dupstatic(plot_t* pTo, const plot_t* pFrom);

/*  --------------------------------------------------------------------
pl_plot_dupstatic    :    Dupliquer les éléments statiques d'un tracé

pFrom                :    pointeur sur la structure du tracé source
pTo                  :    pointeur sur la structure du tracé destination
-------------------------------------------------------------------- */
void pl_plot_dupstatic(plot_t* pTo, const plot_v24_t* pFrom);

// auto label tick
class lmAxisLabel
{

    // lmAxisLabel: class implementing the algorithm described in Justin Talbot et al., "An Extension of Wilkinson’s Algorithm for Positioning Tick Labels on Axes", InfoVis 2010.

private:
    double m_fMin;
    double m_fMax;
    double m_fStep;
    double m_fScore;
    double m_arrQ[6];
    int m_iQ;
    double m_arrW[4];
    int m_iW;
    double m_fEps;

    double weight(double fs, double fc, double fd, double fl)
    {
        return ((m_arrW[0] * fs) + (m_arrW[1] * fc) + (m_arrW[2] * fd) + (m_arrW[3] * fl));
    }

    double modfloor(double fa, double fn)
    {
        return (fa - (fn * floor(fa / fn)));
    }

    double veps(double fmin, double fmax, double fstep)
    {
        return ((modfloor(fmin, fstep) < m_fEps) && (fmin <= 0) && (fmax >= 0)) ? 1.0 : 0.0;
    }

    double simplicity(int ii, int jj, double fmin, double fmax, double fstep)
    {
        if (m_iQ > 1) {
            return (1.0 - static_cast<double> (ii) / static_cast<double> (m_iQ - 1) - static_cast<double> (jj) +veps(fmin, fmax, fstep));
        }
        else {
            return (1.0 - static_cast<double> (jj) +veps(fmin, fmax, fstep));
        }
    }

    double simplicitymax(int ii, int jj)
    {
        if (m_iQ > 1) {
            return (1.0 - (static_cast<double> (ii) / static_cast<double> (m_iQ - 1)) - static_cast<double> (jj + 1));
        }
        else {
            return (1.0 - static_cast<double> (jj + 1));
        }
    }

    double density(int rr, int mm, double fmin, double fmax, double lmin, double lmax)
    {
        double ft = static_cast<double> (rr - 1) / (lmax - lmin);
        double ftt = static_cast<double> (mm - 1) / (max(lmax, fmax) - min(lmin, fmin));
        return (2.0 - max(ft / ftt, ftt / ft));
    }

    double densitymax(int rr, int mm)
    {
        if (rr >= mm) {
            return (2.0 - (static_cast<double> (rr - 1) / static_cast<double> (mm - 1)));
        }
        else {
            return 1.0;
        }
    }

    double coverage(double fmin, double fmax, double lmin, double lmax)
    {
        double fa = fmax - lmax;
        double fb = fmin - lmin;
        double fc = 0.1 * (fmax - fmin);
        return (1.0 - (0.5 * ((fa * fa + fb * fb) / (fc * fc))));
    }

    double coveragemax(double fmin, double fmax, double fspan)
    {
        const double frange = fmax - fmin;
        if (fspan > frange) {
            double fhalf = (fspan - frange) / 2.0;
            double fr = 0.1 * frange;
            return (1.0 - (fhalf * fhalf / (fr * fr)));
        }
        else {
            return 1.0;
        }
    }

public:
    // Constructors
    lmAxisLabel()
    {
        reset();
    }

    // Destructor
    ~lmAxisLabel()
    {

    }

    void reset()
    {
        m_fMin = 0.0;
        m_fMax = 0.0;
        m_fStep = 0.0;
        m_fScore = 0.0;

        m_arrQ[0] = 1.0;
        m_arrQ[1] = 5.0;
        m_arrQ[2] = 2.0;
        m_arrQ[3] = 2.5;
        m_arrQ[4] = 4.0;
        m_arrQ[5] = 3.0;
        m_iQ = 6;

        m_arrW[0] = 0.25;
        m_arrW[1] = 0.2;
        m_arrW[2] = 0.5;
        m_arrW[3] = 0.05;

        m_fEps = 1e-10;
    }

    void search(double *pfMin, double *pfMax, double *pfStep, int *piGrid);
};

#endif
