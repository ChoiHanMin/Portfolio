using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class MonsterBase : MonoBehaviour
{
    //
    //Color값
    //

    public Color _blue = new Color(0f, 0f, 1f, 0.2f);
    public Color _red = new Color(1f, 0f, 0f, 0.2f);

    //
    //부채꼴 각도, 거리 값
    //

    public float angleRange = 45f;
    public float distance = 5f;
    public bool isCollision = false;

    public enum MonsterState
    {
        Born = 0,
        Idle = 1,
        Chase = 2,
        Attack = 3,
        Die = 4,
    }


    public enum MonsterType
    {
        Soldier = 10,
        Boss = 11
    }

    public abstract void Attack();
    public abstract void Idle();
    public abstract bool Chase(GameObject Player);
    public abstract void Move(GameObject Player);
    public abstract void Die();
    public abstract void SetMonsterState();
}