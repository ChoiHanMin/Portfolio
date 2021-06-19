using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using com.ootii.Messages;
using Valve.VR;

public class GameMng : MonoBehaviour
{
    public GameObject[] Boss;
    public GameObject Bee;
    public BossScript[] NowBoss;

    public GameObject[] StageBlock;
    public GameObject[] StageStart;

    public int Stage = 0;

    public Screen_Intro GameEnd;

    public AudioManager SoundMNG;

    private int Score;

    public SoliderCount Count;
    [Header("움직일 포인트")]
    public GameObject[] MovePoint;

    [Header("움직이는 오브젝트")]
    public GameObject MoveObject;
    public GameObject TimeUI;
    public GameObject Obj;

    [Header("바라 볼 객체 넣어줄 곳")]
    public MoveSimulator Simmul;
    public GameObject FirstBossPoint;
    public GameObject SecondBossPoint;
    public GameObject ThirdBossPoint;


    [Header("페이드 인아웃")]
    public FadeInOut_2 Fade;

    [Header("스코어 텍스트")]
    public SetScore score;
    public int Testint;

    private float allTime;
    private bool bIsGameover = false;

    [Header("테스트")]
    public bool VRTest;

    public bool bIsSpawn;

    //public SetTime TimeText;
    public Text Time;
    public int TimeCount;
    bool bIsTimeOut;

    private Coroutine TimeOutCoroutine;

    [Header("벌 소환 위치")]
    public BeeInstantiate BeeInit;

    private void Start()
    {
        NowBoss = new BossScript[Boss.Length];
        Score = 0;
        TimeOutCoroutine = null;

        for (int i = 0; i < Boss.Length; i++)
        {
            NowBoss[i] = Boss[i].GetComponent<BossScript>();
        }

        if (StageStart[Stage] != null)
        {
            StageStart[Stage].SetActive(true);
        }

        MessageDispatcher.AddListener("StartStage", StartStage);
        //MessageDispatcher.AddListener("EndStage", EndStage);
        MessageDispatcher.AddListener("ScorePlus", PlusScore);

    }

    private void OnDestroy()
    {
        MessageDispatcher.RemoveListener("ScorePlus", PlusScore);
        MessageDispatcher.RemoveListener("StartStage", StartStage);
        //MessageDispatcher.RemoveListener("EndStage", EndStage);
    }

    private void Update()
    {
        //if (Input.GetKeyDown(KeyCode.B))
        //{
        //    if (TimeOutCoroutine != null)
        //    {
        //        StopTimeCoroutine();
        //    }
        //    StartCoroutine(MoveCamera());
        //}

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            Application.Quit();
        }
    }

    // 몬스터를 다 잡고 발동되는 함수
    public void SetNextPoint()
    {
        StartCoroutine(MoveCamera());
    }

    // 카메라 페이드 인/아웃 하면서 움직이는 함수
    private IEnumerator MoveCamera()
    {
        if (Stage < MovePoint.Length)
        {
            //MessageDispatcher.SendMessage(this, "FadeIn", 0, 0);
            Fade.FadeIn();

            yield return new WaitForSeconds(0.5f);

            MoveObject.transform.position = MovePoint[Stage].transform.position;

            if (!Obj.activeSelf)
            {
                Obj.SetActive(true);
                SetLookObj(Bee);

                // MoveObject.transform.rotation = Quaternion.Euler(0, 90, 0);

                SoundMNG.BaseAudioPlay(SoundMNG.First_Stage_BGM);
                MoveObject.transform.LookAt(Bee.transform);
                Time.text = TimeCount.ToString();


                ResetViewPOint();

                SetGameOver(true);
            }

            switch (Stage)
            {
                case 1:

                    Bee.SetActive(false);

                    SetLookObj(FirstBossPoint);
                    MoveObject.transform.LookAt(FirstBossPoint.transform);
                    SoundMNG.BaseAudioPause(SoundMNG.Second_Stage_BGM);
                    ResetViewPOint();
                    break;

                case 2:
                    SetLookObj(SecondBossPoint);
                    MoveObject.transform.LookAt(SecondBossPoint.transform);
                    SoundMNG.BaseAudioPlay(SoundMNG.Third_Stage_BGM);
                    ResetViewPOint();
                    break;

                case 3:
                    SetLookObj(ThirdBossPoint);
                    MoveObject.transform.LookAt(ThirdBossPoint.transform);
                    SoundMNG.BaseAudioPause(SoundMNG.Forth_Stage_BGM);
                    ResetViewPOint();
                    break;
            }



            yield return new WaitForSeconds(0.5f);

            //MessageDispatcher.SendMessage(this, "FadeOut", 0, 0);
            Fade.Fadeout();

            yield return new WaitForSeconds(1.0f);
            //Stage++;

            MessageDispatcher.SendMessage(this, "ShowGuide", Stage, 0);

        }
    }

    public void ResetViewPOint()
    {
        if (VRTest == false)
        {
            Valve.VR.OpenVR.Chaperone.ResetZeroPose(ETrackingUniverseOrigin.TrackingUniverseStanding);
        }
    }

    public void SetGameOver(bool bIsOver)
    {
        bIsGameover = bIsOver;
    }

    public int GetStageNum()
    {
        return Stage - 2;
    }

    void SetLookObj(GameObject Obj)
    {
        Simmul.LookAtOjb = Obj;
    }

    public void StartStage(IMessage rMessage)
    {

        for (int i = 0; i < StageBlock.Length; i++)
        {
            if (StageBlock[i] != null)
            {
                StageBlock[i].SetActive(true);
            }
        }


        if (Stage > 0)
        {
            if (NowBoss[Stage - 1] != null)
            {
                Count.ClearParticleList();
                bIsTimeOut = false;
                NowBoss[Stage - 1].AttackRange.transform.localScale = new Vector3(0.01f, 0.01f, 0.01f);
                BossAwake(Stage - 1);
            }
            else
            {
                Boss[Stage - 1].SetActive(true);
            }
        }
        else
        {
            TimeOutCoroutine = StartCoroutine(StageTime());
            //Bee.SetActive(true);
            MessageDispatcher.SendMessage(this, "StartBeeCorutine", 0, 0);
            SetLookObj(Bee);
            if (bIsSpawn)
            {
                BeeInit.StartInstantiate();
            }

        }

        //사운드

        switch (Stage)
        {
            case 0:
                SoundMNG.PlaySpawnBeeSound();
                break;
            case 1:
                // SoundMNG.PlaySpawnFirstGolemSound();
                break;
            case 2:
                SoundMNG.PlaySpawnSecondGolemSound();
                break;
            case 3:
                SoundMNG.PlaySpawnWormSound();
                break;
        }

       // StageStart[Stage].SetActive(false);

        ++Stage;

        // 발키리 켜줌
        if (Simmul.EngineTest == false)
        {
            Simmul.ValkyrieTest = true;
        }
    }

    public void EndStage()
    {
        for (int i = 0; i < StageBlock.Length; i++)
        {
            if (StageBlock[i] != null)
            {
                StageBlock[i].SetActive(false);
            }
        }

        if (bIsTimeOut)
        {

            if (Bee.activeSelf)
            {
                Bee.SetActive(false);
                BeeInit.StopInstantiate();
            }
            else
            {
                if (NowBoss[Stage - 2] != null)
                {
                    NowBoss[Stage - 2].TimeOutBoss();
                }
                else
                {
                    //Boss[Stage].SetActive(true);
                }
            }
        }

        Debug.Log("Stage : " + Stage);

        if ((Stage < 4)/* && (StageStart[Stage] != null)*/)
        {
           // StageStart[Stage].SetActive(true);

            //SetNextPoint();
            Invoke("SetNextPoint", 4.0f);
        }
        else
        {
            //if (bIsTimeOut)
            //{
            //    SetGameOver(false);
            //    GameEnd.GameLose();
            //}
            //else
            //{
            Invoke("GameClear", 5.0f);
            //}
        }

        //발키리 꺼줌
        if (Simmul.EngineTest == false)
        {
            Simmul.ValkyrieTest = false;
        }
    }

    public int GetScore()
    {
        return Score;
    }

    public void BossAwake(int BossNum)
    {
        NowBoss[BossNum].BossWake();
    }

    public void GameClear()
    {
        SetGameOver(false);
        GameEnd.GameWin();
    }

    public void PlusScore(IMessage rMessage)
    {
        switch ((int)rMessage.Data)
        {
            case 0:
                Score += 100;
                break;
            case 1:
                Score += 500;
                break;
            case 2:
                Score += 1000;
                break;
        }

        score.SetScoreText();
    }

    private IEnumerator StageTime()
    {
        bIsTimeOut = false;

        bool bTimeOut = false;
        int Timer = TimeCount;
        TimeUI.SetActive(true);
        if (Timer == 0)
        {
            yield return null;
        }
        else
        {
            while (bTimeOut == false)
            {
                Timer--;

                Time.text = Timer.ToString();

                yield return new WaitForSeconds(1.0f);

                if (Timer == 0)
                {
                    bTimeOut = true;
                    bIsTimeOut = true;
                }
            }

            MessageDispatcher.SendMessage(this, "FirstTimeUp", 0, 0);

            EndStage();
        }
        TimeUI.SetActive(false);
        yield return null;
    }

    public void StopTimeCoroutine()
    {

        if (TimeOutCoroutine != null)
        {
            StopCoroutine(TimeOutCoroutine);
        }
    }

}
