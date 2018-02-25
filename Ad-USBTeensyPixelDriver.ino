// Functions are:
// a = set number of pixels, a|X where X is number of pixels function cmd a parameter a|x
// b = set brightness, values are 0 to 100, but need to internally modify since 0 = max brightness
// (color values are interpreted literally; no scaling), 1 = min
// brightness (off), 255 = just below max brightness. function cmd b,x
//c = update a set of pixels at indexes to the RGB colors function cmd c parameters c|index(r,g,b)index(r,g,b)
//d = turn off all pixels
#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include <string.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel m_strip = Adafruit_NeoPixel(512, PIN, NEO_GRB + NEO_KHZ800);
String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

String test = "";

void setup() {
  m_strip.begin();
  m_strip.setBrightness(100);
  m_strip.show(); // Initialize all pixels to 'off'
  // put your setup code here, to run once:
  Serial.begin(115200); // start serial for output

  // reserve 200 bytes for the inputString:
  inputString.reserve(65000);

  Serial.println("Start");
}

void loop() {
  if (stringComplete) {
    //Serial.println("inputString = " + inputString);
    Serial.println("inputStringlength = " + String(inputString.length()));
    switch (getCommand(inputString)) {
      case 1:
      {
        //Serial.println("before m_strip.numPixels() = " + String(m_strip.numPixels()));
        //Serial.println("getOnlyParameterAsInt() = " + String(getOnlyParameterAsInt(inputString)));
        uint16_t stringLength = getOnlyParameterAsInt(inputString);
        //Serial.println("stringLength = " + String(stringLength));
        updateLength(stringLength);
        //Serial.println("m_strip.numPixels() = " + String(m_strip.numPixels()));
        break;
      }
      case 2:
      {
        //Serial.println("case 2 getOnlyParameter() = " + String(getOnlyParameterAsInt(inputString)));
        updateBrightness(getOnlyParameterAsInt(inputString));
        break;
      }
      case 3:
      {
        //Serial.println("case 3 = " );
        //test = getOnlyParameterAsString(inputString);
        //Serial.println("test = " + test);
        setPixels(getOnlyParameterAsString(inputString));
        break;
      }
      case 4:
      {
        off();
        break;
      }
      case 5:
      {
        Serial.println("light all");
        for (int i = 0; i < m_strip.numPixels(); i++)
        {
          m_strip.setPixelColor(i, m_strip.Color(255, 255, 255));
        }
        m_strip.show();
        break;
      }
      default:
      {
        Serial.println("default = " + String(getCommand(inputString)));
        break;
      }
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

/*
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    inputString = Serial.readString();
    stringComplete = true;
  }
}
*/


int getCommand(String inputString) {
  return (uint8_t)atol(inputString.substring(0, 1).c_str());
}

uint16_t getOnlyParameterAsInt(String inputString) {
  String numPixelsAsString = inputString.substring(2);
  //Serial.println("numPixelsAsString = " + numPixelsAsString);
  const char* ptr = numPixelsAsString.c_str();
  //Serial.println("ptr = " + String(ptr));
  //uint16_t myLong = atol(ptr);
  //Serial.println("myLong = " + String(myLong));
  return atol(ptr);
}

String getOnlyParameterAsString(String inputString) {
  //Serial.println("getOnlyParameterAsString(inputString) = " + inputString);
  return String(inputString.substring(2));
}

void updateLength(uint16_t numPixels) {
  //Serial.println("updateLength(numPixels) = " + String(numPixels));
  m_strip = Adafruit_NeoPixel(numPixels, PIN, NEO_GRB + NEO_KHZ800);
  m_strip.show(); // Initialize all pixels to 'off'
}

void updateBrightness(int brightness) {
  //Serial.println("convertFromPercentToStripBrightness = " + String(convertFromPercentToStripBrightness(brightness)));
  m_strip.setBrightness(convertFromPercentToStripBrightness(brightness));
  m_strip.show();
}

uint8_t convertFromPercentToStripBrightness(uint8_t percent)
{
  //0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  if (percent == 100)
  {
    return 255;
  }
  else
  {
    return (uint8_t)(((float)255) * (((float)percent) / 100));
  }
}

void setPixels(String pixelsAndColor) {
  Serial.println("setPixels = " + pixelsAndColor);
  String lclPixelsAndColor = pixelsAndColor;
  //Serial.println("lclPixelsAndColor.length() = " + String(lclPixelsAndColor.length()));
  uint16_t thisPixelColorStart = 0;
  uint16_t thisPixelEndingData = 0;
  uint16_t firstComma = 0;
  uint16_t secondComma = 0;
  uint16_t pixelIndex = 0;
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  while(lclPixelsAndColor.length() > 1){
    thisPixelColorStart = lclPixelsAndColor.indexOf("(");
    thisPixelEndingData = lclPixelsAndColor.indexOf(")");
    pixelIndex = lclPixelsAndColor.substring(0, thisPixelColorStart).toFloat();
    firstComma = lclPixelsAndColor.indexOf(",", thisPixelColorStart);
    secondComma = lclPixelsAndColor.indexOf(",", firstComma+1);
    r = lclPixelsAndColor.substring(thisPixelColorStart+1, firstComma).toFloat();
    g = lclPixelsAndColor.substring(firstComma+1, secondComma).toFloat();
    b = lclPixelsAndColor.substring(secondComma+1, thisPixelEndingData).toFloat();
    lclPixelsAndColor = lclPixelsAndColor.substring(thisPixelEndingData+1);
    //Serial.println("pixelIndex = " + String(pixelIndex) + ", r = " + String(r) + ", g = " + String(g) + ", b = " + String(b) + " lclPixelsAndColor = " + lclPixelsAndColor + " lclPixelsAndColor.length() = " + String(lclPixelsAndColor.length()));
    m_strip.setPixelColor(pixelIndex, r, g, b);
  }
  m_strip.show();
}

void off() {
  for (int i = 0; i < m_strip.numPixels(); i++)
  {
    m_strip.setPixelColor(i, m_strip.Color(0, 0, 0));
  }
  m_strip.show();
}
/*
  // callback for received data
  void receiveEvent(int howMany) {
  String recdData = "";
  int numOfBytes = Wire.available();
  //display number of bytes and cmd received, as bytes

  char cmd = Wire.read();  //cmd
  //display message received, as char
  for (int i = 0; i < numOfBytes - 1; i++) {
    recdData += char(Wire.read());
  }

  char recdDataChar[recdData.length() + 1];
  recdData.toCharArray(recdDataChar, sizeof(recdDataChar));

  switch (cmd) {
    case 'a':
      m_numPixels = parseSingleInt16FromParameter(recdDataChar);
      m_strip.updateLength(m_numPixels);
      m_strip.show(); // Initialize all pixels to 'off'
      break;
    case 'b':
      m_strip.setBrightness(convertFromPercentToStripBrightness(parseSingleInt8FromParameter(recdDataChar)));
      m_strip.show();
      break;
    case 'c':
      //Serial.println("recdData = " + (String)recdData + " recdDataChar = "+ (String)recdDataChar + " parsePixelIndexFromParameter = " + (String)parsePixelIndexFromParameter(recdDataChar) + " parseColorFromParameter = " + (String)parseColorFromParameter(recdDataChar));
      m_strip.setPixelColor(parsePixelIndexFromParameter(recdDataChar), parseColorFromParameter(recdDataChar));
      m_strip.show();
      break;
    case 'd':
      for (int i = 0; i < m_numPixels; i++)
      {
        m_strip.setPixelColor(i, m_strip.Color(0, 0, 0));
      }
      m_strip.show();
      break;
    default:
      Serial.println("switch default");
      break;
  }
  }

  uint16_t parsePixelIndexFromParameter(char* parameter)
  {
  char* ptr = strtok(parameter, ",");
  return atol(ptr);
  }

  uint32_t parseColorFromParameter(char* parameter)
  {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  int firstComma = ((String)parameter).indexOf(',');
  r = atol((((String)parameter).substring(firstComma + 1, firstComma + 4)).c_str());
  g = atol((((String)parameter).substring(firstComma + 5, firstComma + 8)).c_str());
  b = atol((((String)parameter).substring(firstComma + 9, firstComma + 12)).c_str());

  return m_strip.Color(r, g, b);
  }

  uint16_t parseSingleInt16FromParameter(char* parameter)
  {
  return (uint16_t)atol(parameter);
  }

  uint8_t parseSingleInt8FromParameter(char* parameter)
  {
  return (uint8_t)atol(parameter);
  }

  uint8_t convertFromPercentToStripBrightness(uint8_t percent)
  {
  //0 = max brightness
  // (color values are interpreted literally; no scaling), 1 = min
  // brightness (off), 255 = just below max brightness.
  if (percent == 100)
  {
    return 255;
  }
  else
  {
    return (uint8_t)(((float)255) * (((float)percent) / 100));
  }
  }
*/
