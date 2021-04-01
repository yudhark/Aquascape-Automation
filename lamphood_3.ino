//#include <DS1302RTC.h>
#include <virtuabotixRTC.h>
#include <Time.h>
#include <TimeLib.h>
#include <SPI.h>
#include <Adafruit_GFX.h>  //https://github.com/adafruit/Adafruit-GFX-Library
#include <OneWire.h>
#include <Arduino_ST7789_Fast.h>  //https://github.com/cbm80amiga/Arduino_ST7789_Fast
#include <DallasTemperature.h>   //https://github.com/milesburton/Arduino-Temperature-Control-Library
#define ONE_WIRE_BUS 7
#define TEMP_X  10
#define TEMP_Y  30
#define TFT_DC    40
#define TFT_RST   41
#define SCR_WD   240
#define SCR_HT   240

#define white_pot A0
#define blue_pot A1
#define red_pot A2
#define warm_pot A3

#define white_out 2
#define blue_out 3
#define red_out 4
#define warm_out 5

const int relay[] = {24,25,26,27};
const int button[] = {30,31,34,33};

float previousTemp = 99.9;
float tempC = 0;

int hijau = 0;
int merah = 0;
uint16_t tempCol = 0;
uint16_t weekCol = 0;
uint16_t dateCol = RGBto565(255,128,0);
uint16_t timeCol = RGBto565(255,255,51);

int dow, tanggal, bulan, tahun, jam, menit = 99;
int prevDate, prevYear, prevHour, prevMinute = 99;
String nameOfDay[] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
String nameOfMonth[] = {"Januari","Februari","Maret","April","Mei","Juni","Juli","Agustus","September","Oktober","November","Desember"};
String prevDay,prevBulan = "";

int blockState_1, blockState_2, blockState_3, blockState_4 = 0;
int whiteLevel, blueLevel, redLevel, warmLevel = 0;

// Set pins:  CE, IO,CLK
//DS1302RTC RTC(28, 29, 6);
virtuabotixRTC myRTC(6, 29, 28); //CLK, DAT, RST

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST);

void setup() {
  pinMode(white_pot, INPUT);
  pinMode(blue_pot, INPUT);
  pinMode(red_pot, INPUT);
  pinMode(warm_pot, INPUT);

  pinMode(white_out, OUTPUT);
  pinMode(blue_out, OUTPUT);
  pinMode(red_out, OUTPUT);
  pinMode(warm_out, OUTPUT);

  for (int i = 0; i <= 3; ++i) {
    pinMode(relay[i], OUTPUT);
    digitalWrite(relay[i],HIGH);
  }
  for (int i = 0; i <= 3; ++i) {
    pinMode(button[i], INPUT);
  }
//  setSyncProvider(RTC.get);
//  RTC.haltRTC(false);
//  RTC.writeEN(true);
//  myRTC.setDS1302Time(30, 2, 6, 4, 3, 2, 2021);
  sensors.begin();
  
  tft.init(SCR_WD, SCR_HT);
  tft.fillScreen(BLACK);
  tft.setRotation(3);
  tft.setCursor(45, 0);
  tft.setTextColor(WHITE);  
  tft.setTextSize(2);
  tft.println(" Temperature");
  tft.setCursor(160, TEMP_Y);
  tft.setTextSize(4);
  tft.setTextColor(WHITE,BLACK);  
  tft.println((char)247 );
  tft.setCursor(190, TEMP_Y);
  tft.println("C");
  tft.setCursor(100,90);
  tft.setTextSize(2);
  tft.setTextColor(WHITE,BLACK);
  tft.println("Date");
  tft.setCursor(142,170);
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.println(":");
}

void loop() {
  // put your main code here, to run repeatedly:
  setLevel();
  setBlock1();
  setBlock2();
  setBlock3();
  setBlock4();
  changeColor();
  setColorWeekEnd();
  sensors.setResolution(10);
  sensors.requestTemperatures();
  previousTemp = tempC;
  tempC = sensors.getTempCByIndex(0);
  if (previousTemp!=tempC) {
    deletePreviousTemp();
    printTemp();
  }
  prevDate = tanggal;
  prevDay = nameOfDay[dow-1];
  prevBulan = nameOfMonth[bulan-1];
  prevYear = tahun;
  prevHour = jam;
  prevMinute = menit;
  getCurrentTime();
  if (prevDate!=tanggal) {
    deletePrevDate();
    printDate();
  }
  if (prevDay!=nameOfDay[dow-1]) {
    deletePrevDay();
    printDay();
  }
  if (prevBulan!=nameOfMonth[bulan-1]) {
    deletePrevMonth();
    printMonth();
  }
  if (prevYear!=tahun) {
    deletePrevYear();
    printYear();
  }
  if (prevHour!=jam) {
    deletePrevHour();
    printHour();
  }
  if (prevMinute!=menit) {
    deletePrevMinute();
    printMinute();
  }
}

void setLevel() {
  int white = analogRead(white_pot);
  delay(10);
  int blue = analogRead(blue_pot);
  delay(10);
  int red = analogRead(red_pot);
  delay(10);
  int warm = analogRead(warm_pot);
  delay(10);
  
  whiteLevel = map(white,0,1023,0,255);
  blueLevel = map(blue,0,1023,0,255);
  redLevel = map(red,0,1023,0,255);
  warmLevel = map(warm,0,1023,0,255);

  analogWrite(white_out, whiteLevel);
  analogWrite(blue_out, blueLevel);
  analogWrite(red_out, redLevel);
  analogWrite(warm_out, warmLevel);
}

void setBlock1() {
  int state = digitalRead(button[0]);
  delay(30);
  if (state == HIGH) {
    blockState_1++;
    if (blockState_1 > 1) {
      blockState_1 = 0;
    }
  }
  if ( blockState_1 == 0 ) {
    digitalWrite(relay[0], HIGH);
  }
  else if ( blockState_1 == 1 ) {
    digitalWrite(relay[0], LOW);
  }
}

void setBlock2() {
  int state = digitalRead(button[1]);
  delay(30);
  if (state == HIGH) {
    blockState_2++;
    if (blockState_2 > 1) {
      blockState_2 = 0;
    }
  }
  if ( blockState_2 == 0 ) {
    digitalWrite(relay[1], HIGH);
  }
  else if ( blockState_2 == 1 ) {
    digitalWrite(relay[1], LOW);
  }
}

void setBlock3() {
  int state = digitalRead(button[2]);
  delay(30);
  if (state == HIGH) {
    blockState_3++;
    if (blockState_3 > 1) {
      blockState_3 = 0;
    }
  }
  if ( blockState_3 == 0 ) {
    digitalWrite(relay[2], HIGH);
  }
  else if ( blockState_3 == 1 ) {
    digitalWrite(relay[2], LOW);
  }
}

void setBlock4() {
  int state = digitalRead(button[3]);
  delay(30);
  if (state == HIGH) {
    blockState_4++;
    if (blockState_4 > 1) {
      blockState_4 = 0;
    }
  }
  if ( blockState_4 == 0 ) {
    digitalWrite(relay[3], HIGH);
  }
  else if ( blockState_4 == 1 ) {
    digitalWrite(relay[3], LOW);
  }
}

void getCurrentTime() {
//  tmElements_t tm;
//  RTC.read(tm);
//  dow = tm.Wday;
//  tanggal = tm.Day;
//  bulan = tm.Month;
//  tahun = tmYearToCalendar(tm.Year);
//  jam = tm.Hour;
//  menit = tm.Minute;
  myRTC.updateTime();
  dow = myRTC.dayofweek;
  tanggal = myRTC.dayofmonth;
  bulan = myRTC.month;
  tahun = myRTC.year;
  jam = myRTC.hours;
  menit = myRTC.minutes;
}

void deletePreviousTemp()
{
  tft.setCursor(TEMP_X+25, TEMP_Y);
  tft.setTextSize(5);
  tft.setTextColor(BLACK);
  tft.println(previousTemp,1);
}

void printTemp()
{
  tft.setCursor(TEMP_X+25, TEMP_Y);
  tft.setTextSize(5);
  tft.setTextColor(tempCol);
  tft.println(tempC,1);
}

void changeColor() {
  if (tempC < 28.1 && tempC > 27.9) {
    tempCol = RGBto565(248,248,0);
  }
  else if (tempC >= 29) {
    tempCol = RGBto565(200,0,0);
  }
  else if (tempC <= 26) {
    tempCol = RGBto565(0,180,0);
  }
  else if (tempC >= 28.1) {
    float selisihA = tempC-28;
    hijau = selisihA*10;
    switch(hijau){
      case 1:
        tempCol = RGBto565(248,230,0);
        break;
      case 2:
        tempCol = RGBto565(248,205,0);
        break;
      case 3:
        tempCol = RGBto565(248,180,0);
        break;
      case 4:
        tempCol = RGBto565(248,155,0);
        break;
      case 5:
        tempCol = RGBto565(248,130,0);
        break;
      case 6:
        tempCol = RGBto565(248,105,0);
        break;
      case 7:
        tempCol = RGBto565(248,80,0);
        break;
      case 8:
        tempCol = RGBto565(248,55,0);
        break;
      case 9:
        tempCol = RGBto565(248,30,0);
        break;
      case 10:
        tempCol = RGBto565(248,0,0);
        break;
    }
  }
  else if(tempC <= 27.9) {
    float selisihB = 28 - tempC;
    merah = selisihB*10;
    switch(merah) {
      case 1:
        tempCol = RGBto565(240,248,0);
        break;
      case 2:
        tempCol = RGBto565(225,248,0);
        break;
      case 3:
        tempCol = RGBto565(210,248,0);
        break;
      case 4:
        tempCol = RGBto565(195,248,0);
        break;
      case 5:
        tempCol = RGBto565(180,248,0);
        break;
      case 6:
        tempCol = RGBto565(165,248,0);
        break;
      case 7:
        tempCol = RGBto565(150,248,0);
        break;
      case 8:
        tempCol = RGBto565(135,248,0);
        break;
      case 9:
        tempCol = RGBto565(12065,248,0);
        break;
      case 10:
        tempCol = RGBto565(105,248,0);
        break;
      case 11:
        tempCol = RGBto565(90,248,0);
        break;
      case 12:
        tempCol = RGBto565(75,248,0);
        break;
      case 13:
        tempCol = RGBto565(60,248,0);
        break;
      case 14:
        tempCol = RGBto565(45,248,0);
        break;
      case 15:
        tempCol = RGBto565(30,248,0);
        break;
      case 16:
        tempCol = RGBto565(15,248,0);
        break;
      case 17:
        tempCol = RGBto565(0,248,0);
        break;
      case 18:
        tempCol = RGBto565(0,220,0);
        break;
      case 19:
        tempCol = RGBto565(0,200,0);
        break;
      case 20:
        tempCol = RGBto565(0,180,0);
        break; 
    }
  }
}

void printTime() {
  printDate();
  printDay();
  printMonth();
  printYear();
  printHour();
  printMinute();
}

void printDate() {
  tft.setCursor(20,140);
  tft.setTextSize(7);
  tft.setTextColor(weekCol);
  tft.println(tanggal);
}
void deletePrevDate() {
  tft.setCursor(20,140);
  tft.setTextSize(7);
  tft.setTextColor(BLACK);
  tft.println(prevDate);
}

void printDay() {
  tft.setCursor(24,115);
  tft.setTextSize(2);
  tft.setTextColor(weekCol);
  tft.println(nameOfDay[dow-1]);
}
void deletePrevDay() {
  tft.setCursor(24,115);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.println(prevDay);
}

void printMonth() {
  tft.setCursor(110,115);
  tft.setTextSize(2);
  tft.setTextColor(dateCol);
  tft.println(nameOfMonth[bulan-1]);
}
void deletePrevMonth() {
  tft.setCursor(110,115);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.println(prevBulan);
}

void printYear() {
  tft.setCursor(110,140);
  tft.setTextSize(3);
  tft.setTextColor(dateCol);
  tft.println(tahun);
}
void deletePrevYear() {
  tft.setCursor(110,140);
  tft.setTextSize(3);
  tft.setTextColor(BLACK);
  tft.println(prevYear);
}

void printHour() {
  tft.setCursor(110,170);
  tft.setTextSize(3);
  tft.setTextColor(timeCol);
  tft.println(jam);
}
void deletePrevHour() {
  tft.setCursor(110,170);
  tft.setTextSize(3);
  tft.setTextColor(BLACK);
  tft.println(prevHour);
}

void printMinute() {
  tft.setCursor(155,170);
  tft.setTextSize(3);
  tft.setTextColor(timeCol);
  tft.println(menit);
}
void deletePrevMinute() {
  tft.setCursor(155,170);
  tft.setTextSize(3);
  tft.setTextColor(BLACK);
  tft.println(prevMinute);
}

void setColorWeekEnd() {
  if (dow==1) {
    weekCol = RGBto565(255,102,102);
  }else{
    weekCol = RGBto565(153,255,153);
  }
}
