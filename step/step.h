/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef STEP_H
#define	STEP_H

#include <xc.h> // include processor files - each processor file is guarded.  
//#include "pin_manager.h"
#include "mcc.h"

//for use with stepper motor NEVA-17
//1 rotation = 0.05" vertical movement (1.8 deg, 200 steps/rotation)
#define MOTOR_NUM_STEPS 200 //for 1 revolution at full step size
#define STEP_DELAY 500 //750 //in us
#define STEP_DELAY_CALIBRATION 1000 //in us
typedef enum {UP, DOWN}direction; //0, 1
typedef enum {Full =  1, Half = 2, Quarter = 4, Eighth = 8, Sixteenth = 16, NullStep = 0}step_size; 
//null is for when you need to waste time if step size should be slower than sixteenths
//typedef uint16_t; //why tf do I need this, isn't it in xc.h? --> changed to using int
#define FULL_STEP_SIZE_MULTIPLIER 1
#define HALF_STEP_SIZE_MULTIPLIER 2
#define QUARTER_STEP_SIZE_MULTIPLIER 4
#define EIGHTH_STEP_SIZE_MULTIPLIER 8
#define SIXTEENTH_STEP_SIZE_MULTIPLIER 16
#define NULL_STEP_SIZE_MULTIPLIER  16 //this is one, so it will loop through Step() and waste time

// #define HAPTIC_ARRAY_SIZE 5
#define DIR_UP_BREATH_START 1
#define DIR_UP_BREATH_END 2
#define DIR_DOWN_BREATH_START 5
#define DIR_DOWN_BREATH_END 4

void Step(direction dir, uint16_t numSteps1, step_size size1, uint16_t numSteps2, step_size size2, bool haptic_breaths);
void calibrate(void);
void moveUntilZ(char dir);
void slowStepTest(void);
void inchUp(void);
void inchDown(void);
void topButtonTrigger(void);
void bottomButtonTrigger(void);
void stepperSleep(bool in);
void setStepSize(step_size size);
void setDirection(direction dir);
void sendHapticCommand(uint8_t input);

#endif /*STEP_H*/
