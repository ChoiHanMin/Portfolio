using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using com.ootii.Messages;

using FSG.MeshAnimator;

public class SoliderScript : MonsterBase
{
    private SoliderCount Count;
    //HP

    public MonsterHP HP;
    Coroutine MyCor;

    public Vector3 MyStartTransform;

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
    private bool bIsReturn = false;

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


    public int Mywave;

    [Header("Flock")]
    public bool bIsFlock;
    public ParticleSystem Smoke;
    public GameObject RealBeePos;
    private void Awake()
    {
        col = GetComponent<SphereCollider>();
        SoldierAnim = GetComponent<Animator>();
        MyStartTransform = gameObject.transform.position;
        Count = GameObject.FindGameObjectWithTag("Bee").GetComponent<SoliderCount>();
    }

    // Start is called before the first frame update
    void Start()
    {
        //SoliderHP = SoliderFullHP;
        SoliderMonsterState = (int)MonsterState.Idle;
        SoliderMonsterType = (int)MonsterType.Soldier;

        MessageDispatcher.AddListener("BossDie", BossDie);
        MessageDispatcher.AddListener("StartBeeCorutine", MyState);
        MessageDispatcher.AddListener("FirstTimeUp", FirstTimeUp);


        SoldierAnim.speed = Random.Range(0.5f, 2.0f);

        MyCor = null;
    }


    private void OnDestroy()
    {
        MessageDispatcher.RemoveListener("FirstTimeUp", FirstTimeUp);
        MessageDispatcher.RemoveListener("StartBeeCorutine", MyState);
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


        switch (SoliderMonsterState)
        {
            case (int)MonsterState.Idle:
                Idle();
                break;

            case (int)MonsterState.Chase:
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
        if (!AttackPlayer.enabled)
        {
            AttackPlayer.enabled = true;
        }

        SoldierAnim.SetInteger(hashIdle, (int)MonsterState.Attack);
        //Debug.Log("Attack");
    }

    public override bool Chase(GameObject Player)
    {
        //        Debug.Log("Chase");
        if (Player != null)
        {
            // Debug.Log("Chase");
            if (Vector3.Distance(transform.position, Player.transform.position) > MonsterDistance)
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

        if (AttackPlayer.enabled)
        {
            AttackPlayer.enabled = false;
        }

        if (bIsReturn == false)
        {
            if ((Player != null) && (Vector3.Distance(transform.position, Player.transform.position) > MonsterDistance))
            {
                transform.position = Vector3.MoveTowards(transform.position, Player.transform.position, SoliderMonsterSpeed * Time.deltaTime);
            }
        }
        else
        {
            transform.position = Vector3.MoveTowards(transform.position, MyStartTransform, SoliderMonsterSpeed * Time.deltaTime);
        }
    }



    public override void Die()
    {
        if (bIsBossSpawn == false)
        {

            MessageDispatcher.SendMessage(this, "ScorePlus", 0, 0);


            Count.SoliderCountDown();

            if (bIsFlock)
            {
                //Smoke.Play();
                gameObject.SetActive(false);
                Count.SpawnDieParticle(RealBeePos.transform.position, false);
            }
            else
            {
                Rigid.useGravity = true;
                // 추후에 오브젝트 풀로 변경
                Destroy(this.gameObject, 1.0f);
            }

        }
    }

    IEnumerator DieCor()
    {
        yield return new WaitForSeconds(1.0f);
        gameObject.SetActive(false);
    }

    public void BossDie(IMessage rMessage)
    {
        HP.SetMonsterHP(0);
        bIsBossSpawn = true;
        Destroy(this.gameObject, 3.0f);
    }

    public void FirstTimeUp(IMessage rMessage)
    {
        if (bIsFlock)
        {
            gameObject.SetActive(false);
            Count.SpawnDieParticle(RealBeePos.transform.position, true);
        }
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

    private void MyState(IMessage rMessage)
    {

        if ((int)rMessage.Data == Mywave)
        {
            int i = Random.Range(5, 10);
        }
    }


    IEnumerator StartAttack(int Time)
    {
        while (SoliderMonsterState != (int)MonsterState.Die)
        {
            yield return new WaitForSeconds(Time);

            SoliderMonsterState = (int)MonsterState.Chase;
            bIsReturn = false;
            yield return new WaitForSeconds(Time);

            SoliderMonsterState = (int)MonsterState.Attack;

            yield return new WaitForSeconds(3.0f);

            SoliderMonsterState = (int)MonsterState.Chase;
            bIsReturn = true;
            yield return new WaitForSeconds(Time);
        }

        yield return null;
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
