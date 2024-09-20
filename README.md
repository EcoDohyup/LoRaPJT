# LoRa PJT
## Goal
- LoRa 통신 활용할 수 있는 코드 탬플릿 개발

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
