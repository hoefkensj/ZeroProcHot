#ifndef MSREDIT_H_INCLUDED
#define MSREDIT_H_INCLUDED

#include <efi.h>
#include <efilib.h>
#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>


EFI_SYSTEM_TABLE               *sT;
EFI_BOOT_SERVICES              *tbS;
SIMPLE_TEXT_OUTPUT_INTERFACE   *tcO;
SIMPLE_INPUT_INTERFACE         *tcI;
EFI_TEXT_OUTPUT_STRING          STROUT;
EFI_TEXT_RESET                  CLEAR;
EFI_INPUT_READ_KEY              READ;
EFI_STALL                       SLEEP;
EFI_WAIT_FOR_EVENT              WAITevt;
EFI_EVENT                       WAITKEY;
EFI_TEXT_SET_CURSOR_POSITION    SETCURSOR;

EFI_TEXT_ENABLE_CURSOR          CURSOR;


CHAR16                         *CHARSET[128];
CHAR16                         CHARSETZERO[256];

// CHAR16 STR_MSR_ADDR[6];
// uint32_t MSR_ADDR = 0x1FC;



uint8_t menu (uint8_t choice);

#endif // MSREDIT_H_INCLUDED

