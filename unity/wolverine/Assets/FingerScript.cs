using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FingerScript : MonoBehaviour{


	public float cameraRotationX, cameraRotationY, cameraRotationZ;

	void Start () {
	}

	void Update () {

		this.cameraRotationY = IMUData.thumbRotationY*250;
		this.cameraRotationZ = IMUData.thumbRotationZ*250;

		if (Input.GetKeyDown(KeyCode.K)) {
			IMUData.thumbRotationY = 0.0f;
			IMUData.thumbRotationZ = 0.0f;
		}

		transform.localEulerAngles = new Vector3 (cameraRotationZ, cameraRotationY, 0);
	}

}
