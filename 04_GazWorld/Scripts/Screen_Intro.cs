using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DG.Tweening;
using UnityEngine.SceneManagement;
using Valve.VR;
using com.ootii.Messages;

public class Screen_Intro : MonoBehaviour
{
    private enum EGameState
    {
        Start = 0,
        Win = 1,
        Lose = 2,
    }

    public Texture[] MySprite;
    public Texture[] MyWinSprite;
    public Texture[] MyLoseSprite;

    private Material NowMat;
    public GameObject StartObject;
    public GameObject EndObject;
    public GameObject MyCamera;

    public DOTweenAnimation MyTween;
    private int GameState;
    public int NextMat;

    public TextType TypeText;
    public AudioManager BGMManager;
    public GameMng Gamemng;

    [Header("시뮬레이터")]
    public MoveSimulator Simmul;

    private MeshRenderer MyMeshRenderer;
    public GameObject WaitSub;

    [Header("점수")]
    public GameObject ScoreObj;
    public GameObject[] Star;
    public DOTweenAnimation ScoreText;
    public TextMesh ClearTitle;
    public TextMesh GetScore;
    private bool bIsClear = false;

    [Header("죽거나 클리어시 ")]
    public GameObject[] Monsters;
    public BeeInstantiate Bee;

    private void Awake()
    {
        MyMeshRenderer = this.gameObject.GetComponent<MeshRenderer>();
        NowMat = GetComponent<MeshRenderer>().material;
        NextMat = 0;
        MyTween = GetComponent<DOTweenAnimation>();
    }

    // Start is called before the first frame update
    void Start()
    {
        BGMManager.BaseAudioPlay(BGMManager.StayMusic);
    }

    private void Update()
    {
       
    }

    public void OffMonster()
    {
        for (int i = 0; i < Monsters.Length; i++)
        {
            if (Monsters[i])
            {
                Monsters[i].SetActive(false);
            }
        }
        Bee.StopInstantiate();
    }

   public void GameStart360()
    {
        Debug.LogError("360 Start");
        MyMeshRenderer.enabled = true;
        WaitSub.SetActive(false);

        TypeText.textmesh.text = "";
        BGMManager.BaseAudioPause(BGMManager.Start_Screen_BGM);
        GameState = (int)EGameState.Start;
        TypeText.TypeScenario();
        Gamemng.ResetViewPOint();

        NowMat.DOFade(1, 3).OnComplete<Tweener>(() => { NowMat.DOFade(0, 3).OnComplete<Tweener>(() => { NextMyMaterial(); this.gameObject.transform.rotation = Quaternion.Euler(0, -20, 0); }); });
        NowMat.DOFade(1, 3).OnComplete<Tweener>(() => { NowMat.DOFade(0, 3).OnComplete<Tweener>(() => { NextMyMaterial(); this.gameObject.transform.rotation = Quaternion.Euler(0, -35, 0); }); }).SetDelay<Tweener>(6);
        NowMat.DOFade(1, 3).OnComplete<Tweener>(() => { NowMat.DOFade(0, 3).OnComplete<Tweener>(() => { NextMyMaterial(); this.gameObject.transform.rotation = Quaternion.Euler(0, 0, 0); }); }).SetDelay<Tweener>(12);
        NowMat.DOFade(1, 3).OnComplete<Tweener>(() => { NowMat.DOFade(0, 3).OnComplete<Tweener>(() => { NextMyMaterial(); this.gameObject.transform.rotation = Quaternion.Euler(0, 90, 0); DotweenEnd(); }); }).SetDelay<Tweener>(18);
    }


    public void GameStart()
    {
        MyMeshRenderer.enabled = true;
        WaitSub.SetActive(false);

        TypeText.textmesh.text = "";
        BGMManager.BaseAudioPause(BGMManager.Start_Screen_BGM);
        GameState = (int)EGameState.Start;
        MyTween.DOPlay();
        TypeText.TypeScenario();
        Gamemng.ResetViewPOint();

    }

    public void NextMyMaterial()
    {

        switch (GameState)
        {
            case (int)EGameState.Start:
                if (NextMat < MySprite.Length && NextMat < 5)
                {
                    NowMat.SetTexture("_MainTex", MySprite[NextMat]);
                    GetComponent<MeshRenderer>().material = NowMat;
                }

                break;

            case (int)EGameState.Win:
                if (NextMat < MyWinSprite.Length && NextMat < 2)
                {
                    NowMat.SetTexture("_MainTex", MyWinSprite[NextMat]);
                    GetComponent<MeshRenderer>().material = NowMat;
                }
                else
                {
                    MyTween.DOKill();
                }
                break;

            case (int)EGameState.Lose:
                if (NextMat < MyLoseSprite.Length && NextMat < 1)
                {
                    NowMat.SetTexture("_MainTex", MyLoseSprite[NextMat]);
                    GetComponent<MeshRenderer>().material = NowMat;
                }
                else
                {
                    MyTween.DOKill();
                }
                break;
        }
        NextMat++;
    }

    public void DotweenEnd()
    {
        if (GameState == (int)EGameState.Start)
        {
            Gamemng.SetNextPoint();
        }

        if (GameState == (int)EGameState.Win)
        {
            Invoke("LoadLevel", 5.0f);
        }

        if (GameState == (int)EGameState.Lose)
        {
            Invoke("LoadLevel", 5.0f);
        }
    }

    public void GameWin()
    {
        bIsClear = true;
        Simmul.ValkyrieTest = false;
        NextMat = 0;
        GameState = (int)EGameState.Win;

        OffMonster();


        TypeText.TypeGameClear();

        NowMat.SetTexture("_MainTex", MyWinSprite[NextMat]);
          GetComponent<MeshRenderer>().material = NowMat;

        MyCamera.transform.SetParent(EndObject.transform);
        MyCamera.transform.position = EndObject.transform.position;
        MyCamera.transform.rotation = Quaternion.Euler(0, 0, 0);

        StartObject.transform.position = new Vector3(0, 2.13f, -17.27f);
        StartObject.transform.rotation = Quaternion.Euler(0, 0, 0);

        Gamemng.ResetViewPOint();

        //사운드
        BGMManager.BaseAudioPlay(BGMManager.Win_Screen_BGM);
        BGMManager.SetFirstPlay(false);

        this.gameObject.transform.rotation = Quaternion.Euler(0, -60, 0);
        NowMat.DOFade(1, 2).OnComplete<Tweener>(() => {  NowMat.DOFade(0, 2).OnComplete<Tweener>(() => { this.gameObject.transform.rotation = Quaternion.Euler(0, 180, 0); });  });

        NowMat.DOFade(1, 2).SetDelay(4.0f).OnStart<Tweener>(()=>{ NextMat++; NextMyMaterial(); }).OnComplete<Tweener>(() => { Invoke("GameEnd", 5.0f); });
    }

    public void GameLose()
    {
        bIsClear = false;
        Simmul.ValkyrieTest = false;
        NextMat = 0;
        GameState = (int)EGameState.Lose;

        OffMonster();

        Gamemng.ResetViewPOint();

        TypeText.TypeGameOver();

        NowMat.SetTexture("_MainTex", MyLoseSprite[NextMat]);
        GetComponent<MeshRenderer>().material = NowMat;

        MyCamera.transform.SetParent(EndObject.transform);
        MyCamera.transform.position = EndObject.transform.position;
        MyCamera.transform.rotation = Quaternion.Euler(0, 0, 0);

        StartObject.transform.position = new Vector3(0, 2.13f, -17.27f);
        StartObject.transform.rotation = Quaternion.Euler(0, 0, 0);

        //사운드
        BGMManager.BaseAudioPlay(BGMManager.Lose_Screen_BGM);
        BGMManager.SetFirstPlay(false);

        this.gameObject.transform.rotation = Quaternion.Euler(0, 195, 0);
        NowMat.DOFade(1, 2).OnComplete<Tweener>(() => { NowMat.DOFade(0, 2).OnComplete<Tweener>(() => { Invoke("GameEnd", 1.0f); }); });

    }

    private void GameEnd()
    {
        TypeText.textmesh.text = "";
        ScoreObj.SetActive(true);
        MyMeshRenderer.enabled = false;
        if (bIsClear)
        {
            ClearTitle.text = "대단해요";
            for (int i = 0; i < Star.Length; i++)
            {
                Star[i].SetActive(true);
            }
        }
        else
        {
            ClearTitle.text = "아쉬워요";
        }
        GetScore.text = Gamemng.GetScore().ToString();

        ScoreText.DOPlay();

        Invoke("LoadLevel", 10.0f);
    }


    public void LoadLevel()
    {
        Application.Quit();
    }
}
