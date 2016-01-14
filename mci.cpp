#include <Windows.H>

#pragma comment(linker, "/nodefaultlib:libcmt")
#pragma comment(linker, "/subsystem:windows")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winmm.lib")

VOID WinMainCRTStartup(VOID)
{
  PCHAR *Arguments = NULL;    // Arguments array (argv)
  PCHAR  Argument;            // Current argument being processed
  INT    ArgumentCount = 0;   // Number of arguments in Arguments array (argc)
  PCHAR  ArgumentPointer;     // Pointer to command line
  BOOL   InQuotation = FALSE; // In quotations?

  // Retreive command line
  Argument = ArgumentPointer = GetCommandLine();
  // Bail if function failed
  if(!Argument) ExitProcess(0xFFFFFFFF);

  // Walk command line string
  for(;;)
  {
    // Character is a white-space
    if(*ArgumentPointer == 32)
    {
      // In quotation?
      if(InQuotation)
      {
        // Continue to next character
        ++ArgumentPointer;
        // Begin loop again
        continue;
      }
      // Out of quotation, make character null and move to next character
      *ArgumentPointer++ = 0;
      // Fall through and store argument
    }
    // Character is a quote?
    else if(*ArgumentPointer == 34)
    {
      // Make character null and move to next character
      *ArgumentPointer++ = 0;
      // Toggle 'In quotation' and if it is now 1?
      if((InQuotation ^= 1))
      {
        // Skip character
        ++Argument;
        // Begin loop again
        continue;
      }
      // Fall through and store argument
    }
    // Character is insignificant
    else if(*ArgumentPointer)
    {
      // Skip to next cahracter
      ++ArgumentPointer;
      // Begin loop again
      continue;
    }
    // Store argument
    if(ArgumentCount)
      // Not first argument
      Arguments = (PCHAR*)HeapReAlloc(GetProcessHeap(), 0x4, Arguments, (ArgumentCount + 1) * sizeof(PVOID));
    else
      // First argument
      Arguments = (PCHAR*)HeapAlloc(GetProcessHeap(), 0xC, sizeof(PVOID));
    // Store argument in array
    Arguments[ArgumentCount++] = Argument;
    // Walk through all the white-space characters
    while(*ArgumentPointer == 32)
      // Remove them and move to next character
      *ArgumentPointer++ = 0;
    // If there are no more characters to process?
    if(!*ArgumentPointer)
      // At end of command line so break loop
      break;
    // Update beginning of next argument
    Argument = ArgumentPointer;
  }
  // Create space for a new argument
  Arguments = (PCHAR*)HeapReAlloc(GetProcessHeap(), 0x4, Arguments, (ArgumentCount + 1) * sizeof(PVOID));
  // Make it null
  Arguments[ArgumentCount] = NULL;

  // Create return code
  MCIERROR ReturnCode = 0xFFFFFFFE;
  // Not enough parameters?
  if(ArgumentCount != 2)
    // Send message
    MessageBox(NULL, "Usage: MCI.EXE [\"]<Command>[\"]", "MCI", MB_ICONINFORMATION);
  // Got a parameter!
  else
    // Send command and get return code
    ReturnCode = mciSendString(Arguments[1], NULL, 0, NULL);
  // Done with arguments array
  HeapFree(GetProcessHeap(), 0x4, Arguments);
  // Done
  ExitProcess(ReturnCode);
}