#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// =====================================================
// LCD
// =====================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================================
// GPS
// Arduino D7 = RX <- GPS TX
// Arduino D8 = TX -> GPS RX
// =====================================================
SoftwareSerial gpsSerial(7, 8);

// =====================================================
// ULTRASONIC SENSOR 1
// =====================================================
#define TRIG1 2
#define ECHO1 3

// =====================================================
// ULTRASONIC SENSOR 2
// =====================================================
#define TRIG2 4
#define ECHO2 5

// =====================================================
// VIBRATION SENSOR
// =====================================================
#define VIBRATION_PIN 6

// =====================================================
// DISTANCE LIMIT
// =====================================================
#define DISTANCE_LIMIT 3.0

// =====================================================
// MALLA REDDY UNIVERSITY TEST COORDINATES
// =====================================================
#define DEFAULT_LATITUDE  17.562431
#define DEFAULT_LONGITUDE 78.444306


// =====================================================
// GET DISTANCE
// =====================================================
float getDistance(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0)
  {
    return -1;
  }

  float distance = duration * 0.0343 / 2;

  return distance;
}


// =====================================================
// SETUP
// =====================================================
void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(9600);

  // Ultrasonic 1
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  // Ultrasonic 2
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Vibration sensor
  pinMode(VIBRATION_PIN, INPUT);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Railway Track");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");

  delay(2000);

  lcd.clear();

  Serial.println("================================");
  Serial.println(" SMART RAILWAY TRACK MONITOR");
  Serial.println("================================");

  Serial.println("System Initialized");
  Serial.println("Ultrasonic 1 : READY");
  Serial.println("Ultrasonic 2 : READY");
  Serial.println("Vibration    : READY");
  Serial.println("LCD          : READY");
  Serial.println("GPS          : READY");
  Serial.println();
}


// =====================================================
// MAIN LOOP
// =====================================================
void loop()
{
  // ---------------------------------------------------
  // READ ULTRASONIC SENSORS
  // ---------------------------------------------------

  float distance1 = getDistance(TRIG1, ECHO1);

  delay(50);

  float distance2 = getDistance(TRIG2, ECHO2);


  // ---------------------------------------------------
  // READ VIBRATION
  // ---------------------------------------------------

  int vibration = digitalRead(VIBRATION_PIN);


  // ---------------------------------------------------
  // READ RAW GPS DATA
  // ---------------------------------------------------

  while (gpsSerial.available())
  {
    char gpsData = gpsSerial.read();

    // Display raw GPS data on Serial Monitor
    Serial.write(gpsData);
  }


  // ---------------------------------------------------
  // CHECK DISTANCE
  // ---------------------------------------------------

  bool distanceAbnormal = false;

  if (distance1 > 0 && distance1 <= DISTANCE_LIMIT)
  {
    distanceAbnormal = true;
  }

  if (distance2 > 0 && distance2 <= DISTANCE_LIMIT)
  {
    distanceAbnormal = true;
  }


  // ---------------------------------------------------
  // CHECK VIBRATION
  // ---------------------------------------------------

  bool vibrationDetected = false;

  if (vibration == HIGH)
  {
    vibrationDetected = true;
  }


  // ---------------------------------------------------
  // FINAL ABNORMALITY CONDITION
  // ---------------------------------------------------

  bool abnormal = distanceAbnormal || vibrationDetected;


  // ===================================================
  // SERIAL OUTPUT
  // ===================================================

  Serial.println();
  Serial.println("--------------------------------");

  Serial.print("Ultrasonic 1: ");

  if (distance1 < 0)
  {
    Serial.println("No Echo");
  }
  else
  {
    Serial.print(distance1);
    Serial.println(" cm");
  }


  Serial.print("Ultrasonic 2: ");

  if (distance2 < 0)
  {
    Serial.println("No Echo");
  }
  else
  {
    Serial.print(distance2);
    Serial.println(" cm");
  }


  Serial.print("Vibration: ");

  if (vibrationDetected)
  {
    Serial.println("DETECTED");
  }
  else
  {
    Serial.println("NORMAL");
  }


  // ===================================================
  // ABNORMAL CONDITION
  // ===================================================

  if (abnormal)
  {
    Serial.println();
    Serial.println("!!! ABNORMALITY DETECTED !!!");

    if (distanceAbnormal)
    {
      Serial.println("Reason: Abnormal Distance");
    }

    if (vibrationDetected)
    {
      Serial.println("Reason: Vibration Detected");
    }


    // -------------------------------------------------
    // LCD ALERT
    // -------------------------------------------------

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("!!! ALERT !!!");

    lcd.setCursor(0, 1);
    lcd.print("TRACK ABNORMAL");

    delay(1500);


    // -------------------------------------------------
    // SHOW TEST GPS LOCATION
    // -------------------------------------------------

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("GPS LOCATION");

    lcd.setCursor(0, 1);
    lcd.print("MRU");

    Serial.println();
    Serial.println("GPS TEST LOCATION");
    Serial.println("Malla Reddy University");

    Serial.print("Latitude : ");
    Serial.println(DEFAULT_LATITUDE, 6);

    Serial.print("Longitude: ");
    Serial.println(DEFAULT_LONGITUDE, 6);

    delay(1500);


    // Latitude on LCD
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Latitude:");

    lcd.setCursor(0, 1);
    lcd.print(DEFAULT_LATITUDE, 4);

    delay(1500);


    // Longitude on LCD
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Longitude:");

    lcd.setCursor(0, 1);
    lcd.print(DEFAULT_LONGITUDE, 4);

    delay(1500);
  }


  // ===================================================
  // NORMAL CONDITION
  // ===================================================

  else
  {
    Serial.println();
    Serial.println("STATUS: TRACK NORMAL");


    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("TRACK STATUS:");

    lcd.setCursor(0, 1);
    lcd.print("NORMAL");
  }


  delay(1000);
}
