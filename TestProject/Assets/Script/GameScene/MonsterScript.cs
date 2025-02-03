using System.Collections;
using System.Collections.Generic;
using UniRx.Triggers;
using UnityEngine;

public class MonsterScript : MonoBehaviour
{

    [SerializeField] Animator MonsterAnim;
    [SerializeField] CapsuleCollider Colider;
    GameObject Target;

    private int HP = 0;
    private bool bIsDead;

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag.Equals("Player"))
        {
            Debug.Log("Hit");
            SetDamage();

            if(bIsDead)
            {
                MonsterAnim.SetTrigger("Death");
                Colider.enabled = false;
                return;
            }

        }
    }


    public void SetMonster(bool bIsBoss)
    {
        if (bIsBoss)
        {
            HP = 140;
        }
        else
        {
            HP = 50;
        }

        Target = GameManager.inst.GuardObj;
    }


    void SetDamage()
    {
        HP -= 10;

        if(HP<=0)
        {
            bIsDead = true;
        }
        else
        {
            bIsDead = false;
        }
    }


    void MonsterAttack()
    {

    }

  
}
