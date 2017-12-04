using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AnkleColliderScript : MonoBehaviour {

	public GameObject ankle;
	public bool isCollision = false;

	void OnCollisionStay(Collision collision) {
		isCollision = true;

	}
}
