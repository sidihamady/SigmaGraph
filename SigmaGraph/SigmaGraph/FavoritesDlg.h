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

// CFavoriteDlg dialog

class CFavoriteDlg : public CDialog
{
    DECLARE_DYNAMIC(CFavoriteDlg)

public:
    CFavoriteDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CFavoriteDlg();

// Dialog Data
    enum { IDD = IDD_SCRIPT_FAVORITE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CListBox m_lstFavorite;
    favorite_t *m_pFavorite;
    int_t *m_piFavoriteCount;
    int_t *m_piFavorite;

    CButton m_bnAdd;
    CButton m_bnRemove;
    CButton m_bnLoad;
    CButton m_bnUp;
    CButton m_bnDown;
    CButton m_bnAutoLoad;
    CButton m_bnAutoRun;

    BOOL m_bAutoLoad;
    BOOL m_bAutoRun;

    CString m_strOutput;

    void UpdateFavoriteList();

    virtual BOOL OnInitDialog();
    afx_msg void OnFavoriteAdd();
    afx_msg void OnFavoriteRemove();
    afx_msg void OnFavoriteLoad();
    afx_msg void OnFavoriteUp();
    afx_msg void OnFavoriteDown();
    afx_msg void OnLbnSelchangeFavorite();
    afx_msg void OnBnClickedScriptAutorun();
    afx_msg void OnBnClickedScriptAutoload();
    afx_msg void OnOK();
    afx_msg void OnClose();
};
