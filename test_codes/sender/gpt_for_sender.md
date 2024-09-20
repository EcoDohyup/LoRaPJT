알겠습니다! **Sender** 부의 코드(`Sender.ino`, `LoRaBoards.cpp`, `LoRaBoards.h`)을 상세히 분석하고, 각 부분의 구조와 동작 방식을 차근차근 설명해 드리겠습니다. 이를 통해 코드의 전체적인 흐름과 주요 기능들을 이해하는 데 도움이 되실 것입니다.

---

## **1. 코드 구조 개요**

### **파일 구성**
- **Sender.ino:** 메인 Arduino 스케치 파일로, LoRa 송신기의 동작을 정의합니다.
- **LoRaBoards.cpp:** 다양한 하드웨어 보드와 주변 장치(센서, 디스플레이 등)를 초기화하고 관리하는 함수들을 구현한 파일입니다.
- **LoRaBoards.h:** `LoRaBoards.cpp`에서 구현한 함수들의 선언과 전역 변수, 매크로 정의 등을 포함한 헤더 파일입니다.

### **종속성**
- **라이브러리:** `LoRa.h`, `LoRaBoards.h` 등 다양한 라이브러리를 포함하고 있으며, 특히 LoRa 통신과 관련된 기능을 제공합니다.
- **하드웨어:** ESP32 기반 LilyGo T3S3 보드, LoRa 모듈(SX1276/SX1278), OLED 디스플레이, SD 카드, GPS 모듈 등 다양한 주변 장치를 사용합니다.

---

## **2. Sender.ino 분석**

`Sender.ino`는 LoRa 송신기의 핵심 동작을 정의하는 파일입니다. 아래에서 주요 부분을 단계별로 살펴보겠습니다.

### **2.1. 라이브러리 및 헤더 파일 포함**

```cpp
#include <LoRa.h>
#include "LoRaBoards.h"
```

- **`<LoRa.h>`:** LoRa 통신을 위한 라이브러리로, LoRa 모듈과의 통신을 관리합니다.
- **`"LoRaBoards.h"`:** 하드웨어 보드 초기화와 관련된 함수와 변수들을 선언한 헤더 파일입니다.

### **2.2. 매크로 정의 및 조건부 컴파일**

```cpp
#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           921.1
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   17
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif
```

- **`#ifndef`**와 **`#define`**은 조건부 컴파일 지시어로, 특정 매크로가 정의되지 않은 경우에만 해당 매크로를 정의합니다.
- 여기서는 LoRa 통신에 필요한 주파수(`CONFIG_RADIO_FREQ`), 출력 전력(`CONFIG_RADIO_OUTPUT_POWER`), 대역폭(`CONFIG_RADIO_BW`)을 설정하고 있습니다.
- **예시:**
  - `CONFIG_RADIO_FREQ`가 이미 다른 곳에서 정의되어 있지 않다면 921.1MHz로 설정됩니다.

### **2.3. LoRa 모듈 호환성 체크**

```cpp
#if !defined(USING_SX1276) && !defined(USING_SX1278)
#error "LoRa example is only allowed to run SX1276/78. For other RF models, please run examples/RadioLibExamples"
#endif
```

- 이 부분은 현재 사용 중인 LoRa 모듈이 SX1276 또는 SX1278인지 확인합니다.
- 만약 그렇지 않다면 컴파일 시 에러 메시지를 출력하고 중단합니다.
- 이는 코드가 특정 LoRa 모듈에 맞춰 작성되었기 때문에, 호환되지 않는 모듈에서 실행되지 않도록 방지하는 역할을 합니다.

### **2.4. 전역 변수 정의**

```cpp
int counter = 0;
```

- **`counter`**는 송신되는 패킷의 번호를 카운트하는 변수입니다.

### **2.5. `setup()` 함수**

`setup()` 함수는 Arduino 프로그램에서 처음 한 번만 실행되며, 초기 설정을 담당합니다.

```cpp
void setup()
{
    setupBoards();
    // When the power is turned on, a delay is required.
    delay(1500);

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

    Serial.println("LoRa Sender");
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(CONFIG_RADIO_FREQ * 1000000)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    LoRa.setTxPower(CONFIG_RADIO_OUTPUT_POWER);
    LoRa.setSignalBandwidth(CONFIG_RADIO_BW * 1000);
    LoRa.setSpreadingFactor(10);
    LoRa.setPreambleLength(16);
    LoRa.setSyncWord(0xAB);
    LoRa.disableCrc();
    LoRa.disableInvertIQ();
    LoRa.setCodingRate4(7);
}
```

#### **2.5.1. `setupBoards()` 호출**

- **`setupBoards()`** 함수는 `LoRaBoards.cpp`에서 구현된 함수로, 보드의 초기 설정과 주변 장치들을 초기화합니다.
- 여기서는 전원 관리, 디스플레이 초기화, WiFi 설정 등 다양한 설정을 수행합니다.

#### **2.5.2. 전원 초기화 지연**

```cpp
delay(1500);
```

- 전원이 켜진 후 안정화되기 위해 1.5초의 지연을 줍니다.

#### **2.5.3. 외부 클럭 활성화 (선택적)**

```cpp
#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif
```

- **`RADIO_TCXO_ENABLE`**가 정의된 경우, 외부 클럭을 활성화합니다.
- 이는 LoRa 모듈의 안정적인 동작을 위해 필요할 수 있습니다.

#### **2.5.4. 시리얼 통신 시작 및 LoRa 초기화**

```cpp
Serial.println("LoRa Sender");
LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
if (!LoRa.begin(CONFIG_RADIO_FREQ * 1000000)) {
    Serial.println("Starting LoRa failed!");
    while (1);
}
```

- **`Serial.println("LoRa Sender");`**: 시리얼 모니터에 "LoRa Sender" 메시지를 출력하여 송신기임을 알립니다.
- **`LoRa.setPins(...)`**: LoRa 모듈과의 핀 연결을 설정합니다.
  - `RADIO_CS_PIN`: LoRa 모듈의 칩 선택(Chip Select) 핀
  - `RADIO_RST_PIN`: LoRa 모듈의 리셋 핀
  - `RADIO_DIO0_PIN`: LoRa 모듈의 인터럽트 핀
- **`LoRa.begin(...)`**: LoRa 통신을 초기화합니다.
  - 설정한 주파수(`CONFIG_RADIO_FREQ * 1000000`)로 시작하며, 921.1MHz를 Hz 단위로 변환하여 전달합니다.
  - 초기화에 실패하면 "Starting LoRa failed!" 메시지를 출력하고 무한 루프에 빠집니다.

#### **2.5.5. LoRa 통신 설정**

```cpp
LoRa.setTxPower(CONFIG_RADIO_OUTPUT_POWER);
LoRa.setSignalBandwidth(CONFIG_RADIO_BW * 1000);
LoRa.setSpreadingFactor(10);
LoRa.setPreambleLength(16);
LoRa.setSyncWord(0xAB);
LoRa.disableCrc();
LoRa.disableInvertIQ();
LoRa.setCodingRate4(7);
```

- **`LoRa.setTxPower(...)`**: 출력 전력을 설정합니다. 여기서는 17dBm으로 설정되어 있습니다.
- **`LoRa.setSignalBandwidth(...)`**: 대역폭을 설정합니다. 125kHz로 설정되어 있으며, LoRa 데이터 전송률과 통신 거리에 영향을 미칩니다.
- **`LoRa.setSpreadingFactor(...)`**: 스프레딩 팩터를 설정합니다. 여기서는 10으로 설정되어 있으며, 통신 거리와 데이터 전송률에 영향을 미칩니다.
- **`LoRa.setPreambleLength(...)`**: 프리앰블 길이를 설정합니다. 프리앰블은 패킷의 시작 부분으로, 동기화를 돕습니다.
- **`LoRa.setSyncWord(...)`**: 동기 단어를 설정합니다. 0xAB로 설정되어 있으며, 네트워크 간 간섭을 줄이는 데 사용됩니다.
- **`LoRa.disableCrc();`**: CRC(순환 중복 검사)를 비활성화합니다. CRC는 데이터의 무결성을 확인하는 데 사용되지만, 여기서는 비활성화되었습니다.
- **`LoRa.disableInvertIQ();`**: IQ 신호를 반전시키는 기능을 비활성화합니다.
- **`LoRa.setCodingRate4(...)`**: 코딩 레이트를 설정합니다. 여기서는 7로 설정되어 있으며, 데이터 전송의 신뢰성과 전송률에 영향을 미칩니다.

### **2.6. `loop()` 함수**

`loop()` 함수는 Arduino 프로그램에서 반복적으로 실행되며, 주로 주요 작업을 수행합니다.

```cpp
void loop()
{
    Serial.print("Sending packet: ");
    Serial.println(counter);

    // send packet
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();

    if (u8g2) {
        char buf[256];
        u8g2->clearBuffer();
        u8g2->drawStr(0, 12, "Transmitting: OK!");
        snprintf(buf, sizeof(buf), "Sending: %d", counter);
        u8g2->drawStr(0, 30, buf);
        u8g2->sendBuffer();
    }
    counter++;
    delay(5000);
}
```

#### **2.6.1. 패킷 송신 로그 출력**

```cpp
Serial.print("Sending packet: ");
Serial.println(counter);
```

- **`counter`** 값을 시리얼 모니터에 출력하여 어떤 패킷이 송신되고 있는지 확인할 수 있습니다.

#### **2.6.2. 패킷 송신**

```cpp
LoRa.beginPacket();
LoRa.print("hello ");
LoRa.print(counter);
LoRa.endPacket();
```

- **`LoRa.beginPacket();`**: 새로운 패킷 전송을 시작합니다.
- **`LoRa.print("hello ");`**와 **`LoRa.print(counter);`**: "hello "와 현재 카운터 값을 패킷에 추가합니다.
- **`LoRa.endPacket();`**: 패킷 전송을 완료하고 실제로 데이터를 송신합니다.

#### **2.6.3. OLED 디스플레이 업데이트 (선택적)**

```cpp
if (u8g2) {
    char buf[256];
    u8g2->clearBuffer();
    u8g2->drawStr(0, 12, "Transmitting: OK!");
    snprintf(buf, sizeof(buf), "Sending: %d", counter);
    u8g2->drawStr(0, 30, buf);
    u8g2->sendBuffer();
}
```

- **`u8g2`**는 OLED 디스플레이 객체입니다. 만약 디스플레이가 초기화되었다면(`u8g2`가 `NULL`이 아니면), 화면을 업데이트합니다.
- **`clearBuffer()`**: 디스플레이 버퍼를 지웁니다.
- **`drawStr(...)`**: 특정 위치에 문자열을 그립니다.
  - "Transmitting: OK!"와 "Sending: [counter]" 메시지를 표시합니다.
- **`sendBuffer()`**: 버퍼에 그려진 내용을 실제 디스플레이에 출력합니다.

#### **2.6.4. 카운터 증가 및 지연**

```cpp
counter++;
delay(5000);
```

- **`counter++`**: 패킷 번호를 증가시킵니다.
- **`delay(5000);`**: 5초간 지연을 주어 패킷 송신 간격을 조절합니다.

---

## **3. LoRaBoards.h 분석**

`LoRaBoards.h`는 `LoRaBoards.cpp`에서 구현된 함수들과 전역 변수, 매크로 정의 등을 포함한 헤더 파일입니다. 주요 내용을 살펴보겠습니다.

```cpp
#pragma once

#include "utilities.h"

#ifdef HAS_SDCARD
#include <SD.h>
#endif

#if defined(ARDUINO_ARCH_ESP32)  
#include <FS.h>
#include <WiFi.h>
#endif

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <XPowersLib.h>

#ifndef DISPLAY_MODEL
#define DISPLAY_MODEL           U8G2_SSD1306_128X64_NONAME_F_HW_I2C
#endif

#ifndef OLED_WIRE_PORT
#define OLED_WIRE_PORT          Wire
#endif

#ifndef PMU_WIRE_PORT
#define PMU_WIRE_PORT           Wire
#endif

#ifndef DISPLAY_ADDR
#define DISPLAY_ADDR            0x3C
#endif

#ifndef LORA_FREQ_CONFIG
#define LORA_FREQ_CONFIG        915.0
#endif
```

### **3.1. 헤더 보호 및 기본 설정**

- **`#pragma once`**: 헤더 파일이 여러 번 포함되는 것을 방지합니다.
- **`#include "utilities.h"`**: 추가적인 유틸리티 함수들이 포함된 헤더 파일입니다.
- **조건부 포함:** `HAS_SDCARD`, `ARDUINO_ARCH_ESP32` 등의 매크로가 정의된 경우에만 해당 라이브러리를 포함합니다.

### **3.2. 매크로 정의**

- **`DISPLAY_MODEL`**: OLED 디스플레이 모델을 정의합니다. 기본값은 `U8G2_SSD1306_128X64_NONAME_F_HW_I2C`입니다.
- **`OLED_WIRE_PORT`**: OLED 디스플레이가 연결된 I2C 포트를 정의합니다. 기본값은 `Wire`입니다.
- **`PMU_WIRE_PORT`**: 전원 관리 유닛(PMU)이 연결된 I2C 포트를 정의합니다. 기본값은 `Wire`입니다.
- **`DISPLAY_ADDR`**: OLED 디스플레이의 I2C 주소를 정의합니다. 기본값은 `0x3C`입니다.
- **`LORA_FREQ_CONFIG`**: LoRa 주파수를 정의합니다. 기본값은 915.0MHz입니다.

### **3.3. 데이터 구조 정의**

```cpp
typedef struct {
    String          chipModel;
    float           psramSize;
    uint8_t         chipModelRev;
    uint8_t         chipFreq;
    uint8_t         flashSize;
    uint8_t         flashSpeed;
} DevInfo_t;
```

- **`DevInfo_t`**: 디바이스의 정보를 저장하는 구조체입니다.
  - **`chipModel`**: 칩 모델명
  - **`psramSize`**: PSRAM 크기
  - **`chipModelRev`**: 칩 모델 리비전
  - **`chipFreq`**: 칩 주파수
  - **`flashSize`**: 플래시 메모리 크기
  - **`flashSpeed`**: 플래시 메모리 속도

### **3.4. 함수 선언**

```cpp
void setupBoards(bool disable_u8g2 = false);

bool beginSDCard();

bool beginDisplay();

void disablePeripherals();

bool beginPower();

void printResult(bool radio_online);

void flashLed();

void scanDevices(TwoWire *w);

bool beginGPS();

void loopPMU();
```

- **`setupBoards(...)`**: 보드와 주변 장치들을 초기화하는 함수입니다.
- **`beginSDCard()`**: SD 카드 초기화 함수입니다.
- **`beginDisplay()`**: OLED 디스플레이 초기화 함수입니다.
- **`disablePeripherals()`**: 주변 장치를 비활성화하는 함수입니다.
- **`beginPower()`**: 전원 관리 초기화 함수입니다.
- **`printResult(...)`**: 다양한 하드웨어의 상태를 출력하는 함수입니다.
- **`flashLed()`**: LED를 깜빡이게 하는 함수입니다.
- **`scanDevices(...)`**: I2C 장치들을 스캔하는 함수입니다.
- **`beginGPS()`**: GPS 모듈 초기화 함수입니다.
- **`loopPMU()`**: 전원 관리 유닛(PMU)의 상태를 주기적으로 확인하는 함수입니다.

### **3.5. 전역 변수 선언**

```cpp
#ifdef HAS_PMU
extern XPowersLibInterface *PMU;
extern bool pmuInterrupt;
#endif
extern DISPLAY_MODEL *u8g2;
```

- **`PMU`**: 전원 관리 유닛을 제어하는 객체입니다.
- **`pmuInterrupt`**: PMU의 인터럽트 플래그입니다.
- **`u8g2`**: OLED 디스플레이 객체입니다.

### **3.6. 매크로 정의**

```cpp
#define U8G2_HOR_ALIGN_CENTER(t)    ((u8g2->getDisplayWidth() -  (u8g2->getUTF8Width(t))) / 2)
#define U8G2_HOR_ALIGN_RIGHT(t)     ( u8g2->getDisplayWidth()  -  u8g2->getUTF8Width(t))
```

- **`U8G2_HOR_ALIGN_CENTER(t)`**: 문자열 `t`를 디스플레이의 중앙에 정렬하기 위한 계산식입니다.
- **`U8G2_HOR_ALIGN_RIGHT(t)`**: 문자열 `t`를 디스플레이의 오른쪽에 정렬하기 위한 계산식입니다.

### **3.7. 보드 및 주변 장치 매크로**

```cpp
#if defined(ARDUINO_ARCH_ESP32)

#if defined(HAS_SDCARD)
extern SPIClass SDCardSPI;
#endif

#define SerialGPS Serial1
#elif defined(ARDUINO_ARCH_STM32)
extern HardwareSerial  SerialGPS;
#endif
```

- **ESP32 아키텍처:**
  - **`SDCardSPI`**: SD 카드용 SPI 클래스 객체입니다.
  - **`SerialGPS`**: GPS 모듈과 통신하기 위한 시리얼 인터페이스입니다.
- **STM32 아키텍처:**
  - **`SerialGPS`**: GPS 모듈과 통신하기 위한 시리얼 인터페이스입니다.

---

## **4. LoRaBoards.cpp 분석**

`LoRaBoards.cpp`는 `LoRaBoards.h`에서 선언한 함수들의 구현을 포함하는 파일입니다. 주요 내용을 살펴보겠습니다.

### **4.1. 라이브러리 및 헤더 파일 포함**

```cpp
#include "LoRaBoards.h"

#if defined(HAS_SDCARD)
SPIClass SDCardSPI(HSPI);
#endif

#if defined(ARDUINO_ARCH_STM32)
HardwareSerial  SerialGPS(GPS_RX_PIN, GPS_TX_PIN);
#endif

#if defined(ARDUINO_ARCH_ESP32)
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#include "hal/gpio_hal.h"
#endif
#include "driver/gpio.h"
#endif //ARDUINO_ARCH_ESP32

DISPLAY_MODEL *u8g2 = NULL;
static DevInfo_t  devInfo;

#ifdef HAS_GPS
static bool find_gps = false;
#endif

#ifdef HAS_PMU
XPowersLibInterface *PMU = NULL;
bool     pmuInterrupt;

static void setPmuFlag()
{
    pmuInterrupt = true;
}
#endif
```

- **`DISPLAY_MODEL *u8g2 = NULL;`**: OLED 디스플레이 객체의 포인터를 초기화합니다.
- **`DevInfo_t devInfo;`**: 디바이스 정보를 저장하는 구조체 변수입니다.
- **GPS 관련 변수:** GPS 모듈이 있는 경우, `find_gps` 변수를 사용하여 GPS 모듈의 상태를 추적합니다.
- **PMU 관련 변수:** PMU 객체와 인터럽트 플래그를 선언합니다. `setPmuFlag()` 함수는 PMU 인터럽트가 발생했을 때 호출됩니다.

### **4.2. `beginPower()` 함수**

전원 관리 유닛(PMU)을 초기화하고 설정하는 함수입니다.

```cpp
bool beginPower()
{
#ifdef HAS_PMU
    if (!PMU) {
        PMU = new XPowersAXP2101(PMU_WIRE_PORT);
        if (!PMU->init()) {
            Serial.println("Warning: Failed to find AXP2101 power management");
            delete PMU;
            PMU = NULL;
        } else {
            Serial.println("AXP2101 PMU init succeeded, using AXP2101 PMU");
        }
    }

    if (!PMU) {
        PMU = new XPowersAXP192(PMU_WIRE_PORT);
        if (!PMU->init()) {
            Serial.println("Warning: Failed to find AXP192 power management");
            delete PMU;
            PMU = NULL;
        } else {
            Serial.println("AXP192 PMU init succeeded, using AXP192 PMU");
        }
    }

    if (!PMU) {
        return false;
    }

    PMU->setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

    pinMode(PMU_IRQ, INPUT_PULLUP);
    attachInterrupt(PMU_IRQ, setPmuFlag, FALLING);
    
    // ... (여기서는 AXP192와 AXP2101 칩 모델에 따라 전원 채널을 설정합니다.)

    PMU->enableSystemVoltageMeasure();
    PMU->enableVbusVoltageMeasure();
    PMU->enableBattVoltageMeasure();

    // 전원 채널 상태 출력
    Serial.printf("=========================================\n");
    if (PMU->isChannelAvailable(XPOWERS_DCDC1)) {
        Serial.printf("DC1  : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_DCDC1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC1));
    }
    // ... (다른 전원 채널들도 유사하게 출력)
    Serial.printf("=========================================\n");

    // 전원 키 설정
    PMU->setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
    uint8_t opt = PMU->getPowerKeyPressOffTime();
    Serial.print("PowerKeyPressOffTime:");
    switch (opt) {
    case XPOWERS_POWEROFF_4S: Serial.println("4 Second");
        break;
    case XPOWERS_POWEROFF_6S: Serial.println("6 Second");
        break;
    case XPOWERS_POWEROFF_8S: Serial.println("8 Second");
        break;
    case XPOWERS_POWEROFF_10S: Serial.println("10 Second");
        break;
    default:
        break;
    }
#endif
    return true;
}
```

#### **4.2.1. PMU 객체 초기화**

- **AXP2101 초기화:**
  - **`new XPowersAXP2101(PMU_WIRE_PORT);`**: AXP2101 PMU 객체를 생성합니다.
  - **`PMU->init();`**: PMU를 초기화합니다.
  - 초기화에 실패하면 객체를 삭제하고 `NULL`로 설정합니다.

- **AXP192 초기화:**
  - AXP2101 초기화에 실패한 경우, AXP192 PMU 객체를 생성하고 초기화합니다.
  - 초기화에 실패하면 객체를 삭제하고 `NULL`로 설정합니다.

#### **4.2.2. 인터럽트 설정**

```cpp
PMU->setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

pinMode(PMU_IRQ, INPUT_PULLUP);
attachInterrupt(PMU_IRQ, setPmuFlag, FALLING);
```

- **충전 LED 모드 설정:** 충전 상태를 LED로 표시합니다.
- **인터럽트 핀 설정:** PMU의 인터럽트 핀을 입력 풀업 모드로 설정하고, 핀에 인터럽트가 발생하면 `setPmuFlag()` 함수를 호출하도록 설정합니다.

#### **4.2.3. 전원 채널 설정**

- PMU의 칩 모델(AXP192 또는 AXP2101)에 따라 LoRa, GPS, OLED 등 각 장치의 전원 채널을 설정하고 활성화합니다.
- 필요 없는 전원 채널은 비활성화합니다.

#### **4.2.4. 전원 상태 출력**

```cpp
Serial.printf("=========================================\n");
if (PMU->isChannelAvailable(XPOWERS_DCDC1)) {
    Serial.printf("DC1  : %s   Voltage: %04u mV \n",  PMU->isPowerChannelEnable(XPOWERS_DCDC1)  ? "+" : "-",  PMU->getPowerChannelVoltage(XPOWERS_DCDC1));
}
// ... (다른 전원 채널들도 유사하게 출력)
Serial.printf("=========================================\n");
```

- 활성화된 전원 채널의 상태와 전압을 시리얼 모니터에 출력합니다.

#### **4.2.5. 전원 키 설정**

```cpp
PMU->setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);
uint8_t opt = PMU->getPowerKeyPressOffTime();
Serial.print("PowerKeyPressOffTime:");
switch (opt) {
case XPOWERS_POWEROFF_4S: Serial.println("4 Second");
    break;
case XPOWERS_POWEROFF_6S: Serial.println("6 Second");
    break;
case XPOWERS_POWEROFF_8S: Serial.println("8 Second");
    break;
case XPOWERS_POWEROFF_10S: Serial.println("10 Second");
    break;
default:
    break;
}
```

- 전원 키를 누를 때 시스템이 꺼지기까지의 시간을 설정하고, 이를 시리얼 모니터에 출력합니다.

### **4.3. `disablePeripherals()` 함수**

주변 장치들을 비활성화하는 함수입니다.

```cpp
void disablePeripherals()
{
#ifdef HAS_PMU
    if (!PMU)return;
#if defined(T_BEAM_S3_BPF)
    PMU->disablePowerOutput(XPOWERS_ALDO4); //gps
    PMU->disablePowerOutput(XPOWERS_ALDO2); //Sdcard
    PMU->disablePowerOutput(XPOWERS_DCDC3); // Extern Power source
    PMU->disablePowerOutput(XPOWERS_DCDC5);
    PMU->disablePowerOutput(XPOWERS_ALDO1);
#endif
#endif
}
```

- **PMU가 있는 경우:** PMU 객체가 존재하면 특정 전원 채널들을 비활성화합니다.
- **예시:** GPS, SD 카드, 외부 전원 소스 등을 비활성화합니다.

### **4.4. `loopPMU()` 함수**

PMU의 인터럽트 상태를 처리하는 함수입니다.

```cpp
void loopPMU()
{
#ifdef HAS_PMU
    if (!PMU) {
        return;
    }
    if (!pmuInterrupt) {
        return;
    }

    pmuInterrupt = false;
    // Get PMU Interrupt Status Register
    uint32_t status = PMU->getIrqStatus();
    Serial.print("STATUS => HEX:");
    Serial.print(status, HEX);
    Serial.print(" BIN:");
    Serial.println(status, BIN);

    if (PMU->isVbusInsertIrq()) {
        Serial.println("isVbusInsert");
    }
    // ... (다른 인터럽트 상태도 유사하게 처리)
    // Clear PMU Interrupt Status Register
    PMU->clearIrqStatus();
#endif
}
```

- **인터럽트 발생 시:** `pmuInterrupt`가 `true`일 때 PMU의 인터럽트 상태를 확인하고, 해당 상태에 따라 시리얼 모니터에 메시지를 출력합니다.
- **인터럽트 플래그 초기화:** 인터럽트 처리가 끝난 후, `pmuInterrupt`를 `false`로 설정합니다.

### **4.5. `beginDisplay()` 함수**

OLED 디스플레이를 초기화하는 함수입니다.

```cpp
bool beginDisplay()
{
    Wire.beginTransmission(DISPLAY_ADDR);
    if (Wire.endTransmission() == 0) {
        Serial.printf("Find Display model at 0x%X address\n", DISPLAY_ADDR);
        u8g2 = new DISPLAY_MODEL(U8G2_R0, U8X8_PIN_NONE);
        u8g2->begin();
        u8g2->clearBuffer();
        u8g2->setFont(u8g2_font_inb19_mr);
        u8g2->drawStr(0, 30, "LilyGo");
        u8g2->drawHLine(2, 35, 47);
        u8g2->drawHLine(3, 36, 47);
        u8g2->drawVLine(45, 32, 12);
        u8g2->drawVLine(46, 33, 12);
        u8g2->setFont(u8g2_font_inb19_mf);
        u8g2->drawStr(58, 60, "LoRa");
        u8g2->sendBuffer();
        u8g2->setFont(u8g2_font_fur11_tf);
        delay(3000);
        return true;
    }

    Serial.printf("Warning: Failed to find Display at 0x%0X address\n", DISPLAY_ADDR);
    return false;
}
```

#### **4.5.1. 디스플레이 존재 여부 확인**

```cpp
Wire.beginTransmission(DISPLAY_ADDR);
if (Wire.endTransmission() == 0) {
    // 디스플레이가 정상적으로 연결됨
}
```

- I2C 주소(`DISPLAY_ADDR`)로 디스플레이에 통신을 시도하고, 성공하면 디스플레이가 연결된 것으로 간주합니다.

#### **4.5.2. 디스플레이 초기화 및 초기 화면 표시**

```cpp
u8g2 = new DISPLAY_MODEL(U8G2_R0, U8X8_PIN_NONE);
u8g2->begin();
u8g2->clearBuffer();
u8g2->setFont(u8g2_font_inb19_mr);
u8g2->drawStr(0, 30, "LilyGo");
// ... (다른 그래픽 요소들)
u8g2->sendBuffer();
```

- **`new DISPLAY_MODEL(...)`**: 지정된 디스플레이 모델로 객체를 생성합니다.
- **`begin()`**: 디스플레이 초기화
- **`clearBuffer()`**: 디스플레이 버퍼 초기화
- **`setFont(...)`**, **`drawStr(...)`**, **`drawHLine(...)`**, **`drawVLine(...)`**: 텍스트와 그래픽을 그립니다.
- **`sendBuffer()`**: 버퍼 내용을 실제 디스플레이에 출력합니다.

#### **4.5.3. 초기화 실패 시**

```cpp
Serial.printf("Warning: Failed to find Display at 0x%0X address\n", DISPLAY_ADDR);
return false;
```

- 디스플레이 초기화에 실패하면 경고 메시지를 출력하고 `false`를 반환합니다.

### **4.6. `beginSDCard()` 함수**

SD 카드를 초기화하는 함수입니다.

```cpp
bool beginSDCard()
{
#ifdef SDCARD_CS
    if (SD.begin(SDCARD_CS, SDCardSPI)) {
        uint32_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.print("Sd Card init succeeded, The current available capacity is ");
        Serial.print(cardSize / 1024.0);
        Serial.println(" GB");
        return true;
    } else {
        Serial.println("Warning: Failed to init Sd Card");
    }
#endif
    return false;
}
```

- **`SD.begin(...)`**: SD 카드 초기화 시도
  - `SDCARD_CS`: SD 카드의 칩 선택 핀
  - `SDCardSPI`: SD 카드용 SPI 클래스 객체
- 초기화에 성공하면 SD 카드의 용량을 계산하여 출력하고 `true`를 반환합니다.
- 실패하면 경고 메시지를 출력하고 `false`를 반환합니다.

### **4.7. `beginWiFi()` 함수**

WiFi를 초기화하고 Soft AP(Access Point)를 설정하는 함수입니다.

```cpp
void beginWiFi()
{
    if (!WiFi.softAP(BOARD_VARIANT_NAME)) {
        log_e("Soft AP creation failed.");
    }
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
}
```

- **`WiFi.softAP(...)`**: WiFi를 Soft AP 모드로 설정하고, SSID를 `BOARD_VARIANT_NAME`으로 지정합니다.
- 성공하면 AP의 IP 주소를 시리얼 모니터에 출력합니다.
- 실패하면 오류 로그를 출력합니다.

### **4.8. `printWakeupReason()` 함수**

ESP32가 깨어난 원인을 출력하는 함수입니다.

```cpp
void printWakeupReason()
{
#ifdef ESP32
    Serial.print("Reset reason:");
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_UNDEFINED:
        Serial.println(" In case of deep sleep, reset was not caused by exit from deep sleep");
        break;
    // ... (다른 원인들도 유사하게 처리)
    default :
        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
        break;
    }
#endif
}
```

- **`esp_sleep_get_wakeup_cause()`**: ESP32가 깨어난 원인을 반환합니다.
- 반환된 원인에 따라 시리얼 모니터에 메시지를 출력합니다.

### **4.9. `getChipInfo()` 함수**

디바이스의 하드웨어 정보를 출력하는 함수입니다.

```cpp
void getChipInfo()
{
#if defined(ARDUINO_ARCH_ESP32)

    Serial.println("-----------------------------------");

    printWakeupReason();

#if defined(CONFIG_IDF_TARGET_ESP32)  ||  defined(CONFIG_IDF_TARGET_ESP32S3)

    if (psramFound()) {
        uint32_t psram = ESP.getPsramSize();
        devInfo.psramSize = psram / 1024.0 / 1024.0;
        Serial.printf("PSRAM is enable! PSRAM: %.2fMB\n", devInfo.psramSize);
    } else {
        Serial.println("PSRAM is disable!");
        devInfo.psramSize = 0;
    }

#endif

    Serial.print("Flash:");
    devInfo.flashSize       = ESP.getFlashChipSize() / 1024.0 / 1024.0;
    devInfo.flashSpeed      = ESP.getFlashChipSpeed() / 1000 / 1000;
    devInfo.chipModel       = ESP.getChipModel();
    devInfo.chipModelRev    = ESP.getChipRevision();
    devInfo.chipFreq        = ESP.getCpuFreqMHz();

    Serial.print(devInfo.flashSize);
    Serial.println(" MB");
    Serial.print("Flash speed:");
    Serial.print(devInfo.flashSpeed);
    Serial.println(" M");
    Serial.print("Model:");

    Serial.println(devInfo.chipModel);
    Serial.print("Chip Revision:");
    Serial.println(devInfo.chipModelRev);
    Serial.print("Freq:");
    Serial.print(devInfo.chipFreq);
    Serial.println(" MHZ");
    Serial.print("SDK Ver:");
    Serial.println(ESP.getSdkVersion());
    Serial.print("DATE:");
    Serial.println(__DATE__);
    Serial.print("TIME:");
    Serial.println(__TIME__);

    Serial.print("EFUSE MAC: ");
    Serial.print( ESP.getEfuseMac(), HEX);
    Serial.println();

    Serial.println("-----------------------------------");

#elif defined(ARDUINO_ARCH_STM32)
    uint32_t uid[3];

    uid[0] = HAL_GetUIDw0();
    uid[1] = HAL_GetUIDw1();
    uid[2] = HAL_GetUIDw2();
    Serial.print("STM UID: 0X");
    Serial.print( uid[0], HEX);
    Serial.print( uid[1], HEX);
    Serial.print( uid[2], HEX);
    Serial.println();
#endif
}
```

- **ESP32 아키텍처:**
  - 깨어난 원인 출력
  - PSRAM 여부 확인 및 크기 출력
  - 플래시 메모리 크기, 속도, 칩 모델, 리비전, 주파수 등 정보 출력
  - SDK 버전, 컴파일 날짜 및 시간, MAC 주소 출력
- **STM32 아키텍처:**
  - 고유 ID(UID)를 출력

### **4.10. `setupBoards()` 함수**

보드와 주변 장치들을 초기화하는 함수입니다.

```cpp
void setupBoards(bool disable_u8g2 )
{
    Serial.begin(115200);

    // while (!Serial);

    Serial.println("setupBoards");

    getChipInfo();

#if defined(ARDUINO_ARCH_ESP32)
    SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);
#elif defined(ARDUINO_ARCH_STM32)
    SPI.setMISO(RADIO_MISO_PIN);
    SPI.setMOSI(RADIO_MOSI_PIN);
    SPI.setSCLK(RADIO_SCLK_PIN);
    SPI.begin();
#endif

#ifdef HAS_SDCARD
    SDCardSPI.begin(SDCARD_SCLK, SDCARD_MISO, SDCARD_MOSI);
#endif

#ifdef I2C_SDA
    Wire.begin(I2C_SDA, I2C_SCL);
    scanDevices(&Wire);
#endif

#ifdef I2C1_SDA
    Wire1.begin(I2C1_SDA, I2C1_SCL);
#endif

#ifdef HAS_GPS
#if defined(ARDUINO_ARCH_ESP32)
    SerialGPS.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
#elif defined(ARDUINO_ARCH_STM32)
    SerialGPS.setRx(GPS_RX_PIN);
    SerialGPS.setTx(GPS_TX_PIN);
    SerialGPS.begin(GPS_BAUD_RATE);
#endif // ARDUINO_ARCH_
#endif // HAS_GPS

#if OLED_RST
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, HIGH); delay(20);
    digitalWrite(OLED_RST, LOW);  delay(20);
    digitalWrite(OLED_RST, HIGH); delay(20);
#endif

#ifdef BOARD_LED
    /*
    * T-Beam LED defaults to low level as turn on,
    * so it needs to be forced to pull up
    * * * * */
#if LED_ON == LOW
#if defined(ARDUINO_ARCH_ESP32)
    gpio_hold_dis((gpio_num_t)BOARD_LED);
#endif //ARDUINO_ARCH_ESP32
#endif

    pinMode(BOARD_LED, OUTPUT);
    digitalWrite(BOARD_LED, LED_ON);
#endif

#ifdef GPS_EN_PIN
    pinMode(GPS_EN_PIN, OUTPUT);
    digitalWrite(GPS_EN_PIN, HIGH);
#endif

#ifdef GPS_RST_PIN
    pinMode(GPS_RST_PIN, OUTPUT);
    digitalWrite(GPS_RST_PIN, HIGH);
#endif


#if defined(ARDUINO_ARCH_STM32)
    SerialGPS.println("@GSR"); delay(300);
    SerialGPS.println("@GSR"); delay(300);
    SerialGPS.println("@GSR"); delay(300);
    SerialGPS.println("@GSR"); delay(300);
    SerialGPS.println("@GSR"); delay(300);
#endif


#ifdef RADIO_LDO_EN
    pinMode(RADIO_LDO_EN, OUTPUT);
    digitalWrite(RADIO_LDO_EN, HIGH);
#endif

    beginPower();

    beginSDCard();

    if (!disable_u8g2) {
        beginDisplay();
    }

    beginWiFi();

#ifdef HAS_GPS
#ifdef T_BEAM_S3_BPF
    find_gps = beginGPS();
#endif
#endif

    Serial.println("init done . ");
}
```

#### **4.10.1. 시리얼 통신 시작**

```cpp
Serial.begin(115200);
Serial.println("setupBoards");
```

- 시리얼 통신을 115200 보레이트로 시작하고, "setupBoards" 메시지를 출력합니다.

#### **4.10.2. 하드웨어 초기화**

- **SPI 초기화:**
  - **ESP32 아키텍처:** `SPI.begin(RADIO_SCLK_PIN, RADIO_MISO_PIN, RADIO_MOSI_PIN);`
  - **STM32 아키텍처:** `SPI.setMISO(...)`, `SPI.setMOSI(...)`, `SPI.setSCLK(...)` 후 `SPI.begin();`
- **SD 카드 초기화:** `SDCardSPI.begin(...)` (선택적)
- **I2C 초기화:** `Wire.begin(...)`, `scanDevices(&Wire);` 등
- **GPS 초기화:** `SerialGPS.begin(...)` (선택적)
- **OLED 리셋:** OLED 디스플레이 리셋 핀을 제어하여 초기화합니다.
- **LED 설정:** 보드의 LED 핀을 출력 모드로 설정하고, LED를 켭니다.
- **GPS 전원 및 리셋 핀 설정:** GPS 모듈의 전원과 리셋 핀을 제어합니다.
- **RADIO_LDO_EN 핀 설정:** LoRa 모듈의 전원 제어 핀을 설정합니다.

#### **4.10.3. 전원 및 주변 장치 초기화**

```cpp
beginPower();
beginSDCard();
if (!disable_u8g2) {
    beginDisplay();
}
beginWiFi();
```

- **전원 관리:** PMU 초기화 및 설정
- **SD 카드 초기화:** SD 카드가 있으면 초기화
- **디스플레이 초기화:** `disable_u8g2`가 `false`이면 디스플레이 초기화
- **WiFi 초기화:** Soft AP 설정

#### **4.10.4. GPS 초기화 (선택적)**

```cpp
#ifdef HAS_GPS
#ifdef T_BEAM_S3_BPF
    find_gps = beginGPS();
#endif
#endif
```

- GPS 모듈이 있는 경우, GPS 초기화 함수를 호출하여 GPS 모듈을 초기화하고, 초기화 결과를 `find_gps`에 저장합니다.

#### **4.10.5. 초기화 완료 메시지**

```cpp
Serial.println("init done . ");
```

- 모든 초기화가 완료되었음을 시리얼 모니터에 출력합니다.

### **4.11. `printResult()` 함수**

하드웨어의 상태를 시리얼 모니터와 OLED 디스플레이에 출력하는 함수입니다.

```cpp
void printResult(bool radio_online)
{
    Serial.print("Radio        : ");
    Serial.println((radio_online) ? "+" : "-");

#if defined(CONFIG_IDF_TARGET_ESP32)  ||  defined(CONFIG_IDF_TARGET_ESP32S3)

    Serial.print("PSRAM        : ");
    Serial.println((psramFound()) ? "+" : "-");

    Serial.print("Display      : ");
    Serial.println(( u8g2) ? "+" : "-");

#ifdef HAS_SDCARD
    Serial.print("Sd Card      : ");
    Serial.println((SD.cardSize() != 0) ? "+" : "-");
#endif

#ifdef HAS_PMU
    Serial.print("Power        : ");
    Serial.println(( PMU ) ? "+" : "-");
#endif

#ifdef HAS_GPS
#ifdef T_BEAM_S3_BPF
    Serial.print("GPS          : ");
    Serial.println(( find_gps ) ? "+" : "-");
#endif
#endif

    if (u8g2) {

        u8g2->clearBuffer();
        u8g2->setFont(u8g2_font_NokiaLargeBold_tf );
        uint16_t str_w =  u8g2->getStrWidth(BOARD_VARIANT_NAME);
        u8g2->drawStr((u8g2->getWidth() - str_w) / 2, 16, BOARD_VARIANT_NAME);
        u8g2->drawHLine(5, 21, u8g2->getWidth() - 5);

        u8g2->drawStr( 0, 38, "Disp:");     u8g2->drawStr( 45, 38, ( u8g2) ? "+" : "-");

#ifdef HAS_SDCARD
        u8g2->drawStr( 0, 54, "SD :");      u8g2->drawStr( 45, 54, (SD.cardSize() != 0) ? "+" : "-");
#endif

        u8g2->drawStr( 62, 38, "Radio:");    u8g2->drawStr( 120, 38, ( radio_online ) ? "+" : "-");

#ifdef HAS_PMU
        u8g2->drawStr( 62, 54, "Power:");    u8g2->drawStr( 120, 54, ( PMU ) ? "+" : "-");
#endif

        u8g2->sendBuffer();

        delay(2000);
    }
#endif
}
```

#### **4.11.1. 시리얼 출력**

- **Radio 상태:** `radio_online` 매개변수를 통해 라디오(LoRa 모듈)의 상태를 출력합니다.
- **PSRAM 상태:** PSRAM이 활성화되어 있는지 여부를 출력합니다.
- **디스플레이 상태:** `u8g2` 객체가 존재하는지 여부를 출력합니다.
- **SD 카드 상태:** SD 카드가 초기화되었는지 여부를 출력합니다.
- **전원 상태:** PMU 객체가 존재하는지 여부를 출력합니다.
- **GPS 상태:** GPS 모듈이 초기화되었는지 여부를 출력합니다.

#### **4.11.2. OLED 디스플레이 출력**

- 각 하드웨어의 상태를 OLED 디스플레이에 출력합니다.
- **예시:**
  - **"Disp: +"**: 디스플레이가 활성화됨을 표시
  - **"SD : +"**: SD 카드가 활성화됨을 표시
  - **"Radio: +"**: LoRa 모듈이 활성화됨을 표시
  - **"Power: +"**: PMU가 활성화됨을 표시

### **4.12. `flashLed()` 함수**

LED를 깜빡이게 하는 함수입니다.

```cpp
void flashLed()
{
#ifdef BOARD_LED
    if ((millis() - lastDebounceTime) > debounceDelay) {
        ledState = !ledState;
        if (ledState) {
            digitalWrite(BOARD_LED, LED_ON);
        } else {
            digitalWrite(BOARD_LED, !LED_ON);
        }
        lastDebounceTime = millis();
    }
#endif
}
```

- **디바운스 처리:** LED 상태 변경 시 디바운스(노이즈 방지)를 위해 일정 시간 간격을 둡니다.
- **LED 상태 토글:** `ledState` 변수를 사용하여 LED를 켜고 끕니다.

### **4.13. `scanDevices()` 함수**

I2C 버스에 연결된 장치들을 스캔하여 시리얼 모니터에 출력하는 함수입니다.

```cpp
void scanDevices(TwoWire *w)
{
    uint8_t err, addr;
    int nDevices = 0;
    uint32_t start = 0;

    Serial.println("I2C Devices scanning");
    for (addr = 1; addr < 127; addr++) {
        start = millis();
        w->beginTransmission(addr); delay(2);
        err = w->endTransmission();
        if (err == 0) {
            nDevices++;
            switch (addr) {
            case 0x77:
            case 0x76:
                Serial.println("\tFind BMX280 Sensor!");
                break;
            case 0x34:
                Serial.println("\tFind AXP192/AXP2101 PMU!");
                break;
            case 0x3C:
                Serial.println("\tFind SSD1306/SH1106 dispaly!");
                break;
            case 0x51:
                Serial.println("\tFind PCF8563 RTC!");
                break;
            case 0x1C:
                Serial.println("\tFind QMC6310 MAG Sensor!");
                break;
            default:
                Serial.print("\tI2C device found at address 0x");
                if (addr < 16) {
                    Serial.print("0");
                }
                Serial.print(addr, HEX);
                Serial.println(" !");
                break;
            }

        } else if (err == 4) {
            Serial.print("Unknow error at address 0x");
            if (addr < 16) {
                Serial.print("0");
            }
            Serial.println(addr, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");

    Serial.println("Scan devices done.");
    Serial.println("\n");
}
```

- **I2C 주소 스캔:** 1부터 126까지의 주소를 순회하며 각 주소에 장치가 있는지 확인합니다.
- **장치 발견 시:** 특정 주소에 따라 장치의 종류를 출력합니다.
  - **예시:** 주소 `0x3C`에서 SSD1306 디스플레이를 발견하면 "Find SSD1306/SH1106 display!" 메시지를 출력합니다.
- **오류 처리:** 통신 오류가 발생하면 해당 주소와 함께 오류 메시지를 출력합니다.
- **