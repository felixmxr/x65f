#define ZERO PORTB = 0b01110000;      //Green    H1,Pickle,Mode
#define TWO  PORTB = 0b10110000;      //Yellow   H2,Pinky,Paddle
#define SIX  PORTB = 0b11010000;      //Orange   POV
#define FOUR PORTB = 0b11100000;      //White    CMS,Trigger
#define NONE PORTB = 0b11110000;

#define DELAY 100

void setup() {                

  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);

  NONE

  digitalWrite(8,HIGH);
  digitalWrite(9,HIGH);
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);

  pinMode(0,INPUT);
  pinMode(1,INPUT);
  pinMode(2,INPUT);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(6,INPUT);

  digitalWrite(0,HIGH);
  digitalWrite(1,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(3,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(6,HIGH);
  
  bitSet(ADCSRB,ADHSM);
  
  bitSet(ADCSRA,ADPS2);
  bitSet(ADCSRA,ADPS1);  
  bitClear(ADCSRA,ADPS0);  //prescale 64 ==> 250khz
  
  analogReference(INTERNAL);

}

unsigned int test = 0;

byte pov = 8;  //red in windows test
byte h1 = 8;   //blue
byte h2 = 8;   //black
byte cms = 8;  //green

unsigned long x = 0;
unsigned long y = 0;
unsigned long rz = 0;

byte trigger = 0;
byte mode = 0;
byte pickle = 0;
byte pinky = 0;
byte paddle = 0;

byte data[7] = {0,};
byte temp = 0;

void loop() {
  
  //delay(1000);               // wait for a second
  if (test == 1000){
    Serial.println(millis());
    test=0;
  };
  test++;

  ZERO
  delayMicroseconds(DELAY);
  temp = ~PIND;
  pickle = (temp>>4) & 1;
  mode = (temp>>7) & 1;
  temp &= 0b1111;
  switch(temp){
    case 4:  h1=0; break;  //UP
    case 8:  h1=2; break;  //RIGHT
    case 2:  h1=4; break;  //DOWN
    case 1:  h1=6; break;  //LEFT
    default: h1=8; break;
  }

  TWO
  delayMicroseconds(DELAY);
  temp = ~PIND;
  pinky = (temp>>4) & 1;
  paddle = (temp>>7) & 1;
  temp &= 0b1111;
  switch(temp){
    case 4:  h2=0; break;  //UP
    case 8:  h2=2; break;  //RIGHT
    case 2:  h2=4; break;  //DOWN
    case 1:  h2=6; break;  //LEFT
    default: h2=8; break;
  }
  
  FOUR
  delayMicroseconds(DELAY);
  temp = ~PIND;
  trigger = (temp>>4) & 1;
  temp &= 0b1111;
  switch(temp){
    case 4:  cms=0; break;  //UP
    case 8:  cms=2; break;  //RIGHT
    case 2:  cms=4; break;  //DOWN
    case 1:  cms=6; break;  //LEFT
    default: cms=8; break;
  }
  
  SIX
  delayMicroseconds(DELAY);
  temp = ~PIND;
  temp &= 0b1111;
  switch(temp){
    case 4:    pov=0; break;  //UP
    case 4+1:  pov=1; break;  //UP-RIGHT
    case 1:    pov=2; break;  //RIGHT
    case 1+8:  pov=3; break;  //DOWN-RIGHT
    case 8:    pov=4; break;  //DOWN
    case 8+2:  pov=5; break;  //DOWN-LEFT
    case 2:    pov=6; break;  //LEFT
    case 2+4:  pov=7; break;  //UP-LEFT
    default:   pov=8; break;
  }
  
  int oversampling = 1;
  
  for (int i=0;i<4*oversampling;i++){
     x += analogRead(0); 
  }
  for (int i=0;i<4*oversampling;i++){
     y += analogRead(1); 
  }
    for (int i=0;i<2*oversampling;i++){
     rz += analogRead(2); 
  }
  
  x /= oversampling;
  y /= oversampling;
  rz /= oversampling;
  
  int  xmid = 2736;
  int  xrange = 200;
  int ymid = 2740;
  int yrange = 250;
  int rzmid = 1372;
  int rzrange = 80; 
  
  x=constrain(x,xmid-xrange,xmid+xrange);
  y=constrain(y,ymid-yrange,ymid+yrange);
  rz=constrain(rz,rzmid-rzrange,rzmid+rzrange);
  
  x=map(x,xmid-xrange,xmid+xrange,0,4095);
  y=map(y,ymid-yrange,ymid+yrange,0,4095);
  rz=map(rz,rzmid-rzrange,rzmid+rzrange,0,2047);
  
  
  Serial.println(x);
  Serial.println(y);
  Serial.println(rz);
  
  data[0] =  x;
  data[1] =  x/256 + (y % 256)*16 ;
  data[2] =  y/16; 
  data[3] =  rz;
  data[4] = rz/256 + ((trigger + (pickle<<1) + (mode<<2) + (pinky<<3) + (paddle<<4))<<3);
  data[5] =  pov + h1*16; 
  data[6] =  h2 + cms*16;

  //for (int i=0;i<7;i++) Serial.println(data[i],BIN);

  HID_SendReport(4,&data,7);
  
  x=0;
  y=0;
  rz=0;
}

