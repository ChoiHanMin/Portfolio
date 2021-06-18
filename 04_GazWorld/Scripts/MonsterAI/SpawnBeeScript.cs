using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using com.ootii.Messages;

public class SpawnBeeScript : MonsterBase
{
    public SoliderCount Count;
    //HP

    public MonsterHP HP;

    /// <summary>
    /// 애니메이션
    /// </summary>

    public Animator SoldierAnim;

    readonly int hashIdle = Animator.StringToHash("State");
    readonly int DieTrigger = Animator.StringToHash("Die");
    //
    //부채꼴에 쓰이는 값들
    //

    public SphereCollider col;
    public float fieldofViewAngle = 45.0f;

    /// <summary>
    /// Bool 값
    /// </summary>

    private bool bIsMonsterDie = false;

    /// <summary>
    /// int 값
    /// </summary>

    public int SoliderMonsterState = 0;
    private int SoliderMonsterType = 0;


    /// <summary>
    /// float 값
    /// </summary>


    public float SoliderMonsterSpeed = 0;
    public float SoliderMonsterDef = 0;
    public float SoliderMonsterDmg = 0;
    private float HitDmg = 0;

    /// <summary>
    ///  GameObject 값
    /// </summary>

    public GameObject Playerobj;
    public SphereCollider AttackPlayer;


    public Rigidbody Rigid;

    public float MonsterDistance = 0.0f;

    //사운드

    public AudioManager SoundMNG;

    [Header("보스 소환인가")]
    public bool bIsBossSpawn;

    private BossScript Boss;

    public Vector3 MovePosition;

    [Header("죽었을 때")]
    public GameObject BodyPosition;
    public ParticleSystem DieEffect;
    public GameObject ColiOff;
    private void Awake()
    {
        col = GetComponent<SphereCollider>();
        SoldierAnim = GetComponent<Animator>();
        Boss = GameObject.FindGameObjectWithTag("Boss").GetComponent<BossScript>();
    }

    // Start is called before the first frame update
    void Start()
    {
        SoliderMonsterState = (int)MonsterState.Idle;
        SoliderMonsterType = (int)MonsterType.Soldier;

        MessageDispatcher.AddListener("BossDie", BossDie);

        Invoke("TimeDie", 4.0f);
    }


    private void OnDestroy()
    {
        MessageDispatcher.RemoveListener("BossDie", BossDie);
    }
    // Update is called once per frame
    void Update()
    {


        if (HP.GetMonsterHP() <= 0)
        {
            if (!bIsMonsterDie)
            {
                Playerobj = null;
                bIsMonsterDie = true;

                SoldierAnim.SetTrigger(DieTrigger);
                SoliderMonsterState = (int)MonsterState.Die;
                Die();
            }
            return;
        }

        SetMonsterState();

        switch (SoliderMonsterState)
        {
            case (int)MonsterState.Idle:
                Idle();
                break;

            case (int)MonsterState.Chase:
                Move(Playerobj);
                break;

            case (int)MonsterState.Attack:
                Attack();
                break;

            case (int)MonsterState.Die:
                Debug.Log("Die");
                return;
        }
    }



    public override void Idle()
    {
        SoldierAnim.SetInteger(hashIdle, (int)MonsterState.Idle);
        //Debug.Log("Idle");
    }

    public override void Attack()
    {


        SoldierAnim.SetInteger(hashIdle, (int)MonsterState.Attack);
    }

    public override bool Chase(GameObject Player)
    {
        if (Player != null)
        {
            if (Vector3.Distance(transform.position, Playerobj.transform.position) > MonsterDistance)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        return false;
    }

    public override void Move(GameObject Player)
    {
        //bIsMoving = true;
        SoldierAnim.SetInteger(hashIdle, (int)MonsterState.Chase);

        if (MovePosition == Vector3.zero)
        {
            MovePosition = Playerobj.transform.position;
        }



        if (Player != null)
        {
            transform.position = Vector3.MoveTowards(transform.position, MovePosition, SoliderMonsterSpeed * Time.deltaTime);
        }
    }

    public override void Die()
    {
        MessageDispatcher.SendMessage(this, "SpawnedBee", 1, 0);
        MessageDispatcher.SendMessage(this, "WormSpawnedBee", 1, 0);

        DieEffect.transform.position = BodyPosition.transform.position;
        DieEffect.Play();
        ColiOff.SetActive(false);
        Destroy(this.gameObject, 3.0f);
    }

    public void BossDie(IMessage rMessage)
    {
        HP.SetMonsterHP(0);
        bIsBossSpawn = true;

        MessageDispatcher.SendMessage(this, "SpawnedBee", 1, 0);

        DieEffect.transform.position = BodyPosition.transform.position;
        DieEffect.Play();

        ColiOff.SetActive(false);
        Destroy(this.gameObject, 3.0f);
    }

    public void TimeDie()
    {
        MessageDispatcher.SendMessage(this, "SpawnedBee", 1, 0);
        MessageDispatcher.SendMessage(this, "WormSpawnedBee", 1, 0);
        Destroy(this.gameObject);
    }

    private void OnTriggerStay(Collider other)
    {

        if (other.CompareTag("Player") && !bIsMonsterDie)
        {
            if (Playerobj == null)
            {
                Playerobj = other.gameObject;
            }

            Vector3 vec = Playerobj.transform.position - transform.position;
            Quaternion q = Quaternion.LookRotation(vec);
            transform.rotation = q;
            isCollision = true;
        }

    }

    private void OnTriggerExit(Collider other)
    {
        isCollision = false;
        Playerobj = null;
    }

    public override void SetMonsterState()
    {

        if (isCollision)
        {
            if (Chase(Playerobj))
            {
                SoliderMonsterState = (int)MonsterState.Chase;
            }
            else
            {
                SoliderMonsterState = (int)MonsterState.Attack;
            }
        }
        else
        {
            SoliderMonsterState = (int)MonsterState.Idle;
        }
    }

    //사운드
    public void PlayAttackSound()
    {
        if (SoundMNG)
        {
            SoundMNG.PlayAttackBeeSound();
        }
    }
}
