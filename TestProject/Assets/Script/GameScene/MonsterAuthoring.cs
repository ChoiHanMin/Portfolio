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
//    public int Stage; // 몬스터가 속한 스테이지 (1 또는 2)
//}

public class MonsterAuthoring : MonoBehaviour
{
   // public GameObject Monster = null;
   // public int Stage; // 몬스터의 스테이지
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
   //         // Entity 가져오기
   //         //var entity = GetEntity(TransformUsageFlags.Dynamic);
   //
   //
   //         //AddComponent(entity, default(MonsterECS));
   //
   //         // MonsterData 컴포넌트 추가 및 데이터 설정
   //         //SetComponent(entity, new MonsterECS
   //         //{
   //         //   Stage = authoring.Stage
   //         //});
   //     }
   // }
}