using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class CameraScript : MonoBehaviour {
	
	public float cameraRotationX, cameraRotationY;

	void Start () {
	}

	void Update () {

		this.cameraRotationX = IMUData.handRotationX;
		this.cameraRotationY = IMUData.handRotationZ;

		if (Input.GetKeyDown(KeyCode.K)) {
			IMUData.handRotationY= 0.0f;
			IMUData.handRotationZ= 0.0f;
		}

		transform.localEulerAngles = new Vector3 (-cameraRotationX, cameraRotationY, 0);
	}



}

