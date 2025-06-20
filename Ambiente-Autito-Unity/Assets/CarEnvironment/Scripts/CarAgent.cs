using UnityEngine;
using Unity.MLAgents;
using Unity.MLAgents.Sensors;
using Unity.MLAgents.Actuators;

public class CarAgent : Agent
{

    public Transform Target;
    private Rigidbody rBody;
    private void Start()
    {
        rBody = GetComponent<Rigidbody>();

        Debug.Log(this.transform.position.x);
    }

    public override void OnEpisodeBegin()
    {
        Debug.Log("OnEpisodeBegin");

        // If the Agent fell, zero its momentum
        /*if (this.transform.localPosition.y < 0)
        {
            Debug.Log("y < 0");

            this.rBody.angularVelocity = Vector3.zero;
            this.rBody.linearVelocity = Vector3.zero;
            this.transform.localPosition = new Vector3(0, 0.5f, 0);
        }*/

        // Move the target to a new spot
        Target.localPosition = new Vector3(Random.value * 8 - 4, 0.5f, Random.value * 8 - 4);

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
            SetReward(1.0f);
            EndEpisode();
        }

        // Fell off platform
        else if (this.transform.localPosition.y < 0)
        {
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
