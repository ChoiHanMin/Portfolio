using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterScript : MonoBehaviour, IPauseable
{

    [SerializeField] Animator CharAnim;
    public GameObject WeaponObj;
    [SerializeField] BoxCollider AttackCollider;


    //캐릭터 공격
    public int AttDamage = 10;


    //대쉬관련
    public float dashPower = 100f;
    public float dashDuration = 0.2f;
    public float dashCooldown = 1f;

    private Rigidbody rb;
    private bool isDashing = false;
    private bool canDash = true;

    //콤보어택
    public float comboResetTime = 1f; // 콤보 입력 제한 시간

    private int currentCombo = 0; // 현재 콤보 단계
    private float lastAttackTime = 0f; // 마지막 공격 시간
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
            // 콤보 입력 시간이 초과되었으면 초기화
            currentCombo = 0;
           

            if (isAttacking)
                isAttacking = false;
        }

        if (!isAttacking)
        {
            AttackCollider.enabled = true;
            currentCombo++; // 콤보 단계 증가

            if (currentCombo > 3)
            {
                currentCombo = 1; // 최대 콤보 단계 이후 초기화
            }

            CharAnim.SetTrigger("Attack" + currentCombo); // "Attack1", "Attack2", "Attack3" 트리거 실행
            lastAttackTime = Time.time; // 마지막 공격 시간 업데이트
            isAttacking = true;
        }
    }

    // 애니메이션 이벤트를 통해 호출될 메서드
    public void OnAttackAnimationEnd()
    {
        Debug.Log("End");
        AttackCollider.enabled = false;
        isAttacking = false; // 공격 상태 해제
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
