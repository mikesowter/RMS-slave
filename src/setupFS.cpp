#include <LittleFS.h>
#include <TimeLib.h>

extern FSInfo fs_info;
extern File fd,fe,fh;

time_t myTimeCallback() {
    return now()-10*3600; // UNIX timestamp for GMT
}

void setupFS () {
  
//  if (!LittleFS.format()) Serial.println("LittleFS.format failed");
//  else Serial.println("LittleFS formatted");

  if ( !LittleFS.begin() ) {
      Serial.println("LittleFS.begin failed");
  }
 
  LittleFS.info(fs_info);
  Serial.print(fs_info.totalBytes);
  Serial.println(" bytes available");
  Serial.print(fs_info.usedBytes);
  Serial.println(" bytes used");
/*  Serial.print(fs_info.blockSize);
  Serial.println(" blocksize");
  Serial.print(fs_info.pageSize);
  Serial.println(" pagesize\n");  */

  LittleFS.setTimeCallback(myTimeCallback);
}
