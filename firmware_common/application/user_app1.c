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
  LedOn(RED);
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
  static u32 au32PassWord[100] = {0,1,2,0,1,2,0,1,2,0};//the Initial password
  static u32 au32UserKey[100] = {0};//user input the password 
  static u32 u32Input = 0;
  static u32 u32Check = 0;
  static u32 u32RightBit = 0;
  static u32 u32Clear =0;// To clear what user have input
  static bool bLocked = TRUE;
  static bool bChangeKey = FALSE;//to control when to change the password
  static u32 u32KeyLength = 10;
  
  /*if button3 keep 2s ,user can change the password*/
  if (bLocked == TRUE)
  {
    if(IsButtonHeld(BUTTON3,2000))
    {
     ButtonAcknowledge(BUTTON3);
     u32Input = 0;
     u32RightBit = 0;
     u32Check = 0;
     LedBlink(RED,LED_1HZ);
     LedBlink(GREEN,LED_1HZ);//to hint user to change the password
     bChangeKey = TRUE;
    }
  
   /*start to change password,button0 is number 0,
   button01 is number 1,button2 is number 2,button3 for confirming 
   the password*/
    if (bChangeKey)
    {
      if(WasButtonPressed(BUTTON0))
      {
       ButtonAcknowledge(BUTTON0);
       au32PassWord[u32Input] = 0;
       u32Input++;
      }
      if(WasButtonPressed(BUTTON1))
      {
       ButtonAcknowledge(BUTTON1);
       au32PassWord[u32Input] =1;
       u32Input++;
      }
      if(WasButtonPressed(BUTTON2))
      {
       ButtonAcknowledge(BUTTON2);
       au32PassWord[u32Input] = 2;
       u32Input++;
      }
      if(WasButtonPressed(BUTTON3))
      {
       ButtonAcknowledge(BUTTON3);
       bChangeKey = FALSE;
       u32KeyLength = u32Input;
       u32Input = 0;
       u32RightBit = 0;
       u32Check = 0;
       LedOff(GREEN);
       LedOn(RED);
      }
    }   
   /*Input the password,button0 is number 0,
   button01 is number 1,button2 is number 2,
   button3 for ending what user have inputted*/
   
      if(WasButtonPressed(BUTTON0))
      {
       LedOff(RED);
       ButtonAcknowledge(BUTTON0);
       au32UserKey[u32Input] = 0;
       u32Input++;
      }
      if(WasButtonPressed(BUTTON1))
      {
       LedOff(RED);
       ButtonAcknowledge(BUTTON1);
       au32UserKey[u32Input] = 1;
       u32Input++;
      }
      if(WasButtonPressed(BUTTON2))
      {
       LedOff(RED);
       ButtonAcknowledge(BUTTON2);
       au32UserKey[u32Input] = 2;
       u32Input++;
      }
   /*Press the button3,start check the userkey and the password*/
      if(WasButtonPressed(BUTTON3))
      {
        LedOff(RED);
        u32Input = 0;
        if (u32Check <= u32KeyLength)
        {
        if (au32UserKey[u32Check] == au32PassWord[u32Check])
        {
          u32RightBit++;
        }
        u32Check++;
        }   
      }
  /*if userkey is right LED_GREEN blink*/
      if (u32RightBit == u32KeyLength)
      {
        bLocked = FALSE;
        LedOff(RED);
        u32Input = 0;
        u32Input++;
        u32RightBit = 0;
        u32Check = 0;
        LedBlink(GREEN,LED_1HZ);
        ButtonAcknowledge(BUTTON3);
      }
  /*if userkey is wrong LED_RED blink*/
      if (u32RightBit <u32KeyLength && u32Check == u32KeyLength)
      {
        bLocked = FALSE;
        LedOff(GREEN);
        u32Input = 0;
        u32RightBit = 0;
        u32Check = 0;
        LedBlink(RED,LED_1HZ);
        ButtonAcknowledge(BUTTON3);
      }
  }
  /*Press the BUTTON3 to Lock and clear what user have input*/
  if (bLocked == FALSE)
  {
    if(WasButtonPressed(BUTTON3))
    {
     u32Input = 0;
     LedOff(GREEN);
     ButtonAcknowledge(BUTTON3);
     LedOn(RED);
     for(u32Clear=0;u32Clear <= u32KeyLength;u32Clear++)
     {
       au32UserKey[u32Clear] = 0;
     }
     bLocked = TRUE;
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
