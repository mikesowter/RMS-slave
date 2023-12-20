 /* RMS slave offloads any async behaviour from RMS master
which has a 104us capture loop which can't be interrupted */

#include <main.h>

void setup(void) {
  Serial.begin(115200);
  Serial.println(CODEVERSION);
  digitalWrite(MASTER_RESET,1);
  pinMode(MASTER_RESET, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  // Join Network
  joinNet();
  // start OTA
  init_OTA();
  delay(500);
  // setup FTP server
	ftpSrv.begin("mike","iron");
  // set time related
  setupTime();
  // setup file system
  setupFS();
  // explain restart
	resetDetail.toCharArray(charBuf,resetDetail.length()+1);
  diagMess(charBuf); 				// only give detail
  #ifdef RMS1
  // recover previous values from prometheus
  readPromDB();   
  #endif
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
    Serial.printf(" missed cycle: %u ",wfdTime);
  }
}

void checkScan() {
  if ( millis() - lastScan > 60000UL ) {
    if (!scanFail) {
      diagMess("no scan for 60s");
      scanFail = true;
    }
    // rejoin local network if necessary
	  if ( WiFi.status() != WL_CONNECTED ) joinNet();
  }
  else {
    if (scanFail) {
      scanFail = false;
      diagMess("scan restored");
    }
  }
}

void joinNet() {
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) delay(100);
 
  Serial.println("\nlocal IP address: ");
  localIP = WiFi.localIP();
  Serial.print(localIP);
  long rssi = WiFi.RSSI();
  Serial.print("   signal strength: ");
  Serial.print(rssi);
  Serial.println(" dBm");

  udp.begin(localPort);
}
