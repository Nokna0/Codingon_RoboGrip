/*
 * 플렉스 센서 아두이노 프로젝트
 * 플렉스 센서의 구부림 정도를 읽어서 PWM 출력으로 제어
 */

// 핀 정의
const int FLEX_PIN = A0;      // 플렉스 센서가 연결된 아날로그 입력핀
const int OUTPUT_PIN = 9;     // PWM 출력핀 (9번 디지털 핀)

// 센서 값 변수
int flexValue = 0;            // 플렉스 센서에서 읽은 원시 값
int outputValue = 0;          // PWM 출력을 위해 변환된 값

// 센서 보정값 (실제 센서에 맞게 조정 필요)
const int FLEX_MIN = 200;     // 센서가 완전히 펴졌을 때의 최소값
const int FLEX_MAX = 800;     // 센서가 완전히 구부러졌을 때의 최대값

void setup() {
  // 시리얼 통신 초기화 (디버깅용)
  Serial.begin(9600);
  
  // 핀 모드 설정
  pinMode(FLEX_PIN, INPUT);   // 아날로그 입력핀 설정
  pinMode(OUTPUT_PIN, OUTPUT); // PWM 출력핀 설정
  
  Serial.println("플렉스 센서 초기화 완료");
  Serial.println("센서값 | 출력값");
  Serial.println("----------------");
}

void loop() {
  // 플렉스 센서 값 읽기 (0~1023 범위)
  flexValue = analogRead(FLEX_PIN);
  
  // 센서 값을 PWM 출력 범위로 매핑 (0~255)
  outputValue = map(flexValue, FLEX_MIN, FLEX_MAX, 0, 255);
  
  // 출력값이 범위를 벗어나지 않도록 제한
  outputValue = constrain(outputValue, 0, 255);
  
  // PWM 출력
  analogWrite(OUTPUT_PIN, outputValue);
  
  // 시리얼 모니터에 값 출력 (디버깅용)
  Serial.print(flexValue);
  Serial.print("    | ");
  Serial.println(outputValue);
  
  // 잠깐 대기 (너무 빠른 업데이트 방지)
  delay(50);
}

/*
 * 사용법 및 회로 연결:
 * 
 * 1. 플렉스 센서 연결:
 *    - 플렉스 센서의 한쪽 끝을 5V에 연결
 *    - 플렉스 센서의 다른 쪽 끝을 A0핀과 10kΩ 풀다운 저항을 통해 GND에 연결
 * 
 * 2. 출력 장치 연결 (예: LED):
 *    - 9번 핀에서 220Ω 저항을 거쳐 LED의 양극(+)에 연결
 *    - LED의 음극(-)을 GND에 연결
 * 
 * 3. 보정 방법:
 *    - 시리얼 모니터를 열고 플렉스 센서를 완전히 펴본 상태와 구부린 상태의 값을 확인
 *    - FLEX_MIN과 FLEX_MAX 값을 실제 측정값에 맞게 수정
 * 
 * 4. 동작:
 *    - 손가락을 구부리면 플렉스 센서의 저항값이 변화
 *    - 이에 따라 9번 핀의 PWM 출력이 비례적으로 변화
 *    - LED를 연결했다면 밝기가 손가락 구부림에 따라 변화
 */