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

u8 u8VolumeMessage[] = "VolumeLevel:";
u8 u8ChannelMessage[] = "Channel:";
static u8 u8ShowLevel[1] = "0";
u8 *u8VolumeLevel = u8ShowLevel;


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
  /*Eable the PA_16_BLADE_CS,PA_15_BLADE_SCK,PA_14_BLADE_MOSI,
  PA_13_BLADE_MISO,PA_12_BLADE_UPOMI,PA_11_BLADE_UPIMO
  PB_04_BLADE_AN1*/
  AT91C_BASE_PIOA->PIO_PER |= PA_16_BLADE_CS|PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_13_BLADE_MISO|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO;
  AT91C_BASE_PIOA->PIO_PDR &=~(PA_16_BLADE_CS|PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_13_BLADE_MISO|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO);
  AT91C_BASE_PIOA->PIO_OER  |= (PA_16_BLADE_CS|PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_13_BLADE_MISO|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO);
  AT91C_BASE_PIOA->PIO_ODR &=~ (PA_16_BLADE_CS|PA_15_BLADE_SCK|PA_14_BLADE_MOSI|PA_13_BLADE_MISO|PA_12_BLADE_UPOMI|PA_11_BLADE_UPIMO);

  AT91C_BASE_PIOB->PIO_PER    |= PB_04_BLADE_AN1|PB_03_BLADE_AN0;
  AT91C_BASE_PIOB->PIO_PDR    &=~(PB_04_BLADE_AN1|PB_03_BLADE_AN0);
  AT91C_BASE_PIOB->PIO_OER    |= PB_04_BLADE_AN1;
  AT91C_BASE_PIOB->PIO_ODR    &= ~PB_04_BLADE_AN1;
  
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR,u8VolumeMessage);
  LCDMessage(LINE2_START_ADDR,u8ChannelMessage);

  //MIC IN
  AT91C_BASE_PIOA->PIO_SODR |=PA_11_BLADE_UPIMO;
  AT91C_BASE_PIOA->PIO_CODR |=PA_12_BLADE_UPOMI;
  AT91C_BASE_PIOA->PIO_CODR |=PA_14_BLADE_MOSI;  
  
  u8 u8ChannelMic[] = "Mic";
  LCDMessage(LINE2_START_ADDR + 8,u8ChannelMic);
  LedOn(BLUE);
  
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
  static bool bAdc = FALSE;
  
  //B0 to up the volume
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    RedBlink();
    VolumeUp();
    
    //LCD Volume Level +1
    if (bAdc)
    {
      if (u8ShowLevel[0] < '9')
      {
        u8ShowLevel[0] = *u8VolumeLevel +1;
      }
      
      LCDClearChars(LINE1_START_ADDR + 12, 8);
      LCDMessage(LINE1_START_ADDR + 12,u8ShowLevel);
    }
    
  }
  
  //B1 to down the volume
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    RedBlink();
    VolumeDown();
    
    //LCD Volume Level -1
    if (bAdc)
    {
      if (u8ShowLevel[0] > '0')
      {
        u8ShowLevel[0] = *u8VolumeLevel -1;
      }
      
      
      LCDClearChars(LINE1_START_ADDR + 12, 8);
      LCDMessage(LINE1_START_ADDR + 12,u8ShowLevel);
    }
    
  }
   
  //Detect location,convert location information to volume level
  if (WasButtonPressed(BUTTON2))
  {
    ButtonAcknowledge(BUTTON2);
    UserApp1_StateMachine = Adc;
    RedBlink();
    bAdc = TRUE;
  }
  
  //B3 to change the channel in
  if (WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    RedBlink();
    ChangeChannel();
  }
  
} /* end UserApp1SM_Idle() */

static void Adc(void)
{
  static u32 u32Timedelay = 0;
  static u32 u32AdcResult = 0;
  static u8 u8AdcResultLevel = 0;
  static bool bStartConversion = TRUE;
  
  /*PA_14_BLADE_MOSI(C)=1,connect to 3.3v,PB_04_BLADE_AN1(RE)=1
  turn to channel ADC*/
  if (bStartConversion)
  {
    bStartConversion = FALSE;
    LedOn(WHITE);
    
    AT91C_BASE_PIOB->PIO_SODR |=PB_04_BLADE_AN1;
    AT91C_BASE_PIOA->PIO_SODR |=PA_14_BLADE_MOSI;

    //AN0 connect to ADC channel2,start ADC conversion
    Adc12StartConversion(ADC12_CH2);
  }
   
  if (Adc12StartConversion(ADC12_CH2) == TRUE)//end of conversion
  {
    u32AdcResult = AT91C_BASE_ADC12B->ADC12B_CDR[ADC12_CH2];
    u32AdcResult = u32AdcResult/41; // u32AdcResult/4096/100£»divided into 0-99
    u8AdcResultLevel = u32AdcResult/10; //divided into level 0-9
    u8ShowLevel[0] = u8AdcResultLevel+ '0' ;//number change to ascii
    
    u32Timedelay++;
    
    if (u32Timedelay > 250) //relay 250ms ,then show lcd and return before channel
    {
      u32Timedelay = 0;
      LedOff(WHITE);
      LCDClearChars(LINE1_START_ADDR + 12, 8);
      LCDMessage(LINE1_START_ADDR + 12,u8ShowLevel); 
      
      /*PA_14_BLADE_MOSI(C)=0,connect to A_OUT,PB_04_BLADE_AN1(RE) = 0*/
      AT91C_BASE_PIOB->PIO_CODR |=PB_04_BLADE_AN1;
      AT91C_BASE_PIOA->PIO_CODR |=PA_14_BLADE_MOSI;
      bStartConversion = TRUE;
      UserApp1_StateMachine = UserApp1SM_Idle;
    }
  }    
}

void RedBlink(void)
{
  LedOn(RED);
  
  for (u32 i=0;i<8000;i++)
  {
    
  }
  
  LedOff(RED);
}

void ChangeChannel(void)
{
  u8 u8ChannelMic[] = "Mic";
  u8 u8ChannelMobile[] = "Mobile";
  u8 u8ChannelSlience[] = "Slience";
  u8 u8CurrentChannel = 0;
  u8CurrentChannel = CurrentChannel();
  
  switch(u8CurrentChannel)
  {  
    case 1:
      LCDClearChars(LINE2_START_ADDR + 8, 12);
      LCDMessage(LINE2_START_ADDR + 8,u8ChannelMobile); 
        
      //change to channel moblie in,CD4053 A=0,B=0,C=0
      AT91C_BASE_PIOA->PIO_CODR |=PA_11_BLADE_UPIMO;
      AT91C_BASE_PIOA->PIO_CODR |=PA_12_BLADE_UPOMI;
      AT91C_BASE_PIOA->PIO_CODR |=PA_14_BLADE_MOSI;  
      
      LedOff(BLUE);
      LedOff(PURPLE);
      LedOn(GREEN);
      break;
      
    case 2:
      LCDClearChars(LINE2_START_ADDR + 8, 12);
      LCDMessage(LINE2_START_ADDR + 8,u8ChannelSlience);
        
      //change to slience,CD4053 A=0,B=1,C=0
      AT91C_BASE_PIOA->PIO_CODR |=PA_11_BLADE_UPIMO;
      AT91C_BASE_PIOA->PIO_SODR |=PA_12_BLADE_UPOMI;
      AT91C_BASE_PIOA->PIO_CODR |=PA_14_BLADE_MOSI;  
      
      LedOff(GREEN);
      LedOff(BLUE);
      LedOn(PURPLE);
      break;
      
    case 3:
      LCDClearChars(LINE2_START_ADDR + 8, 12);
      LCDMessage(LINE2_START_ADDR + 8,u8ChannelMic);
      
      //change to channel mic in,CD4053 A=1,B=0,C=0
      AT91C_BASE_PIOA->PIO_SODR |=PA_11_BLADE_UPIMO;
      AT91C_BASE_PIOA->PIO_CODR |=PA_12_BLADE_UPOMI;
      AT91C_BASE_PIOA->PIO_CODR |=PA_14_BLADE_MOSI;  
      
      LedOff(GREEN);
      LedOff(PURPLE);
      LedOn(BLUE);
      break;
  }
    
}

u8 CurrentChannel(void)
{
  u32 u32ChannelCheck = 0x00005800;
  u32 u32Channel1 = 0x0000800;//mic in,PA_11_BLADE_UPIMO(A)=1;
  //PA_12_BLADE_UPOMI(B)=0;PA_14_BLADE_MOSI(C)=0
  
  u32 u32Channel2 = 0x00000000;//mobile in,PA_11_BLADE_UPIMO(A)=0;
  //PA_12_BLADE_UPOMI(B)=0;PA_14_BLADE_MOSI(C)=0
  
  u32 u32Channel3 = 0x00001000;//silence ,PA_11_BLADE_UPIMO(A)=0;
  //PA_12_BLADE_UPOMI(B)=1;PA_14_BLADE_MOSI(C)=0
  
  u32 u32Channel4 = 0x00001800;//silence ,PA_11_BLADE_UPIMO(A)=1;
  //PA_12_BLADE_UPOMI(B)=1;PA_14_BLADE_MOSI(C)=0
  
  //Get the PA_14_BLADE_MOSI(C),PA_12_BLADE_UPOMI(B),PA_11_BLADE_UPIMO(A)
  u32ChannelCheck = AT91C_BASE_PIOA->PIO_ODSR & u32ChannelCheck;
  
  if (u32ChannelCheck == u32Channel1)
  {
    return 1;
  }
  
  if (u32ChannelCheck == u32Channel2)
  {
    return 2;
  }
  
  if (u32ChannelCheck == u32Channel3)
  {
    return 3;
  }
  
  if (u32ChannelCheck == u32Channel4)
  {
    return 3;
  }
  
  return 0;
}

void VolumeUp(void)
{
  AT91C_BASE_PIOA->PIO_CODR |= PA_16_BLADE_CS;// /CS=0
  AT91C_BASE_PIOA->PIO_SODR |= PA_15_BLADE_SCK;// U/D=1,Volume up
  
  for (u8 i=0;i<10;i++)  //increase one volume level
  {
    // /INC=1,then /INC=0,enable
    AT91C_BASE_PIOA->PIO_SODR |= PA_13_BLADE_MISO;
    for (u8 j=0;j<100;j++)
    {
      
    }
    AT91C_BASE_PIOA->PIO_CODR |= PA_13_BLADE_MISO;
  }
  
  AT91C_BASE_PIOA->PIO_SODR |= PA_16_BLADE_CS;// /CS=1
}

void VolumeDown(void)
{
  AT91C_BASE_PIOA->PIO_CODR |= PA_16_BLADE_CS;// /CS=0
  AT91C_BASE_PIOA->PIO_CODR |= PA_15_BLADE_SCK;// U/D=0,Volume down
  
  for (u8 i=0;i<10;i++) //decrease one volume level
  {
    // /INC=1,then /INC=0,enable
    AT91C_BASE_PIOA->PIO_SODR |= PA_13_BLADE_MISO;
    for (u8 j=0;j<100;j++)
    {
      
    }
    AT91C_BASE_PIOA->PIO_CODR |= PA_13_BLADE_MISO;
  }
  
  AT91C_BASE_PIOA->PIO_SODR |= PA_16_BLADE_CS;// /CS=1
}


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
