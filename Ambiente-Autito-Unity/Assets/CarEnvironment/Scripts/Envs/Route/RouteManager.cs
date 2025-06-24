using UnityEngine;

public class RouteManager : MonoBehaviour
{
    [SerializeField] private SmoothLineController lineController;
    public void ContinueRoad() => lineController.MoveLastToNext();
    public Transform GetTarget()
    {
        ContinueRoad();
        return lineController.GetTarget();
    } 
    public Transform GetInitialTarget()
    {
        return lineController.GetTarget();
    } 
}
