// KEGIESDoc.h : CKEGIESDoc 
//
#pragma once
#include "resource.h"
#include "cutSurfTreeMngr2.h"
#include "detailSwapManager.h"
#include "coordAssignManager.h"
#include "groupCutManager.h"
#include "MainControl.h"

class CKEGIESDoc : public CDocument
{
public:
	MainControl document;
	appMode m_mode;
	CStringA filePath;	// Contains the surface mesh file path
	
protected: // serialization
	CKEGIESDoc();
	DECLARE_DYNCREATE(CKEGIESDoc)

public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();

	virtual void Serialize(CArchive& ar);

	virtual ~CKEGIESDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:

	CObj* objItem; //???

	BOOL openLastDoc();
	static DWORD StartThread (LPVOID param);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnToolbarStartthread();

	afx_msg void OnExtLoadobj();
	afx_msg void OnToolConvert();
	void test();
};


