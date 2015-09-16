#pragma once

class View : public CView
{
public:
	View();
	~View();

	// Mouse left/right click flags
	bool LEFT_DOWN;
	bool RIGHT_DOWN;

	// Mouse position
	vec3d m_MousePos;
	vec3d m_PreMousePos;
	vec3d m_DMousePos;

	// Mouse functions
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

