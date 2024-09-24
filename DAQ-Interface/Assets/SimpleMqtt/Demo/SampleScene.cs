using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using UnityEngine;
using UnityEngine.UI;

namespace Rocworks.Mqtt
{
    public class SampleScene : MonoBehaviour
    {
        public MqttClient MqttClient;
        public GameObject ObjectToHandle;
        public Toggle IsConnected;
        private InputField HostInput;
        private InputField PortInput;
        private InputField UsernameInput;
        private InputField PasswordInput;

        private float _voltageMessage = 0;
        private float _currentMessage = 0;
        private float _speedMessage = 0;
        private float _accelerationMessage = 0;
        private float _temperatureMessage = 0;
        private float _altitudMessage = 0;


        public voltageGraph voltage;
        public voltageGraph voltage1;
        public voltageGraph voltage2;
        public digitalTwinGraph voltageDT;

        public accelerationGraph acceleration;
        public accelerationGraph acceleration1;
        public accelerationGraph acceleration2;
        public digitalTwinGraph accelerationDT;

        public mapGraph altitud;
        public mapGraph altitud1;
        public mapGraph altitud2;
        public digitalTwinGraph altitudDT;

        public currentGraph current;
        public currentGraph current1;
        public digitalTwinGraph currentDT;

        public speedGraph speed;
        public speedGraph speed1;
        public digitalTwinGraph speedDT;

        public tempGraph temperature;
        public tempGraph temperature1;

        



        // Start is called before the first frame update
        void Start()
        {
            HostInput.onEndEdit.AddListener(OnInputTextHost);
            PortInput.onEndEdit.AddListener(OnInputTextPort);
            UsernameInput.onEndEdit.AddListener(OnInputTextUsername);
            PasswordInput.onEndEdit.AddListener(OnInputTextPassword);
        }

        // Update is called once per frame
        void Update()
        {

            voltage.VoltageMessage(_voltageMessage);
            voltage1.VoltageMessage(_voltageMessage);
            voltage2.VoltageMessage(_voltageMessage);
            voltageDT.VoltageMessage(_voltageMessage);

            acceleration.AccelerationMessage(_accelerationMessage);
            acceleration1.AccelerationMessage(_accelerationMessage);
            acceleration2.AccelerationMessage(_accelerationMessage);
            accelerationDT.AccelerationMessage(_accelerationMessage);

            current.CurrentMessage(_currentMessage);
            current1.CurrentMessage(_currentMessage);
            currentDT.CurrentMessage(_currentMessage);

            speed.SpeedMessage(_speedMessage);
            speed1.SpeedMessage(_speedMessage);
            speedDT.SpeedMessage(_speedMessage);

            temperature.TemperatureMessage(_temperatureMessage);
            temperature1.TemperatureMessage(_temperatureMessage);

            altitud.AltitudeMessage(_altitudMessage);
            altitud1.AltitudeMessage(_altitudMessage);
            altitud2.AltitudeMessage(_altitudMessage);
            altitudDT.AltitudeMessage(_altitudMessage);
        }


        // Configurations
        public void OnInputTextHost(string value)
        {
            MqttClient.Host = value;

        }

        public void OnInputTextPort(string value)
        {
            if (int.TryParse(value, out int port))
            {
                MqttClient.Port = port;
            }
            else
            {
                Debug.LogError("Invalid port number");
            }
        }

        public void OnInputTextUsername(string value)
        {
            MqttClient.Username = value;
        }

        public void OnInputTextPassword(string value)
        {
            MqttClient.Password = value;
        }

        public void OnToggleButtonWebSocket(bool value)
        {
            MqttClient.Websocket = value;
        }

        public void OnInputTextWebsocketPath(string value)
        {
            MqttClient.WebsocketPath = value;
        }

        public void OnToggleButtonUseTLS(bool value)
        {
            MqttClient.UseTLS = value;
        }

        public void OnToggleButtonAllowUntrusted(bool value)
        {
            MqttClient.AllowUntrusted = value;
        }

        public void OnToggleButtonCleanSession(bool value)
        {
            MqttClient.CleanSession = value;
        }

        // Messages

        public void voltageLecture(string value)
        {
            Debug.Log(value);
            MqttClient.Connection.Publish("voltage", value);
        }
        public void currentLecture(string value)
        {
            Debug.Log(value);
            MqttClient.Connection.Publish("current", value);
        }
        public void speedLecture(string value)
        {
            Debug.Log(value);
            MqttClient.Connection.Publish("speed", value);
        }
        public void accelerationLecture(string value)
        {
            Debug.Log(value);
            MqttClient.Connection.Publish("acceleration", value);
        }
        public void temperatureLecture(string value)
        {
            Debug.Log(value);
            MqttClient.Connection.Publish("temperature", value);
        }
        public void altitudLecture(string value)
        {
            Debug.Log(value);
            MqttClient.Connection.Publish("altitud", value);
        }



        public void SetDebug(bool value)
        {
            MqttClient.DebugMessages = value;
        }

        public void OnMessageArrived(MqttMessage m)
        {
            switch (m.GetTopic())
            {
                case "voltage":
                    _voltageMessage = float.Parse(m.GetString(), CultureInfo.InvariantCulture);
                    break;
                case "current":
                    _currentMessage = float.Parse(m.GetString(), CultureInfo.InvariantCulture);
                    break;
                case "speed":
                    _speedMessage = float.Parse(m.GetString(), CultureInfo.InvariantCulture);
                    break;
                case "acceleration":
                    _accelerationMessage = float.Parse(m.GetString(), CultureInfo.InvariantCulture);
                    break;
                case "temperature":
                    _temperatureMessage = float.Parse(m.GetString(), CultureInfo.InvariantCulture);
                    break;
                case "altitud":
                    _altitudMessage = float.Parse(m.GetString(), CultureInfo.InvariantCulture);
                    break;

            }
        }
    }
}



