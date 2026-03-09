using System;
using System.Collections.Generic;
using UnityEngine;
public class PacketHandler
{
      public static void Handle(PacketReader.Packet packet)
    {
        switch (packet.Type)
        {
            case 2:
                Handle_Server_Game_Data_Broadcast(packet.Data);
                break;
            default:
                break;
        }
        
    }


    private void Handle_Server_Game_Data_Broadcast(byte[] Data)
    {
        int offset =0;
        int player_Number=BitConverter.ToInt32(Data,offset);
        offset+=4;
    
        List<PlayerData> players = new List<PlayerData>();
        List<FruitData> fruits = new List<FruitData>();
        PlayerData player;
        for(int i=0;i<player_Number;i++)
        {
            player.x=BitConverter.ToInt32(Data,offset);
            offset+=4;
            player.y=BitConverter.ToInt32(Data,offset);
            offset+=4;
            player.radius=BitConverter.ToInt32(Data,offset);
            offset+=4;
            player.name=System.Text.Encoding.ASCII.GetString(Data, offset, 16);
            offset += 16;

            players.Add(player);
        }

            int fruit_Number=BitConverter.ToInt32(Data,offset);
            offset+=4;
            FruitData fruit;

        for(int i=0;i<fruit_Number;i++)
        {
            fruit.x=BitConverter.ToInt32(Data,offset);
            offset+=4;
            fruit.y=BitConverter.ToInt32(Data,offset);
            offset+=4;
            fruit.radius=BitConverter.ToInt32(Data,offset);
            offset+=4;


            fruits.Add(fruit);
        }
    
         GameManager.Instance.UpdateGameState(players, fruits);
    }






}
