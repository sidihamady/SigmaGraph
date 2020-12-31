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

#include "stdafx.h"
#include "Sigma.h"
#include "FlatButton.h"


// CFlatButton

IMPLEMENT_DYNAMIC(CFlatButton, CButton)

CFlatButton::CFlatButton()
{
    m_bHover = false;
}

CFlatButton::~CFlatButton()
{

}

BEGIN_MESSAGE_MAP(CFlatButton, CButton)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSEHOVER, &CFlatButton::OnMouseHover)
    ON_MESSAGE(WM_MOUSELEAVE, &CFlatButton::OnMouseLeave)
END_MESSAGE_MAP()

void CFlatButton::OnMouseMove(UINT nFlags, CPoint tPoint)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = m_hWnd;
    tme.dwFlags = TME_LEAVE | TME_HOVER;
    tme.dwHoverTime = 1;
    TrackMouseEvent(&tme);
}

LRESULT CFlatButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
    bool bHover = m_bHover;
    m_bHover = true;
    if (bHover != m_bHover) {
        Invalidate();
    }
    return true;
}

LRESULT CFlatButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
    bool bHover = m_bHover;
    m_bHover = false;
    if (bHover != m_bHover) {
        Invalidate();
    }
    return true;
}

void CFlatButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC dcT;
    dcT.Attach(lpDrawItemStruct->hDC);
    CRect rcT = lpDrawItemStruct->rcItem;

    Gdiplus::Graphics tGraphics(dcT);

    Gdiplus::LinearGradientBrush linGrBrush(
        Gdiplus::PointF((Gdiplus::REAL) (rcT.left), (Gdiplus::REAL) (rcT.top)),
        Gdiplus::PointF((Gdiplus::REAL) (rcT.right), (Gdiplus::REAL) (rcT.top)),
        (m_bHover) ? Gdiplus::Color(128, 240, 230, 210) : Gdiplus::Color(128, 210, 220, 240),
        (m_bHover) ? Gdiplus::Color(128, 200, 220, 160) : Gdiplus::Color(128, 160, 180, 220));

    Gdiplus::Font *pFont = NULL;
    Gdiplus::Pen *pPen = NULL;
    Gdiplus::SolidBrush *pBrush = NULL;
    Gdiplus::Color fontColor(0,0,0);

    pBrush = new Gdiplus::SolidBrush(fontColor);
    pPen = new Gdiplus::Pen((m_bHover) ? Gdiplus::Color(128, 220, 180, 100) : Gdiplus::Color(128, 100, 160, 220));
    pFont = new Gdiplus::Font(dcT.GetSafeHdc());

    tGraphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

    tGraphics.FillRectangle(&linGrBrush, rcT.left, rcT.top, rcT.Width(), rcT.Height());

    CString strTemp;
    GetWindowText(strTemp);        // Get the caption which have been set

    Gdiplus::PointF ptF;
    Gdiplus::REAL centerX = (Gdiplus::REAL) (rcT.left + (rcT.Width() / 2.0f)),
        centerY = (Gdiplus::REAL) (rcT.top + (rcT.Height() / 2.0f));
    Gdiplus::RectF rcF;

    tGraphics.MeasureString(strTemp, strTemp.GetLength(), pFont, ptF, &rcF);
    ptF.X = centerX - (rcF.Width / 2.0f);
    ptF.Y = centerY - (rcF.Height / 2.0f);

    tGraphics.DrawString(strTemp, strTemp.GetLength(), pFont, ptF, pBrush);
    tGraphics.DrawRectangle(pPen, rcT.left, rcT.top, rcT.Width() - 1, rcT.Height() - 1);

    delete pBrush; pBrush = NULL;
    delete pPen; pPen = NULL;
    delete pFont; pFont = NULL;

    dcT.Detach();
}
