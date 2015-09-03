using UnityEngine;
using System.Collections;

public class collisionDetection : MonoBehaviour {
	string targetName;
	bool stop = false;
	bool noNeedAuto = false;
	bool autoStart = false;

	void OnTriggerEnter(Collider cd) {
		//Debug.Log (gameObject.name);

		if (autoStart){

			if (cd.gameObject.name == targetName) {
				Debug.Log("Hit" + targetName);
				stop = true;
			}
		} 
	}


	public void setTarget(string name){
	
		targetName = name;
	}


	public bool getStop(){
		return stop;
	}
	

	public void startAuto(){
		autoStart = true;
	}
}
