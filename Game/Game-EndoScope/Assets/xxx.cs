using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using EndoScope;
using EndoScope.v0;
using EndoScope.core;


namespace EndoScope
{

    public delegate void EndoScope_EVENTstr(string text);
     
    namespace v0
    {
        public class DeviceManager : DEVICEMANAGER, DeviceMethod
        {
            public override void Init()
            { 
            
            }

            public override void RecvText(string text, int L)
            {
                if (L == 8)
                { 
                    int[] d = Arr64.Arr64ToDecimal_ARRAY(text);
                    foreach (var i in d)
                        if (i < 0)
                        {
                            error?.Invoke("001:�g�p�s�\�ȕ���");
                            return;
                        } 
                    device.handle.HandleX = d[0] * 16 + d[1];
                    device.handle.HandleY = d[2] * 16 + d[3];
                    device.oral.Twist = d[4] * 262144 + d[5] * 4096 + d[6] * 64 + d[7] - 8000000;
                }
                else
                {
                    error?.Invoke("000:����������v���܂���");
                }
            }
             
        }
    }

    namespace core
    {

        interface DeviceMethod
        {
            void Init();
            void RecvText(string text, int L); 
        }
         
        public class DEVICEMANAGER : MonoBehaviour
        {
            public DeviceInfo device = new DeviceInfo() { handle = new Handles(), oral = new Oral() };
            public virtual void RecvText(string text, int L) { }
            public virtual void Init( ) { }
            public EndoScope_EVENTstr error;
            public void Start()
            {
               
                //�|�[�g�ڑ�
                //�f�o�C�X�j���h�~
                //
            }

            public void Updaxte(string DebugText = "")
            {
                string text = "";
                if (DebugText != "")
                { 
                    text = DebugText;
                }
                else
                { 
                    //��M
                }

                if (text.Length > 0)
                { 
                    RecvText(text, text.Length);
                }
                //����'\n'���M
            }

            public void OnDestroy()
            { 
                //�|�[�g�����
                //���̂��������
            }
        }


        public static class Arr64
        {
            const string ARR64__ = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-";
            public static int Arr64ToDecimal(string raw) => ARR64__.IndexOf(raw);
            public static int Arr64ToDecimal(char raw) => ARR64__.IndexOf(raw);
            public static string DecimalToArr64(int p) => p > 63 && p < 0 ? "" : ARR64__[p].ToString();

            public static int[] Arr64ToDecimal_ARRAY(string text)
            {
                int[] v = new int[text.Length];
                for (int f = 0; f < text.Length; f++)
                    v[f] = Arr64ToDecimal(text[f]);
                return v;
            }

            public static string DecimalToArr64_ARRAY(int[] raw, char missingWord = '#')
            {
                string v = "", r = "";
                foreach (var i in raw)
                {
                    r = DecimalToArr64(i);
                    v += r == "" ? missingWord : r;  
                }
                return v;
            }
        }






        [System.Serializable]
        public class DeviceInfo
        {
            public Handles handle;
            public Oral oral;
        }




        [System.Serializable]
        public class Handles
        {
            public float HandleX;
            public float HandleY;
        }




        [System.Serializable]
        public class Oral
        { 
            public float Twist;
        }






    }


}


