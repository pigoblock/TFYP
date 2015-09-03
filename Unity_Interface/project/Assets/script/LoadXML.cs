using UnityEngine;
using System.Collections;
using System.Xml;
using Component;
using Joint;
using System.Collections.Generic;
using System.Globalization;

//[ExecuteInEditMode]

public class LoadXML : MonoBehaviour {
	private float startTime;
	private bool resetTime = false;
	private bool othersCanMove = true;

	//public GameObject tryCube; //WL: not needed apparently

	const string bone = "bone";
	const string bone_properties = "bone_properties";
	const string bone_name = "bone_name";
	const string bone_size = "bone_size";
	const string joint_pos_parent = "joint_pos_parent";
	const string joint_pos_child = "joint_pos_child";
	const string bone_type = "bone_type";
	const string rotation_about_xyz = "rotation_about_xyz";
	const string children = "children";
	const string x_ = "x";
	const string y_ = "y";
	const string z_ = "z";
	
	const string original_mesh_path_name = "original_mesh_path_name";
	const string skeleton_mesh_path_name = "skeleton_mesh_path_name";
	const string mesh_cut_part = "mesh_cut_part";
	const string polygon_name = "polygon_name";
	const string origin = "origin";
	const string bone_coord_respect_to_world = "bone_coord_respect_to_world";
	const string local_x_repect_to_wolrd = "local_x_repect_to_wolrd";
	const string local_y_repect_to_wolrd = "local_y_repect_to_wolrd";
	const string local_z_repect_to_wolrd = "local_z_repect_to_wolrd";

	private	string test = "test";

	private bool edit_Symmetry = true;
	private component root;  // store the component tree
	private List<joint> jointList = new List<joint>(); // store all the joints

	private Dictionary<string, component> componentMap = new Dictionary<string, component> (); //The component map: help to find component by name
	private Dictionary<string, Dictionary<string, joint>> jointMap = new Dictionary<string, Dictionary<string, joint>> ();// The joint map: help to find joint by name

	//private component currentbone = null;
	private joint currentJoint = null;
	private joint duplicateJoint = null;

	private Stack mystack = new Stack();

	char[] spliter = {','};

	bool start = false;
	bool auto = false;

	public Texture2D btnTexture;
	public Texture2D btnTexture2;
	public Texture2D btnRestart;
	public Texture2D btnSave;

	private Color[] colors = new Color[] { new Color(0.384f,0.933f,0.384f, 0.6f), new Color(0.384f,0.859f,0.933f, 0.6f), new Color(0.93f,0.470f,0.93f, 0.6f), 
										   new Color(0.957f,0.5f,0.035f, 0.6f), new Color(0.5f,0.035f,0.956f, 0.6f), new Color(0.035f,0.282f,0.956f, 0.6f),
										   new Color(0.741f,0.956f,0.035f, 0.6f), new Color(0.957f,0.298f,0.286f, 0.6f), new Color(0.298f,0.286f,0.956f, 0.6f),
										   new Color(0.957f,0.298f,0.286f, 0.6f) };

	private RaycastHit hit;
	private string target = "";
	private GameObject input;
	private GameObject originalOne;
	private Mesh oldmesh;
	private Rect positionTarget = new Rect (290, 430, 100, 50);
	private Rect positionTarget2 = new Rect (100, 430, 100, 50);
	
	public GUIStyle myStyle;
	public GUIStyle style0;
	public GUIStyle style2;
	public GUIStyle style3;
	bool setUpAuto = true;

	private bool meshOpen = true;
	private bool boneOpen = true;
	private bool sphereOpen = true;
	private bool meshTransparent = false;
	private bool worldBone = true;
	private bool outerBox = false;

	private bool sizeScale = false;

	Shader transparent, shader1;


	private string try1= "";

	
	Material _myMaterial;
	
	Vector3[] _vertexArray;
	ArrayList _vertexArrayList;
	Vector3[] _normalArray;
	ArrayList _normalArrayList;
	Vector2[] _uvArray;
	ArrayList _uvArrayList;
	
	int[] _triangleArray;
	
	ArrayList _facesVertNormUV;


	internal class PlacesByIndex {
		public PlacesByIndex(int index) {
			_index = index;
		}
		public int _index;
		public ArrayList _places = new ArrayList();
	}
	void initArrayLists() {
		_myMaterial = new Material(Shader.Find("Transparent/Diffuse"));
		_uvArrayList = new ArrayList();
		_normalArrayList = new ArrayList();
		_vertexArrayList = new ArrayList();
		_facesVertNormUV = new ArrayList();
	}

	//WL: This function builds the mesh (I think)
	GameObject SomeFunction(string s) {;
		GameObject obj_gameobject = new GameObject ();
		Mesh _myMesh;
		initArrayLists();

		_myMesh = new Mesh();

		s = s.Replace("  ", " ");
		s = s.Replace("  ", " ");
		LoadFile(s);
		_myMesh.vertices = _vertexArray;
		_myMesh.triangles = _triangleArray;
		if (_uvArrayList.Count > 0)
			_myMesh.uv = _uvArray;
		if (_normalArrayList.Count > 0)
			_myMesh.normals = _normalArray;
		else
			_myMesh.RecalculateNormals();
		_myMesh.RecalculateBounds();
		if ((MeshFilter)obj_gameobject.GetComponent("MeshFilter") == null)
			obj_gameobject.AddComponent<MeshFilter>();
		MeshFilter temp;
		temp = (MeshFilter)obj_gameobject.GetComponent("MeshFilter");
		temp.mesh = _myMesh;
		if ((MeshRenderer)obj_gameobject.GetComponent("MeshRenderer") == null)
			obj_gameobject.AddComponent<MeshRenderer>();

		MeshRenderer temp2;
		temp2 = (MeshRenderer)obj_gameobject.GetComponent("MeshRenderer");
		if (_uvArrayList.Count > 0) {
			temp2.material = _myMaterial;
			_myMaterial.shader = Shader.Find("Diffuse");
		}
		obj_gameobject.GetComponent<Renderer>().material.color = colors [0];
		return obj_gameobject;
	}

	void LoadFile(string s) {
		string[] lines = s.Split("\n"[0]);


		try1 = lines [0];
		foreach (string item in lines) {
			ReadLine(item);
		}
		ArrayList tempArrayList = new ArrayList();
		for (int i = 0; i < _facesVertNormUV.Count; ++i) {
			if (_facesVertNormUV[i] != null) {
				PlacesByIndex indextemp = new PlacesByIndex(i);
				indextemp._places.Add(i);
				for (int j = 0; j < _facesVertNormUV.Count; ++j) {
					if (_facesVertNormUV[j] != null) {
						if (i != j) {
							Vector3 iTemp = (Vector3)_facesVertNormUV[i];
							Vector3 jTemp = (Vector3)_facesVertNormUV[j];
							if (iTemp.x == jTemp.x && iTemp.y == jTemp.y) {
								indextemp._places.Add(j);
								_facesVertNormUV[j] = null;
							}
						}
					}
				}
				tempArrayList.Add(indextemp);
			}
		}
		_vertexArray = new Vector3[tempArrayList.Count];
		_uvArray = new Vector2[tempArrayList.Count];
		_normalArray = new Vector3[tempArrayList.Count];
		_triangleArray = new int[_facesVertNormUV.Count];
		int teller = 0;
		foreach (PlacesByIndex item in tempArrayList) {
			foreach (int item2 in item._places) {
				_triangleArray[item2] = teller;
			}
			Vector3 vTemp = (Vector3)_facesVertNormUV[item._index];
			_vertexArray[teller] = (Vector3)_vertexArrayList[(int)vTemp.x - 1];
			if (_uvArrayList.Count > 0) {
				Vector3 tVec = (Vector3)_uvArrayList[(int)vTemp.y - 1];
				_uvArray[teller] = new Vector2(tVec.x, tVec.y);
			}
			if (_normalArrayList.Count > 0) {
				_normalArray[teller] = (Vector3)_normalArrayList[(int)vTemp.z - 1];
			}
			teller++;
		}
	}
	
	void ReadLine(string s) {
		char[] charsToTrim = {' ', '\n', '\t', '\r'};
		s= s.TrimEnd(charsToTrim);
		string[] words = s.Split(" "[0]);
		foreach (string item in words)
			item.Trim();
		if (words[0] == "v")
			_vertexArrayList.Add(new Vector3(System.Convert.ToSingle(words[1], CultureInfo.InvariantCulture), System.Convert.ToSingle(words[2], CultureInfo.InvariantCulture), System.Convert.ToSingle(words[3], CultureInfo.InvariantCulture)));
		
		if (words[0] == "vn")
			_normalArrayList.Add(new Vector3(System.Convert.ToSingle(words[1], CultureInfo.InvariantCulture), System.Convert.ToSingle(words[2], CultureInfo.InvariantCulture), System.Convert.ToSingle(words[3], CultureInfo.InvariantCulture)));
		if (words[0] == "vt") 
			_uvArrayList.Add(new Vector3(System.Convert.ToSingle(words[1], CultureInfo.InvariantCulture), System.Convert.ToSingle(words[2], CultureInfo.InvariantCulture)));
		if (words[0] == "f") {
			ArrayList temp = new ArrayList();
			ArrayList triangleList = new ArrayList();
			for (int j = 1; j < words.Length; ++j)
			{
				Vector3 indexVector = new Vector3(0,0);
				string[] indices = words[j].Split("/"[0]);
				indexVector.x = System.Convert.ToInt32(indices[0], CultureInfo.InvariantCulture);
				if (indices.Length > 1) {
					if (indices[1] != "")
						indexVector.y = System.Convert.ToInt32(indices[1], CultureInfo.InvariantCulture);
				}
				if (indices.Length > 2) {
					if (indices[2] != "")
						indexVector.z = System.Convert.ToInt32(indices[2], CultureInfo.InvariantCulture);
				}
				temp.Add(indexVector);
			}
			for (int i = 1; i < temp.Count - 1; ++i) {
				triangleList.Add(temp[0]);
				triangleList.Add(temp[i]);
				triangleList.Add(temp[i+1]);
			}
			
			foreach (Vector3 item in triangleList) {
				_facesVertNormUV.Add(item);
			}
		}
	}


	void Start()
	{
		transparent = Shader.Find("Transparent/Diffuse");
		shader1 = Shader.Find("Diffuse");
		// read skeleton.xml

		string path = Application.dataPath;
		string toFind = "/";
		
		for (int i = 0; i< 2; i++){
			int pos = path.LastIndexOf (toFind);
			path = path.Substring(0, pos);
		}
		string skeletonPath = path +"/mesh/skeleton.xml";
		//Debug.Log (skeletonPath);
		//TextAsset textXML = (TextAsset)Resources.Load("skeleton", typeof(TextAsset));
		XmlDocument xml = new XmlDocument();
		xml.Load(skeletonPath);

		XmlNode rootXML = xml.FirstChild;
		root = buildTemplate (rootXML);

		// read infor.xml
		string infoPath = path + "/mesh/info.xml";

		//TextAsset textXML2 = (TextAsset)Resources.Load("info", typeof(TextAsset));
		XmlDocument xml2 = new XmlDocument();
		xml2.Load(infoPath);

		XmlNodeList elemList = xml2.GetElementsByTagName ("mesh_cut_part");
		/*
		for (int i = 0; i < elemList.Count; i++) {
			Debug.Log (elemList.Item(i).InnerText);
		}*/

		uploadMesh (elemList);

	
		// prepare the joint sequence
		getJointSequence (root.getName(), jointMap);


		startTime = Time.time;

		jointList [0].prepare ();
		jointList [0].setJointEnableTrue ();

		//DebugTest ();
	}

	void Update(){
		//tryCube.transform.RotateAround (Vector3.zero, Vector3.up, 20 * Time.deltaTime);
		//Debug.Log("1. Note!!!!!!:"+componentMap["lowerLegTwo"].mesh.transform.position);
		if (start){

			for (int i = 0; i< jointList.Count; i++) {
				joint temp = jointList[i];

				if (temp.jointEnable()){
					//print ("Joint enable!!!!");
					if (temp.jointMoveEnable()){
						temp.move(startTime);
						//Debug.Log("Moving "+temp.getJointParentName()+temp.getJointChildName());
					} 

				
					if (temp.jointRotateEnable()){
						if (i < jointList.Count-1){
							
							if (temp.rotate2()){
								temp.PrepareAutoMoveVectot();
								jointList[i].setJointEnableFalse();
								jointList[i+1].prepare();
								jointList[i+1].setJointEnableTrue();
								startTime = Time.time;
							}
						} else {
							// last one
							if (temp.rotate2()){
								temp.PrepareAutoMoveVectot();
								jointList[i].setJointEnableFalse();
								startTime = Time.time;
							}
						}
					} 
				}
				
			}	 
		}
		autoMove ();
		castRay ();
		mode ();
	} 

	void mode(){
		if (Input.GetKeyDown ("d")) {
			edit_Symmetry = (edit_Symmetry==true) ? false: true; 
		}

		if (Input.GetKeyDown ("m")) {
			meshOpen = (meshOpen==true) ? false: true; 
		}

		if (Input.GetKeyDown ("b")) {
			boneOpen = (boneOpen==true) ? false: true; 
		}
		
		if (Input.GetKeyDown ("s")) {
			sphereOpen = (sphereOpen==true) ? false: true; 
		}

		if (Input.GetKeyDown ("t")) {
			meshTransparent = (meshTransparent==true) ? false: true; 

		}

		if (Input.GetKeyDown ("a")) {
			worldBone = (worldBone==true) ? false: true; 
			
		}

		if (Input.GetKeyDown ("r")) {
			outerBox = (outerBox==true) ? false: true; 
		}

		if (Input.GetKeyDown ("e")) {
			sizeScale = (sizeScale==true) ? false: true; 
		}

		if (outerBox) {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.OuterCube.GetComponent<Renderer>().enabled = true;
			}
		} else {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.OuterCube.GetComponent<Renderer>().enabled = false;
			}
		}

		if (meshOpen) {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.mesh.GetComponent<Renderer>().enabled = true;
			}
		} else {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.mesh.GetComponent<Renderer>().enabled = false;
			}
		}

		if (meshTransparent) {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.mesh.GetComponent<Renderer>().material.shader = transparent;
			}
		} else {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.mesh.GetComponent<Renderer>().material.shader = shader1;
			}
		}

		if (boneOpen) {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.skeletonBone.GetComponent<Renderer>().enabled = true;
				if (co.Value.getInsideBone() != null)
					co.Value.getInsideBone().GetComponent<Renderer>().enabled = true;
			}
		} else {
			foreach (KeyValuePair <string, component> co in componentMap){
				co.Value.skeletonBone.GetComponent<Renderer>().enabled = false;
				if (co.Value.getInsideBone() != null)
					co.Value.getInsideBone().GetComponent<Renderer>().enabled = false;
			}
		}

		if (sphereOpen) {
			foreach (KeyValuePair <string, component> co in componentMap){
				if (co.Value.skeletonSphere != null){
					co.Value.skeletonSphere.GetComponent<Renderer>().enabled = true;
				}
				if (co.Value.centerSphere != null){
					co.Value.centerSphere.GetComponent<Renderer>().enabled = true;
				}
			}
		} else {
			foreach (KeyValuePair <string, component> co in componentMap){
				if (co.Value.skeletonSphere != null)
					co.Value.skeletonSphere.GetComponent<Renderer>().enabled = false;
				if (co.Value.centerSphere != null){
					co.Value.centerSphere.GetComponent<Renderer>().enabled = false;
				}
			}
		}
	}

	Mesh CreateMesh(float width, float height)
	{
		Mesh m = new Mesh();
		m.name = "ScriptedMesh";
		m.vertices = new Vector3[] {
			new Vector3(-width, -height, 0.01f),
			new Vector3(width, -height, 0.01f),
			new Vector3(width, height, 0.01f),
			new Vector3(-width, height, 0.01f)
		};
		m.uv = new Vector2[] {
			new Vector2 (0, 0),
			new Vector2 (0, 1),
			new Vector2(1, 1),
			new Vector2 (1, 0)
		};
		m.triangles = new int[] { 0, 1, 2, 0, 2, 3};
		m.RecalculateNormals();
		
		return m;
	}

	void autoMove(){
		collisionDetection cd;
		Vector3 rotationPoint;
		Vector3 moveVector;

		if (setUpAuto) {
			jointList[0].autoEnable = true;
			jointList[0].getChild().openCollision();
			setUpAuto = false;
		}

		if (auto) {
			for (int i = 0; i< jointList.Count; i++) {
				joint temp = jointList[i];

				if (temp.autoEnable){

					component child = temp.getChild();
					component parent = temp.getParent();

					moveVector = temp.getAutoMoveVecotr();

					cd = child.collisionCube.GetComponent<collisionDetection>();


					//Debug.Log(temp.getJointParentName() + temp.getJointChildName());

					//Debug.Log("Need Auto: "+temp.needAuto);

					cd.startAuto();
					//Debug.Log(child.skeletonSphere.n);

					if (!cd.getStop()) {

						child.autoMove(moveVector/300f);
						rotationPoint = child.updateParentBoneSkeleton();
						temp.setJointLocalPosBasedOnPa(rotationPoint);
					} else {
						if (i+1<jointList.Count){
							jointList[i+1].autoEnable = true;
							jointList[i].getChild().closeCollision();
							jointList[i+1].getChild().openCollision();
						}
						else {
							jointList[i].getChild().closeCollision();
							auto = false;
						}
					}


				} // can do auto
			}// for loop
		
		}// begin auto
	}


	void castRay(){
		Ray ray = Camera.main.ScreenPointToRay (Input.mousePosition);
		if (Physics.Raycast (ray, out hit, 1000)) {
			
			
			if (Input.GetMouseButtonDown(1)){
				//Debug.Log(hit.collider.name);
				target = hit.collider.name;
				input = hit.collider.gameObject;
				originalOne = input;
				oldmesh = originalOne.GetComponent<MeshFilter>().mesh;


			}
			
			
		}
		
		Debug.DrawRay (ray.origin, ray.direction*1000, Color.yellow);
	}

	void OnGUI(){
		//GUI.Label (new Rect(20, 20, 50, 50), test);
		var cam = GameObject.Find("mainCamera");

		//Debug.Log ("hahah: "+cam.name);
		string openEdit = "";
		if (edit_Symmetry)
			openEdit = "OPEN";
		else
			openEdit = "CLOSE";

		float inforLeft = Screen.width * 0.03f;
		float inforLeft2 = Screen.width * 0.12f;
		GUI.Label(new Rect(inforLeft, Screen.height*0.56f,70,60), "Component box:\n(press r)", style2);
		GUI.Label (new Rect (inforLeft2, Screen.height*0.56f, 70, 60), "  "+outerBox, style3);

		GUI.Label(new Rect(inforLeft, Screen.height*0.61f,70,60), "Symmetrical Edit:\n(press d)", style2);
		GUI.Label (new Rect (inforLeft2, Screen.height*0.61f, 70, 60), "  "+openEdit, style3);
		GUI.Label(new Rect(inforLeft, Screen.height*0.66f,70,60), "Show mesh:\n(press m)", style2);
		GUI.Label (new Rect (inforLeft2, Screen.height*0.66f, 70, 60), "  "+meshOpen, style3);

		GUI.Label(new Rect(inforLeft, Screen.height*0.71f,70,60), "Show bone:\n(press b)", style2);
		GUI.Label (new Rect (inforLeft2, Screen.height * 0.71f, 70, 60), "  "+boneOpen, style3);
		GUI.Label(new Rect(inforLeft, Screen.height*0.76f,70,60), "Show sphere:\n(press s)", style2);
		GUI.Label (new Rect (inforLeft2, Screen.height * 0.76f, 70, 60), "  "+sphereOpen, style3);

		GUI.Label(new Rect(inforLeft, Screen.height*0.81f,70,60), "Transparent:\n(press t)", style2);
		GUI.Label (new Rect (inforLeft2, Screen.height * 0.81f, 70, 60), "  "+meshTransparent, style3);





		if (GUI.Button (new Rect (30, 10, 40, 40), btnTexture)) {
			start = true;
		}
		
		if (GUI.Button (new Rect (80, 10, 40, 40), btnTexture2)) {
			start = false;
		}

		if (GUI.Button (new Rect (130, 10, 40, 40), btnRestart)) {
			foreach (KeyValuePair <string, component> co in componentMap){
			
				Destroy (co.Value.restart());
				//Debug.Log("Delete!");
				Destroy(co.Value.getInsideBone());
			}
			Debug.Log("Note!!!!!!:"+componentMap["lowerLegTwo"].skeletonBone.transform.position);
			startTime = Time.time;
			jointList [0].prepare ();

			jointList [0].setJointEnableTrue ();

		}




		if (GUI.Button (new Rect (180, 10, 40, 40), btnSave)) {
			writeSkeletonXML();
		}

		if (GUI.Button (new Rect (230, 10, 80, 40), "Auto\nModification")) {
			auto = true;
		}

		if (GUI.Button (new Rect (320, 10, 80, 40), "Auto\nResize")) {
			foreach (KeyValuePair <string, component> co in componentMap){
				Mesh targetM = co.Value.mesh.GetComponent<MeshFilter> ().mesh;
				co.Value.OuterCube.transform.localScale = targetM.bounds.extents * 2;
				co.Value.componentSize = targetM.bounds.extents * 2;
			}
		}

		if (GUI.Button (new Rect (410, 10, 80, 40), "Speed\nup")) {
			joint.moveSpeed+=2;
		}

		if (GUI.Button (new Rect (500, 10, 80, 40), "Speed\ndown")) {
			joint.moveSpeed-=2;
		}

		if (GUI.Button (new Rect (Screen.width - 60, 10, 40, 40), "Quit")) {
			writeSkeletonXML();
			Application.Quit();
		}

		GUI.Label(new Rect(Screen.width*0.6f, Screen.height*0.02f,100,50), "Select: "+target, style0);
		GUI.Label(new Rect(Screen.width*0.6f, Screen.height*0.05f,100,50), "(Right Click)", style0);

		string temp_name = "";
		component targetComp = null;
		component targetCompSym = null;
		component parentOfTargetComp = null;
		component parentOfTargetCompSym = null;
		joint rotationJoint = null;
		joint rotationJointSym = null;
		Vector3 rotationPoint = new Vector3(0f,0f,0f);
		Vector3 rotationAxis = new Vector3(0f,0f,0f);
		Vector3 rotationPointSym = new Vector3(0f,0f,0f);
		Vector3 rotationAxisSym = new Vector3(0f,0f,0f);
		bool hasSym = false;


		if (target != "" )
		{
			// 
			temp_name = target.Substring (2, target.Length - 2);
			if (componentMap.ContainsKey(temp_name) || componentMap.ContainsKey(temp_name+"Two")){
				targetComp = componentMap[temp_name];
				parentOfTargetComp = targetComp.getParent();

				if (targetComp.getBoneType() == "side_bone"){
					hasSym = true;
				}

				if (hasSym){
					if (targetComp.oneOrTwo == 1){
						targetCompSym = componentMap[temp_name+"Two"];
						parentOfTargetCompSym = targetCompSym.getParent();
					} else {
						targetCompSym = componentMap[temp_name.Substring(0, temp_name.Length-3)];
						parentOfTargetCompSym = targetCompSym.getParent();
					}
				}

				MouseOrbitBlender cameraObject = (MouseOrbitBlender)cam.GetComponent("MouseOrbitBlender");
				//print("object:"+gameObject.name);
				cameraObject.changeTarget(targetComp.cube.transform);

			}
			if (componentMap.ContainsKey(temp_name )|| componentMap.ContainsKey(temp_name+"Two")){
				if (temp_name != "Torso"){
					rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
					rotationPoint = parentOfTargetComp.localToWorld (rotationJoint.getJointLocalPosOfParent());
					if (hasSym){
						rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
						rotationPointSym = parentOfTargetCompSym.localToWorld (rotationJointSym.getJointLocalPosOfParent());
					}
				}
			}

		}
		string editMode = "";

		if (sizeScale) {
			editMode = "Component Size editing: \n(press e: go back bone editing)";

		}else{ 
			if (worldBone) {
				editMode = "World Bone Editing:\n(press a: Local Bone Editing)\n(press e: bone size editing)";
			} else {
				editMode = "Local Bone Editing:\n(press a: World Bone Editing)\n(press e: bone size editing)";
			}
		}

		GUI.Label (new Rect (Screen.width*0.025f, Screen.height * 0.12f, 120, 320), editMode, myStyle);

		float leftLabel = Screen.width * 0.03f;
		float leftBtnOne1 = Screen.width * 0.07f;
		float leftBtnOne2 = Screen.width * 0.12f;
		float leftLabelHbase = Screen.height * 0.20f;
		float leftBtnTwoBase = Screen.height * 0.195f;
		float leftBtnTwoAdd = Screen.height * 0.057f;
		float leftBtnThree = Screen.width * 0.04f;
		float leftBtnFour = Screen.width * 0.033f;


		if (!sizeScale) {
			GUI.Label (new Rect (leftLabel, leftLabelHbase, 120, 320), "X-axis\nmove", style2);
		} else {
			GUI.Label (new Rect (leftLabel, leftLabelHbase, 120, 320), "X-axis\nscale", style2);
		}
		if (GUI.Button (new Rect (leftBtnOne1, leftBtnTwoBase, leftBtnThree, leftBtnFour), "+")) {
			if (target != ""){
				if (sizeScale){
					targetComp.outerBoxScaleXPlus();
					if (hasSym && edit_Symmetry){
						targetCompSym.outerBoxScaleXMinus();
					}
				} else if (worldBone){
					targetComp.moveXPlus();
					rotationPoint = targetComp.updateParentBoneSkeleton();
					rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
					rotationJoint.setJointLocalPosBasedOnPa(rotationPoint);
					rotationJoint.PrepareAutoMoveVectot();

					if (hasSym && edit_Symmetry){
						targetCompSym.moveXNagetive();
						rotationPointSym = targetCompSym.updateParentBoneSkeleton();
						rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
						rotationJointSym.setJointLocalPosBasedOnPa(rotationPointSym);
						rotationJointSym.PrepareAutoMoveVectot();
					}
				} else {
					targetComp.localMoveXPlus();
				}
				
			}
		}
		if (GUI.Button (new Rect (leftBtnOne2, leftBtnTwoBase, leftBtnThree, leftBtnFour), "-")) {
			if (target != ""){
				if (sizeScale){
					targetComp.outerBoxScaleXMinus();
					if (hasSym && edit_Symmetry){
						targetCompSym.outerBoxScaleXPlus();
					}
				} else if (worldBone){
					targetComp.moveXNagetive();
					rotationPoint = targetComp.updateParentBoneSkeleton();
					rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
					rotationJoint.setJointLocalPosBasedOnPa(rotationPoint);
					rotationJoint.PrepareAutoMoveVectot();

					if (hasSym && edit_Symmetry){
						targetCompSym.moveXPlus();
						rotationPointSym = targetCompSym.updateParentBoneSkeleton();
						rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
						rotationJointSym.setJointLocalPosBasedOnPa(rotationPointSym);
						rotationJointSym.PrepareAutoMoveVectot();
					}
				} else {
					targetComp.localXNagetive();
				}
			}
		}

		if (!sizeScale)
			GUI.Label (new Rect (leftLabel, leftLabelHbase+ 1*leftBtnTwoAdd, 120, 320), "Y-axis\nmove", style2);
		else 
			GUI.Label (new Rect (leftLabel, leftLabelHbase+ 1*leftBtnTwoAdd, 120, 320), "Y-axis\nscale", style2);

		if (GUI.Button (new Rect (leftBtnOne1, leftBtnTwoBase+leftBtnTwoAdd, leftBtnThree, leftBtnFour), "+")) {
			if (target != ""){
				if (sizeScale){
					targetComp.outerBoxScaleYPlus();
					if (hasSym && edit_Symmetry){
						targetCompSym.outerBoxScaleYPlus();
					}
				} else if (worldBone){
					targetComp.moveYPlus();
					rotationPoint = targetComp.updateParentBoneSkeleton();
					rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
					rotationJoint.setJointLocalPosBasedOnPa(rotationPoint);
					rotationJoint.PrepareAutoMoveVectot();
					if (hasSym && edit_Symmetry){
						targetCompSym.moveYPlus();
						rotationPointSym = targetCompSym.updateParentBoneSkeleton();
						rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
						rotationJointSym.setJointLocalPosBasedOnPa(rotationPointSym);
						rotationJointSym.PrepareAutoMoveVectot();
						
					}
				} else {
					targetComp.localMoveYPlus();
				}
			}
		}
		if (GUI.Button (new Rect (leftBtnOne2, leftBtnTwoBase+leftBtnTwoAdd, leftBtnThree, leftBtnFour), "-")) {
			if (target != ""){
				if (sizeScale){
					targetComp.outerBoxScaleYMinus();
					if (hasSym && edit_Symmetry){
						targetCompSym.outerBoxScaleYMinus();
					}
				} else if (worldBone){
					targetComp.moveYNagetive();
					rotationPoint = targetComp.updateParentBoneSkeleton();
					rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
					rotationJoint.setJointLocalPosBasedOnPa(rotationPoint);
					rotationJoint.PrepareAutoMoveVectot();
					if (hasSym && edit_Symmetry){
						targetCompSym.moveYNagetive();
						rotationPointSym = targetCompSym.updateParentBoneSkeleton();
						rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
						rotationJointSym.setJointLocalPosBasedOnPa(rotationPointSym);
						rotationJointSym.PrepareAutoMoveVectot();
						
					}
				} else {
					targetComp.localYNagetive();
				}
			}
		}

		if (!sizeScale)
			GUI.Label (new Rect (leftLabel, leftLabelHbase+ 2*leftBtnTwoAdd, 120, 320), "Z-axis\nmove", style2);
		else 
			GUI.Label (new Rect (leftLabel, leftLabelHbase+ 2*leftBtnTwoAdd, 120, 320), "Z-axis\nscale", style2);

		if (GUI.Button (new Rect (leftBtnOne1, leftBtnTwoBase+2*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "+")) {
			if (target != ""){
				if (sizeScale){
					targetComp.outerBoxScaleZPlus();
					if (hasSym && edit_Symmetry){
						targetCompSym.outerBoxScaleZPlus();
					}
				} else if (worldBone){
					targetComp.moveZPlus();
					rotationPoint = targetComp.updateParentBoneSkeleton();
					rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
					rotationJoint.setJointLocalPosBasedOnPa(rotationPoint);
					rotationJoint.PrepareAutoMoveVectot();
					if (hasSym && edit_Symmetry){
						targetCompSym.moveZPlus();
						rotationPointSym = targetCompSym.updateParentBoneSkeleton();
						rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
						rotationJointSym.setJointLocalPosBasedOnPa(rotationPointSym);
						rotationJointSym.PrepareAutoMoveVectot();
						
					}
				} else {
					targetComp.localMoveZPlus();
				}
			}
		}
		if (GUI.Button (new Rect (leftBtnOne2, leftBtnTwoBase+2*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "-")) {
			if (target != ""){
				if (sizeScale){
					targetComp.outerBoxScaleZMinus();
					if (hasSym && edit_Symmetry){
						targetCompSym.outerBoxScaleZMinus();
					}
				} else if (worldBone){
					targetComp.moveZNagetive();
					rotationPoint = targetComp.updateParentBoneSkeleton();
					rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
					rotationJoint.setJointLocalPosBasedOnPa(rotationPoint);
					rotationJoint.PrepareAutoMoveVectot();
					if (hasSym && edit_Symmetry){
						targetCompSym.moveZNagetive();
						rotationPointSym = targetCompSym.updateParentBoneSkeleton();
						rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
						rotationJointSym.setJointLocalPosBasedOnPa(rotationPointSym);
						rotationJointSym.PrepareAutoMoveVectot();
						
					}
				} else{
					targetComp.localZNagetive();
				}
			}
		}


		if (!sizeScale){
			GUI.Label (new Rect (leftLabel, leftLabelHbase+ 3*leftBtnTwoAdd, 120, 320), "X-axis\nrotate", style2);
			if (GUI.Button (new Rect (leftBtnOne1, leftBtnTwoBase+3*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "+")) {
				if (target != ""){
					if (worldBone){
						rotationAxis = targetComp.getTransform().TransformDirection( Vector3.left);
						targetComp.rotateAround(rotationPoint, rotationAxis, 5.0f);

						rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
						rotationJoint.addRotation(Vector3.left, 5.0f);
						rotationJoint.PrepareAutoMoveVectot();

						if (hasSym && edit_Symmetry){
							rotationAxisSym = targetCompSym.getTransform().TransformDirection( Vector3.left);
							targetCompSym.rotateAround(rotationPointSym, rotationAxisSym, 5.0f);
							
							rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
							rotationJointSym.addRotation(Vector3.left, 5.0f);
							rotationJointSym.PrepareAutoMoveVectot();
						}
					} else {
						targetComp.localRotateAround(new Vector3(5f, 0f, 0f));
					}
				}
			}
			if (GUI.Button (new Rect (leftBtnOne2, leftBtnTwoBase+3*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "-")) {
				if (target != ""){
					if (worldBone){
						rotationAxis = targetComp.getTransform().TransformDirection( Vector3.right);
						targetComp.rotateAround(rotationPoint, rotationAxis, 5.0f);
						rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
						rotationJoint.addRotation(Vector3.right, -5.0f);
						rotationJoint.PrepareAutoMoveVectot();

						if (hasSym && edit_Symmetry){
							rotationAxisSym = targetCompSym.getTransform().TransformDirection( Vector3.right);
							targetCompSym.rotateAround(rotationPointSym, rotationAxisSym, 5.0f);
							
							rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
							rotationJointSym.addRotation(Vector3.right, 5.0f);
							rotationJointSym.PrepareAutoMoveVectot();
						}
					} else {
						targetComp.localRotateAround(new Vector3(-5f, 0f, 0f));
					}
				}
			}
			GUI.Label (new Rect (leftLabel, leftLabelHbase+ 4*leftBtnTwoAdd, 120, 320), "Y-axis\nrotate", style2);
			if (GUI.Button (new Rect (leftBtnOne1, leftBtnTwoBase+4*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "+")) {
				if (target != ""){
					if (worldBone){
						rotationAxis = targetComp.getTransform().TransformDirection( Vector3.down);
						targetComp.rotateAround(rotationPoint, rotationAxis, 5.0f);
						rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
						rotationJoint.addRotation(Vector3.down, 5.0f);
						rotationJoint.PrepareAutoMoveVectot();

						if (hasSym && edit_Symmetry){
							rotationAxisSym = targetCompSym.getTransform().TransformDirection( Vector3.up);
							targetCompSym.rotateAround(rotationPointSym, rotationAxisSym, 5.0f);
							
							rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
							rotationJointSym.addRotation(Vector3.up, 5.0f);
							rotationJointSym.PrepareAutoMoveVectot();
						}
					} else {
						targetComp.localRotateAround(new Vector3(0f, 5f, 0f));
					}
				}
			}
			if (GUI.Button (new Rect (leftBtnOne2, leftBtnTwoBase+4*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "-")) {
				if (target != ""){
					if (worldBone){
						rotationAxis = targetComp.getTransform().TransformDirection( Vector3.up);
						targetComp.rotateAround(rotationPoint, rotationAxis, 5.0f);
						rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
						rotationJoint.addRotation(Vector3.down, -5.0f);
						rotationJoint.PrepareAutoMoveVectot();

						if (hasSym && edit_Symmetry){
							rotationAxisSym = targetCompSym.getTransform().TransformDirection( Vector3.down);
							targetCompSym.rotateAround(rotationPointSym, rotationAxisSym, 5.0f);
							
							rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
							rotationJointSym.addRotation(Vector3.down, 5.0f);
							rotationJointSym.PrepareAutoMoveVectot();
						}
					}else {
						targetComp.localRotateAround(new Vector3(0f, -5f, 0f));
					}
				}
			}
			GUI.Label (new Rect (leftLabel, leftLabelHbase+ 5*leftBtnTwoAdd, 120, 320), "Z-axis\nrotate", style2);
			if (GUI.Button (new Rect (leftBtnOne1, leftBtnTwoBase+5*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "+")) {
				if (target != ""){
					if (worldBone){
						rotationAxis = targetComp.getTransform().TransformDirection( Vector3.back);
						targetComp.rotateAround(rotationPoint, rotationAxis, 5.0f);
						rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
						rotationJoint.addRotation(Vector3.back, 5.0f);
						rotationJoint.PrepareAutoMoveVectot();

						if (hasSym && edit_Symmetry){
							rotationAxisSym = targetCompSym.getTransform().TransformDirection( Vector3.forward);
							targetCompSym.rotateAround(rotationPointSym, rotationAxisSym, 5.0f);
							
							rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
							rotationJointSym.addRotation(Vector3.forward, 5.0f);
							rotationJointSym.PrepareAutoMoveVectot();
						}
					} else {
						targetComp.localRotateAround(new Vector3(0f, 0f, 5f));
					}
				}
			}
			if (GUI.Button (new Rect (leftBtnOne2, leftBtnTwoBase+5*leftBtnTwoAdd, leftBtnThree, leftBtnFour), "-")) {
				if (target != ""){
					if (worldBone){
						rotationAxis = targetComp.getTransform().TransformDirection( Vector3.forward);
						targetComp.rotateAround(rotationPoint, rotationAxis, 5.0f);
						rotationJoint = jointMap[parentOfTargetComp.getName()][targetComp.getName()];
						rotationJoint.addRotation(Vector3.back, -5.0f);
						rotationJoint.PrepareAutoMoveVectot();

						if (hasSym && edit_Symmetry){
							rotationAxisSym = targetCompSym.getTransform().TransformDirection( Vector3.back);
							targetCompSym.rotateAround(rotationPointSym, rotationAxisSym, 5.0f);
							
							rotationJointSym = jointMap[parentOfTargetCompSym.getName()][targetCompSym.getName()];
							rotationJointSym.addRotation(Vector3.back, 5.0f);
							rotationJointSym.PrepareAutoMoveVectot();
						}
					} else {
						targetComp.localRotateAround(new Vector3(0f, 0f, 5f));
					}
				}
			}
		}

	}



	private void getJointSequence(string start, Dictionary<string, Dictionary<string, joint>> myMap){
		if (myMap.ContainsKey (start)) {
			Dictionary<string, joint> temp = myMap [start];
			foreach(KeyValuePair<string, joint> entry in temp) {
				jointList.Add(entry.Value);
				getJointSequence(entry.Key, myMap);
			}
		}
	}


	// read info.xml, match mesh with its corresponding template part
	private void uploadMesh(XmlNodeList elemList){
		string objName = "";
		string boneName = "";
		string fileName = "";
		int index = 0;

		readObj r0 = new readObj ();

		foreach (XmlNode node in elemList) {
			// get the original mesh name
			if(node.ChildNodes[0].Name.ToString() == polygon_name){
				fileName = node.ChildNodes[0].InnerText;
				fileName = fileName.Substring(0, fileName.IndexOf("."));
				Debug.Log ("fileName: " + fileName);
			}

			// get the corresponding bone name
			if(node.ChildNodes[1].Name.ToString() == bone_name){
				boneName = node.ChildNodes[1].InnerText;
				Debug.Log ("boneName: " + boneName);
			}

			// try read from outside
			// WL: Read meshPath.obj file (each obj file is a cut piece)
			string try2 = r0.write (fileName+".obj");	

			// WL: Build the mesh based on the obj file
			GameObject goodMesh = (SomeFunction(try2));

			// preprocess original mesh
			meshProcess(goodMesh, false);

			goodMesh.GetComponent<Renderer>().material.color = colors[index];

			goodMesh.name = "M_"+boneName;

			// save this mesh in the skeleton
			if (componentMap.ContainsKey(boneName)){
				componentMap[boneName].setMesh(goodMesh);
				componentMap[boneName].saveOriginalPosition();
			}

			// if this mesh is a side_bone
			if (componentMap[boneName].getBoneType() == "side_bone"){
				string try3 = r0.write (fileName+".obj");
				
				GameObject sysmetricGoodMesh = (SomeFunction(try3));
				meshProcess(sysmetricGoodMesh, true);
				sysmetricGoodMesh.name = "M_"+boneName+"Two";
				sysmetricGoodMesh.GetComponent<Renderer>().material.color = colors[index];
				if (componentMap.ContainsKey(boneName+"Two")){
					componentMap[boneName+"Two"].setMesh(sysmetricGoodMesh);
					componentMap[boneName+"Two"].saveOriginalPosition();
				}
			}
			index++;
			
		}
		

	} // end of uploadMesh()
	

	// create the mesh, if the mesh is sysmetric, create its sysmetric mesh
	private GameObject meshProcess(GameObject meshContainer, bool sysmetric){

		MeshFilter viewedModelFilter = meshContainer.GetComponent<MeshFilter> ();
		Mesh targetMesh = viewedModelFilter.mesh;
	
		if (sysmetric) {
			meshContainer.transform.localScale += new Vector3(-2, 0,0);
		}
		Vector3 p;

		Bounds b = targetMesh.bounds;
		Vector3 offset = -1 * b.center;
		Vector3 last_p = new Vector3(offset.x / b.extents.x, offset.y / b.extents.y, offset.z / b.extents.z);
		p = Vector3.zero;

		Vector3 diff = Vector3.Scale(targetMesh.bounds.extents, last_p - p); //Calculate difference in 3d position
		meshContainer.transform.position -= Vector3.Scale(diff, meshContainer.transform.localScale); //Move object position by taking localScale into account
		//Iterate over all vertices and move them in the opposite direction of the object position movement
		Vector3[] verts = targetMesh.vertices; 
		for(int i=0; i<verts.Length; i++) {
			verts[i] += diff;
		}
		targetMesh.vertices = verts; //Assign the vertex array back to the mesh
		targetMesh.RecalculateBounds(); //Recalculate bounds of the mesh, for the renderer's sake
	
		return meshContainer;
	}

	public void writeSkeletonXML(){
	
		XmlDocument xmlDoc = new XmlDocument();
        XmlNode rootNode = traverse(root, xmlDoc);
        xmlDoc.AppendChild(rootNode);


		string path = Application.dataPath;
		string toFind = "/";
		
		for (int i = 0; i< 2; i++){
			int pos = path.LastIndexOf (toFind);
			path = path.Substring(0, pos);
		}

		if (Application.platform == RuntimePlatform.WindowsPlayer || Application.platform == RuntimePlatform.WindowsEditor)
		{
			//string newSkeletonPath = path +"\\mesh\\newSkeleton.xml";
			xmlDoc.Save(path +"\\mesh\\newSkeleton.xml");
		}
		if (Application.platform == RuntimePlatform.OSXPlayer)
		{
			//string newSkeletonPath = path +"/mesh/newSkeleton.xml";
			xmlDoc.Save(path +"/mesh/newSkeleton.xml");
		}

	}



	private XmlNode traverse(component currentCom, XmlDocument xmlDoc){
		string rotationString;
		string posParentS;
		string posChildS;
		joint jointCU;
		
		if (currentCom.getParent() == null){
			rotationString = "0 , 0 , 0";
			posParentS = "0 , 0 , 0";
			posChildS = "0 , 0 , 0";
		} else {
			component parentCurrentCom = currentCom.getParent();
			jointCU = jointMap[parentCurrentCom.getName()][currentCom.getName()];
			rotationString = jointCU.getRotation (0) + " , " + jointCU.getRotation (1) + " , " + jointCU.getRotation (2);
			Vector3 p = jointCU.convertBackToRaw();
			posParentS = p.y.ToString("f4") + " , " + p.x.ToString("f4") + " , " + p.z.ToString("f4");
			posChildS = p.y.ToString("f4") + " , " + p.x.ToString("f4") + " , " + p.z.ToString("f4");
		}

		XmlNode currentBoneNode = xmlDoc.CreateElement(bone);
		
		XmlNode oneBone = xmlDoc.CreateElement(bone_properties);
		currentBoneNode.AppendChild(oneBone);

		XmlNode name = xmlDoc.CreateElement(bone_name);
		name.InnerText = currentCom.getName();
		oneBone.AppendChild(name);

		XmlNode size = xmlDoc.CreateElement(bone_size);
		string sizeString = currentCom.componentSize.y + " , " + currentCom.componentSize.x + " , " + currentCom.componentSize.z;
		size.InnerText = sizeString;
		oneBone.AppendChild(size);

		XmlNode jointPosParent = xmlDoc.CreateElement(joint_pos_parent);
		jointPosParent.InnerText = posParentS;
		oneBone.AppendChild(jointPosParent);

		XmlNode jointPosChild = xmlDoc.CreateElement(joint_pos_child);
		jointPosChild.InnerText = posChildS;
		oneBone.AppendChild(jointPosChild);
		
		XmlNode boneTypeN = xmlDoc.CreateElement(bone_type);
		boneTypeN.InnerText = currentCom.getBoneType();
		oneBone.AppendChild(boneTypeN);
		
		XmlNode rotationN = xmlDoc.CreateElement(rotation_about_xyz);
		rotationN.InnerText = rotationString;
		oneBone.AppendChild(rotationN);
		
		if (currentCom.childrenList.Count == 0)
		{
			return currentBoneNode;
		} else{
			XmlNode childrenS = xmlDoc.CreateElement(children);

			for (int i = 0; i<currentCom.childrenList.Count;++i){
				if (currentCom.childrenList[i].oneOrTwo == 1){
					XmlNode childN = traverse(currentCom.childrenList[i], xmlDoc);
					childrenS.AppendChild(childN);
				}
			}
			currentBoneNode.AppendChild(childrenS);
			return currentBoneNode;
		}
	}




	// read in template xml, and build the template 
	private component buildTemplate(XmlNode currentNode)
	{
		component currentbone = new component ();
		component duplicateBone = new component ();
		currentbone.oneOrTwo = 1;
		duplicateBone.oneOrTwo = 2;
		bool hasDuplicate = false;

		if (currentNode.NodeType == XmlNodeType.Element && currentNode.Name.ToString() == bone)
		{	
			// first node should be bone_properties
			XmlNode node = currentNode.ChildNodes[0];

			if (node.Name.ToString() == bone_properties)
			{
				if (node.ChildNodes[0].Name.ToString() == bone_name)
				{
					currentbone.setName(node.ChildNodes[0].InnerText);
					duplicateBone.setName(node.ChildNodes[0].InnerText+"Two");
					//Debug.Log(currentbone.getName());
				}

				if (node.ChildNodes[1].Name.ToString() == bone_size)
				{
					string sizeStr = node.ChildNodes[1].InnerText;

					string[] sizeArr = sizeStr.Split(spliter, System.StringSplitOptions.RemoveEmptyEntries);

					Vector3 cubeSize = new Vector3();
					cubeSize.x = float.Parse(sizeArr[0], CultureInfo.InvariantCulture.NumberFormat);
					cubeSize.y = float.Parse(sizeArr[1], CultureInfo.InvariantCulture.NumberFormat);
					cubeSize.z = float.Parse(sizeArr[2], CultureInfo.InvariantCulture.NumberFormat);

					currentbone.cubeScaleV3(cubeSize);
					duplicateBone.cubeScaleV3(cubeSize);
				}

				if (node.ChildNodes[2].Name.ToString() == joint_pos_parent)
				{
					currentJoint = new joint();
					duplicateJoint = new joint();

					string joint_pos_pa = node.ChildNodes[2].InnerText;
					string[] sizeJPP = joint_pos_pa.Split(spliter, System.StringSplitOptions.RemoveEmptyEntries);

					currentJoint.rawJointPosOfParentBasedPa.y= float.Parse(sizeJPP[0], CultureInfo.InvariantCulture.NumberFormat);
					currentJoint.rawJointPosOfParentBasedPa.x= float.Parse(sizeJPP[1], CultureInfo.InvariantCulture.NumberFormat);
					currentJoint.rawJointPosOfParentBasedPa.z= float.Parse(sizeJPP[2], CultureInfo.InvariantCulture.NumberFormat);

					duplicateJoint.rawJointPosOfParentBasedPa.y= float.Parse(sizeJPP[0], CultureInfo.InvariantCulture.NumberFormat);
					duplicateJoint.rawJointPosOfParentBasedPa.x= -float.Parse(sizeJPP[1], CultureInfo.InvariantCulture.NumberFormat);
					duplicateJoint.rawJointPosOfParentBasedPa.z= float.Parse(sizeJPP[2], CultureInfo.InvariantCulture.NumberFormat);

				}

				if (node.ChildNodes[3].Name.ToString() == joint_pos_child)
				{
					string joint_pos_ch = node.ChildNodes[3].InnerText;
					string[] sizeJPH = joint_pos_ch.Split(spliter, System.StringSplitOptions.RemoveEmptyEntries);
					
					currentJoint.rawJointPosOfChildBasedPa.y= float.Parse(sizeJPH[0], CultureInfo.InvariantCulture.NumberFormat);
					currentJoint.rawJointPosOfChildBasedPa.x= float.Parse(sizeJPH[1], CultureInfo.InvariantCulture.NumberFormat);
					currentJoint.rawJointPosOfChildBasedPa.z= float.Parse(sizeJPH[2], CultureInfo.InvariantCulture.NumberFormat);

					duplicateJoint.rawJointPosOfChildBasedPa.y= float.Parse(sizeJPH[0], CultureInfo.InvariantCulture.NumberFormat);
					duplicateJoint.rawJointPosOfChildBasedPa.x= float.Parse(sizeJPH[1], CultureInfo.InvariantCulture.NumberFormat);
					duplicateJoint.rawJointPosOfChildBasedPa.z= float.Parse(sizeJPH[2], CultureInfo.InvariantCulture.NumberFormat);

					currentJoint.rawJointPosOfParentBasedPa.y= float.Parse(sizeJPH[0], CultureInfo.InvariantCulture.NumberFormat);
					currentJoint.rawJointPosOfParentBasedPa.x= float.Parse(sizeJPH[1], CultureInfo.InvariantCulture.NumberFormat);
					currentJoint.rawJointPosOfParentBasedPa.z= float.Parse(sizeJPH[2], CultureInfo.InvariantCulture.NumberFormat);
					
					duplicateJoint.rawJointPosOfParentBasedPa.y= float.Parse(sizeJPH[0], CultureInfo.InvariantCulture.NumberFormat);
					duplicateJoint.rawJointPosOfParentBasedPa.x= -float.Parse(sizeJPH[1], CultureInfo.InvariantCulture.NumberFormat);
					duplicateJoint.rawJointPosOfParentBasedPa.z= float.Parse(sizeJPH[2], CultureInfo.InvariantCulture.NumberFormat);

				}

				if (node.ChildNodes[4].Name.ToString() == bone_type)
				{
					currentbone.setType(node.ChildNodes[4].InnerText);
					duplicateBone.setType(node.ChildNodes[4].InnerText);
					if (node.ChildNodes[4].InnerText == "side_bone")
						hasDuplicate = true;
				}

				if (node.ChildNodes[5].Name.ToString() == rotation_about_xyz)
				{
					string rotation = node.ChildNodes[5].InnerText;
					string[] rotationArr = rotation.Split(spliter, System.StringSplitOptions.RemoveEmptyEntries);

					currentJoint.addRotation(Vector3.down, float.Parse(rotationArr[0], CultureInfo.InvariantCulture.NumberFormat));
					currentJoint.addRotation(Vector3.left, float.Parse(rotationArr[1], CultureInfo.InvariantCulture.NumberFormat));
					currentJoint.addRotation(Vector3.back, float.Parse(rotationArr[2], CultureInfo.InvariantCulture.NumberFormat));

					duplicateJoint.addRotation(Vector3.down, -float.Parse(rotationArr[0], CultureInfo.InvariantCulture.NumberFormat));
					duplicateJoint.addRotation(Vector3.left, float.Parse(rotationArr[1], CultureInfo.InvariantCulture.NumberFormat));
					duplicateJoint.addRotation(Vector3.back, -float.Parse(rotationArr[2], CultureInfo.InvariantCulture.NumberFormat));
				}


			} else {
				Debug.Log("Bone don't have properties.");
			}
			// after finish one bone_properties
			// if mystack is empty, this bone is root, do not save joint, put this bone into mystack
			if (mystack.Count==0)
			{
				componentMap.Add(currentbone.getName(), currentbone);
			}
			else{

				component tempParent = (component) mystack.Peek();

				tempParent.pushChild(ref currentbone);
				currentbone.setParent(ref tempParent);

				currentJoint.setJointParent(ref tempParent);
				currentJoint.setJointChild(ref currentbone);

				currentJoint.prepareLocalJointPosOfParent();
				currentJoint.prepareLocalJointPosOfChild();

				//Debug.Log(currentbone.getName());
				componentMap.Add(currentbone.getName(), currentbone);
				if(jointMap.ContainsKey(currentJoint.getJointParentName())){
					jointMap[currentJoint.getJointParentName()].Add(currentJoint.getJointChildName(), currentJoint);
				}
				else {
					Dictionary<string, joint> tempMap = new Dictionary<string, joint>();
					tempMap.Add(currentJoint.getJointChildName(), currentJoint);
					jointMap.Add(currentJoint.getJointParentName(), tempMap);
				}

				if (hasDuplicate){
					if (tempParent.getBoneType() == "side_bone"){
						if (componentMap.ContainsKey(tempParent.getName()+"Two"))
							tempParent = componentMap[tempParent.getName()+"Two"];
						else 
							Debug.Log("TempParent--Do have a duplicate component of "+tempParent.getName());
					} else {
						tempParent = tempParent;
					}

					tempParent.pushChild(ref duplicateBone);
					duplicateBone.setParent(ref tempParent);
					
					duplicateJoint.setJointParent(ref tempParent);
					duplicateJoint.setJointChild(ref duplicateBone);
					
					duplicateJoint.prepareLocalJointPosOfParent();
					duplicateJoint.prepareLocalJointPosOfChild();
					
					componentMap.Add(duplicateBone.getName(), duplicateBone);

					if(jointMap.ContainsKey(duplicateJoint.getJointParentName())){
						jointMap[duplicateJoint.getJointParentName()].Add(duplicateJoint.getJointChildName(), duplicateJoint);
					}
					else {
						Dictionary<string, joint> tempMap = new Dictionary<string, joint>();
						tempMap.Add(duplicateJoint.getJointChildName(), duplicateJoint);
						jointMap.Add(duplicateJoint.getJointParentName(), tempMap);
					}
				}

			}
			
			
			// if bone has children
			if (currentNode.ChildNodes.Count == 2)
			{
				//Debug.Log("!Has children!");

				mystack.Push(currentbone);

				XmlNode node2 = currentNode.ChildNodes[1];
				if (node2.Name.ToString() == children)
				{
					foreach( XmlNode n in node2.ChildNodes)
					{
						if (n.Name.ToString()== bone)
						{
							buildTemplate(n);
						}
					}
				}

				mystack.Pop();
			}

			
		} // finish bone 
	
		return currentbone;
	} // finish buildTemplate() method


	void DebugTest(){
		Debug.Log ("==============================");
		
		showBone (root);

		Debug.Log ("------------------------------");

		foreach (joint temp in jointList) {
			Debug.Log( "JointList: parent and child: " +temp.getJointParentName()+" "+temp.getJointChildName());	
		}
		
		
	}
	void showBone(component bone){
		
		Debug.Log ("Bone name: "+bone.getName());
		Debug.Log ("Bone type: "+bone.getBoneType());
		Debug.Log ("Bone size: " + bone.componentSize);
		Debug.Log ("Bone mesh: " + bone.mesh.name);

		for (int i=0; i<bone.childrenList.Count; i++) {
			showBone (bone.childrenList[i]);		
		}
	}
}




