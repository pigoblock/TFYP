#pragma once
#include "KEGIESDoc.h"

class SuggestionsView : public CView
{
protected: // serialization
	SuggestionsView();
	DECLARE_DYNCREATE(SuggestionsView)

public:
	CKEGIESDoc* GetDocument() const;

	//Drawing
	void DrawView(CDC* pDC);

	//window
	int m_WindowHeight;
	int m_WindowWidth;

public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual ~SuggestionsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual void OnInitialUpdate();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//afx_msg void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};

// if _DEBUG is not defined, will use GetDocument() function
#ifndef _DEBUG  
inline CKEGIESDoc* SuggestionsView::GetDocument() const
{
	return reinterpret_cast<CKEGIESDoc*>(m_pDocument);
}
#endif