using UnityEngine;
using System.Collections;

[AddComponentMenu("Camera-Control/Mouse Orbit with zoom")]

public class zoomInout : MonoBehaviour {

	public Transform Target;
	public float Distance = 70.0f;
	public float xSpeed = 250.0f;
	public float ySpeed = 120.0f;
	public float yMinLimit = -20.0f;
	public float yMaxLimit = 80.0f;
	public GUIStyle style1;
	public GUIStyle style3;
	
	private float x;
	private float y;
	private float z;
	private bool camera_move;
	
	private RaycastHit hit;
	
	void Awake()
	{
		Vector3 angles = transform.eulerAngles;
		x = angles.x;
		y = angles.y;
		z = angles.z;
		
		if(GetComponent<Rigidbody>() != null)
		{
			GetComponent<Rigidbody>().freezeRotation = true;
		}
	}
	
	void LateUpdate()
	{
		if (camera_move){
			if(Target != null)
			{
				//y = ClampAngle(y, yMinLimit, yMaxLimit);
				
				if (Input.GetAxis("Mouse ScrollWheel") > 0)
					Distance -= 3f;
				if (Input.GetAxis("Mouse ScrollWheel") < 0)
					Distance += 3f;
				
				
				Quaternion rotation = Quaternion.Euler(x, y, z);
				Vector3 position = rotation * (new Vector3(0.0f, 0.0f, -Distance)) + Target.position;
				
				transform.rotation = rotation;
				transform.position = position;
			}
		}
	}
	
	
	void Update(){
		if (Input.GetKeyDown ("o")) {
			camera_move = (camera_move==true) ? false: true; 
		}
		
		
	}
	void OnGUI(){
		GUI.Label(new Rect(Screen.width * 0.03f, Screen.height*0.91f,70,60), "Sub Camera \nRotate:\n(press o)", style1);
		GUI.Label (new Rect (Screen.width * 0.12f, Screen.height * 0.935f, 70, 60), "  "+camera_move, style3);
	}
	
	public void changeTarget(Transform t){
		Target = t;
	}
}
