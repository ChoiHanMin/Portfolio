using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SoundMng : MonoBehaviour
{
    public static SoundMng Instance = null;
    [SerializeField] AudioSource _musicSource, _effectsSource;

    [Space,Header("Sounds")] public AudioClip[] BGM;
    public AudioClip UiClip;
    public AudioClip[] HitSounds;

    public AudioSource MusicScource
    {
        get { return _musicSource; }
    }

    public AudioSource EffectScource
    {
        get { return _effectsSource; }
    }

    const float delay = .1f;
    float effectVolume = 1;
    float musicVolume = 1;
    float playtime;

    private void Awake()
    {
        Screen.sleepTimeout = SleepTimeout.NeverSleep;
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

        musicVolume = PlayerPrefs.GetFloat("MUSIC_V", .5f);
        effectVolume = PlayerPrefs.GetFloat("EFFECT_V", 1f);
        ChangeMusicVolume(musicVolume);
        ChangeEffectVolume(effectVolume);
    }

    void ChangeMusicVolume(float v)
    {
        _musicSource.volume = v;
    }

    void ChangeEffectVolume(float v)
    {
        _effectsSource.volume = v;
    }

    public void SaveMusicVolume(float v)
    {
        PlayerPrefs.SetFloat("MUSIC_V", v);
        musicVolume = v;
    }

    public void SaveEffectVolume(float v)
    {
        PlayerPrefs.SetFloat("EFFECT_V", v);
        effectVolume = v;
    }


    public void PlaySound(AudioClip clip)
    {
        if (playtime + delay > Time.time && _effectsSource.clip == clip)
        {
            return;
        }

        _effectsSource.clip = clip;
        _effectsSource.PlayOneShot(clip, effectVolume);
        playtime = Time.time;
    }

    public void StopMusic(Action _action = null)
    {
        StartCoroutine(StopMusicCrt(_action));
    }

    IEnumerator StopMusicCrt(Action _action = null)
    {
        yield return MusicFadeInOut(true);
        _musicSource.Stop();
        _action?.Invoke();
    }

    IEnumerator MusicFadeInOut(bool _fadein)
    {
        if (_fadein)
        {
            float v = 1f;
            while (v > 0)
            {
                v -= Time.deltaTime * 2;
                _musicSource.volume = v * musicVolume;
                yield return null;
            }
        }
        else
        {
            float v = 0;
            while (v <= 1f)
            {
                v += Time.deltaTime * 2;
                _musicSource.volume = v * musicVolume;
                yield return null;
            }
        }
    }

    public void PlayMusic(AudioClip clip)
    {
        _musicSource.clip = clip;
        _musicSource.loop = true;
        _musicSource.volume = musicVolume;
        _musicSource.Play();
        StartCoroutine(PlayMusicFadeInOut());
    }

    IEnumerator PlayMusicFadeInOut()
    {
        //yield return MusicFadeInOut(true);
        yield return MusicFadeInOut(false);
    }

    public void ChangeMasterVolume(float value)
    {
        AudioListener.volume = value;
    }

    public void ToggleEffects()
    {
        _effectsSource.mute = !_effectsSource.mute;
    }

    public void ToggleMusic()
    {
        _musicSource.mute = !_musicSource.mute;
    }
}

