using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MeleeSystem : MonoBehaviour {

	public int Damage = 50;
	public double Distance;
    public double MaxDistance = 2.5;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        if (Input.GetButtonDown("Fire1")) {

            RaycastHit hit;
            if(Physics.Raycast(transform.position, transform.TransformDirection(Vector3.forward), out hit)) {
                Distance = hit.distance;
                if(Distance < MaxDistance) {
                    hit.transform.SendMessage("ApplyDamage", Damage, SendMessageOptions.DontRequireReceiver);
                }

            }

        }
	}
}
