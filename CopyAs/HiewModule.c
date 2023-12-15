#include <windows.h>
#include <strsafe.h>

#include "hem.h"

// HEM SDK required defs

#define HEM_MODULE_VERSION_MAJOR 2
#define HEM_MODULE_VERSION_MINOR 0
#define HEM_MODULE_NAME "CopyAs"
#define HEM_MODULE_FULL_NAME "CopyAs"
#define HEM_MODULE_DESCRIPTION "Copy data in multiple formats"
#define HEM_MODULE_AUTHOR "Fernando Merces - github.com/merces/hem-copyas"

int HEM_API Hem_EntryPoint(HEMCALL_TAG* hemCall);
int HEM_API Hem_Unload(VOID);

HEMINFO_TAG hemMod = {
    sizeof(HEMINFO_TAG),
    sizeof(int),
    0,
    HEM_SDK_VERSION_MAJOR,
    HEM_SDK_VERSION_MINOR,
    HEM_MODULE_VERSION_MAJOR,
    HEM_MODULE_VERSION_MINOR,
    HEM_FLAG_MARKEDBLOCK | HEM_FLAG_FILEMASK | HEM_FLAG_CODE | HEM_FLAG_HEX,
    0,
    Hem_EntryPoint,
    Hem_Unload,
    NULL,
    0,
    0,
    0,
    0,
    HEM_MODULE_NAME,
    HEM_MODULE_FULL_NAME,
    "========================================",
    HEM_MODULE_DESCRIPTION,
    HEM_MODULE_AUTHOR
};

// Module functions

static int ShowHelp(VOID) {
    static PCHAR HelpText[] = {
        "This module allows you to export the content of a marked",
        "block in in multiple formats such as:",
        "",
        "- Hex bytes (no spaces)",
        "- Hex bytes (space-separated)",
        "- Hex string (escaped)",
        "- C array",
        "- C string (escaped non-ASCII)",
        "- VB.NET byte array",
        "",
        "Author: "HEM_MODULE_AUTHOR,
        "",
        "Options:",
        "",
        "   F1    - Show this Help text.",
        "   ENTER - Copy selected formatted string to clipboard",
        "           and close the window.",
    };

    static CHAR title[64] = HEM_MODULE_NAME; // In case StringCchPrintfA() fails, we still have a title :)
    StringCchPrintfA(title, _countof(title), "%s %d.%0.2d", HEM_MODULE_NAME, HEM_MODULE_VERSION_MAJOR, HEM_MODULE_VERSION_MINOR);

    return HiewGate_Window(title, HelpText, _countof(HelpText), 60, NULL, NULL);
}

static BOOL SendTextToClipboard(const PCHAR text) {
    if (!text)
        return FALSE;

    size_t len;

    if (FAILED(StringCchLengthA(text, STRSAFE_MAX_CCH, &len)))
        return FALSE;

    if (len < 1)
        return FALSE;

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len + 1);

    if (!hMem)
        return FALSE;

    LPVOID p = GlobalLock(hMem);

    if (!p)
        return FALSE;

    CopyMemory(p, text, len);

    if (!GlobalUnlock(hMem) && GetLastError() != NO_ERROR)
        return FALSE;

    OpenClipboard(NULL);

    if (!EmptyClipboard())
        return FALSE;

    if (!SetClipboardData(CF_TEXT, hMem)) {
        GlobalFree(hMem);
        return FALSE;
    }

    if (!CloseClipboard())
        return FALSE;

    return TRUE;
}

int HEM_EXPORT Hem_Load(HIEWINFO_TAG* HiewInfo) {
    HiewGate_Set(HiewInfo);
    HiewInfo->hemInfo = &hemMod;
    return HEM_OK;
}

int HEM_API Hem_Unload(VOID) {
    return HEM_OK;
}

int HEM_API Hem_EntryPoint(HEMCALL_TAG* HemCall) {
    HEM_QWORD BaseAddr;
    HEM_UINT BufferSize;
    HIEWGATE_GETDATA HiewData;
    HEM_BYTE* Buffer;
    LPSTR asHex, asHexSpaces, asHexEscaped, asHexCArray, asEscapedStr, asVBByteArray;
    asHex = asHexSpaces = asHexEscaped = asHexCArray = asEscapedStr = asVBByteArray = NULL;

    if (HemCall->cbSize < sizeof(HEMCALL_TAG))
        return HEM_ERROR;

    if (HiewGate_GetData(&HiewData) != HEM_OK)
        return HEM_ERROR;

    // Check if there is an active block, if not use the whole file.
    if (!HiewData.sizeMark) {
        HiewGate_Message("Error", "You have to select a block before copying it");
        return HEM_OK;
    }

    BaseAddr = HiewData.offsetMark1;
    BufferSize = (HEM_UINT)HiewData.sizeMark;
    Buffer = HiewGate_GetMemory(BufferSize);

    if (!Buffer) {
        HiewGate_Message("Error", "Memory allocation error");
        return HEM_OK;
    }

    int read = HiewGate_FileRead(BaseAddr, BufferSize, Buffer);

    if (read != BufferSize) {
        HiewGate_Message("Error", "Error reading data from file");
        goto cleanup;
    }

    ULONG i; // Used as for loops counter but also used to remove trailing characters from final strings
    ULONG j; // Used in asEscapedStr only

    // Hex bytes (no spaces)
    asHex = HiewGate_GetMemory(BufferSize * 2 + 1); // 2 characters: "42"
    if (!asHex) {
        HiewGate_Message("Error", "Memory allocation error");
        goto cleanup;
    }

    for (i = 0; i < BufferSize; i++) {
        if (FAILED(StringCchPrintfA(asHex + i * 2, BufferSize * 2 + 1, "%02x", Buffer[i]))) {
            HiewGate_Message("Error", "StringCchPrintfA() failed: asHex");
            goto cleanup;
        }
    }

    // Hex bytes (space-separated)
    asHexSpaces = HiewGate_GetMemory(BufferSize * 3 + 1); // 3 characters: "42 "
    if (!asHexSpaces) {
        HiewGate_Message("Error", "Memory allocation error");
        goto cleanup;
    }

    for (i = 0; i < BufferSize; i++) {
        if (FAILED(StringCchPrintfA(asHexSpaces + i * 3, BufferSize * 3 + 1, "%02x ", Buffer[i]))) {
            HiewGate_Message("Error", "StringCchPrintfA() failed: asHexSpaces");
            goto cleanup;
        }
    }
    asHexSpaces[i * 3 - 1] = '\0'; // Remove trailing space character

    // Hex string (escaped)
    asHexEscaped = HiewGate_GetMemory(BufferSize * 4 + 1); // 4 characters: "\x42"
    if (!asHexEscaped) {
        HiewGate_Message("Error", "Memory allocation error");
        goto cleanup;
    }

    for (i = 0; i < BufferSize; i++) {
        if (FAILED(StringCchPrintfA(asHexEscaped + i * 4, BufferSize * 4 + 1, "\\x%02x", Buffer[i]))) {
            HiewGate_Message("Error", "StringCchPrintfA() failed: asHexEscaped");
            goto cleanup;
        }
    }
    
    // C array
    asHexCArray = HiewGate_GetMemory(BufferSize * 6 + 1); // 6 characters: "0x42, "
    if (!asHexCArray) {
        HiewGate_Message("Error", "Memory allocation error");
        goto cleanup;
    }

    for (i = 0; i < BufferSize; i++) {
        if (FAILED(StringCchPrintfA(asHexCArray + i * 6, BufferSize * 6 + 1, "0x%02x, ", Buffer[i]))) {
            HiewGate_Message("Error", "StringCchPrintfA() failed: asHexCArray");
            goto cleanup;
        }
    }
    asHexCArray[i * 6 - 2] = '\0'; // Remove trailing comma and space characters

    // C string (escaped non-ASCII)
    asEscapedStr = HiewGate_GetMemory(BufferSize * 4 + 1); // 1 character: "B", but it'll hold 4 characters for non-printable bytes: "\x0d"
    if (!asEscapedStr) {
        HiewGate_Message("Error", "Memory allocation error");
        goto cleanup;
    }

    for (i = 0, j = 0; i < BufferSize; i++) {
        if (Buffer[i] >= ' ' && Buffer[i] <= '~') {
            asEscapedStr[j] = Buffer[i];
            j++;
        } else {
            if (FAILED(StringCchPrintfA(asEscapedStr + j, BufferSize * 4 + 1, "\\x%02x", Buffer[i]))) {
                HiewGate_Message("Error", "StringCchPrintfA() failed: asEscapedStr");
                goto cleanup;
            }
            j += 4;
        }
    }
    asEscapedStr[j] = '\0';

    // VB.NET byte array
    asVBByteArray = HiewGate_GetMemory(BufferSize * 6 + 1); // 6 characters: "&H42, "
    if (!asVBByteArray) {
        HiewGate_Message("Error", "Memory allocation error");
        goto cleanup;
    }

    for (i = 0; i < BufferSize; i++) {
        if (FAILED(StringCchPrintfA(asVBByteArray + i * 6, BufferSize * 6 + 1, "&H%02X, ", Buffer[i]))) {
            HiewGate_Message("Error", "StringCchPrintfA() failed: asVBByteArray");
            goto cleanup;
        }
    }
    asVBByteArray[i * 6 - 2] = '\0'; // Remove trailing comma and space characters


    // Menu code

    // Keys setup
    //                    "123456789ABC|F1____F2____F3____F4____F5____F6____F7____F8____F9____F10___F11___F12___"
    HEM_FNKEYS fnKeys = { "100010000000|Help                                                                    ",   // main Fn
                  "",   // no Alt-Fn
                  "",   // no Ctrl-Fn
                  ""};  // no Shift-Fn

    // Menu entries
    HEM_BYTE* lines[] = {
        asHex,
        asHexSpaces,
        asHexEscaped,
        asHexCArray,
        asEscapedStr,
        asVBByteArray,
    };

    // Menu loop

    HEM_UINT pressedFnKey;

    int item = 1; // Just a reminder menu items start at 1 (not 0)

    while (item = HiewGate_Menu(HEM_MODULE_FULL_NAME, lines, _countof(lines), 40, item, &fnKeys, &pressedFnKey, NULL, NULL)) {
        if (item == HEM_INPUT_ESC)
            break;

        if (pressedFnKey == 0)
            break;

        switch (pressedFnKey) {
        case HEM_FNKEY_F1:
            ShowHelp();
            break;
        default:
            break;
        }
    }

    if (item > 0) {
        if (!SendTextToClipboard(lines[--item])) // Menu items start 1, but arrays still start at 0 :)
            HiewGate_Message("Error", "SendTextToClipboard() failed");
    }

    cleanup:
    HiewGate_FreeMemory(Buffer);
    HiewGate_FreeMemory(asHex);
    HiewGate_FreeMemory(asHexSpaces);
    HiewGate_FreeMemory(asHexEscaped);
    HiewGate_FreeMemory(asHexCArray);
    HiewGate_FreeMemory(asEscapedStr);
    HiewGate_FreeMemory(asVBByteArray);
    return HEM_OK;
}
