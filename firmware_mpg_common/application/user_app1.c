/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

Test1.
Test2 from Engenuics.
Test3.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */
static u8 au8Debug[20];

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
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  static u8 u8ShowMessage1[] = "\n\rLED Programming Interface\n\rPress 1 to program LED command sequence\n\rPress 2 to show current USER program\n\r";
  DebugPrintf(u8ShowMessage1);
  
    /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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
/* Wait for input */
static void UserApp1SM_Idle(void)
{
  static u8 u8Chars = 0;
  static u8 u8I = 0 ;
  static u8 u8Check = 0;
  static bool bInput = FALSE;
  static bool bColorBit = FALSE;
  static u8 u8List = 0; /*To counter the number of item */
  static u8 u8Show = 0;/*for LedDisplayPrintListLine()*/
  static u8 u8ShowMessage1[] = "\n\rLED Programming Interface\n\rPress 1 to program LED command sequence\n\rPress 2 to show current USER program\n\r";
  static u8 u8ShowMessage2[] = "\n\rInvalid command\n\r";
  static u8 u8ShowMessage3[] = "\n\rEnter commands as LED-ONTIME-OFFTIME and press Enter\n\rTime is in milliseconds, max 100 commands";    
  static u8 u8ShowMessage4[] = "\n\rLED colors: R(r),O(o),Y(y),G(g),C(c),B(b),P(p),W(w)\n\rExample: R-100-200 (RED on at 100ms and off at 200ms)\n\rPress Enter on blank line to end\n\r";
  static u8 u8ShowMessage5[] = "\n\rLED   ON TIME   OFF TIME\n\r-----------------------------------\n\r";
  static u8 u8ShowMessage6[] = "\n\rCommand Entry complete\n\rCommand entered:";
  static u8 u8FalseMessage[]= "\n\rInvalid command: incorrrect format. Please use L-ONTIME-OFFTIME\n\r";
  static bool bSecond = FALSE;
  static u8 u8State = 0;
  static LedCommandType T1;
 
  if (u8State == 0)
  {/*to choose mode ;'1': program LED command sequence;'2':show current USER program*/
    u8Chars = DebugScanf(au8Debug);
    au8Debug[u8Chars] = '\0';
    if (u8Chars==1)
    {
      if (au8Debug[0]== 0x31)
      {/*mode '1'*/
        u8List = 0;
        LedDisplayStartList();
        DebugLineFeed();
        u8State = 1;
      }
      else if (au8Debug[0]== 0x32)
      {/*mode '2'*/
        DebugLineFeed();
        u8State = 4;
        DebugPrintf(u8ShowMessage6);
        DebugPrintNumber(u8List);
        DebugLineFeed();
        DebugPrintf(u8ShowMessage5);
      }
      else 
      {
        DebugPrintf(u8ShowMessage2);
        DebugPrintf(u8ShowMessage1);
        u8State = 0;
      }
    }
  }
  if (u8State == 1)
  {
    DebugPrintf(u8ShowMessage3);
    DebugPrintf(u8ShowMessage4);
    u8State = 2;
  }
  if (u8State==2)
  {/*input command*/
    if (G_au8DebugScanfBuffer[u8I] != 0x0D && G_au8DebugScanfBuffer[u8I] !='\0')
    {
      u8I++;
    }
    if (G_au8DebugScanfBuffer[u8I] == 0x0D && u8I >0)
    {
      u8I = 0;
      u8State = 3;
      u8Chars = DebugScanf(au8Debug);
      au8Debug[u8Chars] = '\0';
    }
    if (G_au8DebugScanfBuffer[0] == 0x0D)
    {
      u8Chars = DebugScanf(au8Debug);
      au8Debug[u8Chars] = '\0';
      u8State = 4;
      DebugPrintf(u8ShowMessage6);
      DebugPrintNumber(u8List);
      DebugLineFeed();
      DebugPrintf(u8ShowMessage5);
    }
   }
  if (u8State == 3)
  { /*to check whether the command is right
    if command is right , LedDisplayAddCommand()
    if command is wrong , give a message*/
    bInput = FALSE;
    bColorBit = FALSE;
    bSecond = FALSE;
    u8Check = 0;
    bColorBit = ColorBit(au8Debug);
    bSecond = Second(au8Debug);
    u8State = 2;
    if (bColorBit)
    {
      u8Check++; 
    }
    if (au8Debug[1] == 0x2D)
    {
      u8Check++;
    }
    if (bSecond)
    {
      u8Check++;
    }
    if (u8Check == 3)
    {
      bInput = TRUE;
    }
    if (bInput)
    {
      DebugLineFeed();
      u8State = 2;
      T1.eLED = Color(au8Debug);
      T1.u32Time = StartTime(au8Debug);
      T1.bOn = TRUE;
      T1.eCurrentRate = LED_PWM_100;
      LedDisplayAddCommand(USER_LIST , &T1);

      T1.eLED = Color(au8Debug);
      T1.u32Time = EndTime(au8Debug);
      T1.bOn = FALSE;
      T1.eCurrentRate = LED_PWM_0;
      LedDisplayAddCommand(USER_LIST , &T1);
      u8List++;
    }
    if (bInput == FALSE)
    {
      DebugPrintf(u8FalseMessage);
      u8State = 2;
    }
  }
  if (u8State == 4)
  {/* show current USER program */
    for (u8Show=0;u8Show<u8List;u8Show++)
    {
      LedDisplayPrintListLine(u8Show);
    }
    if (u8Show == u8List)
    {
      u8State = 5;
    }
  }
  if (u8State ==5)
  {
    DebugPrintf(u8ShowMessage1);
    u8State = 0;
  }
 
  
  
} /* end UserApp1SM_Idle() */
                      
u8 Color(u8 *au8Array)
{/*to find what color have input*/
  static u8 au8Color1[] = {'W','P','B','C','G','Y','O','R'};
  static u8 au8Color2[] = {'w','p','b','c','g','y','o','r'};
  static LedNumberType au8Color3[] = {WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED,};
  u8 u8Color1 = 0;
  u8 u8Color2 = 0;
  for (u8Color1=0;u8Color1<8;u8Color1++)
  {
    if (au8Debug[0] == au8Color1[u8Color1])
    {
      u8Color2 = u8Color1;
    }
  }
  for (u8Color1=0;u8Color1<8;u8Color1++)
  {
    if (au8Debug[0] == au8Color2[u8Color1])
    {
      u8Color2 = u8Color1;
    }
  }
  return (au8Color3[u8Color2]);
}
u32 StartTime (u8 *auArray)
{/*to figure out starttime*/
  static u8 u8Check1 = 2;
  static u8 u8Check2 = 0;
  u32 u32StartTime = 0;
  
  for(u8Check1=2;au8Debug[u8Check1]!=0x2D;u8Check1++)
  {
    u8Check2 = u8Check1+1;
  }
  for (u8Check1=2;u8Check1<u8Check2;u8Check1++)
  {
    u32StartTime =u32StartTime*10+(au8Debug[u8Check1]-'0');
  }
  return (u32StartTime);
}
bool ColorBit(u8 *auArray)
{/*to judge whether the first one is a color*/
  static u8 au8Color[] = {'W','P','B','C','G','Y','O','R','w','p','b','c','g','y','o','r'};
  static u8 u8Check = 0;
  bool bColorBit = FALSE;
  for(u8Check=0;u8Check<16;u8Check++)
  {
    if(au8Debug[0] == au8Color[u8Check])
    {
      bColorBit = TRUE;
    }
  }
  return (bColorBit);
}
bool Second(u8 *au8Array)
{/*to judge whether have two '-'*/
  static u8 u8Check = 2;
  static bool bSecond = FALSE;
  
  for(u8Check=2;au8Debug[u8Check]!='\0';u8Check++)
  {
    if (au8Debug[u8Check] == 0x2D)
    {
      bSecond = TRUE;
    }
  }
  return (bSecond);
}
u32 EndTime (u8 *auArray)
{/*to figure out the endtime*/
  u32 u32EndTime = 0;
  static u8 u8Check1 = 2;
  static u8 u8Check2 = 0;
  static u8 u8Check3 = 0;
  static u8 u8Check4 = 0;
  static u8 u8Check5 = 0;
  
  for(u8Check4=0;u8Check4<20;u8Check4++)
  {
    if (au8Debug[u8Check4] == 0x0D)
    {
      u8Check5= u8Check4 -1;
    }
  }
  for(u8Check1=2;au8Debug[u8Check1]!=0x2D;u8Check1++)
  {
    u8Check2 = u8Check1+1;
  }
  
  for (u8Check3=u8Check2+1;u8Check3<=u8Check5;u8Check3++)
  {
    u32EndTime =u32EndTime*10+(au8Debug[u8Check3]-'0');
  }
  return (u32EndTime);
}
          
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
