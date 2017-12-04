using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class HandMovement : MonoBehaviour {

	public float handRotationZ;

	void Start () {

	}

	void Update () {

		this.handRotationZ = IMUData.handRotationY;

		if (Input.GetKeyDown(KeyCode.K)) {
			IMUData.handRotationX = 0.0f;
		}


		transform.localEulerAngles = new Vector3 (0, 0, handRotationZ);
	}

}
