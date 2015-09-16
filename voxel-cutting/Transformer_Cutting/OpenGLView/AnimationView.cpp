#include "stdafx.h"
#include "AnimationView.h"


IMPLEMENT_DYNCREATE(AnimationView, CView)
	BEGIN_MESSAGE_MAP(AnimationView, CView)
		ON_WM_CREATE()
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
