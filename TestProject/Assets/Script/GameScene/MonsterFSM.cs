using System.Collections;
using System.Collections.Generic;
using System.Threading;
using TMPro;
using Unity.Transforms;
using UnityEngine;
using UnityEngine.AI;
using Unity.Entities;
using UnityEngine.UI;

public enum MonsterState
{
    Idle,    // 대기
    Chase,   // 추적
    Attack,  // 공격
    Dead     // 죽음
}

public class TransitionCondition
{
    public bool IsPlayerInSight { get; set; } // 플레이어 시야 감지
    public bool IsPlayerInAttackRange { get; set; } // 공격 범위 내 있는지
    public bool IsDead { get; set; } // 죽었는지 여부
}
public class IdleState : MonsterStateBase
{
    public IdleState(MonsterFSM monster, TransitionCondition conditions) : base(monster, conditions) { }

    //추적 거리
    public float sightRange = 1000f;

    public override void Enter()
    {
        Debug.Log("Idle 상태 진입");
    }

    public override void Update()
    {
        // 죽는상태 전환
        if (conditions.IsDead)
        {
            monster.ChangeState(MonsterState.Dead);
        }

        float distanceToPlayer = Vector3.Distance(monster.transform.position, monster.TargetTrans.position);
        conditions.IsPlayerInSight = (distanceToPlayer <= sightRange);

        // 플레이어가 시야에 들어오면 Chase 상태로 전환
        if (conditions.IsPlayerInSight)
        {
            monster.ChangeState(MonsterState.Chase);
        }

    }

    public override void Exit()
    {
        Debug.Log("Idle 상태 종료");
    }
}
public class ChaseState : MonsterStateBase
{
    public ChaseState(MonsterFSM monster, TransitionCondition conditions) : base(monster, conditions) { }

    //추적 거리
    public float sightRange = 10f;

    public float attackRange = 3f;

    public override void Enter()
    {
        Debug.Log("ChaseState 상태 진입");
        monster.StartChase(); // 쫓기 시작
    }

    public override void Update()
    {
        // 죽는상태 전환
        if (conditions.IsDead)
        {
            monster.ChangeState(MonsterState.Dead);
        }


        float distanceToPlayer = Vector3.Distance(monster.transform.position, monster.TargetTrans.position);
        conditions.IsPlayerInAttackRange = (distanceToPlayer <= attackRange);

        // 플레이어가 시야에 들어오면 Attack 상태로 전환
        if (conditions.IsPlayerInAttackRange)
        {
            monster.ChangeState(MonsterState.Attack);
        }
    }

    public override void Exit()
    {
        monster.EndChase();
        Debug.Log("ChaseState 상태 종료");
    }
}
public class AttackState : MonsterStateBase
{
    public AttackState(MonsterFSM monster, TransitionCondition conditions) : base(monster, conditions) { }

    //추적 거리
    public float sightRange = 10f;

    public float attackRange = 3f;

    public override void Enter()
    {
        Debug.Log("AttackState 상태 진입");
        monster.StartAttack(); // 공격 시작
    }

    public override void Update()
    {
        // 몬스터 HP가 0이하면 죽음
        if (conditions.IsDead)
        {
            monster.ChangeState(MonsterState.Dead);
        }

        float distanceToPlayer = Vector3.Distance(monster.transform.position, monster.TargetTrans.position);
        conditions.IsPlayerInSight = (distanceToPlayer <= sightRange);
        conditions.IsPlayerInAttackRange = (distanceToPlayer <= attackRange);

        if (!conditions.IsPlayerInAttackRange)
        {
            monster.ChangeState(MonsterState.Chase);
        }
        else
        {
            monster.StartAttack(); // 공격 시작
        }

    }

    public override void Exit()
    {
        Debug.Log("Attack 상태 종료");
        monster.MonsterEndAttack();
    }
}
public class DeadState : MonsterStateBase
{
    public DeadState(MonsterFSM monster, TransitionCondition conditions) : base(monster, conditions) { }

    public override void Enter()
    {
        Debug.Log("Dead 상태 진입");
        monster.StartDead();
    }

    public override void Update()
    {

    }

    public override void Exit()
    {
        Debug.Log("Dead 상태 종료");
    }
}


public abstract class MonsterStateBase
{
    protected MonsterFSM monster;
    protected TransitionCondition conditions;

    public MonsterStateBase(MonsterFSM monster, TransitionCondition conditions)
    {
        this.monster = monster;
        this.conditions = conditions;
    }

    public abstract void Enter();
    public abstract void Update();
    public abstract void Exit();
}

public struct Walker : IComponentData
{
    public float ForwardSpeed;
    public float AngularSpeed;
}

public class MonsterFSM : MonoBehaviour, IPauseable
{
    private Dictionary<MonsterState, MonsterStateBase> states;
    private MonsterStateBase currentState;
    private MonsterState wasState;
    public TransitionCondition Conditions { get; } = new TransitionCondition();


    [SerializeField] Animator MonsterAnim;
    [SerializeField] CapsuleCollider Colider;
    [SerializeField] BoxCollider WeaponColider;
    [SerializeField] SkinnedMeshRenderer MonsterRenderer;
    [SerializeField] ParticleSystem DieParticle;

    [SerializeField] Slider HPBar;

    public Material dissolveMaterial;
    public float dissolveSpeed = .5f;

    public Transform TargetTrans;
    public NavMeshAgent agent;


    private int HP = 0;
    private int CurrentHP = 0;
    private bool bIsDead;

    public float _forwardSpeed = 1;
    public float _angularSpeed = 1;
    class Baker : Baker<MonsterFSM>
    {
        public override void Bake(MonsterFSM src)
        {
            var data = new Walker()
            {
                ForwardSpeed = src._forwardSpeed,
                AngularSpeed = src._angularSpeed
            };
            AddComponent(GetEntity(TransformUsageFlags.Dynamic), data);
        }
    }

    void Start()
    {
        dissolveMaterial = MonsterRenderer.materials[0];
        // 상태 초기화
        states = new Dictionary<MonsterState, MonsterStateBase>
        {
            { MonsterState.Idle, new IdleState(this, Conditions) },
            { MonsterState.Chase, new ChaseState(this, Conditions) },
            { MonsterState.Attack, new AttackState(this, Conditions) },
            { MonsterState.Dead, new DeadState(this, Conditions) },
        };

        // 초기 상태 설정
        ChangeState(MonsterState.Idle);
        TargetTrans = GameManager.inst.GuardObj.transform;

    }

    void Update()
    {
        if (isPaused) return;
        transform.LookAt(TargetTrans);
        currentState?.Update();
    }

    //몬스터 디졸브
    private IEnumerator DissolveEffect(float startValue, float endValue)
    {
        float dissolveAmount = startValue;

        while (Mathf.Abs(dissolveAmount - endValue) > 0.01f)
        {
            dissolveAmount = Mathf.MoveTowards(dissolveAmount, endValue, Time.deltaTime * dissolveSpeed);
            dissolveMaterial.SetFloat("_DissolveAmount", dissolveAmount);
            yield return null;
        }

        dissolveMaterial.SetFloat("_DissolveAmount", endValue);
    }

    public void ChangeState(MonsterState newState)
    {
        currentState?.Exit();
        currentState = states[newState];
        currentState.Enter();
    }

    public void StartChase()
    {
        MonsterAnim.SetBool("Move", true);

        agent.SetDestination(TargetTrans.position);
    }

    public void EndChase()
    {
        MonsterAnim.SetBool("Move", false);
    }

    public void StartAttack()
    {
        // 공격 로직 구현
        MonsterAnim.SetTrigger("Attack");
        WeaponColider.enabled = true;
    }
    public void MonsterEndAttack()
    {
        WeaponColider.enabled = false;
    }

    public void StartDead()
    {
        DieParticle.Play();
        // 죽음 로직 구현
        MonsterAnim.SetTrigger("Death");

        StartCoroutine(DissolveEffect(0f, 1f));

        if (WeaponColider.enabled)
            WeaponColider.enabled = false;

        agent.ResetPath();

        GameManager.inst.MonsterDie();

        Destroy(this.gameObject, 2f);
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag.Equals("Player"))
        {
            Debug.Log("Hit");
            SetDamage();

            if (bIsDead)
            {
                Conditions.IsDead = true;
                Colider.enabled = false;
                WeaponColider.enabled = false;
                return;
            }

        }
    }

    public void SetMonster(bool bIsBoss)
    {
        if (bIsBoss)
        {
            HP = 140;
        }
        else
        {
            HP = 50;
        }

        CurrentHP = HP;
        HPBar.value = 1;
    }


    void SetDamage()
    {
        CurrentHP -= 10;
        float hpValue = (float)CurrentHP / HP;
        Debug.Log(CurrentHP / HP);

        HPBar.value = hpValue;

        if (CurrentHP <= 0)
        {
            bIsDead = true;
        }
        else
        {
            bIsDead = false;
        }
    }


    private bool isPaused = false;
    public void OnPause()
    {
        isPaused = true;
        if (bIsDead)
            return;

        MonsterAnim.enabled = false;
        agent.enabled = false;
        ChangeState(MonsterState.Idle);
    }

    public void OnResume()
    {
        isPaused = false;
        MonsterAnim.enabled = true;
        agent.enabled = true;
        ChangeState(wasState);
    }
}
