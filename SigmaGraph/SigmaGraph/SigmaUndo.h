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

#pragma once

#include "../../LibGraph/LibFile/libfile.h"

typedef struct _dataset_t {
    sigmadoc_t* doc;
    plot_t* plot;
} docstate_t;

class CSigmaUndo
{

private:
    CList<docstate_t*, docstate_t*> m_Undo;            // Stores undo states
    CList<docstate_t*, docstate_t*> m_Redo;            // Stores redo states
    long m_iLevels;                                            // Requested Undolevels 

    void AddUndo(docstate_t *pState);
    void AddRedo(docstate_t *pState);
    void ClearList(CList<docstate_t*, docstate_t*> &listT);
    void DeleteState(docstate_t *pState);
    void UpdatePlot(docstate_t *pState, sigmadoc_t* pDoc, plot_t* pPlot);

public:

    // User accessable functions
    CSigmaUndo(long iLevels = 12);                                // Constructor
    ~CSigmaUndo();                                                        // Destructor
    void Reset();                                                        // Reset
    void SaveState(sigmadoc_t* pDoc, plot_t *pPlot);        // Save current state 
    BOOL CanUndo();                                                    // Returns TRUE if can Undo
    BOOL CanRedo();                                                    // Returns TRUE if can Redo
    void Undo(sigmadoc_t* pDoc, plot_t *pPlot);                // Restore next Undo state
    void Redo(sigmadoc_t* pDoc, plot_t *pPlot);                // Restore next Redo state
};

// Constructor
inline CSigmaUndo::CSigmaUndo(long iLevels) : m_iLevels(iLevels)
{

}

inline void CSigmaUndo::DeleteState(docstate_t *pState)
{
    if (pState == NULL) {
        return;
    }

    if (pState->doc != NULL) {
        if (pState->doc->iDataCount > 0) {
            for (int_t jj = 0; jj < pState->doc->iDataCount; jj++) {
                if (pState->doc->ColumnData[jj].dat) {
                    SigmaApp.m_pLib->errcode = 0;
                    ml_vector_delete(&(pState->doc->ColumnData[jj]), SigmaApp.m_pLib);
                }
            }
        }
        if (pState->doc->iDataFitCount > 0) {
            for (int_t jj = 0; jj < pState->doc->iDataFitCount; jj++) {
                if (pState->doc->ColumnDataFit[jj].dat) {
                    SigmaApp.m_pLib->errcode = 0;
                    ml_vector_delete(&(pState->doc->ColumnDataFit[jj]), SigmaApp.m_pLib);
                }
            }
        }
        delete pState->doc;
        pState->doc = NULL;
    }

    if (pState->plot != NULL) {
        delete pState->plot;
        pState->plot = NULL;
    }

    delete pState;
    pState = NULL;
}

inline void CSigmaUndo::ClearList(CList<docstate_t*, docstate_t*> &listT)
{
    // Clear list

    POSITION posT = listT.GetHeadPosition();
    docstate_t* pNextData = NULL;

    while (posT) {
        pNextData = (docstate_t*) listT.GetNext(posT);
        DeleteState(pNextData);
        pNextData = NULL;
    }

    listT.RemoveAll();
}

// Destructor
inline CSigmaUndo::~CSigmaUndo()
{
    Reset();
}

inline void CSigmaUndo::Reset()
{
    // Clear undo list
    ClearList(m_Undo);

    // Clear redo list
    ClearList(m_Redo);
}

// Checks undo availability, may be used to enable menus
inline BOOL CSigmaUndo::CanUndo()
{
    return (m_Undo.GetCount() > 1);
}

// Checks redo availability, may be used to enable menus
inline BOOL CSigmaUndo::CanRedo()
{
    return (m_Redo.GetCount() > 0);
}

// Adds state to the beginning of undo list        
inline void CSigmaUndo::AddUndo(docstate_t* pState)
{
    m_Undo.AddHead(pState);
}

// Place CMemFile instnace on Redo list
inline void CSigmaUndo::AddRedo(docstate_t* pState)
{
    // Move state to head of redo list
    m_Redo.AddHead(pState);
}

// Save current object state to Undo list
inline void CSigmaUndo::SaveState(sigmadoc_t* pDoc, plot_t* pPlot)
{
    // Remove old state if there are more than max allowed
    if (m_Undo.GetCount() > m_iLevels) {
        docstate_t* pState = (docstate_t*) (m_Undo.RemoveTail());
        DeleteState(pState);
        pState = NULL;
    }

    // Add new state to head of undo list
    docstate_t* pState = new docstate_t;
    memset(pState, 0, sizeof(docstate_t));
    pState->doc = new sigmadoc_t;
    memset(pState->doc, 0, sizeof(sigmadoc_t));
    pState->plot = new plot_t;
    memset(pState->plot, 0, sizeof(plot_t));

    SigmaApp.CopyDocument(pState->doc, pDoc);
    SigmaApp.CopyPlot(pState->plot, pPlot);

    // Copy also curves data
    pState->plot->curvecount = pPlot->curvecount;
    pState->plot->curveactive = pPlot->curveactive;
    for (int jj = 0; jj < ML_MAXCURVES; jj++) {
        memcpy(&(pState->plot->curve[jj]), &(pPlot->curve[jj]), sizeof(curve_t));

        // neutraliser les éléments dynamiques
        pState->plot->curve[jj].x = NULL;
        pState->plot->curve[jj].y = NULL;
        pState->plot->curve[jj].ex = NULL;
        pState->plot->curve[jj].ey = NULL;
    }

    // Titre
    pState->plot->otitle = pPlot->otitle;
    memcpy(&(pState->plot->title), &(pPlot->title), sizeof(text_t));

    // Axes
    memcpy(&(pState->plot->axisbottom), &(pPlot->axisbottom), sizeof(axis_t));
    memcpy(&(pState->plot->axisleft), &(pPlot->axisleft), sizeof(axis_t));
    memcpy(&(pState->plot->axistop), &(pPlot->axistop), sizeof(axis_t));
    memcpy(&(pState->plot->axisright), &(pPlot->axisright), sizeof(axis_t));
    memcpy(&(pState->plot->linktop), &(pPlot->linktop), ML_STRSIZE * sizeof(char_t));
    memcpy(&(pState->plot->linkright), &(pPlot->linkright), ML_STRSIZE * sizeof(char_t));

    // Lignes
    pState->plot->linevertcount = pPlot->linevertcount;
    pState->plot->linehorzcount = pPlot->linehorzcount;
    memcpy(&(pState->plot->linevert), &(pPlot->linevert), PL_MAXITEMS * sizeof(line_t));
    memcpy(&(pState->plot->linehorz), &(pPlot->linehorz), PL_MAXITEMS * sizeof(line_t));

    // Text
    pState->plot->textcount = pPlot->textcount;
    memcpy(&(pState->plot->text), &(pPlot->text), PL_MAXITEMS * sizeof(text_t));

    // Lignes GEO
    pState->plot->linecount = pPlot->linecount;
    memcpy(&(pState->plot->line), &(pPlot->line), PL_MAXITEMS * sizeof(line_t));

    // Rectangles GEO
    pState->plot->rectanglecount = pPlot->rectanglecount;
    memcpy(&(pState->plot->rectangle), &(pPlot->rectangle), PL_MAXITEMS * sizeof(rectangle_t));

    // Ellipses GEO
    pState->plot->ellipsecount = pPlot->ellipsecount;
    memcpy(&(pState->plot->ellipse), &(pPlot->ellipse), PL_MAXITEMS * sizeof(ellipse_t));

    AddUndo(pState);

    ClearList(m_Redo);
}

inline void CSigmaUndo::UpdatePlot(docstate_t *pState, sigmadoc_t* pDoc, plot_t* pPlot)
{
    // Copy also curves data
    if ((pState->plot->curvecount >= 0) && (pDoc->iDataCount > 0)) {

        // Curve removed
        if (pState->plot->curvecount == (pPlot->curvecount + 1)) {
            int iCurve = pState->plot->curvecount - 1;
            vector_t *pvecX = NULL, *pvecY = NULL;
            for (int ll = 0; ll < pDoc->iDataCount; ll++) {
                if (pState->plot->curve[iCurve].idx == pDoc->ColumnData[ll].id) {
                    pvecX = &(pDoc->ColumnData[ll]);
                    if (pvecY != NULL) {
                        break;
                    }
                }
                else if (pState->plot->curve[iCurve].idy == pDoc->ColumnData[ll].id) {
                    pvecY = &(pDoc->ColumnData[ll]);
                    if (pvecX != NULL) {
                        break;
                    }
                }
            }
            if ((pvecX == NULL) && (pvecY == NULL) && (pDoc->iDataFitCount > 0)) {
                for (int ll = 0; ll < pDoc->iDataFitCount; ll++) {
                    if (pState->plot->curve[iCurve].idx == pDoc->ColumnDataFit[ll].id) {
                        pvecX = &(pDoc->ColumnDataFit[ll]);
                        if (pvecY != NULL) {
                            break;
                        }
                    }
                    else if (pState->plot->curve[iCurve].idy == pDoc->ColumnDataFit[ll].id) {
                        pvecY = &(pDoc->ColumnDataFit[ll]);
                        if (pvecX != NULL) {
                            break;
                        }
                    }
                }
            }
            if ((pvecX != NULL) && (pvecY != NULL)) {
                pPlot->errcode = 0;
                pl_curve_add(pPlot, pvecX, pvecY, NULL, NULL, pState->plot->curve[iCurve].axisused, 0x01);
            }
        }
        //

        // Curve added
        else if (pState->plot->curvecount == (pPlot->curvecount - 1)) {
            int iCurve = pPlot->curvecount - 1;
            pPlot->errcode = 0;
            pl_curve_remove(pPlot, iCurve);
        }
        //
    }

    // Titre
    pPlot->otitle = pState->plot->otitle;
    memcpy(&(pPlot->title), &(pState->plot->title), sizeof(text_t));

    // Axes
    memcpy(&(pPlot->axisbottom), &(pState->plot->axisbottom), sizeof(axis_t));
    memcpy(&(pPlot->axisleft), &(pState->plot->axisleft), sizeof(axis_t));
    memcpy(&(pPlot->axistop), &(pState->plot->axistop), sizeof(axis_t));
    memcpy(&(pPlot->axisright), &(pState->plot->axisright), sizeof(axis_t));
    memcpy(&(pPlot->linktop), &(pState->plot->linktop), ML_STRSIZE * sizeof(char_t));
    memcpy(&(pPlot->linkright), &(pState->plot->linkright), ML_STRSIZE * sizeof(char_t));

    // Lignes
    pPlot->linevertcount = pState->plot->linevertcount;
    pPlot->linehorzcount = pState->plot->linehorzcount;
    memcpy(&(pPlot->linevert), &(pState->plot->linevert), PL_MAXITEMS * sizeof(line_t));
    memcpy(&(pPlot->linehorz), &(pState->plot->linehorz), PL_MAXITEMS * sizeof(line_t));

    // Text
    pPlot->textcount = pState->plot->textcount;
    memcpy(&(pPlot->text), &(pState->plot->text), PL_MAXITEMS * sizeof(text_t));

    // Lignes GEO
    pPlot->linecount = pState->plot->linecount;
    memcpy(&(pPlot->line), &(pState->plot->line), PL_MAXITEMS * sizeof(line_t));

    // Rectangles GEO
    pPlot->rectanglecount = pState->plot->rectanglecount;
    memcpy(&(pPlot->rectangle), &(pState->plot->rectangle), PL_MAXITEMS * sizeof(rectangle_t));

    // Ellipses GEO
    pPlot->ellipsecount = pState->plot->ellipsecount;
    memcpy(&(pPlot->ellipse), &(pState->plot->ellipse), PL_MAXITEMS * sizeof(ellipse_t));

}

// Perform an Undo command
inline void CSigmaUndo::Undo(sigmadoc_t* pDoc, plot_t* pPlot)
{
    if (CanUndo()) {
        // The head of the undo list is the current state. So move that to the Redo list and load then previous state.
        docstate_t *pState = (docstate_t*) m_Undo.GetHead();
        m_Undo.RemoveHead();
        AddRedo(pState);
        pState = (docstate_t*) m_Undo.GetHead();
        SigmaApp.CopyDocument(pDoc, pState->doc);
        SigmaApp.CopyPlot(pPlot, pState->plot);

        UpdatePlot(pState, pDoc, pPlot);
    }
}

//Perform a Redo Command
inline void CSigmaUndo::Redo(sigmadoc_t* pDoc, plot_t* pPlot)
{
    if (CanRedo()) {
        docstate_t *pState = (docstate_t*) m_Redo.GetHead();
        m_Redo.RemoveHead();
        AddUndo(pState);
        SigmaApp.CopyDocument(pDoc, pState->doc);
        SigmaApp.CopyPlot(pPlot, pState->plot);

        UpdatePlot(pState, pDoc, pPlot);
    }
}
