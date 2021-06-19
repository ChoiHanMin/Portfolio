using JInput;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using Mirror;
using com.ootii.Messages;
using DG.Tweening;

public struct BasicInput
{
    public uint Roll;
    public uint Pitch;
    public uint Yaw;
    public uint Sway;
    public uint Surge;
    public uint Heave;
    public uint Delay;
    public uint Blower;
};

public struct ExtendedInput
{
    public uint Target;
    public uint Delay;
    public uint Mode;
};

public struct DeveloperInput
{
    public uint nDI;
};

public struct DeveloperOutput
{
    public uint nDO;
};

public struct OutputSection
{
    public uint Roll;
    public uint Pitch;
    public uint Yaw;
    public uint Sway;
    public uint Surge;
    public uint Heave;
    public uint RollEx;
    public uint PitchEx;
    public uint YawEx;
};

public struct ExtendedOutput
{
    public OutputSection Src;
    public OutputSection Dest;
    public OutputSection Encode;
};

public struct ExtractedOutput
{
    public uint Pitch;
    public uint Yaw;
};

public struct BodyAngle
{
    public float HeadYaw;
    public float HeadPitch;
    public float LeftLeg;
    public float RightLeg;
    public float y;
    public float sy;
    public float dash;
};

class AngleConverter
{
    //				DEVICE PITCH (Y) IN
    //
    //					20000					(top)
    //
    //						+			10000	(front)
    //
    //						0						(bottom)

    public static uint ToDeviceVer1(float degree)
    {
        float f = degree * 10000f / 90f + 10000f;
        return (uint)Mathf.Clamp(f, 0f, 20000f);
    }

    //			DEVICE PITCH (Y) OUT
    //
    //					 2500				(top)
    //
    //						+		1250	    (front)
    //
    //					    0					(bottom)

    //				Typical PITCH (Y)
    //
    //					   90					(top)
    //
    //						+			0	    (front)
    //
    //					  -90					(bottom)

    public static float ToTypicalVer1(uint pitch)
    {
        return ((float)pitch - 10000f) * 90f / 10000f;
    }

    //				DEVICE YAW (Z)
    //					(front)
    //
    //					18000
    //
    //		27000		+			9000
    //
    //						0

    public static uint ToDeviceVer2(double degree)
    {
        uint n = (uint)(54000 - (degree * 100));
        return n % 36000;
    }

    //				 TYPICAL  YAW (Z)
    //					(front)
    //
    //						0
    //
    //			-90		+			90
    //
    //					  180

    public static double ToTypicalVer2(uint yaw)
    {
        return (18000 - (int)yaw) * 0.01;
    }

    static float PitchRate = 0.5f;
    static float YawRate = 0.5f;

    public static float Repeat180(float t)
    {
        return Mathf.Repeat(t + 180, 360) - 180;
    }

    static float GetApproximateRPM(int delay)
    {
        delay = Mathf.Clamp(delay, 1, 0xff);
        return Mathf.Pow(0.42f, Mathf.Log(delay));
    }

    public static float ModifyOutputYaw(float next, int delay, float before)
    {
        float direction = Repeat180(next - before);
        float tickDistance = GetApproximateRPM(delay) * 0.125f * 360.0f / 6.8f;

        if (tickDistance < Mathf.Abs(direction))
            direction = Mathf.Sign(direction) * tickDistance;

        float r = before + YawRate * direction;

        return r;
    }

    public static float Fmod(float X, float Y)
    {
        return X - (int)(X / Y) * Y;
    }

    public static double Lerp(double A, double B, double Alpha)
    {
        return A * (1 - Alpha) + B * Alpha;
    }

    public static float LerpAngle(float A, float B, float Alpha)
    {
        // if mod < 180 then turn left, else then turn right
        float mod = Repeat180(B - A);
        B = A + mod;

        float interpolation = Mathf.Lerp(A, B, Alpha);
        return Repeat180(interpolation);
    }
};

class FAngleParam
{
    public double TargetDegree;
    public byte Delay;
    public byte Mode;

    public FAngleParam(double targetDegree = 0, float delay = 0xff)
    {
        TargetDegree = targetDegree;
        Delay = (byte)delay;
        Mode = 0;
    }

    static float LeftAndRightWeightSum(float leftPedal, float rightPedal)
    {
        return Mathf.Clamp(leftPedal + rightPedal, -1.0f, 1.0f);
    }

    static float GetDelay_Ver1(float weightRate, float delayToAdd)
    {
        return Mathf.Clamp(1.0f / Mathf.Abs(weightRate) + delayToAdd, 1.0f, 255.0f);
    }

    static float GetDelay_Ver2(float weightRate, float exponential, float delayToAdd)
    {
        float pow = Mathf.Pow(Mathf.Abs(weightRate), exponential);
        return Mathf.Clamp(255f - pow * 254f + delayToAdd, 1.0f, 255.0f);
    }

    public static FAngleParam MakeAngleParam(double currentDegree, float leftPedal, float rightPedal, float exponential, int delayToAdd, int ver)
    {
        if (leftPedal == 0.0f && rightPedal == 0.0f)
            return new FAngleParam(currentDegree);

        float weightRate = LeftAndRightWeightSum(leftPedal, rightPedal);
        double targetDegree = currentDegree + Mathf.Sign(weightRate) * 45.0f;

        float delayRate = 0.0f;
        switch (ver)
        {
            case -1:
                delayRate = GetDelay_Ver1(weightRate, delayToAdd);
                break;
            default:
                delayRate = GetDelay_Ver2(weightRate, exponential, delayToAdd);
                break;
        }
        return new FAngleParam(targetDegree, delayRate);
    }
};

public class MoveSimulator : MonoBehaviour
{
    [DllImport("AvSimDllMotionExternC")]
    private static extern int MotionControl__Initial();
    [DllImport("AvSimDllMotionExternC")]
    private static extern int MotionControl__Destroy();
    [DllImport("AvSimDllMotionExternC")]
    private static extern void MotionControl__DOF_and_Blower(int nRoll, int nPitch, int nYaw, int nSway, int nSurge, int nHeave, int nSpeed, int nBlower);
    [DllImport("AvSimDllMotionExternC")]
    private static extern void ExtendedUpdate(BasicInput[] basic, ExtendedInput[] rollEx, ExtendedInput[] pitchEx, ExtendedInput[] yawEx, DeveloperOutput[] dOutput, DeveloperInput[] dInput, ExtendedOutput[] exOutput, bool bResp = true);

    static BasicInput[] basic;
    static ExtendedInput[] rollEx;
    static ExtendedInput[] pitchEx;
    static ExtendedInput[] yawEx;
    static DeveloperOutput[] devOutput;
    static DeveloperInput[] devInput;
    static ExtractedOutput output;

    static bool enableThread = false;
    static Thread unsafeThread = null;

    private float renderedPitch = 0;
    private float renderedYaw = 0;
    private float renderedRoll = 0;
    float renderedPelvis = 0;
    int version = 2;

    private float rightPedalmoveY;
    private float leftPedalmoveX;
    private float PedalZ;

    JInput.Controller controller;

    BodyAngle bodyAngle = new BodyAngle();

    [SerializeField]
    private Camera cam;
    private Rigidbody myrigidbody;

    [SerializeField]
    private GameObject camObj;

    [SerializeField]
    private GameObject CockpitObj;

    [SerializeField]
    private float ForwardSpeed = 5f;
    [SerializeField]
    private float RotationSpeed = 5f;

    [SerializeField, Header("네트워크")]
    private NetworkIdentity _networkIdentity;

    // Pedal Component
    private bool LeftDash = false;
    private bool RightDash = false;

    // Damage Component
    private bool Damaged = false;
    public InterfaceAnimManager DamageWarning;

    // Valkyrie Movement Test
    public bool ValkyrieTest = false;

    // Tutorial Check
    public bool Tutorial = true;
    public int TutorialNum = 0;

    //
    public bool bIsConnectValkyrie = false;


    private float horizon;
    private float vertical;

    private bool bIsRightPedal = false;
    private bool bIsLeftPedal = false;

    public GameObject rotationobj;

    public GameMng gamemng;
    public AudioManager SoundMng;

    //public bool bIsblockHit = false;
    public bool bIsLeftblockHit = false;
    public bool bIsRightblockHit = false;

    [Header("바라보는방향")]
    public GameObject LookAtOjb;

    bool bIsReset = false;

    public DOTweenAnimation WarnSub;

    [Header("게임 시작")]

    public Screen_Intro GameStart;
    private bool bIsGameStart;

    public bool EngineTest = false;

    private void Awake()
    {
        if (MotionControl__Initial() != 0)
            print(" Walking device Init failed (Check device port or AvSimDllMotionExternC_Environment.ini) ");

        basic = new BasicInput[1];
        rollEx = new ExtendedInput[1];
        pitchEx = new ExtendedInput[1];
        yawEx = new ExtendedInput[1];
        devOutput = new DeveloperOutput[1];
        devInput = new DeveloperInput[1];
        output = new ExtractedOutput();

        myrigidbody = gameObject.GetComponent<Rigidbody>();

        unsafeThread = new Thread(Thread2_UpdateDevice);

        // Damage 관련 UI Component 가져오기
        //        DamageWarning = gameObject.transform.GetChild(0).GetChild(3).GetChild(0).GetComponent<InterfaceAnimManager>();
        //      DamageWarning.testLoop = false;

        // 데미지 효과 관련 Message Dispatcher
        MessageDispatcher.AddListener("DamageEffect_Simulator", DamageEffectHandler);

        // 튜토리얼 관련 Message Dispatcher
        MessageDispatcher.AddListener("MoveTutorial", TutorialCheck);

        MessageDispatcher.AddListener("SetWarnText", SetWarnText);


        //20201205 LIM 카메라 on off추가
        // Invoke("SetCameraOnOff", 0.2f);
    }

    //20201205 LIM 카메라 on off추가
    private void SetCameraOnOff()
    {

        Debug.Log(_networkIdentity.hasAuthority);
        if (_networkIdentity.hasAuthority)
            camObj.SetActive(true);
        else
            camObj.SetActive(false);

    }

    private void Start()
    {
        enableThread = true;
        //unsafeThread = new Thread(Thread2_UpdateDevice);
        unsafeThread.Start();

        bIsGameStart = false;
    }

    public Camera GetCam()
    {
        return cam;
    }

    private void reference()
    {
        controller = JInput.InputManager.getController(0);
        Debug.LogError("ReTry");
    }

    void OnDestroy()
    {
        MessageDispatcher.RemoveListener("SetWarnText", SetWarnText);
        MessageDispatcher.RemoveListener("MoveTutorial", TutorialCheck);
        MessageDispatcher.RemoveListener("DamageEffect_Simulator", DamageEffectHandler);
        MotionControl__Destroy();
        enableThread = false;
    }

    private void Update()
    {

        if (EngineTest == false)
        {
            if (controller == null)
            {
                bIsConnectValkyrie = false;
                reference();
                return;
            }
            else
            {
                bIsConnectValkyrie = true;
            }

            if (controller.data.buttons[0])
            {
                if (bIsGameStart == false)
                {
                    GameStart.GameStart360();
                    bIsGameStart = true;
                }

                
            }

            if(controller.data.buttons[5])
            {
                StartCoroutine(SetView());
            }

            if (ValkyrieTest == true && Tutorial == false)
            {
                MovePosition();
            }
        }
        else if (EngineTest == true)
        {
            if (ValkyrieTest == false && Tutorial == false)
            {
                MoveKeyboard();
            }
        }
        // Debug.Log("ValkyrieTest : " + ValkyrieTest + "  " + "Tutorial :  " + Tutorial);

        if (LookAtOjb != null)
        {
            rotationobj.transform.LookAt(LookAtOjb.transform);
        }
    }

    private void MoveKeyboard()
    {


        Vector3 dirforward = cam.transform.rotation * Vector3.forward;
        Vector3 dirright = cam.transform.rotation * Vector3.right;

        //Debug.Log("dirforward : " + dirforward + "  " + "dirright :  " + dirright);

        horizon = Input.GetAxisRaw("Horizontal");
        vertical = Input.GetAxisRaw("Vertical");


        if (Input.GetKey(KeyCode.A))
        {
            //float turn = horizon * RotationSpeed * 50;//* Time.deltaTime * 50;

            //rigidbody.rotation = rigidbody.rotation * Quaternion.Euler(0f, turn, 0f);

            // Vector3 dir = cam.transform.rotation * Vector3.forward;

            if (bIsLeftblockHit == false)
            {

                Vector3 move = horizon * dirright * ForwardSpeed * Time.deltaTime * 3;
                Vector3 move2 = new Vector3(move.x, 0, 0);

                //rigidbody.MovePosition(rigidbody.position + move2);
                transform.Translate(Vector3.right * horizon * ForwardSpeed * Time.deltaTime * 3);
            }
            //rigidbody.MovePosition(Vector3.right * horizon * ForwardSpeed * Time.deltaTime * 3);
        }

        if (Input.GetKey(KeyCode.D))
        {
            if (bIsRightblockHit == false)
            {

                Vector3 move = horizon * dirright * ForwardSpeed * Time.deltaTime * 3;
                Vector3 move2 = new Vector3(move.x, 0, 0);

                //rigidbody.MovePosition(rigidbody.position + move2);
                transform.Translate(Vector3.right * horizon * ForwardSpeed * Time.deltaTime * 3);
            }
        }

        if (Input.GetKey(KeyCode.E))
        {
            float turn = /*horizon */ -RotationSpeed;//* Time.deltaTime * 50;

            // rotationobj.transform.Rotate(0, RotationSpeed, 0);
        }

        if (Input.GetKey(KeyCode.C))
        {
            float turn =/* -horizon */ RotationSpeed;//* Time.deltaTime * 50;

            //rotationobj.transform.Rotate(0, -RotationSpeed, 0);
        }

        // rotationobj.transform.rotation = Quaternion.Euler(0, cam.transform.rotation.y*RotationSpeed, 0);


    }

    private void MoveFoward(float vertical, float horizon, bool bIsFowrdMove, bool bIsRightMove, bool bIsTurn)
    {

        //        Debug.Log("horizon : " + horizon + "   " + "vertical : " + vertical);

        Vector3 dirforward = cam.transform.rotation * Vector3.forward;
        Vector3 dirright = cam.transform.rotation * Vector3.right;

        if (bIsFowrdMove)
        {
            transform.Translate(rotationobj.transform.forward * vertical * ForwardSpeed * Time.deltaTime * 3, Space.Self);
        }

        if (bIsRightMove)
        {
            transform.Translate(Vector3.right * horizon * ForwardSpeed * Time.deltaTime);
        }

    }

    // Pedal Movement
    // 실질적으로 움직이는건 상위 객체 Only, 회전은 상위 객체 & 하위 콕핏 (기체 회전시 영점 뒤틀림 보정)
    private void MovePosition()
    {
        float left = 0, right = 0, side = 0;

        //  rotationobj.transform.rotation = Quaternion.Euler(cam.transform.rotation * Vector3.forward);


        rightPedalmoveY = controller.getAxisValue(ControllerVec2Axes.RStick).y;
        leftPedalmoveX = controller.getAxisValue(ControllerVec2Axes.RStick).x;

        PedalZ = controller.data.axesValues[4];

        horizon = Input.GetAxisRaw("JoyHorizontal");
        vertical = Input.GetAxisRaw("JoyVertical");


        if (bIsRightPedal == false && bIsLeftPedal == false)
        {
            switch (controller.data.hatSwitch)
            {
                case 2:
                case 3:
                case 4:
                case 5:
                    if (bIsRightblockHit == false)
                    {
                        if (renderedRoll <= 200)
                        {
                            renderedRoll += 2;
                        }
                        MoveFoward(vertical, horizon, false, true, false);
                    }

                    break;

                case 1:
                case 6:
                case 7:
                case 8:
                    if (bIsLeftblockHit == false)
                    {
                        if (renderedRoll >= -200)
                        {
                            renderedRoll -= 2;
                        }
                        horizon *= -1;
                        MoveFoward(vertical, horizon, false, true, false);
                    }

                    break;

                default:

                    break;
            }
        }

        Thread1_UpdateObject(PedalZ, left, right, side);
    }

    // 피격 효과 관련 함수
    // SendMessage("DamageEffect"); << 피격 효과 불러오는 방법
    public void DamageEffectHandler(IMessage rmessage)
    {
        Damaged = true;
        SoundMng.PlayerAttackedSound();
        StartCoroutine(DamageControl());
    }

    IEnumerator SetView()
    {
        if (bIsReset == false)
        {
            gamemng.ResetViewPOint();
            bIsReset = true;
            yield return new WaitForSeconds(1.0f);
            bIsReset = false;
        }

        yield return null;
    }

    IEnumerator DamageControl()
    {
        float ControlTime = 1.5f;
        float decrease = 0.1f;
        float Left = 0, Right = 0, Check = 1;

        // 경고 UI 활성화
        DamageWarning.currentState = CSFHIAnimableState.appearing;

        while (Damaged == true)
        {
            yield return new WaitForSeconds(decrease);

            Left = 0; Right = 0;
            Check += 1f;

            if (Check / 2 == 0)
                Right += 1f;
            else
                Left += 1f;

            //Thread1_UpdateObject(0, Left, Right, 1);

            ControlTime -= decrease;

            if (ControlTime <= 0.1f)
            {
                DamageWarning.currentState = CSFHIAnimableState.disappearing; // 경고 UI 비활성화
                Damaged = false;
                Check = 1;
            }
        }
    }

    //// 페달 좌, 우 대쉬 효과 함수
    //IEnumerator DashEffect(bool Check)
    //{
    //    float Dashtime = 0.5f;
    //    float decrease = 0.1f;
    //    float Right = 0, Left = 0;

    //    if (RightDash != false && Check != false)
    //    {
    //        while (RightDash == true)
    //        {
    //            yield return new WaitForSeconds(decrease);

    //            Right += 0.5f;
    //            Thread1_UpdateObject(0, 0, Right, 0);

    //            Dashtime -= decrease;

    //            if (Dashtime <= 0)
    //            {
    //                RightDash = false;
    //                rigidbody.velocity = new Vector3(0, 0, 0);
    //            }
    //        }
    //    }

    //    else if (LeftDash != false && Check == false)
    //    {
    //        while (LeftDash == true)
    //        {
    //            yield return new WaitForSeconds(decrease);

    //            Left += 0.5f;
    //            Thread1_UpdateObject(0, Left, 0, 0);

    //            Dashtime -= decrease;

    //            if (Dashtime <= 0.1f)
    //            {
    //                LeftDash = false;
    //                rigidbody.velocity = new Vector3(0, 0, 0);
    //            }
    //        }
    //    }
    //}


    public void TutorialCheck(IMessage rmessage)
    {
        TutorialNum += 1;

        if (TutorialNum == 1)
        {
            Tutorial = true;
            ValkyrieTest = false;
        }

        else if (TutorialNum == 7)
        {
            Tutorial = false;
            ValkyrieTest = true;
        }

    }

    // 실질적인 발키리 기체 제어 함수
    void Thread1_UpdateObject(float rotation, float leftSpeed, float rightSpeed, float sideSpeed)
    {
        const int addToDelay = 1;
        // SET SPEED
        float speed = (leftSpeed + rightSpeed) * 7.0f;
        if (Mathf.Abs(leftSpeed) == Mathf.Abs(rightSpeed))
            speed *= 2.0f;

        /// ANIMATION -> ROLL, PITCH
        {
            //움직임 관련, 앞으로 갈때 기우뚱
            Vector2 angle = Thread1_WalkingAnimation(leftSpeed, rightSpeed, sideSpeed);
            renderedRoll = AngleConverter.LerpAngle(renderedRoll, angle.x, 0.1f);
            renderedPitch = AngleConverter.LerpAngle(renderedPitch, angle.y, 0.1f);

            // WRITE
            basic[0].Roll = AngleConverter.ToDeviceVer1(renderedRoll); // X
            basic[0].Pitch = AngleConverter.ToDeviceVer1(renderedPitch); // Y
            basic[0].Heave = AngleConverter.ToDeviceVer1(bodyAngle.sy * 100.0f); // 높낮이 수정
            basic[0].Delay = 1;
        }

        /// YAW (Z)
        {
            // READ
            double current = AngleConverter.ToTypicalVer2(output.Yaw);
            renderedYaw = AngleConverter.LerpAngle(renderedYaw, (float)current, 0.1f);

            // CONTROL 회전 수정
            FAngleParam param = FAngleParam.MakeAngleParam(current, 0.1f * leftSpeed, -0.1f * rightSpeed, 0.02f, addToDelay, 0);

            //회전관련
            // WRITE
            yawEx[0].Target = AngleConverter.ToDeviceVer2(param.TargetDegree);
            yawEx[0].Delay = param.Delay;
        }
    }

    void Thread2_UpdateDevice()
    {
        ExtendedOutput[] temp = new ExtendedOutput[1];


        while (enableThread)
        {
            //uint inPitch = basic[0].Pitch;
            //uint inYaw = yawEx[0].Target;
            //uint yawDelay = yawEx[0].Delay;

            //// Call Device API
            //ExtendedUpdate(basic, rollEx, pitchEx, yawEx, devOutput, devInput, temp);

            //uint outPitch = temp[0].Src.Pitch * 8;
            //uint outYaw = temp[0].Src.YawEx;

            //output.Pitch = 0.7 * Mathf.Abs((int)inPitch - (int)outPitch) / 20000 < 0.125 ? (inPitch + outPitch) / 2 : outPitch;

            //float test = AngleConverter.ModifyOutputYaw(
            //                    (float)AngleConverter.ToTypicalVer2(inYaw),
            //                    (int)yawDelay,
            //                    (float)AngleConverter.ToTypicalVer2(outYaw));
            //output.Yaw = AngleConverter.ToDeviceVer2(test);

            uint inPitch = basic[0].Pitch;
            uint inYaw = yawEx[0].Target;
            uint yawDelay = yawEx[0].Delay;

            // Call Device API
            ExtendedUpdate(basic, rollEx, pitchEx, yawEx, devOutput, devInput, temp);

            uint outPitch = temp[0].Src.Pitch * 8;
            uint outYaw = temp[0].Src.YawEx;

            /// 이번에 목표지점 도달시 거리 수정
            // Update Tick : 1/8 sec
            // Pitch Max Speed : 20000 / 0.7 sec
            // Yaw Max Speed : 36000 / 6.8 sec
            output.Pitch = 0.7 * Mathf.Abs((int)inPitch - (int)outPitch) / 20000 < 0.125 ?
                (inPitch + outPitch) / 2 :
                outPitch;

            /// 원래 버전
            if (version == 1)
            {

                output.Yaw = 6.8 * Mathf.Abs((int)inYaw - (int)outYaw) / 36000 < 0.125 ?
                    (inYaw + outYaw) / 2 :
                    outYaw;
                ///Debug.Log(" before = " + outYaw + "\t next = " + output.Yaw + "\t delay = " + yawDelay + "\t dir = " + ((int)output.Yaw - (int)outYaw));
            }
            /// 미리 회전 버전
            if (version == 2)
            {
                // Debug.Log("WHILE IN!!!!!!!!!!!!!!!");
                float test = AngleConverter.ModifyOutputYaw(
                                    (float)AngleConverter.ToTypicalVer2(inYaw),
                                    (int)yawDelay,
                                    (float)AngleConverter.ToTypicalVer2(outYaw));
                output.Yaw = AngleConverter.ToDeviceVer2(test);
            }

        }
        Debug.Log(" ---- END ---- ");
    }

    // 발키리 기체 걷는 효과
    Vector2 Thread1_WalkingAnimation(float leftSpeed, float rightSpeed, float sideSpeed)
    {
        // Walking
        if (bodyAngle.y <= 1.0f)
        {
            if (0.0f < sideSpeed)
                leftSpeed = rightSpeed = 1.0f;
            else if (sideSpeed < 0.0f)
                leftSpeed = rightSpeed = -1.0f;

            if (leftSpeed == 0)
                bodyAngle.LeftLeg = 0.0f;
            if (rightSpeed == 0)
                bodyAngle.RightLeg = 0.0f;

            if (leftSpeed != 0)
                bodyAngle.LeftLeg += 0.13f * leftSpeed;
            if (rightSpeed != 0)
                bodyAngle.RightLeg += 0.13f * rightSpeed;
        }

        float height = 0.0f;

        // Left Up
        if (leftSpeed != 0 && sideSpeed != 0)
        {
            float f1 = Mathf.Sin(-bodyAngle.LeftLeg) * 0.3f;
            float f2 = Mathf.Cos(-bodyAngle.LeftLeg) * 0.3f;
            float f3 = Mathf.Sin(-bodyAngle.LeftLeg) * 30;
            f3 *= 0.2f;
            if (Mathf.Abs(height) < Mathf.Abs(f3))
                height = f3;
        }

        // Right Up
        if (rightSpeed != 0 && sideSpeed != 0)
        {
            float offset = Mathf.Sign(leftSpeed) == Mathf.Sign(rightSpeed) ? 3.141592f : 0.0f;
            float f1 = Mathf.Sin(-bodyAngle.RightLeg + offset) * 0.3f;
            float f2 = Mathf.Cos(-bodyAngle.RightLeg + offset) * 0.3f;
            float f3 = Mathf.Sin(-bodyAngle.RightLeg + offset) * 30;
            f3 *= 0.2f;
            if (Mathf.Abs(height) < Mathf.Abs(-f3))
                height = -f3;
        }

        return new Vector2(height, Mathf.Abs(height) * 0.2f);
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "LeftBlockStage")
        {
            bIsLeftblockHit = true;
        }

        if (collision.gameObject.tag == "RightBlockStage")
        {
            bIsRightblockHit = true;
        }
    }

    private void OnCollisionExit(Collision collision)
    {
        if (collision.gameObject.tag == "LeftBlockStage")
        {
            bIsLeftblockHit = false;
        }

        if (collision.gameObject.tag == "RightBlockStage")
        {

            bIsRightblockHit = false;
        }
    }

    private void SetWarnText(IMessage rMessage)
    {
        if ((int)rMessage.Data == 0)
        {
            WarnSub.DORestart();
        }
    }

    public void SetGameStart(bool bGameStart)
    {
        bIsGameStart = bGameStart;
    }
}
