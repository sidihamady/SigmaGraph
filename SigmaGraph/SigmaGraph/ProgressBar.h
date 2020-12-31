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

#pragma once

class CProgressBar: public CProgressCtrl
// Creates a ProgressBar in the status bar
{
public:
    CProgressBar();
    CProgressBar(LPCTSTR strMessage, int_t nSize = 100, int_t MaxValue = 100, BOOL bSmooth = TRUE, int_t nPane = 0);
    ~CProgressBar();
    BOOL Create(LPCTSTR strMessage, int_t nSize = 100, int_t MaxValue = 100, BOOL bSmooth = TRUE, int_t nPane = 0);

    DECLARE_DYNCREATE(CProgressBar)

// operations
public:
    BOOL SetRange(int_t nLower, int_t nUpper, int_t nStep = 1);
    BOOL SetText(LPCTSTR strMessage);
    BOOL SetSize(int_t nSize);
    COLORREF SetBarColour(COLORREF clrBar);
    COLORREF SetBkColour(COLORREF clrBk);
    int_t SetPos(int_t nPos);
    int_t OffsetPos(int_t nPos);
    int_t SetStep(int_t nStep);
    int_t StepIt();
    void Clear();

// Overrides

// implementation
protected:
    int_t m_nSize;                    // Percentage size of control
    int_t m_nPane;                    // ID of status bar pane progress bar is to appear in
    CString m_strMessage;        // Message to display to left of control
    CString m_strPrevText;        // Previous text in status bar
    CRect    m_Rect;                    // Dimensions of the whole thing

    CStatusBar *GetStatusBar();
    BOOL Resize();

// Generated message map functions
protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()

};
