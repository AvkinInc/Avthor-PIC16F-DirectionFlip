/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1454
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "./step/step.h" //step() and calibrate() are here
#include "./AvthorStates/AvthorStates.h"

#define PIC16F_SW_VERSION_NUMBER 31 // record keeping

#define INTERRUPT_GlobalInterruptEnable() (INTCONbits.GIE = 1)
#define INTERRUPT_GlobalInterruptDisable() (INTCONbits.GIE = 0)
#define INTERRUPT_PeripheralInterruptEnable() (INTCONbits.PEIE = 1)
#define INTERRUPT_PeripheralInterruptDisable() (INTCONbits.PEIE = 0)

volatile uint8_t UART_RX_in = 0;
uint8_t breath_counter_for_smallVent = 0;

void smallVent(void);
#define DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE 10 //ms, used to be 50
void basicStepperInstruction(char charIn);
void eighthsTest(char charIn);

void __interrupt() INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if(INTCONbits.PEIE == 1)
    {
        if(PIE1bits.RCIE == 1 && EUSART_is_rx_ready())
        {
            UART_RX_in = EUSART_Read(); // read buffer
            PIR1bits.RCIF = 0; // reset flag
            Avthor_Uart_Instruction(UART_RX_in);
        } 
        else
        {
            //Unhandled Interrupt
        }
    }      
    else
    {
        //Unhandled Interrupt
    }
}

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    initAvthorStates();
    
    stepperSleep(true); //stepper sleep - stepper sleep/wake is managed in step.c
    
    // init RX interrupts
    PIE1bits.RCIE = 1; // enable 
    PIR1bits.RCIF = 0; // reset flag

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    while (1)
    {
        switch(get_motor_action()){
            //run the functions that used to be within the interrupt() 
            case topButtonTriggerAction: {
               //  EUSART_Write(NOT_READY_CMD); --> BGM is already not ready
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                topButtonTrigger();
                reset_motor_action();
                EUSART_Write(VENT_THEN_READY_CMD);
                break;
            }case bottomButtonTriggerAction: {
                //  EUSART_Write(NOT_READY_CMD); --> BGM is already not ready
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                bottomButtonTrigger();
                reset_motor_action();
                EUSART_Write(VENT_THEN_READY_CMD);
                break;
            }case calibrating: {
                EUSART_Write(NOT_READY_CMD);
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                calibrate();
                reset_motor_action_if_matches(calibrating);
             //   EUSART_Write(VENT_THEN_READY_CMD); // Ready() is called after the 2500ms venting is done
                // ^^ we dont need this because calibrate() will trigger on of the limit switches, an nd they call this
                break;
            }case moveUntilZUp: {
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                moveUntilZ(UP);
                reset_motor_action_if_matches(moveUntilZUp);
                break;
            }case moveUntilZDown: {
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                moveUntilZ(DOWN);
                reset_motor_action_if_matches(moveUntilZDown);
                break;
            }case slowStepTestAction: {
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                slowStepTest();
                reset_motor_action_if_matches(slowStepTestAction);
                break;
            }case inchUpAction: {
                EUSART_Write(NOT_READY_CMD);
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                inchUp();
                reset_motor_action_if_matches(inchUpAction);
                EUSART_Write(READY_CMD);
                break;
            }case inchDownAction: {
                EUSART_Write(NOT_READY_CMD);
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                inchDown();
                reset_motor_action_if_matches(inchDownAction);
                EUSART_Write(READY_CMD);
                break;
            } 
            //UART switch case, decide what to do 
            //these are the non-instantaneous instructions, the ones that the program 
            //should return to it one of the trip switches goes off and it has to reset 
            case complexBreaths: { //if we are using the five part complex instructions
                set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
                //calculations are done appside - more resources, floating point calculations, etc.
                //0xA9 to start and to end
                //what we receive: amplitude (small/1, med/2, large/4), first_step_size, first_ratio, second_step_size, second_ratio
                //amplitude roughly correlates to the amount of cmH20 that the pressures oscillates (resting to max) 
                step_size step_size_in_1 = NullStep;
                step_size step_size_in_2 = NullStep;
                uint16_t steps_multiplier_1 = 0x0000;
                uint16_t steps_multiplier_2 = 0x0000;
                switch(get_complex_breaths_instructions_index(1)){
                    case 0: 
                    case '0': {
                        step_size_in_1 = NullStep;
                        steps_multiplier_1 = NULL_STEP_SIZE_MULTIPLIER; //no movement
                        break;
                    }case 1: 
                    case '1': {
                        step_size_in_1 = Full;
                        steps_multiplier_1 = FULL_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 2: 
                    case '2': {
                        step_size_in_1 = Half;
                        steps_multiplier_1 = HALF_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 4: 
                    case '4': {
                        step_size_in_1 = Quarter;
                        steps_multiplier_1 = QUARTER_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 8: 
                    case '8': {
                        step_size_in_1 = Eighth;
                        steps_multiplier_1 = EIGHTH_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 16: {
                        step_size_in_1 = Sixteenth;
                        steps_multiplier_1 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
                        break;
                    }default: {
                        //hopefully we will never hit default, it means something is broken
                        step_size_in_1 = NullStep;
                        steps_multiplier_1 = NULL_STEP_SIZE_MULTIPLIER; //no movement
                        break;
                    }
                }
                switch(get_complex_breaths_instructions_index(3)){
                    case 0: 
                    case '0': {
                        step_size_in_2 = NullStep;
                        steps_multiplier_2 = NULL_STEP_SIZE_MULTIPLIER; //no movement
                        break;
                    }case 1: 
                    case '1': {
                        step_size_in_2 = Full;
                        steps_multiplier_2 = FULL_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 2: 
                    case '2': {
                        step_size_in_2 = Half;
                        steps_multiplier_2 = HALF_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 4: 
                    case '4': {
                        step_size_in_2 = Quarter;
                        steps_multiplier_2 = QUARTER_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 8:  
                    case '8': {
                        step_size_in_2 = Eighth;
                        steps_multiplier_2 = EIGHTH_STEP_SIZE_MULTIPLIER;
                        break;
                    }case 16: {
                        step_size_in_2 = Sixteenth;
                        steps_multiplier_2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
                        break;
                    }default: {
                        //hopefully we will never hit default, it means something is broken
                        step_size_in_2 = NullStep;
                        steps_multiplier_2 = NULL_STEP_SIZE_MULTIPLIER; //no movement
                        break;
                    }
                }
                //this needs testing
                //check if this fixes the possible uint8_t input overflow error?
                uint16_t numSteps1 = ((uint16_t)get_complex_breaths_instructions_index(2)*(uint16_t)steps_multiplier_1*(uint16_t)get_complex_breaths_instructions_index(0));
                uint16_t numSteps2 = ((uint16_t)get_complex_breaths_instructions_index(4)*(uint16_t)steps_multiplier_2*(uint16_t)get_complex_breaths_instructions_index(0));
                Step(UP, numSteps1, step_size_in_1, numSteps2, step_size_in_2, true);
             // Step(DOWN, ((uint16_t)get_complex_breaths_instructions_index(2)*(uint16_t)steps_multiplier_1*(uint16_t)get_complex_breaths_instructions_index(0)), step_size_in_1, true);
            //	Step(DOWN, ((uint16_t)get_complex_breaths_instructions_index(4)*(uint16_t)steps_multiplier_2*(uint16_t)get_complex_breaths_instructions_index(0)), step_size_in_2, true);
                __delay_ms(15); //moving time around
                Step(DOWN, numSteps2, step_size_in_2, numSteps1, step_size_in_1, true); //reverse
            //	Step(UP, ((uint16_t)get_complex_breaths_instructions_index(4)*(uint16_t)steps_multiplier_2*(uint16_t)get_complex_breaths_instructions_index(0)), step_size_in_2, true);
            //    Step(UP, ((uint16_t)get_complex_breaths_instructions_index(2)*(uint16_t)steps_multiplier_1*(uint16_t)get_complex_breaths_instructions_index(0)), step_size_in_1, true);
                __delay_ms(5); //moving time around
                // TODO: does this still work/is it still used?? (smallVent)
                if((breath_counter_for_smallVent++ == 10) && get_bellowsHasBeenInchedUpOrDownSinceVenting() == false){
                    /* TODO: only call this if the Avthor has NOT had its baseline moved
                     * since it was last vented to atmosphere
                     * baseline moved = INCH UP/INCH DOWN to have a higher/lower tidaling midpoint
                     */
                    breath_counter_for_smallVent = 0;
                    smallVent();
                }
                break;
            }case stepperInstruction: { //basic stepper instruction  
                if(get_stepper_instruction() != 0){
                    basicStepperInstruction(get_stepper_instruction());
                    //eighthsTest(get_stepper_instruction());
                    // TODO: does this still work/is it still used?? (smallVent)
                    if((breath_counter_for_smallVent++ == 10) && get_bellowsHasBeenInchedUpOrDownSinceVenting() == false){
                        /* TODO: only call this if the Avthor has NOT had its baseline moved
                        * since it was last vented to atmosphere
                        * baseline moved = INCH UP/INCH DOWN to have a higher/lower tidaling midpoint
                        */
                        breath_counter_for_smallVent = 0;
                        smallVent();
                    }
                }
                break;
            }default: {
                break; // do nothing
            }
        }
    }
}

void smallVent(void){
    //quick vent every ten breaths, because the baseline pressure is shifting up in the Atrium
    EUSART_Write(SMALL_VENT_CMD); 
  /*  if(!get_VENT_IS_OPEN()){ //if the vent is not already open
        //vent solenoid
        standby_SetHigh();
        vent_solenoid_SetHigh();
        //solenoid_old_SetHigh();
        __delay_ms(25); // quick vent solenoid to 1ATM
        vent_solenoid_SetLow(); 
        if(!get_BLOOD_IS_ON()){
            standby_SetLow(); //only if pump-solenoid is off
        } 
    }  */
}

void basicStepperInstruction(char charIn){
    set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
    char numRotations = 0;
    step_size stepSize1 = NullStep;
    step_size stepSize2 = NullStep;
    uint8_t stepMultiplier1 = 0;
    uint8_t stepMultiplier2 = 0;
	switch(charIn){
		//preset basic instructions for non-instantaneous commands (breaths):
		//fluid basic commands are done in AvthorStates.c
		case 's': { //0x73
			//breath pattern 1
			//measured: 1 cmH20, 10 breaths/min
            numRotations = 1;
            stepSize1 = Sixteenth;
            stepMultiplier1 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
            stepSize2 = Sixteenth;
            stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
		//	Step(UP, (MOTOR_NUM_STEPS*4), Eighth, (MOTOR_NUM_STEPS*8), Sixteenth, true); //4/8 revolution
		//	Step(DOWN, (MOTOR_NUM_STEPS*8), Sixteenth, true); //4/8 revolution
		//	__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
		//	Step(DOWN, (MOTOR_NUM_STEPS*8), Sixteenth, (MOTOR_NUM_STEPS*4), Eighth, true); //4/8 revolution
		//	Step(UP, (MOTOR_NUM_STEPS*4), Eighth, true); //4/8 revolution
		//	__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
			break;
		}case 't': { //0x74
			//breath pattern 2 
			//measured: 2.5 cmH20, 12 breaths/min
            numRotations = 4;
            stepSize1 = Half;
            stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
            stepSize2 = Half;
            stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
		/*	Step(UP, (MOTOR_NUM_STEPS*8), Half, 0, NullStep, true); //4 revolutions
			__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
			Step(DOWN, (MOTOR_NUM_STEPS*8), Half, 0, NullStep, true); //4 revolutions
			__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);  */
			break;
		}case 'u': { //0x75
			//breath pattern 3
			//measured: 1.5 cmH20, 16 breaths/min
            numRotations = 2;
            stepSize1 = Half;
            stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
            stepSize2 = Quarter;
            stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
		//	Step(UP, (MOTOR_NUM_STEPS*2), Half, (MOTOR_NUM_STEPS*4), Quarter, true); //1 revolutions
		//	Step(DOWN, (MOTOR_NUM_STEPS*4), Quarter, true); //1 revolutions
		//	__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
		//  Step(DOWN, (MOTOR_NUM_STEPS*4), Quarter, (MOTOR_NUM_STEPS*2), Half, true); //1 revolutions
		//	Step(UP, (MOTOR_NUM_STEPS*2), Half, true); //1 revolutions
		//	__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
			break;
		}case 'v': { //0x76 
			//breath pattern 4
			//measured: 2 cmH20, 17 breaths/min
            numRotations = 3;
            stepSize1 = Half;
            stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
            stepSize2 = Half;
            stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
		/*	Step(UP, (MOTOR_NUM_STEPS*6), Half, 0, NullStep, true); //3 revolutions
			__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
			Step(DOWN, (MOTOR_NUM_STEPS*6), Half, 0, NullStep, true); //3 revolutions
			__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);  */
			break;
		}case 'w': { //0x77 
			//breath pattern 5
			//measured: 1.5 cmH20, 23 breaths/min
            numRotations = 2;
            stepSize1 = Half;
            stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
            stepSize2 = Half;
            stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
		/*	Step(UP, (MOTOR_NUM_STEPS*4), Half, 0, NullStep, true); //2 revolutions
			__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
			Step(DOWN, (MOTOR_NUM_STEPS*4), Half, 0, NullStep, true); //2 revolutions
			__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);  */
			break;
		}case 0xB0: {
			//test breath pattern
			//1 rotation, Sixteenth step
			numRotations = 1;
			stepSize1 = Sixteenth;
			stepMultiplier1 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB1: {
			//test breath pattern
			//1 rotation, half Sixteenth half Eighth step
			numRotations = 1;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB2: {
			//test breath pattern
			//1 rotation, Eighth step
			numRotations = 1;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Eighth;
			stepMultiplier2 = EIGHTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB3: {
			//test breath pattern
			//1 rotation, half Eighth half Quarter step
			numRotations = 1;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB4: {
			//test breath pattern
			//1 rotation, Quarter step
			numRotations = 1;
			stepSize1 = Quarter;
			stepMultiplier1 = QUARTER_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB5: {
			//test breath pattern
			//1 rotation, half Quarter half Half step
			numRotations = 1;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB6: {
			//test breath pattern
			//1 rotation, Half step
			numRotations = 1;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB7: {
			//test breath pattern
			//1 rotation, half Half half Full step
			numRotations = 1;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xB8: {
			//test breath pattern
			//1 rotation, Full step
			numRotations = 1;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Full;
			stepMultiplier2 = FULL_STEP_SIZE_MULTIPLIER;
			break;  
		}case 0xB9: {
			//test breath pattern
			//2 rotations, Sixteenth step
			numRotations = 2;
			stepSize1 = Sixteenth;
			stepMultiplier1 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xBA: {
			//test breath pattern
			//2 rotations, half Sixteenth half Eighth step
			numRotations = 2;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xBB: {
			//test breath pattern
			//2 rotations, Eighth step
			numRotations = 2;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Eighth;
			stepMultiplier2 = EIGHTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xBC: {
			//test breath pattern
			//2 rotations, half Eighth half Quarter step
			numRotations = 2;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xBD: {
			//test breath pattern
			//2 rotations, Quarter step
			numRotations = 2;
			stepSize1 = Quarter;
			stepMultiplier1 = QUARTER_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xBE: {
			//test breath pattern
			//2 rotations, half Quarter half Half step
			numRotations = 2;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xBF: {
			//test breath pattern
			//2 rotations, Half step
			numRotations = 2;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC0: {
			//test breath pattern
			//2 rotations, half Half half Full step
			numRotations = 2;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC1: {
			//test breath pattern
			//2 rotations, Full step
			numRotations = 2;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Full;
			stepMultiplier2 = FULL_STEP_SIZE_MULTIPLIER;
			break; 	
		}case 0xC2: {
			//test breath pattern
			//3 rotations, Sixteenth step
			numRotations = 3;
			stepSize1 = Sixteenth;
			stepMultiplier1 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC3: {
			//test breath pattern
			//3 rotations, half Sixteenth half Eighth step
			numRotations = 3;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC4: {
			//test breath pattern
			//3 rotations, Eighth step
			numRotations = 3;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Eighth;
			stepMultiplier2 = EIGHTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC5: {
			//test breath pattern
			//3 rotations, half Eighth half Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC6: {
			//test breath pattern
			//3 rotations, Quarter step
			numRotations = 3;
			stepSize1 = Quarter;
			stepMultiplier1 = QUARTER_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC7: {
			//test breath pattern
			//3 rotations, half Quarter half Half step
			numRotations = 3;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC8: {
			//test breath pattern
			//3 rotations, Half step
			numRotations = 3;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xC9: {
			//test breath pattern
			//3 rotations, half Half half Full step
			numRotations = 3;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xCA: {
			//test breath pattern
			//3 rotations, Full step
			numRotations = 3;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Full;
			stepMultiplier2 = FULL_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xCB: {
			//test breath pattern
			//4 rotations, Sixteenth step
			numRotations = 4;
			stepSize1 = Sixteenth;
			stepMultiplier1 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xCC: {
			//test breath pattern
			//4 rotations, half Sixteenth half Eighth step
			numRotations = 4;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xCD: {
			//test breath pattern
			//4 rotations, Eighth step
			numRotations = 4;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Eighth;
			stepMultiplier2 = EIGHTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xCE: {
			//test breath pattern
			//4 rotations, half Eighth half Quarter step
			numRotations = 4;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xCF: {
			//test breath pattern
			//4 rotations, Quarter step
			numRotations = 4;
			stepSize1 = Quarter;
			stepMultiplier1 = QUARTER_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD0: {
			//test breath pattern
			//4 rotations, half Quarter half Half step
			numRotations = 4;
			stepSize1 = Quarter;
			stepMultiplier1 = QUARTER_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD1: {
			//test breath pattern
			//4 rotations, Half step
			numRotations = 4;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD2: {
			//test breath pattern
			//4 rotations, half Half half Full step
			numRotations = 4;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD3: {
			//test breath pattern
			//4 rotations, Full step
			numRotations = 4;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Full;
			stepMultiplier2 = FULL_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xD4: {
			//test breath pattern
			//5 rotations, Sixteenth step
			numRotations = 5;
			stepSize1 = Sixteenth;
			stepMultiplier1 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD5: {
			//test breath pattern
			//5 rotations, half Sixteenth half Eighth step
			numRotations = 5;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Sixteenth;
			stepMultiplier2 = SIXTEENTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD6: {
			//test breath pattern
			//5 rotations, Eighth step
			numRotations = 5;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Eighth;
			stepMultiplier2 = EIGHTH_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD7: {
			//test breath pattern
			//5 rotations, half Eighth half Quarter step
			numRotations = 5;
			stepSize1 = Eighth;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD8: {
			//test breath pattern
			//5 rotations, Quarter step
			numRotations = 5;
			stepSize1 = Quarter;
			stepMultiplier1 = QUARTER_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xD9: {
			//test breath pattern
			//5 rotations, half Quarter half Half step
			numRotations = 5;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xDA: {
			//test breath pattern
			//5 rotations, Half step
			numRotations = 5;
			stepSize1 = Half;
			stepMultiplier1 = HALF_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xDB: {
			//test breath pattern
			//5 rotations, half Half half Full step
			numRotations = 5;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Half;
			stepMultiplier2 = HALF_STEP_SIZE_MULTIPLIER;
			break;
		}case 0xDC: {
			//test breath pattern
			//5 rotations, Full step
			numRotations = 5;
			stepSize1 = Full;
			stepMultiplier1 = FULL_STEP_SIZE_MULTIPLIER;
			stepSize2 = Full;
			stepMultiplier2 = FULL_STEP_SIZE_MULTIPLIER;
			break; 
		}default: {
			break;
		}
	}
    //send uart flag - this was used to measre how long each step took via the Arduino Serial Port
 //   EUSART2_Write(charIn); //start timer
    __delay_ms(10);
 //   EUSART2_Write('\n');
    //step
    Step(UP, (numRotations*stepMultiplier1*MOTOR_NUM_STEPS/2), stepSize1, (numRotations*stepMultiplier2*MOTOR_NUM_STEPS/2), stepSize2, true);
  //  Step(DOWN, (numRotations*stepMultiplier1*MOTOR_NUM_STEPS/2), stepSize1, true); // 1/2 numRotations revolutions
 //   Step(DOWN, (numRotations*stepMultiplier2*MOTOR_NUM_STEPS/2), stepSize2, true); // 1/2 numRotations revolutions
    __delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
    Step(DOWN, (numRotations*stepMultiplier2*MOTOR_NUM_STEPS/2), stepSize2, (numRotations*stepMultiplier1*MOTOR_NUM_STEPS/2), stepSize1, true);
 //   Step(UP, (numRotations*stepMultiplier2*MOTOR_NUM_STEPS/2), stepSize2, true); // 1/2 numRotations revolutions
 //   Step(UP, (numRotations*stepMultiplier1*MOTOR_NUM_STEPS/2), stepSize1, true); // 1/2 numRotations revolutions
    __delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE); 
    //send uart flag
 //   EUSART2_Write(0x7E); //end timer
    __delay_ms(10);
 //   EUSART2_Write('\n');
}

void eighthsTest(char charIn){
    set_INTERRUPT_BREAK_FLAG(false); //can go through Step() functions 
    char numRotations = 0;
    step_size stepSize1 = NullStep;
    step_size stepSize2 = NullStep;
    uint8_t stepMultiplier1 = 0;
    uint8_t stepMultiplier2 = 0;
    uint8_t ratio1 = 0;
    uint8_t ratio2 = 0;
    switch(charIn){
        case 0xDD: {
			//test breath pattern
			//3 rotations, Eighth step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 200;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 0;
			stepSize2 = NullStep;
			stepMultiplier2 = NULL_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xDE: {
			//test breath pattern
			//3 rotations, 7/8 Eighth 1/8 Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 175;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 25;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xDF: {
			//test breath pattern
			//3 rotations, 3/4 Eighth 1/4 Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 150;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 50;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xE0: {
			//test breath pattern
			//3 rotations, 3/8 Eighth 5/8 Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 125;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 75;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xE1: {
			//test breath pattern
			//3 rotations, 1/2 Eighth 1/2 Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 100;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 100;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xE2: {
			//test breath pattern
			//3 rotations, 3/8 Eighth 5/8 Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 75;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 125;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xE3: {
			//test breath pattern
			//3 rotations, 1/4 Eighth 3/4 Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 50;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 150;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xE4: {
			//test breath pattern
			//3 rotations, 1/8 Eighth 7/8 Quarter step
			numRotations = 3;
			stepSize1 = Eighth;
			ratio1 = 25;
			stepMultiplier1 = EIGHTH_STEP_SIZE_MULTIPLIER;
			ratio2 = 175;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}case 0xE5: {
			//test breath pattern
			//3 rotations, Quarter step
			numRotations = 3;
			stepSize1 = NullStep;
			ratio1 = 0;
			stepMultiplier1 = NULL_STEP_SIZE_MULTIPLIER;
			ratio2 = 200;
			stepSize2 = Quarter;
			stepMultiplier2 = QUARTER_STEP_SIZE_MULTIPLIER;
			break; 
		}default: {
			break;
		}
	}
	//send uart flag
//    EUSART2_Write(charIn); //start timer
	__delay_ms(10);
//    EUSART2_Write('\n');
	//step
    Step(UP, (numRotations*stepMultiplier1*ratio1), stepSize1, (numRotations*stepMultiplier2*ratio2), stepSize2, true);
//	Step(DOWN, (numRotations*stepMultiplier1*ratio1), stepSize1, true); // first ratio * numRotations revolutions
//	Step(DOWN, (numRotations*stepMultiplier2*ratio2), stepSize2, true); // second ratio * numRotations revolutions
	__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE);
    Step(DOWN, (numRotations*stepMultiplier2*ratio2), stepSize2, (numRotations*stepMultiplier1*ratio1), stepSize1, true);
//	Step(UP, (numRotations*stepMultiplier2*ratio2), stepSize2, true); // second ratio * numRotations revolutions
//	Step(UP, (numRotations*stepMultiplier1*ratio1), stepSize1, true); // first ratio * numRotations revolutions
	__delay_ms(DELAY_BETWEEN_STEP_CALLS_DIR_CHANGE); 
	//send uart flag
//    EUSART2_Write(0x7E); //end timer
	__delay_ms(10);
//    EUSART2_Write('\n');
}  

/**
 End of File
*/