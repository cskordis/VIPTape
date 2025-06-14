// the regular Adafruit "TouchScreen.h" library only works on AVRs
// Main Code by Josh Bensadon, with the help of some Libraries. :)
// different mcufriend shields have Touchscreen on different pins
// and rotation.
// Run the TouchScreen_Calibr_native sketch for calibration of your shield

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;       // hard-wired for UNO shields anyway.
#include <TouchScreen.h>

//char *name = "VIP TAPE v1.Wa";  

//The 1.Wa denotes changes by me :) the following are the pins used for the screen and resistive touch calibration

const int XP=6,XM=A2,YP=A1,YM=7; //ID=0x9341
const int TS_LEFT=921,TS_RT=169,TS_TOP=163,TS_BOT=956;

//const int XP=6,XM=A2,YP=A2,YM=7; //240x320 ID=0x9341
//const int TS_LEFT=104,TS_RT=916,TS_TOP=77,TS_BOT=898;
//const int TS_LEFT=907,TS_RT=180,TS_TOP=172,TS_BOT=927;

const byte buttonNO = 22;
const byte buttonYES = 30;
const byte buttonDOWN = 36;
const byte buttonUP = 44;




//PORTRAIT  CALIBRATION     240 x 320
//x = map(p.x, LEFT=918, RT=170, 0, 240)
//y = map(p.y, TOP=159, BOT=957, 0, 320)

//LANDSCAPE CALIBRATION     320 x 240
//x = map(p.y, LEFT=77, RT=898, 0, 320)
//y = map(p.x, TOP=916, BOT=104, 0, 240)



TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 100
#define MAXPRESSURE 1000

int16_t BOXSIZE;
int16_t PENRADIUS = 1;
uint16_t ID, oldcolor, currentcolor;
uint8_t Orientation = 0;    //PORTRAIT

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#include <SPI.h>
#include <SD.h>

File root;

File myFile;

char fileName[16];

const int chipSelect = 10;

const byte ramCS = 27;
const byte ramMISO = 26;
const byte ramCLK = 25;
const byte ramMOSI = 24;

const byte tapeOut = A9;
const byte tapeIn = A8;

const byte AP=LOW;
const byte NAP=HIGH;

word ramAddr=0;
word ramAddrTop=0;

byte mState;
byte parity;

int HighestFile = 0;

int oldx, oldy, oldz, tpevent;
int Highlighted;
int SelectedLine = -1;
int ListTop, ListMax;
int Pressure;
int FirstTouch;
int LastyPos, LastDir;
int PTimer=0;
unsigned long lastMillis;
int keyTimer;


void setup(void)
{

   pinMode(ramCS, OUTPUT);
   pinMode(ramMISO, INPUT);
   pinMode(ramCLK, OUTPUT);
   pinMode(ramMOSI, OUTPUT);

   digitalWrite(ramCS,HIGH);
   digitalWrite(ramCLK,LOW);
   digitalWrite(ramMISO,HIGH);
   delayMicroseconds(100);

   pinMode(tapeOut, OUTPUT);
   TapePulse(0);
   pinMode(tapeIn, INPUT);

   
   pinMode(buttonNO, INPUT);
   pinMode(buttonYES, INPUT);
   pinMode(buttonDOWN, INPUT);
   pinMode(buttonUP, INPUT);

   digitalWrite(buttonNO, HIGH);
   digitalWrite(buttonYES, HIGH);
   digitalWrite(buttonDOWN, HIGH);
   digitalWrite(buttonUP, HIGH);
    
    tft.reset();
    ID = tft.readID();
    tft.begin(ID);

    Serial.begin(9600);
//    show_Serial();

    tft.setRotation(Orientation);
    tft.fillScreen(BLACK);
//    show_tft();

    tft.setTextSize(2);    //20 lines of 20 characters
    tft.setCursor(0, 0);
    tft.println("VIP TAPE v1.Wa");


   RAMsetMode();

   RAMtest();

   RAMClear();



    currentcolor = RED;

  
  tft.println("\r\nInitializing SD card");

  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on Arduino Uno boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
  pinMode(SS, OUTPUT);

  if (!SD.begin(chipSelect)) {
    tft.println("fail!");
    return;
  }
  tft.println("successful.");

  root = SD.open("/");

  delay(1000);


  readDirectory(root, 0);
  Serial.print("ListTop="); Serial.println(ListTop);
  Serial.print("ListMax="); Serial.print(ListMax);

//  ramSerialDumpHEX(ramAddrTop);

  Serial.println("done!");

  printList();

}


void loop() {
    int xpos, ypos, zpos;  //screen coordinates
    
    int i, p;

    if (lastMillis != millis()) {
       lastMillis = millis();
       keyTimer--;
       if (keyTimer<0) keyTimer=0;
   }

    
    tp = ts.getPoint();   //tp.x, tp.y are ADC values

    // if sharing pins, you'll need to fix the directions of the touchscreen pins
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    

   if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
      Pressure=100;
      if (abs(oldx-tp.x)>20) {
          oldx=tp.x;
          tpevent=1;
      }
      if (abs(oldy-tp.y)>20) {
         oldy=tp.y;
         tpevent=1;
      }
      if (abs(oldz-tp.z)>50) {
         oldz=tp.z;
         tpevent=1;
      }
      
      if (tpevent == 1) {
          tpevent=0;
      
          ypos = map(tp.y, TS_TOP, TS_BOT, 0, 16);
          zpos = map(tp.z, 500, 100, 0, 5);

//          printList(0,ypos);

         if (Highlighted) {
             if (SelectedLine!=ypos) DeselectFile();
         }

         if (ypos+ListTop<ListMax) {
         
            Highlighted=1;
            SelectedLine=ypos;
            
// BLACK   BLUE  RED  GREEN CYAN  MAGENTA YELLOW  WHITE 
            switch (zpos) {
                case 0: tft.setTextColor(GREEN, BLACK); break;
                case 1: tft.setTextColor(YELLOW, BLACK); break;
                case 2: tft.setTextColor(YELLOW, BLUE); break;
                case 3: tft.setTextColor(YELLOW, MAGENTA); break;
                case 4: tft.setTextColor(YELLOW, RED); break;
            }

            printLine(ypos);

            if (FirstTouch == 0) FirstTouch=ypos+1;

            if (LastyPos == 0) {
               LastyPos = ypos;
            } else {           
               if (LastyPos < ypos){   //Moving down list
                  Serial.println("Down");
                  if (LastDir >0) {    //If this opposite previous direction, cancel FirstTouch
                     FirstTouch = -1;     
                     Serial.println("**");             
                  }
                  LastDir = -1;
                  LastyPos = ypos;
               }
               if (LastyPos > ypos){   //Moving up list
                 Serial.println("up");
                  if (LastDir <0) {    //If this opposite previous direction, cancel FirstTouch
                     FirstTouch = -1;
                     Serial.println("**");
                  }
                  LastDir = 1;
                  LastyPos = ypos;
               }
            }
         }

         
      }
   } else {
      if (Pressure > 0) {
        Pressure--;
        if (Pressure > 30) {
           PTimer++;
           if (PTimer >80) {
              PTimer = 0;

              ListTop = SelectedLine + ListTop;
              tft.fillScreen(BLACK);
              tft.setCursor(0, 0);  //20 pixels per line
              tft.setTextColor(YELLOW, BLUE); 
              printLine(0);
             
              if (ListTop < ListMax-1) {
                 tft.setTextColor(WHITE, BLACK);              
                 tft.setCursor(0, 40);
                 tft.println("Send File?");
              }

              tft.setCursor(0, 80);
              tft.setTextColor(WHITE, GREEN);
              tft.print("          ");
              tft.setTextColor(WHITE, RED);
              tft.println("          ");
              
              tft.setCursor(0, 96);
              tft.setTextColor(WHITE, GREEN);              
              tft.print("   YES    ");
              tft.setTextColor(WHITE, RED);
              tft.println("    NO    ");

              tft.setCursor(0, 112);
              tft.setTextColor(WHITE, GREEN);
              tft.print("          ");
              tft.setTextColor(WHITE, RED);
              tft.println("          ");


              
              while (1) {
                 delay(200);
                 tp = ts.getPoint();   //tp.x, tp.y are ADC values
                 pinMode(XM, OUTPUT);
                 pinMode(YP, OUTPUT);
                 if (tp.z < MINPRESSURE ) break;                
              }


              while (1) {
                 delay(50);
                 tp = ts.getPoint();   //tp.x, tp.y are ADC values
                 pinMode(XM, OUTPUT);
                 pinMode(YP, OUTPUT);
                 if (digitalRead(buttonYES) == LOW) {
                     p=1000;
                     break;
                 }
                 if (digitalRead(buttonNO) == LOW) { 
                     p=000;
                     break;
                 }

                 if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
                    ypos = map(tp.y, TS_TOP, TS_BOT, 0, 16);
                    p=tp.x;
                    if (ypos >= 3 && ypos <= 6) break;
                 }
                 
              }

              if (p > 200) {
                //Serial.print("YES");
                doYES();

              } else {
                //Serial.print("NO");
                
              }

              readDirectory(root, 0);
              printList();

           }
           
        }  else {
           PTimer=0;
        }
        if (Pressure==0){
           if (Highlighted > 0) {
               int D;
               D=0;
               if (FirstTouch>0) D = FirstTouch-SelectedLine-1;
               Serial.println("+++");
               FirstTouch=0;
               LastDir=0;
               LastyPos=0;
//               DeselectFile();
               if (D!=0) AdjustListTop(D);
           }
        }
      }
   }

   if (keyTimer == 0) {
      keyTimer=75;
      
      if (digitalRead(buttonDOWN) == LOW) {                  //If "DOWN" advance to next file in list, jump to Print DIR
         if (Highlighted == 0) {
            Highlighted =1;
            SelectedLine =0;
            tft.setTextColor(YELLOW, BLUE); 
            printLine(SelectedLine);
         } else {
            if (SelectedLine < 15) {
               if ((ListTop + SelectedLine) < ListMax - 1) {
                  tft.setTextColor(WHITE, BLACK);
                  printLine(SelectedLine++);
                  tft.setTextColor(YELLOW, BLUE); 
                  printLine(SelectedLine);        
               }               
            } else {
                if (ListTop < ListMax) { 
                    AdjustListTop(16);
                }
            }
         }         
      }
      
      if (digitalRead(buttonUP) == LOW) {                  
         if (Highlighted == 0) {
            Highlighted =1;
            SelectedLine =0;
            tft.setTextColor(YELLOW, BLUE); 
            printLine(SelectedLine);
         } else {
            if (SelectedLine > 0) {
               if ((ListTop + SelectedLine) > 0) {
                  tft.setTextColor(WHITE, BLACK);
                  printLine(SelectedLine--);
                  tft.setTextColor(YELLOW, BLUE); 
                  printLine(SelectedLine);        
               }               
            } else {
                if (ListTop > 0) { 
                    AdjustListTop(-16);
                }
            }
         }         
      }

      if (digitalRead(buttonYES) == LOW) {
         if (Highlighted != 0) {
            ListTop = SelectedLine + ListTop;
            doYES();
            readDirectory(root, 0);
            printList();          
         }
      }
   }
}


void doYES(){
   tft.fillScreen(BLACK);
   tft.setTextColor(WHITE, BLACK);

   if (ListTop < ListMax-1) {     //Send File
      readFile();
      TapeSend();
   } else {                       //Record New
      TapeRead();
   }
}

int AdjustListTop(int d){
   int NewListTop = ListTop;
   NewListTop += d;
   if (NewListTop > ListMax-10) NewListTop = ListMax-10;
   if (NewListTop  < 0) NewListTop = 0;
   if (ListTop != NewListTop){
      ListTop = NewListTop;
      Serial.print("ListTop="); Serial.println(ListTop);
      if (d == 16) SelectedLine=0;
      if (d == -16) {
           SelectedLine=15;
           if (SelectedLine > ListMax - 1) SelectedLine = ListMax -1;
      }
      printList();
      return 1;
   }                   
}

void DeselectFile() {
   Highlighted=0;
   tft.setTextColor(WHITE, BLACK);
   printLine(SelectedLine);
}


void junk(){

          // are we in top color box area ?
//          if (ypos < BOXSIZE) {               //draw white border on selected color box
//              oldcolor = currentcolor;
//                currentcolor = RED;
//                tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
//          }

        // are we in drawing area ?
//        if (((ypos - PENRADIUS) > BOXSIZE) && ((ypos + PENRADIUS) < tft.height())) {
//            tft.fillCircle(xpos, ypos, PENRADIUS, currentcolor);
//        }

  
}


void printList(){
   int p = ListTop;
   int m = ListMax;
   int y = 0;
   int i;

   tft.fillScreen(BLACK);
   tft.setTextColor(WHITE, BLACK);
   for (y=0;y<16;y++) {
      if (SelectedLine == y) {
         tft.setTextColor(YELLOW, BLUE); 
         printLine(y);
         tft.setTextColor(WHITE, BLACK);
      } else {
         printLine(y);
      }
   }
}

void printLine(int y){
   word pp = y + ListTop; 
   byte v;
   if (pp >= ListMax) return;
   tft.setCursor(0, y * 20);  //20 pixels per line

   pp *= 16;
   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
   ramSPIwrite(0x03);            //Read RAM
   ramSPIwrite(pp >> 8);         //Address High
   ramSPIwrite(pp & 0xFF);       //Address Low

   for(int i=0;i<=14;i++) {
      v=ramSPIread();
      if (v != 0) tft.write(v);
   }
   v=ramSPIread();
   tft.setCursor(200, y * 20);
   if (v != 0) tft.write(v);
   ramDeselect();  //Complete Command with CS = HIGH
}





//==================================================================================================

void TapeRead(){

   long t=0;
   int j=0;
   byte v=0;
   byte b=0;

   tft.setCursor(0,0);
   tft.print("Tape Read");
   delay(100);  

     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x02);      //Write RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low
     ramAddr=0;

   
   noInterrupts();

   while (t<8000) {     //wait for 2000 pulses of leader
      t++;
      if (TapeInput() == 0) {
         j++;  
      } else {
         j=0;
      }     
      if (j > 8000) { t=0;  break; }
   }
   
   while (t<17000) {     //wait for first start bit
      t++;
      if (TapeInput() == 1) { 
         t=0;
         break; 
      }
   }

   while (t<17000) {
      parity=1;
      for (b=1;b<=8;b++){
         v>>=1;
         if (TapeInput() == 1) v|=0x80;
      }
      TapeInput();    //fetch Parity
      if ((parity & 1) == 0) {
         t=-1;
         break;
      }

      ramSPIwrite(v);
      ramAddr++;
      if (ramAddr==4096) {
        t=4;
        break;
      }

      v=TapeInput();
      if (v != 1) {  //Check for next Start Bit
         t = v-1;    //t=-1 Error, or t=1 EOF
         break;         
      }
   }

     ramDeselect();  //Complete Command with CS = HIGH
     ramAddrTop=ramAddr;

   interrupts();

   tft.setCursor(0,20);
   if (t >= 10000) tft.print("Time Out");   
   switch (t){
     case  4: tft.print("4096 bytes MAX ");     
     case  1: tft.print("EOF");   
             printSize();
             writeFile();
             t=0;
             break;
     case -1: tft.print("Parity Error");     break;        
     case -2: tft.print("Start Bit Error");  break;
     default: tft.print(" Error "); tft.print(t); break;
   }
   if (t) delay(10000);
}

int TapeInput(){
   int b=digitalRead(tapeIn);
   int t=0;

   while(t<1000){
      if (digitalRead(tapeIn) == 1) break;
      t++;
   }
   t=0;   
   while(t<1000){
      if (digitalRead(tapeIn) == 0) break;
      t++;
   }

   if (t<100) return 0;
   parity++;
   if (t<200) return 1;
   return 2;
}


void writeFile(){

  char jnam[20];
  int hf;
  byte v;

  for(int i=0;i<20;i++) jnam[i] = '\0';
  
  HighestFile++;
  itoa(HighestFile,jnam,10);

  for(int i=0;i<20;i++) {
     if (jnam[3] != '\0') break;
     jnam[3] = jnam[2];
     jnam[2] = jnam[1];
     jnam[1] = jnam[0];
     jnam[0] = '0';
  }
  for(int i=4;i<=7;i++) jnam[i] = jnam[i-4];
  
  jnam[0] = 'F';
  jnam[1] = 'I';
  jnam[2] = 'L';
  jnam[3] = 'E';
  jnam[8] = '.';
  jnam[9] = 'B';
  jnam[10] = 'I';
  jnam[11] = 'N';
  jnam[12] = '\0';

  Serial.println(jnam);

  myFile = SD.open(jnam, FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
     tft.println("\r\n  Writing File");
     tft.print(jnam);

     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x03);      //Read RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low

     for (word a=0;a<ramAddrTop;a++){
        v=ramSPIread();
        myFile.write(v);
     }
    myFile.close();
    ramDeselect();  //Complete Command with CS = HIGH
    Serial.println("done.");
    tft.setCursor(12,2);
    tft.print("!DONE!");

  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
    tft.setCursor(12,20);
    tft.print("ERROR");
  }  
}


void readFile(){
   // open the file for reading:

   byte FT=0;
   byte v;
   byte n;
   byte b;
   byte c;
   byte ah;
   byte al;
   byte s;


   ramAddr = ListTop; 
   if (ramAddr >= ListMax) return;

   ramAddr *= 16;

   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
   ramSPIwrite(0x03);            //Read RAM
   ramSPIwrite(ramAddr >> 8);         //Address High
   ramSPIwrite(ramAddr & 0xFF);       //Address Low   
   for(int i=0;i<=14;i++) {
      fileName[i] = ramSPIread();
   }
   FT = ramSPIread();    //File Type 'A', 'B', 'H'
   ramDeselect();  //Complete Command with CS = HIGH

   tft.setCursor(0, 0);
   tft.setTextColor(YELLOW, BLACK);
   tft.print("Sending File");   
   tft.setCursor(0, 20);
   tft.setTextColor(WHITE, BLACK);
   tft.print(fileName);   
   Serial.print("\nOpen File='");
   Serial.print(fileName);
   Serial.println("'");

   if (FT == 'H') tft.print("\r\nHEX MODE");   

   ramAddr=0;
   ramAddrTop=ramAddr;

   myFile = SD.open(fileName);
   if (myFile) {

     if (FT == 'H') {
        s=10;
        while (myFile.available()) {
           v=myFile.read();

           if (v == 10 || v == 13) v='<';   //0xFE;   //End a Line
           else if (v == ':') {n=1; s=1;}                //Begin a line
           else if (v >= '0' && v <= '9') v &= 0xF;     //HEX Digit
           else if (v >= 'A' && v <= 'F') v -= 'A'-10;  //HEX Digit
           else if (v >= 'a' && v <= 'f') v -= 'a'-10;  //HEX Digit
           else v='~'; //0xF0;                                  //Ignore
          
           if (v <= 0xF) {    //HEX Digit
              if (n == 1) {
                 n=0;
                 b = v <<4;
              } else {
                 n=1;
                 b |= v;
                 
                 switch (s) {
                     case 1:    c=b;  break; //Get Length
                     case 2:   ah=b;  break; //Get High Addr
                     case 3:   al=b;  break; //Get Low Addr
                     case 4:   //Get Record Type
                        if (b != 0) s=10;
                        break;
                     case 5:   //Set Address & Get Data
                        digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
                        ramSPIwrite(0x02);      //Write RAM
                        ramSPIwrite(ah);      //Address High
                        ramSPIwrite(al);      //Address Low
                        ramAddr = ah;
                        ramAddr <<=8;
                        ramAddr |=al;                        
                        ramSPIwrite(b);      //Data
                        ramAddr++;
                        c--;
                        if (c == 0) s=6;
                        break;
                      case 6:  //Get Data only
                        c--;
                        if (c != 0) s=5;
                        ramSPIwrite(b);      //Data
                        ramAddr++;
                        break;
                      case 7:
                        ramDeselect();  //Complete Command with CS = HIGH
                        if (ramAddr>ramAddrTop) ramAddrTop = ramAddr;
                        break;
                      default:  s--;  break;                        
                 }
                 s++;
              }
           }
        }
        ramAddrTop--;
        ramAddrTop &=0xFF00;
        ramAddrTop +=0x0100;   // bump up to next page        

     } else {
      
        digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
        ramSPIwrite(0x02);      //Write RAM
        ramSPIwrite(0);      //Address High
        ramSPIwrite(0);      //Address Low
        while (myFile.available()) {
           ramSPIwrite(myFile.read());
           ramAddr++;
           if (ramAddr == 0xFFFF) break;
        }
        ramDeselect();  //Complete Command with CS = HIGH
        ramAddrTop=ramAddr;

     }
     printSize();

//     ramSerialDumpHEX(ramAddrTop);
    
     // read from the file until there's nothing else in it:
     // close the file:
     myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening: ");
    Serial.println(fileName);
     tft.setCursor(0, 60);
     tft.print("ERROR Opening File");   
     delay(10000);
  }
}

void TapeSend(){
     word a;
     byte v;
     byte bits;

     noInterrupts();

     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x03);      //Read RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low

     for (a=0;a<0x4000;a++) TapePulse(0);
     TapePulseOV(0);   //come back 140uSec earlier
     
     for (a=0;a<ramAddrTop;a++){
        v=ramSPIread();
        parity=1;
        TapePulse(1);               //Start Bit
        for (bits=1;bits<=8;bits++){
            TapePulse(v & 1);      //8 Data bits
            v>>=1;
        }
        TapePulseOV(parity & 1);   //Parity Bit        
     }
     ramDeselect();  //Complete Command with CS = HIGH

     digitalWrite(tapeOut,AP);  //finish Tape Send with long pulse
     delayMicroseconds(3000);
     digitalWrite(tapeOut,NAP);
     for (a=0;a<0x500;a++) TapePulse(0);

     interrupts();
     tft.setCursor(0,0);
     tft.setTextColor(GREEN, BLACK);
     tft.print("File Transfer Done!");
     delayMicroseconds(5000);
}

void TapePulseOV(byte b){
  int hl=250;   //0 bit   (2 Khz)
  if (b != 0) {
     hl=625;  //1 bit  (800 Hz)
     parity++;
  }
  hl-=5;   //overhead factor
  digitalWrite(tapeOut,AP);
  delayMicroseconds(hl);
  digitalWrite(tapeOut,NAP);
  delayMicroseconds(hl-140);  
}

void TapePulse(byte b){
  int hl=250;   //0 bit   (2 Khz)
  if (b != 0) {
     hl=625;  //1 bit  (800 Hz)
     parity++;
  }
  hl-=5;   //overhead factor
  digitalWrite(tapeOut,AP);
  delayMicroseconds(hl);
  digitalWrite(tapeOut,NAP);
  delayMicroseconds(hl);  
}

void printSize() {
     long p;
     p = ramAddrTop;
     p = (p+255)/256;
     tft.setCursor(0, 80);
     tft.print("Bytes: ");   
     tft.print(ramAddrTop);   
     tft.setCursor(0, 100);
     tft.print("Pages: ");   
     tft.print(p);   
}


void readDirectory(File dir, int numTabs) {

   char jnam[20];
   byte v;
   byte w;
   int L;
   int ramptr=0;
   int CA;
   int CC;
   int CB;
   int CL;

   SelectedLine = -1;
   ListTop = 0;
   Highlighted=0;
   FirstTouch = 0;
   LastyPos = 0;
   ramAddr=0x0000;

  
  // Begin at the start of the directory
  dir.rewindDirectory();

  ramAddr=0;
  digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
  ramSPIwrite(0x02);      //Write RAM
  ramSPIwrite(0);      //Address High
  ramSPIwrite(0);      //Address Low

  
  while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");

        strcpy(jnam,"  <<RECORD>>   " );
        for (int i=0;i<15;i++){
           ramSPIwrite(jnam[i]);
           ramAddr++;
        }
        while ((ramAddr & 0xF) !=0) {
           ramSPIwrite(0);
           ramAddr++;          
        }
        ListMax=ramAddr/16;
        ramAddrTop=ramAddr;
        ramDeselect();  //Complete Command with CS = HIGH
        break;
     }
     // Print the 8.3 name

     // Recurse for directories, otherwise print the file size
     if (entry.isDirectory()) {
       //Serial.println("/");
       //printDirectory(entry, numTabs+1);
     } else {



      myFile = SD.open(entry.name());
      if (myFile) {
         L=0; CA=0; CB=0; CC=0; CL=0; w=1;
         while (myFile.available()) {
             v = myFile.read();
             if (v == 10 || v == 13) {
                CL++;
                w=1;
             } else {
                if (v >= 0x20 && v < 0x7F) {
                    CA++;
                    if (v == ':' && w == 1) CC++;
                } else {
                    CB++;
                }
                w=0;
             }
             if (L++ > 512) break;
         }

         CL = '?';
         if (CA > 5) CL = 'A';
         if (CC > 5) CL = 'H';
         if (CB > 5) CL = 'B';
         
      }
      myFile.close();


        L=strlen(entry.name());
        strncpy(jnam, entry.name(), L);
        if (L>15) L=15;
        for (int i=0;i<L;i++){
           ramSPIwrite(jnam[i]);
           ramAddr++;
        }
        while ((ramAddr & 0xF) !=0) {
           if ((ramAddr & 0xF) == 0xF) {
              ramSPIwrite(CL);
           } else {
              ramSPIwrite(0);            
           }
           ramAddr++;          
        }
        if ((jnam[0] == 'f') ||  (jnam[0] == 'F')) {
        if ((jnam[1] == 'i') ||  (jnam[1] == 'I')) {
        if ((jnam[2] == 'l') ||  (jnam[2] == 'L')) {
        if ((jnam[3] == 'e') ||  (jnam[3] == 'E')) {
           jnam[0]=jnam[4];
           jnam[1]=jnam[5];
           jnam[2]=jnam[6];
           jnam[3]=jnam[7];
           jnam[4]='\0';
           L = atoi(jnam);
           if (L > HighestFile) HighestFile = L;
           
//           Serial.print(L);
        } } } }
     
     }
     entry.close();
   }
   
}


void RAMsetMode(){
   byte vr;

/*   
   pinMode(ramMISO, OUTPUT);
   digitalWrite(ramMISO,HIGH); 
   digitalWrite(ramMOSI,HIGH);

   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
      digitalWrite(ramCLK,HIGH);
      digitalWrite(ramCLK,LOW);
      digitalWrite(ramCLK,HIGH);
      digitalWrite(ramCLK,LOW);
   ramDeselect();  //Complete Command with CS = HIGH
   delayMicroseconds(100);

   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
      digitalWrite(ramCLK,HIGH);
      digitalWrite(ramCLK,LOW);
      digitalWrite(ramCLK,HIGH);
      digitalWrite(ramCLK,LOW);
      digitalWrite(ramCLK,HIGH);
      digitalWrite(ramCLK,LOW);
      digitalWrite(ramCLK,HIGH);
      digitalWrite(ramCLK,LOW);
   ramDeselect();  //Complete Command with CS = HIGH
   delayMicroseconds(100);

   
   pinMode(ramMISO, INPUT);

*/
   
//   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
//   for (int i=1;i<=100;i++){
//      ramSPIwrite(0xFF);      //Write Mode 
//   }
//   ramDeselect();  //Complete Command with CS = HIGH
//   delayMicroseconds(1000);

  
   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
   ramSPIwrite(0x01);      //Write Mode
   ramSPIwrite(B01000000);       //Sequential Read/Write
   ramDeselect();

   delayMicroseconds(100);
  
   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
   ramSPIwrite(0x05);      //Read Mode
   vr=ramSPIread();
   ramDeselect();  //Complete Command with CS = HIGH
   delayMicroseconds(100);
   Serial.print("Mode=");
   Serial.println(vr);

   delay(100);

   
}

void ramDeselect(){
   digitalWrite(ramCS,HIGH);  //Complete Command with CS = HIGH
//   digitalWrite(ramCLK,HIGH);
//   digitalWrite(ramCLK,LOW);
}


void ramSPIwrite(byte tx) {
   byte txb=tx;
   for (int i=1;i<=8;i++){
      digitalWrite(ramMOSI,(txb & 0x80));
//      delayMicroseconds(10);
      digitalWrite(ramCLK,HIGH);
//      delayMicroseconds(10);
      txb<<=1;
      digitalWrite(ramCLK,LOW);
//      delayMicroseconds(10);
   }
}

byte ramSPIread(){
   byte rxb=0;
//   noInterrupts();
   for (int i=1;i<=8;i++){
      rxb<<=1;
      if (digitalRead(ramMISO)) rxb|=1;
//      delayMicroseconds(10);
      digitalWrite(ramCLK,HIGH);
//      delayMicroseconds(10);
      digitalWrite(ramCLK,LOW);
//      delayMicroseconds(10);
   }
//   noInterrupts();
//   interrupts();
   return rxb;
}



void RAMtest(){
  byte fail;
  byte t;
  byte v;
  byte vr;
  
  tft.setCursor(0, 20);
  tft.println("Test 4K of RAM ");

  t=4;
  fail=false;
  for (byte i=0;i<140;i++){
    //Serial.println(i);
     tft.print(t);
     tft.print(" ");

     t--;
     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x02);      //Write RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low
     v=i;
     for (ramAddr=0;ramAddr<4096;ramAddr++){
        ramSPIwrite(v);
        v+=17;
     }
     ramDeselect();  //Complete Command with CS = HIGH

     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x03);      //Read RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low
     v=i;
     for (ramAddr=0;ramAddr<4096;ramAddr++){
        vr=ramSPIread();
        if (v != vr){
           fail=true;
           Serial.println("FAIL");
           break;
        }
        v+=17;
     }
     ramDeselect();  //Complete Command with CS = HIGH
     if (fail==true) break;
     i+=42;
  }

  //Serial.println("DONE");
  
  if (fail==true) {
     tft.print("Fail");
  } else {
     tft.print("OK");
  }
}

void RAMClear(){
   byte t;
   tft.setCursor(0, 60);
   tft.println("Clearing RAM ");

   ramAddr=0;
   digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
   ramSPIwrite(0x02);      //Write RAM
   ramSPIwrite(ramAddr>>8);      //Address High
   ramSPIwrite(ramAddr & 0xFF);  //Address Low
   digitalWrite(ramMOSI,LOW);
   t=8;
   for (long i=0;i<=524288;i++){
      digitalWrite(ramCLK,HIGH);
      digitalWrite(ramCLK,LOW);
      if ((i & 0xFFFF) == 0){
         tft.print(t);
         tft.print(" ");
        
          t--;
      }
   }
   ramDeselect();  //Complete Command with CS = HIGH
}


void ramSerialDump(word addrtop){
     word a;
     Serial.println("\nRAM Dir:");
     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x03);      //Read RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low
     for (a=0;a<addrtop;a++){
        Serial.write(ramSPIread());
        if ((a & 0xF) == 0xF) Serial.println("");
     }
     ramDeselect();  //Complete Command with CS = HIGH
}

void ramSerialDumpTXT(word addrtop){
     word a;
     Serial.println("\nDump RAM Text:");
     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x03);      //Read RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low
     for (a=0;a<addrtop;a++){
        Serial.write(ramSPIread());
     }
     ramDeselect();  //Complete Command with CS = HIGH
}

void ramSerialDumpHEX(word addrtop){
     word a;
     Serial.println("\nDump RAM Text:");
     digitalWrite(ramCS,LOW);  //Start Command with CS = LOW
     ramSPIwrite(0x03);      //Read RAM
     ramSPIwrite(0);      //Address High
     ramSPIwrite(0);      //Address Low
     for (a=0;a<addrtop;a++){
        if ((a & 0xF) == 0) {
          Serial.println("");
          printHEX4(a);
          Serial.print(" ");
        }
        printHEX2(ramSPIread());
        Serial.print(" ");
     }
     ramDeselect();  //Complete Command with CS = HIGH
}


void printHEX4(word a){
   if (a <= 0xFFF) Serial.print("0");
   if (a <= 0xFF) Serial.print("0");
   if (a <= 0xF) Serial.print("0");
   Serial.print(a,HEX);
}

void printHEX2(byte a){
   if (a <= 0xF) Serial.print("0");
   Serial.print(a,HEX);
}
