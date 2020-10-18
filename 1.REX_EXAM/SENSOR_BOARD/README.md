# Rex AI Kit Sensor board
### Usage

 make

#### HW

* Rex Basic/Pro 보드에 I2C 센서보드가 연결
* I2C 센서보드
  * APDS-9960: 6 방향 제스처 감지 센서
  * MAX30101: 심박수 및 산소포화도 측정 센서
  * MAX30105: 공기 중 입자 검출 센서
  * RA12P: 압력 센서(Analogue)
    * 센서 출력값은 ADC인 MCP3021을 거쳐 10bit 정수값으로 변환됨
    * MCP3021: I2C ADC
  * SHT35-DIS: 온습도 측정 센서
  * Si1145: 가시광선, 자외선 측정 센서

