using UnityEngine;
using System.Collections;

public class scaleForTopback : MonoBehaviour {
	public Vector2 scaleOnRatio = new Vector2 (0.1f, 0.2f);
	public Vector2 haha = new Vector2(10,10);
	private float widthHeightRatio;
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
		widthHeightRatio = (float)Screen.width / Screen.height * Screen.height;
		//myTrans.localScale = new Vector3 (scaleOnRatio.x, widthHeightRatio * scaleOnRatio.y, 1);
		GetComponent<GUITexture>().pixelInset = new Rect(-Screen.width/2, -Screen.height/2, Screen.width*0.2f, Screen.height);
	}
}
