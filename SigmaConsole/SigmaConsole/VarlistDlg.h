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
#include "afxwin.h"

// CVarlistDlg dialog

class CVarlistDlg : public CDialog
{
    DECLARE_DYNAMIC(CVarlistDlg)

public:
    CVarlistDlg(CWnd* pParent = NULL);                        // standard constructor
    virtual ~CVarlistDlg();

    BOOL Create(UINT nID, CWnd *pWnd) { return CDialog::Create(nID, pWnd); }

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_VARLIST };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
    virtual void PostNcDestroy();
    virtual void OnOK();
    virtual void OnCancel();

    DECLARE_MESSAGE_MAP()

public:
    CListCtrl m_lstVar;

    void UpdateVarlist();

    afx_msg void OnNMDblclkVarlist(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnVarlistRefresh();
    afx_msg void OnVarSet();
    afx_msg void OnVarUnset();
    afx_msg void OnClose();
};
