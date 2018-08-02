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

extern u8 au8ChineseWord[][LINE_MAX_NUMBER*2];
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 au8ChineseArray[LINE_MAX_NUMBER][Word_NUMBER*2]; 

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
 
  GetChineseData();
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
  static u16 u16TimeCounter = 0;
  static u8 u8TopDigit = 0;
  u16TimeCounter++; 
  
  Screen_Display();
  
  if(u16TimeCounter == 100)
  {
	u16TimeCounter = 0;
	for(u8 i=0;i<LINE_MAX_NUMBER;i++)
	{
	  u8TopDigit = au8ChineseArray[i][0]  & 0x80; //取第一个字节的最高位
	  for(u8 j=0;j<Word_NUMBER*2;j++)
	  {
		if(j < Word_NUMBER*2-1)
		{
		  au8ChineseArray[i][j] = au8ChineseArray[i][j]<<1; //取到的字节左移一位
		  if((au8ChineseArray[i][j+1] & 0x80) == 0x80)//如果下一个字节的最高位为1
		  {
			au8ChineseArray[i][j] = (au8ChineseArray[i][j] & 0xFE) | 0x01;  //左移一位后的数据或上0x01
		 }
		}
		else
		{
		  au8ChineseArray[i][j] = au8ChineseArray[i][j]<<1;
		  if ((u8TopDigit & 0x80) == 0x80)
		  {
			au8ChineseArray[i][j] = (au8ChineseArray[i][j] & 0xFE) | 0x01;
		  }
		}
	  }
	}
  }
   
} /* end UserApp1SM_Idle() */

/*字符显示*/
static void Screen_Display(void)
{
  static u8 u8LineNumber = 0;
  
  Clear_One_Line(); //清除一行的数据
  
  /*选择行数*/
  Line_Choose_Control(u8LineNumber);
  
  Display_One_Line(u8LineNumber);//显示这一行的数据
  
  u8LineNumber++;
  if(u8LineNumber == LINE_MAX_NUMBER)
  {
	u8LineNumber = 0;//返回第一行
  }
  
}
 
/*发送一行数据*/
static void Send_One_Line_Data(u8* au8DataArray)
{
  static u8 u8Data = 0;
  
  AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;//LE-----1
  AT91C_BASE_PIOA->PIO_SODR = PA_11_BLADE_UPIMO;//OE-----1
  
  for(u8 j=0;j<ROW_MAX_NUMBER;j++)//循环十个字节
  {
	u8Data = *(au8DataArray + ROW_MAX_NUMBER-1-j);//获取其中一个字节的数据
	
	for(u8 i=0;i<8;i++)//每个字节循环八位
	{
	  Row_Choose_Control(u8Data);//SDI输入数据
	  u8Data = u8Data >> 1;//数据右移一位
	  
	  AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;//CLK-------0
      AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;//CLK-------1
	}
  }
}

/*显示一行数据*/
static void Display_One_Line(u8 u8LineNumber)
{
  Send_One_Line_Data(*(au8ChineseArray+u8LineNumber));//发送汉字数据
 
  AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;//OE-----0
}

/*清除一行的显示*/
static void Clear_One_Line(void)
{
  static u8 au8ClearArray[ROW_MAX_NUMBER];
  
  /*初始化一行清除数据*/
  for(u8 i=0;i<ROW_MAX_NUMBER;i++)
  {
	*(au8ClearArray + i) = 0x00;
  }
  Send_One_Line_Data(au8ClearArray);//发送清除数据
}

/*获取中文字模数据*/
static void GetChineseData(void)
{
  static u8 u8ChineseLineIndex = 0;
  
  /*将中文数据 au8ChineseWord 的每一行的两列放到au8ChineseArray的两列中*/
  for(u8 j=0;j<(Word_NUMBER);j++)
  {
	for(u8 i=0;i<LINE_MAX_NUMBER;i++)
	{
	  au8ChineseArray[i][2*j-2*u8ChineseLineIndex] = au8ChineseWord[j][2*i];
	  au8ChineseArray[i][2*j+1-2*u8ChineseLineIndex] = au8ChineseWord[j][2*i+1];
	}
  }
}

/*检查每个字节的最低位来控制SDI的高低电平*/
static void Row_Choose_Control(u8 u8Data)
{
  if((u8Data & 0x01) == 0x01)
  {
	AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;//SDI------1
  }
  else
  {
	AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;//SDI------0
  }
  
}

/*控制　A　B　C　D　的高低电平*/
static void Line_Choose_Control(u8 u8LineNumber)
{
  AT91C_BASE_PIOA->PIO_SODR = PA_03_HSMCI_MCCK;//STB-----1   
  AT91C_BASE_PIOA->PIO_CODR = PA_04_HSMCI_MCCDA;//INH----0
  
  if((u8LineNumber & 0x1) == 0x1)
  {
	AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0;//A-----1
  }
  else
  {
	AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0;//A-----0
  }
  
  if((u8LineNumber & 0x2) == 0x2)
  {
	AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1;//B-----1   
  }
  else
  {
	AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1;//B-----0   
  }
  
  if((u8LineNumber & 0x4) == 0x4)
  {
	AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2;//C-----1
  }
  else
  {
	AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2;//C-----0
  }
  
  if((u8LineNumber & 0x8) == 0x8)
  {
	AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3;//D-----1
  }
  else
  {
	AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3;//D-----0
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
