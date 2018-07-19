/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;             /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */

 //u8 u8ShowData[16][10]={ {0},{0},{0},{0xE0},{0x40},{0x40},{0x40},{0x40},{0x40},{0x40},{0x40},{0x40},{0x42},{0xFE},{0},{0}};

static u8 u8ShowData[16][10]={ {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};


/*南*/
static u8 u8HanziData1[] = {0x01,0x00,0x01,0x00,0xFF,0xFE,0x01,0x00,0x01,0x00,0x7F,0xFC,0x48,0x24,0x44,0x44,0x4F,0xE4,0x41,0x04,0x41,0x04,0x5F,0xF4,0x41,0x04,0x41,0x04,0x41,0x14,0x40,0x08};
/*京*/
static u8 u8HanziData2[] = {0x02,0x00,0x01,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,0x1F,0xF0,0x10,0x10,0x10,0x10,0x10,0x10,0x1F,0xF0,0x01,0x00,0x11,0x10,0x11,0x08,0x21,0x04,0x45,0x04,0x02,0x00};
/*工*/
static u8 u8HanziData3[] = {0x00,0x00,0x00,0x00,0x7F,0xFC,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00};
/*程*/
u8 u8HanziData4[] = {0x08,0x00,0x1D,0xFC,0xF1,0x04,0x11,0x04,0x11,0x04,0xFD,0xFC,0x10,0x00,0x30,0x00,0x39,0xFE,0x54,0x20,0x54,0x20,0x91,0xFC,0x10,0x20,0x10,0x20,0x13,0xFE,0x10,0x00};
/*学*/
u8 u8HanziData5[] = {0x22,0x08,0x11,0x08,0x11,0x10,0x00,0x20,0x7F,0xFE,0x40,0x02,0x80,0x04,0x1F,0xE0,0x00,0x40,0x01,0x80,0xFF,0xFE,0x01,0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02,0x00};
/*院*/
u8 u8HanziData6[] = {0x00,0x40,0x78,0x20,0x4B,0xFE,0x52,0x02,0x54,0x04,0x61,0xF8,0x50,0x00,0x48,0x00,0x4B,0xFE,0x48,0x90,0x68,0x90,0x50,0x90,0x41,0x12,0x41,0x12,0x42,0x0E,0x44,0x00};
/*欢*/
u8 u8HanziData7[] = {0x00,0x80,0x00,0x80,0xFC,0x80,0x04,0xFC,0x05,0x04,0x49,0x08,0x2A,0x40,0x14,0x40,0x10,0x40,0x28,0xA0,0x24,0xA0,0x45,0x10,0x81,0x10,0x02,0x08,0x04,0x04,0x08,0x02};
/*迎*/
u8 u8HanziData8[] = {0x00,0x00,0x20,0x80,0x13,0x3C,0x12,0x24,0x02,0x24,0x02,0x24,0xF2,0x24,0x12,0x24,0x12,0x24,0x12,0xB4,0x13,0x28,0x12,0x20,0x10,0x20,0x28,0x20,0x47,0xFE,0x00,0x00};
/*您*/
u8 u8HanziData9[] = {0x09,0x00,0x09,0x00,0x11,0xFC,0x32,0x04,0x54,0x48,0x99,0x50,0x11,0x48,0x12,0x44,0x14,0x44,0x11,0x40,0x10,0x80,0x02,0x00,0x51,0x04,0x51,0x12,0x90,0x12,0x0F,0xF0};
/*blank*/
u8 u8HanziBlank[32] = {0};

u8 *u8Data1 = u8HanziData1;
u8 *u8Data2 = u8HanziData2;
u8 *u8Data3 = u8HanziData3;
u8 *u8Data4 = u8HanziData4;
u8 *u8Data5 = u8HanziData5;
u8 *u8Data6 = u8HanziData6;
u8 *u8Data7 = u8HanziData7;
u8 *u8Data8 = u8HanziData8;
u8 *u8Data9 = u8HanziData9;
u8 *u8Data10 = u8HanziBlank;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  AT91C_BASE_PIOA->PIO_PER  |= (PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3|PA_03_HSMCI_MCCK);
  AT91C_BASE_PIOA->PIO_PER  |= (PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO);
  AT91C_BASE_PIOA->PIO_PDR  &= ~(PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3|PA_03_HSMCI_MCCK);
  AT91C_BASE_PIOA->PIO_PDR  &= ~(PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO);
  
  AT91C_BASE_PIOA->PIO_OER  |= (PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3|PA_03_HSMCI_MCCK);
  AT91C_BASE_PIOA->PIO_OER  |= (PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO);
  AT91C_BASE_PIOA->PIO_ODR  &= ~(PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3|PA_03_HSMCI_MCCK);
  AT91C_BASE_PIOA->PIO_ODR  &= ~(PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO);
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{ 
  static u32 u32TimeCounter = 0;
  u32TimeCounter++;
  
  WriteMomentShowData(u32TimeCounter);
  
  if (u32TimeCounter == 1500)
  {
    u32TimeCounter = 0;
  }
  
  Show();
                        
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */


void WriteMomentShowData(u32 Time)
{
  switch(Time)
  {
  case 100:
    WriteToShowData(1,u8HanziData1); 
    break;
    
  case 200:
    WriteToShowData(1,u8HanziData2);
    WriteToShowData(2,u8HanziData1);
    break;
    
  case 300:
    WriteToShowData(1,u8HanziData3);
    WriteToShowData(2,u8HanziData2);
    WriteToShowData(3,u8HanziData1);
    break;
    
  case 400:
    WriteToShowData(1,u8HanziData4);
    WriteToShowData(2,u8HanziData3);
    WriteToShowData(3,u8HanziData2);
    WriteToShowData(4,u8HanziData1);
    break;
    
  case 500:
    WriteToShowData(1,u8HanziData5);
    WriteToShowData(2,u8HanziData4);
    WriteToShowData(3,u8HanziData3);
    WriteToShowData(4,u8HanziData2);
    WriteToShowData(5,u8HanziData1);
    break;
    
  case 600:
    WriteToShowData(1,u8HanziData6);
    WriteToShowData(2,u8HanziData5);
    WriteToShowData(3,u8HanziData4);
    WriteToShowData(4,u8HanziData3);
    WriteToShowData(5,u8HanziData2);
    break;
    
  case 700:
    WriteToShowData(1,u8HanziData7);
    WriteToShowData(2,u8HanziData6);
    WriteToShowData(3,u8HanziData5);
    WriteToShowData(4,u8HanziData4);
    WriteToShowData(5,u8HanziData3);
    break;
    
  case 800:
    WriteToShowData(1,u8HanziData8);
    WriteToShowData(2,u8HanziData7);
    WriteToShowData(3,u8HanziData6);
    WriteToShowData(4,u8HanziData5);
    WriteToShowData(5,u8HanziData4);
    break;
    
  case 900:
    WriteToShowData(1,u8HanziData9);
    WriteToShowData(2,u8HanziData8);
    WriteToShowData(3,u8HanziData7);
    WriteToShowData(4,u8HanziData6);
    WriteToShowData(5,u8HanziData5);
    break;
  
  case 1000:
    WriteToShowData(1,u8HanziBlank);
    WriteToShowData(2,u8HanziData9);
    WriteToShowData(3,u8HanziData8);
    WriteToShowData(4,u8HanziData7);
    WriteToShowData(5,u8HanziData6);
    break;
    
  case 1100:
    WriteToShowData(1,u8HanziBlank);
    WriteToShowData(2,u8HanziBlank);
    WriteToShowData(3,u8HanziData9);
    WriteToShowData(4,u8HanziData8);
    WriteToShowData(5,u8HanziData7);
    break;
    
  case 1200:
    WriteToShowData(1,u8HanziBlank);
    WriteToShowData(2,u8HanziBlank);
    WriteToShowData(3,u8HanziBlank);
    WriteToShowData(4,u8HanziData9);
    WriteToShowData(5,u8HanziData8);
    break;
    
  case 1300:
    WriteToShowData(1,u8HanziBlank);
    WriteToShowData(2,u8HanziBlank);
    WriteToShowData(3,u8HanziBlank);
    WriteToShowData(4,u8HanziBlank);
    WriteToShowData(5,u8HanziData9);
    break;
    
  case 1400:
    WriteToShowData(1,u8HanziBlank);
    WriteToShowData(2,u8HanziBlank);
    WriteToShowData(3,u8HanziBlank);
    WriteToShowData(4,u8HanziBlank);
    WriteToShowData(5,u8HanziBlank);
    break;
    
  }
}


void Show(void)
{
  for (u8 i=0;i<16;i++)
  {
    MBI_data(i); //Load row data
    AT91C_BASE_PIOA->PIO_SODR |= PA_11_BLADE_UPIMO;// /OE=1,The output driver is disabled
    Delay(5);
    AT91C_BASE_PIOA->PIO_SODR |= PA_12_BLADE_UPOMI;//LE=1,data is transmitted to the output latch
    Delay(5);
    AT91C_BASE_PIOA->PIO_CODR |= PA_12_BLADE_UPOMI;// LE=0,data locked
    Delay(5);
    LineDisplay(i);    
    AT91C_BASE_PIOA->PIO_CODR |= PA_11_BLADE_UPIMO;// /OE=0,The output driver is enabled
    Delay(5);
  }
}


void WriteToShowData(u8 u8WordLocation,u8 *u8Data)
{
  for (u8 i=0;i<16;i++)
  {
    u8ShowData[i][(2*u8WordLocation)-1] = *u8Data;
    u8Data++;
    u8ShowData[i][(2*u8WordLocation)-2] = *u8Data;
    u8Data++;
  }     
}


void Delay(u8 Delaytime)
{
  for (u8 i=0;i<Delaytime;i++)
  {
    
  }
}


void MBI_data(u8 Line)
{
  u8 u8Data = 0;
  
  for (u8 i=0;i<10;i++)
  {
    u8Data = u8ShowData[Line][i];
    
    for (u8 j=0;j<8;j++)
    {
      AT91C_BASE_PIOA->PIO_CODR |= PA_15_BLADE_SCK;// M_CLK=0;
      
      if ( (0x01&u8Data) == 1)
      {
        AT91C_BASE_PIOA->PIO_SODR |= PA_14_BLADE_MOSI; //M_SDI=1,Light up the LED
      }
      else
      {
        AT91C_BASE_PIOA->PIO_CODR |= PA_14_BLADE_MOSI; //M_SDI=0,The LED is not bright
      }
      
      Delay(5);
      AT91C_BASE_PIOA->PIO_SODR |= PA_15_BLADE_SCK; // M_CLK=1 
      Delay(5);
      u8Data=u8Data>>1;  //start from low bit,and move to M_SDI bit by bit
      Delay(5);
    }
  }
}


void  LineDisplay(u8 Line_)
{
  AT91C_BASE_PIOA->PIO_SODR |=PA_03_HSMCI_MCCK;//CD_STB=1; 
  AT91C_BASE_PIOA->PIO_CODR |=PA_04_HSMCI_MCCDA;//INH=0; 
 
  switch (Line_)
  {
case 0:
      /*A=0,B=0,C=0,D=0,Line0*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3; 
      break;
       
case 1:
      /*A=1,B=0,C=0,D=0,Line1*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3;
      break;
      
case 2:
      /*A=0,B=1,C=0,D=0,Line2*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3;
      break;
      
case 3:
      /*A=1,B=1,C=0,D=0,Line3*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3;  
      break;
      
case 4:
      /*A=0,B=0,C=1,D=0,Line4*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3; 
      break;
      
case 5:
      /*A=1,B=0,C=1,D=0,Line5*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3;   
      break;
      
case 6:
      /*A=0,B=1,C=1,D=0,Line6*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3; 
      break;

case 7:
      /*A=1,B=1,C=1,D=0,Line7*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_CODR |=PA_08_SD_CS_MCDA3; 
      break;
      
case 8:
      /*A=0,B=0,C=0,D=1,Line8*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3; 
      break;

case 9:
      /*A=1,B=0,C=0,D=1,Line9*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3;  
      break;
      
case 10:
      /*A=0,B=1,C=0,D=1,Line10*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3;
      break;
      
case 11:
      /*A=1,B=1,C=0,D=1,Line11*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3;
      break;
      
case 12:
      /*A=0,B=0,C=1,D=1,Line12*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3;
      break;
      
case 13:
      /*A=1,B=0,C=1,D=1,Line13*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_CODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3;
      break;
      
case 14:
      /*A=0,B=1,C=1,D=1,Line14*/
      AT91C_BASE_PIOA->PIO_CODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3;
      break;
      
case 15:
      /*A=1,B=1,C=1,D=1,Line15*/
      AT91C_BASE_PIOA->PIO_SODR |=PA_05_HSMCI_MCDA0;
      AT91C_BASE_PIOA->PIO_SODR |=PA_06_HSMCI_MCDA1;
      AT91C_BASE_PIOA->PIO_SODR |=PA_07_HSMCI_MCDA2;
      AT91C_BASE_PIOA->PIO_SODR |=PA_08_SD_CS_MCDA3;
      break;
  }
   
}


static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
