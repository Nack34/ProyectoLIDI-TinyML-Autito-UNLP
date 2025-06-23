using UnityEngine;

public class RouteManager : MonoBehaviour
{
    [SerializeField] private SmoothLineController lineController;
    [SerializeField] private bool Move = false;
    
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (Move)
        {
            Move = false;
            lineController.MoveLastToNext();
        }
    }
}
