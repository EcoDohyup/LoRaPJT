
## **1. Receiver.ino 코드 분석**

`Receiver.ino`는 LoRa 수신기의 핵심 동작을 정의하는 파일로, Sender와 유사한 구조를 가지고 있습니다. 아래에서 주요 부분을 단계별로 살펴보겠습니다.

### **1.1. 라이브러리 및 헤더 파일 포함**

```cpp
#include <LoRa.h>
#include "LoRaBoards.h"
```

- **`<LoRa.h>`:** LoRa 통신을 위한 라이브러리로, LoRa 모듈과의 통신을 관리합니다.
- **`"LoRaBoards.h"`:** 하드웨어 보드 초기화와 관련된 함수와 변수들을 선언한 헤더 파일입니다.

### **1.2. 매크로 정의 및 조건부 컴파일**

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

### **1.3. LoRa 모듈 호환성 체크**

```cpp
#if !defined(USING_SX1276) && !defined(USING_SX1278)
#error "LoRa example is only allowed to run SX1276/78. For other RF models, please run examples/RadioLibExamples"
#endif
```

- 현재 사용 중인 LoRa 모듈이 SX1276 또는 SX1278인지 확인합니다.
- 만약 그렇지 않다면 컴파일 시 에러 메시지를 출력하고 중단합니다.
- 이는 코드가 특정 LoRa 모듈에 맞춰 작성되었기 때문에, 호환되지 않는 모듈에서 실행되지 않도록 방지하는 역할을 합니다.

### **1.4. `setup()` 함수**

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

    Serial.println("LoRa Receiver");

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

    // put the radio into receive mode
    LoRa.receive();
}
```

#### **1.4.1. `setupBoards()` 호출**

- **`setupBoards()`** 함수는 `LoRaBoards.cpp`에서 구현된 함수로, 보드의 초기 설정과 주변 장치들을 초기화합니다.
- 여기서는 전원 관리, 디스플레이 초기화, WiFi 설정 등 다양한 설정을 수행합니다.

#### **1.4.2. 전원 초기화 지연**

```cpp
delay(1500);
```

- 전원이 켜진 후 안정화되기 위해 1.5초의 지연을 줍니다.

#### **1.4.3. 외부 클럭 활성화 (선택적)**

```cpp
#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif
```

- **`RADIO_TCXO_ENABLE`**가 정의된 경우, 외부 클럭을 활성화합니다.
- 이는 LoRa 모듈의 안정적인 동작을 위해 필요할 수 있습니다.

#### **1.4.4. 시리얼 통신 초기화 및 LoRa 설정**

```cpp
Serial.println("LoRa Receiver");

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
```

- **`LoRa.setPins`**: LoRa 모듈과의 SPI 통신을 위한 핀을 설정합니다.
  - `RADIO_CS_PIN`: Chip Select 핀
  - `RADIO_RST_PIN`: Reset 핀
  - `RADIO_DIO0_PIN`: DIO0 핀 (인터럽트 핀)
- **`LoRa.begin`**: LoRa 모듈을 초기화하고 설정된 주파수로 시작합니다.
  - `CONFIG_RADIO_FREQ * 1000000`: 주파수를 Hz 단위로 설정 (예: 921.1MHz → 921100000Hz)
  - 초기화에 실패하면 에러 메시지를 출력하고 무한 루프에 빠집니다.
- **LoRa 설정**:
  - **출력 전력**: `LoRa.setTxPower(CONFIG_RADIO_OUTPUT_POWER);`
  - **대역폭**: `LoRa.setSignalBandwidth(CONFIG_RADIO_BW * 1000);`
  - **스프레딩 팩터**: `LoRa.setSpreadingFactor(10);`
  - **프리앰블 길이**: `LoRa.setPreambleLength(16);`
  - **동기 단어**: `LoRa.setSyncWord(0xAB);`
  - **CRC 비활성화**: `LoRa.disableCrc();`
  - **Invert IQ 비활성화**: `LoRa.disableInvertIQ();`
  - **코딩 레이트**: `LoRa.setCodingRate4(7);`

#### **1.4.5. 수신 모드로 전환**

```cpp
LoRa.receive();
```

- LoRa 모듈을 수신 모드로 전환하여 패킷 수신을 대기합니다.

### **1.5. `loop()` 함수**

`loop()` 함수는 Arduino 프로그램에서 반복적으로 실행되는 함수로, 여기서는 LoRa 패킷을 수신하고 처리하는 역할을 합니다.

```cpp
void loop()
{
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        // received a packet
        Serial.print("Received packet '");

        String recv = "";
        // read packet
        while (LoRa.available()) {
            recv += (char)LoRa.read();
        }

        Serial.println(recv);

        // print RSSI of packet
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
        if (u8g2) {
            u8g2->clearBuffer();
            char buf[256];
            u8g2->drawStr(0, 12, "Received OK!");
            u8g2->drawStr(0, 26, recv.c_str());
            snprintf(buf, sizeof(buf), "RSSI:%i", LoRa.packetRssi());
            u8g2->drawStr(0, 40, buf);
            snprintf(buf, sizeof(buf), "SNR:%.1f", LoRa.packetSnr());
            u8g2->drawStr(0, 56, buf);
            u8g2->sendBuffer();
        }
    }
}
```

#### **1.5.1. 패킷 수신 시도**

```cpp
int packetSize = LoRa.parsePacket();
if (packetSize) {
    // 패킷 수신 처리
}
```

- **`LoRa.parsePacket()`**: 수신된 패킷의 크기를 반환합니다. 패킷이 수신되었다면 패킷의 크기가 0보다 큰 값을 반환합니다.
- 패킷이 수신되었을 경우, 이후의 코드를 실행하여 패킷을 읽고 처리합니다.

#### **1.5.2. 패킷 읽기 및 출력**

```cpp
String recv = "";
// read packet
while (LoRa.available()) {
    recv += (char)LoRa.read();
}

Serial.println(recv);
```

- **`LoRa.available()`**: 수신된 패킷의 데이터가 읽을 준비가 되었는지 확인합니다.
- **`LoRa.read()`**: 수신된 데이터를 하나씩 읽어 `recv` 문자열에 저장합니다.
- 최종적으로 수신된 패킷 내용을 시리얼 모니터에 출력합니다.

#### **1.5.3. RSSI 및 SNR 출력**

```cpp
Serial.print("' with RSSI ");
Serial.println(LoRa.packetRssi());

if (u8g2) {
    u8g2->clearBuffer();
    char buf[256];
    u8g2->drawStr(0, 12, "Received OK!");
    u8g2->drawStr(0, 26, recv.c_str());
    snprintf(buf, sizeof(buf), "RSSI:%i", LoRa.packetRssi());
    u8g2->drawStr(0, 40, buf);
    snprintf(buf, sizeof(buf), "SNR:%.1f", LoRa.packetSnr());
    u8g2->drawStr(0, 56, buf);
    u8g2->sendBuffer();
}
```

- **RSSI (Received Signal Strength Indicator)**: 수신된 패킷의 신호 강도를 나타냅니다.
  - **`LoRa.packetRssi()`**: RSSI 값을 반환합니다.
- **SNR (Signal-to-Noise Ratio)**: 신호 대 잡음비를 나타내며, 통신 품질을 평가하는 지표입니다.
  - **`LoRa.packetSnr()`**: SNR 값을 반환합니다.
- **OLED 디스플레이 출력**:
  - **`u8g2`**: U8g2 라이브러리를 사용하여 OLED 디스플레이에 정보를 출력합니다.
  - **`drawStr(x, y, "텍스트")`**: OLED 화면의 (x, y) 위치에 문자열을 그립니다.
    - **`x`**: 가로 위치 (픽셀 단위)
    - **`y`**: 세로 위치 (픽셀 단위)
    - **`"텍스트"`**: 출력할 문자열
  - **`clearBuffer()`**: 이전에 그린 내용을 지웁니다.
  - **`sendBuffer()`**: 버퍼에 저장된 내용을 실제 화면에 출력합니다.
  - **`snprintf`**: 형식에 맞게 문자열을 생성하여 OLED에 출력합니다.

---

## **2. 주요 함수 및 라이브러리 설명**

### **2.1. `drawStr` 함수**

- **라이브러리**: `U8g2lib.h`
- **용도**: OLED 디스플레이에 문자열을 그리는 함수입니다.
- **매개변수**:
  - **`x`**: 문자열을 그릴 시작 가로 위치 (픽셀 단위)
  - **`y`**: 문자열을 그릴 시작 세로 위치 (픽셀 단위)
  - **`"텍스트"`**: 출력할 문자열
- **사용 예시**:
  ```cpp
  u8g2->drawStr(0, 12, "Received OK!");
  ```
  - (0, 12) 위치에 "Received OK!"라는 문자열을 출력합니다.
  
- **참고**: `U8g2` 라이브러리는 다양한 폰트와 그리기 기능을 제공하여 OLED 디스플레이에 텍스트, 도형 등을 쉽게 그릴 수 있습니다.

### **2.2. LoRa 라이브러리 주요 함수**

- **`LoRa.begin(frequency)`**: LoRa 모듈을 초기화하고 설정된 주파수로 시작합니다.
- **`LoRa.setPins(cs, reset, dio0)`**: LoRa 모듈과의 SPI 통신을 위한 핀을 설정합니다.
- **`LoRa.setTxPower(power)`**: 송신 전력을 설정합니다.
- **`LoRa.setSignalBandwidth(bandwidth)`**: 신호 대역폭을 설정합니다.
- **`LoRa.setSpreadingFactor(sf)`**: 스프레딩 팩터를 설정합니다.
- **`LoRa.setPreambleLength(length)`**: 프리앰블 길이를 설정합니다.
- **`LoRa.setSyncWord(word)`**: 동기 단어를 설정합니다.
- **`LoRa.disableCrc()`**: CRC 검증을 비활성화합니다.
- **`LoRa.disableInvertIQ()`**: IQ 반전을 비활성화합니다.
- **`LoRa.setCodingRate4(cr)`**: 코딩 레이트를 설정합니다.
- **`LoRa.receive()`**: LoRa 모듈을 수신 모드로 전환합니다.
- **`LoRa.parsePacket()`**: 수신된 패킷의 크기를 반환합니다.
- **`LoRa.read()`**: 수신된 데이터를 읽습니다.
- **`LoRa.packetRssi()`**: 수신된 패킷의 RSSI 값을 반환합니다.
- **`LoRa.packetSnr()`**: 수신된 패킷의 SNR 값을 반환합니다.

---

## **3. Receiver.ino의 전체 동작 흐름**

1. **초기화 단계 (`setup()` 함수)**
   - **하드웨어 설정**: `setupBoards()`를 통해 보드와 주변 장치를 초기화합니다.
   - **전원 안정화 지연**: 1.5초 지연을 통해 전원이 안정화될 시간을 줍니다.
   - **외부 클럭 활성화**: 필요한 경우 외부 클럭을 활성화합니다.
   - **시리얼 통신 초기화**: "LoRa Receiver" 메시지를 시리얼 모니터에 출력합니다.
   - **LoRa 모듈 초기화 및 설정**: LoRa 모듈을 설정된 주파수, 출력 전력, 대역폭, 스프레딩 팩터 등으로 초기화합니다.
   - **수신 모드 전환**: LoRa 모듈을 수신 모드로 전환하여 패킷 수신을 대기합니다.

2. **패킷 수신 및 처리 단계 (`loop()` 함수)**
   - **패킷 수신 시도**: `LoRa.parsePacket()`을 통해 패킷 수신을 시도합니다.
   - **패킷이 수신된 경우**:
     - **패킷 읽기**: 수신된 데이터를 문자열로 읽어들입니다.
     - **시리얼 모니터 출력**: 수신된 패킷 내용과 RSSI 값을 시리얼 모니터에 출력합니다.
     - **OLED 디스플레이 출력**: 수신 성공 메시지, 패킷 내용, RSSI, SNR 값을 OLED 디스플레이에 출력합니다.

---

## **4. 추가적인 고려 사항 및 팁**

### **4.1. OLED 디스플레이 초기화 및 사용**

- **초기화**: `LoRaBoards.cpp`의 `beginDisplay()` 함수에서 OLED 디스플레이를 초기화합니다.
- **폰트 설정**: `u8g2->setFont()` 함수를 사용하여 다양한 폰트를 설정할 수 있습니다. 폰트는 디스플레이에 출력되는 텍스트의 스타일을 결정합니다.
- **텍스트 출력**: `drawStr` 함수를 사용하여 텍스트를 그립니다.
- **버퍼 관리**: `clearBuffer()`로 버퍼를 초기화하고, 그린 후 `sendBuffer()`로 실제 화면에 출력합니다.

### **4.2. LoRa 설정 최적화**

- **주파수 대역**: 현재 설정된 921.1MHz은 일부 지역에서는 허용되지 않을 수 있습니다. 사용 지역의 규정을 확인하여 적절한 주파수 대역을 선택해야 합니다.
- **스프레딩 팩터(SF)**: 스프레딩 팩터를 조절하여 통신 거리와 데이터 전송률 간의 균형을 맞출 수 있습니다. 높은 SF는 더 긴 거리와 더 나은 내성을 제공하지만, 데이터 전송률은 낮아집니다.
- **대역폭(BW)**: 대역폭을 조절하여 데이터 전송률과 통신 거리를 최적화할 수 있습니다. 넓은 BW는 빠른 데이터 전송을 가능하게 하지만, 더 많은 에너지를 소비합니다.

### **4.3. 전원 관리**

- **PMU (Power Management Unit)**: `LoRaBoards.cpp`에서 PMU를 초기화하고 다양한 전원 채널을 관리합니다. 전원 관리는 보드의 안정적인 동작과 배터리 수명을 연장하는 데 중요합니다.
- **전력 소모 최적화**: 필요하지 않은 전원 채널을 비활성화하여 전력 소모를 줄일 수 있습니다.

### **4.4. 디버깅 및 테스트**

- **시리얼 모니터 활용**: 시리얼 모니터를 통해 패킷 수신 여부, RSSI, SNR 등의 정보를 실시간으로 확인할 수 있습니다.
- **OLED 디스플레이 활용**: OLED 디스플레이에 수신 상태와 패킷 정보를 출력하여 시각적으로 확인할 수 있습니다.
- **LED 표시등**: 보드에 LED가 연결되어 있다면, 이를 활용하여 수신 상태를 표시할 수 있습니다.

---

## **5. 결론**

**Receiver.ino**는 **Sender.ino**와 유사한 구조를 가지고 있지만, 수신 기능에 초점을 맞추어 LoRa 모듈을 설정하고 패킷을 수신하여 처리하는 역할을 합니다. 주요 단계는 보드와 주변 장치의 초기화, LoRa 모듈의 설정, 패킷 수신 및 처리로 구성되어 있습니다.

이 코드를 통해 LoRa 통신의 기본적인 송수신 과정을 이해하고, 필요에 따라 설정을 조정하여 원하는 통신 범위와 전력 효율을 달성할 수 있습니다. 추가적으로 OLED 디스플레이와 같은 주변 장치를 활용하여 사용자 인터페이스를 향상시킬 수 있습니다.

앞으로의 연구와 프로젝트에 많은 도움이 되기를 바랍니다! 추가로 궁금한 점이나 특정 부분에 대한 심화 설명이 필요하시면 언제든지 말씀해 주세요. 😊