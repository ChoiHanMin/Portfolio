using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Item : MonoBehaviour
{
    public enum ItemTypes
    {
        Health,
        Sheld,
        Missile,
        None
    }
    [SerializeField]
    private ItemTypes _itemType;

    private string _itemName;
    private bool _isTriggerEnter = false;

    private object _triggerLock = new object() { };
    private void Awake()
    {
        switch (_itemType)
        {
            case ItemTypes.Health:
                _itemName = "HealthItem";
                break;
            case ItemTypes.Sheld:
                _itemName = "SheldItem";
                break;
            case ItemTypes.Missile:
                _itemName = "MissileItem";
                break;
        }
    }

    /// <summary>
    /// 아이템 타입을 데려온다
    /// </summary>
    /// <returns>아이템 타입</returns>
    public ItemTypes GetItemType()
    {
        return _itemType;
    }

    /// <summary>
    /// 아이템을 먹은 뒤인지 확인
    /// </summary>
    /// <returns>true : 이미 다른 사람이 아이템을 먹음 // false : 아직 아무도 아이템을 먹지못함</returns>
    public bool GetIsTriggerEnter()
    {
        lock(_triggerLock)
        {
            if (_isTriggerEnter)
            {
                return true;
            }

            _isTriggerEnter = true;
            return false;
        }
    }

   
}

