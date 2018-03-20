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
extern u8 G_au8DebugScanfBuffer[]; 
extern u8 G_u8DebugScanfCharCount; 
static u8 UserApp_au8UserInputBuffer[U16_USER_INPUT_BUFFER_SIZE];


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
  LedOff(LCD_RED);
  LedOff(LCD_GREEN);
  LedOff(LCD_BLUE);
  for(u16 i = 0; i < U16_USER_INPUT_BUFFER_SIZE  ; i++)
  {
    UserApp_au8UserInputBuffer[i] = 0;
  }

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
  u8 u8State = 0;
  static u32 u32Counter = 0;/*time counter to control buzzer*/
  static bool bBuzzer = FALSE;
  u8 u8CharCount = DebugScanf(UserApp_au8UserInputBuffer);
  UserApp_au8UserInputBuffer[u8CharCount] = '\0';
  /*information show to users*/
  u8 u8Message1[]= "Entering state 1";
  u8 u8Message2[]= "STATE 1";  
  u8 u8Message3[]= "Entering state 2";
  u8 u8Message4[]= "STATE 2";
  u8 u8Message5[]= "Please input 1(Press button1) to enter state1 or 2(Press button2) to enter state2";
  /*to choose the state1 or state2*/
  if (u8State == 0)
  {
    if (WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      u8State = 1;
    }
    if (WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      u8State =2;
    }
    if (UserApp_au8UserInputBuffer[0]!='\0')
    {
      if (UserApp_au8UserInputBuffer[0]==0x31)
      {
        u8State = 1;
      }
      else if (UserApp_au8UserInputBuffer[0]==0x32)
      {
        u8State = 2;
      }
      else
      {
        DebugPrintf(u8Message5);
        DebugLineFeed();
      }
    }
  }
  /*state 1*/
  if (u8State == 1)
  {
    DebugPrintf(u8Message1);
    DebugLineFeed();
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(0x00,u8Message2);
    LedOff(GREEN);
    LedOff(YELLOW);
    LedOff(ORANGE);
    LedOff(RED);
    LedOn(WHITE);
    LedOn(PURPLE);
    LedOn(BLUE);
    LedOn(CYAN);
    LedOn(LCD_RED);
    LedOn(LCD_BLUE);
    bBuzzer = FALSE;
    PWMAudioOff(BUZZER1);
    u8State = 0;
  }
  /*state 2*/
  if (u8State == 2)
  {
    DebugPrintf(u8Message3);
    DebugLineFeed();
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(0x00,u8Message4);
    LedOff(WHITE);
    LedOff(PURPLE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedBlink(GREEN,LED_1HZ);
    LedBlink(YELLOW,LED_2HZ);
    LedBlink(ORANGE,LED_4HZ);
    LedBlink(RED,LED_8HZ);
    LedOff(LCD_BLUE);
    LedOn(LCD_RED);
    LedOn(LCD_GREEN);
    PWMAudioSetFrequency(BUZZER1,200);
    bBuzzer = TRUE;
    u8State = 0;
  } 
  /*to control buzzer works 100ms every second*/
  if (bBuzzer == TRUE)
  {
    u32Counter++;
    if (u32Counter <= 100)
    {
      PWMAudioOn(BUZZER1);
    }
    if (100 < u32Counter && u32Counter <= 1000)
    {
      PWMAudioOff(BUZZER1);
    }
    if (u32Counter > 1000)
    {
      PWMAudioOff(BUZZER1);
      u32Counter = 0;
    }
  }

} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
