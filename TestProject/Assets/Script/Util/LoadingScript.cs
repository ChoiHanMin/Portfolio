using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using DG.Tweening;

public class LoadingScript : MonoBehaviour
{
    [SerializeField] Image LoadingImage;

    public static LoadingScript Instance;

    // Start is called before the first frame update
    void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            if (Instance != this)
                Destroy(gameObject);
        }
    }


    public void FadeIn()
    {
        LoadingImage.raycastTarget = true;
        LoadingImage.DOFade(1, 0.3f);
    }

    public void FadeOut()
    {
        LoadingImage.raycastTarget = false;
        LoadingImage.DOFade(0, 0.3f);
    }

    public void LoadSceneAsync(string sceneName)
    {
        FadeOut();
        // 비동기로 씬 로드
        StartCoroutine(LoadSceneCoroutine(sceneName));
    }

    private IEnumerator LoadSceneCoroutine(string sceneName)
    {
        AsyncOperation operation = SceneManager.LoadSceneAsync(sceneName);

        // 로딩 진행 상태 확인 (선택 사항)
        while (!operation.isDone)
        {
            Debug.Log($"Loading progress: {operation.progress * 100}%");
            yield return null;
        }

        Debug.Log("Scene loaded!");
        FadeOut();
    }

}
