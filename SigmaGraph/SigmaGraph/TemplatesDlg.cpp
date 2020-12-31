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
#include "TemplatesDlg.h"
#include "SigmaDoc.h"
#include "Plotopt.h"

// CTemplateDlg dialog

IMPLEMENT_DYNAMIC(CTemplateDlg, CDialog)

CTemplateDlg::CTemplateDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CTemplateDlg::IDD, pParent)
{
    m_ppPlot = NULL;
    m_pPlot = NULL;
    m_iPlot = -1;
    m_pColumnDataX = NULL;
    m_pColumnDataY = NULL;
    m_pColumnDataYi = NULL;
    m_hWndPlot = NULL;
    m_pDoc = NULL;
    m_iPlot = -1;

    memset(&m_Template, 0, sizeof(template_t));
    m_Template.Software.version = SIGMA_SOFTWARE_VERSION;
    _tcscpy(m_Template.Software.name, SIGMA_SOFTWARE_NAME);
    _tcscpy(m_Template.Software.description, SIGMA_SOFTWARE_DESCRIPTION);
    _tcscpy(m_Template.Software.copyright, SIGMA_SOFTWARE_COPYRIGHT);
    _tcscpy(m_Template.Software.date, SIGMA_SOFTWARE_DATERELEASE);
    _tcscpy(m_Template.Software.time, SIGMA_SOFTWARE_TIMERELEASE);
    _tcscpy(m_Template.Software.reserved, SIGMA_SOFTWARE_RESERVED);

    int_t jj, ll;

    m_pColumnDataX = (vector_t*)malloc(sizeof(vector_t));
    m_pColumnDataY = (vector_t*)malloc(sizeof(vector_t));
    m_pColumnDataYi = (vector_t*)malloc(sizeof(vector_t));
    if (m_pColumnDataX && m_pColumnDataY && m_pColumnDataYi) {
        memset(m_pColumnDataX, 0, sizeof(vector_t));
        memset(m_pColumnDataY, 0, sizeof(vector_t));
        memset(m_pColumnDataYi, 0, sizeof(vector_t));
        SigmaApp.m_pLib->errcode = 0;
        ml_vector_create(m_pColumnDataX, 11, 1, _T("A"), NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            free(m_pColumnDataX);
            m_pColumnDataX = NULL;
            free(m_pColumnDataY);
            m_pColumnDataY = NULL;
            free(m_pColumnDataYi);
            m_pColumnDataYi = NULL;
        }
        ml_vector_create(m_pColumnDataY, 11, 2, _T("B"), NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            ml_vector_delete(m_pColumnDataX, SigmaApp.m_pLib);
            free(m_pColumnDataX);
            m_pColumnDataX = NULL;
            free(m_pColumnDataY);
            m_pColumnDataY = NULL;
            free(m_pColumnDataYi);
            m_pColumnDataYi = NULL;
        }
        ml_vector_create(m_pColumnDataYi, 11, 3, _T("C"), NULL, SigmaApp.m_pLib);
        if (SigmaApp.m_pLib->errcode != 0) {
            ml_vector_delete(m_pColumnDataX, SigmaApp.m_pLib);
            ml_vector_delete(m_pColumnDataY, SigmaApp.m_pLib);
            free(m_pColumnDataX);
            m_pColumnDataX = NULL;
            free(m_pColumnDataY);
            m_pColumnDataY = NULL;
            free(m_pColumnDataYi);
            m_pColumnDataYi = NULL;
        }
        else {
            for (jj = 0; jj < 11; jj++) {
                m_pColumnDataX->dat[jj] = 0.5 * (real_t)jj;
                m_pColumnDataY->dat[jj] = m_pColumnDataX->dat[jj] * m_pColumnDataX->dat[jj];
                m_pColumnDataYi->dat[jj] = 20.0 * (1.0 - exp(-m_pColumnDataX->dat[jj]));
                m_pColumnDataX->mask[jj] = 0x00;
                m_pColumnDataY->mask[jj] = 0x00;
                m_pColumnDataYi->mask[jj] = 0x00;
            }
            m_pColumnDataY->idx = m_pColumnDataX->id;
            m_pColumnDataYi->idx = m_pColumnDataX->id;
            m_pColumnDataX->status = 0x00;
            m_pColumnDataY->status = 0x00;
            m_pColumnDataYi->status = 0x00;
            m_pColumnDataX->type = 0x10;
            m_pColumnDataY->type = 0x20;
            m_pColumnDataYi->type = 0x20;

            _tcscpy(m_pColumnDataX->label, _T("Data"));
            _tcscpy(m_pColumnDataY->label, _T("Data1"));
            _tcscpy(m_pColumnDataYi->label, _T("Data2"));
        }
    }

    if (m_pColumnDataX && m_pColumnDataY && m_pColumnDataYi) {
        m_ppPlot = (plot_t**)malloc(SIGMA_MAXTPL * sizeof(plot_t*));
        if (m_ppPlot) {
            memset(m_ppPlot, 0, SIGMA_MAXTPL * sizeof(plot_t*));
            for (jj = 0; jj < SIGMA_MAXTPL; jj++) {
                m_ppPlot[jj] = (plot_t*)malloc(SIGMA_MAXTPL * sizeof(plot_t));
                if (m_ppPlot[jj] == NULL) {
                    for (ll = 0; ll < jj; ll++) {
                        if (m_ppPlot[ll]) {
                            free(m_ppPlot[ll]);
                            m_ppPlot[ll] = NULL;
                        }
                    }
                    free(m_ppPlot);
                    m_ppPlot = NULL;
                    break;
                }
                memset(m_ppPlot[jj], 0, sizeof(plot_t));
                pl_reset(m_ppPlot[jj], 2);
                m_ppPlot[jj]->curve[0].legend.font.size = 8;
                m_ppPlot[jj]->curve[1].legend.font.size = 8;
                m_ppPlot[jj]->curve[0].linecolor.r = 255;
                m_ppPlot[jj]->curve[0].linecolor.g = 0;
                m_ppPlot[jj]->curve[0].linecolor.b = 0;
                m_ppPlot[jj]->curve[0].legend.font.color.r = 255;
                m_ppPlot[jj]->curve[0].legend.font.color.g = 0;
                m_ppPlot[jj]->curve[0].legend.font.color.b = 0;
                m_ppPlot[jj]->curve[1].linecolor.r = 0;
                m_ppPlot[jj]->curve[1].linecolor.g = 0;
                m_ppPlot[jj]->curve[1].linecolor.b = 255;
                m_ppPlot[jj]->curve[1].legend.font.color.r = 0;
                m_ppPlot[jj]->curve[1].legend.font.color.g = 0;
                m_ppPlot[jj]->curve[1].legend.font.color.b = 255;
                m_ppPlot[jj]->axisbottom.gridcount = 5;
                m_ppPlot[jj]->axistop.gridcount = 5;
                m_ppPlot[jj]->axisleft.gridcount = 5;
                m_ppPlot[jj]->axisright.gridcount = 5;
                m_ppPlot[jj]->axisbottom.min = 0;        m_ppPlot[jj]->axisbottom.max = 5;
                m_ppPlot[jj]->axistop.min = 0;            m_ppPlot[jj]->axistop.max = 5;
                m_ppPlot[jj]->axisleft.min = 0;            m_ppPlot[jj]->axisleft.max = 25;
                m_ppPlot[jj]->axisright.min = 0;            m_ppPlot[jj]->axisright.max = 25;
                _tcscpy(m_ppPlot[jj]->axisbottom.title.text, _T("X"));
                _tcscpy(m_ppPlot[jj]->axistop.title.text, _T("X2"));
                _tcscpy(m_ppPlot[jj]->axisleft.title.text, _T("Y"));
                _tcscpy(m_ppPlot[jj]->axisright.title.text, _T("Y2"));
            }
        }
    }
}

CTemplateDlg::~CTemplateDlg()
{
    if (m_pColumnDataX) {
        ml_vector_delete(m_pColumnDataX, SigmaApp.m_pLib);
        free(m_pColumnDataX);
        m_pColumnDataX = NULL;
    }
    if (m_pColumnDataY) {
        ml_vector_delete(m_pColumnDataY, SigmaApp.m_pLib);
        free(m_pColumnDataY);
        m_pColumnDataY = NULL;
    }
    if (m_pColumnDataYi) {
        ml_vector_delete(m_pColumnDataYi, SigmaApp.m_pLib);
        free(m_pColumnDataYi);
        m_pColumnDataYi = NULL;
    }

    int_t jj;
    if (m_ppPlot) {
        for (jj = 0; jj < SIGMA_MAXTPL; jj++) {
            if (m_ppPlot[jj]) {
                free(m_ppPlot[jj]);
                m_ppPlot[jj] = NULL;
            }
        }
        free(m_ppPlot);
        m_ppPlot = NULL;
    }
}

void CTemplateDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDL_TEMPLATE_LIST, m_lstTemplate);
    DDX_Control(pDX, ID_TEMPLATE_APPLY, m_bnApply);
    DDX_Control(pDX, ID_TEMPLATE_ADD, m_bnAdd);
    DDX_Control(pDX, ID_TEMPLATE_REMOVE, m_bnRemove);
    DDX_Control(pDX, ID_TEMPLATE_EDIT, m_bnEdit);
    DDX_Control(pDX, IDF_TEMPLATE_PLOT, m_PlotFrame);
    DDX_Check(pDX, IDK_TEMPLATE_DEFAULT, m_bDefault);
}


BEGIN_MESSAGE_MAP(CTemplateDlg, CDialog)
    ON_LBN_SELCHANGE(IDL_TEMPLATE_LIST, &CTemplateDlg::OnLbnSelchangeTemplate)
    ON_BN_CLICKED(ID_TEMPLATE_APPLY, &CTemplateDlg::OnTemplateApply)
    ON_BN_CLICKED(ID_TEMPLATE_ADD, &CTemplateDlg::OnTemplateAdd)
    ON_BN_CLICKED(ID_TEMPLATE_REMOVE, &CTemplateDlg::OnTemplateRemove)
    ON_BN_CLICKED(ID_TEMPLATE_EDIT, &CTemplateDlg::OnTemplateEdit)
    ON_LBN_DBLCLK(IDL_TEMPLATE_LIST, &CTemplateDlg::OnLbnDblclkTemplateList)
    ON_BN_CLICKED(IDOK, &CTemplateDlg::OnOK)
    ON_WM_PAINT()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CTemplateDlg::UpdateTemplateList()
{
    char_t szLoc[ML_STRSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*)(SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("Templates.par"));

    if (SigmaApp.OpenSigmaTemplate((LPCTSTR)szLoc, &m_Template, m_ppPlot) == FALSE) {
        char_t szLocA[ML_STRSIZE];
        memset(szLocA, 0, ML_STRSIZE * sizeof(char_t));
        _tcscpy(szLocA, (const char_t*)(SigmaApp.m_szMainDir));
        _tcscat(szLocA, _T("examples\\Templates.par"));
        if (SigmaApp.OpenSigmaTemplate((LPCTSTR)szLocA, &m_Template, m_ppPlot) == TRUE) {
            ::CopyFile((LPCTSTR)szLocA, (LPCTSTR)szLoc, FALSE);
        }
        else {
            m_Template.iCount = 1;
            _tcscpy(m_ppPlot[0]->title.text, _T("SigmaGraph Template"));
        }
    }

    int_t jj, idl;
    m_lstTemplate.ResetContent();
    for (jj = 0; jj < m_Template.iCount; jj++) {
        m_ppPlot[jj]->frame = m_hWndPlot;
        m_ppPlot[jj]->errcode = 0;
        pl_curve_add(m_ppPlot[jj], m_pColumnDataX, m_pColumnDataY, NULL, NULL, m_ppPlot[jj]->curve[0].axisused, 0x01);
        pl_curve_add(m_ppPlot[jj], m_pColumnDataX, m_pColumnDataYi, NULL, NULL, m_ppPlot[jj]->curve[1].axisused, 0x01);
        m_ppPlot[jj]->curvecount = 2;
        m_ppPlot[jj]->curveactive = 0;
        idl = m_lstTemplate.AddString((LPCTSTR)(m_ppPlot[jj]->title.text));
        m_lstTemplate.SetItemData(idl, jj + 1);
    }

    m_bnAdd.EnableWindow(m_Template.iCount < SIGMA_MAXTPL);
    m_bnRemove.EnableWindow(m_Template.iCount > 1);
}

// CTemplateDlg message handlers

BOOL CTemplateDlg::OnInitDialog()
{
    CDialog::OnInitDialog();


    if ((m_pPlot == NULL) || (m_pDoc == NULL)) {
        SigmaApp.Output(_T("Cannot show the 'Templates' dialog: invalid data.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if ((m_ppPlot == NULL) || (m_pColumnDataX == NULL) || (m_pColumnDataY == NULL) || (m_pColumnDataYi == NULL)) {
        SigmaApp.Output(_T("Cannot show the 'Templates' dialog: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    UpdateTemplateList();

    m_lstTemplate.SetCurSel(0);
    m_iPlot = 0;
    m_PlotFrame.m_pPlot = m_ppPlot[m_iPlot];

    m_bnAdd.EnableWindow(m_Template.iCount < SIGMA_MAXTPL);
    m_bnRemove.EnableWindow(m_Template.iCount > 1);

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplateDlg::OnTemplateAdd()
{
    if (m_Template.iCount >= SIGMA_MAXTPL) {
        SigmaApp.Output(_T("Cannot add template: limit (16) reached.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    UpdateData(TRUE);

    m_ppPlot[m_Template.iCount]->errcode = 0;
    pl_plot_dupstyle(m_ppPlot[m_Template.iCount], (const plot_t*)(SigmaApp.m_pDefaultPlot));

    m_hWndPlot = m_PlotFrame.GetSafeHwnd();
    m_ppPlot[m_Template.iCount]->frame = m_hWndPlot;
    m_ppPlot[m_Template.iCount]->errcode = 0;
    pl_curve_add(m_ppPlot[m_Template.iCount], m_pColumnDataX, m_pColumnDataY, NULL, NULL, m_ppPlot[m_Template.iCount]->curve[0].axisused, 0x01);
    pl_curve_add(m_ppPlot[m_Template.iCount], m_pColumnDataX, m_pColumnDataYi, NULL, NULL, m_ppPlot[m_Template.iCount]->curve[1].axisused, 0x01);
    m_ppPlot[m_Template.iCount]->curvecount = 2;
    m_ppPlot[m_Template.iCount]->curveactive = 0;
    _tcsprintf(m_ppPlot[m_Template.iCount]->title.text, ML_STRSIZE, _T("SigmaGraph Template %d"), m_Template.iCount + 1);
    
    m_ppPlot[m_Template.iCount]->curve[0].legend.font.size = 8;
    m_ppPlot[m_Template.iCount]->curve[1].legend.font.size = 8;
    m_ppPlot[m_Template.iCount]->curve[0].linecolor.r = 255;
    m_ppPlot[m_Template.iCount]->curve[0].linecolor.g = 0;
    m_ppPlot[m_Template.iCount]->curve[0].linecolor.b = 0;
    m_ppPlot[m_Template.iCount]->curve[0].legend.font.color.r = 255;
    m_ppPlot[m_Template.iCount]->curve[0].legend.font.color.g = 0;
    m_ppPlot[m_Template.iCount]->curve[0].legend.font.color.b = 0;
    m_ppPlot[m_Template.iCount]->curve[1].linecolor.r = 0;
    m_ppPlot[m_Template.iCount]->curve[1].linecolor.g = 0;
    m_ppPlot[m_Template.iCount]->curve[1].linecolor.b = 255;
    m_ppPlot[m_Template.iCount]->curve[1].legend.font.color.r = 0;
    m_ppPlot[m_Template.iCount]->curve[1].legend.font.color.g = 0;
    m_ppPlot[m_Template.iCount]->curve[1].legend.font.color.b = 255;
    m_ppPlot[m_Template.iCount]->axisbottom.gridcount = 5;
    m_ppPlot[m_Template.iCount]->axistop.gridcount = 5;
    m_ppPlot[m_Template.iCount]->axisleft.gridcount = 5;
    m_ppPlot[m_Template.iCount]->axisright.gridcount = 5;
    m_ppPlot[m_Template.iCount]->axisbottom.min = 0;        m_ppPlot[m_Template.iCount]->axisbottom.max = 5;
    m_ppPlot[m_Template.iCount]->axistop.min = 0;            m_ppPlot[m_Template.iCount]->axistop.max = 5;
    m_ppPlot[m_Template.iCount]->axisleft.min = 0;            m_ppPlot[m_Template.iCount]->axisleft.max = 25;
    m_ppPlot[m_Template.iCount]->axisright.min = 0;            m_ppPlot[m_Template.iCount]->axisright.max = 25;
    _tcscpy(m_ppPlot[m_Template.iCount]->axisbottom.title.text, _T("X"));
    _tcscpy(m_ppPlot[m_Template.iCount]->axistop.title.text, _T("X2"));
    _tcscpy(m_ppPlot[m_Template.iCount]->axisleft.title.text, _T("Y"));
    _tcscpy(m_ppPlot[m_Template.iCount]->axisright.title.text, _T("Y2"));

    int_t idl = m_lstTemplate.AddString((LPCTSTR)(m_ppPlot[m_Template.iCount]->title.text));
    m_lstTemplate.SetItemData(idl, m_Template.iCount + 1);
    m_Template.iCount += 1;

    m_bnAdd.EnableWindow(m_Template.iCount < SIGMA_MAXTPL);
    m_bnRemove.EnableWindow(m_Template.iCount > 1);

    UpdateData(FALSE);
}

void CTemplateDlg::OnTemplateRemove()
{
    if (m_Template.iCount <= 1) {
        return;
    }

    UpdateData(TRUE);

    int_t idl = m_lstTemplate.GetCurSel();
    int_t jj = (int_t) (m_lstTemplate.GetItemData(idl)), ll;

    if ((jj < 1) || (jj > m_Template.iCount)) {
        return;        
    }
    jj -= 1;

    if (jj != (m_Template.iCount - 1)) {
        // Update
        for (ll = jj; ll < (m_Template.iCount - 1); ll++) {
            pl_plot_dupstatic(m_ppPlot[ll], (const plot_t*)(m_ppPlot[ll + 1]));
        }
    }

    memset(m_ppPlot[m_Template.iCount - 1], 0, sizeof(plot_t));
    pl_reset(m_ppPlot[m_Template.iCount - 1], 2);
    m_ppPlot[m_Template.iCount - 1]->axisbottom.gridcount = 5;
    m_ppPlot[m_Template.iCount - 1]->axistop.gridcount = 5;
    m_ppPlot[m_Template.iCount - 1]->axisleft.gridcount = 5;
    m_ppPlot[m_Template.iCount - 1]->axisright.gridcount = 5;
    m_ppPlot[m_Template.iCount - 1]->axisbottom.min = 0;            m_ppPlot[m_Template.iCount - 1]->axisbottom.max = 5;
    m_ppPlot[m_Template.iCount - 1]->axistop.min = 0;                m_ppPlot[m_Template.iCount - 1]->axistop.max = 5;
    m_ppPlot[m_Template.iCount - 1]->axisleft.min = 0;                m_ppPlot[m_Template.iCount - 1]->axisleft.max = 25;
    m_ppPlot[m_Template.iCount - 1]->axisright.min = 0;            m_ppPlot[m_Template.iCount - 1]->axisright.max = 25;

    m_Template.iCount -= 1;

    m_lstTemplate.ResetContent();
    for (ll = 0; ll < m_Template.iCount; ll++) {
        m_ppPlot[ll]->frame = m_hWndPlot;
        m_ppPlot[ll]->errcode = 0;
        pl_curve_add(m_ppPlot[ll], m_pColumnDataX, m_pColumnDataY, NULL, NULL, m_ppPlot[ll]->curve[0].axisused, 0x01);
        pl_curve_add(m_ppPlot[ll], m_pColumnDataX, m_pColumnDataYi, NULL, NULL, m_ppPlot[ll]->curve[1].axisused, 0x01);
        m_ppPlot[ll]->curvecount = 2;
        m_ppPlot[ll]->curveactive = 0;

        idl = m_lstTemplate.AddString((LPCTSTR)(m_ppPlot[ll]->title.text));
        m_lstTemplate.SetItemData(idl, ll + 1);
    }
    if (jj >= m_Template.iCount) {
        jj = m_Template.iCount - 1;
    }
    m_lstTemplate.SetCurSel(jj);
    m_iPlot = jj;
    m_PlotFrame.m_pPlot = m_ppPlot[m_iPlot];

    m_bnAdd.EnableWindow(m_Template.iCount < SIGMA_MAXTPL);
    m_bnRemove.EnableWindow(m_Template.iCount > 1);

    UpdateData(FALSE);
}

void CTemplateDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    if ((m_iPlot < 0) || (m_iPlot >= m_Template.iCount)) {
        return;
    }

    int_t idl = m_lstTemplate.GetCurSel(), jj;
    int_t iPlot = (int_t) (m_lstTemplate.GetItemData(idl)) - 1;
    int_t idl0 = idl;
    if ((iPlot == m_iPlot) && (_tcscmp((const char_t*) (m_ppPlot[m_iPlot]->title.text), (const char_t*) m_szBuffer) != 0)) {
        m_lstTemplate.ResetContent();
        for (jj = 0; jj < m_Template.iCount; jj++) {
            idl = m_lstTemplate.AddString((LPCTSTR) (m_ppPlot[jj]->title.text));
            m_lstTemplate.SetItemData(idl, jj + 1);
        }
        m_lstTemplate.SetCurSel(idl0);
    }

    // Do not call CPropertyPage::OnPaint() for painting messages
}

void CTemplateDlg::OnTemplateEdit()
{
    if ((m_iPlot < 0) || (m_iPlot >= m_Template.iCount)) {
        return;
    }

    CPlotopt *plotOpt = new CPlotopt();
    if (plotOpt == NULL) {
        SigmaApp.Output(_T("Cannot show the 'Plot Options' dialog: insufficient memory.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        return;
    }

    memset(m_szBuffer, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(m_szBuffer, (const char_t*) (m_ppPlot[m_iPlot]->title.text));

    // Options initialization
    m_ppPlot[m_iPlot]->frame = GetSafeHwnd();
    plotOpt->m_pPlot = m_ppPlot[m_iPlot];

    plotOpt->m_bTemplate = TRUE;

    // 0. Curve
    plotOpt->m_Curve.m_pPlot = m_ppPlot[m_iPlot];            // Initializations done in m_Curve::OnInitDialog()
    plotOpt->m_Curve.m_nCurve = 0;
    plotOpt->m_Curve.m_pDoc = m_pDoc;

    // 1. Axis
    plotOpt->m_Axis.m_pPlot = m_ppPlot[m_iPlot];                // Initializations done in m_Axis::OnInitDialog()
    plotOpt->m_Axis.m_bBottomAxis = TRUE;
    plotOpt->m_Axis.m_bLeftAxis = FALSE;
    plotOpt->m_Axis.m_bTopAxis = FALSE;
    plotOpt->m_Axis.m_bRightAxis = FALSE;
    plotOpt->m_Axis.m_pDoc = m_pDoc;

    // 2. Line
    plotOpt->m_Line.m_pPlot = m_ppPlot[m_iPlot];                // Initializations done in m_Line::OnInitDialog()
    plotOpt->m_Line.m_bLineHorz = TRUE;
    plotOpt->m_Line.m_bLineVert = FALSE;
    plotOpt->m_Line.m_pDoc = m_pDoc;

    // 3. Scale
    plotOpt->m_Scale.m_pPlot = m_ppPlot[m_iPlot];            // Initializations done in m_Scale::OnInitDialog()
    plotOpt->m_Scale.m_pDoc = m_pDoc;
    plotOpt->m_Scale.m_bBottomAxis = plotOpt->m_Axis.m_bBottomAxis;
    plotOpt->m_Scale.m_bLeftAxis = plotOpt->m_Axis.m_bLeftAxis;
    plotOpt->m_Scale.m_bTopAxis = plotOpt->m_Axis.m_bTopAxis;
    plotOpt->m_Scale.m_bRightAxis = plotOpt->m_Axis.m_bRightAxis;

    // 4.    General
    plotOpt->m_General.m_pPlot = m_ppPlot[m_iPlot];        // Initializations done in m_General::OnInitDialog()
    plotOpt->m_General.m_pDoc = m_pDoc;

    plotOpt->m_Curve.m_bInitialized = FALSE;
    plotOpt->m_Axis.m_bInitialized = FALSE;
    plotOpt->m_Line.m_bInitialized = FALSE;
    plotOpt->m_Scale.m_bInitialized = FALSE;
    plotOpt->m_General.m_bInitialized = FALSE;

    plotOpt->m_Curve.m_bIsModified = FALSE;
    plotOpt->m_Axis.m_bIsModified = FALSE;
    plotOpt->m_Line.m_bIsModified = FALSE;
    plotOpt->m_Scale.m_bIsModified = FALSE;
    plotOpt->m_General.m_bIsModified = FALSE;

    plotOpt->SetActivePage(&(plotOpt->m_General));
    plotOpt->DoModal();

    m_ppPlot[m_iPlot]->axisbottom.gridcount = 5;
    m_ppPlot[m_iPlot]->axistop.gridcount = 5;
    m_ppPlot[m_iPlot]->axisleft.gridcount = 5;
    m_ppPlot[m_iPlot]->axisright.gridcount = 5;
    m_ppPlot[m_iPlot]->axisbottom.min = 0;            m_ppPlot[m_iPlot]->axisbottom.max = 5;
    m_ppPlot[m_iPlot]->axistop.min = 0;                m_ppPlot[m_iPlot]->axistop.max = 5;
    m_ppPlot[m_iPlot]->axisleft.min = 0;            m_ppPlot[m_iPlot]->axisleft.max = 25;
    m_ppPlot[m_iPlot]->axisright.min = 0;            m_ppPlot[m_iPlot]->axisright.max = 25;

    m_PlotFrame.Invalidate();

    delete plotOpt; plotOpt = NULL;

    int_t idl = m_lstTemplate.GetCurSel(), jj;
    int_t iPlot = (int_t) (m_lstTemplate.GetItemData(idl)) - 1;
    int_t idl0 = idl;
    if ((iPlot == m_iPlot) && (_tcscmp((const char_t*) (m_ppPlot[m_iPlot]->title.text), (const char_t*) m_szBuffer) != 0)) {
        m_lstTemplate.ResetContent();
        for (jj = 0; jj < m_Template.iCount; jj++) {
            idl = m_lstTemplate.AddString((LPCTSTR)(m_ppPlot[jj]->title.text));
            m_lstTemplate.SetItemData(idl, jj + 1);
        }
        m_lstTemplate.SetCurSel(idl0);
    }
}

void CTemplateDlg::OnTemplateApply()
{
    UpdateData(TRUE);

    int_t idl = m_lstTemplate.GetCurSel();
    m_iPlot = (int_t) (m_lstTemplate.GetItemData(idl)) - 1;
    if ((m_iPlot <  0) || (m_iPlot >= m_Template.iCount)) {
        return;
    }

    // >> Sauvergarder les éléments texte et géo pour les restaurer par la suite
    plot_t *pPlot = (plot_t*)malloc(sizeof(plot_t));
    if (pPlot == NULL) {
        return;
    }
    memset(pPlot, 0, sizeof(plot_t));
    pl_reset(pPlot, 2);
    pPlot->errcode = 0;
    pl_plot_dupstylei(pPlot, (const plot_t*)m_pPlot);
    // <<

    m_pPlot->errcode = 0;
    pl_plot_dupstyle(m_pPlot, (const plot_t*)(m_ppPlot[m_iPlot]));

    m_pPlot->errcode = 0;
    pl_plot_dupstylei(m_pPlot, (const plot_t*)pPlot);
    free(pPlot); pPlot = NULL;

    ((CSigmaDoc*) m_pDoc)->SetModifiedFlag(TRUE);

    ::SendMessage(m_pPlot->frame, WM_PAINT, 0, 0);

    ((CSigmaDoc*) m_pDoc)->SaveState();
}

void CTemplateDlg::OnLbnSelchangeTemplate()
{
    UpdateData(TRUE);

    int_t idl = m_lstTemplate.GetCurSel();
    m_iPlot = (int_t) (m_lstTemplate.GetItemData(idl)) - 1;

    if ((m_iPlot < 0) || (m_iPlot >= m_Template.iCount)) {
        m_iPlot = 0;
        m_lstTemplate.SetCurSel(0);
        UpdateData(FALSE);
    }

    m_PlotFrame.m_pPlot = m_ppPlot[m_iPlot];
    m_PlotFrame.Invalidate();

    UpdateData(FALSE);
}

void CTemplateDlg::OnOK()
{
    UpdateData(TRUE);

    char_t szLoc[ML_STRSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*)(SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("Templates.par"));

    SigmaApp.SaveSigmaTemplate((LPCTSTR)szLoc, &m_Template, (const plot_t**)m_ppPlot);

    CDialog::OnOK();
}

void CTemplateDlg::OnClose()
{

    UpdateData(TRUE);

    char_t szLoc[ML_STRSIZE];
    memset(szLoc, 0, ML_STRSIZE * sizeof(char_t));
    _tcscpy(szLoc, (const char_t*)(SigmaApp.m_szUserDir));
    _tcscat(szLoc, _T("Templates.par"));

    SigmaApp.SaveSigmaTemplate((LPCTSTR)szLoc, &m_Template, (const plot_t**)m_ppPlot);

    if ((m_bDefault) && (m_iPlot >= 0) && (m_iPlot < m_Template.iCount)) {
        SigmaApp.m_pDefaultPlot->errcode = 0;
        pl_plot_dupstatic(SigmaApp.m_pDefaultPlot, (const plot_t*)(m_ppPlot[m_iPlot]));
        SigmaApp.setPlotSettings();
    }

    CDialog::OnClose();
}

void CTemplateDlg::OnLbnDblclkTemplateList()
{
    UpdateData(TRUE);

    int_t idl = m_lstTemplate.GetCurSel();
    m_iPlot = (int_t) (m_lstTemplate.GetItemData(idl)) - 1;

    if ((m_iPlot < 0) || (m_iPlot >= m_Template.iCount)) {
        return;
    }

    m_PlotFrame.m_pPlot = m_ppPlot[m_iPlot];
    m_PlotFrame.Invalidate();

    UpdateData(FALSE);

    OnTemplateEdit();
}
