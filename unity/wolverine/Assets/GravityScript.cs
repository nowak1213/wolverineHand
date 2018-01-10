using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GravityScript : MonoBehaviour {

	Rigidbody mRigidBody;

	void OnCollisionEnter(Collision collision) {
		if (collision.gameObject.isStatic) {
			if (GrabObject.isHeld == false) {
				Invoke ("freezeRotation", 2);
			}
		}
	}

	void freezeRotation() {
		mRigidBody = gameObject.GetComponent<Rigidbody> ();
		mRigidBody.freezeRotation = true;
		mRigidBody.constraints = RigidbodyConstraints.FreezeAll;

	}

}
