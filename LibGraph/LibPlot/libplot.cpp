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

#define _CRT_SECURE_NO_DEPRECATE 1

#include "../Target/targetver.h"

#include "libplot.h"

//    VARIABLES D'INTERACTION
const int_t pl_version = 0x26A0;                    /*    Version de libplot */
const real_t pl_logzero = -13.0;                    /*    Valeur prise pour log(x) en x~0 */
const real_t pl_loginf = +27.0;                     /*    Valeur prise pour log(x) pour x 'trés grand' */
const ulong_t pl_perfcount = 0;                     /* Compteur de perf. : durée (microsec.) */
#ifdef _DEBUG
const int_t pl_debug = 1;                           /* Variable de déboguage */
#else
const int_t pl_debug = 0;                           /* Variable de déboguage */
#endif

// 20160122: auto label tick improvement
// lmAxisLabel: class implementing the algorithm described in Justin Talbot et al., "An Extension of Wilkinson's Algorithm for Positioning Tick Labels on Axes", InfoVis 2010.
void lmAxisLabel::search(double *pfMin, double *pfMax, double *pfStep, int *piGrid)
{
    reset();

    m_fMin = *pfMin;
    m_fMax = *pfMax;
    m_fStep = *pfStep;

    double fscore = -2.0;
    double fsm, fdm, fcm, fdelta;
    const int itermax = 32;

    bool bStop = false;

    // Loop >
    int jj = 1;
    while ((bStop == false) && (jj < itermax)) {

        // Loop >>
        for (int ii = 0; (bStop == false) && (ii < m_iQ); ii++) {
            int rr = ii + 1;
            double fq = m_arrQ[ii];
            fsm = simplicitymax(rr, jj);
            if (weight(fsm, 1.0, 1.0, 1.0) < fscore) {
                bStop = true;
                break;
            }

            // Loop >>>
            int mm = 2;
            while ((bStop == false) && (mm < itermax)) {
                fdm = densitymax(mm, *piGrid);
                if (weight(fsm, 1.0, fdm, 1.0) < fscore) {
                    break;
                }
                fdelta = (*pfMax - *pfMin) / static_cast<double> (mm + 1) / (static_cast<double> (jj) * fq);

                // Loop >>>>
                int nn = static_cast<int> (ceil(log10(fdelta)));
                while ((bStop == false) && (nn < itermax)) {
                    double fstep = static_cast<double> (jj) * fq * pow(10.0, static_cast<double> (nn));
                    fcm = coveragemax(*pfMin, *pfMax, fstep * static_cast<double> (mm - 1));
                    if (weight(fsm, fcm, fdm, 1.0) < fscore) {
                        break;
                    }
                    int minstart = static_cast<int> (floor((*pfMax / fstep) - static_cast<double> (mm - 1))) * jj;
                    int maxstart = static_cast<int> (ceil(*pfMin / fstep)) * jj;

                    // Loop >>>>>
                    int uu = 0;
                    for (int istart = minstart; (bStop == false) && (istart <= maxstart) && (uu++ < itermax); istart++) {
                        double lmin = static_cast<double> (istart) * fstep / static_cast<double> (jj);
                        double lmax = lmin + fstep * static_cast<double> (mm - 1);
                        double tc = coverage(*pfMin, *pfMax, lmin, lmax);
                        double ts = simplicity(rr, jj, lmin, lmax, fstep);
                        double td = density(mm, *piGrid, *pfMin, *pfMax, lmin, lmax);
                        double tl = 1.0;
                        double tscore = weight(ts, tc, td, tl);
                        if ((tscore > fscore) && ((lmin <= *pfMin) && (lmax >= *pfMax))) {
                            m_fMin = lmin;
                            m_fMax = lmax;
                            m_fStep = fstep;
                            m_fScore = tscore;
                            fscore = tscore;
                        }
                    }

                    nn = nn + 1;
                }

                mm = mm + 1;
            }
        }

        if (bStop) {
            break;
        }

        jj = jj + 1;
    }

    *pfMin = m_fMin;
    *pfMax = m_fMax;
    *pfStep = m_fStep;
    *piGrid = static_cast<int> ((m_fMax - m_fMin) / m_fStep);
}