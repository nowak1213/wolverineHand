using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ThumbScript : MonoBehaviour{

	public float thumbRotationZ = -20.0f;

	void Start () {
	}

	void Update () {

		if (GrabObject.isHeld == false) {
			this.thumbRotationZ = IMUData.thumbRotationZ;
		} 
		if (Input.GetKeyDown(KeyCode.K)) {
			IMUData.thumbRotationZ = 0.0f;
		}


		transform.localEulerAngles = new Vector3 (0, 0, -thumbRotationZ);
	}
}
