#include <iostream>
using namespace std;

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>


#define BUF_SIZE 255
#define MAX_THREADS 1

DWORD WINAPI MyThreadFunction( LPVOID lpParam );

void ErrorHandler(LPTSTR lpszFunction);

DWORD WINAPI testFunction( LPVOID lpParam );

typedef struct MyData {
    int val1;
    int val2;
} MYDATA, *PMYDATA;

int main()
{
	PMYDATA pDataArray;
	
	// Allocate memory for thread data.
	
	    pDataArray = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(MYDATA));

        if( pDataArray == NULL )
        {
           // If the array allocation fails, the system is out of memory
           // so there is no point in trying to print an error message.
           // Just terminate execution.
            ExitProcess(2);
        }
		
		HANDLE  hThreadArray; 
		DWORD   dwThreadIdArray;
		
		hThreadArray = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            //MyThreadFunction,       // thread function name
			testFunction,
            pDataArray,          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadIdArray);   // returns the thread identifier 
			
		if (hThreadArray == NULL) 
        {
           ErrorHandler(TEXT("CreateThread"));
           ExitProcess(3);
        }
		
		// Wait until thread is terminated.
		
		//WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);
		
			//PROCESS_INFORMATION pi = {};
		//WaitForSingleObject(pi.hProcess, 20 * 1000);
		WaitForSingleObject(hThreadArray, 20 * 1000);
		
		// Close thread handles and free memory allocations.
		
		CloseHandle(hThreadArray);
        if(pDataArray != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray);
            pDataArray = NULL;    // Ensure address is not reused.
        }
	
}


DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
    HANDLE hStdout;
    PMYDATA pDataArray;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    // Make sure there is a console to receive output results. 

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if( hStdout == INVALID_HANDLE_VALUE )
        return 1;

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because 
    // it was checked for NULL before the thread was created.
 
    pDataArray = (PMYDATA)lpParam;

    // Print the parameter values using thread-safe functions.

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"), 
        pDataArray->val1, pDataArray->val2); 
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);
	
	Sleep(15000);

    return 0; 
} 

DWORD WINAPI testFunction( LPVOID lpParam )
{
	cout<<__FUNCTION__<<endl;
	Sleep(15000);
	
	return 0;
}


void ErrorHandler(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK); 

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}
