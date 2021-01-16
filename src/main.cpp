 /* RMS slave unloads any async behaviour from RMS master
which has a 104us capture loop which cant be interrupted

RMS slave handles NTP, FTP and prometheus metrics scrapes */

#include <main.h>

void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Serial.println("RMS slave 20210112");
  // Join Network
  joinNet();
  // start OTA
  init_OTA();
  // setup FTP server
	ftpSrv.begin("mike","iron");
  // set time related
  //setTime(23,59,30,30,11,2016);
  setupTime();
  //if(!LittleFS.format()) Serial.println("LittleFS.format failed");
  if(!LittleFS.begin()) Serial.println("LittleFS.begin failed");
  // open diagnostic files
  fd = LittleFS.open("/diags.txt","a");
  fe = LittleFS.open("/errmess.txt","a");
  // explain restart
  resetReason.toCharArray(charBuf,resetReason.length()+1);
	diagMess(charBuf);       
	resetDetail.toCharArray(charBuf,resetDetail.length()+1);
  diagMess(charBuf); 
	//if ( charBuf[16] != '0' ) diagMess(charBuf); 				// if fatal exception

  // recover previous values from prometheus
  getLastScan();
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
  // wait for data from master
  waitForData();
  //  check for change of minute
  if ( minute() != oldMin ) minProc();
  //  check for async activity
  watchWait(2400); 
  // check the network
  checkScan();
  // feed the dog
  watchDog = 0;
}

void checkScan() {
  if ( millis() - lastScan > 90000UL ) {
    diagMess("no scan for 90s");
    lastScan = millis();
    // rejoin local network if necessary
	  if ( WiFi.status() != WL_CONNECTED ) joinNet();
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
  strcpy(todayName,"/rm");
  strcat(todayName,dateStamp());
  strcat(todayName,".csv");
}

