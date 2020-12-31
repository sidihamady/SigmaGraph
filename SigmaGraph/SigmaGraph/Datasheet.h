/////////////////////////////////////////////////////////////////////////////
// GridCtrl.h : header file
//
// MFC Grid Control - main header
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright(C) 1998-2005. All Rights Reserved.
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
// For use with CGridCtrl v2.20+
//
//////////////////////////////////////////////////////////////////////

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

#pragma once

#include "CellRange.h"
#include "Cell.h"
#include <afxtempl.h>
#include <vector>
#include <map>

#include "Resource.h"

#define DATASHEET_CLASS        _T("DATASHEET_CLASS")        // Window class name
#define IDE_INEDIT            8                                    // ID of inplace edit control

#include <afxole.h>

typedef struct _INEDIT_ITEM {
    int_t col;                        // Column of item
    int_t row;                        // Row of item
    UINT mask;                        // Mask for use in getting/setting tCell data
    UINT nState;                      // cell state (focus/hilighted etc)
    DWORD nFormat;                    // Format of tCell
    COLORREF crBkClr;                 // Background colour (or CLR_DEFAULT)
    COLORREF crFgClr;                 // Forground colour (or CLR_DEFAULT)
    LPARAM lParam;                    // 32-bit value to associate with item
    LOGFONT lfFont;                   // Cell font
    UINT nMargin;                     // Internal tCell margin
    char_t szText[ML_STRSIZEN];
} INEDIT_ITEM;


// This is sent to the Datasheet from child in-place edit controls
typedef struct _CELL_DISPINFO {
    NMHDR hdr;
    INEDIT_ITEM item;
} CELL_DISPINFO;

// Grid line/scrollbar selection
#define DVL_NONE                        0L                        // Neither
#define DVL_HORZ                        1L                        // Horizontal line or scrollbar
#define DVL_VERT                        2L                        // Vertical line or scrollbar
#define DVL_BOTH                        3L                        // Both

// Autosizing option
#define DVS_DEFAULT                    0
#define DVS_HEADER                     1                          // Size using column fixed cells data only
#define DVS_DATA                       2                          // Size using column non-fixed cells data only
#define DVS_BOTH                       3                          // Size using column fixed and non-fixed

class CDatasheet;

class CDatasheet : public CWnd
{
    DECLARE_DYNCREATE(CDatasheet)
    friend class CCell;

// Construction
public:
    CDatasheet(int_t nRows = 0, int_t nCols = 0, int_t nFixedRows = 0, int_t nFixedCols = 0);

    BOOL Create(const RECT& rcT, CWnd* parent, UINT nID, DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE);

    char_t* GetCellText(int_t nCol, int_t nRow, char_t* pszCellText, byte_t *pbMask);
    char_t GetColumnType(int_t nCol);
    char_t GetColumnName(int_t nCol);
    BOOL IsCellMasked(int_t nCol, int_t nRow);

private:
    CWnd* m_pInEdit;

public:
    int_t m_nGroup;
    int_t GetRowCount();
    int_t GetColumnCount();
    int_t GetFixedRowCount() { return m_nFixedRows; }
    int_t GetFixedColumnCount() { return m_nFixedCols; }
    BOOL SetRowCount(int_t nRows = 10);
    BOOL SetColumnCount(int_t nCols = 10);
    BOOL SetFixedRowCount(int_t nFixedRows = 1);
    BOOL SetFixedColumnCount(int_t nFixedCols = 1);

    int_t GetRowHeight(int_t nRow);
    BOOL SetRowHeight(int_t row, int_t height, bool bResetScrollBars = true);
    int_t GetColumnWidth(int_t nCol);
    int_t GetColumnWidth();
    BOOL SetColumnWidth(int_t col, int_t width, bool bResetScrollBars = true);

    BOOL GetCellOrigin(int_t nRow, int_t nCol, LPPOINT pPoint);
    BOOL GetCellOrigin(const CCellID& cell, LPPOINT pPoint);
    BOOL GetCellRect(int_t nRow, int_t nCol, LPRECT pRect);
    BOOL GetCellRect(const CCellID& cell, LPRECT pRect);

    BOOL GetTextRect(const CCellID& cell, LPRECT pRect);
    BOOL GetTextRect(int_t nRow, int_t nCol, LPRECT pRect);

    CCellID GetCellFromPt(CPoint tPoint, BOOL bAllowFixedCellCheck = TRUE);

    int_t GetFixedRowHeight();
    int_t GetFixedColumnWidth();
    long_t GetVirtualWidth();
    long_t GetVirtualHeight();

    CSize GetTextExtent(int_t nRow, int_t nCol, LPCTSTR str);
    // Get extent of current text in cell
    inline CSize GetCellTextExtent(int_t nRow, int_t nCol) { return GetTextExtent(nRow, nCol, GetItemText(nRow,nCol)); }

    void SetTextColor(COLORREF clr) { m_cellDefault.SetTextClr(clr); }
    COLORREF GetTextColor() { return m_cellDefault.GetTextClr(); }
    void SetTextBkColor(COLORREF clr) { m_cellDefault.SetBackClr(clr); }
    COLORREF GetTextBkColor() { return m_cellDefault.GetBackClr(); }
    void SetFixedTextColor(COLORREF clr) { m_cellFixedRowDef.SetTextClr(clr); m_cellFixedColDef.SetTextClr(clr); m_cellFixedRowColDef.SetTextClr(clr); }
    COLORREF GetFixedTextColor() { return m_cellFixedRowDef.GetTextClr(); }
    void SetFixedBkColor(COLORREF clr) { m_cellFixedRowDef.SetBackClr(clr); m_cellFixedColDef.SetBackClr(clr); m_cellFixedRowColDef.SetBackClr(clr); }
    COLORREF GetFixedBkColor() { return m_cellFixedRowDef.GetBackClr(); }

    void SetDefCellMargin( int_t nMargin)  { m_cellDefault.SetMargin(nMargin); m_cellFixedRowDef.SetMargin(nMargin); m_cellFixedColDef.SetMargin(nMargin); m_cellFixedRowColDef.SetMargin(nMargin); }
    int_t GetDefCellMargin() { return m_cellDefault.GetMargin(); }

    int_t GetDefCellHeight() { return m_cellDefault.GetHeight(); }
    void SetDefCellHeight(int_t nHeight) { m_cellDefault.SetHeight(nHeight); m_cellFixedRowDef.SetHeight(nHeight); m_cellFixedColDef.SetHeight(nHeight); m_cellFixedRowColDef.SetHeight(nHeight); }
    int_t GetDefCellWidth() { return m_cellDefault.GetWidth(); }
    void SetDefCellWidth(int_t nWidth) { m_cellDefault.SetWidth(nWidth); m_cellFixedRowDef.SetWidth(nWidth); m_cellFixedColDef.SetWidth(nWidth); m_cellFixedRowColDef.SetWidth(nWidth); }

    int_t GetSelectedCount() { return (int_t) (m_SelectedCellMap.size()); }

    CCellID SetFocusCell(CCellID cell);
    CCellID SetFocusCell(int_t nRow, int_t nCol);
    CCellID GetFocusCell() { return m_idCurrentCell; }

    void SetGridLines(int_t nWhichLines = DVL_BOTH);
    int_t GetGridLines() { return m_nGridLines; }
    void SetEditable(BOOL bEditable = TRUE) { m_bEditable = bEditable; }
    BOOL IsEditable() { return m_bEditable; }
    void SetSingleRowSelection(BOOL bSing = TRUE) { m_bSingleRowSelection = bSing; }
    BOOL GetSingleRowSelection() { return m_bSingleRowSelection; }
    void SetSingleColSelection(BOOL bSing = TRUE) { m_bSingleColSelection = bSing; }
    BOOL GetSingleColSelection() { return m_bSingleColSelection;     }
    void EnableSelection(BOOL bEnable = TRUE) { ResetSelectedRange(); m_bEnableSelection = bEnable; ResetSelectedRange(); }
    BOOL IsSelectable() { return m_bEnableSelection;        }
    void SetFixedColumnSelection(BOOL bSelect) { m_bFixedColumnSelection = bSelect;}
    BOOL GetFixedColumnSelection() { return m_bFixedColumnSelection; }
    void SetFixedRowSelection(BOOL bSelect) { m_bFixedRowSelection = bSelect; }
    BOOL GetFixedRowSelection() { return m_bFixedRowSelection; }
    void SetRowResize(BOOL bResize = TRUE) { m_bAllowRowResize = bResize; }
    BOOL GetRowResize() { return m_bAllowRowResize; }
    void SetColumnResize(BOOL bResize = TRUE) { m_bAllowColumnResize = bResize; }
    BOOL GetColumnResize() { return m_bAllowColumnResize; }
    void SetHandleTabKey(BOOL bHandleTab = TRUE) { m_bHandleTabKey = bHandleTab; }
    BOOL GetHandleTabKey() { return m_bHandleTabKey; }
    void SetDoubleBuffering(BOOL bBuffer = TRUE) { m_bDoubleBuffer = bBuffer; }
    BOOL GetDoubleBuffering() { return m_bDoubleBuffer; }
    void SetTrackFocusCell(BOOL bTrack) { m_bTrackFocusCell = bTrack; }
    BOOL GetTrackFocusCell() { return m_bTrackFocusCell; }
    void SetFrameFocusCell(BOOL bFrame) { m_bFrameFocus = bFrame; }
    BOOL GetFrameFocusCell() { return m_bFrameFocus; }
    void SetAutoSizeStyle(int_t nStyle = DVS_BOTH) { m_nAutoSizeColumnStyle = nStyle;  }
    int_t GetAutoSizeStyle() { return m_nAutoSizeColumnStyle; }

    CDocument* GetDocument(CView *pView = NULL);
    BOOL* GetRunning();
    void SetRunning(BOOL bRunning);
    vector_t* GetSelectedColumn(int_t *pIndex = NULL);
    stat_t* GetSelectedColumnStats(int_t *pIndex = NULL);

public:
    CCell* GetDefaultCell(BOOL bFixedRow, BOOL bFixedCol);

public:
    CCell* GetCell(int_t nRow, int_t nCol, CCell* pCell, CELL_DISPINFO *pDispInfo);        // Get the actual cell!

    void SetModified(BOOL bModified = TRUE, BOOL bSaveState = FALSE);
    BOOL GetModified(int_t nRow = -1, int_t nCol = -1);
    BOOL IsCellFixed(int_t nRow, int_t nCol);

    BOOL SetItem(const GV_ITEM* pItem);
    BOOL GetItem(GV_ITEM* pItem);
    BOOL SetItemText(int_t nRow, int_t nCol, LPCTSTR szText, BOOL bParse = TRUE);
    LPCTSTR GetItemText(int_t nRow, int_t nCol);

    LPARAM GetItemData(int_t nRow, int_t nCol);
    BOOL SetItemState(int_t nRow, int_t nCol, UINT state);
    BOOL SetItemStateSelected(int_t nRow, int_t nCol);
    BOOL SetItemStateUnselected(int_t nRow, int_t nCol);
    UINT GetItemState(int_t nRow, int_t nCol);

    BOOL IsItemEditing(int_t nRow, int_t nCol);

    BOOL SetCellType(int_t nRow, int_t nCol, CRuntimeClass* pRuntimeClass);
    BOOL SetDefaultCellType(CRuntimeClass* pRuntimeClass);

public:
    BOOL DeleteAllItems();

    BOOL AutoSizeRow(int_t nRow, BOOL bResetScroll = TRUE);
    BOOL AutoSizeColumn(int_t nCol, UINT nAutoSizeStyle = DVS_DEFAULT, BOOL bResetScroll = TRUE);
    void AutoSizeRows();
    void AutoSizeColumns(UINT nAutoSizeStyle = DVS_DEFAULT);
    void AutoSize(UINT nAutoSizeStyle = DVS_DEFAULT);
    void ExpandColumnsToFit(BOOL bExpandFixed = TRUE);
    void ExpandLastColumn();
    void ExpandRowsToFit(BOOL bExpandFixed = TRUE);
    void ExpandToFit(BOOL bExpandFixed = TRUE);

    void Refresh();
    void AutoFill();        // Fill with blank cells

    void EnsureVisible(CCellID &cell) { EnsureVisible(cell.row, cell.col); }
    void EnsureVisible(int_t nRow, int_t nCol);
    BOOL IsCellVisible(int_t nRow, int_t nCol);
    BOOL IsCellVisible(CCellID cell);
    BOOL IsCellEditable(int_t nRow, int_t nCol);
    BOOL IsCellEditable(CCellID &cell);
    BOOL IsCellSelected(int_t nRow, int_t nCol);
    BOOL IsCellSelected(CCellID &cell);
    BOOL IsColumnSelected(int_t nCol);
    BOOL IsRowSelected(int_t nRow);
    BOOL IsAllCellsSelected() { return m_bAllCellsSelected; }

    BOOL RedrawCell(int_t nRow, int_t nCol, CDC* pDC = NULL, BOOL bInEdit = FALSE);
    BOOL RedrawCell(const CCellID& cell, CDC* pDC = NULL, BOOL bInEdit = FALSE);
    BOOL RedrawRow(int_t row);
    BOOL RedrawColumn(int_t col);

public:
    int_t m_nSelectedColumn;
    CCellRange GetCellRange();
    CCellRange GetSelectedCellRange();
    void SetSelectedRange(const CCellRange& Range, BOOL bSelectCells = TRUE, UINT nFlags = 0);
    void SetSelectedRange(int_t nMinRow, int_t nMinCol, int_t nMaxRow, int_t nMaxCol, BOOL bSelectCells = TRUE, UINT nFlags = 0);
    BOOL isValid(int_t nRow, int_t nCol);
    BOOL isValid(const CCellID& cell);
    BOOL isValid(const CCellRange& range);

    void CutSelectedText();
    COleDataSource* CopyTextFromDatasheet();
    BOOL PasteTextToDatasheet(CCellID cell, COleDataObject* pDataObject, BOOL bSelectPastedCells = TRUE);
    void OnEditCut();
    void OnEditCopy();
    void OnEditClear();
    void OnEditPaste();
    void OnEditSelectAll();

public:

    void SetCompareFunction(PFNLVCOMPARE pfnCompare);

public:
    void Print(CPrintDialog* pPrintDialog = NULL);

    // Use -1 to have it keep the existing value
    void SetPrintMarginInfo(int_t nHeaderHeight, int_t nFooterHeight,
        int_t nLeftMargin, int_t nRightMargin, int_t nTopMargin,
        int_t nBottomMargin, int_t nGap);

    void GetPrintMarginInfo(int_t &nHeaderHeight, int_t &nFooterHeight,
        int_t &nLeftMargin, int_t &nRightMargin, int_t &nTopMargin,
        int_t &nBottomMargin, int_t &nGap);

public:
    void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    void OnPrint(CDC *pDC, CPrintInfo *pInfo);
    void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);

    void ResetScrollBars();

// Implementation
public:
    ~CDatasheet();

protected:
    BOOL RegisterWindowClass();
    BOOL Initialize();
    void SetupDefaultCells();

    BOOL InvalidateCellRect(const int_t row, const int_t col);
    BOOL InvalidateCellRect(const CCellID& cell);
    BOOL InvalidateCellRect(const CCellRange& cellRange);
    void EraseBkgnd(CDC* pDC);

    BOOL GetCellRangeRect(const CCellRange& cellRange, LPRECT lpRect);

    int_t SetMouseMode(int_t nMode) { int_t nOldMode = m_MouseMode; m_MouseMode = nMode; return nOldMode; }
    int_t GetMouseMode() { return m_MouseMode; }

    BOOL MouseOverRowResizeArea(CPoint& tPoint);
    BOOL MouseOverColumnResizeArea(CPoint& tPoint);

    CCellID GetTopleftNonFixedCell(BOOL bForceRecalculation = FALSE);
    CCellRange GetVisibleNonFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE, int *piWidth = NULL, int *piHeight = NULL);
    CCellRange GetVisibleFixedCellRange(LPRECT pRect = NULL, BOOL bForceRecalculation = FALSE);

    BOOL IsVisibleVScroll() { return ( (m_nBarState & DVL_VERT) > 0); }
    BOOL IsVisibleHScroll() { return ( (m_nBarState & DVL_HORZ) > 0); }
    void EnableScrollBars(int_t nBar, BOOL bEnable = TRUE);
    int_t GetScrollPos32(int_t nBar, BOOL bGetTrackPos = FALSE);
    BOOL SetScrollPos32(int_t nBar, int_t nPos, BOOL bRedraw = TRUE);

    CPoint GetPointClicked(int_t nRow, int_t nCol, const CPoint& tPoint);

    void ValidateAndModifyCellContents(int_t nRow, int_t nCol, LPCTSTR szText);

public:
    void ResetSelectedRange();

    // Editing
    CCellID GetNextCell(UINT nChar);
    BOOL GotoToNextCell(UINT nChar);
    void OnEditCell(int_t nRow, int_t nCol, CPoint tPoint, UINT nChar);
    BOOL Edit(int_t nRow, int_t nCol, CRect rcT, CPoint /* tPoint */, UINT nID, UINT nChar);
    void EndEdit();
    void DestroyEdit(int_t nRow, int_t nCol, UINT uiLastChar, LPCTSTR pszNewText);
    BOOL IsEditing(int_t nRow, int_t nCol);
    BOOL IsEditing();

// Overrrides
protected:
    void PreSubclassWindow();

protected:
    std::map<DWORD, CCellID> m_SelectedCellMap, m_PrevSelectedCellMap;

    // Printing
    void PrintFixedRowCells(int_t nStartColumn, int_t nStopColumn, int_t& nRow, CRect& rcT, CDC *pDC, BOOL& bFirst);
    void PrintColumnHeadings(CDC *pDC, CPrintInfo *pInfo);
    void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
    void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
    void PrintRowButtons(CDC *pDC, CPrintInfo* /*pInfo*/);

    // Mouse Clicks
    void OnFixedColumnClick(CCellID& cell);
    void OnFixedRowClick(CCellID& cell);

    // Drawing
    void  OnDraw(CDC* pDC);

// Attributes
protected:

    // General attributes
    static const COLORREF DATASHEET_BGCOLOR;
    static const COLORREF DATASHEET_GRIDCOLOR;

    COLORREF m_crWindowText, m_crWindowColour, m_cr3DFace, m_crShadow;        // System colours

    int_t m_nGridLines;
    BOOL m_bEditable;
    BOOL m_bModified;
    BOOL m_bSingleRowSelection;
    BOOL m_bSingleColSelection;
    BOOL m_bEnableSelection;
    BOOL m_bFixedRowSelection, m_bFixedColumnSelection;
    BOOL m_bHandleTabKey;
    BOOL m_bDoubleBuffer;
    int_t m_nBarState;
    BOOL m_bWysiwygPrinting;
    BOOL m_bHiddenColUnhide, m_bHiddenRowUnhide;
    BOOL m_bAllowColHide, m_bAllowRowHide;
    BOOL m_bAutoSizeSkipColHdr;
    BOOL m_bTrackFocusCell;
    BOOL m_bFrameFocus;
    UINT m_nAutoSizeColumnStyle;
    BOOL m_bAllCellsSelected;

    // Cell size details
    int_t m_nRows, m_nFixedRows, m_nCols, m_nFixedCols;
    CUIntArray m_arRowHeights, m_arColWidths;
    int_t m_nVScrollMax, m_nHScrollMax;

    // Fonts and images
    CRuntimeClass* m_pRtcDefault;                    // determines kind of Datasheet Cell created by default
    CCellDefault m_cellDefault;                    // "default" cell. Contains default colours, font etc.
    CCellDefault m_cellFixedColDef, m_cellFixedRowDef, m_cellFixedRowColDef;
    CFont m_PrinterFont;                                // for the printer

    // Mouse operations such as cell selection
    int_t m_MouseMode;
    BOOL m_bLMouseButtonDown, m_bRMouseButtonDown;
    CPoint m_LeftClickDownPoint, m_LastMousePoint;
    CCellID m_LeftClickDownCell, m_SelectionStartCell;
    CCellID m_idCurrentCell, m_idTopLeftCell;
    int_t m_nResizeCaptureRange;
    BOOL m_bAllowRowResize, m_bAllowColumnResize;
    int_t m_nRowsPerWheelNotch;

    // Drag and drop
    CCellID m_LastDragOverCell;

    // Printing information
    CSize m_CharSize;
    int_t m_nPageHeight;
    CSize m_LogicalPageSize,                    // Page size in datasheet units.
            m_PaperSize;                            // Page size in device units.
    // additional properties to support Wysiwyg printing
    int_t m_nPageWidth;
    int_t m_nPrintColumn;
    int_t m_nCurrPrintRow;
    int_t m_nNumPages;
    int_t m_nPageMultiplier;

    // Added support for user-definable margins.  Top and bottom are in 
    // lines.  Left, right, and gap are in characters (avg width is used).
    int_t m_nHeaderHeight, m_nFooterHeight, m_nLeftMargin,
        m_nRightMargin, m_nTopMargin, m_nBottomMargin, m_nGap;

    int_t m_nMinWidth;
    int_t m_nMinHeight;

public:
    void SelectColumns(CCellID currentCell, BOOL bSelectCells = TRUE, UINT nFlags = 0);

protected:
    void SelectAllCells();
    void SelectRows(CCellID currentCell, BOOL bSelectCells = TRUE, UINT nFlags = 0);
    void SelectCells(CCellID currentCell, BOOL bSelectCells = TRUE, UINT nFlags = 0);
    void OnSelecting(const CCellID& currentCell);

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // Generated message map functions
    afx_msg void OnPaint();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int_t cx, int_t cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint tPoint);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint tPoint);
    afx_msg void OnMouseMove(UINT nFlags, CPoint tPoint);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint tPoint);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint tPoint);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint tPoint);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnImeChar(WPARAM wCharCode, LPARAM lParam);
    afx_msg void OnEnChangeNote();

    DECLARE_MESSAGE_MAP()

    enum eMouseModes {    MOUSE_NOTHING, MOUSE_SELECT_ALL, MOUSE_SELECT_COL, MOUSE_SELECT_ROW,
                                MOUSE_SELECT_CELLS, MOUSE_SCROLLING_CELLS,
                                MOUSE_OVER_ROW_DIVIDE, MOUSE_SIZING_ROW,
                                MOUSE_OVER_COL_DIVIDE, MOUSE_SIZING_COL,
                                MOUSE_PREPARE_EDIT, };

// for sort in mode, and column order, save and load layer
public:
    typedef std::vector<int_t> intlist;
    intlist m_arRowOrder, m_arColOrder;

private:
    void ResetVirtualOrder();
    bool m_InDestructor;

public:
    void UpdateColumnMenu(CCmdUI* pCmdUI);
    void UpdateRowMenu(CCmdUI* pCmdUI);
    void UpdateCellMenu(CCmdUI* pCmdUI);

    afx_msg void OnUpdateDataImport(CCmdUI *pCmdUI);
    afx_msg void OnUpdateDataImportDlg(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePlotAddCurve(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAppendRow(CCmdUI *pCmdUI);

    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
    afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);
    HICON GetIconForItem(UINT itemID) const;

    vector_t* InsertColumn(int_t nCol, int_t nDim = 0);
    afx_msg void OnInsertColumn();
    afx_msg void OnUpdateInsertColumn(CCmdUI* pCmdUI);

    void DeleteColumn(int_t nColumn);
    afx_msg void OnDeleteColumn();
    afx_msg void OnUpdateDeleteColumn(CCmdUI* pCmdUI);

    inline vector_t* AppendColumn();
    afx_msg void OnAppendColumn();
    afx_msg void OnUpdateAppendColumn(CCmdUI* pCmdUI);

    int_t InsertRows(int_t nRow, int_t nCount = 1);
    afx_msg void OnInsertRow();
    afx_msg void OnUpdateInsertRow(CCmdUI* pCmdUI);

    void DeleteRow(int_t nRow, BOOL bRefresh = TRUE);
    afx_msg void OnDeleteRow();
    afx_msg void OnUpdateDeleteRow(CCmdUI* pCmdUI);

    int_t AppendRows(int_t iCount);
    afx_msg void OnAppendRow();
    afx_msg void OnAutoResize();

    afx_msg void OnSetColumnValues();
    afx_msg void OnUpdateSetColumnValues(CCmdUI* pCmdUI);

    afx_msg void OnCreateSeries();
    afx_msg void OnUpdateCreateSeries(CCmdUI* pCmdUI);

    afx_msg void OnSortAsc();
    afx_msg void OnUpdateSortAsc(CCmdUI* pCmdUI);

    afx_msg void OnSortDesc();
    afx_msg void OnUpdateSortDesc(CCmdUI* pCmdUI);

    afx_msg void OnMask();
    afx_msg void OnUpdateMask(CCmdUI* pCmdUI);

    afx_msg void OnUnmask();
    afx_msg void OnUpdateUnmask(CCmdUI* pCmdUI);

    afx_msg void OnInvertmask();
    afx_msg void OnUpdateInvertmask(CCmdUI* pCmdUI);

    afx_msg void OnStats();
    afx_msg void OnUpdateStats(CCmdUI* pCmdUI);

    afx_msg void OnColumnProperties();
    afx_msg void OnUpdateColumnProperties(CCmdUI* pCmdUI);
    afx_msg void OnPlotAddCurve();
    afx_msg void OnEditUndo();
    afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
    afx_msg void OnEditRedo();
    afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);

    // Op�rations d'importation de donn�es ASCII
    afx_msg void OnDataImport();
    afx_msg void OnDataImportDlg();
};

// Returns the default cell implementation for the given region
__inline CCell* CDatasheet::GetDefaultCell(BOOL bFixedRow, BOOL bFixedCol)
{
    if (bFixedRow && bFixedCol) { return (CCell*) &m_cellFixedRowColDef; }
    if (bFixedRow) { return (CCell*) &m_cellFixedRowDef; }
    if (bFixedCol) { return (CCell*) &m_cellFixedColDef; }

    return (CCell*) &m_cellDefault;
}

__inline CCell* CDatasheet::GetCell(int_t nRow, int_t nCol, CCell* pCell, CELL_DISPINFO *pDispInfo)
{
    if ((pCell == NULL) || (pDispInfo == NULL) || (nRow < 0) || (nRow >= m_nRows) || (nCol < 0) || (nCol >= m_nCols)) {
        return NULL;
    }

    CCell* pDefCell = GetDefaultCell(nRow < m_nFixedRows, nCol < m_nFixedCols);

    pDispInfo->item.row = nRow;
    pDispInfo->item.col = nCol;
    pDispInfo->item.mask = 0xFFFFFFFF;
    pDispInfo->item.nState = 0;
    pDispInfo->item.nFormat = pDefCell->GetFormat();
    pDispInfo->item.crBkClr = pDefCell->GetBackClr();
    pDispInfo->item.crFgClr = pDefCell->GetTextClr();
    pDispInfo->item.lParam = pDefCell->GetData();
    memcpy(&(pDispInfo->item.lfFont), pDefCell->GetFont(), sizeof(LOGFONT));
    pDispInfo->item.nMargin = pDefCell->GetMargin();
    _tcscpy(pDispInfo->item.szText, _T(""));

    // Fix the state bits
    if (IsCellSelected(nRow, nCol)) { pDispInfo->item.nState |= DVIS_SELECTED; }
    if (nRow < GetFixedRowCount()) { pDispInfo->item.nState |= (DVIS_FIXED | DVIS_FIXEDROW); }
    if (nCol < GetFixedColumnCount()) { pDispInfo->item.nState |= (DVIS_FIXED | DVIS_FIXEDCOL); }
    if (GetFocusCell() == CCellID(nRow, nCol)) { pDispInfo->item.nState |= DVIS_FOCUSED; }
    if (!m_InDestructor) {
        pDispInfo->item.row = m_arRowOrder[nRow];
        pDispInfo->item.col = m_arColOrder[nCol];
        byte_t bMask = 0x00;
        GetCellText(nCol, nRow, (char_t*)(pDispInfo->item.szText), &bMask);
        if ((bMask & 0x0F) != 0x0F) {
            if ((bMask & 0x0F) == 0x01) {    // Masked item
                pDispInfo->item.crBkClr = pDefCell->GetBackClr();
                pDispInfo->item.crFgClr = RGB(255,0,0);
            }
        }
        pDispInfo->item.row = nRow;
        pDispInfo->item.col = nCol;
    }

    pCell->SetDatasheet((CDatasheet*)this);
    pCell->SetState(pDispInfo->item.nState);
    pCell->SetFormat(pDispInfo->item.nFormat);
    pCell->SetBackClr(pDispInfo->item.crBkClr);
    pCell->SetTextClr(pDispInfo->item.crFgClr);
    pCell->SetData(pDispInfo->item.lParam);
    pCell->SetFont(&(pDispInfo->item.lfFont));
    pCell->SetMargin(pDispInfo->item.nMargin);
    pCell->SetText((LPCTSTR)(pDispInfo->item.szText));

    return pCell;
}
