using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.TextCore.Text;

[RequireComponent(typeof(CharacterController))]
public class CharacterMovement : MonoBehaviour
{
    public JoyStick joystick; // Joystick ��ũ��Ʈ�� ����
    public CharacterScript character;
    public NavMeshAgent agent; // NavMeshAgent ������Ʈ
    public float moveSpeed = 5.0f; // �̵� �ӵ�
    public float rotationSpeed = 10.0f; // ȸ�� �ӵ�
    public float gravity = -9.81f; // �߷� ��

    private CharacterController controller;
    private Vector3 velocity;
    private bool isGrounded;

    void Start()
    {
        controller = GetComponent<CharacterController>();
    }

    void Update()
    {

        // ���̽�ƽ �Է� �� ��������
        float horizontal = joystick.InputVector.x;
        float vertical = joystick.InputVector.y;

        // �Է� ���� ������ ����
        if (horizontal == 0 && vertical == 0)
        {
            agent.ResetPath(); // ��� �ʱ�ȭ
            return;
        }

        // ���̽�ƽ �������� �̵� ��ǥ ���
        Vector3 direction = new Vector3(horizontal, 0, vertical).normalized;
        Vector3 targetPosition = transform.position + direction * moveSpeed * Time.deltaTime;
        character.SetAnim(true);

        // NavMeshAgent�� ����� �̵�
        if (NavMesh.SamplePosition(targetPosition, out NavMeshHit hit, 1f, NavMesh.AllAreas))
        {
            agent.SetDestination(hit.position);
        }
    }


    // ���� ��� �ִ��� Ȯ��
    // isGrounded = controller.isGrounded;
    // if (isGrounded && velocity.y < 0)
    // {
    //     velocity.y = -2f; // ���� �ٵ��� ����
    // }
    //
    // // ���̽�ƽ �Է� ���� ��������
    // Vector2 input = joystick.InputVector;
    //
    // // �Է� ���͸� ������� ĳ���� �̵� ó��
    // Vector3 direction = new Vector3(input.x, 0, input.y);
    // transform.Translate(direction * moveSpeed * Time.deltaTime, Space.World);
    //
    // // ĳ���Ͱ� �̵� ������ �ٶ󺸵��� ȸ�� ó��
    // if (direction != Vector3.zero)
    // {
    //     character.SetAnim(true);
    //     Quaternion toRotation = Quaternion.LookRotation(direction, Vector3.up);
    //     transform.rotation = Quaternion.RotateTowards(transform.rotation, toRotation, moveSpeed * 100 * Time.deltaTime);
    // }

    // �̵� �Է� ó�� (WASD �Ǵ� ȭ��ǥ Ű)
    // float moveX = Input.GetAxis("Horizontal");
    // float moveZ = Input.GetAxis("Vertical");
    // Vector3 move = transform.right * moveX + transform.forward * moveZ;
    //
    // // ĳ���͸� �ش� ��ġ�� �̵�
    // //transform.position = Vector3.MoveTowards(transform.position, touchPosition, moveSpeed * Time.deltaTime);
    // controller.Move(move * moveSpeed * Time.deltaTime);
    //
    // // ���� �Է� ó�� (�����̽���)
    // if (Input.GetButtonDown("Jump") && isGrounded)
    // {
    //     velocity.y = Mathf.Sqrt(jumpHeight * -2f * gravity);
    // }
    //
    // // �߷� ����
    // velocity.y += gravity * Time.deltaTime;
    // controller.Move(velocity * Time.deltaTime);
}
