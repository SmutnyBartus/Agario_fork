using UnityEngine;

public class PlayerEntity : MonoBehaviour
{

    public Vector3 targetPosition;
    private float targetRadius;
    private string playerName;
    private float lerpSpeed = 10f;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float currentScale = transform.localScale.x;
        float newScale = Mathf.Lerp(currentScale, targetRadius, lerpSpeed*Time.deltaTime);
        transform.localScale = new Vector3(newScale, newScale, 1);
        transform.position = Vector3.Lerp(transform.position, targetPosition, lerpSpeed * Time.deltaTime);
    }

    public void SetTarget(int x, int y, int radius)
    {
        targetPosition = new Vector3(x,y,0);
        targetRadius= radius;
    }



}
