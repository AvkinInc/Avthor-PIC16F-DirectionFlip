// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef AVTHOR_STATES_H
#define	AVTHOR_STATES_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "mcc.h"
#include <stdbool.h>
#include <stdint.h>

//constant chars
#define UNLOCK_CMD_CHAR 0xEE
#define COMPLEX_BREATHS_INSTRUCTION_CHAR 0xA9
#define COMPLEX_FLUIDS_INSTRUCTION_CHAR 0xAA
#define HAPTIC_COMMAND_CHAR 0xBB // send this than another char over UART to BLE, the second char is a command for a connected Avband
#define TOP_BUTTON_HIT 0xFC
#define BOTTOM_BUTTON_HIT 0xFD

#define START_MOVE_UP_UNTIL_Z_CMD 0xE6 //birthing test
#define START_MOVE_DOWN_UNTIL_Z_CMD 0xE7 //birthing test

#define NOT_READY_CMD 0xFA
#define READY_CMD 0xFB
#define VENT_THEN_READY_CMD 0xFE // this command tells the BGM to vent the solenoid for 2500ms then call Ready()
#define SMALL_VENT_CMD 0xF9

#define COMPLEX_INSTRUCTIONS_BREATHS_ARRAY_LENGTH 5
// amplitude (small/1, med/2, large/4), first_step_size, first_ratio, second_step_size, second_ratio

// other than complex breaths and stepperIntruction, 
// these are all one-shot actions for the motor
// so after the action is done we reset the motorAction to none
typedef enum {
    none = 0x1,
    complexBreaths = 0x2, // see Avthor_States.complex_breaths_instructions --> this is for a repeating breath
    calibrating = 0x3,
    inchUpAction = 0x4, 
    inchDownAction = 0x5, 
    topButtonTriggerAction = 0x6, 
    bottomButtonTriggerAction = 0x7, 
    stepperInstruction = 0x8, // see Avthor_States.stepper_instruction --> this is for a repeating breath
    slowStepTestAction = 0x9, 
    moveUntilZUp = 0xA,
    moveUntilZDown = 0xB
}motorAction;

typedef volatile struct{
    uint8_t stepper_instruction; //holds the char for the latest breath instruction (s through w), perhaps the test ones here too (0xB1 - 0xDC)
    bool INTERRUPT_BREAK_FLAG;
    motorAction motor_action;
    char complex_breaths_instructions[COMPLEX_INSTRUCTIONS_BREATHS_ARRAY_LENGTH];
    char *complex_breaths_instruction_pointer; //this one moves
    bool IS_RECEIVING_COMPLEX_BREATHS_COMMANDS;
    bool bellowsHasBeenInchedUpOrDownSinceVenting; //kek
}avthorStates;

void sendSoundboardDataToBle(void);
void initAvthorStates(void);
//getters/setters for Avthor States - reduce global variables
uint8_t get_UART_RX_IN(void);
uint8_t get_stepper_instruction(void);
motorAction get_motor_action(void);
void reset_motor_action(void);
void reset_motor_action_if_matches(motorAction action);
bool get_INTERRUPT_BREAK_FLAG(void);
uint8_t get_complex_breaths_instructions_index(uint8_t index);
bool get_IS_RECEIVING_COMPLEX_BREATHS_COMMANDS(void);
bool get_bellowsHasBeenInchedUpOrDownSinceVenting(void);
void set_stepper_instruction(uint8_t in);
void set_INTERRUPT_BREAK_FLAG(bool in);
void set_IS_RECEIVING_COMPLEX_BREATHS_COMMANDS(bool in);
void Avthor_Uart_Instruction(uint8_t instr);

#endif /*AVTHOR_STATES_H*/