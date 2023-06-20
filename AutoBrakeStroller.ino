#include <Wire.h>
#include <Servo.h>
#include <MPU6050.h>

MPU6050 mpu;  // MPU6050 센서 객체 생성
Servo servo;  // Servo 객체 생성

const int touchPin = 2;  // 터치 센서 핀 번호
bool touchDetected = false;  // 터치 센서 감지 여부
int prevTouchState = HIGH;  // 이전 터치 상태 저장

const int servoPin = 9;  // 서보모터 핀 번호
const int servoRestAngle = 90;  
const int servoActiveFront = 100;  // 터치 감지 시 한 방향으로 10도
const int servoActiveBack = 80;  // 터치 감지되지 않을 때 반대 방향으로 10도


const int FSR1 = A0;  // FSR 센서 1 핀 번호
const int FSR2 = A1;  // FSR 센서 2 핀 번호

float prevXAngle = 0;  // 이전 X축 각도 저장
bool isServoActiveTouch = false;  // 서보모터 동작 여부 저장
bool isServoActive10 = false;  // 서보모터 동작 여부 저장
bool isServoActive20 = false;  // 서보모터 동작 여부 저장
bool isServoActiveFSR = false;  // 서보모터 동작 여부 저장


void setup() {
  Serial.begin(9600);
  
  pinMode(touchPin, INPUT);  // 터치 센서를 입력으로 설정
  servo.attach(servoPin);  // 서보모터 핀 설정

  mpu.initialize();  // MPU6050 초기화
  mpu.setFullScaleAccelRange(2);  // 가속도계 범위를 +/- 2g로 설정
}

void loop() {
  int FSR1_value = analogRead(FSR1);
  int FSR2_value = analogRead(FSR2);
  Serial.print("FSR1 값: ");
  Serial.println(FSR1_value);
  Serial.print("FSR2 값: ");
  Serial.println(FSR2_value);


  int touchState = digitalRead(touchPin);  // 터치 센서 상태 읽기

  // 터치 상태가 이전 상태와 다르면 터치 감지 여부 업데이트
  if (touchState != prevTouchState) {
    if (touchState == LOW) {
      touchDetected = true;
      Serial.println("감지");
    } else {
      touchDetected = false;
      Serial.println("감지X");
    }
  }

  prevTouchState = touchState;  // 현재 상태를 이전 상태로 저장

  // 터치가 감지된 경우
  if (touchDetected) {
    if(!isServoActiveTouch){
      //  servo.write(servoActiveFront );
      //  delay(7000); // 7초 동안 모터 동작
      onMotor(false,7000);
      isServoActiveTouch=true;
    }
    MpuFuc(FSR1_value,FSR2_value);
  } else {
    // 터치가 감지되지 않는 경우 초기화해준다
    if(isServoActiveTouch){
      // isServoActiveTouch 초기화
      onMotor(true,7000);
      isServoActiveTouch=false;
    }
    if(isServoActive10){
    }
    if(isServoActive20){
    }
    servo.write(servoRestAngle);
  }

  if(FSR2_value<=FSR1_value&& isServoActiveFSR){
    //FSR작동해제
    onMotor(false,5000);
    isServoActiveFSR= false;
  }
 
  
  delay(100);  // 100ms 딜레이
}

void MpuFuc(int FSR1_value, int FSR2_value){
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);  // 가속도 값 읽기
    
  // X축의 기울기 계산
  float xAngle = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * 180 / PI;

  Serial.print("X축 각도: ");
  Serial.println(xAngle);

  // X축의 기울기에 따라 서보모터의 동작 결정
  if(xAngle<10 && isServoActive10){
    onMotor(false,1000);
    isServoActive10 = false;
  } else if (xAngle >= 10 && xAngle < 20 && !isServoActive10) {
    if (isServoActive20) {
      onMotor(false,3000);
      isServoActive20 = false;
    }
    onMotor(true,1000);
    isServoActive10 = true;
  } else if ( xAngle >= 20 && !isServoActive20&&!isServoActiveFSR ) {
    if (isServoActive10) {
      onMotor(false,1000);
      isServoActive10 = false;
    }
    onMotor(true,3000);
    isServoActive20= true;
  } else if (xAngle < 20 && isServoActive20) {
    onMotor(false,3000);
    isServoActive20 = false;
  }else if(xAngle > 20 &&FSR2_value>FSR1_value&& !isServoActiveFSR){
    if (isServoActive20) {
      onMotor(false,3000);
      isServoActive20 = false;
    }
     onMotor(true,5000);
     isServoActiveFSR= true;
  }
  
  prevXAngle = xAngle;

  // if (xAngle >= 20) {
  //   //20도 이상 FSR작동조건 충족
  //   //조건으로 보아 20도 이상 도달했을때 isServoActive20 true일 가능성이 있음
  //   if(isServoActive20){
  //     onMotor(false,3000);
  //     isServoActive20=false;
  //   }
  //   if(!isServoActiveFSR){
  //     onMotor(true,5000);
  //     isServoActiveFSR= true;
  //   }
  // }
  servo.write(servoRestAngle);

}
void onMotor(bool active,int time){
  //모터를 동작시키는 코드
  if(active){
    servo.write(servoActiveFront);
  }else{
    servo.write(servoActiveBack);
  }
  delay(time); // 5초 동안 모터 동작
}