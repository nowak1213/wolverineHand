using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GrabObject : MonoBehaviour {

	public static bool thumbCollided, finger1Collided, finger2Collided;
	public GameObject hand;
	private GameObject grabbedObject = null;
	public Rigidbody rb = null;
	private Transform objectOrgParent = null;
	public static bool isHeld = false; 


	void OnCollisionStay(Collision collision) {
		if (isHeld == false) {
			Debug.Log (gameObject.name + "Thumb " + thumbCollided + " finger 1 " + finger1Collided + " finger 2 " + finger2Collided);
			if (thumbCollided == true && finger1Collided == true && finger2Collided == true) {
				// the object we grab
				grabbedObject = collision.gameObject;
				// Make it kinematic as we are holding it now
				rb = grabbedObject.GetComponent<Rigidbody> ();
				rb.isKinematic = true;
				rb.constraints = RigidbodyConstraints.None;
				// Store the original parent to restore it when letting loose
				objectOrgParent = grabbedObject.transform.parent;
				// And parent it to the hand
				grabbedObject.transform.parent = hand.transform;

				thumbCollided = true;
				finger1Collided = true;
				finger2Collided = true;
				isHeld = true;
			}
		}
	}

	void FixedUpdate() {
		if (isHeld == true) {
			if (grabbedObject != null) {
				if (Input.GetKeyDown(KeyCode.P) == true) {
					isHeld = false;
					thumbCollided = false;
					finger1Collided = false;
					finger2Collided = false;
					// unparent it from the thumb
					grabbedObject.transform.parent = objectOrgParent;
					// make it non-kinematic again
					rb = grabbedObject.GetComponent<Rigidbody> ();
					rb.isKinematic = false;
					// and clear the grabbed object
					grabbedObject = null;
				}
			}
		}

	}
}
