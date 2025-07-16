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

void setup() {
  // 시리얼 통신 초기화 (디버깅용)
  Serial.begin(9600);
  
  // 서보모터 초기화
  myServo.attach(SERVO_PIN);
  myServo.write(0);  // 초기 위치를 0도로 설정
  
  Serial.println("플렉스 센서 서보모터 제어 시작");
  Serial.println("처음 5초간 센서를 구부렸다 펴면서 캘리브레이션을 진행하세요.");
  
  // 5초간 캘리브레이션
  unsigned long calibrationTime = millis();
  while (millis() - calibrationTime < 5000) {
    int flexValue = analogRead(FLEX_PIN);
    
    // 최소값과 최대값 업데이트
    if (flexValue < flexMin) {
      flexMin = flexValue;
    }
    if (flexValue > flexMax) {
      flexMax = flexValue;
    }
    
    delay(10);
  }
  
  Serial.print("캘리브레이션 완료 - 최소값: ");
  Serial.print(flexMin);
  Serial.print(", 최대값: ");
  Serial.println(flexMax);
  
  delay(1000);
}

void loop() {
  // 플렉스 센서 값 읽기
  int flexValue = analogRead(FLEX_PIN);
  
  // 스무딩 필터 적용 (급격한 변화를 줄임)
  flexValue = (flexValue + previousFlexValue * (SMOOTH_FACTOR - 1)) / SMOOTH_FACTOR;
  previousFlexValue = flexValue;
  
  // 센서 값을 서보모터 각도로 변환 (0-180도)
  int servoAngle = map(flexValue, flexMin, flexMax, 0, 180);
  
  // 각도 값 제한 (0-180도 범위 내)
  servoAngle = constrain(servoAngle, 0, 180);
  
  // 서보모터 제어
  myServo.write(servoAngle);
  
  // 디버그 정보 출력
  Serial.print("플렉스 센서 값: ");
  Serial.print(flexValue);
  Serial.print(" -> 서보 각도: ");
  Serial.print(servoAngle);
  Serial.println("도");
  
  // 안정성을 위한 지연
  delay(50);
}

// 추가 함수들

// 센서 재캘리브레이션 함수 (필요시 사용)
void recalibrate() {
  Serial.println("재캘리브레이션 시작...");
  flexMin = 1023;
  flexMax = 0;
  
  unsigned long calibrationTime = millis();
  while (millis() - calibrationTime < 3000) {
    int flexValue = analogRead(FLEX_PIN);
    
    if (flexValue < flexMin) {
      flexMin = flexValue;
    }
    if (flexValue > flexMax) {
      flexMax = flexValue;
    }
    
    delay(10);
  }
  
  Serial.println("재캘리브레이션 완료");
}