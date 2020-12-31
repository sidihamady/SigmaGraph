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
#include "PlotoptGeneral.h"
#include "SigmaDoc.h"


// CPlotoptGeneral dialog

IMPLEMENT_DYNAMIC(CPlotoptGeneral, CPropertyPage)

CPlotoptGeneral::CPlotoptGeneral()
    : CPropertyPage(CPlotoptGeneral::IDD)
    , m_strTitle(_T(""))
    , m_bOTitle(FALSE)
{
    m_pPlot = NULL;
    m_pDoc = NULL;
    m_bIsModified = FALSE;
    m_bClosing = FALSE;
    m_bTitleColor = FALSE;
    m_bPlotColor = FALSE;
    m_bPlotColorB = FALSE;

    m_iModPoints = PL_MODPOINTS_DEF;

    m_bInitialized = FALSE;

    m_bTemplate = FALSE;
}

CPlotoptGeneral::~CPlotoptGeneral()
{

}

void CPlotoptGeneral::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDE_TITLE, m_strTitle);
    DDV_MaxChars(pDX, m_strTitle, ML_STRSIZE - 2);
    DDX_Check(pDX, IDK_OTITLE, m_bOTitle);
    DDX_Control(pDX, IDC_TITLE_FONTCOLOR, m_cbTitleColor);
    DDX_Control(pDX, IDC_PLOT_COLOR, m_cbPlotColor);
    DDX_Control(pDX, IDC_PLOT_COLORB, m_cbPlotColorB);
    DDX_Text(pDX, IDE_MODPOINTS, m_iModPoints);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}


BEGIN_MESSAGE_MAP(CPlotoptGeneral, CPropertyPage)
    ON_BN_CLICKED(ID_FONTCHOOSE, &CPlotoptGeneral::OnFontChoose)
    ON_BN_CLICKED(ID_GENERAL_DEFAULT, &CPlotoptGeneral::OnPlotDefault)
    ON_CBN_SELCHANGE(IDC_TITLE_FONTCOLOR, &CPlotoptGeneral::OnCbnSelchangeFontcolor)
    ON_CBN_SELCHANGE(IDC_PLOT_COLOR, &CPlotoptGeneral::OnCbnSelchangePlotcolor)
    ON_CBN_SELCHANGE(IDC_PLOT_COLORB, &CPlotoptGeneral::OnCbnSelchangePlotcolorB)
    ON_BN_CLICKED(IDK_OTITLE, &CPlotoptGeneral::OnBnClickedOtitle)
    ON_EN_CHANGE(IDE_TITLE, &CPlotoptGeneral::OnEnChangeTitle)
    ON_EN_CHANGE(IDE_MODPOINTS, &CPlotoptGeneral::OnEnChangeModPoints)
    ON_COMMAND(ID_HELP_CONTENTS, &CPlotoptGeneral::OnHelpContents)
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CPlotoptGeneral::Initialize()
{
    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        return FALSE;
    }

    m_bOTitle = (m_pPlot->otitle == 0) ? FALSE : TRUE;
    m_strTitle = m_pPlot->title.text;
    m_iModPoints = m_pPlot->modpoints;

    ::SendDlgItemMessage(GetSafeHwnd(), IDE_MODPOINTS, EM_LIMITTEXT, (WPARAM)4, (LPARAM)0);

    m_cbTitleColor.SetSelectedColor(RGB(m_pPlot->title.font.color.r, m_pPlot->title.font.color.g, m_pPlot->title.font.color.b));
    m_cbPlotColor.SetSelectedColor(RGB(m_pPlot->pbackcolor.r, m_pPlot->pbackcolor.g, m_pPlot->pbackcolor.b));
    m_cbPlotColorB.SetSelectedColor(RGB(m_pPlot->backcolor.r, m_pPlot->backcolor.g, m_pPlot->backcolor.b));

    UpdateData(FALSE);

    m_bIsModified = FALSE;

    return TRUE;
}

BOOL CPlotoptGeneral::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    return TRUE;
}

BOOL CPlotoptGeneral::OnApply()
{
    if (m_bIsModified) {
        UpdateData(TRUE);    // Copy data to variables

        // Processing
        if (m_bTitleColor == TRUE) {
            COLORREF clrTmp = m_cbTitleColor.GetSelectedColor();
            m_pPlot->title.font.color.r = GetRValue(clrTmp);
            m_pPlot->title.font.color.g = GetGValue(clrTmp);
            m_pPlot->title.font.color.b = GetBValue(clrTmp);
        }

        COLORREF clrT;
        if (m_bPlotColor) {
            clrT = m_cbPlotColor.GetSelectedColor();
            m_pPlot->pbackcolor.r = GetRValue(clrT);
            m_pPlot->pbackcolor.g = GetGValue(clrT);
            m_pPlot->pbackcolor.b = GetBValue(clrT);
        }
        if (m_bPlotColorB) {
            clrT = m_cbPlotColorB.GetSelectedColor();
            m_pPlot->backcolor.r = GetRValue(clrT);
            m_pPlot->backcolor.g = GetGValue(clrT);
            m_pPlot->backcolor.b = GetBValue(clrT);
        }

        SigmaApp.CStringToChar(m_strTitle, m_pPlot->title.text, ML_STRSIZE - 1);
        m_pPlot->otitle = (m_bOTitle == FALSE) ? 0 : 1;

        if (m_iModPoints < PL_MODPOINTS_MIN) {
            m_iModPoints = PL_MODPOINTS_MIN;
            UpdateData(FALSE);
        }
        if (m_iModPoints > PL_MODPOINTS_MAX) {
            m_iModPoints = PL_MODPOINTS_MAX;
            UpdateData(FALSE);
        }
        m_pPlot->modpoints = m_iModPoints;

        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }

        // Update window plot
        if (m_bClosing == FALSE) {
            if (m_bTemplate == TRUE) {
                GetParent()->GetParent()->Invalidate();
            }
            else {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }

        ((CSigmaDoc*) m_pDoc)->SaveState();
    }

    CPropertyPage::OnApply();

    m_bTitleColor = FALSE;

    m_bIsModified = FALSE;

    return TRUE;
}

void CPlotoptGeneral::OnFontChoose()
{
    LOGFONT lft;
    memset(&lft, 0, sizeof(lft));
    
    if (SigmaApp.FontToLogfont(m_pPlot->title.font, &lft) == FALSE) {
        return;
    }

    if (SigmaApp.GetFont(m_hWnd, &lft)) {

        SigmaApp.LogfontToFont(lft, &(m_pPlot->title.font));

        SetModified(TRUE);    m_bIsModified = TRUE;

        // [FIX-SG200-002] The document state is saved when the graph fonts change.
        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(m_bIsModified); }

        // Update window plot
        if (m_bClosing == FALSE) {
            if (m_bTemplate == TRUE) {
                GetParent()->GetParent()->Invalidate();
            }
            else {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }
    }
}

void CPlotoptGeneral::OnPlotDefault()
{
    OnApply();

    SigmaApp.m_pDefaultPlot->errcode = 0;
    pl_plot_dupstatic(SigmaApp.m_pDefaultPlot, m_pPlot);

    SigmaApp.setPlotSettings();
}

void CPlotoptGeneral::OnCbnSelchangeFontcolor()
{
    m_bTitleColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptGeneral::OnCbnSelchangePlotcolor()
{
    m_bPlotColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptGeneral::OnCbnSelchangePlotcolorB()
{
    m_bPlotColorB = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptGeneral::OnBnClickedOtitle()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptGeneral::OnEnChangeTitle()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptGeneral::OnEnChangeModPoints()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptGeneral::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    if (m_bInitialized == FALSE) {
        Initialize();
    }
}

void CPlotoptGeneral::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/graph.html#graph_options_general"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CPlotoptGeneral::PreTranslateMessage(MSG* pMsg)
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

    return CPropertyPage::PreTranslateMessage(pMsg);
}

void CPlotoptGeneral::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}
