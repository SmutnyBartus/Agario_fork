using UnityEngine;
using System.Collections.Generic;
public class GameManager : MonoBehaviour
{
    public static GameManager Instance;
    public GameObject playerPrefab;
    public GameObject fruitPrefab;
    Dictionary<string, GameObject> players;

    List<GameObject> fruits;
  

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        players = new Dictionary<string, GameObject>();
        fruits = new List<GameObject>();

    }

    // Update is called once per frame
    void Update()
    {
        
    }

    

    void Awake()
    {
         Instance = this;
    }


    public UpdateGameState(List<PlayerData> newPlayers, List<FruitData> newFruits)
    {
        foreach(GameObject f in fruits)
            Destroy(f);
        fruits.Clear();
        for(int i=0;i<newFruits.Count;i++)
            SpawnFruit(newFruits[i]);


        foreach(GameObject p in players.Values )
            Destroy(p);
        players.Clear();
        for(int i=0;i<newPlayers.Count;i++)
            SpawnPlayer(newPlayers[i]);

    }

    void SpawnPlayer(PlayerData player)
    {
        GameObject obj = Instantiate(playerPrefab, new Vector3(player.x, player.y, 0), Quaternion.identity);
        obj.transform.localScale = new Vector3(player.radius, player.radius, 1);
        players.Add(player.name,obj);
    }
    void SpawnFruit(FruitData fruit)
    {
        GameObject obj = Instantiate(fruitPrefab, new Vector3(fruit.x, fruit.y, 0), Quaternion.identity);
        fruits.Add(obj);
    }

}
