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
    [SerializeField] private List<Transform> controlPoints = new List<Transform>();

    [Header("Configuración de Línea")]
    [Range(0.1f, 5f)][SerializeField] private float lineWidth = 0.5f;
    [Range(2, 100)][SerializeField] private int resolution = 30;

    private LineRenderer lineRenderer;

    void Start()
    {
        InitializeControlPoints();
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

    // ---------------------------------------------------------------------------------------
    private void InitializeControlPoints()
    {
        int i=0;
        Vector3 last = Vector3.zero;
        Vector3 secondLast = Vector3.zero;
        foreach (var point in controlPoints)
        {
            if (i == 0)
            {
                secondLast = point.position;
            }
            else if (i == 1)
            {
                last = point.position;
            }
            else
            {
                Vector3 newPos = getNextPos(last, secondLast);
                setPos2D(point, newPos);
                secondLast = last;
                last = point.position;
            }
            i++;
        }
    }


    private Vector3 getNextPos(Vector3 last, Vector3 secondLast)
    {
        // Dirección entre los dos últimos puntos
        Vector3 direction = (last - secondLast).normalized;

        // Random de distancia y ángulo
        float distance = Random.Range(5f, 20f);
        float angle = Random.Range(-90f, 90f);

        // Rotar la dirección en el plano XZ
        Quaternion rotation = Quaternion.Euler(0f, angle, 0f);
        Vector3 rotatedDirection = rotation * direction;

        // Calcular nueva posición
        Vector3 newPosition = last + rotatedDirection * distance;
        return newPosition;
    }
    private Transform setPos2D(Transform t, Vector3 newPos) {
        t.position = new Vector3(newPos.x, t.position.y, newPos.y);
        return t;
    }
    public void MoveLastToNext()
    {
        if (controlPoints.Count < 2) return;

        Vector3 last = controlPoints[controlPoints.Count - 1].position;
        Vector3 secondLast = controlPoints[controlPoints.Count - 2].position;

        Vector3 newPosition = getNextPos(last, secondLast);

        // Pasar solo X y Z como Vector2
        MoveLastPointToFirstAt(new Vector2(newPosition.x, newPosition.z));
    }

    private void MoveLastPointToFirstAt(Vector2 newPos)
    {
        Transform point = RemovePoint(0);
        point = setPos2D(point, newPos);
        AddPoint(point);
    }
    private void AddPoint(Transform newPoint)
    {
        if (newPoint != null)
        {
            controlPoints.Add(newPoint);
            UpdateLine();
        }
    }
    private Transform RemovePoint(int pointIndex)
    {
        if (controlPoints.Count <= pointIndex) return null;
        Transform point = controlPoints[pointIndex];

        if (point != null)
        {
            controlPoints.Remove(point);
            UpdateLine();
        }

        return point;
    }

}