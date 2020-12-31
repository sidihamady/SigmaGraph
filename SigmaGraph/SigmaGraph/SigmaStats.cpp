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
#include "SigmaStats.h"

static int_t calcStats(const vector_t *pColumn, stat_t* pstats, libvar_t *pLib)
{
    int_t ii, ndat = pColumn->dim;
    real_t ftmp, fa, fb, fc;

    real_t *ptmp = NULL;

    if (pLib->errcode) { return NULL; }

    pLib->errcode = 1;

    if ((pColumn == NULL) || (pColumn->dat == NULL) || (ndat < 1) || (ndat > ML_MAXPOINTS)) {
        return 0;
    }

    if (ndat == 1) {
        if ((pColumn->mask[0] & 0x0F) != 0x00) {
            return 0;
        }

        pstats->points = 1;
        pstats->min = pColumn->dat[0];
        pstats->max = pColumn->dat[0];
        pstats->sum = pColumn->dat[0];
        pstats->mean = pColumn->dat[0];
        pstats->median = pColumn->dat[0];
        pstats->rms = 0.0;
        pstats->var = 0.0;
        pstats->dev = 0.0;
        pstats->coeff = 0.0;

        pstats->done = 1;

        pLib->errcode = 0;
        return 1;
    }

    ptmp = (real_t*) malloc(ndat * sizeof(real_t));
    if (ptmp == NULL) {
        return NULL;
    }

    pstats->sum = 0.0;
    ftmp = 0.0;
    bool bFound = false;
    int_t ll = 0;
    for (ii = 0; ii < ndat; ii++) {
        if ((bFound == false) && ((pColumn->mask[ii] & 0x0F) == 0x00)) {
            pstats->min = pstats->max = pColumn->dat[ii];
            bFound = true;
        }
        if ((bFound == false) || ((pColumn->mask[ii] & 0x0F) != 0x00)) {
            continue;
        }

        ptmp[ll] = pColumn->dat[ii];

        if (ptmp[ll] > pstats->max) {
            pstats->max = ptmp[ll];
        }
        if (ptmp[ll] < pstats->min) {
            pstats->min = ptmp[ll];
        }

        pstats->sum += ptmp[ll];
        ftmp += (ptmp[ll] * ptmp[ll]);

        ll += 1;
    }

    if (ll < 1) {
        free(ptmp); ptmp = NULL;
        return 0;
    }

    pstats->mean = pstats->sum / (real_t) ll;
    pstats->rms = (real_t) sqrt(ftmp / (real_t) ll);

    // Ordonner les données si nécessaire
    if (pstats->sorted == 0) {
        pLib->errcode = 0;
        ml_sort(ptmp, ll, 1, pLib);
        if (pLib->errcode) {
            free(ptmp); ptmp = NULL;
            return NULL;
        }
    }

    ftmp = 0.5 * (real_t) ll;
    ii = ll / 2;
    if (ftmp == (real_t) ii) {
        pstats->median = 0.5 * (ptmp[ii - 1] + ptmp[ii]);
    }
    else {
        pstats->median = ptmp[ii];
    }

    fa = 0.0; fb = 0.0; fc = 0.0;
    for (ii = 0; ii < ll; ii++) {
        ftmp = (ptmp[ii] - pstats->mean) * (ptmp[ii] - pstats->mean);
        fa += ftmp;
        fb += ftmp * (ptmp[ii] - pstats->mean);
        fc += ftmp * ftmp;
    }
    pstats->var = fa / (real_t) (ll - 1);
    pstats->dev = (real_t) sqrt(pstats->var);
    if (pstats->mean != 0.0) {
        pstats->coeff = pstats->dev / pstats->mean;
    }
    else {
        pstats->coeff = 0.0;
    }

    if (fa != 0.0) {
        pstats->skew = ((real_t) sqrt((real_t) (ll))) * fb / ((real_t) pow(fa, 1.5));
        pstats->kurt = (((real_t) (ll)) * fc / (fa * fa)) - 3.0;
    }
    else {
        pstats->skew = 0.0;
        pstats->kurt = 0.0;
    }

    pLib->errcode = 0;
    free(ptmp); ptmp = NULL;

    return ll;
}

// OLE stuff for clipboard operations
#include <afxadv.h>                        // For CSharedFile
#include <afxconv.h>                        // For LPTSTR -> LPSTR macros

// CSigmaStats dialog

IMPLEMENT_DYNAMIC(CSigmaStats, CDialog)

CSigmaStats::CSigmaStats(CWnd* pParent /*=NULL*/)
    : CDialog(CSigmaStats::IDD, pParent)
{
    m_pColumn = NULL;
    m_pStats = NULL;
    m_pbRunning = NULL;

    m_nPoints = 0;

    memset(m_szMin, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szMax, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szSum, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szMean, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szMedian, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szVar, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szDev, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szCoeff, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szRMS, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szSkew, 0, ML_STRSIZES * sizeof(char_t));
    memset(m_szKurt, 0, ML_STRSIZES * sizeof(char_t));
}

CSigmaStats::~CSigmaStats()
{

}

void CSigmaStats::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDE_STATS_POINTS, m_edtPoints);
    DDX_Control(pDX, IDE_STATS_MIN, m_edtMin);
    DDX_Control(pDX, IDE_STATS_MAX, m_edtMax);
    DDX_Control(pDX, IDE_STATS_SUM, m_edtSum);
    DDX_Control(pDX, IDE_STATS_MEAN, m_edtMean);
    DDX_Control(pDX, IDE_STATS_MEDIAN, m_edtMedian);
    DDX_Control(pDX, IDE_STATS_VAR, m_edtVar);
    DDX_Control(pDX, IDE_STATS_STDDEV, m_edtDev);
    DDX_Control(pDX, IDE_STATS_COEFF, m_edtCoeff);
    DDX_Control(pDX, IDE_STATS_RMS, m_edtRMS);
    DDX_Control(pDX, IDE_STATS_SKEW, m_edtSkew);
    DDX_Control(pDX, IDE_STATS_KURT, m_edtKurt);
    DDX_Text(pDX, IDE_STATS_POINTS, m_nPoints);
    DDX_Text(pDX, IDE_STATS_MIN, (CString)m_szMin);
    DDX_Text(pDX, IDE_STATS_MAX, (CString)m_szMax);
    DDX_Text(pDX, IDE_STATS_SUM, (CString)m_szSum);
    DDX_Text(pDX, IDE_STATS_MEAN, (CString)m_szMean);
    DDX_Text(pDX, IDE_STATS_MEDIAN, (CString)m_szMedian);
    DDX_Text(pDX, IDE_STATS_VAR, (CString)m_szVar);
    DDX_Text(pDX, IDE_STATS_STDDEV, (CString)m_szDev);
    DDX_Text(pDX, IDE_STATS_COEFF, (CString)m_szCoeff);
    DDX_Text(pDX, IDE_STATS_RMS, (CString)m_szRMS);
    DDX_Text(pDX, IDE_STATS_SKEW, (CString)m_szSkew);
    DDX_Text(pDX, IDE_STATS_KURT, (CString)m_szKurt);
}


BEGIN_MESSAGE_MAP(CSigmaStats, CDialog)
    ON_BN_CLICKED(IDCOPY, &CSigmaStats::OnCopy)
END_MESSAGE_MAP()


// CSigmaStats message handlers

BOOL CSigmaStats::OnInitDialog()
{
    CDialog::OnInitDialog();

    if ((m_pColumn == NULL) || (m_pStats == NULL)
        || (m_pbRunning == NULL)) {

        SigmaApp.Output(_T("Cannot show the statistics dialog: invalid column.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    if ((m_pColumn->dim < 1) || (m_pColumn->dim > ML_MAXPOINTS)) {
        SigmaApp.Output(_T("Cannot show the statistics dialog: invalid column dimension.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    SigmaApp.m_pLib->errcode = 0;
    m_pStats->points = m_pColumn->dim;
    int_t ll = calcStats((const vector_t*) (m_pColumn), m_pStats, SigmaApp.m_pLib);
    m_pStats->points = ll;
    m_pStats->done = 1;

    if (ll < 1) {
        SigmaApp.Output(_T("Cannot show the statistics dialog: all data masked.\r\n"), TRUE, TRUE, SIGMA_OUTPUT_ERROR);
        CDialog::EndDialog(-1);
        return FALSE;
    }

    this->GetDlgItem(IDL_STATS_COMMENT)->SetWindowText((ll != m_pColumn->dim) ? _T("Statitics calculated only for non-masked data.") : _T(""));

    m_nPoints = m_pStats->points;
    SigmaApp.Reformat(m_pStats->min, m_szMin, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->max, m_szMax, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->sum, m_szSum, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->mean, m_szMean, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->median, m_szMedian, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->var, m_szVar, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->dev, m_szDev, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->coeff, m_szCoeff, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->rms, m_szRMS, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->skew, m_szSkew, ML_STRSIZES - 1);
    SigmaApp.Reformat(m_pStats->kurt, m_szKurt, ML_STRSIZES - 1);

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CSigmaStats::OnCopy()
{
    USES_CONVERSION;

    // Write to shared file (REMEBER: CF_TEXT is ANSI, not UNICODE, so we need to convert)
    CSharedFile sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

    // Get a tab delimited string to copy to cache
    CString strT, strTT;

    strT.Format(_T("Points\tMin\tMax\tSum\tMean\tMedian\tVariance\tStdDev\tVarCoeff\tRMS\tSkewness\tKurtosis\r\n%d\t"), m_nPoints);
    strT.Append((LPCTSTR)m_szMin);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szMax);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szSum);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szMean);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szMedian);    strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szVar);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szDev);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szCoeff);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szRMS);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szSkew);        strT.Append(_T("\t"));
    strT.Append((LPCTSTR)m_szKurt);
    strT.Append(_T("\r\n"));

    sf.Write(T2A(strT.GetBuffer(1)), strT.GetLength());
    strT.ReleaseBuffer();

    char_t c = _T('\0');
    sf.Write(&c, 1);

    DWORD dwLen = (DWORD)(sf.GetLength());
    HGLOBAL hMem = sf.Detach();
    if (!hMem) {
        return;
    }

    hMem = ::GlobalReAlloc(hMem, dwLen, GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
    if (!hMem) {
        return;
    }

    // Cache data
    COleDataSource* pSource = new COleDataSource();
    if (pSource != NULL) {
        pSource->CacheGlobalData(CF_TEXT, hMem);
    }

    pSource->SetClipboard();
}
