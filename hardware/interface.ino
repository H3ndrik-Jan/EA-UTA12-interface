/*
  interface.ino

   Author: Hendrik-Jan Kuijt

   Description:
    Connect your UTA12 port to the internet! This code lets the ESP32 send data to a MySQL database over WiFi,
    and also reads settings for the power supply from this database.
*/

#include <Preferences.h>
#include <IPAddress.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <Wire.h>
#include <SPI.h>
#include <Mcp320x.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFiClient.h>
#include <WiFi.h>

//ADC definitions:
#define SPI_CS      2        // SPI slave select
#define ADC_VREF    5000     // 3.3V Vref
#define ADC_CLK     1600000  // SPI clock 1.6MHz
#define CS_PIN 5
#define CLOCK_PIN 18
#define MOSI_PIN 23
#define MISO_PIN 19

//Other pin definitions:
#define SELP 33
#define StandbyP 32
#define button1 4
#define cvcc 17 //AKA TX2
#define ovp 16 //AKA RX2
#define otp 2
#define VOLDAC 25
#define CURDAC 26
#define statLed 15

//Others:
#define updateTimerInterval 3   //  Interval in seconds for when the device should update the sql-database

//Function declarations:
void setupPins(void);
void initADC(void);
void update_db(void);
void IRAM_ATTR onTimer();
void initUpdateTimer(void);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/ 21, /* reset=*/ U8X8_PIN_NONE);

MCP3204 adc(ADC_VREF, CS_PIN);

//Struct with all 'secret settings'. They're given by the user over the serial port
typedef struct {
  char ssid[32];
  char pass[32];
  char user[32];
  char sqlpass[32];
  IPAddress sql_ip;
} creds_t;

creds_t creds;
Preferences prefs;

int setvol = 0;
int setcur = 0;
double voltage;
double current;
bool sql_ext;
bool sql_sta;
float sql_setv;
float sql_setc;
int sql_updfreq;

WiFiClient client;
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

volatile bool dataUpdateTime = false;   //  Variable is used in ISR so needs to be declared volatile

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void setup() {
  Serial.begin(115200);
  Serial.println("To change the network settings, you can start by typing the SSID of the WiFi-network you want to connect to.");
  setupPins();
  initADC();
  u8g2.begin();
  u8g2.setCursor(0, 10);
  u8g2.clearBuffer();
  fetchCredentials();
}

void loop()
{
  bool buttonState;
  static uint8_t connectionState = 0;

  dacWrite(VOLDAC, setvol); //Variable output is disabled right now, so for safety reasons write 0 to the DAC
  dacWrite(CURDAC, setcur); //Variable output is disabled right now, so for safety reasons write 0 to the DAC
  buttonState = digitalRead(button1);

  if (buttonState)
  {
    //only now connect to wifi and sql-database
    if ((WiFi.status() != WL_CONNECTED) && (connectionState != 1)) connectionState = 0;
    if ((WiFi.status() == WL_CONNECTED) && !conn.connected() && (connectionState != 3))  connectionState = 2;
    if ((WiFi.status() == WL_CONNECTED) && conn.connected() && (connectionState != 5))   connectionState = 4;

    switch (connectionState) {
      case 0:                 //  Connect to WiFi
        WiFi.mode(WIFI_STA);
        WiFi.begin(creds.ssid, creds.pass);
        connectionState = 1;
        break;
      case 1:
        break;
      case 2:                 //  Connect to SQL-server
        conn.connect(creds.sql_ip, 3306, creds.user, creds.sqlpass);
        connectionState = 3;
        break;
      case 3:
        break;
      case 4:                 //  SQL connection is established
        connectionState = 5;
        initUpdateTimer();
        digitalWrite(statLed, 1);
        break;
    }
  }

  else if (connectionState != 0) //if button is released
  {
    conn.close();
    WiFi.mode(WIFI_OFF);
    WiFi.disconnect();
    u8g2.setCursor(120, 60);
    u8g2.print(" ");
    digitalWrite(statLed, 0);
    connectionState = 0;
  }
  else  digitalWrite(statLed, 0);

  uint16_t adc1 = adc.read(MCP3204::Channel::SINGLE_0);
  uint16_t adc2 = adc.read(MCP3204::Channel::SINGLE_1);
  voltage = adc1 * 0.03685;
  current = adc2 * 0.0009768;

  u8g2.setFont(u8g2_font_profont10_mr);
  u8g2.setCursor(0, 10);
  u8g2.print("Voltage:");
  u8g2.setCursor(0, 40);
  u8g2.print("Current:");

  u8g2.setFont(u8g2_font_logisoso18_tn);
  u8g2.setCursor(40, 20);
  u8g2.print(voltage);
  u8g2.setCursor(40, 50);
  u8g2.print(current);
  u8g2.setFont(u8g2_font_profont10_mr);

  u8g2.setCursor(10, 25);
  (digitalRead(cvcc)) ? (u8g2.print("CC")) : (u8g2.print("CV")); //constant voltage or current
  u8g2.drawFrame(7, 17, 15, 10);

  if (digitalRead(ovp) || digitalRead(otp))   //  Show warning label if over-voltage or -temperature
  {
    u8g2.setFont(u8g2_font_twelvedings_t_all);
    static bool s = true;
    (s) ? (u8g2.drawGlyph(10, 60, 0x0021)) : (u8g2.drawGlyph(10, 60, 0x0020));  //warning label blinks
    s = !s;
  }
  u8g2.sendBuffer();
  u8g2.clearBuffer();

  if (dataUpdateTime) {
    portENTER_CRITICAL(&timerMux);
    dataUpdateTime = false;
    portEXIT_CRITICAL(&timerMux);

    if (connectionState == 5) update_db();
  }
  handleSerialInput();
}

void update_db(void)
{
  char cur[10];
  char vol[10];
  char INSERT_SQL[] = "INSERT INTO labpsu.log (voltage, current, overtemp) VALUES (%s, %s, %d)";
  char SELECT_SQL[] = "SELECT extern, standby, vset, cset, upd_freq FROM labpsu.settings WHERE 1";
  char query[128];

  cursor = new MySQL_Cursor(&conn);
  row_values *row = NULL;

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

  dtostrf(voltage, 4, 2, vol);
  dtostrf(current, 4, 2, cur);
  sprintf(query, INSERT_SQL, vol, cur, 0);
  cursor->execute(query);
  int nsql_updfreq;
  cursor->execute(SELECT_SQL);
  column_names *columns = cur_mem->get_columns();
  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
      sql_ext = atoi(row->values[0]);
      sql_sta = atoi(row->values[1]);
      sql_setv = atof(row->values[2]);
      sql_setc = atof(row->values[3]);
      nsql_updfreq = atoi(row->values[4]);
    }
  } while (row != NULL);
  cursor->close();
  if (nsql_updfreq != sql_updfreq) {
    sql_updfreq = nsql_updfreq;
    timerAlarmWrite(timer, updateTimerInterval * (sql_updfreq * 407), true);
  }
  delete cur_mem;
  digitalWrite(SELP, sql_ext);
  digitalWrite(StandbyP, sql_sta);
}

void initADC(void)
{
  // initialize SPI interface for MCP3208
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);
  adc.calibrate(MCP3204::Channel::SINGLE_0);
  adc.calibrate(MCP3204::Channel::SINGLE_1);
}

void initUpdateTimer(void)
{
  timer = timerBegin(0, 65536, true); //  Use timer 0, max prescaling, increase timer
  timerAttachInterrupt(timer, &onTimer, true);  //  Attach ISR updateTimer to timer
  timerAlarmWrite(timer, updateTimerInterval * 1221, true); //  Interrupt about once per 3 seconds, timer counts to 407 per second
  timerAlarmEnable(timer);
}

void setupPins(void)
{
  pinMode(CS_PIN, OUTPUT);
  pinMode(SELP, OUTPUT);
  pinMode(StandbyP, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(cvcc, INPUT_PULLUP);
  pinMode(ovp, INPUT_PULLUP);
  pinMode(otp, INPUT_PULLUP);
  pinMode(statLed, OUTPUT);

  // set initial output state
  digitalWrite(CS_PIN, HIGH);
  digitalWrite(SELP, LOW);
  digitalWrite(StandbyP, LOW);
  digitalWrite(statLed, LOW);
}

//Fetches the credentials from the NVM when powering up.
void fetchCredentials(void)
{
  prefs.begin("creds");
  prefs.getString("ssid", creds.ssid, 32);
  prefs.getString("pass", creds.pass, 32);
  prefs.getString("user", creds.user, 32);
  prefs.getString("sqlpass", creds.sqlpass, 32);
  char strIP[32];
  prefs.getString("ip", strIP, 32);
  IPAddress(creds.sql_ip.fromString(strIP));
  prefs.end();
}

void handleSerialInput(void) 
{
  if (Serial.available() > 0) 
  {
    static uint8_t settingState = 0;

    switch (settingState)
    {

      case 0: {
          String strSsid = Serial.readString(); //typecasting is neccesary
          strSsid.toCharArray(creds.ssid, strSsid.length() - 1);

          prefs.begin("creds");
          prefs.putString("ssid", creds.ssid);
          prefs.end();

          Serial.print("Received a WiFi-network SSID. Please now enter the password for: ");
          Serial.println(creds.ssid);
          settingState = 1;
        }
        break;

      case 1: {
          String strPass = Serial.readString();
          strPass.toCharArray(creds.pass, strPass.length() - 1);

          prefs.begin("creds");
          prefs.putString("pass", creds.pass);
          prefs.end();

          settingState = 2;
          Serial.println("Received a password. The given network will be used next time a WiFi-connection is made. Also change mySQL settings? (Y/N)");
        }
        break;

      case 2:
        if (Serial.read() == 'Y')
        {
          Serial.println("Okay! Please give MySQL username.");
          settingState = 3;
        }
        else {
          Serial.println("Okay, see you later!");
          settingState = 0;
        }
        Serial.flush();
        break;

      case 3: {
          String strUser = Serial.readString();
          strUser.toCharArray(creds.user, strUser.length() - 1);

          prefs.begin("creds");
          prefs.putString("user", creds.user);
          prefs.end();

          settingState = 4;
          Serial.println("Got the MySQL username. Next the password.");
        }
        break;

      case 4: {
          String strSqlpass = Serial.readString();
          strSqlpass.toCharArray(creds.sqlpass, strSqlpass.length() - 1);

          prefs.begin("creds");
          prefs.putString("sqlpass", creds.sqlpass);
          prefs.end();

          settingState = 5;
          Serial.println("Received the password. What is the server IP? (format: 127.0.0.1)");
        }
        break;


      case 5: {
          String strIP = Serial.readString();
          strIP.trim();
          IPAddress(creds.sql_ip.fromString(strIP));
          prefs.begin("creds");
          prefs.putString("ip", strIP);
          prefs.end();

          settingState = 0;
          Serial.print("Received the server IP as well ("); Serial.print(strIP); Serial.println(")."); 
          Serial.println("Everything is stored in the NVM. We are done.");
        }
        break;
    }
  }
}

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  dataUpdateTime = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}
