using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ThumbConnector : MonoBehaviour {
	
	public void OnCollisionEnter(Collision otherCollider) {
		GrabObject.thumbCollided = true;
	}

	public void OnCollisionExit(Collision otherCollider) {
		GrabObject.thumbCollided = false;
	}

}
