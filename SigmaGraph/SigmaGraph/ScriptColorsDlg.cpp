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
#include "ScriptColorsDlg.h"

// CScriptColorsDlg dialog

IMPLEMENT_DYNAMIC(CScriptColorsDlg, CDialog)

CScriptColorsDlg::CScriptColorsDlg(CWnd *pParent)
    : CDialog(CScriptColorsDlg::IDD, pParent)
{
    m_pWndEditor = NULL;

    m_pWndFontStyleDef = NULL;
    m_pWndFontStyleKey = NULL;
    m_pWndFontStyleStr = NULL;
    m_pWndFontStyleNum = NULL;
    m_pWndFontStyleCom = NULL;
    m_pWndFontStyleLib = NULL;
    m_pWndFontStyleLinenum = NULL;

    m_bColorDef = FALSE;
    m_bColorKey = FALSE;
    m_bColorStr = FALSE;
    m_bColorNum = FALSE;
    m_bColorCom = FALSE;
    m_bColorLib = FALSE;

    m_bFontStyleDef = FALSE;
    m_bFontStyleKey = FALSE;
    m_bFontStyleStr = FALSE;
    m_bFontStyleNum = FALSE;
    m_bFontStyleCom = FALSE;
    m_bFontStyleLib = FALSE;
    m_bFontStyleLinenum = FALSE;
}

CScriptColorsDlg::~CScriptColorsDlg()
{
    try {
        m_FontBoldDef.DeleteObject();
        m_FontBoldKey.DeleteObject();
        m_FontBoldStr.DeleteObject();
        m_FontBoldNum.DeleteObject();
        m_FontBoldCom.DeleteObject();
        m_FontBoldLib.DeleteObject();
        m_FontBoldLinenum.DeleteObject();
    }
    catch (...) {

    }
}

void CScriptColorsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, ID_SCRIPT_COLOR_DEF, m_cbColorDef);
    DDX_Control(pDX, ID_SCRIPT_COLOR_KEY, m_cbColorKey);
    DDX_Control(pDX, ID_SCRIPT_COLOR_STR, m_cbColorStr);
    DDX_Control(pDX, ID_SCRIPT_COLOR_NUM, m_cbColorNum);
    DDX_Control(pDX, ID_SCRIPT_COLOR_COM, m_cbColorCom);
    DDX_Control(pDX, ID_SCRIPT_COLOR_LIB, m_cbColorLib);
    DDX_Control(pDX, ID_SCRIPT_COLOR_LINENUM, m_cbColorLinenum);
    DDX_Check(pDX, IDK_SCRIPT_FONTSTYLE_DEF, m_bFontStyleDef);
    DDX_Check(pDX, IDK_SCRIPT_FONTSTYLE_KEY, m_bFontStyleKey);
    DDX_Check(pDX, IDK_SCRIPT_FONTSTYLE_STR, m_bFontStyleStr);
    DDX_Check(pDX, IDK_SCRIPT_FONTSTYLE_NUM, m_bFontStyleNum);
    DDX_Check(pDX, IDK_SCRIPT_FONTSTYLE_COM, m_bFontStyleCom);
    DDX_Check(pDX, IDK_SCRIPT_FONTSTYLE_LIB, m_bFontStyleLib);
    DDX_Check(pDX, IDK_SCRIPT_FONTSTYLE_LINENUM, m_bFontStyleLinenum);
}

BEGIN_MESSAGE_MAP(CScriptColorsDlg, CDialog)
    ON_CBN_SELCHANGE(ID_SCRIPT_COLOR_DEF, &CScriptColorsDlg::OnCbnSelchangeColorDef)
    ON_CBN_SELCHANGE(ID_SCRIPT_COLOR_KEY, &CScriptColorsDlg::OnCbnSelchangeColorKey)
    ON_CBN_SELCHANGE(ID_SCRIPT_COLOR_STR, &CScriptColorsDlg::OnCbnSelchangeColorStr)
    ON_CBN_SELCHANGE(ID_SCRIPT_COLOR_NUM, &CScriptColorsDlg::OnCbnSelchangeColorNum)
    ON_CBN_SELCHANGE(ID_SCRIPT_COLOR_COM, &CScriptColorsDlg::OnCbnSelchangeColorCom)
    ON_CBN_SELCHANGE(ID_SCRIPT_COLOR_LIB, &CScriptColorsDlg::OnCbnSelchangeColorLib)
    ON_CBN_SELCHANGE(ID_SCRIPT_COLOR_LINENUM, &CScriptColorsDlg::OnCbnSelchangeColorLinenum)
    ON_BN_CLICKED(IDK_SCRIPT_FONTSTYLE_DEF, &CScriptColorsDlg::OnBnFontStyleDef)
    ON_BN_CLICKED(IDK_SCRIPT_FONTSTYLE_KEY, &CScriptColorsDlg::OnBnFontStyleKey)
    ON_BN_CLICKED(IDK_SCRIPT_FONTSTYLE_STR, &CScriptColorsDlg::OnBnFontStyleStr)
    ON_BN_CLICKED(IDK_SCRIPT_FONTSTYLE_NUM, &CScriptColorsDlg::OnBnFontStyleNum)
    ON_BN_CLICKED(IDK_SCRIPT_FONTSTYLE_COM, &CScriptColorsDlg::OnBnFontStyleCom)
    ON_BN_CLICKED(IDK_SCRIPT_FONTSTYLE_LIB, &CScriptColorsDlg::OnBnFontStyleLib)
    ON_BN_CLICKED(IDK_SCRIPT_FONTSTYLE_LINENUM, &CScriptColorsDlg::OnBnFontStyleLinenum)
    ON_COMMAND(ID_HELP_CONTENTS, &CScriptColorsDlg::OnHelpContents)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CScriptColorsDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    if ((m_pScintillaPrefs == NULL) || (m_pWndEditor == NULL)) {
        return FALSE;
    }

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    m_bFontStyleDef = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_DEFAULT].fontstyle & SCRIPT_STYLE_BOLD) > 0);
    m_bFontStyleKey = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_KEYWORD].fontstyle & SCRIPT_STYLE_BOLD) > 0);
    m_bFontStyleStr = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_STRING_START].fontstyle & SCRIPT_STYLE_BOLD) > 0);
    m_bFontStyleNum = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_NUMBER].fontstyle & SCRIPT_STYLE_BOLD) > 0);
    m_bFontStyleCom = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_COMMENT_START].fontstyle & SCRIPT_STYLE_BOLD) > 0);
    m_bFontStyleLib = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_LIB_START].fontstyle & SCRIPT_STYLE_BOLD) > 0);
    m_bFontStyleLinenum = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_LINENUM].fontstyle & SCRIPT_STYLE_BOLD) > 0);

    UpdateData(FALSE);

    m_pWndFontStyleDef = GetDlgItem(IDL_SCRIPT_FONTSTYLE_DEF);
    m_pWndFontStyleKey = GetDlgItem(IDL_SCRIPT_FONTSTYLE_KEY);
    m_pWndFontStyleStr = GetDlgItem(IDL_SCRIPT_FONTSTYLE_STR);
    m_pWndFontStyleNum = GetDlgItem(IDL_SCRIPT_FONTSTYLE_NUM);
    m_pWndFontStyleCom = GetDlgItem(IDL_SCRIPT_FONTSTYLE_COM);
    m_pWndFontStyleLib = GetDlgItem(IDL_SCRIPT_FONTSTYLE_LIB);
    m_pWndFontStyleLinenum = GetDlgItem(IDL_SCRIPT_FONTSTYLE_LINENUM);

    return TRUE;
}

HBRUSH CScriptColorsDlg::OnCtlColor(CDC* pDC, CWnd *pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    if (m_pWndFontStyleDef && (pWnd->m_hWnd == m_pWndFontStyleDef->m_hWnd)) {
        pDC->SetTextColor(m_pScintillaPrefs->style[STYLEINFO_INDEX_DEFAULT].foreground);

        try {
            CFont* pFont = m_pWndFontStyleDef->GetFont();
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            lf.lfWeight = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_DEFAULT].fontstyle & SCRIPT_STYLE_BOLD) > 0) ? FW_BOLD : FW_NORMAL;
            m_FontBoldDef.DeleteObject();
            m_FontBoldDef.CreateFontIndirect(&lf);
            pDC->SelectObject(&m_FontBoldDef);
        }
        catch (...) {

        }

        return hbr;
    }

    if (m_pWndFontStyleKey && (pWnd->m_hWnd == m_pWndFontStyleKey->m_hWnd)) {
        pDC->SetTextColor(m_pScintillaPrefs->style[STYLEINFO_INDEX_KEYWORD].foreground);

        try {
            CFont* pFont = m_pWndFontStyleKey->GetFont();
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            lf.lfWeight = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_KEYWORD].fontstyle & SCRIPT_STYLE_BOLD) > 0) ? FW_BOLD : FW_NORMAL;
            m_FontBoldKey.DeleteObject();
            m_FontBoldKey.CreateFontIndirect(&lf);
            pDC->SelectObject(&m_FontBoldKey);
        }
        catch (...) {

        }

        return hbr;
    }

    if (m_pWndFontStyleStr && (pWnd->m_hWnd == m_pWndFontStyleStr->m_hWnd)) {
        pDC->SetTextColor(m_pScintillaPrefs->style[STYLEINFO_INDEX_STRING_START].foreground);

        try {
            CFont* pFont = m_pWndFontStyleStr->GetFont();
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            lf.lfWeight = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_STRING_START].fontstyle & SCRIPT_STYLE_BOLD) > 0) ? FW_BOLD : FW_NORMAL;
            m_FontBoldStr.DeleteObject();
            m_FontBoldStr.CreateFontIndirect(&lf);
            pDC->SelectObject(&m_FontBoldStr);
        }
        catch (...) {

        }

        return hbr;
    }

    if (m_pWndFontStyleNum && (pWnd->m_hWnd == m_pWndFontStyleNum->m_hWnd)) {
        pDC->SetTextColor(m_pScintillaPrefs->style[STYLEINFO_INDEX_NUMBER].foreground);

        try {
            CFont* pFont = m_pWndFontStyleNum->GetFont();
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            lf.lfWeight = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_NUMBER].fontstyle & SCRIPT_STYLE_BOLD) > 0) ? FW_BOLD : FW_NORMAL;
            m_FontBoldNum.DeleteObject();
            m_FontBoldNum.CreateFontIndirect(&lf);
            pDC->SelectObject(&m_FontBoldNum);
        }
        catch (...) {

        }

        return hbr;
    }

    if (m_pWndFontStyleCom && (pWnd->m_hWnd == m_pWndFontStyleCom->m_hWnd)) {
        pDC->SetTextColor(m_pScintillaPrefs->style[STYLEINFO_INDEX_COMMENT_START].foreground);

        try {
            CFont* pFont = m_pWndFontStyleCom->GetFont();
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            lf.lfWeight = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_COMMENT_START].fontstyle & SCRIPT_STYLE_BOLD) > 0) ? FW_BOLD : FW_NORMAL;
            m_FontBoldCom.DeleteObject();
            m_FontBoldCom.CreateFontIndirect(&lf);
            pDC->SelectObject(&m_FontBoldCom);
        }
        catch (...) {

        }

        return hbr;
    }

    if (m_pWndFontStyleLib && (pWnd->m_hWnd == m_pWndFontStyleLib->m_hWnd)) {
        pDC->SetTextColor(m_pScintillaPrefs->style[STYLEINFO_INDEX_LIB_START].foreground);

        try {
            CFont* pFont = m_pWndFontStyleLib->GetFont();
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            lf.lfWeight = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_LIB_START].fontstyle & SCRIPT_STYLE_BOLD) > 0) ? FW_BOLD : FW_NORMAL;
            m_FontBoldLib.DeleteObject();
            m_FontBoldLib.CreateFontIndirect(&lf);
            pDC->SelectObject(&m_FontBoldLib);
        }
        catch (...) {

        }

        return hbr;
    }

    if (m_pWndFontStyleLinenum && (pWnd->m_hWnd == m_pWndFontStyleLinenum->m_hWnd)) {
        pDC->SetTextColor(m_pScintillaPrefs->style[STYLEINFO_INDEX_LINENUM].foreground);

        try {
            CFont* pFont = m_pWndFontStyleLinenum->GetFont();
            LOGFONT lf;
            pFont->GetLogFont(&lf);
            lf.lfWeight = ((m_pScintillaPrefs->style[STYLEINFO_INDEX_LINENUM].fontstyle & SCRIPT_STYLE_BOLD) > 0) ? FW_BOLD : FW_NORMAL;
            m_FontBoldLinenum.DeleteObject();
            m_FontBoldLinenum.CreateFontIndirect(&lf);
            pDC->SelectObject(&m_FontBoldLinenum);
        }
        catch (...) {

        }

        return hbr;
    }

    return hbr;
}

void CScriptColorsDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // Do not call CDialog::OnPaint() for painting messages
}

void CScriptColorsDlg::OnCbnSelchangeColorDef()
{
    m_bColorDef = TRUE;
    updateStyle();
    m_pWndFontStyleDef->Invalidate();
}
void CScriptColorsDlg::OnCbnSelchangeColorKey()
{
    m_bColorKey = TRUE;
    updateStyle();
    m_pWndFontStyleKey->Invalidate();
}
void CScriptColorsDlg::OnCbnSelchangeColorStr()
{
    m_bColorStr = TRUE;
    updateStyle();
    m_pWndFontStyleStr->Invalidate();
}
void CScriptColorsDlg::OnCbnSelchangeColorNum()
{
    m_bColorNum = TRUE;
    updateStyle();
    m_pWndFontStyleNum->Invalidate();
}
void CScriptColorsDlg::OnCbnSelchangeColorCom()
{
    m_bColorCom = TRUE;
    updateStyle();
    m_pWndFontStyleCom->Invalidate();
}
void CScriptColorsDlg::OnCbnSelchangeColorLib()
{
    m_bColorLib = TRUE;
    updateStyle();
    m_pWndFontStyleLib->Invalidate();
}
void CScriptColorsDlg::OnCbnSelchangeColorLinenum()
{
    m_bColorLinenum = TRUE;
    updateStyle();
    m_pWndFontStyleLinenum->Invalidate();
}

void CScriptColorsDlg::OnBnFontStyleDef()
{
    updateStyle();
    m_pWndFontStyleDef->Invalidate();
}
void CScriptColorsDlg::OnBnFontStyleKey()
{
    updateStyle();
    m_pWndFontStyleKey->Invalidate();
}
void CScriptColorsDlg::OnBnFontStyleStr()
{
    updateStyle();
    m_pWndFontStyleStr->Invalidate();
}
void CScriptColorsDlg::OnBnFontStyleNum()
{
    updateStyle();
    m_pWndFontStyleNum->Invalidate();
}
void CScriptColorsDlg::OnBnFontStyleCom()
{
    updateStyle();
    m_pWndFontStyleCom->Invalidate();
}
void CScriptColorsDlg::OnBnFontStyleLib()
{
    updateStyle();
    m_pWndFontStyleLib->Invalidate();
}
void CScriptColorsDlg::OnBnFontStyleLinenum()
{
    updateStyle();    
    m_pWndFontStyleLinenum->Invalidate();
}

void CScriptColorsDlg::updateStyle(void)
{
    UpdateData(TRUE);

    // Bolding
    m_pScintillaPrefs->style[STYLEINFO_INDEX_DEFAULT].fontstyle = (m_bFontStyleDef ? SCRIPT_STYLE_BOLD : 0);
    m_pScintillaPrefs->style[STYLEINFO_INDEX_OPERATOR].fontstyle = (m_bFontStyleDef ? SCRIPT_STYLE_BOLD : 0);
    m_pScintillaPrefs->style[STYLEINFO_INDEX_IDENTIFIER].fontstyle = (m_bFontStyleDef ? SCRIPT_STYLE_BOLD : 0);

    m_pScintillaPrefs->style[STYLEINFO_INDEX_KEYWORD].fontstyle = (m_bFontStyleKey ? SCRIPT_STYLE_BOLD : 0);

    int iStyle = (m_bFontStyleStr ? SCRIPT_STYLE_BOLD : 0);
    for (int ii = STYLEINFO_INDEX_STRING_START; ii <= STYLEINFO_INDEX_STRING_END; ii++) {
        m_pScintillaPrefs->style[ii].fontstyle = iStyle;
    }
    m_pScintillaPrefs->style[STYLEINFO_INDEX_STRINGEOL].fontstyle = iStyle;

    m_pScintillaPrefs->style[STYLEINFO_INDEX_NUMBER].fontstyle = (m_bFontStyleNum ? SCRIPT_STYLE_BOLD : 0);
    
    iStyle = (m_bFontStyleCom ? SCRIPT_STYLE_BOLD : 0);
    for (int ii = STYLEINFO_INDEX_COMMENT_START; ii <= STYLEINFO_INDEX_COMMENT_END; ii++) {
        m_pScintillaPrefs->style[ii].fontstyle = iStyle;
    }

    iStyle = (m_bFontStyleLib ? SCRIPT_STYLE_BOLD : 0);
    for (int ii = STYLEINFO_INDEX_LIB_START; ii <= STYLEINFO_INDEX_LIB_END; ii++) {
        m_pScintillaPrefs->style[ii].fontstyle = iStyle;
    }
    m_pScintillaPrefs->style[STYLEINFO_INDEX_LINENUM].fontstyle = (m_bFontStyleLinenum ? SCRIPT_STYLE_BOLD : 0);

    // Colors
    if (m_bColorDef == TRUE) {
        COLORREF clrT = m_cbColorDef.GetSelectedColor();
        m_pScintillaPrefs->style[STYLEINFO_INDEX_DEFAULT].foreground = clrT;
        m_pScintillaPrefs->style[STYLEINFO_INDEX_OPERATOR].foreground = clrT;
        m_pScintillaPrefs->style[STYLEINFO_INDEX_IDENTIFIER].foreground = clrT;
    }
    if (m_bColorKey == TRUE) {
        m_pScintillaPrefs->style[STYLEINFO_INDEX_KEYWORD].foreground = m_cbColorKey.GetSelectedColor();
    }
    if (m_bColorStr == TRUE) {
        COLORREF clrT = m_cbColorStr.GetSelectedColor();
        for (int ii = STYLEINFO_INDEX_STRING_START; ii <= STYLEINFO_INDEX_STRING_END; ii++) {
            m_pScintillaPrefs->style[ii].foreground = clrT;
        }
        m_pScintillaPrefs->style[STYLEINFO_INDEX_STRINGEOL].foreground = clrT;
    }
    if (m_bColorNum == TRUE) {
        m_pScintillaPrefs->style[STYLEINFO_INDEX_NUMBER].foreground = m_cbColorNum.GetSelectedColor();
    }
    if (m_bColorCom == TRUE) {
        COLORREF clrT = m_cbColorCom.GetSelectedColor();
        for (int ii = STYLEINFO_INDEX_COMMENT_START; ii <= STYLEINFO_INDEX_COMMENT_END; ii++) {
            m_pScintillaPrefs->style[ii].foreground = clrT;
        }
    }
    if (m_bColorLib == TRUE) {
        COLORREF clrT = m_cbColorLib.GetSelectedColor();
        for (int ii = STYLEINFO_INDEX_LIB_START; ii <= STYLEINFO_INDEX_LIB_END; ii++) {
            m_pScintillaPrefs->style[ii].foreground = clrT;
        }
    }
    if (m_bColorLinenum == TRUE) {
        m_pScintillaPrefs->style[STYLEINFO_INDEX_LINENUM].foreground = m_cbColorLinenum.GetSelectedColor();
    }

    m_pWndEditor->initPrefs(*m_pScintillaPrefs);
}

void CScriptColorsDlg::OnOK()
{
    updateStyle();

    CDialog::OnOK();
}

void CScriptColorsDlg::OnClose()
{
    CDialog::OnClose();
}

void CScriptColorsDlg::OnCancel()
{
    CDialog::OnCancel();
}

void CScriptColorsDlg::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/scripting.html"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CScriptColorsDlg::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if ((pMsg->wParam == VK_F1)) {
                if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
                    return TRUE;
                }
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CScriptColorsDlg::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}