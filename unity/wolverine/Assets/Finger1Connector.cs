using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Finger1Connector : MonoBehaviour {

	public void OnCollisionEnter(Collision otherCollider) {
		GrabObject.finger1Collided = true;
	}

	public void OnCollisionExit(Collision otherCollider) {
		GrabObject.finger1Collided = false;
		if (GrabObject.isHeld == true) {
			GrabObject.finger1Collided = true;
		}

	}

//	public void OnCollisionStay(Collision otherCollider) {
//		GrabObject.finger1Collided = true;
//	}
}
