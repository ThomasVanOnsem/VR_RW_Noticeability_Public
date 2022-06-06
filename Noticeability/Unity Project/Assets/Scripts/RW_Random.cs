using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.XR;

public class RW_Random : MonoBehaviour
{
    
    private const string IP = "192.168.1.146";
    private const int Port = 5555;

    private const int MessageSendFrequency = 15;

    private IPEndPoint _remoteEndPoint;
    private UdpClient _client;
    
    private GameObject _user;
    private GameObject _terrain;
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
        _terrain = GameObject.Find("Field");
        
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
        if (_resetting)
        {
            return;
        }
        SendString("PLAYER_POS:" + _user.transform.position.ToString("F9") + 
                   "|PLAYER_ROT:" + _user.transform.rotation.eulerAngles.ToString("F9"));
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
        if (_resetting)
        {
            return;
        }
        if (data.Contains("RESET"))
        {
            print("RESET!");
            _text.text = "RESET";
            _resetting = true;
            //TODO remove RESET from data
            data = data.Split(':')[1];
        }
        
        float received = float.Parse(data, CultureInfo.InvariantCulture.NumberFormat);
        if (received == _prevRotOffset)
        {
            return;
        }
        
        
        _terrain.transform.RotateAround(_user.transform.position, Vector3.up, 0.5f);
        _prevRotOffset = received;
        print("Rotated: " + received.ToString("F9"));
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
