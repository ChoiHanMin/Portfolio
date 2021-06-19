using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using com.ootii.Messages;
using DG.Tweening;
using RenderHeads.Media.AVProVideo;

public class AudioManager : MonoBehaviour
{
    public AudioSource BaseAudio;
    public AudioSource ChangeAudio;
    public AudioSource EffcetAudio;

    [Header("BGM")]
    public AudioClip StayMusic;
    public AudioClip First_Stage_BGM;
    public AudioClip Second_Stage_BGM;
    public AudioClip Third_Stage_BGM;
    public AudioClip Forth_Stage_BGM;
    public AudioClip Character_Select_BGM;
    public AudioClip Start_Screen_BGM;
    public AudioClip Win_Screen_BGM;
    public AudioClip Lose_Screen_BGM;



    [Header("벌 등장")]
    public AudioClip Bee_Spawn_Sound;

    [Header("골렘 등장")]
    public AudioClip First_Golam_Spawn_Sound;
    public AudioClip Second_Golam_Spawn_Sound;
    public AudioClip Landing_Sound;

    [Header("지렁이 등장")]
    public AudioClip Worm_Appear_Disappear_Sound;

    [Header("골렘 죽음")]
    public AudioClip Golam_Down_Sound;

    [Header("지렁이 죽음")]
    public AudioClip Worm_Down_Sound;

    [Header("벌 공격")]
    public AudioClip Bee_Attak_Sound;

    [Header("골렘 공격")]
    public AudioClip Golam_FirstAttack_Sound;
    public AudioClip Golam_SecondAttack_Sound;
    public AudioClip Golam_ThirdAttack_Sound;
    public AudioClip Golam_StampAttack_Sound;


    [Header("지렁이 공격")]
    public AudioClip Worm_Attack_Sound;
    public AudioClip Worm_Breath_Sound;

    [Header("골렘 울부짖음")]
    public AudioClip Golem_Roar_Sound;

    [Header("플레이어 맞았을 시 사운드")]
    public AudioClip Player_Damaged_Sound;


    [Header("플레이어 공격 사운드")]
    public AudioClip Player_Attack_Sound;

    [Header("플레이어 아이템 사용 사운드")]
    public AudioClip Player_Heal_Sound;
    public AudioClip Player_Sheild_Sound;
    public AudioClip Player_Rocket_Sound;

    private bool bIsBaseAudioFirstPlay = false;
    private void Start()
    {
        // ChangeAudio.Play();
    }

    private void OnDestroy()
    {
    }

    private void Update()
    {

    }



    /// <summary>
    /// BGM
    /// </summary>

    public void SetFirstPlay(bool bPlay)
    {
        bIsBaseAudioFirstPlay = bPlay;
    }

    public void BaseAudioPause(AudioClip PlayAudio)
    {
        BaseAudio.DOFade(0, 5).OnComplete<Tweener>(() => { BaseAudio.Pause(); });
        ChangeAudio.DOFade(0.5f, 5).OnStart<Tweener>(() => { ChangeAudio.clip = PlayAudio; ChangeAudio.Play(); });
    }

    public void BaseAudioPlay(AudioClip PlayAudio)
    {
        ChangeAudio.DOFade(0, 5).OnComplete<Tweener>(() => { ChangeAudio.Pause(); });
        BaseAudio.DOFade(0.5f, 5).OnStart<Tweener>(() => { BaseAudio.clip = PlayAudio; BaseAudio.Play(); });
    }

    public void BaseAudioUnPause(bool bIsFirst)
    {
        if (bIsFirst == false)
        {
            BaseAudio.Play();
            bIsBaseAudioFirstPlay = true;
        }
        else
        {
            BaseAudio.DOFade(1, 2).OnStart<Tweener>(() => { BaseAudio.UnPause(); });
        }
    }


    /// <summary>
    /// 몬스터 등장 사운드
    /// </summary>
    public void PlaySpawnBeeSound()
    {
        EffcetAudio.PlayOneShot(Bee_Spawn_Sound);
    }

    public void PlaySpawnFirstGolemSound()
    {

        EffcetAudio.PlayOneShot(First_Golam_Spawn_Sound);
    }

    public void PlaySpawnSecondGolemSound()
    {

        EffcetAudio.PlayOneShot(Second_Golam_Spawn_Sound);
    }

    public void PlaySpawnWormSound()
    {

        EffcetAudio.PlayOneShot(Worm_Appear_Disappear_Sound);
    }

    public void PlayRoarGolemSound()
    {

        EffcetAudio.PlayOneShot(Golem_Roar_Sound);
    }

    public void GolemLandingSound()
    {

        EffcetAudio.PlayOneShot(Landing_Sound);
    }
    


    /// <summary>
    /// 몬스터 죽음 사운드
    /// </summary>
    /// 

    public void PlayDieGolemSound()
    {

        EffcetAudio.PlayOneShot(Golam_Down_Sound);
    }


    public void PlayDieWormSound()
    {

        EffcetAudio.PlayOneShot(Worm_Down_Sound);
    }



    /// <summary>
    /// 몬스터 공격 사운드
    /// </summary>


    public void PlayAttackBeeSound()
    {

        EffcetAudio.PlayOneShot(Bee_Attak_Sound);
    }

    public void PlayFirstAttackGolemSound()
    {

        EffcetAudio.PlayOneShot(Golam_FirstAttack_Sound);
    }

    public void PlaySecondAttackGolemSound()
    {

        EffcetAudio.PlayOneShot(Golam_SecondAttack_Sound);
    }

    public void PlayThirdAttackGolemSound()
    {

        EffcetAudio.PlayOneShot(Golam_ThirdAttack_Sound);
    }

    public void PlayStampAttackGolemSound()
    {

        EffcetAudio.PlayOneShot(Golam_StampAttack_Sound);
    }

    public void PlayAttackWormSound()
    {

        EffcetAudio.PlayOneShot(Worm_Attack_Sound);
    }

    public void PlayBreathWormSound()
    {

        EffcetAudio.PlayOneShot(Worm_Breath_Sound);
    }

    /// <summary>
    /// 플레이어 맞았을 때 사운드
    /// </summary>
    public void PlayerAttackedSound()
    {

        EffcetAudio.volume = 0.25f;

        EffcetAudio.PlayOneShot(Player_Damaged_Sound);

        EffcetAudio.volume = 1.0f;
    }

    /// <summary>
    /// 플레이어 공격 사운드
    /// </summary>
    public void PlayerAttackSound()
    {
        EffcetAudio.volume = 0.35f;
        EffcetAudio.PlayOneShot(Player_Attack_Sound);
    }

    /// <summary>
    /// 플레이어 아이템 사용 사운드
    /// </summary>
    public void PlayerUseHeal()
    {
        EffcetAudio.PlayOneShot(Player_Heal_Sound);
    }

    public void PlayerUseShield()
    {
        EffcetAudio.PlayOneShot(Player_Sheild_Sound);
    }

    public void PlayerUseRocket()
    {
        EffcetAudio.PlayOneShot(Player_Rocket_Sound);
    }
}
