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

#include <stdlib.h>

static PBITMAPINFO bmpInfo(HWND hWnd, HBITMAP hBmp);
static byte_t bmpSave(HWND hWnd, LPCTSTR pszFile, HBITMAP hBmp, HDC hDC, libvar_t *pLib);

void fl_wndcapt(HWND hWnd, const char_t *filename, byte_t iformat, libvar_t *pLib)
{
    HBITMAP hBmp = NULL, hBmpOld;
    RECT rect = {0,0,0,0};
    HDC hDC = NULL, memDC = NULL;
    HPALETTE hPal = NULL;

    int_t rwidth, rheight;

    if (pLib->errcode) { return; }

    pLib->errcode = 1;

    GetClientRect(hWnd, &rect);
    rwidth = (int_t)(rect.right - rect.left);
    rheight = (int_t)(rect.bottom - rect.top);

    if ((hDC = GetDC(hWnd)) == NULL) { goto fl_wndcapt_RET; }

    if ((memDC = CreateCompatibleDC(hDC)) == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        goto fl_wndcapt_RET;
    }

    hBmp = CreateCompatibleBitmap(hDC, rwidth, rheight);
    if (hBmp == NULL) {
        _tcscpy(pLib->message, _T("Insufficient memory"));
        goto fl_wndcapt_RET;
    }

    if (GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE) {
        UINT size = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
        LOGPALETTE *lp = (LOGPALETTE*)malloc(size * sizeof(BYTE));
        if (lp == NULL) {
            _tcscpy(pLib->message, _T("Insufficient memory"));
            goto fl_wndcapt_RET;
        }
        lp->palVersion = 0x300;
        lp->palNumEntries = GetSystemPaletteEntries(hDC, 0, 255, lp->palPalEntry);
        hPal = CreatePalette(lp);
        SelectPalette(hDC, hPal, FALSE);
        RealizePalette(hDC);
        free(lp);
    }

    if ((hBmpOld = (HBITMAP)SelectObject(memDC, hBmp)) == NULL) { 
        _tcscpy(pLib->message, _T("Cannot create bitmap"));
        goto fl_wndcapt_RET;
    }
    BitBlt(memDC, 0, 0, rwidth, rheight, hDC, 0, 0, SRCCOPY);

    /* OK */
    switch (iformat) {
        case 0:
            if (bmpSave(hWnd, (LPCTSTR)filename, hBmp, hDC, pLib) == 0) {
                pLib->errcode = 0;
            }
            break;
        default:
            if (bmpSave(hWnd, (LPCTSTR)filename, hBmp, hDC, pLib) == 0) {
                pLib->errcode = 0;
            }
            break;
    }

fl_wndcapt_RET:
    if (hBmpOld) { SelectObject(memDC, hBmpOld); }
    if (hBmp) { DeleteObject(hBmp); }
    if (memDC) { DeleteDC(memDC); }
    if (hDC) { ReleaseDC(hWnd, hDC); }

    return;
}

static PBITMAPINFO bmpInfo(HWND hWnd, HBITMAP hBmp)
{ 
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD cClrBits;
    HANDLE hHeap = GetProcessHeap();

    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))  {
        return NULL;
    }

    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1) { cClrBits = 1; }
    else if (cClrBits <= 4) { cClrBits = 4; }
    else if (cClrBits <= 8) { cClrBits = 8; }
    else if (cClrBits <= 16) { cClrBits = 16; }
    else if (cClrBits <= 24) { cClrBits = 24; }
    else { cClrBits = 32; }

    if (cClrBits != 24) {
        pbmi = (PBITMAPINFO)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * (1 << cClrBits)));
    }
    else {
        pbmi = (PBITMAPINFO)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(BITMAPINFOHEADER));
    }

    if (pbmi == NULL)  { return NULL; }

    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = cClrBits;
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;

    if (cClrBits < 24) {
        pbmi->bmiHeader.biClrUsed = (1 << cClrBits);
    }

    pbmi->bmiHeader.biCompression = BI_RGB;

    pbmi->bmiHeader.biSizeImage = ((((pbmi->bmiHeader.biWidth * cClrBits) + 31) & ~31) / 8) * pbmi->bmiHeader.biHeight; 

    pbmi->bmiHeader.biClrImportant = 0;

    return pbmi;
} 

static byte_t bmpSave(HWND hWnd, LPCTSTR pszFile, HBITMAP hBmp, HDC hDC, libvar_t *pLib)
{
    HANDLE hf;
    BITMAPFILEHEADER hdr;
    PBITMAPINFOHEADER pbih;
    PBITMAPINFO pbi;
    LPBYTE lpBits;
    DWORD dwTotal, cb, dwTmp;
    BYTE *hp;
    HANDLE hHeap = GetProcessHeap();

    if ((pbi = bmpInfo(hWnd, hBmp)) == NULL) { return 1; }

    pbih = (PBITMAPINFOHEADER)pbi; 
    lpBits = (LPBYTE)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, pbih->biSizeImage);

    if (lpBits == NULL) {
        HeapFree(hHeap, 0, pbi);
        _tcscpy(pLib->message, _T("Insufficient memory"));
        return 1;
    }

    if (!GetDIBits(hDC, hBmp, 0, (WORD)pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)) {
        HeapFree(hHeap, 0, lpBits);
        HeapFree(hHeap, 0, pbi);
        _tcscpy(pLib->message, _T("Cannot create bitmap"));
        return 1;
    }

    hf = CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
    if (hf == INVALID_HANDLE_VALUE) {
        HeapFree(hHeap, 0, lpBits);
        HeapFree(hHeap, 0, pbi);
        _tcscpy(pLib->message, _T("Cannot open file"));
        return 1;
    }
    hdr.bfType = 0x4d42;
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + (pbih->biClrUsed * sizeof(RGBQUAD)) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + (pbih->biClrUsed * sizeof (RGBQUAD)); 

    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL)) {
        CloseHandle(hf);
        HeapFree(hHeap, 0, lpBits);
        HeapFree(hHeap, 0, pbi);
        _tcscpy(pLib->message, _T("Cannot open file"));
        return 1;
    }

    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + (pbih->biClrUsed * sizeof (RGBQUAD)), 
        (LPDWORD) &dwTmp, NULL)) {
        CloseHandle(hf);
        HeapFree(hHeap, 0, lpBits);
        HeapFree(hHeap, 0, pbi);
        _tcscpy(pLib->message, _T("Cannot open file"));
        return 1;
    }

    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR) hp, (int_t) cb, (LPDWORD) &dwTmp,NULL)) {
        CloseHandle(hf);
        HeapFree(hHeap, 0, lpBits);
        HeapFree(hHeap, 0, pbi);
        _tcscpy(pLib->message, _T("Cannot open file"));
        return 1;
    }

    CloseHandle(hf);
    HeapFree(hHeap, 0, lpBits);
    HeapFree(hHeap, 0, pbi);

    return 0;
}
