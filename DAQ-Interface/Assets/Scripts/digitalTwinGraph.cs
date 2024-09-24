using System;
using System.Collections;
using System.Collections.Generic;
using MathNet.Numerics.LinearAlgebra;
using UnityEngine;
using ChartAndGraph;

public class digitalTwinGraph : MonoBehaviour
{
    public GraphChart chart1;
    public GraphChart chart2;

    private double t = 0;

    private double map = 0;
    private double map1 = 0;
    private double current = 0;
    private double current1 = 0;
    private double voltage = 0;
    private double voltage1 = 0;
    private double speed = 0;
    private double speed1 = 0;
    private double acceleration = 0;
    private double acceleration1 = 0;

    private double Timer = 0.6;
    public static MatrixBuilder<double> M = Matrix<double>.Build;

    // Start is called before the first frame update
    void Start()
    {
        chart1.DataSource.StartBatch();

        chart1.DataSource.ClearCategory("Current");
        chart1.DataSource.AddPointToCategory("Current", 0, 0);

        chart1.DataSource.EndBatch();

        chart2.DataSource.StartBatch();

        chart2.DataSource.ClearCategory("Speed");
        chart2.DataSource.AddPointToCategory("Speed", 0, 0);

        chart2.DataSource.EndBatch();

    }

    // Update is called once per frame
    void Update()
    {
        var Y = M.DenseOfArray(new double[,]
        {
            {speed, current, speed1, current1}
        });

        var U = M.DenseOfArray(new double[,]
        {
            {acceleration, voltage, map, acceleration1, voltage1, map1 }
        });

        var Yest = ConsMatriz(Y, U);

        Timer -= Time.deltaTime;
        if (Timer < 0f)
        {
            Timer = 0.6;

            chart1.DataSource.AddPointToCategoryRealtime("Current", t, Yest[0,1], 1f);
            chart2.DataSource.AddPointToCategoryRealtime("Speed", t, Yest[0,0], 1f);

            t += 0.6;
        };

        current1 = current;
        speed1 = speed;
        voltage1 = voltage;
        acceleration1 = acceleration;
        map1 = map;
    }

    // ============ INPUTS =============

    public void VoltageMessage(float value)
    {
        voltage = value;
    }

    public void AccelerationMessage(float value)
    {
        acceleration = value;
    }

    public void AltitudeMessage(float value)
    {
        map = value;
    }

    // ============ OUTPUTS ============

    public void SpeedMessage(float value)
    {
        speed = value;
    }

    public void CurrentMessage(float value)
    {
        current = value;
    }

    // =========== CONSTRUCCION DE MATRICES ============

    private Matrix<double> ConsMatriz(Matrix<double> Y, Matrix<double> U)
    {
        var theta = M.DenseOfArray(new double[,]
            {
            {-0.95,  0.00},
            {-1.84, -0.56},
            { 0.10, -0.00},
            { 0.49, -0.23},
            {14.28,  0.42},
            {-2.94, -0.07},
            {-0.50,  0.08},
            { 5.42, -0.70},
            { 3.93,  0.10},
            { 0.48, -0.08}
            });

        var H = (-Y).Append(U);

        var Yest = H * theta;

        return Yest;
    }
}
