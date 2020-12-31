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

#include "FitterModel.h"
#include "FitterMask.h"
#include "FitterOptions.h"

// CFitter

class CFitter : public CPropertySheet
{
    DECLARE_DYNAMIC(CFitter)

public:
    CFitterModel m_Model;
    CFitterMask m_Mask;
    CFitterOptions m_Options;

    CFitter(CWnd* pParentWnd = NULL) : CPropertySheet (_T("Fitter"), pParentWnd, 0)
    {
        AddPage(&m_Model);
        AddPage(&m_Mask);
        AddPage(&m_Options);
    }

    virtual ~CFitter();

protected:
    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
};


