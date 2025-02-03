using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.TextCore.Text;

public class JoyStick : MonoBehaviour, IDragHandler, IPointerUpHandler, IPointerDownHandler
{
    public RectTransform joystickBackground; // ���̽�ƽ ���
    public RectTransform joystickHandle;    // ���̽�ƽ �ڵ�

    private Vector2 inputVector;
    public Vector2 InputVector => inputVector; // �ܺο��� �Է� ���͸� �������� ���� ������Ƽ
    public void OnDrag(PointerEventData eventData)
    {
        // ��ġ ��ġ�� ���� ��ǥ�� ��ȯ
        Vector2 position;
        RectTransformUtility.ScreenPointToLocalPointInRectangle(joystickBackground, eventData.position, eventData.pressEventCamera, out position);

        // ��ǥ ����ȭ (-1 ~ 1)
        position.x = position.x / (joystickBackground.sizeDelta.x / 2);
        position.y = position.y / (joystickBackground.sizeDelta.y / 2);

        // �Է� ���� ����
        inputVector = new Vector2(position.x, position.y);
        inputVector = (inputVector.magnitude > 1.0f) ? inputVector.normalized : inputVector;

        // �ڵ� ��ġ ������Ʈ
        joystickHandle.anchoredPosition = new Vector2(inputVector.x * (joystickBackground.sizeDelta.x / 2), inputVector.y * (joystickBackground.sizeDelta.y / 2));
    }

    public void OnPointerDown(PointerEventData eventData)
    {
        OnDrag(eventData); // ��ġ ���� �� �巡�� ó��
    }

    public void OnPointerUp(PointerEventData eventData)
    {
        inputVector = Vector2.zero; // �Է� �ʱ�ȭ
        joystickHandle.anchoredPosition = Vector2.zero; // �ڵ��� �߾����� �ʱ�ȭ
        GameManager.inst.character.SetAnim(false);
    }
}
