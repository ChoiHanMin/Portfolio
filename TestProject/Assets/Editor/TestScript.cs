using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class TestScript : MonoBehaviour
{
    [MenuItem("Tools/Get SubScene GUID")]
    public static void GetSubSceneGUID()
    {
        // SubScene 파일을 선택해야 함
        var selectedObject = Selection.activeObject;

        if (selectedObject == null)
        {
            Debug.LogError("선택된 에셋이 없습니다. SubScene 파일을 선택하세요.");
            return;
        }

        // 선택된 에셋 경로 가져오기
        string assetPath = AssetDatabase.GetAssetPath(selectedObject);

        // GUID 가져오기
        string guid = AssetDatabase.AssetPathToGUID(assetPath);

        Debug.Log($"SubScene Path: {assetPath}");
        Debug.Log($"SubScene GUID: {guid}");
    }
}
