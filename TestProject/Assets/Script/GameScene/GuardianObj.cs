using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GuardianObj : MonoBehaviour
{
    private int FullHP;
    private int CurrentHP;
    private bool bIsDead;

    [SerializeField] Slider GuardianHP;

    private void Start()
    {
        FullHP = 500;
        CurrentHP = FullHP;
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag.Equals("Monster"))
        {
            Debug.Log("Guardian");
            SetDamage();

            if (bIsDead)
            {
                GameManager.inst.GameOver();
                return;
            }

        }
    }

    void SetDamage()
    {
        CurrentHP -= 10;
        float hpValue = (float)CurrentHP / FullHP;
        
        GuardianHP.value = hpValue;

        if (CurrentHP <= 0)
        {
            bIsDead = true;
        }
        else
        {
            bIsDead = false;
        }
    }
}
