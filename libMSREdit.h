#ifndef LIBMSREDIT_H_INCLUDED
#define LIBMSREDIT_H_INCLUDED


#include "MSREdit.h"
void tcOPrint(CHAR16 *string);
void PrintXY(CHAR16 * string,uint64_t x, uint64_t y 	);
void setCursor(uint16_t col, uint64_t row);
void Charset();


uint8_t HexBoard(uint8_t current,uint64_t max);
uint8_t KeyTimeout(uint64_t timeout,	uint8_t Progress);
uint8_t menu (uint8_t choice);

#endif // LIBMSREDIT_H_INCLUDED
