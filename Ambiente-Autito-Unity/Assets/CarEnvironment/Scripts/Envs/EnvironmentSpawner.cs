using UnityEngine;

public class EnvironmentSpawner : MonoBehaviour
{
    public GameObject envPrefab;
    public int numEnvironmentsSqrt = 4; // numEnvironmentsSqrt**2 sera la cantidad de envs
    public float spacing = 50f; // separación entre entornos

    void Start()
    {
        for (int i = 0; i < numEnvironmentsSqrt; i++) {
            Vector3 position = new Vector3(i*spacing, 0, 0);
            
            for (int j = 0; j < numEnvironmentsSqrt; j++)
            {
                // Instancia el entorno
                position.z = j * spacing;

                GameObject envInstance = Instantiate(envPrefab, position, Quaternion.identity);
                envInstance.name = "Environment_" + (i*numEnvironmentsSqrt + j + 1);

                // Nombre de la capa correspondiente
                string layerName = "Env" + (i*numEnvironmentsSqrt + j + 1);
                int layer = LayerMask.NameToLayer(layerName);
                if (layer == -1)
                {
                    Debug.LogError($"La capa '{layerName}' no existe. ¡Creala en Edit > Project Settings > Tags and Layers!");
                    continue;
                }

                // Asigna la capa a todo el entorno
                SetLayerRecursively(envInstance, layer);

                // Encuentra la cámara (asumiendo que hay solo una en el prefab)
                Camera agentCamera = envInstance.GetComponentInChildren<Camera>();
                if (agentCamera != null)
                {
                    agentCamera.cullingMask = 1 << layer;
                }
                else
                {
                    Debug.LogWarning($"No se encontró una cámara en el entorno {envInstance.name}");
                }
            }
        }
    }

    // Asigna recursivamente la layer a todos los objetos hijos
    void SetLayerRecursively(GameObject obj, int layer)
    {
        obj.layer = layer;
        foreach (Transform child in obj.transform)
        {
            SetLayerRecursively(child.gameObject, layer);
        }
    }
}
