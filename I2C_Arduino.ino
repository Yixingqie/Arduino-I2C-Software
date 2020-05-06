int sda = 3;
int scl = 5;
bool sdaOn = false;
bool sclOn = false;
int delay_time = 3;

void sdaHigh() {
  if (!sdaOn) {
    PIND = _BV(PB3);
  }
  sdaOn = true;
}
void sdaLow() {
  if (sdaOn) {
    PIND = _BV(PB3);
  }
  sdaOn = false;
}

void sclHigh() {
  if (!sclOn) {
    PIND = _BV(PB5);
  }
  sclOn = true;
}
void sclLow() {
  if (sclOn) {
    PIND = _BV(PB5);
  }
  sclOn = false;
}

void address_Masterwrite(uint8_t address) {
  uint8_t modified = (address << 1);
  write_i2c(modified); //8 bit
}

void address_SlaveRead(uint8_t address) {
  uint8_t modified = (address >> 1);

}

void address_MasterRead(uint8_t address) {
  uint8_t modified = (address << 1) + (uint8_t) 0x01;
  write_i2c(modified);
}


void initialize_i2c() {
  pinMode(scl, OUTPUT);
  pinMode(sda, OUTPUT);
  // digitalWrite(sda, HIGH);
  //  digitalWrite(scl, HIGH);
  PIND = _BV(PB3); //sda
  PIND = _BV(PB5); //scl
  sdaOn = true;
  sclOn = true;

}

void start_i2c() {
  sdaHigh();
  sclHigh();
  delayMicroseconds(delay_time);
  sdaLow();
  delayMicroseconds(delay_time);
  sclLow();
  delayMicroseconds(delay_time);
}

void stop_i2c() {
  sdaLow();
  delayMicroseconds(delay_time);
  sclHigh();
  delayMicroseconds(delay_time);
  sdaHigh();
  delayMicroseconds(delay_time);
}

void write_i2c(uint8_t value) {
  for (uint8_t curr = 0X80; curr != 0; curr >>= 1) {
    if (curr & value) {
      sdaHigh();
      //    Serial.print(1);
    } else {
      sdaLow();
      //  Serial.print(0);
    }
    delayMicroseconds(delay_time);
    sclHigh();
    delayMicroseconds(delay_time);
    sclLow();
    delayMicroseconds(delay_time);
  }
}

uint8_t read_i2c(bool last) { //bool last can be used
  uint8_t b = 0;
  sdaHigh();
  pinMode(sda, INPUT_PULLUP);
  for (uint8_t i = 0; i < 8; i++) {
    b <<= 1;
    delayMicroseconds(delay_time);
    sclHigh();
    if (digitalRead(sda))
    {
      b |= 1;
    }
    sclLow();
  }
  pinMode(sda, OUTPUT);
  digitalWrite(sda, HIGH);
  sdaHigh();
  if (last) {
    sdaHigh();
  } else {
    sdaLow();
  }
  delayMicroseconds(delay_time);
  sclHigh();
  delayMicroseconds(delay_time);
  sclLow();
  delayMicroseconds(delay_time);
  sdaLow();
  return b;
}

bool i2c_wait_ack() {
  int wait  = 0;
  sdaHigh();
  pinMode(sda, INPUT_PULLUP);
  delayMicroseconds(delay_time);
  sclHigh();
  delayMicroseconds(delay_time);
  uint8_t ack = digitalRead(sda);
  sclLow();
  // delayMicroseconds(1);
  pinMode(sda, OUTPUT);
  if (ack) {
    stop_i2c();
    return false;
  }
  return true;
}

void setup() {

  Serial.begin(9600);
  initialize_i2c();
}

void loop() {
  start_i2c();
  //master write
  /*
    address_Masterwrite(1);
    if(i2c_wait_ack()){
    write_i2c(2);
    i2c_wait_ack();
    stop_i2c();
    }
  */

  //master read
  address_MasterRead(2);
  if (i2c_wait_ack()) {
    Serial.println(read_i2c(true)); // Serial.println shows delay on scope before stop_i2c
    stop_i2c();
  }



  delay(1000);

}
