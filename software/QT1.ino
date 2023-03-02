/*
Copyright (C) 2023 OpenQRNG
All software is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this source. If not, see https://www.gnu.org/licenses/.
 */

const int td_inter  = 6;    //Tunnel diode voltage interrupt pin
const int pwm_pin   = 9;    //PWM pin
const int pwm_inter = 8;    //PWM interrupt pin
volatile unsigned long cycle_start, cycle_end, cycle_count;
volatile bool start_flag = 0, inter_flag = 0;

volatile byte rnd_count = 0;
volatile byte rnd_byte  = 0;  //byte storing random bits to be send to the serial port

void setup() {
  
  analogWriteResolution(7);

  //Set TD interrupt
  pinMode(td_inter, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(td_inter), inter_ISRTD, RISING);

  //Set PWM interrupt
  pinMode(pwm_inter, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pwm_inter), inter_ISRPWM, RISING);

  //Set PWM
  analogWriteFrequency(pwm_pin, 1000000);
  analogWrite(pwm_pin, 89);

  Serial.begin(1000000);
}

void loop() {
  while (inter_flag == 0) ;

  rnd_byte=rnd_byte<<1;
  rnd_byte |= bool(cycle_count%2);
  
  if (rnd_count<7){  
    rnd_count += 1;
  }
  else {
    Serial.print(rnd_byte);
    rnd_count = 0;
    rnd_byte = 0;
  }
  
  inter_flag = 0;
}


void inter_ISRTD() {
  if (start_flag) {
      cycle_end = ARM_DWT_CYCCNT;
      cycle_count = cycle_end - cycle_start;
      start_flag = 0;
      inter_flag = 1;
  }
}

void inter_ISRPWM() {
  if (inter_flag==0) {
  cycle_start = ARM_DWT_CYCCNT;
  start_flag = 1;
  }
}
