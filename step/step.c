#include "step.h"
#include "AvthorStates.h"

void Step(direction dir, uint16_t num_steps1, step_size size1, uint16_t num_steps2, step_size size2, bool haptic_breaths){
    // we need 1 and 2 because breaths can be made up of two different step sizes
    stepperSleep(false);
    setDirection(dir);
    // step set #1
    setStepSize(size1); 
    // testLED_SetHigh();
    
    if(dir == UP){
        //CW
        // haptic commands: 1-5
        sendHapticCommand(DIR_UP_BREATH_START);
    }else { //if(dir == DOWN){
        //CCW
        // haptic commands: 5-1
        sendHapticCommand(DIR_DOWN_BREATH_START);
    }
    
    /* haptic_breaths: 
      * we split num_breaths into 5 checkpoints, send haptic commands 1-5 (and 0 to stop)
      * populate an array to hold these stopping points:
      */
  /*  uint16_t haptic_array[HAPTIC_ARRAY_SIZE];
    for(uint16_t q = 0; q < (HAPTIC_ARRAY_SIZE); q++){
        haptic_array[q] = ((num_steps1 + num_steps2)/HAPTIC_ARRAY_SIZE) * (q + 1);
    }
    uint16_t *haptic_array_pointer = &haptic_array[0]; //init at the start of the array
    // start by sending the haptic commands to eusart1, then a 1
    // the lowest haptic setting
    char send_letter;  */
    
    __delay_ms(20); //for stability
    
    /* DIR pin: LOW=CW, HIGH=CCW 
     * steps: 
     * Full - MS1=LOW, MS2=LOW, MS3=LOW
     * Half - MS1=HIGH, MS2=LOW, MS3=LOW
     * Quarter - MS1=LOW, MS2=HIGH, MS3=LOW
     * Eighth - MS1=HIGH, MS2=HIGH, MS3=LOW
     * Sixteenth - MS1=HIGH, MS2=HIGH, MS3=HIGH
     */
    for(uint16_t i = 0; i < (num_steps1); i++){ //on B1
        if(get_INTERRUPT_BREAK_FLAG()){
            break; //from for loop, thrown back to switch statement for new instruction
        }else{
            STEP_SetHigh();
            __delay_us(STEP_DELAY);
            STEP_SetLow();
            __delay_us(STEP_DELAY);
            // check if we need to send another haptic command
        /*    if (haptic_breaths && (i == *haptic_array_pointer)){ // checking the value
                haptic_array_pointer++;
                if(dir == UP){
                    send_letter++;
                }else{ // DOWN
                    send_letter--;
                }
                EUSART1_Write(HAPTIC_COMMAND_CHAR);
                __delay_ms(10); //stability
                EUSART1_Write(send_letter); 
            }  */
        } 
    }
   // testLED_SetLow();
    
    //  step set #2
    stepperSleep(false); // reset
    setStepSize(size2);
   // testLED_SetHigh();  
    
    for(uint16_t i = 0; i < (num_steps2); i++){ //on B1
        if(get_INTERRUPT_BREAK_FLAG()){
            break; //from for loop, thrown back to switch statement for new instruction
        }else{
            STEP_SetHigh();
            __delay_us(STEP_DELAY);
            STEP_SetLow();
            __delay_us(STEP_DELAY);
            // check if we need to send another haptic command
        /*    if (haptic_breaths && ((i + num_steps1) == *haptic_array_pointer)){ // checking the value
                haptic_array_pointer++;
                if(dir == UP){
                    send_letter++;
                }else{ // DOWN
                    send_letter--;
                }
                EUSART1_Write(HAPTIC_COMMAND_CHAR);
                __delay_ms(10); //stability
                EUSART1_Write(send_letter); 
            }  */
        }
    }
   // testLED_SetLow();
    stepperSleep(true);
    
    if(dir == UP){
        //CW
        // haptic commands: 1-5
        sendHapticCommand(DIR_UP_BREATH_END);
    }else { //if(dir == DOWN){
        //CCW
        // haptic commands: 5-1
        sendHapticCommand(DIR_DOWN_BREATH_END);
    } 
}

void slowStepTest(void){
    //CW
    setDirection(DOWN);
    setStepSize(Full);
    stepperSleep(false);
    while(!get_INTERRUPT_BREAK_FLAG()){
        STEP_SetHigh();
        __delay_ms(500);
        STEP_SetLow();
        __delay_ms(500);
    }
    set_INTERRUPT_BREAK_FLAG(false);
    stepperSleep(true);
}

void moveUntilZ(char dir){
    stepperSleep(false);
  /*  if(dir == 1){
        DIR_SetLow(); //Dir_SetLow() makes the bellows travel upwards
    }else  if(dir == 2){
        DIR_SetHigh(); //downwards
    }   */
    setDirection(dir);
    setStepSize(Quarter);
    while(!get_INTERRUPT_BREAK_FLAG()){
        //move stepper up until sending 'z' sets this false
        STEP_SetHigh();
        __delay_us(STEP_DELAY_CALIBRATION);
        STEP_SetLow();
        __delay_us(STEP_DELAY_CALIBRATION);
    }
    stepperSleep(true);
}

void calibrate(void){
    stepperSleep(false);
    /*CALIBRATION START*/ 
    set_INTERRUPT_BREAK_FLAG(false);
    setDirection(DOWN); //Dir_SetLow() makes the bellows travel upwards
    setStepSize(Half);
    while(!get_INTERRUPT_BREAK_FLAG()){
        //move stepper up until it hits the top, then it will go to the center and vent solenoid to 1ATM
        STEP_SetHigh();
        __delay_us(STEP_DELAY_CALIBRATION);
        STEP_SetLow();
        __delay_us(STEP_DELAY_CALIBRATION);
    }
    set_INTERRUPT_BREAK_FLAG(false); 
    /*CALIBRATION END*/  
    stepperSleep(true);
}

void inchUp(void){
    Step(UP, (MOTOR_NUM_STEPS*2*QUARTER_STEP_SIZE_MULTIPLIER), Quarter, 0, NullStep, false); //2 revolutions
}

void inchDown(void){
    Step(DOWN, (MOTOR_NUM_STEPS*2*QUARTER_STEP_SIZE_MULTIPLIER), Quarter, 0, NullStep, false); //2 revolutions
}

void topButtonTrigger(void){
    STEP_SetLow();
    __delay_ms(50); //brake stepper motor
    uint8_t rotations = 12; //24 rotations for the whole distance
    //we want to be near the bottom as breaths all go UP first then DOWN
    // ****** it is ok to do this here because we want this to be blocking ()
    Step(DOWN, (MOTOR_NUM_STEPS*rotations*QUARTER_STEP_SIZE_MULTIPLIER), Quarter, 0, NullStep, false); //move down however many rotations you need to get to the center
    set_INTERRUPT_BREAK_FLAG(true); //this needs to be set after Step(), tells the main loop to fall though, get back to the top of the loop
}

void bottomButtonTrigger(void){
    STEP_SetLow();
    __delay_ms(50); //brake stepper motor
    uint8_t rotations = 6; //24 rotations for the whole distance
    //we want to be near the bottom as breaths all go UP first then DOWN
    // ****** it is ok to do this here because we want this to be blocking ()
    Step(UP, (MOTOR_NUM_STEPS*rotations*QUARTER_STEP_SIZE_MULTIPLIER), Quarter, 0, NullStep, false); //move down however many rotations you need to get to the center
    set_INTERRUPT_BREAK_FLAG(true); //this needs to be set after Step(), tells the main loop to fall though, get back to the top of the loop
}

void stepperSleep(bool in){
    if(in){
        SLEEP_SetLow();
    }else{
        SLEEP_SetHigh();
    }
}

void setStepSize(step_size size){
    switch(size){
        case Full: {
            MS1_SetLow();
            MS2_SetLow();
            MS3_SetLow();
            break;
        }case Half: {
            MS1_SetHigh();
            MS2_SetLow();
            MS3_SetLow();
            break;
        }case Quarter: {
            MS1_SetLow();
            MS2_SetHigh();
            MS3_SetLow();
            break;
        }case Eighth: {
            MS1_SetHigh();
            MS2_SetHigh();
            MS3_SetLow();
            break;
        }case Sixteenth: {
            MS1_SetHigh();
            MS2_SetHigh();
            MS3_SetHigh();
            break;
        }case NullStep:
        default: {
            stepperSleep(true);
            //this is for when we want to
            //go slower than 16th steps; we split time
            //between 16ths and this state
            //ex: 80% sixteenth steps. 20% null steps
            break;
        }
    }  
}

void setDirection(direction dir){
    if(dir == UP){ //these bits can be flipped as necessary in order to achieve correct directions when the system is connected
        //CCW
        DIR_SetHigh();
    }else { //if(dir == DOWN){
        //CW
        DIR_SetLow();
    }
}

void sendHapticCommand(uint8_t input){
    return;
    // send first letter, set haptic state on BLE to 1
    EUSART_Write(HAPTIC_COMMAND_CHAR);
    __delay_ms(10); //stability
    EUSART_Write(input); 
}