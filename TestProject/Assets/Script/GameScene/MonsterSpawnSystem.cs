using Unity.Entities;
using Unity.Transforms;
using Unity.Mathematics;
using UnityEngine;
using Unity.Collections;
//
//[UpdateInGroup(typeof(InitializationSystemGroup))]
//public partial struct SpawnSystem : ISystem
//{
//    public void OnCreate(ref SystemState state) => state.RequireForUpdate<MonsterECS>();
//
//    [GenerateTestsForBurstCompatibility]
//    public void OnUpdate(ref SystemState state)
//    {
//        var config = SystemAPI.GetSingleton<MonsterECS>();
//        var instances = state.EntityManager.Instantiate(config.Prefab, config.Stage,Allocator.Temp);
//        foreach (var entity in instances)
//        {
//            var xform = SystemAPI.GetComponentRW<LocalTransform>(entity);
//
//        }
//
//        state.Enabled = false;
//
//    }
//}



//public partial class MonsterSpawnSystem : SystemBase
//{
//    private Entity prefabEntity; // ���� ������ Entity
//    private int maxMonsters = 15; // �ִ� ���� ��
//
//    protected override void OnCreate()
//    {
//        base.OnCreate();
//
//        // Subscene���� ���� ������ ��������
//        var entityQuery = GetEntityQuery(ComponentType.ReadOnly<MonsterECS>());
//        prefabEntity = entityQuery.GetSingletonEntity();
//    }
//
//
//    protected override void OnUpdate()
//    {
//        if (prefabEntity == Entity.Null) return;
//
//        for (int i = 0; i < 15; i++)
//        {
//            Entity monsterEntity = EntityManager.Instantiate(prefabEntity);
//
//            //EntityManager.SetComponentData()
//            // ������ ����
//            EntityManager.SetComponentData(monsterEntity, new MonsterECS
//            {
//                Stage = UnityEngine.Random.Range(1, 3),
//            });
//
//            // ��ġ ����
//            float3 position = new float3(
//                UnityEngine.Random.Range(-10f, 10f),
//                0f,
//                UnityEngine.Random.Range(-10f, 10f)
//            );
//           // EntityManager.SetComponentData(monsterEntity, new Translation { Value = position });
//        }
//    }
//}

