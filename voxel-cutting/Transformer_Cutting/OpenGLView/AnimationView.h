#pragma once

class AnimationView : public CView
{
protected:
	AnimationView();
	DECLARE_DYNCREATE(AnimationView);
	DECLARE_MESSAGE_MAP();

public:
	virtual void OnDraw(CDC *pDC);

	~AnimationView();
};

