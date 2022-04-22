using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;
using EndoScopeDeviceDLL;
using EndoScopeDeviceDLL.CORE;

namespace EndoScopeDeviceDLL
{



    namespace CORE
    {

        public class EndoScopeBehavior
        {
            public void Start
        }

        public interface deviceMethod
        {
            void RecvFlag(string data);
        }

        public class SerialConect
        {
            public delegate void Eve(string x);

            public SerialPort port = new SerialPort()
            {
                NewLine = "\n",
                BaudRate = 115200,
                Handshake = Handshake.None,
                Encoding = System.Text.Encoding.ASCII,
                ReadTimeout = 100,
                WriteTimeout = 100,
                Parity = Parity.None,
                DataBits = 8,
                StopBits = StopBits.One,
                RtsEnable = false,
            };

            public SerialConect(string PortName, int BaudRate, Eve Error)
            {
                port.PortName = PortName;
                port.BaudRate = BaudRate;
                 
                if (Error != null)
                    port.ErrorReceived += delegate (object m, SerialErrorReceivedEventArgs e)
                    {
                        Error(e.EventType.ToString());
                    };

                port.Open();
                while (!port.IsOpen) ;
            }

            public void Send(string x)
            { 
                if (CheckPORT && x != string.Empty) port.Write(x);
            }

            public string RecvLine(string N_A = "")
            {
                return CheckPORT ? port.ReadLine() : N_A;
            }


            public string RecvLine_flag(string flag ="\n", string N_A = "")
            {
                if (!CheckPORT) return N_A;
                port.Write(flag);
                return port.ReadLine();
            }

            bool CheckPORT => port == null || !port.IsOpen;
        }
    }
}
