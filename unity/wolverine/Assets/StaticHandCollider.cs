using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StaticHandCollider : MonoBehaviour {

	bool collided;
	Collider collider;

	// Update is called once per frame
	void Update () {
		
	}

	public void OnTriggerEnter(Collider otherCollider) {
		if (!collided && otherCollider.gameObject.isStatic == true) {
			collided = true;
			collider = otherCollider;
		}
	}

	public void OnTriggerExit(Collider otherCollider) {
		if (collided && otherCollider == collider) {
			collided = false;
			collider = null;
		}
	}
}
