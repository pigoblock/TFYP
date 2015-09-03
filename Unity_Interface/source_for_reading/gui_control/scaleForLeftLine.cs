using UnityEngine;
using System.Collections;

public class scaleForLeftLine : MonoBehaviour {
	private Transform myTrans; 
	// Use this for initialization
	void Start () {
		myTrans = transform;
	}
	
	// Update is called once per frame
	void Update () {
		setScale ();
	}
	
	void setScale(){
		guiTexture.pixelInset = new Rect(-Screen.width*0.3f,-Screen.height/2, Screen.height*0.01f, Screen.height*0.9f);
	}
}
