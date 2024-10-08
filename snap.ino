#include <LiquidCrystal.h>
LiquidCrystal lcd(13,12,11,10,9,8);


#include <Adafruit_Fingerprint.h>
#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;


#include <Servo.h>
Servo myservo;


int check=0;
int g;


void setup() {
  pinMode(14,INPUT);
  pinMode(15,INPUT);
  pinMode(16,INPUT);
  pinMode(17,INPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  myservo.attach(5);

  
  myservo.write(0);
  
  
  analogWrite(4,0);
  lcd.begin(16,2);
  lcd.print("Fingerprnt based");
  lcd.setCursor(0,1);
  lcd.print("Security System");
  delay(5000);
  lcd.clear();
  lcd.print("Group No. 15");
  lcd.setCursor(0,1);
  lcd.print("Branch - DSAI");
  delay(5000);
  lcd.clear();



  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    lcd.print("Found finger");
    lcd.setCursor(0,1);
    lcd.print("print sensor!");
    delay(1500);
    lcd.clear();
  } else {
    lcd.print("Sensor not found");
    lcd.clear();
    while (1) { delay(1); }
  }

  startsys();
}

void loop() 
{
  g=0;
  digitalWrite(7, HIGH);
  if(digitalRead(14)==0)
  {
    while(!getFingerprintEnroll())
    {
      continue;
    }      
    startsys();
  }
  if (check==1)
  {
    while(!getFingerprintEnroll())
    {
      continue;
    }      
    startsys();
  }
  else
  {
    if(digitalRead(16)==0 || digitalRead(17)==0)
    {
      for(int i=0;i<=50;i++)
      {
        getFingerprintID();
        delay(50);
        if (g==1)
          break;
      }
      if (g==0)
      {
        lcd.clear();
        lcd.print("Please put your");
        lcd.setCursor(0,1);
        lcd.print("finger on sensor");
        delay(2500);
        lcd.clear();
        startsys();
      }
    }
    else if (digitalRead(15)==0)
    {
      
      lcd.clear();
      lcd.print("Y-Confirm Delete");
      lcd.setCursor(0,1);
      lcd.print("N-Cancel Delete");
      start:
      if (digitalRead(16)==0)
      {
        finger.emptyDatabase();
      }
      else if(digitalRead(17)==0 || digitalRead(14)==0)
      {
        startsys();
      }
      else
        goto start;
    }
  }
}



void startsys()
{
  finger.getTemplateCount();
  if (finger.templateCount==0)
  {
    lcd.print("templates not");
    lcd.setCursor(0,1);
    lcd.print("found, enroll!");
    check=1;
    delay(4000);
    lcd.clear();
  }
  else
  {
    lcd.print("Press UP/Down");
    lcd.setCursor(0,1);
    lcd.print("to start System ");
    check=0;
  }
}



void del()
{
  if (digitalRead(15)==0)
  {
    digitalWrite(6,HIGH);
    delay(1000);
    digitalWrite(6,LOW);
  }
}




uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
      return p;
  }
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
      lcd.clear();
      lcd.print("Somekind of"); 
      lcd.setCursor(0,1);
      lcd.print("error occurred");
      for(int i=0;i<7;i++)
      {
        digitalWrite(6, LOW);
        delay(50);
        digitalWrite(6, LOW);
        delay(50);
      }
      lcd.clear();
      g=1;
      startsys();
      return p;
  }
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) 
  {
    myservo.write(180);
    lcd.clear();
    lcd.print("Found ID #"); lcd.print(finger.fingerID);
    lcd.setCursor(0,1);
    lcd.print("Confidence : "); lcd.print(finger.confidence);
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    delay(2500);
    myservo.write(0);
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
    lcd.clear();
    g=1;
    startsys();
  }
  else 
  {
    lcd.clear();
    lcd.print("Fingerprint not"); 
    lcd.setCursor(0,1);
    lcd.print("found...");
    for(int i=0;i<7;i++)
    {
      digitalWrite(7, LOW);
      delay(50);
      digitalWrite(7, HIGH);
      delay(50);
    }
    lcd.clear();
    g=1;
    startsys();
  }
}




uint8_t getFingerprintEnroll() 
{
  finger.getTemplateCount();
  int id=finger.templateCount+1;
  int p = -1;
  lcd.clear();
  lcd.print("valid finger to "); 
  lcd.setCursor(0,1);
  lcd.print("enroll as #");lcd.print(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image taken(1)");
      delay(2000);
      lcd.clear();
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      break;
    case FINGERPRINT_IMAGEFAIL:
      break;
    default:
      break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }

  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  lcd.clear();
  p = -1;
  lcd.print("Place same ");
  lcd.setCursor(0,1);
  lcd.print("finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image taken(2)");
      delay(2000);
      lcd.clear();
      break;
    case FINGERPRINT_NOFINGER:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      break;
    case FINGERPRINT_IMAGEFAIL:
      break;
    default:
      break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  }

  lcd.print("Creating model  ");  
  lcd.setCursor(0,1);
  lcd.print("for #");lcd.print(id);
  delay(3000);
  lcd.clear();

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.print("Prints matched!");
    delay(1000);
    lcd.clear();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Com error");
    delay(1000);
    lcd.clear();
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.print("Not matched");
    delay(1000);
    lcd.clear();
    return p;
  } else {
    lcd.print("Unknown error");
    delay(1000);
    lcd.clear();
    return p;
  }

  lcd.print("ID #"); lcd.println(id);
  lcd.setCursor(0,1);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    lcd.print("Stored!");
    delay(1000);
    lcd.clear();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.print("Com error");
    delay(1000);
    lcd.clear();
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    lcd.print("Bad location");
    delay(1000);
    lcd.clear();
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    lcd.print("Error flash");
    delay(1000);
    lcd.clear();
    return p;
  } else {
    lcd.print("Unknown error");
    delay(1000);
    lcd.clear();
    return p;
  }
  return true;
}
