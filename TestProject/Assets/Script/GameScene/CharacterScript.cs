using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterScript : MonoBehaviour, IPauseable
{

    [SerializeField] Animator CharAnim;
    public GameObject WeaponObj;
    [SerializeField] BoxCollider AttackCollider;


    //ĳ���� ����
    public int AttDamage = 10;


    //�뽬����
    public float dashPower = 100f;
    public float dashDuration = 0.2f;
    public float dashCooldown = 1f;

    private Rigidbody rb;
    private bool isDashing = false;
    private bool canDash = true;

    //�޺�����
    public float comboResetTime = 1f; // �޺� �Է� ���� �ð�

    private int currentCombo = 0; // ���� �޺� �ܰ�
    private float lastAttackTime = 0f; // ������ ���� �ð�
    private bool isAttacking = false;

    private void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    public void SetAnim(bool bIsRun)
    {
        CharAnim.SetBool("Move", bIsRun);
    }

    private void Update()
    {
        if(Input.GetKeyDown(KeyCode.K))
        {
            SetDash();
        }
    }
    public void SetDash()
    {
        CharAnim.SetTrigger("Dash");
        Vector3 inputDirection = new Vector3(Input.GetAxis("Horizontal"), 0, Input.GetAxis("Vertical")).normalized;
        if (inputDirection != Vector3.zero)
        {
            StartCoroutine(Dash(inputDirection));
        }
    }
    private IEnumerator Dash(Vector3 direction)
    {
        canDash = false;
        isDashing = true;

        rb.velocity = direction * dashPower;

        yield return new WaitForSeconds(dashDuration);

        rb.velocity = Vector3.zero;
        isDashing = false;

        yield return new WaitForSeconds(dashCooldown);
        canDash = true;
    }

    public void PerformCombo()
    {
        Debug.Log(isAttacking);

        if (Time.time - lastAttackTime >= comboResetTime)
        {
            // �޺� �Է� �ð��� �ʰ��Ǿ����� �ʱ�ȭ
            currentCombo = 0;
           

            if (isAttacking)
                isAttacking = false;
        }

        if (!isAttacking)
        {
            AttackCollider.enabled = true;
            currentCombo++; // �޺� �ܰ� ����

            if (currentCombo > 3)
            {
                currentCombo = 1; // �ִ� �޺� �ܰ� ���� �ʱ�ȭ
            }

            CharAnim.SetTrigger("Attack" + currentCombo); // "Attack1", "Attack2", "Attack3" Ʈ���� ����
            lastAttackTime = Time.time; // ������ ���� �ð� ������Ʈ
            isAttacking = true;
        }
    }

    // �ִϸ��̼� �̺�Ʈ�� ���� ȣ��� �޼���
    public void OnAttackAnimationEnd()
    {
        Debug.Log("End");
        AttackCollider.enabled = false;
        isAttacking = false; // ���� ���� ����
    }


    public void OnPause()
    {
        CharAnim.enabled = false;
    }

    public void OnResume()
    {
        CharAnim.enabled = true;
    }
}
