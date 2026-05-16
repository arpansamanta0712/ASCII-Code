#include <avr/interrupt.h>

// Define maximum tasks and stack size for the UNO's small memory
#define MAX_TASKS 2
#define STACK_SIZE 128

// This structure represents the memory state of a single task
struct RealTask {
  uint8_t* stackPointer; // Points to where this task's memory is stored
};

RealTask taskTable[MAX_TASKS];
volatile int currentTask = 0;

// Dynamic memory space allocated for each task's stack
uint8_t taskStacks[MAX_TASKS][STACK_SIZE];

// --- The Kernel Task Creator ---
// This function takes your raw code and forces it into the memory table
void createKernelTask(int taskIndex, void (*taskFunction)()) {
  uint8_t* topOfStack = &taskStacks[taskIndex][STACK_SIZE - 1];

  // Manually place the function's address onto the fake stack
  *topOfStack = (uint16_t)taskFunction & 0xFF;         // Low byte of address
  *(topOfStack - 1) = ((uint16_t)taskFunction >> 8) & 0xFF; // High byte

  // Leave space for the 32 registers the AVR CPU uses to think
  taskTable[taskIndex].stackPointer = topOfStack - 34; 
}

// --- The Core Kernel Multitasker ---
// This Interrupt Service Routine (ISR) triggers on a timer tick.
// It forces the CPU to switch tasks even if the current task is busy.
ISR(TIMER1_COMPA_vect) __attribute__((naked)); 
ISR(TIMER1_COMPA_vect) {
  // 1. SAVE CURRENT TASK CONTEXT (Push all CPU registers to memory)
  asm volatile (
    "push r0 \n\t"
    "in r0, __SREG__ \n\t"
    "push r0 \n\t"
    "push r1 \n\t"
    "clr r1 \n\t"
    "push r2 \n\t" // ... (Kernel saves r2 through r31 here)
  );

  // Save the current stack pointer into our task table
  taskTable[currentTask].stackPointer = (uint8_t*)SPH << 8 | SPL;

  // 2. SWITCH TO THE NEXT TASK
  currentTask = (currentTask + 1) % MAX_TASKS;

  // 3. RESTORE THE NEW TASK CONTEXT
  SPL = (uint16_t)taskTable[currentTask].stackPointer & 0xFF;
  SPH = ((uint16_t)taskTable[currentTask].stackPointer >> 8) & 0xFF;

  asm volatile (
    "pop r2 \n\t" // ... (Kernel restores r31 through r2 here)
    "pop r1 \n\t"
    "pop r0 \n\t"
    "out __SREG__, r0 \n\t"
    "pop r0 \n\t"
    "reti \n\t" // Return from interrupt into the new task code
  );
}

// --- Your Code Tasks ---
void kernelTask1() {
  while(1) {
    digitalWrite(13, HIGH);
    // Instead of freezing, the kernel will eventually cut this task off
  }
}

void kernelTask2() {
  while(1) {
    digitalWrite(13, LOW);
  }
}

void setup() {
  pinMode(13, OUTPUT);

  // Setup our kernel tasks into memory
  createKernelTask(0, kernelTask1);
  createKernelTask(1, kernelTask2);

  // Setup Hardware Timer 1 to trigger the kernel switch every few milliseconds
  cli(); 
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 250; // Set the speed of the multitasking switch
  TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10); 
  TIMSK1 |= (1 << OCIE1A); 
  sei(); 
}

void loop() {
  // The main loop stays empty because the kernel completely takes over the CPU
}
