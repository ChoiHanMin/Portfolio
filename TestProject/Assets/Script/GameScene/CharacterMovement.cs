using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.TextCore.Text;

[RequireComponent(typeof(CharacterController))]
public class CharacterMovement : MonoBehaviour
{
    public JoyStick joystick; // Joystick 스크립트를 참조
    public CharacterScript character;

    [SerializeField] Rigidbody rb;
    public float moveSpeed = 5.0f; // 이동 속도
    public float rotationSpeed = 10.0f; // 회전 속도

    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    void Update()
    {

        // 조이스틱 입력 값 가져오기
        float horizontal = joystick.InputVector.x;
        float vertical = joystick.InputVector.y;

        // 입력 값이 없으면 움직이지 않음
        if (horizontal == 0 && vertical == 0) return;

        // 이동 방향 계산
        Vector3 direction = new Vector3(horizontal, 0, vertical).normalized;

        // Rigidbody를 사용해 물리적으로 이동
        rb.MovePosition(transform.position + direction * moveSpeed * Time.fixedDeltaTime);
        character.SetAnim(true);

        // 캐릭터가 이동 방향을 바라보도록 회전
        if (direction != Vector3.zero)
            rb.rotation = Quaternion.LookRotation(direction);
    }
}
