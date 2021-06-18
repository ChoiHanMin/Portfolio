using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using com.ootii.Messages;

public class StartAnim : StateMachineBehaviour
{

    // OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    {
        if (stateInfo.IsName("HitNode"))
        {
            MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
        }

        if (stateInfo.IsName("GotHitBody"))
        {
            MessageDispatcher.SendMessage(this, "SetWormAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetWormAnimEnd", 0, 0);
        }
        
    }

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    //override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
        
    //}

    // OnStateExit is called when a transition ends and the state machine finishes evaluating this state
    //override public void OnStateExit(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    
    //}

    // OnStateMove is called right after Animator.OnAnimatorMove()
    //override public void OnStateMove(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    // Implement code that processes and affects root motion
    //}

    // OnStateIK is called right after Animator.OnAnimatorIK()
    //override public void OnStateIK(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    // Implement code that sets up animation IK (inverse kinematics)
    //}
}
