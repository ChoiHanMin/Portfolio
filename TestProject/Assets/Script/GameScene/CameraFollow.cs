using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraFollow : MonoBehaviour
{
    public Transform target; // ���� ��� (ĳ����)
    public Vector3 offset; // ī�޶�� ĳ���� ���� �Ÿ�
    public float smoothSpeed = 0.125f; // ī�޶� �̵� �ε巯��

    void LateUpdate()
    {
        // ��ǥ ��ġ ���
        Vector3 desiredPosition = target.position + offset;

        // �ε巴�� ��ġ �̵�
        Vector3 smoothedPosition = Vector3.Lerp(transform.position, desiredPosition, smoothSpeed);

        transform.position = smoothedPosition;

        // ��� �ٶ󺸱�
        transform.LookAt(target);
    }
}
