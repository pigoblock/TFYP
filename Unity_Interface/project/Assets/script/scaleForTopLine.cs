using UnityEngine;
using System.Collections;

public class scaleForTopLine : MonoBehaviour {
	
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
		GetComponent<GUITexture>().pixelInset = new Rect(-Screen.width*0.3f,Screen.height*0.4f-Screen.height*0.01f, Screen.width*0.6f, Screen.height*0.01f);
	}
}
