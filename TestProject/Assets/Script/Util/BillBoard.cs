using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BillBoard : MonoBehaviour
{
    private Transform cameraTransform;
    void Start()
    {
        cameraTransform = Camera.main.transform; // ���� ī�޶� ����
    }

    void LateUpdate()
    {
        // ī�޶� ������ �ٶ󺸵��� ȸ��
        transform.LookAt(transform.position + cameraTransform.forward);
    }
}
