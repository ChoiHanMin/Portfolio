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

    [SerializeField] Rigidbody rb;
    public float moveSpeed = 5.0f; // �̵� �ӵ�
    public float rotationSpeed = 10.0f; // ȸ�� �ӵ�

    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    void Update()
    {

        // ���̽�ƽ �Է� �� ��������
        float horizontal = joystick.InputVector.x;
        float vertical = joystick.InputVector.y;

        // �Է� ���� ������ �������� ����
        if (horizontal == 0 && vertical == 0) return;

        // �̵� ���� ���
        Vector3 direction = new Vector3(horizontal, 0, vertical).normalized;

        // Rigidbody�� ����� ���������� �̵�
        rb.MovePosition(transform.position + direction * moveSpeed * Time.fixedDeltaTime);
        character.SetAnim(true);

        // ĳ���Ͱ� �̵� ������ �ٶ󺸵��� ȸ��
        if (direction != Vector3.zero)
            rb.rotation = Quaternion.LookRotation(direction);
    }
}
