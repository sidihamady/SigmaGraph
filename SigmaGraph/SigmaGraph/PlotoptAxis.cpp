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

#include "stdafx.h"
#include "Sigma.h"
#include "PlotoptAxis.h"
#include "SigmaDoc.h"


// CPlotoptAxis dialog

IMPLEMENT_DYNAMIC(CPlotoptAxis, CPropertyPage)

CPlotoptAxis::CPlotoptAxis()
    : CPropertyPage(CPlotoptAxis::IDD)
    , m_bOTitle(TRUE)
    , m_bOGrid(TRUE)
    , m_bOTick(TRUE)
    , m_bOGridM(FALSE)
    , m_bOTickM(FALSE)
    , m_bOLabel(TRUE)
    , m_bBottomAxis(FALSE)
    , m_bLeftAxis(FALSE)
    , m_bTopAxis(FALSE)
    , m_bRightAxis(FALSE)
    , m_bAxis(TRUE)
    , m_strTitle(_T(""))
    , m_nAxisSize(1)
    , m_nGridCount(5)
    , m_nGridMCount(5)
{
    m_pPlot = NULL;
    m_pDoc = NULL;
    m_bIsModified = FALSE;
    m_bClosing = FALSE;
    m_bFontColor = FALSE;
    m_bAxisColor = FALSE;
    m_bLabelColor = FALSE;
    m_bGridColor = FALSE;
    m_bGridMColor = FALSE;
    m_bInitialized = FALSE;
    m_bTemplate = FALSE;
}

CPlotoptAxis::~CPlotoptAxis()
{

}

void CPlotoptAxis::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);

    DDX_Check(pDX, IDO_AXIS_BOTTOM, m_bBottomAxis);
    DDX_Check(pDX, IDO_AXIS_LEFT, m_bLeftAxis);
    DDX_Check(pDX, IDO_AXIS_TOP, m_bTopAxis);
    DDX_Check(pDX, IDO_AXIS_RIGHT, m_bRightAxis);
    DDX_Check(pDX, IDK_AXIS_STATUS, m_bAxis);
    DDX_Check(pDX, IDK_AXIS_OTITLE, m_bOTitle);
    DDX_Check(pDX, IDK_AXIS_OGRID, m_bOGrid);
    DDX_Check(pDX, IDK_AXIS_OTICK, m_bOTick);
    DDX_Check(pDX, IDK_AXIS_OGRIDM, m_bOGridM);
    DDX_Check(pDX, IDK_AXIS_OTICKM, m_bOTickM);
    DDX_Check(pDX, IDK_AXIS_OLABEL, m_bOLabel);
    DDX_Text(pDX, IDE_AXIS_TITLE, m_strTitle);
    DDV_MaxChars(pDX, m_strTitle, ML_STRSIZE - 10);
    DDX_Text(pDX, IDE_AXIS_SIZE, m_nAxisSize);
    DDX_Text(pDX, IDE_AXIS_GRIDCOUNT, m_nGridCount);
    DDX_Text(pDX, IDE_AXIS_GRIDMCOUNT, m_nGridMCount);
    DDX_Control(pDX, IDC_AXIS_FONTCOLOR, m_cbFontColor);
    DDX_Control(pDX, IDC_AXIS_COLOR, m_cbAxisColor);
    DDX_Control(pDX, IDC_AXIS_LABELCOLOR, m_cbLabelColor);
    DDX_Control(pDX, IDC_AXIS_GRIDCOLOR, m_cbGridColor);
    DDX_Control(pDX, IDC_AXIS_GRIDMCOLOR, m_cbGridMColor);
    DDX_Control(pDX, IDE_AXIS_LINK, m_edtLink);
    DDX_Text(pDX, IDE_AXIS_LINK, m_strLink);
    DDX_Text(pDX, IDL_AXIS_LINK, m_strLinkOutput);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}


BEGIN_MESSAGE_MAP(CPlotoptAxis, CPropertyPage)
    ON_BN_CLICKED(ID_AXIS_TFONT, &CPlotoptAxis::OnAxisTitleFont)
    ON_BN_CLICKED(ID_AXIS_LFONT, &CPlotoptAxis::OnAxisLabelFont)
    ON_BN_CLICKED(IDO_AXIS_BOTTOM, &CPlotoptAxis::OnAxisBottom)
    ON_BN_CLICKED(IDO_AXIS_LEFT, &CPlotoptAxis::OnAxisLeft)
    ON_BN_CLICKED(IDO_AXIS_TOP, &CPlotoptAxis::OnAxisTop)
    ON_BN_CLICKED(IDO_AXIS_RIGHT, &CPlotoptAxis::OnAxisRight)
    ON_BN_CLICKED(ID_AXIS_APPLY, &CPlotoptAxis::OnAxisApply)
    ON_BN_CLICKED(ID_AXIS_APPLYALL, &CPlotoptAxis::OnAxisApplyAll)
    ON_EN_CHANGE(IDE_AXIS_SIZE, &CPlotoptAxis::OnEnChangeAxisSize)
    ON_BN_CLICKED(IDK_AXIS_OTITLE, &CPlotoptAxis::OnBnClickedAxisOtitle)
    ON_BN_CLICKED(IDK_AXIS_STATUS, &CPlotoptAxis::OnBnClickedAxisStatus)
    ON_CBN_SELCHANGE(IDC_AXIS_FONTCOLOR, &CPlotoptAxis::OnCbnSelchangeFontcolor)
    ON_CBN_SELCHANGE(IDC_AXIS_COLOR, &CPlotoptAxis::OnCbnSelchangeAxiscolor)
    ON_CBN_SELCHANGE(IDC_AXIS_LABELCOLOR, &CPlotoptAxis::OnCbnSelchangeLabelcolor)
    ON_BN_CLICKED(IDK_AXIS_OGRID, &CPlotoptAxis::OnBnClickedAxisOgrid)
    ON_BN_CLICKED(IDK_AXIS_OTICK, &CPlotoptAxis::OnBnClickedAxisOtick)
    ON_BN_CLICKED(IDK_AXIS_OGRIDM, &CPlotoptAxis::OnBnClickedAxisOgridM)
    ON_BN_CLICKED(IDK_AXIS_OTICKM, &CPlotoptAxis::OnBnClickedAxisOtickM)
    ON_BN_CLICKED(IDK_AXIS_OLABEL, &CPlotoptAxis::OnBnClickedAxisOlabel)
    ON_CBN_SELCHANGE(IDC_AXIS_GRIDCOLOR, &CPlotoptAxis::OnCbnSelchangeGridcolor)
    ON_CBN_SELCHANGE(IDC_AXIS_GRIDMCOLOR, &CPlotoptAxis::OnCbnSelchangeGridMcolor)
    ON_BN_CLICKED(ID_PLOT_DEFAULT, &CPlotoptAxis::OnPlotDefault)
    ON_EN_CHANGE(IDE_AXIS_GRIDCOUNT, &CPlotoptAxis::OnEnChangeAxisGridcount)
    ON_EN_CHANGE(IDE_AXIS_GRIDMCOUNT, &CPlotoptAxis::OnEnChangeAxisGridMcount)
    ON_EN_CHANGE(IDE_AXIS_TITLE, &CPlotoptAxis::OnEnChangeAxisTitle)
    ON_EN_CHANGE(IDE_AXIS_LINK, &CPlotoptAxis::OnEnChangeAxisLink)
    ON_COMMAND(ID_HELP_CONTENTS, &CPlotoptAxis::OnHelpContents)
    ON_WM_DESTROY()
    ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CPlotoptAxis::AxisLinkParse()
{
    m_strLinkOutput = _T("");

    if ((m_bBottomAxis == FALSE) && (m_bLeftAxis == FALSE)) {
        return FALSE;
    }

    BOOL *pbRunning = &(((CSigmaDoc*)m_pDoc)->m_bRunning);
    if (pbRunning == NULL) {
        m_strLinkOutput = _T("Internal error.");
        return FALSE;
    }

    if (*pbRunning == TRUE) {
        m_strLinkOutput = _T("Internal error.");
        return FALSE;
    }

    char_t szFormula[ML_STRSIZEN];
    memset(szFormula, 0, ML_STRSIZEN * sizeof(char_t));
    SigmaApp.CStringToChar(m_strLink, szFormula, ML_STRSIZEN - 1);
    int_t nLength = (int_t)_tcslen((const char_t*)szFormula);
    if (nLength < 2) {
        return FALSE;
    }
    if (nLength >= ML_STRSIZEN) {
        m_strLinkOutput = _T("Invalid expression.");
        return FALSE;
    }

    real_t fval = 0.0, fbak;
    char_t szOutput[ML_STRSIZE], szT[ML_STRSIZES];
    memset(szT, 0, ML_STRSIZES * sizeof(char_t));

    *pbRunning = TRUE;

    BOOL bOK = FALSE, bBak = FALSE;

    // >>    Sauvergarder les variables du parser avant de lancer le calcul ...
    //        .. et définir les valeurs de x ou y avant d'évaluer l'expression
    bBak = SigmaApp.DoParse((m_bBottomAxis) ? _T("x=") : _T("y="), szOutput, &fbak);
    _tcsprintf(szT, ML_STRSIZES - 1, (m_bBottomAxis) ? _T("x=%e") : _T("y=%e"), (m_bBottomAxis) ? m_pPlot->axisbottom.min : m_pPlot->axisleft.min);
    SigmaApp.DoParse((const char_t*)szT, szOutput);

    bOK = SigmaApp.DoParse((const char_t*)szFormula, szOutput, &fval);
    if (bOK == FALSE) {
        m_strLinkOutput = _T("Invalid expression.");
        *pbRunning = FALSE;
        return FALSE;
    }

    m_strLinkOutput = (LPCTSTR)szOutput;

    if (m_bBottomAxis) {
        _tcscpy(m_pPlot->linktop, (const char_t*)szFormula);
    }
    else if (m_bLeftAxis) {
        _tcscpy(m_pPlot->linkright, (const char_t*)szFormula);
    }

    if (bBak) {
        _tcsprintf(szT, ML_STRSIZES - 1, (m_bBottomAxis) ? _T("x=%e") : _T("y=%e"), fbak);
        SigmaApp.DoParse((const char_t*)szT, szOutput);
    }

    *pbRunning = FALSE;
    return TRUE;
    // <<
}

BOOL CPlotoptAxis::Initialize()
{
    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        return FALSE;
    }

    axis_t *paxis = NULL;
    if (m_bBottomAxis == TRUE) {
        paxis = &(m_pPlot->axisbottom);
    }
    else if (m_bLeftAxis == TRUE) {
        paxis = &(m_pPlot->axisleft);
    }
    else if (m_bTopAxis == TRUE) {
        paxis = &(m_pPlot->axistop);
    }
    else if (m_bRightAxis == TRUE) {
        paxis = &(m_pPlot->axisright);
    }
    else {
        if ((m_pPlot->curvecount > 0) && (m_pPlot->curveactive >= 0) && (m_pPlot->curveactive < m_pPlot->curvecount)) {
            if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0x0F) == 0x00) {            // Y-Axis
                paxis = &(m_pPlot->axisleft);
                m_bLeftAxis = TRUE;
            }
            else if ((m_pPlot->curve[m_pPlot->curveactive].axisused & 0x0F) == 0x01) {        // Y2-Axis
                paxis = &(m_pPlot->axisright);
                m_bRightAxis = TRUE;
            }
            else {            // Y-Axis
                paxis = &(m_pPlot->axisleft);
                m_bLeftAxis = TRUE;
            }
        }
        else {            // Y-Axis
            paxis = &(m_pPlot->axisleft);
            m_bLeftAxis = TRUE;
        }
    }

    if (m_bBottomAxis == TRUE) {
        m_strLink = (LPCTSTR)(m_pPlot->linktop);
        m_edtLink.EnableWindow(TRUE);
    }
    else if (m_bLeftAxis == TRUE) {
        m_strLink = (LPCTSTR)(m_pPlot->linkright);
        m_edtLink.EnableWindow(TRUE);
    }
    else {
        m_strLink = _T("");
        m_edtLink.EnableWindow(FALSE);
    }

    m_bOTitle = (paxis->otitle == 1) ? TRUE : FALSE;
    m_strTitle = paxis->title.text;
    m_bAxis = ((paxis->status & 0x0F) == 0x0F) ? FALSE : TRUE;
    m_nAxisSize = (int_t) (paxis->size);
    m_bOGrid = (paxis->ogrid == 1) ? TRUE : FALSE;
    m_bOTick = (paxis->otick == 1) ? TRUE : FALSE;
    m_bOGridM = (paxis->ogridm == 1) ? TRUE : FALSE;
    m_bOTickM = (paxis->otickm == 1) ? TRUE : FALSE;
    m_bOLabel = (paxis->olabel == 1) ? TRUE : FALSE;
    m_nGridCount = paxis->gridcount;
    m_nGridMCount = paxis->gridmcount;

    m_cbFontColor.SetSelectedColor(RGB(paxis->title.font.color.r, paxis->title.font.color.g, paxis->title.font.color.b));
    m_cbAxisColor.SetSelectedColor(RGB(paxis->color.r, paxis->color.g, paxis->color.b));
    m_cbLabelColor.SetSelectedColor(RGB(paxis->lfont.color.r, paxis->lfont.color.g, paxis->lfont.color.b));
    m_cbGridColor.SetSelectedColor(RGB(paxis->gridcolor.r, paxis->gridcolor.g, paxis->gridcolor.b));
    m_cbGridMColor.SetSelectedColor(RGB(paxis->gridmcolor.r, paxis->gridmcolor.g, paxis->gridmcolor.b));

    UpdateData(FALSE);

    m_bIsModified = FALSE;

    return TRUE;
}

BOOL CPlotoptAxis::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  Add your specialized creation code here

    HWND hWnd = GetSafeHwnd();
    if (hWnd != NULL) {
        ::SendDlgItemMessage(hWnd, IDE_AXIS_SIZE, EM_LIMITTEXT, (WPARAM)1, (LPARAM)0);
        ::SendDlgItemMessage(hWnd, IDS_AXIS_SIZE, UDM_SETRANGE, 0, (LPARAM)MAKELONG(7,1));
        ::SendDlgItemMessage(hWnd, IDE_AXIS_LINK, EM_LIMITTEXT, (WPARAM)(ML_STRSIZEN - 1), (LPARAM)0);
    }

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    return TRUE;
}

BOOL CPlotoptAxis::OnApply()
{
    if (m_bIsModified) {
        // Processing
        OnAxisApply();

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

        if (m_bTemplate == FALSE) {
            ((CSigmaDoc*) m_pDoc)->SaveState();
        }
    }

    CPropertyPage::OnApply();

    m_bIsModified = FALSE;
    m_bFontColor = FALSE;
    m_bAxisColor = FALSE;
    m_bLabelColor = FALSE;
    m_bGridColor = FALSE;
    m_bGridMColor = FALSE;

    return TRUE;
}

// CPlotoptAxis message handlers

void CPlotoptAxis::OnAxisTitleFont()
{
    LOGFONT lft;
    axis_t *paxis = NULL;

    UpdateData(TRUE);    // Copy data to variables

    if (m_bBottomAxis == TRUE) {
        paxis = &(m_pPlot->axisbottom);
    }
    else if (m_bLeftAxis == TRUE) {
        paxis = &(m_pPlot->axisleft);
    }
    else if (m_bTopAxis == TRUE) {
        paxis = &(m_pPlot->axistop);
    }
    else if (m_bRightAxis == TRUE) {
        paxis = &(m_pPlot->axisright);
    }
    else {
        return;
    }

    memset(&lft, 0, sizeof(lft));
    
    if (SigmaApp.FontToLogfont(paxis->title.font, &lft) == FALSE) {
        return;
    }

    if (SigmaApp.GetFont(m_hWnd, &lft)) {
        SigmaApp.LogfontToFont(lft, &(paxis->title.font));

        // Update window plot
        if (m_bClosing == FALSE) {
            if (m_bTemplate == TRUE) {
                GetParent()->GetParent()->Invalidate();
            }
            else {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }

        // [FIX-SG200-002] The document state is saved when the graph fonts change.
        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }
    }
}

void CPlotoptAxis::OnAxisLabelFont()
{
    LOGFONT lft;
    axis_t *paxis = NULL;

    UpdateData(TRUE);    // Copy data to variables

    if (m_bBottomAxis == TRUE) {
        paxis = &(m_pPlot->axisbottom);
    }
    else if (m_bLeftAxis == TRUE) {
        paxis = &(m_pPlot->axisleft);
    }
    else if (m_bTopAxis == TRUE) {
        paxis = &(m_pPlot->axistop);
    }
    else if (m_bRightAxis == TRUE) {
        paxis = &(m_pPlot->axisright);
    }
    else {
        return;
    }

    memset(&lft, 0, sizeof(lft));

    if (SigmaApp.FontToLogfont(paxis->lfont, &lft) == FALSE) {
        return;
    }

    if (SigmaApp.GetFont(m_hWnd, &lft)) {
        SigmaApp.LogfontToFont(lft, &(paxis->lfont));

        // Update window plot
        if (m_bClosing == FALSE) {
            if (m_bTemplate == TRUE) {
                GetParent()->GetParent()->Invalidate();
            }
            else {
                ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
            }
        }

        // [FIX-SG200-002] The document state is saved when the graph fonts change.
        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }
    }
}

void CPlotoptAxis::OnAxisBottom()
{
    UpdateData(TRUE);    // Copy data to variables
    if (m_bBottomAxis == TRUE) {
        m_strTitle = m_pPlot->axisbottom.title.text;
        m_bOTitle = (m_pPlot->axisbottom.otitle == 1) ? TRUE : FALSE;
        m_bAxis = ((m_pPlot->axisbottom.status & 0x0F) == 0x0F) ? FALSE : TRUE;
        m_nAxisSize = (int_t) (m_pPlot->axisbottom.size);
        m_bOGrid = (m_pPlot->axisbottom.ogrid == 1) ? TRUE : FALSE;
        m_bOTick = (m_pPlot->axisbottom.otick == 1) ? TRUE : FALSE;
        m_bOGridM = (m_pPlot->axisbottom.ogridm == 1) ? TRUE : FALSE;
        m_bOTickM = (m_pPlot->axisbottom.otickm == 1) ? TRUE : FALSE;
        m_bOLabel = (m_pPlot->axisbottom.olabel == 1) ? TRUE : FALSE;
        m_nGridCount = m_pPlot->axisbottom.gridcount;
        m_nGridMCount = m_pPlot->axisbottom.gridmcount;
        m_strLink = (LPCTSTR)(m_pPlot->linktop);
        m_edtLink.EnableWindow(TRUE);
        m_cbFontColor.SetSelectedColor(RGB(m_pPlot->axisbottom.title.font.color.r, m_pPlot->axisbottom.title.font.color.g, m_pPlot->axisbottom.title.font.color.b));
        m_cbAxisColor.SetSelectedColor(RGB(m_pPlot->axisbottom.color.r, m_pPlot->axisbottom.color.g, m_pPlot->axisbottom.color.b));
        m_cbLabelColor.SetSelectedColor(RGB(m_pPlot->axisbottom.lfont.color.r, m_pPlot->axisbottom.lfont.color.g, m_pPlot->axisbottom.lfont.color.b));
        m_cbGridColor.SetSelectedColor(RGB(m_pPlot->axisbottom.gridcolor.r, m_pPlot->axisbottom.gridcolor.g, m_pPlot->axisbottom.gridcolor.b));
        m_cbGridMColor.SetSelectedColor(RGB(m_pPlot->axisbottom.gridmcolor.r, m_pPlot->axisbottom.gridmcolor.g, m_pPlot->axisbottom.gridmcolor.b));
        UpdateData(FALSE);
    }
}

void CPlotoptAxis::OnAxisLeft()
{
    UpdateData(TRUE);    // Copy data to variables
    if (m_bLeftAxis == TRUE) {
        m_strTitle = m_pPlot->axisleft.title.text;
        m_bOTitle = (m_pPlot->axisleft.otitle == 1) ? TRUE : FALSE;
        m_bAxis = ((m_pPlot->axisleft.status & 0x0F) == 0x0F) ? FALSE : TRUE;
        m_nAxisSize = (int_t) (m_pPlot->axisleft.size);
        m_bOGrid = (m_pPlot->axisleft.ogrid == 1) ? TRUE : FALSE;
        m_bOTick = (m_pPlot->axisleft.otick == 1) ? TRUE : FALSE;
        m_bOGridM = (m_pPlot->axisleft.ogridm == 1) ? TRUE : FALSE;
        m_bOTickM = (m_pPlot->axisleft.otickm == 1) ? TRUE : FALSE;
        m_bOLabel = (m_pPlot->axisleft.olabel == 1) ? TRUE : FALSE;
        m_nGridCount = m_pPlot->axisleft.gridcount;
        m_nGridMCount = m_pPlot->axisleft.gridmcount;
        m_strLink = (LPCTSTR)(m_pPlot->linkright);
        m_edtLink.EnableWindow(TRUE);
        m_cbFontColor.SetSelectedColor(RGB(m_pPlot->axisleft.title.font.color.r, m_pPlot->axisleft.title.font.color.g, m_pPlot->axisleft.title.font.color.b));
        m_cbAxisColor.SetSelectedColor(RGB(m_pPlot->axisleft.color.r, m_pPlot->axisleft.color.g, m_pPlot->axisleft.color.b));
        m_cbLabelColor.SetSelectedColor(RGB(m_pPlot->axisleft.lfont.color.r, m_pPlot->axisleft.lfont.color.g, m_pPlot->axisleft.lfont.color.b));
        m_cbGridColor.SetSelectedColor(RGB(m_pPlot->axisleft.gridcolor.r, m_pPlot->axisleft.gridcolor.g, m_pPlot->axisleft.gridcolor.b));
        m_cbGridMColor.SetSelectedColor(RGB(m_pPlot->axisleft.gridmcolor.r, m_pPlot->axisleft.gridmcolor.g, m_pPlot->axisleft.gridmcolor.b));
        UpdateData(FALSE);
    }
}

void CPlotoptAxis::OnAxisTop()
{
    UpdateData(TRUE);    // Copy data to variables
    if (m_bTopAxis == TRUE) {
        m_strTitle = m_pPlot->axistop.title.text;
        m_bOTitle = (m_pPlot->axistop.otitle == 1) ? TRUE : FALSE;
        m_bAxis = ((m_pPlot->axistop.status & 0x0F) == 0x0F) ? FALSE : TRUE;
        m_nAxisSize = (int_t) (m_pPlot->axistop.size);
        m_bOGrid = (m_pPlot->axistop.ogrid == 1) ? TRUE : FALSE;
        m_bOTick = (m_pPlot->axistop.otick == 1) ? TRUE : FALSE;
        m_bOGridM = (m_pPlot->axistop.ogridm == 1) ? TRUE : FALSE;
        m_bOTickM = (m_pPlot->axistop.otickm == 1) ? TRUE : FALSE;
        m_bOLabel = (m_pPlot->axistop.olabel == 1) ? TRUE : FALSE;
        m_nGridCount = m_pPlot->axistop.gridcount;
        m_nGridMCount = m_pPlot->axistop.gridmcount;
        m_strLink = _T("");
        m_edtLink.EnableWindow(FALSE);
        m_cbFontColor.SetSelectedColor(RGB(m_pPlot->axistop.title.font.color.r, m_pPlot->axistop.title.font.color.g, m_pPlot->axistop.title.font.color.b));
        m_cbAxisColor.SetSelectedColor(RGB(m_pPlot->axistop.color.r, m_pPlot->axistop.color.g, m_pPlot->axistop.color.b));
        m_cbLabelColor.SetSelectedColor(RGB(m_pPlot->axistop.lfont.color.r, m_pPlot->axistop.lfont.color.g, m_pPlot->axistop.lfont.color.b));
        m_cbGridColor.SetSelectedColor(RGB(m_pPlot->axistop.gridcolor.r, m_pPlot->axistop.gridcolor.g, m_pPlot->axistop.gridcolor.b));
        m_cbGridMColor.SetSelectedColor(RGB(m_pPlot->axistop.gridmcolor.r, m_pPlot->axistop.gridmcolor.g, m_pPlot->axistop.gridmcolor.b));
        UpdateData(FALSE);
    }
}

void CPlotoptAxis::OnAxisRight()
{
    UpdateData(TRUE);    // Copy data to variables
    if (m_bRightAxis == TRUE) {
        m_strTitle = m_pPlot->axisright.title.text;
        m_bOTitle = (m_pPlot->axisright.otitle == 1) ? TRUE : FALSE;
        m_bAxis = ((m_pPlot->axisright.status & 0x0F) == 0x0F) ? FALSE : TRUE;
        m_nAxisSize = (int_t) (m_pPlot->axisright.size);
        m_bOGrid = (m_pPlot->axisright.ogrid == 1) ? TRUE : FALSE;
        m_bOTick = (m_pPlot->axisright.otick == 1) ? TRUE : FALSE;
        m_bOGridM = (m_pPlot->axisright.ogridm == 1) ? TRUE : FALSE;
        m_bOTickM = (m_pPlot->axisright.otickm == 1) ? TRUE : FALSE;
        m_bOLabel = (m_pPlot->axisright.olabel == 1) ? TRUE : FALSE;
        m_nGridCount = m_pPlot->axisright.gridcount;
        m_nGridMCount = m_pPlot->axisright.gridmcount;
        m_strLink = _T("");
        m_edtLink.EnableWindow(FALSE);
        m_cbFontColor.SetSelectedColor(RGB(m_pPlot->axisright.title.font.color.r, m_pPlot->axisright.title.font.color.g, m_pPlot->axisright.title.font.color.b));
        m_cbAxisColor.SetSelectedColor(RGB(m_pPlot->axisright.color.r, m_pPlot->axisright.color.g, m_pPlot->axisright.color.b));
        m_cbLabelColor.SetSelectedColor(RGB(m_pPlot->axisright.lfont.color.r, m_pPlot->axisright.lfont.color.g, m_pPlot->axisright.lfont.color.b));
        m_cbGridColor.SetSelectedColor(RGB(m_pPlot->axisright.gridcolor.r, m_pPlot->axisright.gridcolor.g, m_pPlot->axisright.gridcolor.b));
        m_cbGridMColor.SetSelectedColor(RGB(m_pPlot->axisright.gridmcolor.r, m_pPlot->axisright.gridmcolor.g, m_pPlot->axisright.gridmcolor.b));
        UpdateData(FALSE);
    }
}

void CPlotoptAxis::OnAxisApply()
{
    COLORREF clrTmp;
    axis_t *paxis = NULL;

    
    if (m_bIsModified == FALSE) {
        return;
    }

    UpdateData(TRUE);    // Copy data to variables

    if (m_bBottomAxis == TRUE) {
        paxis = &(m_pPlot->axisbottom);
    }
    else if (m_bLeftAxis == TRUE) {
        paxis = &(m_pPlot->axisleft);
    }
    else if (m_bTopAxis == TRUE) {
        paxis = &(m_pPlot->axistop);
    }
    else if (m_bRightAxis == TRUE) {
        paxis = &(m_pPlot->axisright);
    }
    else {
        return;
    }

    if (m_bFontColor == TRUE) {
        clrTmp = m_cbFontColor.GetSelectedColor();
        paxis->title.font.color.r = GetRValue(clrTmp);
        paxis->title.font.color.g = GetGValue(clrTmp);
        paxis->title.font.color.b = GetBValue(clrTmp);
    }
    if (m_bLabelColor == TRUE) {
        clrTmp = m_cbLabelColor.GetSelectedColor();
        paxis->lfont.color.r = GetRValue(clrTmp);
        paxis->lfont.color.g = GetGValue(clrTmp);
        paxis->lfont.color.b = GetBValue(clrTmp);
    }
    if (m_bAxisColor == TRUE) {
        clrTmp = m_cbAxisColor.GetSelectedColor();
        paxis->color.r = GetRValue(clrTmp);
        paxis->color.g = GetGValue(clrTmp);
        paxis->color.b = GetBValue(clrTmp);
    }
    if (m_bGridColor == TRUE) {
        clrTmp = m_cbGridColor.GetSelectedColor();
        paxis->gridcolor.r = GetRValue(clrTmp);
        paxis->gridcolor.g = GetGValue(clrTmp);
        paxis->gridcolor.b = GetBValue(clrTmp);
        paxis->tickcolor.r = GetRValue(clrTmp);
        paxis->tickcolor.g = GetGValue(clrTmp);
        paxis->tickcolor.b = GetBValue(clrTmp);
    }
    if (m_bGridMColor == TRUE) {
        clrTmp = m_cbGridMColor.GetSelectedColor();
        paxis->gridmcolor.r = GetRValue(clrTmp);
        paxis->gridmcolor.g = GetGValue(clrTmp);
        paxis->gridmcolor.b = GetBValue(clrTmp);
        paxis->tickmcolor.r = GetRValue(clrTmp);
        paxis->tickmcolor.g = GetGValue(clrTmp);
        paxis->tickmcolor.b = GetBValue(clrTmp);
    }
    if (m_bAxis == TRUE) {
        paxis->status &= 0xF0;
    }
    else {
        paxis->status |= 0x0F;
    }
    paxis->otitle = (m_bOTitle == TRUE) ? 1 : 0;
    SigmaApp.CStringToChar(m_strTitle, paxis->title.text, ML_STRSIZE - 1);

    if ((m_nAxisSize >= 1) && (m_nAxisSize <= 7)) {
        paxis->size = (byte_t)m_nAxisSize;
    }
    paxis->ogrid = (m_bOGrid == TRUE) ? 1 : 0;
    paxis->otick = (m_bOTick == TRUE) ? 1 : 0;
    paxis->ogridm = (m_bOGridM == TRUE) ? 1 : 0;
    paxis->otickm = (m_bOTickM == TRUE) ? 1 : 0;
    paxis->olabel = (m_bOLabel == TRUE) ? 1 : 0;
    paxis->gridcount = m_nGridCount;
    paxis->gridmcount = m_nGridMCount;

    if (paxis->scale == 1) {
        paxis->gridmcount = 9;
    }

    if (m_strLink.GetLength() > 2) {
        AxisLinkParse();
    }
    else {
        m_strLink = _T("");
    }

    // Update window plot
    if (m_bClosing == FALSE) {
        if (m_bTemplate == TRUE) {
            GetParent()->GetParent()->Invalidate();
        }
        else {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }

    m_bIsModified = FALSE;
    m_bFontColor = FALSE;
    m_bAxisColor = FALSE;
    m_bLabelColor = FALSE;
    m_bGridColor = FALSE;

    UpdateData(FALSE);

    return;
}

void CPlotoptAxis::OnAxisApplyAll()
{
    COLORREF clrTmp;
    
    if (m_bIsModified == FALSE) {
        return;
    }

    UpdateData(TRUE);    // Copy data to variables

    if (m_bFontColor == TRUE) {
        clrTmp = m_cbFontColor.GetSelectedColor();
        m_pPlot->axisbottom.title.font.color.r = GetRValue(clrTmp);
        m_pPlot->axisbottom.title.font.color.g = GetGValue(clrTmp);
        m_pPlot->axisbottom.title.font.color.b = GetBValue(clrTmp);
        m_pPlot->axisleft.title.font.color.r = GetRValue(clrTmp);
        m_pPlot->axisleft.title.font.color.g = GetGValue(clrTmp);
        m_pPlot->axisleft.title.font.color.b = GetBValue(clrTmp);
        m_pPlot->axistop.title.font.color.r = GetRValue(clrTmp);
        m_pPlot->axistop.title.font.color.g = GetGValue(clrTmp);
        m_pPlot->axistop.title.font.color.b = GetBValue(clrTmp);
        m_pPlot->axisright.title.font.color.r = GetRValue(clrTmp);
        m_pPlot->axisright.title.font.color.g = GetGValue(clrTmp);
        m_pPlot->axisright.title.font.color.b = GetBValue(clrTmp);
    }
    if (m_bLabelColor == TRUE) {
        clrTmp = m_cbLabelColor.GetSelectedColor();
        m_pPlot->axisbottom.lfont.color.r = GetRValue(clrTmp);
        m_pPlot->axisbottom.lfont.color.g = GetGValue(clrTmp);
        m_pPlot->axisbottom.lfont.color.b = GetBValue(clrTmp);
        m_pPlot->axisleft.lfont.color.r = GetRValue(clrTmp);
        m_pPlot->axisleft.lfont.color.g = GetGValue(clrTmp);
        m_pPlot->axisleft.lfont.color.b = GetBValue(clrTmp);
        m_pPlot->axistop.lfont.color.r = GetRValue(clrTmp);
        m_pPlot->axistop.lfont.color.g = GetGValue(clrTmp);
        m_pPlot->axistop.lfont.color.b = GetBValue(clrTmp);
        m_pPlot->axisright.lfont.color.r = GetRValue(clrTmp);
        m_pPlot->axisright.lfont.color.g = GetGValue(clrTmp);
        m_pPlot->axisright.lfont.color.b = GetBValue(clrTmp);
    }
    if (m_bAxisColor == TRUE) {
        clrTmp = m_cbAxisColor.GetSelectedColor();
        m_pPlot->axisbottom.color.r = GetRValue(clrTmp);
        m_pPlot->axisbottom.color.g = GetGValue(clrTmp);
        m_pPlot->axisbottom.color.b = GetBValue(clrTmp);
        m_pPlot->axisleft.color.r = GetRValue(clrTmp);
        m_pPlot->axisleft.color.g = GetGValue(clrTmp);
        m_pPlot->axisleft.color.b = GetBValue(clrTmp);
        m_pPlot->axistop.color.r = GetRValue(clrTmp);
        m_pPlot->axistop.color.g = GetGValue(clrTmp);
        m_pPlot->axistop.color.b = GetBValue(clrTmp);
        m_pPlot->axisright.color.r = GetRValue(clrTmp);
        m_pPlot->axisright.color.g = GetGValue(clrTmp);
        m_pPlot->axisright.color.b = GetBValue(clrTmp);
    }
    if (m_bGridColor == TRUE) {
        clrTmp = m_cbGridColor.GetSelectedColor();
        m_pPlot->axisbottom.gridcolor.r = GetRValue(clrTmp);
        m_pPlot->axisbottom.gridcolor.g = GetGValue(clrTmp);
        m_pPlot->axisbottom.gridcolor.b = GetBValue(clrTmp);
        m_pPlot->axisleft.gridcolor.r = GetRValue(clrTmp);
        m_pPlot->axisleft.gridcolor.g = GetGValue(clrTmp);
        m_pPlot->axisleft.gridcolor.b = GetBValue(clrTmp);
        m_pPlot->axisbottom.tickcolor.r = GetRValue(clrTmp);
        m_pPlot->axisbottom.tickcolor.g = GetGValue(clrTmp);
        m_pPlot->axisbottom.tickcolor.b = GetBValue(clrTmp);
        m_pPlot->axisleft.tickcolor.r = GetRValue(clrTmp);
        m_pPlot->axisleft.tickcolor.g = GetGValue(clrTmp);
        m_pPlot->axisleft.tickcolor.b = GetBValue(clrTmp);
        m_pPlot->axistop.gridcolor.r = GetRValue(clrTmp);
        m_pPlot->axistop.gridcolor.g = GetGValue(clrTmp);
        m_pPlot->axistop.gridcolor.b = GetBValue(clrTmp);
        m_pPlot->axisright.gridcolor.r = GetRValue(clrTmp);
        m_pPlot->axisright.gridcolor.g = GetGValue(clrTmp);
        m_pPlot->axisright.gridcolor.b = GetBValue(clrTmp);
        m_pPlot->axistop.tickcolor.r = GetRValue(clrTmp);
        m_pPlot->axistop.tickcolor.g = GetGValue(clrTmp);
        m_pPlot->axistop.tickcolor.b = GetBValue(clrTmp);
        m_pPlot->axisright.tickcolor.r = GetRValue(clrTmp);
        m_pPlot->axisright.tickcolor.g = GetGValue(clrTmp);
        m_pPlot->axisright.tickcolor.b = GetBValue(clrTmp);
    }
    if (m_bGridMColor == TRUE) {
        clrTmp = m_cbGridMColor.GetSelectedColor();
        m_pPlot->axisbottom.gridmcolor.r = GetRValue(clrTmp);
        m_pPlot->axisbottom.gridmcolor.g = GetGValue(clrTmp);
        m_pPlot->axisbottom.gridmcolor.b = GetBValue(clrTmp);
        m_pPlot->axisleft.gridmcolor.r = GetRValue(clrTmp);
        m_pPlot->axisleft.gridmcolor.g = GetGValue(clrTmp);
        m_pPlot->axisleft.gridmcolor.b = GetBValue(clrTmp);
        m_pPlot->axisbottom.tickmcolor.r = GetRValue(clrTmp);
        m_pPlot->axisbottom.tickmcolor.g = GetGValue(clrTmp);
        m_pPlot->axisbottom.tickmcolor.b = GetBValue(clrTmp);
        m_pPlot->axisleft.tickmcolor.r = GetRValue(clrTmp);
        m_pPlot->axisleft.tickmcolor.g = GetGValue(clrTmp);
        m_pPlot->axisleft.tickmcolor.b = GetBValue(clrTmp);
        m_pPlot->axistop.gridmcolor.r = GetRValue(clrTmp);
        m_pPlot->axistop.gridmcolor.g = GetGValue(clrTmp);
        m_pPlot->axistop.gridmcolor.b = GetBValue(clrTmp);
        m_pPlot->axisright.gridmcolor.r = GetRValue(clrTmp);
        m_pPlot->axisright.gridmcolor.g = GetGValue(clrTmp);
        m_pPlot->axisright.gridmcolor.b = GetBValue(clrTmp);
        m_pPlot->axistop.tickmcolor.r = GetRValue(clrTmp);
        m_pPlot->axistop.tickmcolor.g = GetGValue(clrTmp);
        m_pPlot->axistop.tickmcolor.b = GetBValue(clrTmp);
        m_pPlot->axisright.tickmcolor.r = GetRValue(clrTmp);
        m_pPlot->axisright.tickmcolor.g = GetGValue(clrTmp);
        m_pPlot->axisright.tickmcolor.b = GetBValue(clrTmp);
    }

    if (m_bOTitle == FALSE) {
        if ((m_bTopAxis == FALSE) && (m_bRightAxis == FALSE)) {
            m_pPlot->axisbottom.otitle = 0;
        }
    }
    else {
        m_pPlot->axisbottom.otitle = 1;
    }

    if (m_bAxis == TRUE) {
        m_pPlot->axisbottom.status &= 0xF0;
    }
    else {
        m_pPlot->axisbottom.status |= 0x0F;
    }
    m_pPlot->axistop.otitle = (m_bOTitle == TRUE) ? 1 : 0;
    if ((m_pPlot->axisbottom.status & 0x0F) == 0x0F) {
        if (m_bAxis == TRUE) {
            m_pPlot->axistop.status &= 0xF0;
        }
        else {
            m_pPlot->axistop.status |= 0x0F;
        }
    }
    if ((m_nAxisSize >= 1) && (m_nAxisSize <= 7)) {
        m_pPlot->axisbottom.size = (byte_t)m_nAxisSize;
        m_pPlot->axistop.size = (byte_t)m_nAxisSize;
    }
    m_pPlot->axisbottom.ogrid = (m_bOGrid == TRUE) ? 1 : 0;
    m_pPlot->axisbottom.otick = (m_bOTick == TRUE) ? 1 : 0;
    m_pPlot->axisbottom.ogridm = (m_bOGridM == TRUE) ? 1 : 0;
    m_pPlot->axisbottom.otickm = (m_bOTickM == TRUE) ? 1 : 0;

    if (m_bOLabel == FALSE) {
        if ((m_bTopAxis == FALSE) && (m_bRightAxis == FALSE)) {
            m_pPlot->axisbottom.olabel = 0;
        }
    }
    else {
        m_pPlot->axisbottom.olabel = 1;
    }

    m_pPlot->axisbottom.gridcount = m_nGridCount;
    m_pPlot->axisbottom.gridmcount = m_nGridMCount;
    m_pPlot->axistop.ogrid = (m_bOGrid == TRUE) ? 1 : 0;
    m_pPlot->axistop.otick = (m_bOTick == TRUE) ? 1 : 0;
    m_pPlot->axistop.ogridm = (m_bOGridM == TRUE) ? 1 : 0;
    m_pPlot->axistop.otickm = (m_bOTickM == TRUE) ? 1 : 0;
    m_pPlot->axistop.olabel = (m_bOLabel == TRUE) ? 1 : 0;
    m_pPlot->axistop.gridcount = m_nGridCount;
    m_pPlot->axistop.gridmcount = m_nGridMCount;

    if (m_bOTitle == FALSE) {
        if ((m_bTopAxis == FALSE) && (m_bRightAxis == FALSE)) {
            m_pPlot->axisleft.otitle = 0;
        }
    }
    else {
        m_pPlot->axisleft.otitle = 1;
    }

    if (m_bAxis == TRUE) {
        m_pPlot->axisleft.status &= 0xF0;
    }
    else {
        m_pPlot->axisleft.status |= 0x0F;
    }
    m_pPlot->axisright.otitle = (m_bOTitle == TRUE) ? 1 : 0;
    if ((m_pPlot->axisleft.status & 0x0F) == 0x0F) {
        if (m_bAxis == TRUE) {
            m_pPlot->axisright.status &= 0xF0;
        }
        else {
            m_pPlot->axisright.status |= 0x0F;
        }
    }
    if ((m_nAxisSize >= 1) && (m_nAxisSize <= 7)) {
        m_pPlot->axisleft.size = (byte_t)m_nAxisSize;
        m_pPlot->axisright.size = (byte_t)m_nAxisSize;
    }
    m_pPlot->axisleft.ogrid = (m_bOGrid == TRUE) ? 1 : 0;
    m_pPlot->axisleft.otick = (m_bOTick == TRUE) ? 1 : 0;
    m_pPlot->axisleft.ogridm = (m_bOGridM == TRUE) ? 1 : 0;
    m_pPlot->axisleft.otickm = (m_bOTickM == TRUE) ? 1 : 0;

    if (m_bOLabel == FALSE) {
        if ((m_bTopAxis == FALSE) && (m_bRightAxis == FALSE)) {
            m_pPlot->axisleft.olabel = 0;
        }
    }
    else {
        m_pPlot->axisleft.olabel = 1;
    }

    m_pPlot->axisleft.gridcount = m_nGridCount;
    m_pPlot->axisleft.gridmcount = m_nGridMCount;
    m_pPlot->axisright.ogrid = (m_bOGrid == TRUE) ? 1 : 0;
    m_pPlot->axisright.otick = (m_bOTick == TRUE) ? 1 : 0;
    m_pPlot->axisright.ogridm = (m_bOGridM == TRUE) ? 1 : 0;
    m_pPlot->axisright.otickm = (m_bOTickM == TRUE) ? 1 : 0;
    m_pPlot->axisright.olabel = (m_bOLabel == TRUE) ? 1 : 0;
    m_pPlot->axisright.gridcount = m_nGridCount;
    m_pPlot->axisright.gridmcount = m_nGridMCount;

    // Update window plot
    if (m_bClosing == FALSE) {
        if (m_bTemplate == TRUE) {
            GetParent()->GetParent()->Invalidate();
        }
        else {
            ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);
        }
    }

    m_bIsModified = FALSE;
    m_bFontColor = FALSE;
    m_bAxisColor = FALSE;
    m_bLabelColor = FALSE;
    m_bGridColor = FALSE;
    m_bGridMColor = FALSE;

    if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }
}

void CPlotoptAxis::OnEnChangeAxisSize()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnBnClickedAxisOtitle()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnBnClickedAxisStatus()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnCbnSelchangeFontcolor()
{
    m_bFontColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnCbnSelchangeAxiscolor()
{
    m_bAxisColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnCbnSelchangeLabelcolor()
{
    m_bLabelColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnBnClickedAxisOgrid()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnBnClickedAxisOtick()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnBnClickedAxisOgridM()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnBnClickedAxisOtickM()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnBnClickedAxisOlabel()
{
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnCbnSelchangeGridcolor()
{
    m_bGridColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnCbnSelchangeGridMcolor()
{
    m_bGridMColor = TRUE;
    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnPlotDefault()
{
    OnApply();

    SigmaApp.m_pDefaultPlot->errcode = 0;
    pl_plot_dupstatic(SigmaApp.m_pDefaultPlot, m_pPlot);

    SigmaApp.setPlotSettings();    // Dynamic element of plot will be null
}

void CPlotoptAxis::OnEnChangeAxisGridcount()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnEnChangeAxisGridMcount()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnEnChangeAxisTitle()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnEnChangeAxisLink()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    SetModified(TRUE);    m_bIsModified = TRUE;
}

void CPlotoptAxis::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // Do not call CPropertyPage::OnPaint() for painting messages

    if (m_bInitialized == FALSE) {
        Initialize();
    }
}

void CPlotoptAxis::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/graph.html#graph_options_axes"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CPlotoptAxis::PreTranslateMessage(MSG* pMsg)
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

void CPlotoptAxis::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}


