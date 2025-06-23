using System.Collections.Generic;
using UnityEngine;

[ExecuteAlways]
public class SmoothLineController : MonoBehaviour
{
    #if UNITY_EDITOR
        void OnEnable()
        {
            UnityEditor.EditorApplication.update += EditorUpdate;
        }

        void OnDisable()
        {
            UnityEditor.EditorApplication.update -= EditorUpdate;
        }

        void EditorUpdate()
        {
            if (!Application.isPlaying && CheckControlPointsChanged())
            {
                UpdateLine();
            }
        }
    #endif

    [Header("Puntos de Control")]
    public List<Transform> controlPoints = new List<Transform>();

    [Header("Configuración de Línea")]
    [Range(0.1f, 5f)] public float lineWidth = 0.5f;
    [Range(2, 100)] public int resolution = 30;

    private LineRenderer lineRenderer;

    void Start()
    {
        InitializeLineRenderer();
    }

    void OnValidate()
    {
        if (controlPoints.Count < 2)
        {
            Debug.LogWarning("Se necesitan al menos 2 puntos de control");
            return;
        }

        InitializeLineRenderer();
        UpdateLine();
    }

    void Update()
    {
        if (transform.hasChanged || CheckControlPointsChanged())
        {
            UpdateLine();
        }
    }

    private void InitializeLineRenderer()
    {
        if (lineRenderer == null)
        {
            lineRenderer = GetComponent<LineRenderer>();
            if (lineRenderer == null)
            {
                lineRenderer = gameObject.AddComponent<LineRenderer>();
            }
        }

        lineRenderer.useWorldSpace = true;
        lineRenderer.widthMultiplier = lineWidth;
        lineRenderer.material = new Material(Shader.Find("Sprites/Default"));
        lineRenderer.startColor = Color.white;
        lineRenderer.endColor = Color.white;
        
        // Hace que el quad de la línea se alinee con el eje Z del transform,
        // no hacia la cámara
        transform.rotation = Quaternion.Euler(-90f, 0f, 0f);
        lineRenderer.alignment = LineAlignment.TransformZ;
    }

    private bool CheckControlPointsChanged()
    {
        foreach (var point in controlPoints)
        {
            if (point != null && point.hasChanged) return true;
        }
        return false;
    }

    private void UpdateLine()
    {
        if (controlPoints.Count < 2) return;

        List<Vector3> linePoints = new List<Vector3>();

        // Generar puntos para cada segmento de la curva
        for (int i = 0; i < controlPoints.Count - 1; i++)
        {
            Vector3 p0 = i > 0 ? controlPoints[i - 1].position : controlPoints[i].position;
            Vector3 p1 = controlPoints[i].position;
            Vector3 p2 = controlPoints[i + 1].position;
            Vector3 p3 = (i + 2 < controlPoints.Count) ? controlPoints[i + 2].position : p2 + (p2 - p1);

            for (int j = 0; j < resolution; j++)
            {
                float t = j / (float)(resolution - 1);
                Vector3 point = CalculateCatmullRomPoint(t, p0, p1, p2, p3);
                linePoints.Add(point);
            }
        }

        // Configurar LineRenderer
        lineRenderer.positionCount = linePoints.Count;
        lineRenderer.SetPositions(linePoints.ToArray());
    }

    private Vector3 CalculateCatmullRomPoint(float t, Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3)
    {
        // Fórmula de Catmull-Rom
        float t2 = t * t;
        float t3 = t2 * t;

        return 0.5f * (
            (2f * p1) +
            (-p0 + p2) * t +
            (2f * p0 - 5f * p1 + 4f * p2 - p3) * t2 +
            (-p0 + 3f * p1 - 3f * p2 + p3) * t3
        );
    }

    // Método para agregar puntos desde el inspector
    public void AddControlPoint(GameObject newPoint)
    {
        if (newPoint != null)
        {
            controlPoints.Add(newPoint.transform);
            UpdateLine();
        }
    }
}