#include "C:\Users\Mikes 6700K\Dropbox\sketchbook\RMS slave\src\extern.h"

// add a web page with a listing of the SPIFFS "/" folder

void dirList() {
  char fileSize[]="999999";
  htmlStr[0]='\0';
  addCstring("<!DOCTYPE html><html><body><HR>");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    dir.fileName().toCharArray(fileName, 14);
    addCstring("<P>");
    addCstring(fileName);
    addCstring("&emsp;");
    itoa(dir.fileSize(),fileSize,7);
    addCstring(fileSize);
  }
  addCstring( "<HR></body></html>" );
  server.send ( 200, "text/html", htmlStr );
  //Serial.println(htmlStr);
}

uint8_t listDiags() {
  char line[66];
  int k;
  htmlStr[0]='\0';
  addCstring("<!DOCTYPE html><html><body><HR>");
  fd.close();
  fd = SPIFFS.open("/diags.txt", "r");

  while (fd.available()) {
    k=fd.readBytesUntil('\r',line,64);
    line[k]='\0';
    addCstring(line);
    addCstring("<P>");
  }
  addCstring( "<HR></body></html>" );
  //Serial.println(htmlStr);
  server.send ( 200, "text/html", htmlStr );
  fd.close();
  return 1;
}

void helpPage() {
  htmlStr[0]='\0';
  addCstring("<!DOCTYPE html><html><body><HR>");
  addCstring("Valid options include:");
  addCstring("<P>");
  addCstring("8.3 filename");
  addCstring("<P>");
  addCstring("day");
  addCstring("<P>");
  addCstring("deldiags");
  addCstring("<P>");
  addCstring("delerrs");
  addCstring("<P>");
  addCstring("diags");
  addCstring("<P>");
  addCstring("dir");
  addCstring("<P>");
  addCstring("metrics");
  addCstring("<P>");
  addCstring("reset");
  addCstring("<P>");
  addCstring("shutdown");
  addCstring("<P>");
  addCstring("week");
  addCstring("<HR>");
  addCstring( "<HR></body></html>" );
  server.send ( 200, "text/html", htmlStr );
  //Serial.println(htmlStr);
}
