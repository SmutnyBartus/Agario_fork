using UnityEngine;
public class PlayerInput : MonoBehaviour
{
    NetworkManager networkManager;
    private Vector2 lastSentPosition;
    private float sendThreshold=0.1f;
    private Vector3 mousePos; 
    private float angle;
    private int angleDeg;
    private Vector2 direction;

    private Vector2 playerPos;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        networkManager = GetComponent<NetworkManager>();
    }

    // Update is called once per frame
    void Update()
    {

        playerPos = GameManager.Instance.localPlayer.transform.position;
        mousePos = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        direction = new Vector2(mousePos.x-playerPos.x,mousePos.y-playerPos.y);
        angle=Mathf.Atan2(direction.y, direction.x)* Mathf.Rad2Deg;
        angleDeg = Mathf.RoundToInt(angle);
        if(Vector2.Distance(mousePos, lastSentPosition)> sendThreshold)
        {
            byte[] packet = PacketBuilder.BuildPlayerInput(angleDeg);
            networkManager.SendUDP(packet);
            lastSentPosition=mousePos;
        }
        
        
    }
}
