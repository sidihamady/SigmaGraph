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
#include "MainFrm.h"
#include "SigmaToolBar.h"


// CSigmaToolBar

IMPLEMENT_DYNAMIC(CSigmaToolBar, CToolBar)

CSigmaToolBar::CSigmaToolBar()
{

}

CSigmaToolBar::~CSigmaToolBar()
{

}

BOOL CSigmaToolBar::LoadTrueColorToolBar(int nBtnWidth, UINT uToolBar, UINT uToolBarHot, UINT uToolBarDisabled)
{
    if (!SetTrueColorToolBar(TB_SETIMAGELIST, uToolBar, nBtnWidth)) {
        return FALSE;
    }

    if (uToolBarHot) {
        if (!SetTrueColorToolBar(TB_SETHOTIMAGELIST, uToolBarHot, nBtnWidth)) {
            return FALSE;
        }
    }

    if (uToolBarDisabled) {
        if (!SetTrueColorToolBar(TB_SETDISABLEDIMAGELIST, uToolBarDisabled, nBtnWidth)) {
            return FALSE;
        }
    }

    return TRUE;
}


BOOL CSigmaToolBar::SetTrueColorToolBar(UINT uToolBarType, UINT uToolBar, int  nBtnWidth)
{
    CImageList cImageList;
    CBitmap cBitmap;
    BITMAP bmBitmap;

    if (!cBitmap.Attach(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(uToolBar), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION))
        || !cBitmap.GetBitmap(&bmBitmap)) {
        return FALSE;
    }

    CSize cSize(bmBitmap.bmWidth, bmBitmap.bmHeight);
    int nNbBtn = cSize.cx / nBtnWidth;
    COLORREF rgbMask = RGB(255, 0, 255);

    if (!cImageList.Create(nBtnWidth, cSize.cy, ILC_COLOR8 | ILC_MASK, nNbBtn, 0)) {
        return FALSE;
    }

    if (cImageList.Add(&cBitmap, rgbMask) == -1) {
        return FALSE;
    }

    SendMessage(uToolBarType, 0, (LPARAM) cImageList.m_hImageList);
    cImageList.Detach();
    cBitmap.Detach();

    return TRUE;
}


BEGIN_MESSAGE_MAP(CSigmaToolBar, CToolBar)
    ON_WM_MOVE()
END_MESSAGE_MAP()


void CSigmaToolBar::OnMove(int x, int y)
{
    CToolBar::OnMove(x, y);
}
