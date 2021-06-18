using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using DG.Tweening;
using com.ootii.Messages;

public class BossScript : MonsterBase
{

    public GameMng MyGameMng;

    //text

    public UnityEngine.AI.NavMeshAgent Nav;
    [SerializeField, Header("레이어 검출")] LayerMask layer = 0;


    public enum BossPhase
    {
        Phase_1 = 0,
        Phase_2 = 1,
        Phase_3 = 2,
        Phase_4 = 3,
        Phase_5 = 4,
        BlockCrashSucsess = 5,
        BlockCrashFail = 6,
    }

    /// HP

    public MonsterHP HP;
    public GameObject BossColider;
    public GameObject SpecialCol;


    /// <summary>
    /// 애니메이션 
    /// </summary>

    public Animator BossAnim;

    readonly int hashIdle = Animator.StringToHash("State");
    readonly int hashPhase = Animator.StringToHash("Phase");
    readonly int DieTrigger = Animator.StringToHash("Die");
    readonly int StartTrigger = Animator.StringToHash("Start");
    readonly int HitTrigger = Animator.StringToHash("HitTrigger");
    readonly int Roar = Animator.StringToHash("Roar");

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
    public bool bIsSecondPhase = false;
    public bool bIsThirdPhase = false;
    public bool bIsFinalPhase = false;
    public bool SpecialAttackClear = false;
    public bool bIsIceGolem = false;
    private bool bIsBeeSpawn = false;
    public bool bIsBeesSpawn = false;

    /// <summary>
    /// int 값
    /// </summary>

    public int BossMonsterState = 0;
    private int BossMonsterType = 0;
    private int BossNowPhase = 0;

    /// <summary>
    ///  float 값
    /// </summary>

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

    public Collider[] coll;
    public GameObject PlayerOBJ;

    //레이캐스트

    private LineRenderer line;
    private RaycastHit hit;

    public GameObject StartEffect;

    [Header("공격범위")]
    public GameObject AttackRange;

    [Header("벌 소환 위치")]
    public BeeInstantiate BeeInit;


    [Header("보스몹 HP바")]
    public GameObject MonsterHPBar;
    private bool BossTimeOut;

    public int SpawnedBees = 0;

    public SetGolColider coli;

    public SpecialColider SpecialAttackColider;

    [Header("보스몹 디졸브")]
    public BossDissolveController Dissolve;
    public DOTweenPath Path;
    public GolemAttackEffect Effect;


    private void Awake()
    {
        SphereCol = GetComponent<SphereCollider>();
        BossAnim = GetComponent<Animator>();
    }

    // Start is called before the first frame update
    private void Start()
    {

        BossMonsterState = (int)MonsterState.Idle;
        BossMonsterType = (int)MonsterType.Boss;
        MessageDispatcher.AddListener("SetGolAnimEnd", SetGolAnimEnd);
        MessageDispatcher.AddListener("LookAtPlayer", LookAtPlayer);
        MessageDispatcher.AddListener("PlayerDie", PlayerDie);
        MessageDispatcher.AddListener("SpawnedBee", SpawnedBee);
        MessageDispatcher.AddListener("GolHitTrigger", GolHitTrigger);
        
    }

    private void OnDestroy()
    {
        MessageDispatcher.RemoveListener("GolHitTrigger", GolHitTrigger);
        MessageDispatcher.RemoveListener("SpawnedBee", SpawnedBee);
        MessageDispatcher.RemoveListener("PlayerDie", PlayerDie);
        MessageDispatcher.RemoveListener("LookAtPlayer", LookAtPlayer);
        MessageDispatcher.RemoveListener("SetGolAnimEnd", SetGolAnimEnd);
    }

    // Update is called once per frame
    void Update()
    {
        if (HP.CharHP <= 0)
        {
            if (!bIsMonsterDie)
            {
                bIsMonsterDie = true;

                BossAnim.speed = 1.0f;
                BossAnim.SetTrigger(DieTrigger);
                BossMonsterState = (int)MonsterState.Die;
                bIsBeeSpawn = false;
                Die();
            }
            return;
        }


        if (!isCollision)
        {
            Nav.velocity = Vector3.zero;
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


    public void LookAtPlayer(IMessage rMessage)
    {
        AttackRange.transform.localScale = new Vector3(0.01f, 0.01f, 0.01f);
        Vector3 direction = PlayerOBJ.transform.position - transform.position;

        Quaternion q = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
        StartCoroutine("MonsterRotation", q);
    }

    public IEnumerator MonsterRotation(Quaternion q)
    {
        for (float t = 0; t < 1.0f; t += Time.smoothDeltaTime)
        {
            transform.rotation = Quaternion.Lerp(transform.rotation, q, t);
            yield return null;
        }
    }


    public override void Idle()
    {

        if (Nav.enabled)
        {
            BossAnim.SetInteger(hashIdle, (int)MonsterState.Idle);
            bIsAnimEnd = true;
        }
        else
        {
            BossAnim.SetInteger(hashIdle, (int)MonsterState.Born);
        }
        // Debug.Log("Idle");
    }

    public void BossWake()
    {
        if (bIsIceGolem)
        {
            gameObject.SetActive(true);
            Path.DOPlay();
        }

        HP.SetMonsterHP(400);

        BossAnim.SetTrigger(StartTrigger);
        BossColider.SetActive(true);

        if (!bIsIceGolem)
        {
            BossStartEffect();
            Dissolve.StartDissolve();
            MonsterHPBar.SetActive(true);
        }

        AttackRange.SetActive(true);


        coli.Rocks[0].transform.localPosition = new Vector3(0, -1, 1.89f);
        coli.Rocks[1].transform.localPosition = new Vector3(-0.67f, -1, 3.87f);
        coli.Rocks[2].transform.localPosition = new Vector3(0.49f, -1, 3.79f);
        coli.Rocks[3].transform.localPosition = new Vector3(-1.323f, -1, 5.954f);
        coli.Rocks[4].transform.localPosition = new Vector3(0.012f, -1, 6.022f);
        coli.Rocks[5].transform.localPosition = new Vector3(1.1f, -1, 6.17f);

        Vector3 direction = PlayerOBJ.transform.position - transform.position;
        Quaternion q = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));

        BossTimeOut = false;

        StartCoroutine("MonsterRotation", q);
    }

    public void ShowHPBar()
    {
        MonsterHPBar.SetActive(true);
    }

    int Randnum = 0;
    public override void Attack()
    {
        if (bIsBeeSpawn == false && bIsBeesSpawn == true)
        {
            Debug.Log("BossBeeSpawn");
            BeeInit.StartInstantiate();
            bIsBeeSpawn = true;
        }

        if (!bIsAnimEnd)
        {
            BossAnim.SetInteger(hashIdle, (int)MonsterState.Attack);

            Randnum = Random.Range(0, 4);

            CheckHPPercent(HP.CharHP);

            bIsAnimEnd = true;
        }
    }

    private void CheckHPPercent(float CurrentHP)
    {
        float HPPercent = CurrentHP / HP.MaxHP;

        bIsAttacking = true;

        switch (Randnum)
        {
            case 0:
            case 1:
                Combo_1();
                break;

            case 2:
                Combo_2();
                break;

            case 3:
                Combo_3();
                break;
        }
    }

    private void Combo_1()
    {
        BossNowPhase = (int)BossPhase.Phase_1;

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_1);

        bIsAttacking = false;
    }

    private void Combo_2()
    {
        BossNowPhase = (int)BossPhase.Phase_2;


        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_2);


        bIsAttacking = false;
    }

    private void Combo_3()
    {
        BossNowPhase = (int)BossPhase.Phase_3;


        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_3);

        bIsAttacking = false;
    }

    private IEnumerator Combo_4()
    {
        BossNowPhase = (int)BossPhase.Phase_4;

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_4);

        bIsAttacking = false;
        yield return null;
    }

    private IEnumerator Combo_5()
    {
        BossNowPhase = (int)BossPhase.Phase_5;

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_5);

        bIsAttacking = false;
        yield return null;
    }

    public override bool Chase(GameObject Player)
    {
        if (Player != null && !bIsAnimEnd)
        {
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

    public override void Die()
    {

        BossColider.SetActive(false);
        MonsterHPBar.SetActive(false);

        if (BossTimeOut == false)
        {
            MessageDispatcher.SendMessage(this, "ScorePlus", 1, 0);

            if (MyGameMng != null)
            {
                MyGameMng.StopTimeCoroutine();
                MyGameMng.EndStage();
            }
        }

        SpawnedBees = 0;
        BeeInit.StopInstantiate();

        MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
        Destroy(this.gameObject, 4.0f);

    }


    private void OnTriggerStay(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            isCollision = true;
        }
    }

    public GameObject CalDis(Collider[] Players)
    {
        float Dis = 0;
        float OtherDis = 0;
        GameObject Player = null;
        if (Players.Length != 0)
        {

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
            return Player;
        }
        return null;
    }

    public void PlayerDie(IMessage message)
    {
        bIsAnimEnd = false;
    }

    public override void SetMonsterState()
    {

        if (isCollision)
        {
            if (Chase(PlayerOBJ))
            {
                BossMonsterState = (int)MonsterState.Chase;
            }
            else
            {
                BossMonsterState = (int)MonsterState.Attack;
            }
        }
        else
        {
            BossMonsterState = (int)MonsterState.Idle;
        }
    }

    public void SetGolAnimEnd(IMessage rMessage)
    {
        if ((int)rMessage.Data == 0)
        {
            bIsAnimEnd = false;
        }
    }

    public void GolHitTrigger(IMessage rMessage)
    {
        AnimNormal();

        BossAnim.SetTrigger(HitTrigger);
    }


    public void BossStartEffect()
    {
        StartEffect.SetActive(true);
    }

    public void SpawnedBee(IMessage rMessage)
    {
        if ((int)rMessage.Data == 0)
        {
            SpawnedBees++;
        }
        else
        {
            SpawnedBees--;
            if (SpawnedBees < 0)
            {
                SpawnedBees = 0;
            }
        }
    }

    public int GetBeeNum()
    {
        return SpawnedBees;
    }

    public void TimeOutBoss()
    {
        BossTimeOut = true;
        HP.SetMonsterHP(0);
    }


    /// <summary>
    /// Animation
    /// </summary>

    public void StopAnim()
    {
        BossAnim.speed = 0.01f;

        SpecialCol.SetActive(true);
        BossColider.SetActive(false);

        SpecialAttackColider.StartSpecialAttack();

        if (bIsIceGolem == false)
        {
            Effect.Start_FX_Attack_Point();
        }
    }

    public void PlayAnim()
    {
        SpecialCol.SetActive(false);
        BossColider.SetActive(true);

        if (bIsIceGolem == false)
        {
            Effect.End_FX_Attack_Point();
        }


        if (SpecialAttackClear == true)
        {
            BossAnim.SetInteger(hashPhase, (int)BossPhase.BlockCrashSucsess);

            Effect.Start_FX_Crack_Particle();
        }
        else
        {
            BossAnim.SetInteger(hashPhase, (int)BossPhase.BlockCrashFail);
        }

        BossAnim.speed = 1.0f;
    }

    public void AnimSlow()
    {
        BossAnim.speed = 0.3f;
    }

    public void AnimLandSlow()
    {
        BossAnim.speed = 0.5f;
    }

    public void AnimFast()
    {
        BossAnim.speed = 2.0f;
    }
    public void AnimNormal()
    {
        BossAnim.speed = 1.0f;
    }
}
