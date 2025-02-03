using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraFollow : MonoBehaviour
{
    public Transform target; // 따라갈 대상 (캐릭터)
    public Vector3 offset; // 카메라와 캐릭터 간의 거리
    public float smoothSpeed = 0.125f; // 카메라 이동 부드러움

    void LateUpdate()
    {
        // 목표 위치 계산
        Vector3 desiredPosition = target.position + offset;

        // 부드럽게 위치 이동
        Vector3 smoothedPosition = Vector3.Lerp(transform.position, desiredPosition, smoothSpeed);

        transform.position = smoothedPosition;

        // 대상 바라보기
        transform.LookAt(target);
    }
}
