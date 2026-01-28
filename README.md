# TC375 MCU 기반 자율주차 RC카 시스템
🚗**Original Repository**: [autonomous-skid-steer-vehicle](https://github.com/team-2niverse/autonomous-skid-steer-vehicle.git) 

<br>

## 📝 프로젝트 개요
* **수행 기간:** 2025. 07. 23. ~ 2025. 08. 08. (현대오토에버, 현대엔지비, 한국전파진흥협회)
* **프로젝트 목표:** V-Model 프로세스 기반 TC375 MCU를 활용하여 원격 제어, 자동 긴급 제동(AEB), 자율 후방 주차가 가능한 RC카 시스템 개발
* **주요 내용:**
   * TC375 MCU와 Raspberry Pi 4를 이용한 임베디드 제어 시스템 구축
   * 컨트롤러를 이용한 원격 제어 기능 구현
   * ToF 센서 및 엔코더를 활용한 자동 긴급 제동(AEB) 기능 구현
   * 초음파 센서를 활용한 자율 후방 주차 기능 구현
<br>

---

## 👤 나의 역할 및 수행 업무
* **담당 역할:** 시스템 아키텍처 설계, TC375 MCU 펌웨어 및 자율주차 알고리즘 개발, 산출물 관리
* **수행 업무:**
    * 시스템 요구사항 분석 및 전체 시스템(HW/SW) 아키텍처 설계
    * TC375 MCU 디바이스 드라이버(초음파, 엔코더) 개발 및 모듈 통합
    * 자율주차 로직(공간 탐색, 후방 주차 기동, 경고 알림) 설계 및 시나리오 기반 테스트
    * Confluence를 활용한 V-Model 단계별 산출물 관리
<br>

---

## 🛠️ 사용 기술 및 환경

| 분류 | 상세 내용 |
| :--- | :--- |
| **Languages** | C, C++ |
| **MCU & SBC** | Infineon TC375, Raspberry Pi 4 |
| **Sensors & Actuators** | Ultrasonic Sensor, ToF Sensor, Encoder, DC Motor, Motor Driver, CAN HAT, LED, Buzzer |
| **Protocols & Frameworks** | CAN, Bluetooth |
| **Tools** | AURIX Development Studio, UDE Visual Platform, Git, Confluence |
<br>

---

## ⚙️ 시스템 아키텍처
**System Architecture**<br>
<img width="695" height="452" alt="image" src="https://github.com/user-attachments/assets/853611bc-3c33-4ff3-8186-87814f7b1c70" />
<br>

**Key SW Modules**<br>
<img width="711" height="338" alt="image" src="https://github.com/user-attachments/assets/a006ecf3-2387-4a2b-b0d2-38d2cb1375c0" />
<br>

**SW Flowchart**<br>
<img width="751" height="577" alt="image" src="https://github.com/user-attachments/assets/e2606a21-f295-4c19-9492-7fe8fa226f6e" />


<br>

---

## 🎬 주요 기능 및 결과물
**전체 기능 데모 시나리오**
* 원격주행(8자돌기) → AEB(전방의 벽) → AEB(전방주차) → 자동 후방주차


https://github.com/user-attachments/assets/82387ad2-8cca-4d68-ad48-2ef9a93111a7



<br>

---

## 🔍 트러블슈팅 및 회고

#### 📌 Case #1. 한정된 MCU 자원을 활용한 다수 센서 제어
* **문제 상황:** 초음파 센서(3개)와 엔코더(2개) 제어를 위해 5개의 외부 인터럽트 핸들러가 필요했으나, TC375 HW 구조상 독립적으로 활성화 가능한 외부 인터럽트(SCUERU) 노드가 4개로 제한됨
* **원인 분석:** 사용 가능한 OGU(Output Gating Unit)가 4개뿐이어서 독립적인 ISR을 4개까지만 생성할 수 있는 HW 제약 확인
* **해결 방법:**
  * 측후방 초음파 센서 3개를 하나의 OGU2(SCUERU2) 노드에 통합 연결
  * 해당 ISR 내에서 EIFR(External Interrupt Flag Register) 레지스터의 INTF 필드를 분석하여 이벤트를 발생시킨 센서를 판별 후 개별 처리
  * 처리 후 FMR 레지스터를 통해 플래그를 초기화하여 다음 인터럽트 대기 상태 확보

#### 📌 Case #2. 엔코더 측정값 불안정 및 노이즈 발생
* **문제 상황:** 주행 중 엔코더로부터 산출되는 속도(RPM) 값이 비정상적으로 튀거나 불안정한 현상 발생
* **원인 분석:** 하드웨어 글리치(Glitch) 및 매우 짧은 시간의 신호 변화가 인터럽트로 감지되어 RPM 계산식(*RPM = 1500000/avg_diff*)에 오류 유발
* **해결 방법:**
  * **HW 필터링**: SCU의 디지털 글리치 필터를 활성화하여 일정 시간 이상 유지되는 유효 에지만 통과하도록 디바운싱 설정
  * **SW 필터링**: 에지 간 간격(diff)이 500us 미만이거나 출력 핀 상태 변화가 없는 경우 유효하지 않은 에지로 판단하여 계산에서 배제
  * 결과적으로 AEB 및 PWM 보정 기능에 사용되는 속도 데이터의 신뢰성 확보
<br>
