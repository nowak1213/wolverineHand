using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class CameraScript : MonoBehaviour {

	SerialPort stream = new SerialPort("COM3", 9600);
	private double amountToMove;
	public float gX, gY, gZ, aX, aY, aZ;

	public Transform target;



	Vector3 actualImuInput = Vector3.zero;

	void Start () {
		stream.Open ();
		stream.ReadTimeout = 2000;

	}
	// Update is called once per frame
	void Update () {
		if (!stream.IsOpen) 
			stream.Open ();


		StartCoroutine
		(
			AsynchronousReadFromArduino
			(   (string s) => Debug.Log(s),     // Callback
				() => Debug.LogError("Error!"), // Error callback
				10f                             // Timeout (seconds)
			)
		);

		if (Input.GetKeyDown(KeyCode.K)) {
			this.gX = 0.0f;
			this.gY = 0.0f;
			this.gZ = 0.0f;
		}
		//			transform.rotation = Quaternion.Euler (0, 0, 0);

		this.transform.Rotate (0, gY, 0);

	}

	public IEnumerator AsynchronousReadFromArduino(Action<string> callback, Action fail = null, float timeout = float.PositiveInfinity){
		DateTime initialTime = DateTime.Now;
		DateTime nowTime;
		TimeSpan diff = default(TimeSpan);

		char delimeterChar = ',';
		string dataString = null;
		string[] imuValues;

		do {
			try {
				dataString = stream.ReadLine();
			}
			catch (TimeoutException) {
				dataString = null;
			}

			if (dataString != null)
			{
				imuValues = dataString.Split(delimeterChar);
				this.gX += float.Parse(imuValues[0]) / 3000;
				this.gY += float.Parse(imuValues[2]) / 3000;
				this.gZ += float.Parse(imuValues[1]) / 3000;

				this.aX = float.Parse(imuValues[3]) / 3000;
				this.aY = float.Parse(imuValues[5]) / 3000;
				this.aZ += float.Parse(imuValues[4]) / 3000;

				callback(dataString);

				yield return null;
			} else
				yield return new WaitForSeconds(0.05f);

			nowTime = DateTime.Now;
			diff = nowTime - initialTime;

		} while (diff.Milliseconds < timeout);

		if (fail != null)
			fail();
		yield return null;

	}

}

