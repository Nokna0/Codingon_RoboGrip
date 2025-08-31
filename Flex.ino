#include <Servo.h>

// 핀 정의
const int FLEX_PIN = A0;    // 플렉스 센서를 아날로그 핀 A0에 연결
const int SERVO_PIN = 9;    // 서보 모터를 디지털 핀 9에 연결
const int LED_PIN = 13;     // 상태 표시 LED를 디지털 핀 13에 연결

// 서보 모터 객체 생성
Servo myServo;

// 플렉스 센서 값 범위 설정
const int FLEX_MIN = 200;   // 센서가 구부러지지 않았을 때의 최소값
const int FLEX_MAX = 1023;  // 센서가 최대로 구부러졌을 때의 최대값 (ADC 최댓값)

// MG996R 360도 연속회전 서보모터 제어값
const int STOP_SPEED = 90;      // 정지 (1.5ms 펄스)
const int CLOCKWISE_SPEED = 120;    // 시계방향 속도
const int COUNTER_CLOCKWISE_SPEED = 60; // 반시계방향 속도

// 플렉스 센서 임계값
const int FLEX_THRESHOLD = 220; // 이 값 이상일 때만 모터 동작
const int FLEX_RETURN_THRESHOLD = 200; // 복귀 판단 임계값

// 시간 관리 변수
const unsigned long MAX_FORWARD_TIME = 700;    // 최대 전진 시간 0.7초 (700ms)
unsigned long forwardStartTime = 0;            // 전진 시작 시간
unsigned long forwardDuration = 0;             // 실제 전진한 시간
unsigned long returnStartTime = 0;             // 복귀 시작 시간
unsigned long returnDuration = 0;              // 복귀해야 할 시간

// 상태 변수
enum MotorState {
  STOPPED,      // 정지
  FORWARD,      // 전진 (시계방향)
  RETURNING     // 복귀 (반시계방향)
};

MotorState currentState = STOPPED;

void setup() {
  // 시리얼 통신 초기화 (디버깅용)
  Serial.begin(9600);
  
  // 핀 모드 설정
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED 초기값: 꺼짐
  
  // 서보 모터 핀 연결
  myServo.attach(SERVO_PIN);
  
  // 서보 모터를 정지 상태로 초기화
  myServo.write(STOP_SPEED);
  
  Serial.println("=== 플렉스 센서 제어 시스템 ===");
  Serial.println("동작 방식:");
  Serial.println("1. 센서 구부림 → 최대 0.7초까지 시계방향 회전");
  Serial.println("2. 센서 놓음 → 이전 작동시간만큼 반시계방향 복귀");
  Serial.println("전진 임계값: " + String(FLEX_THRESHOLD));
  Serial.println("복귀 임계값: " + String(FLEX_RETURN_THRESHOLD));
  Serial.println("==============================");
}

void loop() {
  // 플렉스 센서 값 읽기
  int flexValue = analogRead(FLEX_PIN);
  unsigned long currentTime = millis();
  
  switch(currentState) {
    case STOPPED:
      // 정지 상태 - 센서가 구부러지면 전진 시작
      if (flexValue >= FLEX_THRESHOLD) {
        currentState = FORWARD;
        forwardStartTime = currentTime;
        myServo.write(CLOCKWISE_SPEED);
        digitalWrite(LED_PIN, HIGH);
        Serial.println("전진 시작!");
      }
      break;
      
    case FORWARD:
      // 전진 중
      if (flexValue < FLEX_RETURN_THRESHOLD) {
        // 센서를 놓으면 복귀 시작
        forwardDuration = currentTime - forwardStartTime;
        returnDuration = forwardDuration; // 전진한 시간만큼 복귀
        
        currentState = RETURNING;
        returnStartTime = currentTime;
        myServo.write(COUNTER_CLOCKWISE_SPEED);
        
        Serial.print("전진 완료! 전진시간: ");
        Serial.print(forwardDuration);
        Serial.print("ms → 복귀시간: ");
        Serial.print(returnDuration);
        Serial.println("ms");
        
      } else if (currentTime - forwardStartTime >= MAX_FORWARD_TIME) {
        // 최대 시간 도달하면 자동으로 복귀 대기
        forwardDuration = MAX_FORWARD_TIME;
        returnDuration = forwardDuration;
        
        myServo.write(STOP_SPEED);
        digitalWrite(LED_PIN, LOW);
        
        Serial.print("최대 시간 도달! 전진시간: ");
        Serial.print(forwardDuration);
        Serial.println("ms → 센서를 놓으면 복귀 시작");
        
        // 센서가 놓일 때까지 대기 상태로 변경
        while(analogRead(FLEX_PIN) >= FLEX_RETURN_THRESHOLD) {
          delay(10); // 센서가 놓일 때까지 대기
        }
        
        // 센서를 놓으면 복귀 시작
        currentState = RETURNING;
        returnStartTime = millis();
        myServo.write(COUNTER_CLOCKWISE_SPEED);
        Serial.println("복귀 시작!");
      }
      break;
      
    case RETURNING:
      // 복귀 중
      if (currentTime - returnStartTime >= returnDuration) {
        // 복귀 완료
        currentState = STOPPED;
        myServo.write(STOP_SPEED);
        digitalWrite(LED_PIN, LOW);
        
        Serial.print("복귀 완료! 복귀시간: ");
        Serial.print(currentTime - returnStartTime);
        Serial.println("ms");
        
        // 변수 초기화
        forwardDuration = 0;
        returnDuration = 0;
        
      } else {
        // 복귀 중 LED 깜빡임
        digitalWrite(LED_PIN, (millis() / 200) % 2);
      }
      break;
  }
  
  // 디버깅 정보 출력 (시리얼 모니터용)
  Serial.print("센서: ");
  Serial.print(flexValue);
  Serial.print(" | 상태: ");
  
  String stateText = "";
  int stateValue = 0; // 플로터용 상태 값
  
  switch(currentState) {
    case STOPPED:
      stateText = "정지";
      stateValue = 0;
      break;
    case FORWARD:
      stateText = "전진중 (";
      stateText += String(currentTime - forwardStartTime);
      stateText += "ms)";
      stateValue = 1;
      break;
    case RETURNING:
      stateText = "복귀중 (";
      stateText += String(currentTime - returnStartTime);
      stateText += "/";
      stateText += String(returnDuration);
      stateText += "ms)";
      stateValue = 2;
      break;
  }
  
  Serial.println(stateText);
  
  // 시리얼 플로터용 출력 (변수명:값 형식)
  Serial.print("FlexSensor:");
  Serial.print(flexValue);
  Serial.print(",");
  
  Serial.print("State:");
  Serial.print(stateValue);  // 0=정지, 1=전진, 2=복귀
  Serial.print(",");
  
  Serial.print("Threshold:");
  Serial.print(FLEX_THRESHOLD);
  Serial.print(",");
  
  Serial.print("ReturnThreshold:");
  Serial.print(FLEX_RETURN_THRESHOLD);
  
  // 현재 동작 시간 표시 (전진 중이면 전진 시간, 복귀 중이면 복귀 시간)
  if (currentState == FORWARD) {
    Serial.print(",");
    Serial.print("ForwardTime:");
    Serial.print(currentTime - forwardStartTime);
  } else if (currentState == RETURNING) {
    Serial.print(",");
    Serial.print("ReturnTime:");
    Serial.print(currentTime - returnStartTime);
    Serial.print(",");
    Serial.print("TargetTime:");
    Serial.print(returnDuration);
  } else {
    Serial.print(",");
    Serial.print("ForwardTime:");
    Serial.print(0);
    Serial.print(",");
    Serial.print("ReturnTime:");
    Serial.print(0);
  }
  
  Serial.println(); // 플로터용 줄바꿈
  
  // 약간의 딜레이
  delay(50);
}

/*
 * 동작 원리:
 * 
 * 1. 상태 기반 제어:
 *    - STOPPED: 정지 상태, 센서 입력 대기
 *    - FORWARD: 시계방향 회전 중 (최대 0.7초)
 *    - RETURNING: 반시계방향 복귀 중 (전진한 시간만큼)
 * 
 * 2. 시간 추적:
 *    - forwardDuration: 실제로 전진한 시간
 *    - returnDuration: 복귀해야 할 시간 (= forwardDuration)
 * 
 * 3. LED 상태:
 *    - 켜짐: 전진 중
 *    - 깜빡임: 복귀 중
 *    - 꺼짐: 정지
 * 
 * 4. 동작 시퀀스:
 *    센서 구부림 → 전진 시작 → (센서 놓음 OR 0.7초 경과) → 복귀 시작 → 전진시간만큼 복귀 → 정지
 * 
 * 회로 연결:
 * - 플렉스 센서: A0 핀 (10kΩ 풀다운 저항 필요)
 * - 서보 모터: 디지털 핀 9
 * - LED: 디지털 핀 13 (220Ω 저항 필요)
 * 
 * 특징:
 * - 전진한 시간과 복귀 시간이 정확히 일치
 * - 최대 0.7초 전진 제한
 * - 상태 기반으로 명확한 동작 구분
 */