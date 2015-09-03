using UnityEngine;
using System.Collections;

public class scaleForDownLine : MonoBehaviour {
	
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
		GetComponent<GUITexture>().pixelInset = new Rect(-Screen.width*0.3f,-Screen.height/2, Screen.width*0.6f, Screen.height*0.01f);
	}
}
