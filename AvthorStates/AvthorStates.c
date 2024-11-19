#include "AvthorStates.h"
#include "step.h"
// #include "../EUSART_Helpers/EUSART_Helpers.h"

//pointer info: https://www.geeksforgeeks.org/pointer-array-array-pointer/
//more: https://www.tutorialspoint.com/cprogramming/c_pointer_to_an_array.htm
// " Once you store the address of the first element in 'p', 
// you can access the array elements using *p, *(p+1), *(p+2) and so on. 
// *(balance + 4) is a legitimate way of accessing the data at balance[4]. "
//https://overiq.com/c-programming-101/pointers-as-structure-member-in-c/

volatile static avthorStates Avthor_States;
// static becasuse we only want to give access to getters/setters form outside

//AvthorStates init
void initAvthorStates(void){
   // setUartFunction(EUSART2_Write);
    Avthor_States.stepper_instruction = 0;
    Avthor_States.INTERRUPT_BREAK_FLAG = false;
    Avthor_States.motor_action = none;
    for (int i = 0; i < COMPLEX_INSTRUCTIONS_BREATHS_ARRAY_LENGTH; i++){
        Avthor_States.complex_breaths_instructions[i] = 0;
    }
    Avthor_States.complex_breaths_instruction_pointer = &Avthor_States.complex_breaths_instructions[0]; //setting address
    Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS = false;
    Avthor_States.bellowsHasBeenInchedUpOrDownSinceVenting = true; //set to true so the vent solenoid doesn't flicker on startup
}

//getters/setters - reduce global variables
/*  uint8_t get_UART_RX_IN(void){
    return UART_RX_IN;
} */
uint8_t get_stepper_instruction(void){
    return Avthor_States.stepper_instruction;
}
bool get_INTERRUPT_BREAK_FLAG(void){
    return Avthor_States.INTERRUPT_BREAK_FLAG;
}
motorAction get_motor_action(void){
    return Avthor_States.motor_action;
}
void reset_motor_action(void){
    Avthor_States.motor_action = none;
}
void reset_motor_action_if_matches(motorAction action){
    if(action == Avthor_States.motor_action){
        // this means that there is no new information about an action to take (ex: topButtonTrigger)
        // so it  can be safely reset
        reset_motor_action();
    }
}
uint8_t get_complex_breaths_instructions_index(uint8_t index){
    return Avthor_States.complex_breaths_instructions[index];
}
bool get_IS_RECEIVING_COMPLEX_BREATHS_COMMANDS(void){
    return Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS;
}
bool get_bellowsHasBeenInchedUpOrDownSinceVenting(void){
    return Avthor_States.bellowsHasBeenInchedUpOrDownSinceVenting;
}

void set_stepper_instruction(uint8_t in){
    Avthor_States.stepper_instruction = in;
}
void set_INTERRUPT_BREAK_FLAG(bool in){
    Avthor_States.INTERRUPT_BREAK_FLAG = in;
}
void set_IS_RECEIVING_COMPLEX_BREATHS_COMMANDS(bool in){
    Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS = in;
}

/* 
 *   single char commands - instantaneous:
 * 'a' 0x61 - start flow (turn on solenoid-fluid)
 * 'd' 0x64 - stop flow (turn off solenoid-fluid)
 * 'e' 0x65 - vent solenoid-vent
 * 'f' 0x66 - increment up - one time test method
 * 'g' 0x67 - increment down - one time test method
 * 'h' 0x68 - calibrate command
 * 'i' 0x69 - slowStepTest()
 ******** single char commands for breaths - repeating:
 * 'r' - stop breaths
 * 's' 0x73 - Breath 1
 * 't' 0x74 - Breath 2
 * 'u' 0x75 - Breath 3
 * 'v' 0x76 - Breath 4
 * 'w' 0x76 - Breath 5
 * 0xB0 - 0xC7 - timing testing breaths
 ********* complex controls:
 * 0xA9 - signal for complex breaths instructions
 *        0xA9, pressure/cmH20 (rotations), step size 1, ratio 1, step size 2, ratio 2, 0xA9
 * 'z' 0x7A - stop complex instruction actuation, also general stop
 * 
 */

//we do not have to wrap this in interrupt disables, this is only ever called from an interrupt
//*************** TODO: figure out if uint16_t pointer ++ incremets correctly *************************//
void Avthor_Uart_Instruction(uint8_t instr){
  //  write2bytesHex(instr);
  //  writeByte('\n');
    if(instr == 'z'){ //0x7A
        //Other than stepper motor location, puts the Avthor in a known state
        //this needs to be at the top so it can reset it after a complex character drop
        //STOP breaths and pump/solenoid = everything
        //stop calibration
        //exit UART data while loop, hang out in main while(1) loop until you get another UART command
        Avthor_States.stepper_instruction = 0x00; //this will cause the main switch statement to be useless until new UART RX info comes in
        Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
        Avthor_States.motor_action = none;
        Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS = false;
        stepperSleep(true); //stepper driver sleep - this is in the exit of all Step.c functions 
        EUSART_Write(READY_CMD); // send this in case the stop cmd was sent while calibrating or inching up/down
        // stop fluids in BGM
        return;
    }else if (instr == 'r') { //0x72
		//stop breaths
		Avthor_States.stepper_instruction = 0; //stops basic breaths in main()
		Avthor_States.motor_action = none; //stops complex breaths/all breath
		Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
        stepperSleep(true); //stepper driver sleep - this is in the exit of all Step.c functions 
        // vent is closed in BGM
        return;
    }else if (instr == TOP_BUTTON_HIT){
        // BGM is already set to notReady()
        Avthor_States.motor_action = topButtonTriggerAction;
        Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
        Avthor_States.stepper_instruction = 0;
        return;
    }else if(instr == BOTTOM_BUTTON_HIT){
        // BGM is already set to notReady()
        Avthor_States.motor_action = bottomButtonTriggerAction;
        Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
        Avthor_States.stepper_instruction = 0;
        return;
    }
    if((Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS) && (instr != COMPLEX_BREATHS_INSTRUCTION_CHAR)){
        *Avthor_States.complex_breaths_instruction_pointer = instr; //instr; //setting the value
        //*complex_instruction_pointer = UART_RX_IN;
        Avthor_States.complex_breaths_instruction_pointer++; //setting address
        //do not go through switch statement
    }else{
        //switch case for pump/instantaneous/stop commands - return to program execution afterwards 
        switch(instr){  
            case COMPLEX_BREATHS_INSTRUCTION_CHAR: { 
                //complex commands start/end
                if(!Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS){  //not receiving complex commands
                    Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS = true; //now receiving complex commands  
                    Avthor_States.complex_breaths_instruction_pointer = &Avthor_States.complex_breaths_instructions[0]; //resetting address for next set of complex commands
                }else{ //is receiving complex commands
                    Avthor_States.IS_RECEIVING_COMPLEX_BREATHS_COMMANDS = false; //now not receiving complex commands
                    //now tell the main loop to start executing complex breaths!
                    Avthor_States.motor_action = complexBreaths;
                }
                break;
            }case 'A': { 
                //this happens when BLE connection is broken - we are not ready for commands
                break;  
            }case UNLOCK_CMD_CHAR: { 
                // do nothing
                break; 
            //cases a through d are now available					 
            }case 'e': { //0x65
        		//vent solenoid for 5 sec
                Avthor_States.bellowsHasBeenInchedUpOrDownSinceVenting = false;
              /*  ventOpen();
                __delay_ms(5000); //vent solenoid to 1ATM
				ventClose();
				__delay_ms(500); //stability   */ // venting done in BGM
				//solenoid_old_SetLow();
				break;  
			}case 'j': { //0x6A - independent vent open
				Avthor_States.bellowsHasBeenInchedUpOrDownSinceVenting = false;
			/*	Avthor_States.VENT_IS_OPEN = true;
				ventOpen(); */ // venting done in BGM
				break;
			}case 'k': { //0x6B -  independent vent closed
			/*	Avthor_States.VENT_IS_OPEN = false;
				ventClose();  */ // venting done in BGM
				break;
			}case 'f': { //0x66
				//increment up - one time test method
				__delay_ms(10);
                Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
				Avthor_States.motor_action = inchUpAction;
				Avthor_States.bellowsHasBeenInchedUpOrDownSinceVenting = true;
				//do not need to stop complex commands, we can inch up then go back to them
				//need to move this to main() to let stop command interrupt it
				//Step(UP, (MOTOR_NUM_STEPS*20), Half ); //2 revolutions
				//stepper_instruction = 0x00;
				break;
			}case 'g': { //0x67
				//increment down - one time test method
				__delay_ms(10);
                Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
				Avthor_States.motor_action = inchDownAction;
				Avthor_States.bellowsHasBeenInchedUpOrDownSinceVenting = true;
				//do not need to stop complex commands, we can inch down then go back to them
				//need to move this to main() to let stop command interrupt it
				//Step(DOWN, (MOTOR_NUM_STEPS*20), Half); //2 revolutions
				//stepper_instruction = 0x00;
				break;
			}case 'h': { //0x68
				//calibrate command
                Avthor_States.motor_action = calibrating;
                Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
				Avthor_States.stepper_instruction = 0;
				break;
			}case 'i': { //0x69
				//slowStepTest();
                Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
				Avthor_States.motor_action = slowStepTestAction;
				break; 
            }case START_MOVE_UP_UNTIL_Z_CMD: { //birthing test
				//slowStepTest();
                Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
				Avthor_States.motor_action = moveUntilZUp;
				break;
			}case START_MOVE_DOWN_UNTIL_Z_CMD: { //birthing test
				//slowStepTest();
                Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
				Avthor_States.motor_action = moveUntilZDown;
				break;
			}default: { //for a stepper instruction
				Avthor_States.stepper_instruction = instr;
				Avthor_States.motor_action = stepperInstruction;
				Avthor_States.INTERRUPT_BREAK_FLAG = true; //we need to break to stop current step function
				__delay_ms(10);
				break;
			}
		}
	}
}
