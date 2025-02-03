using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Sparcle : MonoBehaviour
{

    [SerializeField] ParticleSystem Sparkle;

    private void OnCollisionEnter(Collision collision)
    {
        Debug.Log("ParticlePlay");
        Sparkle.Play();
    }
}
