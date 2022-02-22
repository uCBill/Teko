#include "Watchy_TEKO.h"

#define DARKMODE false

const uint8_t BATTERY_SEGMENT_WIDTH = 7;
const uint8_t BATTERY_SEGMENT_HEIGHT = 11;
const uint8_t BATTERY_SEGMENT_SPACING = 9;
const uint8_t WEATHER_ICON_WIDTH = 48;
const uint8_t WEATHER_ICON_HEIGHT = 32;
String textstring;

void WatchyTEKO::drawWatchFace() {
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  drawTime();
  drawDate();
  drawSteps();
  drawWeather();
  drawBattery();
  display.drawBitmap(120, 77, WIFI_CONFIGURED ? wifi : wifioff, 26, 18, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  if (BLE_CONFIGURED) {
    display.drawBitmap(100, 75, bluetooth, 13, 21, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  }
}

void WatchyTEKO::drawTime() {
  display.setFont(&Teko_Regular40pt7b);//was '&DSEG7_Classic_Bold_53'
  int displayHour;
  if (currentTime.Hour > 12) {
     textstring = ((currentTime.Hour+11)%12)+1;
  } else if (currentTime.Hour < 1) {
    textstring = 12;
  } else {
    textstring = currentTime.Hour;
  }
    textstring += ":";
  if (currentTime.Minute < 10) {
    textstring += "0";
  } else {
    textstring += "";
  }
    textstring += currentTime.Minute;
      //This option displays am or pm to 12 hour clock
  if (currentTime.Hour >= 12) {
    textstring += "PM";
  } else {
    textstring += "AM";
  }

  int16_t  x1, y1;
  uint16_t w, h;
    
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.setCursor(100-w/2, 57);
      display.print(textstring);
}

void WatchyTEKO::drawDate() {
  display.setFont(&Teko_Regular12pt7b);

  int16_t  x1, y1;
  uint16_t w, h;

  String dayOfWeek = dayStr(currentTime.Wday);
  display.getTextBounds(dayOfWeek, 5, 85, &x1, &y1, &w, &h);
  if (currentTime.Wday == 4) {
    w = w - 5;
  }
  display.setCursor(85 - w, 85);
  display.println(dayOfWeek);

  String month = monthShortStr(currentTime.Month);
  display.getTextBounds(month, 60, 110, &x1, &y1, &w, &h);
  display.setCursor(85 - w, 110);
  display.println(month);

  display.setFont(&Teko_Regular20pt7b);
  display.setCursor(5, 120);
  if (currentTime.Day < 10) {
    display.print("0");
  }
  display.println(currentTime.Day);
  display.setCursor(5, 150);
  display.println(tmYearToCalendar(currentTime.Year));// offset from 1970, since year is stored in uint8_t
}
void WatchyTEKO::drawSteps() {
  // reset step counter at midnight
  if (currentTime.Hour == 0 && currentTime.Minute == 0) {
    sensor.resetStepCounter();
  }
  display.setFont(&Teko_Regular20pt7b);  
  uint32_t stepCount = sensor.getCounter();
  display.drawBitmap(10, 165, steps, 19, 23, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.setCursor(35, 190);
  display.println(stepCount);
}

      // draw battery
void WatchyTEKO::drawBattery(){
    display.setFont(&Poky20pt7b);
    String textstring;
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();
    if(VBAT > 4.2){
        textstring = "T";
    }
    else if(VBAT > 4.1 && VBAT <= 4.2){
        textstring = "5";
    }
    else if(VBAT > 4.01 && VBAT <= 4.1){
        textstring = "4";
    }
    else if(VBAT > 3.91 && VBAT <= 4.01){
        textstring = "3";
    }    
    else if(VBAT > 3.83 && VBAT <= 3.91){
        textstring = "2";
    }    
    else if(VBAT > 3.74 && VBAT <= 3.83){
        textstring = "1";
    }    
    else if(VBAT <= 3.74){
        textstring = "Q";
    }

  int16_t  x1, y1;
  uint16_t w, h;
    
      display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
      display.setCursor(154, 105);
      display.print(textstring);
}

void WatchyTEKO::drawWeather() {

  weatherData currentWeather = getWeatherData();

  int8_t temperature = currentWeather.temperature;
  int16_t weatherConditionCode = currentWeather.weatherConditionCode;

  display.setFont(&Teko_Regular40pt7b);
  int16_t  x1, y1;
  uint16_t w, h;
  display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
  if (159 - w - x1 > 87) {
    display.setCursor(159 - w - x1, 150);
  } else {
    display.setFont(&Teko_Regular40pt7b);
    display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
    display.setCursor(159 - w - x1, 136);
  }
  display.println(temperature);
  display.drawBitmap(165, 110, currentWeather.isMetric ? celsius : fahrenheit, 26, 20, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  const unsigned char* weatherIcon;

  //https://openweathermap.org/weather-conditions
  if (weatherConditionCode > 801) { //Cloudy
    weatherIcon = cloudy;
  } else if (weatherConditionCode == 801) { //Few Clouds
    weatherIcon = cloudsun;
  } else if (weatherConditionCode == 800) { //Clear
    weatherIcon = sunny;
  } else if (weatherConditionCode >= 700) { //Atmosphere
    weatherIcon = atmosphere;
  } else if (weatherConditionCode >= 600) { //Snow
    weatherIcon = snow;
  } else if (weatherConditionCode >= 500) { //Rain
    weatherIcon = rain;
  } else if (weatherConditionCode >= 300) { //Drizzle
    weatherIcon = drizzle;
  } else if (weatherConditionCode >= 200) { //Thunderstorm
    weatherIcon = thunderstorm;
  } else
    return;
  display.drawBitmap(145, 158, weatherIcon, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}
