 /* RMS slave offloads any async behaviour from RMS master
which has a 104us capture loop which can't be interrupted
 */

#include <main.h>


void setup(void) {
  Serial.begin(115200);
  #ifdef RMS2
    Serial.println("\nRMS2 slave 20231004");
  #else
    Serial.println("\nRMS16 slave 20230901");
  #endif
  digitalWrite(MASTER_RESET,1);
  pinMode(MASTER_RESET, OUTPUT);
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
  #ifndef RMS2
  // recover previous values from prometheus
  getLastScan();   
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
  //  check for change of minute
  if ( minute() != oldMin ) minProc();
  // check the network
  checkScan();
  // synchronise with master
  sync2Master();
}

void sync2Master() {
  missedCycle += (uint16_t) (wfdTime/1000);  
  syncDelay = 1000 - wfdTime%1000;
  syncDelay += (wfdTime%1000 - 250) /10;
  syncDelay = syncDelay>800?750:syncDelay;
  Serial.printf("%s sync: %i, wfd: %i\n",timeStamp(),syncDelay,wfdTime);
  watchWait(syncDelay);
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

void setupTime() {
  // Set epoch and timers
  setTime(getTime());
  setupSPIslave();      // tell Master the time
  SPISlave.end();
  oldMin = minute();
  oldQtr = oldMin/15;
  oldHour = hour();
  oldDay = day();
  oldMonth = month();
  oldYear = year();
  Serial.printf("\nDate:%s Time:%s\n",dateStamp(),timeStamp());
  // generate file name for day
  strcpy(todayName,"/RM");
  strcat(todayName,dateStamp());
  strcat(todayName,".csv");
}

