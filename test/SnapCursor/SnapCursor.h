//////////////////////////////////////////////////////////////////////////////
//
// SnapCursor.h: declaration of the CSnapCursor class.
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

#if !defined(AFX_SNAPCURSOR_H__675A2FC2_27F5_4771_8C57_686CE7567F0C__INCLUDED_)
#define AFX_SNAPCURSOR_H__675A2FC2_27F5_4771_8C57_686CE7567F0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSnapCursor
{
private:
	int      m_nSnapIncrement;
	CPoint   m_OldSnapPoint;
    CDC     *m_pDC;
	BOOL     m_bSnapOn;
    BOOL     m_bEraseOnMove;
    int      m_nX_offset;
    int      m_nY_offset;
public:
	void SetSnapOffset(int nX_offset, int nY_offset);
    void Reset();
	void Enable(BOOL bOn);
	void SetSnapIncrement(int nIncrement);
	void GetFirstSnapPos(CPoint *point);
	void Draw(CDC *pDC, CPoint *point);
	CSnapCursor();
	virtual ~CSnapCursor();
};

#endif // !defined(AFX_SNAPCURSOR_H__675A2FC2_27F5_4771_8C57_686CE7567F0C__INCLUDED_)
