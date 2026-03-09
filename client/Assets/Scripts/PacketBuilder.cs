using UnityEngine;
using System;
public class PacketBuilder
{
    


    public static byte[] BuildPlayerInput(int angleDeg)
    {
        byte[] inputPacket = new byte[7];
        inputPacket[0]=3;


        byte[] lengthBytes = BitConverter.GetBytes((ushort)4);
        inputPacket[1]= lengthBytes[0];
        inputPacket[2]= lengthBytes[1];


        byte[] angleBytes = BitConverter.GetBytes(angleDeg);
        inputPacket[3] = angleBytes[0];
        inputPacket[4] = angleBytes[1];
        inputPacket[5] = angleBytes[2];
        inputPacket[6] = angleBytes[3];

        return inputPacket;
    }

    public static byte[] BuildInitialConnection(string name)
    {
        byte[] inputPacket = new byte[20];
        inputPacket[0]=0;


        byte[] lengthBytes = BitConverter.GetBytes((ushort)16);
        inputPacket[1]= lengthBytes[0];
        inputPacket[2]= lengthBytes[1];

        name = name.Substring(0, Math.Min(name.Length, 15));
        byte[] nameBytes = System.Text.Encoding.ASCII.GetBytes(name);
        for(int i=0;i<nameBytes.Length;i++)
        {
            inputPacket[i+3]=nameBytes[i];
        }

        return inputPacket;
    }

    public static byte[] BuildGameStarted()
    {
        byte[] inputPacket = new byte[3];
        inputPacket[0]=1;


        byte[] lengthBytes = BitConverter.GetBytes((ushort)0);
        inputPacket[1]= lengthBytes[0];
        inputPacket[2]= lengthBytes[1];

        return inputPacket;
    }
}
