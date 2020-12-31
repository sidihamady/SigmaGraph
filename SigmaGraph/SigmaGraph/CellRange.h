///////////////////////////////////////////////////////////////////////
// CellRange.h: header file
//
// MFC Grid Control - interface for the CCellRange class.
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright(C) 1998-2002. All Rights Reserved.
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
//   Copyright(C) 1998-2007 Sidi OULD SAAD HAMADY
//   http://www.hamady.org
//   sidi@hamady.org
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../LibGraph/LibFile/libfile.h"

class CCellID
{
    // Attributes
public:
    int_t row, col;

    // Operations
public:
    explicit CCellID(int_t nRow = -1, int_t nCol = -1) : row(nRow), col(nCol) {}

    int_t isValid() const { return ((row >= 0) && (col >= 0)); }
    int_t operator==(const CCellID& rhs) const { return ((row == rhs.row) && (col == rhs.col)); }
    int_t operator!=(const CCellID& rhs) const { return !operator==(rhs); }
    bool operator<(const CCellID& rhs) const
    {
        if (row < rhs.row) {
            return true;
        }
        else  if (row == rhs.row) {
            return (col < rhs.col);
        }

        return false;
    }
    CCellID& operator=(const CCellID& rhs)
    {
        if (this == &rhs) {
            return *this;
        }

        row = rhs.row;
        col = rhs.col;
        return *this;
    };
};

class CCellRange
{
public:

    CCellRange(int_t nMinRow = -1, int_t nMinCol = -1, int_t nMaxRow = -1, int_t nMaxCol = -1)
    {
        Set(nMinRow, nMinCol, nMaxRow, nMaxCol);
    }

    void Set(int_t nMinRow = -1, int_t nMinCol = -1, int_t nMaxRow = -1, int_t nMaxCol = -1);

    int_t isValid() const;
    int_t InRange(int_t row, int_t col) const;
    int_t InRange(const CCellID& cellID) const;
    int_t Count() { return (m_nMaxRow - m_nMinRow + 1) * (m_nMaxCol - m_nMinCol + 1); }

    CCellID  GetTopLeft() const;
    CCellRange  Intersect(const CCellRange& rhs) const;

    int_t GetMinRow() const { return m_nMinRow; }
    void SetMinRow(int_t minRow) { m_nMinRow = minRow; }

    int_t GetMinCol() const { return m_nMinCol; }
    void SetMinCol(int_t minCol) { m_nMinCol = minCol; }

    int_t GetMaxRow() const { return m_nMaxRow; }
    void SetMaxRow(int_t maxRow) { m_nMaxRow = maxRow; }

    int_t GetMaxCol() const { return m_nMaxCol; }
    void SetMaxCol(int_t maxCol) { m_nMaxCol = maxCol; }

    int_t GetRowSpan() const { return m_nMaxRow - m_nMinRow + 1; }
    int_t GetColSpan() const { return m_nMaxCol - m_nMinCol + 1; }

    void operator=(const CCellRange& rhs);
    int_t operator==(const CCellRange& rhs);
    int_t operator!=(const CCellRange& rhs);

protected:
    int_t m_nMinRow;
    int_t m_nMinCol;
    int_t m_nMaxRow;
    int_t m_nMaxCol;
};

inline void CCellRange::Set(int_t minRow, int_t minCol, int_t maxRow, int_t maxCol)
{
    m_nMinRow = minRow;
    m_nMinCol = minCol;
    m_nMaxRow = maxRow;
    m_nMaxCol = maxCol;
}

inline void CCellRange::operator=(const CCellRange& rhs)
{
    if (this != &rhs) Set(rhs.m_nMinRow, rhs.m_nMinCol, rhs.m_nMaxRow, rhs.m_nMaxCol);
}

inline int_t CCellRange::operator==(const CCellRange& rhs)
{
    return ((m_nMinRow == rhs.m_nMinRow) && (m_nMinCol == rhs.m_nMinCol) && (m_nMaxRow == rhs.m_nMaxRow) && (m_nMaxCol == rhs.m_nMaxCol));
}

inline int_t CCellRange::operator!=(const CCellRange& rhs)
{
    return !operator==(rhs);
}

inline int_t CCellRange::isValid() const
{
    return (m_nMinRow >= 0 && m_nMinCol >= 0 && m_nMaxRow >= 0 && m_nMaxCol >= 0 && m_nMinRow <= m_nMaxRow && m_nMinCol <= m_nMaxCol);
}

inline int_t CCellRange::InRange(int_t row, int_t col) const
{
    return ((row >= m_nMinRow) && (row <= m_nMaxRow) && (col >= m_nMinCol) && (col <= m_nMaxCol));
}

inline int_t CCellRange::InRange(const CCellID& cellID) const
{
    return InRange(cellID.row, cellID.col);
}

inline CCellID CCellRange::GetTopLeft() const
{
    return CCellID(m_nMinRow, m_nMinCol);
}

inline CCellRange CCellRange::Intersect(const CCellRange& rhs) const
{
    return CCellRange(max(m_nMinRow,rhs.m_nMinRow), max(m_nMinCol,rhs.m_nMinCol), min(m_nMaxRow,rhs.m_nMaxRow), min(m_nMaxCol,rhs.m_nMaxCol));
}

