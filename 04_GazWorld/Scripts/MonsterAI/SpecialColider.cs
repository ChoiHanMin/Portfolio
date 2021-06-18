using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpecialColider : MonoBehaviour
{

    public int Count;
    int MyCount;
    private Queue<int> DmgCountQueue;
    int GetCount;
    public float SpecialTime;
    private Coroutine SpecialCoroutine;



    [Header("골렘==============================================")]
    public BossScript Boss;
    public SetGolColider GolColi;
    public bool WoodGol;



    [Header("지렁이============================================")]
    public WormMonster WormBoss;
    public bool bIsWorm;


    // Start is called before the first frame update
    void Start()
    {
        DmgCountQueue = new Queue<int>();
        SpecialCoroutine = null;
    }

    // Update is called once per frame
    void Update()
    {


        GetCount = GetTakeDmg();

        if (GetCount != 0)
        {
            MyCount -= GetCount;

            if (MyCount <= 0)
            {
                if (bIsWorm == false)
                {
                    Boss.SpecialAttackClear = true;
                    Boss.PlayAnim();

                    if (SpecialCoroutine != null)
                    {
                        StopCoroutine(SpecialCoroutine);
                    }
                    DmgCountQueue.Clear();
                }
                else
                {
                    WormBoss.SpecialAttackClear = true;
                    WormBoss.PlayAnim();

                    if (SpecialCoroutine != null)
                    {
                        StopCoroutine(SpecialCoroutine);
                    }
                    DmgCountQueue.Clear();
                }
            }
        }

    }
    public void StartSpecialAttack()
    {
        if (SpecialCoroutine == null)
        {
            SpecialCoroutine = StartCoroutine(CountStart());
        }
        else
        {
            StopCoroutine(SpecialCoroutine);
            SpecialCoroutine = StartCoroutine(CountStart());
        }
    }

    IEnumerator CountStart()
    {
        float CountDown = SpecialTime;
        MyCount = Count;

        while (CountDown != 0)
        {
            CountDown--;
            yield return new WaitForSeconds(1.0f);
        }

        if (bIsWorm == false)
        {
            Boss.SpecialAttackClear = false;
            Boss.PlayAnim();
            GolColi.WarnText();

            if (WoodGol == true)
            {
                GolColi.SetColiAnim();
            }
            DmgCountQueue.Clear();
        }
        else if (bIsWorm == true)
        {
            WormBoss.SpecialAttackClear = false;
            WormBoss.PlayAnim();
            DmgCountQueue.Clear();
        }

        yield return null;
    }


    public void PushDmg(int Dmg)
    {
        DmgCountQueue.Enqueue(Dmg);
    }

    int GetTakeDmg()
    {
        if (DmgCountQueue.Count > 0)
        {
            return DmgCountQueue.Dequeue();
        }
        else
        {
            return 0;
        }
    }
}
