//////////////////////////////////////////////////////////////////////
// GridCtrl.cpp : implementation file
//
// MFC Grid Control v2.26
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright(C) 1998-2005. All Rights Reserved.
//
// The code contained in this file was based on the original
// WorldCom Grid control written by Joe Willcoxson,
//        mailto:chinajoe@aol.com
//        http://users.aol.com/chinajoe
// (These addresses may be out of date) The code has gone through 
// so many modifications that I'm not sure if there is even a single 
// original line of code. In any case Joe's code was a great 
// framework on which to build.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Modifications:
//   GridCtrl.cpp to Datasheet.cpp
//   Adapted to numerical data handling:
//      performance improvement for large dataset,
//      data masking, datasheet routines for plotting.
//   Copyright(C) 1998-2007 Sidi OULD SAAD HAMADY
//   http://www.hamady.org
//   sidi@hamady.org
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SigmaDC.h"
#include "InEdit.h"
#include "Datasheet.h"

#include <algorithm>

#include "Sigma.h"
#include "ColumnDlg.h"
#include "SeriesDlg.h"
#include "SigmaStats.h"
#include "MainFrm.h"
#include "FormulaDlg.h"
#include "AddCurveDlg.h"
#include "SigmaViewPlot.h"
#include "InputRealDlg.h"

// OLE stuff for clipboard operations
#include <afxadv.h>                        // For CSharedFile
#include <afxconv.h>                        // For LPTSTR -> LPSTR macros

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDatasheet, CWnd)

// Get the number of lines to scroll with each mouse wheel notch
UINT GetMouseScrollLines()
{
    int_t nScrollLines = 3;                    // reasonable default

    SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);

    return nScrollLines;
}

const COLORREF CDatasheet::DATASHEET_BGCOLOR = ::GetSysColor(COLOR_3DLIGHT);
const COLORREF CDatasheet::DATASHEET_GRIDCOLOR = RGB(180, 200, 220);

CDatasheet::CDatasheet(int_t nRows, int_t nCols, int_t nFixedRows, int_t nFixedCols)
{
    RegisterWindowClass();

    m_nGroup = 0;

    m_crWindowText = ::GetSysColor(COLOR_WINDOWTEXT);
    m_crWindowColour = ::GetSysColor(COLOR_WINDOW);
    m_cr3DFace = ::GetSysColor(COLOR_3DFACE);
    m_crShadow = ::GetSysColor(COLOR_3DSHADOW);

    m_nRows = 0;
    m_nCols = 0;
    m_nFixedRows = 0;
    m_nFixedCols = 0;
    m_InDestructor = false;

    m_nVScrollMax = 0;                                        // Scroll position
    m_nHScrollMax = 0;
    m_nRowsPerWheelNotch = GetMouseScrollLines();    // Get the number of lines per mouse wheel notch to scroll
    m_nBarState = DVL_NONE;
    m_MouseMode = MOUSE_NOTHING;
    m_nGridLines = DVL_BOTH;
    m_bEditable = TRUE;
    m_bSingleRowSelection = FALSE;
    m_bSingleColSelection = FALSE;
    m_bLMouseButtonDown = FALSE;
    m_bRMouseButtonDown = FALSE;
    m_bEnableSelection = TRUE;
    m_bFixedColumnSelection = TRUE;
    m_bFixedRowSelection = TRUE;
    m_bAllowRowResize = TRUE;
    m_bAllowColumnResize = TRUE;
    m_bHandleTabKey = TRUE;
    m_bDoubleBuffer = TRUE;                                    // Use double buffering to avoid flicker?

    m_bWysiwygPrinting = TRUE;                                // use tSize-to-width printing

    m_bHiddenColUnhide = TRUE;                                // 0-width columns can be expanded via mouse
    m_bHiddenRowUnhide = TRUE;                                // 0-Height rows can be expanded via mouse

    m_bAllowColHide = FALSE;                                // Columns can be contracted to 0-width via mouse
    m_bAllowRowHide = FALSE;                                // Rows can be contracted to 0-height via mouse

    m_nAutoSizeColumnStyle = DVS_BOTH;                      // Autosize grid using header and data info

    m_nResizeCaptureRange = 3;                                // When resizing columns/row, the cursor has to be within +/-3 pixels of the dividing line for resizing to be possible
    m_bTrackFocusCell = TRUE;                                // Track Focus tCell?
    m_bFrameFocus = TRUE;                                    // Frame the selected tCell?
    m_pRtcDefault = RUNTIME_CLASS(CCell);

    SetupDefaultCells();

    // Set up the initial grid tSize
    SetRowCount(nRows);
    SetColumnCount(nCols);
    SetFixedRowCount(nFixedRows);
    SetFixedColumnCount(nFixedCols);

    m_nSelectedColumn = -1;

    // Added to support user-definable margins.
    SetPrintMarginInfo(2, 2, 4, 4, 1, 1, 1);

    m_nMinWidth = 20;
    m_nMinHeight = 20;

    m_pInEdit = NULL;
}

CDatasheet::~CDatasheet()
{
    m_InDestructor = true;
    DeleteAllItems();

    DestroyWindow();

    // BUGFIX
    COleDataSource *pSource = COleDataSource::GetClipboardOwner();
    if (pSource) {
        COleDataSource::FlushClipboard();
    }
}

// Register the window class if it has not already been registered.
BOOL CDatasheet::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetResourceHandle();

    if (!(::GetClassInfo(hInst, DATASHEET_CLASS, &wndcls))) {
        // otherwise we need to register a new class
        wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc = ::DefWindowProc;
        wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
        wndcls.hInstance = hInst;
        wndcls.hIcon = NULL;
        wndcls.hCursor = SigmaApp.LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName = NULL;
        wndcls.lpszClassName = DATASHEET_CLASS;

        if (!AfxRegisterClass(&wndcls)) {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CDatasheet::Initialize()
{
    // Stop re-entry problems
    static BOOL bInProcedure = FALSE;

    if (bInProcedure) {
        return FALSE;
    }
    bInProcedure = TRUE;

    // Kludge: Make sure the client edge shows
    // This is so horrible it makes my eyes water.
    CRect rcT;
    GetWindowRect(rcT);
    CWnd* pParent = GetParent();
    if (pParent != NULL) {
        pParent->ScreenToClient(rcT);
    }
    rcT.InflateRect(1,1);    MoveWindow(rcT);
    rcT.DeflateRect(1,1);    MoveWindow(rcT);

    bInProcedure = FALSE;
    return TRUE;
}

// creates the control - use like any other window create control
BOOL CDatasheet::Create(const RECT& rcT, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
    ASSERT(pParentWnd->GetSafeHwnd());

    if (!CWnd::Create(DATASHEET_CLASS, NULL, dwStyle, rcT, pParentWnd, nID)) {
        return FALSE;
    }

    //Initialise(); - called in PreSubclassWnd

    // The number of rows and columns will only be non-zero if the constructor
    // was called with non-zero initialising parameters. If this window was created
    // using a dialog template then the number of rows and columns will be 0 (which
    // means that the code below will not be needed - which is lucky 'cause it ain't
    // gonna get called in a dialog-template-type-situation.

    TRY {
        m_arRowHeights.SetSize(m_nRows);                // initialize row heights
        m_arColWidths.SetSize(m_nCols);                // initialize column widths
    }
    CATCH (CMemoryException, excT) {
        excT->ReportError();
        return FALSE;
    }
    END_CATCH

    int_t ii;
    for (ii = 0; ii < m_nRows; ii++) {
        m_arRowHeights[ii] = m_cellDefault.GetHeight();
    }
    for (ii = 0; ii < m_nCols; ii++) {
        m_arColWidths[ii] = m_cellDefault.GetWidth();
    }

    return TRUE;
}

void CDatasheet::SetupDefaultCells()
{
    m_cellDefault.SetDatasheet(this);                    // Normal editable tCell
    m_cellFixedColDef.SetDatasheet(this);                // Cell for fixed columns
    m_cellFixedRowDef.SetDatasheet(this);                // Cell for fixed rows
    m_cellFixedRowColDef.SetDatasheet(this);            // Cell for area overlapped by fixed columns/rows

    m_cellDefault.SetTextClr(m_crWindowText);   
    m_cellDefault.SetBackClr(m_crWindowColour); 
    m_cellFixedColDef.SetTextClr(m_crWindowText);
    m_cellFixedColDef.SetBackClr(m_cr3DFace);
    m_cellFixedRowDef.SetTextClr(m_crWindowText);
    m_cellFixedRowDef.SetBackClr(m_cr3DFace);
    m_cellFixedRowColDef.SetTextClr(m_crWindowText);
    m_cellFixedRowColDef.SetBackClr(m_cr3DFace);
}

void CDatasheet::PreSubclassWindow()
{
    CWnd::PreSubclassWindow();

    Initialize();
}

char_t CDatasheet::GetColumnType(int_t nCol)
{
    char_t cType = _T('y');

    CView *pOwner = (CView*) GetOwner();
    if (pOwner == NULL) {
        return cType;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*) (pOwner->GetDocument());
    if (pDoc == NULL) {
        return cType;
    }
    vector_t *pColumn = pDoc->GetColumn(m_nGroup, nCol - 1);
    if (pColumn == NULL) {
        return cType;
    }
    if ((pColumn->type & 0xF0) == 0x10) {
        return _T('x');
    }
    else if ((pColumn->type & 0xF0) == 0x20) {
        return _T('y');
    }
    
    return cType;
}

BOOL CDatasheet::IsCellMasked(int_t nCol, int_t nRow)
{
    BOOL bMask = FALSE;

    if ((nRow < 1) || (nRow > m_nRows) || (nCol < 1) || (nCol > m_nCols)) {
        return bMask;
    }

    CView *pOwner = (CView*) GetOwner();
    if (pOwner == NULL) {
        return bMask;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*) (pOwner->GetDocument());
    if (pDoc == NULL) {
        return bMask;
    }
    vector_t *pColumn = pDoc->GetColumn(m_nGroup, nCol - 1);
    if (pColumn == NULL) {
        return bMask;
    }
    byte_t bT = pColumn->mask[nRow - 1];

    if ((bT & 0x0F) != 0x0F) {
        if ((bT & 0x0F) == 0x01) {    // Masked item
            return TRUE;
        }
    }

    return bMask;
}

char_t CDatasheet::GetColumnName(int_t nCol)
{
    return _T('a') + (nCol - 1);
}

inline char_t* CDatasheet::GetCellText(int_t nCol, int_t uiRow, char_t* pszCellText, byte_t *pbMask)
{
    real_t fItem = 0.0;

    if ((pszCellText == NULL) || (pbMask == NULL)) {
        return NULL;
    }

    _tcscpy(pszCellText, _T(""));
    *pbMask = 0xFF;

    CView *pOwner = (CView*)GetOwner();
    if (pOwner == NULL) {
        return pszCellText;
    }

    if (::IsWindow(pOwner->m_hWnd) == FALSE) {
        return pszCellText;
    }

    if ((nCol == 0) && (uiRow == 0)) {
        return pszCellText;
    }

    if (nCol == 0) {
        _tcsprintf(pszCellText, ML_STRSIZEN - 1, _T("%d"), uiRow);
        return pszCellText;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(pOwner->GetDocument());
    if (pDoc == NULL) {
        return pszCellText;
    }

    if (uiRow == 0) {
        char_t szName[ML_STRSIZET];
        pDoc->GetColumnName(m_nGroup, nCol - 1, szName, 0);
        char_t szBuffer[ML_STRSIZEN];
        if (pDoc->GetColumnNameOrLabel(m_nGroup, nCol - 1, szBuffer, 0)) {
            _tcscpy(pszCellText, (const char_t*) szBuffer);
        }
        return pszCellText;
    }

    *pbMask = pDoc->GetColumnItemMask(m_nGroup, nCol - 1, uiRow - 1, 0);

    if ((*pbMask & 0x0F) != 0x0F) {
        fItem = pDoc->GetColumnItem(m_nGroup, nCol - 1, uiRow - 1, 0);
        char_t szFormat[ML_STRSIZET];
        if (pDoc->GetColumnFormat(m_nGroup, nCol - 1, szFormat, 0)) {
            _tcsprintf(pszCellText, ML_STRSIZEN - 1, (const char_t*)szFormat, fItem);
            SigmaApp.m_pLib->errcode = 0;
            ml_reformat(pszCellText);
        }
    }

    return pszCellText;
}

BEGIN_MESSAGE_MAP(CDatasheet, CWnd)
    ON_WM_PAINT()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_GETDLGCODE()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_CHAR()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_WM_SYSKEYDOWN()
    ON_WM_SETCURSOR()
    ON_WM_RBUTTONUP()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_CAPTURECHANGED()
    ON_WM_DRAWITEM()
    ON_WM_MEASUREITEM()
    ON_WM_INITMENUPOPUP()
    ON_WM_MOUSEWHEEL()
    ON_MESSAGE(WM_SETFONT, &CDatasheet::OnSetFont)
    ON_MESSAGE(WM_GETFONT, &CDatasheet::OnGetFont)
    ON_MESSAGE(WM_IME_CHAR, &CDatasheet::OnImeChar)
    ON_COMMAND(ID_EDIT_SELECT_ALL, &CDatasheet::OnEditSelectAll)
    ON_COMMAND(ID_EDIT_COPY, &CDatasheet::OnEditCopy)
    ON_COMMAND(ID_EDIT_CLEAR, &CDatasheet::OnEditClear)
    ON_COMMAND(ID_EDIT_CUT, &CDatasheet::OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, &CDatasheet::OnEditPaste)
    ON_COMMAND(ID_EDIT_CELLCOPY, &CDatasheet::OnEditCopy)
    ON_COMMAND(ID_EDIT_CELLPASTE, &CDatasheet::OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, &CDatasheet::OnUpdateEditSelectAll)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CDatasheet::OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &CDatasheet::OnUpdateEditClear)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CDatasheet::OnUpdateEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CDatasheet::OnUpdateEditPaste)
    ON_EN_CHANGE(IDE_INEDIT, &CDatasheet::OnEnChangeNote)
    ON_UPDATE_COMMAND_UI(ID_DATA_IMPORT, &CDatasheet::OnUpdateDataImport)
    ON_UPDATE_COMMAND_UI(ID_DATA_IMPORTDLG, &CDatasheet::OnUpdateDataImportDlg)
    ON_UPDATE_COMMAND_UI(ID_PLOT_ADDCURVE, &CDatasheet::OnUpdatePlotAddCurve)
    ON_UPDATE_COMMAND_UI(ID_DATA_APPENDROW, &CDatasheet::OnUpdateAppendRow)
    ON_COMMAND(ID_DATA_INSERTCOL, &CDatasheet::OnInsertColumn)
    ON_UPDATE_COMMAND_UI(ID_DATA_INSERTCOL, &CDatasheet::OnUpdateInsertColumn)
    ON_COMMAND(ID_DATA_DELETECOL, &CDatasheet::OnDeleteColumn)
    ON_UPDATE_COMMAND_UI(ID_DATA_DELETECOL, &CDatasheet::OnUpdateDeleteColumn)
    ON_COMMAND(ID_DATA_APPENDCOL, &CDatasheet::OnAppendColumn)
    ON_UPDATE_COMMAND_UI(ID_DATA_APPENDCOL, &CDatasheet::OnUpdateAppendColumn)
    ON_COMMAND(ID_DATA_INSERTROW, &CDatasheet::OnInsertRow)
    ON_UPDATE_COMMAND_UI(ID_DATA_INSERTROW, &CDatasheet::OnUpdateInsertRow)
    ON_COMMAND(ID_DATA_DELETEROW, &CDatasheet::OnDeleteRow)
    ON_UPDATE_COMMAND_UI(ID_DATA_DELETEROW, &CDatasheet::OnUpdateDeleteRow)
    ON_COMMAND(ID_DATA_APPENDROW, &CDatasheet::OnAppendRow)
    ON_COMMAND(ID_DATA_AUTORESIZE, &CDatasheet::OnAutoResize)
    ON_COMMAND(ID_DATA_SETCOLVAL, &CDatasheet::OnSetColumnValues)
    ON_UPDATE_COMMAND_UI(ID_DATA_SETCOLVAL, &CDatasheet::OnUpdateSetColumnValues)
    ON_COMMAND(ID_DATA_CREATESERIES, &CDatasheet::OnCreateSeries)
    ON_UPDATE_COMMAND_UI(ID_DATA_CREATESERIES, &CDatasheet::OnUpdateCreateSeries)
    ON_COMMAND(ID_DATA_SORTASC, &CDatasheet::OnSortAsc)
    ON_UPDATE_COMMAND_UI(ID_DATA_SORTASC, &CDatasheet::OnUpdateSortAsc)
    ON_COMMAND(ID_DATA_SORTDESC, &CDatasheet::OnSortDesc)
    ON_UPDATE_COMMAND_UI(ID_DATA_SORTDESC, &CDatasheet::OnUpdateSortDesc)
    ON_COMMAND(ID_DATA_MASK, &CDatasheet::OnMask)
    ON_UPDATE_COMMAND_UI(ID_DATA_MASK, &CDatasheet::OnUpdateMask)
    ON_COMMAND(ID_DATA_UNMASK, &CDatasheet::OnUnmask)
    ON_UPDATE_COMMAND_UI(ID_DATA_UNMASK, &CDatasheet::OnUpdateUnmask)
    ON_COMMAND(ID_DATA_INVERTMASK, &CDatasheet::OnInvertmask)
    ON_UPDATE_COMMAND_UI(ID_DATA_INVERTMASK, &CDatasheet::OnUpdateInvertmask)
    ON_COMMAND(ID_DATA_STATS, &CDatasheet::OnStats)
    ON_UPDATE_COMMAND_UI(ID_DATA_STATS, &CDatasheet::OnUpdateStats)
    ON_COMMAND(ID_DATA_PROPERTIES, &CDatasheet::OnColumnProperties)
    ON_UPDATE_COMMAND_UI(ID_DATA_PROPERTIES, &CDatasheet::OnUpdateColumnProperties)
    ON_COMMAND(ID_PLOT_ADDCURVE, &CDatasheet::OnPlotAddCurve)
    ON_COMMAND(ID_EDIT_UNDO, &CDatasheet::OnEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CDatasheet::OnUpdateEditUndo)
    ON_COMMAND(ID_EDIT_REDO, &CDatasheet::OnEditRedo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CDatasheet::OnUpdateEditRedo)
    ON_COMMAND(ID_DATA_IMPORT, &CDatasheet::OnDataImport)
    ON_COMMAND(ID_DATA_IMPORTDLG, &CDatasheet::OnDataImportDlg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatasheet message handlers

void CDatasheet::OnPaint()
{
    CPaintDC dcT(this);                // device context for painting

    if (m_bDoubleBuffer) {            // Use a memory DC to remove flicker
        CSigmaDC sigmaDC(&dcT);
        OnDraw(&sigmaDC);
    }
    else {                                // Draw raw - this helps in debugging vis problems.
        OnDraw(&dcT);
    }
}

BOOL CDatasheet::OnEraseBkgnd(CDC* /*pDC*/)
{
    return TRUE;                        // Don't erase the background.
}

void CDatasheet::EraseBkgnd(CDC* pDC)
{
    CRect VisRect, ClipRect, rcT;
    CBrush FixedRowColBack(GetDefaultCell(TRUE, TRUE)->GetBackClr()),
        FixedRowBack(GetDefaultCell(TRUE, FALSE)->GetBackClr()),
        FixedColBack(GetDefaultCell(FALSE, TRUE)->GetBackClr()),
        TextBack(GetDefaultCell(FALSE, FALSE)->GetBackClr());
    CBrush Back(CDatasheet::DATASHEET_BGCOLOR);

    if (pDC->GetClipBox(ClipRect) == ERROR) {
        return;
    }
    GetVisibleNonFixedCellRange(VisRect);

    int_t nFixedColumnWidth = GetFixedColumnWidth();
    int_t nFixedRowHeight = GetFixedRowHeight();

    // Draw Fixed row/column background
    if (ClipRect.left < nFixedColumnWidth && ClipRect.top < nFixedRowHeight) {
        pDC->FillRect(CRect(ClipRect.left, ClipRect.top, nFixedColumnWidth, nFixedRowHeight), &FixedRowColBack);
    }

    // Draw Fixed columns background
    if (ClipRect.left < nFixedColumnWidth && ClipRect.top < VisRect.bottom) {
        pDC->FillRect(CRect(ClipRect.left, ClipRect.top, nFixedColumnWidth, VisRect.bottom), &FixedColBack);
    }

    // Draw Fixed rows background
    if (ClipRect.top < nFixedRowHeight && ClipRect.right > nFixedColumnWidth && ClipRect.left < VisRect.right) {
        pDC->FillRect(CRect(nFixedColumnWidth-1, ClipRect.top, VisRect.right, nFixedRowHeight), &FixedRowBack);
    }

    // Draw non-fixed tCell background
    if (rcT.IntersectRect(VisRect, ClipRect)) {
        CRect CellRect(__max(nFixedColumnWidth, rcT.left), __max(nFixedRowHeight, rcT.top), rcT.right, rcT.bottom);
        pDC->FillRect(CellRect, &TextBack);
    }

    // Draw right hand side of window outside grid
    if (VisRect.right < ClipRect.right) {
        pDC->FillRect(CRect(VisRect.right, ClipRect.top, ClipRect.right, ClipRect.bottom), &Back);
    }

    // Draw bottom of window below grid
    if (VisRect.bottom < ClipRect.bottom && ClipRect.left < VisRect.right) {
        pDC->FillRect(CRect(ClipRect.left, VisRect.bottom, VisRect.right, ClipRect.bottom), &Back);
    }
}

void CDatasheet::OnSize(UINT nType, int_t cx, int_t cy)
{  
    static BOOL bAlreadyInsideThisProcedure = FALSE;

    if (bAlreadyInsideThisProcedure) {
        return;
    }

    // Start re-entry blocking
    bAlreadyInsideThisProcedure = TRUE;

    EndEdit();                            // destroy any InEdit's
    CWnd::OnSize(nType, cx, cy);
    ResetScrollBars();

    // End re-entry blocking
    bAlreadyInsideThisProcedure = FALSE;
}

UINT CDatasheet::OnGetDlgCode()
{
    UINT nCode = DLGC_WANTARROWS | DLGC_WANTCHARS;        // DLGC_WANTALLKEYS;

    if (m_bHandleTabKey && !CTRL_Pressed()) {
        nCode |= DLGC_WANTTAB;
    }

    return nCode;
}

CCellID CDatasheet::GetNextCell(UINT nChar)
{
    CCellID tNextCell(-1,-1);

    if (!isValid(m_idCurrentCell)) {
        return tNextCell;
    }

    tNextCell = m_idCurrentCell;

    BOOL bChangeLine = FALSE;
    BOOL bHorzScrollAction = FALSE;
    BOOL bVertScrollAction = FALSE;

    BOOL bFoundVisible;
    int_t iOrig;

    if (nChar == VK_DOWN) {
        // don't let user go to a hidden row
        bFoundVisible = FALSE;
        iOrig = tNextCell.row;
        tNextCell.row++;
        while (tNextCell.row < GetRowCount()) {
            if (GetRowHeight(tNextCell.row) > 0) {
                bFoundVisible = TRUE;
                break;
            }
            tNextCell.row++;
        }
        if (!bFoundVisible) {
            tNextCell.row = iOrig;
        }
    }
    else if (nChar == VK_UP) {
        // don't let user go to a hidden row
        bFoundVisible = FALSE;
        iOrig = tNextCell.row;
        tNextCell.row--;
        while (tNextCell.row >= m_nFixedRows) {
            if (GetRowHeight(tNextCell.row) > 0) {
                bFoundVisible = TRUE;
                break;
            }
            tNextCell.row--;
        }
        if (!bFoundVisible) {
            tNextCell.row = iOrig;
        }
    }
    else if ((nChar == VK_RIGHT) || ((nChar == VK_TAB) && !SHIFT_Pressed())) {
        // don't let user go to a hidden column
        bFoundVisible = FALSE;
        iOrig = tNextCell.col;
        tNextCell.col++;

        if (nChar == VK_TAB) {
            // If we're at the end of a row, go down a row till we find a non-hidden row
            if (tNextCell.col == (GetColumnCount()) && tNextCell.row < (GetRowCount() - 1)) {
                tNextCell.row++;
                while (tNextCell.row < GetRowCount()) {
                    if (GetRowHeight(tNextCell.row) > 0) {
                        bFoundVisible = TRUE;
                        break;
                    }
                    tNextCell.row++;
                }

                tNextCell.col = m_nFixedCols;    // Place focus on first non-fixed column
                bChangeLine = TRUE;
            }
        }

        // We're on a non-hidden row, so look across for the tNextCell non-hidden column
        while (tNextCell.col < GetColumnCount()) {
            if (GetColumnWidth(tNextCell.col) > 0) {
                bFoundVisible = TRUE;
                break;
            }
            tNextCell.col++;
        }

        // If nothing worked then don't bother
        if (!bFoundVisible) {
            tNextCell.col = iOrig;
        }
    }
    else if ((nChar == VK_LEFT) || ((nChar == VK_TAB) && SHIFT_Pressed())) {
        // don't let user go to a hidden column
        bFoundVisible = FALSE;
        iOrig = tNextCell.col;
        tNextCell.col--;

        if (nChar == VK_TAB) {
            if ((tNextCell.col == (GetFixedColumnCount() - 1)) && (tNextCell.row > GetFixedRowCount())) {
                tNextCell.row--;
                while (tNextCell.row > GetFixedRowCount()) {
                    if (GetRowHeight(tNextCell.row) > 0) {
                        bFoundVisible = TRUE;
                        break;
                    }
                    tNextCell.row--;
                }

                tNextCell.col = GetColumnCount() - 1;
                bChangeLine = TRUE;
            }
        }

        while (tNextCell.col >= m_nFixedCols) {
            if (GetColumnWidth(tNextCell.col) > 0) {
                bFoundVisible = TRUE;
                break;
            }
            tNextCell.col--;
        }
        if (!bFoundVisible) {
            tNextCell.col = iOrig;
        }
    }
    else if (nChar == VK_NEXT) {
        CCellID idOldTopLeft = GetTopleftNonFixedCell();
        SendMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
        bVertScrollAction = TRUE;
        CCellID idNewTopLeft = GetTopleftNonFixedCell();

        int_t increment = idNewTopLeft.row - idOldTopLeft.row;
        if (increment) {
            tNextCell.row += increment;
            if (tNextCell.row >(GetRowCount() - 1))
                tNextCell.row = GetRowCount() - 1;
        }
        else {
            tNextCell.row = GetRowCount() - 1;
        }
    }
    else if (nChar == VK_PRIOR) {
        CCellID idOldTopLeft = GetTopleftNonFixedCell();
        SendMessage(WM_VSCROLL, SB_PAGEUP, 0);
        bVertScrollAction = TRUE;
        CCellID idNewTopLeft = GetTopleftNonFixedCell();

        int_t increment = idNewTopLeft.row - idOldTopLeft.row;
        if (increment) {
            tNextCell.row += increment;
            if (tNextCell.row < m_nFixedRows) {
                tNextCell.row = m_nFixedRows;
            }
        }
        else {
            tNextCell.row = m_nFixedRows;
        }
    }
    else if (nChar == VK_HOME) {
        // Home and Ctrl-Home work more like Excel
        //  and don't let user go to a hidden tCell
        if (CTRL_Pressed()) {
            SendMessage(WM_VSCROLL, SB_TOP, 0);
            SendMessage(WM_HSCROLL, SB_LEFT, 0);
            bVertScrollAction = TRUE;
            bHorzScrollAction = TRUE;
            tNextCell.row = m_nFixedRows;
            tNextCell.col = m_nFixedCols;
        }
        else {
            SendMessage(WM_HSCROLL, SB_LEFT, 0);
            bHorzScrollAction = TRUE;
            tNextCell.col = m_nFixedCols;
        }
        // adjust column to avoid hidden columns and rows
        while (tNextCell.col < GetColumnCount() - 1) {
            if (GetColumnWidth(tNextCell.col) > 0) {
                break;
            }
            tNextCell.col++;
        }
        while (tNextCell.row < GetRowCount() - 1) {
            if (GetRowHeight(tNextCell.row) > 0) {
                break;
            }
            tNextCell.row++;
        }
    }
    else if (nChar == VK_END) {
        // End and Ctrl-End work more like Excel
        //  and don't let user go to a hidden tCell
        if (CTRL_Pressed()) {
            SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
            SendMessage(WM_HSCROLL, SB_RIGHT, 0);
            bHorzScrollAction = TRUE;
            bVertScrollAction = TRUE;
            tNextCell.row = GetRowCount() - 1;
            tNextCell.col = GetColumnCount() - 1;
        }
        else {
            SendMessage(WM_HSCROLL, SB_RIGHT, 0);
            bHorzScrollAction = TRUE;
            tNextCell.col = GetColumnCount() - 1;
        }
        // adjust column to avoid hidden columns and rows
        while (tNextCell.col > (m_nFixedCols + 1)) {
            if (GetColumnWidth(tNextCell.col) > 0) {
                break;
            }
            tNextCell.col--;
        }
        while (tNextCell.row > (m_nFixedRows + 1)) {
            if (GetRowHeight(tNextCell.row) > 0) {
                break;
            }
            tNextCell.row--;
        }
    }

    return tNextCell;
}

BOOL CDatasheet::GotoToNextCell(UINT nChar)
{
    if (!isValid(m_idCurrentCell)) {
        return FALSE;
    }

    BOOL bChangeLine = FALSE;
    BOOL bHorzScrollAction = FALSE;
    BOOL bVertScrollAction = FALSE;

    CCellID tPrevCell = m_idCurrentCell;
    CCellID tNextCell = GetNextCell(nChar);

    if (tNextCell != m_idCurrentCell) {

        SetItemStateUnselected(m_idCurrentCell.row, m_idCurrentCell.col);
        SetItemStateSelected(tNextCell.row, tNextCell.col);
        SetFocusCell(tNextCell);

        if (!IsCellVisible(tNextCell)) {
            switch (nChar) {
                case VK_RIGHT:
                    SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
                    bHorzScrollAction = TRUE;
                    break;

                case VK_LEFT:
                    SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
                    bHorzScrollAction = TRUE;
                    break;

                case VK_DOWN:
                    SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
                    bVertScrollAction = TRUE;
                    break;

                case VK_UP:
                    SendMessage(WM_VSCROLL, SB_LINEUP, 0);
                    bVertScrollAction = TRUE;
                    break;

                case VK_TAB:
                    if (SHIFT_Pressed()) {
                        if (bChangeLine) {
                            SendMessage(WM_VSCROLL, SB_LINEUP, 0);
                            bVertScrollAction = TRUE;
                            SetScrollPos32(SB_HORZ, m_nHScrollMax);
                            break;
                        }
                        else {
                            SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
                            bHorzScrollAction = TRUE;
                        }
                    }
                    else {
                        if (bChangeLine) {
                            SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
                            bVertScrollAction = TRUE;
                            SetScrollPos32(SB_HORZ, 0);
                            break;
                        }
                        else {
                            SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
                            bHorzScrollAction = TRUE;
                        }
                    }
                    break;
            }
        }
        EnsureVisible(tNextCell); // Make sure tCell is visible
        Refresh();

        if (bHorzScrollAction) {
            SendMessage(WM_HSCROLL, SB_ENDSCROLL, 0);
        }
        if (bVertScrollAction) {
            SendMessage(WM_VSCROLL, SB_ENDSCROLL, 0);
        }
    }

    return TRUE;
}

// move about with keyboard
void CDatasheet::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

    if (nChar == VK_DELETE)
        AfxMessageBox(_T("It is a \"DELETE\" key!"));

    if (!isValid(m_idCurrentCell)) {
        return;
    }

    BOOL bChangeLine = FALSE;
    BOOL bHorzScrollAction = FALSE;
    BOOL bVertScrollAction = FALSE;

    if (CTRL_Pressed()) {
        switch (nChar) {
            case 'A':
                OnEditSelectAll();
                break;
            case 'k':        // This is ctrl+ on french keyboard, may need to be better processed for other locales
                AutoSizeColumns();
                Refresh();
                break;
            case 'X':
                OnEditCut();
                break;
            case VK_INSERT:
            case 'C':
                OnEditCopy();
                break;
            case 'V':
                OnEditPaste();
                break;
        }
    }

    if (SHIFT_Pressed() && (nChar == VK_INSERT)) { OnEditPaste(); }

    if (nChar == VK_DELETE) {
        CutSelectedText();
    }

    CCellID tNextCell = GetNextCell(nChar);

    if (tNextCell != m_idCurrentCell) {

        if (m_MouseMode == MOUSE_NOTHING) {
            try {
                m_PrevSelectedCellMap.clear();
            }
            catch (...) {

            }
            m_MouseMode = MOUSE_SELECT_CELLS;
            if (!SHIFT_Pressed() || (nChar == VK_TAB)) {
                m_SelectionStartCell = tNextCell;
            }

            OnSelecting(tNextCell);

            m_MouseMode = MOUSE_NOTHING;
        }

        SetFocusCell(tNextCell);

        if (!IsCellVisible(tNextCell)) {
            switch (nChar) {
                case VK_RIGHT:
                    SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
                    bHorzScrollAction = TRUE;
                    break;

                case VK_LEFT:
                    SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
                    bHorzScrollAction = TRUE;
                    break;

                case VK_DOWN:
                    SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
                    bVertScrollAction = TRUE;
                    break;

                case VK_UP:
                    SendMessage(WM_VSCROLL, SB_LINEUP, 0);
                    bVertScrollAction = TRUE;
                    break;

                case VK_TAB:
                    if (SHIFT_Pressed()) {
                        if (bChangeLine) {
                            SendMessage(WM_VSCROLL, SB_LINEUP, 0);
                            bVertScrollAction = TRUE;
                            SetScrollPos32(SB_HORZ, m_nHScrollMax);
                            break;
                        }
                        else {
                            SendMessage(WM_HSCROLL, SB_LINELEFT, 0);
                            bHorzScrollAction = TRUE;
                        }
                    }
                    else {
                        if (bChangeLine) {
                            SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
                            bVertScrollAction = TRUE;
                            SetScrollPos32(SB_HORZ, 0);
                            break;
                        }
                        else {
                            SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
                            bHorzScrollAction = TRUE;
                        }
                    }
                    break;
            }
            EnsureVisible(tNextCell); // Make sure tCell is visible
            Refresh();
        }
        EnsureVisible(tNextCell); // Make sure tCell is visible

        if (bHorzScrollAction) {
            SendMessage(WM_HSCROLL, SB_ENDSCROLL, 0);
        }
        if (bVertScrollAction) {
            SendMessage(WM_VSCROLL, SB_ENDSCROLL, 0);
        }
    }
    else {
        return;
    }

}

void CDatasheet::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CDatasheet::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CDatasheet::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST)) {
        if (pMsg->message == WM_KEYDOWN) {
            if ((pMsg->wParam == VK_DELETE) || (pMsg->wParam == VK_BACK)) {
                if (!IsEditing()) {
                    OnEditClear();
                    return TRUE;
                }
            }
        }
    }

    return CWnd::PreTranslateMessage(pMsg);
}

// Instant editing of cells when keys are pressed
void CDatasheet::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // BUGFIX
    if (!CTRL_Pressed() && (m_MouseMode == MOUSE_NOTHING) && (nChar != VK_ESCAPE)) {
        if (!m_bHandleTabKey || (m_bHandleTabKey && nChar != VK_TAB)) {
            OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint(-1, -1), nChar);
        }
    }

    CWnd::OnChar(nChar, nRepCnt, nFlags);
}

LRESULT CDatasheet::OnImeChar(WPARAM wCharCode, LPARAM)
{
    // BUGFIX
    if (!CTRL_Pressed() && (m_MouseMode == MOUSE_NOTHING) && (wCharCode != VK_ESCAPE)) {
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint(-1, -1), (UINT) wCharCode);
    }

    return 0;
}

BOOL CDatasheet::IsEditing(int_t nRow, int_t nCol)
{
    if (m_pInEdit == NULL) {
        return FALSE;
    }
    return ((CInEdit*)m_pInEdit)->IsEditing(nRow, nCol);
}

BOOL CDatasheet::IsEditing()
{
    return (m_pInEdit != NULL);
}

void CDatasheet::DestroyEdit(int_t nRow, int_t nCol, UINT uiLastChar, LPCTSTR pszNewText)
{
    m_pInEdit = NULL; // CInEdit destroys itself
    RedrawCell(nRow, nCol, NULL, FALSE);

    if (pszNewText == NULL) {
        return;
    }

    CView *pOwner = (CView*)GetOwner();
    if (pOwner == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(pOwner->GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pDoc->m_bIsModified = TRUE;

    CString strPrevText = GetItemText(nRow, nCol);
    if (*pszNewText != _T('\0')) {
        char_t szOutput[ML_STRSIZE], szFmt[ML_STRSIZET];
        _tcscpy(szOutput, pszNewText);
        real_t ftmp = 0.0;
        pDoc->SetColumnStatus(m_nGroup, nCol - 1, 0x00, 0);
        pDoc->GetColumnFormat(m_nGroup, nCol - 1, szFmt);
        SigmaApp.DoParse(pszNewText, szOutput, &ftmp, (const char_t*) szFmt);

        CString strCurrentText = (LPCTSTR) szOutput;
        if ((strCurrentText != strPrevText) && (strCurrentText.IsEmpty() == false)) {
            SetItemText(nRow, nCol, (LPCTSTR) szOutput, FALSE);
            SetModified(TRUE, TRUE);
        }
    }
    else {
        if (strPrevText.GetLength() > 0) {
            pDoc->SetColumnItem(m_nGroup, nCol - 1, nRow - 1, 0.0, 0);
            pDoc->SetColumnItemMask(m_nGroup, nCol - 1, nRow - 1, 0xFF, 0);
            SetModified(TRUE, TRUE);
        }
    }
    //

    switch (uiLastChar) {
        case VK_TAB:
        case VK_DOWN:
        case VK_UP:
        case VK_RIGHT:
        case VK_LEFT:
        case VK_NEXT:
        case VK_PRIOR:
        case VK_HOME:
        case VK_DELETE:
        case VK_END:
            if (GotoToNextCell(uiLastChar)) {
                OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, CPoint(-1, -1), uiLastChar);
            }
            break;
    }
}

void CDatasheet::OnEnChangeNote()
{
    CView *pOwner = (CView*)GetOwner();
    if (pOwner == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(pOwner->GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pDoc->m_bIsModified = TRUE;
}

// Handle horz scrollbar notifications
void CDatasheet::OnHScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    EndEdit();

    int_t scrollPos = GetScrollPos32(SB_HORZ);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rcT;
    GetClientRect(rcT);

    switch (nSBCode) {

        case SB_LINERIGHT:
            if (scrollPos < m_nHScrollMax) {
                // may have contiguous hidden columns.  Blow by them
                while ((idTopLeft.col < (GetColumnCount() - 1)) && (GetColumnWidth(idTopLeft.col) < 1)) {
                    idTopLeft.col++;
                }
                int_t xScroll = GetColumnWidth(idTopLeft.col);
                SetScrollPos32(SB_HORZ, scrollPos + xScroll);
                if (GetScrollPos32(SB_HORZ) == scrollPos) {
                    break;        // didn't work
                }
                rcT.left = GetFixedColumnWidth();
                InvalidateRect(rcT);
            }
            break;

        case SB_LINELEFT:
            if ((scrollPos > 0) && (idTopLeft.col > GetFixedColumnCount())) {
                int_t iColToUse = idTopLeft.col-1;
                // may have contiguous hidden columns.  Blow by them
                while(  iColToUse > GetFixedColumnCount() && GetColumnWidth( iColToUse) < 1) {
                    iColToUse--;
                }
                int_t xScroll = GetColumnWidth(iColToUse);
                SetScrollPos32(SB_HORZ, __max(0, scrollPos - xScroll));
                rcT.left = GetFixedColumnWidth();
                InvalidateRect(rcT);
            }
            break;

        case SB_PAGERIGHT:
            if (scrollPos < m_nHScrollMax) {
                rcT.left = GetFixedColumnWidth();
                int_t offset = rcT.Width();
                int_t pos = min(m_nHScrollMax, scrollPos + offset);
                SetScrollPos32(SB_HORZ, pos);
                rcT.left = GetFixedColumnWidth();
                InvalidateRect(rcT);
            }
            break;

        case SB_PAGELEFT:
            if (scrollPos > 0) {
                rcT.left = GetFixedColumnWidth();
                int_t offset = -rcT.Width();
                int_t pos = __max(0, scrollPos + offset);
                SetScrollPos32(SB_HORZ, pos);
                rcT.left = GetFixedColumnWidth();
                InvalidateRect(rcT);
            }
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            {
                SetScrollPos32(SB_HORZ, GetScrollPos32(SB_HORZ, TRUE));
                m_idTopLeftCell.row = -1;
                CCellID idNewTopLeft = GetTopleftNonFixedCell();
                if (idNewTopLeft != idTopLeft) {
                    rcT.left = GetFixedColumnWidth();
                    InvalidateRect(rcT);
                }
            }
            break;

        case SB_LEFT:
            if (scrollPos > 0) {
                SetScrollPos32(SB_HORZ, 0);
                Refresh();
            }
            break;

        case SB_RIGHT:
            if (scrollPos < m_nHScrollMax) {
                SetScrollPos32(SB_HORZ, m_nHScrollMax);
                Refresh();
            }
            break;

        default: 
            break;
    }
}

// Handle vert scrollbar notifications
void CDatasheet::OnVScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    EndEdit();

    // Get the scroll position ourselves to ensure we get a 32 bit value
    int_t scrollPos = GetScrollPos32(SB_VERT);

    CCellID idTopLeft = GetTopleftNonFixedCell();

    CRect rcT;
    GetClientRect(rcT);

    switch (nSBCode) {
        case SB_LINEDOWN:
            if (scrollPos < m_nVScrollMax) {
                // may have contiguous hidden rows.  Blow by them
                while (idTopLeft.row < (GetRowCount()-1) && GetRowHeight(idTopLeft.row) < 1) {
                    idTopLeft.row++;
                }
                int_t yScroll = GetRowHeight(idTopLeft.row);
                SetScrollPos32(SB_VERT, scrollPos + yScroll);
                if (GetScrollPos32(SB_VERT) == scrollPos) {
                    break;                // didn't work
                }
                rcT.top = GetFixedRowHeight();
                InvalidateRect(rcT);
            }
            break;

        case SB_LINEUP:
            if ((scrollPos > 0) && (idTopLeft.row > GetFixedRowCount())) {
                // may have contiguous hidden rows.  Blow by them
                while (idTopLeft.row > GetFixedRowCount() && GetRowHeight(idTopLeft.row) < 1) {
                    idTopLeft.row--;
                }
                int_t yScroll = GetRowHeight(idTopLeft.row);
                SetScrollPos32(SB_VERT, __max(0, scrollPos - yScroll));
                rcT.top = GetFixedRowHeight();
                InvalidateRect(rcT);
            }
            break;

        case SB_PAGEDOWN:
            if (scrollPos < m_nVScrollMax) {
                rcT.top = GetFixedRowHeight();
                scrollPos = min(m_nVScrollMax, scrollPos + rcT.Height());
                SetScrollPos32(SB_VERT, scrollPos);
                rcT.top = GetFixedRowHeight();
                InvalidateRect(rcT);
            }
            break;

        case SB_PAGEUP:
            if (scrollPos > 0) {
                rcT.top = GetFixedRowHeight();
                int_t offset = -rcT.Height();
                int_t pos = __max(0, scrollPos + offset);
                SetScrollPos32(SB_VERT, pos);
                rcT.top = GetFixedRowHeight();
                InvalidateRect(rcT);
            }
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            {
                SetScrollPos32(SB_VERT, GetScrollPos32(SB_VERT, TRUE));
                m_idTopLeftCell.row = -1;
                CCellID idNewTopLeft = GetTopleftNonFixedCell();
                if (idNewTopLeft != idTopLeft) {
                    rcT.top = GetFixedRowHeight();
                    InvalidateRect(rcT);
                }
            }
            break;

        case SB_TOP:
            if (scrollPos > 0) {
                SetScrollPos32(SB_VERT, 0);
                Refresh();
            }
            break;

        case SB_BOTTOM:
            if (scrollPos < m_nVScrollMax) {
                SetScrollPos32(SB_VERT, m_nVScrollMax);
                Refresh();
            }
            break;

        default: 
            break;
    }
}

void CDatasheet::OnDraw(CDC* pDC)
{
    CRect clipRect;
    if (pDC->GetClipBox(&clipRect) == ERROR) {
        return;
    }

    EraseBkgnd(pDC);                    // OnEraseBkgnd does nothing, so erase bkgnd here.
                                            // This is necessary since we may be using a Memory DC.

#ifdef _DEBUG
    LARGE_INTEGER iStartCount;
    QueryPerformanceCounter(&iStartCount);
#endif

    CRect rcT;
    int_t row, col;
    CCell* pCell = NULL;
    CCell xCell;
    CELL_DISPINFO xDispInfo;

    int_t nFixedRowHeight = GetFixedRowHeight();
    int_t nFixedColWidth = GetFixedColumnWidth();

    CCellID idTopLeft = GetTopleftNonFixedCell();
    int_t minVisibleRow = idTopLeft.row, minVisibleCol = idTopLeft.col;

    CRect VisRect;
    CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
    int_t maxVisibleRow = VisCellRange.GetMaxRow(), maxVisibleCol = VisCellRange.GetMaxCol();

    // draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
    rcT.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++) {
        if (GetRowHeight(row) <= 0) { continue; }
        rcT.top = rcT.bottom + 1;
        rcT.bottom = rcT.top + GetRowHeight(row) - 1;
        rcT.right = -1;
        for (col = 0; col < m_nFixedCols; col++) {
            if (GetColumnWidth(col) <= 0) { continue; }

            rcT.left = rcT.right + 1;
            rcT.right = rcT.left + GetColumnWidth(col) - 1;

            pCell = GetCell(row, col, &xCell, &xDispInfo);
            if (pCell) {
                pCell->Draw(pDC, row, col, rcT, IsEditing(row, col));
            }
        }
    }

    // draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
    rcT.bottom = nFixedRowHeight-1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++) {
        if (GetRowHeight(row) <= 0) { continue; }

        rcT.top = rcT.bottom + 1;
        rcT.bottom = rcT.top + GetRowHeight(row) - 1;

        // rcT.bottom = bottom pixel of previous row
        if (rcT.top > clipRect.bottom) { break; }                // Gone past cliprect
        if (rcT.bottom < clipRect.top) { continue; }            // Reached cliprect yet?

        rcT.right = -1;
        for (col = 0; col < m_nFixedCols; col++) {
            if (GetColumnWidth(col) <= 0) { continue; }

            rcT.left = rcT.right + 1;
            rcT.right = rcT.left + GetColumnWidth(col)-1;

            if (rcT.left > clipRect.right) { break; }            // gone past cliprect
            if (rcT.right < clipRect.left) { continue; }        // Reached cliprect yet?

            pCell = GetCell(row, col, &xCell, &xDispInfo);
            if (pCell) {
                pCell->Draw(pDC, row, col, rcT, IsEditing(row, col));
            }
        }
    }

    // draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
    rcT.bottom = -1;
    for (row = 0; row < m_nFixedRows; row++) {
        if (GetRowHeight(row) <= 0) { continue; }

        rcT.top = rcT.bottom+1;
        rcT.bottom = rcT.top + GetRowHeight(row) - 1;

        // rcT.bottom = bottom pixel of previous row
        if (rcT.top > clipRect.bottom) { break; }                // Gone past cliprect
        if (rcT.bottom < clipRect.top) { continue; }            // Reached cliprect yet?

        rcT.right = nFixedColWidth - 1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++) {
            if (GetColumnWidth(col) <= 0) continue;

            rcT.left = rcT.right;
            rcT.right = rcT.left + GetColumnWidth(col);

            if (rcT.left > clipRect.right) { break; }            // gone past cliprect
            if (rcT.right < clipRect.left) { continue; }        // Reached cliprect yet?

            pCell = GetCell(row, col, &xCell, &xDispInfo);
            if (pCell) {
                pCell->Draw(pDC, row, col, rcT, IsEditing(row, col));
            }
        }
    }

    // draw rest of non-fixed cells
    rcT.bottom = nFixedRowHeight - 1;
    for (row = minVisibleRow; row <= maxVisibleRow; row++) {
        if (GetRowHeight(row) <= 0) { continue; }

        rcT.top = rcT.bottom + 1;
        rcT.bottom = rcT.top + GetRowHeight(row) - 1;

        // rcT.bottom = bottom pixel of previous row
        if (rcT.top > clipRect.bottom) { break; }                // Gone past cliprect
        if (rcT.bottom < clipRect.top) { continue; }            // Reached cliprect yet?

        rcT.right = nFixedColWidth - 1;
        for (col = minVisibleCol; col <= maxVisibleCol; col++) {
            if (GetColumnWidth(col) <= 0) { continue; }

            rcT.left = rcT.right+1;
            rcT.right = rcT.left + GetColumnWidth(col) - 1;

            if (rcT.left > clipRect.right) { break; }            // gone past cliprect
            if (rcT.right < clipRect.left) { continue; }        // Reached cliprect yet?

            pCell = GetCell(row, col, &xCell, &xDispInfo);
            if (pCell) {
                pCell->Draw(pDC, row, col, rcT, IsEditing(row, col));
            }
        }
    }

    CPen penGrid, *penOld = NULL;
    penGrid.CreatePen(PS_SOLID, 1, DATASHEET_GRIDCOLOR);
    penOld = pDC->SelectObject(&penGrid);

    // draw vertical lines (drawn at ends of cells)
    if ((m_nGridLines == DVL_BOTH) || (m_nGridLines == DVL_VERT)) {
        int_t x = nFixedColWidth;
        for (col = minVisibleCol; col <= maxVisibleCol; col++) {
            if (GetColumnWidth(col) <= 0) { continue; }

            x += GetColumnWidth(col);
            pDC->MoveTo(x-1, nFixedRowHeight);
            pDC->LineTo(x-1, VisRect.bottom);
        }
    }

    // draw horizontal lines (drawn at bottom of each tCell)
    if ((m_nGridLines == DVL_BOTH) || (m_nGridLines == DVL_HORZ)) {
        int_t y = nFixedRowHeight;
        for (row = minVisibleRow; row <= maxVisibleRow; row++) {
            y += GetRowHeight(row);
            if (GetRowHeight(row) <= 0) { continue; }
            pDC->MoveTo(nFixedColWidth, y-1);
            pDC->LineTo(VisRect.right,  y-1);
        }
    }

    pDC->SelectObject(penOld);
    penGrid.DeleteObject();

#ifdef _DEBUG
    LARGE_INTEGER iEndCount;
    QueryPerformanceCounter(&iEndCount);
    TRACE1("Draw counter ticks: %d\n", iEndCount.LowPart - iStartCount.LowPart);
#endif
}

// Is a given tCell designation valid (ie within the bounds of our number
// of columns/rows)?
BOOL CDatasheet::isValid(int_t nRow, int_t nCol)
{
    return ((nRow >= 0) && (nRow < m_nRows) && (nCol >= 0) && (nCol < m_nCols));
}

BOOL CDatasheet::isValid(const CCellID& tCell)
{
    return isValid(tCell.row, tCell.col);
}

// Is a given tCell range valid (ie within the bounds of our number
// of columns/rows)?
BOOL CDatasheet::isValid(const CCellRange& range)
{
    return ((range.GetMinRow() >= 0) && (range.GetMinCol() >= 0) && (range.GetMaxRow() >= 0) && (range.GetMaxCol() >= 0) && (range.GetMaxRow() < m_nRows) && (range.GetMaxCol() < m_nCols) && (range.GetMinRow() <= range.GetMaxRow()) && (range.GetMinCol() <= range.GetMaxCol()));
}

// Forces a redraw of a tCell immediately (using a direct DC construction,
// or the supplied dc)
BOOL CDatasheet::RedrawCell(const CCellID& tCell, CDC* pDC /* = NULL */, BOOL bInEdit/* = FALSE*/)
{
    return RedrawCell(tCell.row, tCell.col, pDC, bInEdit);
}

BOOL CDatasheet::RedrawCell(int_t nRow, int_t nCol, CDC* pDC /* = NULL */, BOOL bInEdit/* = FALSE*/)
{
    BOOL bResult = TRUE;
    BOOL bMustReleaseDC = FALSE;

    if (!IsCellVisible(nRow, nCol)) {
        return FALSE;
    }

    CRect rcT, rcX;
    if (!GetCellRect(nRow, nCol, rcT)) {
        return FALSE;
    }
    rcX = rcT;
    if (pDC == NULL) {
        pDC = GetDC();
        if (pDC) {
            bMustReleaseDC = TRUE;
        }
    }

    if (pDC) {
        // Redraw cells directly
        if (nRow < m_nFixedRows || nCol < m_nFixedCols) {
            CCell xCell; CELL_DISPINFO xDispInfo;
            CCell* pCell = GetCell(nRow, nCol, &xCell, &xDispInfo);
            if (pCell) {
                bResult = pCell->Draw(pDC, nRow, nCol, rcT, bInEdit);
            }
        }
        else {
            CCell xCell;
            CELL_DISPINFO xDispInfo;
            CCell* pCell = GetCell(nRow, nCol, &xCell, &xDispInfo);
            if (pCell) {
                bResult = pCell->Draw(pDC, nRow, nCol, rcT, bInEdit);
            }

            // Since we have erased the background, we will need to redraw the gridlines

            if (bInEdit) {
                // If this fixed tCell is on the same row/col as the focus tCell,
                // highlight it.
                CPen penHeader(PS_SOLID, 1, ::GetSysColor(COLOR_HIGHLIGHT));
                CPen *pOldPen = pDC->SelectObject(&penHeader);
                pDC->MoveTo(rcX.right, rcX.top - 1);
                pDC->LineTo(rcX.right, rcX.bottom);
                pDC->LineTo(rcX.left - 1, rcX.bottom);
                pDC->LineTo(rcX.left - 1, rcX.top - 1);
                pDC->LineTo(rcX.right, rcX.top - 1);
                penHeader.DeleteObject();
                pDC->SelectObject(pOldPen);
                //
            }
            else {
                CPen penLine;
                penLine.CreatePen(PS_SOLID, 1, DATASHEET_GRIDCOLOR);

                CPen* pOldPen = (CPen*) pDC->SelectObject(&penLine);
                if (m_nGridLines == DVL_BOTH || m_nGridLines == DVL_HORZ) {
                    pDC->MoveTo(rcT.left, rcT.bottom);
                    pDC->LineTo(rcT.right + 1, rcT.bottom);
                }
                if (m_nGridLines == DVL_BOTH || m_nGridLines == DVL_VERT) {
                    pDC->MoveTo(rcT.right, rcT.top);
                    pDC->LineTo(rcT.right, rcT.bottom + 1);
                }
                pDC->SelectObject(pOldPen);
                penLine.DeleteObject();
            }
        }
    }
    else {
        InvalidateRect(rcT, TRUE);        // Could not get a DC - invalidate it anyway and hope that OnPaint manages to get one
    }

    if (bMustReleaseDC) {
        ReleaseDC(pDC); pDC = NULL;
    }

    return bResult;
}

// redraw a complete row
BOOL CDatasheet::RedrawRow(int_t row)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int_t col = 0; col < GetColumnCount(); col++) {
        bResult = RedrawCell(row, col, pDC, FALSE) && bResult;
    }
    if (pDC) {
        ReleaseDC(pDC); pDC = NULL;
    }

    return bResult;
}

// redraw a complete column
BOOL CDatasheet::RedrawColumn(int_t col)
{
    BOOL bResult = TRUE;

    CDC* pDC = GetDC();
    for (int_t row = 0; row < GetRowCount(); row++) {
        bResult = RedrawCell(row, col, pDC, FALSE) && bResult;
    }
    if (pDC) {
        ReleaseDC(pDC); pDC = NULL;
    }

    return bResult;
}

// Sets the currently selected tCell, returning the previous current tCell
CCellID CDatasheet::SetFocusCell(int_t nRow, int_t nCol)
{
    return SetFocusCell(CCellID(nRow, nCol));
}

CCellID CDatasheet::SetFocusCell(CCellID tCell)
{
    if (tCell == m_idCurrentCell) {
        return m_idCurrentCell;
    }

    CCellID idPrev = m_idCurrentCell;

    m_idCurrentCell = tCell;

    if (isValid(idPrev)) {
        SetItemState(idPrev.row, idPrev.col, GetItemState(idPrev.row, idPrev.col) & ~DVIS_FOCUSED);
    }

    if (isValid(m_idCurrentCell)) {
        SetItemState(m_idCurrentCell.row, m_idCurrentCell.col, GetItemState(m_idCurrentCell.row, m_idCurrentCell.col) | DVIS_FOCUSED);
    }

    return idPrev;
}

// Sets the range of currently selected cells
void CDatasheet::SetSelectedRange(const CCellRange& tRange, BOOL bSelectCells/*=TRUE*/, UINT nFlags/* = 0*/)
{
    SetSelectedRange(tRange.GetMinRow(), tRange.GetMinCol(), tRange.GetMaxRow(), tRange.GetMaxCol(), bSelectCells, nFlags);
}

void CDatasheet::SetSelectedRange(int_t nMinRow, int_t nMinCol, int_t nMaxRow, int_t nMaxCol, BOOL bSelectCells /* = TRUE*/, UINT nFlags /* = 0*/)
{
    if (!m_bEnableSelection) {
        return;
    }

    m_bAllCellsSelected = FALSE;

    // Bug fix - Don't allow selection of fixed rows
    int_t Left = GetFixedColumnCount();

    if ((nMinRow >= 0) && (nMinRow < GetFixedRowCount())) {
        nMinRow = GetFixedRowCount();
    }
    if ((nMaxRow >= 0) && (nMaxRow < GetFixedRowCount())) {
        nMaxRow = GetFixedRowCount();
    }
    if ((nMinCol >= 0) && (nMinCol < Left)) {
        nMinCol = GetFixedColumnCount();
    }
    if ((nMaxCol >= 0) && (nMaxCol < Left)) {
        nMaxCol = GetFixedColumnCount();
    }

    int jj;
    CCellID tCell;

    try {

        if (bSelectCells) {

            std::map<DWORD, CCellID>::iterator itr;

            // SLOW - Better speed since MFC CMap replacement by std::map and some tweeks (Sidi)

            long ii = 0;

            // Unselect all previously selected cells, if necessary
            long iMax = (GetRowCount() - 1) * (GetColumnCount() - 1);
            long iSize = m_SelectedCellMap.size();

            if (m_SelectedCellMap.empty() == false) {
                m_SelectedCellMap.clear();
            }

            // Add Previously selected cell if any
            if (m_PrevSelectedCellMap.empty() == false) {
                iSize = m_PrevSelectedCellMap.size();
                if (iSize > iMax) { iSize = iMax; }
                if (iSize >= 1) {
                    for (itr = m_PrevSelectedCellMap.begin(), ii = 0; (itr != m_PrevSelectedCellMap.end()) && (ii < iSize); ++itr, ii++) {
                        m_SelectedCellMap.insert(std::pair<DWORD, CCellID>(itr->first, itr->second));
                    }
                }
                m_PrevSelectedCellMap.clear();
            }
            //

            if (!GetSingleRowSelection() && (nMinRow >= 0) && (nMinCol >= 0) && (nMaxRow >= 0) && (nMaxCol >= 0)
                && (nMinRow <= nMaxRow) && (nMinCol <= nMaxCol)) {
                // >> SLOW - Better speed since MFC CMap replacement by std::map and some tweeks (Sidi)
                for (ii = nMinRow; ii <= nMaxRow; ii++) {
                    for (jj = nMinCol; jj <= nMaxCol; jj++) {
                        tCell = CCellID(ii, jj);
                        if (!isValid(tCell)) { continue; }
                        //int_t nState = GetItemState(tCell.row, tCell.col);
                        // Set state as Selected. This will add the tCell to m_SelectedCellMap
                        SetItemStateSelected(tCell.row, tCell.col);
                    }
                }
                // <<
            }
        }
        else { // (bSelectCells == FALSE)
            if (nFlags & MK_CONTROL) {
                // Deselect cells
                if (m_SelectedCellMap.empty() == false) {
                    if (!GetSingleRowSelection() && (nMinRow >= 0) && (nMinCol >= 0) && (nMaxRow >= 0) && (nMaxCol >= 0)
                        && (nMinRow <= nMaxRow) && (nMinCol <= nMaxCol)) {
                        int ii, jj;
                        // >> SLOW - Better speed since MFC CMap replacement by std::map and some tweeks
                        for (ii = nMinRow; ii <= nMaxRow; ii++) {
                            for (jj = nMinCol; jj <= nMaxCol; jj++) {
                                tCell = CCellID(ii, jj);
                                if (!isValid(tCell)) { continue; }
                                //int_t nState = GetItemState(tCell.row, tCell.col);
                                // Set state as Unselected. This will remove the tCell from m_SelectedCellMap
                                SetItemStateUnselected(tCell.row, tCell.col);
                            }
                        }
                        // <<
                    }
                }
            }
        }

    }
    catch (...) {

    }
}

// selects all cells
void CDatasheet::SelectAllCells()
{
    if (!m_bEnableSelection) { return; }

    CWaitCursor wcT;

    SetSelectedRange(m_nFixedRows, m_nFixedCols, GetRowCount() - 1, GetColumnCount() - 1);
    m_bAllCellsSelected = TRUE;
    SetFocusCell(0,0);
    Refresh();
}

// selects columns
void CDatasheet::SelectColumns(CCellID currentCell, BOOL bSelectCells /*=TRUE*/, UINT nFlags/* = 0*/)
{
    CWaitCursor wcT;

    if (!m_bEnableSelection) { return; }

    if (currentCell.col < m_nFixedCols) { return; }

    if (!isValid(currentCell)) { return; }

    if (GetSingleColSelection()) {
        SetSelectedRange(GetFixedRowCount(), currentCell.col, GetRowCount() - 1, currentCell.col, bSelectCells, nFlags);
    }
    else {
        SetSelectedRange(GetFixedRowCount(), min(m_SelectionStartCell.col, currentCell.col), GetRowCount() - 1, max(m_SelectionStartCell.col, currentCell.col), bSelectCells, nFlags);
    }
    SetFocusCell(currentCell);
    Refresh();
}

// selects rows
void CDatasheet::SelectRows(CCellID currentCell, BOOL bSelectCells /*=TRUE*/, UINT nFlags/* = 0*/)
{
    if (!m_bEnableSelection) {
        return;
    }

    if (currentCell.row < m_nFixedRows) {
        return;
    }

    if (!isValid(currentCell)) {
        return;
    }

    int_t Left = GetFixedColumnCount();
    if (GetSingleRowSelection()) {
        SetSelectedRange(currentCell.row, Left, currentCell.row, GetColumnCount() - 1, bSelectCells, nFlags);
    }
    else {
        SetSelectedRange(__min(m_SelectionStartCell.row, currentCell.row), Left, __max(m_SelectionStartCell.row, currentCell.row), GetColumnCount() - 1, bSelectCells, nFlags);
    }
    SetFocusCell(currentCell);
    Refresh();
}

// selects cells
void CDatasheet::SelectCells(CCellID currentCell, BOOL bSelectCells /*=TRUE*/, UINT nFlags/* = 0*/)
{
    if (!m_bEnableSelection) {
        return;
    }

    int_t row = currentCell.row;
    int_t col = currentCell.col;
    if ((row < m_nFixedRows) || (col < m_nFixedCols)) {
        return;
    }
    if (!isValid(currentCell)) {
        return;
    }

    SetSelectedRange(__min(m_SelectionStartCell.row, row), __min(m_SelectionStartCell.col, col), __max(m_SelectionStartCell.row, row), __max(m_SelectionStartCell.col, col), bSelectCells, nFlags);

    Refresh();
}

// Called when mouse/keyboard selection is a-happening.
void CDatasheet::OnSelecting(const CCellID& currentCell)
{
    if (!m_bEnableSelection) {
        return;
    }

    switch (m_MouseMode) {
        case MOUSE_SELECT_ALL:
            SelectAllCells();
            break;
        case MOUSE_SELECT_COL:
            SelectColumns(currentCell);
            break;
        case MOUSE_SELECT_ROW:
            SelectRows(currentCell);
            break;
        case MOUSE_SELECT_CELLS:
            SelectCells(currentCell);
            break;
    }
}

void CDatasheet::ValidateAndModifyCellContents(int_t nRow, int_t nCol, LPCTSTR szText)
{
    if (!IsCellEditable(nRow, nCol)) {
        return;
    }

    CString strCurrentText = GetItemText(nRow, nCol);
    if (strCurrentText != szText) {
        SetItemText(nRow, nCol, szText);
        SetModified(TRUE, FALSE);
        RedrawCell(nRow, nCol, NULL, IsEditing(nRow, nCol));
    }
}

// Deletes the contents from the selected cells
void CDatasheet::CutSelectedText()
{
    if (!IsEditable()) {
        return;
    }

    try {

        std::map<DWORD, CCellID>::iterator itr;
        DWORD key;
        CCellID tCell;

        long ii = 0;
        long iMax = (GetRowCount() - 1) * (GetColumnCount() - 1);
        long iSize = m_SelectedCellMap.size();
        if (iSize > iMax) { iSize = iMax; }
        if (iSize < 1) { return; }

        for (itr = m_SelectedCellMap.begin(), ii = 0; (itr != m_SelectedCellMap.end()) && (ii < iSize); ++itr, ii++) {
            key = itr->first;
            tCell = itr->second;
            ValidateAndModifyCellContents(tCell.row, tCell.col, _T(""));
        }

    }
    catch (...) {

    }
}

// Copies text from the selected cells to the clipboard
COleDataSource* CDatasheet::CopyTextFromDatasheet()
{
    byte_t bMask;
    CSigmaDoc* pDoc = (CSigmaDoc*) (GetDocument());
    if (pDoc == NULL) {
        return NULL;
    }

    USES_CONVERSION;

    CCellRange Selection = GetSelectedCellRange();
    if (!isValid(Selection)) {
        return NULL;
    }

    // Write to shared file (REMEBER: CF_TEXT is ANSI, not UNICODE, so we need to convert)
    CSharedFile sf(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

    // Get a tab delimited string to copy to cache
    CString str;
    CCell xCell; CELL_DISPINFO xDispInfo;
    CCell *pCell = NULL;
    for (int_t row = Selection.GetMinRow(); row <= Selection.GetMaxRow(); row++) {
        // don't copy hidden cells
        if (m_arRowHeights[row] <= 0 ) { continue; }

        str.Empty();
        for (int_t col = Selection.GetMinCol(); col <= Selection.GetMaxCol(); col++) {
            // don't copy hidden cells
            if (m_arColWidths[col] <= 0) { continue; }

            pCell = GetCell(row, col, &xCell, &xDispInfo);
            if (pCell && (pCell->GetState() & DVIS_SELECTED)) {
                bMask = pDoc->GetColumnItemMask(m_nGroup, col - 1, row - 1, 0);
                if ((bMask & 0x0F) == 0x0F) {
                    str += _T("-");
                }
                else {
                    str += pCell->GetText();
                }
            }
            if (col != Selection.GetMaxCol()) {
                str += _T("\t");
            }
        }

        if (row != Selection.GetMaxRow()) {
            str += _T("\r\n");
        }

        sf.Write(T2A(str.GetBuffer(1)), str.GetLength());
        str.ReleaseBuffer();
    }

    char_t c = _T('\0');
    sf.Write(&c, 1);

    DWORD dwLen = (DWORD)(sf.GetLength());
    HGLOBAL hMem = sf.Detach();
    if (!hMem) {
        return NULL;
    }

    hMem = ::GlobalReAlloc(hMem, dwLen, GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
    if (!hMem) {
        return NULL;
    }

    // Cache data
    COleDataSource* pSource = new COleDataSource();
    if (pSource != NULL) {
        pSource->CacheGlobalData(CF_TEXT, hMem);
    }

    return pSource;
}

// Pastes text from the clipboard to the selected cells
BOOL CDatasheet::PasteTextToDatasheet(CCellID tCell, COleDataObject* pDataObject, BOOL bSelectPastedCells /*=TRUE*/)
{
    if (!isValid(tCell) || !IsCellEditable(tCell) || !pDataObject->IsDataAvailable(CF_TEXT)) {
        return FALSE;
    }

    // Get the text from the COleDataObject
    HGLOBAL hmem = pDataObject->GetGlobalData(CF_TEXT);
    if (hmem == NULL) {
        return FALSE;
    }
    SIZE_T sizeT = ::GlobalSize(hmem);
    if ((sizeT < 1) || (sizeT > ML_MAXMAX)) {
        return FALSE;
    }

    CMemFile sf((BYTE*)::GlobalLock(hmem), (UINT)sizeT);

    // CF_TEXT is ANSI text, so we need to allocate a char* buffer to hold this.
    char* pszBuffer = new char[sizeT];
    if (pszBuffer == NULL) {
        return FALSE;
    }

    sf.Read(pszBuffer, (UINT)::GlobalSize(hmem));
    ::GlobalUnlock(hmem);

    char_t* pszBufferW = NULL;
    int_t nLen = 0;
    pszBufferW = new char_t[sizeT];
    if (pszBufferW == NULL) {
        delete pszBuffer;    pszBuffer = NULL;
        return FALSE;
    }
    nLen = MultiByteToWideChar(CP_ACP, 0, pszBuffer, -1, pszBufferW, sizeT);
    if ((nLen < 1) || (nLen > (int_t)sizeT)) {
        delete pszBuffer;        pszBuffer = NULL;
        delete pszBufferW;    pszBufferW = NULL;
        return FALSE;
    }
    pszBufferW[nLen - 1] = _T('\0');
    delete pszBuffer;        pszBuffer = NULL;

    int_t ii, jj, nRow = 0, nColumn = 0, iFound = 0, nlenT = 0;
    byte_t bMask;

    // Find the end of the first line
    CCellRange pasteRange(tCell.row, tCell.col, -1, -1);

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        delete pszBufferW;    pszBufferW = NULL;
        return FALSE;
    }

    int_t nRowMax = GetRowCount(), nColMax = GetColumnCount();
    if ((nRowMax < 1) || (nRowMax > (ML_MAXPOINTS + 1))
        || (nColMax < 1) || (nColMax > (ML_MAXCOLUMNS + 1))) {
            delete pszBufferW;    pszBufferW = NULL;
            return FALSE;
    }

    char_t *pszT = &pszBufferW[0], *pszTT = NULL;
    nRow = 0;

    // >>
    int_t iColT[SIGMA_MAXCOLUMNS], iColTCount = 0;
    memset(iColT, 0, SIGMA_MAXCOLUMNS * sizeof(int_t));
    // <<

    for (ii = 0; ii < nLen; ii++) {
        if ((pszBufferW[ii] == _T('\n')) || (pszBufferW[ii] == _T('\0'))) {
            if (ii > 0) {
                if (pszBufferW[ii - 1] == _T('\r')) {
                    pszBufferW[ii - 1] = _T('\0');
                }
                else {
                    pszBufferW[ii] = _T('\0');
                }
                int_t iRowVis = tCell.row + nRow;

                // >> Traiter ligne (pszW)
                nlenT = (int_t)_tcslen((const char_t*)pszT);
                if ((nlenT >= 1) && (nlenT < (ML_MAXCOLUMNS * ML_STRSIZES))) {
                    pszTT = &pszT[0];
                    nColumn = 0;
                    for (jj = 0; jj < nlenT + 1; jj++) {
                        if ((pszT[jj] == _T('\0')) || (pszT[jj] == _T('\t')) || (pszT[jj] == _T(';')) || (pszT[jj] == _T(' ')) || (pszT[jj] == _T('\r')) || (pszT[jj] == _T('\n'))) {
                            if (jj > 0) {
                                pszT[jj] = _T('\0');
                                int_t iColVis = tCell.col + nColumn;
                                CCellID TargetCell(iRowVis, iColVis);
                                if (isValid(TargetCell)) {

                                    // >>
                                    if (iColTCount < (nColMax - 1)) {
                                        iColT[iColTCount] = TargetCell.col - 1;
                                        iColTCount += 1;
                                    }
                                    // <<

                                    // Si la cellule n'�tait pas �ditable, la rendre �ditable avant de coller
                                    bMask = pDoc->GetColumnItemMask(m_nGroup, TargetCell.col - 1, TargetCell.row - 1, 0);
                                    if ((bMask & 0x0F) == 0x0F) {
                                        pDoc->SetColumnItemMask(m_nGroup, TargetCell.col - 1, TargetCell.row - 1, 0x00, 0);
                                    }
                                    ValidateAndModifyCellContents(TargetCell.row, TargetCell.col, (LPCTSTR)pszTT);
                                    // Make sure tCell is not selected to avoid data loss
                                    SetItemStateUnselected(TargetCell.row, TargetCell.col);
                                    if (iRowVis > pasteRange.GetMaxRow()) { pasteRange.SetMaxRow(iRowVis); }
                                    if (iColVis > pasteRange.GetMaxCol()) { pasteRange.SetMaxCol(iColVis); }
                                }
                                nColumn += 1;
                                if (nColumn > nColMax) {
                                    break;
                                }
                            }
                            if (jj < (nlenT - 1)) {
                                pszTT = &pszT[jj + 1];
                            }
                        }
                    }
                }
                // <<
            }
            if (ii < (nLen - 1)) {
                pszT = &pszBufferW[ii + 1];
                nRow += 1;
                if (nRow > nRowMax) {
                    break;
                }
            }
        }
    }

    delete pszBufferW;    pszBufferW = NULL;

    // >>
    if (iColTCount > 0) {
        for (jj = 0; jj < iColTCount; jj++) {
            pDoc->SetColumnStatus(m_nGroup, iColT[jj], 0x00, 0);
        }
    }
    // <<

    if (bSelectPastedCells) {
        SetSelectedRange(pasteRange);
        Refresh();
    }
    else {
        ResetSelectedRange();
        Refresh();
    }

    pDoc->SaveState();

    return TRUE;
}

void CDatasheet::OnEditCut()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    COleDataSource* pSource = CopyTextFromDatasheet();
    if (!pSource) {
        return;
    }

    pSource->SetClipboard();
    CutSelectedText();
}

void CDatasheet::OnEditCopy()
{
    COleDataSource* pSource = CopyTextFromDatasheet();
    if (!pSource) {
        return;
    }

    pSource->SetClipboard();
}

void CDatasheet::OnEditClear()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*) (GetDocument());
    if (pDoc == NULL) {
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumnMin = cellRange.GetMinCol() - 1, nColumnMax = cellRange.GetMaxCol() - 1, jj;
    int_t nRowMin = cellRange.GetMinRow() - 1, nRowMax = cellRange.GetMaxRow() - 1, ii;

    if ((nColumnMin < 0) || (nColumnMin >= GetColumnCount())
        || (nColumnMax < 0) || (nColumnMax >= GetColumnCount())
        || (nColumnMin > nColumnMax)) {
        return;
    }
    if ((nRowMin < 0) || (nRowMin >= GetRowCount())
        || (nRowMax < 0) || (nRowMax >= GetRowCount())
        || (nRowMin > nRowMax)) {
        return;
    }

    // ... invalider les �l�ments s�lectionn�s 
    // (!!! pour chaque colonne X, il faut faire correspondre les colonnes Y avec le mask ad hoc et vice-versa !!!)
    // (Association g�r�e par le trac�)
    // ...
    byte_t bMask;
    vector_t *pColumn = NULL;
    for (ii = nRowMin; ii <= nRowMax; ii++) {
        for (jj = nColumnMin; jj <= nColumnMax; jj++) {
            if (IsCellSelected(ii + 1, jj + 1) == FALSE) {
                continue;
            }
            pColumn = pDoc->GetColumn(m_nGroup, jj);
            if (pColumn == NULL) {
                continue;
            }
            if ((pColumn->status & 0x0F) == 0x0F) {    // Colonne non encore �dit�e
                continue;
            }
            bMask = pDoc->GetColumnItemMask(m_nGroup, jj, ii, 0);
            if (bMask == 0xFF) {        // Don't mask or unmask non edited cell
                continue;
            }
            pDoc->SetColumnItemMask(m_nGroup, jj, ii, 0xFF, 0);
        }
    }
    pDoc->UpdateAllViews(NULL);
    SetModified(TRUE, TRUE);
}

void CDatasheet::OnEditPaste()
{
    CCellRange cellRange = GetSelectedCellRange();

    // Get the top-left selected tCell, or the Focus tCell, or the topleft (non-fixed) tCell
    CCellID tCell;
    if (cellRange.isValid()) {
        tCell.row = cellRange.GetMinRow();
        tCell.col = cellRange.GetMinCol();
    }
    else {
        tCell = GetFocusCell();
        if (!isValid(tCell)) {
            tCell = GetTopleftNonFixedCell();
        }
        if (!isValid(tCell)) {
            return;
        }
    }

    // If a tCell is being edited, then call it's edit window paste function.
    if (IsItemEditing(tCell.row, tCell.col)) {
        CCell xCell; CELL_DISPINFO xDispInfo;
        CCell* pCell = GetCell(tCell.row, tCell.col, &xCell, &xDispInfo);
        ASSERT(pCell);
        if (!pCell) { return; }

        if (m_pInEdit && m_pInEdit->IsKindOf(RUNTIME_CLASS(CEdit))) {
            ((CEdit*) m_pInEdit)->Paste();
            return;
        }
    }

    // Attach a COleDataObject to the clipboard and paste the data to the grid
    COleDataObject obj;
    if (obj.AttachClipboard()) {
        PasteTextToDatasheet(tCell, &obj);
    }
}

void CDatasheet::OnEditSelectAll()
{
    SelectAllCells();
}

void CDatasheet::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetSelectedCount() > 0);
}

void CDatasheet::OnUpdateEditClear(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(GetSelectedCount() > 0);
}

void CDatasheet::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(IsEditable() && GetSelectedCount() > 0);
}

void CDatasheet::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    CCellID tCell = GetFocusCell();

    BOOL bCanPaste = (GetSelectedCount() > 0) && ::IsClipboardFormatAvailable(CF_TEXT);

    pCmdUI->Enable(bCanPaste);
}

void CDatasheet::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_bEnableSelection);
}

// TRUE if the mouse is over a row resize area
BOOL CDatasheet::MouseOverRowResizeArea(CPoint& tPoint)
{
    if (tPoint.x >= GetFixedColumnWidth()) {
        return FALSE;
    }

    CCellID idCurrentCell = GetCellFromPt(tPoint);
    CPoint start;
    if (!GetCellOrigin(idCurrentCell, &start)) {
        return FALSE;
    }

    int_t endy = start.y + GetRowHeight(idCurrentCell.row);

    if ((((tPoint.y - start.y) < m_nResizeCaptureRange) && (idCurrentCell.row != 0)) || ((endy - tPoint.y) < m_nResizeCaptureRange)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

// TRUE if the mouse is over a column resize area. tPoint is in Client coords
BOOL CDatasheet::MouseOverColumnResizeArea(CPoint& tPoint)
{
    if (tPoint.y >= GetFixedRowHeight()) {
        return FALSE;
    }

    CCellID idCurrentCell = GetCellFromPt(tPoint);
    CPoint start;
    if (!GetCellOrigin(idCurrentCell, &start)) {
        return FALSE;
    }

    int_t endx = start.x + GetColumnWidth(idCurrentCell.col);

    if ((((tPoint.x - start.x) < m_nResizeCaptureRange) && (idCurrentCell.col != 0)) || ((endx - tPoint.x) < m_nResizeCaptureRange)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

CCellID CDatasheet::GetCellFromPt(CPoint tPoint, BOOL bAllowFixedCellCheck /*=TRUE*/)
{
    CCellID cellID; // return value

    CCellID idTopLeft = GetTopleftNonFixedCell();
    if (!bAllowFixedCellCheck && !isValid(idTopLeft)) {
        return cellID;
    }

    // calculate column index
    int_t fixedColWidth = GetFixedColumnWidth();

    if ((tPoint.x < 0) || (!bAllowFixedCellCheck && (tPoint.x < fixedColWidth))) { // not in window
        cellID.col = -1;
    }
    else if (tPoint.x < fixedColWidth) { // in fixed col
        int_t xpos = 0;
        int_t col = 0;
        while (col < m_nFixedCols) {
            xpos += GetColumnWidth(col);
            if (xpos > tPoint.x) { break; }
            col++;
        }
        cellID.col = col;
    }
    else {                // in non-fixed col
        int_t xpos = fixedColWidth;
        int_t col = idTopLeft.col; //m_nFixedCols;
        while (col < GetColumnCount()) {
            xpos += GetColumnWidth(col);
            if (xpos > tPoint.x) { break; }
            col++;
        }

        if (col >= GetColumnCount()) { cellID.col = -1; }
        else { cellID.col = col; }
    }

    // calculate row index
    int_t fixedRowHeight = GetFixedRowHeight();
    if ((tPoint.y < 0) || (!bAllowFixedCellCheck && (tPoint.y < fixedRowHeight))) { // not in window
        cellID.row = -1;
    }
    else if (tPoint.y < fixedRowHeight) {        // in fixed row

        if ((MOUSE_SELECT_CELLS == m_MouseMode) || (MOUSE_SELECT_ROW == m_MouseMode)) {
            CCellRange cellRange = GetSelectedCellRange();
            cellID.row = cellRange.GetMinRow() - 1, cellID.col = cellRange.GetMinCol() - 1;
        }
        else {
            int_t ypos = 0;
            int_t row = 0;
            while (row < m_nFixedRows) {
                ypos += GetRowHeight(row);
                if (ypos > tPoint.y) { break; }
                row++;
            }
            cellID.row = row;
        }

    }
    else {
        int_t ypos = fixedRowHeight;
        int_t row = idTopLeft.row; //m_nFixedRows;
        while (row < GetRowCount()) {
            ypos += GetRowHeight(row);
            if (ypos > tPoint.y) { break; }
            row++;
        }

        if (row >= GetRowCount()) {
            cellID.row = -1;
        }
        else {
            cellID.row = row;
        }
    }

    return cellID;
}

// CDatasheet cellrange functions

// Gets the first non-fixed tCell ID
CCellID CDatasheet::GetTopleftNonFixedCell(BOOL bForceRecalculation /*=FALSE*/)
{
    // Used cached value if possible
    if (m_idTopLeftCell.isValid() && !bForceRecalculation)
        return m_idTopLeftCell;

    int_t nVertScroll = GetScrollPos(SB_VERT), nHorzScroll = GetScrollPos(SB_HORZ);

    m_idTopLeftCell.col = m_nFixedCols;
    int_t nRight = 0;
    while ((nRight < nHorzScroll) && (m_idTopLeftCell.col < (GetColumnCount()-1))) {
        nRight += GetColumnWidth(m_idTopLeftCell.col++);
    }

    m_idTopLeftCell.row = m_nFixedRows;
    int_t nTop = 0;
    while ((nTop < nVertScroll) && (m_idTopLeftCell.row < (GetRowCount() - 1))) {
        nTop += GetRowHeight(m_idTopLeftCell.row++);
    }

    return m_idTopLeftCell;
}

// This gets even partially visible cells
CCellRange CDatasheet::GetVisibleNonFixedCellRange(LPRECT pRect /*=NULL*/, BOOL bForceRecalculation /*=FALSE*/, int *piWidth/* = NULL*/, int *piHeight/* = NULL*/)
{
    int_t i;
    CRect rcT;
    GetClientRect(rcT);

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    if (piHeight) {
        *piHeight = 0;
    }
    if (piWidth) {
        *piWidth = 0;
    }

    // calc bottom
    int_t bottom = GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++) {
        bottom += GetRowHeight(i);
        if (bottom >= rcT.bottom) {
            bottom = rcT.bottom;
            break;
        }
    }
    if (piHeight) {
        *piHeight = bottom;
    }
    int_t maxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int_t right = GetFixedColumnWidth();
    for (i = idTopLeft.col; i < GetColumnCount(); i++) {
        right += GetColumnWidth(i);
        if (right >= rcT.right) {
            right = rcT.right;
            break;
        }
    }
    if (piWidth) {
        *piWidth = right;
    }
    
    int_t maxVisibleCol = min(i, GetColumnCount() - 1);
    if (pRect) {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, idTopLeft.col, maxVisibleRow, maxVisibleCol);
}

CCellRange CDatasheet::GetVisibleFixedCellRange(LPRECT pRect /*=NULL*/, BOOL bForceRecalculation /*=FALSE*/)
{
    int_t i;
    CRect rcT;
    GetClientRect(rcT);

    CCellID idTopLeft = GetTopleftNonFixedCell(bForceRecalculation);

    // calc bottom
    int_t bottom = GetFixedRowHeight();
    for (i = idTopLeft.row; i < GetRowCount(); i++) {
        bottom += GetRowHeight(i);
        if (bottom >= rcT.bottom) {
            bottom = rcT.bottom;
            break;
        }
    }
    int_t maxVisibleRow = min(i, GetRowCount() - 1);

    // calc right
    int_t right = 0;
    for (i = 0; i < GetFixedColumnCount(); i++) {
        right += GetColumnWidth(i);
        if (right >= rcT.right) {
            right = rcT.right;
            break;
        }
    }
    int_t maxVisibleCol = min(i, GetColumnCount() - 1);
    if (pRect) {
        pRect->left = pRect->top = 0;
        pRect->right = right;
        pRect->bottom = bottom;
    }

    return CCellRange(idTopLeft.row, 0, maxVisibleRow, maxVisibleCol);
}

// Returns the minimum bounding range of the current selection
// If no selection, then the returned CCellRange will be invalid
CCellRange CDatasheet::GetSelectedCellRange()
{
    CCellRange SelectionT(GetRowCount(), GetColumnCount(), -1,-1);

    try {

        std::map<DWORD, CCellID>::iterator itr;
        DWORD key;
        CCellID tCell;

        long iMax = (GetRowCount() - 1) * (GetColumnCount() - 1), ii = 0;
        long iSize = m_SelectedCellMap.size();
        if (iSize > iMax) { iSize = iMax; }
        if (iSize < 1) { return SelectionT; }

        long minRow = SelectionT.GetMinRow(),
            maxRow = SelectionT.GetMaxRow();
        long minCol = SelectionT.GetMinCol(),
            maxCol = SelectionT.GetMaxCol();
        for (itr = m_SelectedCellMap.begin(), ii = 0; (itr != m_SelectedCellMap.end()) && (ii < iSize); ++itr, ii++) {
            key = itr->first;
            tCell = itr->second;
            minRow = __min(minRow, tCell.row);
            minCol = __min(minCol, tCell.col);
            maxRow = __max(maxRow, tCell.row);
            maxCol = __max(maxCol, tCell.col);
        }
        SelectionT.SetMinRow(minRow);
        SelectionT.SetMinCol(minCol);
        SelectionT.SetMaxRow(maxRow);
        SelectionT.SetMaxCol(maxCol);

    }
    catch (...) {

    }

    return SelectionT;
}

// Returns ALL the cells in the grid
CCellRange CDatasheet::GetCellRange()
{
    return CCellRange(0, 0, GetRowCount() - 1, GetColumnCount() - 1);
}

// Resets the selected tCell range to the empty set.
void CDatasheet::ResetSelectedRange()
{
    try {
        m_PrevSelectedCellMap.clear();
        if (m_InDestructor) {
            m_SelectedCellMap.clear();
        }
    }
    catch (...) {

    }

    SetSelectedRange(-1,-1,-1,-1);
    SetFocusCell(-1, -1);
    Refresh();
}

// Get/Set scroll position using 32 bit functions
int_t CDatasheet::GetScrollPos32(int_t nBar, BOOL bGetTrackPos /* = FALSE */)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);

    if (bGetTrackPos) {
        if (GetScrollInfo(nBar, &si, SIF_TRACKPOS)) {
            return si.nTrackPos;
        }
    }
    else {
        if (GetScrollInfo(nBar, &si, SIF_POS)) {
            return si.nPos;
        }
    }

    return 0;
}

BOOL CDatasheet::SetScrollPos32(int_t nBar, int_t nPos, BOOL bRedraw /* = TRUE */)
{
    m_idTopLeftCell.row = -1;

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_POS;
    si.nPos = nPos;
    return SetScrollInfo(nBar, &si, bRedraw);
}

void CDatasheet::EnableScrollBars(int_t nBar, BOOL bEnable /*=TRUE*/)
{
    if (bEnable) {
        if (!IsVisibleHScroll() && ((nBar == SB_HORZ) || (nBar == SB_BOTH))) {
            m_nBarState |= DVL_HORZ;
            CWnd::EnableScrollBarCtrl(SB_HORZ, bEnable);
        }

        if (!IsVisibleVScroll() && ((nBar == SB_VERT) || (nBar == SB_BOTH))) {
            m_nBarState |= DVL_VERT;
            CWnd::EnableScrollBarCtrl(SB_VERT, bEnable);
        }
    }
    else {
        if ( IsVisibleHScroll() && ((nBar == SB_HORZ) || (nBar == SB_BOTH))) {
            m_nBarState &= ~DVL_HORZ; 
            CWnd::EnableScrollBarCtrl(SB_HORZ, bEnable);
        }

        if ( IsVisibleVScroll() && ((nBar == SB_VERT) || (nBar == SB_BOTH))) {
            m_nBarState &= ~DVL_VERT;
            CWnd::EnableScrollBarCtrl(SB_VERT, bEnable);
        }
    }
}

// If resizing or tCell counts/sizes change, call this - it'll fix up the scroll bars
void CDatasheet::ResetScrollBars()
{
    // Force a refresh. 
    m_idTopLeftCell.row = -1;

    CRect rcT;

    GetClientRect(rcT);

    if ((rcT.left == rcT.right) || (rcT.top == rcT.bottom)) {
        return;
    }

    if (IsVisibleVScroll()) {
        rcT.right += GetSystemMetrics(SM_CXVSCROLL) + GetSystemMetrics(SM_CXBORDER);
    }

    if (IsVisibleHScroll()) {
        rcT.bottom += GetSystemMetrics(SM_CYHSCROLL) + GetSystemMetrics(SM_CYBORDER);
    }

    rcT.left += GetFixedColumnWidth();
    rcT.top += GetFixedRowHeight();

    if (rcT.left >= rcT.right || rcT.top >= rcT.bottom) {
        EnableScrollBarCtrl(SB_BOTH, FALSE);
        return;
    }

    CRect VisibleRect(GetFixedColumnWidth(), GetFixedRowHeight(), rcT.right, rcT.bottom);
    CRect VirtualRect(GetFixedColumnWidth(), GetFixedRowHeight(), GetVirtualWidth(), GetVirtualHeight());

    // If vertical scroll bar, horizontal space is reduced
    if (VisibleRect.Height() < VirtualRect.Height()) {
        VisibleRect.right -= ::GetSystemMetrics(SM_CXVSCROLL);
    }
    // If horz scroll bar, vert space is reduced
    if (VisibleRect.Width() < VirtualRect.Width()) {
        VisibleRect.bottom -= ::GetSystemMetrics(SM_CYHSCROLL);
    }
    // Recheck vertical scroll bar
    //if (VisibleRect.Height() < VirtualRect.Height())
    // VisibleRect.right -= ::GetSystemMetrics(SM_CXVSCROLL);

    if (VisibleRect.Height() < VirtualRect.Height()) {
        EnableScrollBars(SB_VERT, TRUE); 
        m_nVScrollMax = VirtualRect.Height() - 1;
    }
    else {
        EnableScrollBars(SB_VERT, FALSE); 
        m_nVScrollMax = 0;
    }

    if (VisibleRect.Width() < VirtualRect.Width()) {
        EnableScrollBars(SB_HORZ, TRUE); 
        m_nHScrollMax = VirtualRect.Width() - 1;
    }
    else {
        EnableScrollBars(SB_HORZ, FALSE); 
        m_nHScrollMax = 0;
    }

    ASSERT(m_nVScrollMax < INT_MAX && m_nHScrollMax < INT_MAX); // This should be fine

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = (m_nHScrollMax>0)? VisibleRect.Width() : 0;
    si.nMin = 0;
    si.nMax = m_nHScrollMax;
    SetScrollInfo(SB_HORZ, &si, TRUE);

    si.fMask |= SIF_DISABLENOSCROLL;
    si.nPage = (m_nVScrollMax>0)? VisibleRect.Height() : 0;
    si.nMin = 0;
    si.nMax = m_nVScrollMax;
    SetScrollInfo(SB_VERT, &si, TRUE);
}

// returns the top left tPoint of the tCell. Returns FALSE if tCell not visible.
BOOL CDatasheet::GetCellOrigin(int_t nRow, int_t nCol, LPPOINT pPoint)
{
    int_t ii;

    if (!isValid(nRow, nCol)) {
        return FALSE;
    }

    CCellID idTopLeft;
    if (nCol >= m_nFixedCols || nRow >= m_nFixedRows) {
        idTopLeft = GetTopleftNonFixedCell();
    }

    if (((nRow >= m_nFixedRows) && (nRow < idTopLeft.row)) || ((nCol >= m_nFixedCols) && (nCol < idTopLeft.col))) {
        return FALSE;
    }

    pPoint->x = 0;
    if (nCol < m_nFixedCols) {                                    // is a fixed column
        for (ii = 0; ii < nCol; ii++) {
            pPoint->x += GetColumnWidth(ii);
        }
    }
    else {                                                            // is a scrollable data column
        for (ii = 0; ii < m_nFixedCols; ii++) {
            pPoint->x += GetColumnWidth(ii);
        }
        for (ii = idTopLeft.col; ii < nCol; ii++) {
            pPoint->x += GetColumnWidth(ii);
        }
    }

    pPoint->y = 0;
    if (nRow < m_nFixedRows) {                                    // is a fixed row
        for (ii = 0; ii < nRow; ii++) {
            pPoint->y += GetRowHeight(ii);
        }
    }
    else {                                                            // is a scrollable data row
        for (ii = 0; ii < m_nFixedRows; ii++) {
            pPoint->y += GetRowHeight(ii);
        }
        for (ii = idTopLeft.row; ii < nRow; ii++) {
            pPoint->y += GetRowHeight(ii);
        }
    }

    return TRUE;
}

BOOL CDatasheet::GetCellOrigin(const CCellID& tCell, LPPOINT pPoint)
{
    return GetCellOrigin(tCell.row, tCell.col, pPoint);
}

// Returns the bounding box of the tCell
BOOL CDatasheet::GetCellRect(const CCellID& tCell, LPRECT pRect)
{
    return GetCellRect(tCell.row, tCell.col, pRect);
}

BOOL CDatasheet::GetCellRect(int_t nRow, int_t nCol, LPRECT pRect)
{
    CPoint CellOrigin;
    if (!GetCellOrigin(nRow, nCol, &CellOrigin)) {
        return FALSE;
    }

    pRect->left = CellOrigin.x;
    pRect->top = CellOrigin.y;
    pRect->right  = CellOrigin.x + GetColumnWidth(nCol) - 1;
    pRect->bottom = CellOrigin.y + GetRowHeight(nRow) - 1;

    return TRUE;
}

BOOL CDatasheet::GetTextRect(const CCellID& tCell, LPRECT pRect)
{
    return GetTextRect(tCell.row, tCell.col, pRect);
}

BOOL CDatasheet::GetTextRect(int_t nRow, int_t nCol, LPRECT pRect)
{
    CCell xCell; CELL_DISPINFO xDispInfo;
    CCell* pCell = GetCell(nRow, nCol, &xCell, &xDispInfo);
    if (pCell == NULL) {
        return FALSE;
    }

    if (!GetCellRect( nRow, nCol, pRect)) {
        return FALSE;
    }

    return pCell->GetTextRect(pRect);
}

// Returns the bounding box of a range of cells
BOOL CDatasheet::GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect)
{
    CPoint MinOrigin, MaxOrigin;

    if (!GetCellOrigin(cellRange.GetMinRow(), cellRange.GetMinCol(), &MinOrigin)) {
        return FALSE;
    }
    if (!GetCellOrigin(cellRange.GetMaxRow(), cellRange.GetMaxCol(), &MaxOrigin)) {
        return FALSE;
    }

    lpRect->left = MinOrigin.x;
    lpRect->top = MinOrigin.y;
    lpRect->right = MaxOrigin.x + GetColumnWidth(cellRange.GetMaxCol()) - 1;
    lpRect->bottom = MaxOrigin.y + GetRowHeight(cellRange.GetMaxRow()) - 1;

    return TRUE;
}

LRESULT CDatasheet::OnSetFont(WPARAM hFont, LPARAM /*lParam */)
{
    LRESULT result = Default();

    // Get the logical font
    LOGFONT lf;
    if (!GetObject((HFONT) hFont, sizeof(LOGFONT), &lf)) {
        return result;
    }

    m_cellDefault.SetFont(&lf);
    m_cellFixedColDef.SetFont(&lf);
    m_cellFixedRowDef.SetFont(&lf);
    m_cellFixedRowColDef.SetFont(&lf);

    Refresh();

    CDC* pDC = CDC::FromHandle(::GetDC(NULL));
    if (pDC) {
        lf.lfWeight = FW_BOLD;
        CFont tFont;
        if (tFont.CreateFontIndirect(&lf)) {
            CFont* pOldFont = pDC->SelectObject(&tFont);
            CSize sizeTT = pDC->GetTextExtent(_T("S "), 2);
            if ((sizeTT.cx > 4) && (sizeTT.cx < 256) && (sizeTT.cy > 5) && (sizeTT.cy < 256)) {
                m_nMinWidth = sizeTT.cx;
                m_nMinHeight = sizeTT.cy;
            }
            tFont.DeleteObject();
            pDC->SelectObject(pOldFont);
        }
        ::ReleaseDC(NULL, pDC->GetSafeHdc());    pDC = NULL;
    }

    return result;
}

LRESULT CDatasheet::OnGetFont(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return (LRESULT) m_cellDefault.GetFontObject()->GetSafeHandle();
}

BOOL CDatasheet::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (nHitTest == HTCLIENT) {
        switch (m_MouseMode) {
            case MOUSE_SIZING_COL:
            case MOUSE_OVER_COL_DIVIDE:
                SetCursor(SigmaApp.LoadStandardCursor(IDC_SIZEWE));
                break;
            case MOUSE_SIZING_ROW:
            case MOUSE_OVER_ROW_DIVIDE:
                SetCursor(SigmaApp.LoadStandardCursor(IDC_SIZENS));
                break;
            default:
                SetCursor(SigmaApp.LoadStandardCursor(IDC_ARROW));
                break;
        }
        return TRUE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CDatasheet::SetFixedRowCount(int_t nFixedRows)
{
    if (m_nFixedRows == nFixedRows) {
        return TRUE;
    }

    ASSERT(nFixedRows >= 0);

    ResetSelectedRange();

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nFixedRows > GetRowCount()) {
        if (!SetRowCount(nFixedRows)) {
            return FALSE;
        }
    }

    if (m_idCurrentCell.row < nFixedRows) {
        SetFocusCell(-1, - 1);
    }

    m_nFixedRows = nFixedRows;

    Refresh();

    return TRUE;
}

BOOL CDatasheet::SetFixedColumnCount(int_t nFixedCols)
{
    if (m_nFixedCols == nFixedCols) {
        return TRUE;
    }

    ASSERT(nFixedCols >= 0);

    if (nFixedCols > GetColumnCount()) {
        if (!SetColumnCount(nFixedCols)) {
            return FALSE;
        }
    }

    if (m_idCurrentCell.col < nFixedCols) {
        SetFocusCell(-1, - 1);
    }

    ResetSelectedRange();

    // Force recalculation
    m_idTopLeftCell.col = -1;

    m_nFixedCols = nFixedCols;

    Refresh();

    return TRUE;
}

BOOL CDatasheet::SetRowCount(int_t nRows)
{
    BOOL bResult = TRUE;

    ASSERT(nRows >= 0);
    if (nRows == GetRowCount()) { return bResult; }

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nRows < m_nFixedRows) {
        m_nFixedRows = nRows;
    }

    if (m_idCurrentCell.row >= nRows) {
        SetFocusCell(-1, -1);
    }

    int_t addedRows = nRows - GetRowCount();

    // If we are about to lose rows, then we need to delete the Cell objects
    // in each column within each row
    if (addedRows < 0) {
        m_nRows = nRows;
    }

    TRY {
        m_arRowHeights.SetSize(nRows);

        // Change the number of rows.
        m_nRows = nRows;
        if (addedRows > 0) {
            int_t startRow = nRows - addedRows;
            int_t iHeight = m_cellDefault.GetHeight();
            for (int_t row = startRow; row < nRows; row++) {
                m_arRowHeights[row] = iHeight;
            }
        }
        ResetVirtualOrder();
    }
    CATCH (CMemoryException, excT) {
        excT->ReportError();
        bResult = FALSE;
    }
    END_CATCH

    SetModified(TRUE, TRUE);
    ResetScrollBars();
    Refresh();

    return bResult;
}

BOOL CDatasheet::SetColumnCount(int_t nCols)
{
    BOOL bResult = TRUE;

    ASSERT(nCols >= 0);

    if (nCols == GetColumnCount()) {
        return bResult;
    }

    // Force recalculation
    m_idTopLeftCell.col = -1;

    if (nCols < m_nFixedCols) {
        m_nFixedCols = nCols;
    }

    if (m_idCurrentCell.col >= nCols) {
        SetFocusCell(-1, - 1);
    }

    int_t addedCols = nCols - GetColumnCount();

    TRY {
        // Change the number of columns.
        m_arColWidths.SetSize(nCols);

        // If we have just added columns, we need to construct new elements for each tCell
        // and set the default column width
        if (addedCols > 0) {
            int_t col;

            // initialized column widths
            int_t startCol = nCols - addedCols, iWidth = m_cellFixedColDef.GetWidth();
            for (col = startCol; col < nCols; col++) {
                m_arColWidths[col] = iWidth;
            }
        }
    }
    CATCH (CMemoryException, excT) {
        excT->ReportError();
        bResult = FALSE;
    }
    END_CATCH

    m_arColOrder.resize(nCols);  // Reset Column Order

    for (int_t ii = 0; ii < nCols; ii++) {
        m_arColOrder[ii] = ii;
    }

    m_nCols = nCols;

    ResetScrollBars();
    Refresh();

    return bResult;
}

BOOL CDatasheet::SetCellType(int_t nRow, int_t nCol, CRuntimeClass* pRuntimeClass)
{
    return FALSE;
}

BOOL CDatasheet::SetDefaultCellType(CRuntimeClass* pRuntimeClass)
{
    ASSERT(pRuntimeClass != NULL);
    if (!pRuntimeClass->IsDerivedFrom(RUNTIME_CLASS(CCell))) {
        ASSERT( FALSE);
        return FALSE;
    }
    m_pRtcDefault = pRuntimeClass;
    return TRUE;
}

// Removes all rows, columns and data from the grid.
BOOL CDatasheet::DeleteAllItems()
{
    ResetSelectedRange();

    m_arColWidths.RemoveAll();
    m_arRowHeights.RemoveAll();

    m_arRowOrder.clear();

    m_idCurrentCell.row = m_idCurrentCell.col = -1;
    m_nRows = m_nFixedRows = m_nCols = m_nFixedCols = 0;

    ResetScrollBars();

    SetModified(TRUE, TRUE);

    return TRUE;
}

void CDatasheet::AutoFill()
{
    CRect rcT;
    GetClientRect(rcT);

    SetColumnCount(rcT.Width() / m_cellDefault.GetWidth() + 1);
    SetRowCount(rcT.Height() / m_cellDefault.GetHeight() + 1);
    SetFixedRowCount(1);
    SetFixedColumnCount(1);
    ExpandToFit();
}

BOOL CDatasheet::SetItem(const GV_ITEM* pItem)
{
    return FALSE;
}

BOOL CDatasheet::GetItem(GV_ITEM* pItem)
{
    if (!pItem) {
        return FALSE;
    }

    CCell xCell; CELL_DISPINFO xDispInfo;
    CCell* pCell = GetCell(pItem->row, pItem->col, &xCell, &xDispInfo);
    if (!pCell) {
        return FALSE;
    }

    if (pItem->mask & DVIF_TEXT) {
        CString strT = GetItemText(pItem->row, pItem->col);
        _tcscpy(pItem->szText, (LPCTSTR)strT);
    }
    if (pItem->mask & DVIF_PARAM) {
        pItem->lParam  = pCell->GetData();;
    }
    if (pItem->mask & DVIF_STATE) {
        pItem->nState  = pCell->GetState();
    }
    if (pItem->mask & DVIF_FORMAT) {
        pItem->nFormat = pCell->GetFormat();
    }
    if (pItem->mask & DVIF_BKCLR) {
        pItem->crBkClr = pCell->GetBackClr();
    }
    if (pItem->mask & DVIF_FGCLR) {
        pItem->crFgClr = pCell->GetTextClr();
    }
    if (pItem->mask & DVIF_FONT) {
        memcpy(&(pItem->lfFont), pCell->GetFont(), sizeof(LOGFONT));
    }
    if (pItem->mask & DVIF_MARGIN) {
        pItem->nMargin = pCell->GetMargin();
    }

    return TRUE;
}

BOOL CDatasheet::SetItemText(int_t nRow, int_t nCol, LPCTSTR szText, BOOL bParse/* = TRUE*/)
{
    if (szText == NULL) {
        return FALSE;
    }

    CView *pOwner = (CView*)GetOwner();
    if (pOwner == NULL) {
        return FALSE;
    }

    if (::IsWindow(pOwner->m_hWnd) == FALSE) {
        return FALSE;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(pOwner->GetDocument());
    if (pDoc == NULL) {
        return FALSE;
    }

    if ((nCol <= 0) || (nRow <= 0)) {
        return FALSE;
    }

    if ((*szText == _T('\0')) || (_tcscmp(szText, _T("-")) == 0)
        || ((*(szText + 1) == _T('\0')) && !_istdigit(*szText))) {
        pDoc->SetColumnItem(m_nGroup, nCol - 1, nRow - 1, 0.0, 0);
        pDoc->SetColumnItemMask(m_nGroup, nCol - 1, nRow - 1, 0xFF, 0);
        return TRUE;
    }

    char_t szOutput[ML_STRSIZE], szFmt[ML_STRSIZET];
    real_t fval = 0.0;
    if (bParse) {
        int_t nlen = _tcslen(szText);
        if (nlen < 2) {
            fval = ml_toreal((const char_t*) szText, NULL, NULL);
        }
        else {
            pDoc->GetColumnFormat(m_nGroup, nCol - 1, szFmt);
            if (SigmaApp.DoParse((const char_t*) szText, szOutput, &fval, (const char_t*) szFmt) == FALSE) {
                return FALSE;
            }
        }
    }
    else {
        fval = ml_toreal((const char_t*) szText, NULL, NULL);
    }
    //

    pDoc->SetColumnItem(m_nGroup, nCol - 1, nRow - 1, fval, 0);
    byte_t bMask = pDoc->GetColumnItemMask(m_nGroup, nCol - 1, nRow - 1, 0);
    if ((bMask & 0x0F) == 0x0F) {
        pDoc->SetColumnItemMask(m_nGroup, nCol - 1, nRow - 1, 0x00, 0);
    }

    return TRUE;
}

__inline LPARAM CDatasheet::GetItemData(int_t nRow, int_t nCol)
{
    CCell xCell;
    CELL_DISPINFO xDispInfo;
    CCell* pCell = GetCell(nRow, nCol, &xCell, &xDispInfo);
    if (!pCell) {
        return (LPARAM) 0;
    }

    return pCell->GetData();
}

__inline BOOL CDatasheet::SetItemStateSelected(int_t nRow, int_t nCol)
{
    DWORD keyT = MAKELONG(nRow, nCol);

    try {
        CCellID tCell(nRow, nCol);
        m_SelectedCellMap.insert(std::pair<DWORD, CCellID>(keyT, tCell));
        return TRUE;
    }
    catch (...) {
        return FALSE;
    }
}

__inline BOOL CDatasheet::SetItemStateUnselected(int_t nRow, int_t nCol)
{
    DWORD keyT = MAKELONG(nRow, nCol);

    try {
        m_SelectedCellMap.erase(keyT);
        m_PrevSelectedCellMap.erase(keyT);
        return TRUE;
    }
    catch (...) {
        return FALSE;
    }
}

__inline BOOL CDatasheet::SetItemState(int_t nRow, int_t nCol, UINT state)
{
    DWORD keyT = MAKELONG(nRow, nCol);

    try {
        // If the tCell is being unselected, remove it from the selected list
        if (!(state & DVIS_SELECTED)) {
            m_SelectedCellMap.erase(keyT);
            m_PrevSelectedCellMap.erase(keyT);
            return TRUE;
        }

        // If tCell is being selected, add it to the list of selected cells
        if (state & DVIS_SELECTED) {
            CCellID tCell(nRow, nCol);
            m_SelectedCellMap.insert(std::pair<DWORD, CCellID>(keyT, tCell));
            return TRUE;
        }

    }
    catch (...) {

    }

    return FALSE;
}

__inline UINT CDatasheet::GetItemState(int_t nRow, int_t nCol)
{
    CCell xCell;
    CELL_DISPINFO xDispInfo;
    CCell* pCell = (CCell*)GetCell(nRow, nCol, &xCell, &xDispInfo);
    ASSERT(pCell);
    if (!pCell) {
        return 0;
    }

    return pCell->GetState();
}

__inline BOOL CDatasheet::IsItemEditing(int_t nRow, int_t nCol)
{
    return ((m_pInEdit != NULL) && ::IsWindow(m_pInEdit->GetSafeHwnd()));
}

// Row/Column tSize functions

__inline int_t CDatasheet::GetRowCount()
{
    return m_nRows;
}

__inline int_t CDatasheet::GetColumnCount()
{
    return m_nCols;
}

long_t CDatasheet::GetVirtualWidth()
{
    long_t lVirtualWidth = 0;
    int_t iColCount = GetColumnCount();
    for (int_t i = 0; i < iColCount; i++) {
        lVirtualWidth += m_arColWidths[i];
    }

    return lVirtualWidth;
}

long_t CDatasheet::GetVirtualHeight()
{
    long_t lVirtualHeight = 0;
    int_t iRowCount = GetRowCount();
    for (int_t i = 0; i < iRowCount; i++) {
        lVirtualHeight += m_arRowHeights[i];
    }

    return lVirtualHeight;
}

int_t CDatasheet::GetRowHeight(int_t nRow)
{
    ASSERT((nRow >= 0) && (nRow < m_nRows));
    if ((nRow < 0) || (nRow >= m_nRows)) {
        return -1;
    }

    return m_arRowHeights[nRow];
}

int_t CDatasheet::GetColumnWidth(int_t nCol)
{
    ASSERT((nCol >= 0) && (nCol < m_nCols));
    if ((nCol < 0) || (nCol >= m_nCols)) {
        return -1;
    }

    return m_arColWidths[m_arColOrder[nCol]];
}

int_t CDatasheet::GetColumnWidth()
{
    ASSERT(m_nCols > 0);
    if (m_nCols < 2) {
        return -1;
    }

    return m_arColWidths[m_nCols - 1];
}

__inline BOOL CDatasheet::SetRowHeight(int_t nRow, int_t height, bool bResetScrollBars/* = true*/)
{
    ASSERT((nRow >= 0) && (nRow < m_nRows) && (height >= 0));
    if ((nRow < 0) || (nRow >= m_nRows)) {
        return FALSE;
    }

    if (height < m_nMinHeight) {
        height = m_nMinHeight;
    }

    m_arRowHeights[nRow] = height;
    if (bResetScrollBars) {
        ResetScrollBars();
    }

    return TRUE;
}

__inline BOOL CDatasheet::SetColumnWidth(int_t nCol, int_t width, bool bResetScrollBars/* = true*/)
{
    ASSERT((nCol >= 0) && (nCol < m_nCols) && (width >= 0));
    if ((nCol < 0) || (nCol >= m_nCols)) {
        return FALSE;
    }

    if (width < m_nMinWidth) {
        width = m_nMinWidth;
    }

    m_arColWidths[m_arColOrder[nCol]] = width;
    if (bResetScrollBars) {
        ResetScrollBars();
    }

    return TRUE;
}

__inline int_t CDatasheet::GetFixedRowHeight()
{
    int_t nHeight = 0;
    for (int_t i = 0; i < m_nFixedRows; i++) {
        nHeight += GetRowHeight(i);
    }

    return nHeight;
}

__inline int_t CDatasheet::GetFixedColumnWidth()
{
    int_t nWidth = 0;
    for (int_t i = 0; i < m_nFixedCols; i++) {
        nWidth += GetColumnWidth(i);
    }

    return nWidth;
}

BOOL CDatasheet::AutoSizeColumn(int_t nCol, UINT nAutoSizeStyle /*=DVS_DEFAULT*/, BOOL bResetScroll /*=TRUE*/)
{
    ASSERT((nCol >= 0) && (nCol < m_nCols));
    if ((nCol < 0) || (nCol >= m_nCols)) {
        return FALSE;
    }

    //  Skip hidden columns when autosizing
    if (GetColumnWidth(nCol) <= 0) {
        return FALSE;
    }

    CDC* pDC = GetDC();
    if (pDC == NULL) {
        return FALSE;
    }

    ASSERT(DVS_DEFAULT <= nAutoSizeStyle && nAutoSizeStyle <= DVS_BOTH);
    if (nAutoSizeStyle == DVS_DEFAULT) {
        nAutoSizeStyle = GetAutoSizeStyle();
    }

    int_t nStartRow = (nAutoSizeStyle & DVS_HEADER)? 0 : GetFixedRowCount();
    int_t nEndRow = (nAutoSizeStyle & DVS_DATA) ? (GetRowCount() - 1) : (GetFixedRowCount() - 1);

    CCell xCell;
    CELL_DISPINFO xDispInfo;
    CCell* pCell;
    CSize tSize, sizeT;
    int_t nWidth = 0;

    for (int_t nRow = nStartRow; nRow <= nEndRow; nRow++) {
        pCell = (CCell*) GetCell(nRow, nCol, &xCell, &xDispInfo);
        if (pCell) {
            tSize = pCell->GetCellExtent(pDC);
        }
        if (tSize.cx > nWidth) {
            nWidth = tSize.cx;
        }
    }

    m_arColWidths[nCol] = nWidth;

    ReleaseDC(pDC); pDC = NULL;
    if (bResetScroll) {
        ResetScrollBars();
    }

    return TRUE;
}

BOOL CDatasheet::AutoSizeRow(int_t nRow, BOOL bResetScroll /*=TRUE*/)
{
    ASSERT((nRow >= 0) && (nRow < m_nRows));
    if ((nRow < 0) || (nRow >= m_nRows)) {
        return FALSE;
    }

    //  Skip hidden rows when autosizing
    if (GetRowHeight(nRow) <=0) {
        return FALSE;
    }

    CSize tSize;
    CDC* pDC = GetDC();
    if (pDC == NULL) {
        return FALSE;
    }

    int_t nHeight = 0;
    int_t nNumColumns = GetColumnCount();

    for (int_t nCol = 0; nCol < nNumColumns; nCol++) {
        CCell xCell;
        CELL_DISPINFO xDispInfo;
        CCell* pCell = (CCell*)GetCell(nRow, nCol, &xCell, &xDispInfo);
        if (pCell) {
            tSize = pCell->GetCellExtent(pDC);
        }
        if (tSize.cy > nHeight) {
            nHeight = tSize.cy;
        }
    }
    m_arRowHeights[nRow] = nHeight;

    ReleaseDC(pDC); pDC = NULL;
    if (bResetScroll) {
        ResetScrollBars();
    }

    return TRUE;
}

void CDatasheet::AutoSizeColumns(UINT nAutoSizeStyle /*=DVS_DEFAULT*/)
{
    int_t nNumColumns = GetColumnCount();
    for (int_t nCol = 0; nCol < nNumColumns; nCol++) {
        //  Skip hidden columns when autosizing
        if (GetColumnWidth(nCol) > 0) {
            AutoSizeColumn(nCol, nAutoSizeStyle, FALSE);
        }
    }
    ResetScrollBars();
}

void CDatasheet::AutoSizeRows()
{
    int_t nNumRows = GetRowCount();
    for (int_t nRow = 0; nRow < nNumRows; nRow++) {
        //  Skip hidden rows when autosizing
        if (GetRowHeight(nRow) > 0) {
            AutoSizeRow(nRow, FALSE);
        }
    }
    ResetScrollBars();
}

// sizes all rows and columns
// faster than calling both AutoSizeColumns() and AutoSizeRows()
void CDatasheet::AutoSize(UINT nAutoSizeStyle /*=DVS_DEFAULT*/)
{
    CDC* pDC = GetDC();
    if (pDC == NULL) {
        return;
    }

    int_t nNumColumns = GetColumnCount();

    int_t nCol, nRow;

    ASSERT(DVS_DEFAULT <= nAutoSizeStyle && nAutoSizeStyle <= DVS_BOTH);
    if (nAutoSizeStyle == DVS_DEFAULT) {
        nAutoSizeStyle = GetAutoSizeStyle();
    }

    int_t nStartRow = (nAutoSizeStyle & DVS_HEADER)? 0 : GetFixedRowCount();
    int_t nEndRow = (nAutoSizeStyle & DVS_DATA) ? (GetRowCount() - 1) : (GetFixedRowCount() - 1);

    // Row initialisation - only work on rows whose height is > 0
    for (nRow = nStartRow; nRow <= nEndRow; nRow++) {
        if (GetRowHeight( nRow) > 0) {
            m_arRowHeights[nRow] = 1;
        }
    }

    CSize tSize;
    for (nCol = 0; nCol < nNumColumns; nCol++) {
        // Don't tSize hidden columns or rows
        if (GetColumnWidth( nCol) > 0) {
            // Skip columns that are hidden, but now initialize
            m_arColWidths[nCol] = 0;
            for (nRow = nStartRow; nRow <= nEndRow; nRow++) {
                if (GetRowHeight(nRow) > 0) {
                    CCell xCell;
                    CELL_DISPINFO xDispInfo;
                    CCell* pCell = (CCell*) GetCell(nRow, nCol, &xCell, &xDispInfo);
                    if (pCell) {
                        tSize = pCell->GetCellExtent(pDC);
                        tSize.cx += 5;
                    }
                    if (tSize.cx > (int_t) m_arColWidths[nCol]) {
                        m_arColWidths[nCol] = tSize.cx;
                    }
                    if (tSize.cy > (int_t) m_arRowHeights[nRow]) {
                        m_arRowHeights[nRow] = tSize.cy;
                    }
                }
            }
        }
    }

    ReleaseDC(pDC); pDC = NULL;

    ResetScrollBars();
    Refresh();
}

// Expands the columns to fit the screen space. If bExpandFixed is FALSE then fixed 
// columns will not be affected
void CDatasheet::ExpandColumnsToFit(BOOL bExpandFixed /*=TRUE*/)
{
    if (bExpandFixed) {
        if (GetColumnCount() <= 0) { return; }
    } 
    else {
        if (GetColumnCount() <= GetFixedColumnCount()) { return; }
    }

    EnableScrollBars(SB_HORZ, FALSE);

    int_t col;
    CRect rcT;
    GetClientRect(rcT);

    int_t nFirstColumn = (bExpandFixed) ? 0 : GetFixedColumnCount();

    int_t nNumColumnsAffected = 0;
    for (col = nFirstColumn; col < GetColumnCount(); col++) {
        if (m_arColWidths[col] > 0) {
            nNumColumnsAffected++;
        }
    }

    if (nNumColumnsAffected <= 0) {
        return;
    }

    long_t virtualWidth = GetVirtualWidth();
    int_t nDifference = rcT.Width() - (int_t)virtualWidth;
    int_t nColumnAdjustment = nDifference / nNumColumnsAffected;

    for (col = nFirstColumn; col < GetColumnCount(); col++) {
        if (m_arColWidths[col] > 0) {
            m_arColWidths[col] += nColumnAdjustment;
        }
    }

    if (nDifference > 0) {
        int_t leftOver = nDifference % nNumColumnsAffected;
        for (int_t nCount = 0, col = nFirstColumn; (col < GetColumnCount()) && (nCount < leftOver); col++, nCount++) {
            if (m_arColWidths[col] > 0)
                m_arColWidths[col] += 1;
        }
    }
    else {
        int_t leftOver = (-nDifference) % nNumColumnsAffected;
        for (int_t nCount = 0, col = nFirstColumn; (col < GetColumnCount()) && (nCount < leftOver); col++, nCount++) {
            if (m_arColWidths[col] > 0) {
                m_arColWidths[col] -= 1;
            }
        }
    }

    Refresh();

    ResetScrollBars();
}

void CDatasheet::ExpandLastColumn()
{
    if (GetColumnCount() <= 0) {
        return;
    }

    // Search for last non-hidden column
    int_t nLastColumn = GetColumnCount()-1;
    while (m_arColWidths[nLastColumn] <= 0) {
        nLastColumn--;
    }

    if (nLastColumn <= 0) {
        return;
    }

    EnableScrollBars(SB_HORZ, FALSE);

    CRect rcT;
    GetClientRect(rcT);

    long_t virtualWidth = GetVirtualWidth();
    int_t nDifference = rcT.Width() -(int_t) virtualWidth;

    if (nDifference > 0) {
        m_arColWidths[nLastColumn] += nDifference;
        Refresh();
    }

    ResetScrollBars();
}

// Expands the rows to fit the screen space. If bExpandFixed is FALSE then fixed 
// rows will not be affected
void CDatasheet::ExpandRowsToFit(BOOL bExpandFixed /*=TRUE*/)
{
    if (bExpandFixed) {
        if (GetRowCount() <= 0) { return; }
    }
    else {
        if (GetRowCount() <= GetFixedRowCount()) { return; }
    }

    EnableScrollBars(SB_VERT, FALSE); 

    int_t nRow;
    CRect rcT;
    GetClientRect(rcT);

    int_t nFirstRow = (bExpandFixed) ? 0 : GetFixedRowCount();

    int_t nNumRowsAffected = 0;
    for (nRow = nFirstRow; nRow < GetRowCount(); nRow++) {
        if (m_arRowHeights[nRow] > 0) {
            nNumRowsAffected++;
        }
    }

    if (nNumRowsAffected <= 0) {
        return;
    }

    long_t virtualHeight = GetVirtualHeight();
    int_t nDifference = rcT.Height() -(int_t) virtualHeight;
    int_t nRowAdjustment = nDifference / nNumRowsAffected;

    for (nRow = nFirstRow; nRow < GetRowCount(); nRow++) {
        if (m_arRowHeights[nRow] > 0) {
            m_arRowHeights[nRow] += nRowAdjustment;
        }
    }

    if (nDifference > 0) {
        int_t leftOver = nDifference % nNumRowsAffected;
        for (int_t nCount = 0, nRow = nFirstRow; (nRow < GetRowCount()) && (nCount < leftOver); nRow++, nCount++) {
            if (m_arRowHeights[nRow] > 0)
                m_arRowHeights[nRow] += 1;
        }
    } 
    else {
        int_t leftOver = (-nDifference) % nNumRowsAffected;
        for (int_t nCount = 0, nRow = nFirstRow; (nRow < GetRowCount()) && (nCount < leftOver); nRow++, nCount++) {
            if (m_arRowHeights[nRow] > 0) {
                m_arRowHeights[nRow] -= 1;
            }
        }
    }

    Refresh();

    ResetScrollBars();
}

// Expands the cells to fit the screen space. If bExpandFixed is FALSE then fixed 
// cells  will not be affected
void CDatasheet::ExpandToFit(BOOL bExpandFixed /*=TRUE*/)
{
    ExpandColumnsToFit(bExpandFixed);        // This will remove any existing horz scrollbar
    ExpandRowsToFit(bExpandFixed);            // This will remove any existing vert scrollbar
    ExpandColumnsToFit(bExpandFixed);        // Just in case the first adjustment was with a vert
    // scrollbar in place
    Refresh();
}

void CDatasheet::SetGridLines(int_t nWhichLines /*=DVL_BOTH*/) 
{
    m_nGridLines = nWhichLines;
    Refresh();
}

BOOL CDatasheet::IsCellFixed(int_t nRow, int_t nCol)
{
    return ((nRow < GetFixedRowCount()) || (nCol < GetFixedColumnCount()));
}

void CDatasheet::SetModified(BOOL bModified /*=TRUE*/, BOOL bSaveState/* = FALSE*/)
{
    m_bModified = bModified;

    CSigmaDoc *pDoc = (CSigmaDoc*) GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return;
    }
    pDoc->SetModifiedFlag(m_bModified);
    if (m_bModified && bSaveState) {
        pDoc->SaveState();
    }
}

BOOL CDatasheet::GetModified(int_t nRow /*=-1*/, int_t nCol /*=-1*/)
{
    if (nRow >= 0 && nCol >= 0) {
        return ((GetItemState(nRow, nCol) & DVIS_MODIFIED) == DVIS_MODIFIED);
    }
    else {
        return m_bModified;
    }
}

void CDatasheet::Refresh()
{
    if (GetSafeHwnd()) {
        Invalidate();
    }
}

// EnsureVisible supplied by Roelf Werkman
void CDatasheet::EnsureVisible(int_t nRow, int_t nCol)
{
    CRect rectWindow;

    // We are going to send some scroll messages, which will steal the focus 
    // from it's rightful owner.
    CWnd* pFocusWnd = GetFocus();

    CCellRange VisibleCells = GetVisibleNonFixedCellRange();

    int_t right = nCol - VisibleCells.GetMaxCol();
    int_t left  = VisibleCells.GetMinCol() - nCol;
    int_t down = nRow - VisibleCells.GetMaxRow();
    int_t up = VisibleCells.GetMinRow() - nRow;

    int_t iColumnStart;
    int_t iRowStart;

    iColumnStart = VisibleCells.GetMaxCol() + 1;
    while (right > 0) {
        if (GetColumnWidth( iColumnStart ) > 0) {
            SendMessage( WM_HSCROLL, SB_LINERIGHT, 0 );
        }

        right--;
        iColumnStart++;
    }

    iColumnStart = VisibleCells.GetMinCol() - 1;
    while (left > 0) {
        if (GetColumnWidth( iColumnStart ) > 0) {
            SendMessage( WM_HSCROLL, SB_LINELEFT, 0);
        }
        left--;
        iColumnStart--;
    }

    iRowStart = VisibleCells.GetMaxRow() + 1;
    while (down > 0) {
        if (GetRowHeight( iRowStart ) > 0) {
            SendMessage( WM_VSCROLL, SB_LINEDOWN, 0 );
        }
        down--;
        iRowStart++;
    }

    iRowStart = VisibleCells.GetMinRow() - 1;
    while (up > 0) {
        if (GetRowHeight( iRowStart ) > 0) {
            SendMessage( WM_VSCROLL, SB_LINEUP, 0 );
        }
        up--;
        iRowStart--;
    }

    // Move one more if we only see a snall bit of the tCell
    CRect rectCell;
    if (!GetCellRect(nRow, nCol, rectCell)) {
        if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd())) {
            pFocusWnd->SetFocus();
        }
        return;
    }

    GetClientRect(rectWindow);

    int ii = 0, iimax = m_nCols;
    while ((ii++ < iimax) && rectCell.right > rectWindow.right && rectCell.left > GetFixedColumnWidth() && IsVisibleHScroll()) {
        SendMessage(WM_HSCROLL, SB_LINERIGHT, 0);
        if (!GetCellRect(nRow, nCol, rectCell)) {
            if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd())) {
                pFocusWnd->SetFocus();
            }
            return;
        }
    }

    ii = 0, iimax = m_nRows;
    while ((ii++ < iimax) && rectCell.bottom > rectWindow.bottom && rectCell.top > GetFixedRowHeight() && IsVisibleVScroll()) {
        SendMessage(WM_VSCROLL, SB_LINEDOWN, 0);
        if (!GetCellRect(nRow, nCol, rectCell)) {
            if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd())) {
                pFocusWnd->SetFocus();
            }
            return;
        }
    }

    // restore focus to whoever owned it
    if (pFocusWnd && ::IsWindow(pFocusWnd->GetSafeHwnd())) {
        pFocusWnd->SetFocus();
    }
}

BOOL CDatasheet::IsCellEditable(CCellID &tCell)
{
    return IsCellEditable(tCell.row, tCell.col);
}

BOOL CDatasheet::IsCellEditable(int_t nRow, int_t nCol)
{
    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return FALSE;
    }

    byte_t bMask = pDoc->GetColumnItemMask(m_nGroup, nCol - 1, nRow - 1, 0);

    if ((bMask & 0x0F) != 0x0F) {
        if ((bMask & 0x0F) == 0x01) {
            return FALSE;
        }
    }

    return IsEditable() && ((GetItemState(nRow, nCol) & DVIS_READONLY) != DVIS_READONLY);
}

BOOL CDatasheet::IsCellSelected(CCellID &tCell)
{
    return IsCellSelected(tCell.row, tCell.col);
}

BOOL CDatasheet::IsCellSelected(int_t nRow, int_t nCol)
{
    if (!IsSelectable()) {
        return FALSE;
    }

    try {
        if (m_SelectedCellMap.empty()) {
            return FALSE;
        }

        CCellID tCell;
        DWORD key = MAKELONG(nRow, nCol);
        std::map<DWORD, CCellID>::iterator itr = m_SelectedCellMap.find(key);
        return (itr != m_SelectedCellMap.end());
    }
    catch (...) {
        return FALSE;
    }

}

BOOL CDatasheet::IsColumnSelected(int_t nCol)
{
    int_t ii, nRowCount = GetRowCount(), nColumnCount = GetColumnCount();
    if (nRowCount < 1) {
        return FALSE;
    }
    if ((nCol < 0) || (nCol >= nColumnCount)) {
        return FALSE;
    }

    int_t nCount = (nRowCount < SIGMA_DELTAITER) ? nRowCount : SIGMA_DELTAITER;
    for (ii = 1; ii < nCount; ii++) {
        if (IsCellSelected(ii, nCol) == FALSE) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CDatasheet::IsRowSelected(int_t nRow)
{
    int_t jj, nRowCount = GetRowCount(), nColumnCount = GetColumnCount();
    if (nColumnCount < 1) {
        return FALSE;
    }
    if ((nRow < 0) || (nRow >= nRowCount)) {
        return FALSE;
    }

    int_t nCount = (nColumnCount < SIGMA_DELTAITER) ? nColumnCount : SIGMA_DELTAITER;
    for (jj = 1; jj < nCount; jj++) {
        if (IsCellSelected(nRow, jj) == FALSE) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CDatasheet::IsCellVisible(CCellID tCell) 
{
    return IsCellVisible(tCell.row, tCell.col);
}

BOOL CDatasheet::IsCellVisible(int_t nRow, int_t nCol)
{
    int_t x, y;

    CCellID TopLeft;
    if (nCol >= GetFixedColumnCount() || nRow >= GetFixedRowCount()) {
        TopLeft = GetTopleftNonFixedCell();
        if (nCol >= GetFixedColumnCount() && nCol < TopLeft.col) {
            return FALSE;
        }
        if (nRow >= GetFixedRowCount() && nRow < TopLeft.row) {
            return FALSE;
        }
    }

    CRect rcT;
    GetClientRect(rcT);
    if (nCol < GetFixedColumnCount()) {
        x = 0;
        for (int_t i = 0; i <= nCol; i++) {
            if (x >= rcT.right) {
                return FALSE;
            }
            x += GetColumnWidth(i);
        }
    } 
    else {
        x = GetFixedColumnWidth();
        for (int_t i = TopLeft.col; i <= nCol; i++) {
            if (x >= rcT.right) {
                return FALSE;
            }
            x += GetColumnWidth(i);    
        }
    }

    if (nRow < GetFixedRowCount()) {
        y = 0;
        for (int_t i = 0; i <= nRow; i++) {
            if (y >= rcT.bottom) {
                return FALSE;
            }
            y += GetRowHeight(i);
        }
    } 
    else {
        if (nRow < TopLeft.row) {
            return FALSE;
        }
        y = GetFixedRowHeight();
        for (int_t i = TopLeft.row; i <= nRow; i++) {
            if (y >= rcT.bottom) {
                return FALSE;
            }
            y += GetRowHeight(i);
        }
    }

    return TRUE;
}

BOOL CDatasheet::InvalidateCellRect(const CCellID& tCell)
{
    return InvalidateCellRect(tCell.row, tCell.col);
}

BOOL CDatasheet::InvalidateCellRect(const int_t row, const int_t col)
{
    if (!isValid(row, col)) {
        return FALSE;
    }

    if (!IsCellVisible(row, col)) {
        return FALSE;
    }

    CRect rcT;
    if (!GetCellRect(row, col, rcT)) {
        return FALSE;
    }
    rcT.right++;
    rcT.bottom++;
    InvalidateRect(rcT, TRUE);

    return TRUE;
}

BOOL CDatasheet::InvalidateCellRect(const CCellRange& cellRange)
{
    ASSERT(isValid(cellRange));

    CCellRange visibleCellRange = GetVisibleNonFixedCellRange().Intersect(cellRange);

    CRect rcT;
    if (!GetCellRangeRect(visibleCellRange, rcT)) {
        return FALSE;
    }

    rcT.right++;
    rcT.bottom++;
    InvalidateRect(rcT, TRUE);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl Mouse stuff

// Handles mouse wheel notifications
BOOL CDatasheet::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // A m_nRowsPerWheelNotch value less than 0 indicates that the mouse
    // wheel scrolls whole pages, not just lines.
    if (m_nRowsPerWheelNotch == -1) {
        int_t nPagesScrolled = zDelta / 120;

        if (nPagesScrolled > 0) {
            for (int_t i = 0; i < nPagesScrolled; i++) {
                PostMessage(WM_VSCROLL, SB_PAGEUP, 0);
            }
        }
        else {
            for (int_t i = 0; i > nPagesScrolled; i--) {
                PostMessage(WM_VSCROLL, SB_PAGEDOWN, 0);
            }
        }
    }
    else {
        int_t nRowsScrolled = m_nRowsPerWheelNotch * zDelta / 120;

        if (nRowsScrolled > 0) {
            for (int_t i = 0; i < nRowsScrolled; i++) {
                PostMessage(WM_VSCROLL, SB_LINEUP, 0);
            }
        }
        else {
            for (int_t i = 0; i > nRowsScrolled; i--) {
                PostMessage(WM_VSCROLL, SB_LINEDOWN, 0);
            }
        }
    }

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CDatasheet::OnMouseMove(UINT /*nFlags*/, CPoint tPoint)
{
    CRect rcT;
    GetClientRect(rcT);

    // when mouse leaves the window and releases left button outside, m_bLMouseButtonDown is still set to TRUE
    if (m_bLMouseButtonDown) {
        m_bLMouseButtonDown = ((::GetKeyState(VK_LBUTTON) & 0x80) != 0) ? TRUE : FALSE;
    }

    // Sometimes a MOUSEMOVE message can come after the left buttons
    // has been let go, but before the BUTTONUP message has been processed.
    // We'll keep track of mouse buttons manually to avoid this.
    // All bMouseButtonDown's have been replaced with the member m_bLMouseButtonDown
    // BOOL bMouseButtonDown = ((nFlags & MK_LBUTTON) == MK_LBUTTON);

    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!m_bLMouseButtonDown || (m_bLMouseButtonDown && (m_MouseMode == MOUSE_NOTHING))) {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(tPoint)) {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE) {
                SetCursor(SigmaApp.LoadStandardCursor(IDC_SIZEWE));
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(tPoint)) {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE) {
                SetCursor(SigmaApp.LoadStandardCursor(IDC_SIZENS));
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        else if (m_MouseMode != MOUSE_NOTHING) {
            SetCursor(SigmaApp.LoadStandardCursor(IDC_ARROW));
            m_MouseMode = MOUSE_NOTHING;
        }

        if (m_MouseMode == MOUSE_NOTHING) {
            CCell* pCell = NULL;
            CCellID idCurrentCell;
        }

        m_LastMousePoint = tPoint;
        return;
    }

    if (!isValid(m_LeftClickDownCell)) {
        m_LastMousePoint = tPoint;
        return;
    }

    // If the left mouse button is down, then process appropriately
    if (m_bLMouseButtonDown) {
        switch (m_MouseMode) {
            case MOUSE_SELECT_ALL:
                break;

            case MOUSE_SELECT_COL:
            case MOUSE_SELECT_ROW:
            case MOUSE_SELECT_CELLS:
                {
                    CCellID idCurrentCell = GetCellFromPt(tPoint);
                    if (!isValid(idCurrentCell)) {
                        return;
                    }

                    if (idCurrentCell != GetFocusCell()) {
                        OnSelecting(idCurrentCell);
                        EnsureVisible(idCurrentCell.row, idCurrentCell.col);
                        // BUGFIX - Keep the appropriate cell row and/or
                        // column focused.  A fix in SetFocusCell() will place
                        // the cursor in a non-fixed tCell as needed.
                        if((idCurrentCell.row >= m_nFixedRows && idCurrentCell.col >= m_nFixedCols)
                            /*|| m_MouseMode == MOUSE_SELECT_COL*/ || m_MouseMode == MOUSE_SELECT_ROW) {
                                SetFocusCell(idCurrentCell);
                        }
                    }
                    break;
                }

            case MOUSE_SIZING_COL:
                {
                    CDC* pDC = GetDC();
                    if (pDC == NULL) {
                        break;
                    }
                    int nHeight = rcT.bottom - rcT.top;
                    CRect rectCell;
                    GetCellRect(GetRowCount() - 1, 0, rectCell);
                    if (rectCell.bottom < rcT.bottom) {
                        nHeight = rectCell.bottom - rcT.top;
                    }
                    CRect oldInvertedRect(m_LastMousePoint.x, rcT.top, m_LastMousePoint.x + 2, rcT.top + nHeight);
                    pDC->InvertRect(&oldInvertedRect);
                    CRect newInvertedRect(tPoint.x, rcT.top, tPoint.x + 2, rcT.top + nHeight);
                    pDC->InvertRect(&newInvertedRect);
                    ReleaseDC(pDC); pDC = NULL;
                }
                break;

            case MOUSE_SIZING_ROW:
                {
                    CDC* pDC = GetDC();
                    if (pDC == NULL) {
                        break;
                    }
                    int nWidth = rcT.right - rcT.left;
                    CRect rectCell;
                    GetCellRect(0, GetColumnCount() - 1, rectCell);
                    if (rectCell.right < rcT.right) {
                        nWidth = rectCell.right - rcT.left;
                    }
                    CRect oldInvertedRect(rcT.left, m_LastMousePoint.y, rcT.left + nWidth, m_LastMousePoint.y + 2);
                    pDC->InvertRect(&oldInvertedRect);
                    CRect newInvertedRect(rcT.left, tPoint.y, rcT.left + nWidth, tPoint.y + 2);
                    pDC->InvertRect(&newInvertedRect);
                    ReleaseDC(pDC); pDC = NULL;
                }
                break;
        }
    }

    m_LastMousePoint = tPoint;
}

// Returns the tPoint inside the tCell that was clicked (coords relative to tCell top left)
CPoint CDatasheet::GetPointClicked(int_t nRow, int_t nCol, const CPoint& tPoint)
{
    CPoint PointCellOrigin;
    if (!GetCellOrigin(nRow, nCol, &PointCellOrigin)) {
        return CPoint(0,0);
    }

    CPoint PointClickedCellRelative( tPoint);
    PointClickedCellRelative -= PointCellOrigin;
    return PointClickedCellRelative;
}

void CDatasheet::OnLButtonDblClk(UINT nFlags, CPoint tPoint)
{
    CCellID tCell = GetCellFromPt(tPoint);
    if (!isValid(tCell)) {
        return;
    }

    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) {
        CPoint startPoint;
        if (!GetCellOrigin(0, tCell.col, &startPoint)) {
            return;
        }

        if ((tPoint.x - startPoint.x) < m_nResizeCaptureRange) {        // Clicked right of border
            tCell.col--;
        }

        // ignore columns that are hidden and look left towards first visible column
        BOOL bFoundVisible = FALSE;
        while (tCell.col >= 0) {
            if (GetColumnWidth(tCell.col) > 0) {
                bFoundVisible = TRUE;
                break;
            }
            tCell.col--;
        }
        if (!bFoundVisible) {
            return;
        }

        AutoSizeColumn(tCell.col, GetAutoSizeStyle());
        Refresh();
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) {
        CPoint startPoint;
        if (!GetCellOrigin(0, tCell.col, &startPoint)) {
            return;
        }

        if ((tPoint.y - startPoint.y) < m_nResizeCaptureRange) {    // Clicked below border
            tCell.row--;
        }

        //  ignore rows that are hidden and look up towards first visible row
        BOOL bFoundVisible = FALSE;
        while (tCell.row >= 0) {
            if (GetRowHeight (tCell.row) > 0) {
                bFoundVisible = TRUE;
                break;
            }
            tCell.row--;
        }
        if (!bFoundVisible) {
            return;
        }

        AutoSizeRow(tCell.row);
        Refresh();
    }
    else if (m_MouseMode == MOUSE_NOTHING) {
        CPoint pointClickedRel;
        pointClickedRel = GetPointClicked(tCell.row, tCell.col, tPoint);

        CCell xCell;
        CELL_DISPINFO xDispInfo;
        CCell* pCell = NULL;
        if (isValid(tCell)) {
            pCell = (CCell*) GetCell(tCell.row, tCell.col, &xCell, &xDispInfo);
        }

        // Clicked in the text area? Only then will tCell selection work
        BOOL bInTextArea = FALSE;
        if (pCell) {
            CRect rectCell;
            if (GetCellRect(tCell.row, tCell.col, rectCell) && pCell->GetTextRect(rectCell)) {
                bInTextArea = rectCell.PtInRect(tPoint);
            }
        }

        if (tCell.row >= m_nFixedRows && isValid(m_LeftClickDownCell) && tCell.col >= m_nFixedCols && bInTextArea) {
            OnEditCell(tCell.row, tCell.col, pointClickedRel, VK_LBUTTON);
        }
        else if ((tCell.row < m_nFixedRows) && (tCell.col >= m_nFixedCols)) {
            SelectColumns(tCell);
            OnColumnProperties();
        }
    }

    CWnd::OnLButtonDblClk(nFlags, tPoint);
}

void CDatasheet::OnLButtonDown(UINT nFlags, CPoint tPoint)
{
    SetFocus();
    m_bLMouseButtonDown = TRUE;

    m_LeftClickDownPoint = tPoint;
    m_LeftClickDownCell  = GetCellFromPt(tPoint);
    if (!isValid(m_LeftClickDownCell)) {
        return;
    }

    if ((nFlags & MK_SHIFT) != MK_SHIFT) {
        m_SelectionStartCell = m_LeftClickDownCell;
    }
    else {
        if (!isValid(m_SelectionStartCell)) {
            m_SelectionStartCell = m_idCurrentCell;
        }
    }

    EndEdit();

    // Clicked in the text area? Only then will cell selection work
    BOOL bInTextArea = FALSE;
    CRect rectCell;
    if (GetCellRect(m_LeftClickDownCell.row, m_LeftClickDownCell.col, rectCell)) {
        bInTextArea = rectCell.PtInRect(tPoint);
    }

    // If the user clicks on the current tCell, then prepare to edit it.
    // (If the user moves the mouse, then dragging occurs)
    if ((m_LeftClickDownCell.row >= m_nFixedRows && isValid(m_LeftClickDownCell) && m_LeftClickDownCell.col >= m_nFixedCols && bInTextArea)
        && m_LeftClickDownCell == m_idCurrentCell && !(nFlags & MK_CONTROL) && bInTextArea && IsCellEditable(m_LeftClickDownCell)) {
        m_MouseMode = MOUSE_PREPARE_EDIT;
        return;
    }

    else if (IsCellSelected(m_LeftClickDownCell)) {
        SetFocusCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);

        // If control is pressed then unselect the tCell or row (depending on the list mode)
        if (nFlags & MK_CONTROL) {
            SetFocusCell(m_LeftClickDownCell);
            SelectCells(m_LeftClickDownCell, FALSE, nFlags);
            return;
        }
    }

    else if ((m_MouseMode != MOUSE_OVER_COL_DIVIDE) && (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)) {
        if (m_LeftClickDownCell.row >= GetFixedRowCount() && m_LeftClickDownCell.col >= GetFixedColumnCount()) {
            SetFocusCell(m_LeftClickDownCell.row, m_LeftClickDownCell.col);
        }
        else {
            SetFocusCell(-1,-1);
        }
    }

    if (m_MouseMode == MOUSE_NOTHING) {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(tPoint)) {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE) {
                SetCursor(SigmaApp.LoadStandardCursor(IDC_SIZEWE));
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
                SetCapture();
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(tPoint)) {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE) {
                SetCursor(SigmaApp.LoadStandardCursor(IDC_SIZENS));
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
                SetCapture();
            }
        }
    }

    if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) {                // sizing column

        m_MouseMode = MOUSE_SIZING_COL;

        CRect VisRect;
        GetVisibleNonFixedCellRange(VisRect);

        // Kludge for if we are over the last column...
        if (GetColumnWidth(GetColumnCount() - 1) < m_nResizeCaptureRange) {
            if (abs(tPoint.x - VisRect.right) < m_nResizeCaptureRange) {
                m_LeftClickDownCell.col = GetColumnCount() - 1;
            }
        }

        CPoint startPoint;
        if (!GetCellOrigin(0, m_LeftClickDownCell.col, &startPoint)) {
            return;
        }

        if (!m_bHiddenColUnhide) {
            // ignore columns that are hidden and look left towards first visible column
            BOOL bLookForVisible = TRUE;
            BOOL bIsCellRightBorder = tPoint.x - startPoint.x >= m_nResizeCaptureRange;

            if (bIsCellRightBorder && m_LeftClickDownCell.col + 1 >= GetColumnCount()) {
                // clicked on last column's right border
                // if last column is visible, don't do anything
                if (m_LeftClickDownCell.col >= 0) {
                    bLookForVisible = FALSE;
                }
            }

            if (bLookForVisible) {
                // clicked on column divider other than last right border
                BOOL bFoundVisible = FALSE;
                int_t iOffset = 1;

                if (bIsCellRightBorder) {
                    iOffset = 0;
                }

                while ((m_LeftClickDownCell.col - iOffset) >= 0) {
                    if (GetColumnWidth(m_LeftClickDownCell.col - iOffset) > 0) {
                        bFoundVisible = TRUE;
                        break;
                    }
                    m_LeftClickDownCell.col--;
                }
                if (!bFoundVisible) {
                    return;
                }
            }
        }

        CRect rcT;
        GetClientRect(rcT);
        int nHeight = rcT.bottom - rcT.top;
        CRect rectCell;
        GetCellRect(GetRowCount() - 1, 0, rectCell);
        if (rectCell.bottom < rcT.bottom) {
            nHeight = rectCell.bottom - rcT.top;
        }
        CRect invertedRect(tPoint.x, rcT.top, tPoint.x + 2, rcT.top + nHeight);

        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC); pDC = NULL;
        }

        if ((tPoint.x - startPoint.x) < m_nResizeCaptureRange) {
            if (m_LeftClickDownCell.col < GetColumnCount()-1 || GetColumnWidth(GetColumnCount()-1) >= m_nResizeCaptureRange) {
                if (!GetCellOrigin(0, --m_LeftClickDownCell.col, &startPoint)) {
                    return;
                }
            }
        }

        // Allow a tCell resize width no greater than that which can be viewed within
        // the grid itself
        int_t nMaxCellWidth = rcT.Width()-GetFixedColumnWidth();
        rcT.left = startPoint.x + 1;
        rcT.right = rcT.left + nMaxCellWidth;

        ClientToScreen(rcT);
        ClipCursor(rcT);
    }
    else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) {            // sizing row

        m_MouseMode = MOUSE_SIZING_ROW;

        // Kludge for if we are over the last column...
        if (GetRowHeight(GetRowCount() - 1) < m_nResizeCaptureRange) {
            CRect VisRect;
            GetVisibleNonFixedCellRange(VisRect);
            if (abs(tPoint.y - VisRect.bottom) < m_nResizeCaptureRange) {
                m_LeftClickDownCell.row = GetRowCount()-1;
            }
        }

        CPoint startPoint;
        if (!GetCellOrigin(m_LeftClickDownCell, &startPoint)) {
            return;
        }

        if (!m_bHiddenRowUnhide) {
            //  ignore rows that are hidden and look up towards first visible row
            BOOL bLookForVisible = TRUE;
            BOOL bIsCellBottomBorder = (tPoint.y - startPoint.y) >= m_nResizeCaptureRange;

            if (bIsCellBottomBorder && ((m_LeftClickDownCell.row + 1) >= GetRowCount())) {
                // clicked on last row's bottom border

                // if last row is visible, don't do anything
                if (m_LeftClickDownCell.row >= 0) {
                    bLookForVisible = FALSE;
                }
            }

            if (bLookForVisible) {
                // clicked on row divider other than last bottom border
                BOOL bFoundVisible = FALSE;
                int_t iOffset = 1;

                if (bIsCellBottomBorder) {
                    iOffset = 0;
                }

                while ((m_LeftClickDownCell.row - iOffset) >= 0) {
                    if (GetRowHeight(m_LeftClickDownCell.row - iOffset) > 0) {
                        bFoundVisible = TRUE;
                        break;
                    }
                    m_LeftClickDownCell.row--;
                }
                if (!bFoundVisible) {
                    return;
                }
            }
        }

        CRect rcT;
        GetClientRect(rcT);
        int nWidth = rcT.right - rcT.left;
        CRect rectCell;
        GetCellRect(0, GetColumnCount() - 1, rectCell);
        if (rectCell.right < rcT.right) {
            nWidth = rectCell.right - rcT.left;
        }
        CRect invertedRect(rcT.left, tPoint.y, rcT.left + nWidth, tPoint.y + 2);

        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC); pDC = NULL;
        }

        // If we clicked below the row divide, then reset the click-down tCell
        // as the tCell above the row divide - UNLESS we clicked on the last row
        // and the last row is teensy (kludge fix)
        if (tPoint.y - startPoint.y < m_nResizeCaptureRange) {            // clicked below border
            if ((m_LeftClickDownCell.row < (GetRowCount() - 1)) || (GetRowHeight(GetRowCount() - 1) >= m_nResizeCaptureRange)) {
                if (!GetCellOrigin(--m_LeftClickDownCell.row, 0, &startPoint)) {
                    return;
                }
            }
        }

        int_t nMaxCellHeight = rcT.Height() - GetFixedRowHeight();
        rcT.top = startPoint.y + 1;
        rcT.bottom = rcT.top + nMaxCellHeight;

        ClientToScreen(rcT);

        ClipCursor(rcT);
    }
    else {
        // If Ctrl pressed, save the current tCell selection. This will get added
        // to the new tCell selection at the end of the tCell selection process

        try {

            m_PrevSelectedCellMap.clear();

            if (nFlags & MK_CONTROL) {

                std::map<DWORD, CCellID>::iterator itr;

                // >> Partie du code lente - SLOW
                DWORD key;
                CCellID tCell;

                long ii = 0;
                long iMax = (GetRowCount() - 1) * (GetColumnCount() - 1);
                long iSize = m_SelectedCellMap.size();
                if (iSize > iMax) { iSize = iMax; }
                if (iSize >= 1) {
                    // Unselect all previously selected cells
                    for (itr = m_SelectedCellMap.begin(), ii = 0; (itr != m_SelectedCellMap.end()) && (ii < iSize); ++itr, ii++) {
                        key = itr->first;
                        tCell = itr->second;
                        m_PrevSelectedCellMap.insert(std::pair<DWORD, CCellID>(key, tCell));
                    }
                }

                m_SelectedCellMap.clear();
            }

        }
        catch (...) {

        }

        if (m_LeftClickDownCell.row < GetFixedRowCount()) {
            OnFixedRowClick(m_LeftClickDownCell);
        }
        else if (m_LeftClickDownCell.col < GetFixedColumnCount()) {
            OnFixedColumnClick(m_LeftClickDownCell);
        }
        else {
            m_MouseMode = MOUSE_SELECT_CELLS;
            OnSelecting(m_LeftClickDownCell);
        }
    }

    m_LastMousePoint = tPoint;
}

void CDatasheet::OnLButtonUp(UINT nFlags, CPoint tPoint)
{
    CWnd::OnLButtonUp(nFlags, tPoint);

    m_bLMouseButtonDown = FALSE;

    ClipCursor(NULL);

    if (GetCapture()->GetSafeHwnd() == GetSafeHwnd()) {
        ReleaseCapture();
    }

    CPoint pointClickedRel;
    pointClickedRel = GetPointClicked(m_idCurrentCell.row, m_idCurrentCell.col, tPoint);

    // m_MouseMode == MOUSE_PREPARE_EDIT only if user clicked down on current tCell
    // and then didn't move mouse before clicking up (releasing button)
    if (m_MouseMode == MOUSE_PREPARE_EDIT) {
        OnEditCell(m_idCurrentCell.row, m_idCurrentCell.col, pointClickedRel, VK_LBUTTON);
    }

    else if (m_MouseMode == MOUSE_SIZING_COL) {
        CRect rcT;
        GetClientRect(rcT);
        int nHeight = rcT.bottom - rcT.top;
        CRect rectCell;
        GetCellRect(GetRowCount() - 1, 0, rectCell);
        if (rectCell.bottom < rcT.bottom) {
            nHeight = rectCell.bottom - rcT.top;
        }
        CRect invertedRect(m_LastMousePoint.x, rcT.top, m_LastMousePoint.x + 2, rcT.top + nHeight);

        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC); pDC = NULL;
        }

        if (m_LeftClickDownPoint != tPoint && (tPoint.x != 0 || tPoint.y != 0)) {
            CPoint startPoint;
            if (!GetCellOrigin(m_LeftClickDownCell, &startPoint)) {
                return;
            }
            int_t nColumnWidth = __max(tPoint.x - startPoint.x, m_bAllowColHide ? 0 : 1);

            SetColumnWidth(m_LeftClickDownCell.col, nColumnWidth);
            ResetScrollBars();
            Refresh();
        }
    }
    else if (m_MouseMode == MOUSE_SIZING_ROW) {
        CRect rcT;
        GetClientRect(rcT);
        int nWidth = rcT.right - rcT.left;
        CRect rectCell;
        GetCellRect(0, GetColumnCount() - 1, rectCell);
        if (rectCell.right < rcT.right) {
            nWidth = rectCell.right - rcT.left;
        }
        CRect invertedRect(rcT.left, m_LastMousePoint.y, rcT.left + nWidth, m_LastMousePoint.y + 2);

        CDC* pDC = GetDC();
        if (pDC) {
            pDC->InvertRect(&invertedRect);
            ReleaseDC(pDC); pDC = NULL;
        }

        if ((m_LeftClickDownPoint != tPoint)  && ((tPoint.x != 0) || (tPoint.y != 0))) {
            CPoint startPoint;
            if (!GetCellOrigin(m_LeftClickDownCell, &startPoint)) {
                return;
            }
            int_t nRowHeight = __max(tPoint.y - startPoint.y, m_bAllowRowHide ? 0 : 1);

            SetRowHeight(m_LeftClickDownCell.row, nRowHeight);
            ResetScrollBars();
            Refresh();
        }
    }

    m_MouseMode = MOUSE_NOTHING;

    SetCursor(SigmaApp.LoadStandardCursor(IDC_ARROW));

    if (!isValid(m_LeftClickDownCell)) {
        ResetSelectedRange();
        return;
    }
}

void CDatasheet::OnRButtonDown(UINT nFlags, CPoint tPoint)
{
    CWnd::OnRButtonDown(nFlags, tPoint);
    m_bRMouseButtonDown = TRUE;
}

// Added to forward right click to parent so that a context
// menu can be shown without deriving a new grid class.
void CDatasheet::OnRButtonUp(UINT nFlags, CPoint tPoint)
{
    m_bRMouseButtonDown = FALSE;

    CCellID FocusCell;
    FocusCell = GetCellFromPt(tPoint);

    EndEdit();                        // Auto-destroy any InEdit's

    // If not a valid tCell, pass -1 for row and column
    if (!isValid(FocusCell) || ((FocusCell.col == 0) && (FocusCell.row == 0))) {
        ResetSelectedRange();

        if ((FocusCell.col == 0) && (FocusCell.row == 0)) {
            CWnd::OnRButtonUp(nFlags, tPoint);
            return;
        }

        CMenu contextMenu; 
        CMenu *pMenu = NULL;
        if (contextMenu.LoadMenu(IDM_DATAPOP_MAIN) == TRUE) {
            pMenu = contextMenu.GetSubMenu(0);
        }
        if (pMenu != NULL) {
            ClientToScreen(&tPoint);
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, this, NULL);
        }
        CWnd::OnRButtonUp(nFlags, tPoint);
    }
    else {
        int_t iFixedRowCount = GetFixedRowCount(), iFixedColumnCount = GetFixedColumnCount();

        CCellRange cellRange = GetSelectedCellRange();
        int_t nMinCol = cellRange.GetMinCol(), nMaxCol = cellRange.GetMaxCol();

        // display ad hoc popmenu
        CMenu contextMenu; 
        CMenu *pMenu = NULL;
        if (FocusCell.row < GetFixedRowCount()) {
            if (IsColumnSelected(FocusCell.col) == FALSE) {
                ResetSelectedRange();
                SetSelectedRange(iFixedRowCount, FocusCell.col, GetRowCount() - 1, FocusCell.col);
            }
            SetFocusCell(FocusCell);
            Refresh();
            if (contextMenu.LoadMenu((IsEditable() == TRUE) ? IDM_DATAPOP_COLUMN : IDM_DATAPOP_COLUMN_RO) == TRUE) {
                pMenu = contextMenu.GetSubMenu(0);
            }
        }
        else if (FocusCell.col < GetFixedColumnCount()) {
            if (IsRowSelected(FocusCell.row) == FALSE) {
                ResetSelectedRange();
                SetSelectedRange(FocusCell.row, iFixedColumnCount, FocusCell.row, GetColumnCount() - 1);
            }
            SetFocusCell(FocusCell);
            Refresh();
            if (contextMenu.LoadMenu((IsEditable() == TRUE) ? IDM_DATAPOP_ROW : IDM_DATAPOP_ROW_RO) == TRUE) {
                pMenu = contextMenu.GetSubMenu(0);
            }
        }
        else {
            if (IsCellSelected(FocusCell.row, FocusCell.col) == FALSE) {
                ResetSelectedRange();
                SetSelectedRange(FocusCell.row, FocusCell.col, FocusCell.row, FocusCell.col);
            }
            SetFocusCell(FocusCell);
            Refresh();
            if (contextMenu.LoadMenu((IsEditable() == TRUE) ? IDM_DATAPOP_CELL : IDM_DATAPOP_CELL_RO) == TRUE) {
                pMenu = contextMenu.GetSubMenu(0);
            }
        }
        //<<

        if (pMenu != NULL) {
            ClientToScreen(&tPoint);
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, tPoint.x, tPoint.y, this, NULL);
        }
        CWnd::OnRButtonUp(nFlags, tPoint);
    }
}

// New print margin support functions
void CDatasheet::SetPrintMarginInfo(int_t nHeaderHeight, int_t nFooterHeight, int_t nLeftMargin, int_t nRightMargin,
                                                int_t nTopMargin, int_t nBottomMargin, int_t nGap)
{
    // If any parameter is -1, keep the existing setting
    if (nHeaderHeight > -1) { m_nHeaderHeight = nHeaderHeight; }
    if (nFooterHeight > -1) { m_nFooterHeight = nFooterHeight; }
    if (nLeftMargin > -1) { m_nLeftMargin = nLeftMargin; }
    if (nRightMargin > -1) { m_nRightMargin = nRightMargin; }
    if (nTopMargin > -1) { m_nTopMargin = nTopMargin; }
    if (nBottomMargin > -1) { m_nBottomMargin = nBottomMargin; }
    if (nGap > -1) { m_nGap = nGap; }
}

void CDatasheet::GetPrintMarginInfo(int_t &nHeaderHeight, int_t &nFooterHeight, int_t &nLeftMargin, int_t &nRightMargin,
                                                int_t &nTopMargin, int_t &nBottomMargin, int_t &nGap)
{
    nHeaderHeight = m_nHeaderHeight;
    nFooterHeight = m_nFooterHeight;
    nLeftMargin = m_nLeftMargin;
    nRightMargin = m_nRightMargin;
    nTopMargin = m_nTopMargin;
    nBottomMargin = m_nBottomMargin;
    nGap = m_nGap;
}

void CDatasheet::Print(CPrintDialog* pPrintDialog /*=NULL*/)
{
    CDC dcT;

    if (pPrintDialog == NULL) {
        CPrintDialog printDlg(FALSE);
        if (printDlg.DoModal() != IDOK) {                    // Get printer settings from user
            return;
        }

        dcT.Attach(printDlg.GetPrinterDC());                    // attach a printer DC
    }
    else {
        dcT.Attach(pPrintDialog->GetPrinterDC());        // attach a printer DC
    }

    dcT.m_bPrinting = TRUE;

    CString strTitle;
    strTitle.LoadString(AFX_IDS_APP_TITLE);

    if (strTitle.IsEmpty()) {
        CWnd *pParentWnd = GetParent();
        while (pParentWnd) {
            pParentWnd->GetWindowText(strTitle);
            if (strTitle.GetLength()) {                    // can happen if it is a CWnd, CChildFrm has the title
                break;
            }
            pParentWnd = pParentWnd->GetParent();
        }
    }

    DOCINFO di;                                                    // Initialise print doc details
    memset(&di, 0, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;

    BOOL bPrintingOK = dcT.StartDoc(&di);                // Begin a new print job

    CPrintInfo Info;
    Info.m_rectDraw.SetRect(0,0, dcT.GetDeviceCaps(HORZRES), dcT.GetDeviceCaps(VERTRES));

    OnBeginPrinting(&dcT, &Info);                            // Initialise printing
    for (UINT page = Info.GetMinPage(); page <= Info.GetMaxPage() && bPrintingOK; page++) {
        dcT.StartPage();                                        // begin new page
        Info.m_nCurPage = page;
        OnPrint(&dcT, &Info);                                // Print page
        bPrintingOK = (dcT.EndPage() > 0);                // end page
    }
    OnEndPrinting(&dcT, &Info);                            // Clean up after printing

    if (bPrintingOK) {
        dcT.EndDoc();                                            // end a print job
    }
    else {
        dcT.AbortDoc();                                        // abort job.
    }

    dcT.Detach();                                                // detach the printer DC
}

// Various changes in the tNextCell few functions to support the
// new print margins and a few other adjustments.
void CDatasheet::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo)
{
    // OnBeginPrinting() is called after the user has committed to
    // printing by OK'ing the Print dialog, and after the framework
    // has created a CDC object for the printer or the preview view.

    // This is the right opportunity to set up the page range.
    // Given the CDC object, we can determine how many rows will
    // fit on a page, so we can in turn determine how many printed
    // pages represent the entire document.

    ASSERT(pDC && pInfo);
    if ((pDC == NULL) || (pInfo == NULL)) { return; }

    // Get a DC for the current window (will be a screen DC for print previewing)
    CDC *pCurrentDC = GetDC();                    // will have dimensions of the client area
    if (pCurrentDC == NULL) { return; }

    CSize PaperPixelsPerInch(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
    CSize ScreenPixelsPerInch(pCurrentDC->GetDeviceCaps(LOGPIXELSX), pCurrentDC->GetDeviceCaps(LOGPIXELSY));
    if ((PaperPixelsPerInch.cx == 0) || (PaperPixelsPerInch.cy == 0)
        || (ScreenPixelsPerInch.cx == 0) || (ScreenPixelsPerInch.cy == 0)) {

        ReleaseDC(pCurrentDC);
        pCurrentDC = NULL;
        return;
    }

    // Create the printer font
    m_PrinterFont.CreateFont(-MulDiv(7, ::GetDeviceCaps(pCurrentDC->GetSafeHdc(), LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
        OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Times New Roman"));

    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Get the average character width (in Datasheet units) and hence the margins
    m_CharSize = pDC->GetTextExtent(_T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSATUVWXYZ"), 52);
    m_CharSize.cx /= 52;
    int_t nMargins = (m_nLeftMargin + m_nRightMargin) * m_CharSize.cx;

    // Get the page sizes (physical and logical)
    m_PaperSize = CSize(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

    if (m_bWysiwygPrinting) {
        m_LogicalPageSize.cx = ScreenPixelsPerInch.cx * m_PaperSize.cx / PaperPixelsPerInch.cx * 3 / 4;
        m_LogicalPageSize.cy = ScreenPixelsPerInch.cy * m_PaperSize.cy / PaperPixelsPerInch.cy * 3 / 4;
    }
    else {
        m_LogicalPageSize.cx = GetVirtualWidth() + nMargins;
        m_LogicalPageSize.cy = MulDiv(m_LogicalPageSize.cx, m_PaperSize.cy, m_PaperSize.cx);
    }

    m_nPageHeight = m_LogicalPageSize.cy - GetFixedRowHeight() - (m_nHeaderHeight + m_nFooterHeight + (2 * m_nGap)) * m_CharSize.cy;

    // Get the number of pages. Assumes no row is bigger than the page tSize.
    int_t nTotalRowHeight = 0;
    m_nNumPages = 1;
    for (int_t row = GetFixedRowCount(); row < GetRowCount(); row++) {
        nTotalRowHeight += GetRowHeight(row);
        if (nTotalRowHeight > m_nPageHeight) {
            m_nNumPages++;
            nTotalRowHeight = GetRowHeight(row);
        }
    }

    // now, figure out how many additional pages must print out if rows ARE bigger
    //  than page tSize
    int_t iColumnOffset = 0;
    int_t ii;
    for (ii = 0; ii < GetFixedColumnCount(); ii++) {
        iColumnOffset += GetColumnWidth(ii);
    }
    m_nPageWidth = m_LogicalPageSize.cx - iColumnOffset - nMargins;
    m_nPageMultiplier = 1;

    if (m_bWysiwygPrinting) {
        int_t iTotalRowWidth = 0;
        for (ii = GetFixedColumnCount(); ii < GetColumnCount(); ii++) {
            iTotalRowWidth += GetColumnWidth(ii);
            if (iTotalRowWidth > m_nPageWidth) {
                m_nPageMultiplier++;
                iTotalRowWidth = GetColumnWidth(ii);
            }
        }
        m_nNumPages *= m_nPageMultiplier;
    }

    // Set up the print info
    pInfo->SetMaxPage(m_nNumPages);
    pInfo->m_nCurPage = 1;                                        // start printing at page# 1

    ReleaseDC(pCurrentDC);
    pCurrentDC = NULL;
    pDC->SelectObject(pOldFont);
}

void CDatasheet::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
    if ((pDC == NULL) || (pInfo == NULL)) {
        return;
    }

    //CRect rcPage(pInfo->m_rectDraw);
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    // Set the page map mode to use Datasheet units, and setup margin
    int_t iMapMode = pDC->SetMapMode(MM_ISOTROPIC);
    pDC->SetWindowExt(m_LogicalPageSize);
    pDC->SetViewportExt(m_PaperSize);
    pDC->SetWindowOrg(-m_nLeftMargin * m_CharSize.cx, 0);

    // Header
    pInfo->m_rectDraw.top = 0;
    pInfo->m_rectDraw.left = 0;
    pInfo->m_rectDraw.right = m_LogicalPageSize.cx - (m_nLeftMargin + m_nRightMargin) * m_CharSize.cx;
    pInfo->m_rectDraw.bottom = m_nHeaderHeight * m_CharSize.cy;
    PrintHeader(pDC, pInfo);
    pDC->OffsetWindowOrg(0, -m_nHeaderHeight * m_CharSize.cy);

    // Gap between header and column headings
    pDC->OffsetWindowOrg(0, -m_nGap * m_CharSize.cy);

    pDC->OffsetWindowOrg(0, -GetFixedRowHeight());

    // We need to find out which row to start printing for this page.
    int_t nTotalRowHeight = 0;
    UINT nNumPages = 1;
    m_nCurrPrintRow = GetFixedRowCount();


    // Not only the row, but we need to figure out column, too

    // Can print 4 pages, where page 1 and 2 represent the same rows but
    // with different WIDE columns.
    //
    //        1 .2 .    If representing page 3  -->        iPageIfIgnoredWideCols = 2
    // .......                                                    iWideColPageOffset = 0
    // .3 .4 .  If representing page 2  -->            iPageIfIgnoredWideCols = 1
    // .......                                                    iWideColPageOffset = 1

    int_t iPageIfIgnoredWideCols = pInfo->m_nCurPage / m_nPageMultiplier;
    int_t iWideColPageOffset = pInfo->m_nCurPage - (iPageIfIgnoredWideCols * m_nPageMultiplier);
    if (iWideColPageOffset > 0) {
        iPageIfIgnoredWideCols++;
    }

    if (iWideColPageOffset == 0) {
        iWideColPageOffset = m_nPageMultiplier;
    }
    iWideColPageOffset--;

    // calculate current print row based on iPageIfIgnoredWideCols
    while (m_nCurrPrintRow < GetRowCount() && (int_t)nNumPages < iPageIfIgnoredWideCols) {
        nTotalRowHeight += GetRowHeight(m_nCurrPrintRow);
        if (nTotalRowHeight > m_nPageHeight) {
            nNumPages++;
            if ((int_t)nNumPages == iPageIfIgnoredWideCols) { break; }
            nTotalRowHeight = GetRowHeight(m_nCurrPrintRow);
        }
        m_nCurrPrintRow++;
    }

    m_nPrintColumn = GetFixedColumnCount();
    int_t iTotalRowWidth = 0;
    int_t i1, i2;

    // now, calculate which print column to start displaying
    for (i1 = 0; i1 < iWideColPageOffset; i1++) {
        for (i2 = m_nPrintColumn; i2 < GetColumnCount(); i2++) {
            iTotalRowWidth += GetColumnWidth( i2);
            if (iTotalRowWidth > m_nPageWidth) {
                m_nPrintColumn = i2;
                iTotalRowWidth = 0;
                break;
            }
        }
    }

    PrintRowButtons(pDC, pInfo);   // print row buttons on each page
    int_t iColumnOffset = 0;
    for (i1 = 0; i1 < GetFixedColumnCount(); i1++) {
        iColumnOffset += GetColumnWidth( i1);
    }

    // Print the column headings
    pInfo->m_rectDraw.bottom = GetFixedRowHeight();

    if (m_nPrintColumn == GetFixedColumnCount()) {
        // have the column headings fcn draw the upper left fixed cells for the very first columns, only
        pDC->OffsetWindowOrg( 0, +GetFixedRowHeight());

        m_nPageWidth += iColumnOffset;
        m_nPrintColumn = 0;
        PrintColumnHeadings(pDC, pInfo);
        m_nPageWidth -= iColumnOffset;
        m_nPrintColumn = GetFixedColumnCount();

        pDC->OffsetWindowOrg(-iColumnOffset, -GetFixedRowHeight());
    }
    else {
        // changed all of this here to match above almost exactly same
        pDC->OffsetWindowOrg(0, +GetFixedRowHeight());

        m_nPageWidth += iColumnOffset;

        // print from column 0 ... last column that fits on the current page
        PrintColumnHeadings(pDC, pInfo);

        m_nPageWidth -= iColumnOffset;

        pDC->OffsetWindowOrg(-iColumnOffset, -GetFixedRowHeight());
    }

    if (m_nCurrPrintRow >= GetRowCount()) { return; }

    // Draw as many rows as will fit on the printed page.
    // Clip the printed page so that there is no partially shown
    // row at the bottom of the page (the same row which will be fully
    // shown at the top of the tNextCell page).

    BOOL bFirstPrintedRow = TRUE;
    CRect rcT;
    rcT.bottom = -1;
    while (m_nCurrPrintRow < GetRowCount()) {
        rcT.top = rcT.bottom + 1;
        rcT.bottom = rcT.top + GetRowHeight(m_nCurrPrintRow) - 1;

        if (rcT.bottom > m_nPageHeight) { break; }            // Gone past end of page

        rcT.right = -1;

        // modified to allow printing of wide grids on multiple pages
        for (int_t col = m_nPrintColumn; col < GetColumnCount(); col++) {
            rcT.left = rcT.right + 1;
            rcT.right = rcT.left + GetColumnWidth(col) - 1;
            if (rcT.right > m_nPageWidth) {
                break;
            }

            CCell xCell;
            CELL_DISPINFO xDispInfo;
            CCell* pCell = (CCell*)GetCell(m_nCurrPrintRow, col, &xCell, &xDispInfo);
            if (pCell) {
                pCell->PrintCell(pDC, m_nCurrPrintRow, col, rcT);
            }

            if (m_nGridLines == DVL_BOTH || m_nGridLines == DVL_HORZ) {
                int_t Overlap = (col == 0)? 0:1;
                pDC->MoveTo(rcT.left-Overlap, rcT.bottom);
                pDC->LineTo(rcT.right, rcT.bottom);
                if (m_nCurrPrintRow == 0) {
                    pDC->MoveTo(rcT.left-Overlap, rcT.top);
                    pDC->LineTo(rcT.right, rcT.top);
                }
            }
            if (m_nGridLines == DVL_BOTH || m_nGridLines == DVL_VERT) {
                int_t Overlap = (bFirstPrintedRow)? 0:1;
                pDC->MoveTo(rcT.right, rcT.top-Overlap);
                pDC->LineTo(rcT.right, rcT.bottom);
                if (col == 0) {
                    pDC->MoveTo(rcT.left, rcT.top-Overlap);
                    pDC->LineTo(rcT.left, rcT.bottom);
                }
            }

        }
        m_nCurrPrintRow++;
        bFirstPrintedRow = FALSE;
    }

    // Footer
    pInfo->m_rectDraw.bottom = m_nFooterHeight * m_CharSize.cy;
    pDC->SetWindowOrg(-m_nLeftMargin * m_CharSize.cx, -m_LogicalPageSize.cy + m_nFooterHeight * m_CharSize.cy);
    PrintFooter(pDC, pInfo);

    // SetWindowOrg back for tNextCell page
    pDC->SetWindowOrg(0,0);

    pDC->SetMapMode(iMapMode);
    pDC->SelectObject(pOldFont);
}

// added by M.Fletcher 12/17/00
void CDatasheet::PrintFixedRowCells(int_t nStartColumn, int_t nStopColumn, int_t& row, CRect& rcT, CDC *pDC, BOOL& bFirst)
{
    // print all cells from nStartColumn to nStopColumn on row
    for (int_t col =nStartColumn; col < nStopColumn; col++) {
        rcT.left = rcT.right+1;
        rcT.right = rcT.left + GetColumnWidth( col) - 1;

        if (rcT.right > m_nPageWidth) {
            break;
        }

        CCell xCell;
        CELL_DISPINFO xDispInfo;
        CCell* pCell = (CCell*)GetCell(row, col, &xCell, &xDispInfo);
        if (pCell) {
            pCell->PrintCell(pDC, row, col, rcT);
        }

        if (m_nGridLines == DVL_BOTH || m_nGridLines == DVL_HORZ) {
            int_t Overlap = (col == 0) ? 0 : 1;

            pDC->MoveTo(rcT.left-Overlap, rcT.bottom);
            pDC->LineTo(rcT.right, rcT.bottom);

            if (row == 0) {
                pDC->MoveTo(rcT.left-Overlap, rcT.top);
                pDC->LineTo(rcT.right, rcT.top);
            }
        }

        if ((m_nGridLines == DVL_BOTH) || (m_nGridLines == DVL_VERT)) {
            int_t Overlap = (row == 0) ? 0 : 1;

            pDC->MoveTo(rcT.right, rcT.top-Overlap);
            pDC->LineTo(rcT.right, rcT.bottom);

            if (bFirst) {
                pDC->MoveTo(rcT.left-1, rcT.top-Overlap);
                pDC->LineTo(rcT.left-1, rcT.bottom);
                bFirst = FALSE;
            }
        }
    }
}

void CDatasheet::PrintColumnHeadings(CDC *pDC, CPrintInfo* /*pInfo*/)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rcT;
    rcT.bottom = -1;

    BOOL bFirst = TRUE;
    BOOL bOriginal;


    // modified to allow column hdr printing of multi-page wide grids
    for (int_t row = 0; row < GetFixedRowCount(); row++) {
        rcT.top = rcT.bottom+1;
        rcT.bottom = rcT.top + GetRowHeight(row) - 1;

        rcT.right = -1;

        // if printColumn > fixedcolumncount we are on page 2 or more
        // lets printout those fixed cell headings again the 1 or more that would be missed
        // added by M.Fletcher 12/17/00
        if (m_nPrintColumn >= GetFixedColumnCount()) {
            bOriginal=bFirst;
            // lets print the missing fixed cells on left first out to last fixed column
            PrintFixedRowCells(0,GetFixedColumnCount(), row, rcT, pDC, bFirst);
            bFirst=bOriginal;
        }

        // now back to normal business print cells in heading after all fixed columns
        PrintFixedRowCells(m_nPrintColumn, GetColumnCount(), row, rcT, pDC, bFirst);
    }

    pDC->SelectObject(pOldFont);
}

// Prints line of row buttons on each page of the printout.  Assumes that
// the window origin is setup before calling
void CDatasheet::PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/)
{
    CFont *pOldFont = pDC->SelectObject(&m_PrinterFont);

    CRect rcT;
    rcT.right = -1;

    BOOL bFirst = TRUE;
    for( int_t iCol = 0; iCol < GetFixedColumnCount(); iCol++) {
        rcT.left = rcT.right + 1;
        rcT.right = rcT.left + GetColumnWidth( iCol) - 1;

        rcT.bottom = -1;
        for( int_t iRow = m_nCurrPrintRow; iRow < GetRowCount(); iRow++) {
            rcT.top = rcT.bottom+1;
            rcT.bottom = rcT.top + GetRowHeight( iRow) - 1;

            if (rcT.bottom > m_nPageHeight) { break; }

            CCell xCell;
            CELL_DISPINFO xDispInfo;
            CCell* pCell = (CCell*)GetCell(iRow, iCol, &xCell, &xDispInfo);
            if (pCell) {
                pCell->PrintCell(pDC, iRow, iCol, rcT);
            }

            if (m_nGridLines == DVL_BOTH || m_nGridLines == DVL_HORZ) {
                int_t Overlap = (iCol == 0)? 0:1;
                pDC->MoveTo(rcT.left-Overlap, rcT.bottom);
                pDC->LineTo(rcT.right, rcT.bottom);
                if (bFirst) {
                    pDC->MoveTo(rcT.left-Overlap, rcT.top-1);
                    pDC->LineTo(rcT.right, rcT.top-1);
                    bFirst = FALSE;
                }
            }
            if (m_nGridLines == DVL_BOTH || m_nGridLines == DVL_VERT) {
                int_t Overlap = (iRow == 0) ? 0 : 1;
                pDC->MoveTo(rcT.right, rcT.top-Overlap);
                pDC->LineTo(rcT.right, rcT.bottom);
                if (iCol == 0) {
                    pDC->MoveTo(rcT.left, rcT.top-Overlap);
                    pDC->LineTo(rcT.left, rcT.bottom);
                }
            }
        }
    }

    pDC->SelectObject(pOldFont);
}

void CDatasheet::PrintHeader(CDC *pDC, CPrintInfo *pInfo)
{
    // print App title on top right margin
    CString strRight;
    COleDateTime tm = COleDateTime::GetCurrentTime();
    strRight = tm.Format(_T("%Y/%m/%d - %H:%M:%S"));

    // print parent window title in the centre (Gert Rijs)
    CString strCenter;
    CWnd *pParentWnd = GetParent();
    while (pParentWnd) {
        pParentWnd->GetWindowText(strCenter);
        if (strCenter.GetLength()) {            // can happen if it is a CWnd, CChildFrm has the title
            break;
        }
        pParentWnd = pParentWnd->GetParent();
    }

    CFont BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    VERIFY(m_PrinterFont.GetLogFont(&lf));
    //lf.lfWeight = FW_BOLD;
    VERIFY(BoldFont.CreateFontIndirect(&lf));

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int_t nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    CRect rc(pInfo->m_rectDraw);
    CRect recT(rc);
    recT.bottom -= 2;
    if (!strCenter.IsEmpty()) {
        pDC->DrawText( strCenter, &recT, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM);
    }
    if (!strRight.IsEmpty()) {
        pDC->DrawText(strRight, &recT, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM);
    }

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();

    // draw ruled-line across top
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.bottom);
    pDC->LineTo(rc.right, rc.bottom);
}

//print footer with a line and date, and page number
void CDatasheet::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
    // page numbering on left
    CString strLeft;
    strLeft.Format(_T("Page %d of %d"), pInfo->m_nCurPage, pInfo->GetMaxPage() );

    // date and time on the right
    CString strRight = _T("Generated by SigmaGraph - Data Plotting and Analysis Software - http://www.hamady.org");

    CRect rc(pInfo->m_rectDraw);

    // draw ruled line on bottom
    pDC->SelectStockObject(BLACK_PEN);
    pDC->MoveTo(rc.left, rc.top);
    pDC->LineTo(rc.right, rc.top);

    CFont BoldFont;
    LOGFONT lf;

    //create bold font for header and footer
    m_PrinterFont.GetLogFont(&lf);
    //lf.lfWeight = FW_BOLD;
    BoldFont.CreateFontIndirect(&lf);

    CFont *pNormalFont = pDC->SelectObject(&BoldFont);
    int_t nPrevBkMode = pDC->SetBkMode(TRANSPARENT);

    // Bug fix - Force text color to black.  It doesn't always
    // get set to a printable color when it gets here.
    pDC->SetTextColor(RGB(0, 0, 0));

    if (!strLeft.IsEmpty()) {
        pDC->DrawText(strLeft, &rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_TOP);
    }
    if (!strRight.IsEmpty()) {
        pDC->DrawText(strRight, &rc, DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_TOP);
    }

    pDC->SetBkMode(nPrevBkMode);
    pDC->SelectObject(pNormalFont);
    BoldFont.DeleteObject();
}

void CDatasheet::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    m_PrinterFont.DeleteObject();
}

void CDatasheet::OnFixedRowClick(CCellID& tCell)
{
    if (!isValid(tCell)) {
        return;
    }

    // Did the user click on a fixed column cell (so the cell was within the overlap of fixed row and column cells)
    if (GetFixedColumnSelection()) {
        if (tCell.col < GetFixedColumnCount()) {
            m_MouseMode = MOUSE_SELECT_ALL;
            OnSelecting(tCell);
        }
        else {
            m_MouseMode = MOUSE_SELECT_COL;
            OnSelecting(tCell);
        }
    }
}

void CDatasheet::OnFixedColumnClick(CCellID& tCell)
{
    if (!isValid(tCell)) {
        return;
    }

    // Did the user click on a fixed row tCell (so the tCell was within the overlap of
    // fixed row and column cells)
    if (GetFixedRowSelection()) {
        if (tCell.row < GetFixedRowCount()) {
            m_MouseMode = MOUSE_SELECT_ALL;
            OnSelecting(tCell);
        }
        else {
            m_MouseMode = MOUSE_SELECT_ROW;
            OnSelecting(tCell);
        }
    }
}

// Gets the extent of the text pointed to by str (no CDC needed)
// By default this uses the selected font (which is a bigger font)
CSize CDatasheet::GetTextExtent(int_t nRow, int_t nCol, LPCTSTR str)
{
    CCell xCell;
    CELL_DISPINFO xDispInfo;
    CCell* pCell = (CCell*)GetCell(nRow, nCol, &xCell, &xDispInfo);
    if (!pCell) {
        return CSize(0, 0);
    }
    else {
        return pCell->GetTextExtent(str);
    }
}

void CDatasheet::OnEditCell(int_t nRow, int_t nCol, CPoint tPoint, UINT nChar)
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    // Can we do it?
    CCellID tCell(nRow, nCol);
    if (!isValid(tCell) || !IsCellEditable(nRow, nCol)) {
        return;
    }

    // Can we see what we are doing?
    EnsureVisible(nRow, nCol);
    if (!IsCellVisible(nRow, nCol)) {
        return;
    }

    // Where, exactly, are we gonna do it??
    CRect rcT;
    if (!GetCellRect(tCell, rcT)) {
        return;
    }

    Edit(nRow, nCol, rcT, tPoint, IDE_INEDIT, nChar);
}

void CDatasheet::EndEdit()
{
    if (m_pInEdit) {
        ((CInEdit*) m_pInEdit)->EndEdit();
    }
}

// virtual
LPCTSTR CDatasheet::GetItemText(int_t nRow, int_t nCol)
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) {
        return _T("");
    }

    CCell xCell;
    CELL_DISPINFO xDispInfo;
    CCell* pCell = GetCell(nRow, nCol, &xCell, &xDispInfo);
    ASSERT(pCell);
    if (!pCell) {
        return _T("");
    }

    return pCell->GetText();
}

void CDatasheet::ResetVirtualOrder()
{
    m_arRowOrder.resize(m_nRows);  
    for (int_t ii = 0; ii < m_nRows; ii++) {
        m_arRowOrder[ii] = ii;
    }
}

// Op�rations sur les colonnes

inline CDocument* CDatasheet::GetDocument(CView *pView/* = NULL*/)
{
    CView *pOwner = NULL;
    CDocument* pDoc = NULL;

    pOwner = (CView*)GetOwner();
    ASSERT(pOwner);
    if (pOwner == NULL) {
        return NULL;
    }
    pDoc = pOwner->GetDocument();
    ASSERT(pDoc);
    if (pDoc == NULL) {
        return NULL;
    }
    if (pView != NULL) {
        pView = pOwner;
    }

    return pDoc;
}

BOOL* CDatasheet::GetRunning()
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return NULL;
    }
    BOOL *pRunning = pDoc->GetRunning();

    return pRunning;
}

void CDatasheet::SetRunning(BOOL bRunning)
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }
    pDoc->SetRunning(bRunning);
}

vector_t* CDatasheet::GetSelectedColumn(int_t *pIndex)
{
    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumn = cellRange.GetMinCol() - 1;

    if (pIndex != NULL) {
        *pIndex = -1;
    }

    if ((nColumn < 0) || (nColumn >= GetColumnCount())) {
        return NULL;
    }

    if (pIndex != NULL) {
        *pIndex = nColumn;
    }

    CSigmaDoc* pDoc = NULL;
    vector_t* pColumn = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return NULL;
    }
    pColumn = pDoc->GetColumn(m_nGroup, nColumn);

    return pColumn;
}

stat_t* CDatasheet::GetSelectedColumnStats(int_t *pIndex)
{
    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumn = cellRange.GetMinCol() - 1;

    if (pIndex != NULL) {
        *pIndex = -1;
    }

    if ((nColumn < 0) || (nColumn >= GetColumnCount())) {
        return NULL;
    }

    if (pIndex != NULL) {
        *pIndex = nColumn;
    }

    CView *pOwner = NULL;
    CSigmaDoc* pDoc = NULL;

    stat_t* pStats = NULL;

    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return NULL;
    }
    pStats = pDoc->GetStats(m_nGroup, nColumn);

    return pStats;
}

void CDatasheet::UpdateRowMenu(CCmdUI* pCmdUI)
{
    CCellRange cellRange = GetSelectedCellRange();

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    int_t nColumnCount = pDoc->GetColumnCount(m_nGroup);
    int_t nColSpan = cellRange.GetColSpan(), nRowMin = cellRange.GetMinRow() - 1,
        nRowMax = cellRange.GetMaxRow() - 1;
    if (nColSpan != nColumnCount) {
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
}

void CDatasheet::UpdateCellMenu(CCmdUI* pCmdUI)
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        pCmdUI->Enable(FALSE);
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    int_t nColumnCount = pDoc->GetColumnCount(m_nGroup);
    int_t nCol = cellRange.GetMinCol() - 1, nRow = cellRange.GetMinRow() - 1;
    if ((nCol < 0) || (nCol >= nColumnCount) || IsEditing()) {
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
}

void CDatasheet::UpdateColumnMenu(CCmdUI* pCmdUI)
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        if ((pCmdUI->m_nID != ID_EDIT_COPY) && (pCmdUI->m_nID != ID_EDIT_PASTE)
            && (pCmdUI->m_nID != ID_EDIT_CLEAR)
            && (pCmdUI->m_nID != ID_EDIT_CELLCOPY) && (pCmdUI->m_nID != ID_EDIT_CELLPASTE)
            && (pCmdUI->m_nID != ID_DATA_DELETEROW) && (pCmdUI->m_nID != ID_DATA_DELETECOL)
            && (pCmdUI->m_nID != ID_DATA_STATS) && (pCmdUI->m_nID != ID_DATA_PROPERTIES)) {
            
            pCmdUI->Enable(FALSE);
            return;
        }
    }

    CCellRange cellRange = GetSelectedCellRange();

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    int_t nColumnCount = pDoc->GetColumnCount(m_nGroup), nDim = 0;
    int_t nCol = cellRange.GetMinCol() - 1, nRow = cellRange.GetMaxRow();
    if ((nCol < 0) || (nCol >= nColumnCount) || IsEditing()) {
        pCmdUI->Enable(FALSE);
        return;
    }

    nDim = pDoc->GetColumnDim(m_nGroup, nCol);
    if (nRow != nDim) {
        pCmdUI->Enable(FALSE);
    }
    else {
        if (pCmdUI->m_nID == ID_EDIT_PASTE) {
            pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
            return;
        }
        else {
            pCmdUI->Enable(TRUE);
        }
    }
}

// Ins�rer une colonne
vector_t* CDatasheet::InsertColumn(int_t nCol, int_t nDim/* = 0*/)
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return NULL;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return NULL;
    }

    vector_t *pVec = NULL;
    if ((pVec = pDoc->InsertColumn(m_nGroup, nCol)) != NULL) {
        SetColumnCount(1 + pDoc->GetColumnCount(m_nGroup));
        int_t nColCount = GetColumnCount();
        if ((nColCount > 1) && (nCol < (nColCount - 1))) {
            m_arColWidths[nColCount - 1] = m_arColWidths[nCol + 1];
        }
        else if (nColCount > 1) {
            m_arColWidths[nColCount - 1] = m_arColWidths[0];
        }
    }

    ResetScrollBars();

    Refresh();

    if (pVec) {
        if (nDim > pVec->dim) {
            AppendRows(nDim);
        }
    }

    return pVec;
}
void CDatasheet::OnInsertColumn()
{
    CCellRange cellRange = GetSelectedCellRange();

    CSigmaDoc* pDoc = (CSigmaDoc*) (GetDocument());
    if (pDoc == NULL) {
        return;
    }

    int nColumnCount = pDoc->GetColumnCount(m_nGroup);

    InsertColumn(cellRange.GetMinCol() - 1);

    bool bModified = (nColumnCount < pDoc->GetColumnCount(m_nGroup));
    SetModified(bModified, bModified);
}

void CDatasheet::OnUpdateInsertColumn(CCmdUI* pCmdUI)
{
    UpdateColumnMenu(pCmdUI);
}

void CDatasheet::DeleteColumn(int_t nColumn)
{
    if ((nColumn < 0) || (nColumn >= GetColumnCount())) {
        return;
    }

    m_arColWidths.RemoveAt(nColumn);
    m_nCols -= 1;
    if (nColumn < m_nFixedCols) {
        m_nFixedCols -= 1;
    }

    if (nColumn == m_idCurrentCell.col) {
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    }
    else if (nColumn < m_idCurrentCell.col) {
        m_idCurrentCell.col -= 1;
    }
}
void CDatasheet::OnDeleteColumn()
{
    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumnMin = cellRange.GetMinCol() - 1, nColumnMax = cellRange.GetMaxCol() - 1, jj;

    if ((nColumnMin < 0) || (nColumnMin >= GetColumnCount())
        || (nColumnMax < 0) || (nColumnMax >= GetColumnCount())
        || (nColumnMin > nColumnMax)) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    int_t nColumnCount = pDoc->GetColumnCount(m_nGroup);
    if ((nColumnMin >= nColumnCount) || (nColumnMax >= nColumnCount) || (nColumnCount < 1)) {
        return;
    }

    int_t nSelColumnCount = 0;
    int_t iSelColumnT[SIGMA_MAXCOLUMNS];
    memset(iSelColumnT, 0, SIGMA_MAXCOLUMNS * sizeof(int_t));
    for (jj = nColumnMin; jj <= nColumnMax; jj++) {
        if (IsCellSelected(1, jj + 1) == TRUE) {
            iSelColumnT[jj] = 1;
            nSelColumnCount += 1;
        }
        else {
            iSelColumnT[jj] = 0;
        }
    }

    // Delete the last column ?
    if ((nColumnCount == nSelColumnCount) && (m_nGroup == 0)) {
        if (this->MessageBox(_T("Remove all columns and close document?"), _T("SigmaGraph"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
            return;
        }
        pDoc->OnCloseDocument();
        return;
    }

    if (this->MessageBox(_T("Remove column from datasheet?"), _T("SigmaGraph"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) {
        return;
    }

    ResetSelectedRange();

    vector_t *pColumn = NULL;

    for (jj = nColumnMax; jj >= nColumnMin; jj--) {
        if (iSelColumnT[jj] == 0) {
            continue;
        }
        pColumn = pDoc->GetColumn(m_nGroup, jj);
        if (pColumn == NULL) {
            continue;
        }
        pDoc->DeleteColumn(m_nGroup, -1, pColumn->id);
    }

    ResetScrollBars();

    Refresh();

    bool bModified = (nColumnCount > pDoc->GetColumnCount(m_nGroup));
    SetModified(bModified, bModified);

    if ((pDoc->GetColumnCount(m_nGroup) == 0) && (m_nGroup == 1)) {
        CView *pView = pDoc->GetDataFitView();
        if (pView) {
            CWnd* pChildFrame = pView->GetParent();
            if (pChildFrame) {
                pChildFrame->DestroyWindow();
            }
        }
    }
}
void CDatasheet::OnUpdateDeleteColumn(CCmdUI* pCmdUI)
{
    UpdateColumnMenu(pCmdUI);
}

inline vector_t* CDatasheet::AppendColumn()
{
    vector_t* pColumn = NULL;

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return NULL;
    }

    int_t nCol = pDoc->GetColumnCount(m_nGroup) - 1, nColCount;

    if ((pColumn = pDoc->InsertColumn(m_nGroup, nCol)) != NULL) {
        SetColumnCount(1 + pDoc->GetColumnCount(m_nGroup));
        nColCount = GetColumnCount();
        if ((nColCount > 1) && (nCol < (nColCount - 1))) {
            m_arColWidths[nColCount - 1] = m_arColWidths[nCol + 1];
        }
        else if (nColCount > 1) {
            m_arColWidths[nColCount - 1] = m_arColWidths[0];
        }
        return pColumn;
    }

    return NULL;
}

void CDatasheet::OnAppendColumn()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    real_t ftmp = 1.0;
    CInputRealDlg dlgAppend;
    dlgAppend.m_iLimit = 3;
    dlgAppend.m_strLabel1 = _T("Number of columns:");
    dlgAppend.m_pfInput1 = &ftmp;
    dlgAppend.m_pfInput2 = NULL;
    dlgAppend.DoModal();

    if (dlgAppend.m_bOK == TRUE) {
        CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
        if (pDoc == NULL) {
            return;
        }
        int_t nColumnCount = GetColumnCount() - 1;
        int_t nAppendColumn = (int_t)ceil(ftmp), ii;
        if ((nAppendColumn < 1) || ((nColumnCount + nAppendColumn) > ML_MAXCOLUMNS)) {
            return;
        }

        for (ii = 0; ii < nAppendColumn; ii++) {
            if (AppendColumn() == NULL) {
                break;
            }
        }

        Refresh();
        ResetScrollBars();

        bool bModified = (nColumnCount < pDoc->GetColumnCount(m_nGroup));
        SetModified(bModified, bModified);
    }
}

void CDatasheet::OnUpdateAppendColumn(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_nGroup == 0);
}


void CDatasheet::OnUpdateDataImport(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_nGroup == 0);
}

void CDatasheet::OnUpdateDataImportDlg(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_nGroup == 0);
}

void CDatasheet::OnUpdatePlotAddCurve(CCmdUI *pCmdUI)
{
    BOOL bEnable = TRUE;
    CSigmaDoc *pDoc = (CSigmaDoc*) (GetDocument());
    if (pDoc != NULL) {
        if (pDoc->GetPlot() != NULL) {
            pCmdUI->SetText((pDoc->GetPlot()->curvecount > 0) ? _T("&Add/Remove Curve") : _T("&Add Curve"));
        }
    }
    pCmdUI->Enable(m_nGroup == 0);
}

void CDatasheet::OnUpdateAppendRow(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_nGroup == 0);
}

int_t CDatasheet::InsertRows(int_t nRow, int_t nCount/* = 1*/)
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return 0;
    }

    if (GetColumnCount() < 1) {
        return 0;
    }

    // If the insertion is for a specific row, check it's within range.
    if ((nRow >= 0) && (nRow >= GetRowCount())) {
        return 0;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return 0;
    }
    int_t nRowCount = pDoc->GetColumnDim(m_nGroup, 0, 0) - 1;
    if ((nCount < 1) || ((nRowCount + nCount) > ML_MAXPOINTS)) {
        return 0;
    }

    int_t iRowHeight = GetRowHeight(nRow), jj;
    int_t ii, iCount = 0;
    for (ii = 1; ii <= nCount; ii++) {
        if (pDoc->InsertRow(m_nGroup, nRow) < 0) {
            break;
        }
        iCount += 1;

        m_nRows += 1;
        m_arRowHeights.Add(0);

        m_arRowHeights[nRow + ii] = iRowHeight;
        if ((m_idCurrentCell.row != -1) && ((nRow + ii) < m_idCurrentCell.row)) {
            m_idCurrentCell.row++;
        }
    }

    m_idTopLeftCell.col = -1;

    ResetVirtualOrder();

    int_t idat = 0;
    for (jj = m_nRows - 1; jj > nRow + 1; jj--) {
        m_arRowHeights[jj] = m_arRowHeights[jj - 1];
    }

    Refresh();
    ResetScrollBars();

    return iCount;
}
void CDatasheet::OnInsertRow()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    if (GetColumnCount() < 1) {
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    int_t nRow = cellRange.GetMinRow() - 1;

    if ((nRow >= 0) && (nRow < m_nFixedRows)) {
        return;
    }

    if (InsertRows(nRow, 1) > 0) {
        SetModified(TRUE, TRUE);
    }
}
void CDatasheet::OnUpdateInsertRow(CCmdUI* pCmdUI)
{
    UpdateRowMenu(pCmdUI);
}

void CDatasheet::DeleteRow(int_t nRow, BOOL bRefresh/* = TRUE*/)
{
    int_t iRowCount = GetRowCount();

    if (iRowCount <= 2) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return;
    }

    if (pDoc->DeleteRow(m_nGroup, nRow) < 0) {
        return;
    }

    int_t jj, idat = 0;
    for (jj = nRow; jj < iRowCount - 1; jj++) {
        m_arRowHeights[jj] = m_arRowHeights[jj + 1];
    }

    m_arRowHeights.RemoveAt(iRowCount - 1);

    m_idTopLeftCell.col = -1;

    m_nRows -= 1;

    ResetVirtualOrder();

    if (nRow == m_idCurrentCell.row) {
        m_idCurrentCell.row = m_idCurrentCell.col = -1;
    }
    else if (nRow < m_idCurrentCell.row) {
        m_idCurrentCell.row--;
    }

    if (bRefresh) {
        Refresh();
        ResetScrollBars();
    }
}
void CDatasheet::OnDeleteRow()
{
    int_t ii;

    if (GetColumnCount() < 1) {
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    int_t nRowMin = cellRange.GetMinRow() - 1, nRowMax = cellRange.GetMaxRow() - 1, iRowCount = GetRowCount();

    if (iRowCount <= 2) {
        return;
    }

    if (nRowMin > nRowMax) {
        SigmaApp.Output(_T("Cannot delete rows: invalid range\r\n"));
        return;
    }

    // If the insertion is for a specific row, check it's within range.
    if ((nRowMin >= 0) && (nRowMin >= iRowCount)) {
        SigmaApp.Output(_T("Cannot delete rows: invalid range\r\n"));
        return;
    }
    if ((nRowMax >= 0) && (nRowMax >= iRowCount)) {
        SigmaApp.Output(_T("Cannot delete rows: invalid range\r\n"));
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return;
    }

    for (ii = nRowMin; ii <= nRowMax; ii++) {
        DeleteRow(nRowMin);
    }

    pDoc->UpdateAllViews(NULL);

    bool bModified = (iRowCount > GetRowCount());
    SetModified(bModified, bModified);
}
void CDatasheet::OnUpdateDeleteRow(CCmdUI* pCmdUI)
{
    UpdateRowMenu(pCmdUI);
}

int_t CDatasheet::AppendRows(int_t iCount)
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return 0;
    }

    if (GetColumnCount() < 1) {
        return 0;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return 0;
    }
    int_t nRowCount = pDoc->GetColumnDim(m_nGroup, 0, 0) - 1;
    if ((iCount < 1) || ((nRowCount + iCount) > ML_MAXPOINTS)) {
        return 0;
    }

    int_t nCount = 0, nRow, ii, iRowHeight, iRowCount;
    for (ii = 0; ii < iCount; ii++) {
        nRow = pDoc->GetColumnDim(m_nGroup, 0, 0) - 1;
        iRowCount = GetRowCount();

        if (pDoc->InsertRow(m_nGroup, nRow) < 0) {
            break;
        }

        iRowHeight = GetRowHeight(iRowCount - 1);
        m_idTopLeftCell.col = -1;
        m_nRows += 1;
        m_arRowHeights.Add(0);
        ResetVirtualOrder();
        m_arRowHeights[iRowCount] = iRowHeight;
        if ((m_idCurrentCell.row != -1) && ((nRow + 1) < m_idCurrentCell.row)) {
            m_idCurrentCell.row++;
        }
        nCount += 1;
    }

    Refresh();
    ResetScrollBars();

    return nCount;
}

void CDatasheet::OnAppendRow()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    if (GetColumnCount() < 1) {
        return;
    }

    real_t ftmp = 1.0;
    CInputRealDlg dlgAppend;
    dlgAppend.m_iLimit = 3;
    dlgAppend.m_strLabel1 = _T("Number of rows:");
    dlgAppend.m_pfInput1 = &ftmp;
    dlgAppend.m_pfInput2 = NULL;
    dlgAppend.DoModal();

    if (dlgAppend.m_bOK == TRUE) {
        int_t nAppendRow = (int_t)ceil(ftmp);
        if (AppendRows(nAppendRow) > 0) {
            SetModified(TRUE, TRUE);
        }
    }
}

void CDatasheet::OnAutoResize()
{
    AutoSize();
}

void CDatasheet::OnSetColumnValues()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return;
    }

    CFormulaDlg formulaDlg;
    int_t nSelectedColumn = -1, ii;
    vector_t *pColumun = GetSelectedColumn(&nSelectedColumn);
    if (pColumun == NULL) {
        return;
    }

    formulaDlg.m_nColumnCount = GetColumnCount() - 1;
    for (ii = 0; ii < formulaDlg.m_nColumnCount; ii++) {
        formulaDlg.m_pColumn[ii] = pDoc->GetColumn(m_nGroup, ii);
    }
    for (ii = formulaDlg.m_nColumnCount; ii < SIGMA_MAXCOLUMNS; ii++) {
        formulaDlg.m_pColumn[ii] = NULL;
    }
    formulaDlg.m_nSelectedColumn = nSelectedColumn;
    formulaDlg.m_pbRunning = GetRunning();
    formulaDlg.SetOwner(this);
    formulaDlg.DoModal();

    Refresh();

    SetModified(formulaDlg.m_bOK, formulaDlg.m_bOK);
}
void CDatasheet::OnUpdateSetColumnValues(CCmdUI* pCmdUI)
{
    UpdateColumnMenu(pCmdUI);
}

void CDatasheet::OnCreateSeries()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CDocument* pDoc = GetDocument();
    if (pDoc == NULL) {
        return;
    }

    vector_t *pColumun = GetSelectedColumn();
    if (pColumun == NULL) {
        return;
    }

    CSeriesDlg seriesDlg;
    seriesDlg.m_pDoc = pDoc;
    seriesDlg.m_pColumn = pColumun;
    seriesDlg.DoModal();

    Refresh();

    SetModified(seriesDlg.m_bOK, seriesDlg.m_bOK);
}
void CDatasheet::OnUpdateCreateSeries(CCmdUI* pCmdUI)
{
    UpdateColumnMenu(pCmdUI);
}

void CDatasheet::OnSortAsc()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    int_t nColumnCount = GetColumnCount(), jj, nPivot = 0;
    CCellRange cellRange = GetSelectedCellRange();

    CCellID tCell = GetFocusCell();
    if (isValid(tCell)) {
        nPivot = tCell.col - 1;
    }
    else {
        nPivot = cellRange.GetMinCol() - 1;
    }
    if ((nPivot < 0) || (nPivot >= nColumnCount)) {
        return;
    }

    int_t nMinCol = cellRange.GetMinCol(), nMaxCol = cellRange.GetMaxCol();
    if ((nMinCol < 1) || (nMinCol > nColumnCount)
        || (nMaxCol < 1) || (nMaxCol > nColumnCount)
        || (nMaxCol < nMinCol)) {
            return;
    }
    int_t nStart = nMinCol - 1, nStop = nMaxCol - 1;
    byte_t ilink[ML_MAXCOLUMNS];
    memset(ilink, 0, ML_MAXCOLUMNS * sizeof(byte_t));
    if (nStop > nStart) {
        for (jj = nStart; jj <= nStop; jj++) {
            // Colonne s�lectionn�e ?
            if (IsColumnSelected(jj + 1) == TRUE) {
                ilink[jj] = 1;
            }
            else {
                ilink[jj] = 0;
            }
        }
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return;
    }

    if (nStop > nStart) {
        if (pDoc->SortColumns(m_nGroup, nPivot, 1, ilink)) {
            SetModified(TRUE, TRUE);
        }
    }
    else {
        if (pDoc->SortColumns(m_nGroup, nPivot, 1, NULL)) {
            SetModified(TRUE, TRUE);
        }
    }

    Refresh();
    pDoc->UpdateAllViews(NULL);
}
void CDatasheet::OnUpdateSortAsc(CCmdUI* pCmdUI)
{
    UpdateColumnMenu(pCmdUI);
}

void CDatasheet::OnSortDesc()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    int_t nColumnCount = GetColumnCount(), jj, nPivot = 0;
    CCellRange cellRange = GetSelectedCellRange();

    CCellID tCell = GetFocusCell();
    if (isValid(tCell)) {
        nPivot = tCell.col - 1;
    }
    else {
        nPivot = cellRange.GetMinCol() - 1;
    }
    if ((nPivot < 0) || (nPivot >= nColumnCount)) {
        return;
    }

    int_t nMinCol = cellRange.GetMinCol(), nMaxCol = cellRange.GetMaxCol();
    if ((nMinCol < 1) || (nMinCol > nColumnCount)
        || (nMaxCol < 1) || (nMaxCol > nColumnCount)
        || (nMaxCol < nMinCol)) {
            return;
    }
    int_t nStart = nMinCol - 1, nStop = nMaxCol - 1;
    byte_t ilink[ML_MAXCOLUMNS];
    memset(ilink, 0, ML_MAXCOLUMNS * sizeof(byte_t));
    if (nStop > nStart) {
        for (jj = nStart; jj <= nStop; jj++) {
            // Colonne s�lectionn�e ?
            if (IsColumnSelected(jj + 1) == TRUE) {
                ilink[jj] = 1;
            }
            else {
                ilink[jj] = 0;
            }
        }
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)GetDocument();
    if (pDoc == NULL) {
        return;
    }

    if (nStop > nStart) {
        if (pDoc->SortColumns(m_nGroup, nPivot, 0, ilink)) {
            SetModified(TRUE, TRUE);
        }
    }
    else {
        if (pDoc->SortColumns(m_nGroup, nPivot, 0, NULL)) {
            SetModified(TRUE, TRUE);
        }
    }

    Refresh();
    pDoc->UpdateAllViews(NULL);
}
void CDatasheet::OnUpdateSortDesc(CCmdUI* pCmdUI)
{
    UpdateColumnMenu(pCmdUI);
}

void CDatasheet::OnMask()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumnMin = cellRange.GetMinCol() - 1, nColumnMax = cellRange.GetMaxCol() - 1, jj;
    int_t nRowMin = cellRange.GetMinRow() - 1, nRowMax = cellRange.GetMaxRow() - 1, ii;

    if ((nColumnMin < 0) || (nColumnMin >= GetColumnCount())
        || (nColumnMax < 0) || (nColumnMax >= GetColumnCount())
        || (nColumnMin > nColumnMax)) {
        return;
    }
    if ((nRowMin < 0) || (nRowMin >= GetRowCount())
        || (nRowMax < 0) || (nRowMax >= GetRowCount())
        || (nRowMin > nRowMax)) {
        return;
    }

    // ... masquer les �l�ments s�lectionn�s 
    // (!!! pour chaque colonne X, il faut faire correspondre les colonnes Y avec le mask ad hoc et vice-versa !!!)
    // (Association g�r�e par le trac�)
    // ...
    byte_t bMask;
    vector_t *pColumn = NULL;
    for (ii = nRowMin; ii <= nRowMax; ii++) {
        for (jj = nColumnMin; jj <= nColumnMax; jj++) {
            if (IsCellSelected(ii + 1, jj + 1) == FALSE) {
                continue;
            }
            pColumn = pDoc->GetColumn(m_nGroup, jj);
            if (pColumn == NULL) {
                continue;
            }
            if ((pColumn->status & 0x0F) == 0x0F) {    // Colonne non encore �dit�e
                continue;
            }
            bMask = pDoc->GetColumnItemMask(m_nGroup, jj, ii, 0);
            if (bMask == 0xFF) {        // Don't mask or unmask non edited cell
                continue;
            }
            pDoc->SetColumnItemMask(m_nGroup, jj, ii, (bMask & 0xF0) | 0x01, 0);
        }
    }
    pDoc->UpdateAllViews(NULL);
    SetModified(TRUE, TRUE);
}
void CDatasheet::OnUpdateMask(CCmdUI* pCmdUI)
{
    UpdateCellMenu(pCmdUI);
}

void CDatasheet::OnUnmask()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumnMin = cellRange.GetMinCol() - 1, nColumnMax = cellRange.GetMaxCol() - 1, jj;
    int_t nRowMin = cellRange.GetMinRow() - 1, nRowMax = cellRange.GetMaxRow() - 1, ii;

    if ((nColumnMin < 0) || (nColumnMin >= GetColumnCount())
        || (nColumnMax < 0) || (nColumnMax >= GetColumnCount())
        || (nColumnMin > nColumnMax)) {
            return;
    }
    if ((nRowMin < 0) || (nRowMin >= GetRowCount())
        || (nRowMax < 0) || (nRowMax >= GetRowCount())
        || (nRowMin > nRowMax)) {
            return;
    }

    // ... masquer les �l�ments s�lectionn�s 
    // (!!! pour chaque colonne X, il faut faire correspondre les colonnes Y avec le mask ad hoc et vice-versa !!!)
    // (Association g�r�e par le trac�)
    // ...
    byte_t bMask;
    vector_t *pColumn = NULL;
    for (ii = nRowMin; ii <= nRowMax; ii++) {
        for (jj = nColumnMin; jj <= nColumnMax; jj++) {
            if (IsCellSelected(ii + 1, jj + 1) == FALSE) {
                continue;
            }
            pColumn = pDoc->GetColumn(m_nGroup, jj);
            if (pColumn == NULL) {
                continue;
            }
            if ((pColumn->status & 0x0F) == 0x0F) {    // Colonne non encore �dit�e
                continue;
            }
            bMask = pDoc->GetColumnItemMask(m_nGroup, jj, ii, 0);
            if (bMask == 0xFF) {        // Don't mask or unmask non edited cell
                continue;
            }
            pDoc->SetColumnItemMask(m_nGroup, jj, ii, bMask & 0xF0, 0);
        }
    }
    pDoc->UpdateAllViews(NULL);
    SetModified(TRUE, TRUE);
}
void CDatasheet::OnUpdateUnmask(CCmdUI* pCmdUI)
{
    UpdateCellMenu(pCmdUI);
}

void CDatasheet::OnInvertmask()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumnMin = cellRange.GetMinCol() - 1, nColumnMax = cellRange.GetMaxCol() - 1, jj;
    int_t nRowMin = cellRange.GetMinRow() - 1, nRowMax = cellRange.GetMaxRow() - 1, ii;

    if ((nColumnMin < 0) || (nColumnMin >= GetColumnCount())
        || (nColumnMax < 0) || (nColumnMax >= GetColumnCount())
        || (nColumnMin > nColumnMax)) {
            return;
    }
    if ((nRowMin < 0) || (nRowMin >= GetRowCount())
        || (nRowMax < 0) || (nRowMax >= GetRowCount())
        || (nRowMin > nRowMax)) {
            return;
    }

    // ... masquer les �l�ments s�lectionn�s 
    // (!!! pour chaque colonne X, il faut faire correspondre les colonnes Y avec le mask ad hoc et vice-versa !!!)
    // (Association g�r�e par le trac�)
    // ...
    byte_t bMask;
    for (ii = nRowMin; ii <= nRowMax; ii++) {
        for (jj = nColumnMin; jj <= nColumnMax; jj++) {
            bMask = pDoc->GetColumnItemMask(m_nGroup, jj, ii, 0);
            if (bMask == 0xFF) {        // Don't mask or unmask non edited cell
                continue;
            }
            if ((bMask & 0x0F) == 0x01) {
                bMask &= 0xF0;
            }
            else {
                bMask = (bMask & 0xF0) | 0x01;
            }
            pDoc->SetColumnItemMask(m_nGroup, jj, ii, bMask, 0);
        }
    }
    pDoc->UpdateAllViews(NULL);
    SetModified(TRUE, TRUE);
}
void CDatasheet::OnUpdateInvertmask(CCmdUI* pCmdUI)
{
    UpdateCellMenu(pCmdUI);
}

void CDatasheet::OnStats()
{
    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumn = cellRange.GetMinCol() - 1;

    if (nColumn < 0 || nColumn >= GetColumnCount()) {
        return;
    }

    CView *pOwner = NULL;
    CSigmaDoc* pDoc = NULL;

    CSigmaStats statsDlg;

    vector_t *pColumun = GetSelectedColumn();
    if (pColumun == NULL) {
        return;
    }

    stat_t *pStats = GetSelectedColumnStats(), Stats;
    if (pStats == NULL) {
        memset(&Stats, 0, sizeof(stat_t));
        pStats = &Stats;
    }

    BOOL *pRunning = GetRunning();
    if (pRunning == NULL) {
        return;
    }

    statsDlg.m_pColumn = pColumun;
    statsDlg.m_pStats = pStats;
    statsDlg.m_pbRunning = pRunning;

    statsDlg.DoModal();
}
void CDatasheet::OnUpdateStats(CCmdUI* pCmdUI)
{
    UpdateColumnMenu(pCmdUI);
}

void CDatasheet::OnColumnProperties()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CCellRange cellRange = GetSelectedCellRange();

    int_t nColumn = cellRange.GetMinCol() - 1;

    if ((nColumn < 0) || (nColumn >= GetColumnCount())) {
        return;
    }

    CView *pOwner = (CView*)GetOwner();
    if (pOwner == NULL) {
        return;
    }

    CSigmaDoc* pDoc = (CSigmaDoc*)(pOwner->GetDocument());
    if (pDoc == NULL) {
        return;
    }

    vector_t *pColumun = NULL;
    int_t nColumnCount = 0;
    pColumun = pDoc->GetColumns(m_nGroup, &nColumnCount);

    if ((pColumun != NULL) && (nColumnCount > 0) && (nColumn >= 0) && (nColumn < nColumnCount)) {

        CColumnDlg columnDlg;
        columnDlg.m_pDoc = (CDocument*)pDoc;
        columnDlg.m_pColumn = pColumun;
        columnDlg.m_nColumnCount = nColumnCount;
        columnDlg.m_nCurrentColumn = nColumn;
        columnDlg.m_pSelectedColumnIndex = &m_nSelectedColumn;
        columnDlg.DoModal();

        Refresh();
        m_nSelectedColumn = -1;
    }
}

void CDatasheet::OnUpdateColumnProperties(CCmdUI* pCmdUI)
{
    if (m_nGroup != 0) {
        pCmdUI->Enable(FALSE);
        return;
    }

    UpdateColumnMenu(pCmdUI);
}

void CDatasheet::OnPlotAddCurve()
{
    // Lecture seule ?
    if (IsEditable() == FALSE) {
        return;
    }

    CSigmaDoc* pDoc = NULL;
    vector_t* pColumn = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    int_t nColumnCount = 0;
    pColumn = pDoc->GetColumns(m_nGroup, &nColumnCount);

    // Show the plot window before showing the 'add curve' dialog
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    ASSERT(pFrame != NULL);
    if (pFrame == NULL) {
        return;
    }
    if (pFrame->CreateSigmaView(SigmaApp.m_pTemplatePlot, RUNTIME_CLASS(CSigmaViewPlot), TRUE) == FALSE) {
        return;
    }

    int_t nSelectedColumnX = 0, nSelectedColumnY = 0;
    CCellRange selectedRange = GetSelectedCellRange();
    if (isValid(selectedRange)) {
        nSelectedColumnX = selectedRange.GetMinCol();
        nSelectedColumnY = selectedRange.GetMaxCol();
    }

    CAddCurveDlg addCurveDlg;
    addCurveDlg.m_pDoc = GetDocument();
    addCurveDlg.m_nSelectedColumnX = nSelectedColumnX - 1;
    addCurveDlg.m_nSelectedColumnY = nSelectedColumnY - 1;
    addCurveDlg.m_pPlot = pDoc->GetPlot();
    addCurveDlg.m_pColumn = pColumn;
    addCurveDlg.m_pColumnAutoX = pDoc->GetColumnAutoX();
    addCurveDlg.m_nColumnCount = nColumnCount;

    int_t iCurveCount = pDoc->GetPlot()->curvecount;

    addCurveDlg.DoModal();

    bool bModified = (iCurveCount != pDoc->GetPlot()->curvecount);
    SetModified(bModified, bModified);
}

void CDatasheet::OnEditUndo()
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pDoc->Undo();
}

void CDatasheet::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pCmdUI->Enable(pDoc->CanUndo());
}

void CDatasheet::OnEditRedo()
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pDoc->Redo();
}

void CDatasheet::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pCmdUI->Enable(pDoc->CanRedo());
}

void CDatasheet::OnDataImport()
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pDoc->OnDataImport();
}

void CDatasheet::OnDataImportDlg()
{
    CSigmaDoc* pDoc = NULL;
    pDoc = (CSigmaDoc*)(GetDocument());
    if (pDoc == NULL) {
        return;
    }

    pDoc->OnDataImportDlg();
}

HICON CDatasheet::GetIconForItem(UINT itemID) const
{
    HICON hIcon = (HICON) NULL;

    if (IS_INTRESOURCE(itemID)) {
        hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(itemID), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
    }

    if (hIcon == NULL) {
        CMenu *pMenu = GetMenu();
        if (pMenu) {
            CString sItem = _T(""); // look for a named item in resources
            if (::IsMenu(pMenu->m_hMenu)) {
                pMenu->GetMenuString(itemID, sItem, MF_BYCOMMAND);
            }
            // cannot have resource items with space in name
            if (sItem.IsEmpty() == false) {
                sItem.Replace(_T(' '), _T('_'));
                hIcon = (HICON)::LoadImage(::AfxGetResourceHandle(), sItem, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED);
            }
        }
    }
    return hIcon;
}

void CDatasheet::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
    if ((lpdis == NULL) || (lpdis->CtlType != ODT_MENU)) {
        CWnd::OnDrawItem(nIDCtl, lpdis);
        return; //not for a menu
    }
    if (lpdis->itemState & ODS_GRAYED) {
        CWnd::OnDrawItem(nIDCtl, lpdis);
        return;
    }

    HICON hIcon = GetIconForItem(lpdis->itemID);
    if (hIcon) {
        ICONINFO iconinfoT;
        ::GetIconInfo(hIcon, &iconinfoT);

        BITMAP bitmapT;
        ::GetObject(iconinfoT.hbmColor, sizeof(bitmapT), &bitmapT);
        ::DeleteObject(iconinfoT.hbmColor);
        ::DeleteObject(iconinfoT.hbmMask);

        ::DrawIconEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, hIcon, bitmapT.bmWidth, bitmapT.bmHeight, 0, NULL, DI_NORMAL);
    }
}

void CDatasheet::OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu)
{
    CWnd::OnInitMenuPopup(pMenu, nIndex, bSysMenu);

    if (bSysMenu) {
        pMenu = GetSystemMenu(FALSE);
    }
    MENUINFO mnfo;
    mnfo.cbSize = sizeof(mnfo);
    mnfo.fMask = MIM_STYLE;
    mnfo.dwStyle = MNS_CHECKORBMP | MNS_AUTODISMISS;
    pMenu->SetMenuInfo(&mnfo);

    MENUITEMINFO minfo;
    minfo.cbSize = sizeof(minfo);

    for (UINT pos = 0; pos < (UINT) (pMenu->GetMenuItemCount()); pos++) {
        minfo.fMask = MIIM_FTYPE | MIIM_ID;
        pMenu->GetMenuItemInfo(pos, &minfo, TRUE);

        HICON hIcon = GetIconForItem(minfo.wID);

        if (hIcon && !(minfo.fType & MFT_OWNERDRAW)) {
            minfo.fMask = MIIM_FTYPE | MIIM_BITMAP;
            minfo.hbmpItem = HBMMENU_CALLBACK;
            minfo.fType = MFT_STRING;
            pMenu->SetMenuItemInfo(pos, &minfo, TRUE);
        }
    }
}

void CDatasheet::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
    if ((lpmis == NULL) || (lpmis->CtlType != ODT_MENU)) {
        CWnd::OnMeasureItem(nIDCtl, lpmis); //not for a menu
        return;
    }

    lpmis->itemWidth = 16;
    lpmis->itemHeight = 16;

    CMenu *pMenu = GetMenu();
    ASSERT(pMenu);
    if (pMenu == NULL) {
        CWnd::OnMeasureItem(nIDCtl, lpmis);
        return;
    }

    CString sItem = _T("");
    if (::IsMenu(pMenu->m_hMenu)) {
        pMenu->GetMenuString(lpmis->itemID, sItem, MF_BYCOMMAND);
    }

    HICON hIcon = GetIconForItem(lpmis->itemID);

    if (hIcon) {
        ICONINFO iconinfoT;
        ::GetIconInfo(hIcon, &iconinfoT);

        BITMAP bitmapT;
        ::GetObject(iconinfoT.hbmColor, sizeof(bitmapT), &bitmapT);
        ::DeleteObject(iconinfoT.hbmColor);
        ::DeleteObject(iconinfoT.hbmMask);

        lpmis->itemWidth = bitmapT.bmWidth;
        lpmis->itemHeight = bitmapT.bmHeight;
    }
}

BOOL CDatasheet::Edit(int_t nRow, int_t nCol, CRect rcT, CPoint /* tPoint */, UINT nID, UINT nChar)
{
    if (m_pInEdit) {
        m_pInEdit->SendMessage(WM_CHAR, nChar);
    }
    else {
        DWORD dwStyle = ES_RIGHT;
        if ((nRow == 0) || (nCol == 0)) {
            dwStyle = ES_CENTER;
        }

        char_t szT[ML_STRSIZEN];
        byte_t bMask;

        GetCellText(nCol, nRow, szT, &bMask);

        m_pInEdit = new CInEdit(this, rcT, dwStyle, nID, nRow, nCol, (LPCTSTR) szT, nChar);
    }

    return TRUE;
}
