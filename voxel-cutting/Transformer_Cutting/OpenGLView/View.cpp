#include "stdafx.h"
#include "View.h"


View::View()
{
}


View::~View()
{
}

void View::OnLButtonDown(UINT nFlags, CPoint point)
{
	LEFT_DOWN = true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnLButtonDown(nFlags, point);
}

void View::OnLButtonUp(UINT nFlags, CPoint point)
{
	LEFT_DOWN = false;
	CView::OnLButtonUp(nFlags, point);
}

void View::OnRButtonDown(UINT nFlags, CPoint point)
{
	RIGHT_DOWN = true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnRButtonDown(nFlags, point);
}

void View::OnRButtonUp(UINT nFlags, CPoint point)
{
	RIGHT_DOWN = false;
	CView::OnRButtonUp(nFlags, point);
}

void View::OnMouseMove(UINT nFlags, CPoint point)
{

}

BOOL View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}