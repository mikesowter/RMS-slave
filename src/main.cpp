/* RMS slave unloads any async behaviour from RMS master
which has a 104us capture loop which cant be interrupted

RMS slave handles NTP, FTP and prometheus metrics scrapes */

#include <main.h>

void setup(void) {
  Serial.begin(115200);
  Serial.println();
  Serial.println("RMS slave 20190905");
  // Join Network
  joinNet();
  // Resolve server
  WiFi.hostByName(ntpServerName, timeServerIP);
  // start OTA
  init_OTA();
  // setup FTP server
	ftpSrv.begin("mike","iron");
  // Set epoch and timers
  udp.begin(localPort);
  setTime(getTime());
  setupSPIslave();      // with startSeconds in status register
  SPISlave.end();
  Serial.println(timeStamp());
  oldMin = minute();
  oldQtr = oldMin/15;
  oldHour = hour();
  oldDay = day();
  oldMonth = month();
  oldYear = year();
  // generate new file name for day
  strcpy(todayName,"/rm");
  strcat(todayName,dateStamp());
  strcat(todayName,".csv");
  //if(!SPIFFS.format()) Serial.println("SPIFFS.format failed");
  if(!SPIFFS.begin()) Serial.println("SPIFFS.begin failed");

  SPIFFS.info(fs_info);
  Serial.print(fs_info.totalBytes);
  Serial.println(" bytes available");
  Serial.print(fs_info.usedBytes);
  Serial.println(" bytes used:");

  fd = SPIFFS.open("/diags.txt","a");
  fe = SPIFFS.open("/errmess.txt","a");

  resetReason.toCharArray(charBuf,resetReason.length()+1);
	diagMess(charBuf);       // restart message
	resetDetail.toCharArray(charBuf,resetDetail.length()+1);
	if ( charBuf[16] != '0' )	{				// if fatal exception
		diagMess(charBuf); 
	}

  server.on ( "/", handleRoot );
  server.on ( "/dir", handleDir );
  server.on ( "/metrics", handleMetrics );
  server.on ( "/hotwater", handleWater );
  server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println( "HTTP server started" );

  secondTick.attach(1,ISRwatchDog);
}

void loop() { 
  // wait for data from master
  waitForData();
  //  check for change of minute
  if ( minute() != oldMin ) minProc();
  //  check for async activity
  watchWait(2300); 
  // check for network
  if (WiFi.status() != WL_CONNECTED) {
    diagMess("disconnected from network");
    joinNet();
  } 
  watchDog = 0;
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
  // Resolve servers
  WiFi.hostByName(ntpServerName, timeServerIP);
}
