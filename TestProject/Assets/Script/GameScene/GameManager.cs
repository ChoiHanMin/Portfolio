using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UniRx;

public class GameManager : MonoBehaviour, IPauseable
{

    public static GameManager inst;
    public CharacterScript character;
    public PauseManager PauseMng;
    public GameObject GuardObj;
    public GameObject MonsterObj;
    public GameObject BossMonsterObj;
    private int monsterSpawned;
    int TotalStage = 2;
    int CurrentStage = 0;

    [SerializeField,Space] Button DashButton;
    [SerializeField] Image PauseImage;
    [SerializeField] Button AttackButton;
    [SerializeField] Button PauseButton;

    [SerializeField] Button ResumeButton;
    [SerializeField] Button GameOverButton;
    [SerializeField] Button GameClearButton;
    [SerializeField] Button TitleButton;



    [SerializeField] GameObject[] MonsterSpawnPoints;

    private void Awake()
    {
        if (inst != null)
            return;

        inst = this;
    }

    // Start is called before the first frame update
    void Start()
    {
        DashButton.onClick.AsObservable().Subscribe(_ => { character.SetDash(); }).AddTo(this);
        AttackButton.onClick.AsObservable().Subscribe(_ =>
        {
            SoundMng.Instance.PlaySound(SoundMng.Instance.UiClip);
            character.PerformCombo();
        }).AddTo(this);

        PauseButton.onClick.AsObservable().Subscribe(_ =>
        {
            SoundMng.Instance.PlaySound(SoundMng.Instance.UiClip);
            PauseImage.gameObject.SetActive(true);
            PauseMng.PauseAll();
        }).AddTo(this);

        ResumeButton.onClick.AsObservable().Subscribe(_ =>
        {
            SoundMng.Instance.PlaySound(SoundMng.Instance.UiClip);
            PauseImage.gameObject.SetActive(false);
            PauseMng.ResumeAll();
        }).AddTo(this);

        GameOverButton.onClick.AsObservable().Subscribe(_ =>
        {
            SoundMng.Instance.PlaySound(SoundMng.Instance.UiClip);
            SoundMng.Instance.PlayMusic(SoundMng.Instance.BGM[0]);
            LoadingScript.Instance.LoadSceneAsync("TitleScene");
        }).AddTo(this);

        GameClearButton.onClick.AsObservable().Subscribe(_ =>
        {
            SoundMng.Instance.PlaySound(SoundMng.Instance.UiClip);
            SoundMng.Instance.PlayMusic(SoundMng.Instance.BGM[0]);
            LoadingScript.Instance.LoadSceneAsync("TitleScene");
        }).AddTo(this);

        TitleButton.onClick.AsObservable().Subscribe(_ =>
        {
            SoundMng.Instance.PlaySound(SoundMng.Instance.UiClip);
            SoundMng.Instance.PlayMusic(SoundMng.Instance.BGM[0]);
           LoadingScript.Instance.LoadSceneAsync("TitleScene");
            PauseMng.ResumeAll();
        }).AddTo(this);

    }

    private void Update()
    {
        if (isPaused)
            return;

        if (monsterSpawned <= 0 && CurrentStage < TotalStage)
        {
            Debug.Log("CurrentStage = " + CurrentStage);
            if (CurrentStage < TotalStage)
            {
                StartCoroutine(StartGame());
            }
            CurrentStage++;
        }

        if(CurrentStage >= TotalStage&& bIsStageClear)
        {
            GameClearButton.gameObject.SetActive(true);
        }
    }

    bool bIsStageClear = false;
    public void MonsterDie()
    {
        monsterSpawned--;
        if (monsterSpawned <= 0)
            bIsStageClear = true;
    }

    IEnumerator StartGame()
    {
        bIsStageClear = false;

        if (CurrentStage == TotalStage-1)
        {
            Instantiate(BossMonsterObj, MonsterSpawnPoints[0].transform).GetComponent<MonsterFSM>().SetMonster(true);
        }
        else
        {
            for (int i = 0; i < 2; i++)
            {
                int SpawnNum = Random.Range(0, MonsterSpawnPoints.Length);
                Debug.Log(SpawnNum);
                Instantiate(MonsterObj, MonsterSpawnPoints[SpawnNum].transform).GetComponent<MonsterFSM>().SetMonster(false);
                monsterSpawned++;
                yield return new WaitForSeconds(.5f);
            }
        }

        yield return null;

    }


    public void GameOver()
    {
        PauseMng.PauseAll();
        GameOverButton.gameObject.SetActive(true);
    }

    private bool isPaused = false;
    public void OnPause()
    {
        isPaused = true;
    }

    public void OnResume()
    {
        isPaused = false;
    }
}
