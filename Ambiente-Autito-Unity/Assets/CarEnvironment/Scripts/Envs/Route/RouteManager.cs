using UnityEngine;

public class RouteManager : MonoBehaviour
{
    [SerializeField] private SmoothLineController lineController;
    [SerializeField] private bool Move = false;
    [SerializeField] private float lastMoveTime = 0f;

    void Update()
    {
        if (Move)
        {
            Move = false;
            ContinueRoad();
            lastMoveTime = Time.time;
        }

        if (Time.time - lastMoveTime >= 1f)
        {
            Move = true;
        }
    }

    public void ContinueRoad() => lineController.MoveLastToNext();
}
