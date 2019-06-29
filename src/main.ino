/* RMS slave unloads any async behaviour from RMS master
which has a 104us capture loop which cant be interrupted

RMS slave handles NTP, FTP and prometheus metrics scrapes */

#include "main.h"

void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Serial.println("RMS slave  03 Oct 2018");
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("local IP address: ");
  localIP=WiFi.localIP();
  Serial.print(localIP);
  long rssi = WiFi.RSSI();
  Serial.print("   signal strength: ");
  Serial.print(rssi);
  Serial.println(" dBm"); 
  // SaveCrash.print();
  init_OTA();

  udp.begin(localPort);
  // Resolve servers
  WiFi.hostByName(ntpServerName, timeServerIP);
  // Set epoch and timers
  getTime();
  setTime(startSeconds);
  setupSPIslave();      // with startSeconds in status register
  SPISlave.end();

  Serial.println(timeStamp());
  startMillis = millis();
  oldMin = minute();
  oldQtr = oldMin/15;
  oldHour = hour();
  oldDay = day();
  oldMonth = month();
  oldYear = year();

  //if(!SPIFFS.format()||!SPIFFS.begin())     //use to format SPIFFS drive
  if(!SPIFFS.begin())
  {
    Serial.println("SPIFFS.begin failed");
  }
  SPIFFS.info(fs_info);
  /* Serial.print(fs_info.totalBytes);
  Serial.println(" bytes available");
  Serial.print(fs_info.usedBytes);
  Serial.println(" bytes used:"); */

  fd=SPIFFS.open("/diags.txt","a");
  fe=SPIFFS.open("/errmess.txt","a");

  resetReason.toCharArray(charBuf,resetReason.length()+1);
  Serial.println(charBuf);
	diagMess(charBuf);       // restart message

  server.on ( "/", handleMetrics );
  server.on ( "/metrics", handleMetrics );
  server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println( "HTTP server started" );

  secondTick.attach(1,ISRwatchDog);
}

void loop() { 
  // wait for data from master
  waitForData();
  //  check for change of the quarter hour
  if ( minute()/15 != oldQtr ) qtrProc();
  //  check for async activity
  watchWait(1700); 
  // check for network
  if (WiFi.status() != WL_CONNECTED) {
    diagMess("disconnected from network");
    fd.close();
    fe.close(); 
    ESP.restart();
  } 
}
