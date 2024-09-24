using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class MQTTManager : MonoBehaviour
{
    private static MQTTManager instance;

    private CanvasGroup canvasGroup;
    public string activatedScene;

    void Awake()
    {
        // Asegúrate de que solo haya una instancia del objeto persistente
        if (instance == null)
        {
            instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            Destroy(gameObject);
        }

        // Obtener el CanvasGroup al inicio
        canvasGroup = GetComponentInChildren<CanvasGroup>();
        if (canvasGroup == null)
        {
            Debug.LogError("No se encontró CanvasGroup en los hijos del objeto.");
        }
    }

    void Update()
    {
        // Obtén la escena actual.
        Scene escenaActual = SceneManager.GetActiveScene();

        // Compara el nombre de la escena actual con el nombre de la escena en la que el objeto debe estar activo.
        if (escenaActual.name == activatedScene)
        {
            MostrarCanvas();
        }
        else
        {
            OcultarCanvas();
        }
    }

    public void MostrarCanvas()
    {
        if (canvasGroup != null)
        {
            canvasGroup.alpha = 1;
            canvasGroup.interactable = true;
            canvasGroup.blocksRaycasts = true;
        }
        else
        {
            Debug.LogWarning("El CanvasGroup no está asignado.");
        }
    }

    public void OcultarCanvas()
    {
        if (canvasGroup != null)
        {
            canvasGroup.alpha = 0;
            canvasGroup.interactable = false;
            canvasGroup.blocksRaycasts = false;
        }
        else
        {
            Debug.LogWarning("El CanvasGroup no está asignado.");
        }
    }
}
