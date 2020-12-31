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

// :TODO: to complete doc and translate to english

#ifndef LIBPLOT_LEGACY_H
#define LIBPLOT_LEGACY_H

// PLOT STRUCT
typedef struct _plot_v24_t
{
    // Type
    byte_t type;                                   //    Type de tracé :    0x1B:xy
                                                   //                            0x2B:Barres
                                                   //                            0x3B:Colonnes
                                                   //                            0x4B:Histogramme
                                                   //                            0x5B:Proba
                                                   //                            0x6B:Polaire

    // Cadre
    frame_t frame;                                 //    Fenêtre du graphe
    dc_t dc;                                       //    Device Context du graphe
    color_t backcolor, pbackcolor;                 //    Couleurs des fonds de la fenêtre et du tracé - RGB

    // Positions
    rect_t relpos[PL_FRAMEITEMS];                  //    Position des différents rectangles
                                                   //    0:WND, 1:PLOT, 2:TITLE

    // Titre
    text_t title;                                  //    Titre du graphe
    byte_t otitle;                                 //    Option Titre

    // Axes
    axis_t axisbottom, axisleft, axistop, axisright;
    char_t linktop[ML_STRSIZE];                    //    Formule d'axe lié
    char_t linkright[ML_STRSIZE];                  //    Formule d'axe lié

    // Courbes
    int_t curvecount;                             //    Nombre de tracés
    int_t curveactive;                            //    Tracé actif
    curve_t curve[ML_MAXCURVES];                  //    Tracés

    // Lignes
    int_t linevertcount;                          //    Nombre de lignes verticales
    int_t linehorzcount;                          //    Nombre de lignes horizontales
    line_t linevert[PL_MAXITEMS];                 //    Lignes verticales 
    line_t linehorz[PL_MAXITEMS];                 //    Lignes horizontales

    // Text
    int_t textcount;                              //    Nombre de zones de texte
    text_t text[PL_MAXITEMS];                     //    Zones de texte

    // Lignes GEO
    int_t linecount;                              //    Nombre de lignes
    line_t line[PL_MAXITEMS];                     //    Lignes

    // Rectangles GEO
    int_t rectanglecount;                         //    Nombre de zones de texte
    rectangle_t rectangle[PL_MAXITEMS];           //    Rectangle

    // Ellipses GEO
    int_t ellipsecount;                           //    Nombre d'ellipses
    ellipse_t ellipse[PL_MAXITEMS];               //    Ellipses

    plint_t plint;                                //    Structure interne au plot

    int_t errcode;                                // Variable d'erreur (0 si OK)
    char_t message[ML_STRSIZE];                   //    Message (erreur, info, ...)

    int_t modpoints;                              // Nombre max. de points à tracer

    int_t ireserved;                              // Variable int_t réservée
    byte_t breserved;                             // Variable byte_t réservée
} plot_v24_t;

#endif
