using UnityEngine;
using System;
using System.Collections.Generic;

public class PacketReader
{
    public struct Packet
    {
        public byte Type;
        public ushort Length;
        public byte[] Data;
    }

public static Packet? TryReadNext(List<byte> raw)
    {
        if (raw==null || raw.Count< 3)
            return null;

        byte type= raw[0];
        ushort length=BitConverter.ToUInt16(raw.ToArray(), 1);
        if(raw.Count<length+3)
        return null;
            
        byte [] data = new byte[length];
        Array.Copy(raw.ToArray(),3,data,0,length);
        raw.RemoveRange(0,3+length);

        return new Packet {Type=type, Length=length, Data=data};    
    }
 public static Packet? Parse(byte[] raw)
    {

        if (raw==null || raw.Length< 3)
            return null;
        byte type= raw[0];
        ushort length=BitConverter.ToUInt16(raw, 1);
        

        byte[] data= new byte[length];
        Array.Copy(raw,3,data,0,length);




        return new Packet {Type= type, Length=length, Data=data};
    }


}
