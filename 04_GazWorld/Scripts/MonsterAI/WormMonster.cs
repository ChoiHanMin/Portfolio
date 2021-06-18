using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using com.ootii.Messages;

public class WormMonster : MonsterBase
{
    public GameMng MyGameMng;

    public enum BossPhase
    {
        Phase_1 = 0,
        Phase_2 = 1,
        Phase_3 = 2,
        Phase_4 = 3,
        Phase_5 = 4,
    }

    [SerializeField, Header("레이어 검출")] LayerMask layer = 0;

    /// HP

    public MonsterHP HP;
    public GameObject[] BossColider;
    public GameObject SpecialCol_Head;
    public GameObject SpecialCol_Body;

    /// <summary>
    /// 애니메이션 
    /// </summary>

    public Animator BossAnim;

    readonly int hashIdle = Animator.StringToHash("State");
    readonly int hashPhase = Animator.StringToHash("Phase");
    readonly int DieTrigger = Animator.StringToHash("Die");
    readonly int CancelTrigger = Animator.StringToHash("Cancel");
    readonly int HitTrigger = Animator.StringToHash("HitTrigger");

    //
    //부채꼴에 쓰이는 값들
    //

    public SphereCollider SphereCol;
    public float fieldofViewAngle = 120.0f;

    /// <summary>
    /// Bool 값
    /// </summary>

    private bool bIsMonsterDie = false;
    public bool bIsAttacking = false;
    public bool bIsAnimEnd = false;
    public bool SpecialAttackClear = false;
    private bool bIsHeadSpecial = false;

    /// <summary>
    /// int 값
    /// </summary>

    public int BossMonsterState = 0;
    private int BossMonsterType = 0;
    private int BossNowPhase = 0;

    /// <summary>
    ///  float 값
    /// </summary>

    //public float BossFullHP = 10;
    //public float BossHP = 0;
    public float BossMonsterSpeed = 0;
    public float BossMonsterDef = 0;
    public float BossMonsterDmg = 0;
    private float BossAttackDmg = 0;

    public float FirstPhase = 0.8f;
    public float SecondPhase = 0.5f;
    public float LastPhase = 0.3f;
    public float MaxMove = 15.0f;
    /// <summary>
    ///  GameObject 값
    /// </summary>

    public GameObject Playerobj = null;


    public Collider[] coll;


    //레이캐스트

    private LineRenderer line;
    private RaycastHit hit;

    //사운드

    public AudioManager SoundMNG;

    [Header("머리 콜라이더")]
    public GameObject HeadCol;

    [Header("죽었을 때 꺼주는 콜라이더")]
    public GameObject[] HitColliOff;



    [Header("벌 소환 위치")]
    public BeeInstantiate BeeInit;

    public GameObject HPbar;
    int WormSpawnedBees;
    public bool bIsSpawn;

    public SpecialColider[] SpecialAttackColider;
    public WormAttackEffect Effect;

    private void Awake()
    {
        SphereCol = GetComponent<SphereCollider>();
        BossAnim = GetComponent<Animator>();
    }

    // Start is called before the first frame update
    private void Start()
    {
        HP.SetMonsterHP(500);

        HPbar.SetActive(true);



        BossMonsterState = (int)MonsterState.Idle;
        BossMonsterType = (int)MonsterType.Boss;
        MessageDispatcher.AddListener("PlayerDie", PlayerDie);
        MessageDispatcher.AddListener("SetWormAnimEnd", SetWormAnimEnd);
        MessageDispatcher.AddListener("WormSpawnedBee", WormSpawnedBee);
        MessageDispatcher.AddListener("WormHitTrigger", WormHitTrigger);

    }

    private void OnDestroy()
    {
        MessageDispatcher.RemoveListener("WormHitTrigger", WormHitTrigger);
        MessageDispatcher.RemoveListener("WormSpawnedBee", WormSpawnedBee);
        MessageDispatcher.RemoveListener("SetWormAnimEnd", SetWormAnimEnd);
        MessageDispatcher.RemoveListener("PlayerDie", PlayerDie);
    }

    // Update is called once per frame
    void Update()
    {

        if (HP.CharHP <= 0)
        {
            if (!bIsMonsterDie)
            {
                HPbar.SetActive(false);
                bIsMonsterDie = true;

                BossAnim.SetTrigger(DieTrigger);
                BossMonsterState = (int)MonsterState.Die;

                BeeInit.StopInstantiate();
                MessageDispatcher.SendMessage(this, "ScorePlus", 2, 0);


                if (MyGameMng != null)
                {
                    MyGameMng.EndStage();
                }

                Die();
            }
            return;
        }


        if (coll.Length == 0)
        {
            Idle();

        }
        else
        {
            SetMonsterState();

            switch (BossMonsterState)
            {
                case (int)MonsterState.Idle:
                    Idle();
                    break;

                case (int)MonsterState.Chase:
                    Move(CalDis(coll));
                    break;

                case (int)MonsterState.Attack:
                    Attack();
                    break;

                case (int)MonsterState.Die:
                    Debug.Log("Die");
                    return;
            }
        }
    }

    int GetPhase(IMessage message)
    {
        return BossNowPhase;
    }


    public override void Idle()
    {
        bIsAnimEnd = false;

        BossAnim.SetInteger(hashIdle, (int)MonsterState.Idle);
        BossAnim.SetInteger(hashPhase, 0);
        // Debug.Log("Idle");
    }

    public bool bIsSpawnBee = false;
    int Randnum = 0;
    public override void Attack()
    {
        if (bIsSpawnBee == false && bIsSpawn == true)
        {
            BeeInit.StartInstantiate();
            bIsSpawnBee = true;
        }

        if (!bIsAnimEnd)
        {

            HeadCol.SetActive(true);
            BossAnim.SetInteger(hashIdle, (int)MonsterState.Attack);
            //   Debug.Log("Attack In");

            Randnum = Random.Range(0, 3);
            Debug.Log("Worm :" + Randnum);

            CheckHPPercent(HP.CharHP);

            bIsAnimEnd = true;
        }
        //Debug.Log("Attack");
    }

    private void CheckHPPercent(float CurrentHP)
    {
        switch (Randnum)
        {
            case 0:
                StartCoroutine("Combo_1");
                break;
            case 1:
                StartCoroutine("Combo_2");
                break;
            case 2:
            case 3:
                StartCoroutine("Combo_3");
                break;

        }
    }

    private IEnumerator Combo_1()
    {
        BossNowPhase = (int)BossPhase.Phase_1;
        MessageDispatcher.SendMessage(this, "SetWormColi", BossNowPhase, 0);

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_1);

        bIsAttacking = false;
        yield return null;
    }

    private IEnumerator Combo_2()
    {
        BossNowPhase = (int)BossPhase.Phase_2;
        MessageDispatcher.SendMessage(this, "SetWormColi", BossNowPhase, 0);

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_2);

        bIsHeadSpecial = false;
        bIsAttacking = false;
        yield return null;
    }

    private IEnumerator Combo_3()
    {
        BossNowPhase = (int)BossPhase.Phase_3;
        MessageDispatcher.SendMessage(this, "SetWormColi", BossNowPhase, 0);

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_3);

        bIsHeadSpecial = true;
        bIsAttacking = false;
        yield return null;
    }

    public override bool Chase(GameObject Player)
    {
        if (Player != null && !bIsAnimEnd)
        {
            // Debug.Log("Chase");
            if (Vector3.Distance(transform.position, Player.transform.position) > MaxMove)
            {
                bIsAnimEnd = false;
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
        return;
    }

    public override void Die()
    {
        for (int i = 0; i < HitColliOff.Length; i++)
        {
            HitColliOff[i].SetActive(false);
        }


        BeeInit.StopInstantiate();
    }

    private void OnTriggerStay(Collider other)
    {
        coll = Physics.OverlapSphere(transform.position, SphereCol.radius + 30.0f, layer);
        Vector3 direction = Playerobj.transform.position - transform.position;

        //  if (!bIsAnimEnd)
        {
            direction.Normalize();
            Quaternion q = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
            transform.rotation = Quaternion.Lerp(transform.rotation, q, 0.5f);
        }

        //스피어콜라이더 내의 특정 레이어 콜라이더 검출
        if (other.CompareTag("Player") && Playerobj != null)
        {
            isCollision = true;
        }

    }

    public IEnumerator MonsterRotation(Quaternion q)
    {
        for (float t = 0; t < 1.0f; t += Time.smoothDeltaTime)
        {
            transform.rotation = Quaternion.Lerp(transform.rotation, q, t);
            yield return new WaitForSeconds(0.1f);
        }

        yield return null;
    }

    public void PlayerDie(IMessage message)
    {
        bIsAnimEnd = false;
    }

    public GameObject CalDis(Collider[] Players)
    {
        float Dis = 0;
        float OtherDis = 0;
        GameObject Player = null;
        if (Players.Length != 0)
        {
            //            Debug.LogError("CalDis");

            for (int i = 0; i < Players.Length; i++)
            {
                if (i == 0)
                {
                    Dis = Vector3.Distance(transform.position, Players[i].transform.position);
                    Player = Players[i].gameObject;
                }
                else
                {
                    OtherDis = Vector3.Distance(transform.position, Players[i].transform.position);
                    if (OtherDis < Dis)
                    {
                        Player = Players[i].gameObject;
                    }
                }
            }

            Playerobj = Player;
            return Player;
        }
        return null;
    }

    public override void SetMonsterState()
    {

        if (bIsAnimEnd)
        {
            BossMonsterState = (int)MonsterState.Attack;
        }
        else
        {
            if (Chase(CalDis(coll)))
            {
                BossMonsterState = (int)MonsterState.Idle;
            }
            else
            {
                BossMonsterState = (int)MonsterState.Attack;
            }
        }
    }

    public void SetWormAnimEnd(IMessage rMessage)
    {
        bIsAnimEnd = false;
    }


    public void WormSpawnedBee(IMessage rMessage)
    {
        if ((int)rMessage.Data == 0)
        {
            WormSpawnedBees++;
        }
        else
        {
            WormSpawnedBees--;

            if (WormSpawnedBees < 0)
            {
                WormSpawnedBees = 0;
            }
        }
    }
    public int GetWormSpawnBee()
    {
        return WormSpawnedBees;
    }

    //사운드
    public void PlayAttackWormSound()
    {
        SoundMNG.PlayAttackWormSound();
    }

    public void PlayBreathWormSound()
    {
        SoundMNG.PlayBreathWormSound();
    }

    public void PlayDieWormSound()
    {
        SoundMNG.PlayDieWormSound();
    }

    public void TimeOutBoss()
    {
        HP.SetMonsterHP(0);
    }

    /// <summary>
    /// Animation
    /// </summary>

    public void StopAnim()
    {
        BossAnim.speed = 0.1f;

        if (bIsHeadSpecial == true)
        {
            SpecialCol_Head.SetActive(true);
            Effect.Start_FX_Sheild_Effect_Particle();

            for (int i = 0; i < BossColider.Length; i++)
            {
                BossColider[i].SetActive(false);
            }
        }
        else
        {
            SpecialCol_Body.SetActive(true);
            Effect.Start_FX_Attack_Point_Particle();

            for (int i = 0; i < BossColider.Length; i++)
            {
                BossColider[i].SetActive(false);
            }
        }
        if (BossNowPhase == 1)
        {
            SpecialAttackColider[0].StartSpecialAttack();
        }

        if (BossNowPhase == 2)
        {
            SpecialAttackColider[1].StartSpecialAttack();
        }
    }

    public void PlayAnim()
    {
        if (bIsHeadSpecial == true)
        {
            SpecialCol_Head.SetActive(false);
            Effect.End_FX_Sheild_Effect_Particle();
            for (int i = 0; i < BossColider.Length; i++)
            {
                BossColider[i].SetActive(true);
            }
        }
        else
        {
            SpecialCol_Body.SetActive(false);
            Effect.End_FX_Attack_Point_Particle();
            for (int i = 0; i < BossColider.Length; i++)
            {
                BossColider[i].SetActive(true);
            }
        }

        if (SpecialAttackClear == true)
        {
            BossAnim.SetTrigger(CancelTrigger);
            if (bIsHeadSpecial == true)
            {
                Effect.Start_FX_Sheild_Crack_Particle();
            }
            else
            {
                Effect.Start_FX_Attack_Crack_Particle();
            }
        }

        BossAnim.speed = 1.0f;
    }

    public void AnimSlow()
    {
        BossAnim.speed = 0.3f;
    }

    public void AnimFast()
    {
        BossAnim.speed = 2.0f;
    }
    public void AnimNormal()
    {
        BossAnim.speed = 1.0f;
    }

    public void WormHitTrigger(IMessage rMessage)
    {
        BossAnim.SetTrigger(HitTrigger);
    }
 
}
