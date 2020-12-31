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
#include "PlotoptScale.h"
#include "SigmaDoc.h"


// CPlotoptScale dialog

IMPLEMENT_DYNAMIC(CPlotoptScale, CPropertyPage)

CPlotoptScale::CPlotoptScale()
    : CPropertyPage(CPlotoptScale::IDD)
    , m_bBottomAxis(FALSE)
    , m_bLeftAxis(TRUE)
    , m_bTopAxis(FALSE)
    , m_bRightAxis(FALSE)
    , m_fAxisMin(0.0)
    , m_fAxisMax(1.0)
    , m_nPrefix(2)
    , m_nSuffix(2)
    , m_bAxisLinear(TRUE)
    , m_bAxisLog(FALSE)
    , m_bAxisAbs(FALSE)
{
    m_pPlot = NULL;
    m_pDoc = NULL;
    m_bIsModified = FALSE;
    m_bClosing = FALSE;
    m_bInitialized = FALSE;

    m_bTemplate = FALSE;
}

CPlotoptScale::~CPlotoptScale()
{

}

void CPlotoptScale::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Check(pDX, IDO_AXIS_BOTTOM, m_bBottomAxis);
    DDX_Check(pDX, IDO_AXIS_LEFT, m_bLeftAxis);
    DDX_Check(pDX, IDO_AXIS_TOP, m_bTopAxis);
    DDX_Check(pDX, IDO_AXIS_RIGHT, m_bRightAxis);
    DDX_Control(pDX, IDE_AXIS_MIN, m_edtAxisMin);
    DDX_Control(pDX, IDE_AXIS_MAX, m_edtAxisMax);
    DDX_Text(pDX, IDE_AXIS_PREFIX, m_nPrefix);
    DDV_MinMaxInt(pDX, m_nPrefix, 0, 9);
    DDX_Text(pDX, IDE_AXIS_SUFFIX, m_nSuffix);
    DDV_MinMaxInt(pDX, m_nSuffix, 0, 9);
    DDX_Check(pDX, IDO_AXIS_LINEAR, m_bAxisLinear);
    DDX_Check(pDX, IDO_AXIS_LOG, m_bAxisLog);
    DDX_Check(pDX, IDK_AXIS_ABS, m_bAxisAbs);
    DDX_Control(pDX, ID_HELP_CONTENTS, m_btnHelp);
}

BEGIN_MESSAGE_MAP(CPlotoptScale, CPropertyPage)
    ON_CBN_SELCHANGE(IDC_AXIS_FORMAT, &CPlotoptScale::OnCbnSelchangeAxisformat)
    ON_EN_CHANGE(IDE_AXIS_MIN, &CPlotoptScale::OnEnChangeAxismin)
    ON_EN_CHANGE(IDE_AXIS_MAX, &CPlotoptScale::OnEnChangeAxismax)
    ON_BN_CLICKED(IDO_AXIS_BOTTOM, &CPlotoptScale::OnAxisBottom)
    ON_BN_CLICKED(IDO_AXIS_LEFT, &CPlotoptScale::OnAxisLeft)
    ON_BN_CLICKED(IDO_AXIS_TOP, &CPlotoptScale::OnAxisTop)
    ON_BN_CLICKED(IDO_AXIS_RIGHT, &CPlotoptScale::OnAxisRight)
    ON_EN_CHANGE(IDE_AXIS_PREFIX, &CPlotoptScale::OnEnChangeAxisprefix)
    ON_EN_CHANGE(IDE_AXIS_SUFFIX, &CPlotoptScale::OnEnChangeAxissuffix)
    ON_BN_CLICKED(ID_SCALE_DEFAULT, &CPlotoptScale::OnPlotDefault)
    ON_BN_CLICKED(IDO_AXIS_LINEAR, &CPlotoptScale::OnBnClickedAxisLinear)
    ON_BN_CLICKED(IDO_AXIS_LOG, &CPlotoptScale::OnBnClickedAxisLog)
    ON_BN_CLICKED(IDK_AXIS_ABS, &CPlotoptScale::OnBnClickedAxisAbs)
    ON_BN_CLICKED(ID_SCALE_AUTO, &CPlotoptScale::OnBnClickedScaleAuto)
    ON_COMMAND(ID_HELP_CONTENTS, &CPlotoptScale::OnHelpContents)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CPlotoptScale::VariableToControl()
{
    axis_t *paxis = NULL;

    if (m_bLeftAxis == TRUE) {
        paxis = &(m_pPlot->axisleft);
    }
    else if (m_bBottomAxis == TRUE) {
        paxis = &(m_pPlot->axisbottom);
    }
    else     if (m_bTopAxis == TRUE) {
        paxis = &(m_pPlot->axistop);
    }
    else if (m_bRightAxis == TRUE) {
        paxis = &(m_pPlot->axisright);
    }
    else {
        return;
    }

    m_edtAxisMin.SetValue(m_fAxisMin, (const char_t*)(paxis->format));
    m_edtAxisMax.SetValue(m_fAxisMax, (const char_t*)(paxis->format));

    UpdateData(FALSE);
}

void CPlotoptScale::ControlToVariable()
{
    UpdateData(TRUE);

    m_fAxisMin = m_edtAxisMin.GetValue();
    m_fAxisMax = m_edtAxisMax.GetValue();
}

BOOL CPlotoptScale::Initialize()
{
    CComboBox *pComboFormat = NULL;

    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        return FALSE;
    }

    axis_t *paxis = NULL;

    if (m_bLeftAxis == TRUE) {
        paxis = &(m_pPlot->axisleft);
    }
    else if (m_bBottomAxis == TRUE) {
        paxis = &(m_pPlot->axisbottom);
    }
    else     if (m_bTopAxis == TRUE) {
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

    m_fAxisMin = paxis->min;
    m_fAxisMax = paxis->max;
    ml_getformat((const char_t*)(paxis->format), &m_nPrefix, &m_nSuffix);

    pComboFormat = (CComboBox*)GetDlgItem(IDC_AXIS_FORMAT);

    if (pComboFormat == NULL) {
        return FALSE;
    }

    int_t iFmt = -1, ilen = _tcslen((const char_t*)(paxis->format));
    for (int ii = 0; ii < ilen; ii++) {
        if ((paxis->format[ii] == _T('f')) || (paxis->format[ii] == _T('F'))) {
            iFmt = 0;
            break;
        }
        if ((paxis->format[ii] == _T('e')) || (paxis->format[ii] == _T('E'))) {
            iFmt = 1;
            break;
        }
    }
    pComboFormat->SetCurSel(iFmt);

    m_bAxisLinear = (paxis->scale == 0) ?  TRUE : FALSE;
    m_bAxisLog = (paxis->scale == 1) ?  TRUE : FALSE;
    m_bAxisAbs = (paxis->op == 0x01) ?  TRUE : FALSE;

    VariableToControl();

    m_bInitialized = TRUE;

    m_bIsModified = FALSE;

    return TRUE;
}

// CPlotoptScale message handlers
BOOL CPlotoptScale::OnInitDialog()
{
    CComboBox *pComboFormat = NULL;

    CPropertyPage::OnInitDialog();

    CRect rcCombo;

    pComboFormat = (CComboBox*)GetDlgItem(IDC_AXIS_FORMAT);

    if (pComboFormat == NULL) {
        return FALSE;
    }

    pComboFormat->GetWindowRect(rcCombo);
    ScreenToClient(&rcCombo);
    rcCombo.bottom += 100;
    pComboFormat->MoveWindow(rcCombo);
    pComboFormat->AddString(_T("Decimal 1000"));
    pComboFormat->AddString(_T("Scientific 1E3"));

    m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    return TRUE;
}

void CPlotoptScale::OnAxisBottom()
{
    ControlToVariable();    // Copy data to variables

    if (m_bBottomAxis == TRUE) {
        m_fAxisMin = m_pPlot->axisbottom.min;
        m_fAxisMax = m_pPlot->axisbottom.max;
        ml_getformat((const char_t*)(m_pPlot->axisbottom.format), &m_nPrefix, &m_nSuffix);

        CComboBox* pComboFormat = (CComboBox*)GetDlgItem(IDC_AXIS_FORMAT);
        if (pComboFormat == NULL) {
            return;
        }
        int_t iFmt = -1, ilen = _tcslen((const char_t*)(m_pPlot->axisbottom.format));
        for (int ii = 0; ii < ilen; ii++) {
            if ((m_pPlot->axisbottom.format[ii] == _T('f')) || (m_pPlot->axisbottom.format[ii] == _T('F'))) {
                iFmt = 0;
                break;
            }
            if ((m_pPlot->axisbottom.format[ii] == _T('e')) || (m_pPlot->axisbottom.format[ii] == _T('E'))) {
                iFmt = 1;
                break;
            }
        }
        pComboFormat->SetCurSel(iFmt);

        m_bAxisLinear = (m_pPlot->axisbottom.scale == 0) ?  TRUE : FALSE;
        m_bAxisLog = (m_pPlot->axisbottom.scale == 1) ?  TRUE : FALSE;
        m_bAxisAbs = (m_pPlot->axisbottom.op == 0x01) ?  TRUE : FALSE;

        VariableToControl();
    }
}

void CPlotoptScale::OnAxisLeft()
{
    ControlToVariable();    // Copy data to variables

    if (m_bLeftAxis == TRUE) {
        m_fAxisMin = m_pPlot->axisleft.min;
        m_fAxisMax = m_pPlot->axisleft.max;
        ml_getformat((const char_t*)(m_pPlot->axisleft.format), &m_nPrefix, &m_nSuffix);

        CComboBox* pComboFormat = (CComboBox*)GetDlgItem(IDC_AXIS_FORMAT);
        if (pComboFormat == NULL) {
            return;
        }
        int_t iFmt = -1, ilen = _tcslen((const char_t*)(m_pPlot->axisleft.format));
        for (int ii = 0; ii < ilen; ii++) {
            if ((m_pPlot->axisleft.format[ii] == _T('f')) || (m_pPlot->axisleft.format[ii] == _T('F'))) {
                iFmt = 0;
                break;
            }
            if ((m_pPlot->axisleft.format[ii] == _T('e')) || (m_pPlot->axisleft.format[ii] == _T('E'))) {
                iFmt = 1;
                break;
            }
        }
        pComboFormat->SetCurSel(iFmt);

        m_bAxisLinear = (m_pPlot->axisleft.scale == 0) ?  TRUE : FALSE;
        m_bAxisLog = (m_pPlot->axisleft.scale == 1) ?  TRUE : FALSE;
        m_bAxisAbs = (m_pPlot->axisleft.op == 0x01) ?  TRUE : FALSE;

        VariableToControl();
    }
}

void CPlotoptScale::OnAxisTop()
{
    ControlToVariable();    // Copy data to variables

    if (m_bTopAxis == TRUE) {
        m_fAxisMin = m_pPlot->axistop.min;
        m_fAxisMax = m_pPlot->axistop.max;
        ml_getformat((const char_t*)(m_pPlot->axistop.format), &m_nPrefix, &m_nSuffix);

        CComboBox* pComboFormat = (CComboBox*)GetDlgItem(IDC_AXIS_FORMAT);
        if (pComboFormat == NULL) {
            return;
        }
        int_t iFmt = -1, ilen = _tcslen((const char_t*)(m_pPlot->axistop.format));
        for (int ii = 0; ii < ilen; ii++) {
            if ((m_pPlot->axistop.format[ii] == _T('f')) || (m_pPlot->axistop.format[ii] == _T('F'))) {
                iFmt = 0;
                break;
            }
            if ((m_pPlot->axistop.format[ii] == _T('e')) || (m_pPlot->axistop.format[ii] == _T('E'))) {
                iFmt = 1;
                break;
            }
        }
        pComboFormat->SetCurSel(iFmt);

        m_bAxisLinear = (m_pPlot->axistop.scale == 0) ?  TRUE : FALSE;
        m_bAxisLog = (m_pPlot->axistop.scale == 1) ?  TRUE : FALSE;
        m_bAxisAbs = (m_pPlot->axistop.op == 0x01) ?  TRUE : FALSE;

        VariableToControl();
    }
}

void CPlotoptScale::OnAxisRight()
{
    ControlToVariable();    // Copy data to variables

    if (m_bRightAxis == TRUE) {
        m_fAxisMin = m_pPlot->axisright.min;
        m_fAxisMax = m_pPlot->axisright.max;
        ml_getformat((const char_t*)(m_pPlot->axisright.format), &m_nPrefix, &m_nSuffix);

        CComboBox* pComboFormat = (CComboBox*)GetDlgItem(IDC_AXIS_FORMAT);
        if (pComboFormat == NULL) {
            return;
        }
        int_t iFmt = -1, ilen = _tcslen((const char_t*)(m_pPlot->axisright.format));
        for (int ii = 0; ii < ilen; ii++) {
            if ((m_pPlot->axisright.format[ii] == _T('f')) || (m_pPlot->axisright.format[ii] == _T('F'))) {
                iFmt = 0;
                break;
            }
            if ((m_pPlot->axisright.format[ii] == _T('e')) || (m_pPlot->axisright.format[ii] == _T('E'))) {
                iFmt = 1;
                break;
            }
        }
        pComboFormat->SetCurSel(iFmt);

        m_bAxisLinear = (m_pPlot->axisright.scale == 0) ?  TRUE : FALSE;
        m_bAxisLog = (m_pPlot->axisright.scale == 1) ?  TRUE : FALSE;
        m_bAxisAbs = (m_pPlot->axisright.op == 0x01) ?  TRUE : FALSE;

        VariableToControl();
    }
}

BOOL CPlotoptScale::OnApply()
{
    if (m_bIsModified) {
        ControlToVariable();

        axis_t *paxis = NULL;

        if (m_bBottomAxis == TRUE) {
            paxis = &(m_pPlot->axisbottom);
        }
        else if (m_bLeftAxis == TRUE) {
            paxis = &(m_pPlot->axisleft);
        }
        else     if (m_bTopAxis == TRUE) {
            paxis = &(m_pPlot->axistop);
        }
        else if (m_bRightAxis == TRUE) {
            paxis = &(m_pPlot->axisright);
        }
        else {
            return FALSE;
        }

        // Processing
        if (m_fAxisMin < m_fAxisMax) {
            paxis->min = m_fAxisMin;
            paxis->max = m_fAxisMax;
        }
        if ((m_nPrefix < 0) || (m_nPrefix > 9)) {
            return FALSE;
        }
        if ((m_nSuffix < 0) || (m_nSuffix > 9)) {
            return FALSE;
        }

        int_t iFmt = 0;
        char_t szTmp[14], szFmt[16];
        szFmt[0] = _T('%');
        szFmt[1] = _T('\0');
        CComboBox* pComboFormat = (CComboBox*)GetDlgItem(IDC_AXIS_FORMAT);
        if (pComboFormat == NULL) {
            return FALSE;
        }
        iFmt = pComboFormat->GetCurSel();
        if ((iFmt == CB_ERR) || (iFmt < 0) || (iFmt > 2)) {
            return FALSE;
        }
        if ((m_nPrefix != 0) && (m_nSuffix != 0)) {
            _tcsprintf(szTmp, 14, _T("%01d.%01d"), m_nPrefix, m_nSuffix);
            _tcscat(szFmt, (const char_t*)szTmp);
        }
        else if ((m_nPrefix == 0) && (m_nSuffix != 0)) {
            _tcsprintf(szTmp, 14, _T(".%01d"), m_nSuffix);
            _tcscat(szFmt, (const char_t*)szTmp);
        }
        else if ((m_nPrefix != 0) && (m_nSuffix == 0)) {
            _tcsprintf(szTmp, 14, _T("%01d.0"), m_nPrefix, m_nSuffix);
            _tcscat(szFmt, (const char_t*)szTmp);
        }
        if (iFmt == 0) {
            _tcscat(szFmt, _T("f"));
        }
        else if (iFmt == 1) {
            _tcscat(szFmt, _T("e"));
        }
        else if (iFmt == 2) {
            _tcscat(szFmt, _T("g"));
        }
        _tcscpy(paxis->format, (const char_t*)szFmt);

        byte_t prevscale = paxis->scale;
        paxis->scale = (m_bAxisLog == TRUE) ?  1 : 0;
        if (paxis->scale == 1) { paxis->gridmcount = 9; }

        paxis->op = (m_bAxisAbs == TRUE) ?  0x01 : 0x00;

        if (m_bTemplate == FALSE) { ((CSigmaDoc*)m_pDoc)->SetModifiedFlag(TRUE); }

        // Update window plot
        if (m_bClosing == FALSE) {
            if (prevscale != paxis->scale) {
                m_pPlot->errcode = 0;
                pl_autoscale(m_pPlot);
            }
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

    return TRUE;
}

void CPlotoptScale::OnCbnSelchangeAxisformat()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnEnChangeAxismin()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnEnChangeAxismax()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnEnChangeAxisprefix()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnEnChangeAxissuffix()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnPlotDefault()
{
    OnApply();

    SigmaApp.m_pDefaultPlot->errcode = 0;
    pl_plot_dupstatic(SigmaApp.m_pDefaultPlot, m_pPlot);

    SigmaApp.setPlotSettings();
}

void CPlotoptScale::OnBnClickedAxisLinear()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnBnClickedAxisLog()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnBnClickedAxisAbs()
{
    if (m_bInitialized) {
        SetModified(TRUE);    m_bIsModified = TRUE;
    }
}

void CPlotoptScale::OnBnClickedScaleAuto()
{
    m_pPlot->errcode = 0;
    pl_autoscale(m_pPlot);

    ControlToVariable();    // Copy data to variables

    axis_t *paxis = NULL;

    if (m_bLeftAxis == TRUE) {
        paxis = &(m_pPlot->axisleft);
    }
    else if (m_bBottomAxis == TRUE) {
        paxis = &(m_pPlot->axisbottom);
    }
    else     if (m_bTopAxis == TRUE) {
        paxis = &(m_pPlot->axistop);
    }
    else if (m_bRightAxis == TRUE) {
        paxis = &(m_pPlot->axisright);
    }
    else {
        return;
    }

    m_fAxisMin = paxis->min;
    m_fAxisMax = paxis->max;
    VariableToControl();

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

void CPlotoptScale::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    if (m_bInitialized == FALSE) {
        Initialize();
    }
}

void CPlotoptScale::OnHelpContents()
{
    char_t szHelpFilename[ML_STRSIZE + ML_STRSIZES];
    _tcscpy(szHelpFilename, (LPCTSTR) (SigmaApp.m_szHelpFilename));
    _tcscat(szHelpFilename, _T("::/graph.html#graph_options_scale"));
    HWND hWnd = ::HtmlHelp(0, (LPCTSTR) szHelpFilename, HH_DISPLAY_TOPIC, NULL);
}

BOOL CPlotoptScale::PreTranslateMessage(MSG* pMsg)
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

void CPlotoptScale::OnDestroy()
{
    if (m_hAccel) {
        DestroyAcceleratorTable(m_hAccel);
        m_hAccel = NULL;
    }

    CWnd::OnDestroy();
}