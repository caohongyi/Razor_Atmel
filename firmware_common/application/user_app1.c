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

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout = 0;                    /* Timeout counter used across states */
static u8 au8HRDisplayMessage1[] = "Welcome to use!";
static u8 au8HRDisplayMessage2[] = "Current HR: 000 bpm ";
static u8 au8BatteryDisplayMessage[] = "Battery Level: 000% ";
static u8 au8BatteryStatusMessage[] = "Battery Status: ";

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
  
  
  AntAssignChannelInfoType sAntSetupData;
  
 
  
 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  sAntSetupData.AntNetworkKey[0] = 0xB9;
  sAntSetupData.AntNetworkKey[1] = 0xA5;
  sAntSetupData.AntNetworkKey[2] = 0x21;
  sAntSetupData.AntNetworkKey[3] = 0xFB;
  sAntSetupData.AntNetworkKey[4] = 0xBD;
  sAntSetupData.AntNetworkKey[5] = 0x72;
  sAntSetupData.AntNetworkKey[6] = 0xC3;
  sAntSetupData.AntNetworkKey[7] = 0x45;
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel is configured, so change LED to yellow */
    LedOff(RED);
    LedOn(YELLOW);
    UserApp1_StateMachine = UserApp1SM_AntChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    LedBlink(RED, LED_4HZ);
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
static void UserApp1SM_AntChannelAssign()
{
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    /* Channel assignment is successful, so open channel and
    proceed to Idle state */
    
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE))
  {
    
    UserApp1_StateMachine = UserApp1SM_Error;    
  }
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
   /*look for Button0 to open channel*/
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    LedOff(YELLOW);
    LedOn(GREEN);
    
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    
    
    
     /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
  }
} /* end UserApp1SM_Idle() */
    
static void UserApp1SM_WaitChannelOpen()
{
  
  
  /* Monitor the channel status to check if channel is opened */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
  {
    LCDCommand(LCD_CLEAR_CMD);
    
    LCDMessage(LINE1_START_ADDR,au8HRDisplayMessage1); 
    LCDMessage(LINE2_START_ADDR,au8HRDisplayMessage2);
    UserApp1_StateMachine = UserApp1SM_ChannelOpen;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
}

static void UserApp1SM_ChannelOpen()
{
  static u8 au8TestMessage[]={0x46,0xFF,0xFF,0xFF,
                              0xFF,0x80,0x07,0x01};
  static u8 au8TestMessage1[]={0x4C,0xFF,0xFF,0xFF,
                               0xFF,0xFF,0xFF,0x00}; 
    
  static bool bGotNewData = FALSE;
  static u8 u8CurrentStatus = HEART_RATE_STATUS;
  static u8 au8String1[] = "NEW";
  static u8 au8String2[] = "GOOD";
  static u8 au8String3[] = "OK";
  static u8 au8String4[] = "LOW";
  static u8 au8String5[] = "CRI";
  static u8 au8String6[] = "!!!";
  static u8 au8ModeSelectionString1[]="Running";
  static u8 au8ModeSelectionString2[]="Cycling";
  static u8 au8ModeSelectionString3[]="Swimming";
  
  
  
  
 
    if ( AntReadAppMessageBuffer() )
    {
      
      /* New data message: check what it is */
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {
        //if(G_au8AntApiCurrentMessageBytes[7] != NULL)
        //{
        
        if(u8CurrentStatus == HEART_RATE_STATUS)
        {
          bGotNewData = TRUE;
          au8HRDisplayMessage2[12] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[7] / 100);
          au8HRDisplayMessage2[13] = HexToASCIICharUpper((G_au8AntApiCurrentMessageBytes[7] / 10)%10);
          au8HRDisplayMessage2[14] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[7] %10);
          //}
          
          if(bGotNewData)
          {
            LCDMessage(LINE2_START_ADDR,au8HRDisplayMessage1);
            LCDMessage(LINE2_START_ADDR,au8HRDisplayMessage2); 
            bGotNewData = FALSE;
          }
        }
        
        else if (u8CurrentStatus == BATTERY_LEVEL_STATUS)
        {
          AntQueueAcknowledgedMessage(ANT_CHANNEL_USERAPP, au8TestMessage);
          if(G_au8AntApiCurrentMessageBytes[0] ==0x07)
          {
            au8BatteryDisplayMessage[15] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[1] / 100);
            au8BatteryDisplayMessage[16] = HexToASCIICharUpper((G_au8AntApiCurrentMessageBytes[1] / 10)%10);
            au8BatteryDisplayMessage[17] = HexToASCIICharUpper(G_au8AntApiCurrentMessageBytes[1] %10);
            
             if((G_au8AntApiCurrentMessageBytes[3] >>4) == 0x01)
             {
              LCDMessage(LINE1_START_ADDR,au8BatteryStatusMessage);
              LCDMessage(LINE1_START_ADDR+LEVEL_DIS_PLACE,au8String1);
             }
               
             else if ((G_au8AntApiCurrentMessageBytes[3] >>4) == 0x02)
             {
               LCDMessage(LINE1_START_ADDR,au8BatteryStatusMessage);
               LCDMessage(LINE1_START_ADDR+LEVEL_DIS_PLACE,au8String2);
             }
             
             else if ((G_au8AntApiCurrentMessageBytes[3] >>4) == 0x03)
             {
               LCDMessage(LINE1_START_ADDR,au8BatteryStatusMessage);
               LCDMessage(LINE1_START_ADDR+LEVEL_DIS_PLACE,au8String3);
             }
              
              else if ((G_au8AntApiCurrentMessageBytes[3] >>4) == 0x04)
             {
               LCDMessage(LINE1_START_ADDR,au8BatteryStatusMessage);
               LCDMessage(LINE1_START_ADDR+LEVEL_DIS_PLACE,au8String4);
             }
             
              else if ((G_au8AntApiCurrentMessageBytes[3] >>4) == 0x05)
             {
               LCDMessage(LINE1_START_ADDR,au8BatteryStatusMessage);
               LCDMessage(LINE1_START_ADDR+LEVEL_DIS_PLACE,au8String5);
             }
             
             else
             {
                LCDMessage(LINE1_START_ADDR,au8BatteryStatusMessage);
                LCDMessage(LINE1_START_ADDR+LEVEL_DIS_PLACE,au8String6);
             }
            
           
            LCDMessage(LINE2_START_ADDR,au8BatteryDisplayMessage);
         }
         
        }
        
        else
        {
          AntQueueAcknowledgedMessage(ANT_CHANNEL_USERAPP, au8TestMessage1);
           if(G_au8AntApiCurrentMessageBytes[7] ==0x01)
          {
            LCDMessage(LINE1_START_ADDR,au8ModeSelectionString1);
          }
          
            else if(G_au8AntApiCurrentMessageBytes[7] ==0x02)
          {
            LCDMessage(LINE1_START_ADDR,au8ModeSelectionString2);
          }
          
            else if(G_au8AntApiCurrentMessageBytes[7] ==0x05)
          {
            LCDMessage(LINE1_START_ADDR,au8ModeSelectionString3);
          }
          
          else
          {
            
          }
          
        }
      }
      
      else if(G_eAntApiCurrentMessageClass == ANT_TICK)
      {
       
      } /* end else if(G_eAntApiCurrentMessageClass == ANT_TICK) */
    }/* end AntReadAppMessageBuffer() */ 
    
    
    /*Press BUTTON1 to enter  the mode of viewing power*/
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      u8CurrentStatus = BATTERY_LEVEL_STATUS;
      LCDCommand(LCD_CLEAR_CMD);
      //LCDMessage(LINE2_START_ADDR,au8BatteryDisplayMessage);
    }
    
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      u8CurrentStatus = MODE_SELECTION_STATUS;
      LCDCommand(LCD_CLEAR_CMD);
    }
    
      
   
   /* Look for BUTTON 1 to close channel */
  if(WasButtonPressed(BUTTON3))
  {
    ButtonAcknowledge(BUTTON3);
    AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
    u8CurrentStatus = HEART_RATE_STATUS;
    
    /* Set timer and advance states */
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_WaitChannelClose;
  }

}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_WaitChannelClose()
{
  /* Monitor the channel status to check if channel is closed */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    

    UserApp1_StateMachine =  UserApp1SM_Idle;
  }
  
  /* Check for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, TIMEOUT_VALUE) )
  {
    

   UserApp1_StateMachine = UserApp1SM_Error;    
  }
  
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
