using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColisionTest : MonoBehaviour {

	public bool czyTriggered = false; 
	void OnTriggerEnter(Collider collider) {
		Debug.Log (gameObject.name + " was triggered " + collider.gameObject.name);
		czyTriggered = true;
	}
}
