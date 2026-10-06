# RTC 시계 + 계산기 (ATmega128A, LCD)

DS1307 RTC로 시각을 표시·설정하는 **시계 모드**와 4×4 키패드로 수식을 계산하는 **계산기 모드**를 버튼으로 전환하는 다기능 장치. I2C 마스터 드라이버, 텍스트 LCD 드라이버 레지스터 수준 구현.

- **기간**: 2026-06-29 ~ 07-03
- **MCU**: ATmega128A (16MHz) · **언어**: C · **IDE**: Microchip Studio
- **형태**: 교육 과정 팀 과제(팀원별 개별 구현), 본인 구현 버전

## 주요 기능

| 모드 | 버튼 | 동작 |
|---|---|---|
| 시계 | BTN3 | 시계 ↔ 시각 설정 전환 (설정 모드 종료 시 RTC 저장) |
| | BTN2 | 설정 항목 선택 (연 → 월 → 일 → 시 → 분 → 초 순환) |
| | BTN0 / BTN1 | 선택 항목 값 감소 / 증가 |
| | BTN4 | 계산기 모드 전환 |
| 계산기 | 키패드 | 숫자, `+ - * /` 입력 |
| | BTN2 / BTN3 | `(` / `)` 입력 |
| | BTN1 | 백스페이스 |
| | BTN0 | 입력 초기화 |
| | BTN4 | 시계 모드 복귀 |

## 구현 포인트

- **I2C 마스터 드라이버** (`I2C_MASTER.c`)
  - TWI 레지스터(`TWCR`, `TWSR`) 제어로 START / STOP, 슬레이브 주소 + R/W 전송, 데이터 쓰기, ACK/NACK 읽기 구현
  - 단계별 상태 코드 확인, NACK 시 UART 오류 출력
- **BCD ↔ 10진 변환** (`clock.c`)
  - DS1307의 BCD 시·분·초·날짜 변환
  - 시(hour) 레지스터 상위 비트 마스킹으로 24시간제 처리
- **LCD 4비트 드라이버** (`lcd.c`)
  - 한 바이트를 상·하위 니블로 나눠 전송, Enable 펄스로 래치
  - 초기화 순서: `4비트 모드 → 2행 5×8 폰트 → Display ON → Clear → Entry Mode`
  - 갱신 플래그(`lcd_update_flag`) 설정 시에만 화면 갱신, 불필요한 깜빡임 감소
- **키패드 입력 큐 버퍼링** (`queue.c`)
  - Timer0 1ms 인터럽트에서 60ms마다 키패드 스캔 → 원형 큐 저장 → 메인 루프에서 처리
  - 빠른 입력에도 유실 없음
- **계산기: 중위 → 후위 표기 변환 후 스택 계산** (`calc.c`)
  - `parsing()`: 괄호, 연산자 우선순위(`* /` > `+ -`) 처리, 후위 표기식 생성
  - `postfix_calculator()`: 스택 기반 계산
  - 잘못된 수식은 `error_flag`로 구분, LCD 표시
- **모드별 버튼 처리**
  - 함수 포인터 배열(`{clock_btn_processing, calc_btn_processing}`)을 모드 값으로 인덱싱
  - 같은 버튼이 모드에 따라 다른 동작

## 하드웨어 연결

| 장치 | 연결 |
|---|---|
| LCD 데이터 (4비트) | PORTB |
| LCD 제어 (RS, RW, E) | PORTC (PC0~2) |
| RTC DS1307 (I2C) | PD0 (SCL), PD1 (SDA) |
| 키패드 | PORTA |
| 버튼 5개 | PORTD (PD2~6), active-high |

## 코드 구조

```
src/
├── main.c         초기화, 메인 루프
├── common.h       Clock, Calculator, Clock_calculator 자료형
├── clock.c        RTC 읽기·쓰기, BCD 변환, 시각 설정 로직
├── calc.c         중위→후위 변환, 스택 계산
├── I2C_MASTER.c   I2C(TWI) 드라이버
├── lcd.c          LCD 드라이버, 화면 출력
├── keypad.c       키패드 스캔
├── queue.c        원형 큐
├── button.c       버튼 에지 검출, 모드별 버튼 처리
├── timer.c        Timer0 1ms 인터럽트, 키패드 스캔 주기
└── uart0.c        디버그 출력
```

## 빌드

- Microchip Studio: `src/LCD_clock_ver2.atsln` 열어서 빌드
- avr-gcc:

```bash
avr-gcc -mmcu=atmega128a -Os -I src -o lcd_clock.elf src/*.c
```

## 상태

- 구현: 시계 모드(표시·설정·저장), 계산기 모드(사칙연산·괄호·백스페이스·오류 표시)
- 미구현: 계산기 무입력 시 시계 모드 자동 복귀
