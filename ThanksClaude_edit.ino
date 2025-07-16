#include <Servo.h>

// 핀 설정
const int FLEX_PIN = A0;        // 플렉스 센서 아날로그 입력핀
const int SERVO_PIN = 9;        // 서보모터 디지털 출력핀

// 서보모터 객체 생성
Servo myServo;

// 센서 범위(직접 지정)
const int flexMin = 400;        // 센서 최소값 (구부리지 않은 상태, 예시값)
const int flexMax = 900;        // 센서 최대값 (완전히 구부린 상태, 예시값)

// 필터링을 위한 변수
int previousFlexValue = 0;
const int SMOOTH_FACTOR = 5;    // 스무딩 팩터 (값이 클수록 더 부드러움)

void setup() {
  Serial.begin(115200); // 통신속도 개선
  delay(500); // 전원 안정화

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  Serial.println("플렉스 센서 서보모터 제어 시작(캘리브레이션 없음)");
}

void loop() {
  // 플렉스 센서 값 읽기
  int flexValue = analogRead(FLEX_PIN);
  // 스무딩 필터
  flexValue = (flexValue + previousFlexValue * (SMOOTH_FACTOR - 1)) / SMOOTH_FACTOR;
  previousFlexValue = flexValue;
  // 센서 값을 서보모터 각도로 변환 (0-180도)
  int servoAngle = 0;
  if (flexMax != flexMin) {
    servoAngle = map(flexValue, flexMin, flexMax, 0, 180);
  }
  servoAngle = constrain(servoAngle, 0, 180);
  myServo.write(servoAngle);

  Serial.print("플렉스 센서 값: ");
  Serial.print(flexValue);
  Serial.print(" -> 서보 각도: ");
  Serial.print(servoAngle);
  Serial.println("도");

  delay(50);

  // 캘리브레이션 관련 코드 제거됨
}
