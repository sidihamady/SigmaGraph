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

#include "libplot.h"

#include <math.h>

__inline static bool isRealEqual(double tx, double ty, double fprec, bool bUserPrec)
{
    double feps = (bUserPrec == false) ? (fabs(tx) * DBL_EPSILON) : fprec;
    if (feps < DBL_EPSILON) {
        feps = fabs(ty) * DBL_EPSILON;
        if (feps < DBL_EPSILON) {
            feps = DBL_EPSILON;
        }
    }
    return (fabs(tx - ty) < feps);
}

__inline static bool isRealInt(double tx, double fprec)
{
    double ftx = floor(tx);
    double ctx = ceil(tx);
    return (isRealEqual(tx, ftx, fprec, true) && isRealEqual(tx, ctx, fprec, true));
}

// Reset
__inline static void pl_reset_axis(plot_t* pplot, byte_t iAxis);
__inline static void pl_reset_curve(plot_t* pplot, int_t iCurve, byte_t bnew);

// Validate
__inline static byte_t pl_validate_axis(plot_t* pplot, byte_t iAxis);

// Draw (Axis)
__inline static void pl_draw_axis(plot_t *pplot, byte_t iAxis);

// Grid
__inline static void pl_grid_axis(plot_t* pplot, byte_t iAxis, int_t iX, int_t iX2, int_t iY, int_t iY2);

// Label
__inline static void pl_label_axis(plot_t* pplot, byte_t iAxis);

// Duplicate Axis
__inline static void pl_axis_dup(axis_t* pTo, const axis_t* pFrom);

// Log Scale
__inline static void pl_logscale(real_t fmin, real_t fmax, real_t *fminl, real_t *fmaxl, byte_t *gridcount);

// DrawText
__inline static void pl_draw_text(plot_t* pplot, text_t* ptext, irect_t* prect);
// Print Text
__inline static void pl_print_text(plot_t* pplot);
// Draw Line
__inline static void pl_draw_line(plot_t* pplot);
// Draw Rectangle
__inline static void pl_draw_rect(plot_t* pplot);
// Draw Ellipse
__inline static void pl_draw_ellipse(plot_t* pplot);
// Draw Arrow
__inline static void pl_draw_arrow(plot_t *pplot, int_t ii);

// Main Functions
static void pl_axis(plot_t* pplot);
static void pl_draw_frame(plot_t* pplot);
static void pl_grid(plot_t* pplot, int_t iX, int_t iX2, int_t iY, int_t iY2, byte_t intX, byte_t intX2);
static void pl_line(plot_t* pplot, int_t iX, int_t iX2, int_t iY, int_t iY2);
static void pl_title(plot_t* pplot);
//

__inline static int getFontSizeSm(font_t *pfont)
{
    int_t iSizeSm = 8;
    if (pfont->size < 9) {
        iSizeSm = pfont->size - 2;
    }
    else if ((pfont->size >= 9) && (pfont->size < 12)) {
        iSizeSm = pfont->size - 3;
    }
    else if ((pfont->size >= 12) && (pfont->size < 16)) {
        iSizeSm = pfont->size - 4;
    }
    else if ((pfont->size >= 16) && (pfont->size < 20)) {
        iSizeSm = pfont->size - 5;
    }
    else if ((pfont->size >= 20) && (pfont->size < 32)) {
        iSizeSm = pfont->size - 6;
    }
    else if (pfont->size >= 32) {
        iSizeSm = pfont->size - 7;
    }
    return iSizeSm;
}

__inline static void setPenTX(plot_t *pplot, Gdiplus::Pen **penT)
{
    if ((penT == NULL) || (*penT == NULL)) {
        pplot->pen = NULL;
        return;
    }

    pplot->pen = *penT;

    // SVG
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        Gdiplus::Color clrT;
        (*penT)->GetColor(&clrT);
        pplot->svgdoc->setStroke(svgStroke((real_t) ((*penT)->GetWidth()), svgColor((int_t) (clrT.GetR()), (int_t) (clrT.GetG()), (int_t) (clrT.GetB()))));
        pplot->svgdoc->setFill(svgFill(svgColor(0, 0, 0, true)));

        Gdiplus::DashStyle istyle = (*penT)->GetDashStyle();
        if (Gdiplus::DashStyleDash == istyle) {
            pplot->svgdoc->setStrokeStyle(SVG_STROKESTYLE_DASH);
        }
        else if (Gdiplus::DashStyleDot == istyle) {
            pplot->svgdoc->setStrokeStyle(SVG_STROKESTYLE_DOT);
        }
        else if (Gdiplus::DashStyleDashDot == istyle) {
            pplot->svgdoc->setStrokeStyle(SVG_STROKESTYLE_DASHDOT);
        }
        else {
            pplot->svgdoc->setStrokeStyle(SVG_STROKESTYLE_SOLID);
        }
    }
}

__inline static void drawLineTX(plot_t *pplot, double x1, double y1, double x2, double y2, bool bJoinRound = true)
{
    pplot->canvas->DrawLine(pplot->pen, (int_t) x1, (int_t) y1, (int_t) (x2), (int_t) (y2));
    // EMF
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        pplot->image->DrawLine(pplot->pen, (int_t) x1, (int_t) y1, (int_t) (x2), (int_t) (y2));
    }
    // SVG
    else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        *(pplot->svgdoc) << svgLine(svgPoint(x1, y1), svgPoint(x2, y2),
            pplot->svgdoc->getStroke(), svgFill(),
            bJoinRound ? uT("round") : uT("miter"),
            bJoinRound ? uT("round") : uT("butt"));
    }
}

__inline static void drawRectangleTX(plot_t *pplot, double x1, double y1, double x2, double y2, Gdiplus::Color *clrT = NULL)
{
    pplot->canvas->DrawRectangle(pplot->pen, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    // EMF
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        pplot->image->DrawRectangle(pplot->pen, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    }
    // SVG
    else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        *(pplot->svgdoc) << svgRectangle(svgPoint(x1, y1), x2 - x1, y2 - y1,
            (NULL == clrT) ? pplot->svgdoc->getStroke() : svgStroke((double) (pplot->pen->GetWidth()), svgColor((int_t) (clrT->GetR()), (int_t) (clrT->GetG()), (int_t) (clrT->GetB()), false)),
            svgFill());
    }
}

__inline static void fillRectangleTX(plot_t *pplot, double x1, double y1, double x2, double y2, Gdiplus::Color *clrT, bool drawborder = true)
{
    pplot->canvas->FillRectangle(pplot->brush, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    // EMF
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        pplot->image->FillRectangle(pplot->brush, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    }
    // SVG
    else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        *(pplot->svgdoc) << svgRectangle(svgPoint(x1, y1), x2 - x1, y2 - y1,
            drawborder ? pplot->svgdoc->getStroke() : svgStroke(0, svgColor(0, 0, 0, true)),
            svgFill(svgColor((int_t) (clrT->GetR()), (int_t) (clrT->GetG()), (int_t) (clrT->GetB()), false)));
    }
}

__inline static void drawEllipseTX(plot_t *pplot, double x1, double y1, double x2, double y2, Gdiplus::Color *clrT = NULL)
{
    pplot->canvas->DrawEllipse(pplot->pen, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    // EMF
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        pplot->image->DrawEllipse(pplot->pen, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    }
    // SVG
    else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        double cx = (x2 + x1) / 2.0;
        double cy = (y2 + y1) / 2.0;
        *(pplot->svgdoc) << svgEllipse(svgPoint(cx, cy), (x2 - x1) / 2.0, (y2 - y1) / 2.0,
            (NULL == clrT) ? pplot->svgdoc->getStroke() : svgStroke((double) (pplot->pen->GetWidth()), svgColor((int_t) (clrT->GetR()), (int_t) (clrT->GetG()), (int_t) (clrT->GetB()), false)),
            svgFill());
    }
}

__inline static void fillEllipseTX(plot_t *pplot, double x1, double y1, double x2, double y2, Gdiplus::Color *clrT)
{
    pplot->canvas->FillEllipse(pplot->brush, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    // EMF
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        pplot->image->FillEllipse(pplot->brush, (int_t) round(x1), (int_t) round(y1), (int_t) round(x2 - x1), (int_t) round(y2 - y1));
    }
    // SVG
    else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        double cx = (x2 + x1) / 2.0;
        double cy = (y2 + y1) / 2.0;
        *(pplot->svgdoc) << svgEllipse(svgPoint(cx, cy), (x2 - x1) / 2.0, (y2 - y1) / 2.0,
            (NULL == clrT) ? pplot->svgdoc->getStroke() : svgStroke((double) (pplot->pen->GetWidth()), svgColor((int_t) (clrT->GetR()), (int_t) (clrT->GetG()), (int_t) (clrT->GetB()), false)),
            svgFill(svgColor((int_t) (clrT->GetR()), (int_t) (clrT->GetG()), (int_t) (clrT->GetB()), false)));
    }
}

__inline static void drawPolygonTX(plot_t *pplot, Gdiplus::Point *ptT, svgPoint *svgptT, int numberPoints, Gdiplus::Color *clrT = NULL)
{
    pplot->canvas->DrawPolygon(pplot->pen, ptT, numberPoints);
    // EMF
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        pplot->image->DrawPolygon(pplot->pen, ptT, numberPoints);
    }
    // SVG
    else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        if (numberPoints > 1) {
            svgPolygon polyT((NULL == clrT) ? pplot->svgdoc->getStroke() : svgStroke((double) (pplot->pen->GetWidth()), svgColor((int_t) (clrT->GetR()), (int_t) (clrT->GetG()), (int_t) (clrT->GetB()), false)),
                pplot->svgdoc->getFill());
            for (int ii = 0; ii < numberPoints; ii++) {
                polyT << svgptT[ii];
            }
            *(pplot->svgdoc) << polyT;
        }
    }
}

__inline static void fillPolygonTX(plot_t *pplot, Gdiplus::Point *ptT, svgPoint *svgptT, int numberPoints, Gdiplus::Color *clrT)
{
    pplot->canvas->FillPolygon(pplot->brush, ptT, numberPoints);
    // SVG
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        pplot->image->FillPolygon(pplot->brush, ptT, numberPoints);
    }
    // EMF
    else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        if (numberPoints > 1) {
            svgPolygon polyT(pplot->svgdoc->getStroke(),
                svgFill(svgColor((int_t) (clrT->GetR()), (int_t) (clrT->GetG()), (int_t) (clrT->GetB()), false)));
            for (int ii = 0; ii < numberPoints; ii++) {
                polyT << svgptT[ii];
            }
            *(pplot->svgdoc) << polyT;
        }
    }
}

__inline static bool drawCurveLineTX(Gdiplus::GraphicsPath *linePath, plot_t *pplot, byte_t istyle, double fxPrev, double fyPrev, double fx, double fy)
{
    double fSizeLx = (fxPrev + fx) / 2.0;
    double fSizeLy = (fyPrev + fy) / 2.0;

    int nSizeLx = (int_t) round(fSizeLx);
    int nSizeLy = (int_t) round(fSizeLy);

    int nxPrev = (int_t) round(fxPrev),
        nyPrev = (int_t) round(fyPrev),
        nx = (int_t) round(fx),
        ny = (int_t) round(fy);

    bool bPath = false;

    switch (istyle & 0x0F) {
        case 0x01:    // .
            if (linePath) {
                linePath->AddLine(Gdiplus::Point(nxPrev, nyPrev), Gdiplus::Point(nx, ny));
                bPath = true;
            }
            else {
                drawLineTX(pplot, nxPrev, nyPrev, nx, ny);
            }
            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                pplot->svgdoc->addPoint(svgPoint(fx, fy));
            }
            break;
        case 0x02:    // .
            drawLineTX(pplot, nxPrev, nyPrev, nx, nyPrev);
            drawLineTX(pplot, nx, nyPrev, nx, ny);
            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                pplot->svgdoc->addPoint(svgPoint(fx, fyPrev));
                pplot->svgdoc->addPoint(svgPoint(fx, fy));
            }
            break;
        case 0x03:    // .
            drawLineTX(pplot, nxPrev, nyPrev, nxPrev, ny);
            drawLineTX(pplot, nxPrev, ny, nx + 1, ny);
            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                pplot->svgdoc->addPoint(svgPoint(fxPrev, fy));
                pplot->svgdoc->addPoint(svgPoint((fx + 1.0), fy));
            }
            break;
        case 0x04:    // .
            drawLineTX(pplot, nxPrev, nyPrev, nSizeLx, nyPrev);
            drawLineTX(pplot, nSizeLx, nyPrev, nSizeLx, ny);
            drawLineTX(pplot, nSizeLx, ny, nx + 1, ny);
            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                pplot->svgdoc->addPoint(svgPoint(fSizeLx, fyPrev));
                pplot->svgdoc->addPoint(svgPoint(fSizeLx, fy));
                pplot->svgdoc->addPoint(svgPoint((fx + 1.0), fy));
            }
            break;
        case 0x05:    // .
            drawLineTX(pplot, nxPrev, nyPrev, nxPrev, nSizeLy + 1);
            drawLineTX(pplot, nxPrev, nSizeLy + 1, nx, nSizeLy + 1);
            drawLineTX(pplot, nx, nSizeLy + 1, nx, ny - 1);
            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                pplot->svgdoc->addPoint(svgPoint(fxPrev, (fSizeLy + 1)));
                pplot->svgdoc->addPoint(svgPoint(fx, (fSizeLy + 1)));
                pplot->svgdoc->addPoint(svgPoint(fx, (fy - 1)));
            }
            break;
        default:    // .
            break;
    }

    return bPath;
}
__inline static void drawCurveDotTX(plot_t *pplot, byte_t dotstyle, byte_t dotsize, double fx, double fy, Gdiplus::Color *clrT, byte_t dotlinesize, Gdiplus::Color *clrDL)
{
    Gdiplus::Point ptT[4];
    svgPoint svgptT[4];

    Gdiplus::REAL fdotsize = (Gdiplus::REAL) dotsize;

    Gdiplus::Color clrO;
    Gdiplus::REAL sizeO;
    Gdiplus::REAL flinesize = (Gdiplus::REAL) dotlinesize;
    if (flinesize > ((Gdiplus::REAL) (0.5) * fdotsize)) {
        flinesize = (Gdiplus::REAL) (0.5) * fdotsize;
    }
    else if (flinesize < ((Gdiplus::REAL) (0.05) * fdotsize)) {
        flinesize = (Gdiplus::REAL) (0.0);
    }

    switch (dotstyle) {
        case 0x02:    // .
            fillEllipseTX(pplot, fx - 2.0, fy - 2.0, fx + 2.0, fy + 2.0, clrT);
            break;
        case 0x04:    // +
            drawLineTX(pplot, fx - fdotsize, fy, fx + fdotsize, fy);
            drawLineTX(pplot, fx, fy - fdotsize, fx, fy + fdotsize);
            break;
        case 0x06:    // -
            drawLineTX(pplot, fx - fdotsize, fy, fx + fdotsize, fy);
            break;
        case 0x08:    // |
            drawLineTX(pplot, fx, fy - fdotsize, fx, fy + fdotsize);
            break;
        case 0x10:    // Circle
            drawEllipseTX(pplot, fx - fdotsize, fy - fdotsize, fx + fdotsize, fy + fdotsize);
            break;
        case 0x11:
            fillEllipseTX(pplot, fx - fdotsize, fy - fdotsize, fx + fdotsize, fy + fdotsize, clrT);
            if (clrDL && (flinesize > (Gdiplus::REAL) (0.0)) && (clrT->GetValue() != clrDL->GetValue())) {
                pplot->pen->GetColor(&clrO);
                sizeO = pplot->pen->GetWidth();
                pplot->pen->SetColor(*clrDL);
                pplot->pen->SetWidth(flinesize);
                drawEllipseTX(pplot, fx - fdotsize, fy - fdotsize, fx + fdotsize, fy + fdotsize, clrDL);
                pplot->pen->SetColor(clrO);
                pplot->pen->SetWidth(sizeO);
            }
            break;
        case 0x20:    // Square
            drawRectangleTX(pplot, fx - fdotsize, fy - fdotsize, fx + fdotsize, fy + fdotsize);
            break;
        case 0x21:
            fillRectangleTX(pplot, fx - fdotsize, fy - fdotsize, fx + fdotsize, fy + fdotsize, clrT);
            if (clrDL && (flinesize > (Gdiplus::REAL) (0.0)) && (clrT->GetValue() != clrDL->GetValue())) {
                pplot->pen->GetColor(&clrO);
                sizeO = pplot->pen->GetWidth();
                pplot->pen->SetColor(*clrDL);
                pplot->pen->SetWidth(flinesize);
                drawRectangleTX(pplot, fx - fdotsize, fy - fdotsize, fx + fdotsize, fy + fdotsize, clrDL);
                pplot->pen->SetColor(clrO);
                pplot->pen->SetWidth(sizeO);
            }
            break;
        case 0x30:    // Trapeze
        case 0x31:
            svgptT[0].x = fx - fdotsize;
            svgptT[0].y = fy;
            svgptT[1].x = fx;
            svgptT[1].y = fy + fdotsize;
            svgptT[2].x = fx + fdotsize;
            svgptT[2].y = fy;
            svgptT[3].x = fx;
            svgptT[3].y = fy - fdotsize;
            ptT[0].X = (int_t)(fx - fdotsize);
            ptT[0].Y = (int_t) (fy);
            ptT[1].X = (int_t) (fx);
            ptT[1].Y = (int_t) (fy + fdotsize);
            ptT[2].X = (int_t) (fx + fdotsize);
            ptT[2].Y = (int_t) (fy);
            ptT[3].X = (int_t) (fx);
            ptT[3].Y = (int_t) (fy - fdotsize);
            if (dotstyle == 0x31) {
                fillPolygonTX(pplot, ptT, svgptT, 4, clrT);
                if (clrDL && (flinesize > (Gdiplus::REAL) (0.0)) && (clrT->GetValue() != clrDL->GetValue())) {
                    pplot->pen->GetColor(&clrO);
                    sizeO = pplot->pen->GetWidth();
                    pplot->pen->SetColor(*clrDL);
                    pplot->pen->SetWidth(flinesize);
                    drawPolygonTX(pplot, ptT, svgptT, 4, clrDL);
                    pplot->pen->SetColor(clrO);
                    pplot->pen->SetWidth(sizeO);
                }
            }
            else {
                drawPolygonTX(pplot, ptT, svgptT, 4);
            }
            break;
        case 0x40:    // Triangle up
        case 0x41:
            svgptT[0].x = fx - fdotsize;
            svgptT[0].y = fy + fdotsize;
            svgptT[1].x = fx + fdotsize;
            svgptT[1].y = fy + fdotsize;
            svgptT[2].x = fx;
            svgptT[2].y = fy - fdotsize;
            ptT[0].X = (int_t) (fx - fdotsize);
            ptT[0].Y = (int_t) (fy + fdotsize);
            ptT[1].X = (int_t) (fx + fdotsize);
            ptT[1].Y = (int_t) (fy + fdotsize);
            ptT[2].X = (int_t) (fx);
            ptT[2].Y = (int_t) (fy - fdotsize);
            if (dotstyle == 0x41) {
                fillPolygonTX(pplot, ptT, svgptT, 3, clrT);
                if (clrDL && (flinesize > (Gdiplus::REAL) (0.0)) && (clrT->GetValue() != clrDL->GetValue())) {
                    pplot->pen->GetColor(&clrO);
                    sizeO = pplot->pen->GetWidth();
                    pplot->pen->SetColor(*clrDL);
                    pplot->pen->SetWidth(flinesize);
                    drawPolygonTX(pplot, ptT, svgptT, 3, clrDL);
                    pplot->pen->SetColor(clrO);
                    pplot->pen->SetWidth(sizeO);
                }
            }
            else {
                drawPolygonTX(pplot, ptT, svgptT, 3);
            }
            break;
        case 0x50:    // Triangle down
        case 0x51:
            svgptT[0].x = fx - fdotsize;
            svgptT[0].y = fy - fdotsize;
            svgptT[1].x = fx + fdotsize;
            svgptT[1].y = fy - fdotsize;
            svgptT[2].x = fx;
            svgptT[2].y = fy + fdotsize;
            ptT[0].X = (int_t) (fx - fdotsize);
            ptT[0].Y = (int_t) (fy - fdotsize);
            ptT[1].X = (int_t) (fx + fdotsize);
            ptT[1].Y = (int_t) (fy - fdotsize);
            ptT[2].X = (int_t) (fx);
            ptT[2].Y = (int_t) (fy + fdotsize);
            if (dotstyle == 0x51) {
                fillPolygonTX(pplot, ptT, svgptT, 3, clrT);
                if (clrDL && (flinesize > (Gdiplus::REAL) (0.0)) && (clrT->GetValue() != clrDL->GetValue())) {
                    pplot->pen->GetColor(&clrO);
                    sizeO = pplot->pen->GetWidth();
                    pplot->pen->SetColor(*clrDL);
                    pplot->pen->SetWidth(flinesize);
                    drawPolygonTX(pplot, ptT, svgptT, 3, clrDL);
                    pplot->pen->SetColor(clrO);
                    pplot->pen->SetWidth(sizeO);
                }
            }
            else {
                drawPolygonTX(pplot, ptT, svgptT, 3);
            }
            break;
        case 0x60:    // Triangle left
        case 0x61:
            svgptT[0].x = fx - fdotsize;
            svgptT[0].y = fy;
            svgptT[1].x = fx + fdotsize;
            svgptT[1].y = fy + fdotsize;
            svgptT[2].x = fx + fdotsize;
            svgptT[2].y = fy - fdotsize;
            ptT[0].X = (int_t) (fx - fdotsize);
            ptT[0].Y = (int_t) (fy);
            ptT[1].X = (int_t) (fx + fdotsize);
            ptT[1].Y = (int_t) (fy + fdotsize);
            ptT[2].X = (int_t) (fx + fdotsize);
            ptT[2].Y = (int_t) (fy - fdotsize);
            if (dotstyle == 0x61) {
                fillPolygonTX(pplot, ptT, svgptT, 3, clrT);
                if (clrDL && (flinesize > (Gdiplus::REAL) (0.0)) && (clrT->GetValue() != clrDL->GetValue())) {
                    pplot->pen->GetColor(&clrO);
                    sizeO = pplot->pen->GetWidth();
                    pplot->pen->SetColor(*clrDL);
                    pplot->pen->SetWidth(flinesize);
                    drawPolygonTX(pplot, ptT, svgptT, 3, clrDL);
                    pplot->pen->SetColor(clrO);
                    pplot->pen->SetWidth(sizeO);
                }
            }
            else {
                drawPolygonTX(pplot, ptT, svgptT, 3);
            }
            break;
        case 0x70:    // Triangle right
        case 0x71:
            svgptT[0].x = fx - fdotsize;
            svgptT[0].y = fy - fdotsize;
            svgptT[1].x = fx - fdotsize;
            svgptT[1].y = fy + fdotsize;
            svgptT[2].x = fx + fdotsize;
            svgptT[2].y = fy;
            ptT[0].X = (int_t) (fx - fdotsize);
            ptT[0].Y = (int_t) (fy - fdotsize);
            ptT[1].X = (int_t) (fx - fdotsize);
            ptT[1].Y = (int_t) (fy + fdotsize);
            ptT[2].X = (int_t) (fx + fdotsize);
            ptT[2].Y = (int_t) (fy);
            if (dotstyle == 0x71) {
                fillPolygonTX(pplot, ptT, svgptT, 3, clrT);
                if (clrDL && (flinesize > (Gdiplus::REAL) (0.0)) && (clrT->GetValue() != clrDL->GetValue())) {
                    pplot->pen->GetColor(&clrO);
                    sizeO = pplot->pen->GetWidth();
                    pplot->pen->SetColor(*clrDL);
                    pplot->pen->SetWidth(flinesize);
                    drawPolygonTX(pplot, ptT, svgptT, 3, clrDL);
                    pplot->pen->SetColor(clrO);
                    pplot->pen->SetWidth(sizeO);
                }
            }
            else {
                drawPolygonTX(pplot, ptT, svgptT, 3);
            }
            break;
        default:
            break;
    }
}
__inline static void drawStringTX(plot_t *pplot, const char_t* pszT, int nlen, Gdiplus::Font *pFont,
    Gdiplus::SolidBrush *pBrush, Gdiplus::RectF *rcF, byte_t bStyle, irect_t *ircText, Gdiplus::Font *pFontSm = NULL,
    SVG_ALIGN talign = SVG_ALIGN_MIDDLE)
{
    Gdiplus::PointF ptF, ptSm;
    Gdiplus::REAL centerX = rcF->X + (rcF->Width / 2.0f), centerY = rcF->Y + (rcF->Height / 2.0f),
        textDim = 0.0f, textDimH = 0.0f;

    Gdiplus::RectF rcT(*rcF);

    Gdiplus::GraphicsState stateT = pplot->canvas->Save();
    Gdiplus::GraphicsState stateI;
    if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
        stateI = pplot->image->Save();
    }

    double fCenterx = (real_t) (centerX),
        fCentery = (real_t) (centerY);

    bool bRestore = false;

    if (((bStyle & 0xF0) == 0x10) || (pFont->GetSize() > 12.0f)) {
        pplot->canvas->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
            pplot->image->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        }
        bRestore = true;
    }

    bool bIndex = (((_tcschr(pszT, _T('^')) != NULL) || (_tcschr(pszT, _T('_')) != NULL)) && (nlen > 2));

    bool bVert090 = false, bVert270 = false, bCenter = false;

    // SVG
    std::wstring strContent = uT(""), strOrient = uT(""), strFont = uT("");
    char_t szOrient[ML_STRSIZEN];
    SVG_FONTSTYLE tstyle = SVG_FONTSTYLE_REGULAR;
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        INT fontStyle = pFont->GetStyle();
        if (fontStyle == Gdiplus::FontStyleBold) {
            tstyle = SVG_FONTSTYLE_BOLD;
        }
        else if (fontStyle == Gdiplus::FontStyleBoldItalic) {
            tstyle = SVG_FONTSTYLE_BOLDITALIC;
        }
    }

    if ((bStyle & 0xF0) == 0x10) { // Vertical 90
        bVert090 = true;
        bRestore = true;

        // SVG
        if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
            wsprintf(szOrient, uT("rotate(90,%d,%d)"), (int_t) (centerX), (int_t) (centerY));
            strOrient = (const char_t*)(szOrient);
        }

        if ((bStyle & 0x0F) == 0x01) { // Center align
            bCenter = true;
            // Offset the coordinate system so that point (0, 0) is at the center
            pplot->canvas->TranslateTransform(centerX, centerY);
            pplot->canvas->RotateTransform(90.0f);
            pplot->canvas->MeasureString(pszT, nlen, pFont, ptF, &rcT);
            // EMF
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->TranslateTransform(centerX, centerY);
                pplot->image->RotateTransform(90.0f);
            }
            ptF.X = +(rcT.Width / 2.0f);
            ptF.Y = +(rcT.Height / 2.0f);

            if (ircText) {
                ircText->top = (int_t) (centerY - (rcT.Width / 2.0f));
                ircText->bottom = (int_t) (centerY + (rcT.Width / 2.0f));
                ircText->left = (int_t) (centerX - (rcT.Height / 2.0f));
                ircText->right = (int_t) (centerX + (rcT.Height / 2.0f));
            }
        }
        else { // Left align
            bCenter = false;
            // Offset the coordinate system so that point (0, 0) is at the top
            pplot->canvas->TranslateTransform(rcF->X + rcF->Width, rcF->Y);
            pplot->canvas->RotateTransform(90.0f);
            // EMF
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->TranslateTransform(rcF->X + rcF->Width, rcF->Y);
                pplot->image->RotateTransform(90.0f);
            }
            ptF.X = 0.0f;
            ptF.Y = 0.0f;
            pplot->canvas->MeasureString(pszT, nlen, pFont, ptF, &rcT);
            rcF->Height = rcT.Height;
            rcF->Width = rcT.Width;

            if (ircText) {
                ircText->top = (int_t) (rcF->Y);
                ircText->bottom = ircText->top + (int_t) (rcT.Width);
                ircText->left = (int_t) (rcF->X);
                ircText->right = ircText->left + (int_t) (rcT.Height);
            }
        }
    }
    else if ((bStyle & 0xF0) == 0x20) { // Vertical 270
        bVert270 = true;
        bRestore = true;

        // SVG
        if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
            wsprintf(szOrient, uT("rotate(270,%d,%d)"), (int_t) (centerX), (int_t) (centerY));
            strOrient = (const char_t*)(szOrient);
        }

        if ((bStyle & 0x0F) == 0x01) { // Center align
            bCenter = true;
            // Offset the coordinate system so that point (0, 0) is at the center
            pplot->canvas->TranslateTransform(centerX, centerY);
            pplot->canvas->RotateTransform(270.0f);
            pplot->canvas->MeasureString(pszT, nlen, pFont, ptF, &rcT);
            // EMF
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->TranslateTransform(centerX, centerY);
                pplot->image->RotateTransform(270.0f);
            }
            ptF.X = -(rcT.Width / 2.0f);
            ptF.Y = -(rcT.Height / 2.0f);

            if (ircText) {
                ircText->top = (int_t) (centerY - (rcT.Width / 2.0f));
                ircText->bottom = (int_t) (centerY + (rcT.Width / 2.0f));
                ircText->left = (int_t) (centerX - (rcT.Height / 2.0f));
                ircText->right = (int_t) (centerX + (rcT.Height / 2.0f));
            }
        }
        else { // Left align
            bCenter = false;
            // Offset the coordinate system so that point (0, 0) is at the bottom
            pplot->canvas->TranslateTransform(rcF->X, rcF->Y + rcF->Height);
            pplot->canvas->RotateTransform(270.0f);
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->TranslateTransform(rcF->X, rcF->Y + rcF->Height);
                pplot->image->RotateTransform(270.0f);
            }
            ptF.X = 0.0f;
            ptF.Y = 0.0f;
            pplot->canvas->MeasureString(pszT, nlen, pFont, ptF, &rcT);
            rcF->Y = (rcF->Y + rcF->Height) - rcT.Height;
            rcF->Height = rcT.Height;
            rcF->Width = rcT.Width;

            if (ircText) {
                ircText->bottom = (int_t) (rcF->Y + rcF->Height);
                ircText->top = ircText->bottom - (int_t) (rcT.Width);
                ircText->left = (int_t) (rcF->X);
                ircText->right = ircText->left + (int_t) (rcT.Height);
            }
        }
    }
    else {
        //Horizontal

        // SVG
        if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
            wsprintf(szOrient, uT("rotate(0,%d,%d)"), (int_t) (centerX), (int_t) (centerY));
            strOrient = (const char_t*)(szOrient);
        }

        if ((bStyle & 0x0F) == 0x01) { // Center align
            bCenter = true;
            pplot->canvas->MeasureString(pszT, nlen, pFont, ptF, &rcT);
            ptF.X = centerX - (rcT.Width / 2.0f);
            ptF.Y = centerY - (rcT.Height / 2.0f);

            if (ircText) {
                ircText->top = (int_t) (centerY - (rcT.Height / 2.0f));
                ircText->bottom = (int_t) (centerY + (rcT.Height / 2.0f));
                ircText->left = (int_t) (centerX - (rcT.Width / 2.0f));
                ircText->right = (int_t) (centerX + (rcT.Width / 2.0f));
            }
        }
        else { // Left align
            bCenter = false;
            ptF.X = rcF->X;
            ptF.Y = rcF->Y;
            pplot->canvas->MeasureString(pszT, nlen, pFont, ptF, &rcT);
            rcF->Height = rcT.Height;
            rcF->Width = rcT.Width;

            if (talign == SVG_ALIGN_MIDDLE) {
                fCenterx = (real_t) (ptF.X + (rcT.Width / 2.0f));
                fCentery = (real_t) (ptF.Y + (rcT.Height / 2.0f));
            }
            else {
                fCenterx = (real_t) (ptF.X);
                fCentery = (real_t) (ptF.Y + (rcT.Height / 2.0f));
            }

            if (ircText) {
                ircText->top = (int_t) (rcF->Y);
                ircText->bottom = ircText->top + (int_t) (rcT.Height);
                ircText->left = (int_t) (rcF->X);
                ircText->right = ircText->left + (int_t) (rcT.Width);
            }
        }
    }

    if ((bIndex == false) || (pFontSm == NULL)) {
        pplot->canvas->DrawString(pszT, nlen, pFont, ptF, pBrush);
        // EMF
        if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
            pplot->image->DrawString(pszT, nlen, pFont, ptF, pBrush);
        }
        // SVG
        else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
            strContent = (const char_t*) (pszT);
            Gdiplus::FontFamily ffT;
            pFont->GetFamily(&ffT);
            char_t szTW[ML_STRSIZEN];
            szTW[0] = _T('\0');
            ffT.GetFamilyName(szTW);
            strFont = (const char_t*) (szTW);
            Gdiplus::Color clrT;
            pBrush->GetColor(&clrT);

            svgText::clean(strContent, uT("<"), uT("&lt;"));
            svgText::clean(strContent, uT(">"), uT("&gt;"));
            svgText::clean(strContent, uT("&"), uT("&amp;"));
            svgText::clean(strContent, uT("\""), uT("&quot;"));
            svgText::clean(strContent, uT("'"), uT("&apos;"));

            *(pplot->svgdoc) << svgText(strContent, svgPoint(fCenterx, fCentery), strOrient,
                svgFont((int_t) (pFont->GetSize()), strFont, tstyle),
                svgStroke((real_t) (0), svgColor((int_t) (clrT.GetR()), (int_t) (clrT.GetG()), (int_t) (clrT.GetB()))),
                svgFill(svgColor((int_t) (clrT.GetR()), (int_t) (clrT.GetG()), (int_t) (clrT.GetB()))),
                talign);
        }
        if (bRestore) {
            pplot->canvas->Restore(stateT);
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->Restore(stateI);
            }
        }
        return;
    }

    // Prend en compte les exposants et les indices

    char_t szCA[2];
    // SVG
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        strContent = uT("");
        szCA[1] = uT('\0');
    }

    Gdiplus::StringFormat strFormat(Gdiplus::StringFormat::GenericDefault());
    strFormat.SetFormatFlags(strFormat.GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);
    if (bVert090 || bVert270) {
        strFormat.SetFormatFlags(strFormat.GetFormatFlags() | Gdiplus::StringFormatFlagsDirectionVertical);
    }
    Gdiplus::Region *pCharRangeRegions = NULL; // pointer to CharacterRange regions
    Gdiplus::CharacterRange *pCharRanges = NULL;

    char_t szT[2];
    szT[1] = _T('\0');

    int ii;
    int iSmall = 0;
    pplot->font = pFont;
    bool bFlag = false, bSuper = false, bSub = false;
    int iFlag = 0;
    Gdiplus::REAL fWidth = rcT.Width / (Gdiplus::REAL)nlen, fHeight = rcT.Height;
    Gdiplus::REAL fBox = (rcF->Width > rcF->Height) ? rcF->Width : rcF->Height;
    Gdiplus::REAL fYb = ptF.Y + rcT.Height;

    textDim = 0.0f;
    textDimH = 0.0f;
    char_t cT;

    for (ii = 0; ii < nlen; ii++) {

        szT[0] = pszT[ii];

        if (szT[0] == _T('\\')) {
            cT = (ii < (nlen - 1)) ? pszT[ii + 1] : _T('\0');
            if (((cT == _T('_')) || (cT == _T('^'))) && (iSmall == 0)) {
                continue;
            }
        }

        // Superscript
        if ((pszT[ii] == _T('^')) && (iSmall == 0)) {

            cT = (ii > 0) ? pszT[ii - 1] : _T('\0');

            if (cT != _T('\\')) {
                iSmall = ii;
                pplot->font = pFontSm;
                bSuper = true;
                bSub = false;

                // SVG
                if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                    strContent += uT("<tspan baseline-shift=\"super\">");
                }

                continue;
            }
        }
        
        // Subscript
        if ((pszT[ii] == _T('_')) && (iSmall == 0)) {

            cT = (ii > 0) ? pszT[ii - 1] : _T('\0');

            if (cT != _T('\\')) {
                iSmall = ii;
                pplot->font = pFontSm;
                bSuper = false;
                bSub = true;

                // SVG
                if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                    strContent += uT("<tspan baseline-shift=\"sub\">");
                }

                continue;
            }
        }

        if (pszT[ii] == _T('\r')) {
            continue;
        }

        if ((iSmall > 0) && (ii == (iSmall + 1)) && (pszT[ii] == PL_BLOCK_START)) {
            bFlag = true;
            iFlag = 1;
            continue;
        }

        if ((iSmall > 0) && bFlag) {
            if (pszT[ii] == PL_BLOCK_START) {
                iFlag += 1;
            }
            else if (pszT[ii] == PL_BLOCK_END) {
                iFlag -= 1;
            }
        }

        if ((iSmall > 0) && bFlag && (iFlag <= 0)) {
            iSmall = 0;
            bSuper = false;
            bSub = false;
            bFlag = false;
            iFlag = 0;
            pplot->font = pFont;

            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                strContent += uT("</tspan>");
            }

            continue;
        }

        if ((iSmall > 0) && (bFlag == false) && (pszT[ii] == _T(' '))) {
            iSmall = 0;
            bSuper = false;
            bSub = false;
            bFlag = false;
            pplot->font = pFont;

            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                strContent += uT("</tspan>");
            }
        }

        if ((iSmall > 0) && (bFlag == false) && (ii > (iSmall + 1)) && !_istdigit(pszT[ii])) {
            iSmall = 0;
            bSuper = false;
            bSub = false;
            pplot->font = pFont;

            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                strContent += uT("</tspan>");
            }
        }

        if (pCharRanges) {
            delete pCharRanges;
            pCharRanges = NULL;
        }
        if (pCharRangeRegions) {
            delete pCharRangeRegions;
            pCharRangeRegions = NULL;
        }

        try {
            pCharRangeRegions = new Gdiplus::Region(); // pointer to CharacterRange regions
        }
        catch (...) {
            pCharRangeRegions = NULL;
            goto drawStringTX_RET;
        }

        try {
            pCharRanges = new Gdiplus::CharacterRange();
        }
        catch (...) {
            pCharRanges = NULL;
            goto drawStringTX_RET;
        }

        pCharRanges->First = 0;
        pCharRanges->Length = 1;
        strFormat.SetMeasurableCharacterRanges(1, pCharRanges);
        rcT.X = ptF.X;
        rcT.Y = ptF.Y;
        rcT.Width = fBox;
        rcT.Height = fBox;
        pplot->canvas->MeasureCharacterRanges(szT, 1, pplot->font, rcT, &strFormat, 1, pCharRangeRegions);
        pCharRangeRegions->GetBounds(&rcT, pplot->canvas);
        if (rcT.Width > 0.0f) {
            fWidth = rcT.Width;
        }
        if (rcT.Height > 0.0f) {
            fHeight = rcT.Height;
        }

        if (pszT[ii] == _T('\n')) {
            iSmall = 0;
            bSuper = false;
            bSub = false;
            if (bVert090 || bVert270) {
                ptF.Y = rcF->Y;
                ptF.X += fWidth;
            }
            else {
                ptF.X = rcF->X;
                ptF.Y += fHeight;
            }
            continue;
        }

        if (bVert090) {
            ptSm = ptF;
            if (iSmall > 0) {
                if (bCenter) {
                    ptSm.X += 1.0f;
                }
                else {
                    ptSm.X += 1.0f;
                }
                if (bSuper) {
                    ptSm.Y -= (fWidth / 3.0f);
                    if ((fWidth + (fWidth / 3.0f)) > textDimH) {
                        textDimH = fWidth + (fWidth / 3.0f);
                    }
                }
                else {
                    ptSm.Y += ((2.0f * fWidth) / 3.0f);
                    if ((fWidth + ((2.0f * fWidth) / 3.0f)) > textDimH) {
                        textDimH = fWidth + ((2.0f * fWidth) / 3.0f);
                    }
                }
            }
            else if (fWidth > textDimH) {
                textDimH = fWidth;
            }

            pplot->canvas->DrawString(szT, 1, pplot->font, ptSm, pBrush);
            // EMF
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->DrawString(szT, 1, pplot->font, ptSm, pBrush);
            }
            // SVG
            else if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                if (szT[0] == _T('<')) {
                    strContent += uT("&lt;");
                }
                else if (szT[0] == _T('>')) {
                    strContent += uT("&gt;");
                }
                else if (szT[0] == _T('&')) {
                    strContent += uT("&amp;");
                }
                else if (szT[0] == _T('\"')) {
                    strContent += uT("&quot;");
                }
                else if (szT[0] == _T('\'')) {
                    strContent += uT("&apos;");
                }
                else {
                    strContent += (const char_t*) (szT);
                }
            }

            textDim += fHeight;

            if (bCenter) {
                ptF.X += fHeight;
            }
            else {
                ptF.X += fHeight;
            }
            if (iSmall > 0) {
                if (bCenter) {
                    ptF.X += 1.0f;
                    textDim += 1.0f;
                }
                else {
                    ptF.X += 1.0f;
                    textDim += 1.0f;
                }
            }
        }
        else if (bVert270) {
            ptSm = ptF;
            if (iSmall > 0) {
                if (bCenter) {
                    ptSm.X += 1.0f;
                }
                else {
                    ptSm.X += 1.0f;
                }
                if (bSuper) {
                    ptSm.Y -= (fWidth / 3.0f);
                    if ((fWidth + (fWidth / 3.0f)) > textDimH) {
                        textDimH = fWidth + (fWidth / 3.0f);
                    }
                }
                else {
                    ptSm.Y += ((2.0f * fWidth) / 3.0f);
                    if ((fWidth + ((2.0f * fWidth) / 3.0f)) > textDimH) {
                        textDimH = fWidth + ((2.0f * fWidth) / 3.0f);
                    }
                }
            }
            else if (fWidth > textDimH) {
                textDimH = fWidth;
            }

            pplot->canvas->DrawString(szT, 1, pplot->font, ptSm, pBrush);
            // EMF
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->DrawString(szT, 1, pplot->font, ptSm, pBrush);
            }
            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                if (szT[0] == _T('<')) {
                    strContent += uT("&lt;");
                }
                else if (szT[0] == _T('>')) {
                    strContent += uT("&gt;");
                }
                else if (szT[0] == _T('&')) {
                    strContent += uT("&amp;");
                }
                else if (szT[0] == _T('\"')) {
                    strContent += uT("&quot;");
                }
                else if (szT[0] == _T('\'')) {
                    strContent += uT("&apos;");
                }
                else {
                    strContent += (const char_t*) (szT);
                }
            }

            textDim += fHeight;

            if (bCenter) {
                ptF.X += fHeight;
            }
            else {
                ptF.X += fHeight;
            }
            if (iSmall > 0) {
                if (bCenter) {
                    ptF.X += 1.0f;
                    textDim += 1.0f;
                }
                else {
                    ptF.X += 1.0f;
                    textDim += 1.0f;
                }
            }
        }
        else {
            ptSm = ptF;
            if (iSmall > 0) {
                ptSm.X += 1.0f;
                if (bSuper) {
                    ptSm.Y -= (fHeight / 4.0f);
                    if ((fHeight + (fHeight / 3.0f)) > textDimH) {
                        textDimH = fHeight + (fHeight / 3.0f);
                    }
                }
                else {
                    ptSm.Y += ((2.0f * fHeight) / 3.0f);
                    if ((fHeight + ((2.0f * fHeight) / 3.0f)) > textDimH) {
                        textDimH = fHeight + ((2.0f * fHeight) / 3.0f);
                    }
                }
            }
            else if (fHeight > textDimH) {
                textDimH = fHeight;
            }

            pplot->canvas->DrawString(szT, 1, pplot->font, ptSm, pBrush);
            // EMF
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->DrawString(szT, 1, pplot->font, ptSm, pBrush);
            }
            // SVG
            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                if (szT[0] == _T('<')) {
                    strContent += uT("&lt;");
                }
                else if (szT[0] == _T('>')) {
                    strContent += uT("&gt;");
                }
                else if (szT[0] == _T('&')) {
                    strContent += uT("&amp;");
                }
                else if (szT[0] == _T('\"')) {
                    strContent += uT("&quot;");
                }
                else if (szT[0] == _T('\'')) {
                    strContent += uT("&apos;");
                }
                else {
                    strContent += (const char_t*) (szT);
                }
            }
            textDim += fWidth;

            ptF.X += fWidth;
            if (iSmall > 0) {
                ptF.X += 1.0f;
                textDim += 1.0f;
            }
        }

    }

    // SVG
    if ((iSmall > 0) && (pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        strContent += uT("</tspan>");
    }

    if (ircText) {
        if (bVert090) {
            ircText->bottom = ircText->top + (LONG) (textDim);
            ircText->right = ircText->left + (LONG) (textDimH);
        }
        else if (bVert270) {
            ircText->top = ircText->bottom - (LONG) (textDim);
            ircText->right = ircText->left + (LONG) (textDimH);
        }
        else {
            ircText->right = (LONG) (ptF.X);
            ircText->bottom = ircText->top + (LONG) (textDimH);
        }
    }

    // SVG
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        Gdiplus::FontFamily ffT;
        pFont->GetFamily(&ffT);
        char_t szTW[ML_STRSIZE];
        szTW[0] = _T('\0');
        ffT.GetFamilyName(szTW);
        strFont = (const char_t*) (szTW);
        Gdiplus::Color clrT;
        pplot->pen->GetColor(&clrT);
        *(pplot->svgdoc) << svgText(strContent, svgPoint(fCenterx, fCentery), strOrient,
            svgFont((int_t) (pFont->GetSize()), strFont, tstyle),
            svgStroke((real_t) (0), svgColor((int_t) (clrT.GetR()), (int_t) (clrT.GetG()), (int_t) (clrT.GetB()))),
            svgFill(svgColor((int_t) (clrT.GetR()), (int_t) (clrT.GetG()), (int_t) (clrT.GetB()))), talign);
    }

    if (bRestore) {
        pplot->canvas->Restore(stateT);
        if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
            pplot->image->Restore(stateI);
        }
    }

drawStringTX_RET:
    if (pCharRanges) {
        delete pCharRanges;
        pCharRanges = NULL;
    }
    if (pCharRangeRegions) {
        delete pCharRangeRegions;
        pCharRangeRegions = NULL;
    }
}

/*  --------------------------------------------------------------------
    pl_reformat         :    post-formatage des nombres r�els (%f , %e)
    
        szBuffer        :    Chaîne obtenue avec _tcsprintf avec %f ou %e
        
        Retourne la longueur de la chaine pl_reformat�e
    -------------------------------------------------------------------- */
__inline static int_t pl_reformat(char_t *szBuffer)
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

static double beautifyNumber(double fNumber, bool bRound = false)
{
    double fExponent;
    double fFraction;
    double fFractionX;

    fExponent = floor(log10(fNumber));
    fFraction = fNumber / pow(10.0, fExponent);

    if (bRound) {
        if (fFraction < 1.5) {
            fFractionX = 1.0;
        }
        else if (fFraction < 3.0) {
            fFractionX = 2.0;
        }
        else if (fFraction < 7.0) {
            fFractionX = 5.0;
        }
        else {
            fFractionX = 10.0;
        }
    }
    else {
        if (fFraction <= 1.0) {
            fFractionX = 1.0;
        }
        else if (fFraction <= 2.0) {
            fFractionX = 2.0;
        }
        else if (fFraction <= 5.0) {
            fFractionX = 5.0;
        }
        else {
            fFractionX = 10.0;
        }
    }

    return (fFractionX * pow(10.0, fExponent));
}

// 20160122: auto label tick improvement
void pl_autotick(double *pfMin, double *pfMax, double *pfStep, int *piGrid, byte_t iscale)
{
    if (*pfMax <= *pfMin) {
        return;
    }

    if (iscale == 0) {                    // Linear Scale
        lmAxisLabel labelT;
        *pfStep = (*pfMax - *pfMin) / 5.0;
        *piGrid = 5;
        labelT.search(pfMin, pfMax, pfStep, piGrid);
        return;
    }
    else if (iscale == 1) {                // Log Scale
        ml_round(pfMin, true);
        ml_round(pfMax, true);
        double fT = static_cast<double>(pow(10.0, pl_logzero));
        if (fabs((*pfMin)) < fT) {
            (*pfMin) = fT;
        }
        if ((*pfMax) < fT) {
            (*pfMax) = fT;
        }
        fT = static_cast<double>(pow(10.0, pl_loginf));
        if (fabs((*pfMin)) > fT) {
            (*pfMin) = fT;
        }
        if ((*pfMax) > fT) {
            (*pfMax) = fT;
        }
    }
}

/*  --------------------------------------------------------------------
    pl_autoscale     :    AutoScale

        pplot        :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
void pl_autoscale(plot_t *pplot)
{
    int_t ii, jj, nvis = 0, iflagX = 0, iflagX2 = 0, iflagY = 0, iflagY2 = 0,
        iX = 0, iX2 = 0, iY = 0, iY2 = 0;
    real_t fxmin = 0.0, fxmax = 0.0, fymin = 0.0, fymax = 0.0, fT = 0.0, fTT = 0.0, xx, yy;
    real_t fx2min = 0.0, fx2max = 0.0, fy2min = 0.0, fy2max = 0.0;
    real_t fxsum = 0.0, fysum = 0.0, fxmoy = 0.0, fymoy = 0.0;
    real_t fx2sum = 0.0, fy2sum = 0.0, fx2moy = 0.0, fy2moy = 0.0;
    byte_t iautoBottom = 0, iautoTop = 0;

    if (pplot->curvecount < 1) {
        return;
    }

    iflagX = 0;    iflagX2 = 0;
    iflagY = 0;    iflagY2 = 0;
    iX = 0;    iX2 = 0;
    iY = 0;    iY2 = 0;

    for (jj = 0; jj < pplot->curvecount; jj++) {
        if ((pplot->curve[jj].x == NULL) || (pplot->curve[jj].y == NULL)) {
            continue;
        }
        if ((pplot->curve[jj].x->dat == NULL) || (pplot->curve[jj].y->dat == NULL)
            || (pplot->curve[jj].points < 2) || ((pplot->curve[jj].status & 0x0F) == 0x0F)) {
            continue;
        }

        if ((pplot->curve[jj].axisused & 0xF0) == 0x00) {                // X-Axis
            iX += 1;
        }
        else if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {        // X2-Axis
            iX2 += 1;
        }
        if ((pplot->curve[jj].axisused & 0x0F) == 0x00) {                // Y-Axis
            iY += 1;
        }
        else if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {        // Y2-Axis
            iY2 += 1;
        }

        fxsum = 0.0;    fysum = 0.0;
        fx2sum = 0.0;    fy2sum = 0.0;

        for (ii = 0; ii < pplot->curve[jj].points; ii++) {
            // Les points masqu�s ne sont pas pris en compte...
            if (((pplot->curve[jj].x->mask[ii] & 0x0F) == 0x01) || ((pplot->curve[jj].y->mask[ii] & 0x0F) == 0x01)) {
                continue;
            }
            // ...et des �l�ments non valides.
            if (((pplot->curve[jj].x->mask[ii] & 0x0F) == 0x0F) || ((pplot->curve[jj].y->mask[ii] & 0x0F) == 0x0F)) {
                continue;
            }
            //

            if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {                // X2-Axis
                xx = ((pplot->axistop.op == 0x01) ? fabs(pplot->curve[jj].x->dat[ii]) : pplot->curve[jj].x->dat[ii]);
            }
            else {                                                                        // X-Axis
                xx = ((pplot->axisbottom.op == 0x01) ? fabs(pplot->curve[jj].x->dat[ii]) : pplot->curve[jj].x->dat[ii]);
            }

            if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {                // Y2-Axis
                yy = ((pplot->axisright.op == 0x01) ? fabs(pplot->curve[jj].y->dat[ii]) : pplot->curve[jj].y->dat[ii]);
            }
            else {                                                                        // Y-Axis
                yy = ((pplot->axisleft.op == 0x01) ? fabs(pplot->curve[jj].y->dat[ii]) : pplot->curve[jj].y->dat[ii]);
            }

            if ((iflagX == 0) && ((pplot->curve[jj].axisused & 0xF0) == 0x00)) {                // X-Axis
                if (pplot->curve[jj].x) {
                    if (pplot->curve[jj].x->id == (ML_MAXCOLUMNS + 99)) {    // AutoX
                        iautoBottom += 1;
                    }
                }
                if (pplot->axisbottom.scale == 1) {        // Log Scale
                    if (xx > 0.0) {
                        fxmin = xx;
                        fxmax = xx;
                        iflagX = 1;
                    }
                }
                else {
                    fxmin = xx;
                    fxmax = xx;
                    iflagX = 1;
                }
                if (iflagX == 0) {
                    continue;
                }
            }
            if ((iflagX2 == 0) && ((pplot->curve[jj].axisused & 0xF0) == 0x10)) {        // X2-Axis
                if (pplot->curve[jj].x) {
                    if (pplot->curve[jj].x->id == (ML_MAXCOLUMNS + 99)) {    // AutoX
                        iautoTop += 1;
                    }
                }
                if (pplot->axistop.scale == 1) {        // Log Scale
                    if (xx > 0.0) {
                        fx2min = xx;
                        fx2max = xx;
                        iflagX2 = 1;
                    }
                }
                else {
                    fx2min = xx;
                    fx2max = xx;
                    iflagX2 = 1;
                }
                if (iflagX2 == 0) {
                    continue;
                }
            }
            if ((iflagY == 0) && ((pplot->curve[jj].axisused & 0x0F) == 0x00)) {                // Y-Axis
                if (pplot->axisleft.scale == 1) {        // Log Scale
                    if (yy > 0.0) {
                        fymin = yy;
                        fymax = yy;
                        iflagY = 1;
                    }
                }
                else {
                    fymin = yy;
                    fymax = yy;
                    iflagY = 1;
                }
                if (iflagY == 0) {
                    continue;
                }
            }
            if ((iflagY2 == 0) && ((pplot->curve[jj].axisused & 0x0F) == 0x01)) {        // Y2-Axis
                if (pplot->axisright.scale == 1) {        // Log Scale
                    if (yy > 0.0) {
                        fy2min = yy;
                        fy2max = yy;
                        iflagY2 = 1;
                    }
                }
                else {
                    fy2min = yy;
                    fy2max = yy;
                    iflagY2 = 1;
                }
                if (iflagY2 == 0) {
                    continue;
                }
            }
            //

            if ((pplot->curve[jj].axisused & 0xF0) == 0x00) {    // X-Axis
                if (xx < fxmin) {
                    if (pplot->axisbottom.scale == 1) {          // Log Scale
                        if ((ii > 0) && (xx > 0.0)) {
                            if ((xx / fxmin) > 0.001) {
                                fxmin = xx;
                                fxsum += xx;
                            }
                        }
                    }
                    else {
                        fxmin = xx;
                        fxsum += xx;
                    }
                }
                else if (xx > fxmax) {
                    fxmax = xx;
                    fxsum += xx;
                }
            }
            else if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {        // X2-Axis
                if (xx < fx2min) {
                    if (pplot->axistop.scale == 1) {                      // Log Scale
                        if ((ii > 0) && (xx > 0.0)) {
                            if ((xx / fx2min) > 0.001) {
                                fx2min = xx;
                                fx2sum += xx;
                            }
                        }
                    }
                    else {
                        fx2min = xx;
                        fx2sum += xx;
                    }
                }
                else if (xx > fx2max) {
                    fx2max = xx;
                    fx2sum += xx;
                }
            }
            if ((pplot->curve[jj].axisused & 0x0F) == 0x00) {                // Y-Axis
                if (yy < fymin) {
                    if (pplot->axisleft.scale == 1) {                        // Log Scale
                        if ((ii > 0) && (yy > 0.0)) {
                            if ((yy / fymin) > 0.001) {
                                fymin = yy;
                                fysum += yy;
                            }
                        }
                    }
                    else {
                        fymin = yy;
                        fysum += yy;
                    }
                }
                else if (yy > fymax) {
                    fymax = yy;
                    fysum += yy;
                }
            }
            else if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {        // Y2-Axis
                if (yy < fy2min) {
                    if (pplot->axisright.scale == 1) {                     // Log Scale
                        if ((ii > 0) && (yy > 0.0)) {
                            if ((yy / fy2min) > 0.001) {
                                fy2min = yy;
                                fy2sum += yy;
                            }
                        }
                    }
                    else {
                        fy2min = yy;
                        fy2sum += yy;
                    }
                }
                else if (yy > fy2max) {
                    fy2max = yy;
                    fy2sum += yy;
                }
            }
        }

        if ((pplot->curve[jj].axisused & 0xF0) == 0x00) {                    // X-Axis
            fxmoy += (fxsum / static_cast<double>(pplot->curve[jj].points));
        }
        else if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {                // X2-Axis
            fx2moy += (fx2sum / static_cast<double>(pplot->curve[jj].points));
        }
        if ((pplot->curve[jj].axisused & 0x0F) == 0x00) {                    // Y-Axis
            fymoy += (fysum / static_cast<double>(pplot->curve[jj].points));
        }
        else if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {                // Y2-Axis
            fy2moy += (fy2sum / static_cast<double>(pplot->curve[jj].points));
        }
    }

    if (iX > 0) {                        // X-Axis
        if (fxmax <= fxmin) {
            if (pplot->axisbottom.scale == 1) {
                fxmax += 0.5;
            }
            else {
                fxmin -= 0.5;
                fxmax += 0.5;
            }
        }

        double fstep = 0.1;
        int igrid = 5;
        pl_autotick(&fxmin, &fxmax, &fstep, &igrid, pplot->axisbottom.scale);
        pplot->axisbottom.min = fxmin;
        pplot->axisbottom.max = fxmax;
        pplot->axisbottom.increment = fstep;
        pplot->axisbottom.gridcount = igrid;
        pplot->axisbottom.gridmcount = 1;

        // >> 20100330: Adapter le format num�rique de l'axe
        if (pplot->axisbottom.scale == 0) {    // Linear Scale
            if ((fabs(pplot->axisbottom.min) > 1e-3) && (fabs(pplot->axisbottom.min) < 1e3)
                && (fabs(pplot->axisbottom.max) > 1e-3) && (fabs(pplot->axisbottom.max) < 1e3)) {
                _tcscpy(pplot->axisbottom.format, (const char_t*)ml_format_f);
            }
        }
        // <<
    }

    if (iX2 > 0) {                        // X2-Axis
        if (fx2max <= fx2min) {
            if (pplot->axistop.scale == 1) {
                fx2max += 0.5;
            }
            else {
                fx2min -= 0.5;
                fx2max += 0.5;
            }
        }

        double fstep = 0.1;
        int igrid = 5;
        pl_autotick(&fx2min, &fx2max, &fstep, &igrid, pplot->axistop.scale);
        pplot->axistop.min = fx2min;
        pplot->axistop.max = fx2max;
        pplot->axistop.increment = fstep;
        pplot->axistop.gridcount = igrid;
        pplot->axistop.gridmcount = 1;

        // >> 20100330: Adapter le format num�rique de l'axe
        if (pplot->axistop.scale == 0) {    // Linear Scale
            if ((fabs(pplot->axistop.min) > 1e-3) && (fabs(pplot->axistop.min) < 1e3)
                && (fabs(pplot->axistop.max) > 1e-3) && (fabs(pplot->axistop.max) < 1e3)) {
                _tcscpy(pplot->axistop.format, (const char_t*)ml_format_f);
            }
        }
        // <<
    }

    if (iY > 0) {                        // Y-Axis
        if (fymax <= fymin) {
            if (pplot->axisleft.scale == 1) {
                fymax += 0.5;
            }
            else {
                fymin -= 0.5;
                fymax += 0.5;
            }
        }

        double fstep = 0.1;
        int igrid = 5;
        pl_autotick(&fymin, &fymax, &fstep, &igrid, pplot->axisleft.scale);
        pplot->axisleft.min = fymin;
        pplot->axisleft.max = fymax;
        pplot->axisleft.increment = fstep;
        pplot->axisleft.gridcount = igrid;
        pplot->axisleft.gridmcount = 1;

        // >> 20100330: Adapter le format num�rique de l'axe
        if (pplot->axisleft.scale == 0) {    // Linear Scale
            if ((fabs(pplot->axisleft.min) > 1e-3) && (fabs(pplot->axisleft.min) < 1e3)
                && (fabs(pplot->axisleft.max) > 1e-3) && (fabs(pplot->axisleft.max) < 1e3)) {
                _tcscpy(pplot->axisleft.format, (const char_t*)ml_format_f);
            }
        }
        // <<
    }

    if (iY2 > 0) {                        // Y2-Axis
        if (fy2max <= fy2min) {
            if (pplot->axisright.scale == 1) {
                fy2max += 0.5;
            }
            else {
                fy2min -= 0.5;
                fy2max += 0.5;
            }
        }

        double fstep = 0.1;
        int igrid = 5;
        pl_autotick(&fy2min, &fy2max, &fstep, &igrid, pplot->axisright.scale);
        pplot->axisright.min = fy2min;
        pplot->axisright.max = fy2max;
        pplot->axisright.increment = fstep;
        pplot->axisright.gridcount = igrid;
        pplot->axisright.gridmcount = 1;

        // >> 20100330: Adapter le format num�rique de l'axe
        if (pplot->axisright.scale == 0) {    // Linear Scale
            if ((fabs(pplot->axisright.min) > 1e-3) && (fabs(pplot->axisright.min) < 1e3)
                && (fabs(pplot->axisright.max) > 1e-3) && (fabs(pplot->axisright.max) < 1e3)) {
                _tcscpy(pplot->axisright.format, (const char_t*)ml_format_f);
            }
        }
        // <<
    }
}

/*  --------------------------------------------------------------------
    pl_logscale        :    Calcul de l'�chelle Log � partir du range lin�aire
    -------------------------------------------------------------------- */
__inline static void pl_logscale(real_t fmin, real_t fmax, real_t *fminl, real_t *fmaxl, byte_t *gridcount)
{
    real_t tmpval;

    if (fmin > 0.0) {
        *fminl = floor(log10(fmin));
    }
    else {
        if (fmax > 0.0) {
            *fminl = floor(log10(fmax)) - 1.0;
        }
        else {
            *fminl = pl_logzero - 1.0;
        }
    }

    if (fmax > 0.0) {
        *fmaxl = log10(fmax);
        tmpval = floor(*fmaxl);
        if (tmpval != *fmaxl) { *fmaxl = tmpval + 1.0; }
        else { *fmaxl = tmpval; }
    }
    else {
        *fmaxl = *fminl + 1.0;
    }

    if (*fmaxl < *fminl) {
        tmpval = *fmaxl;
        *fmaxl = *fminl;
        *fminl = tmpval;
    }
    else if (*fmaxl == *fminl) {
        *fmaxl += 1.0;
    }

    *gridcount = (byte_t)(*fmaxl - *fminl);

    return;
}

/*  --------------------------------------------------------------------
    pl_reset_axis        :    Reset des axes
    -------------------------------------------------------------------- */
__inline static void pl_reset_axis(plot_t* pplot, byte_t iAxis)
{
    axis_t *paxis;
    byte_t colorR = 0, colorG = 0, colorB = 0;

    if (pplot == NULL) {
        return;
    }

    switch (iAxis) {
        case 0:
            paxis = &(pplot->axisbottom);
            paxis->trelpos.xa = 0.40;
            paxis->trelpos.xb = 0.60;
            paxis->trelpos.ya = 0.94;
            paxis->trelpos.yb = 0.98;
            colorR = 0;
            colorG = 0;
            colorB = 0;
            break;
        case 1:
            paxis = &(pplot->axisleft);
            paxis->trelpos.xa = 0.01;
            paxis->trelpos.xb = 0.05;
            paxis->trelpos.ya = 0.40;
            paxis->trelpos.yb = 0.60;
            colorR = 0;
            colorG = 0;
            colorB = 0;
            break;
        case 2:
            paxis = &(pplot->axistop);
            paxis->trelpos.xa = 0.40;
            paxis->trelpos.xb = 0.60;
            paxis->trelpos.ya = 0.02;
            paxis->trelpos.yb = 0.06;
            colorR = 80;
            colorG = 140;
            colorB = 200;
            break;
        case 3:
            paxis = &(pplot->axisright);
            paxis->trelpos.xa = 0.95;
            paxis->trelpos.xb = 0.99;
            paxis->trelpos.ya = 0.40;
            paxis->trelpos.yb = 0.60;
            colorR = 80;
            colorG = 140;
            colorB = 200;
            break;
        default:
            return;
    }

    // Status
    paxis->status = 0x00;
    // Label
    paxis->lfont.ptr = NULL;
    _tcscpy(paxis->lfont.name, _T("Tahoma"));
    paxis->lfont.color.r = colorR;
    paxis->lfont.color.g = colorG;
    paxis->lfont.color.b = colorB;
    paxis->lfont.orient = 0;
    paxis->lfont.size = 10;
    paxis->lfont.style = 0x10;
    // Title
    paxis->title.font.ptr = NULL;
    _tcscpy(paxis->title.font.name, _T("Tahoma"));
    paxis->title.font.color.r = colorR;
    paxis->title.font.color.g = colorG;
    paxis->title.font.color.b = colorB;
    paxis->title.font.orient = 0;
    paxis->title.font.size = 11;
    paxis->title.font.style = 0x10;
    // Format
    _tcscpy(paxis->format, _T("%.3f"));
    // Colors
    paxis->color.r = 100;                paxis->color.g = 110;                paxis->color.b = 140;
    paxis->gridcolor.r = 170;            paxis->gridcolor.g = 180;            paxis->gridcolor.b = 190;
    paxis->gridmcolor.r = 203;            paxis->gridmcolor.g = 203;            paxis->gridmcolor.b = 190;
    paxis->tickcolor.r = 100;            paxis->tickcolor.g = 110;            paxis->tickcolor.b = 140;
    paxis->tickmcolor.r = 180;            paxis->tickmcolor.g = 190;            paxis->tickmcolor.b = 200;
    // Grid
    paxis->gridcount = 5;                paxis->gridmcount = 5;
    // Size
    paxis->size = 2;
    paxis->gridsize = paxis->gridmsize = 1;
    paxis->ticksize = paxis->tickmsize = 1;
    paxis->ticklen = 8;
    paxis->tickmlen = 4;
    paxis->tickstyle = 0x01;
    paxis->tickmstyle = 0x01;
    // Scale
    paxis->min = 0.0;                        paxis->max = 1.0;
    paxis->scale = 0;
    // Options
    paxis->ogrid = 1;                        paxis->ogridm = 0;
    paxis->otick = 1;                        paxis->otickm = 0;
    paxis->olabel = 1;                    paxis->otitle = 1;
}

/*  --------------------------------------------------------------------
    pl_axis_dup        :    copier un axe
    -------------------------------------------------------------------- */
__inline static void pl_axis_dup(axis_t* pTo, const axis_t* pFrom)
{
    if ((pTo == NULL) || (pFrom == NULL)) {
        return;
    }

    // Status
    pTo->status = pFrom->status;
    // Label
    _tcscpy(pTo->lfont.name, pFrom->lfont.name);
    pTo->lfont.color.r = pFrom->lfont.color.r;
    pTo->lfont.color.g = pFrom->lfont.color.g;
    pTo->lfont.color.b = pFrom->lfont.color.b;
    pTo->lfont.orient = pFrom->lfont.orient;
    pTo->lfont.size = pFrom->lfont.size;
    pTo->lfont.style = pFrom->lfont.style;
    // Title
    _tcscpy(pTo->title.text, (const char_t*)(pFrom->title.text));
    _tcscpy(pTo->title.font.name, pFrom->title.font.name);
    pTo->title.font.color.r = pFrom->title.font.color.r;
    pTo->title.font.color.g = pFrom->title.font.color.g;
    pTo->title.font.color.b = pFrom->title.font.color.b;
    pTo->title.font.orient = pFrom->title.font.orient;
    pTo->title.font.size = pFrom->title.font.size;
    pTo->title.font.style = pFrom->title.font.style;
    // Format
    _tcscpy(pTo->format, pFrom->format);
    // Colors
    pTo->color.r = pFrom->color.r;
    pTo->color.g = pFrom->color.g;
    pTo->color.b = pFrom->color.b;
    pTo->gridcolor.r = pFrom->gridcolor.r;
    pTo->gridcolor.g = pFrom->gridcolor.g;
    pTo->gridcolor.b = pFrom->gridcolor.b;
    pTo->gridmcolor.r = pFrom->gridmcolor.r;
    pTo->gridmcolor.g = pFrom->gridmcolor.g;
    pTo->gridmcolor.b = pFrom->gridmcolor.b;
    pTo->tickcolor.r = pFrom->tickcolor.r;
    pTo->tickcolor.g = pFrom->tickcolor.g;
    pTo->tickcolor.b = pFrom->tickcolor.b;
    pTo->tickmcolor.r = pFrom->tickmcolor.r;
    pTo->tickmcolor.g = pFrom->tickmcolor.g;
    pTo->tickmcolor.b = pFrom->tickmcolor.b;
    // Grid
    pTo->gridcount = pFrom->gridcount;
    pTo->gridmcount = pFrom->gridmcount;
    // Size
    pTo->size = pFrom->size;
    pTo->gridsize = pFrom->gridsize;
    pTo->gridmsize = pFrom->gridmsize;
    pTo->ticksize = pFrom->ticksize;
    pTo->tickmsize = pFrom->tickmsize;
    pTo->ticklen = pFrom->ticklen;
    pTo->tickmlen = pFrom->tickmlen;
    pTo->tickstyle = pFrom->tickstyle;
    pTo->tickmstyle = pFrom->tickmstyle;
    // Scale
    pTo->min = pFrom->min;
    pTo->max = pFrom->max;
    pTo->scale = pFrom->scale;
    // Options
    pTo->ogrid = pFrom->ogrid;
    pTo->ogridm = pFrom->ogridm;
    pTo->otick = pFrom->otick;
    pTo->otickm = pFrom->otickm;
    pTo->olabel = pFrom->olabel;
    pTo->otitle = pFrom->otitle;
}

__inline static void pl_reset_curve(plot_t* pplot, int_t iCurve, byte_t bnew)
{
    if (pplot == NULL) {
        return;
    }
    if ((iCurve < 0) || (iCurve >= ML_MAXCURVES)) {
        return;
    }

    pplot->curve[iCurve].points = 0;
    pplot->curve[iCurve].x = pplot->curve[iCurve].y = NULL;
    pplot->curve[iCurve].ex = pplot->curve[iCurve].ey = NULL;
    pplot->curve[iCurve].idx = 0;
    pplot->curve[iCurve].idy = 0;
    pplot->curve[iCurve].status = 0x00;    // Visible
    pplot->curve[iCurve].axisused = 0x00;
    // font.ptr never allocated
    pplot->curve[iCurve].legend.font.ptr = NULL;

    pplot->plint.curvePointA[iCurve] = NULL;
    pplot->plint.curvePointB[iCurve] = NULL;

    if (bnew == 1) {
        pplot->curve[iCurve].linestyle = 0x11;
        pplot->curve[iCurve].linesize = 2;
        pplot->curve[iCurve].linecolor.r = 0;
        pplot->curve[iCurve].linecolor.g = 0;
        pplot->curve[iCurve].linecolor.b = 128;
        pplot->curve[iCurve].exsize = 1;
        pplot->curve[iCurve].excolor.r = 0;
        pplot->curve[iCurve].excolor.g = 128;
        pplot->curve[iCurve].excolor.b = 0;
        pplot->curve[iCurve].eysize = 1;
        pplot->curve[iCurve].eycolor.r = 0;
        pplot->curve[iCurve].eycolor.g = 128;
        pplot->curve[iCurve].eycolor.b = 0;
        pplot->curve[iCurve].dotstyle = 0x01;
        pplot->curve[iCurve].dotsize = 3;
        pplot->curve[iCurve].dotcolor.r = 128;
        pplot->curve[iCurve].dotcolor.g = 0;
        pplot->curve[iCurve].dotcolor.b = 0;
        pplot->curve[iCurve].dropstyleh = 0x00;
        pplot->curve[iCurve].dropstylev = 0x00;
        pplot->curve[iCurve].dropsizeh = 1;
        pplot->curve[iCurve].dropcolorh.r = 0;
        pplot->curve[iCurve].dropcolorh.g = 0;
        pplot->curve[iCurve].dropcolorh.b = 128;
        pplot->curve[iCurve].dropsizev = 1;
        pplot->curve[iCurve].dropcolorv.r = 0;
        pplot->curve[iCurve].dropcolorv.g = 0;
        pplot->curve[iCurve].dropcolorv.b = 128;

        _tcscpy(pplot->curve[iCurve].legend.font.name, _T("Tahoma"));
        pplot->curve[iCurve].legend.font.size = 10;
        pplot->curve[iCurve].legend.font.style = 0;
        pplot->curve[iCurve].legend.font.orient = 0;
        pplot->curve[iCurve].legend.font.color.r = 0;    pplot->curve[iCurve].legend.font.color.g = 0;    pplot->curve[iCurve].legend.font.color.b = 0;

        pplot->curve[iCurve].olegend = 1;

        pplot->curve[iCurve].lrelpos.xa = pplot->relpos[1].xa + 0.02;
        pplot->curve[iCurve].lrelpos.xb = pplot->relpos[1].xa + 0.12;
        if (iCurve > 0) {
            pplot->curve[iCurve].lrelpos.ya = pplot->curve[iCurve - 1].lrelpos.yb;
            if (pplot->curvecount > 0) {
                pplot->curve[iCurve].lrelpos.yb = pplot->curve[iCurve].lrelpos.ya + ((pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (pplot->curvecount));
            }
            else {
                pplot->curve[iCurve].lrelpos.yb = pplot->curve[iCurve].lrelpos.ya + ((pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (ML_MAXCURVES));
            }
        }
        else {
            pplot->curve[iCurve].lrelpos.ya = pplot->relpos[1].ya;
            if (pplot->curvecount > 0) {
                pplot->curve[iCurve].lrelpos.yb = pplot->curve[iCurve].lrelpos.ya + ((pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (pplot->curvecount));
            }
            else {
                pplot->curve[iCurve].lrelpos.yb = pplot->curve[iCurve].lrelpos.ya + ((pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (ML_MAXCURVES));
            }
        }
    }
}

/*  --------------------------------------------------------------------
    pl_validate_axis    :    validation les données

        pplot           :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static byte_t pl_validate_axis(plot_t *pplot, byte_t iAxis)
{
    real_t fminl, fmaxl, fcc, fx = 0.01, fy = 0.001;

    axis_t *paxis = NULL;

    if (pplot == NULL) {
        return 1;
    }

    if (iAxis == 0) {
        paxis = &(pplot->axisbottom);
        fx = 0.01; fy = 0.001;
    }
    else if (iAxis == 1) {
        paxis = &(pplot->axisleft);
        fx = 0.001; fy = 0.01;
    }
    else if (iAxis == 2) {
        paxis = &(pplot->axistop);
        fx = 0.01; fy = 0.001;
    }
    else if (iAxis == 3) {
        paxis = &(pplot->axisright);
        fx = 0.001; fy = 0.01;
    }

    switch (iAxis) {
        case 0:
            if ((paxis->trelpos.xa < 0.0) || (paxis->trelpos.xa > 1.0) || (paxis->trelpos.xb < 0.0) || (paxis->trelpos.xb > 1.0)
                || (paxis->trelpos.ya < 0.0) || (paxis->trelpos.ya > 1.0) || (paxis->trelpos.yb < 0.0) || (paxis->trelpos.yb > 1.0)
                || ((paxis->trelpos.xb - paxis->trelpos.xa) < fx) || ((paxis->trelpos.yb - paxis->trelpos.ya) < fy)) {
                fcc = 0.5 * (pplot->relpos[1].xb + pplot->relpos[1].xa);
                paxis->trelpos.xa = fcc - 0.10;
                paxis->trelpos.xb = fcc + 0.10;
                paxis->trelpos.yb = pplot->relpos[0].yb - 0.02;
                paxis->trelpos.ya = paxis->trelpos.yb - 0.04;
            }
            break;
        case 1:
            if ((paxis->trelpos.xa < 0) || (paxis->trelpos.xa > 1.0) || (paxis->trelpos.xb < 0.0) || (paxis->trelpos.xb > 1.0)
                || (paxis->trelpos.ya < 0) || (paxis->trelpos.ya > 1.0) || (paxis->trelpos.yb < 0.0) || (paxis->trelpos.yb > 1.0)
                || ((paxis->trelpos.xb - paxis->trelpos.xa) < 0.001) || ((paxis->trelpos.yb - paxis->trelpos.ya) < 0.01)) {
                fcc = 0.5 * (pplot->relpos[1].yb + pplot->relpos[1].ya);
                paxis->trelpos.xa = pplot->relpos[0].xa + 0.02;
                paxis->trelpos.xb = paxis->trelpos.xa + 0.04;
                paxis->trelpos.ya = fcc - 0.10;
                paxis->trelpos.yb = fcc + 0.10;
            }
            break;
        case 2:
            if ((paxis->trelpos.xa < 0) || (paxis->trelpos.xa > 1.0) || (paxis->trelpos.xb < 0.0) || (paxis->trelpos.xb > 1.0)
                || (paxis->trelpos.ya < 0) || (paxis->trelpos.ya > 1.0) || (paxis->trelpos.yb < 0.0) || (paxis->trelpos.yb > 1.0)
                || ((paxis->trelpos.xb - paxis->trelpos.xa) < 0.01) || ((paxis->trelpos.yb - paxis->trelpos.ya) < 0.001)) {
                fcc = 0.5 * (pplot->relpos[1].xb + pplot->relpos[1].xa);
                paxis->trelpos.xa = fcc - 0.10;
                paxis->trelpos.xb = fcc + 0.10;
                paxis->trelpos.ya = pplot->relpos[0].ya + 0.02;
                paxis->trelpos.yb = paxis->trelpos.ya + 0.04;
            }
            break;
        case 3:
            if ((paxis->trelpos.xa < 0) || (paxis->trelpos.xa > 1.0) || (paxis->trelpos.xb < 0.0) || (paxis->trelpos.xb > 1.0)
                || (paxis->trelpos.ya < 0) || (paxis->trelpos.ya > 1.0) || (paxis->trelpos.yb < 0.0) || (paxis->trelpos.yb > 1.0)
                || ((paxis->trelpos.xb - paxis->trelpos.xa) < 0.001) || ((paxis->trelpos.yb - paxis->trelpos.ya) < 0.01)) {
                fcc = 0.5 * (pplot->relpos[1].yb + pplot->relpos[1].ya);
                paxis->trelpos.xb = pplot->relpos[0].xb - 0.02;
                paxis->trelpos.xa = paxis->trelpos.xb - 0.04;
                paxis->trelpos.ya = fcc - 0.10;
                paxis->trelpos.yb = fcc + 0.10;
            }
            break;
        default:
            return 1;
    }

    // Min et Max
    if (paxis->max <= paxis->min) {
        return 1;
    }

    // GRID et TICK
    if (paxis->gridcount < 1) { paxis->ogrid = paxis->ogridm = 0; }
    if (paxis->gridcount > PL_MAXNGRID) { paxis->gridcount = PL_MAXNGRID; paxis->ogridm = 0; }
    if (paxis->gridmcount < 1) { paxis->ogridm = 0; }
    if (paxis->gridmcount > PL_MAXNGRIDM) { paxis->gridmcount = PL_MAXNGRIDM; }

    if (paxis->scale == 1) {
        pl_logscale(paxis->min, paxis->max, &fminl, &fmaxl, &(paxis->gridcount));
    }

    // COHERENCE
    if ((paxis->ogrid == 0) || (paxis->gridcount > 5)) { paxis->ogridm = 0; }
    if (paxis->scale == 1) { paxis->gridmcount = 9; }
    if (paxis->otick == 0) { paxis->otickm = 0; }

    // FORMAT
    if (paxis->format[0] == _T('\0')) {
        _tcscpy(paxis->format, _T("%.3f"));
    }

    // Avoid ugly axis labels (e.g. 0.1666667 0.6666667 ...)
    // -> I suppose that min and max are well chosen
    if (paxis->scale == 0) {
        char_t szBuffer[ML_STRSIZE];
        szBuffer[ML_STRSIZE - 1] = _T('\0');

        _tcsprintf(szBuffer, ML_STRSIZE - 1, static_cast<const char_t*>(paxis->format), paxis->min);
        const size_t iLenA = pl_reformat(szBuffer);

        _tcsprintf(szBuffer, ML_STRSIZE - 1, static_cast<const char_t*>(paxis->format), paxis->max);
        const size_t iLenB = pl_reformat(szBuffer);

        const size_t iLen = (iLenA > iLenB) ? iLenA : iLenB;
        if (iLen >= 8) {
            paxis->gridcount = 1;
        }
        else if (paxis->gridcount > 1) {
            double fVal = paxis->max - paxis->min;

            _tcsprintf(szBuffer, ML_STRSIZE - 1, static_cast<const char_t*>(paxis->format), fVal / static_cast<double>(paxis->gridcount));
            size_t iLenC = pl_reformat(szBuffer);

            if (iLenC >= (iLen + 3)) {
                for (int ii = 1; ii <= 3; ii++) {
                    _tcsprintf(szBuffer, ML_STRSIZE - 1, static_cast<const char_t*>(paxis->format), fVal / static_cast<double>(paxis->gridcount + ii));
                    iLenC = pl_reformat(szBuffer);
                    if (iLenC < (iLen + 3)) {
                        paxis->gridcount += ii;
                        break;
                    }

                    if (paxis->gridcount == ii) {
                        continue;
                    }

                    _tcsprintf(szBuffer, ML_STRSIZE - 1, static_cast<const char_t*>(paxis->format), fVal / static_cast<double>(paxis->gridcount - ii));
                    iLenC = pl_reformat(szBuffer);
                    if (iLenC < (iLen + 3)) {
                        paxis->gridcount -= ii;
                        break;
                    }
                }
            }
        }
    }
    //

    return 0;
}

/*  --------------------------------------------------------------------
    pl_draw_axis    :    dessin des axes du graphe

        pplot            :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static void pl_draw_axis(plot_t *pplot, byte_t iAxis)
{
    Gdiplus::Pen *penLine = NULL;

    axis_t *paxis = NULL;
    int_t iLineSize = 1;

    if (pplot == NULL) {
        return;
    }

    switch (iAxis) {
        case 0:
            paxis = &(pplot->axisbottom);
            break;
        case 1:
            paxis = &(pplot->axisleft);
            break;
        case 2:
            paxis = &(pplot->axistop);
            break;
        case 3:
            paxis = &(pplot->axisright);
            break;
        default:
            return;
    }

    if ((paxis->status & 0x0F) == 0x0F) { // Non visible axis
        return;
    }

    if (pplot->errcode) { return; }

    pplot->errcode = 1;

    Gdiplus::Color acolor(paxis->color.r, paxis->color.g, paxis->color.b);

    try {
        penLine = new Gdiplus::Pen(acolor);
    }
    catch (...) {
        penLine = NULL;
        goto pl_draw_axis_RET;
    }

    int_t iDt = 0, iDb = 0, iDl = 0, iDr = 0;
    iDl = (pplot->axisleft.size / 2);
    if (pplot->axisleft.size > 1) {
        iDl += (pplot->axisleft.size % 2);
    }
    iDr = (pplot->axisright.size / 2);
    if (pplot->axisright.size > 1) {
        iDr += (pplot->axisright.size % 2);
    }
    iDt = (pplot->axistop.size / 2);
    if (pplot->axistop.size > 1) {
        iDt += (pplot->axistop.size % 2);
    }
    iDb = (pplot->axisbottom.size / 2);
    if (pplot->axisbottom.size > 1) {
        iDb += (pplot->axisbottom.size % 2);
    }

    penLine->SetWidth((Gdiplus::REAL)(paxis->size));
    penLine->SetLineJoin(Gdiplus::LineJoin::LineJoinMiter);

    setPenTX(pplot, &penLine);

    switch (iAxis) {
        case 0:    // Bottom
            drawLineTX(pplot, pplot->plint.frameRect[1].left - iDl, pplot->plint.frameRect[1].bottom, pplot->plint.frameRect[1].right + iDr, pplot->plint.frameRect[1].bottom,
                false);
            break;

        case 2:    // Top
            drawLineTX(pplot, pplot->plint.frameRect[1].left - iDl, pplot->plint.frameRect[1].top, pplot->plint.frameRect[1].right + iDr, pplot->plint.frameRect[1].top,
                false);
            break;

        case 1:    // Left
            drawLineTX(pplot, pplot->plint.frameRect[1].left, pplot->plint.frameRect[1].top - iDt, pplot->plint.frameRect[1].left, pplot->plint.frameRect[1].bottom + iDb,
                false);
            break;

        case 3:    // Right
            drawLineTX(pplot, pplot->plint.frameRect[1].right, pplot->plint.frameRect[1].top - iDt, pplot->plint.frameRect[1].right, pplot->plint.frameRect[1].bottom + iDb,
                false);
            break;

        default:
            break;
    }

    pplot->errcode = 0;

pl_draw_axis_RET:
    if (penLine) { delete(penLine); penLine = NULL; }
    return;
}

/*  --------------------------------------------------------------------
    pl_grid_axis    :    Grid et Tick (axe)

        pplot       :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static void pl_grid_axis(plot_t* pplot, byte_t iAxis, int_t iX, int_t iX2, int_t iY, int_t iY2)
{
    byte_t ii, jj, iGridMax = 0;
    real_t fn, fm, fxTickA, fxTickB, fxTickBm, fyTickA, fyTickB, fyTickBm,
        fxGridA, fxGridB, fyGridA, fyGridB;
    real_t fval, fvalm, ww, hh, deltaGrid, deltaGridm, fminl, fmaxl;

    Gdiplus::Pen *penGrid = NULL, *penGridm = NULL, *penTick = NULL, *penTickm = NULL;
    Gdiplus::Color gridcolor, gridmcolor, tickcolor, tickmcolor;

    // POUR L'ECHELLE LOG
    const real_t dmf[8] = {0.301029995664, 0.176091259056, 0.124938736608, 0.096910013008,
        0.079181246048, 0.066946789631, 0.057991946978, 0.051152522447};
    //

    axis_t *paxis;

    byte_t isHorz;

    if (pplot == NULL) {
        return;
    }

    switch (iAxis) {
        case 0:
            paxis = &(pplot->axisbottom);

            isHorz = 1;
            fyTickA = pplot->plint.height * pplot->relpos[1].yb;
            if (paxis->tickmstyle == 0x00) {
                fyTickB = fyTickA + (real_t) (paxis->ticklen);
                fyTickBm = fyTickA + (real_t) (paxis->ticklen / 2.0);
            }
            else {
                fyTickB = fyTickA - paxis->ticklen;
                fyTickBm = fyTickA - (real_t) (paxis->ticklen / 2.0);
            }
            fyGridA = (pplot->plint.height * pplot->relpos[1].ya);
            fyGridB = (pplot->plint.height * pplot->relpos[1].yb);
            break;
        case 1:
            paxis = &(pplot->axisleft);

            isHorz = 0;
            fxTickA = (pplot->plint.width * pplot->relpos[1].xa);
            if (paxis->tickmstyle == 0x00) {
                fxTickB = fxTickA - (real_t) (paxis->ticklen);
                fxTickBm = fxTickA - (real_t) (paxis->ticklen / 2.0);
            }
            else {
                fxTickB = fxTickA + (real_t) (paxis->ticklen);
                fxTickBm = fxTickA + (real_t) (paxis->ticklen / 2.0);
            }
            fxGridA = (pplot->plint.width * pplot->relpos[1].xa);
            fxGridB = (pplot->plint.width * pplot->relpos[1].xb);
            break;
        case 2:
            paxis = &(pplot->axistop);

            isHorz = 1;
            if ((pplot->curvecount < 1) || (iX > 0) || (iX2 < 1)) {
                paxis->ogrid = 0;            // No major grid for X2 axis
                paxis->ogridm = 0;        // No minor grid for X2 axis
            }
            fyTickA = (pplot->plint.height * pplot->relpos[1].ya);
            if (paxis->tickmstyle == 0x00) {
                fyTickB = fyTickA - (real_t) (paxis->ticklen);
                fyTickBm = fyTickA - (real_t) (paxis->ticklen / 2.0);
            }
            else {
                fyTickB = fyTickA + (real_t) (paxis->ticklen);
                fyTickBm = fyTickA + (real_t) (paxis->ticklen / 2.0);
            }
            fyGridA = (pplot->plint.height * pplot->relpos[1].ya);
            fyGridB = (pplot->plint.height * pplot->relpos[1].yb);
            break;
        case 3:
            paxis = &(pplot->axisright);

            if ((pplot->curvecount < 1) || (iY > 0) || (iY2 < 1)) {
                paxis->ogrid = 0;            // No major grid for Y2 axis
                paxis->ogridm = 0;        // No minor grid for Y2 axis
            }
            isHorz = 0;
            fxTickA = (pplot->plint.width * pplot->relpos[1].xb);
            if (paxis->tickmstyle == 0x00) {
                fxTickB = fxTickA + (real_t) (paxis->ticklen);
                fxTickBm = fxTickA + (real_t) (paxis->ticklen / 2.0);
            }
            else {
                fxTickB = fxTickA - (real_t) (paxis->ticklen);
                fxTickBm = fxTickA - (real_t) (paxis->ticklen / 2.0);
            }
            fxGridA = (pplot->plint.width * pplot->relpos[1].xa);
            fxGridB = (pplot->plint.width * pplot->relpos[1].xb);
            break;
        default:
            return;
    }

    if (pplot->errcode) { return; }

    if ((paxis->status & 0xF0) == 0xF0) { // Non used axis
        return;
    }
    if ((paxis->status & 0x0F) == 0x0F) { // Non visible axis
        return;
    }

    if ((paxis->ogrid == 0) && (paxis->otick == 0)) {
        return;
    }

    pplot->errcode = 1;

    // Plot frame dimension
    ww = pplot->plint.width * (pplot->relpos[1].xb - pplot->relpos[1].xa);
    hh = pplot->plint.height * (pplot->relpos[1].yb - pplot->relpos[1].ya);
    if ((ww < PL_MINWIDTH) || (hh < PL_MINHEIGHT)) {
        _tcscpy(pplot->message, _T("Invalid plot frame"));
        return;
    }
    //

    gridcolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, paxis->gridcolor.r, paxis->gridcolor.g, paxis->gridcolor.b));
    gridmcolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, paxis->gridmcolor.r, paxis->gridmcolor.g, paxis->gridmcolor.b));
    tickcolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, paxis->tickcolor.r, paxis->tickcolor.g, paxis->tickcolor.b));
    tickmcolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, paxis->tickmcolor.r, paxis->tickmcolor.g, paxis->tickmcolor.b));

    try {
        penGrid = new Gdiplus::Pen(gridcolor);
    }
    catch (...) {
        penGrid = NULL;
        goto pl_grid_axis_RET;
    }

    try {
        penGridm = new Gdiplus::Pen(gridmcolor);
    }
    catch (...) {
        penGridm = NULL;
        goto pl_grid_axis_RET;
    }

    try {
        penTick = new Gdiplus::Pen(tickcolor);
    }
    catch (...) {
        penTick = NULL;
        goto pl_grid_axis_RET;
    }

    try {
        penTickm = new Gdiplus::Pen(tickmcolor);
    }
    catch (...) {
        penTickm = NULL;
        goto pl_grid_axis_RET;
    }

    penGrid->SetWidth((Gdiplus::REAL)(paxis->gridsize));
    penGridm->SetWidth((Gdiplus::REAL)(paxis->gridmsize));
    penTick->SetWidth((Gdiplus::REAL)(paxis->ticksize));
    penTickm->SetWidth((Gdiplus::REAL)(paxis->tickmsize));

    iGridMax = 0;
    if (paxis->scale == 1) {
        pl_logscale(paxis->min, paxis->max, &fminl, &fmaxl, &(paxis->gridcount));
        if (paxis->gridcount > PL_MAXNGRID) {
            paxis->ogridm = 0;
            paxis->otickm = 0;
            iGridMax = 1;
        }
    }

    if (isHorz) {
        deltaGrid = (pplot->relpos[1].xb - pplot->relpos[1].xa) / (real_t) (paxis->gridcount);
    }
    else {
        deltaGrid = (pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (paxis->gridcount);
    }

    // SVG
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        pplot->svgdoc->setFill(svgFill(svgColor(0, 0, 0, true)));
    }

    if (paxis->ogridm == 1) {        // GRID MINOR

        deltaGridm = deltaGrid / (real_t) (paxis->gridmcount);

        fvalm = (isHorz) ? pplot->relpos[1].xa : pplot->relpos[1].yb;
        for (jj = 1; jj < paxis->gridmcount; jj++) {
            if (isHorz) {
                if (paxis->scale == 1) {
                    fvalm += (dmf[jj-1] * deltaGrid);
                }
                else {
                    fvalm += deltaGridm;
                }
                if (fvalm >= pplot->relpos[1].xb) { break; }
                fm = (pplot->plint.width * fvalm);
                setPenTX(pplot, &penGridm);

                drawLineTX(pplot, fm, fyGridA, fm, fyGridB);

                // Tick
                if (paxis->otickm == 1) {
                    setPenTX(pplot, &penTickm);

                    drawLineTX(pplot, fm, fyTickA, fm, fyTickBm);
                }

            }
            else {
                if (paxis->scale == 1) {
                    fvalm -= (dmf[jj-1] * deltaGrid);
                }
                else {
                    fvalm -= deltaGridm;
                }
                if (fvalm <= pplot->relpos[1].ya) { break; }
                fm = (pplot->plint.height * fvalm);
                setPenTX(pplot, &penGridm);

                drawLineTX(pplot, fxGridA, fm, fxGridB, fm);

                // Tick
                if (paxis->otickm == 1) {
                    setPenTX(pplot, &penTickm);

                    drawLineTX(pplot, fxTickBm, fm, fxTickA, fm);
                }
            }
        }

        fval = (isHorz) ? pplot->relpos[1].xa : pplot->relpos[1].yb;

        //
        for (ii = 1; ii < paxis->gridcount; ii++) {
            if (isHorz) {
                fval += deltaGrid;
                if (fval >= pplot->relpos[1].xb) { break; }

                fn = (pplot->plint.width * fval);

                setPenTX(pplot, &penGrid);

                drawLineTX(pplot, fn, fyGridA, fn, fyGridB);

                // Tick
                if (paxis->otick == 1) {
                    setPenTX(pplot, &penTick);
                    drawLineTX(pplot, fn, fyTickA, fn, fyTickB);
                }

                // GRID MINOR
                fvalm = fval;
                for (jj = 1; jj < paxis->gridmcount; jj++) {
                    if (paxis->scale == 1) {
                        fvalm += (dmf[jj-1] * deltaGrid);
                    }
                    else {
                        fvalm += deltaGridm;
                    }
                    if (fvalm >= pplot->relpos[1].xb) { break; }

                    fm = (pplot->plint.width * fvalm);

                    setPenTX(pplot, &penGridm);

                    drawLineTX(pplot, fm, fyGridA, fm, fyGridB);

                    // Tick
                    if (paxis->otickm == 1) {
                        setPenTX(pplot, &penTickm);
                        drawLineTX(pplot, fm, fyTickA, fm, fyTickBm);
                    }
                }
            }
            else {
                fval -= deltaGrid;
                if (fval <= pplot->relpos[1].ya) { break; }

                fn = (pplot->plint.height * fval);

                setPenTX(pplot, &penGrid);

                drawLineTX(pplot, fxGridA, fn, fxGridB, fn);

                // Tick
                if (paxis->otick == 1) {
                    setPenTX(pplot, &penTick);

                    drawLineTX(pplot, fxTickB, fn, fxTickA, fn);
                }

                // GRID MINOR
                fvalm = fval;
                for (jj = 1; jj < paxis->gridmcount; jj++) {
                    if (paxis->scale == 1) {
                        fvalm -= (dmf[jj-1] * deltaGrid);
                    }
                    else {
                        fvalm -= deltaGridm;
                    }
                    if (fvalm <= pplot->relpos[1].ya) { break; }

                    fm = (pplot->plint.height * fvalm);

                    setPenTX(pplot, &penGridm);

                    drawLineTX(pplot, fxGridA, fm, fxGridB, fm);

                    // Tick
                    if (paxis->otickm == 1) {
                        setPenTX(pplot, &penGrid);

                        drawLineTX(pplot, fxTickBm, fm, fxTickA, fm);
                    }
                }
            }

        }
    }
    else if (paxis->otickm == 1) {        // TICK MINOR

        deltaGridm = deltaGrid / (real_t) (paxis->gridmcount);

        fvalm = (isHorz) ? pplot->relpos[1].xa : pplot->relpos[1].yb;
        for (jj = 1; jj < paxis->gridmcount; jj++) {
            if (isHorz) {
                if (paxis->scale == 1) {
                    fvalm += (dmf[jj-1] * deltaGrid);
                }
                else {
                    fvalm += deltaGridm;
                }
                if (fvalm >= pplot->relpos[1].xb) { break; }
                fm = (pplot->plint.width * fvalm);
                setPenTX(pplot, &penTickm);

                drawLineTX(pplot, fm, fyTickA, fm, fyTickBm);
            }
            else {
                if (paxis->scale == 1) {
                    fvalm -= (dmf[jj-1] * deltaGrid);
                }
                else {
                    fvalm -= deltaGridm;
                    //fvalm -= deltaGrid;
                }
                if (fvalm <= pplot->relpos[1].ya) { break; }
                fm = (pplot->plint.height * fvalm);
                setPenTX(pplot, &penTickm);

                drawLineTX(pplot, fxTickBm, fm, fxTickA, fm);
            }
        }

        fval = (isHorz) ? pplot->relpos[1].xa : pplot->relpos[1].yb;

        //
        for (ii = 1; ii < paxis->gridcount; ii++) {
            if (isHorz) {
                fval += deltaGrid;
                if (fval >= pplot->relpos[1].xb) { break; }

                fn = (pplot->plint.width * fval);
                setPenTX(pplot, &penTick);

                drawLineTX(pplot, fn, fyTickA, fn, fyTickB);

                // TICK MINOR
                fvalm = fval;
                setPenTX(pplot, &penTickm);

                for (jj = 1; jj < paxis->gridmcount; jj++) {
                    if (paxis->scale == 1) {
                        fvalm += (dmf[jj-1] * deltaGrid);
                    }
                    else {
                        fvalm += deltaGridm;
                    }
                    if (fvalm >= pplot->relpos[1].xb) { break; }

                    fm = (pplot->plint.width * fvalm);

                    drawLineTX(pplot, fm, fyTickA, fm, fyTickBm);
                }
            }
            else {
                fval -= deltaGrid;
                if (fval <= pplot->relpos[1].ya) { break; }

                fn = (pplot->plint.height * fval);

                setPenTX(pplot, &penTick);

                drawLineTX(pplot, fxTickB, fn, fxTickA, fn);

                // TICK MINOR
                fvalm = fval;
                for (jj = 1; jj < paxis->gridmcount; jj++) {
                    if (paxis->scale == 1) {
                        fvalm -= (dmf[jj-1] * deltaGrid);
                    }
                    else {
                        fvalm -= deltaGridm;
                        //fvalm -= deltaGrid;
                    }
                    if (fvalm <= pplot->relpos[1].ya) { break; }

                    fm = (pplot->plint.height * fvalm);

                    setPenTX(pplot, &penTickm);

                    drawLineTX(pplot, fxTickBm, fm, fxTickA, fm);
                }
            }

        }
    }
    else if (paxis->ogrid == 1) {            // Non major or minor grid for X2 and Y2 axis
        if (isHorz) {                        // Horizontal Axis
            fval = pplot->relpos[1].xa;
            for (ii = 1; ii < paxis->gridcount; ii++) {
                fval += deltaGrid;
                if (fval >= pplot->relpos[1].xb) { break; }

                if (iGridMax) {
                    if ((ii % 2) == 1) { continue; }
                }

                fn = (pplot->plint.width * fval);

                setPenTX(pplot, &penGrid);

                drawLineTX(pplot, fn, fyGridA, fn, fyGridB);

                // Tick
                if (paxis->otick == 1) {
                    setPenTX(pplot, &penTick);
                    if (paxis->tickstyle == 0x00) {
                        drawLineTX(pplot, fn, fyGridB, fn, fyGridB + (real_t) (paxis->ticklen));
                    }
                    else {
                        drawLineTX(pplot, fn, fyGridB, fn, fyGridB - (real_t) (paxis->ticklen));
                    }
                }
            }
        }
        else {                                    // Vertical Axis
            fval = pplot->relpos[1].yb;
            for (ii = 1; ii < paxis->gridcount; ii++) {
                fval -= deltaGrid;
                if (fval <= pplot->relpos[1].ya) { break; }

                if (iGridMax) {
                    if ((ii % 2) == 1) { continue; }
                }

                fn = (pplot->plint.height * fval);

                setPenTX(pplot, &penGrid);

                drawLineTX(pplot, fxGridA, fn, fxGridB, fn);

                // Tick
                if (paxis->otick == 1) {
                    setPenTX(pplot, &penTick);

                    if (paxis->tickstyle == 0x00) {
                        drawLineTX(pplot, fxGridA, fn, fxGridA - (real_t) (paxis->ticklen), fn);
                    }
                    else {
                        drawLineTX(pplot, fxGridA, fn, fxGridA + (real_t) (paxis->ticklen), fn);
                    }
                }
            }
        }
    }
    else if (paxis->otick == 1) {
        if (isHorz) {    // Horizontal Axis
            fval = pplot->relpos[1].xa;
            for (ii = 1; ii < paxis->gridcount; ii++) {
                fval += deltaGrid;
                if (fval >= pplot->relpos[1].xb) { break; }

                if (iGridMax) {
                    if ((ii % 2) == 1) { continue; }
                }

                fn = (pplot->plint.width * fval);

                setPenTX(pplot, &penTick);

                drawLineTX(pplot, fn, fyTickA, fn, fyTickB);
            }
        }
        else {    // Vertical Axis
            fval = pplot->relpos[1].yb;
            for (ii = 1; ii < paxis->gridcount; ii++) {
                fval -= deltaGrid;
                if (fval <= pplot->relpos[1].ya) { break; }

                if (iGridMax) {
                    if ((ii % 2) == 1) { continue; }
                }

                fn = (pplot->plint.height * fval);

                setPenTX(pplot, &penTick);

                drawLineTX(pplot, fxTickA, fn, fxTickB, fn);
            }
        }
    }

    pplot->errcode = 0;

pl_grid_axis_RET:
    if (penGrid) { delete(penGrid); penGrid = NULL; }
    if (penGridm) { delete(penGridm); penGridm = NULL; }
    if (penTick) { delete(penTick); penTick = NULL; }
    if (penTickm) { delete(penTickm); penTickm = NULL; }
}


/*  --------------------------------------------------------------------
    pl_label_axis        :    label axe

        pplot            :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static void pl_label_axis(plot_t* pplot, byte_t iAxis)
{
    irect_t recFrame = {0,0,0,0}, frameRect = {0,0,0,0};
    int_t ii;
    real_t fval, ft, fw, ttd, frange, xa, xb, ya, yb, ll, rr, tt, bb, 
        deltaGrid, axismin, axismax;
    char_t szBuffer[ML_STRSIZE];
    axis_t *paxis;
    byte_t isHorz, iGridMax = 0;
    LONG uiTmp = 0L;
    SIZE sizeText = {0L,0L};
    int_t nlen;
    BOOL bItalic = FALSE;
    int_t iDelta = 4;

    Gdiplus::SolidBrush *pBrushTitle = NULL, *pBrushLabel = NULL;
    Gdiplus::Font *pFontTitle = NULL, *pFontTitleSm = NULL, *pFontLabel = NULL;
    Gdiplus::Color fontcolorLabel, fontcolorTitle;

    if (pplot == NULL) {
        return;
    }

    switch (iAxis) {
        case 0:
            paxis = &(pplot->axisbottom);
            isHorz = 1;
            xa = pplot->relpos[1].xa;    xb = pplot->relpos[1].xb;
            ya = pplot->relpos[1].yb;    yb = ya + 0.05;
            iDelta = 4;
            break;
        case 1:
            paxis = &(pplot->axisleft);
            isHorz = 0;
            xb = pplot->relpos[1].xa;    xa = xb - 0.10;
            if (xa < 0.0) { xa = 0.0; }
            ya = pplot->relpos[1].ya;    yb = pplot->relpos[1].yb;
            iDelta = -4;
            break;
        case 2:
            paxis = &(pplot->axistop);
            isHorz = 1;
            xa = pplot->relpos[1].xa;    xb = pplot->relpos[1].xb;
            yb = pplot->relpos[1].ya;    ya = yb - 0.05;
            if (ya < 0.0) { ya = 0.0; }
            iDelta = -4;
            break;
        case 3:
            paxis = &(pplot->axisright);
            isHorz = 0;
            xa = pplot->relpos[1].xb;    xb = xa + 0.10;
            ya = pplot->relpos[1].ya;    yb = pplot->relpos[1].yb;
            iDelta = 4;
            break;
        default:
            return;
    }

    if (pplot->errcode) { return; }

    if ((paxis->status & 0xF0) == 0xF0) { // Non used axis
        return;
    }
    if ((paxis->status & 0x0F) == 0x0F) { // Non visible axis
        return;
    }

    if ((paxis->olabel == 0) && (paxis->otitle == 0)) {
        return;
    }

    iGridMax = 0;
    if (paxis->scale == 0) {
        axismin = paxis->min;        axismax = paxis->max;
    }
    else {
        pl_logscale(paxis->min, paxis->max, &axismin, &axismax, &(paxis->gridcount));
        if (paxis->gridcount > PL_MAXNGRID) {
            paxis->ogridm = 0;
            paxis->otickm = 0;
            iGridMax = 1;
        }
    }

    if (axismax <= axismin) {
        return;
    }

    pplot->errcode = 1;

    frange = axismax - axismin;

    fontcolorLabel.SetValue(Gdiplus::Color::MakeARGB(0xFF, paxis->lfont.color.r, paxis->lfont.color.g, paxis->lfont.color.b));
    fontcolorTitle.SetValue(Gdiplus::Color::MakeARGB(0xFF, paxis->title.font.color.r, paxis->title.font.color.g, paxis->title.font.color.b));

    Gdiplus::FontStyle fontStyleTitle = Gdiplus::FontStyleRegular;
    int iSizeSm = getFontSizeSm(&(paxis->title.font));

    Gdiplus::RectF rcF;
    Gdiplus::PointF ptF;

    try {
        pBrushTitle = new Gdiplus::SolidBrush(fontcolorTitle);
    }
    catch (...) {
        pBrushTitle = NULL;
        goto pl_label_axis_RET;
    }

    try {
        pBrushLabel = new Gdiplus::SolidBrush(fontcolorLabel);
    }
    catch (...) {
        pBrushLabel = NULL;
        goto pl_label_axis_RET;
    }

    if ((paxis->title.font.style & 0xF0) == 0x10) {
        if ((paxis->title.font.style & 0x0F) == 0x01) {
            fontStyleTitle = Gdiplus::FontStyleBoldItalic;
        }
        else {
            fontStyleTitle = Gdiplus::FontStyleBold;
        }
    }
    else if ((paxis->title.font.style & 0x0F) == 0x01) {
        fontStyleTitle = Gdiplus::FontStyleItalic;
    }
    Gdiplus::FontStyle fontStyleLabel = Gdiplus::FontStyleRegular;
    if ((paxis->lfont.style & 0xF0) == 0x10) {
        if ((paxis->lfont.style & 0x0F) == 0x01) {
            fontStyleLabel = Gdiplus::FontStyleBoldItalic;
        }
        else {
            fontStyleLabel = Gdiplus::FontStyleBold;
        }
    }
    else if ((paxis->lfont.style & 0x0F) == 0x01) {
        fontStyleLabel = Gdiplus::FontStyleItalic;
    }

    try {
        pFontTitle = new Gdiplus::Font(paxis->title.font.name, (Gdiplus::REAL)(paxis->title.font.size), fontStyleTitle);
    }
    catch (...) {
        pFontTitle = NULL;
        goto pl_label_axis_RET;
    }

    try {
        pFontTitleSm = new Gdiplus::Font(paxis->title.font.name, (Gdiplus::REAL)iSizeSm, fontStyleTitle);
    }
    catch (...) {
        pFontTitleSm = NULL;
        goto pl_label_axis_RET;
    }

    try {
        pFontLabel = new Gdiplus::Font(paxis->lfont.name, (Gdiplus::REAL)(paxis->lfont.size), fontStyleLabel);
    }
    catch (...) {
        pFontLabel = NULL;
        goto pl_label_axis_RET;
    }

    if (paxis->gridcount >= 1) {
        if (isHorz) {
            deltaGrid = (pplot->relpos[1].xb - pplot->relpos[1].xa) / (real_t) (paxis->gridcount);
        }
        else {
            deltaGrid = (pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (paxis->gridcount);
        }
    }

    // [Add]20110323
    if (pplot->canvas->MeasureString(_T("D"), 1, pFontLabel, ptF, &rcF) == Gdiplus::Status::Ok) {
        if (iDelta > 0) {
            iDelta = (int_t) (rcF.Height / 2.0f);
            if (iDelta < 2) {
                iDelta = 2;
            }
            else if (iDelta > 20) {
                iDelta = 20;
            }
        }
        else if (iDelta < 0) {
            iDelta = -(int_t) (rcF.Height / 2.0f);
            if (-iDelta < 2) {
                iDelta = -2;
            }
            else if (-iDelta > 20) {
                iDelta = -20;
            }
        }
    }

    // SVG
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        pplot->svgdoc->setStroke(svgStroke(1.0, svgColor((int_t) (fontcolorLabel.GetR()), (int_t) (fontcolorLabel.GetG()), (int_t) (fontcolorLabel.GetB()))));
        pplot->svgdoc->setFill(svgFill(svgColor(0, 0, 0, true)));
    }

    bool bDrawLabel = true;
    int iMod = 1;
    if ((paxis->gridcount >= 12) && (paxis->gridcount < 24)) {
        iMod = 2;
    }
    else if ((paxis->gridcount >= 24) && (paxis->gridcount < 36)) {
        iMod = 3;
    }
    else if (paxis->gridcount >= 36) {
        iMod = 4;
    }

    // LABEL
    if ((paxis->olabel == 1) && (paxis->gridcount >= 1)) {

        bItalic = FALSE;

        if (isHorz) {
            ttd = deltaGrid * 0.5;
            if (ttd > pplot->relpos[1].xa) { ttd = pplot->relpos[1].xa; };
            if (ttd > (1.0 - pplot->relpos[1].xb)) { ttd = 1.0 - pplot->relpos[1].xb; };

            // Labels attached to the axis
            tt = pplot->plint.height * ya;
            bb = pplot->plint.height * yb;
            ll = pplot->plint.width * (xa - ttd);
            rr = pplot->plint.width * (xa + ttd);
            if (iDelta > 0) {
                frameRect.top = (LONG) round(tt) + iDelta;
                frameRect.bottom = (LONG) round(bb);
            }
            else {
                frameRect.top = (LONG) round(tt);
                frameRect.bottom = (LONG) round(bb) + iDelta;
            }
            frameRect.left = (LONG) round(ll);
            frameRect.right = (LONG) round(rr);

            if (paxis->scale == 0) {
                _tcsprintf(szBuffer, (ML_STRSIZE - 1), (const char_t*)(paxis->format), axismin);
                pl_reformat(szBuffer);
            }
            else {
                if (axismin > 0.0) {
                    _tcsprintf(szBuffer, (ML_STRSIZE - 1), (axismin < 9) ? _T("1e+%01d") : _T("1e+%02d"), (int_t)axismin);
                }
                else if (axismin == 0.0) {
                    _tcscpy(szBuffer, _T("1e00"));
                }
                else {
                    _tcsprintf(szBuffer, (ML_STRSIZE - 1), (fabs(axismin) < 9) ? _T("1e-%01d") : _T("1e-%02d"), (int_t)fabs(axismin));
                }
            }
            nlen = (int_t)_tcslen(szBuffer);
            // Adapter � la taille du texte
            if (pplot->canvas->MeasureString(szBuffer, nlen, pFontLabel, ptF, &rcF) == Gdiplus::Status::Ok) {
                if (iDelta > 0) {
                    frameRect.bottom = frameRect.top + (int_t)(rcF.Height);
                }
                else {
                    frameRect.top = frameRect.bottom - (int_t)(rcF.Height);
                }
            }
            ptF.X = (Gdiplus::REAL)(((frameRect.left + frameRect.right) >> 1) - (int_t) (rcF.Width / 2.0f));
            rcF.X = ptF.X;
            if (iAxis == 0) {
                ptF.Y = (Gdiplus::REAL)(frameRect.top);
            }
            else {
                ptF.Y = (Gdiplus::REAL)((frameRect.bottom) - (int_t) (rcF.Height));
            }
            rcF.Y = ptF.Y;

            drawStringTX(pplot, (const char_t*) szBuffer, (int_t) _tcslen(szBuffer), pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);

            //    Position des différents rectangles        0:LABEL-BOTTOM        1:LABEL-LEFT    2:LABEL-TOP            3:LABEL-RIGHT
            pplot->plint.axislRect[iAxis].bottom = frameRect.bottom;
            pplot->plint.axislRect[iAxis].top = frameRect.top;
            pplot->plint.axislRect[iAxis].left = frameRect.left;

            ll = pplot->plint.width * (xa - ttd);
            rr = pplot->plint.width * (xa + ttd);

            fw = frange / (real_t) (paxis->gridcount);

            fval = axismin;
            for (ii = 1; ii < paxis->gridcount; ii++) {
                fval += fw;

                // >> éviter un affichage du genre 1.23768e-25
                ft = fabs(fval);
                if ((ft < DBL_EPSILON) && (ft < (0.01 * fabs(frange)))) {
                    fval = 0.0;
                }
                // <<

                bDrawLabel = ((ii % iMod) == 0);

                if (bDrawLabel) {
                    if (paxis->scale == 0) {
                        _tcsprintf(szBuffer, (ML_STRSIZE - 1), (const char_t*) (paxis->format), fval);
                    }
                    else {
                        if (fval > 0) {
                            _tcsprintf(szBuffer, (ML_STRSIZE - 1), (fval < 9) ? _T("1e+%01d") : _T("1e+%02d"), (int_t) fval);
                        }
                        else if (fval == 0.0) {
                            _tcscpy(szBuffer, _T("1e00"));
                        }
                        else {
                            _tcsprintf(szBuffer, (ML_STRSIZE - 1), (fabs(fval) < 9) ? _T("1e-%01d") : _T("1e-%02d"), (int_t) fabs(fval));
                        }
                    }
                }

                ll += deltaGrid * pplot->plint.width; rr += deltaGrid * pplot->plint.width;
                frameRect.left = (LONG) round(ll); frameRect.right = (LONG) round(rr);
    
                if (iGridMax) {
                    if ((ii % 2) == 1) { continue; }
                }

                if (bDrawLabel) {
                    pl_reformat(szBuffer);
                    nlen = (int_t) _tcslen(szBuffer);
                    pplot->canvas->MeasureString(szBuffer, nlen, pFontLabel, ptF, &rcF);
                    ptF.X = (Gdiplus::REAL)(((frameRect.left + frameRect.right) >> 1) - (int_t) (rcF.Width / 2.0f));
                    if (iAxis == 0) {
                        ptF.Y = (Gdiplus::REAL)(frameRect.top);
                    }
                    else {
                        ptF.Y = (Gdiplus::REAL)((frameRect.bottom) - (int_t) (rcF.Height));
                    }
                    rcF.X = ptF.X;
                    rcF.Y = ptF.Y;
                    drawStringTX(pplot, (const char_t*) szBuffer, (int_t) _tcslen(szBuffer), pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);
                }
            }

            ll += deltaGrid * pplot->plint.width; rr += deltaGrid * pplot->plint.width;
            frameRect.left = (LONG) round(ll); frameRect.right = (LONG) round(rr);

            bDrawLabel = ((paxis->gridcount % iMod) == 0);

            if (bDrawLabel) {

                if (paxis->scale == 0) {
                    _tcsprintf(szBuffer, (ML_STRSIZE - 1), (const char_t*) (paxis->format), axismax);
                }
                else {
                    if (axismax > 0) {
                        _tcsprintf(szBuffer, (ML_STRSIZE - 1), (axismax < 9) ? _T("1e+%01d") : _T("1e+%02d"), (int_t) axismax);
                    }
                    else if (axismax == 0.0) {
                        _tcscpy(szBuffer, _T("1e00"));
                    }
                    else {
                        _tcsprintf(szBuffer, (ML_STRSIZE - 1), (fabs(axismax) < 9) ? _T("1e-%01d") : _T("1e-%02d"), (int_t) fabs(axismax));
                    }
                }
                pl_reformat(szBuffer);
                nlen = (int_t) _tcslen(szBuffer);
                pplot->canvas->MeasureString(szBuffer, nlen, pFontLabel, ptF, &rcF);
                if (iGridMax) {
                    if ((paxis->gridcount % 2) == 0) {
                        ptF.X = (Gdiplus::REAL)(((frameRect.left + frameRect.right) >> 1) - (int_t) (rcF.Width / 2.0f));
                        if (iAxis == 0) {
                            ptF.Y = (Gdiplus::REAL)(frameRect.top);
                        }
                        else {
                            ptF.Y = (Gdiplus::REAL)((frameRect.bottom) - (int_t) (rcF.Height));
                        }
                        rcF.X = ptF.X;
                        rcF.Y = ptF.Y;
                        drawStringTX(pplot, (const char_t*) szBuffer, (int_t) _tcslen(szBuffer), pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);
                    }
                }
                else {
                    ptF.X = (Gdiplus::REAL)(((frameRect.left + frameRect.right) >> 1) - (int_t) (rcF.Width / 2.0f));
                    if (iAxis == 0) {
                        ptF.Y = (Gdiplus::REAL)(frameRect.top);
                    }
                    else {
                        ptF.Y = (Gdiplus::REAL)((frameRect.bottom) - (int_t) (rcF.Height));
                    }
                    rcF.X = ptF.X;
                    rcF.Y = ptF.Y;
                    drawStringTX(pplot, (const char_t*) szBuffer, (int_t) _tcslen(szBuffer), pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);
                }
            }

            //    Position des différents rectangles        0:LABEL-BOTTOM        1:LABEL-LEFT    2:LABEL-TOP            3:LABEL-RIGHT
            pplot->plint.axislRect[iAxis].right = frameRect.right;
        }
        else { // Vertical axis
            ttd = deltaGrid * 0.5;
            if (ttd > pplot->relpos[1].ya) { ttd = pplot->relpos[1].ya; };
            if (ttd > (1.0 - pplot->relpos[1].yb)) { ttd = 1.0 - pplot->relpos[1].yb; };

            if (iDelta > 0) {
                ll = pplot->plint.width * xa + iDelta;
                rr = pplot->plint.width * xb;
            }
            else {
                ll = pplot->plint.width * xa;
                rr = pplot->plint.width * xb + iDelta;
            }
            tt = pplot->plint.height * (yb - ttd);
            bb = pplot->plint.height * (yb + ttd);
            frameRect.left = (LONG) round(ll);
            frameRect.right = (LONG) round(rr);
            frameRect.top = (LONG) round(tt);
            frameRect.bottom = (LONG) round(bb);

            if (paxis->scale == 0) {
                _tcsprintf(szBuffer, (ML_STRSIZE - 7), (const char_t*)(paxis->format), axismin);
                pl_reformat(szBuffer);
            }
            else {
                if (axismin > 0) {
                    _tcsprintf(szBuffer, (ML_STRSIZE - 7), (axismin < 9) ? _T("1e+%01d") : _T("1e+%02d"), (int_t)axismin);
                }
                else if (axismin == 0) {
                    _tcscpy(szBuffer, _T("1e00"));
                }    
                else {
                    _tcsprintf(szBuffer, (ML_STRSIZE - 7), (fabs(axismin) < 9) ? _T("1e-%01d") : _T("1e-%02d"), (int_t)fabs(axismin));
                }
            }

            nlen = (int_t)_tcslen(szBuffer);
            if (bItalic) {
                szBuffer[nlen] = _T(' ');
                szBuffer[nlen + 1] = _T('\0');
                nlen += 1;
            }
            // Adapter à la taille du texte
            if (pplot->canvas->MeasureString(szBuffer, nlen, pFontLabel, ptF, &rcF) == Gdiplus::Status::Ok) {
                if (iDelta > 0) {        // Right-Axis
                    frameRect.right = frameRect.left + (int_t)(rcF.Width);
                }
                else {                    // Left-Axis
                    frameRect.left = frameRect.right - (int_t)(rcF.Width);
                }
            }

            ptF.X = (Gdiplus::REAL)(frameRect.left);
            ptF.Y = (Gdiplus::REAL)(((frameRect.top + frameRect.bottom) >> 1) - (int_t) (rcF.Height / 2.0f));
            rcF.X = ptF.X;
            rcF.Y = ptF.Y;
            drawStringTX(pplot, (const char_t*) szBuffer, nlen, pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);

            //    Position des différents rectangles        0:LABEL-BOTTOM        1:LABEL-LEFT    2:LABEL-TOP            3:LABEL-RIGHT
            pplot->plint.axislRect[iAxis].bottom = frameRect.bottom;
            pplot->plint.axislRect[iAxis].left = frameRect.left;
            pplot->plint.axislRect[iAxis].right = frameRect.right;

            tt = pplot->plint.height * (yb - ttd);
            bb = pplot->plint.height * (yb + ttd);

            fval = axismin; fw = frange / (real_t) (paxis->gridcount);

            for (ii = 1; ii < paxis->gridcount; ii++) {

                if (paxis->scale == 0) {
                    fval += fw;
                }
                else {
                    fval += 1.0;
                }

                // >> éviter un affichage du genre 1.23768e-25
                ft = fabs(fval);
                if ((ft < DBL_EPSILON) && (ft < (0.01 * fabs(frange)))) {
                    fval = 0.0;
                }
                // <<

                bDrawLabel = ((ii % iMod) == 0);

                if (bDrawLabel) {
                    if (paxis->scale == 0) {
                        _tcsprintf(szBuffer, (ML_STRSIZE - 1), (const char_t*) (paxis->format), fval);
                        pl_reformat(szBuffer);
                    }
                    else {
                        if (fval > 0) {
                            _tcsprintf(szBuffer, (ML_STRSIZE - 1), (fval < 9) ? _T("1e+%01d") : _T("1e+%02d"), (int_t) fval);
                        }
                        else if (fval == 0) {
                            _tcscpy(szBuffer, _T("1e00"));
                        }
                        else {
                            _tcsprintf(szBuffer, (ML_STRSIZE - 1), (fabs(fval) < 9) ? _T("1e-%01d") : _T("1e-%02d"), (int_t) fabs(fval));
                        }
                    }
                }

                tt -= deltaGrid * pplot->plint.height;
                bb -= deltaGrid * pplot->plint.height;
                frameRect.top = (LONG) round(tt); frameRect.bottom = (LONG) round(bb);

                nlen = (int_t)_tcslen(szBuffer);
                if (bItalic) {
                    szBuffer[nlen] = _T(' ');
                    szBuffer[nlen + 1] = _T('\0');
                    nlen += 1;
                }
                // Adapter � la taille du texte
                
                if (pplot->canvas->MeasureString(szBuffer, nlen, pFontLabel, ptF, &rcF) == Gdiplus::Status::Ok) {
                    if (iDelta > 0) {        // Right-Axis
                        frameRect.right = frameRect.left + (int_t)(rcF.Width);
                    }
                    else {                    // Left-Axis
                        frameRect.left = frameRect.right - (int_t)(rcF.Width);
                    }
                }

                if (iGridMax) {
                    if ((ii % 2) == 1) { continue; }
                }

                if (bDrawLabel) {
                    ptF.X = (Gdiplus::REAL)(frameRect.left);
                    ptF.Y = (Gdiplus::REAL)(((frameRect.top + frameRect.bottom) >> 1) - (int_t) (rcF.Height / 2.0f));
                    rcF.X = ptF.X;
                    rcF.Y = ptF.Y;
                    drawStringTX(pplot, (const char_t*) szBuffer, nlen, pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);
                }

                //    Position des différents rectangles        0:LABEL-BOTTOM        1:LABEL-LEFT    2:LABEL-TOP            3:LABEL-RIGHT
                if (iDelta > 0) {        // Right-Axis
                    if (frameRect.right > pplot->plint.axislRect[iAxis].right) {
                        pplot->plint.axislRect[iAxis].right = frameRect.right;
                    }
                }
                else {                    // Left-Axis
                    if (frameRect.left < pplot->plint.axislRect[iAxis].left) {
                        pplot->plint.axislRect[iAxis].left = frameRect.left;
                    }
                }
            }

            tt -= deltaGrid * pplot->plint.height;
            bb -= deltaGrid * pplot->plint.height;
            frameRect.top = (LONG) round(tt); frameRect.bottom = (LONG) round(bb);

            bDrawLabel = ((paxis->gridcount % iMod) == 0);

            if (bDrawLabel) {
                if (paxis->scale == 0) {
                    _tcsprintf(szBuffer, (ML_STRSIZE - 1), (const char_t*) (paxis->format), axismax);
                }
                else {
                    if (axismax > 0) {
                        _tcsprintf(szBuffer, (ML_STRSIZE - 1), (axismax < 9) ? _T("1e+%01d") : _T("1e+%02d"), (int_t) axismax);
                    }
                    else if (axismax == 0) {
                        _tcscpy(szBuffer, _T("1e00"));
                    }
                    else {
                        _tcsprintf(szBuffer, (ML_STRSIZE - 1), (fabs(axismax) < 9) ? _T("1e-%01d") : _T("1e-%02d"), (int_t) fabs(axismax));
                    }
                }

                pl_reformat(szBuffer);
                nlen = (int_t) _tcslen(szBuffer);
                if (bItalic) {
                    szBuffer[nlen] = _T(' ');
                    szBuffer[nlen + 1] = _T('\0');
                    nlen += 1;
                }
                // Adapter � la taille du texte
                if (pplot->canvas->MeasureString(szBuffer, nlen, pFontLabel, ptF, &rcF) == Gdiplus::Status::Ok) {
                    if (iDelta > 0) {        // Right-Axis
                        frameRect.right = frameRect.left + (int_t) (rcF.Width);
                    }
                    else {                    // Left-Axis
                        frameRect.left = frameRect.right - (int_t) (rcF.Width);
                    }
                }

                if (iGridMax) {
                    if ((paxis->gridcount % 2) == 0) {
                        ptF.X = (Gdiplus::REAL)(frameRect.left);
                        ptF.Y = (Gdiplus::REAL)(((frameRect.top + frameRect.bottom) >> 1) - (int_t) (rcF.Height / 2.0f));
                        rcF.X = ptF.X;
                        rcF.Y = ptF.Y;
                        drawStringTX(pplot, (const char_t*) szBuffer, (int_t) _tcslen(szBuffer), pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);
                    }
                }
                else {
                    ptF.X = (Gdiplus::REAL)(frameRect.left);
                    ptF.Y = (Gdiplus::REAL)(((frameRect.top + frameRect.bottom) >> 1) - (int_t) (rcF.Height / 2.0f));
                    rcF.X = ptF.X;
                    rcF.Y = ptF.Y;
                    drawStringTX(pplot, (const char_t*) szBuffer, (int_t) _tcslen(szBuffer), pFontLabel, pBrushLabel, &rcF, 0x00, NULL, NULL);
                }
            }

            //    Position des différents rectangles        0:LABEL-BOTTOM        1:LABEL-LEFT    2:LABEL-TOP            3:LABEL-RIGHT
            pplot->plint.axislRect[iAxis].top = frameRect.top;
            if (iDelta > 0) {        // Right-Axis
                if (frameRect.right > pplot->plint.axislRect[iAxis].right) {
                    pplot->plint.axislRect[iAxis].right = frameRect.right;
                }
            }
            else {                    // Left-Axis
                if (frameRect.left < pplot->plint.axislRect[iAxis].left) {
                    pplot->plint.axislRect[iAxis].left = frameRect.left;
                }
            }
        }
    }

    // SVG
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        pplot->svgdoc->setStroke(svgStroke(1.0, svgColor((int_t) (fontcolorLabel.GetR()), (int_t) (fontcolorLabel.GetG()), (int_t) (fontcolorLabel.GetB()))));
    }

    // TITLE
    if (paxis->otitle == 1) {
        int_t nlen;
        real_t rr, ll, tt, bb;
        Gdiplus::Font *pFontTmp = NULL;

        if (isHorz) {
            tt = pplot->plint.height * paxis->trelpos.ya;
            bb = pplot->plint.height * paxis->trelpos.yb;
            ll = pplot->plint.width * paxis->trelpos.xa;
            rr = pplot->plint.width * paxis->trelpos.xb;
            frameRect.top = (LONG) round(tt);
            frameRect.bottom = (LONG) round(bb);
            frameRect.left = (LONG) round(ll);
            frameRect.right = (LONG) round(rr);

            nlen = (int_t)_tcslen(paxis->title.text);

            rcF.X = (Gdiplus::REAL) (frameRect.left);
            rcF.Y = (Gdiplus::REAL) (frameRect.top);
            rcF.Width = (Gdiplus::REAL) (frameRect.right - frameRect.left);
            rcF.Height = (Gdiplus::REAL) (frameRect.bottom - frameRect.top);
            drawStringTX(pplot, (const char_t*) (paxis->title.text), nlen, pFontTitle, pBrushTitle, &rcF, 0x01, &frameRect, pFontTitleSm);

            //    Position des différents rectangles        0:TITLE-BOTTOM        1:TITLE-LEFT    2:TITLE-TOP            3:TITLE-RIGHT
            pplot->plint.axistRect[iAxis].bottom = frameRect.bottom;
            pplot->plint.axistRect[iAxis].top = frameRect.top;
            pplot->plint.axistRect[iAxis].left = frameRect.left;
            pplot->plint.axistRect[iAxis].right = frameRect.right;
        }
        else { // Vert. Axis
            tt = pplot->plint.height * paxis->trelpos.ya;
            bb = pplot->plint.height * paxis->trelpos.yb;
            ll = pplot->plint.width * paxis->trelpos.xa;
            rr = pplot->plint.width * paxis->trelpos.xb;
            frameRect.top = (LONG) round(tt);
            frameRect.bottom = (LONG) round(bb);
            frameRect.left = (LONG) round(ll);
            frameRect.right = (LONG) round(rr);

            nlen = (int_t)_tcslen(paxis->title.text);

            rcF.X = (Gdiplus::REAL) (frameRect.left);
            rcF.Y = (Gdiplus::REAL) (frameRect.top);
            rcF.Width = (Gdiplus::REAL) (frameRect.right - frameRect.left);
            rcF.Height = (Gdiplus::REAL) (frameRect.bottom - frameRect.top);
            drawStringTX(pplot, (const char_t*) (paxis->title.text), nlen, pFontTitle, pBrushTitle, &rcF, 0x21, &frameRect, pFontTitleSm);

            //    Position des différents rectangles        0:TITLE-BOTTOM        1:TITLE-LEFT    2:TITLE-TOP            3:TITLE-RIGHT
            pplot->plint.axistRect[iAxis].bottom = frameRect.bottom;
            pplot->plint.axistRect[iAxis].top = frameRect.top;
            pplot->plint.axistRect[iAxis].left = frameRect.left;
            pplot->plint.axistRect[iAxis].right = frameRect.right;
        }
    }

    pplot->errcode = 0;

pl_label_axis_RET:
    if (pBrushLabel) { delete (pBrushLabel); pBrushLabel = NULL; }
    if (pBrushTitle) { delete (pBrushTitle); pBrushTitle = NULL; }
    if (pFontTitle) { delete (pFontTitle); pFontTitle = NULL; }
    if (pFontTitleSm) { delete (pFontTitleSm); pFontTitleSm = NULL; }
    if (pFontLabel) { delete (pFontLabel); pFontLabel = NULL; }
    return;
}

/*  --------------------------------------------------------------------
    pl_reset         :    r�-initialisation des données internes

        istatic      :    valider uniquement les données statiques
                         (et supprimer les données dynamiques)
        pplot        :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
void pl_reset(plot_t* pplot, byte_t istatic)
{
    int_t jj;

    if (pplot == NULL) {
        return;
    }

    pplot->errcode = 0;
    pplot->plint.running = 1;

    pplot->modpoints = PL_MODPOINTS_DEF;

    // Il faut donner le nombre de trac�s
    if (istatic == 0) {
        if (pplot->curvecount < 1) {
            _tcscpy(pplot->message, _T("Invalid curves."));
            pplot->errcode = 1;
            pplot->plint.running = 0;
            return;
        }
    }
    else {    // (istatic == 1) || (istatic == 2)
        pplot->dc = NULL;
        pplot->frame = NULL;
        pplot->title.font.ptr = NULL;
        pplot->curvecount = 0;
        pplot->linevertcount = 0;
        pplot->linehorzcount = 0;
        for (jj = 0; jj < ML_MAXCURVES; jj++) {
            pplot->curve[jj].points = 0;
            pplot->curve[jj].x = NULL;
            pplot->curve[jj].y = NULL;
            pplot->curve[jj].ex = NULL;
            pplot->curve[jj].ey = NULL;
            pplot->curve[jj].idx = 0;
            pplot->curve[jj].idy = 0;
            pplot->curve[jj].legend.font.ptr = NULL;
        }

        pplot->curve[0].dotcolor.r = 255;
        pplot->curve[0].dotcolor.g = 0;
        pplot->curve[0].dotcolor.b = 0;
        pplot->curve[0].linecolor.r = 0;
        pplot->curve[0].linecolor.g = 0;
        pplot->curve[0].linecolor.b = 255;

        pplot->curve[1].dotcolor.r = 192;
        pplot->curve[1].dotcolor.g = 0;
        pplot->curve[1].dotcolor.b = 0;
        pplot->curve[1].linecolor.r = 0;
        pplot->curve[1].linecolor.g = 0;
        pplot->curve[1].linecolor.b = 192;

        pplot->curve[2].dotcolor.r = 128;
        pplot->curve[2].dotcolor.g = 0;
        pplot->curve[2].dotcolor.b = 0;
        pplot->curve[2].linecolor.r = 0;
        pplot->curve[2].linecolor.g = 0;
        pplot->curve[2].linecolor.b = 128;
    }

    // Type
    pplot->type = PLOTTYPE_XY;

    // Texte, lignes, etc.
    pplot->textcount = 0;
    pplot->linecount = 0;
    pplot->rectanglecount = 0;
    pplot->ellipsecount = 0;
    for (jj = 0; jj < PL_MAXITEMS; jj++) {
        // Text
        pplot->text[jj].border = 0;
        _tcscpy(pplot->text[jj].font.name, _T("Tahoma"));
        pplot->text[jj].font.size = 10;
        pplot->text[jj].font.style = 0;
        pplot->text[jj].font.orient = 0;
        pplot->text[jj].font.color.r = 0;    pplot->text[jj].font.color.g = 0;    pplot->text[jj].font.color.b = 0;
        pplot->text[jj].font.ptr = NULL;
        pplot->text[jj].rect.xa = 0.0;
        pplot->text[jj].rect.xb = 0.0;
        pplot->text[jj].rect.ya = 0.0;
        pplot->text[jj].rect.yb = 0.0;
        pplot->text[jj].len = 0;
        pplot->text[jj].status = 0x00;
        memset(pplot->text[jj].text, 0, ML_STRSIZE);
        // Line GEO
        pplot->line[jj].color.r = 0;    pplot->line[jj].color.g = 0;    pplot->line[jj].color.b = 0;
        pplot->line[jj].rect.xa = 0.0;
        pplot->line[jj].rect.xb = 0.0;
        pplot->line[jj].rect.ya = 0.0;
        pplot->line[jj].rect.yb = 0.0;
        pplot->line[jj].arrlength = 0;
        pplot->line[jj].arrwidth = 0;
        pplot->line[jj].arrstyle = 0x00;
        pplot->line[jj].size = 1;
        pplot->line[jj].status = 0x00;
        pplot->line[jj].style = 0x00;                // ligne simple
        // Rectangle GEO
        pplot->rectangle[jj].color.r = 0;        pplot->rectangle[jj].color.g = 0;        pplot->rectangle[jj].color.b = 0;
        pplot->rectangle[jj].fcolor.r = 255;    pplot->rectangle[jj].fcolor.g = 255;    pplot->rectangle[jj].fcolor.b = 255;
        pplot->rectangle[jj].rect.xa = 0.0;
        pplot->rectangle[jj].rect.xb = 0.0;
        pplot->rectangle[jj].rect.ya = 0.0;
        pplot->rectangle[jj].rect.yb = 0.0;
        pplot->rectangle[jj].size = 1;
        pplot->rectangle[jj].status = 0x00;
        pplot->rectangle[jj].style = 0x00;        // rectangle sans remplissage
        // Ellipse GEO
        pplot->ellipse[jj].color.r = 0;        pplot->ellipse[jj].color.g = 0;        pplot->ellipse[jj].color.b = 0;
        pplot->ellipse[jj].fcolor.r = 255;    pplot->ellipse[jj].fcolor.g = 255;    pplot->ellipse[jj].fcolor.b = 255;
        pplot->ellipse[jj].rect.xa = 0.0;
        pplot->ellipse[jj].rect.xb = 0.0;
        pplot->ellipse[jj].rect.ya = 0.0;
        pplot->ellipse[jj].rect.yb = 0.0;
        pplot->ellipse[jj].size = 1;
        pplot->ellipse[jj].status = 0x00;
        pplot->ellipse[jj].style = 0x00;            // ellipse sans remplissage
    }

    // Frame
    pplot->dc = NULL;
    pplot->frame = NULL;
    pplot->backcolor.r = 255;        pplot->backcolor.g = 255;        pplot->backcolor.b = 255;
    pplot->pbackcolor.r = 255;        pplot->pbackcolor.g = 255;        pplot->pbackcolor.b = 255;
    pplot->title.font.ptr = NULL;
    _tcscpy(pplot->title.font.name, _T("Tahoma"));
    pplot->title.font.color.r = 0;    pplot->title.font.color.g = 0;    pplot->title.font.color.b = 0;
    pplot->title.font.orient = 0;
    pplot->title.font.size = 8;
    pplot->title.font.style = 0;
    pplot->otitle = 0;

    pl_reset_axis(pplot, 0);
    pl_reset_axis(pplot, 1);
    pl_reset_axis(pplot, 2);
    pl_reset_axis(pplot, 3);

    // Lines
    pplot->linevertcount = pplot->linehorzcount = 0;
    for (jj = 0; jj < PL_MAXITEMS; jj++) {
        // Vertical
        pplot->linevert[jj].size = 1;                pplot->linevert[jj].status = 0x00;
        pplot->linevert[jj].style = 0x10;
        pplot->linevert[jj].color.r = 255;        pplot->linevert[jj].color.g = 0;        pplot->linevert[jj].color.b = 0;
        pplot->linevert[jj].rect.xa = pplot->linevert[jj].rect.xb = pplot->linevert[jj].rect.ya = pplot->linevert[jj].rect.yb = 0.0;

        // Horizontal
        pplot->linehorz[jj].size = 1;                pplot->linehorz[jj].status = 0x00;
        pplot->linehorz[jj].style = 0x10;
        pplot->linehorz[jj].color.r = 255;        pplot->linehorz[jj].color.g = 0;        pplot->linehorz[jj].color.b = 0;
        pplot->linehorz[jj].rect.xa = pplot->linehorz[jj].rect.xb = pplot->linehorz[jj].rect.ya = pplot->linehorz[jj].rect.yb = 0.0;
    }

    // Positions

    // RECT WND
    pplot->relpos[0].xa = 0.00; pplot->relpos[0].xb = 1.00; pplot->relpos[0].ya = 0.00; pplot->relpos[0].yb = 1.00;

    // RECT PLOT
    pplot->relpos[1].xa = 0.14; pplot->relpos[1].xb = 0.86;
    pplot->relpos[1].ya = 0.14; pplot->relpos[1].yb = 0.86;

    // RECT TITLE
    pplot->relpos[2].xa = 0.14; pplot->relpos[2].xb = 0.86;
    pplot->relpos[2].ya = 0.01; pplot->relpos[2].yb = 0.04;

    for (jj = 0; jj < ML_MAXCURVES; jj++) {
        pl_reset_curve(pplot, jj, 1);
    }

    pplot->curveactive = 0;

    // Axis link
    memset(pplot->linktop, 0, ML_STRSIZE * sizeof(char_t));
    memset(pplot->linkright, 0, ML_STRSIZE * sizeof(char_t));

    pplot->plint.running = 0;
    return;
}

/*  --------------------------------------------------------------------
    pl_validate    :    validation les données

        pplot      :    pointeur sur la structure du tracé
        istatic    :    valider uniquement les données statiques
    -------------------------------------------------------------------- */
void pl_validate(plot_t* pplot, byte_t istatic)
{
    int_t visibleCurve, jj, ierr = 0, errcount, nlen, linkAxis, topaxisCurve;
    irect_t recFrame = {0,0,0,0};
    real_t fdx, fdy;
    long_t iDelete[ML_MAXCURVES];
    int_t nDel = 0;
    int_t iX = 0, iX2 = 0, iY = 0, iY2 = 0;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    pplot->errcode = 1;

    if (pplot->modpoints == 0) {
        pplot->modpoints = PL_MODPOINTS_DEF;
    }

    if (pplot->modpoints < PL_MODPOINTS_MIN) {
        pplot->modpoints = PL_MODPOINTS_MIN;
    }
    if (pplot->modpoints > PL_MODPOINTS_MAX) {
        pplot->modpoints = PL_MODPOINTS_MAX;
    }

    if (istatic == 0) {
        if ((pplot->dc == NULL) && (pplot->frame == NULL)) {
            _tcscpy(pplot->message, _T("Invalid plot frame"));
            return;
        }

        // Si dc est fourni alors pplot->plint.width et pplot->plint.height sont aussi fournis
        if (pplot->frame != NULL) {
            GetClientRect(pplot->frame, &recFrame);
            pplot->plint.width = (real_t) (recFrame.right - recFrame.left);
            pplot->plint.height = (real_t) (recFrame.bottom - recFrame.top);
        }

        if (pplot->curvecount == 0) {
            _tcscpy(pplot->message, _T("Empty plot"));
            goto pl_validate_RET;
        }

        if ((pplot->curvecount < 1) || (pplot->curvecount > ML_MAXCURVES)) {
            _tcscpy(pplot->message, _T("Invalid number of curves"));
            goto pl_validate_RET;
        }

        // Lines
        if ((pplot->linevertcount < 1) || (pplot->linevertcount > PL_MAXITEMS)) {
            pplot->linevertcount = 0;
            for (jj = 0; jj < PL_MAXITEMS; jj++) {
                pplot->linevert[jj].status |= 0x0F;        // Lines not visible
            }
        }
        if ((pplot->linehorzcount < 1) || (pplot->linehorzcount > PL_MAXITEMS)) {
            pplot->linehorzcount = 0;
            for (jj = 0; jj < PL_MAXITEMS; jj++) {
                pplot->linehorz[jj].status |= 0x0F;        // Lines not visible
            }
        }
    }
    else {    // istatic == 1
        pplot->dc = NULL;
        pplot->frame = NULL;
        pplot->imagetype = 0;
        pplot->svgdoc = NULL;
        pplot->imagesaved = false;
        pplot->image = NULL;
        pplot->metafile = NULL;
        pplot->canvas = NULL;
        setPenTX(pplot, NULL);
        pplot->brush = NULL;
        pplot->title.font.ptr = NULL;
        pplot->axisbottom.title.font.ptr = NULL;
        pplot->axisleft.title.font.ptr = NULL;
        pplot->axistop.title.font.ptr = NULL;
        pplot->axisright.title.font.ptr = NULL;
        pplot->axisbottom.lfont.ptr = NULL;
        pplot->axisleft.lfont.ptr = NULL;
        pplot->axistop.lfont.ptr = NULL;
        pplot->axisright.lfont.ptr = NULL;
        pplot->curvecount = 0;
        pplot->curveactive = 0;
        for (jj = 0; jj < ML_MAXCURVES; jj++) {
            pplot->curve[jj].points = 0;
            pplot->curve[jj].x = NULL;
            pplot->curve[jj].y = NULL;
            pplot->curve[jj].ex = NULL;
            pplot->curve[jj].ey = NULL;
            pplot->curve[jj].idx = 0;
            pplot->curve[jj].idy = 0;
            pplot->curve[jj].legend.font.ptr = NULL;
        }
    }

    // Axis link
    linkAxis = 0;
    nlen = 0;

    // Les plots doivent �tre valides s'ils sont visibles
    topaxisCurve = 0;    visibleCurve = 0;    ierr = 0;
    iX = 0;    iX2 = 0;    iY = 0;    iY2 = 0;
    if (istatic == 0) {
        for (jj = 0; jj < ML_MAXCURVES; jj++) {
            iDelete[jj] = 0L;
        }
        for (jj = 0; jj < pplot->curvecount; jj++) {
            if ((pplot->curve[jj].y->idx != pplot->curve[jj].x->id) || ((pplot->curve[jj].y->type & 0xF0) != 0x20)) {
                iDelete[nDel] = pplot->curve[jj].idy;
                nDel += 1;
                continue;
            }

            if ((pplot->curve[jj].status & 0x0F) != 0x0F) {        // Visible curve
                if (pplot->curve[jj].x == NULL) {
                    pplot->curve[jj].points = 0;
                    _tcscpy(pplot->message, _T("Invalid curve data"));
                    ierr = 1;
                }
                else {
                    if (pplot->curve[jj].x->dat == NULL) {
                        pplot->curve[jj].points = 0;
                        _tcscpy(pplot->message, _T("Invalid curve data"));
                        ierr = 1;
                    }
                }
                if (pplot->curve[jj].y == NULL) {
                    pplot->curve[jj].points = 0;
                    _tcscpy(pplot->message, _T("Invalid curve data"));
                    ierr = 1;
                }
                else {
                    if (pplot->curve[jj].y->dat == NULL) {
                        pplot->curve[jj].points = 0;
                        _tcscpy(pplot->message, _T("Invalid curve data"));
                        ierr = 1;
                    }
                }
                if (ierr == 0) {
                    // >>
                    pplot->curve[jj].points = pplot->curve[jj].y->dim;
                    if ((pplot->curve[jj].y->opt & 0xF0) == 0x10) {
                        _tcscpy(pplot->curve[jj].legend.text, (const char_t*)(pplot->curve[jj].y->name));
                    }
                    else {
                        _tcscpy(pplot->curve[jj].legend.text, (const char_t*)(pplot->curve[jj].y->label));
                    }
                    _tcscat(pplot->curve[jj].legend.text, _T(" "));
                    if ((pplot->curve[jj].points < 1) || (pplot->curve[jj].points > ML_MAXPOINTS)
                        || (pplot->curve[jj].points != pplot->curve[jj].x->dim)) {
                        _tcscpy(pplot->message, _T("Invalid curve data"));
                        ierr = 1;
                    }
                    // <<
                    if (ierr == 0) {
                        if ((pplot->curve[jj].axisused & 0xF0) == 0x00) {                // X-Axis
                            iX += 1;
                        }
                        else if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {        // X2-Axis
                            iX2 += 1;
                        }
                        if ((pplot->curve[jj].axisused & 0x0F) == 0x00) {                // Y-Axis
                            iY += 1;
                        }
                        else if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {        // Y2-Axis
                            iY2 += 1;
                        }
                    }
                }
                if (pplot->curve[jj].axisused == 0x11) {
                    topaxisCurve = 1;
                }
                visibleCurve += 1;
            }
        }
        //pl_curve_add
        if (nDel > 0) {
            for (jj = 0; jj < nDel; jj++) {
                pl_curve_removei(pplot, iDelete[jj]);
            }
        }
    }

    // Axis
    errcount = 0;
    errcount += pl_validate_axis(pplot, 0);
    errcount += pl_validate_axis(pplot, 1);
    errcount += pl_validate_axis(pplot, 2);
    errcount += pl_validate_axis(pplot, 3);
    if (errcount > 0) {
        _tcscpy(pplot->message, _T("Invalid axis properties"));
        return;
    }

    // RECTANGLES
    if (istatic == 0) {
        if ((pplot->plint.width == 0.0) || (pplot->plint.height == 0.0)) {
            goto pl_validate_RET;
        }

        // Sauvegarder la position
        /*    Position des différents rectangles
            0:WND, 1:PLOT, 2:TITLE */
        pplot->plint.frameRect[0].bottom = recFrame.bottom;
        pplot->plint.frameRect[0].top = recFrame.top;
        pplot->plint.frameRect[0].left = recFrame.left;
        pplot->plint.frameRect[0].right = recFrame.right;

        if (pplot->textcount > 0) {
            for (jj = 0; jj < pplot->textcount; jj++) {
                fdx = pplot->text[jj].rect.xb - pplot->text[jj].rect.xa;
                if ((fdx <= 0.001) || (fdx >= 1.0)) { fdx = 0.05; }
                fdy = pplot->text[jj].rect.yb - pplot->text[jj].rect.ya;
                if ((fdy <= 0.001) || (fdy >= 1.0)) { fdy = 0.05; }

                if (pplot->text[jj].rect.xa < 0.0) { pplot->text[jj].rect.xa = 0.0; pplot->text[jj].rect.xb = fdx; }
                if (pplot->text[jj].rect.xa > 1.0) { pplot->text[jj].rect.xb = 1.0; pplot->text[jj].rect.xa = 1.0 - fdx; }
                if (pplot->text[jj].rect.xb < 0.0) { pplot->text[jj].rect.xa = 0.0; pplot->text[jj].rect.xb = fdx; }
                if (pplot->text[jj].rect.xb > 1.0) { pplot->text[jj].rect.xb = 1.0; pplot->text[jj].rect.xa = 1.0 - fdx; }
                if (pplot->text[jj].rect.ya < 0.0) { pplot->text[jj].rect.ya = 0.0; pplot->text[jj].rect.yb = fdy; }
                if (pplot->text[jj].rect.ya > 1.0) { pplot->text[jj].rect.yb = 1.0; pplot->text[jj].rect.ya = 1.0 - fdy; }
                if (pplot->text[jj].rect.yb < 0.0) { pplot->text[jj].rect.ya = 0.0; pplot->text[jj].rect.yb = fdy; }
                if (pplot->text[jj].rect.yb > 1.0) { pplot->text[jj].rect.yb = 1.0; pplot->text[jj].rect.ya = 1.0 - fdy; }
            }
        }
        if (pplot->rectanglecount > 0) {
            for (jj = 0; jj < pplot->rectanglecount; jj++) {
                fdx = pplot->rectangle[jj].rect.xb - pplot->rectangle[jj].rect.xa;
                if ((fdx <= 0.001) || (fdx >= 1.0)) { fdx = 0.05; }
                fdy = pplot->rectangle[jj].rect.yb - pplot->rectangle[jj].rect.ya;
                if ((fdy <= 0.001) || (fdy >= 1.0)) { fdy = 0.05; }

                if (pplot->rectangle[jj].rect.xa < 0.0) { pplot->rectangle[jj].rect.xa = 0.0; pplot->rectangle[jj].rect.xb = fdx; }
                if (pplot->rectangle[jj].rect.xa > 1.0) { pplot->rectangle[jj].rect.xb = 1.0; pplot->rectangle[jj].rect.xa = 1.0 - fdx; }
                if (pplot->rectangle[jj].rect.xb < 0.0) { pplot->rectangle[jj].rect.xa = 0.0; pplot->rectangle[jj].rect.xb = fdx; }
                if (pplot->rectangle[jj].rect.xb > 1.0) { pplot->rectangle[jj].rect.xb = 1.0; pplot->rectangle[jj].rect.xa = 1.0 - fdx; }
                if (pplot->rectangle[jj].rect.ya < 0.0) { pplot->rectangle[jj].rect.ya = 0.0; pplot->rectangle[jj].rect.yb = fdy; }
                if (pplot->rectangle[jj].rect.ya > 1.0) { pplot->rectangle[jj].rect.yb = 1.0; pplot->rectangle[jj].rect.ya = 1.0 - fdy; }
                if (pplot->rectangle[jj].rect.yb < 0.0) { pplot->rectangle[jj].rect.ya = 0.0; pplot->rectangle[jj].rect.yb = fdy; }
                if (pplot->rectangle[jj].rect.yb > 1.0) { pplot->rectangle[jj].rect.yb = 1.0; pplot->rectangle[jj].rect.ya = 1.0 - fdy; }
            }
        }
        if (pplot->ellipsecount > 0) {
            for (jj = 0; jj < pplot->ellipsecount; jj++) {
                fdx = pplot->ellipse[jj].rect.xb - pplot->ellipse[jj].rect.xa;
                if ((fdx <= 0.001) || (fdx >= 1.0)) { fdx = 0.05; }
                fdy = pplot->ellipse[jj].rect.yb - pplot->ellipse[jj].rect.ya;
                if ((fdy <= 0.001) || (fdy >= 1.0)) { fdy = 0.05; }

                if (pplot->ellipse[jj].rect.xa < 0.0) { pplot->ellipse[jj].rect.xa = 0.0; pplot->ellipse[jj].rect.xb = fdx; }
                if (pplot->ellipse[jj].rect.xa > 1.0) { pplot->ellipse[jj].rect.xb = 1.0; pplot->ellipse[jj].rect.xa = 1.0 - fdx; }
                if (pplot->ellipse[jj].rect.xb < 0.0) { pplot->ellipse[jj].rect.xa = 0.0; pplot->ellipse[jj].rect.xb = fdx; }
                if (pplot->ellipse[jj].rect.xb > 1.0) { pplot->ellipse[jj].rect.xb = 1.0; pplot->ellipse[jj].rect.xa = 1.0 - fdx; }
                if (pplot->ellipse[jj].rect.ya < 0.0) { pplot->ellipse[jj].rect.ya = 0.0; pplot->ellipse[jj].rect.yb = fdy; }
                if (pplot->ellipse[jj].rect.ya > 1.0) { pplot->ellipse[jj].rect.yb = 1.0; pplot->ellipse[jj].rect.ya = 1.0 - fdy; }
                if (pplot->ellipse[jj].rect.yb < 0.0) { pplot->ellipse[jj].rect.ya = 0.0; pplot->ellipse[jj].rect.yb = fdy; }
                if (pplot->ellipse[jj].rect.yb > 1.0) { pplot->ellipse[jj].rect.yb = 1.0; pplot->ellipse[jj].rect.ya = 1.0 - fdy; }
            }
        }
        if (pplot->linecount > 0) {
            for (jj = 0; jj < pplot->linecount; jj++) {
                fdx = pplot->line[jj].rect.xb - pplot->line[jj].rect.xa;
                if ((fdx <= 0.001) || (fdx >= 1.0)) { fdx = 0.05; }
                fdy = pplot->line[jj].rect.yb - pplot->line[jj].rect.ya;
                if ((fdy <= 0.001) || (fdy >= 1.0)) { fdy = 0.05; }

                if (pplot->line[jj].rect.xa < 0.0) { pplot->line[jj].rect.xa = 0.0; pplot->line[jj].rect.xb = fdx; }
                if (pplot->line[jj].rect.xa > 1.0) { pplot->line[jj].rect.xb = 1.0; pplot->line[jj].rect.xa = 1.0 - fdx; }
                if (pplot->line[jj].rect.xb < 0.0) { pplot->line[jj].rect.xa = 0.0; pplot->line[jj].rect.xb = fdx; }
                if (pplot->line[jj].rect.xb > 1.0) { pplot->line[jj].rect.xb = 1.0; pplot->line[jj].rect.xa = 1.0 - fdx; }
                if (pplot->line[jj].rect.ya < 0.0) { pplot->line[jj].rect.ya = 0.0; pplot->line[jj].rect.yb = fdy; }
                if (pplot->line[jj].rect.ya > 1.0) { pplot->line[jj].rect.yb = 1.0; pplot->line[jj].rect.ya = 1.0 - fdy; }
                if (pplot->line[jj].rect.yb < 0.0) { pplot->line[jj].rect.ya = 0.0; pplot->line[jj].rect.yb = fdy; }
                if (pplot->line[jj].rect.yb > 1.0) { pplot->line[jj].rect.yb = 1.0; pplot->line[jj].rect.ya = 1.0 - fdy; }
            }
        }

        for (jj = 0; jj < PL_FRAMEITEMS; jj++) {
            fdx = pplot->relpos[jj].xb - pplot->relpos[jj].xa;
            if ((fdx <= 0.001) || (fdx >= 1.0)) { fdx = 0.05; }
            fdy = pplot->relpos[jj].yb - pplot->relpos[jj].ya;
            if ((fdy <= 0.001) || (fdy >= 1.0)) { fdy = 0.05; }

            if (pplot->relpos[jj].xa < 0.0) { pplot->relpos[jj].xa = 0.0; pplot->relpos[jj].xb = fdx; }
            if (pplot->relpos[jj].xa > 1.0) { pplot->relpos[jj].xb = 1.0; pplot->relpos[jj].xa = 1.0 - fdx; }
            if (pplot->relpos[jj].xb < 0.0) { pplot->relpos[jj].xa = 0.0; pplot->relpos[jj].xb = fdx; }
            if (pplot->relpos[jj].xb > 1.0) { pplot->relpos[jj].xb = 1.0; pplot->relpos[jj].xa = 1.0 - fdx; }
            if (pplot->relpos[jj].ya < 0.0) { pplot->relpos[jj].ya = 0.0; pplot->relpos[jj].yb = fdy; }
            if (pplot->relpos[jj].ya > 1.0) { pplot->relpos[jj].yb = 1.0; pplot->relpos[jj].ya = 1.0 - fdy; }
            if (pplot->relpos[jj].yb < 0.0) { pplot->relpos[jj].ya = 0.0; pplot->relpos[jj].yb = fdy; }
            if (pplot->relpos[jj].yb > 1.0) { pplot->relpos[jj].yb = 1.0; pplot->relpos[jj].ya = 1.0 - fdy; }
        }
    
        fdx = pplot->curve[0].lrelpos.xb - pplot->curve[0].lrelpos.xa;
        if ((fdx <= 0.001) || (fdx >= 1.0)) { fdx = 0.05; }
        if (pplot->curve[0].lrelpos.xa < 0.0) { pplot->curve[0].lrelpos.xa = 0.0; pplot->curve[0].lrelpos.xb = fdx; }
        if (pplot->curve[0].lrelpos.xa > 1.0) { pplot->curve[0].lrelpos.xb = 1.0; pplot->curve[0].lrelpos.xa = 1.0 - fdx; }
        if (pplot->curve[0].lrelpos.xb < 0.0) { pplot->curve[0].lrelpos.xa = 0.0; pplot->curve[0].lrelpos.xb = fdx; }
        if (pplot->curve[0].lrelpos.xb > 1.0) { pplot->curve[0].lrelpos.xb = 1.0; pplot->curve[0].lrelpos.xa = 1.0 - fdx; }
        fdy = pplot->curve[0].lrelpos.yb - pplot->curve[0].lrelpos.ya;
        if ((fdy <= 0.001) || (fdy >= 1.0)) { fdy = 0.05; }
        if (pplot->curve[0].lrelpos.ya < 0.0) { pplot->curve[0].lrelpos.ya = 0.0; pplot->curve[0].lrelpos.yb = fdy; }
        if (pplot->curve[0].lrelpos.ya > 1.0) { pplot->curve[0].lrelpos.yb = 1.0; pplot->curve[0].lrelpos.ya = 1.0 - fdy; }
        if (pplot->curve[0].lrelpos.yb < 0.0) { pplot->curve[0].lrelpos.ya = 0.0; pplot->curve[0].lrelpos.yb = fdy; }
        if (pplot->curve[0].lrelpos.yb > 1.0) { pplot->curve[0].lrelpos.yb = 1.0; pplot->curve[0].lrelpos.ya = 1.0 - fdy; }

        for (jj = 0; jj < pplot->curvecount; jj++) {
            if (jj > 0) {
                pplot->curve[jj].lrelpos.xa = pplot->curve[0].lrelpos.xa;
                pplot->curve[jj].lrelpos.xb = pplot->curve[0].lrelpos.xb;
            }
            if (pplot->curve[jj].dotstyle == 0x00) {
                pplot->curve[jj].dropstyleh = pplot->curve[jj].dropstylev = 0x00;
            }
        }

        pplot->plint.dbottom = (pplot->relpos[1].xb - pplot->relpos[1].xa) / (real_t) (pplot->axisbottom.gridcount);
        pplot->plint.dleft = (pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (pplot->axisleft.gridcount);
        pplot->plint.dtop = (pplot->relpos[1].xb - pplot->relpos[1].xa) / (real_t) (pplot->axistop.gridcount);
        pplot->plint.dright = (pplot->relpos[1].yb - pplot->relpos[1].ya) / (real_t) (pplot->axisright.gridcount);

        pplot->plint.dmbottom = pplot->plint.dbottom / (real_t)pplot->axisbottom.gridmcount;
        pplot->plint.dmleft = pplot->plint.dleft / (real_t) (pplot->axisleft.gridmcount);
        pplot->plint.dmtop = pplot->plint.dtop / (real_t)pplot->axistop.gridmcount;
        pplot->plint.dmright = pplot->plint.dright/ (real_t) (pplot->axisright.gridmcount);
    }

    // Aucune courbe visible
    if (istatic == 0) {
        if (visibleCurve == 0) {
            _tcscpy(pplot->message, _T("Nothing to plot"));
            goto pl_validate_RET;
        }
    }

    pplot->errcode = ierr;

pl_validate_RET:
    if ((istatic == 0) && (pplot->errcode != 0)) {
        ierr = pplot->errcode;
        pplot->errcode = 0;
        try {
            if (pplot->canvas) { delete(pplot->canvas); pplot->canvas = NULL; }
            if (pplot->svgdoc) { delete(pplot->svgdoc); pplot->svgdoc = NULL; }
            if (pplot->metafile) { delete(pplot->metafile); pplot->metafile = NULL; }
            if (pplot->image) { delete(pplot->image); pplot->image = NULL; }

            try {
                pplot->canvas = new Gdiplus::Graphics(pplot->dc);
            }
            catch (...) {
                pplot->canvas = NULL;
                throw;
            }

            pplot->canvas->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            pplot->canvas->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
            // EMF
            if (pplot->imagetype == 1) {

                try {
                    pplot->metafile = new Gdiplus::Metafile((const char_t*) (pplot->imagefilename), pplot->dc, Gdiplus::EmfTypeEmfPlusDual);
                }
                catch (...) {
                    pplot->metafile = NULL;
                    throw;
                }

                try {
                    pplot->image = new Gdiplus::Graphics(pplot->metafile);
                }
                catch (...) {
                    pplot->image = NULL;
                    throw;
                }

                pplot->image->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
                pplot->image->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
            }
            // SVG
            else if (pplot->imagetype == 2) {
                std::wstring strFilename = (const char_t*) (pplot->imagefilename);
                try {
                    pplot->svgdoc = new svgDocument(strFilename, svgLayout(pplot->plint.width, pplot->plint.height));
                }
                catch (...) {
                    pplot->svgdoc = NULL;
                    throw;
                }
            }

            pl_draw_frame(pplot);
            pl_grid(pplot, iX, iX2, iY, iY2, 0, 0);
            pl_title(pplot);
            pl_draw_rect(pplot);
            pl_draw_ellipse(pplot);
            pl_draw_line(pplot);
            pl_print_text(pplot);
        }
        catch (...) {
            if (pplot->canvas) { delete(pplot->canvas); pplot->canvas = NULL; }
            if (pplot->svgdoc) { delete(pplot->svgdoc); pplot->svgdoc = NULL; }
            if (pplot->metafile) { delete(pplot->metafile); pplot->metafile = NULL; }
            if (pplot->image) { delete(pplot->image); pplot->image = NULL; }
        }

        if (pplot->canvas) { delete(pplot->canvas); pplot->canvas = NULL; }
        if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
            pplot->svgdoc->save();
        }
        if (pplot->svgdoc) { delete(pplot->svgdoc); pplot->svgdoc = NULL; }
        if (pplot->metafile) { delete(pplot->metafile); pplot->metafile = NULL; }
        if (pplot->image) { delete(pplot->image); pplot->image = NULL; }
        pplot->imagetype = 0;
        pplot->errcode = ierr;
    }
    return;
}

/*  --------------------------------------------------------------------
    pl_draw_frame    :    couleurs du cadre

    pplot            :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
static void pl_draw_frame(plot_t* pplot)
{
    Gdiplus::SolidBrush *brBackground = NULL, *brPBackground = NULL;
    Gdiplus::Color backcolor, pbackcolor;

    // Sauvegarder la position des différents rectangles        0:WND, 1:PLOT, 2:TITLE 
    pplot->plint.frameRect[0].bottom = (LONG) round(pplot->plint.height * pplot->relpos[0].yb);
    pplot->plint.frameRect[0].top = (LONG) round(pplot->plint.height * pplot->relpos[0].ya);
    pplot->plint.frameRect[0].left = (LONG) round(pplot->plint.width * pplot->relpos[0].xa);
    pplot->plint.frameRect[0].right = (LONG) round(pplot->plint.width * pplot->relpos[0].xb);
    pplot->plint.frameRect[1].bottom = (LONG) round(pplot->plint.height * pplot->relpos[1].yb);
    pplot->plint.frameRect[1].top = (LONG) round(pplot->plint.height * pplot->relpos[1].ya);
    pplot->plint.frameRect[1].left = (LONG) round(pplot->plint.width * pplot->relpos[1].xa);
    pplot->plint.frameRect[1].right = (LONG) round(pplot->plint.width * pplot->relpos[1].xb);

    backcolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, pplot->backcolor.r, pplot->backcolor.g, pplot->backcolor.b));
    pbackcolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, pplot->pbackcolor.r, pplot->pbackcolor.g, pplot->pbackcolor.b));

    try {
        brBackground = new Gdiplus::SolidBrush(backcolor);
    }
    catch (...) {
        brBackground = NULL;
        goto pl_draw_frame_RET;
    }

    try {
        brPBackground = new Gdiplus::SolidBrush(pbackcolor);
    }
    catch (...) {
        brPBackground = NULL;
        goto pl_draw_frame_RET;
    }

    pplot->brush = brBackground;
    fillRectangleTX(pplot, (real_t) (pplot->plint.frameRect[0].left), (real_t) (pplot->plint.frameRect[0].top),
        (real_t) (pplot->plint.frameRect[0].right), (real_t) (pplot->plint.frameRect[0].bottom), &backcolor, false);

    pplot->brush = brPBackground;
    fillRectangleTX(pplot, (real_t) (pplot->plint.frameRect[1].left), (real_t) (pplot->plint.frameRect[1].top),
        (real_t) (pplot->plint.frameRect[1].right), (real_t) (pplot->plint.frameRect[1].bottom), &pbackcolor, false);

pl_draw_frame_RET:
    if (brBackground) { delete(brBackground); brBackground = NULL; }
    if (brPBackground) { delete(brPBackground); brPBackground = NULL; }
    pplot->brush = NULL;
}

/*  --------------------------------------------------------------------
    pl_grid            :    Grid, Tick et Label

        pplot          :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
static void pl_grid(plot_t* pplot, int_t iX, int_t iX2, int_t iY, int_t iY2, byte_t intX, byte_t intX2)
{
    // Grid and Tick Axis
    if ((pplot->axisbottom.ogrid != 0) || (pplot->axisbottom.otick != 0)) {
        if (intX > 0) {
            if (pplot->axisbottom.scale == 0) {
                double fstep = 0.1;
                int igrid = 5;
                double fmin = pplot->axisbottom.min, fmax = pplot->axisbottom.max;
                pl_autotick(&fmin, &fmax, &fstep, &igrid, pplot->axisbottom.scale);
                pplot->axisbottom.min = fmin;
                pplot->axisbottom.max = fmax;
                pplot->axisbottom.increment = fstep;
                pplot->axisbottom.gridcount = igrid;
                pplot->axisbottom.gridmcount = 1;
            }
        }
        pl_grid_axis(pplot, 0, iX, iX2, iY, iY2);
    }
    if ((pplot->axisleft.ogrid != 0) || (pplot->axisleft.otick != 0)) {
        pl_grid_axis(pplot, 1, iX, iX2, iY, iY2);
    }
    if ((pplot->axistop.ogrid != 0) || (pplot->axistop.otick != 0)) {
        if (intX2 > 0) {
            double fstep = 0.1;
            int igrid = 5;
            double fmin = pplot->axistop.min, fmax = pplot->axistop.max;
            pl_autotick(&fmin, &fmax, &fstep, &igrid, pplot->axistop.scale);
            pplot->axistop.min = fmin;
            pplot->axistop.max = fmax;
            pplot->axistop.increment = fstep;
            pplot->axistop.gridcount = igrid;
            pplot->axistop.gridmcount = 1;
        }
        pl_grid_axis(pplot, 2, iX, iX2, iY, iY2);
    }
    if ((pplot->axisright.ogrid != 0) || (pplot->axisright.otick != 0)) {
        pl_grid_axis(pplot, 3, iX, iX2, iY, iY2);
    }

    pl_line(pplot, iX, iX2, iY, iY2);

    byte_t iBsize = pplot->axisbottom.size,
        iLsize = pplot->axisleft.size,
        iTsize = pplot->axistop.size,
        iRsize = pplot->axisright.size;
    color_t *pBclr = &(pplot->axisbottom.color),
        *pLclr = &(pplot->axisleft.color),
        *pTclr = &(pplot->axistop.color),
        *pRclr = &(pplot->axisright.color);

    if (((pplot->axisbottom.status & 0x0F) == 0x00) && ((pplot->axisleft.status & 0x0F) == 0x00)
        && ((pplot->axistop.status & 0x0F) == 0x00) && ((pplot->axisright.status & 0x0F) == 0x00)
        && (iBsize == iLsize) && (iLsize == iTsize) && (iTsize == iRsize)
        && (pBclr->r == pLclr->r) && (pLclr->r == pTclr->r) && (pTclr->r == pRclr->r)
        && (pBclr->r == pLclr->g) && (pLclr->r == pTclr->g) && (pTclr->r == pRclr->g)
        && (pBclr->r == pLclr->b) && (pLclr->r == pTclr->b) && (pTclr->r == pRclr->b)) {

        // Draw Frame

        Gdiplus::Color acolor(pBclr->r, pBclr->g, pBclr->b);

        Gdiplus::Pen *penLine = NULL;

        try {
            penLine = new Gdiplus::Pen(acolor);
        }
        catch (...) {
            penLine = NULL;
            return;
        }

        penLine->SetWidth((Gdiplus::REAL)(iBsize));
        penLine->SetLineJoin(Gdiplus::LineJoin::LineJoinRound);

        setPenTX(pplot, &penLine);

        drawRectangleTX(pplot, (real_t) (pplot->plint.frameRect[1].left), (real_t) (pplot->plint.frameRect[1].top),
            (real_t) (pplot->plint.frameRect[1].right), (real_t) (pplot->plint.frameRect[1].bottom));

        if (penLine != NULL) {
            delete penLine;
            penLine = NULL;
        }
    }
    else {
        pl_draw_axis(pplot, 0);
        pl_draw_axis(pplot, 2);
        pl_draw_axis(pplot, 1);
        pl_draw_axis(pplot, 3);
    }

    // Label Axis
    if ((pplot->axisbottom.olabel != 0) || (pplot->axisbottom.otitle != 0)) {
        pl_label_axis(pplot, 0);
    }
    if ((pplot->axisleft.olabel != 0) || (pplot->axisleft.otitle != 0)) {
        pl_label_axis(pplot, 1);
    }
    if ((pplot->axistop.olabel != 0) || (pplot->axistop.otitle != 0)) {
        pl_label_axis(pplot, 2);
    }
    if ((pplot->axisright.olabel != 0) || (pplot->axisright.otitle != 0)) {
        pl_label_axis(pplot, 3);
    }
}

/*  --------------------------------------------------------------------
    pl_line        :    Ajout de lignes verticales et horizontales

        pplot      :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
static void pl_line(plot_t* pplot, int_t iX, int_t iX2, int_t iY, int_t iY2)
{
    irect_t recPlot = {0,0,0,0}, recFrame = {0,0,0,0}, frameRect = {0,0,0,0};
    byte_t jj;
    real_t fx0, fy0, fx, fxx, fy, fyy, fw, fh;
    int_t linescount, nPenStyle;
    real_t wx, wy, xrange, yrange, ftmp, xmin, xmax, ymin, ymax, xa, ya, xb, yb;
    axis_t *paxisX = NULL, *paxisY = NULL;

    Gdiplus::Pen *penLine = NULL;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    if ((pplot->linevertcount < 1) && (pplot->linehorzcount < 1)) {
        return;
    }

    linescount = 0;
    if (pplot->linevertcount > 0) {
        for (jj = 0; jj < pplot->linevertcount; jj++) {
            if ((pplot->linevert[jj].status & 0x0F) != 0x0F) {
                linescount += 1;
                break;
            }
        }
    }
    if (pplot->linehorzcount > 0) {
        for (jj = 0; jj < pplot->linehorzcount; jj++) {
            if ((pplot->linehorz[jj].status & 0x0F) != 0x0F) {
                linescount += 1;
                break;
            }
        }
    }
    if (linescount < 1) { return; }

    if (iX > 0) {
        paxisX = &(pplot->axisbottom);
    }
    else {
        paxisX = &(pplot->axistop);
    }

    if (iY > 0) {
        paxisY = &(pplot->axisleft);
    }
    else {
        paxisY = &(pplot->axisright);
    }

    if (paxisX->scale == 0) {
        xmin = paxisX->min;
        xmax = paxisX->max;
    }
    else {
        pl_logscale(paxisX->min, paxisX->max, &xmin, &xmax, &(paxisX->gridcount));
    }
    if (paxisY->scale == 0) {
        ymin = paxisY->min;
        ymax = paxisY->max;
    }
    else {
        pl_logscale(paxisY->min, paxisY->max, &ymin, &ymax, &(paxisY->gridcount));
    }

    pplot->errcode = 1;
    xrange = xmax - xmin;
    yrange = ymax - ymin;

    wx = pplot->plint.width * (pplot->relpos[1].xb - pplot->relpos[1].xa) / xrange;
    wy = pplot->plint.height * (pplot->relpos[1].yb - pplot->relpos[1].ya) / yrange;

    fw = (pplot->plint.width * (pplot->relpos[1].xb - pplot->relpos[1].xa));
    fh = (pplot->plint.height * (pplot->relpos[1].yb - pplot->relpos[1].ya));

    fx0 = (pplot->plint.width * pplot->relpos[1].xa);
    fy0 = (pplot->plint.height * pplot->relpos[1].ya);

    // VERTICAL LINES
    if (pplot->linevertcount > 0) {
        for (jj = 0; jj < pplot->linevertcount; jj++) {

            if (((pplot->linevert[jj].style & 0xF0) == 0x00) || ((pplot->linevert[jj].status & 0x0F) == 0x0F)) {
                continue;
            }

            if (penLine) {
                delete(penLine); penLine = NULL;
            }

            try {
                penLine = new Gdiplus::Pen(Gdiplus::Color(pplot->linevert[jj].color.r, pplot->linevert[jj].color.g, pplot->linevert[jj].color.b), (Gdiplus::REAL)(pplot->linevert[jj].size));
            }
            catch (...) {
                penLine = NULL;
                goto pl_line_RET;
            }

            penLine->SetLineJoin(Gdiplus::LineJoinRound);

            switch (pplot->linevert[jj].style & 0xF0) {
                case 0x10:
                    penLine->SetDashStyle(Gdiplus::DashStyleSolid);
                    nPenStyle = PS_SOLID;
                    break;
                case 0x20:
                    penLine->SetDashStyle(Gdiplus::DashStyleDash);
                    nPenStyle = PS_DASH;
                    break;
                case 0x30:
                    penLine->SetDashStyle(Gdiplus::DashStyleDot);
                    nPenStyle = PS_DOT;
                    break;
                case 0x40:
                    penLine->SetDashStyle(Gdiplus::DashStyleDashDot);
                    nPenStyle = PS_DASHDOT;
                    break;
                case 0x50:
                    penLine->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                    nPenStyle = PS_DASHDOTDOT;
                    break;
                default:
                    penLine->SetDashStyle(Gdiplus::DashStyleCustom);
                    nPenStyle = PS_NULL;
                    break;
            }
            if (nPenStyle == PS_NULL) {
                continue;
            }
            setPenTX(pplot, &penLine);

            // >> Corriger les coordonnées
            if (pplot->linevert[jj].rect.ya == pplot->linevert[jj].rect.yb) {
                pplot->linevert[jj].rect.ya = paxisY->min;
                pplot->linevert[jj].rect.yb = paxisY->max;
            }
            if (pplot->linevert[jj].rect.ya > pplot->linevert[jj].rect.yb) {
                ftmp = pplot->linevert[jj].rect.yb;
                pplot->linevert[jj].rect.yb = pplot->linevert[jj].rect.ya;
                pplot->linevert[jj].rect.ya = ftmp;
            }
            // <<

            if ((pplot->linevert[jj].rect.xa >= paxisX->min) && (pplot->linevert[jj].rect.xa <= paxisX->max)) {
                xa = pplot->linevert[jj].rect.xa;
                if (paxisX->scale == 1) {
                    if (xa > 0.0) { xa = log10(xa); }
                    else { continue; }
                }
                ya = pplot->linevert[jj].rect.ya;
                if (paxisY->scale == 1) {
                    if (ya > 0.0) { ya = log10(ya); }
                    else { continue; }
                }
                yb = pplot->linevert[jj].rect.yb;
                if (paxisY->scale == 1) {
                    if (yb > 0.0) { yb = log10(yb); }
                    else { continue; }
                }

                fx = ((xa - xmin) * wx);

                fyy = 0;
                if ((pplot->linevert[jj].rect.ya < pplot->linevert[jj].rect.yb)
                    && (pplot->linevert[jj].rect.ya >= paxisY->min)
                    && (pplot->linevert[jj].rect.ya <= paxisY->max)
                    && (pplot->linevert[jj].rect.yb >= paxisY->min)
                    && (pplot->linevert[jj].rect.yb <= paxisY->max)) {

                    fy = fh - ((ya - ymin) * wy);
                    fyy = fh - ((yb - ymin) * wy);
                }
                else {
                    fy = fh - 1.0;
                    fyy = fh - ((ymax - ymin) * wy);
                }

                if (fx < 1.0) { fx = 1.0; }
                else if (fx >= fw) { fx = fw - 1.0; }
                if (fy < 1.0) { fy = 1.0; }
                else if (fy >= fh) { fy = fh - 1.0; }
                if (fyy < 1) { fyy = 1; }
                else if (fyy >= fh) { fyy = fh - 1; }

                fx += fx0;    fy += fy0;    fyy += fy0;

                drawLineTX(pplot, fx, fy, fx, fyy);
            }
        }
    }

    // HORIZONTAL LINE
    if (pplot->linehorzcount > 0) {
        for (jj = 0; jj < pplot->linehorzcount; jj++) {

            if (((pplot->linehorz[jj].style & 0xF0) == 0x00) || ((pplot->linehorz[jj].status & 0x0F) == 0x0F)) {
                continue;
            }

            if (penLine) {
                delete(penLine); penLine = NULL;
            }

            try {
                penLine = new Gdiplus::Pen(Gdiplus::Color(pplot->linehorz[jj].color.r, pplot->linehorz[jj].color.g, pplot->linehorz[jj].color.b), (Gdiplus::REAL)(pplot->linehorz[jj].size));
            }
            catch (...) {
                penLine = NULL;
                goto pl_line_RET;
            }

            penLine->SetLineJoin(Gdiplus::LineJoinRound);

            switch (pplot->linehorz[jj].style & 0xF0) {
                case 0x10:
                    penLine->SetDashStyle(Gdiplus::DashStyleSolid);
                    nPenStyle = PS_SOLID;
                    break;
                case 0x20:
                    penLine->SetDashStyle(Gdiplus::DashStyleDash);
                    nPenStyle = PS_DASH;
                    break;
                case 0x30:
                    penLine->SetDashStyle(Gdiplus::DashStyleDot);
                    nPenStyle = PS_DOT;
                    break;
                case 0x40:
                    penLine->SetDashStyle(Gdiplus::DashStyleDashDot);
                    nPenStyle = PS_DASHDOT;
                    break;
                case 0x50:
                    penLine->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                    nPenStyle = PS_DASHDOTDOT;
                    break;
                default:
                    penLine->SetDashStyle(Gdiplus::DashStyleCustom);
                    nPenStyle = PS_NULL;
                    break;
            }
            if (nPenStyle == PS_NULL) {
                continue;
            }
            setPenTX(pplot, &penLine);

            // >> Corriger les coordonnées
            if (pplot->linehorz[jj].rect.xa > pplot->linehorz[jj].rect.xb) {
                ftmp = pplot->linehorz[jj].rect.xb;
                pplot->linehorz[jj].rect.xb = pplot->linehorz[jj].rect.xa;
                pplot->linehorz[jj].rect.xa = ftmp;
            }
            if (pplot->linehorz[jj].rect.xa == pplot->linehorz[jj].rect.xb) {
                pplot->linehorz[jj].rect.xa = paxisX->min;
                pplot->linehorz[jj].rect.xb = paxisX->max;
            }
            // <<

            if ((pplot->linehorz[jj].rect.ya >= paxisY->min) && (pplot->linehorz[jj].rect.ya <= paxisY->max)) {

                ya = pplot->linehorz[jj].rect.ya;
                if (paxisY->scale == 1) {
                    if (ya > 0.0) { ya = log10(ya); }
                    else { continue; }
                }
                xa = pplot->linehorz[jj].rect.xa;
                if (paxisX->scale == 1) {
                    if (xa > 0.0) { xa = log10(xa); }
                    else { continue; }
                }
                xb = pplot->linehorz[jj].rect.xb;
                if (paxisX->scale == 1) {
                    if (xb > 0.0) { xb = log10(xb); }
                    else { continue; }
                }

                fy = fh - ((ya - ymin) * wy);

                fxx = 0;
                if ((pplot->linehorz[jj].rect.xa < pplot->linehorz[jj].rect.xb)
                    && (pplot->linehorz[jj].rect.xa >= paxisX->min)
                    && (pplot->linehorz[jj].rect.xa <= paxisX->max)
                    && (pplot->linehorz[jj].rect.xb >= paxisX->min)
                    && (pplot->linehorz[jj].rect.xb <= paxisX->max)) {

                    fx = ((xa - xmin) * wx);
                    fxx = ((xb - xmin) * wx);
                }
                else {
                    fx = 1;
                    fxx = ((xmax - xmin) * wx);
                }

                if (fx < 1.0) { fx = 1.0; }
                else if (fx >= fw) { fx = fw - 1.0; }
                if (fxx < 1.0) { fxx = 1.0; }
                else if (fxx >= fw) { fxx = fw - 1.0; }
                if (fy < 1.0) { fy = 1.0; }
                else if (fy >= fh) { fy = fh - 1.0; }

                fx += fx0;    fxx += fx0;    fy += fy0;

                drawLineTX(pplot, fx, fy, fxx, fy);
            }
        }
    }

    pplot->errcode = 0;

pl_line_RET:
    if (penLine) { delete(penLine); penLine = NULL; }
    return;
}

/*  --------------------------------------------------------------------
    pl_title        :    titre du graphe

        pplot       :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
static void pl_title(plot_t* pplot)
{
    irect_t frameRect;
    real_t xa, xb, ya, yb;
    SIZE sizeText = {0L, 0L};
    int_t nlen;

    Gdiplus::Font *pFont = NULL, *pFontSm = NULL;
    Gdiplus::SolidBrush *pBrush = NULL;

    if (pplot == NULL) {
        return;
    }

    if (pplot->otitle == 0) {
        return;
    }

    if (pplot->errcode) { return; }

    pplot->errcode = 1;

    Gdiplus::Color fontcolor(pplot->title.font.color.r, pplot->title.font.color.g, pplot->title.font.color.b);

    Gdiplus::FontStyle fontStyleTitle = Gdiplus::FontStyleRegular;
    if ((pplot->title.font.style & 0xF0) == 0x10) {
        if ((pplot->title.font.style & 0x0F) == 0x01) {
            fontStyleTitle = Gdiplus::FontStyleBoldItalic;
        }
        else {
            fontStyleTitle = Gdiplus::FontStyleBold;
        }
    }
    else if ((pplot->title.font.style & 0x0F) == 0x01) {
        fontStyleTitle = Gdiplus::FontStyleItalic;
    }

    try {
        pBrush = new Gdiplus::SolidBrush(fontcolor);
    }
    catch (...) {
        pBrush = NULL;
        goto pl_title_RET;
    }

    try {
        pFont = new Gdiplus::Font(pplot->title.font.name, (Gdiplus::REAL)(pplot->title.font.size), fontStyleTitle);
    }
    catch (...) {
        pFont = NULL;
        goto pl_title_RET;
    }

    try {
        int iSizeSm = getFontSizeSm(&(pplot->title.font));
        pFontSm = new Gdiplus::Font(pplot->title.font.name, (Gdiplus::REAL)iSizeSm, fontStyleTitle);
    }
    catch (...) {
        pFontSm = NULL;
        goto pl_title_RET;
    }

    // TITLE - PLOT

    xa = pplot->relpos[0].xa;    xb = pplot->relpos[0].xb;
    ya = pplot->relpos[0].ya;    yb = ya + 0.05;

    if (pplot->otitle == 1) {

        frameRect.left = (LONG) round(pplot->plint.width * xa);
        frameRect.right = (LONG) round(pplot->plint.width * xb);
        frameRect.top = (LONG) round(pplot->plint.height * ya);
        frameRect.bottom = (LONG) round(pplot->plint.height * yb);

        nlen = (int_t)_tcslen(pplot->title.text);
        // Adapter � la taille du texte

        Gdiplus::RectF rcF;

        rcF.X = (Gdiplus::REAL) (frameRect.left);
        rcF.Y = (Gdiplus::REAL) (frameRect.top);
        rcF.Width = (Gdiplus::REAL) (frameRect.right - frameRect.left);
        rcF.Height = (Gdiplus::REAL) (frameRect.bottom - frameRect.top);
        drawStringTX(pplot, (const char_t*) (pplot->title.text), nlen, pFont, pBrush, &rcF, 0x01, &frameRect, pFontSm);

        // Position des différents rectangles        0:WND        1:PLOT    2:TITLE
        pplot->plint.frameRect[2].bottom = frameRect.bottom;
        pplot->plint.frameRect[2].top = frameRect.top;
        pplot->plint.frameRect[2].left = frameRect.left;
        pplot->plint.frameRect[2].right = frameRect.right;
    }

    pplot->errcode = 0;

pl_title_RET:
    if (pFont) { delete(pFont); pFont = NULL; }
    if (pFontSm) { delete(pFontSm); pFontSm = NULL; }
    if (pBrush) { delete(pBrush); pBrush = NULL; }
    setPenTX(pplot, NULL);
    pplot->brush = NULL;
    pplot->font = NULL;
    return;
}

/*  --------------------------------------------------------------------
    pl_draw_text         :    draw text

        pplot            :    pointeur sur la structure du tracé
        pplot            :    pointeur sur la structure du texte
        pplot            :    pointeur sur la structure du rectangle (sortie)
    -------------------------------------------------------------------- */
__inline static void pl_draw_text(plot_t* pplot, text_t* ptext, irect_t* prect)
{
    irect_t recText = {0,0,0,0}, recT = {0,0,0,0};
    real_t tt, bb, ll, rr;

    if ((pplot == NULL) || (ptext == NULL)) {
        return;
    }

    if ((ptext->status & 0x0F) == 0x0F) {    // Texte invisible
        return;
    }

    real_t fw = (ptext->rect.xb - ptext->rect.xa);
    real_t fh = (ptext->rect.yb - ptext->rect.ya);

    if ((ptext->rect.xa < 0.0) || (ptext->rect.xb < 0.0)) {
        ptext->rect.xa = 0.0;
        ptext->rect.xb = ptext->rect.xa + fw;
    }
    else if ((ptext->rect.xa > 1.0) || (ptext->rect.xb > 1.0)) {
        ptext->rect.xb = 1.0;
        ptext->rect.xa = ptext->rect.xb - fw;
    }
    if ((ptext->rect.ya < 0.0) || (ptext->rect.yb < 0.0)) {
        ptext->rect.ya = 0.0;
        ptext->rect.yb = ptext->rect.ya + fh;
    }
    else if ((ptext->rect.ya > 1.0) || (ptext->rect.yb > 1.0)) {
        ptext->rect.yb = 1.0;
        ptext->rect.ya = ptext->rect.yb - fh;
    }

    if ((ptext->rect.ya < 0.0) || (ptext->rect.ya > 1.0)
        || (ptext->rect.yb < 0.0) || (ptext->rect.yb > 1.0)
        || (ptext->rect.xa < 0.0) || (ptext->rect.xa > 1.0)
        || (ptext->rect.xb < 0.0) || (ptext->rect.xb > 1.0)
        || (ptext->rect.ya >= ptext->rect.yb)
        || (ptext->rect.xa >= ptext->rect.xb)
        || (ptext->text[0] == _T('\0'))) {
        return;
    }

    Gdiplus::RectF rcF;
    Gdiplus::Font *pFont = NULL, *pFontSm = NULL;
    Gdiplus::Pen *pPen = NULL;
    Gdiplus::SolidBrush *pBrush = NULL;
    Gdiplus::Color fontColor(ptext->font.color.r, ptext->font.color.g, ptext->font.color.b);

    int iSizeSm = getFontSizeSm(&(ptext->font));

    Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular;
    if ((ptext->font.style & 0xF0) == 0x10) {
        if ((ptext->font.style & 0x0F) == 0x01) {
            fontStyle = Gdiplus::FontStyleBoldItalic;
        }
        else {
            fontStyle = Gdiplus::FontStyleBold;
        }
    }
    else if ((ptext->font.style & 0x0F) == 0x01) {
        fontStyle = Gdiplus::FontStyleItalic;
    }

    try {
        pPen = new Gdiplus::Pen(fontColor, 1);
    }
    catch (...) {
        pPen = NULL;
        goto pl_draw_text_RET;
    }

    try {
        pBrush = new Gdiplus::SolidBrush(fontColor);
    }
    catch (...) {
        pBrush = NULL;
        goto pl_draw_text_RET;
    }

    try {
        pFont = new Gdiplus::Font(ptext->font.name, (Gdiplus::REAL)(ptext->font.size), fontStyle);
    }
    catch (...) {
        pFont = NULL;
        goto pl_draw_text_RET;
    }

    try {
        pFontSm = new Gdiplus::Font(ptext->font.name, (Gdiplus::REAL)iSizeSm, fontStyle);
    }
    catch (...) {
        pFontSm = NULL;
        goto pl_draw_text_RET;
    }

    setPenTX(pplot, &pPen);

    if (ptext->font.orient == 0x00) {
        // Rectangle contenant le texte
        tt = pplot->plint.height * ptext->rect.ya;
        bb = pplot->plint.height * ptext->rect.yb;
        ll = pplot->plint.width * ptext->rect.xa;
        rr = pplot->plint.width * ptext->rect.xb;
        recText.top = (LONG) round(tt);
        recText.bottom = (LONG) round(bb);
        recText.left = (LONG) round(ll);
        recText.right = (LONG) round(rr);

        ptext->len = (int_t)_tcslen((const char_t*)(ptext->text));

        rcF.X = (Gdiplus::REAL) (recText.left);
        rcF.Y = (Gdiplus::REAL) (recText.top);
        rcF.Width = (Gdiplus::REAL) (recText.right - recText.left);
        rcF.Height = (Gdiplus::REAL) (recText.bottom - recText.top);
        drawStringTX(pplot, (const char_t*) (ptext->text), ptext->len, pFont, pBrush, &rcF, 0x00, &recText, pFontSm, SVG_ALIGN_START);
    }
    else if (ptext->font.orient != 0x00) { // Vert. Text
        recText.left = (LONG) round(pplot->plint.width * ptext->rect.xa);
        recText.right = (LONG) round(pplot->plint.width * ptext->rect.xb);
        recText.top = (LONG) round(pplot->plint.height * ptext->rect.ya);
        recText.bottom = (LONG) round(pplot->plint.height * ptext->rect.yb);

        ptext->len = (int_t)_tcslen(ptext->text);

        rcF.X = (Gdiplus::REAL) (recText.left);
        rcF.Y = (Gdiplus::REAL) (recText.top);
        rcF.Width = (Gdiplus::REAL) (recText.right - recText.left);
        rcF.Height = (Gdiplus::REAL) (recText.bottom - recText.top);
        drawStringTX(pplot, (const char_t*) ptext->text, ptext->len, pFont, pBrush, &rcF, (ptext->font.orient == 0x09) ? 0x20 : 0x10, &recText, pFontSm);
    }

    // Sauvegarder la position
    if (prect) {
        prect->top = recText.top;
        prect->bottom = recText.bottom;
        prect->left = recText.left;
        prect->right = recText.right;
    }

pl_draw_text_RET:
    if (pPen) { delete(pPen); pPen = NULL; }
    if (pFont) { delete(pFont); pFont = NULL; }
    if (pFontSm) { delete(pFontSm); pFontSm = NULL; }
    if (pBrush) { delete(pBrush); pBrush = NULL; }
    setPenTX(pplot, NULL);
    pplot->brush = NULL;
    pplot->font = NULL;
    return;
}

/*  --------------------------------------------------------------------
    pl_print_text        :    print text

        pplot            :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static void pl_print_text(plot_t* pplot)
{
    int_t ii;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    // Aucun texte
    if ((pplot->textcount < 1) || (pplot->textcount > PL_MAXITEMS)) {
        return;
    }

    pplot->errcode = 1;

    for (ii = 0; ii < pplot->textcount; ii++) {
        pl_draw_text(pplot, &(pplot->text[ii]), &(pplot->plint.textRect[ii]));
    }

    pplot->errcode = 0;

    return;
}

/*  --------------------------------------------------------------------
    pl_draw_line         :    draw line

        pplot            :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static void pl_draw_line(plot_t* pplot)
{
    int_t ii, nPenStyle;
    real_t fx, fy, ftmp;
    long_t nx, ny, nxp, nyp;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    // Aucune ligne
    if ((pplot->linecount < 1) || (pplot->linecount > PL_MAXITEMS)) {
        return;
    }

    pplot->errcode = 1;

    Gdiplus::Pen *penLine = NULL;
    Gdiplus::SolidBrush *pBrush = NULL;

    for (ii = 0; ii < pplot->linecount; ii++) {
        if ((pplot->line[ii].status & 0x0F) == 0x0F) {    // Ligne invisible
            continue;
        }
        fx = pplot->line[ii].rect.xb - pplot->line[ii].rect.xa;
        fy = pplot->line[ii].rect.yb  - pplot->line[ii].rect.ya;

        if ((fx == 0.0) && (fy == 0.0)) {
            continue;
        }
        if ((fx > 1.0) || (fy > 1.0)) {
            continue;
        }

        // >> Corriger les coordonnées
        if (pplot->line[ii].rect.xa > pplot->line[ii].rect.xb) {
            ftmp = pplot->line[ii].rect.xb;
            pplot->line[ii].rect.xb = pplot->line[ii].rect.xa;
            pplot->line[ii].rect.xa = ftmp;
            ftmp = pplot->line[ii].rect.yb;
            pplot->line[ii].rect.yb = pplot->line[ii].rect.ya;
            pplot->line[ii].rect.ya = ftmp;
            // Invert Arrows
            if ((pplot->line[ii].arrstyle & 0xF0) == 0x10) {
                pplot->line[ii].arrstyle &= 0x0F;
                pplot->line[ii].arrstyle |= 0x20;
            }
            else if ((pplot->line[ii].arrstyle & 0xF0) == 0x20) {
                pplot->line[ii].arrstyle &= 0x0F;
                pplot->line[ii].arrstyle |= 0x10;
            }
        }
        // <<

        if ((pplot->line[ii].rect.xa < 0.0) || (pplot->line[ii].rect.xb < 0.0)) {
            pplot->line[ii].rect.xa = 0.0;
            pplot->line[ii].rect.xb = pplot->line[ii].rect.xa + fx;
        }
        if ((pplot->line[ii].rect.xa > 1.0) || (pplot->line[ii].rect.xb > 1.0)) {
            pplot->line[ii].rect.xb = 1.0;
            pplot->line[ii].rect.xa = pplot->line[ii].rect.xb - fx;
        }
        if (pplot->line[ii].orient == 0) {
            if ((pplot->line[ii].rect.ya < 0.0) || (pplot->line[ii].rect.yb < 0.0)) {
                pplot->line[ii].rect.ya = 0.0;
                pplot->line[ii].rect.yb = pplot->line[ii].rect.ya + fy;
            }
            if ((pplot->line[ii].rect.ya > 1.0) || (pplot->line[ii].rect.yb > 1.0)) {
                pplot->line[ii].rect.yb = 1.0;
                pplot->line[ii].rect.ya = pplot->line[ii].rect.yb - fy;
            }
        }
        else {
            if ((pplot->line[ii].rect.ya < 0.0) || (pplot->line[ii].rect.yb < 0.0)) {
                pplot->line[ii].rect.yb = 0.0;
                pplot->line[ii].rect.ya = pplot->line[ii].rect.yb - fy;
            }
            if ((pplot->line[ii].rect.ya > 1.0) || (pplot->line[ii].rect.yb > 1.0)) {
                pplot->line[ii].rect.ya = 1.0;
                pplot->line[ii].rect.yb = pplot->line[ii].rect.ya + fy;
            }
        }

        if (penLine) {
            delete(penLine); penLine = NULL;
        }

        try {
            penLine = new Gdiplus::Pen(Gdiplus::Color(pplot->line[ii].color.r, pplot->line[ii].color.g, pplot->line[ii].color.b), (Gdiplus::REAL)(pplot->line[ii].size));
        }
        catch (...) {
            penLine = NULL;
            goto pl_draw_line_RET;
        }

        penLine->SetLineJoin(Gdiplus::LineJoinRound);

        switch (pplot->line[ii].style & 0xF0) {
            case 0x10:
                penLine->SetDashStyle(Gdiplus::DashStyleSolid);
                nPenStyle = PS_SOLID;
                break;
            case 0x20:
                penLine->SetDashStyle(Gdiplus::DashStyleDash);
                nPenStyle = PS_DASH;
                break;
            case 0x30:
                penLine->SetDashStyle(Gdiplus::DashStyleDot);
                nPenStyle = PS_DOT;
                break;
            case 0x40:
                penLine->SetDashStyle(Gdiplus::DashStyleDashDot);
                nPenStyle = PS_DASHDOT;
                break;
            case 0x50:
                penLine->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                nPenStyle = PS_DASHDOTDOT;
                break;
            default:
                penLine->SetDashStyle(Gdiplus::DashStyleCustom);
                nPenStyle = PS_NULL;
                break;
        }
        if (nPenStyle == PS_NULL) {
            continue;
        }

        setPenTX(pplot, &penLine);

        fx = pplot->plint.width * pplot->line[ii].rect.xa;
        fy = pplot->plint.height * pplot->line[ii].rect.ya;
        nx = (long_t) round(fx);
        ny = (long_t) round(fy);

        // Sauvegarder la position
        pplot->plint.lineRect[ii].left = nx;
        pplot->plint.lineRect[ii].top = ny;

        fx = pplot->plint.width * pplot->line[ii].rect.xb;
        fy = pplot->plint.height * pplot->line[ii].rect.yb;
        nxp = (long_t) round(fx);
        nyp = (long_t) round(fy);

        drawLineTX(pplot, nx, ny, nxp, nyp);

        nx = nxp;
        ny = nyp;

        // Sauvegarder la position
        pplot->line[ii].orient = 0;
        pplot->plint.lineRect[ii].right = nx;
        pplot->plint.lineRect[ii].bottom = ny;
        if (pplot->plint.lineRect[ii].right < pplot->plint.lineRect[ii].left) {
            nx = pplot->plint.lineRect[ii].right;
            pplot->plint.lineRect[ii].right = pplot->plint.lineRect[ii].left;
            pplot->plint.lineRect[ii].left = nx;
            pplot->line[ii].orient = 1;
        }
        if (pplot->plint.lineRect[ii].bottom < pplot->plint.lineRect[ii].top) {
            ny = pplot->plint.lineRect[ii].bottom;
            pplot->plint.lineRect[ii].bottom = pplot->plint.lineRect[ii].top;
            pplot->plint.lineRect[ii].top = ny;
            pplot->line[ii].orient = 1;
        }

        if (pplot->line[ii].arrstyle != 0x00) {
            if (pBrush) { delete(pBrush); pBrush = NULL; }

            try {
                pBrush = new Gdiplus::SolidBrush(Gdiplus::Color(pplot->line[ii].color.r, pplot->line[ii].color.g, pplot->line[ii].color.b));
            }
            catch (...) {
                pBrush = NULL;
                goto pl_draw_line_RET;
            }

            pplot->brush = pBrush;
            pl_draw_arrow(pplot, ii);
        }
    }

    pplot->errcode = 0;

pl_draw_line_RET:
    if (penLine) { delete(penLine); penLine = NULL; }
    if (pBrush) { delete(pBrush); pBrush = NULL; }
    setPenTX(pplot, NULL);
    pplot->brush = NULL;
    pplot->font = NULL;
    return;
}

/*  --------------------------------------------------------------------
    pl_draw_rect         :    draw rectangle

        pplot            :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static void pl_draw_rect(plot_t* pplot)
{
    irect_t recTT = {0,0,0,0};
    int_t ii, nPenStyle;
    real_t bb, tt, ll, rr, fx, fy, ftmp;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    // Aucune ligne
    if ((pplot->rectanglecount < 1) || (pplot->rectanglecount > PL_MAXITEMS)) {
        return;
    }

    pplot->errcode = 1;

    Gdiplus::Pen *penLine = NULL;
    Gdiplus::SolidBrush *pBrush = NULL;

    Gdiplus::Color tfillcolor;

    for (ii = 0; ii < pplot->rectanglecount; ii++) {
        if ((pplot->rectangle[ii].status & 0x0F) == 0x0F) {    // Rectangle invisible
            continue;
        }

        fx = pplot->rectangle[ii].rect.xb - pplot->rectangle[ii].rect.xa;
        fy = pplot->rectangle[ii].rect.yb  - pplot->rectangle[ii].rect.ya;

        if ((fx == 0.0) && (fy == 0.0)) {
            continue;
        }
        if ((fx > 1.0) || (fy > 1.0)) {
            continue;
        }

        // >> Corriger les coordonnées
        if (pplot->rectangle[ii].rect.xa > pplot->rectangle[ii].rect.xb) {
            ftmp = pplot->rectangle[ii].rect.xb;
            pplot->rectangle[ii].rect.xb = pplot->rectangle[ii].rect.xa;
            pplot->rectangle[ii].rect.xa = ftmp;
        }
        if (pplot->rectangle[ii].rect.ya > pplot->rectangle[ii].rect.yb) {
            ftmp = pplot->rectangle[ii].rect.yb;
            pplot->rectangle[ii].rect.yb = pplot->rectangle[ii].rect.ya;
            pplot->rectangle[ii].rect.ya = ftmp;
        }
        // <<

        if ((pplot->rectangle[ii].rect.xa < 0.0) || (pplot->rectangle[ii].rect.xb < 0.0)) {
            pplot->rectangle[ii].rect.xa = 0.0;
            pplot->rectangle[ii].rect.xb = pplot->rectangle[ii].rect.xa + fx;
        }
        if ((pplot->rectangle[ii].rect.xa > 1.0) || (pplot->rectangle[ii].rect.xb > 1.0)) {
            pplot->rectangle[ii].rect.xb = 1.0;
            pplot->rectangle[ii].rect.xa = pplot->rectangle[ii].rect.xb - fx;
        }
        if ((pplot->rectangle[ii].rect.ya < 0.0) || (pplot->rectangle[ii].rect.yb < 0.0)) {
            pplot->rectangle[ii].rect.ya = 0.0;
            pplot->rectangle[ii].rect.yb = pplot->rectangle[ii].rect.ya + fy;
        }
        if ((pplot->rectangle[ii].rect.ya > 1.0) || (pplot->rectangle[ii].rect.yb > 1.0)) {
            pplot->rectangle[ii].rect.yb = 1.0;
            pplot->rectangle[ii].rect.ya = pplot->rectangle[ii].rect.yb - fy;
        }

        // Rectangle
        tt = pplot->plint.height * pplot->rectangle[ii].rect.ya;
        bb = pplot->plint.height * pplot->rectangle[ii].rect.yb;
        ll = pplot->plint.width * pplot->rectangle[ii].rect.xa;
        rr = pplot->plint.width * pplot->rectangle[ii].rect.xb;
        recTT.top = (LONG) round(tt); recTT.bottom = (LONG) round(bb);
        recTT.left = (LONG) round(ll); recTT.right = (LONG) round(rr);
        // Sauvegarder la position
        pplot->plint.rectangleRect[ii].top = recTT.top;
        pplot->plint.rectangleRect[ii].bottom = recTT.bottom;
        pplot->plint.rectangleRect[ii].left = recTT.left;
        pplot->plint.rectangleRect[ii].right = recTT.right;

        if (penLine) {
            delete(penLine); penLine = NULL;
        }

        try {
            penLine = new Gdiplus::Pen(Gdiplus::Color(pplot->rectangle[ii].color.r, pplot->rectangle[ii].color.g, pplot->rectangle[ii].color.b), (Gdiplus::REAL)(pplot->rectangle[ii].size));
        }
        catch (...) {
            penLine = NULL;
            goto pl_draw_rect_RET;
        }

        penLine->SetLineJoin(Gdiplus::LineJoinRound);

        switch (pplot->rectangle[ii].style & 0xF0) {
            case 0x10:
                penLine->SetDashStyle(Gdiplus::DashStyleSolid);
                nPenStyle = PS_SOLID;
                break;
            case 0x20:
                penLine->SetDashStyle(Gdiplus::DashStyleDash);
                nPenStyle = PS_DASH;
                break;
            case 0x30:
                penLine->SetDashStyle(Gdiplus::DashStyleDot);
                nPenStyle = PS_DOT;
                break;
            case 0x40:
                penLine->SetDashStyle(Gdiplus::DashStyleDashDot);
                nPenStyle = PS_DASHDOT;
                break;
            case 0x50:
                penLine->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                nPenStyle = PS_DASHDOTDOT;
                break;
            default:
                penLine->SetDashStyle(Gdiplus::DashStyleCustom);
                nPenStyle = PS_NULL;
                break;
        }
        if (nPenStyle == PS_NULL) {
            continue;
        }

        setPenTX(pplot, &penLine);

        // Filled or hollow rectangle
        if (pBrush) { delete(pBrush); pBrush = NULL; }
        if (((pplot->rectangle[ii].style) & 0x01) == 0x01) {

            tfillcolor = Gdiplus::Color(pplot->rectangle[ii].fcolor.r, pplot->rectangle[ii].fcolor.g, pplot->rectangle[ii].fcolor.b);
            try {
                pBrush = new Gdiplus::SolidBrush(tfillcolor);
            }
            catch (...) {
                pBrush = NULL;
                goto pl_draw_rect_RET;
            }

            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                pplot->svgdoc->setFill(svgFill(svgColor((int_t) (pplot->rectangle[ii].fcolor.r), (int_t) (pplot->rectangle[ii].fcolor.g), (int_t) (pplot->rectangle[ii].fcolor.b), false)));
            }

            pplot->brush = pBrush;
            fillRectangleTX(pplot, recTT.left, recTT.top, recTT.right, recTT.bottom, &tfillcolor);
        }
        drawRectangleTX(pplot, recTT.left, recTT.top, recTT.right, recTT.bottom);
    }

    pplot->errcode = 0;

pl_draw_rect_RET:
    if (pBrush) { delete(pBrush); pBrush = NULL; }
    if (penLine) { delete(penLine); penLine = NULL; }
    setPenTX(pplot, NULL);
    pplot->brush = NULL;
    pplot->font = NULL;
    return;
}

/*  --------------------------------------------------------------------
    pl_draw_ellipse          :    draw ellipse

        pplot                :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
__inline static void pl_draw_ellipse(plot_t* pplot)
{
    irect_t recTT = {0,0,0,0};
    int_t ii, nPenStyle;
    real_t bb, tt, ll, rr, fx, fy, ftmp;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    // Aucune ligne
    if ((pplot->ellipsecount < 1) || (pplot->ellipsecount > PL_MAXITEMS)) {
        return;
    }

    pplot->errcode = 1;

    Gdiplus::Color tfillcolor;

    Gdiplus::Pen *penLine = NULL;
    Gdiplus::SolidBrush *pBrush = NULL;

    for (ii = 0; ii < pplot->ellipsecount; ii++) {
        if ((pplot->ellipse[ii].status & 0x0F) == 0x0F) {    // Cercle invisible
            continue;
        }
        
        fx = pplot->ellipse[ii].rect.xb - pplot->ellipse[ii].rect.xa;
        fy = pplot->ellipse[ii].rect.yb  - pplot->ellipse[ii].rect.ya;

        if ((fx == 0.0) && (fy == 0.0)) {
            continue;
        }
        if ((fx > 1.0) || (fy > 1.0)) {
            continue;
        }

        // >> Corriger les coordonnées
        if (pplot->ellipse[ii].rect.xa > pplot->ellipse[ii].rect.xb) {
            ftmp = pplot->ellipse[ii].rect.xb;
            pplot->ellipse[ii].rect.xb = pplot->ellipse[ii].rect.xa;
            pplot->ellipse[ii].rect.xa = ftmp;
        }
        if (pplot->ellipse[ii].rect.ya > pplot->ellipse[ii].rect.yb) {
            ftmp = pplot->ellipse[ii].rect.yb;
            pplot->ellipse[ii].rect.yb = pplot->ellipse[ii].rect.ya;
            pplot->ellipse[ii].rect.ya = ftmp;
        }
        // <<

        if ((pplot->ellipse[ii].rect.xa < 0.0) || (pplot->ellipse[ii].rect.xb < 0.0)) {
            pplot->ellipse[ii].rect.xa = 0.0;
            pplot->ellipse[ii].rect.xb = pplot->ellipse[ii].rect.xa + fx;
        }
        if ((pplot->ellipse[ii].rect.xa > 1.0) || (pplot->ellipse[ii].rect.xb > 1.0)) {
            pplot->ellipse[ii].rect.xb = 1.0;
            pplot->ellipse[ii].rect.xa = pplot->ellipse[ii].rect.xb - fx;
        }
        if ((pplot->ellipse[ii].rect.ya < 0.0) || (pplot->ellipse[ii].rect.yb < 0.0)) {
            pplot->ellipse[ii].rect.ya = 0.0;
            pplot->ellipse[ii].rect.yb = pplot->ellipse[ii].rect.ya + fy;
        }
        if ((pplot->ellipse[ii].rect.ya > 1.0) || (pplot->ellipse[ii].rect.yb > 1.0)) {
            pplot->ellipse[ii].rect.yb = 1.0;
            pplot->ellipse[ii].rect.ya = pplot->ellipse[ii].rect.yb - fy;
        }

        // Rectangle
        tt = pplot->plint.height * pplot->ellipse[ii].rect.ya;
        bb = pplot->plint.height * pplot->ellipse[ii].rect.yb;
        ll = pplot->plint.width * pplot->ellipse[ii].rect.xa;
        rr = pplot->plint.width * pplot->ellipse[ii].rect.xb;
        recTT.top = (LONG) round(tt); recTT.bottom = (LONG) round(bb);
        recTT.left = (LONG) round(ll); recTT.right = (LONG) round(rr);
        // Sauvegarder la position
        pplot->plint.ellipseRect[ii].top = recTT.top;
        pplot->plint.ellipseRect[ii].bottom = recTT.bottom;
        pplot->plint.ellipseRect[ii].left = recTT.left;
        pplot->plint.ellipseRect[ii].right = recTT.right;

        if (penLine) {
            delete(penLine); penLine = NULL;
        }

        try {
            penLine = new Gdiplus::Pen(Gdiplus::Color(pplot->ellipse[ii].color.r, pplot->ellipse[ii].color.g, pplot->ellipse[ii].color.b), (Gdiplus::REAL)(pplot->ellipse[ii].size));
        }
        catch (...) {
            penLine = NULL;
            goto pl_draw_ellipse_RET;
        }

        penLine->SetLineJoin(Gdiplus::LineJoinRound);

        switch (pplot->ellipse[ii].style & 0xF0) {
            case 0x10:
                penLine->SetDashStyle(Gdiplus::DashStyleSolid);
                nPenStyle = PS_SOLID;
                break;
            case 0x20:
                penLine->SetDashStyle(Gdiplus::DashStyleDash);
                nPenStyle = PS_DASH;
                break;
            case 0x30:
                penLine->SetDashStyle(Gdiplus::DashStyleDot);
                nPenStyle = PS_DOT;
                break;
            case 0x40:
                penLine->SetDashStyle(Gdiplus::DashStyleDashDot);
                nPenStyle = PS_DASHDOT;
                break;
            case 0x50:
                penLine->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                nPenStyle = PS_DASHDOTDOT;
                break;
            default:
                penLine->SetDashStyle(Gdiplus::DashStyleCustom);
                nPenStyle = PS_NULL;
                break;
        }
        if (nPenStyle == PS_NULL) {
            continue;
        }

        setPenTX(pplot, &penLine);

        // Filled or hollow ellipse
        if (pBrush) { delete(pBrush); pBrush = NULL; }
        if (((pplot->ellipse[ii].style) & 0x01) == 0x01) {

            tfillcolor = Gdiplus::Color(pplot->ellipse[ii].fcolor.r, pplot->ellipse[ii].fcolor.g, pplot->ellipse[ii].fcolor.b);

            try {
                pBrush = new Gdiplus::SolidBrush(Gdiplus::Color(pplot->ellipse[ii].fcolor.r, pplot->ellipse[ii].fcolor.g, pplot->ellipse[ii].fcolor.b));
            }
            catch (...) {
                pBrush = NULL;
                goto pl_draw_ellipse_RET;
            }

            if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
                pplot->svgdoc->setFill(svgFill(svgColor((int_t) (pplot->ellipse[ii].fcolor.r), (int_t) (pplot->ellipse[ii].fcolor.g), (int_t) (pplot->ellipse[ii].fcolor.b), false)));
            }

            pplot->brush = pBrush;
            fillEllipseTX(pplot, recTT.left, recTT.top, recTT.right, recTT.bottom, &tfillcolor);
        }
        drawEllipseTX(pplot, recTT.left, recTT.top, recTT.right, recTT.bottom);
    }

    pplot->errcode = 0;

pl_draw_ellipse_RET:
    if (pBrush) { delete(pBrush); pBrush = NULL; }
    if (penLine) { delete(penLine); penLine = NULL; }
    setPenTX(pplot, NULL);
    pplot->brush = NULL;
    pplot->font = NULL;
    return;
}

__inline static void pl_draw_arrow(plot_t *pplot, int_t ii)
{
    real_t fslope, fcos, fsin;
    real_t flen = 10.0;

    Gdiplus::Point ptT[3];
    svgPoint fbegin, fend, svgptT[3];

    if (pplot == NULL) {
        return;
    }
    if ((ii < 0) || (ii >= pplot->linecount)) {
        return;
    }
    if (pplot->line[ii].arrstyle == 0x00) {
        return;
    }

    fbegin.x = (pplot->plint.width * pplot->line[ii].rect.xa);
    fbegin.y = (pplot->plint.height * pplot->line[ii].rect.ya);
    fend.x = (pplot->plint.width * pplot->line[ii].rect.xb);
    fend.y = (pplot->plint.height * pplot->line[ii].rect.yb);

    fslope = atan2(fbegin.y - fend.y, fbegin.x - fend.x);
    fcos = cos(fslope);
    fsin = sin(fslope);
    if (pplot->line[ii].size <= 3) {
        flen = 6.0 * (real_t) pplot->line[ii].size;
    }
    else {
        flen = (real_t) ML_MIN((4.0 * (real_t) pplot->line[ii].size), 20.0);
    }

    // Begin
    if (((pplot->line[ii].arrstyle & 0xF0) == 0x10) || ((pplot->line[ii].arrstyle & 0xF0) == 0x30)) {
        svgptT[0].x = fbegin.x + ((-flen * fcos) + ((flen / 2.0) * fsin));
        svgptT[0].y = fbegin.y - ((((flen / 2.0) * fcos)) + (flen * fsin));
        svgptT[1].x = fbegin.x;
        svgptT[1].y = fbegin.y;
        svgptT[2].x = fbegin.x + ((-flen * fcos) - ((flen / 2.0) * fsin));
        svgptT[2].y = fbegin.y + ((-flen * fsin) + ((flen / 2.0) * fcos));
        if ((pplot->line[ii].arrstyle & 0x0F) == 0x01) {
            ptT[0].X = (int_t) round(svgptT[0].x);
            ptT[0].Y = (int_t) round(svgptT[0].y);
            ptT[1].X = (int_t) round(svgptT[1].x);
            ptT[1].Y = (int_t) round(svgptT[1].y);
            ptT[2].X = (int_t) round(svgptT[2].x);
            ptT[2].Y = (int_t) round(svgptT[2].y);
            Gdiplus::Color tfillcolor(pplot->line[ii].color.r, pplot->line[ii].color.g, pplot->line[ii].color.b);
            fillPolygonTX(pplot, ptT, svgptT, 3, &tfillcolor);
        }
        drawLineTX(pplot, svgptT[0].x, svgptT[0].y, svgptT[1].x, svgptT[1].y);
        drawLineTX(pplot, svgptT[1].x, svgptT[1].y, svgptT[2].x, svgptT[2].y);
    }
    // End
    if (((pplot->line[ii].arrstyle & 0xF0) == 0x20) || ((pplot->line[ii].arrstyle & 0xF0) == 0x30)) {
        svgptT[0].x = fend.x + ((flen * fcos) - ((flen / 2.0) * fsin));
        svgptT[0].y = fend.y + ((flen * fsin) + ((flen / 2.0) * fcos));
        svgptT[1].x = fend.x;
        svgptT[1].y = fend.y;
        svgptT[2].x = fend.x + ((flen * fcos) + ((flen / 2.0) * fsin));
        svgptT[2].y = fend.y - (((flen / 2.0) * fcos) - (flen * fsin));
        if ((pplot->line[ii].arrstyle & 0x0F) == 0x01) {
            ptT[0].X = (int_t) round(svgptT[0].x);
            ptT[0].Y = (int_t) round(svgptT[0].y);
            ptT[1].X = (int_t) round(svgptT[1].x);
            ptT[1].Y = (int_t) round(svgptT[1].y);
            ptT[2].X = (int_t) round(svgptT[2].x);
            ptT[2].Y = (int_t) round(svgptT[2].y);
            Gdiplus::Color tfillcolor(pplot->line[ii].color.r, pplot->line[ii].color.g, pplot->line[ii].color.b);
            fillPolygonTX(pplot, ptT, svgptT, 3, &tfillcolor);
        }
        drawLineTX(pplot, svgptT[0].x, svgptT[0].y, svgptT[1].x, svgptT[1].y);
        drawLineTX(pplot, svgptT[1].x, svgptT[1].y, svgptT[2].x, svgptT[2].y);
    }
}

/*  --------------------------------------------------------------------
    pl_plot          :    Trac� du graphe

        pplot        :    pointeur sur la structure du tracé
    -------------------------------------------------------------------- */
void pl_plot(plot_t* pplot, bool isPrinting/* = false*/)
{
    int_t ii, jj, visibleCount, iflag = 0, iFirstLegend, iPoints, iMod;
    int_t nPenStyle;
    real_t fx, fy, fx0, fy0, fxPrev, fyPrev, fex, fey, fw, fh, fTx, fTy, fSizeT, fLegT;
    real_t dfex, dfey, fSizeE;
    real_t xx, yy, wx, wy, xrange, yrange, xmin, xmax, ymin, ymax, fT, fTT;
    int_t nlen, iX = 0, iX2 = 0, iY = 0, iY2 = 0, iautoBottom = 0, iautoTop = 0;
    byte_t bLine, bMultiAxes, bAxisused;
    SIZE sizeText = {0L,0L};
    irect_t recPlot = {0,0,0,0}, frameRect = {0,0,0,0};
    irect_t recTT;
    axis_t *paxisX = NULL, *paxisY = NULL;
    int_t dotsize, linesize, iSize;
    Gdiplus::Color clrT;
    bool bLegActive;

    real_t *pnx = NULL, *pny = NULL;

    Gdiplus::Pen *penLine = NULL, *penDot = NULL, *penDrop = NULL, *penFocus = NULL,
        *penBarX = NULL, *penBarY = NULL, *penAxisX = NULL, *penAxisY = NULL;
    Gdiplus::SolidBrush *brDot = NULL, *pBrush = NULL;
    Gdiplus::Color linecolor, dotlinecolor, dotcolor, dropcolor;
    Gdiplus::Font *pFont = NULL, *pFontSm = NULL;
    Gdiplus::Point ptT[4], ptTT[3];
    svgPoint svgptTT[3];

    Gdiplus::GraphicsPath linePath;
    bool bPath = false;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { 
        _tcscpy(pplot->message, _T("Invalid plot"));
        return;
    }

    if (pplot->plint.running) {
        _tcscpy(pplot->message, _T("Invalid plot"));
        pplot->errcode = 1;
        return;
    }

    pplot->plint.running = 1;

    setPenTX(pplot, NULL);
    pplot->brush = NULL;
    pplot->font = NULL;

    try {
        pplot->canvas = new Gdiplus::Graphics(pplot->dc);
    }
    catch (...) {
        pplot->canvas = NULL;
        goto pl_plot_RET;
    }

    pplot->canvas->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    pplot->canvas->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    // EMF
    if (pplot->imagetype == 1) {
        try {
            pplot->metafile = new Gdiplus::Metafile((const char_t*) (pplot->imagefilename), pplot->dc, Gdiplus::EmfTypeEmfPlusDual);
        }
        catch (...) {
            pplot->metafile = NULL;
            goto pl_plot_RET;
        }

        try {
            pplot->image = new Gdiplus::Graphics(pplot->metafile);
        }
        catch (...) {
            pplot->image = NULL;
            goto pl_plot_RET;
        }

        pplot->image->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        pplot->image->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
    }
    // SVG
    else if (pplot->imagetype == 2) {
        std::wstring strFilename = (const char_t*) (pplot->imagefilename);
        try {
            pplot->svgdoc = new svgDocument(strFilename, svgLayout(pplot->plint.width, pplot->plint.height));
        }
        catch (...) {
            pplot->svgdoc = NULL;
            goto pl_plot_RET;
        }
    }

    if (isPrinting) {
        int dpix = ::GetDeviceCaps(pplot->dc, LOGPIXELSX);
        int dpiy = ::GetDeviceCaps(pplot->dc, LOGPIXELSY);
        pplot->canvas->ScaleTransform(100.0f / (Gdiplus::REAL) dpix, 100.0f / (Gdiplus::REAL) dpiy);
        if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
            pplot->image->ScaleTransform(100.0f / (Gdiplus::REAL) dpix, 100.0f / (Gdiplus::REAL) dpiy);
        }
    }

    try {
        penLine = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penLine = NULL;
        goto pl_plot_RET;
    }

    try {
        penDot = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penDot = NULL;
        goto pl_plot_RET;
    }

    try {
        penDrop = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penDrop = NULL;
        goto pl_plot_RET;
    }

    try {
        penFocus = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penFocus = NULL;
        goto pl_plot_RET;
    }

    try {
        penBarX = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penBarX = NULL;
        goto pl_plot_RET;
    }

    try {
        penBarY = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penBarY = NULL;
        goto pl_plot_RET;
    }

    try {
        penAxisX = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penAxisX = NULL;
        goto pl_plot_RET;
    }

    try {
        penAxisY = new Gdiplus::Pen(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        penAxisY = NULL;
        goto pl_plot_RET;
    }

    penLine->SetLineJoin(Gdiplus::LineJoinRound);
    penDot->SetLineJoin(Gdiplus::LineJoinRound);
    penDrop->SetLineJoin(Gdiplus::LineJoinRound);
    penFocus->SetLineJoin(Gdiplus::LineJoinRound);
    penBarX->SetLineJoin(Gdiplus::LineJoinRound);
    penBarY->SetLineJoin(Gdiplus::LineJoinRound);
    penAxisX->SetLineJoin(Gdiplus::LineJoinRound);
    penAxisY->SetLineJoin(Gdiplus::LineJoinRound);

    try {
        brDot = new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0));
    }
    catch (...) {
        brDot = NULL;
        goto pl_plot_RET;
    }

    iX = 0;    iX2 = 0;
    iY = 0;    iY2 = 0;
    iautoBottom = 0;
    iautoTop = 0;
    if (pplot->curvecount > 0) {
        for (jj = 0; jj < pplot->curvecount; jj++) {
            if ((pplot->curve[jj].x == NULL) || (pplot->curve[jj].y == NULL)) {
                continue;
            }
            if ((pplot->curve[jj].x->dat == NULL) || (pplot->curve[jj].y->dat == NULL)
                || (pplot->curve[jj].points < 2) || ((pplot->curve[jj].status & 0x0F) == 0x0F)) {
                continue;
            }

            if ((pplot->curve[jj].axisused & 0xF0) == 0x00) {                // X-Axis
                iX += 1;
                if (pplot->curve[jj].x->id == (ML_MAXCOLUMNS + 99)) {        // AutoX
                    iautoBottom += 1;
                }
            }
            else if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {           // X2-Axis
                iX2 += 1;
                if (pplot->curve[jj].x->id == (ML_MAXCOLUMNS + 99)) {        // AutoX
                    iautoTop += 1;
                }
            }
            if ((pplot->curve[jj].axisused & 0x0F) == 0x00) {                // Y-Axis
                iY += 1;
            }
            else if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {           // Y2-Axis
                iY2 += 1;
            }
        }
    }

    if (iX <= 0) {
        pplot->axisbottom.status = pplot->axisbottom.status | 0xF0;          // Non used axis
    }
    else {
        pplot->axisbottom.status = pplot->axisbottom.status & 0x0F;          // Used axis
    }
    if (iX2 <= 0) {
        pplot->axistop.status = pplot->axistop.status | 0xF0;                // Non used axis
    }
    else {
        pplot->axistop.status = pplot->axistop.status & 0x0F;                // Used axis
    }
    if (iY <= 0) {
        pplot->axisleft.status = pplot->axisleft.status | 0xF0;              // Non used axis
    }
    else {
        pplot->axisleft.status = pplot->axisleft.status & 0x0F;              // Used axis
    }
    if (iY2 <= 0) {
        pplot->axisright.status = pplot->axisright.status | 0xF0;            // Non used axis
    }
    else {
        pplot->axisright.status = pplot->axisright.status & 0x0F;            // Used axis
    }

    pl_draw_frame(pplot);
    pl_grid(pplot, iX, iX2, iY, iY2, (iautoBottom > 0) ? 1 : 0, (iautoTop > 0) ? 1 : 0);
    pl_title(pplot);

    pplot->errcode = 1;

    fx0 = (pplot->plint.width * pplot->relpos[1].xa);
    fy0 = (pplot->plint.height * pplot->relpos[1].ya);

    fw = (pplot->plint.width * (pplot->relpos[1].xb - pplot->relpos[1].xa));
    fh = (pplot->plint.height * (pplot->relpos[1].yb - pplot->relpos[1].ya));

    // PLOT

    fSizeT = 8.0;
    iFirstLegend = -1;
    bAxisused = pplot->curve[0].axisused;
    bMultiAxes = 0;

    bool bFirstPoint = true;

    for (jj = 0; jj < pplot->curvecount; jj++) {
        if ((pplot->curve[jj].x == NULL) || (pplot->curve[jj].y == NULL)) {
            continue;
        }
        if ((pplot->curve[jj].x->dat == NULL) || (pplot->curve[jj].y->dat == NULL)
            || ((pplot->curve[jj].status & 0x0F) == 0x0F) || (pplot->curve[jj].points < 1)) {
            continue;
        }
        if ((pplot->curve[jj].dotstyle == 0x00) && ((pplot->curve[jj].linestyle & 0xF0) == 0x00)) {
            continue;
        }

        if ((bMultiAxes == 0) && (bAxisused != pplot->curve[jj].axisused)) {
            bMultiAxes = 1;
        }

        if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {        // Axis X2
            if (pplot->axistop.scale == 0) {
                xmin = pplot->axistop.min;
                xmax = pplot->axistop.max;
            }
            else {
                pl_logscale(pplot->axistop.min, pplot->axistop.max, &xmin, &xmax, &(pplot->axistop.gridcount));
            }
        }
        else {                                                   // Axis X - Default
            if (pplot->axisbottom.scale == 0) {
                xmin = pplot->axisbottom.min;
                xmax = pplot->axisbottom.max;
            }
            else {
                pl_logscale(pplot->axisbottom.min, pplot->axisbottom.max, &xmin, &xmax, &(pplot->axisbottom.gridcount));
            }
        }
        if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {        // Axis Y2
            if (pplot->axisright.scale == 0) {
                ymin = pplot->axisright.min;
                ymax = pplot->axisright.max;
            }
            else {
                pl_logscale(pplot->axisright.min, pplot->axisright.max, &ymin, &ymax, &(pplot->axisright.gridcount));
            }
        }
        else {                                                    // Axis Y - Default
            if (pplot->axisleft.scale == 0) {
                ymin = pplot->axisleft.min;
                ymax = pplot->axisleft.max;
            }
            else {
                pl_logscale(pplot->axisleft.min, pplot->axisleft.max, &ymin, &ymax, &(pplot->axisleft.gridcount));
            }
        }

        xrange = xmax - xmin;
        yrange = ymax - ymin;
        wx = pplot->plint.width * (pplot->relpos[1].xb - pplot->relpos[1].xa) / xrange;
        wy = pplot->plint.height * (pplot->relpos[1].yb - pplot->relpos[1].ya) / yrange;

        // Line
        bLine = 0;
        if (pplot->curve[jj].linestyle != 0x00) {

            pplot->curve[jj].legend.font.color.r = pplot->curve[jj].linecolor.r;
            pplot->curve[jj].legend.font.color.g = pplot->curve[jj].linecolor.g;
            pplot->curve[jj].legend.font.color.b = pplot->curve[jj].linecolor.b;

            switch (pplot->curve[jj].linestyle & 0xF0) {
                case 0x10:
                    penLine->SetDashStyle(Gdiplus::DashStyleSolid);
                    nPenStyle = PS_SOLID;
                    break;
                case 0x20:
                    penLine->SetDashStyle(Gdiplus::DashStyleDash);
                    nPenStyle = PS_DASH;
                    break;
                case 0x30:
                    penLine->SetDashStyle(Gdiplus::DashStyleDot);
                    nPenStyle = PS_DOT;
                    break;
                case 0x40:
                    penLine->SetDashStyle(Gdiplus::DashStyleDashDot);
                    nPenStyle = PS_DASHDOT;
                    break;
                case 0x50:
                    penLine->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                    nPenStyle = PS_DASHDOTDOT;
                    break;
                default:
                    penLine->SetDashStyle(Gdiplus::DashStyleCustom);
                    nPenStyle = PS_NULL;
                    break;
            }
            if (nPenStyle != PS_NULL) {
                linesize = pplot->curve[jj].linesize;
                penLine->SetWidth((Gdiplus::REAL)linesize);
                penLine->SetColor(Gdiplus::Color(pplot->curve[jj].linecolor.r, pplot->curve[jj].linecolor.g, pplot->curve[jj].linecolor.b));
                bLine = 1;
                linecolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, pplot->curve[jj].linecolor.r, pplot->curve[jj].linecolor.g, pplot->curve[jj].linecolor.b));
            }
        }

        // Symbol
        if (pplot->curve[jj].dotstyle != 0x00) {

            if (pplot->curve[jj].linestyle == 0x00) {
                pplot->curve[jj].legend.font.color.r = pplot->curve[jj].dotcolor.r;
                pplot->curve[jj].legend.font.color.g = pplot->curve[jj].dotcolor.g;
                pplot->curve[jj].legend.font.color.b = pplot->curve[jj].dotcolor.b;
            }

            dotcolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, pplot->curve[jj].dotcolor.r, pplot->curve[jj].dotcolor.g, pplot->curve[jj].dotcolor.b));
            dotlinecolor.SetValue(Gdiplus::Color::MakeARGB(0xFF, pplot->curve[jj].linecolor.r, pplot->curve[jj].linecolor.g, pplot->curve[jj].linecolor.b));
            penDot->SetColor(dotcolor);

            // Filled or hollow symbol
            if ((((pplot->curve[jj].dotstyle) & 0x01) == 0x01) || (pplot->curve[jj].dotstyle == 0x02)) {
                brDot->SetColor(dotcolor);
                pplot->brush = brDot;
                penDot->SetWidth((Gdiplus::REAL)1);
            }
            else if (((pplot->curve[jj].dotstyle) & 0x01) == 0x00) {
                dotsize = 1;
                if ((pplot->curve[jj].dotsize >= 4) && (pplot->curve[jj].dotsize < 8)) {
                    dotsize = 2;
                }
                else if (pplot->curve[jj].dotsize >= 8) {
                    dotsize = 3;
                }
                brDot->SetColor(dotcolor);
                pplot->brush = brDot;
                penDot->SetWidth((Gdiplus::REAL)dotsize);
            }
        }

        // Error Bar - X
        if ((pplot->curve[jj].ex != NULL) && (pplot->curve[jj].exstyle != 0x00)) {
            if ((pplot->curve[jj].ex->dat != NULL) && ((pplot->curve[jj].ex->type & 0xF0) == 0x30)) {
                switch (pplot->curve[jj].exstyle & 0xF0) {
                    case 0x10:
                        penBarX->SetDashStyle(Gdiplus::DashStyleSolid);
                        nPenStyle = PS_SOLID;
                        break;
                    case 0x20:
                        penBarX->SetDashStyle(Gdiplus::DashStyleDash);
                        nPenStyle = PS_DASH;
                        break;
                    case 0x30:
                        penBarX->SetDashStyle(Gdiplus::DashStyleDot);
                        nPenStyle = PS_DOT;
                        break;
                    case 0x40:
                        penBarX->SetDashStyle(Gdiplus::DashStyleDashDot);
                        nPenStyle = PS_DASHDOT;
                        break;
                    case 0x50:
                        penBarX->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                        nPenStyle = PS_DASHDOTDOT;
                        break;
                    default:
                        penBarX->SetDashStyle(Gdiplus::DashStyleCustom);
                        nPenStyle = PS_NULL;
                        break;
                }
                if (nPenStyle != PS_NULL) {
                    penBarX->SetWidth((Gdiplus::REAL)(pplot->curve[jj].exsize));
                    penBarX->SetColor(Gdiplus::Color(pplot->curve[jj].excolor.r, pplot->curve[jj].excolor.g, pplot->curve[jj].excolor.b));
                }
            }
        }

        // Error Bar - Y
        if ((pplot->curve[jj].ey != NULL) && (pplot->curve[jj].eystyle != 0x00)) {
            if ((pplot->curve[jj].ey->dat != NULL) && ((pplot->curve[jj].ey->type & 0xF0) == 0x40)) {
                switch (pplot->curve[jj].eystyle & 0xF0) {
                    case 0x10:
                        penBarY->SetDashStyle(Gdiplus::DashStyleSolid);
                        nPenStyle = PS_SOLID;
                        break;
                    case 0x20:
                        penBarY->SetDashStyle(Gdiplus::DashStyleDash);
                        nPenStyle = PS_DASH;
                        break;
                    case 0x30:
                        penBarY->SetDashStyle(Gdiplus::DashStyleDot);
                        nPenStyle = PS_DOT;
                        break;
                    case 0x40:
                        penBarY->SetDashStyle(Gdiplus::DashStyleDashDot);
                        nPenStyle = PS_DASHDOT;
                        break;
                    case 0x50:
                        penBarY->SetDashStyle(Gdiplus::DashStyleDashDotDot);
                        nPenStyle = PS_DASHDOTDOT;
                        break;
                    default:
                        penBarY->SetDashStyle(Gdiplus::DashStyleCustom);
                        nPenStyle = PS_NULL;
                        break;
                }
                if (nPenStyle != PS_NULL) {
                    penBarY->SetWidth((Gdiplus::REAL)(pplot->curve[jj].eysize));
                    penBarY->SetColor(Gdiplus::Color(pplot->curve[jj].eycolor.r, pplot->curve[jj].eycolor.g, pplot->curve[jj].eycolor.b));
                }
            }
        }

        fxPrev = -1.0; fyPrev = -1.0;

        visibleCount = 0;    // Visible point counter

        if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {        // Axis X2
            paxisX = &(pplot->axistop);
        }
        else {        // Axis X
            paxisX = &(pplot->axisbottom);
        }
        if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {        // Axis Y2
            paxisY = &(pplot->axisright);
        }
        else {        // Axis Y
            paxisY = &(pplot->axisleft);
        }
        
        // >> Moduler le nombre de points trac�s
        iMod = 1;
        iPoints = pplot->curve[jj].points;
        if (iPoints > pplot->modpoints) {
            if (pplot->curve[jj].x->dat[1] > pplot->curve[jj].x->dat[0]) {
                if (paxisX->scale == 0) {         // Linear
                    iPoints = (int_t)floor(xrange / (pplot->curve[jj].x->dat[1] - pplot->curve[jj].x->dat[0]));
                }
                else {                            // Log
                    iPoints = (int_t)floor(xrange / log10(pplot->curve[jj].x->dat[1] / pplot->curve[jj].x->dat[0]));
                }
            }
        }
        if (iPoints > pplot->modpoints) {
            iMod = (int_t)floor(((real_t)iPoints) / ((real_t) (pplot->modpoints)));
            if (((pplot->curve[jj].points % 2) == 0)) {
                if (((iMod % 2) != 0)) {
                    iMod += 1;
                }
            }
            else {
                if (((iMod % 2) == 0)) {
                    iMod += 1;
                }
            }
        }
        if (iMod < 2) {
            iMod = 1;
        }
        if (iMod > 256) {
            iMod = 256;
        }
        // <<

        // SVG
        if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
            setPenTX(pplot, &penLine);
            pplot->svgdoc->newPolyline();
        }

        if (pnx) { free(pnx); pnx = NULL; }
        if (pny) { free(pny); pny = NULL; }
        pnx = (real_t*) malloc(pplot->curve[jj].points * sizeof(real_t));
        pny = (real_t*) malloc(pplot->curve[jj].points * sizeof(real_t));
        if ((pnx == NULL) || (pny == NULL)) { goto pl_plot_RET; }

        fxPrev = -1.0;
        fyPrev = -1.0;
        bFirstPoint = true;
        iflag = 0;

        if (bLine == 1) {
            linePath.StartFigure();
        }
        bPath = false;

        for (ii = 0; ii < pplot->curve[jj].points; ii += iMod) {

            // Les points non valides (en cours d'�dition par exemple) ne sont pas pris en compte
            if (((pplot->curve[jj].x->mask[ii] & 0x0F) == 0x0F) || ((pplot->curve[jj].y->mask[ii] & 0x0F) == 0x0F)) {
                pnx[ii] = -1.0;
                pny[ii] = -1.0;
                continue;
            }
            // Les points masqu�s ne sont pas pris en compte
            if (((pplot->curve[jj].x->mask[ii] & 0x0F) == 0x01) || ((pplot->curve[jj].y->mask[ii] & 0x0F) == 0x01)) {
                pnx[ii] = -1.0;
                pny[ii] = -1.0;
                continue;
            }
            //

            xx = pplot->curve[jj].x->dat[ii];
            if (paxisX->scale == 1) {
                if (xx > 0.0) { xx = log10(xx); }
                else { 
                    pnx[ii] = -1.0;
                    pny[ii] = -1.0;
                    continue;
                }
            }
            yy = ((paxisY->op == 0x01) ? fabs(pplot->curve[jj].y->dat[ii]) : pplot->curve[jj].y->dat[ii]);
            if (paxisY->scale == 1) {
                if (yy > 0.0) { yy = log10(yy); }
                else { 
                    pnx[ii] = -1.0;
                    pny[ii] = -1.0;
                    continue;
                }
            }

            pnx[ii] = -1.0;
            pny[ii] = -1.0;

            if ((xx >= xmin) && (xx <= xmax) && (yy >= ymin) && (yy <= ymax)) {

                fx = ((xx - xmin) * wx);
                fy = fh - ((yy - ymin) * wy);

                fx += fx0;
                fy += fy0;

                pnx[ii] = fx;
                pny[ii] = fy;

                // Sauvegarder la position des points
                if ((pplot->plint.curvePointA[jj] != NULL) && (pplot->plint.curvePointB[jj] != NULL)) {
                    pplot->plint.curvePointA[jj][visibleCount].x = (int_t) round(fx);
                    pplot->plint.curvePointA[jj][visibleCount].y = (int_t) round(fy) - (PL_MAXLINESIZE / 2);
                    pplot->plint.curvePointB[jj][visibleCount].x = (int_t) round(fx);
                    pplot->plint.curvePointB[jj][visibleCount].y = (int_t) round(fy) + (PL_MAXLINESIZE / 2);
                }
                visibleCount += 1;
                // <<

                // Line
                if (bLine == 1) {
                    if ((ii > 0) && (bFirstPoint == false) && (iflag == 0)) {
                        setPenTX(pplot, &penLine);
                        bPath = drawCurveLineTX(&linePath, pplot, pplot->curve[jj].linestyle, fxPrev, fyPrev, fx, fy);
                    }
                    else {
                        // SVG
                        if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL) && ((pplot->curve[jj].linestyle & 0x0F) == 0x01)) {
                            pplot->svgdoc->addPoint(svgPoint(fx, fy));
                        }
                    }

                    fxPrev = fx;
                    fyPrev = fy;
                    iflag = 0;
                    bFirstPoint = false;
                }

                // Symbol:                0x02:.                    0x04:+        0x06:-        0x08:|
                //                            0x10:circle                0x20:square                    0x30:trapeze
                //                            0x40:triangle up        0x50:triangle down
                //                            0x60:triangle left    0x70:triangle right
                if (pplot->curve[jj].dotstyle != 0x00) {

                    // Dots are drawn later...

                    if (pplot->curve[jj].dropstyleh != 0x00) {
                        setPenTX(pplot, &penDrop);
                        if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {        // Y2-Axis
                            drawLineTX(pplot, fx, fy, fx0 + fw, fy);
                        }
                        else {                                                                // Y-Axis
                            drawLineTX(pplot, fx, fy, fx0, fy);
                        }
                    }
                    if (pplot->curve[jj].dropstylev != 0x00) {
                        setPenTX(pplot, &penDrop);
                        if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {        // X2-Axis
                            drawLineTX(pplot, fx, fy, fx, fy0);
                        }
                        else {                                                                // X-Axis
                            drawLineTX(pplot, fx, fy, fx, fy0 + fh);
                        }
                    }
                }

                // Barres d'erreur
                if (pplot->curve[jj].ex != NULL) {
                    if (pplot->curve[jj].ex->dat != NULL) {
                        if (pplot->curve[jj].dotstyle != 0) {
                            fSizeE = pplot->curve[jj].dotsize;
                        }
                        else {
                            fSizeE = (3.0 * pplot->curve[jj].exsize) / 2.0;
                        }
                        if (fSizeE < 3) {
                            fSizeE = 3.0;
                        }

                        dfex = (pplot->curve[jj].ex->dat[ii] * wx);
                        fex = fx - fx0 - (dfex / 2.0);
                        if ((dfex > 0) && (dfex <= fw)) {
                            fex += fx0;
                            fey = fy;
                            setPenTX(pplot, &penBarX);
                            drawLineTX(pplot, fex, fey - fSizeE, fex, fey + fSizeE);
                            drawLineTX(pplot, fex, fey, fex + dfex, fey);
                            drawLineTX(pplot, fex + dfex, fey - fSizeE, fex + dfex, fey + fSizeE);
                        }
                    }
                }
                if (pplot->curve[jj].ey != NULL) {
                    if (pplot->curve[jj].ey->dat != NULL) {
                        if (pplot->curve[jj].dotstyle != 0) {
                            fSizeE = pplot->curve[jj].dotsize;
                        }
                        else {
                            fSizeE = (3.0 * pplot->curve[jj].eysize) / 2.0;
                        }
                        if (fSizeE < 3) {
                            fSizeE = 3.0;
                        }

                        dfey = (pplot->curve[jj].ey->dat[ii] * wy);
                        fey = fy - fy0 - (dfey / 2);
                        if ((dfey > 0) && (dfey <= fh)) {
                            fey += fy0;
                            fex = fx;
                            setPenTX(pplot, &penBarY);
                            drawLineTX(pplot, fex - fSizeE, fey, fex + fSizeE, fey);
                            drawLineTX(pplot, fex, fey, fex, fey + dfey);
                            drawLineTX(pplot, fex - fSizeE, fey + dfey, fex + fSizeE, fey + dfey);
                        }
                    }
                }
            }
            if ((yy < ymin) || (yy > ymax)) {
                iflag = 1;
            }
            else {
                iflag = 0;
            }
        }

        if (bPath) {
            if ((pplot->imagetype == 1) && (pplot->image != NULL)) {
                pplot->image->DrawPath(penLine, &linePath);
            }
            pplot->canvas->DrawPath(penLine, &linePath);
            linePath.Reset();
        }

        // SVG
        if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
            pplot->svgdoc->savePolyline();
        }

        // Symbol:                0x02:.                  0x04:+              0x06:-        0x08:|
        //                        0x10:circle             0x20:square         0x30:trapeze
        //                        0x40:triangle up        0x50:triangle down
        //                        0x60:triangle left      0x70:triangle right

        if (pplot->curve[jj].dotstyle != 0x00) {

            setPenTX(pplot, &penDot);

            byte_t dotlinesize = (bLine == 1) ? 0x00 : pplot->curve[jj].linesize;
            Gdiplus::Color *pDLC = (bLine == 1) ? NULL : &dotlinecolor;

            for (ii = 0; ii < pplot->curve[jj].points; ii += iMod) {

                if ((pnx[ii] < 0) || (pny[ii] < 0)) {
                    continue;
                }

                drawCurveDotTX(pplot, pplot->curve[jj].dotstyle, pplot->curve[jj].dotsize, pnx[ii], pny[ii], &dotcolor, dotlinesize, pDLC);
            }
        }

        // LEGENDE DU PLOT jj
        if (pplot->curve[jj].olegend == 1) {
            if (pBrush) {
                delete(pBrush); pBrush = NULL;
            }
            if (pFont) {
                delete(pFont); pFont = NULL;
            }
            if (pFontSm) {
                delete(pFontSm); pFontSm = NULL;
            }

            iSize = getFontSizeSm(&(pplot->curve[jj].legend.font));

            Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular;
            if ((pplot->curve[jj].legend.font.style & 0xF0) == 0x10) {
                if ((pplot->curve[jj].legend.font.style & 0x0F) == 0x01) {
                    fontStyle = Gdiplus::FontStyleBoldItalic;
                }
                else {
                    fontStyle = Gdiplus::FontStyleBold;
                }
            }
            else if ((pplot->curve[jj].legend.font.style & 0x0F) == 0x01) {
                fontStyle = Gdiplus::FontStyleItalic;
            }

            try {
                pBrush = new Gdiplus::SolidBrush(Gdiplus::Color(pplot->curve[jj].legend.font.color.r, pplot->curve[jj].legend.font.color.g, pplot->curve[jj].legend.font.color.b));
            }
            catch (...) {
                pBrush = NULL;
                goto pl_plot_RET;
            }

            try {
                pFont = new Gdiplus::Font(pplot->curve[jj].legend.font.name, (Gdiplus::REAL)(pplot->curve[jj].legend.font.size), fontStyle);
            }
            catch (...) {
                pFont = NULL;
                goto pl_plot_RET;
            }

            try {
                pFontSm = new Gdiplus::Font(pplot->curve[jj].legend.font.name, (Gdiplus::REAL)iSize, fontStyle);
            }
            catch (...) {
                pFontSm = NULL;
                goto pl_plot_RET;
            }

            pplot->brush = pBrush;
            pplot->font = pFont;

            nlen = (int_t)_tcslen(pplot->curve[jj].legend.text);
            bLegActive = false;
            if ((jj == pplot->curveactive) && (nlen < (ML_STRSIZE - 2))) {
                pplot->curve[jj].legend.text[nlen] = _T(' ');
                pplot->curve[jj].legend.text[nlen + 1] = _T('|');
                pplot->curve[jj].legend.text[nlen + 2] = _T('\0');
                nlen += 2;
                bLegActive = true;
            }
            // Adapter � la taille du texte
            Gdiplus::RectF rcF;
            Gdiplus::PointF ptF;
            if (pplot->canvas->MeasureString(pplot->curve[jj].legend.text, nlen, pFont, ptF, &rcF) == Gdiplus::Status::Ok) {
                sizeText.cx = (int_t) (rcF.Width);
                sizeText.cy = (int_t) (rcF.Height);
                if (iFirstLegend >= 0) {
                    pplot->curve[jj].lrelpos.xa = fT;
                    pplot->curve[jj].lrelpos.ya = fTT;
                }
                else {
                    fT = pplot->curve[jj].lrelpos.xa;
                    fTT = pplot->curve[jj].lrelpos.yb;
                }
                pplot->curve[jj].lrelpos.xb = pplot->curve[jj].lrelpos.xa + (((real_t) (sizeText.cx)) / pplot->plint.width);
                pplot->curve[jj].lrelpos.yb = pplot->curve[jj].lrelpos.ya + (((real_t) (sizeText.cy)) / pplot->plint.height);
                fT = pplot->curve[jj].lrelpos.xa;
                fTT = pplot->curve[jj].lrelpos.yb;
            }
            //

            fLegT = 20.0;
            pplot->curve[jj].lrelpos.xb += fLegT / pplot->plint.width;

            recTT.left = (LONG) round(pplot->plint.width * pplot->curve[jj].lrelpos.xa);
            recTT.right = (LONG) round(pplot->plint.width * pplot->curve[jj].lrelpos.xb);
            recTT.top = (LONG) round(pplot->plint.height * pplot->curve[jj].lrelpos.ya) + 2;
            recTT.bottom = (LONG) round(pplot->plint.height * pplot->curve[jj].lrelpos.yb) + 2;

            fTy = 1.0 + (((pplot->plint.height * pplot->curve[jj].lrelpos.ya) + (pplot->plint.height * pplot->curve[jj].lrelpos.yb)) / 2.0f);
            //
            if (bLine == 1) {
                setPenTX(pplot, &penLine);
                drawLineTX(pplot, (real_t) (recTT.left), fTy, (real_t) (recTT.left) + fLegT, fTy);
            }

            // Symbol:                0x02:.                    0x04:+        0x06:-        0x08:|
            //                            0x10:circle                0x20:square                    0x30:trapeze
            //                            0x40:triangle up        0x50:triangle down
            //                            0x60:triangle left    0x70:triangle right
            fx = 0;
            if (pplot->curve[jj].dotstyle != 0x00) {
                fTx = (pplot->plint.width * pplot->curve[jj].lrelpos.xa) + (fLegT / 2.0);
                fx = 2.0 * (real_t) (pplot->curve[jj].dotsize);
                if (fx > (fLegT - 4.0)) { fx = fLegT - 4.0; }
                fSizeT = (fx / 2.0);
                setPenTX(pplot, &penDot);
                pplot->brush = brDot;
                drawCurveDotTX(pplot, pplot->curve[jj].dotstyle, (byte_t) fSizeT, fTx, fTy - 0.5, &dotcolor, pplot->curve[jj].linesize, &dotlinecolor);
                setPenTX(pplot, &penLine);
            }

            recTT.left += (int_t) round(fLegT) + PL_RESIZE;

            rcF.X = (Gdiplus::REAL) (recTT.left);
            rcF.Y = (Gdiplus::REAL) (recTT.top);
            rcF.Width = (Gdiplus::REAL) (recTT.right - recTT.left);
            rcF.Height = (Gdiplus::REAL) (recTT.bottom - recTT.top);

            drawStringTX(pplot, (const char_t*) (pplot->curve[jj].legend.text), nlen, pFont, pBrush, &rcF, 0x00, &recTT, pFontSm, SVG_ALIGN_START);
            if (bLegActive) {
                pplot->curve[jj].legend.text[nlen - 2] = _T('\0');
                nlen -= 2;
            }

            // Sauvegarder la position
            pplot->plint.legendRect[jj].bottom = recTT.bottom;
            pplot->plint.legendRect[jj].top = recTT.top;
            pplot->plint.legendRect[jj].left = recTT.left - (int_t) round(fLegT) - PL_RESIZE;
            pplot->plint.legendRect[jj].right = recTT.right + 1;
    
            if (iFirstLegend >= 0) {
                pplot->plint.legendsRect.bottom += (recTT.bottom - recTT.top);
                if (pplot->plint.legendRect[jj].right > pplot->plint.legendsRect.right) {
                    pplot->plint.legendsRect.right = pplot->plint.legendRect[jj].right;
                }
            }
            else {
                pplot->plint.legendsRect.bottom = recTT.bottom;
                pplot->plint.legendsRect.top = recTT.top;
                pplot->plint.legendsRect.left = recTT.left - (int_t) round(fLegT) - PL_RESIZE;
                pplot->plint.legendsRect.right = recTT.right + 1;
                iFirstLegend = jj;
            }

            if (bLegActive) {
                pplot->plint.legendsRect.right += (7 + (int_t) round(fLegT));
                bLegActive = false;
            }
            else {
                pplot->plint.legendsRect.right += 7;
            }
        }
    }

    // Show curve axis legend
    if ((iFirstLegend >= 0) && (bMultiAxes == 1)) {
        for (jj = 0; jj < pplot->curvecount; jj++) {
            if ((pplot->curve[jj].x == NULL) || (pplot->curve[jj].y == NULL)) {
                continue;
            }
            if ((pplot->curve[jj].x->dat == NULL) || (pplot->curve[jj].y->dat == NULL)
                || ((pplot->curve[jj].status & 0x0F) == 0x0F) || (pplot->curve[jj].points < 1)) {
                continue;
            }
            if ((pplot->curve[jj].dotstyle == 0x00) && ((pplot->curve[jj].linestyle & 0xF0) == 0x00)) {
                continue;
            }
            if (pplot->curve[jj].olegend == 0) {
                continue;
            }
            setPenTX(pplot, &penFocus);

            if ((pplot->curve[jj].axisused & 0xF0) == 0x10) {        // Axis X2
                penAxisX->SetColor(Gdiplus::Color(pplot->axistop.color.r, pplot->axistop.color.g, pplot->axistop.color.b));
                penAxisX->SetWidth((Gdiplus::REAL)(pplot->axistop.size));
            }
            else {        // Axis X
                penAxisX->SetColor(Gdiplus::Color(pplot->axisbottom.color.r, pplot->axisbottom.color.g, pplot->axisbottom.color.b));
                penAxisX->SetWidth((Gdiplus::REAL)(pplot->axisbottom.size));
            }

            if ((pplot->curve[jj].axisused & 0x0F) == 0x01) {        // Axis Y2
                penAxisY->SetColor(Gdiplus::Color(pplot->axisright.color.r, pplot->axisright.color.g, pplot->axisright.color.b));
                penAxisY->SetWidth((Gdiplus::REAL)(pplot->axisright.size));
            }
            else {        // Axis Y
                penAxisY->SetColor(Gdiplus::Color(pplot->axisleft.color.r, pplot->axisleft.color.g, pplot->axisleft.color.b));
                penAxisY->SetWidth((Gdiplus::REAL)(pplot->axisleft.size));
            }

            int_t iDt = 0, iDb = 0;
            iDt = (pplot->axistop.size / 2);
            if (pplot->axistop.size > 1) {
                iDt += (pplot->axistop.size % 2);
            }
            iDb = (pplot->axisbottom.size / 2);
            if (pplot->axisbottom.size > 1) {
                iDb += (pplot->axisbottom.size % 2);
            }

            fLegT = (real_t) (pplot->plint.legendRect[jj].bottom - pplot->plint.legendRect[jj].top);
            switch (pplot->curve[jj].axisused) {
                case 0x00:
                    setPenTX(pplot, &penAxisX);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].bottom - 2),
                        (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].bottom - 2), false);

                    setPenTX(pplot, &penAxisY);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].top + 4),
                        (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].bottom - 2 + iDb), false);
                    break;

                case 0x10:
                    setPenTX(pplot, &penAxisX);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].top + 4),
                        (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].top + 4), false);

                    setPenTX(pplot, &penAxisY);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].top + 4 - iDt),
                        (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].bottom - 2), false);
                    break;

                case 0x01:
                    setPenTX(pplot, &penAxisX);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].bottom - 2),
                        (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].bottom - 2), false);

                    setPenTX(pplot, &penAxisY);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].top + 4),
                        (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].bottom - 2 + iDb), false);
                    break;

                case 0x11:
                    setPenTX(pplot, &penAxisX);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + 2), (real_t) (pplot->plint.legendRect[jj].top + 4),
                        (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].top + 4), false);

                    setPenTX(pplot, &penAxisY);
                    drawLineTX(pplot, (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].top + 4 - iDt),
                        (real_t) (pplot->plint.legendsRect.right + fLegT), (real_t) (pplot->plint.legendRect[jj].bottom - 2), false);
                    break;
            }
        }
    }

    pplot->errcode = 0;

    // Imprimer et tracer les �l�ments (texte, lignes, ...) en dernier.
    pl_draw_rect(pplot);
    pl_draw_ellipse(pplot);
    pl_draw_line(pplot);
    pl_print_text(pplot);

pl_plot_RET:
    if (pnx) { free(pnx); pnx = NULL; }
    if (pny) { free(pny); pny = NULL; }
    if (pFont) { delete(pFont); pFont = NULL; }
    if (pFontSm) { delete(pFontSm); pFontSm = NULL; }
    if (brDot) { delete(brDot); brDot = NULL; }
    if (pBrush) { delete(pBrush); pBrush = NULL; }
    if (penLine) { delete(penLine); penLine = NULL; }
    if (penDot) { delete(penDot); penDot = NULL; }
    if (penDrop) { delete(penDrop); penDrop = NULL; }
    if (penFocus) { delete(penFocus); penFocus = NULL; }
    if (penBarX) { delete(penBarX); penBarX = NULL; }
    if (penBarY) { delete(penBarY); penBarY = NULL; }
    if (penAxisX) { delete(penAxisX); penAxisX = NULL; }
    if (penAxisY) { delete(penAxisY); penAxisY = NULL; }
    if (pplot->canvas) { delete(pplot->canvas); pplot->canvas = NULL; }
    if ((pplot->imagetype == 2) && (pplot->svgdoc != NULL)) {
        pplot->imagesaved = pplot->svgdoc->save();
    }
    if (pplot->svgdoc) { delete(pplot->svgdoc); pplot->svgdoc = NULL; }
    if (pplot->metafile) { delete(pplot->metafile); pplot->metafile = NULL; }
    if (pplot->image) { delete(pplot->image); pplot->image = NULL; }
    pplot->imagetype = 0;
    setPenTX(pplot, NULL);
    pplot->brush = NULL;
    pplot->font = NULL;
    pplot->plint.running = 0;
    return;
}

/*  --------------------------------------------------------------------
    pl_curve_add          :    Ajouter une courbe au trac�

        pplot             :    pointeur sur la structure du tracé
        pvecX, pvecY      :    pointeurs sur les vecteurs X et Y
        pvecEX, pvecEY    :    pointeurs sur les vecteurs barres d'erreur X et Y
        axisused          :    axes : 0x00 (XY), 0x01 (XY2), 0x10 (X2Y), 0x11 (X2Y2)

        retourne l'indice de la courbe
    -------------------------------------------------------------------- */
int_t pl_curve_add(plot_t* pplot, vector_t *pvecX, vector_t *pvecY,
                         vector_t *pvecEX, vector_t *pvecEY, byte_t axisused,
                         byte_t autoformat)
{
    int_t iStyle = FW_NORMAL;
    BOOL bItalic = FALSE, bUnderline = FALSE;

    if (pplot == NULL) {
        return -1;
    }

    if (pplot->errcode) { return -1; }

    pplot->errcode = 1;

    if ((pvecX == NULL) || (pvecY == NULL)) {
        pplot->errcode = 0;
        return -1;
    }
    if ((pvecX->dat == NULL) || (pvecY->dat == NULL)
        || (pvecX->dim < 1) || (pvecX->dim > ML_MAXPOINTS)
        || (pvecX->dim != pvecY->dim)) {
        pplot->errcode = 0;
        return -1;
    }
    // Il faut que les colonnes soient du type ad hoc
    if (((pvecX->type & 0xF0) != 0x10) || ((pvecY->type & 0xF0) != 0x20)) {
        pplot->errcode = 0;
        return -1;
    }
    if ((pplot->curvecount < 0) || (pplot->curvecount > ML_MAXCURVES)) {
        return -1;
    }

    // vérifier que la courbe n'est pas d�j� trac�e
    if (pplot->curvecount > 0) {
        int_t jj;
        for (jj = 0; jj < pplot->curvecount; jj++) {
            if (pplot->curve[jj].y->id == pvecY->id) {
                pplot->curve[jj].status &= 0xF0; // Faire appara�tre (au cas où la courbe serait cach�e)
                // :BEGIN:DEBUG:20140111: si la courbe existe, ajouter la possibilit� de changer son X (et uniquement X)
                pvecY->idx = pvecX->id;
                pplot->curve[jj].x = pvecX;
                pplot->curve[jj].y = pvecY;
                pplot->curve[jj].idx = pplot->curve[jj].x->id;
                pplot->curve[jj].idy = pplot->curve[jj].y->id;
                // :END:
                pplot->errcode = 0;
                return jj;
            }
        }
    }
    if (pplot->curvecount >= ML_MAXCURVES) {
        _tcscpy(pplot->message, _T("Maximum number of curves reached."));
        return -1;
    }

    pvecY->idx = pvecX->id;

    pl_reset_curve(pplot, pplot->curvecount, 0);
    if ((pplot->curve[pplot->curvecount].legend.font.style & 0xF0) == 0x10) {
        iStyle = FW_BOLD;
    }
    if ((pplot->curve[pplot->curvecount].legend.font.style & 0x01) != 0x00) {
        bItalic = FALSE;
    }
    if ((pplot->curve[pplot->curvecount].legend.font.style & 0x02) != 0x00) {
        bUnderline = TRUE;
    }

    pplot->curve[pplot->curvecount].x = pvecX;
    pplot->curve[pplot->curvecount].points = pvecX->dim;
    pplot->curve[pplot->curvecount].y = pvecY;
    pplot->curve[pplot->curvecount].y->status = 0x00;
    pplot->curve[pplot->curvecount].ex = pvecEX;
    pplot->curve[pplot->curvecount].ey = pvecEY;
    pplot->curve[pplot->curvecount].idx = pplot->curve[pplot->curvecount].x->id;
    pplot->curve[pplot->curvecount].idy = pplot->curve[pplot->curvecount].y->id;
    pplot->curve[pplot->curvecount].axisused = axisused;
    if ((autoformat != 0x00) && (pplot->curvecount == 0)) {
        _tcscpy(pplot->axisbottom.format, (const char_t*)(pvecX->format));
        _tcscpy(pplot->axisleft.format, (const char_t*)(pvecY->format));
        _tcscpy(pplot->axistop.format, (const char_t*)(pvecX->format));
        _tcscpy(pplot->axisright.format, (const char_t*)(pvecY->format));
    }

    pplot->curvecount += 1;

    pplot->errcode = 0;
    return (pplot->curvecount - 1);
}

/*  --------------------------------------------------------------------
    pl_curve_modify     :    Modifier les �l�ments dynamiques d'une courbe

        pplot           :    pointeur sur la structure du tracé
        pxOld, pyOld    :    pointeurs sur les anciens vecteurs X et Y
        pxNew, pyNew    :    pointeurs sur les nouveaux vecteurs X et Y
        pEX, pEY        :    pointeurs sur les nouveaux vecteurs barres
                             d'erreur X et Y
    -------------------------------------------------------------------- */
void pl_curve_modify(plot_t* pplot, vector_t *pxOld, vector_t *pyOld,
                     vector_t *pxNew, vector_t *pyNew,
                     vector_t *pEX, vector_t *pEY)
{
    int_t jj;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    pplot->errcode = 1;

    if ((pxOld == NULL) || (pyOld == NULL)
        || (pxNew == NULL) || (pyNew == NULL)) {
        pplot->errcode = 0;
        return;
    }
    if ((pxOld->dat == NULL) || (pyOld->dat == NULL)
        || (pxOld->dim < 1) || (pxOld->dim > ML_MAXPOINTS)
        || (pxOld->dim != pyOld->dim)) {
        pplot->errcode = 0;
        return;
    }
    if ((pxNew->dat == NULL) || (pyNew->dat == NULL)
        || (pxNew->dim < 1) || (pxNew->dim > ML_MAXPOINTS)
        || (pxNew->dim != pyNew->dim)) {
        pplot->errcode = 0;
        return;
    }
    // Il faut que les colonnes soient du type ad hoc
    if (((pxNew->type & 0xF0) != 0x10) || ((pyNew->type & 0xF0) != 0x20)) {
        pplot->errcode = 0;
        return;
    }

    if (pplot->curvecount < 1) {
        return;
    }

    // vérifier que la courbe est d�j� trac�e et la modifier le cas �ch�ant
    for (jj = 0; jj < pplot->curvecount; jj++) {
        if (pplot->curve[jj].y->id == pyOld->id) {
            pplot->curve[jj].x = pxNew;
            pplot->curve[jj].points = pxNew->dim;
            pyNew->status = 0x00;
            pplot->curve[jj].y = pyNew;
            pyNew->idx = pxNew->id;
            pplot->curve[jj].ex = pEX;
            pplot->curve[jj].ey = pEY;
            pplot->curve[jj].idx = pplot->curve[jj].x->id;
            pplot->curve[jj].idy = pplot->curve[jj].y->id;
            pplot->errcode = 0;
            return;
        }
    }

    pplot->errcode = 0;
    return;
}

/*  --------------------------------------------------------------------
    pl_curve_removei    :    Enlever une courbe du trac�

        pplot           :    pointeur sur la structure du tracé
        id              :    ID (unique) de la colonne Y
    -------------------------------------------------------------------- */
void pl_curve_removei(plot_t* pplot, long_t id)
{
    int_t jj, ll;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    pplot->errcode = 1;

    if (pplot->curvecount < 1) {
        pplot->errcode = 0;
        return;
    }

    for (jj = 0; jj < pplot->curvecount; jj++) {
        if (pplot->curve[jj].idy == id) {
            if (jj == (pplot->curvecount - 1)) {
                pl_reset_curve(pplot, jj, 0);
            }
            else {
                for (ll = jj; ll < (pplot->curvecount - 1); ll++) {
                    pplot->errcode = 0;
                    pl_reset_curve(pplot, ll, 0);
                    pl_curve_dup(pplot, ll, ll+1);
                }
                pl_reset_curve(pplot, pplot->curvecount - 1, 0);
            }
            pplot->curvecount -= 1;
            pplot->errcode = 0;
            return;
        }
    }
    pplot->errcode = 0;
}

/*  --------------------------------------------------------------------
    pl_curve_remove    :    Enlever une courbe du trac�

        pplot          :    pointeur sur la structure du tracé
        iCurve         :    indice de la courbe � enlever
    -------------------------------------------------------------------- */
void pl_curve_remove(plot_t* pplot, int_t iCurve)
{
    int_t jj;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }

    pplot->errcode = 1;

    if ((iCurve < 0) || (iCurve >= pplot->curvecount)) {
        pplot->errcode = 0;
        return;
    }

    if (iCurve == (pplot->curvecount - 1)) {
        pl_reset_curve(pplot, iCurve, 0);
    }
    else {
        for (jj = iCurve; jj < (pplot->curvecount - 1); jj++) {
            pplot->errcode = 0;
            pl_reset_curve(pplot, jj, 0);
            pl_curve_dup(pplot, jj, jj + 1);
        }
        pl_reset_curve(pplot, pplot->curvecount - 1, 0);
    }

    pplot->curvecount -= 1;

    pplot->errcode = 0;
}

/*  --------------------------------------------------------------------
    pl_curve_dup        :    Dupliquer une courbe du trac�

        pplot           :    pointeur sur la structure du tracé
        iFrom           :    indice de la courbe source
        iTo             :    indice de la courbe destination
    -------------------------------------------------------------------- */
void pl_curve_dup(plot_t* pplot, int_t iTo, int_t iFrom)
{
    int_t iStyle = FW_NORMAL;
    BOOL bItalic = FALSE, bUnderline = FALSE;

    if (pplot == NULL) {
        return;
    }

    if (pplot->errcode) { return; }
    pplot->errcode = 1;

    if ((iFrom < 0) || (iFrom >= pplot->curvecount) || (iTo < 0) || (iTo >= pplot->curvecount)) {
        pplot->errcode = 0;
        return;
    }

    pplot->curve[iTo].points = pplot->curve[iFrom].points;
    pplot->curve[iTo].x = pplot->curve[iFrom].x;
    pplot->curve[iTo].y = pplot->curve[iFrom].y;
    pplot->curve[iTo].ex = pplot->curve[iFrom].ex;
    pplot->curve[iTo].ey = pplot->curve[iFrom].ey;
    pplot->curve[iTo].idx = pplot->curve[iFrom].idx;
    pplot->curve[iTo].idy = pplot->curve[iFrom].idy;
    pplot->curve[iTo].status = pplot->curve[iFrom].status;
    pplot->curve[iTo].axisused = pplot->curve[iFrom].axisused;
    pplot->curve[iTo].linestyle = pplot->curve[iFrom].linestyle;
    pplot->curve[iTo].linesize = pplot->curve[iFrom].linesize;
    pplot->curve[iTo].linecolor.r = pplot->curve[iFrom].linecolor.r;
    pplot->curve[iTo].linecolor.g = pplot->curve[iFrom].linecolor.g;
    pplot->curve[iTo].linecolor.b = pplot->curve[iFrom].linecolor.b;
    pplot->curve[iTo].exstyle = pplot->curve[iFrom].exstyle;
    pplot->curve[iTo].exsize = pplot->curve[iFrom].exsize;
    pplot->curve[iTo].excolor.r = pplot->curve[iFrom].excolor.r;
    pplot->curve[iTo].excolor.g = pplot->curve[iFrom].excolor.g;
    pplot->curve[iTo].excolor.b = pplot->curve[iFrom].excolor.b;
    pplot->curve[iTo].eystyle = pplot->curve[iFrom].eystyle;
    pplot->curve[iTo].eysize = pplot->curve[iFrom].eysize;
    pplot->curve[iTo].eycolor.r = pplot->curve[iFrom].eycolor.r;
    pplot->curve[iTo].eycolor.g = pplot->curve[iFrom].eycolor.g;
    pplot->curve[iTo].eycolor.b = pplot->curve[iFrom].eycolor.b;
    pplot->curve[iTo].dotstyle = pplot->curve[iFrom].dotstyle;
    pplot->curve[iTo].dotsize = pplot->curve[iFrom].dotsize;
    pplot->curve[iTo].dotcolor.r = pplot->curve[iFrom].dotcolor.r;
    pplot->curve[iTo].dotcolor.g = pplot->curve[iFrom].dotcolor.g;
    pplot->curve[iTo].dotcolor.b = pplot->curve[iFrom].dotcolor.b;
    pplot->curve[iTo].dropstyleh = pplot->curve[iFrom].dropstyleh;
    pplot->curve[iTo].dropstylev = pplot->curve[iFrom].dropstylev;
    pplot->curve[iTo].dropsizeh = pplot->curve[iFrom].dropsizeh;
    pplot->curve[iTo].dropcolorh.r = pplot->curve[iFrom].dropcolorh.r;
    pplot->curve[iTo].dropcolorh.g = pplot->curve[iFrom].dropcolorh.g;
    pplot->curve[iTo].dropcolorh.b = pplot->curve[iFrom].dropcolorh.b;
    pplot->curve[iTo].dropsizev = pplot->curve[iFrom].dropsizev;
    pplot->curve[iTo].dropcolorv.r = pplot->curve[iFrom].dropcolorv.r;
    pplot->curve[iTo].dropcolorv.g = pplot->curve[iFrom].dropcolorv.g;
    pplot->curve[iTo].dropcolorv.b = pplot->curve[iFrom].dropcolorv.b;
    pplot->curve[iTo].axisused = pplot->curve[iFrom].axisused;

    _tcscpy(pplot->curve[iTo].legend.font.name, pplot->curve[iFrom].legend.font.name);
    pplot->curve[iTo].legend.font.size = pplot->curve[iFrom].legend.font.size;
    pplot->curve[iTo].legend.font.style = pplot->curve[iFrom].legend.font.style;
    pplot->curve[iTo].legend.font.orient = pplot->curve[iFrom].legend.font.orient;
    pplot->curve[iTo].legend.font.color.r = pplot->curve[iFrom].legend.font.color.r;
    pplot->curve[iTo].legend.font.color.g = pplot->curve[iFrom].legend.font.color.g;
    pplot->curve[iTo].legend.font.color.b = pplot->curve[iFrom].legend.font.color.b;
    // font.ptr never allocated
    pplot->curve[iTo].legend.font.ptr = NULL;
    if ((pplot->curve[iTo].legend.font.style & 0xF0) == 0x10) {
        iStyle = FW_BOLD;
    }
    if ((pplot->curve[iTo].legend.font.style & 0x01) != 0x00) {
        bItalic = FALSE;
    }
    if ((pplot->curve[iTo].legend.font.style & 0x02) != 0x00) {
        bUnderline = TRUE;
    }
    // font.ptr never allocated
    pplot->curve[iTo].legend.font.ptr = NULL;
    pplot->curve[iTo].olegend = pplot->curve[iFrom].olegend;

    pplot->curve[iTo].lrelpos.xa = pplot->curve[iFrom].lrelpos.xa;
    pplot->curve[iTo].lrelpos.xb = pplot->curve[iFrom].lrelpos.xb;
    pplot->curve[iTo].lrelpos.ya = pplot->curve[iFrom].lrelpos.ya;
    pplot->curve[iTo].lrelpos.yb = pplot->curve[iFrom].lrelpos.yb;

    pplot->plint.curvePointA[iTo] = pplot->plint.curvePointA[iFrom];
    pplot->plint.curvePointB[iTo] = pplot->plint.curvePointB[iFrom];

    pplot->errcode = 0;
}

/*  --------------------------------------------------------------------
    pl_plot_dupstyle    :    Dupliquer le style d'un trac�

        pFrom           :    pointeur sur le trac� source
        pTo             :    pointeur sur le trac� destination
    -------------------------------------------------------------------- */
void pl_plot_dupstyle(plot_t* pTo, const plot_t* pFrom)
{
    int_t ii;
    axis_t *paxisTo[4], *paxisFrom[4];
    int_t iStyle = FW_NORMAL;
    BOOL bItalic = FALSE, bUnderline = FALSE;

    if (pTo->errcode) { return; }

    if ((pTo == NULL) || (pFrom == NULL)) {
        return;
    }

    pTo->errcode = 1;

    pTo->backcolor.r = pFrom->backcolor.r;
    pTo->backcolor.g = pFrom->backcolor.g;
    pTo->backcolor.b = pFrom->backcolor.b;
    pTo->pbackcolor.r = pFrom->pbackcolor.r;
    pTo->pbackcolor.g = pFrom->pbackcolor.g;
    pTo->pbackcolor.b = pFrom->pbackcolor.b;
    _tcscpy(pTo->title.font.name, (const char_t*)(pFrom->title.font.name));
    pTo->title.font.size = pFrom->title.font.size;
    pTo->title.font.style = pFrom->title.font.style;
    pTo->title.font.orient = pFrom->title.font.orient;
    pTo->title.font.color.r = pFrom->title.font.color.r;
    pTo->title.font.color.g = pFrom->title.font.color.g;
    pTo->title.font.color.b = pFrom->title.font.color.b;
    // font.ptr never allocated
    pTo->title.font.ptr = NULL;

    iStyle = FW_NORMAL;
    bItalic = FALSE;
    if ((pTo->title.font.style & 0xF0) == 0x10) {
        iStyle = FW_BOLD;
    }
    if ((pTo->title.font.style & 0x01) != 0x00) {
        bItalic = FALSE;
    }
    if ((pTo->title.font.style & 0x02) != 0x00) {
        bUnderline = TRUE;
    }
    pTo->otitle = pFrom->otitle;

    paxisTo[0] = &(pTo->axisbottom);
    paxisTo[1] = &(pTo->axisleft);
    paxisTo[2] = &(pTo->axistop);
    paxisTo[3] = &(pTo->axisright);
    paxisFrom[0] = (axis_t*)(&(pFrom->axisbottom));
    paxisFrom[1] = (axis_t*)(&(pFrom->axisleft));
    paxisFrom[2] = (axis_t*)(&(pFrom->axistop));
    paxisFrom[3] = (axis_t*)(&(pFrom->axisright));

    // Copy Axis Style
    for (ii = 0; ii < 4; ii++) {
    // Label
        paxisTo[ii]->status = paxisFrom[ii]->status;
        paxisTo[ii]->scale = paxisFrom[ii]->scale;
        _tcscpy(paxisTo[ii]->lfont.name, paxisFrom[ii]->lfont.name);
        paxisTo[ii]->lfont.color.r = paxisFrom[ii]->lfont.color.r;
        paxisTo[ii]->lfont.color.g = paxisFrom[ii]->lfont.color.g;
        paxisTo[ii]->lfont.color.b = paxisFrom[ii]->lfont.color.b;
        paxisTo[ii]->lfont.orient = paxisFrom[ii]->lfont.orient;
        paxisTo[ii]->lfont.size = paxisFrom[ii]->lfont.size;
        paxisTo[ii]->lfont.style = paxisFrom[ii]->lfont.style;
        // font.ptr never allocated
        paxisTo[ii]->lfont.ptr = NULL;
        iStyle = FW_NORMAL;
        bItalic = FALSE;
        if ((paxisTo[ii]->lfont.style & 0xF0) == 0x10) {
            iStyle = FW_BOLD;
        }
        if ((paxisTo[ii]->lfont.style & 0x01) != 0x00) {
            bItalic = FALSE;
        }
        if ((paxisTo[ii]->lfont.style & 0x02) != 0x00) {
            bUnderline = TRUE;
        }

        // font.ptr never allocated
        paxisTo[ii]->lfont.ptr = NULL;

        // Title
        _tcscpy(paxisTo[ii]->title.font.name, paxisFrom[ii]->title.font.name);
        paxisTo[ii]->title.font.color.r = paxisFrom[ii]->title.font.color.r;
        paxisTo[ii]->title.font.color.g = paxisFrom[ii]->title.font.color.g;
        paxisTo[ii]->title.font.color.b = paxisFrom[ii]->title.font.color.b;
        paxisTo[ii]->title.font.orient = paxisFrom[ii]->title.font.orient;
        paxisTo[ii]->title.font.size = paxisFrom[ii]->title.font.size;
        paxisTo[ii]->title.font.style = paxisFrom[ii]->title.font.style;
        // font.ptr never allocated
        paxisTo[ii]->title.font.ptr = NULL;
        iStyle = FW_NORMAL;
        bItalic = FALSE;
        if ((paxisTo[ii]->title.font.style & 0xF0) == 0x10) {
            iStyle = FW_BOLD;
        }
        if ((paxisTo[ii]->title.font.style & 0x01) != 0x00) {
            bItalic = FALSE;
        }
        if ((paxisTo[ii]->title.font.style & 0x02) != 0x00) {
            bUnderline = TRUE;
        }
        // font.ptr never allocated
        paxisTo[ii]->title.font.ptr = NULL;
        // Format
        _tcscpy(paxisTo[ii]->format, paxisFrom[ii]->format);
        // Colors
        paxisTo[ii]->color.r = paxisFrom[ii]->color.r;
        paxisTo[ii]->color.g = paxisFrom[ii]->color.g;
        paxisTo[ii]->color.b = paxisFrom[ii]->color.b;
        paxisTo[ii]->gridcolor.r = paxisFrom[ii]->gridcolor.r;
        paxisTo[ii]->gridcolor.g = paxisFrom[ii]->gridcolor.g;
        paxisTo[ii]->gridcolor.b = paxisFrom[ii]->gridcolor.b;
        paxisTo[ii]->gridmcolor.r = paxisFrom[ii]->gridmcolor.r;
        paxisTo[ii]->gridmcolor.g = paxisFrom[ii]->gridmcolor.g;
        paxisTo[ii]->gridmcolor.b = paxisFrom[ii]->gridmcolor.b;
        paxisTo[ii]->tickcolor.r = paxisFrom[ii]->tickcolor.r;
        paxisTo[ii]->tickcolor.g = paxisFrom[ii]->tickcolor.g;
        paxisTo[ii]->tickcolor.b = paxisFrom[ii]->tickcolor.b;
        paxisTo[ii]->tickmcolor.r = paxisFrom[ii]->tickmcolor.r;
        paxisTo[ii]->tickmcolor.g = paxisFrom[ii]->tickmcolor.g;
        paxisTo[ii]->tickmcolor.b = paxisFrom[ii]->tickmcolor.b;
        // Grid
        paxisTo[ii]->gridcount = paxisFrom[ii]->gridcount;
        paxisTo[ii]->gridmcount = paxisFrom[ii]->gridcount;
        // Size
        paxisTo[ii]->size = paxisFrom[ii]->size;
        paxisTo[ii]->gridsize = paxisFrom[ii]->gridsize;
        paxisTo[ii]->gridmsize = paxisFrom[ii]->gridmsize;
        paxisTo[ii]->ticksize = paxisFrom[ii]->ticksize;
        paxisTo[ii]->tickmsize = paxisFrom[ii]->tickmsize;
        paxisTo[ii]->ticklen = paxisFrom[ii]->ticklen;
        paxisTo[ii]->tickmlen = paxisFrom[ii]->tickmlen;
        paxisTo[ii]->tickstyle = paxisFrom[ii]->tickstyle;
        paxisTo[ii]->tickmstyle = paxisFrom[ii]->tickmstyle;
        // Options
        paxisTo[ii]->ogrid = paxisFrom[ii]->ogrid;
        paxisTo[ii]->ogridm = paxisFrom[ii]->ogridm;
        paxisTo[ii]->otick = paxisFrom[ii]->otick;
        paxisTo[ii]->otickm = paxisFrom[ii]->otickm;
        paxisTo[ii]->olabel = paxisFrom[ii]->olabel;
        paxisTo[ii]->otitle = paxisFrom[ii]->otitle;
    }

    // Copy Curves Style
    for (ii = 0; ii < ML_MAXCURVES; ii++) {
        pTo->curve[ii].axisused = pFrom->curve[ii].axisused;
        pTo->curve[ii].linestyle = pFrom->curve[ii].linestyle;
        pTo->curve[ii].linesize = pFrom->curve[ii].linesize;
        pTo->curve[ii].linecolor.r = pFrom->curve[ii].linecolor.r;
        pTo->curve[ii].linecolor.g = pFrom->curve[ii].linecolor.g;
        pTo->curve[ii].linecolor.b = pFrom->curve[ii].linecolor.b;
        pTo->curve[ii].exstyle = pFrom->curve[ii].exstyle;
        pTo->curve[ii].exsize = pFrom->curve[ii].exsize;
        pTo->curve[ii].excolor.r = pFrom->curve[ii].excolor.r;
        pTo->curve[ii].excolor.g = pFrom->curve[ii].excolor.g;
        pTo->curve[ii].excolor.b = pFrom->curve[ii].excolor.b;
        pTo->curve[ii].eystyle = pFrom->curve[ii].eystyle;
        pTo->curve[ii].eysize = pFrom->curve[ii].eysize;
        pTo->curve[ii].eycolor.r = pFrom->curve[ii].eycolor.r;
        pTo->curve[ii].eycolor.g = pFrom->curve[ii].eycolor.g;
        pTo->curve[ii].eycolor.b = pFrom->curve[ii].eycolor.b;
        pTo->curve[ii].dotstyle = pFrom->curve[ii].dotstyle;
        pTo->curve[ii].dotsize = pFrom->curve[ii].dotsize;
        pTo->curve[ii].dotcolor.r = pFrom->curve[ii].dotcolor.r;
        pTo->curve[ii].dotcolor.g = pFrom->curve[ii].dotcolor.g;
        pTo->curve[ii].dotcolor.b = pFrom->curve[ii].dotcolor.b;
        pTo->curve[ii].dropstyleh = pFrom->curve[ii].dropstyleh;
        pTo->curve[ii].dropstylev = pFrom->curve[ii].dropstylev;
        pTo->curve[ii].dropsizeh = pFrom->curve[ii].dropsizeh;
        pTo->curve[ii].dropcolorh.r = pFrom->curve[ii].dropcolorh.r;
        pTo->curve[ii].dropcolorh.g = pFrom->curve[ii].dropcolorh.g;
        pTo->curve[ii].dropcolorh.b = pFrom->curve[ii].dropcolorh.b;
        pTo->curve[ii].dropsizev = pFrom->curve[ii].dropsizev;
        pTo->curve[ii].dropcolorv.r = pFrom->curve[ii].dropcolorv.r;
        pTo->curve[ii].dropcolorv.g = pFrom->curve[ii].dropcolorv.g;
        pTo->curve[ii].dropcolorv.b = pFrom->curve[ii].dropcolorv.b;

        _tcscpy(pTo->curve[ii].legend.font.name, pFrom->curve[ii].legend.font.name);
        pTo->curve[ii].legend.font.size = pFrom->curve[ii].legend.font.size;
        pTo->curve[ii].legend.font.style = pFrom->curve[ii].legend.font.style;
        pTo->curve[ii].legend.font.orient = pFrom->curve[ii].legend.font.orient;
        if ((pTo->curve[ii].linestyle & 0xF0) != 0x00) {
            pTo->curve[ii].legend.font.color.r = pTo->curve[ii].linecolor.r;
            pTo->curve[ii].legend.font.color.g = pTo->curve[ii].linecolor.g;
            pTo->curve[ii].legend.font.color.b = pTo->curve[ii].linecolor.b;
        }
        else if (pTo->curve[ii].dotstyle != 0x00) {
            pTo->curve[ii].legend.font.color.r = pTo->curve[ii].dotcolor.r;
            pTo->curve[ii].legend.font.color.g = pTo->curve[ii].dotcolor.g;
            pTo->curve[ii].legend.font.color.b = pTo->curve[ii].dotcolor.b;
        }
        // font.ptr never allocated
        pTo->curve[ii].legend.font.ptr = NULL;
        iStyle = FW_NORMAL;
        bItalic = FALSE;
        if ((pTo->curve[ii].legend.font.style & 0xF0) == 0x10) {
            iStyle = FW_BOLD;
        }
        if ((pTo->curve[ii].legend.font.style & 0x01) != 0x00) {
            bItalic = FALSE;
        }
        if ((pTo->curve[ii].legend.font.style & 0x02) != 0x00) {
            bUnderline = TRUE;
        }
        // font.ptr never allocated
        pTo->curve[ii].legend.font.ptr = NULL;
        pTo->curve[ii].olegend = pFrom->curve[ii].olegend;
    }

    for (ii = 0; ii < PL_MAXITEMS; ii++) {
        // Text
        pTo->text[ii].border = pFrom->text[ii].border;
        _tcscpy(pTo->text[ii].font.name, pFrom->text[ii].font.name);
        pTo->text[ii].font.size = pFrom->text[ii].font.size;
        pTo->text[ii].font.style = pFrom->text[ii].font.style;
        pTo->text[ii].font.orient = pFrom->text[ii].font.orient;
        pTo->text[ii].font.color.r = pFrom->text[ii].font.color.r;
        pTo->text[ii].font.color.g = pFrom->text[ii].font.color.g;
        pTo->text[ii].font.color.b = pFrom->text[ii].font.color.b;
        // font.ptr never allocated
        pTo->text[ii].font.ptr = NULL;

        iStyle = FW_NORMAL;
        bItalic = FALSE;
        if ((pTo->text[ii].font.style & 0xF0) == 0x10) {
            iStyle = FW_BOLD;
        }
        if ((pTo->text[ii].font.style & 0x01) != 0x00) {
            bItalic = FALSE;
        }
        if ((pTo->text[ii].font.style & 0x02) != 0x00) {
            bUnderline = TRUE;
        }
        // font.ptr never allocated
        pTo->text[ii].font.ptr = NULL;

        // Line GEO
        pTo->line[ii].color.r = pFrom->line[ii].color.r;
        pTo->line[ii].color.g = pFrom->line[ii].color.g;
        pTo->line[ii].color.b = pFrom->line[ii].color.b;
        pTo->line[ii].arrlength = pFrom->line[ii].arrlength;
        pTo->line[ii].arrwidth = pFrom->line[ii].arrwidth;
        pTo->line[ii].arrstyle = pFrom->line[ii].arrstyle;
        pTo->line[ii].size = pFrom->line[ii].size;
        pTo->line[ii].style = pFrom->line[ii].style;
        // Rectangle GEO
        pTo->rectangle[ii].color.r = pFrom->rectangle[ii].color.r;
        pTo->rectangle[ii].color.g = pFrom->rectangle[ii].color.g;
        pTo->rectangle[ii].color.b = pFrom->rectangle[ii].color.b;
        pTo->rectangle[ii].fcolor.r = pFrom->rectangle[ii].fcolor.r;
        pTo->rectangle[ii].fcolor.g = pFrom->rectangle[ii].fcolor.g;
        pTo->rectangle[ii].fcolor.b = pFrom->rectangle[ii].fcolor.b;
        pTo->rectangle[ii].size = pFrom->rectangle[ii].size;
        pTo->rectangle[ii].style = pFrom->rectangle[ii].style;
        // Ellipse GEO
        pTo->ellipse[ii].color.r = pFrom->ellipse[ii].color.r;
        pTo->ellipse[ii].color.g = pFrom->ellipse[ii].color.g;
        pTo->ellipse[ii].color.b = pFrom->ellipse[ii].color.b;
        pTo->ellipse[ii].fcolor.r = pFrom->ellipse[ii].fcolor.r;
        pTo->ellipse[ii].fcolor.g = pFrom->ellipse[ii].fcolor.g;
        pTo->ellipse[ii].fcolor.b = pFrom->ellipse[ii].fcolor.b;
        pTo->ellipse[ii].size = pFrom->ellipse[ii].size;
        pTo->ellipse[ii].style = pFrom->ellipse[ii].style;

        // Lignes - Verticales
        pTo->linevert[ii].size = pFrom->linevert[ii].size;
        pTo->linevert[ii].style = pFrom->linevert[ii].style;
        pTo->linevert[ii].color.r = pFrom->linevert[ii].color.r;
        pTo->linevert[ii].color.g = pFrom->linevert[ii].color.g;
        pTo->linevert[ii].color.b = pFrom->linevert[ii].color.b;

        // Lignes - Horizontales
        pTo->linehorz[ii].size = pFrom->linehorz[ii].size;
        pTo->linehorz[ii].style = pFrom->linehorz[ii].style;
        pTo->linehorz[ii].color.r = pFrom->linehorz[ii].color.r;
        pTo->linehorz[ii].color.g = pFrom->linehorz[ii].color.g;
        pTo->linehorz[ii].color.b = pFrom->linehorz[ii].color.b;
    }

    // Positions

    // RECT WND
    pTo->relpos[0].xa = pFrom->relpos[0].xa;
    pTo->relpos[0].xb = pFrom->relpos[0].xb;
    pTo->relpos[0].ya = pFrom->relpos[0].ya;
    pTo->relpos[0].yb = pFrom->relpos[0].yb;

    // RECT PLOT
    pTo->relpos[1].xa = pFrom->relpos[1].xa;
    pTo->relpos[1].xb = pFrom->relpos[1].xb;
    pTo->relpos[1].ya = pFrom->relpos[1].ya;
    pTo->relpos[1].yb = pFrom->relpos[1].yb;

    // RECT TITLE
    pTo->relpos[2].xa = pFrom->relpos[2].xa;
    pTo->relpos[2].xb = pFrom->relpos[2].xb;
    pTo->relpos[2].ya = pFrom->relpos[2].ya;
    pTo->relpos[2].yb = pFrom->relpos[2].yb;

    pTo->errcode = 0;
}

/*  --------------------------------------------------------------------
    pl_plot_dupstylei    :    Dupliquer le style items [texte, g�o] d'un trac�

        pFrom            :    pointeur sur le trac� source
        pTo              :    pointeur sur le trac� destination
    -------------------------------------------------------------------- */
void pl_plot_dupstylei(plot_t* pTo, const plot_t* pFrom)
{
    int_t ii;
    int_t iStyle = FW_NORMAL;
    BOOL bItalic = FALSE, bUnderline = FALSE;

    if (pTo->errcode) { return; }

    if ((pTo == NULL) || (pFrom == NULL)) {
        return;
    }

    pTo->errcode = 1;

    for (ii = 0; ii < PL_MAXITEMS; ii++) {
        // Text
        pTo->text[ii].border = pFrom->text[ii].border;
        _tcscpy(pTo->text[ii].font.name, pFrom->text[ii].font.name);
        pTo->text[ii].font.size = pFrom->text[ii].font.size;
        pTo->text[ii].font.style = pFrom->text[ii].font.style;
        pTo->text[ii].font.orient = pFrom->text[ii].font.orient;
        pTo->text[ii].font.color.r = pFrom->text[ii].font.color.r;
        pTo->text[ii].font.color.g = pFrom->text[ii].font.color.g;
        pTo->text[ii].font.color.b = pFrom->text[ii].font.color.b;
        // font.ptr never allocated
        pTo->text[ii].font.ptr = NULL;
        // Line GEO
        pTo->line[ii].color.r = pFrom->line[ii].color.r;
        pTo->line[ii].color.g = pFrom->line[ii].color.g;
        pTo->line[ii].color.b = pFrom->line[ii].color.b;
        pTo->line[ii].arrlength = pFrom->line[ii].arrlength;
        pTo->line[ii].arrwidth = pFrom->line[ii].arrwidth;
        pTo->line[ii].arrstyle = pFrom->line[ii].arrstyle;
        pTo->line[ii].size = pFrom->line[ii].size;
        pTo->line[ii].style = pFrom->line[ii].style;
        // Rectangle GEO
        pTo->rectangle[ii].color.r = pFrom->rectangle[ii].color.r;
        pTo->rectangle[ii].color.g = pFrom->rectangle[ii].color.g;
        pTo->rectangle[ii].color.b = pFrom->rectangle[ii].color.b;
        pTo->rectangle[ii].fcolor.r = pFrom->rectangle[ii].fcolor.r;
        pTo->rectangle[ii].fcolor.g = pFrom->rectangle[ii].fcolor.g;
        pTo->rectangle[ii].fcolor.b = pFrom->rectangle[ii].fcolor.b;
        pTo->rectangle[ii].size = pFrom->rectangle[ii].size;
        pTo->rectangle[ii].style = pFrom->rectangle[ii].style;
        // Ellipse GEO
        pTo->ellipse[ii].color.r = pFrom->ellipse[ii].color.r;
        pTo->ellipse[ii].color.g = pFrom->ellipse[ii].color.g;
        pTo->ellipse[ii].color.b = pFrom->ellipse[ii].color.b;
        pTo->ellipse[ii].fcolor.r = pFrom->ellipse[ii].fcolor.r;
        pTo->ellipse[ii].fcolor.g = pFrom->ellipse[ii].fcolor.g;
        pTo->ellipse[ii].fcolor.b = pFrom->ellipse[ii].fcolor.b;
        pTo->ellipse[ii].size = pFrom->ellipse[ii].size;
        pTo->ellipse[ii].style = pFrom->ellipse[ii].style;

        // Lignes - Verticales
        pTo->linevert[ii].size = pFrom->linevert[ii].size;
        pTo->linevert[ii].style = pFrom->linevert[ii].style;
        pTo->linevert[ii].color.r = pFrom->linevert[ii].color.r;
        pTo->linevert[ii].color.g = pFrom->linevert[ii].color.g;
        pTo->linevert[ii].color.b = pFrom->linevert[ii].color.b;

        // Lignes - Horizontales
        pTo->linehorz[ii].size = pFrom->linehorz[ii].size;
        pTo->linehorz[ii].style = pFrom->linehorz[ii].style;
        pTo->linehorz[ii].color.r = pFrom->linehorz[ii].color.r;
        pTo->linehorz[ii].color.g = pFrom->linehorz[ii].color.g;
        pTo->linehorz[ii].color.b = pFrom->linehorz[ii].color.b;
    }

    pTo->errcode = 0;
}

/*  --------------------------------------------------------------------
    pl_plot_dupstatic    :    Dupliquer les �l�ments statiques d'un trac�

        pFrom            :    pointeur sur la structure du tracé source
        pTo              :    pointeur sur la structure du tracé destination
    -------------------------------------------------------------------- */
void pl_plot_dupstatic(plot_t* pTo, const plot_t* pFrom)
{
    int_t jj;

    if ((pTo == NULL) || (pFrom == NULL)) {
        return;
    }

    pTo->errcode = 1;

    memset(pTo, 0, sizeof(plot_t));
    memcpy(pTo, pFrom, sizeof(plot_t));

    // Annuler les �l�ments dynamiques
    pTo->dc = NULL;
    pTo->frame = NULL;
    pTo->title.font.ptr = NULL;
    pTo->axisbottom.title.font.ptr = NULL;
    pTo->axisleft.title.font.ptr = NULL;
    pTo->axistop.title.font.ptr = NULL;
    pTo->axisright.title.font.ptr = NULL;
    pTo->axisbottom.lfont.ptr = NULL;
    pTo->axisleft.lfont.ptr = NULL;
    pTo->axistop.lfont.ptr = NULL;
    pTo->axisright.lfont.ptr = NULL;
    pTo->curvecount = 0;
    pTo->curveactive = 0;
    for (jj = 0; jj < ML_MAXCURVES; jj++) {
        pTo->curve[jj].points = 0;
        pTo->curve[jj].x = NULL;
        pTo->curve[jj].y = NULL;
        pTo->curve[jj].ex = NULL;
        pTo->curve[jj].ey = NULL;
        pTo->curve[jj].legend.font.ptr = NULL;
    }
    for (jj = 0; jj < PL_MAXITEMS; jj++) {
        pTo->text[jj].font.ptr = NULL;
    }

    pTo->errcode = 0;
}

/*  --------------------------------------------------------------------
    pl_plot_dupstatic    :    Dupliquer les �l�ments statiques d'un trac�

    pFrom                :    pointeur sur la structure du tracé source
    pTo                  :    pointeur sur la structure du tracé destination
    -------------------------------------------------------------------- */
void pl_plot_dupstatic(plot_t* pTo, const plot_v24_t* pFrom)
{
    int_t jj;

    if ((pTo == NULL) || (pFrom == NULL)) {
        return;
    }

    pTo->errcode = 1;

    memset(pTo, 0, sizeof(plot_t));
    memcpy(pTo, pFrom, sizeof(plot_v24_t));

    // Annuler les �l�ments dynamiques
    pTo->dc = NULL;
    pTo->frame = NULL;
    pTo->canvas = NULL;
    pTo->pen = NULL;
    pTo->brush = NULL;
    pTo->title.font.ptr = NULL;
    pTo->axisbottom.title.font.ptr = NULL;
    pTo->axisleft.title.font.ptr = NULL;
    pTo->axistop.title.font.ptr = NULL;
    pTo->axisright.title.font.ptr = NULL;
    pTo->axisbottom.lfont.ptr = NULL;
    pTo->axisleft.lfont.ptr = NULL;
    pTo->axistop.lfont.ptr = NULL;
    pTo->axisright.lfont.ptr = NULL;
    pTo->curvecount = 0;
    pTo->curveactive = 0;
    for (jj = 0; jj < ML_MAXCURVES; jj++) {
        pTo->curve[jj].points = 0;
        pTo->curve[jj].x = NULL;
        pTo->curve[jj].y = NULL;
        pTo->curve[jj].ex = NULL;
        pTo->curve[jj].ey = NULL;
        pTo->curve[jj].legend.font.ptr = NULL;
    }
    for (jj = 0; jj < PL_MAXITEMS; jj++) {
        pTo->text[jj].font.ptr = NULL;
    }

    pTo->errcode = 0;
}
