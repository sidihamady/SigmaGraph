//////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//           25 feb 98 - fixed minor assertion bug
//
// This class implements a memory Device Context
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Modifications Copyright(C) 1998 Sidi OULD SAAD HAMADY
// http://www.hamady.org
// sidi@hamady.org
//////////////////////////////////////////////////

#pragma once

class CSigmaDC : public CDC
{
private:
    CBitmap m_Bitmap;                       // Offscreen bitmap
    CBitmap* m_pOldBitmap;                  // Bitmap originally found in CSigmaDC
    CDC* m_pDC;                             // Saves CDC passed in constructor
    CRect m_Rect;                           // Rectangle of drawing area.
    BOOL m_bMemDC;                          // TRUE if CDC really is a Memory DC.

public:
    // >> Constructors
    CSigmaDC() : CDC()
    {
        m_pDC = NULL;
        m_pOldBitmap = NULL;
        m_bMemDC = TRUE;
        m_hDC = NULL;
        m_hAttribDC = NULL;
    }
    CSigmaDC(CDC* pDC) : CDC()
    {
        m_pDC = NULL;
        m_pOldBitmap = NULL;
        m_bMemDC = TRUE;
        m_hDC = NULL;
        m_hAttribDC = NULL;
        
        SetDC(pDC);
    }
    // <<

    // >> Initialize DC
    void SetDC(CDC* pDC)
    {
        if (m_pDC) {
            return;
        }
        ASSERT(pDC != NULL);
        if (pDC == NULL) {
            return;
        }

        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = (pDC->IsPrinting() == FALSE);

        if (m_bMemDC) {
            m_pDC->GetClipBox(&m_Rect);
            CreateCompatibleDC(m_pDC);        // Create a Memory DC
            m_Bitmap.CreateCompatibleBitmap(m_pDC, m_Rect.Width(), m_Rect.Height());
            m_pOldBitmap = SelectObject(&m_Bitmap);
            SetWindowOrg(m_Rect.left, m_Rect.top);

            // Bug fix - Fill background in case the user has overridden
            // WM_ERASEBKGND.  We end up with garbage otherwise.
            // moved to fix a bug in the fix.
            FillSolidRect(m_Rect, m_pDC->GetBkColor());
        }
        else {                    // Make a copy of the relevent parts of the current DC for printing
            m_bPrinting = m_pDC->m_bPrinting;
            m_hDC = m_pDC->GetSafeHdc();
            m_hAttribDC = m_pDC->m_hAttribDC;
        }
    }
    // <<

    // >> Draw to screen
    void DrawIt()
    {
        if (m_pDC) {
            if (m_bMemDC) {
                // Copy the offscreen bitmap onto the screen.
                m_pDC->BitBlt(m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height(), this, m_Rect.left, m_Rect.top, SRCCOPY);
                // Swap back the original bitmap.
                SelectObject(m_pOldBitmap);
                m_Bitmap.DeleteObject();
                DeleteDC();
            }
        }
        m_pDC = NULL;
        m_pOldBitmap = NULL;
        m_bMemDC = TRUE;
        m_hDC = NULL;
        m_hAttribDC = NULL;
    }
    // <<

    // >> Destructor copies the contents of the mem DC to the original DC
    ~CSigmaDC()
    {
        DrawIt();
    }
    // <<
};
