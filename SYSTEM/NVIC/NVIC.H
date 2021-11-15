void DK_NVIC_Init(unsigned char Group);
void DK_NVIC_IRQ_Set(unsigned char NVIC_IRQChannel, unsigned char pre_pri, unsigned char sub_pri, FunctionalState cmd);
void NVIC_Configuration(void);
