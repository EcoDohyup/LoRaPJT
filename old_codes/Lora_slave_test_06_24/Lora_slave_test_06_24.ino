#include <HardwareSerial.h>
#include <ESP32Servo.h>  // Servo.h 대신 ESP32Servo.h를 사용

// 모터 제어 핀 설정
#define BLDC_Motor_Pin1 12  // 모터1 드라이버의 신호 입력에 연결된 ESP32 핀 번호
#define BLDC_Motor_Pin2 14  // 모터2 드라이버의 신호 입력에 연결된 ESP32 핀 번호

HardwareSerial lora(1); // Serial1, pins 16 (RX) and 17 (TX)

Servo Mc1;  // 모터1 객체 생성
Servo Mc2;  // 모터2 객체 생성

int currentSpeedForward = 1700;  // 기본 모터 속도 (전진)
int currentSpeedBackward = 1300;  // 기본 모터 속도 (후진)
char currentCommand = 's';  // 현재 모터 상태를 저장

void setup() {
  Serial.begin(115200);
  lora.begin(115200, SERIAL_8N1, 16, 17);

  // Set initial parameters
  sendATCommand("AT+PARAMETER=9,7,1,12");
  
  // 모터 핀 초기화
  Mc1.attach(BLDC_Motor_Pin1);
  Mc2.attach(BLDC_Motor_Pin2);

  motorStop(); // 모터를 초기 상태에서 정지

  Serial.println("Setup completed. Ready to receive:");
}

void loop() {
  if (lora.available()) {
    String data = lora.readStringUntil('\n');
    Serial.println("Received raw data: " + data); // 디버깅 메시지 추가
    if (data.startsWith("+RCV")) {
      handleReceivedData(data);
    }
  }
}

void handleReceivedData(String data) {
  Serial.println("Handling received data: " + data); // 디버깅 메시지 추가

  // 데이터에서 메시지 부분만 추출하여 처리
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);
  int thirdComma = data.indexOf(',', secondComma + 1);
  int fourthComma = data.indexOf(',', thirdComma + 1);

  if (firstComma != -1 && secondComma != -1 && thirdComma != -1 && fourthComma != -1) {
    String message = data.substring(secondComma + 1, thirdComma);
    Serial.println("Extracted message: " + message); // 디버깅 메시지 추가

    // 수신된 데이터에 따라 모터 제어
    if (message.length() > 0) {
      char input = message.charAt(0); // 수신된 첫 번째 문자를 명령으로 사용
      if (input == '1' || input == '2' || input == '3') {
        setSpeed(input); // 속도 설정
        controlMotor(currentCommand); // 현재 명령 다시 실행하여 속도 적용
      } else {
        controlMotor(input); // 모터 제어 함수 호출
      }
    }
  } else {
    Serial.println("Error parsing data."); // 디버깅 메시지 추가
  }
}

void sendATCommand(String command) {
  lora.println(command);
  delay(100);
  while (lora.available()) {
    String response = lora.readStringUntil('\n');
    Serial.println(response);
  }
}

// 속도 설정 함수
void setSpeed(char speedChar) {
  switch (speedChar) {
    case '1':
      currentSpeedForward = 1550; // 느리게 전진
      currentSpeedBackward = 1450; // 느리게 후진
      break;
    case '2':
      currentSpeedForward = 1650; // 중간 전진
      currentSpeedBackward = 1350; // 중간 후진
      break;
    case '3':
      currentSpeedForward = 1750; // 빠르게 전진
      currentSpeedBackward = 1250; // 빠르게 후진
      break;
    default:
      currentSpeedForward = 1650; // 기본값 중간 속도 전진
      currentSpeedBackward = 1350; // 기본값 중간 속도 후진
      break;
  }
  Serial.println("Speed set to: " + String(currentSpeedForward) + " (Forward), " + String(currentSpeedBackward) + " (Backward)");
}

// 모터 제어 함수
void controlMotor(char input) {
  currentCommand = input; // 현재 명령을 저장
  switch (input) {
    case 'w':
      motorForward(); // 전진
      break;
    case 's':
      motorStop(); // 정지
      break;
    case 'a':
      motorLeft(); // 좌회전
      break;
    case 'd':
      motorRight(); // 우회전
      break;
    case 'z':
      motorBackward(); // 후진
      break;
    default:
      Serial.println("Unknown command: " + String(input));
      break;
  }
}

// 모터 전진 함수
void motorForward() {
  Mc1.writeMicroseconds(currentSpeedForward); // 모터1 전진
  Mc2.writeMicroseconds(currentSpeedForward); // 모터2 전진
}

// 모터 후진 함수
void motorBackward() {
  Mc1.writeMicroseconds(currentSpeedBackward); // 모터1 후진
  Mc2.writeMicroseconds(currentSpeedBackward); // 모터2 후진
}

// 모터 정지 함수
void motorStop() {
  Mc1.writeMicroseconds(1500); // 모터1 정지
  Mc2.writeMicroseconds(1500); // 모터2 정지
}

// 모터 좌회전 함수
void motorLeft() {
  Mc1.writeMicroseconds(currentSpeedBackward); // 모터1 후진
  Mc2.writeMicroseconds(currentSpeedForward); // 모터2 전진
}

// 모터 우회전 함수
void motorRight() {
  Mc1.writeMicroseconds(currentSpeedForward); // 모터1 전진
  Mc2.writeMicroseconds(currentSpeedBackward); // 모터2 후진
}