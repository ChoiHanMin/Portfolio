using System.Collections;
using System.Collections.Generic;
using Unity.Entities;
//using UnityEditor.SceneManagement;
using UnityEngine;
using static UnityEngine.EventSystems.EventTrigger;

//[GenerateAuthoringComponent]
//public struct MonsterECS : IComponentData
//{
//    public Entity Prefab;
//    public int Stage; // ���Ͱ� ���� �������� (1 �Ǵ� 2)
//}

public class MonsterAuthoring : MonoBehaviour
{
   // public GameObject Monster = null;
   // public int Stage; // ������ ��������
   //
   // public class MonsterAuthoringBaker : Baker<MonsterAuthoring>
   // {
   //     public override void Bake(MonsterAuthoring authoring)
   //     {
   //
   //         var data = new MonsterECS()
   //         {
   //             Prefab = GetEntity(authoring.gameObject, TransformUsageFlags.Dynamic),
   //             Stage = authoring.Stage
   //         };
   //
   //         AddComponent(GetEntity(TransformUsageFlags.Dynamic), data);
   //
   //
   //         // Entity ��������
   //         //var entity = GetEntity(TransformUsageFlags.Dynamic);
   //
   //
   //         //AddComponent(entity, default(MonsterECS));
   //
   //         // MonsterData ������Ʈ �߰� �� ������ ����
   //         //SetComponent(entity, new MonsterECS
   //         //{
   //         //   Stage = authoring.Stage
   //         //});
   //     }
   // }
}