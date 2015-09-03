using UnityEngine;
using System.Collections;

public class ScaleBack1 : MonoBehaviour {

	// Use this for initialization
	void Start () {
	}
	
	// Update is called once per frame
	void Update () {
		setScale ();
	}
	
	void setScale(){
		GetComponent<GUITexture>().pixelInset = new Rect(-Screen.width*0.46f, -Screen.height*0.44f, Screen.width*0.135f, Screen.height*0.3f);
	}
}
