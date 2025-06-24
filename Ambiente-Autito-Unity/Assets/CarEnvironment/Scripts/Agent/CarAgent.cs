using UnityEngine;
using Unity.MLAgents;
using Unity.MLAgents.Sensors;
using Unity.MLAgents.Actuators;
using System.Collections.Generic;
using System.Linq;

public class CarAgent : Agent
{

    private int dir = 1;
    private int currPos = 0;
    public List<Vector2> TargetPos;
    public Transform Target;
    private Rigidbody rBody;
    private void Start()
    {
        rBody = GetComponent<Rigidbody>();
        //Debug.Log(this.transform.position.x);


        if (TargetPos.Count < 1)
        {
            TargetPos.Add(Vector2.zero);
            TargetPos.Add(new Vector2(1, 0));
            TargetPos.Add(new Vector2(-1, 0));
        }
    }


    public override void OnEpisodeBegin()
    {
        Debug.Log("OnEpisodeBegin");

        // Move the target to a new spot
        Target.localPosition = new Vector3(TargetPos[currPos].x, 0.5f, TargetPos[currPos].y);
        currPos += dir;
        if (currPos == TargetPos.Count-1 || currPos == 0) dir *= -1;
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
        float distanceToTarget = Vector3.Distance(this.transform.localPosition, Target.localPosition);

        // Reached target
        if (distanceToTarget < 1.42f)
        {
            Debug.Log("CERCA");
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
