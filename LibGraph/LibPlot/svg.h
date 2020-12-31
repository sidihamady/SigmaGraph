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

#ifndef LIBSVG_H
#define LIBSVG_H

#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#ifndef uT
#define uT(s) L ## s
#endif

enum SVG_ALIGN
{
    SVG_ALIGN_START = 1,
    SVG_ALIGN_MIDDLE,
    SVG_ALIGN_END
};

enum SVG_FONTSTYLE
{
    SVG_FONTSTYLE_REGULAR = 1,
    SVG_FONTSTYLE_BOLDITALIC,
    SVG_FONTSTYLE_BOLD
};

enum SVG_STROKESTYLE
{
    SVG_STROKESTYLE_SOLID = 1,
    SVG_STROKESTYLE_DASH,
    SVG_STROKESTYLE_DOT,
    SVG_STROKESTYLE_DASHDOT,
    SVG_STROKESTYLE_DASHDOTDOT,
    SVG_STROKESTYLE_NULL
};

#define SVG_COLOR_NONE    true
#define SVG_COLOR_FILL    false

struct svgPoint
{
    double x;
    double y;

    svgPoint(double tx = 0, double ty = 0)
    {
        x = tx;
        y = ty;
    }
};

struct svgLayout
{
    double width;
    double height;

    svgLayout(double twidth = 0.0, double theight = 0.0)
    {
        width = twidth;
        height = theight;
    }

    svgLayout & operator = (svgLayout const &layout)
    {
        width = layout.width;
        height = layout.height;
        return *this;
    }
};

template <typename T>
inline std::wstring xmlAttribute(const std::wstring &attributeName, T const &attributeValue, const std::wstring &attributeUnit = L"")
{
    std::wstringstream strT;
    strT << uT("\n\t") << attributeName << uT("=\"") << attributeValue << attributeUnit << uT("\" ");
    return strT.str();
}
inline std::wstring xmlElementStart(const std::wstring &tName, bool withAttributes = true)
{
    return uT("\n<") + tName + (withAttributes ? uT(" ") : uT(">"));
}
inline std::wstring xmlElementEnd(const std::wstring &tName)
{
    return uT("</") + tName + uT(">\n");
}
inline std::wstring xmlElementEnd()
{
    return uT("/>\n");
}

class svgColor
{
private:
    int red;
    int green;
    int blue;
    bool none;

public:
    svgColor()
    {
        none = false;
        red = 0;
        green = 0;
        blue = 0;
    }
    svgColor(const int tred, const int tgreen, const int tblue, const bool tnone = SVG_COLOR_FILL)
    {
        set(tred, tgreen, tblue, tnone);
    }
    virtual ~svgColor()
    {

    }

    void set(const int tred, const int tgreen, const int tblue, const bool tnone = SVG_COLOR_FILL)
    {
        red = tred;
        green = tgreen;
        blue = tblue;
        none = tnone;
    }
    void copyfrom(const svgColor &clrT)
    {
        red = clrT.red;
        green = clrT.green;
        blue = clrT.blue;
        blue = clrT.blue;
        none = clrT.none;
    }
    void copyto(svgColor &clrT) const
    {
        clrT.red = red;
        clrT.green = green;
        clrT.blue = blue;
        clrT.none = none;
    }

    std::wstring toString(svgLayout const &) const
    {
        std::wstringstream strT;
        if (none) {
            strT << uT("none");
        }
        else {
            strT << uT("rgb(") << red << uT(",") << green << uT(",") << blue << uT(")");
        }
        return strT.str();
    }
};

class svgFill
{
private:
    svgColor color;

public:
    explicit svgFill(const svgColor &tcolor = svgColor(0, 0, 0, true))
    {
        set(tcolor);
    }
    void set(const svgColor &tcolor)
    {
        color.copyfrom(tcolor);
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlAttribute(uT("fill"), color.toString(layout));
        return strT.str();
    }
};

class svgStroke
{
private:
    double width;
    svgColor color;
    SVG_STROKESTYLE style;

public:
    svgStroke(const double twidth = 1, const svgColor &tcolor = svgColor(0, 0, 0, false), const SVG_STROKESTYLE tstyle = SVG_STROKESTYLE_SOLID)
    {
        set(twidth, tcolor, tstyle);
    }
    void set(const double twidth, const svgColor &tcolor, const SVG_STROKESTYLE tstyle)
    {
        width = twidth;
        color.copyfrom(tcolor);
        style = tstyle;
    }
    void setStyle(const SVG_STROKESTYLE tstyle)
    {
        style = tstyle;
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstring strStyle = uT("");
        wchar_t szStyle[32];
        switch (style) {
            case SVG_STROKESTYLE_DOT:
#ifdef WIN32
                wsprintf(szStyle, uT("%d, %d"), (int) width, (int) width);
#else
                swprintf(szStyle, sizeof(szStyle) / sizeof(*szStyle), uT("%d, %d"), (int) width, (int) width);
#endif
                strStyle = xmlAttribute(uT("stroke-dasharray"), (const char*) szStyle);
                break;
            case SVG_STROKESTYLE_DASH:
#ifdef WIN32
                wsprintf(szStyle, uT("%d, %d"), 5 * (int) width, 5 * (int) width);
#else
                swprintf(szStyle, sizeof(szStyle) / sizeof(*szStyle), uT("%d, %d"), 5 * (int) width, 5 * (int) width);
#endif
                strStyle = xmlAttribute(uT("stroke-dasharray"), (const char*) szStyle);
                break;
            case SVG_STROKESTYLE_DASHDOT:
#ifdef WIN32
                wsprintf(szStyle, uT("%d, %d"), 5 * (int) width, (int) width);
#else
                swprintf(szStyle, sizeof(szStyle) / sizeof(*szStyle), uT("%d, %d"), 5 * (int) width, (int) width);
#endif
                strStyle = xmlAttribute(uT("stroke-dasharray"), (const char*) szStyle);
                break;
            default:
                break;
        }
        std::wstringstream strT;
        strT << xmlAttribute(uT("stroke-width"), width)
            << xmlAttribute(uT("stroke"), color.toString(layout))
            << strStyle;
        return strT.str();
    }
};

class svgFont
{
private:
    double size;
    std::wstring family;
    SVG_FONTSTYLE style;

public:
    svgFont(const double tsize = 12, const std::wstring &tfamily = uT("Verdana"), const SVG_FONTSTYLE tstyle = SVG_FONTSTYLE_REGULAR)
    {
        set(tsize, tfamily, tstyle);
    }

    void set(const double tsize, const std::wstring &tfamily, const SVG_FONTSTYLE tstyle)
    {
        size = tsize;
        family = tfamily;
        style = tstyle;
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlAttribute(uT("font-size"), size)
            << xmlAttribute(uT("font-family"), family)
            << (((style == SVG_FONTSTYLE_BOLD) || (style == SVG_FONTSTYLE_BOLDITALIC)) ? xmlAttribute(uT("font-weight"), uT("bold")) : uT(""))
            << ((style == SVG_FONTSTYLE_BOLDITALIC) ? xmlAttribute(uT("font-style"), uT("italic")) : uT(""));
        return strT.str();
    }
};

class svgShape
{
protected:
    svgFill fill;
    svgStroke stroke;

public:
    svgShape(svgFill const &tfill = svgFill(), svgStroke const &tstroke = svgStroke())
    {
        fill = tfill;
        stroke = tstroke;
    }
    virtual ~svgShape()
    {

    }

    virtual std::wstring toString(const svgLayout &layout) const = 0;
};

class svgLine : public svgShape
{
private:
    svgPoint p1;
    svgPoint p2;
    std::wstring join;
    std::wstring cap;

public:
    svgLine(const svgPoint &tp1, const svgPoint &tp2,
        const svgStroke &tstroke, const svgFill &tfill = svgFill(),
        const std::wstring tjoin = uT("round"),
        const std::wstring tcap = uT("round"))
    {
        set(tp1, tp2);
        stroke = tstroke;
        fill = tfill;
        join = tjoin;
        cap = tcap;
    }

    void set(const svgPoint &tp1, const svgPoint &tp2)
    {
        p1 = tp1;
        p2 = tp2;
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlElementStart(uT("line"))
            << xmlAttribute(uT("stroke-linejoin"), join)
            << xmlAttribute(uT("stroke-linecap"), cap)
            << xmlAttribute(uT("x1"), p1.x)
            << xmlAttribute(uT("y1"), p1.y)
            << xmlAttribute(uT("x2"), p2.x)
            << xmlAttribute(uT("y2"), p2.y)
            << stroke.toString(layout)
            << xmlElementEnd();
        return strT.str();
    }
};

class svgRectangle : public svgShape
{
private:
    svgPoint edge;
    double width;
    double height;

public:
    svgRectangle(const svgPoint &tedge, const double twidth, const double theight,
        const svgStroke &tstroke, const svgFill &tfill = svgFill())
    {
        set(tedge, twidth, theight);
        stroke = tstroke;
        fill = tfill;
    }

    void set(const svgPoint &tcenter, const double twidth, const double theight)
    {
        edge = tcenter;
        width = twidth;
        height = theight;
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlElementStart(uT("rect"))
            << xmlAttribute(uT("x"), edge.x)
            << xmlAttribute(uT("y"), edge.y)
            << xmlAttribute(uT("width"), width)
            << xmlAttribute(uT("height"), height)
            << fill.toString(layout)
            << stroke.toString(layout)
            << xmlElementEnd();
        return strT.str();
    }
};

class svgEllipse : public svgShape
{
private:
    svgPoint center;
    double rwidth;
    double rheight;

public:
    svgEllipse(const svgPoint &tcenter, const double trwidth, const double trheight,
        const svgStroke &tstroke, const svgFill &tfill = svgFill())
    {
        set(tcenter, trwidth, trheight);
        stroke = tstroke;
        fill = tfill;
    }

    void set(const svgPoint &tcenter, const double trwidth, const double trheight)
    {
        center = tcenter;
        rwidth = trwidth;
        rheight = trheight;
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlElementStart(uT("ellipse"))
            << xmlAttribute(uT("cx"), center.x)
            << xmlAttribute(uT("cy"), center.y)
            << xmlAttribute(uT("rx"), rwidth)
            << xmlAttribute(uT("ry"), rheight)
            << fill.toString(layout)
            << stroke.toString(layout)
            << xmlElementEnd();
        return strT.str();
    }
    void offset(svgPoint const &offset)
    {
        center.x += offset.x;
        center.y += offset.y;
    }
};

class svgPolyline : public svgShape
{
private:
    std::vector<svgPoint> points;

public:
    svgPolyline(const svgStroke &tstroke, const svgFill &tfill = svgFill())
    {
        stroke = tstroke;
        fill = tfill;
    }

    svgPolyline & operator << (svgPoint const &point)
    {
        points.push_back(point);
        return *this;
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlElementStart(uT("polyline"));
        strT << uT("points=\"");
        for (unsigned int ii = 0; ii < points.size(); ii++) {
            strT << (points[ii].x) << uT(",") << (points[ii].y) << uT(" ");
        }
        strT << uT("\" ");
        strT << fill.toString(layout)
            << stroke.toString(layout)
            << xmlElementEnd();
        return strT.str();
    }
};

class svgPolygon : public svgShape
{
private:
    std::vector<svgPoint> points;

public:
    svgPolygon(const svgStroke &tstroke, const svgFill &tfill = svgFill())
    {
        stroke = tstroke;
        fill = tfill;
    }

    svgPolygon & operator << (svgPoint const &point)
    {
        points.push_back(point);
        return *this;
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlElementStart(uT("polygon"));
        strT << uT("points=\"");
        for (unsigned int ii = 0; ii < points.size(); ii++) {
            strT << (points[ii].x) << uT(",") << (points[ii].y) << uT(" ");
        }
        strT << uT("\" ");
        strT << fill.toString(layout)
            << stroke.toString(layout)
            << xmlElementEnd();
        return strT.str();
    }
};

class svgText : public svgShape
{
private:
    std::wstring content;
    svgPoint position;
    std::wstring orient;
    svgFont font;
    svgColor fontcolor;
    SVG_ALIGN align;

public:
    svgText(const std::wstring &tcontent, const svgPoint &tposition,
        const std::wstring &torient = uT("rotate(0,0,0)"), const svgFont &tfont = svgFont(),
        const svgStroke &tstroke = svgStroke(), const svgFill &tfill = svgFill(),
        const SVG_ALIGN talign = SVG_ALIGN_MIDDLE)
    {
        content = tcontent;
        position = tposition;
        orient = torient;
        font = tfont;
        stroke = tstroke;
        fill = tfill;
        align = talign;
    }

    static void clean(std::wstring &strT, const std::wstring &search, const std::wstring &replace)
    {
        for (size_t pos = 0;; pos += replace.length()) {
            pos = strT.find(search, pos);
            if (pos == std::wstring::npos) {
                break;
            }
            strT.erase(pos, search.length());
            strT.insert(pos, replace);
        }
    }

    std::wstring toString(const svgLayout &layout) const
    {
        std::wstringstream strT;
        strT << xmlElementStart(uT("text"))
            << xmlAttribute(uT("x"), position.x)
            << xmlAttribute(uT("y"), position.y)
            << ((orient.empty() == false) ? xmlAttribute(uT("transform"), orient) : uT(""))
            << xmlAttribute(uT("text-anchor"), (SVG_ALIGN_MIDDLE == align) ? uT("middle") : ((SVG_ALIGN_START == align) ? uT("start") : uT("end")))
            << xmlAttribute(uT("dy"), uT("0.35em"))
            << fill.toString(layout)
            << stroke.toString(layout)
            << font.toString(layout)
            << uT(">")
            << xmlElementStart(uT("tspan"), false)
            << uT("\n")
            << content
            << uT("\n")
            << xmlElementEnd(uT("tspan"))
            << xmlElementEnd(uT("text"));
        return strT.str();
    }
};

class svgDocument
{
private:
    svgLayout layout;
    svgFill fill;
    svgStroke stroke;
    svgFont font;
    std::wstring filename;
    std::wstring bodynodes;
    svgPolyline *polyline;

public:
    svgDocument(const std::wstring &tfilename, svgLayout tlayout)
    {
        polyline = NULL;
        fill = svgFill();
        stroke = svgStroke();
        filename = tfilename;
        layout = tlayout;
    }
    ~svgDocument()
    {
        if (polyline) {
            delete polyline;
            polyline = NULL;
        }
    }

    void newPolyline(void)
    {
        savePolyline();
        try {
            polyline = new svgPolyline(stroke, fill);
        }
        catch (...) {
            polyline = NULL;
        }
    }

    void addPoint(svgPoint const &point)
    {
        if (polyline) {
            *polyline << point;
        }
    }

    void savePolyline(void)
    {
        if (polyline) {
            bodynodes += polyline->toString(layout);
            delete polyline;
            polyline = NULL;
        }
    }

    svgDocument & operator << (svgShape const &shape)
    {
        bodynodes += shape.toString(layout);
        return *this;
    }

    const svgFill & getFill(void) const
    {
        return fill;
    }

    void setFill(const svgFill &tFill)
    {
        fill = tFill;
    }

    const svgStroke & getStroke(void) const
    {
        return stroke;
    }

    void setStroke(const svgStroke &tStroke)
    {
        stroke = tStroke;
    }
    void setStrokeStyle(const SVG_STROKESTYLE tstyle)
    {
        stroke.setStyle(tstyle);
    }

    const svgFont & getFont(void) const
    {
        return font;
    }

    void setFont(svgFont &tFont)
    {
        font = tFont;
    }

    std::wstring toString() const
    {
        std::wstringstream strT;
        strT << uT("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n")
            << uT("\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n ")
            << uT("   \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n")
            << uT("<!-- Created with SigmaGraph (http://www.hamady.org/) -->\n")
            << xmlElementStart(uT("svg"))
            << xmlAttribute(uT("xmlns"), uT("http://www.w3.org/2000/svg"))
            << xmlAttribute(uT("version"), uT("1.1"))
            << xmlAttribute(uT("width"), layout.width)
            << xmlAttribute(uT("height"), layout.height)
            << uT(">\n")
            << bodynodes
            << uT("\n")
            << xmlElementEnd(uT("svg"));
        return strT.str();
    }

    int doConvUTF8(char *tUTF8, wchar_t tWC, size_t tcount)
    {
        int tlen = 0;
        unsigned int iWC = (unsigned int) tWC;

        if (iWC < 0x80) {
            if (tcount >= 1) {
                tUTF8[tlen++] = (char) iWC;
            }
        }
        else if (iWC < 0x800) {
            if (tcount >= 2) {
                tUTF8[tlen++] = 0xc0 | (iWC >> 6);
                tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
            }
        }
        else if (iWC < 0x10000) {
            if (tcount >= 3) {
                tUTF8[tlen++] = 0xe0 | (iWC >> 12);
                tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
                tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
            }
        }
        else if (iWC < 0x200000) {
            if (tcount >= 4) {
                tUTF8[tlen++] = 0xf0 | (iWC >> 18);
                tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
                tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
                tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
            }
        }
        else if (iWC < 0x4000000) {
            if (tcount >= 5) {
                tUTF8[tlen++] = 0xf8 | (iWC >> 24);
                tUTF8[tlen++] = 0x80 | ((iWC >> 18) & 0x3f);
                tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
                tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
                tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
            }
        }
        else if (iWC < 0x80000000) {
            if (tcount >= 6) {
                tUTF8[tlen++] = 0xfc | (iWC >> 30);
                tUTF8[tlen++] = 0x80 | ((iWC >> 24) & 0x3f);
                tUTF8[tlen++] = 0x80 | ((iWC >> 18) & 0x3f);
                tUTF8[tlen++] = 0x80 | ((iWC >> 12) & 0x3f);
                tUTF8[tlen++] = 0x80 | ((iWC >> 6) & 0x3f);
                tUTF8[tlen++] = 0x80 | (iWC & 0x3f);
            }
        }
        else {
            tlen = -1;
        }

        return tlen;
    }

    int convUTF8(char *tUTF8, const wchar_t *tWC, size_t tcount)
    {
        char *pUTF8 = tUTF8;
        int nn;

        while (*tWC) {
            nn = doConvUTF8(pUTF8, *tWC++, tcount);
            if (nn <= 0) {
                break;
            }
            pUTF8 += nn;
            tcount -= nn;
        }

        if (nn == 0) {
            while (tcount--) {
                *pUTF8++ = 0;
            }
        }
        else if (tcount) {
            *pUTF8 = 0;
        }

        if (nn == -1) {
            return -1;
        }

        return (pUTF8 - tUTF8);
    }

    bool save()
    {
        std::wstring strContentW = toString();

        const size_t iLen = strContentW.length();
        if (iLen < 12) {
            return false;
        }

        FILE *fpOut = _tfopen(filename.c_str(), uT("wb"));
        if (NULL == fpOut) {
            return false;
        }

        // handle multiline text
        std::wstring::size_type iFind = 0u;
        std::wstring::size_type iR = 0u;
        char_t szT[ML_STRSIZEN];
        size_t ii = 0;
        while ((ii++ < iLen) && ((iFind = strContentW.find(uT("\r\n"), iFind)) != std::wstring::npos)) {
            const std::wstring::size_type iRFind = strContentW.rfind(uT("x=\""), iFind);
            if (iRFind == std::wstring::npos) {
                break;
            }
            const std::wstring::size_type iRFindX = strContentW.find(uT("\""), iRFind + 3u);
            if ((iRFindX == std::wstring::npos) || (iRFindX < (iRFind + 4u))) {
                break;
            }
            std::wstring strT = strContentW.substr(iRFind + 3u, iRFindX - (iRFind + 3u));

#ifdef WIN32
            wsprintf(szT, uT("</tspan>\n<tspan x=\"%s\" dy=\"1.6em\">"), strT.c_str());
#else
            swprintf(szT, sizeof(szT) / sizeof(*szT), uT("</tspan>\n<tspan x=\"%ls\" dy=\"1.6em\">"), strT.c_str());
#endif

            iR = wcslen((const char_t *) (szT));

            strContentW.replace(iFind, 2, (const char_t *) (szT));
            iFind += iR;
        }
        //

        const size_t tLen = 1 + (iLen * 6);
        char *pUTF8 = (char *) malloc(tLen * sizeof(char));
        if (NULL == pUTF8) {
            fclose(fpOut);
            fpOut = NULL;
            return false;
        }
        pUTF8[tLen - 1] = '\0';

        int iRet = convUTF8(pUTF8, strContentW.c_str(), tLen - 1);

        bool bRet = false;

        if (iRet >= (int) iLen) {
            fwrite(pUTF8, iRet, 1, fpOut);
            bRet = true;
        }

        free(pUTF8);
        pUTF8 = NULL;

        fclose(fpOut);
        fpOut = NULL;

        return bRet;
    }

};

#endif
