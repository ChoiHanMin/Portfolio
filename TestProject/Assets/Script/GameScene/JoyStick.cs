using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.TextCore.Text;

public class JoyStick : MonoBehaviour, IDragHandler, IPointerUpHandler, IPointerDownHandler
{
    public RectTransform joystickBackground; // 조이스틱 배경
    public RectTransform joystickHandle;    // 조이스틱 핸들

    private Vector2 inputVector;
    public Vector2 InputVector => inputVector; // 외부에서 입력 벡터를 가져오기 위한 프로퍼티
    public void OnDrag(PointerEventData eventData)
    {
        // 터치 위치를 로컬 좌표로 변환
        Vector2 position;
        RectTransformUtility.ScreenPointToLocalPointInRectangle(joystickBackground, eventData.position, eventData.pressEventCamera, out position);

        // 좌표 정규화 (-1 ~ 1)
        position.x = position.x / (joystickBackground.sizeDelta.x / 2);
        position.y = position.y / (joystickBackground.sizeDelta.y / 2);

        // 입력 벡터 제한
        inputVector = new Vector2(position.x, position.y);
        inputVector = (inputVector.magnitude > 1.0f) ? inputVector.normalized : inputVector;

        // 핸들 위치 업데이트
        joystickHandle.anchoredPosition = new Vector2(inputVector.x * (joystickBackground.sizeDelta.x / 2), inputVector.y * (joystickBackground.sizeDelta.y / 2));
    }

    public void OnPointerDown(PointerEventData eventData)
    {
        OnDrag(eventData); // 터치 시작 시 드래그 처리
    }

    public void OnPointerUp(PointerEventData eventData)
    {
        inputVector = Vector2.zero; // 입력 초기화
        joystickHandle.anchoredPosition = Vector2.zero; // 핸들을 중앙으로 초기화
        GameManager.inst.character.SetAnim(false);
    }
}
