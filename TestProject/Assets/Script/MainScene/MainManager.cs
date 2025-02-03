using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UniRx;


public class LoadScene : MonoBehaviour
{
    [SerializeField] Button StartButton;
    private int maxClicks=1;

    private void Start()
    {

        SoundMng.Instance.PlayMusic(SoundMng.Instance.BGM[0]);

       StartButton.onClick
             .AsObservable()
             .Take(maxClicks) // 최대 5번만 처리
             .Subscribe(_ =>
             {
                 SoundMng.Instance.PlaySound(SoundMng.Instance.UiClip);
                 SoundMng.Instance.PlayMusic(SoundMng.Instance.BGM[1]);
                 LoadingScript.Instance.LoadSceneAsync("GameScene");
             }, () =>
             {
                 Debug.Log("Max clicks reached.");
             })
             .AddTo(this);
    }

}
