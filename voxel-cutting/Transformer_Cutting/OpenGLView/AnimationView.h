#pragma once

#include "KEGIESDoc.h"

enum DisplayMode{
	SHOW_SKELETON,
	SHOW_MESH,
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
	bool showAxis;
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

	//Mouse functions
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// Toolbar functions
	afx_msg void OnPlayBtn();
	afx_msg void OnRestartBtn();
	afx_msg void OnSpeedBtns(UINT nID);
	afx_msg void OnViewBtns(UINT nID);

	// Values to parse to MainControl
	bool animViewDisplayMode[DISPLAY_MODE_SIZE];
	int animationMode;
	float animationSpeed;

	// Shortcut Keys
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

