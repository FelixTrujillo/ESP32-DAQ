using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class sceneLoader : MonoBehaviour
{
    public List<GameObject> gameObjectCanvas; // Lista de CanvasGroup

    // Este método mostrará el CanvasGroup correspondiente al índice

    void Start()
    {
        MostrarCanvas(0);
    }

    public void MostrarCanvas(int index)
    {
        // Recorremos todos los CanvasGroup de la lista
        for (int i = 0; i < gameObjectCanvas.Count; i++)
        {
            if (i == index)
            {
                // Activar el canvas del objeto en el indice i
                gameObjectCanvas[i].SetActive(true);
            }
            else
            {
                // Desactivar el canvas del objeto en el indice i
                gameObjectCanvas[i].SetActive(false);

            }
        }
    }
}
