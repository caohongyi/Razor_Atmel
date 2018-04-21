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

extern u32 G_u32AntApiCurrentDataTimeStamp;                       
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;   
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];
extern AntExtendedDataType G_sAntApiCurrentMessageExtData; 
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static AntAssignChannelInfoType sAntSetupData;
static bool bChannelIsMaster = FALSE;
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
  u8 au8WelcomeMessage[] = "Hide and Go Seek!";
  u8 au8Instructions[] = "Press B0 to Start";
  
  
  /* Clear screen and place start messages */
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8WelcomeMessage); 
  LCDMessage(LINE2_START_ADDR, au8Instructions); 

  
  
 /* Configure ANT for this application */
  
  if(bChannelIsMaster)
  {
    sAntSetupData.AntChannel          = ANT_CHANNEL_MASTER;
    sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_MASTER;
    sAntSetupData.AntDeviceIdLo       = ANT_DEVICE0ID_LO_USERAPP;
    sAntSetupData.AntDeviceIdHi       = ANT_DEVICE0ID_HI_USERAPP;
  }
  else
  {
    sAntSetupData.AntChannel          = ANT_CHANNEL_SLAVE;
    sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_SLAVE;
    sAntSetupData.AntDeviceIdLo       = ANT_DEVICE1ID_LO_USERAPP;
    sAntSetupData.AntDeviceIdHi       = ANT_DEVICE1ID_HI_USERAPP;
  }
    
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel is configured */
    
    UserApp1_StateMachine = UserApp1SM_AntChannelAssign;
  }
  else
  {
    
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
/* Wait for assignment of channel */
static void UserApp1SM_AntChannelAssign(void)
{
   
  if(AntRadioStatusChannel(sAntSetupData.AntChannel) == ANT_CONFIGURED)
  {
    /* if channel configure succfully,open channel*/
    if(bChannelIsMaster)
    {
      UserApp1_StateMachine = UserApp1SM_Idle;
    }
    else
    {
      UserApp1_StateMachine =  UserApp1Initialize;
      bChannelIsMaster = TRUE;
    }
    
   
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    
    UserApp1_StateMachine = UserApp1SM_Error;
  }
}

static void UserApp1SM_Idle(void)
{  
 
  /* Look for BUTTON 0 to open channel */
  if(WasButtonPressed(BUTTON0))
  {
    
    ButtonAcknowledge(BUTTON0);
    
   
    AntOpenChannelNumber(sAntSetupData.AntChannel);

   
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserAppSM_WaitChannelOpen;
  }

} /* end UserApp1SM_Idle() */
    
static void UserAppSM_WaitChannelOpen(void)
{
  static u8 au8MasterMessage1[]="Hide!";
  static u8 au8MasterMessage2[]="I am hiding";
  static u8 au8SlaveMessage1[]="Seeker!";
  static u8 au8SlaveMessage2[]="He is hiding:10s";
  if(AntRadioStatusChannel(sAntSetupData.AntChannel) == ANT_OPEN)
  {
    if(bChannelIsMaster)
    {
      
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,au8MasterMessage1); 
      LCDMessage(LINE2_START_ADDR,au8MasterMessage2); 
      UserApp1_StateMachine = UserAppSM_ChannelMasterOpen;
    }
    else
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR,au8SlaveMessage1); 
      LCDMessage(LINE2_START_ADDR,au8SlaveMessage2); 
      UserApp1_StateMachine = UserAppSM_ChannelSlaveOpen;
    }
    
  }
} /* end UserAppSM_WaitChannelOpen() */

static void UserAppSM_ChannelMasterOpen()
{
  static u8 au8TestMessage[] = {0, 0, 0, 0, 0xA5, 0, 0, 0};
  static u8 au8MasterMessage3[]="Hide Ok?";
  static bool bSearching = FALSE;
  
   if( AntReadAppMessageBuffer() )
  {
     /* New data message: check what it is */
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      if(G_au8AntApiCurrentMessageBytes[7] == KEY_PARAMETER)
      {
        LCDClearChars(LINE2_START_ADDR, 20); 
        LCDMessage(LINE2_START_ADDR,au8MasterMessage3);
        LCDMessage(LINE2_START_ADDR + 10,"(B1 Ok)");
      }
      
      if(IsButtonPressed(BUTTON1))
      {
        bSearching = TRUE;
        au8TestMessage[7] = KEY_PARAMETER;
        LCDClearChars(LINE2_START_ADDR, 20); 
      }
      
      if(G_au8AntApiCurrentMessageBytes[6] == KEY_PARAMETER
         && bSearching)
      {
        LCDMessage(LINE2_START_ADDR,"He is searching me");
      }
      
      if(G_au8AntApiCurrentMessageBytes[5] == KEY_PARAMETER)
      {
        LCDClearChars(LINE2_START_ADDR, 20);
        au8TestMessage[5] = KEY_PARAMETER;
        bSearching = FALSE;
        LCDMessage(LINE2_START_ADDR,"Found me!(B2 quit)");
        UserApp1_StateMachine = UserAppSM_EndSearching;
      }
      AntQueueBroadcastMessage(sAntSetupData.AntChannel,au8TestMessage);
    } /* end if(G_eAntApiCurrentMessageClass == ANT_DATA) */

    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
     
    } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
  } /* end AntReadAppMessageBuffer() */  
}

static void UserAppSM_EndSearching(void)
{
  
}

static void UserAppSM_ChannelSlaveOpen(void)
{
  
}
static void UserAppSM_WaitChannelClose(void)
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_MASTER) == ANT_CLOSED)
  {
    LedOff(GREEN);
    LedOn(YELLOW);

    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    LedOff(GREEN);
    LedOff(YELLOW);
    LedBlink(RED, LED_4HZ);

    UserApp1_StateMachine = UserApp1SM_Error;
  }
  
  
    
} /* end UserAppSM_WaitChannelClose() */



/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
