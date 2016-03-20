/*--------------------------------------------------------------------------*
* IoThing Configuration Library
*---------------------------------------------------------------------------*
* 06-Feb-2016 ShaneG
*
* Initial version
*--------------------------------------------------------------------------*/
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "DNSServer.h"
#include "EEPROM.h"
#include "TGL.h"
#include "IotConfig.h"
#include "Json.h"

//--- SoftAP configuration
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

//--- Constants
#define WIFI_SSID     "ssid"
#define WIFI_PASSWORD "password"

//---------------------------------------------------------------------------
// Configuration page.
//
// This is the HTML page used for configuration converted to a character
// array with the 'mkpage.py' script
//---------------------------------------------------------------------------

const char CONFIG_PAGE[] PROGMEM = {
  0x3c, 0x21, 0x44, 0x4f, 0x43, 0x54, 0x59, 0x50, 0x45, 0x20, 0x68, 0x74,
  0x6d, 0x6c, 0x3e, 0x0a, 0x3c, 0x68, 0x74, 0x6d, 0x6c, 0x3e, 0x0a, 0x3c,
  0x68, 0x65, 0x61, 0x64, 0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x74, 0x69, 0x74,
  0x6c, 0x65, 0x3e, 0x57, 0x69, 0x46, 0x69, 0x20, 0x53, 0x65, 0x74, 0x74,
  0x69, 0x6e, 0x67, 0x73, 0x3c, 0x2f, 0x74, 0x69, 0x74, 0x6c, 0x65, 0x3e,
  0x0a, 0x20, 0x20, 0x3c, 0x73, 0x74, 0x79, 0x6c, 0x65, 0x3e, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x62, 0x6f, 0x64, 0x79, 0x20, 0x7b, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x70, 0x61, 0x64, 0x64, 0x69, 0x6e, 0x67, 0x3a,
  0x20, 0x30, 0x2e, 0x35, 0x65, 0x6d, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x6d, 0x61, 0x72, 0x67, 0x69, 0x6e, 0x3a, 0x20, 0x61, 0x75,
  0x74, 0x6f, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6f,
  0x6e, 0x74, 0x2d, 0x66, 0x61, 0x6d, 0x69, 0x6c, 0x79, 0x3a, 0x20, 0x41,
  0x72, 0x69, 0x61, 0x6c, 0x2c, 0x20, 0x48, 0x65, 0x6c, 0x76, 0x65, 0x74,
  0x69, 0x63, 0x61, 0x2c, 0x20, 0x73, 0x61, 0x6e, 0x73, 0x2d, 0x73, 0x65,
  0x72, 0x69, 0x66, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x63,
  0x6f, 0x6c, 0x6f, 0x72, 0x3a, 0x20, 0x62, 0x6c, 0x61, 0x63, 0x6b, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x40, 0x6d, 0x65, 0x64, 0x69, 0x61, 0x20, 0x73, 0x63, 0x72, 0x65,
  0x65, 0x6e, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x6d, 0x61, 0x78, 0x2d,
  0x77, 0x69, 0x64, 0x74, 0x68, 0x3a, 0x20, 0x31, 0x30, 0x32, 0x34, 0x70,
  0x78, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x62,
  0x6f, 0x64, 0x79, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x70, 0x61, 0x64, 0x64, 0x69, 0x6e, 0x67, 0x2d, 0x6c, 0x65,
  0x66, 0x74, 0x3a, 0x20, 0x33, 0x65, 0x6d, 0x3b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x61, 0x64, 0x64, 0x69, 0x6e, 0x67,
  0x2d, 0x72, 0x69, 0x67, 0x68, 0x74, 0x3a, 0x20, 0x33, 0x65, 0x6d, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x6d, 0x61, 0x78,
  0x2d, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3a, 0x20, 0x31, 0x30, 0x30, 0x25,
  0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6f,
  0x6e, 0x74, 0x2d, 0x73, 0x69, 0x7a, 0x65, 0x3a, 0x20, 0x32, 0x34, 0x70,
  0x74, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x40, 0x6d, 0x65, 0x64, 0x69, 0x61, 0x20, 0x73, 0x63, 0x72, 0x65,
  0x65, 0x6e, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x28, 0x6d, 0x69, 0x6e, 0x2d,
  0x77, 0x69, 0x64, 0x74, 0x68, 0x3a, 0x20, 0x31, 0x30, 0x32, 0x34, 0x70,
  0x78, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x62,
  0x6f, 0x64, 0x79, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x6d, 0x61, 0x78, 0x2d, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3a,
  0x20, 0x32, 0x30, 0x65, 0x6d, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x6c, 0x61, 0x62, 0x65, 0x6c, 0x20, 0x7b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6f, 0x6e, 0x74, 0x2d,
  0x73, 0x74, 0x79, 0x6c, 0x65, 0x3a, 0x20, 0x62, 0x6f, 0x6c, 0x64, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6f, 0x6e, 0x74, 0x2d,
  0x73, 0x69, 0x7a, 0x65, 0x3a, 0x20, 0x31, 0x2e, 0x32, 0x65, 0x6d, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72,
  0x3a, 0x20, 0x23, 0x30, 0x30, 0x33, 0x33, 0x63, 0x63, 0x3b, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x69,
  0x6e, 0x70, 0x75, 0x74, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x64, 0x69, 0x73, 0x70, 0x6c, 0x61, 0x79, 0x3a, 0x20, 0x62, 0x6c,
  0x6f, 0x63, 0x6b, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x77,
  0x69, 0x64, 0x74, 0x68, 0x3a, 0x20, 0x31, 0x30, 0x30, 0x25, 0x3b, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x6d, 0x61, 0x72, 0x67, 0x69, 0x6e,
  0x2d, 0x74, 0x6f, 0x70, 0x3a, 0x20, 0x30, 0x2e, 0x33, 0x65, 0x6d, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6f, 0x6e, 0x74, 0x2d,
  0x73, 0x69, 0x7a, 0x65, 0x3a, 0x20, 0x31, 0x2e, 0x32, 0x35, 0x65, 0x6d,
  0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x62, 0x75, 0x74, 0x74, 0x6f, 0x6e, 0x20, 0x7b, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6f, 0x6e, 0x74, 0x2d, 0x73, 0x69,
  0x7a, 0x65, 0x3a, 0x20, 0x31, 0x2e, 0x32, 0x35, 0x65, 0x6d, 0x3b, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x2e, 0x68, 0x65, 0x61, 0x64, 0x65, 0x72, 0x20, 0x7b, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x3a, 0x20, 0x23,
  0x30, 0x30, 0x33, 0x33, 0x63, 0x63, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x2e, 0x68, 0x65, 0x61,
  0x64, 0x65, 0x72, 0x20, 0x70, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x3a, 0x20, 0x62, 0x6c, 0x61,
  0x63, 0x6b, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x2e, 0x66, 0x6f, 0x6f, 0x74, 0x65, 0x72, 0x20,
  0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x6d, 0x61, 0x72, 0x67,
  0x69, 0x6e, 0x2d, 0x74, 0x6f, 0x70, 0x3a, 0x20, 0x31, 0x65, 0x6d, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x2e, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x7b, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2d, 0x61, 0x6c, 0x69,
  0x67, 0x6e, 0x3a, 0x20, 0x72, 0x69, 0x67, 0x68, 0x74, 0x3b, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x23,
  0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x2c, 0x20, 0x23, 0x65, 0x72, 0x72,
  0x6f, 0x72, 0x2c, 0x20, 0x23, 0x66, 0x69, 0x6e, 0x69, 0x73, 0x68, 0x65,
  0x64, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x64, 0x69,
  0x73, 0x70, 0x6c, 0x61, 0x79, 0x3a, 0x20, 0x6e, 0x6f, 0x6e, 0x65, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x23, 0x73, 0x68, 0x6f, 0x77, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3a, 0x20, 0x61, 0x75,
  0x74, 0x6f, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x64, 0x69,
  0x73, 0x70, 0x6c, 0x61, 0x79, 0x3a, 0x20, 0x69, 0x6e, 0x6c, 0x69, 0x6e,
  0x65, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20,
  0x20, 0x3c, 0x2f, 0x73, 0x74, 0x79, 0x6c, 0x65, 0x3e, 0x0a, 0x20, 0x20,
  0x3c, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x3e, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x73, 0x68,
  0x6f, 0x77, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x28, 0x6e, 0x61,
  0x6d, 0x65, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x76, 0x61, 0x72, 0x20, 0x68, 0x69, 0x64, 0x64, 0x65, 0x6e, 0x20, 0x3d,
  0x20, 0x5b, 0x20, 0x22, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x22,
  0x2c, 0x20, 0x22, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x22, 0x2c, 0x20,
  0x22, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x22, 0x2c, 0x20, 0x22, 0x66, 0x69,
  0x6e, 0x69, 0x73, 0x68, 0x65, 0x64, 0x22, 0x20, 0x5d, 0x3b, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6f, 0x72, 0x28, 0x76, 0x61, 0x72,
  0x20, 0x69, 0x3d, 0x30, 0x3b, 0x20, 0x69, 0x3c, 0x68, 0x69, 0x64, 0x64,
  0x65, 0x6e, 0x2e, 0x6c, 0x65, 0x6e, 0x67, 0x74, 0x68, 0x3b, 0x20, 0x69,
  0x2b, 0x2b, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x68, 0x69, 0x64, 0x64, 0x65, 0x6e,
  0x5b, 0x69, 0x5d, 0x20, 0x3d, 0x3d, 0x20, 0x6e, 0x61, 0x6d, 0x65, 0x29,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x64,
  0x6f, 0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x67, 0x65, 0x74, 0x45,
  0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x68,
  0x69, 0x64, 0x64, 0x65, 0x6e, 0x5b, 0x69, 0x5d, 0x29, 0x2e, 0x73, 0x74,
  0x79, 0x6c, 0x65, 0x2e, 0x64, 0x69, 0x73, 0x70, 0x6c, 0x61, 0x79, 0x20,
  0x3d, 0x20, 0x22, 0x62, 0x6c, 0x6f, 0x63, 0x6b, 0x22, 0x3b, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x64, 0x6f,
  0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x67, 0x65, 0x74, 0x45, 0x6c,
  0x65, 0x6d, 0x65, 0x6e, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x68, 0x69,
  0x64, 0x64, 0x65, 0x6e, 0x5b, 0x69, 0x5d, 0x29, 0x2e, 0x73, 0x74, 0x79,
  0x6c, 0x65, 0x2e, 0x64, 0x69, 0x73, 0x70, 0x6c, 0x61, 0x79, 0x20, 0x3d,
  0x20, 0x22, 0x6e, 0x6f, 0x6e, 0x65, 0x22, 0x3b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x7d, 0x0a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x75, 0x6e, 0x63,
  0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6f, 0x6e, 0x43, 0x6f, 0x6e, 0x66, 0x69,
  0x67, 0x28, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x73, 0x29, 0x20, 0x7b, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65,
  0x6e, 0x74, 0x2e, 0x67, 0x65, 0x74, 0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e,
  0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x22, 0x73, 0x73, 0x69, 0x64, 0x22,
  0x29, 0x2e, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x20, 0x3d, 0x20, 0x76, 0x61,
  0x6c, 0x75, 0x65, 0x73, 0x5b, 0x22, 0x73, 0x73, 0x69, 0x64, 0x22, 0x5d,
  0x20, 0x7c, 0x7c, 0x20, 0x22, 0x22, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x73, 0x68, 0x6f, 0x77, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f,
  0x6e, 0x28, 0x22, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x22, 0x29, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x67,
  0x65, 0x74, 0x4a, 0x73, 0x6f, 0x6e, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e,
  0x73, 0x65, 0x28, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x2c, 0x20, 0x70,
  0x61, 0x79, 0x6c, 0x6f, 0x61, 0x64, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x73, 0x74, 0x61, 0x74,
  0x75, 0x73, 0x20, 0x21, 0x3d, 0x20, 0x32, 0x30, 0x30, 0x29, 0x20, 0x7b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x68, 0x6f,
  0x77, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x28, 0x22, 0x65, 0x72,
  0x72, 0x6f, 0x72, 0x22, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20, 0x6e, 0x75,
  0x6c, 0x6c, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x74, 0x72, 0x79, 0x20,
  0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x72, 0x65,
  0x74, 0x75, 0x72, 0x6e, 0x20, 0x4a, 0x53, 0x4f, 0x4e, 0x2e, 0x70, 0x61,
  0x72, 0x73, 0x65, 0x28, 0x70, 0x61, 0x79, 0x6c, 0x6f, 0x61, 0x64, 0x29,
  0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x63, 0x61, 0x74, 0x63, 0x68, 0x28,
  0x65, 0x72, 0x72, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x73, 0x68, 0x6f, 0x77, 0x53, 0x65, 0x63, 0x74, 0x69,
  0x6f, 0x6e, 0x28, 0x22, 0x65, 0x72, 0x72, 0x6f, 0x72, 0x22, 0x29, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20,
  0x6e, 0x75, 0x6c, 0x6c, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x7d, 0x0a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x75, 0x6e, 0x63, 0x74,
  0x69, 0x6f, 0x6e, 0x20, 0x73, 0x61, 0x76, 0x65, 0x43, 0x68, 0x61, 0x6e,
  0x67, 0x65, 0x73, 0x28, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x2f, 0x2f, 0x20, 0x47, 0x65, 0x74, 0x20, 0x74, 0x68, 0x65,
  0x20, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x20, 0x66, 0x72, 0x6f, 0x6d,
  0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x76, 0x61, 0x72, 0x20, 0x72, 0x65, 0x73, 0x75,
  0x6c, 0x74, 0x20, 0x3d, 0x20, 0x7b, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x72, 0x65, 0x73, 0x75, 0x6c, 0x74, 0x5b, 0x22, 0x73,
  0x73, 0x69, 0x64, 0x22, 0x5d, 0x20, 0x3d, 0x20, 0x64, 0x6f, 0x63, 0x75,
  0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x67, 0x65, 0x74, 0x45, 0x6c, 0x65, 0x6d,
  0x65, 0x6e, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x22, 0x73, 0x73, 0x69,
  0x64, 0x22, 0x29, 0x2e, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x3b, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x72, 0x65, 0x73, 0x75, 0x6c, 0x74, 0x5b,
  0x22, 0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64, 0x22, 0x5d, 0x20,
  0x3d, 0x20, 0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x67,
  0x65, 0x74, 0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x42, 0x79, 0x49,
  0x64, 0x28, 0x22, 0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64, 0x22,
  0x29, 0x2e, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x3b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x2f, 0x2f, 0x20, 0x50, 0x6f, 0x73, 0x74, 0x20, 0x69,
  0x74, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x65, 0x72,
  0x76, 0x65, 0x72, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x76, 0x61,
  0x72, 0x20, 0x75, 0x72, 0x6c, 0x20, 0x3d, 0x20, 0x22, 0x68, 0x74, 0x74,
  0x70, 0x3a, 0x2f, 0x2f, 0x22, 0x20, 0x2b, 0x20, 0x6c, 0x6f, 0x63, 0x61,
  0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x68, 0x6f, 0x73, 0x74, 0x6e, 0x61, 0x6d,
  0x65, 0x20, 0x2b, 0x20, 0x28, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f,
  0x6e, 0x2e, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x3f, 0x20, 0x27, 0x3a, 0x27,
  0x20, 0x2b, 0x20, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e,
  0x70, 0x6f, 0x72, 0x74, 0x20, 0x3a, 0x20, 0x27, 0x27, 0x29, 0x20, 0x2b,
  0x20, 0x22, 0x2f, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x22, 0x3b, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x76, 0x61, 0x72, 0x20, 0x78, 0x6d,
  0x6c, 0x48, 0x74, 0x74, 0x70, 0x20, 0x3d, 0x20, 0x6e, 0x65, 0x77, 0x20,
  0x58, 0x4d, 0x4c, 0x48, 0x74, 0x74, 0x70, 0x52, 0x65, 0x71, 0x75, 0x65,
  0x73, 0x74, 0x28, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x2e, 0x6f, 0x6e, 0x72, 0x65,
  0x61, 0x64, 0x79, 0x73, 0x74, 0x61, 0x74, 0x65, 0x63, 0x68, 0x61, 0x6e,
  0x67, 0x65, 0x20, 0x3d, 0x20, 0x66, 0x75, 0x6e, 0x63, 0x74, 0x69, 0x6f,
  0x6e, 0x28, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x69, 0x66, 0x28, 0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70,
  0x2e, 0x72, 0x65, 0x61, 0x64, 0x79, 0x53, 0x74, 0x61, 0x74, 0x65, 0x20,
  0x3d, 0x3d, 0x20, 0x34, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x76, 0x61, 0x72, 0x20, 0x72, 0x65,
  0x73, 0x75, 0x6c, 0x74, 0x20, 0x3d, 0x20, 0x67, 0x65, 0x74, 0x4a, 0x73,
  0x6f, 0x6e, 0x52, 0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x28, 0x78,
  0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x2e, 0x73, 0x74, 0x61, 0x74, 0x75,
  0x73, 0x2c, 0x20, 0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x2e, 0x72,
  0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x54, 0x65, 0x78, 0x74, 0x29,
  0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x69, 0x66, 0x20, 0x28, 0x21, 0x72, 0x65, 0x73, 0x75, 0x6c, 0x74, 0x5b,
  0x22, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x22, 0x5d, 0x29, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x73,
  0x68, 0x6f, 0x77, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x28, 0x22,
  0x65, 0x72, 0x72, 0x6f, 0x72, 0x22, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x73, 0x68, 0x6f, 0x77, 0x53, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x28,
  0x22, 0x66, 0x69, 0x6e, 0x69, 0x73, 0x68, 0x65, 0x64, 0x22, 0x29, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70,
  0x2e, 0x6f, 0x70, 0x65, 0x6e, 0x28, 0x22, 0x50, 0x4f, 0x53, 0x54, 0x22,
  0x2c, 0x20, 0x75, 0x72, 0x6c, 0x2c, 0x20, 0x74, 0x72, 0x75, 0x65, 0x29,
  0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6d, 0x6c, 0x48,
  0x74, 0x74, 0x70, 0x2e, 0x73, 0x65, 0x74, 0x52, 0x65, 0x71, 0x75, 0x65,
  0x73, 0x74, 0x48, 0x65, 0x61, 0x64, 0x65, 0x72, 0x28, 0x22, 0x43, 0x6f,
  0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74, 0x79, 0x70, 0x65, 0x22, 0x2c,
  0x20, 0x22, 0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6f,
  0x6e, 0x2f, 0x6a, 0x73, 0x6f, 0x6e, 0x22, 0x29, 0x3b, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x2e,
  0x73, 0x65, 0x6e, 0x64, 0x28, 0x4a, 0x53, 0x4f, 0x4e, 0x2e, 0x73, 0x74,
  0x72, 0x69, 0x6e, 0x67, 0x69, 0x66, 0x79, 0x28, 0x72, 0x65, 0x73, 0x75,
  0x6c, 0x74, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x7d, 0x0a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x75, 0x6e, 0x63, 0x74,
  0x69, 0x6f, 0x6e, 0x20, 0x73, 0x68, 0x6f, 0x77, 0x50, 0x61, 0x73, 0x73,
  0x77, 0x6f, 0x72, 0x64, 0x28, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x76, 0x61, 0x72, 0x20, 0x63, 0x62, 0x20, 0x3d, 0x20,
  0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x67, 0x65, 0x74,
  0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28,
  0x22, 0x73, 0x68, 0x6f, 0x77, 0x22, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x76, 0x61, 0x72, 0x20, 0x70, 0x77, 0x20, 0x3d, 0x20,
  0x64, 0x6f, 0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74, 0x2e, 0x67, 0x65, 0x74,
  0x45, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28,
  0x22, 0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64, 0x22, 0x29, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x63,
  0x62, 0x2e, 0x63, 0x68, 0x65, 0x63, 0x6b, 0x65, 0x64, 0x29, 0x20, 0x7b,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x77, 0x2e,
  0x74, 0x79, 0x70, 0x65, 0x20, 0x3d, 0x20, 0x22, 0x74, 0x65, 0x78, 0x74,
  0x22, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x65, 0x6c, 0x73, 0x65, 0x20,
  0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70, 0x77,
  0x2e, 0x74, 0x79, 0x70, 0x65, 0x20, 0x3d, 0x20, 0x22, 0x70, 0x61, 0x73,
  0x73, 0x77, 0x6f, 0x72, 0x64, 0x22, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x7d, 0x0a, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x75, 0x6e, 0x63, 0x74,
  0x69, 0x6f, 0x6e, 0x20, 0x72, 0x65, 0x61, 0x64, 0x43, 0x6f, 0x6e, 0x66,
  0x69, 0x67, 0x28, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x76, 0x61, 0x72, 0x20, 0x75, 0x72, 0x6c, 0x20, 0x3d, 0x20, 0x22,
  0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x22, 0x20, 0x2b, 0x20, 0x6c,
  0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x68, 0x6f, 0x73, 0x74,
  0x6e, 0x61, 0x6d, 0x65, 0x20, 0x2b, 0x20, 0x28, 0x6c, 0x6f, 0x63, 0x61,
  0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x3f, 0x20,
  0x27, 0x3a, 0x27, 0x20, 0x2b, 0x20, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69,
  0x6f, 0x6e, 0x2e, 0x70, 0x6f, 0x72, 0x74, 0x20, 0x3a, 0x20, 0x27, 0x27,
  0x29, 0x20, 0x2b, 0x20, 0x22, 0x2f, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67,
  0x22, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x76, 0x61, 0x72,
  0x20, 0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x20, 0x3d, 0x20, 0x6e,
  0x65, 0x77, 0x20, 0x58, 0x4d, 0x4c, 0x48, 0x74, 0x74, 0x70, 0x52, 0x65,
  0x71, 0x75, 0x65, 0x73, 0x74, 0x28, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x2e, 0x6f,
  0x6e, 0x72, 0x65, 0x61, 0x64, 0x79, 0x73, 0x74, 0x61, 0x74, 0x65, 0x63,
  0x68, 0x61, 0x6e, 0x67, 0x65, 0x20, 0x3d, 0x20, 0x66, 0x75, 0x6e, 0x63,
  0x74, 0x69, 0x6f, 0x6e, 0x28, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x69, 0x66, 0x28, 0x78, 0x6d, 0x6c, 0x48,
  0x74, 0x74, 0x70, 0x2e, 0x72, 0x65, 0x61, 0x64, 0x79, 0x53, 0x74, 0x61,
  0x74, 0x65, 0x20, 0x3d, 0x3d, 0x20, 0x34, 0x29, 0x0a, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x6f, 0x6e, 0x43, 0x6f, 0x6e,
  0x66, 0x69, 0x67, 0x28, 0x67, 0x65, 0x74, 0x4a, 0x73, 0x6f, 0x6e, 0x52,
  0x65, 0x73, 0x70, 0x6f, 0x6e, 0x73, 0x65, 0x28, 0x78, 0x6d, 0x6c, 0x48,
  0x74, 0x74, 0x70, 0x2e, 0x73, 0x74, 0x61, 0x74, 0x75, 0x73, 0x2c, 0x20,
  0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x2e, 0x72, 0x65, 0x73, 0x70,
  0x6f, 0x6e, 0x73, 0x65, 0x54, 0x65, 0x78, 0x74, 0x29, 0x29, 0x3b, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x78, 0x6d, 0x6c, 0x48, 0x74, 0x74, 0x70, 0x2e,
  0x6f, 0x70, 0x65, 0x6e, 0x28, 0x22, 0x47, 0x45, 0x54, 0x22, 0x2c, 0x20,
  0x75, 0x72, 0x6c, 0x2c, 0x20, 0x74, 0x72, 0x75, 0x65, 0x29, 0x3b, 0x20,
  0x2f, 0x2f, 0x20, 0x74, 0x72, 0x75, 0x65, 0x20, 0x66, 0x6f, 0x72, 0x20,
  0x61, 0x73, 0x79, 0x6e, 0x63, 0x68, 0x72, 0x6f, 0x6e, 0x6f, 0x75, 0x73,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6d, 0x6c, 0x48, 0x74,
  0x74, 0x70, 0x2e, 0x73, 0x65, 0x6e, 0x64, 0x28, 0x6e, 0x75, 0x6c, 0x6c,
  0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20,
  0x20, 0x3c, 0x2f, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x3e, 0x0a, 0x3c,
  0x2f, 0x68, 0x65, 0x61, 0x64, 0x3e, 0x0a, 0x3c, 0x62, 0x6f, 0x64, 0x79,
  0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x64, 0x69, 0x76, 0x20, 0x63, 0x6c, 0x61,
  0x73, 0x73, 0x3d, 0x22, 0x68, 0x65, 0x61, 0x64, 0x65, 0x72, 0x22, 0x3e,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x68, 0x31, 0x3e, 0x57, 0x69, 0x46,
  0x69, 0x20, 0x53, 0x65, 0x74, 0x74, 0x69, 0x6e, 0x67, 0x73, 0x3c, 0x2f,
  0x68, 0x31, 0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x2f, 0x64, 0x69, 0x76, 0x3e,
  0x0a, 0x20, 0x20, 0x3c, 0x64, 0x69, 0x76, 0x20, 0x69, 0x64, 0x3d, 0x22,
  0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x22, 0x3e, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x4c, 0x6f, 0x61, 0x64, 0x69, 0x6e, 0x67, 0x20, 0x63, 0x75,
  0x72, 0x72, 0x65, 0x6e, 0x74, 0x20, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67,
  0x75, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x2e, 0x2e, 0x2e, 0x0a,
  0x20, 0x20, 0x3c, 0x2f, 0x64, 0x69, 0x76, 0x3e, 0x0a, 0x20, 0x20, 0x3c,
  0x64, 0x69, 0x76, 0x20, 0x69, 0x64, 0x3d, 0x22, 0x65, 0x72, 0x72, 0x6f,
  0x72, 0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x41, 0x6e, 0x20, 0x65,
  0x72, 0x72, 0x6f, 0x72, 0x20, 0x6f, 0x63, 0x63, 0x75, 0x72, 0x72, 0x65,
  0x64, 0x20, 0x77, 0x68, 0x69, 0x6c, 0x65, 0x20, 0x63, 0x6f, 0x6d, 0x6d,
  0x75, 0x6e, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6e, 0x67, 0x20, 0x77, 0x69,
  0x74, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20, 0x64, 0x65, 0x76, 0x69, 0x63,
  0x65, 0x2e, 0x20, 0x50, 0x6c, 0x65, 0x61, 0x73, 0x65, 0x20, 0x72, 0x65,
  0x66, 0x72, 0x65, 0x73, 0x68, 0x20, 0x74, 0x68, 0x65, 0x20, 0x70, 0x61,
  0x67, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x74, 0x72, 0x79, 0x20, 0x61, 0x67,
  0x61, 0x69, 0x6e, 0x2e, 0x0a, 0x20, 0x20, 0x3c, 0x2f, 0x64, 0x69, 0x76,
  0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x64, 0x69, 0x76, 0x20, 0x69, 0x64, 0x3d,
  0x22, 0x66, 0x69, 0x6e, 0x69, 0x73, 0x68, 0x65, 0x64, 0x22, 0x3e, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x54, 0x68, 0x65, 0x20, 0x63, 0x6f, 0x6e, 0x66,
  0x69, 0x67, 0x75, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x61,
  0x73, 0x20, 0x62, 0x65, 0x65, 0x6e, 0x20, 0x75, 0x70, 0x64, 0x61, 0x74,
  0x65, 0x64, 0x20, 0x2d, 0x20, 0x70, 0x6c, 0x65, 0x61, 0x73, 0x65, 0x20,
  0x72, 0x65, 0x73, 0x74, 0x61, 0x72, 0x74, 0x20, 0x74, 0x68, 0x65, 0x20,
  0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x61, 0x70,
  0x70, 0x6c, 0x79, 0x20, 0x63, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x73, 0x2e,
  0x0a, 0x20, 0x20, 0x3c, 0x2f, 0x64, 0x69, 0x76, 0x3e, 0x0a, 0x20, 0x20,
  0x3c, 0x64, 0x69, 0x76, 0x20, 0x69, 0x64, 0x3d, 0x22, 0x63, 0x6f, 0x6e,
  0x66, 0x69, 0x67, 0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x70,
  0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x45, 0x6e, 0x74, 0x65,
  0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x53, 0x53, 0x49, 0x44, 0x20, 0x61,
  0x6e, 0x64, 0x20, 0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64, 0x20,
  0x66, 0x6f, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x6e, 0x65, 0x74, 0x77,
  0x6f, 0x72, 0x6b, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x77, 0x69, 0x73, 0x68,
  0x20, 0x74, 0x68, 0x69, 0x73, 0x20, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65,
  0x20, 0x74, 0x6f, 0x20, 0x6a, 0x6f, 0x69, 0x6e, 0x2e, 0x20, 0x43, 0x6c,
  0x69, 0x63, 0x6b, 0x20, 0x3c, 0x69, 0x3e, 0x53, 0x61, 0x76, 0x65, 0x3c,
  0x2f, 0x69, 0x3e, 0x20, 0x74, 0x6f, 0x20, 0x73, 0x74, 0x6f, 0x72, 0x65,
  0x20, 0x74, 0x68, 0x65, 0x20, 0x6e, 0x65, 0x77, 0x20, 0x73, 0x65, 0x74,
  0x74, 0x69, 0x6e, 0x67, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x72, 0x65,
  0x73, 0x74, 0x61, 0x72, 0x74, 0x20, 0x74, 0x68, 0x65, 0x20, 0x73, 0x65,
  0x6e, 0x73, 0x6f, 0x72, 0x2e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x2f,
  0x70, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x64, 0x69, 0x76, 0x20,
  0x63, 0x6c, 0x61, 0x73, 0x73, 0x3d, 0x22, 0x66, 0x69, 0x65, 0x6c, 0x64,
  0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x6c, 0x61,
  0x62, 0x65, 0x6c, 0x20, 0x66, 0x6f, 0x72, 0x3d, 0x22, 0x73, 0x73, 0x69,
  0x64, 0x22, 0x3e, 0x53, 0x53, 0x49, 0x44, 0x3c, 0x2f, 0x6c, 0x61, 0x62,
  0x65, 0x6c, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x69,
  0x6e, 0x70, 0x75, 0x74, 0x20, 0x69, 0x64, 0x3d, 0x22, 0x73, 0x73, 0x69,
  0x64, 0x22, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3d, 0x22, 0x74, 0x65, 0x78,
  0x74, 0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x2f, 0x64, 0x69,
  0x76, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x64, 0x69, 0x76, 0x20,
  0x63, 0x6c, 0x61, 0x73, 0x73, 0x3d, 0x22, 0x66, 0x69, 0x65, 0x6c, 0x64,
  0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x6c, 0x61,
  0x62, 0x65, 0x6c, 0x20, 0x66, 0x6f, 0x72, 0x3d, 0x22, 0x70, 0x61, 0x73,
  0x73, 0x77, 0x6f, 0x72, 0x64, 0x22, 0x3e, 0x50, 0x61, 0x73, 0x73, 0x77,
  0x6f, 0x72, 0x64, 0x3c, 0x2f, 0x6c, 0x61, 0x62, 0x65, 0x6c, 0x3e, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x69, 0x6e, 0x70, 0x75, 0x74,
  0x20, 0x69, 0x64, 0x3d, 0x22, 0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72,
  0x64, 0x22, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3d, 0x22, 0x70, 0x61, 0x73,
  0x73, 0x77, 0x6f, 0x72, 0x64, 0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x3c, 0x64, 0x69, 0x76, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73,
  0x3d, 0x22, 0x72, 0x69, 0x67, 0x68, 0x74, 0x22, 0x3e, 0x0a, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x69, 0x6e, 0x70, 0x75, 0x74,
  0x20, 0x69, 0x64, 0x3d, 0x22, 0x73, 0x68, 0x6f, 0x77, 0x22, 0x20, 0x74,
  0x79, 0x70, 0x65, 0x3d, 0x22, 0x63, 0x68, 0x65, 0x63, 0x6b, 0x62, 0x6f,
  0x78, 0x22, 0x20, 0x6f, 0x6e, 0x63, 0x6c, 0x69, 0x63, 0x6b, 0x3d, 0x22,
  0x73, 0x68, 0x6f, 0x77, 0x50, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64,
  0x28, 0x29, 0x22, 0x3e, 0x53, 0x68, 0x6f, 0x77, 0x20, 0x70, 0x61, 0x73,
  0x73, 0x77, 0x6f, 0x72, 0x64, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x3c, 0x2f, 0x64, 0x69, 0x76, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c,
  0x2f, 0x64, 0x69, 0x76, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x64,
  0x69, 0x76, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x3d, 0x22, 0x66, 0x6f,
  0x6f, 0x74, 0x65, 0x72, 0x20, 0x72, 0x69, 0x67, 0x68, 0x74, 0x22, 0x3e,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x62, 0x75, 0x74, 0x74,
  0x6f, 0x6e, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3d, 0x22, 0x62, 0x75, 0x74,
  0x74, 0x6f, 0x6e, 0x22, 0x20, 0x6f, 0x6e, 0x63, 0x6c, 0x69, 0x63, 0x6b,
  0x3d, 0x22, 0x73, 0x61, 0x76, 0x65, 0x43, 0x68, 0x61, 0x6e, 0x67, 0x65,
  0x73, 0x28, 0x29, 0x22, 0x3e, 0x53, 0x61, 0x76, 0x65, 0x3c, 0x2f, 0x62,
  0x75, 0x74, 0x74, 0x6f, 0x6e, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x3c,
  0x2f, 0x64, 0x69, 0x76, 0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x2f, 0x64, 0x69,
  0x76, 0x3e, 0x0a, 0x20, 0x20, 0x3c, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74,
  0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x28, 0x66, 0x75, 0x6e, 0x63, 0x74,
  0x69, 0x6f, 0x6e, 0x28, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x72, 0x65, 0x61, 0x64, 0x43, 0x6f, 0x6e, 0x66, 0x69, 0x67,
  0x28, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x29, 0x28, 0x29,
  0x3b, 0x0a, 0x20, 0x20, 0x3c, 0x2f, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74,
  0x3e, 0x0a, 0x3c, 0x2f, 0x62, 0x6f, 0x64, 0x79, 0x3e, 0x0a, 0x3c, 0x2f,
  0x68, 0x74, 0x6d, 0x6c, 0x3e, 0x0a, 0x00
  };

//---------------------------------------------------------------------------
// Helper functions
//---------------------------------------------------------------------------

static String chooseUniqueName() {
  String result;
  if(strlen(Config.m_szNode)==0) {
    // Use the ESP8266 chip ID
    result += "IoThing_";
    result += ESP.getChipId();
    }
  else {
    // Use the node ID
    result += "IOT";
    result += Config.m_szNode;
    }
  return result;
  }

/** Attempt to connect to the access point
 *
 * @param cszSSID the SSID of the AP to connect to
 * @param cszPassword the password to use for the connection
 *
 * @return true if the connection was successful
 */
static bool wifiConnect(const char *cszSSID, const char *cszPassword) {
  // Make sure we have a SSID
  if(strlen(cszSSID)==0)
    return false;
  // Clear current settings
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Try and establish a connection
  int attempts = 3;
  while (attempts) {
    int status;
    if(strlen(cszPassword)==0)
      status = WiFi.begin(cszSSID);
    else
      status = WiFi.begin(cszSSID, cszPassword);
    if(status==WL_CONNECTED)
      return true;
    // Wait 10 seconds for the next attempt
    if(--attempts) // Wait before next attempt
      delay(10000);
    }
  // Failed
  return false;
  }

/** Enter access point mode for system configuration
 */
static void wifiAccessPoint() {
  static char szSSID[12];
  sprintf(szSSID, "IoThing %02d", ESP.getChipId() % 100);
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  // Make sure the requested SSID is not being used
  bool exists = true;
  while(exists) {
    int n = WiFi.scanNetworks();
    exists = false;
    for (int i=0; i<n; i++) {
      String ssid = WiFi.SSID(i);
      if(strcmp(szSSID, ssid.c_str())==0)
        exists = true;
      }
    if(exists) {
      DMSG("AP '%s' is already in use, waiting.", szSSID);
      delay(5000); // Wait before scanning again
      }
    }
  // Set up the open AP with the given SSID
  DMSG("Setting up AP with name '%s'", szSSID);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(szSSID);
  delay(100);
  }

/** Update a single field from the JSON buffer
 */
static bool updateField(JsonParser &parser, const char *cszName, char *buffer, int size) {
  int field = parser.find(0, cszName);
  if((field>0)&&(parser.str(field)!=NULL)) {
    if(parser.len(field)>=size)
      return false;
    // Update the value
    memset(buffer, 0, size);
    strncpy(buffer, parser.str(field), parser.len(field));
    }
  return true;
  }

//---------------------------------------------------------------------------
// Web server
//---------------------------------------------------------------------------

ESP8266WebServer httpServer(80);
DNSServer dnsServer;
WIFI_CONFIG Config;

void handleNotFound() {
  httpServer.send(404, "text/plain", "Resource not found.");
  }

void handleConfig() {
  bool status = true;
  if(httpServer.method() == HTTP_POST) {
    // Process the configuration
    if (httpServer.hasArg("plain")) {
      JsonToken tokens[16];
      JsonParser parser(tokens, 16);
      String json = httpServer.arg("plain");
      int count = parser.parse(json.c_str());
      if(count > 0) {
        // Extract the settings
        status |= updateField(parser, "ssid", Config.m_szSSID, MAX_SSID_LENGTH);
        status |= updateField(parser, "password", Config.m_szPass, MAX_PASSWORD_LENGTH);
        status |= updateField(parser, "node", Config.m_szNode, MAX_NODEID_LENGTH);
        status |= updateField(parser, "mqtt", Config.m_szMqtt, MAX_SERVER_NAME_LENGTH);
        status |= updateField(parser, "topic", Config.m_szTopic, MAX_TOPIC_NAME_LENGTH);
        // Recalculate the CRC
        Crc16 crc;
        Config.m_crc16 = crc.XModemCrc((uint8_t *)&Config, 0, sizeof(WIFI_CONFIG) - sizeof(uint16_t));
        // Save to EEPROM
        if(status) {
          EEPROM.put(IotConfig.getEepromOffset(), Config);
          EEPROM.commit();
          }
        }
      else
        status = false;
      }
    }
  // Build the response with the current values
  JsonBuilder builder;
  builder.add("ssid", Config.m_szSSID);
  builder.add("node", Config.m_szNode);
  builder.add("mqtt", Config.m_szMqtt);
  builder.add("topic", Config.m_szTopic);
  if (httpServer.method() == HTTP_POST) {
    builder.add("status", status);
    if(status)
      IotConfig.onConfigChange();
    }
  builder.end();
  httpServer.send(200, "application/json", builder.getResult());
  }

void handleDefault() {
  httpServer.send_P(200, PSTR("text/html"), CONFIG_PAGE);
  }

void webServer(bool withForm) {
  httpServer.on("/config", handleConfig);
  if(withForm) {
    httpServer.onNotFound(handleDefault);
    /* Setup the DNS server redirecting all the domains to the apIP */
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(53, "*", apIP);
    }
  else
    httpServer.onNotFound(handleNotFound);
  httpServer.begin();
  }

//---------------------------------------------------------------------------
// Public API
//---------------------------------------------------------------------------

/** Default constructor
 */
IotConfigClass::IotConfigClass() {
  m_state = StateIdle;
  m_pfnCallback = NULL;
  m_pfnUpdate = NULL;
  m_eepromOffset = 0;
  }

/** Set up the library
 *
 * @param force if true the library will go into wifi configuration mode
 *              regardless of the current settings.
 */
bool IotConfigClass::setup(bool force, int eepromOffset) {
  m_eepromOffset = eepromOffset;
  // Load and verify the stored configuration
  EEPROM.get(m_eepromOffset, Config);
  Crc16 crc;
  uint16_t expected = crc.XModemCrc((uint8_t *)&Config, 0, sizeof(WIFI_CONFIG) - sizeof(uint16_t));
  if(expected!=Config.m_crc16) {
    memset(&Config, 0, sizeof(WIFI_CONFIG));
    force = true;
    }
  // If we are not forcing config mode try and connect
  if(!force) {
    onStateChange(StateConnecting);
    force = !wifiConnect(
      Config.m_szSSID,
      Config.m_szPass
      );
    }
  // Do we need to enter system mode ?
  if(force) {
    onStateChange(StateSystemConfig);
    wifiAccessPoint();
    webServer(true);
    }
  else {
    onStateChange(StateConnected);
    webServer(false);
    }
  // Set up the MDNS server
  String name = chooseUniqueName();
  MDNS.begin(name.c_str());
  MDNS.addService("iothing", "tcp", 80);
  // Let the caller know what mode we are in
  return force;
  }

/** Main loop
 *
 * This method must be called from the applications main processing loop
 * to handle incoming configuration changes.
 */
void IotConfigClass::loop() {
  httpServer.handleClient();
  dnsServer.processNextRequest();
  MDNS.update();
  }

// The singleton instance
IotConfigClass IotConfig = IotConfigClass();
