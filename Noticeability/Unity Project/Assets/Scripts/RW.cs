using System;
using System.Collections.Generic;
using System.Globalization;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.XR;

public class RW : MonoBehaviour
{
    private const string IP = "192.168.0.234";
    private const int Port = 5555;

    private const int MessageSendFrequency = 5;

    private IPEndPoint _remoteEndPoint;
    private UdpClient _client;
    
    private GameObject _user;
    private GameObject _path;
    [SerializeField]
    private Text _text;

    private IPEndPoint _remoteIpEndPoint;
    
    private Thread _tSend;
    private Thread _tReceive;

    private float _prevRotOffset;
    private int _resetCounter = 0;
    private const int ResetShowLength = 10;
    private bool _resetting = false;
    
    // Start is called before the first frame update
    void Start()
    {
        _user = GameObject.Find("Main Camera");
        _path = GameObject.Find("RotationPoint");
        //AdjustPath();
        
        SetupUDP();
        SendEnvData();

        //Start threads for sending and receiving of packets
        _tSend = new Thread(SendingLoop) {Name = "Sending Loop"};
        _tReceive = new Thread(ReceivingLoop) {Name = "Receiving Loop"};
        _tSend.Start();
        _tReceive.Start();
    }

    // Update is called once per frame
    void Update()
    {
        //Update Z coordinate of path's rotation point to always rotate the path at the feet of the user
        Vector3 userPos = _user.transform.position;
        Vector3 pathPos = _path.transform.position;
        _path.transform.position = new Vector3(pathPos.x, pathPos.y, userPos.z);

        if (_resetting)
        {
            if (_resetCounter > ResetShowLength)
            {
                _text.text = "";
                _resetCounter = 0;
                _resetting = false;
            }
            else
            {
                _resetCounter += 1;
            }
        }
    }

    /**
     * Make sure the path is aligned with the users rotation and location.
     */
    private void AdjustPath()
    {
        Vector3 adjustedPos = _user.transform.position;
        adjustedPos.y = _path.transform.position.y;
        _path.transform.position = adjustedPos;
        Quaternion adjustedRot = _path.transform.rotation;
        adjustedRot.y = _user.transform.rotation.y;
        _path.transform.rotation = adjustedRot;
    }
    
    /**
     * Make a connection with the RW server.
     */
    private void SetupUDP()
    {
        _remoteEndPoint = new IPEndPoint(IPAddress.Parse(IP), Port);
        _client = new UdpClient();
        _remoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
        print("Connecting to " + IP + " : " + Port);
    }

    /*
     * Send border and obstacle coordinates. 
     */
    private void SendEnvData()
    {
        List<XRInputSubsystem> subsystems = new List<XRInputSubsystem>();
        SubsystemManager.GetInstances(subsystems);
        foreach (var subsystem in subsystems)
        {
            List<Vector3> coordinates = new List<Vector3>();
            subsystem.TryGetBoundaryPoints(coordinates);
            foreach (var coordinate in coordinates)
            {
                SendString("BORDER:" + coordinate.ToString());
            }
        }
        print("Environment data sent!");
    }

    /*
     * Infinite loop sending the user positional data.
     */
    private void SendingLoop()
    {
        while (true)
        {
            SendPosAndRot();
            Thread.Sleep(MessageSendFrequency);
            //InvokeRepeating(nameof(SendPosAndRot), 0, 1.0f);
        }
    }
    
    private void SendPosAndRot()
    {
        SendString("PLAYER_POS:" + _user.transform.position + "|PLAYER_ROT:" + _user.transform.rotation.eulerAngles);
    }

    /*
     * Infinite loop trying to receive correction data from the RW server.
     */
    private void ReceivingLoop()
    {
        while (true)
        {
            ReceiveNewCoordinates();
            Thread.Sleep(MessageSendFrequency);
            //InvokeRepeating(nameof(ReceiveNewCoordinates), 0, 0.1f);
        }
    }

    private void ReceiveNewCoordinates()
    {
        try
        {
            Byte[] receiveBytes = _client.Receive(ref _remoteIpEndPoint);
            string returnData = Encoding.ASCII.GetString(receiveBytes);
            UpdatePath(returnData);
        }
        catch (Exception err)
        {
            print(err.ToString());
        }
    }

    /*
     * Update the rotation of the path based on the adjustment coordinates.
     */
    private void UpdatePath(string data)
    {
        if (data.Contains("RESET"))
        {
            print("RESET!");
            _text.text = "RESET";
            _resetting = true;
        }
        
        Vector3 coordinates = ParseCoordinate(data);
        //print("Updating rot: " + coordinates);
        Vector3 userLoc = _user.transform.position;
        Vector3 pathLoc = _path.transform.position;
        
        //Current path vector
        Vector3 v1 = new Vector3(pathLoc.x, pathLoc.y, pathLoc.z + 1) - pathLoc;
        ////Rotate Vector to align with path
        v1 = Quaternion.Euler(0, _path.transform.rotation.eulerAngles.y, 0) * v1;  //TODO NOT CORRECT?, UNCOMMENT?
        //Redirection vector
        Vector3 v2 = (userLoc + coordinates) - userLoc;
        print("Vector1: " + v1);
        print("Vector2: " + v2);
        
        //Angle between both vectors used to adjust rotation
        float offset = Vector3.Angle(v1, v2);
        Vector3 pathRot = _path.transform.rotation.eulerAngles;
        print("Path ROT: " + pathRot);
        print("Offsetting ROT: " + offset);
        //Don't adjust the path if the correction is the same and the user is standing still.
        if (offset != _prevRotOffset)
        {
            _path.transform.Rotate(0,offset,0);
        }
        _prevRotOffset = offset;
    }

    /*
     * Parse a coordinate string to a Vector3.
     */
    private static Vector3 ParseCoordinate(string coordinateStr)
    {
        //Remove the brackets and comma's from the string
        string stripped = coordinateStr.Remove(coordinateStr.Length - 1, 1);
        stripped = stripped.Remove(0, 1);
        stripped = stripped.Remove(stripped.Length-1, 1);
        string[] elements = stripped.Split(',');
        
        //Construct Vector3 based on string array of coordinates
        Vector3 coordinate;
        coordinate.x = float.Parse(elements[0], CultureInfo.InvariantCulture.NumberFormat); //TODO loses lots of precision
        coordinate.y = float.Parse(elements[1], CultureInfo.InvariantCulture.NumberFormat);
        coordinate.z = float.Parse(elements[2], CultureInfo.InvariantCulture.NumberFormat);
        return coordinate;
    }

    /*
     * Send a string via a UDP packet to the server.
     */
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
