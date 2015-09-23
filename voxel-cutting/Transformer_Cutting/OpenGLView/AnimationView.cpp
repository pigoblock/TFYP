#include "stdafx.h"
#include "AnimationView.h"
#include "KEGIESDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(AnimationView, CView)
	BEGIN_MESSAGE_MAP(AnimationView, CView)
		ON_WM_CREATE()
		ON_WM_KEYDOWN()
		ON_WM_SIZE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_TIMER()
		ON_WM_RBUTTONDOWN()
		ON_WM_RBUTTONUP()
		ON_WM_MOUSEMOVE()
		ON_WM_MOUSEWHEEL()
		//ON_COMMAND(ID_COLOR_BACKGROUND, &View2::OnColorBackground)
	END_MESSAGE_MAP()

AnimationView::AnimationView()
{
}

AnimationView::~AnimationView()
{
}

int AnimationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1){
		return -1;
	}

	SetTimer(TIMER_UPDATE_VIEW, 10, NULL);

	return 0;
}

BOOL AnimationView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void AnimationView::InitGL()
{
	COpenGL Initgl;

	//Init
	Initgl.SetHWND(m_hWnd);
	Initgl.SetupPixelFormatAA(3);
	base = Initgl.base;

	m_hDC = Initgl.m_hDC;
	m_hRC = Initgl.m_hRC;

	m_Camera = AppSetting::loadcamera();	//returns a CCamera object

	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc){
		return;
	}
}

void AnimationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	LEFT_DOWN = false;
	RIGHT_DOWN = false;

	InitGL();
}

void AnimationView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_UPDATE_VIEW){
		InvalidateRect(NULL, FALSE);

		CKEGIESDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
	}
	CView::OnTimer(nIDEvent);
}

void AnimationView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CSize size(cx, cy);
	m_WindowHeight = size.cy;
	m_WindowWidth = size.cx;
}

void AnimationView::OnDraw(CDC *pDC)
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc){
		return;
	}

	wglMakeCurrent(m_hDC, m_hRC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	arrayVec3f bColor = { Vec3f(0, 0, 0), Vec3f(0.7, 0.7, 0.7), Vec3f(1, 1, 1) };
	glClearColor(bColor[2][0], bColor[2][1], bColor[2][1], 1);

	DrawView();

	SwapBuffers(m_hDC);
}

void AnimationView::DrawView()
{
	CKEGIESDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc){
		return;
	}

	glPushMatrix();
		UpdateCameraView();	// Set up camera 
		SetupShadersAndLight();	

	//	if (m_displayMode[0]){
			drawAxis(true, &m_Camera);
	//	}
		pDoc->document.drawAnimationView();
	glPopMatrix();
	glPopAttrib();
}

void AnimationView::drawAxis(bool atOrigin, CCamera* cam)
{
	glPushMatrix();
		float length = 0.5*cam->m_Distance;
		// Re-orientate
		if (!atOrigin){
			float textPosX = -0.5*(m_WindowWidth / m_WindowHeight)*cam->m_Distance / 1.4;
			float textPosY = -0.5*cam->m_Distance / 1.4;
			float textPosZ = 0.0*cam->m_Distance;
			vec3d textPos = vec3d(textPosX, textPosY, textPosZ);

			matrix rotateM = cam->m_RotMatrix;
			textPos = rotateM.mulVector(textPos);
			glTranslatef(cam->m_Center.x, cam->m_Center.y, cam->m_Center.z);
			glTranslatef(textPos.x, textPos.y, textPos.z);

			length = 0.05*cam->m_Distance;
		}

		// Draws the axis
		glBegin(GL_LINES);
			glColor3f(1, 0, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(length, 0, 0);

			glColor3f(0, 1, 0);
			glVertex3f(0, 0, 0);
			glVertex3f(0, length, 0);

			glColor3f(0, 0, 1);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, length);
		glEnd();
	glPopMatrix();
}

void AnimationView::SetupShadersAndLight()
{
	GLfloat diffuseLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat ambientLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat position[] = { m_Camera.m_Pos.x, m_Camera.m_Pos.y, m_Camera.m_Pos.z, 0.0 };

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);

	glFrontFace(GL_CW);

	glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void AnimationView::UpdateCameraView()
{
	int _w = m_WindowWidth;
	int _h = m_WindowHeight;

	glViewport(0, 0, _w, _h);
	float fovy = 45;
	float aspect = float(_w) / float(_h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fovy, aspect, 1, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(m_Camera.m_Pos.x, m_Camera.m_Pos.y, m_Camera.m_Pos.z,
		m_Camera.m_Center.x, m_Camera.m_Center.y, m_Camera.m_Center.z,
		m_Camera.m_Up.x, m_Camera.m_Up.y, m_Camera.m_Up.z);
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

#ifdef _DEBUG
void AnimationView::AssertValid() const
{
	CView::AssertValid();
}

void AnimationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKEGIESDoc *AnimationView::GetDocument() const // 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKEGIESDoc)));
	return (CKEGIESDoc*)m_pDocument;
}

#endif //_DEBUG