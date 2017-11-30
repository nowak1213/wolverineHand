using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;
public class IMUData : MonoBehaviour {

	private SerialPort stream = new SerialPort("COM3", 9600);

	public static float handPositionX, handPositionY, handPositionZ, handRotationX, handRotationY, handRotationZ;
	public static float thumbPositionX, thumbPositionY, thumbPositionZ, thumbRotationX, thumbRotationY, thumbRotationZ;

	// Use this for initialization
	void Start () {
		stream.Open ();
		stream.ReadTimeout = 2000;
	}
	
	// Update is called once per frame
	void Update () {
		if (!stream.IsOpen)
			stream.Open ();

		handRotationZ = Mathf.Clamp (handRotationZ, -90, 90);
		handRotationX = Mathf.Clamp (handRotationX, -90, 90);

		StartCoroutine
		(
			AsynchronousReadFromArduino
			((string s) => Debug.Log (s),     // Callback
				() => Debug.LogError ("Error!"), // Error callback
				10f                             // Timeout (seconds)
			)
		);
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
				handRotationX += float.Parse(imuValues[0]) / 3000;
				handRotationY += float.Parse(imuValues[1]) / 3000;
				handRotationZ += float.Parse(imuValues[2]) / 3000;

//				handPositionX = float.Parse(imuValues[3]) / 3000;
//				handPositionY = float.Parse(imuValues[5]) / 3000;
//				handPositionZ = float.Parse(imuValues[4]) / 3000;

				thumbRotationX += float.Parse(imuValues[6]) / 3000;
				thumbRotationY += float.Parse(imuValues[7]) / 3000;
				thumbRotationZ += float.Parse(imuValues[8]) / 3000;

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
