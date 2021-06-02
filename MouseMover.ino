/******************************************************************************/
/*                                                                            */
/*                   E M B E D D E D   C O N T R O L L E R                    */
/*                                                                            */
/*                           M O U S E   M O V E R                            */
/*                                                                            */
/*                                                                            */  
/*                      Copyright (c) 2021  Rick Groome.                      */
/*                                                                            */  
/* Permission is hereby granted, free of charge, to any person obtaining a    */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation  */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,   */
/* and/or sell copies of the Software, and to permit persons to whom the      */
/* Software is furnished to do so, subject to the following conditions:       */
/*                                                                            */
/* The above copyright notice and this permission notice shall be included in */
/* all copies or substantial portions of the Software.                        */
/*                                                                            */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        */
/* DEALINGS IN THE SOFTWARE.                                                  */
/*                                                                            */  
/*                                                                            */  
/*   PROCESSOR: ProMicro/Leonardo (ATMEGA32U4)      COMPILER: Arduino 1.8.5   */
/*   Written by: Rick Groome 2021                                             */
/*                                                                            */  
/******************************************************************************/

/* 
 
 This controller moves the mouse cursor a tiny amount every few minutes, 
 simulating the user being at the computer and using it to prevent the host PC 
 from going to sleep or locking the screen.  Unlike software only solutions 
 that many IT infrastructure systems know how to ignore, this solution looks 
 as though a real (second) mouse is added to the computer.  The normal mouse is 
 still used for normal operation.  Usually moving the mouse cursor a few pixels 
 every minute or so is enough to keep the computer from going to sleep or 
 locking.   Because the amount moved is small and it only is done every minute 
 or so, this will usually not interfere with normal PC operation.  To maintain 
 some semblance of system security (that IT groups want) this added mouse 
 controller can be programmed to only run for a limited time, after which it 
 stops moving the mouse, until commanded to run again. 

 This controller has a number of user changeable parameters.  All of the 
 parameters for the controller can be modified by sending commands to 
 the controllers serial port, which is also added to the host PC when the 
 controller is plugged in.  This controller will appear to Windows as a mouse 
 device (HID) and a serial com port.   Once the controller is programmed, it 
 will have default parameters selected for all of the changeable parameters, but 
 these can be changed and then saved to EEPROM so that the user selected 
 parameters are used each time the controller is connected to the PC. 

 This firmware is designed for use with a ProMicro Arduino board (that uses an 
 ATMEGA32U4) and is connected to the PC with it's USB port.  It could probably 
 be adapted to other Arduino controllers like the Arduino Leonardo, but the 
 Pro Micro board is tiny, powered by the USB port and low cost, making it the 
 ideal hardware solution for this task.  The only hardware required for this 
 solution is an Arduino Pro Micro controller and a USB cable that connects 
 this controller to the PC.

 There are a few jumpers that can be used with this firmware. Usually the 
 jumpers are not used, but are provided for, in case the user prefers to 
 control this device in this way. 

 One jumper called JPE (defined below) can be used to disable the controller. 
 By grounding this pin, the mouse functionality will stop (as though you 
 unplugged the mouse) and will be restored when the jumper is removed.

 A second jumper, JP2 (defined below), if grounded will disable the software 
 run timer and keep the mouse "running" all the time, no matter what the 
 programmed run time is.  Remove the jumper to restore normal operation. 

 A third jumper, JP3 (defined below), if grounded will cause the mouse to move 
 every 5 seconds, instead of the configured move time.
  
 A LED can be added between A3 and VCC to indicate when the MouseMover is on 
 (Pins labeled "A3" on ProMicro) (LED/resistor from here to VCC).  While not 
 required, it is handy to have this indicator so that you know if the MouseMover
 is on or off. 

 This controller accepts serial commands to perform various functions and report 
 current settings.  Open com port with any baud rate (other than 1200 baud)
 (N,8,1) using a program like Hyperterm or Teraterm (or your favorite terminal 
 program).   You can also create batch files to send commands to the com port.

 Commands are a single letter (to read) or a single letter followed by a number 
 (to set value) and a <CR> or <CR><LF> as in  X<CR> or X20<CR> (can also be X=20)
 
 Read  Set
 V             Return a version / copyright string.
 *IDN?         Return a version / copyright string 
               (This command is for compatibility with SCPI devices).
 X     X4      Amount to move mouse in x direction (0..100) (default 4)
 Y     Y0      Amount to move mouse in y direction (0..100) (default 0)
               (Note: For X and Y values, the mouse will move plus this many 
               pixels on the first move and then move minus this many pixels 
               on the next move and then alternate between + and - moves.)
 T     T60     How often to move mouse (seconds) (2..3600) (default 60=1 min)
 R     R540    How long to run the mouse mover (minutes) (-1..64800)(45 days max) 
               (0=Turn off, -1=run forever) (default 540=9 hrs)
 Z             Return all variables in a comma separated list.
               X,Y,<set T>,<set R>,  <current T>,<current R>, days HH:MM:SS 
               (days HH:MM:SS is total run time)
 E     E0      Reinitialize current variables to default values (shown above)               
       E1      Get current variables from EEPROM
       E2      Save current variables to EEPROM
 ?             Return a help string of commands.
 H             Return a help string of commands.
 
You can create a batch file to send a command to this device as follows: 

@echo off
rem MoverPort is comport address in format \\.\COM# (set to your comport#)
set MoverPort=\\.\COM4
rem R=<# minutes to run> (9 hours=540) (0=off)(%1=get from cmd line)
echo R=%1 >%MoverPort%

Then call the batch file with a parameter as in:    MyBatchFile 540  
(This will set Runtime to 540 minutes)

rem If you wanted to send a bunch of commands use 
copy Cmd.txt \\.\COM4
where "cmd.txt" contains the commands to send.

You can then create a desktop shortcut that can call this batch file with 
a parameter by setting the "target" of the shortcut to 

"MyBatchFile.bat 540"    
(either set the "Start in" field to the directory where the batch file is 
 or add the path to the target batchfile name)

Then when the icon is double clicked the shortcut will turn on the controller 
for another 9 hours.  (user would do this daily)

The user could also create a scheduled task to call this batch file every 
weekday (or whatever interval is desired) or every time the user logs on.

This firmware has been released to the public domain using the license 
included with this file set.   The only request I ask is that the version 
string continues to contain my name.  You are free to use it as you see fit.   
No warrantee or fitness for any purpose is implied.

********************* DISCLAIMER ***********************
This firmware and it's associated device can be used to override corporate 
IT department directives and security mechanisms in place (by keeping the 
system from going back to a lock screen).  This may not be allowed in a 
corporate or government environment and may result in mild to severe penalties.   
If in doubt, check with your IT department before using this device.  
Use at your own risk.
********************* DISCLAIMER ***********************

*****************************************
*           Revision Info               *
*****************************************

 1-20-21  REG   1.00  Initial version.
 2-3-21   REG   1.01  Added help commands (?,H), V command.  
                      Added printf / printfROM function. 
                      Revision info added. Removed extraneous debug code, 
                      Changed JP names to be in order. 
                      Notes about creating batch file/Windows shortcut/
                      scheduled task.
                      Released to the public domain.
                  
*/

// Fuses for Pro Micro: (Default)(You do not need to reprogram these)
//   (LOW=0xFF & HIGH=0xD8 & EXTENDED=0xFB & LOCKBIT=0x2F)

#include <Arduino.h>
#include <Mouse.h>

#define VERSION "1.01"  

#define VERSIONSTRING "MouseMover v" VERSION ",(c) 2021 Rick Groome (Enter '?' for help)"  

#define uint  unsigned int          // save some typing

//******************************************************************************
//*                             DIO pin assignments                            *
//******************************************************************************
// You don't need any of these, but you can use if you want. 
#define JPE       6     // (PD7) Input, JPE jumper (low to disable Mouse function)
#define JP2       2     // (PD1) Input, JP2 jumper (low to Run forever[ignore "R"])
#define JP3       5     // (PC6) Input, JP3 jumper (low to move mouse more often)

#define USRLED   21     // (PF4) User LED (LED on when mouse active)(low=on) 
//                      // (labeled "A3" on ProMicro) 
//                      // (Add LED/resistor from here to VCC)
//
// Note: Was going to use the TX/RX LEDs already on the ProMicro board but each 
// mouse move command resets the lights, so just use an added LED. 

//******************************************************************************
//*                     Structures and Variables                               *
//******************************************************************************

uint FastMoveTime = 5;     // (in seconds) (5 sec) (only if JP3 active)

#define SIGNATUREVALUE     0x1234
typedef struct SettingsTyp 
{
  byte              SettingSize;  // number of bytes in this structure
  uint              Signature;    // A unique value to look for
  uint              X;            // Amount the mouse moves on each "wiggle"                          
  uint              Y;            // Amount the mouse moves on each "wiggle"
  uint              T;            // Time between each mouse "wiggle" (seconds)  
  uint              R;            // Amount of time to run (minutes)
} SettingsTyp;

#define DfltSettings  { sizeof(Settings),SIGNATUREVALUE,4,0,1*60,9*60+1 }           

SettingsTyp  Settings=DfltSettings;

//******************************************************************************
//*          Code to implement printf via the USB virtual serial port.         *
//******************************************************************************
// This allows the use of printf or printf_P -- Sending output to Serial.write

static int COM1putter (char c, FILE *stream)  
 // putter function for Serial.write
{
  if( c == '\n' ) Serial.write('\r');
  Serial.write(c) ;   return 0 ;
}

// This is a hack to get around C++ not being able to conditionally initialize 
// parts of a variable and producing the message
// Error:  "expected primary-expression before '.' token".
// So temporarily define it this way and hope that the definition of "FILE" 
// doesn't change soon.  Using this definition saves including 'alloc' code.
#undef FDEV_SETUP_STREAM 
#define FDEV_SETUP_STREAM(p, g, f) { 0, 0, f, 0, 0, p, g, 0 } 
FILE COM1 = FDEV_SETUP_STREAM(COM1putter, NULL, _FDEV_SETUP_WRITE);
// When COM1 is defined, then the library printf function will output to the USB
// port. You must then include "stdout = &COM1" in setup or loop to use printf.
//
// Macro to get format string from flash memory.
#define printfROM(fmt, ...)   printf_P(PSTR(fmt),##__VA_ARGS__)


//******************************************************************************
//*                        Generic  Timer 1  Functions                         *
//******************************************************************************

static void (*Timer1IntFunc)(void) = NULL;

void attachTimer1Interrupt(unsigned int count, byte divisor, void (*userFunc)(void))
// Start a recurring timer interrupt on timer1. Count is the interrupt frequency, 
// divisor is the divisor to use -- 0=timer off, 1=/1,2=/8,3=/64,4=/256,
// 5=/1024,6=/ext fall,7=/ext rise.  Interrupt rate is F_OSC/divisor/count. 
// Examples (divisor=/4) 100Hz=625, 50Hz=1250, 2Hz=31250
// userFunc is the function to call each time the timer interrupts.
// Can be coded as "attachTimer1Interrupt(F_CPU/256/interuptfreq, 4, MyISR);" 
{
  if (userFunc==NULL) return;
  noInterrupts();                // disable all interrupts
  Timer1IntFunc  = userFunc;
  TCCR1A = 0;   TCCR1B = 0;
  TCCR1B |= (divisor&7);         // Prescaler.. 0=timer off, 1=/1,2=/8,3=/64,4=/256,5=/1024,6=/ext fall,7=/ext rise
  
  // auto reloading timer
  TCNT1 = 0;
  OCR1A = count;
  TCCR1B |= (1 << WGM12);        // CTC mode
  TIMSK1 |= (1 << OCIE1A);       // enable timer compare interrupt
  interrupts();                  // enable all interrupts
}

void detachTimer1Interrupt(void)
{
  noInterrupts();               // disable all interrupts
  // Shut down the timer
  Timer1IntFunc=NULL;  TCCR1B &= ~7;   TIMSK1 &= ~(1 << TOIE1);   
  interrupts();                 // enable all interrupts
}

ISR(TIMER1_COMPA_vect)        // interrupt service routine 
{
  if (Timer1IntFunc) (*Timer1IntFunc)(); 
}


//******************************************************************************
//*                 Application Specific  Timer 1 Functions                    *
//******************************************************************************

#define TIMERISRRATE    0.01                    // 10mS Timer1 rate

// this is the number of seconds since power on
unsigned long TOD=0; // 45+(59*60L)+(23*3600L)+(1*(24*3600L)); //1 day 23:59:45
// Move mouse when MoveTim counts down to 0 (seconds)
unsigned int MoveTim = 10;      
// Stop moving mouse when RunTim counts down to 1 (minutes)
// If RunTim=0xFFFF then don't decrement counter.  (Running=Runtime!=0)
unsigned int RunTim  = Settings.R;    // 9 hours

static void Timer1ISR(void)     // interrupt occurs every 10mS
{
  static byte tic = 100;  // 10mS counter
  
  if (!(--tic))           // this interrupt occurs every second
  {
    tic=100;
    if (MoveTim) MoveTim--;     // countdown counter for mouse move 
    if (((++TOD)%60)==0)        // each minute (60sec)
    { if ((RunTim>1) && (RunTim!=0xFFFF)) RunTim--; }
  }
}

//******************************************************************************
//*                                  EEPROM                                    *
//******************************************************************************

// Have EEPROM 0..0x400  0..0x2F used by other applications... 
// How about placing it at 0x2F0

typedef struct EEStructTyp 
{
  char        unusedspace[0x2F0]; // This puts the settings in eeprom at 0x2F0
  SettingsTyp EE;                 // The Settings structure 
} EEStructTyp;

EEStructTyp  EEMEM  EPROM;        // The Settings structure in EEPROM

void GetEESettings(void)
  // Get our device settings from EEPROM memory. Update EEPROM if blank.
{
  byte buffer[10];
  // read the signature word and see if EEPROM has been initialized
  eeprom_read_block(buffer,&EPROM.EE,sizeof(buffer));
  // if signature doesn't match, then write EEPROM with initialized RAM values
  if (!(*(int*)(buffer+1) == SIGNATUREVALUE) || buffer[0]!=sizeof(Settings)) 
  { eeprom_write_block(&Settings,&EPROM.EE,sizeof(Settings));  }
  // else EEPROM has good data, initialize Settings from it
  else  
  { eeprom_read_block(&Settings,&EPROM.EE,sizeof(Settings));  }
}

void SaveEESettings(void)
{
  eeprom_write_block(&Settings,&EPROM.EE,sizeof(Settings));
}

//******************************************************************************
//*                          Command processing                                *
//******************************************************************************

int strtouint(char *St,unsigned int *val)
// Convert St to integer. Returns number of characters converted and value 
// converted.  If value is larger than integer, returns 0.  If value is -1 
// then return 0xFFFF.
{
  long int V;  int i;  char *endptr;
  V=strtol(St,&endptr,10);
  if ((i=(endptr-St)) && V<0x10000 && V>-2 )  
    *val=(unsigned int)(V & 0xFFFF); 
  else 
    i=0;
  return i;
}

#define max_InBuff 80

int ProcessInput(void)
  // Receive a character from the host and buffer it.
  // When packet complete or <CR> then we have a completed message.  
  // Returns pointer to string if we have a completed message or NULL;
{
  static char InBuff[max_InBuff];  static int i;  static char LastChar;
  char ch; char *ptr,*p; unsigned int Val; int cmd,j;

  if (!Serial.available()) return 0; else  ch=Serial.read();
  // If <LF> following a <CR> then ignore it
  if (( ch=='\n') && LastChar=='\r') return 0;
  // If backspace then remove last char from buffer
  if (ch=='\b') { if (i) { i--; } LastChar=ch; return 0; } 
  // If not <CR> or <LF> and space, put character in buffer
  if (!(ch=='\r' || ch=='\n' || i>max_InBuff-2)) 
  { InBuff[i++]=ch; LastChar=ch;  return 0; }
  // Terminator received. Zero terminate buffer. If nothing in buffer, get out.
  InBuff[i++]=0;  i=0;  if (!*InBuff) return 0;
  // We have a complete string to interpret. Interpret it.
  // Remove whitespace, convert to upper case
  p=InBuff;
  while (*p) 
  { 
    if (*p==' ') strcpy(p,p+1); 
    *p=toupper(*p);  p++; 
  }

  // See if we have a valid command.
  // The if statement below looks for the characters in the string and creates
  // a 'cmd' number from it or sets 'cmd' to -1 if character is not valid cmd.  
  i=0; cmd=-1; j=0;  // i is buffer ptr, cmd is command, j is value in string.
  if ((ptr=strchr(p=(char*)"RTXY*ZE?HV",InBuff[i]))) { i+=1; cmd=ptr-p; } 
  if (cmd>=0) 
  {
    // If there's an equals sign remove it. 
    if (InBuff[i]=='=') i++;
    // If there's a number convert it (j=number of characters converted)
    j=strtouint(InBuff+i,&Val); 
    if (j && InBuff[i+j]) goto Err;
  }

  switch (cmd)
  {
              // ? or h   Help
    case 7:   
    case 8:   
              printfROM(VERSIONSTRING "\n\n"
              "Commands:\n"
              "Most commands are single letter to read or letter=value to set value\n"
              "followed by a <CR> (or <Enter>) character.\n"
              "   (e.g. 'X<CR>' to read x value and 'X=22<CR>' to set x to 22.)\n"
              "\n"
              "X    - Amount to move mouse in x direction (0..100) (dflt 4).\n"
              "Y    - Amount to move mouse in y direction (0..100) (dflt 0).\n"
              "T    - How often to move mouse (seconds) (2..3600)(dflt 60=1min).\n"
              "R    - How long to run MouseMover (minutes) (-1 to 64800)\n"
              "       0=turn off, -1=run forever, dflt=540=9hrs (max=45days)\n"
              "Z    - Return all variables in a comma separated list.\n"
              "       X,Y,<set T>,<set R>,  <current T>,<current R>, days HH:MM:SS\n" 
              "       (days HH:MM:SS is total run time)\n"
              "*IDN?  Print id / version string  (SCPI)\n"
              "V    - Print id / version string\n"
              "E    - Reinitialize current variables to default (ROM) values.\n"
              "       (Issue cmd 'E' and 'E2' to restore defaults to original).\n"
              "E1   - Get values from EEPROM.\n"
              "E2   - Store current values to EEPROM. (updates power up values)\n"
              "?,H  - Print this help text.\n"
              "\n"
              "You can create a batch file to set a parameter as follows:\n"
              "@echo off\n"
              "rem MoverPort is comport address in format \\\\.\\COM# (set to your comport#)\n"
              "set MoverPort=\\\\.\\COM4\n"
              "rem R=<# minutes to run> (9 hours=540) (0=off)(%%1=get from cmd line)\n"
              "echo R=%%1 >%%MoverPort%%\n"
              "Then call the batch file with a parameter as in:    MyBatchFile 540\n"
              "(This will set Runtime to 540)(9 hours)\n"
              "\n"
              "Enter command:\n"
              );
              break; 

              // E  save,get EEPROM values. 
    case 6:   if (!j || (j && !Val))         // 0 Reinitialize to default values
              {
                SettingsTyp  DSettings=DfltSettings;
                memcpy(&Settings,&DSettings,sizeof(SettingsTyp));
                printfROM("Default settings restored\n");
                goto Setup;
              }
              else switch (Val) {
                            // 1 = Get values from EEPROM
                case 1:     GetEESettings(); 
                            printfROM("EEPROM settings selected\n");
Setup:                      MoveTim=Settings.T;  RunTim=Settings.R; 
                            digitalWrite(USRLED,(RunTim)?LOW:HIGH);
                            break;
                            // 2 = Set values to EEPROM
                case 2:     SaveEESettings();
                            printfROM("Settings saved to EEPROM\n");
                            break;
                default:    goto Err;
              }
              break;

              //Z Status
    case 5:   noInterrupts();                // disable all interrupts
              sprintf(InBuff,(Settings.R==0xFFFF)?"%u,%u,%u,%d":"%u,%u,%u,%u",
              Settings.X,Settings.Y,Settings.T,
              (Settings.R==0xFFFF||!Settings.R)?Settings.R:Settings.R-1);       
              sprintf(InBuff+strlen(InBuff),(RunTim==0xFFFF)?", %u,%d":", %u,%u",
              MoveTim,(RunTim==0xFFFF||!RunTim)?RunTim:RunTim-1);
              sprintf(InBuff+strlen(InBuff),", %2d %02d:%02d:%02d", 
              (int)(TOD/(24*3600L)),(int)((TOD/3600)%24),(int)((TOD/60)%60),(int)(TOD%60));
              interrupts(); 
              printfROM("%s\n",InBuff);  break; 
              // *IDN? 
    case 4:   if (strcmp(&InBuff[i],"IDN?")) goto Err; 
              // V -- Get version 
    case 9:   printfROM(VERSIONSTRING "\n");  break; 
              
              // X and Y  (amount to move the mouse)
    case 3:   if (!j) 
              { 
                j=Settings.Y; 
PrintJ:         printfROM("%d (ok)\n",j); break; 
              }
              if (Val>100) goto Err;     
              Settings.Y=Val; 
Ok:           printfROM("%s (ok)\n",InBuff); break;
    case 2:   if (!j) { j=Settings.X; goto PrintJ;  }
              if (Val>100) goto Err;     
              Settings.X=Val; goto Ok;

              // T (move the mouse this often)
    case 1:   if (!j) { noInterrupts(); j=MoveTim; interrupts();  goto PrintJ; }
              // Move time:  2 sec to 1 hr is valid
              if (Val<2 || Val>3600) goto Err;    
              Settings.T=Val;  MoveTim=Val; goto Ok;

              // R (run mouse mover this long)
    case 0:   if (!j) 
              { 
                noInterrupts();  Val=(RunTim==0xFFFF||!RunTim)?RunTim:RunTim-1; interrupts();
                printfROM("%u\n",Val); break;
              }  
              // Run time (minutes)  1 min to 45 days is valid.  0 min = run forever;
              if (Val>64800 && Val!=0xFFFF) goto Err;     
              Settings.R=(Val==0xFFFF||!Val)?Val:Val+1;
              // if currently off and new command is for ON, then turn on process.
              if (Settings.R && !RunTim) { RunTim=Settings.R; digitalWrite(USRLED,LOW); }
              // if currently on and new command is for OFF, then turn off process.
              else if (!Settings.R && RunTim) { RunTim=Settings.R; digitalWrite(USRLED,HIGH); }
              else RunTim=Settings.R;  
              goto Ok;

    default:
Err:          printfROM("Error: %s\n",InBuff); 
  }
  i=0; return 1;
}

//******************************************************************************
//*                                                                            *
//*                         Main routines (setup / loop)                       *
//*                                                                            *
//******************************************************************************

void setup() {
  // setup code here, to run once

  // Set printf to point to Serial.write, start serial.
  stdout=&COM1;  Serial.begin(19200);  
  // Set JPE,JP2,JP3 as input with pullup
  pinMode(JPE,INPUT_PULLUP); 
  pinMode(JP2,INPUT_PULLUP);   pinMode(JP3,INPUT_PULLUP);   
  // Set up the outputs
  pinMode(USRLED, OUTPUT);  digitalWrite(USRLED,HIGH);
  // start the timer interrupt
  attachTimer1Interrupt(F_CPU/64/(1/0.01), 3, Timer1ISR);  
  // Get the current eeprom settings
  GetEESettings();    
}

void loop() {
  // main code here, run repeatedly:
  static bool Online = false;           // Mouse is active
  static bool Dir    = false;           // Move + or - direction

  if (digitalRead(JPE))                 // if mousemove function enabled
  {
    if (!Online) 
    {
      if (digitalRead(JP3)) MoveTim=Settings.T; else MoveTim=FastMoveTime;
      Online=true;  
      Mouse.begin(); 
      RunTim=Settings.R;
      digitalWrite(USRLED,LOW);  
    }
    if (RunTim>1)       // if run timer is active
    {
      if (!MoveTim)    // if time to move the mouse
      {
        if (digitalRead(JP3)) MoveTim=Settings.T; else MoveTim=FastMoveTime;
        if (Dir) 
          Mouse.move(Settings.X,Settings.Y,0); 
        else 
          Mouse.move(-Settings.X,-Settings.Y,0);
        Dir=!Dir;
      }
    }
    // If RunTimer not running and Stayrunning is on, restart the runtimer
    if ((!RunTim) && !digitalRead(JP2))  
    { 
      RunTim=Settings.R; digitalWrite(USRLED,LOW);  
    } 
    // If run timer has expired, either restart it if stayrunning is active or 
    // shut it down 
    if (RunTim==1) 
    {
      if (!digitalRead(JP2)) { RunTim=Settings.R; } else 
      {
        RunTim=0;  digitalWrite(USRLED,HIGH);  
      }
    }
    delay(1);
  }
  else          // MouseMover disabled
  {
    if (Online) 
    {
      Online=false;  RunTim=0;   // MoveTim=0;
      Mouse.end();  digitalWrite(USRLED,HIGH);
    }
  }
  ProcessInput();
}


