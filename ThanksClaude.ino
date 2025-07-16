#include <Servo.h>

// 핀 설정
const int FLEX_PIN = A0;        // 플렉스 센서 아날로그 입력핀
const int SERVO_PIN = 9;        // 서보모터 디지털 출력핀

// 서보모터 객체 생성
Servo myServo;

// 센서 캘리브레이션을 위한 변수들
int flexMin = 1023;             // 센서 최소값 (구부리지 않은 상태)
int flexMax = 0;                // 센서 최대값 (완전히 구부린 상태)

// 필터링을 위한 변수
int previousFlexValue = 0;
const int SMOOTH_FACTOR = 5;    // 스무딩 팩터 (값이 클수록 더 부드러움)

// 재사용 가능한 캘리브레이션 함수
void calibrateSensor(unsigned long duration_ms) {
  flexMin = 1023;
  flexMax = 0;
  unsigned long calibrationTime = millis();
  while (millis() - calibrationTime < duration_ms) {
    int flexValue = analogRead(FLEX_PIN);
    if (flexValue < flexMin) flexMin = flexValue;
    if (flexValue > flexMax) flexMax = flexValue;
    delay(10);
  }
  // 캘리브레이션 값이 정상 범위인지 체크
  if (flexMin == flexMax || flexMax - flexMin < 10) {
    Serial.println("경고: 캘리브레이션 범위가 너무 작거나 0입니다. 센서 상태를 확인하세요!");
  }
  Serial.print("캘리브레이션 완료 - 최소값: ");
  Serial.print(flexMin);
  Serial.print(", 최대값: ");
  Serial.println(flexMax);
}

void setup() {
  Serial.begin(115200); // 통신속도 개선
  delay(500); // 전원 안정화

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  Serial.println("플렉스 센서 서보모터 제어 시작");
  Serial.println("처음 5초간 센서를 구부렸다 펴면서 캘리브레이션을 진행하세요.");
  calibrateSensor(5000);
  delay(1000);
}

void loop() {
  // 플렉스 센서 값 읽기
  int flexValue = analogRead(FLEX_PIN);
  // 스무딩 필터 (이전 샘플 여러 개로 평균내기, 여긴 간단하게 유지)
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

  // 예시: 시리얼 입력으로 'r'이 들어오면 재캘리브레이션
  if (Serial.available()) {
    char input = Serial.read();
    if (input == 'r' || input == 'R') {
      Serial.println("사용자 요청으로 재캘리브레이션 실행");
      calibrateSensor(3000);
    }
  }
}