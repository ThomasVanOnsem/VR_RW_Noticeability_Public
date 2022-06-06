using System;
using System.Collections;
using System.Collections.Generic;
using Unity.XR.OpenVR;
using UnityEngine;
using UnityEngine.SpatialTracking;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Globalization;
using System.Text;
using UnityEngine.Serialization;
using UnityEngine.XR;

public class Test : MonoBehaviour
{
    //private string _IP = "127.0.0.1";
    private string _IP = "192.168.0.234";
    private int _port = 5555;

    private IPEndPoint _remoteEndPoint;
    private UdpClient _client;
    
    private GameObject _centerEye;
    private Vector3 _headsetPos;

    private IPEndPoint _remoteIpEndPoint;

    public void Start()
    {
        _remoteEndPoint = new IPEndPoint(IPAddress.Parse(_IP), _port);
        _client = new UdpClient();
        _centerEye = GameObject.Find("Main Camera");
        _remoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);


        print("Sending to "+_IP+" : "+_port);
        SendString("Test message");
        
        List<XRInputSubsystem> subsystems = new List<XRInputSubsystem>();
        SubsystemManager.GetInstances(subsystems);
        foreach (var subsystem in subsystems)
        {
            print("IN LOOP");
            List<Vector3> testing = new List<Vector3>();
            subsystem.TryGetBoundaryPoints(testing);
            foreach (var elem in testing)
            {
                //print("ELEM: " + elem);
                SendString(elem.ToString());
            }
        }
    }

    public void Update()
    {
        _headsetPos = _centerEye.transform.position;
        SendString("Pos: " + _headsetPos);
        //SendString("Rot: " + _centerEye.transform.rotation);
        try
        {
            Byte[] receiveBytes = _client.Receive(ref _remoteIpEndPoint);
            string returnData = Encoding.ASCII.GetString(receiveBytes);
            print("RECIEVED: " + returnData.ToString());
        }
        catch (Exception err)
        {
            print(err.ToString());
        }

    }

    private void SendString(string message)
    {
        try
        {
            byte[] data = Encoding.UTF8.GetBytes(message);
            _client.Send(data, data.Length, _remoteEndPoint);
        }
        catch (Exception err)
        {
            print(err.ToString());
        }
    }

}
