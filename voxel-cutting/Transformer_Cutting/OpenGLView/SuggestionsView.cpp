#include "StdAfx.h"
#include "SuggestionsView.h"

#include "KEGIESDoc.h"
#include "KEGIESView.h"
#include "MainFrm.h"
#include "GL\glut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(SuggestionsView, CView)
BEGIN_MESSAGE_MAP(SuggestionsView, CView)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

SuggestionsView::SuggestionsView(void)
{
}

SuggestionsView::~SuggestionsView()
{

}

void SuggestionsView::OnDraw(CDC* pDC)
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc){
		return;
	}

	// Credit: https://www.experts-exchange.com/questions/20534656/MFC-Double-buffering.html
	// Get clip box
	CRect rcClipBox;
	pDC->GetClipBox(rcClipBox);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	memDC.SetMapMode(pDC->GetMapMode());
	memDC.SetViewportOrg(pDC->GetViewportOrg());
	memDC.IntersectClipRect(rcClipBox);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rcClipBox.Width(), rcClipBox.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);

	// Draw to memDC
	memDC.FillSolidRect(rcClipBox, GetSysColor(COLOR_WINDOW));
	DrawView(&memDC);

	// Blit to the original dc
	pDC->BitBlt(rcClipBox.left, rcClipBox.top, rcClipBox.Width(), rcClipBox.Height(), &memDC, rcClipBox.left, rcClipBox.top, SRCCOPY);

	// Restore old bitmap
	memDC.SelectObject(pOldBmp);
}

void SuggestionsView::DrawView(CDC* pDC)
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc){
		return;
	}

	pDoc->document.drawSuggestionsView(pDC);
}

BOOL SuggestionsView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

BOOL SuggestionsView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

#ifdef _DEBUG
void SuggestionsView::AssertValid() const
{
	CView::AssertValid();
}

void SuggestionsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKEGIESDoc* SuggestionsView::GetDocument() const // 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKEGIESDoc)));
	return (CKEGIESDoc*)m_pDocument;
}

#endif //_DEBUG

int SuggestionsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1){
		return -1;
	}

	SetTimer(TIMER_UPDATE_VIEW, 10, NULL);

	return 0;
}

void SuggestionsView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void SuggestionsView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CSize size(cx, cy);
	m_WindowHeight = size.cy;
	m_WindowWidth = size.cx;
}

void SuggestionsView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_UPDATE_VIEW){
		InvalidateRect(NULL, FALSE);
	}
	CView::OnTimer(nIDEvent);
}

/*
void SuggestionsView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView){
	if (bActivate){
		std::cout << "suggestions view activated" << std::endl;
	}
}
*/