# LoRa PJT
## Goal
- LoRa 통신 활용할 수 있는 코드 탬플릿 개발
## HW
1. ESP32
2. "Reyax rylr998" lora 모듈
3. 이후 stm32 평가보드와 진행 예정

## 참고 링크
[esp32 - lora](https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/)

[arduino - lora](https://www.youtube.com/watch?v=LiWlPERp1ec&t=220s)

## 관련 국내법령
**국내 주파수는 920.9 - 923.3 MHz**

## 통신 프로토콜
- UART

## LoRa 모듈 세팅 현황
### 초기화 및 세팅 절차
1. AT
2. AT+RESET
3. AT+MODE=0
4. AT+IPR=9600
5. AT+BAND=923000000M
6. AT+PARAMETER=7,9,4,7
7. AT+ADDRESS=주소
8. AT+NETWORKID=6
9. AT+CRFOP=12



### A : Transmitter
- 주소 = 2
- ESP32와 연결

### B : Receiver
- 주소 = 3
- Arduino Nano 33 ble와 연결


## 스위치
- 위 y = 1023
- 좌 x = 1023



## AT COMMANDS
The command format is: AT+PARAMETER=<SF>,<BW>,<CR>,<Preamble>

SF (Spreading Factor) = 5 ~ 11

Higher values (like 11) increase range but reduce speed
Lower values (like 5) increase speed but reduce range
Values:

SF=5: Fastest data rate, shortest range
SF=7: ↓
SF=8: ↓
SF=9: ↓
SF=10: ↓
SF=11: Slowest data rate, longest range




BW (Bandwidth) = 7 ~ 9
Values:
7: 125KHZ
8: 250KHZ
9: 500KHZ
Lower bandwidth (like 0) = longer range but slower speed
Higher bandwidth (like 9) = shorter range but faster speed


CR (Coding Rate) = 1
Values: 1 to 4
CR=1: 4/5 coding rate
CR=2: 4/6 coding rate
CR=3: 4/7 coding rate
CR=4: 4/8 coding rate
Higher values provide more error correction but reduce effective data rate


Preamble = 4 ~ 24

Range: 4 to 7
Length of the preamble signal
Longer preamble helps with synchronization but adds overhead



In our code, we used AT+PARAMETER=12,7,1,7 which means:

SF=12: Maximum range (but slowest speed)
BW=7: 125 kHz bandwidth (good balance)
CR=1: 4/5 coding rate (minimal error correction)
Preamble=7: Maximum preamble length

If you want:

Longer range: Increase SF, decrease BW
Faster speed: Decrease SF, increase BW
Better error correction: Increase CR
Better synchronization: Increase Preamble

Would you like me to provide some common parameter combinations for different use cases?

## Receiving Data
**+RCV = \<Address>, \<Length>, \<Data>, \<RSSI>, \<SNR>**


# 이하 폐기 버전
## HW
- Lilygo T3S3 Board with SX1276 radio chip

## 국내 주파수 사용 조건
- 920MHz 대 사용, 보드로 사용 가능 확인
- 라디오 칩으로 사용 가능한 주파수 범위 확인 필요 + 실제로 920MHz 대로 사용 하고 있는 것인지 확인 필요

## 제조사 깃허브
<https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series>

## 세팅
1. 

## Troubleshoot
### 대부분의 경우 제조사 깃허브에서 해결 가능


1. 코드 Flash 가 안될 때
- 컴파일 후 무조건 BOOT 버튼 꾹 누르고 있는다. 
- Boot 버튼 떼기 전에 리셋 한번 눌렀다 뗀다.
- Boot 버튼 떼고 본다.

2. 그래도 컴파일 안 될때
- 위 깃허브의 firmware 폴더에 나온 방법 참고
