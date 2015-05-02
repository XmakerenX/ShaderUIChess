#include "main.h"
#include "CGameWin.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	CGameWin gameWin;
	int retCode;

	//creating a debug console 
	//TODO: create an actual developer console some day ...
	if (AllocConsole()) {
		std::string s = "I made a console window!\n";
		DWORD n;
		char c;

		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), s.c_str(), s.size(), &n, 0);
	}

	// redirect unbuffered STDOUT to the console or in other words make cout print to the debug console
	int hConHandle;
	long lStdHandle;
	FILE *fp;

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen( hConHandle, "w" );

	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// Initialise the engine.
	if (!gameWin.InitInstance( hInstance, lpCmdLine, nCmdShow )) return 0;

	// Begin the gameplay process. Will return when app due to exit.
	retCode = gameWin.BeginGame();

	// Shut down the engine, just to be polite, before exiting.
	if ( !gameWin.ShutDown() )  MessageBox( 0, "Failed to shut system down correctly, please check file named 'debug.txt'.\r\n\r\nIf the problem persists, please contact technical support.", "Non-Fatal Error", MB_OK | MB_ICONEXCLAMATION );

	// Return the correct exit code.
	return retCode;
}