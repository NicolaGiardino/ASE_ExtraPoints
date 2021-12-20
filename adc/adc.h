#include <string.h>

/* lib_adc.c */
void ADC_init (void);
void ADC_start_conversion (void);
void MovePotentiometer();
/* IRQ_adc.c */
void ADC_IRQHandler(void);
