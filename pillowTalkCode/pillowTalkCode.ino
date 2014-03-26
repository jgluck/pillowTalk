#include <SPI.h> //sdcard lib
#include <SdFat.h> //sdcard fs
#include <SFEMP3Shield.h> //mp3decoder


const int zipPin = A0;
int zipVal = 0; //zipperValue


const int EN_GPIO1 = A2; //enable amplifier + midi mp3 mode select
const int SD_CS = 9; //chip seleect for sd card? 

SFEMP3Shield MP3player; //mp3player object
SdFat sd; // sd card object

boolean deb = false; //becasue this will be in the pillow, best not to 
                      //waste time
boolean switchDo = false ;
                   
//              
//char filename[13];
//char filename2[13];
//char filename3[13];
//char filename4[13];
//char filename5[15];

char filename[10][13];
int index = 0;
int lastIndex = 0;
int numTracks = 0;
boolean wasJustOn = false;
boolean shouldPlay = true;

boolean firstLoop = true;
boolean directionUp =false ;

int vol = 255;

void setup(){
  int x, index; //figure these out later
  SdFile file; //file object
  byte result; //TODO
  char tempfilename[13]; //for opening
  pinMode(zipPin,INPUT); //zipper pin
  digitalWrite(zipPin,LOW); //maybe get rid
  
  pinMode(EN_GPIO1,OUTPUT); //mode pin for playing
  digitalWrite(EN_GPIO1,LOW); //MP3 mode /amp off
  
//  firstLoop = true;
//  directionUp = false;
  
  if(deb){ //turn on serial if debugging
    Serial.begin(9600);
    Serial.println(F("PillO time debugging enabled"));
  }
  
  result = sd.begin(SD_CS, SPI_HALF_SPEED); //1 for success
  
  result = MP3player.begin(); //0 or 6 for success
  
  debP("Result of creating mp3Player is: " + String(result));
  
   sd.chdir("/",true);
   while(file.openNext(sd.vwd(),O_READ)){
     //get filename
     file.getFilename(tempfilename);
     debP("The filename was: "+ String(tempfilename[0]));
     if(tempfilename[0] >= '1' && tempfilename[0] <= '9'){ 
       index = tempfilename[0] - '1';
       numTracks = numTracks + 1;
       
       strcpy(filename[index],tempfilename);
     }
       
//     if(tempfilename[0] == '1'){
//       debP("found filename beginning with 1: " + String(tempfilename));
//       strcpy(filename,tempfilename);
//     }else if(tempfilename[0] == '2'){
//       debP("found filename beginning with 2: " + String(tempfilename));
//       strcpy(filename2,tempfilename);
//     }else if(tempfilename[0] == '3'){
//       debP("found filename beginning with 3: " + String(tempfilename));
//       strcpy(filename3, tempfilename);
//     }else if(tempfilename[0] == '4'){
//       strcpy(filename4, tempfilename);
//     }else if(tempfilename[0] == '5'){
//       strcpy(filename5, tempfilename);
//     }
   
   file.close();
  } //end while loop
  
  MP3player.setVolume(10,10); //volume high
  digitalWrite(EN_GPIO1,HIGH);
  index = 0;
  delay(20);
}
  
void loop(){
  byte result;
 

  if(firstLoop){
    debP("In first loop");
    stopNoise();
    firstLoop = false;
  }
//  if(firstLoop){
//    debP("FirstLoop!!!");
//    startNoise();
//    changeVolume(10);
//    firstLoop = false;
//  }else 

//    startNoise();

  if(shouldPlay){
    startNoise();
    shouldPlay = false;
  }

//  if(!MP3player.isPlaying()){
//    debP("Player not playing gonna play");
//    startNoise();
////    changeVolume(10);
//  }
  
  readZip();
  changeVolume(vol);
  debP("Changing volume to: "+ String(vol));
    

}

void startNoise(){
  byte result;
//  index = 0;
  result = MP3player.playMP3(filename[index]);
  debP("Result of play is: " + String(result));
//  delay(50);
}

void stopNoise(){
  byte result;
  MP3player.stopTrack();
}

void changeVolume(int x){
  MP3player.setVolume(x,x);
}

void readZip(){
 int zipValue = -1;
 zipValue = analogRead(zipPin);
 if(zipValue < 900){
   zipValue = 900;
 }else if(zipValue > 1000){
   zipValue = 1000;
 }
 
 int preVal = (zipValue - 1000)*-1;
 int val = 10;
 
 if(preVal <= 10){
   
    val = 255;
   
   if(wasJustOn){
     wasJustOn = false;
     if(switchDo){
       stopNoise();
       index = (index + 1);
       if(index >= numTracks){
         index = index - numTracks;
       }}}    

 }else{
   wasJustOn = true;
   if(!MP3player.isPlaying()){
     shouldPlay = true;
   }
   val = map(zipValue, 900, 1000, 0, 50);
 }
 vol = val;
 debP("Setting volume to: "+ String(vol));
 
// vol = ((zipValue - 1000) * 2)+ 200;
 
}
  
void debP(String x){
 if(deb){
   Serial.println(x);
 } 
}

