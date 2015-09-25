#pragma once
#include "stdafx.h"
#include "voxelObject.h"
#include "Graphics\Surfaceobj.h"
#include "cutSurfTreeMngr2.h"
#include "detailSwapManager.h"
#include "groupCutManager.h"
#include "coordAssignManager.h"
#include "debugInfo.h"
#include "manipulateVoxel.h"
#include "processHoleMesh.h"
#include "FilterCutDialog.h"
#include "movePlacedBoxDlg.h"
#include "TransformerAnimation.h"

extern class CKEGIESView;

//#include "MeshCutting.h"
#define XML_INFOR				"infor"
#define XML_ORIGINAL_MESH_KEY "original_mesh_path_name"
#define XML_SKELETON_MESH_KEY "skeleton_mesh_path_name"
#define XML_MESH_PART			"mesh_cut_part"
#define XML_CUT_MESH_NAME		"polygon_name"
#define XML_BONE_NAME			"bone_name"
#define XML_COORD_MAP			"bone_mesh_coordinate_map"
#define XML_BONE_COORD_RESPECT_TO_WORLD "bone_coord_respect_to_world"
#define XML_ORIGIN				"origin"
#define XML_LOCAL_X_RESPECT_TO_WORLD	"local_x_repect_to_wolrd"
#define XML_LOCAL_Y_RESPECT_TO_WORLD	"local_y_repect_to_wolrd"
#define XML_LOCAL_Z_RESPECT_TO_WORLD	"local_z_repect_to_wolrd"

typedef enum
{
	MODE_FINDING_CUT_SURFACE,
	MODE_ASSIGN_COORDINATE,
	MODE_SWAP_VOXEL,
	MODE_SPLIT_BONE_GROUP,
	MODE_SWAP_FINAL,
	MODE_FIT_BONE,
	MODE_CUTTING_MESH,
	MODE_NONE,
	MODE_TEST
}appMode;

class myDocment
{
public:
	myDocment();
	~myDocment();

	// Draws for view1/KEGIESView
	void draw(BOOL mode[10]);

	// Draws for view2 (the one that shows the skeleton)
	void draw2(bool mode[10]);

	// Draws for the animation view
	void drawAnimationView();

	// Load .stl and .xml files for cutting
	void loadFile(CStringA meshFilePath); 

	void receiveKey(UINT nchar);
	void updateIdx(int yIdx, int zIdx);
	void updateRealtime();

	void updateFilterCutGroup();

private:
	void initState();
	void changeState();
	void constructCutTree();
	void changeToWrapMode();
	void changeToCutGroupBone();
	void changeStateBack();
	void changeToCoordAssignMode();
	void getBoneArrayAndMeshBox(std::vector<bone*> &boneFullArray, std::vector<bvhVoxel> &meshBoxFull);
	void changeToSwapFinal();
	void changeToCuttingMeshMode();

	void saveFile();

	void writeMeshBoxStateFinalSwap();
	void startToStateCuttingMesh();
	bool getSavePath(CString &path);
	void writePolygon(Polyhedron* cutPieces, const char* path);
	static UINT swapVoxelThread(LPVOID p);
	void loadStateForFinalSwap();
	void loadStateForPostProcess();
	void drawTest(BOOL mode[]);
	void drawTest();
	void keyPressModeTest(char c);
	void saveCurrentBoxCut();
	void loadSwapGroupFromFile();
	std::vector<arrayInt> getVoxelIdxFullFromVoxelProcess();
	void saveCutMeshToObj();
	void convertPolyHedronToMayaObj(Polyhedron *cutPieces, const char* path) const;
	float getVoxelSize(int nbVoxel);

	void setDisplayOptions(std::initializer_list<int> opts);
	Polyhedron* getSymmetric_by_X(Polyhedron* cutPieces);

public:
	// Process
	processHoleMeshPtr holeMesh;
	appMode m_curMode; // Application state
	cutSurfTreeMngr2 m_cutSurface; 
	FilterCutDialog * cutFilterDialog;
	movePlacedBoxDlgPtr m_boxPlaceMngr;

	detailSwapManager *m_swapMngr;
	groupCutManager *m_groupCutMngr;
	coordAssignManager *m_coordAssign;
	detailSwapManager *m_finalSwap;
	manipulateVoxelPtr m_voxelProcess;
	MeshCutting * m_meshCutting;

	// Mesh and skeleton data
	SurfaceObj *m_surfaceObj;
	voxelObject *m_highResFullVoxel;
	voxelObject *m_highResVoxel;
	voxelObject *m_lowResVoxel;
	skeleton *m_skeleton;
	TransformerAnimation *m_tAnimation;

	debugInfoPtr m_debug;

	// Debug
	int idx1, idx2;
	int leafIdx;
	int shift;

	CKEGIESView *view1;
	void *view2;
};