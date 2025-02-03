using System.Collections;
using System.Collections.Generic;
using Unity.Entities;
using Unity.Scenes;
using UnityEngine;
using Unity.Entities.Serialization;
using Unity.Collections;



//#if UNITY_EDITOR
//using UnityEditor;
//#endif
//
//public class SceneLoaderAuthoring : MonoBehaviour
//{
//    public SceneAsset Scene; // UnityEditor.SceneAsset으로 SubScene 참조
//}
//
//#if UNITY_EDITOR
//public class SceneLoaderBaker : Baker<SceneLoaderAuthoring>
//{
//    public override void Bake(SceneLoaderAuthoring authoring)
//    {
//        // Entity 생성
//        var entity = GetEntity(TransformUsageFlags.None);
//
//        // EntitySceneReference 생성 및 추가
//        var reference = new EntitySceneReference(authoring.Scene);
//        AddComponent(entity, new SceneLoader { SceneReference = reference });
//    }
//}
//#endif

// IComponentData로 런타임에 사용할 데이터 정의
public struct SceneLoader : IComponentData
{
    public EntitySceneReference SceneReference;
}

[RequireMatchingQueriesForUpdate]
public partial class SceneLoaderSystem : SystemBase
{
    private EntityQuery _sceneLoadQuery;

    protected override void OnCreate()
    {
        // SceneLoader 컴포넌트를 가진 엔티티 쿼리 생성
        _sceneLoadQuery = GetEntityQuery(typeof(SceneLoader));
    }

    protected override void OnUpdate()
    {
        // SceneLoader 데이터를 가져옴
        var loaders = _sceneLoadQuery.ToComponentDataArray<SceneLoader>(Allocator.Temp);

        foreach (var loader in loaders)
        {
            var world = World.Unmanaged;

            // EntitySceneReference로 SubScene 로드
            SceneSystem.LoadSceneAsync(world, loader.SceneReference);
            UnityEngine.Debug.Log($"SubScene with GUID {loader.SceneReference} loading initiated.");
        }

        loaders.Dispose();
    }
}

public class SceneLoad : MonoBehaviour
{

   
    public void UnloadSubScene(Unity.Entities.Hash128 sceneGuid)
    {
        var world = World.DefaultGameObjectInjectionWorld.Unmanaged;

        // GUID로 SubScene 언로드
        SceneSystem.UnloadScene(world, sceneGuid);
        Debug.Log("SubScene unloaded by GUID.");
    }
    public void LoadSubScene(Unity.Entities.Hash128 sceneGuid)
    {

        WorldUnmanaged worldUnmanaged = World.DefaultGameObjectInjectionWorld.Unmanaged;
        
        //   Debug.Log("SubScene loading initiated.");
    }
    void Update()
    {
        if (UnityEngine.Input.GetKeyDown(UnityEngine.KeyCode.L))
        {
        //    UnloadSubScene(subsceneGuid);

            UnityEngine.Debug.Log("SubScene UnLoaded!");
        }

        if (UnityEngine.Input.GetKeyDown(UnityEngine.KeyCode.K))
        {
           // LoadSubScene(subsceneGuid);

            UnityEngine.Debug.Log("SubScene Loaded!");
        }
    }
}
