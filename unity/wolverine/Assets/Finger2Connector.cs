using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Finger2Connector : MonoBehaviour {

	public bool col;

	public void OnCollisionEnter(Collision otherCollider) {
		GrabObject.finger2Collided = true;
		col = true;
	}

	public void OnCollisionExit(Collision otherCollider) {
		GrabObject.finger2Collided = false;
		if (GrabObject.isHeld == true) {
			GrabObject.finger2Collided = true;
		}
	}

//	public void OnCollisionStay(Collision otherCollider) {
//		GrabObject.finger2Collided = true;
//	}
}
