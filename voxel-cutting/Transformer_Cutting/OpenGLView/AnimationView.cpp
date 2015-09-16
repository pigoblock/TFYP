#include "stdafx.h"
#include "AnimationView.h"


IMPLEMENT_DYNCREATE(AnimationView, CView)
	BEGIN_MESSAGE_MAP(AnimationView, CView)
		ON_WM_CREATE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_RBUTTONDOWN()
		ON_WM_RBUTTONUP()
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSEWHEEL()
	END_MESSAGE_MAP()

AnimationView::AnimationView()
{
}


AnimationView::~AnimationView()
{
}

void AnimationView::OnDraw(CDC *pDC)
{

}

void AnimationView::OnLButtonDown(UINT nFlags, CPoint point)
{
	LEFT_DOWN = true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnLButtonDown(nFlags, point);
}

void AnimationView::OnLButtonUp(UINT nFlags, CPoint point)
{
	LEFT_DOWN = false;
	CView::OnLButtonUp(nFlags, point);
}

void AnimationView::OnRButtonDown(UINT nFlags, CPoint point)
{
	RIGHT_DOWN = true;
	m_PreMousePos.x = point.x;
	m_PreMousePos.y = -point.y;
	CView::OnRButtonDown(nFlags, point);
}

void AnimationView::OnRButtonUp(UINT nFlags, CPoint point)
{
	RIGHT_DOWN = false;
	CView::OnRButtonUp(nFlags, point);
}

// Rotation and moving the view
void AnimationView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_MousePos.x = point.x;
	m_MousePos.y = -point.y;
	m_DMousePos = m_MousePos - m_PreMousePos;

	if (LEFT_DOWN){
		m_Camera.RotCamPos(m_DMousePos);
	}
	else if (RIGHT_DOWN){
		m_Camera.MoveCamPos(m_DMousePos);
	}

	m_PreMousePos = m_MousePos;
	CView::OnMouseMove(nFlags, point);
}

// Zooming the view
BOOL AnimationView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	vec3d temp;

	m_Camera.m_Distance -= zDelta*m_Camera.m_Distance*0.001;
	m_Camera.RotCamPos(temp);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
