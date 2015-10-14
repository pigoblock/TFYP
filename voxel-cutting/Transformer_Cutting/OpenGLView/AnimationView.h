#pragma once

#include "KEGIESDoc.h"

enum DisplayMode{
	SHOW_AXIS,
	SHOW_CUT_MESH,
	SHOW_SKELETON_AND_JOINTS,
	DISPLAY_MODE_SIZE
};

class AnimationView : public CView
{
protected:
	AnimationView();
	DECLARE_DYNCREATE(AnimationView);
	DECLARE_MESSAGE_MAP();

public:
	~AnimationView();

	// Initialize
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void InitGL();
	virtual void OnInitialUpdate();

	HDC     m_hDC;
	HGLRC   m_hRC;
	GLuint	base;

	// Window
	int m_WindowHeight;
	int m_WindowWidth;

	// Drawing functions
	virtual void OnDraw(CDC *pDC);
	void DrawView();
	void SetupShadersAndLight();
	void UpdateCameraView();
	void drawAxis(bool atOrigin, CCamera *cam);

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

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// Shortcut Keys
	bool animViewDisplayMode[DISPLAY_MODE_SIZE];
	bool animationMode[ANIMATION_MODE_SIZE];
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	CKEGIESDoc* GetDocument() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

// if _DEBUG is not defined, will use GetDocument() function
#ifndef _DEBUG 
inline CKEGIESDoc* AnimationView::GetDocument() const
{
	return reinterpret_cast<CKEGIESDoc*>(m_pDocument);
}
#endif

