using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using ChartAndGraph;

public class currentGraph : MonoBehaviour
{
    public GraphChart chart;
    private double t = 0;
    private double current = 0;
    private double Timer = 0.6;

    // Start is called before the first frame update
    void Start()
    {
        chart.DataSource.StartBatch();
        chart.DataSource.ClearCategory("Current");
        chart.DataSource.AddPointToCategory("Current", 0, 0);
        chart.DataSource.EndBatch();
    }

    // Update is called once per frame
    void Update()
    {
        Timer -= Time.deltaTime;
        if (Timer < 0f)
        {
            Timer = 0.6;
            Debug.Log(current);
            chart.DataSource.AddPointToCategoryRealtime("Current", t, current, 1f);
            t += 0.6;
        }
    }

    public void CurrentMessage(float value)
    {
        current = value;
    }
}
