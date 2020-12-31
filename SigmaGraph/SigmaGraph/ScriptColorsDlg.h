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

#include "ComboBoxColor.h"

#include "ScriptEdit.h"

// CScriptColorsDlg dialog

class CScriptColorsDlg : public CDialog
{
    DECLARE_DYNAMIC(CScriptColorsDlg)

public:
    CScriptColorsDlg(CWnd *pParent);
    virtual ~CScriptColorsDlg();

    virtual BOOL OnInitDialog();

// Dialog Data
    enum { IDD = IDD_SCRIPT_COLORS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:

    CScriptEdit *m_pWndEditor;
    ScintillaPrefs *m_pScintillaPrefs;

    CWnd *m_pWndFontStyleDef;
    CWnd *m_pWndFontStyleKey;
    CWnd *m_pWndFontStyleStr;
    CWnd *m_pWndFontStyleNum;
    CWnd *m_pWndFontStyleCom;
    CWnd *m_pWndFontStyleLib;
    CWnd *m_pWndFontStyleLinenum;

    CComboBoxColor m_cbColorDef;
    BOOL m_bColorDef;
    CComboBoxColor m_cbColorKey;
    BOOL m_bColorKey;
    CComboBoxColor m_cbColorStr;
    BOOL m_bColorStr;
    CComboBoxColor m_cbColorNum;
    BOOL m_bColorNum;
    CComboBoxColor m_cbColorCom;
    BOOL m_bColorCom;
    CComboBoxColor m_cbColorLib;
    BOOL m_bColorLib;
    CComboBoxColor m_cbColorLinenum;
    BOOL m_bColorLinenum;

    BOOL m_bFontStyleDef;
    BOOL m_bFontStyleKey;
    BOOL m_bFontStyleStr;
    BOOL m_bFontStyleNum;
    BOOL m_bFontStyleCom;
    BOOL m_bFontStyleLib;
    BOOL m_bFontStyleLinenum;

    CFont m_FontBoldDef;
    CFont m_FontBoldKey;
    CFont m_FontBoldStr;
    CFont m_FontBoldNum;
    CFont m_FontBoldCom;
    CFont m_FontBoldLib;
    CFont m_FontBoldLinenum;

    void updateStyle(void);

    HACCEL m_hAccel; // accelerator table
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor);

    afx_msg void OnCbnSelchangeColorDef();
    afx_msg void OnCbnSelchangeColorKey();
    afx_msg void OnCbnSelchangeColorStr();
    afx_msg void OnCbnSelchangeColorNum();
    afx_msg void OnCbnSelchangeColorCom();
    afx_msg void OnCbnSelchangeColorLib();
    afx_msg void OnCbnSelchangeColorLinenum();

    afx_msg void OnBnFontStyleDef();
    afx_msg void OnBnFontStyleKey();
    afx_msg void OnBnFontStyleStr();
    afx_msg void OnBnFontStyleNum();
    afx_msg void OnBnFontStyleCom();
    afx_msg void OnBnFontStyleLib();
    afx_msg void OnBnFontStyleLinenum();

    afx_msg void OnOK();
    afx_msg void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnPaint();
    afx_msg void OnHelpContents();

    afx_msg void OnDestroy();
};
