/* (C) Copyright 2014, MongoDB, Inc. */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#endif

int main(int argc, char** argv) {
    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Provide a stub WinMain so that the application will run without a console window on Windows.
    return main(__argc, __argv);
}
#endif
