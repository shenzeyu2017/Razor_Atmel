/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

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
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u8 u8Role = 0x01;                             /*1--Seek(slave)  2-- Hide(master)*/
static bool bSeek = TRUE;                               
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static AntAssignChannelInfoType sAntSetupData;

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
  u8 au8GameName[] = "Hide and Go Seek!";
  u8 au8HintMessage[] = "Press B0 to Start"; 
 
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8GameName); 
  LCDMessage(LINE2_START_ADDR, au8HintMessage); 

  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE1_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  if( AntAssignChannel(&sAntSetupData) )
  {
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    UserApp1_StateMachine = UserApp1SM_Error;
  }

}/* end UserApp1Initialize() */


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
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */


static void UserApp1SM_Idle(void)
{
  /*Press Button0 to start the game*/
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    LCDCommand(LCD_CLEAR_CMD); 
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP); 
    UserApp1_StateMachine = UserApp1SM_CountDown;
  }   
} /* end UserApp1SM_Idle() */

/*Change the role , slave for seek and master for hide*/
static void UserApp1SM_RoleChange(void)
{
  if (u8Role == 0x01)
  {
    /*change the channel type to slave*/
    sAntSetupData.AntChannelType = ANT_CHANNEL_TYPE1_USERAPP;
    if( AntAssignChannel(&sAntSetupData))
    {
      UserApp1_StateMachine = UserApp1SM_WaitOpen;
    }
    else
    {
      UserApp1_StateMachine = UserApp1SM_Error;
    }
  }
  
  /*change the channel type to master*/
  if (u8Role == 0x02)
  {
    sAntSetupData.AntChannelType = ANT_CHANNEL_TYPE2_USERAPP;
    if( AntAssignChannel(&sAntSetupData))
    {
      UserApp1_StateMachine = UserApp1SM_WaitOpen;
    }
    else
    {
      UserApp1_StateMachine = UserApp1SM_Error;
    }
  }
}


static void UserApp1SM_WaitOpen(void)
{
 if (AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED) 
 {
   AntOpenChannelNumber(ANT_CHANNEL_USERAPP); 
   if (u8Role == 0x02)
   {
     UserApp1_StateMachine = UserApp1SM_Hide;
   }
   if (u8Role == 0x01)
   {
     UserApp1_StateMachine = UserApp1SM_CountDown;
   }
   
 }
}

/*10s for coutdown*/
static void UserApp1SM_CountDown(void)
{
  u8 au8Role1Message[] = "       Seeker     ";
  static u8 au8ShowTime[3] = {0,0,0};
  static u32 u32TimeCount = 0;
  u8 au8CountDownMessage[] = "CountDown";
  u8 au8Message1[] = "   Ready or not ?";
  u8 au8Message2[] = "   Here I come !";
  
  
  if (u32TimeCount == 0)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = '1';
    au8ShowTime[2] = '0';
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR , au8Role1Message);
    LCDMessage(LINE2_START_ADDR,au8CountDownMessage); 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 1000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '9'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 2000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '8'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 3000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '7'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 4000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '6'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 5000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '5'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 6000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '4'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 7000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '3'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 8000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '2'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 9000)
  {
    au8ShowTime[0] = ':';
    au8ShowTime[1] = ' ';
    au8ShowTime[2] = '1'; 
    LCDMessage(LINE2_START_ADDR + 10, au8ShowTime); 
  }
  if (u32TimeCount == 10000)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Message1);
    LCDMessage(LINE2_START_ADDR, au8Message2);
  }  
  u32TimeCount++;
  if (u32TimeCount == 11000)
  {
    u32TimeCount = 0;
    UserApp1_StateMachine = UserApp1SM_Seek;
  }
}


static void UserApp1SM_Seek(void)
{
  static bool bBuzzer = TRUE;
  static bool bLedOn = FALSE;
  u8 au8FoundMessage[] = {1,1,1,1,1,1,1,1};
  static s8 s8Rssi = 0;
  static u32 u32TimeCounter = 0;
  static u8 au8Seeking[] = "Seeking";
  static u8 au8Dot[3] = {0};
/**************************************************************************/ 
/*show the information "seeking..."*/
  if (bSeek == TRUE)
  {
    u32TimeCounter++;
    if (u32TimeCounter == 1000)
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8Seeking);
      bLedOn = TRUE;
    }
    if (u32TimeCounter == 2000)
    {
      au8Dot[0] = '.';
      au8Dot[1] = ' ';
      au8Dot[2] = ' ';
      LCDMessage(LINE1_START_ADDR, au8Seeking);
      LCDMessage(LINE1_START_ADDR +7, au8Dot);
    }
    if (u32TimeCounter == 3000)
    {
      au8Dot[0] = '.';
      au8Dot[1] = '.';
      au8Dot[2] = ' ';
      LCDMessage(LINE1_START_ADDR, au8Seeking);
      LCDMessage(LINE1_START_ADDR +7, au8Dot);
    }
    if (u32TimeCounter == 4000)
    {
      u32TimeCounter = 0;
      au8Dot[0] = '.';
      au8Dot[1] = '.';
      au8Dot[2] = '.';
      LCDMessage(LINE1_START_ADDR, au8Seeking);
      LCDMessage(LINE1_START_ADDR +7, au8Dot);
    }
  }
 /*************************************************************************/ 
 
  /*according to the RSSI to judge if fing the other,
  different RSSI has different performance*/
  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      s8Rssi = G_sAntApiCurrentMessageExtData.s8RSSI;
      if (bSeek == TRUE && bLedOn == TRUE)
      {
        if (s8Rssi && s8Rssi <= -90)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,100);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOff(PURPLE);    
          LedOff(BLUE);    
          LedOff(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);       
        }
        if (-90 < s8Rssi && s8Rssi <= -83)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,300);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOff(BLUE);    
          LedOff(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);      
        }
        if (-83 < s8Rssi && s8Rssi <= -76)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,500);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOff(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);        
        }
        if (-76 < s8Rssi && s8Rssi <= -69)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,700);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);      
        }
        if (-69 < s8Rssi && s8Rssi <= -62)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,900);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);       
        }
        if (-62 < s8Rssi && s8Rssi <= -55)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,1100);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOn(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED); 
        }
         if (-55 < s8Rssi && s8Rssi <= -48)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,1400);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOn(YELLOW);    
          LedOn(ORANGE);    
          LedOff(RED); 
        }
         if (-48 < s8Rssi && s8Rssi <= -41)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,1700);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOn(YELLOW);    
          LedOn(ORANGE);    
          LedOn(RED); 
          s8Rssi = 0;
          AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8FoundMessage);
          UserApp1_StateMachine = UserApp1SM_Found;
        }
      }
    }
  }
  /*********************************************************************/
  
  /*press B3 to close the BUZZER*/
  if (bBuzzer == TRUE)
  {
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      PWMAudioOff(BUZZER1);
      bBuzzer = FALSE;
    }
  }
  if (bBuzzer == FALSE)
  {
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      bBuzzer = TRUE;
      PWMAudioOn(BUZZER1);
    }
  }
  /*****************************************************************/
 
  /*check the channel status , if channel not open , 
  jump to the UserApp1SM_StayOpen to open the channel*/
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) != ANT_OPEN)
  {
    UserApp1_StateMachine = UserApp1SM_StayOpen;
    u32TimeCounter = 0;
  }
  
}

/*show some information and prepare to close the channel*/
static void UserApp1SM_Found(void)
{
  PWMAudioOff(BUZZER1);
  u8 au8Found[]= "Found you !";
  static u32 u32TimeCounter = 0;
  u32TimeCounter++;
    
  if (u32TimeCounter >= 1000)
  {
    u32TimeCounter = 0;
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR +5, au8Found);
    LedOff(WHITE);
    LedOff(PURPLE);    
    LedOff(BLUE);    
    LedOff(CYAN);
    LedBlink(GREEN,LED_2HZ);    
    LedOff(YELLOW);    
    LedOff(ORANGE);    
    LedOff(RED); 
    u8Role = 0x02; 
    UserApp1_StateMachine = UserApp1SM_CloseWait;
  }
}

/*the role is hide ,the channel type is master*/
static void UserApp1SM_Hide(void)
{
  PWMAudioOff(BUZZER1);
  static u8 u8Check = 0;
  static bool bShow = TRUE;
  u8 au8TestMessage[] = {0,0,0,0,0,0,0,0};
  u8 au8Role2Message[] = "       Hide     ";
  u8 au8FoundData [] = {2,2,2,2,2,2,2,2};
  u8 au8FoundMessage [] ="   You Found Me   ";
  
 
  if (bShow == TRUE)
  {
    LedOff(GREEN);
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Role2Message);
    bShow = FALSE;
  }
  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      for(u8 i=0;i<ANT_DATA_BYTES;i++)
      {
        if(G_au8AntApiCurrentMessageBytes[i] == au8FoundData[i])
        {
          u8Check++;
        }
      }
      if (u8Check == 8)
      {
        AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
        bShow = TRUE;
        bSeek = TRUE;
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, au8FoundMessage);
        UserApp1_StateMachine = UserApp1SM_CloseWait;
        u8Role = 0x01;
      }
      u8Check = 0;
    }
    if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
     AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8TestMessage);
    }
  }  
}

/*wait for the channel close */
static void UserApp1SM_CloseWait(void)
{
  static u32 u32TimeCounter = 0;
    
  if (AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    u32TimeCounter++;
  }
  if (u32TimeCounter >= 4000)
  {
    u32TimeCounter = 0;
    AntUnassignChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_RoleChange; 
  }
}


static void UserApp1SM_StayOpen(void)
{
  if (u8Role == 1)
  {
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Seek;
  }
}

     




/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */
