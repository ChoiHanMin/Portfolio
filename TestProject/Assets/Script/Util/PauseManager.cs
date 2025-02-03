using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public interface IPauseable
{
    void OnPause();
    void OnResume();
}

public class PauseManager : MonoBehaviour
{
    public void PauseAll()
    {
        foreach (var pauseable in FindObjectsOfType<MonoBehaviour>().OfType<IPauseable>())
        {
            pauseable.OnPause();
        }
    }

    public void ResumeAll()
    {
        foreach (var pauseable in FindObjectsOfType<MonoBehaviour>().OfType<IPauseable>())
        {
            pauseable.OnResume();
        }
    }
}
