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
extern u32 G_u32AntApiCurrentMessageTimeStamp;                    /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;                /* From ant_api.c */
extern u8 G_au8DebugScanfBuffer[];

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;              /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                        /* Timeout counter used across states */

static AntAssignChannelInfoType UserApp1_sChannelInfo; /* ANT setup parameters */

static u8 UserApp1_au8MessageFail[] = "\n\r***ANT channel setup failed***\n\n\r";


static u8 au8Max[1] = {0};
static u8 au8Min[1] = {0xFF};
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
  u8 ANTPLUS_NETWORK_KEY[8] = {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};
  LCDCommand(LCD_CLEAR_CMD);
  for(u32 i = 0; i < 10000; i++);

  UserApp1_sChannelInfo.AntChannel          = ANT_CHANNEL_USERAPP;
  UserApp1_sChannelInfo.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
 
  UserApp1_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  UserApp1_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  UserApp1_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntFrequency        = ANT_FREQUENCY_USERAPP;
  UserApp1_sChannelInfo.AntTxPower          = ANT_TX_POWER_USERAPP;

  UserApp1_sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    UserApp1_sChannelInfo.AntNetworkKey[i] = ANTPLUS_NETWORK_KEY[i];
  }
  
  /* Attempt to queue the ANT channel setup */
  if( AntAssignChannel(&UserApp1_sChannelInfo) )
  {
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    DebugPrintf(UserApp1_au8MessageFail);
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
/* Wait for ANT channel assignment */
static void UserApp1SM_AntChannelAssign()
{ 
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    /* Channel assignment is successful, so open channel and
    proceed to Idle state */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_ModeChoose;
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp1_u32Timeout, 3000))
  {
    DebugPrintf(UserApp1_au8MessageFail);
    UserApp1_StateMachine = UserApp1SM_Error;    
  }
     
} /* end UserApp1SM_AntChannelAssign */


static void UserApp1SM_ModeChoose(void)
{
  static bool bOpened = FALSE;
  u8 au8Mode1[] = "B0:Real-time Rate";
  u8 au8Mode2[] = "B1:Extended Data";
  u8 au8Mode3[] = "B2:Battery";
  u8 au8Mode4[] = "B3:hold 3s to close";
  
  static u32 u32TimeCounter = 0;
  
  /***************************************************************/
  /*Show the prompt information*/
  if (u32TimeCounter == 0)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR , au8Mode1);
    LCDMessage(LINE2_START_ADDR , au8Mode2);
  }
  u32TimeCounter++;
  if (u32TimeCounter == 2000)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR , au8Mode3);
    LCDMessage(LINE2_START_ADDR , au8Mode4);
  }
  if (u32TimeCounter == 4000)
  {
    u32TimeCounter = 0;
  }
 /********************************************************************/
  
  /*press B0 to show real-time rate*/
  if (WasButtonPressed(BUTTON0))
  {
    u32TimeCounter = 0;
    ButtonAcknowledge(BUTTON0);
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_RealTimeRate;
  }
  
  /*press B1 to show min and max*/
  if (WasButtonPressed(BUTTON1))
  {
    u32TimeCounter = 0;
    ButtonAcknowledge(BUTTON1);
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_ExtendedData;
  }
  
  /*press B2 to show the status of battery*/
  if (WasButtonPressed(BUTTON2))
  {
    u32TimeCounter = 0;
    ButtonAcknowledge(BUTTON2);
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_Battery;
  }
  
  /*hold B3 for 3s to close the display*/
  if (IsButtonHeld(BUTTON3,3000))
  {
    u32TimeCounter = 0;
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_Close;
  }
  
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    bOpened = TRUE;
  }
   /*channel closed,disconnect with HRM*/
  if (bOpened)
  {
    if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
    {
      u32TimeCounter = 0;
      bOpened = FALSE;
      LCDCommand(LCD_CLEAR_CMD);
      UserApp1_StateMachine = UserApp1SM_Disconnect;
    }
  }
}


static void UserApp1SM_RealTimeRate(void)
{
  static u8 au8LastData[1] = {0xFF};
  
  /*receive new data,show the rate,and store min and max*/
  if( AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA )
    {
      if(au8LastData[0] != G_au8AntApiCurrentMessageBytes[7])
      {
        au8LastData[0] = G_au8AntApiCurrentMessageBytes[7];
        ShowRate();
        StoreMax();
        StoreMin();
      }
    }
  }
  
  /*press B0 return to mode choose*/
  if (WasButtonPressed(BUTTON0))
  {
    au8LastData[0] = 0xFF;
    ButtonAcknowledge(BUTTON0);
    UserApp1_StateMachine = UserApp1SM_ModeChoose;
  }
  
  /*channel closed,disconnect with HRM*/
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_Disconnect;
  }
   /*disenable the B1,B2,B3*/
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
  }
  if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
  }
  if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
  }
} 


static void UserApp1SM_ExtendedData(void)
{
  static bool bShow = TRUE;
  u8 au8MaxMessage[] = "Max:";
  u8 au8MinMessage[] = "Min:";
  static u8 u8MaxDec = 0;
  static u8 u8MinDec = 0;
  static u8 au8ShowMax[3] = {0,0,0};
  static u8 au8ShowMin[3] ={0,0,0};
   
  if (bShow)
  {
    /*if go directly to show min and max,min=0,max=0*/
    if (au8Max[0] == 0)
    {
      au8Min[0] = 0;
    }
    
    /*hex to dec,plit into 3 bits*/
    u8MaxDec = HexToDec(au8Max[0]);
    au8ShowMax[0] = (u8MaxDec/100) + '0';
    au8ShowMax[1] = ( (u8MaxDec%100)/10 ) + '0';
    au8ShowMax[2] = ( (u8MaxDec%100)%10 ) + '0';
    if (au8ShowMax[0] == '0')
    {
      au8ShowMax[0] = ' ';
    }
    
    /*hex to dec,split into 3 bits*/
    u8MinDec = HexToDec(au8Min[0]);
    au8ShowMin[0] = (u8MinDec/100) + '0';
    au8ShowMin[1] = ( (u8MinDec%100)/10 ) + '0';
    au8ShowMin[2] = ( (u8MinDec%100)%10 ) + '0';
    if (au8ShowMin[0] == '0')
    {
      au8ShowMin[0] = ' ';
    }
     
    /*show min and max on LCD*/
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR , au8MaxMessage);
    LCDMessage(LINE1_START_ADDR +4 , au8ShowMax);
    LCDMessage(LINE1_START_ADDR +9 , au8MinMessage);
    LCDMessage(LINE1_START_ADDR +13 , au8ShowMin);
    bShow = FALSE;
  }
   /*channel closed ,disconnect with HRM*/
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_Disconnect;
  }
  
  /*press B1,return to the mode choose*/
  if (WasButtonPressed(BUTTON1))
  {
    au8Max[0] = 0;
    au8Min[0] = 0xFF;
    ButtonAcknowledge(BUTTON1);
    LCDCommand(LCD_CLEAR_CMD);
    bShow = TRUE;
    UserApp1_StateMachine = UserApp1SM_ModeChoose;
  }
  
  /*disenable the B0,B2,B3*/
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
  }
  if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
  }
  if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
  }
}


static void UserApp1SM_Battery(void)
{
  static bool bReceive = FALSE;
  static bool bShowed = FALSE;
  static u8 u8Status = 0x00;
  u8 au8CommonMeaasge[] = {0x46,0xFF,0xFF,0xFF,0xFF,0x07,0x07,0x01};
 
  /*check if the display receive the information about battery*/
  if( AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA )
    {
     if( (G_au8AntApiCurrentMessageBytes[0] == 0x07) )
     {
       u8Status = G_au8AntApiCurrentMessageBytes[3] & 0x70;
       if (bShowed == FALSE)
       {
         bReceive = TRUE;
       }
     }     
    }
    if (G_eAntApiCurrentMessageClass == ANT_TICK )
    {
      AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP,au8CommonMeaasge);
    }
  }
  
  /*if receive the battery information,show it on LCD*/
  if (bReceive)
  {
    ShowStatus(u8Status);//show information on LCD
    bReceive = FALSE;
    bShowed = TRUE;
  }
   
  /*channel closed ,disconnect with HRM*/
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_Disconnect;
  }
  
  /*press B2 return to mode choose*/
   if (WasButtonPressed(BUTTON2))
  {
    bReceive = FALSE;
    bShowed = FALSE;
    ButtonAcknowledge(BUTTON2);
    UserApp1_StateMachine = UserApp1SM_ModeChoose;
  }
  
  /*disenable the B0,B1,B3*/
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
  }
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
  }
  if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
  }
}


static void UserApp1SM_Disconnect(void)
{
  u8 au8Disconnect[] = "Lost Connection";
  u8 au8Ask[] = "Connect or close ?";
  u8 au8Choose1[] = "B0:connect again";
  u8 au8Choose2[] = "B1:close";
  static u32 u32TimeCounter = 0;
  
  /****************************************************************/
  /*Show the prompt information*/
  if (u32TimeCounter == 0)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR , au8Disconnect);
    LCDMessage(LINE2_START_ADDR , au8Ask);
  }
  u32TimeCounter++;
  if (u32TimeCounter == 2000)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR , au8Choose1);
    LCDMessage(LINE2_START_ADDR , au8Choose2);
  }
  if (u32TimeCounter == 4000)
  {
    u32TimeCounter = 0;
  }
  /***************************************************************/
  
  /*press B0,open channel again ,and return to mode choose*/
  if (WasButtonPressed(BUTTON0))
  {
    LCDCommand(LCD_CLEAR_CMD);
    u32TimeCounter = 0;
    ButtonAcknowledge(BUTTON0);
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_ModeChoose;
  }
   
  /*press B1 to close the display*/
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    LCDCommand(LCD_CLEAR_CMD);
    u32TimeCounter = 0;
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Close;
  }
   
  /*disenable the B2,B3*/
   if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
  }
   if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
  }
  
}


static void UserApp1SM_Close(void)
{
  u8 u8Close[] = "Bye";
  static u32 u32TimeCounter = 0;
  
  
  /*show the information*/
  if (u32TimeCounter == 0)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR +8 , u8Close);
  }
  u32TimeCounter++;
  
  /*wait for 4s,and close the LCD*/
  if (u32TimeCounter == 4000) 
  {
    LCDCommand(LCD_CLEAR_CMD);
    LedOff(LCD_RED);
    LedOff(LCD_GREEN);
    LedOff(LCD_BLUE);
  }
    
}


static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */

static void ShowRate(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  u8 au8ShowMessage[] = "Rate:";
  u8 u8RateDec = 0;
  u8 au8Show[3] = {0,0,0};
  
  /*hex to dec,transform each bit to ascii*/
  u8RateDec = HexToDec(G_au8AntApiCurrentMessageBytes[7]);
  au8Show[0] = (u8RateDec/100) + '0';
  au8Show[1] = ( (u8RateDec%100)/10 ) + '0';
  au8Show[2] = ( (u8RateDec%100)%10 ) + '0'; 
  
  if (au8Show[0] == '0')
  {
    au8Show[0] = ' ';
  }
   
  /*show the rate on LCD*/
  LCDMessage(LINE1_START_ADDR, au8ShowMessage);
  LCDMessage(LINE1_START_ADDR + 5, au8Show);
  return;
}

static void StoreMax(void)
{
  if (au8Max[0] < G_au8AntApiCurrentMessageBytes[7])
  {
    au8Max[0] = G_au8AntApiCurrentMessageBytes[7];
    return;
  }
  else
  {
    return;
  }
}

static void StoreMin(void)
{
 if (au8Min[0] > G_au8AntApiCurrentMessageBytes[7])
  {
    au8Min[0] = G_au8AntApiCurrentMessageBytes[7];
    return;
  }
  else
  {
    return;
  } 
}


u8 HexToDec(u8 u8Char_)
{
  u8 au8Change[2] = {0,0};
  u8 u8ReturnValue = 0;
  
  au8Change[0] = (u8Char_ /16) * 16;
  au8Change[1] = u8Char_ %16;
  u8ReturnValue = au8Change[0] + au8Change[1];
  return(u8ReturnValue);
}


void ShowStatus(u8 u8Status)
{
  u8 au8Status[] = "Status:";
  u8 au8New[] = "New";
  u8 au8Good[] = "Good";
  u8 au8Ok[] = "Ok";
  u8 au8Low[] = "Low";
  u8 au8Critical[] = "Critical";
  
  if (u8Status == 0x10)
  {
    LCDMessage(LINE1_START_ADDR, au8Status);
    LCDMessage(LINE1_START_ADDR + 7, au8New);
  }
  if (u8Status == 0x20)
  {
    LCDMessage(LINE1_START_ADDR, au8Status);
    LCDMessage(LINE1_START_ADDR + 7, au8Good);
  }
  if (u8Status == 0x30)
  {
    LCDMessage(LINE1_START_ADDR, au8Status);
    LCDMessage(LINE1_START_ADDR + 7, au8Ok);
  }
  if (u8Status == 0x40)
  {
    LCDMessage(LINE1_START_ADDR, au8Status);
    LCDMessage(LINE1_START_ADDR + 7, au8Low);
  }
  if (u8Status == 0x50)
  {
    LCDMessage(LINE1_START_ADDR, au8Status);
    LCDMessage(LINE1_START_ADDR + 7, au8Critical);
  }
  
}


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
