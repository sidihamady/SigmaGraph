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

#include "../../LibGraph/LibFile/libfile.h"


class CFlatButton : public CButton
{
    DECLARE_DYNAMIC(CFlatButton)

public:
    CFlatButton();
    virtual ~CFlatButton();

    bool m_bHover;

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:

    DECLARE_MESSAGE_MAP()

    afx_msg void OnMouseMove(UINT nFlags, CPoint tPoint);
    afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
};

