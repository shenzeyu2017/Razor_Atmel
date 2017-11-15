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

static u8 au8Message1[] = "B0:Name";
static u8 au8Message2[] = "B1:Debug";
static u8 au8Message3[] = "B3:MENU";
/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */
extern u8 G_au8DebugScanfBuffer[];
extern u8 G_u8DebugScanfCharCount;
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


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
static u8 UserApp_au8UserInputBuffer[U16_USER_INPUT_BUFFER_SIZE];
void UserApp1Initialize(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR,au8Message1);
  LCDMessage(LINE1_START_ADDR + 10,au8Message2);
  LCDMessage(LINE2_START_ADDR,au8Message3);
  LCDCommand(LCD_HOME_CMD);
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
  static u8 au8Message[] = "Shenzeyu"; 
  static u32 u32TimeCounter = 0;
  static u32 u32TimeCounter1 = 0;
  static u32 u32TimeCounter2 = 0;
  static u32 u32Address = 0x00;
  static u8 u8State = 0;
  static u8  u8CharCount = 0;
  static u32  u32Address1 = 0;
  static s32  s32Address2= 0;
  static bool bRight = FALSE;
  static bool bLeft = FALSE;
  static bool bMenu = TRUE;
  static bool bChangeLine = FALSE;
  static bool bLine1 = TRUE;
  
  if (bMenu)
  { /*to choose the state,state1: name,state2:debug*/
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,au8Message);
      u8State = 1;
      bMenu = FALSE;
    }
    if (WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      LCDCommand(LCD_CLEAR_CMD);
      u8State = 2;
      bMenu = FALSE;
      DebugPrintf("\n\rPlease input \n\r");
      DebugPrintf("\n\rEnter to change the line \n\r");
      DebugPrintf("\n\rbutton3 to return to the Menu \n\r");
    }
  }
  if(u8State == 1)
  {/*mode : NAME*/
    if (WasButtonPressed(BUTTON3))
    {/*return to the Menu*/
      ButtonAcknowledge(BUTTON3);
      bMenu = TRUE;
      LedOff(CYAN);
      LedOff(BLUE);
      LedOff(GREEN);
      LedOff(RED);
      PWMAudioOff(BUZZER1);
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,au8Message1);
      LCDMessage(LINE1_START_ADDR + 10,au8Message2);
      LCDMessage(LINE2_START_ADDR,au8Message3);  
      u8State = 0;
      u32Address = 0;
      u32TimeCounter = 0;
      u32TimeCounter1 = 0;
      u32TimeCounter2 = 0;
      bRight = FALSE;
      bLeft = FALSE;
    }
    if (WasButtonPressed(BUTTON1))
    {/*choose right shift*/
      ButtonAcknowledge(BUTTON1);
      bRight = TRUE;
      bLeft = FALSE;
    }
    if (WasButtonPressed(BUTTON0))
    {/*choose left shift*/
      ButtonAcknowledge(BUTTON0);
      bRight = FALSE;
      bLeft = TRUE;
    }  
    if (bRight)
    {/*right shift*/
      if (u32Address <0x4C)
      {
        u32TimeCounter2++;
        if (u32TimeCounter2 <= 100)
        {
          LedOff(GREEN);
          LedOff(CYAN);
          LedOn(BLUE);
          PWMAudioSetFrequency(BUZZER1,200);
          PWMAudioOn(BUZZER1);
        }
        if (u32TimeCounter2 >100 && u32TimeCounter2 <= 200)
        {
          LedOff(GREEN);
          LedOff(BLUE);
          LedOn(CYAN);
          PWMAudioSetFrequency(BUZZER1,400);
          PWMAudioOn(BUZZER1);
        }
        if (u32TimeCounter2 >200 && u32TimeCounter2 <= 300)
        {
          LedOff(CYAN);
          LedOff(BLUE);
          LedOn(GREEN);
          PWMAudioSetFrequency(BUZZER1,600);
          PWMAudioOn(BUZZER1);
        }
        if (u32TimeCounter2 > 300) 
        {
          LedOff(GREEN);
          LedOff(CYAN);
          LedOff(BLUE);
          u32TimeCounter2 = 0;
        }
      }
      if (bChangeLine)
      {
        u32TimeCounter = 0; 
        u32TimeCounter1++;
        if (u32TimeCounter1 == 500)
        {
          u32TimeCounter1 = 0;
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          u32Address++;
          bChangeLine = FALSE; 
        }
      }  
      u32TimeCounter ++; 
      if(u32TimeCounter >= 500)  
      {
        if (u32Address <= 0x13)
        {
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          LCDMessage(u32Address+20,au8Message);
          u32TimeCounter = 0; 
          u32Address++;
        }
        
        if (u32Address == 0x14)
        {
          u32Address = 0x40;
          bChangeLine = TRUE; 
          u32TimeCounter = 0;
        }
        if (u32Address >=0x41 && u32Address < 0x4C)
        {
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          u32TimeCounter = 0; 
          u32Address++;
        }
        if (u32Address == 0x4B)
        {
          bChangeLine = TRUE; 
          u32TimeCounter = 0;
        }
        if(u32Address == 0x4C)
        {
          LedOff(CYAN);
          LedOff(BLUE);
          LedOff(GREEN);
          u32TimeCounter2 = 0;
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          u32TimeCounter = 0; 
          PWMAudioSetFrequency(BUZZER1,1000);
          LedOn(RED);
        }
        else 
        {
          LedOff(RED);
        }
      }
    }
    if (bLeft)
    {/*left shift*/
      if (u32Address >0x00)
      {
        u32TimeCounter2++;
        if (u32TimeCounter2 <= 500)
        {
          LedOff(BLUE);
          LedOff(CYAN);
          LedOn(GREEN);
          PWMAudioSetFrequency(BUZZER1,700);
          PWMAudioOn(BUZZER1);
        }
        if (u32TimeCounter2 >100 && u32TimeCounter2 <= 200)
        {
          LedOff(GREEN);
          LedOff(BLUE);
          LedOn(CYAN);
          PWMAudioSetFrequency(BUZZER1,800);
          PWMAudioOn(BUZZER1);
        }
        if (u32TimeCounter2 >200 && u32TimeCounter2 <= 300)
        {
          LedOff(CYAN);
          LedOff(GREEN);
          LedOn(BLUE);
          PWMAudioSetFrequency(BUZZER1,900);
          PWMAudioOn(BUZZER1);
        }
        if (u32TimeCounter2 > 300) 
        {
          LedOff(GREEN);
          LedOff(CYAN);
          LedOff(BLUE);
          u32TimeCounter2 = 0;
        }
      }
      if (bChangeLine)
      {
        u32TimeCounter = 0; 
        u32TimeCounter1++;
        if (u32TimeCounter1 == 500)
        {
          if (u32Address == 0x27)
          {
            u32TimeCounter1 = 0;
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(u32Address,au8Message);
            LCDMessage(u32Address-20,au8Message);
            u32Address--;
            bChangeLine = FALSE; 
          }
          else
          {
            u32TimeCounter1 = 0;
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(u32Address,au8Message);
            u32Address--;
            bChangeLine = FALSE; 
          }
        }
      }  
      u32TimeCounter ++; 
      if (u32TimeCounter>=500)
      {
        if (u32Address <=0x4C && u32Address >= 0x40)
        {
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          u32TimeCounter = 0; 
          u32Address--;
        }
        if ( u32Address == 0x3F)
        {
          u32Address = 0x27;
          bChangeLine = TRUE; 
          u32TimeCounter = 0;
        }
        if (u32Address<= 0x26 && u32Address>= 0x1F)
        {
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          LCDMessage(u32Address-20,au8Message);
          u32TimeCounter = 0; 
          u32Address--;
        }
        if (u32Address == 0x1E)
        { 
          u32Address = 0x0A;
          bChangeLine = TRUE; 
          u32TimeCounter = 0;
        }
        if (u32Address>0x0A && u32Address <=0x13)
        {
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          LCDMessage(u32Address +20,au8Message);
          u32TimeCounter = 0; 
          u32Address--;
        }
        if (u32Address == 0x0A)
        {
          bChangeLine = TRUE; 
          u32TimeCounter = 0;
        }
        if (u32Address <= 0x09 && u32Address >0x01)
        {
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          u32TimeCounter = 0; 
          u32Address--;
        }
        if (u32Address == 0x01)
        {
          bChangeLine = TRUE; 
          u32TimeCounter = 0;
        }
        if (u32Address == 0x00)
        {
          LedOff(CYAN);
          LedOff(BLUE);
          LedOff(GREEN);
          u32TimeCounter2 = 0;
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(u32Address,au8Message);
          u32TimeCounter = 0;  
          PWMAudioSetFrequency(BUZZER1,1000);
          PWMAudioOn(BUZZER1);
          LedOn(RED);
        }
        else 
        {
          LedOff(RED);
          PWMAudioOff(BUZZER1);
        }
      }    
    }   
  }
  if (u8State == 2)
  {/*mode : debug*/
    if (WasButtonPressed(BUTTON3))
    { /*return to the Menu*/
      ButtonAcknowledge(BUTTON3);
      bMenu = TRUE;
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,au8Message1);
      LCDMessage(LINE1_START_ADDR + 10,au8Message2);
      LCDMessage(LINE2_START_ADDR,au8Message3);  
      u8State = 0;
      bChangeLine = FALSE;
      s32Address2 = 0;
      u32Address1 = 0;
      bLine1 = TRUE;
    }
    u8CharCount = DebugScanf(UserApp_au8UserInputBuffer);
    UserApp_au8UserInputBuffer[u8CharCount] = '\0';
    if (u8CharCount == 1)
    {
      if (bLine1)
      {/*show on the line1*/
        if ( bChangeLine )
        {
          if (s32Address2 == 0)
          {
            bChangeLine = FALSE;
            u32Address1++;
          }
          else 
          {
            bChangeLine = FALSE;
            s32Address2 = 0;
            u32Address1 = 0;
          }
        }
        if (u32Address1 < 20)
        {
          if (UserApp_au8UserInputBuffer[0] == 0x0D)
          {/*change the line*/
            u8CharCount = 0;
            s32Address2 = 0;
            u32Address1 =19;            
            LCDClearChars(LINE2_START_ADDR,20);
            DebugPrintf("\n\rHave a new Line !\n\r");
          }
          LCDMessage(LINE1_START_ADDR + u32Address1, UserApp_au8UserInputBuffer);
          u32Address1++;
        }
        if (u32Address1 == 20)
        {
          bChangeLine = TRUE;
        }
        if( u32Address1 == 21)
        {
          bLine1 = FALSE;;
          LCDClearChars(LINE2_START_ADDR,20);
        }
      } 
      if (bLine1 == FALSE)
      {/*show on the line2*/
        if (bChangeLine)
        {
          s32Address2 = 0;
          bChangeLine = FALSE;
          u32Address1 = 0;
        }
        if (s32Address2 < 20)
        {
          if (UserApp_au8UserInputBuffer[0] == 0x0D)
          {/*change the line*/
            u8CharCount = 0;
            u32Address1 = 0;
            s32Address2 = 19;
            LCDClearChars(LINE1_START_ADDR,20);
            DebugPrintf("\n\rHave a new Line !\n\r");
          }
          LCDMessage(LINE2_START_ADDR + s32Address2, UserApp_au8UserInputBuffer);
          s32Address2++;
        }
        if (s32Address2 == 20)
        {
          bChangeLine = TRUE;
          bLine1 = TRUE;
          LCDClearChars(LINE1_START_ADDR,20);
        }
      }
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
