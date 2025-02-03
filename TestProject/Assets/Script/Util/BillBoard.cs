using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BillBoard : MonoBehaviour
{
    private Transform cameraTransform;
    void Start()
    {
        cameraTransform = Camera.main.transform; // 메인 카메라 참조
    }

    void LateUpdate()
    {
        // 카메라 방향을 바라보도록 회전
        transform.LookAt(transform.position + cameraTransform.forward);
    }
}
