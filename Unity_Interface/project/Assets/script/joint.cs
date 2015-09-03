using UnityEngine;
using System.Collections;
using Component;
using System;
using System.Collections.Generic;






namespace Joint{

	public class rotationUnit{
		public Vector3 rotationLocalVector;
		public float degree;
		public bool ifStart;
		
		public rotationUnit(){
			rotationLocalVector = new Vector3 (0f, 0f, 0f);
			degree = 0f;
			ifStart = false;
		}

		public rotationUnit(Vector3 rv, float d){
			rotationLocalVector = rv;
			degree = d;
			ifStart = false;
		}
	}

	public class joint{

		public static float moveSpeed = 10f;
		public static float rotateSpeed = 60.0f;

		component parent;
		component child;

		// data in the skeleton.xml
		public Vector3 rawJointPosOfParentBasedPa;
		public Vector3 rawJointPosOfChildBasedPa;

		// data in GameObject's coordinates (scale)
		private Vector3 jointLocalPosBasedOnPa;
		private Vector3 jointLocalPosBasedOnChi;

		// data in world coordinates
		Vector3 jointWorldPosParent;
		Vector3 jointWorldPosChild;

		//Vector3 jointPosOfPaInPa;
		//Vector3 jointPosOfChiInPa;

		bool enable = false;
		bool enableMove = true;
		bool enableSelfMove = false;
		bool enableRotate = false;
		//public bool startRotate = false;
		float distanceInPa = 0.0F;
		float distance = 0.0F;
		float distaneX, distancY, distanceZ;

		private List<rotationUnit> rotationList;


		float angleCovered = 0;

		//Vector3 parentPosition;
		Vector3 childPosition;

		Vector3 autoMoveVector = new Vector3(0f,0f,0f);

		public bool autoEnable;

		public joint(){
			rotationList = new List<rotationUnit> ();
		}

		public joint(component pa, component chi){
			parent = pa;
			child = chi;

		}

		public void prepare(){

			// get the parent world location
			jointWorldPosParent = parent.localToWorld (jointLocalPosBasedOnPa);
			jointWorldPosChild = child.localToWorld (jointLocalPosBasedOnChi);

			//Debug.Log ("&&&&&&: "+jointWorldPosParent);
			//Debug.Log ("&&&&&&: "+jointWorldPosChild);
			child.setJointPosOnParent (jointLocalPosBasedOnPa);
			child.skeletonSphereDraw (parent.getName());
			child.skeletonBoneDraw (new Vector3(0.0f,0.0f,0.0f));

			if (parent.parent == null) {
				//Debug.Log("12345678");
				//Debug.Log("Name: "+parent.getName());
				parent.TorsoCenterDraw();
				child.TorsoInsideDraw();
			}

			//jointPosOfPaInPa = jointLocalPosBasedOnPa;
			//jointPosOfChiInPa = parent.worldToLocal(child.localToWorld(jointLocalPosBasedOnChi));
			// Since we want the child move in parent's coordinates, we need to find each joint connect point's position in parent's coordinates.



			distance = Vector3.Distance (jointWorldPosParent, jointWorldPosChild);
			//parentPosition = parent.getPosition ();
			childPosition = child.getPosition ();

			//Debug.Log ("Joint Prepare: Rotate X angles: " + rotateAngleByX);
			//Debug.Log ("Joint Prepare: Rotate Y angles: " + rotateAngleByY);
			//Debug.Log ("Joint Prepare: Rotate Z angles: " + rotateAngleByZ);
			//rotateAngleByX = rotateAngleByX ;//+ parent.getRotationAngleX();
			//rotateAngleByY = rotateAngleByY ;//+ parent.getRotationAngleY();
			//rotateAngleByZ = rotateAngleByZ ;//+ parent.getRotationAngleZ();
			//Debug.Log ("Rotation X "+rotateAngleByX);
			//Debug.Log ("Rotation Y "+rotateAngleByY);
			//Debug.Log ("Rotation Z "+rotateAngleByZ);
			//child.setRotationXYZ (rotateAngleByX, rotateAngleByY, rotateAngleByZ);

			setJointMoveEnableTrue ();
			foreach (rotationUnit unit in rotationList)
			{
				if (unit.degree < 0){
					if (unit.rotationLocalVector == Vector3.left)
						unit.rotationLocalVector = Vector3.right;
					if (unit.rotationLocalVector == Vector3.down)
						unit.rotationLocalVector = Vector3.up;
					if (unit.rotationLocalVector == Vector3.back)
						unit.rotationLocalVector = Vector3.forward;

					unit.degree = Mathf.Abs(unit.degree);
				}
			}
			rotationList [0].ifStart = true;

			angleCovered = 0.0f;

			//Vector3 endpos = childPosition + (jointWorldPosParent - jointWorldPosChild);

			//Debug.Log ("Start at: " + childPosition);
			//Debug.Log ("End at: " + endpos);
			//Debug.Log ("Joint: "+getJointParentName()+getJointChildName());
			//Debug.Log ("Joint Prepare: Rotate X Vector: " + rotateXVector);
			//Debug.Log ("Joint Prepare: Rotate Y Vector: " + rotateYVector);
			//Debug.Log ("Joint Prepare: Rotate Z Vector: " + rotateZVector);
			child.prepareLerp ();
		}

		public void move(float startTime){
			collisionDetection cd = child.collisionCube.GetComponent<collisionDetection> ();
			cd.setTarget ("M_" + parent.getName ());




			float distCovered = (Time.time - startTime) * moveSpeed;
			float fracJourney = distCovered / distance;

			Vector3 endpos = childPosition + (jointWorldPosParent - jointWorldPosChild);

			//Debug.Log (getJointChildName() + "+" + getJointParentName());
	
			child.lerp(fracJourney, childPosition, endpos);

			//Debug.Log ("~~~~~~~  cd.getNoNeedAuto" + cd.getNoNeedAuto());


			if (fracJourney > 1.0F){
				setJointMoveEnableFalse();
				setJointRotateEnableTrue();
			}
		}


		public bool rotate2(){
			for (int i = 0; i < rotationList.Count; i++)
			{
				rotationUnit unit = rotationList[i];

				if (unit.ifStart == true){
					angleCovered = angleCovered + rotateSpeed * Time.deltaTime;
					Vector3 worldRotationVector = child.getTransform().TransformDirection(unit.rotationLocalVector);
					child.rotateAround(jointWorldPosParent, worldRotationVector, rotateSpeed * Time.deltaTime);	

					if (i+1 < rotationList.Count) {
						if (angleCovered >= unit.degree) {
							child.selfMove();
							unit.ifStart = false;
							rotationList[i+1].ifStart = true;
							angleCovered = 0.0f;
						}
					} else {
						// the last one
						if (angleCovered >= unit.degree) {
							child.selfMove();
							unit.ifStart = false;
							angleCovered = 0.0f;
							setJointEnableFalse();
							setJointRotateEnableFalse();
							return true;
						}
					}
				} 
			} // end of for loop
			return false;
		}
		public bool jointEnable(){
			return enable;
		}
		public void setJointEnableTrue(){
			enable = true;
		}
		public void setJointEnableFalse(){
			enable = false;
		}

		public bool jointMoveEnable(){
			return enableMove;
		}
		public void setJointMoveEnableTrue(){
			enableMove = true;
		}
		public void setJointMoveEnableFalse(){
			enableMove = false;
		}

		public bool jointRotateEnable(){
			return enableRotate;
		}
		public void setJointRotateEnableTrue(){
			enableRotate = true;
		}
		public void setJointRotateEnableFalse(){
			enableRotate = false;
		}

		/*public void setStartRotate(){
			startRotate = true;
		}

		public bool getStartRotate(){
			return startRotate;
		} */

		public component getChild(){
			return child;		
		}

		public component getParent(){
			return parent;		
		}


		public string getJointParentName(){
			if (parent != null) {
				return parent.getName();			
			}
			return "";
		}
		public string getJointChildName(){
			if (child != null) {
				return child.getName();			
			}
			return "";
		}

		public void setJointParent(ref component pa){
			parent = pa;
		}

		public void setJointChild(ref component chi){
			child = chi;
		}

		public void prepareLocalJointPosOfParent(){

				//Debug.Log ("Enter prepare");
			jointLocalPosBasedOnPa.x = rawJointPosOfParentBasedPa.x / parent.componentSize.x;
			jointLocalPosBasedOnPa.z = (rawJointPosOfParentBasedPa.z - parent.componentSize.z/2)/parent.componentSize.z;
			jointLocalPosBasedOnPa.y = rawJointPosOfParentBasedPa.y / parent.componentSize.y;

			//Debug.Log ("Parent joint: "+jointLocalPosBasedOnPa);
		}

		public Vector3 convertBackToRaw(){
			rawJointPosOfParentBasedPa.x = jointLocalPosBasedOnPa.x * parent.componentSize.x;
			rawJointPosOfParentBasedPa.z = jointLocalPosBasedOnPa.z * parent.componentSize.z + parent.componentSize.z/2;
			rawJointPosOfParentBasedPa.y = jointLocalPosBasedOnPa.y * parent.componentSize.y;

			return rawJointPosOfParentBasedPa;
		}

		public void prepareLocalJointPosOfChild(){

			//	jointLocalPosBasedOnChi.x = rawJointPosOfChildBasedPa.x / child.componentSize.x;
			//	jointLocalPosBasedOnChi.y = (rawJointPosOfChildBasedPa.y - child.componentSize.y/2)/child.componentSize.y;
			//	jointLocalPosBasedOnChi.z = rawJointPosOfChildBasedPa.z / child.componentSize.z;
			jointLocalPosBasedOnChi.x = 0f;
			jointLocalPosBasedOnChi.y = 0f;
			jointLocalPosBasedOnChi.z = -0.5f;
			//Debug.Log ("Child joint: "+jointLocalPosBasedOnChi);
		}

		public Vector3 getJointLocalPosOfParent(){
			return jointLocalPosBasedOnPa;
		}

		public void setJointLocalPosBasedOnPa(Vector3 newPos){
			jointLocalPosBasedOnPa = newPos;
		}
		public Vector3 getJointLocalPosOfChild(){
			return jointLocalPosBasedOnChi;
		}

		public Vector3 getJointRawLocalPosOfParent(){
			return rawJointPosOfParentBasedPa;
		}
		public Vector3 getJointRawLocalPosOfChild(){
			return rawJointPosOfChildBasedPa;
		}

	
		public float getRotation(int index){
			float curDe = rotationList [index].degree;



			return rotationList [index].degree;
		}

		public void addRotation(Vector3 cv, float degree){
			if (rotationList.Count > 0) {
				// not empty	
				rotationUnit last = rotationList[rotationList.Count-1];

				if (last.rotationLocalVector == cv)
					last.degree += degree;
				else
					rotationList.Add (new rotationUnit(cv, degree));
			} else {
				// empty
				rotationList.Add (new rotationUnit(cv, degree));
			}
		}

		public void PrepareAutoMoveVectot(){
			autoMoveVector = parent.mesh.transform.position - child.skeletonSphere.transform.position;
		}

		public Vector3 getAutoMoveVecotr(){
			return autoMoveVector;
		}

	}
}
