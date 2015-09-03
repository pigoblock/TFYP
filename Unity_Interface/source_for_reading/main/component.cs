using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;

/*
	Stucture of one component.

*/


namespace Component
{
	
	public class component 
	{
		private string componentName;
		public GameObject cube;
		public GameObject mesh;
		public component parent;
		public GameObject OuterCube;
		public MeshCollider mc;
		public GameObject collisionCube;

		private float rotationAnglebyX, rotationAnglebyY, rotationAnglebyZ;
		public List<component> childrenList = new List<component>();
		public List<Vector3> childrenPosStartList = new List<Vector3>();
		public List<Vector3> childrenPosEndList = new List<Vector3>();

		private string boneType = "";
		public int oneOrTwo = 0;
		public Vector3 componentSize;

		private bool lerpPrepareChildrenSuccess = false;

		private float edit_speed = 1f;

		public GameObject skeletonSphere;
		private float sphereRadius = 5.0f;
		public GameObject skeletonBone;

		public Vector3 musclePosition;
		public Vector3 muscleLocalPosition;

		private bool torsoSkeletonDrawn = false;

		public Vector3 jointPosOnParent;

		public Vector3 originalPosition;

		// only Torso's child use 
		private GameObject insideBone;

		Shader transparent, shader1;

		private List<Vector3> localRotationList;
		public GameObject centerSphere;

		public component(){
			transparent = Shader.Find("Transparent/Diffuse");

			cube = GameObject.CreatePrimitive (PrimitiveType.Cube);	
			OuterCube = GameObject.CreatePrimitive (PrimitiveType.Cube);	
			collisionCube = GameObject.CreatePrimitive (PrimitiveType.Cube);
			skeletonBone = GameObject.CreatePrimitive (PrimitiveType.Sphere);
			jointPosOnParent = new Vector3 (0f, 0f, 0f);
			rotationAnglebyX = 0f;
			rotationAnglebyY = 0f;
			rotationAnglebyZ = 0f;
			cubeUnEnable ();
			localRotationList = new List<Vector3> ();
		}
	
		public void setMesh(GameObject m){
			mesh = m;
			mc = mesh.AddComponent ("MeshCollider") as MeshCollider;
			mc.isTrigger = true;
			mesh.AddComponent<Rigidbody> ();
			mesh.GetComponent<Rigidbody> ().isKinematic = true;
			Mesh targetM = mesh.GetComponent<MeshFilter> ().mesh;
			cube.transform.localScale = targetM.bounds.extents * 2;
			cube.transform.position = mesh.transform.position;
			OuterCube.transform.position = mesh.transform.position;
			collisionCube.transform.localScale = targetM.bounds.extents * 2 * 0.8f;;
			collisionCube.transform.position = mesh.transform.position;
			collisionCube.name = "collision_"+componentName;
			musclePosition = mesh.transform.position;

		}

		public void cubeUnEnable(){
			cube.renderer.enabled = false;	
			cube.collider.enabled = false;
			OuterCube.renderer.enabled = false;	
			OuterCube.collider.enabled = false;
			collisionCube.renderer.enabled = false;
			collisionCube.collider.enabled = false;
			collisionCube.AddComponent<collisionDetection> ();
			collisionCube.AddComponent<Rigidbody> ();
			collisionCube.GetComponent<Rigidbody> ().isKinematic = true;
			collisionCube.GetComponent<BoxCollider> ().isTrigger = true;
		}

		public void openCollision(){
			collisionCube.collider.enabled = true;
		}
		public void closeCollision(){
			collisionCube.collider.enabled = false;
		}

		// set the name
		public void setName(string name){
			componentName = name;
			cube.name = name;
		}

		// set the cube size
		public void cubeScaleV3(Vector3 cubeSize){
			OuterCube.transform.localScale = cubeSize;
			componentSize = cubeSize;
		}

		// set the bone type
		public void setType(string t){
			boneType = t;		
		}

		public string getBoneType(){
			return boneType;		
		}

		// get name
		public string getName(){
			return componentName;
		}

		private void meshPosition(Vector3 loc){
			mesh.transform.position = loc;
		}

		public Vector3 getPosition(){
			return mesh.transform.position;		
		}

		// only for child, except torso
		public void setJointPosOnParent(Vector3 pos){
			jointPosOnParent = pos;
		}

		public Vector3 localToWorld(Vector3 local){

			return cube.transform.TransformPoint(local);
		}

		public Vector3 worldToLocal(Vector3 world){
			//GameObject testSpere = GameObject.CreatePrimitive (PrimitiveType.Sphere);
			//testSpere.transform.position = cube.transform.InverseTransformPoint (world);
			return cube.transform.InverseTransformPoint (world);		
		}
	
		// as child
		public void skeletonSphereDraw(string parentName){
			skeletonSphere = GameObject.CreatePrimitive (PrimitiveType.Sphere);
			skeletonSphere.transform.localScale = new Vector3(sphereRadius, sphereRadius, sphereRadius);
			skeletonSphere.transform.position = parent.localToWorld(jointPosOnParent);
			skeletonSphere.name = "sphere_" + parentName + "_"+componentName;
		}

		// as child
		public void skeletonBoneDraw(Vector3 pos){
			MeshFilter viewedModelFilter = mesh.GetComponentInChildren<MeshFilter> ();
			Mesh targetMesh = viewedModelFilter.mesh;

			float length = targetMesh.bounds.size.z;

			skeletonBone.transform.localScale = new Vector3(3.0f, 3.0f, length);
			skeletonBone.transform.position = cube.transform.TransformPoint (pos);
			skeletonBone.name = "bone_" + componentName;
			skeletonBone.collider.enabled=false;
		}

		// just for torso, draw center sphere
		public void TorsoCenterDraw(){
			if (!torsoSkeletonDrawn) {

				centerSphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
				centerSphere.transform.localScale = new Vector3(sphereRadius, sphereRadius, sphereRadius);
				centerSphere.transform.position = mesh.transform.position;
				centerSphere.name = "sphere_center";
				torsoSkeletonDrawn = true;
			}
		}

		// for torso's child, draw skeleton inside Torso
		public void TorsoInsideDraw(){

			insideBone = GameObject.CreatePrimitive (PrimitiveType.Sphere);

			Vector3 start = parent.localToWorld(jointPosOnParent);
			Vector3 end = parent.getPosition ();

			insideBone.transform.position = (start + end) / 2;

			Vector3 direction = start - end;
			insideBone.transform.rotation = Quaternion.LookRotation(direction);
			insideBone.transform.localScale = new Vector3(3,3,direction.magnitude);
			//Debug.Log ("Draw" + Quaternion.LookRotation(direction));
			insideBone.name = "InsideBone_for_"+componentName;
			insideBone.collider.enabled = false;
		}
		public GameObject getInsideBone(){
			return insideBone;
		}

		public void prepareChildStartPosList(){
			childrenPosStartList.Clear ();
			if (childrenList != null) {
				for(int i = 0;i<childrenList.Count;i++){
					childrenPosStartList.Add(childrenList[i].getPosition());
				}
			}
		}
		
		public void prepareChildEndPosList(Vector3 dis){
			childrenPosEndList.Clear ();
			if (childrenList != null) {
				for(int i = 0;i<childrenList.Count;i++){
					childrenPosEndList.Add(childrenList[i].getPosition()+dis);
				}
			}
		}

		public void prepareLerp(){
			lerpPrepareChildrenSuccess = false;
			if (childrenList != null) {
				for(int i = 0;i<childrenList.Count;i++){
					childrenList[i].prepareLerp();
				}
			}
		}

		public void lerp(float fracJourney, Vector3 start, Vector3 end){
			cube.transform.position = Vector3.Lerp(start, end, fracJourney);
			mesh.transform.position = Vector3.Lerp (start, end, fracJourney);
			OuterCube.transform.position = Vector3.Lerp (start, end, fracJourney);
			skeletonBone.transform.position = Vector3.Lerp (start, end, fracJourney);
			collisionCube.transform.position = Vector3.Lerp (start, end, fracJourney);

			Vector3 dis = end - start;

			if (lerpPrepareChildrenSuccess == false){
				prepareChildStartPosList ();
				prepareChildEndPosList (end - start);
				lerpPrepareChildrenSuccess = true;
			}

			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					
					childrenList[i].lerp(fracJourney, childrenPosStartList[i], childrenPosEndList[i]);
				}
			}
		}


		public void rotateAround(Vector3 point, Vector3 axis, float angle){
			cube.transform.RotateAround(point, axis, angle);
			OuterCube.transform.RotateAround(point, axis, angle);
			mesh.transform.RotateAround(point, axis, angle);
			skeletonBone.transform.RotateAround(point, axis, angle);
			collisionCube.transform.RotateAround(point, axis, angle);

			if (skeletonSphere != null)
				skeletonSphere.transform.RotateAround (point, axis, angle);

			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].rotateAround(point, axis, angle);
				}
			}
		}

		public void changeLocalPosition(Vector3 newPos){
			cube.transform.localPosition = newPos;
		}

		public Transform getTransform(){
			return cube.transform;		
		}

 		public void setParent(ref component pa){
			parent = pa;
		}

		public component getParent(){
			return parent;
		}

		public float getRotationAngleX(){
			return rotationAnglebyX;		
		}
		public float getRotationAngleY(){
			return rotationAnglebyY;		
		}
		public float getRotationAngleZ(){
			return rotationAnglebyZ;		
		}

		public void pushChild(ref component child){
			childrenList.Add (child);
		}

		public void setParameters(float headZSize){

			edit_speed = 0;
			sphereRadius = 0;
		}

		public void hideCube(){
			cube.renderer.enabled = false;
		}

		public Vector3 updateParentBoneSkeleton(){
			if (skeletonSphere != null){

				Vector3 jointWorld = skeletonSphere.transform.position;
				jointPosOnParent = parent.worldToLocal (jointWorld); // update jointPosOnParent
				if (parent != null) {
					if (parent.getName() != "Torso"){
						//Debug.Log("Normal Child");
						Vector3 start = jointWorld;
						Vector3 end = parent.parent.localToWorld (parent.jointPosOnParent);

						Vector3 meshOldLocation = parent.mesh.transform.position;
						Vector3 boneOldLocation = parent.skeletonBone.transform.position;
						Vector3 dis = meshOldLocation - boneOldLocation;

						Vector3 direction = start - end;
						parent.skeletonBone.transform.position = (start + end)/2;
						parent.skeletonBone.transform.rotation = Quaternion.LookRotation(direction);
						parent.skeletonBone.transform.localScale = new Vector3 (3, 3, direction.magnitude);



						parent.mesh.transform.position = (start + end)/2 + dis;
						parent.mesh.transform.rotation = Quaternion.LookRotation(direction);

						parent.cube.transform.position = (start + end)/2 + dis;
						parent.cube.transform.rotation = Quaternion.LookRotation(direction);
						parent.cube.transform.localScale = new Vector3 (3, 3, direction.magnitude);

						parent.OuterCube.transform.position = (start + end)/2 + dis;
						parent.OuterCube.transform.rotation = Quaternion.LookRotation(direction);

						parent.collisionCube.transform.position = (start + end)/2 + dis;
						parent.collisionCube.transform.rotation = Quaternion.LookRotation(direction);

					}
					else {
						// as Torso's child
						//Debug.Log("Torso's Child");
						Vector3 start = jointWorld;
						Vector3 end = parent.getPosition ();
						insideBone.transform.position = (start + end) / 2;
						
						Vector3 direction1 = start - end;
						insideBone.transform.rotation = Quaternion.LookRotation(direction1);
						insideBone.transform.localScale = new Vector3(3,3,direction1.magnitude);
					}
				}
			}
			return jointPosOnParent;
		}

		public void moveXPlus(){
			mesh.transform.Translate (edit_speed, 0f, 0f, Space.World);
			cube.transform.Translate (edit_speed, 0f, 0f, Space.World);
			OuterCube.transform.Translate (edit_speed, 0f, 0f, Space.World);
			skeletonBone.transform.Translate (edit_speed, 0f, 0f, Space.World);
			skeletonSphere.transform.Translate (edit_speed, 0f, 0f, Space.World);
			collisionCube.transform.Translate (edit_speed, 0f, 0f, Space.World);
			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].moveXPlus();
				}
			}
		}

		public void moveYPlus(){
			mesh.transform.Translate (0f, edit_speed, 0f, Space.World);
			cube.transform.Translate (0f, edit_speed, 0f, Space.World);
			OuterCube.transform.Translate (0f, edit_speed, 0f, Space.World);
			skeletonBone.transform.Translate (0f, edit_speed, 0f, Space.World);
			skeletonSphere.transform.Translate (0f, edit_speed, 0f, Space.World);
			collisionCube.transform.Translate (0f, edit_speed, 0f, Space.World);
			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].moveYPlus();
				}
			}
		}

		public void moveZPlus(){
			mesh.transform.Translate (0f, 0f, edit_speed, Space.World);
			cube.transform.Translate (0f, 0f, edit_speed, Space.World);
			OuterCube.transform.Translate (0f, 0f, edit_speed, Space.World);
			skeletonBone.transform.Translate (0f, 0f, edit_speed, Space.World);
			skeletonSphere.transform.Translate (0f, 0f, edit_speed, Space.World);
			collisionCube.transform.Translate (0f, 0f, edit_speed, Space.World);
			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].moveZPlus();
				}
			}
		}

		public void moveXNagetive(){
			mesh.transform.Translate (-edit_speed, 0f, 0f, Space.World);
			cube.transform.Translate (-edit_speed, 0f, 0f, Space.World);
			OuterCube.transform.Translate (-edit_speed, 0f, 0f, Space.World);
			skeletonBone.transform.Translate (-edit_speed, 0f, 0f, Space.World);
			skeletonSphere.transform.Translate (-edit_speed, 0f, 0f, Space.World);
			collisionCube.transform.Translate (-edit_speed, 0f, 0f, Space.World);
			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].moveXNagetive();
				}
			}
		}
		
		public void moveYNagetive(){
			mesh.transform.Translate (0f, -edit_speed, 0f, Space.World);
			cube.transform.Translate (0f, -edit_speed, 0f, Space.World);
			OuterCube.transform.Translate (0f, -edit_speed, 0f, Space.World);
			skeletonBone.transform.Translate (0f, -edit_speed, 0f, Space.World);
			skeletonSphere.transform.Translate (0f, -edit_speed, 0f, Space.World);
			collisionCube.transform.Translate (0f, -edit_speed, 0f, Space.World);
			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].moveYNagetive();
				}
			}
		}
		
		public void moveZNagetive(){
			mesh.transform.Translate (0f, 0f, -edit_speed, Space.World);
			cube.transform.Translate (0f, 0f, -edit_speed, Space.World);
			OuterCube.transform.Translate (0f, 0f, -edit_speed, Space.World);
			skeletonBone.transform.Translate (0f, 0f, -edit_speed, Space.World);
			skeletonSphere.transform.Translate (0f, 0f, -edit_speed, Space.World);
			collisionCube.transform.Translate (0f, 0f, -edit_speed, Space.World);
			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].moveZNagetive();
				}
			}
		}

		public void outerBoxScaleXPlus(){
			OuterCube.transform.localScale += new Vector3(0.3F, 0, 0);
			componentSize = OuterCube.transform.localScale;
		}

		public void outerBoxScaleXMinus(){
			OuterCube.transform.localScale -= new Vector3(0.3F, 0, 0);
			componentSize = OuterCube.transform.localScale;
		}

		public void outerBoxScaleYPlus(){
			OuterCube.transform.localScale += new Vector3(0, 0.3F, 0);
			componentSize = OuterCube.transform.localScale;
		}
		
		public void outerBoxScaleYMinus(){
			OuterCube.transform.localScale -= new Vector3(0, 0.3F, 0);
			componentSize = OuterCube.transform.localScale;
		}

		public void outerBoxScaleZPlus(){
			OuterCube.transform.localScale += new Vector3(0, 0, 0.3F);
			componentSize = OuterCube.transform.localScale;
		}
		
		public void outerBoxScaleZMinus(){
			OuterCube.transform.localScale -= new Vector3(0, 0, 0.3F);
			componentSize = OuterCube.transform.localScale;
		}

		public void localMoveXPlus(){
			mesh.transform.Translate (edit_speed, 0f, 0f, mesh.transform);
			
			musclePosition = mesh.transform.position;
			muscleLocalPosition = cube.transform.InverseTransformPoint (musclePosition);
		}
		
		public void localMoveYPlus(){
			mesh.transform.Translate (0f, edit_speed, 0f, mesh.transform);
			
			musclePosition = mesh.transform.position;
			muscleLocalPosition = cube.transform.InverseTransformPoint (musclePosition);
		}
		
		public void localMoveZPlus(){
			mesh.transform.Translate (0f, 0f, edit_speed, mesh.transform);
			
			musclePosition = mesh.transform.position;
			muscleLocalPosition = cube.transform.InverseTransformPoint (musclePosition);
		}
		
		public void localXNagetive(){
			mesh.transform.Translate (-edit_speed, 0f, 0f, mesh.transform);
			
			musclePosition = mesh.transform.position;
			muscleLocalPosition = cube.transform.InverseTransformPoint (musclePosition);    
		}
		
		public void localYNagetive(){
			mesh.transform.Translate (0f, -edit_speed, 0f, mesh.transform);
			
			musclePosition = mesh.transform.position;
			muscleLocalPosition = cube.transform.InverseTransformPoint (musclePosition);    
		}
		
		public void localZNagetive(){
			mesh.transform.Translate (0f, 0f, -edit_speed, mesh.transform);
			
			musclePosition = mesh.transform.position;
			muscleLocalPosition = cube.transform.InverseTransformPoint (musclePosition);    
		}



		public void localRotateAround(Vector3 angles){

			mesh.transform.Rotate(angles,Space.Self);
			localRotationList.Add (angles);
		}

		public void saveOriginalPosition(){
			originalPosition = mesh.transform.position;
		}

		public GameObject restart(){
			Debug.Log(componentName + "======");
			mesh.transform.position = originalPosition;
			mesh.transform.rotation = new Quaternion(0f,0f,0f,0f);
			cube.transform.position = originalPosition;
			cube.transform.rotation = new Quaternion(0f,0f,0f,0f);
			skeletonBone.transform.position = originalPosition;
			Debug.Log ("Location: "+mesh.transform.position);
			skeletonBone.transform.rotation = new Quaternion(0f,0f,0f,0f);
			skeletonBone.transform.localScale = new Vector3 (0f,0f,0f);
			Debug.Log (skeletonBone.name+": "+skeletonBone.transform.localScale);
			collisionCube.transform.position = originalPosition;
			collisionCube.transform.rotation = new Quaternion(0f,0f,0f,0f);
			return skeletonSphere;
		}

		public GameObject getCollisionCube(){
			return collisionCube;
		}

		public void autoMove(Vector3 move){
			mesh.transform.Translate (move, Space.World);
			cube.transform.Translate (move, Space.World);
			OuterCube.transform.Translate (move, Space.World);
			if (skeletonSphere != null)
				skeletonSphere.transform.Translate (move, Space.World);
			skeletonBone.transform.Translate (move, Space.World);
			collisionCube.transform.Translate (move, Space.World);

			if (childrenList.Count != 0){
				for (int i = 0; i < childrenList.Count; i++) {
					childrenList[i].autoMove(move);
				}
			}
		}

		public void selfMove(){
			mesh.transform.position = localToWorld (muscleLocalPosition);
		}
	}
	
}