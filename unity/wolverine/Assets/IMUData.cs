using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;
public class IMUData : MonoBehaviour {

	private SerialPort stream = new SerialPort("COM3", 9600);

	public static float handRotationX, handRotationY, handRotationZ;
	public static float thumbRotationZ;
	public static float distanceSensor1, distanceSensor2;
	public const float distanceConst = 6.4f;
	public const float maxFingerRange = -10.0f;
	public bool previousCollision1 = false, previousCollision2 = false;

	// Use this for initialization
	void Start () {
		stream.Open ();
		stream.ReadTimeout = 2000;
	}
	
	// Update is called once per frame
	void Update () {
		if (!stream.IsOpen)
			stream.Open();

//		handRotationZ = Mathf.Clamp (handRotationZ, -90, 90);
		handRotationY = Mathf.Clamp (handRotationY, -90, 90);
		handRotationX = Mathf.Clamp (handRotationX, -90, 90);
		thumbRotationZ = Mathf.Clamp (thumbRotationZ, -20, 25);
		writeServo1(GrabObject.finger1Collided);
		writeServo2(GrabObject.finger2Collided);

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
		String dataString = null;
		String[] imuValues;
		float imuHandRotationX, imuHandRotationY, imuHandRotationZ;
		float imuThumbRotationZ;
		float imuDistance1, imuDistance2;
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
				imuHandRotationX = float.Parse(imuValues[0])*1.5f;
				imuHandRotationY = float.Parse(imuValues[1]);
				imuHandRotationZ = float.Parse(imuValues[2])*1.5f;
				imuThumbRotationZ = float.Parse(imuValues[5])*1.5f;
				imuDistance1 = float.Parse(imuValues[6]);
				imuDistance2 = float.Parse(imuValues[7]);

				if(imuHandRotationX >= 1.5f || imuHandRotationX <= -1.5f){
					handRotationX += imuHandRotationX;
				}
				if(imuHandRotationY >= 0.5f || imuHandRotationY <= -0.5f){
					handRotationY += imuHandRotationY;
				}
				if(imuHandRotationZ >= 1.5f || imuHandRotationZ <= -1.5f){
					handRotationZ += imuHandRotationZ;
				}

				if(imuHandRotationZ <= 1.5f && imuHandRotationZ >= -1.5f){
					if(imuThumbRotationZ >= 1.5f || imuThumbRotationZ <= -1.5f){
						thumbRotationZ += imuThumbRotationZ;
					}
				}

				if (imuDistance2 < 3) {
					distanceSensor2 = maxFingerRange;
				} else if (imuDistance2 > 3 && imuDistance2 < 4) {
					distanceSensor2 = maxFingerRange + distanceConst;
				} else if (imuDistance2 > 4 && imuDistance2 < 5) {
					distanceSensor2 = maxFingerRange + 2*distanceConst;
				} else if (imuDistance2 > 5 && imuDistance2 < 6) {
					distanceSensor2 = maxFingerRange + 3*distanceConst;
				} else if (imuDistance2 > 6 && imuDistance2 < 7) {
					distanceSensor2 = maxFingerRange + 4*distanceConst;
				} else if (imuDistance2 > 7 && imuDistance2 < 8) {
					distanceSensor2 = maxFingerRange + 5*distanceConst;
				} else if (imuDistance2 > 8 && imuDistance2 < 9) {
					distanceSensor2 = maxFingerRange + 6*distanceConst;
				} else {
					distanceSensor2 = maxFingerRange + 7*distanceConst;
				}

				if (imuDistance1 < 3) {
					distanceSensor1 = maxFingerRange;
				} else if (imuDistance1 > 3 && imuDistance1 < 4) {
					distanceSensor1 = maxFingerRange + distanceConst;
				} else if (imuDistance1 > 4 && imuDistance1 < 5) {
					distanceSensor1 = maxFingerRange + 2*distanceConst;
				} else if (imuDistance1 > 5 && imuDistance1 < 6) {
					distanceSensor1 = maxFingerRange + 3*distanceConst;
				} else if (imuDistance1 > 6 && imuDistance1 < 7) {
					distanceSensor1 = maxFingerRange + 4*distanceConst;
				} else if (imuDistance1 > 7 && imuDistance1 < 8) {
					distanceSensor1 = maxFingerRange + 5*distanceConst;
				} else if (imuDistance1 > 8 && imuDistance1 < 9) {
					distanceSensor1 = maxFingerRange + 6*distanceConst;
				} else {
					distanceSensor1 = maxFingerRange + 7*distanceConst;
				}

//				writeToArduino(Convert.ToString(GrabObject.finger1Collided), Convert.ToString(GrabObject.finger2Collided));

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

	public void writeToArduino(String collision1) {
		stream.WriteLine (collision1);
		stream.BaseStream.Flush ();
	}

	public void writeServo1(bool collision) {
		String dataToSend;
		if (collision == true && previousCollision1 == false) {
			dataToSend = "servo1True";
			stream.WriteLine (dataToSend);
			stream.BaseStream.Flush ();
		} else if (collision == false && previousCollision1 == true) {
			dataToSend = "servo1False";
			stream.WriteLine (dataToSend);
			stream.BaseStream.Flush ();
		}
		previousCollision1 = collision;
	}

	public void writeServo2(bool collision) {
		String dataToSend;
		if (collision == true && previousCollision2 == false) {
			dataToSend = "servo2True";
			stream.WriteLine (dataToSend);
			stream.BaseStream.Flush ();
		} else if (collision == false && previousCollision2 == true) {
			dataToSend = "servo2False";
			stream.WriteLine (dataToSend);
			stream.BaseStream.Flush ();
		}
		previousCollision2 = collision;
	}
}
