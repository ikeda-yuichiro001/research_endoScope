using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using EndoScope.v0;

public class SerialTest : MonoBehaviour
{
    public string ee = "";
    public bool e;

    public DeviceManager device;
    void Start()
    {
        device = gameObject.AddComponent<DeviceManager>();
        device.error += Debug.Log;
        device.Updaxte("0a1d0fe3");
    }

    // Update is called once per frame
    void Update()
    {
        if (e)
        {
            device.Updaxte(ee);
            e = false;
            Debug.Log("call deviceUpdate");
        }
    }
}
 
//GW前の段かい
//データ解析をテストして確認したところ