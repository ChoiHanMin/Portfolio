using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using com.ootii.Messages;

public class IceBossScript : MonsterBase
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
    }

    /// HP

    public MonsterHP HP;
    public GameObject BossColider;


    /// <summary>
    /// 애니메이션 
    /// </summary>

    public Animator BossAnim;

    readonly int hashIdle = Animator.StringToHash("State");
    readonly int hashPhase = Animator.StringToHash("Phase");
    readonly int DieTrigger = Animator.StringToHash("Die");
    readonly int StartTrigger = Animator.StringToHash("Start");
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

    [Header("죽었을 때 꺼주는 콜라이더")]
    public GameObject[] HitColliOff;


    [Header("보스몹 HP바")]
    public GameObject MonsterHPBar;

    public int SpawnedBees = 0;


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
    }

    private void OnDestroy()
    {
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
                Nav.velocity = Vector3.zero;
                Nav.isStopped = true;

                bIsMonsterDie = true;

                BossAnim.SetTrigger(DieTrigger);
                BossMonsterState = (int)MonsterState.Die;

                if (MyGameMng != null)
                {
                    MyGameMng.EndStage();
                }


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
                    Nav.velocity = Vector3.zero;
                    Nav.isStopped = true;
                    Idle();
                    break;

                case (int)MonsterState.Chase:
                    Nav.isStopped = false;

                    Move(PlayerOBJ);
                    break;

                case (int)MonsterState.Attack:
                    Nav.velocity = Vector3.zero;
                    Nav.isStopped = true;
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
    }

    public void BossWake()
    {
        HP.SetMonsterHP(400);

        BossAnim.SetTrigger(StartTrigger);
        BossColider.SetActive(true);
        BossStartEffect();

        AttackRange.SetActive(true);
        MonsterHPBar.SetActive(true);

        Vector3 direction = PlayerOBJ.transform.position - transform.position;
        Quaternion q = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));

        StartCoroutine("MonsterRotation", q);
        Invoke("SetNavTrue", 2.0f);


    }

    public void SetNavTrue()
    {
        Nav.enabled = true;
        BeeInit.StartInstantiate();
    }

    public int Randnum = 0;
    public override void Attack()
    {
        if (!bIsAnimEnd)
        {
            BossAnim.SetInteger(hashIdle, (int)MonsterState.Attack);

            Randnum = Random.Range(0, 5);
            Debug.Log(Randnum);

            CheckHPPercent(HP.CharHP);

            bIsAnimEnd = true;
        }
    }

    private void CheckHPPercent(float CurrentHP)
    {
        float HPPercent = CurrentHP / HP.MaxHP;
        Debug.Log("HP Percent : " + HPPercent);

        if (HPPercent >= FirstPhase || HPPercent == 1)
        {
            bIsAttacking = true;


            if (Randnum <= 2)
            {
                StartCoroutine("Combo_1");
            }
            else
            {
                StartCoroutine("Combo_2");
            }
        }
        else if (HPPercent >= SecondPhase && HPPercent < FirstPhase)
        {
            bIsAttacking = true;
            if (!bIsSecondPhase)
            {
                BossAnim.SetTrigger(Roar);
                bIsSecondPhase = true;
            }


            if (Randnum <= 1)
            {
                StartCoroutine("Combo_3");
            }
            else
            {
                StartCoroutine("Combo_2");
            }
        }
        else if (HPPercent >= LastPhase && HPPercent < SecondPhase)
        {
            bIsAttacking = true;

            if (!bIsThirdPhase)
            {
                BossAnim.SetTrigger(Roar);
                bIsThirdPhase = true;
            }
            if (Randnum % 2 == 1)
            {
                StartCoroutine("Combo_4");
            }
            else
            {
                StartCoroutine("Combo_1");
            }
        }
        else
        {

            Debug.Log("Last Phase");

            if (!bIsFinalPhase)
            {
                BossAnim.SetTrigger(Roar);
                bIsFinalPhase = true;
            }

            bIsAttacking = true;

            switch (Randnum)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                    StartCoroutine("Combo_5");
                    break;
                case 4:
                    StartCoroutine("Combo_2");
                    break;
                case 5:
                    StartCoroutine("Combo_4");
                    break;
            }
        }
    }

    private IEnumerator Combo_1()
    {
        BossNowPhase = (int)BossPhase.Phase_1;

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_1);

        bIsAttacking = false;
        yield return null;
    }

    private IEnumerator Combo_2()
    {
        BossNowPhase = (int)BossPhase.Phase_2;

        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_2);


        bIsAttacking = false;
        yield return null;
    }

    private IEnumerator Combo_3()
    {
        BossNowPhase = (int)BossPhase.Phase_3;


        BossAnim.SetInteger(hashPhase, (int)BossPhase.Phase_3);

        bIsAttacking = false;
        yield return null;
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
        BossAnim.SetInteger(hashIdle, (int)MonsterState.Chase);
        if (Player != null)
        {
            Nav.SetDestination(Player.transform.position);
        }
    }

    public override void Die()
    {
        for (int i = 0; i < HitColliOff.Length; i++)
        {
            HitColliOff[i].SetActive(false);
        }


        MonsterHPBar.SetActive(false);
        MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
        MessageDispatcher.SendMessage(this, "ScorePlus", 1, 0);

        SpawnedBees = 0;
        BeeInit.StopInstantiate();

        Destroy(this);

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

    public void BossStartEffect()
    {
        StartEffect.SetActive(true);
    }

    public void SpawnedBee(IMessage rMessage)
    {
        if((int)rMessage.Data==0)
        {
            SpawnedBees++;
        }
        else
        {
            SpawnedBees--;
        }
    }

    public int GetBeeNum()
    {
        return SpawnedBees;
    }
  
}
