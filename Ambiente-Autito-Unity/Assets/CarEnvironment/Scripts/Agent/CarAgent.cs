using UnityEngine;
using Unity.MLAgents;
using Unity.MLAgents.Sensors;
using Unity.MLAgents.Actuators;
using System.Collections.Generic;
using System.Linq;

public class CarAgent : Agent
{
    public Transform Target;
    public RouteManager routeManager;
    private Rigidbody rBody;
    protected override void Awake()
    {
        rBody = GetComponent<Rigidbody>();
        Target = routeManager.GetInitialTarget();
        base.Awake();
    }


    public override void OnEpisodeBegin()
    {
        Debug.Log("OnEpisodeBegin");
    }


    [SerializeField] private bool useConstantMovement = true;      
    [SerializeField] private float constantMovementValue = 1.0f;      
    [SerializeField] private float steeringSpeed = 100f;      // Qué tan rápido gira (grados por segundo por unidad de input)
    [SerializeField] private float moveSpeed = 5f;            // Qué tan rápido acelera

    public void MoveCar(float steeringInput, float throttleInput)
    {
        // Rotar el cubo (como girar el volante)
        float turnAmount = steeringInput * steeringSpeed * Time.deltaTime;
        transform.Rotate(0f, turnAmount, 0f);

        // Mover el cubo hacia adelante (en su propia orientación)
        Vector3 move = transform.forward * throttleInput * moveSpeed * Time.deltaTime;
        transform.localPosition += move;
    }

    public override void OnActionReceived(ActionBuffers actionBuffers)
    {
        MoveCar(actionBuffers.ContinuousActions[0], !useConstantMovement ? actionBuffers.ContinuousActions[1] : constantMovementValue);

        // Rewards
        Vector2 carPos2D = new Vector2 (this.transform.position.x, this.transform.position.z);
        Vector2 targetPos2D = new Vector2 (Target.position.x, Target.position.z); 
        float distanceToTarget = Vector2.Distance(carPos2D, targetPos2D);

        // Reached target
        if (distanceToTarget < 1.42f)
        {
            Debug.Log("CERCA");
            Target = routeManager.GetTarget();
            SetReward(1.0f);
            EndEpisode();
        }

        // Fell off platform
        else if (this.transform.localPosition.y < 0)
        {
            Debug.Log("ABAJO");
            EndEpisode();
        }
    }
    
    public override void Heuristic(in ActionBuffers actionsOut)
    {
        var continuousActionsOut = actionsOut.ContinuousActions;
        continuousActionsOut[0] = Input.GetAxis("Horizontal");
        continuousActionsOut[1] = Input.GetAxis("Vertical");
    }

}
