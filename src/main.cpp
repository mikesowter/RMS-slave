 /* RMS slave offloads any async behaviour from RMS masters 1&2
which have a fixed duty loop which can't be interrupted */

#include <main.h>

void setup(void) {
  Serial.begin(115200);
  Serial.println(CODEVERSION);
  pinMode(MASTER_RESET, OUTPUT);
  digitalWrite(MASTER_RESET,1);
  pinMode(LED_PIN, OUTPUT);
  // Join Network
  joinNet();
  // start OTA
  init_OTA();
  // setup FTP server
	ftpSrv.begin("mike","iron");
  // set time related
  setupTime();
  // setup file system
  setupFS();
  // explain restart
	resetDetail.toCharArray(charBuf,resetDetail.length()+1);
  diagMess(charBuf); 				// only give detail
  // recover previous values from prometheus
  readPromDB();  
//  fillBarrel();          // only required for demand analysis
  // check for peak
  peakPeriod = hour() >= 16 && hour() < 20;
  // setup server
  server.on ( "/", handleRoot );
  server.on ( "/dir", handleDir );
  server.on ( "/metrics", handleMetrics );
  server.on ( "/hotwater", handleWater );
  server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println( "HTTP server started" );
  // start watchdog
  secondTick.attach(1,ISRwatchDog);
}

void loop() { 
  loopStart = millis();
  // wait for data from master
  waitForData();
  //  check the time
  minProc();
  // check the network
  checkScan();
  // synchronise with master
  sync2Master();
}

void sync2Master() {
  if ( wfdTime > 1500U ) {       // no cycle missed under 1500ms
    missedCycle++;
    Serial.printf(" missed cycle: %lu ",wfdTime);
  }
}

void checkScan() {
  if ( millis() - lastScan > 30000UL ) {
    if (!scanFail) {
      startOutage = lastScan;
      scanFail = true;
    }
    // rejoin local network if necessary
	  if ( WiFi.status() != WL_CONNECTED ) joinNet();
    if ( WiFi.status() != WL_CONNECTED ) ESP.restart();
  }
  else {
    if (scanFail) {
      scanFail = false;
      sprintf(charBuf,"scan outage of %lus",(millis()-startOutage)/1000UL);
      diagMess(charBuf);
    }
  }
}

void joinNet() {
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, pass);
  t0 = millis() + 10000;

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    if ( millis() > t0 ) break;
  }

#ifdef RMS2
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("\n\nConnecting to ");
    Serial.println(bkp_ssid);
    WiFi.begin(bkp_ssid, bkp_pass);
    t0 = millis() + 10000;

    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      if ( millis() > t0 ) break;
    }
  }
#endif

  if (WiFi.status() != WL_CONNECTED) {
    diagMess("failed to connect to either network");
    return;
  }
 
  Serial.println("\nlocal IP address: ");
  localIP = WiFi.localIP();
  Serial.print(localIP);
  long rssi = WiFi.RSSI();
  Serial.print("   signal strength: ");
  Serial.print(rssi);
  Serial.println(" dBm");
  udp.begin(localPort);
}
