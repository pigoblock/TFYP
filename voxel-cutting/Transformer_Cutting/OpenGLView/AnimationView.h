#pragma once

#include "View.h"

class AnimationView : public CView
{
protected:
	AnimationView();
	DECLARE_DYNCREATE(AnimationView);
	DECLARE_MESSAGE_MAP();

public:
	virtual void OnDraw(CDC *pDC);

	~AnimationView();

	// Camera manipulation
	CCamera m_Camera;

	// Mouse click flag
	bool LEFT_DOWN;
	bool RIGHT_DOWN;

	// Mouse position
	vec3d m_MousePos;
	vec3d m_PreMousePos;
	vec3d m_DMousePos;

	//Mouse function
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

