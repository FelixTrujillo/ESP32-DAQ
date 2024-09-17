using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MQTTManager : MonoBehaviour
{
    private static MQTTManager instance;

    private void Awake()
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
    }
    
}
