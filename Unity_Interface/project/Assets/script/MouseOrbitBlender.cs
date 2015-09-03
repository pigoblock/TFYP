using UnityEngine;
using System.Collections;

[AddComponentMenu("Camera-Control/Mouse Orbit with zoom")]

public class MouseOrbitBlender : MonoBehaviour
{
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
	private bool camera_move;

	private RaycastHit hit;

	void Awake()
	{
		Vector3 angles = transform.eulerAngles;
		x = angles.x;
		y = angles.y;
		
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
				x += (float)(Input.GetAxis("Mouse X") * xSpeed * 0.02f);
				y -= (float)(Input.GetAxis("Mouse Y") * ySpeed * 0.02f);
				
				//y = ClampAngle(y, yMinLimit, yMaxLimit);

				if (Input.GetAxis("Mouse ScrollWheel") > 0)
					Distance -= 3f;
				if (Input.GetAxis("Mouse ScrollWheel") < 0)
					Distance += 3f;


				Quaternion rotation = Quaternion.Euler(y, x, 0);
				Vector3 position = rotation * (new Vector3(0.0f, 0.0f, -Distance)) + Target.position;
				
				transform.rotation = rotation;
				transform.position = position;
			}
		}
	}


	void Update(){
		if (Input.GetKeyDown ("c")) {
			camera_move = (camera_move==true) ? false: true; 
		}


	}
	void OnGUI(){
		GUI.Label(new Rect(Screen.width * 0.03f, Screen.height*0.86f,70,60), "Camera Rotate:\n(press c)", style1);
		GUI.Label (new Rect (Screen.width * 0.12f, Screen.height * 0.86f, 70, 60), "  "+camera_move, style3);
	}

	public void changeTarget(Transform t){
		Target = t;
	}
}
