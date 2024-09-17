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

        private float _floatMessage = 0;
        private string _boolMessage = "";
        private string _stringMessage = "";


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

        public void OnToggleButtonChanged(bool value)
        {
            Debug.Log("OnToggleButtonChanged");
            this.MqttClient.Connection.SetConnectFlag(value);
        }

        public void OnSliderTurnLed(float value)
        {
            MqttClient.Connection.Publish("Rexroth/float/message", value.ToString(CultureInfo.InvariantCulture));
        }

        public void OnToggleTurnLed(bool value)
        {
            MqttClient.Connection.Publish("Rexroth/bool/message", value.ToString(CultureInfo.InvariantCulture));
        }

        public void OnInputTextTurnLed(string value)
        {
            MqttClient.Connection.Publish("Rexroth/string/message", value);
        }

        public void VerificationMessageLecture(string value)
        {
            Debug.Log(value);
            MqttClient.Connection.Publish("sensor1", value);
        }



        public void SetDebug(bool value)
        {
            MqttClient.DebugMessages = value;
        }

        public void OnMessageArrived(MqttMessage m)
        {
            switch (m.GetTopic())
            {
                case "Rexroth/float/message":
                    _floatMessage = float.Parse(m.GetString(), CultureInfo.InvariantCulture);
                    break;
                case "Rexroth/bool/message":
                    _boolMessage = m.GetString();
                    break;
                case "Rexroth/string/message":
                    _stringMessage = m.GetString();
                    break;

            }
        }
    }
}



