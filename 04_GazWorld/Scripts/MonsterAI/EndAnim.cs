using com.ootii.Messages;
using UnityEngine;

public class EndAnim : StateMachineBehaviour
{


    //OnStateEnter is called when a transition starts and the state machine starts to evaluate this state
    //override public void OnStateEnter(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //}

    // OnStateUpdate is called on each Update frame between OnStateEnter and OnStateExit callbacks
    //override public void OnStateUpdate(Animator animator, AnimatorStateInfo stateInfo, int layerIndex)
    //{
    //    
    //}

    // OnStateExit is called when a transition ends and the state machine finishes evaluating this state
    //OnStateExit
    override public void OnStateExit(Animator animator, AnimatorStateInfo stateinfo, int layerindex)
    {
        //        Debug.Log("AnimEnd");

        if (stateinfo.IsName("G_Idle"))
        {
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
            //Debug.Log("Golem Phase1_End");
        }
        
        if (stateinfo.IsName("G_Phase1_End"))
        {
            MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
            //Debug.Log("Golem Phase1_End");
        }

        if (stateinfo.IsName("G_Phase2_End"))
        {
            MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
            //Debug.Log("Golem Phase2_End");
        }

        if (stateinfo.IsName("G_Phase3_End"))
        {
            MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
            //Debug.Log("Golem Phase3_End");
        }

        if (stateinfo.IsName("G_Phase4_End"))
        {
            MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
            //Debug.Log("Golem Phase4_End");
        }

        if (stateinfo.IsName("G_Phase5_End"))
        {
            MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
            //Debug.Log("Golem Phase5_End");
        }

        if (stateinfo.IsName("GWoodBorn"))
        {
            MessageDispatcher.SendMessage(this, "SetWoodNavTrue", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
        }

        if (stateinfo.IsName("HitNode"))
        {
            MessageDispatcher.SendMessage(this, "SetAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetGolAnimEnd", 0, 0);
            MessageDispatcher.SendMessage(this, "LookAtPlayer", 0, 0);
        }

        
        ///
        ///지렁이 페이즈 앤드
        ///

        if (stateinfo.IsName("Worm_Phase1_End"))
        {
            MessageDispatcher.SendMessage(this, "SetWormAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetWormAnimEnd", 0, 0);
        }

        if (stateinfo.IsName("Worm_Phase2_End"))
        {
            MessageDispatcher.SendMessage(this, "SetWormAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetWormAnimEnd", 0, 0);
        }

        if (stateinfo.IsName("Worm_Phase3_End"))
        {
            MessageDispatcher.SendMessage(this, "SetWormAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetWormAnimEnd", 0, 0);

            MessageDispatcher.SendMessage(this, "SetBreathEnd", 0, 0);
        }

        if (stateinfo.IsName("Worm_Cancel"))
        {
            MessageDispatcher.SendMessage(this, "SetWormAllColiEnable", 0, 0);
            MessageDispatcher.SendMessage(this, "SetWormAnimEnd", 0, 0);
        }
    }

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
