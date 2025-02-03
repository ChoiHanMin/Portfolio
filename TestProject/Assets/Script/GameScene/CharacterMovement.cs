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
    public NavMeshAgent agent; // NavMeshAgent 컴포넌트
    public float moveSpeed = 5.0f; // 이동 속도
    public float rotationSpeed = 10.0f; // 회전 속도
    public float gravity = -9.81f; // 중력 값

    private CharacterController controller;
    private Vector3 velocity;
    private bool isGrounded;

    void Start()
    {
        controller = GetComponent<CharacterController>();
    }

    void Update()
    {

        // 조이스틱 입력 값 가져오기
        float horizontal = joystick.InputVector.x;
        float vertical = joystick.InputVector.y;

        // 입력 값이 없으면 정지
        if (horizontal == 0 && vertical == 0)
        {
            agent.ResetPath(); // 경로 초기화
            return;
        }

        // 조이스틱 방향으로 이동 목표 계산
        Vector3 direction = new Vector3(horizontal, 0, vertical).normalized;
        Vector3 targetPosition = transform.position + direction * moveSpeed * Time.deltaTime;
        character.SetAnim(true);

        // NavMeshAgent를 사용해 이동
        if (NavMesh.SamplePosition(targetPosition, out NavMeshHit hit, 1f, NavMesh.AllAreas))
        {
            agent.SetDestination(hit.position);
        }
    }


    // 땅에 닿아 있는지 확인
    // isGrounded = controller.isGrounded;
    // if (isGrounded && velocity.y < 0)
    // {
    //     velocity.y = -2f; // 땅에 붙도록 설정
    // }
    //
    // // 조이스틱 입력 벡터 가져오기
    // Vector2 input = joystick.InputVector;
    //
    // // 입력 벡터를 기반으로 캐릭터 이동 처리
    // Vector3 direction = new Vector3(input.x, 0, input.y);
    // transform.Translate(direction * moveSpeed * Time.deltaTime, Space.World);
    //
    // // 캐릭터가 이동 방향을 바라보도록 회전 처리
    // if (direction != Vector3.zero)
    // {
    //     character.SetAnim(true);
    //     Quaternion toRotation = Quaternion.LookRotation(direction, Vector3.up);
    //     transform.rotation = Quaternion.RotateTowards(transform.rotation, toRotation, moveSpeed * 100 * Time.deltaTime);
    // }

    // 이동 입력 처리 (WASD 또는 화살표 키)
    // float moveX = Input.GetAxis("Horizontal");
    // float moveZ = Input.GetAxis("Vertical");
    // Vector3 move = transform.right * moveX + transform.forward * moveZ;
    //
    // // 캐릭터를 해당 위치로 이동
    // //transform.position = Vector3.MoveTowards(transform.position, touchPosition, moveSpeed * Time.deltaTime);
    // controller.Move(move * moveSpeed * Time.deltaTime);
    //
    // // 점프 입력 처리 (스페이스바)
    // if (Input.GetButtonDown("Jump") && isGrounded)
    // {
    //     velocity.y = Mathf.Sqrt(jumpHeight * -2f * gravity);
    // }
    //
    // // 중력 적용
    // velocity.y += gravity * Time.deltaTime;
    // controller.Move(velocity * Time.deltaTime);
}
