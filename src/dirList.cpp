#include "extern.h"

// add a web page with a listing of the LittleFS "/" folder

void handleDir() {
  char fileSizeStr[10];
  longStr[0]='\0';
  LittleFS.info(fs_info);
  ltoa(fs_info.usedBytes,fileSizeStr,10);
  strcat(longStr,ltoa(fs_info.usedBytes,fileSizeStr,10));
	strcat(longStr," bytes used:\n");
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    dir.fileName().toCharArray(fileName, 20);
    strcat(longStr,"\n");
    strcat(longStr,fileName);
    strcat(longStr,"\t");
    itoa(dir.fileSize(),fileSizeStr,10);
    strcat(longStr,fileSizeStr);
  }
  server.send ( 200, "text/plain", longStr );
  activity("dir:");
  //Serial.println(longStr);
}

void helpPage() {
  longStr[0]='\0';
  strcat(longStr,"<!DOCTYPE html><html><body><HR>");
  strcat(longStr,"Valid options include:");
  strcat(longStr,"<P>");
  strcat(longStr,"filename");
  strcat(longStr,"<P>");
  strcat(longStr,"deldiags");
  strcat(longStr,"<P>");
  strcat(longStr,"delerrs");
  strcat(longStr,"<P>");
  strcat(longStr,"diags");
  strcat(longStr,"<P>");
  strcat(longStr,"dir");
  strcat(longStr,"<P>");
  strcat(longStr,"goback-hh");
  strcat(longStr,"<P>");
  strcat(longStr,"metrics");
  strcat(longStr,"<P>");
  strcat(longStr,"reset");
  strcat(longStr,"<P>");
  strcat(longStr,"shutdown");
  strcat(longStr,"<HR>");
  strcat(longStr,"spareSol");
  strcat(longStr,"<P>");
  strcat(longStr, "<HR></body></html>" );
  server.send ( 200, "text/html", longStr );
  //Serial.println(longStr);
}
