using UnityEngine;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Generic;
using System;
public class NetworkManager : MonoBehaviour
{
    private TcpClient tcp;
    private UdpClient udp;
    private NetworkStream stream;
    private Thread tcpThread;
    private Thread udpThread;
    private Queue<PacketReader.Packet> packetQueue;
    private object queueLock = new object();
    private bool isRunning;

    public string serverIP = "127.0.0.1";
    public int tcpPort = 8080;
    public int udpPort = 9090;
    void Start()
    {
        try
        {
            
        
        packetQueue = new Queue<PacketReader.Packet>();
         tcp = new TcpClient();
        tcp.Connect(serverIP, tcpPort);  
        stream = tcp.GetStream();

        udp = new UdpClient();
        udp.Connect(serverIP, udpPort);  


        isRunning = true;
        tcpThread = new Thread(TCPReceiveLoop);
        tcpThread.IsBackground = true;
        tcpThread.Start();

        udpThread = new Thread(UDPReceiveLoop);
        udpThread.IsBackground = true;
        udpThread.Start();


        byte[] data = PacketBuilder.BuildInitialConnection("PlayerName");
        SendTCP(data);
        Debug.Log("Connected successfully");
        }
        catch(Exception e)
        {
            Debug.LogError("Connection failed: " + e.Message);
        }

        
    }

    private void TCPReceiveLoop()
    {
        List<byte> tcpBuffer = new List<byte>();
        byte[] temp = new byte[1024];
        while (isRunning)
        {
            int bytesRead = stream.Read(temp,0,temp.Length);
            for (int i = 0; i < bytesRead; i++)
            {
                tcpBuffer.Add(temp[i]);
            }


            PacketReader.Packet? packet;
            while((packet = PacketReader.TryReadNext(tcpBuffer))!=null)
            {
                lock(queueLock)
                    packetQueue.Enqueue(packet.Value);
            }
                
        }
        
    }

    private void UDPReceiveLoop()
    {
        while (isRunning)
        {
            IPEndPoint remote = new IPEndPoint(IPAddress.Any, 0);
            byte[] data = udp.Receive(ref remote);

            PacketReader.Packet? packet =PacketReader.Parse(data);
            if(packet != null)
            {
                lock(queueLock)
                    packetQueue.Enqueue(packet.Value);
            }
        }
        
    }

    public void SendTCP(byte[] data)
    {
        stream.Write(data, 0, data.Length);
    }

    public void SendUDP(byte[] data)
    {
        
        udp.Send(data, data.Length);
    }

    // Update is called once per frame
    void Update()
    {
        lock(queueLock)
        {
            while(packetQueue.Count>0)
            {
                PacketReader.Packet p = packetQueue.Dequeue();
                PacketHandler.Handle(p);
            }
        }
    }

     void OnDestroy()
    {
        isRunning = false; 
        tcp.Close();
        udp.Close();
    }

    public void OnStartButtonPressed()
    {
        SendTCP(PacketBuilder.BuildGameStarted());
    }

}
