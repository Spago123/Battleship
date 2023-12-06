#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"

void initButton(GPIO_TypeDef*port, uint16_t pin);
__weak void buttonPressed();

#endif
