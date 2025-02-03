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
//    public SceneAsset Scene; // UnityEditor.SceneAsset���� SubScene ����
//}
//
//#if UNITY_EDITOR
//public class SceneLoaderBaker : Baker<SceneLoaderAuthoring>
//{
//    public override void Bake(SceneLoaderAuthoring authoring)
//    {
//        // Entity ����
//        var entity = GetEntity(TransformUsageFlags.None);
//
//        // EntitySceneReference ���� �� �߰�
//        var reference = new EntitySceneReference(authoring.Scene);
//        AddComponent(entity, new SceneLoader { SceneReference = reference });
//    }
//}
//#endif

// IComponentData�� ��Ÿ�ӿ� ����� ������ ����
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
        // SceneLoader ������Ʈ�� ���� ��ƼƼ ���� ����
        _sceneLoadQuery = GetEntityQuery(typeof(SceneLoader));
    }

    protected override void OnUpdate()
    {
        // SceneLoader �����͸� ������
        var loaders = _sceneLoadQuery.ToComponentDataArray<SceneLoader>(Allocator.Temp);

        foreach (var loader in loaders)
        {
            var world = World.Unmanaged;

            // EntitySceneReference�� SubScene �ε�
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

        // GUID�� SubScene ��ε�
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
