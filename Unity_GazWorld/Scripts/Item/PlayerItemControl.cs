using BlackManager;
using Mirror;
using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Weapone;
using com.ootii.Messages;


public class PlayerItemControl : MonoBehaviour
{
    //권한 확인용
    [SerializeField, Header("네트워크 identity")]
    private NetworkIdentity _networkIdentity;

    [SerializeField, Header("아이템UI")]
    private GameObject _healthUIObj;
    [SerializeField]
    private GameObject _sheldUIObj;
    [SerializeField]
    private GameObject _missileUIObj;
    [SerializeField]
    private Transform[] _itemUITransform;
    [SerializeField]
    private Text _itemCountTxt;

    [SerializeField, Header("아이템사용")]
    private PlayerData _playerData;
    [SerializeField]
    private MissileLauncherCtrl _launcherCtrl;

    [SerializeField, Header("아이템")]
    private Item _healthItem;
    [SerializeField]
    private Item _shildeItem;
    [SerializeField]
    private Item _missileItem;
    [SerializeField]
    private float _itemCoolTime = 10f;

    [SerializeField, Header("아이템 파티클 효과")]
    private ParticleSystem Healing;
    [SerializeField]
    private ParticleSystem ShieldParticle;

    [SerializeField, Header("디버깅")]
    private bool _isDebug;

    [Header("인트로 끝났는지 확인")]
    public MoveSimulator Simul;
    [Header("BGM Mng")]
    public AudioManager SoundMng;

    private int _itemCount = 0;
    private Dictionary<int, GameObject> _itemList = new Dictionary<int, GameObject>();

    //버튼 클릭 이벤트 확인용
    private bool _onBtn1 = false;
    private bool _onBtn2 = false;
    private bool _onBtn3 = false;

    private JInput.Controller _controller;

    private void OnEnable()
    {
        //아이템 리스트 초기화
        foreach (KeyValuePair<int, GameObject> item in _itemList)
        {
            Destroy(item.Value);
        }
        _itemList.Clear();

        _itemCount = _itemList.Count;
        _itemCountTxt.text = _itemCount.ToString();

        Invoke("InitItems", 1f);
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag.Equals("Item"))
        {
            Debug.Log(other.GetComponent<Item>());
            if (!other.GetComponent<Item>().GetIsTriggerEnter())
            {
                AddItem(other.GetComponent<Item>());
            }
            other.gameObject.SetActive(false);
        }
    }

    private void Update()
    {

        //컨트롤러 받아오기
        if (_isDebug == false)
        {

            if (_controller == null)
            {
                _controller = JInput.InputManager.getController(0);
                return;
            }


            if (!_isDebug && Simul.ValkyrieTest == true)
            {

                if (_controller.data.buttons[2] != _onBtn1)
                {
                    _onBtn1 = _controller.data.buttons[2];

                    if (_onBtn1)
                    {
                        StartItemEvent(0);
                    }
                }

                if (_controller.data.buttons[3] != _onBtn2)
                {
                    _onBtn2 = _controller.data.buttons[3];

                    if (_onBtn2)
                    {
                        StartItemEvent(1);
                    }
                }

                if (_controller.data.buttons[4] != _onBtn3)
                {
                    _onBtn3 = _controller.data.buttons[4];

                    if (_onBtn3)
                    {
                        StartItemEvent(2);
                    }
                }


            }
        }
        else if (_isDebug == true)
        {
            if (Input.GetKeyDown(KeyCode.Alpha1))
            {
                StartItemEvent(0);
            }
            else if (Input.GetKeyDown(KeyCode.Alpha2))
            {
                StartItemEvent(1);
            }
            else if (Input.GetKeyDown(KeyCode.Alpha3))
            {
                Debug.Log("3==================");
                StartItemEvent(2);
            }
        }
    }

    /// <summary>
    /// 초기화
    /// </summary>
    private void InitItems()
    {
        AddItem(_healthItem);
        AddItem(_missileItem);
        AddItem(_shildeItem);
    }

    #region ItemUI

    /// <summary>
    /// 아이템을 추가한다
    /// </summary>
    /// <param name="item">아이템</param>
    private void AddItem(Item item)
    {
        //아이템의 갯수가 3개가 넘으면 return
        _itemCount = _itemList.Count;
        if (_itemCount >= 3)
        {
            return;
        }

        int itemNo = 9;
        switch (item.GetItemType())
        {
            case Item.ItemTypes.Health:
                itemNo = 0;
                break;
            case Item.ItemTypes.Sheld:
                itemNo = 1;
                break;
            case Item.ItemTypes.Missile:
                itemNo = 2;
                break;
        }

        if (!_itemList.ContainsKey(itemNo))
        {
            //Debug.Log("item GetType : " + item.GetItemType());
            GameObject itemObj = null;

            switch (item.GetItemType())
            {
                case Item.ItemTypes.Health:
                    itemObj = Instantiate(_healthUIObj, _itemUITransform[itemNo]);
                    break;
                case Item.ItemTypes.Sheld:
                    itemObj = Instantiate(_sheldUIObj, _itemUITransform[itemNo]);
                    break;
                case Item.ItemTypes.Missile:
                    itemObj = Instantiate(_missileUIObj, _itemUITransform[itemNo]);
                    break;
            }

            if (itemObj != null)
            {
                _itemList.Add(itemNo, itemObj);
                _itemCount = _itemList.Count;
                _itemCountTxt.text = _itemCount.ToString();
            }
            return;
        }
    }

    #endregion

    #region 아이템 발동
    /// <summary>
    /// 아이템별 이벤트를 발생시킨다
    /// </summary>
    private void StartItemEvent(int btnNo)
    {
        try
        {
            if (_itemList.ContainsKey(btnNo))
            {
                GameObject itemObj = null;
                if (_itemList.TryGetValue(btnNo, out itemObj))
                {
                    if (itemObj == null)
                        return;
                    Item item = itemObj.GetComponent<Item>();
                    switch (item.GetItemType())
                    {
                        case Item.ItemTypes.Health:
                            Debug.Log("아이템::체력 회복 아이템 사용");
                            _playerData.CharHP += 5;
                            Healing.Play();
                            SoundMng.PlayerUseHeal();
                            HPbar.instance.WasteHPBar();
                            break;

                        case Item.ItemTypes.Sheld:
                            Debug.Log("아이템::쉴드 아이템 사용");
                            SoundMng.PlayerUseShield();
                            StartCoroutine(StartShield());
                            break;

                        case Item.ItemTypes.Missile:
                            Debug.Log("아이템::미사일 아이템 사용");
                            SoundMng.PlayerUseRocket();
                            _launcherCtrl.MissileShoot();
                            break;
                    }
                    _itemList.Remove(btnNo);
                    Destroy(itemObj);

                    _itemCount = _itemList.Count;
                    _itemCountTxt.text = _itemCount.ToString();

                    StartCoroutine(ItemCoolTime(item));
                }
            }
        }
        catch (Exception e)
        {
            Debug.Log("아이템 :: " + e.Message);
        }

    }
    private IEnumerator StartShield()
    {
        MessageDispatcher.SendMessage(this, "SetShield", 1, 0);

        ShieldParticle.Play();

        Debug.Log("실드 On!!!!");

        yield return new WaitForSeconds(5.0f);

        MessageDispatcher.SendMessage(this, "SetShield", 0, 0);

        Debug.Log("실드 Off!!!!");

        yield return null;
    }
    private IEnumerator ItemCoolTime(Item item)
    {
        float cool = 10f;

        while (cool > 1.0f)
        {
            cool -= Time.deltaTime;
            yield return new WaitForFixedUpdate();
        }
        AddItem(item);
    }

    #endregion
}
