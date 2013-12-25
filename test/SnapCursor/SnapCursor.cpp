//////////////////////////////////////////////////////////////////////////////
//
// SnapCursor.cpp: implementation of the CSnapCursor class.
// 
// Code James P. Hawkins
// Mail: jim@jphawkins.com
// Web:  www.jphawkins.com
//
//////////////////////////////////////////////////////////////////////////////
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
//////////////////////////////////////////////////////////////////////////////
//
// History
// --------
// #v1.0
//	10/18/02:	Created
//
// #v1.01
//	mm/dd/yy:	None
//
// Note(s)
// -----
//
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SnapCursor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSnapCursor::CSnapCursor() :
     m_nSnapIncrement(10),
     m_bSnapOn(TRUE),
     m_bEraseOnMove(FALSE),
     m_nX_offset(0),
     m_nY_offset(0)
{
}

CSnapCursor::~CSnapCursor()
{

}

//
// Call this function to Disable or Enable Snap
// Default is Enabled
// Enable = TRUE
// Disable = FALSE
//
void CSnapCursor::Enable(BOOL bOn)
{
    m_bSnapOn = bOn;
}

//
// Call this function to set the snap-to-grid increment
// Default is 10 units
//
void CSnapCursor::SetSnapIncrement(int nIncrement)
{
    m_nSnapIncrement = nIncrement;
}

//
// Set snap offset.
// Used to align cursor snap to
// various grid configurations.
// For example, if snap increment is set to 50, major
// Grid marks start at something other than 50, the cursor
// is not going to line up with the major grid lines unless
// an offset is used.  This is tricky, because if you set
// an offset of 40, 40 and your increment is 5 or 10, its just
// going to make the cursor follow 50 units away from the Windows
// cursor.  These values default to 0, 0.
//
void CSnapCursor::SetSnapOffset(int nX_offset, int nY_offset)
{
    m_nX_offset = nX_offset;
    m_nY_offset = nY_offset;
}

//
// Call this function from OnLButtonDown() or OnRButtonDown() to compute the
// initial, "Snapped-to" coordinate to begin drawing an object
// such as a line, circle, etc.
//
void CSnapCursor::GetFirstSnapPos(CPoint *point)
{
    if(!m_bSnapOn)
        return;

    point->x = ((int)(point->x/m_nSnapIncrement) * m_nSnapIncrement) + m_nX_offset;
    point->y = ((int)(point->y/m_nSnapIncrement) * m_nSnapIncrement) + m_nY_offset;
}

//
// Call this function in OnMouseMove() before the point values are used.
// Use the passed in mouse position in "point.x point.y" to determine
// If the cursor needs to be redrawn at a new position.
// The calling function then uses the modified position for drawing.
// "Snap" a cursor to the next grid position.
// 
void CSnapCursor::Draw(CDC *pDC, CPoint *point)
{
    int old_mode;
    CPen cursorPen, oldPen;
    if(!m_bSnapOn)
        return;
                                                                                                                

    CPen* pOldPen = pDC->GetCurrentPen();

//    cursorPen.CreatePen(PS_SOLID, 1, m_cursorColor);
//	pDC->SelectObject(&cursorPen);

    // Snap the x,y if needed.
    point->x = ((int)(point->x/m_nSnapIncrement) * m_nSnapIncrement) + m_nX_offset;
    point->y = ((int)(point->y/m_nSnapIncrement) * m_nSnapIncrement) + m_nY_offset;

    //
    // Erase the old cursor and draw the new cursor 
    // The SetROP2(R2_XORPEN)
    // R2_NOT - Pixel is Inverted screen color
    //

    // Cross part
    old_mode = pDC->SetROP2(R2_NOT);

    if(m_bEraseOnMove)
    {
    pDC->MoveTo(m_OldSnapPoint.x - 10, m_OldSnapPoint.y);
    pDC->LineTo(m_OldSnapPoint.x + 10, m_OldSnapPoint.y);
    pDC->MoveTo(m_OldSnapPoint.x, m_OldSnapPoint.y - 10);
    pDC->LineTo(m_OldSnapPoint.x, m_OldSnapPoint.y + 10);
    
    // Box part
    pDC->MoveTo(m_OldSnapPoint.x - 5, m_OldSnapPoint.y - 5);
    pDC->LineTo(m_OldSnapPoint.x + 5, m_OldSnapPoint.y - 5);
    pDC->MoveTo(m_OldSnapPoint.x + 5, m_OldSnapPoint.y - 5);
    pDC->LineTo(m_OldSnapPoint.x + 5, m_OldSnapPoint.y + 5);
    pDC->MoveTo(m_OldSnapPoint.x + 5, m_OldSnapPoint.y + 5);
    pDC->LineTo(m_OldSnapPoint.x - 5, m_OldSnapPoint.y + 5);
    pDC->MoveTo(m_OldSnapPoint.x - 5, m_OldSnapPoint.y + 5);
    pDC->LineTo(m_OldSnapPoint.x - 5, m_OldSnapPoint.y - 5);
    }

    pDC->MoveTo(point->x - 10, point->y);
    pDC->LineTo(point->x + 10, point->y);
    pDC->MoveTo(point->x, point->y - 10);
    pDC->LineTo(point->x, point->y + 10);


    pDC->MoveTo(point->x - 5, point->y - 5);
    pDC->LineTo(point->x + 5, point->y - 5);
    pDC->MoveTo(point->x + 5, point->y - 5);
    pDC->LineTo(point->x + 5, point->y + 5);
    pDC->MoveTo(point->x + 5, point->y + 5);
    pDC->LineTo(point->x - 5, point->y + 5);
    pDC->MoveTo(point->x - 5, point->y + 5);
    pDC->LineTo(point->x - 5, point->y - 5);


    pDC->SetROP2(old_mode);

    //
    // Save the current cursor position as the old one,
    // so it can be erased on the next call if this function
    // if the cursor has to be moved.
    //
    m_OldSnapPoint = *point;
    m_bEraseOnMove = TRUE;

	pDC->SelectObject(&pOldPen);
}


void CSnapCursor::Reset()
{
    m_bEraseOnMove = FALSE;
}

