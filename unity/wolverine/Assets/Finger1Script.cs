using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Finger1Script : MonoBehaviour {

	public float fingerRotation = -10.0f;

	void Update () {

		if (GrabObject.isHeld == false) {
			this.fingerRotation = IMUData.distanceSensor1;
		} 
			
		transform.localEulerAngles = new Vector3 (fingerRotation, 0, 0);
	}

}
