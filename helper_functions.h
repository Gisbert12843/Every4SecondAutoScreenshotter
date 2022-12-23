#pragma once
#include "wholeinclude.h"

//const wchar_t* GetWC(const char* c)
//{
//	const size_t cSize = strlen(c) + 1;
//	wchar_t* wc = new wchar_t[cSize];
//	mbstowcs(wc, c, cSize);
//
//	return wc;
//}
void setCredentials(std::string projectpath, std::string& username, std::string& password, std::string& port, bool& didloadcredentials);


bool fix_connectionphp(std::string projectpath, std::string username, std::string password, std::string port);


bool doCmdCommand(std::string thepath, std::string running_exe = "");


bool doCmdCommandInNewWindow(std::string command, std::string running_exe = "");


bool killProcessByName(const wchar_t* filename);


HANDLE GetProcessByName(PCSTR name);

bool isRunning(LPCSTR pName);


void LoadFileInResource(int name, int type, DWORD& size, const char*& data);

std::string fixPath(std::string theString);



int createWindowsMessage(std::string thestring);

//return: yes==6 no==7 cancel==2 abort==3
int createWindowsChoice(std::string thestring);

int createWindowsError(std::string thestring);

void killProgrammMessage();

void startSQLService(std::string servicename);


void stopSQLService(std::string servicename);


std::string initProjectFiles();

std::string findMaria();


void loadCredentials(std::string projectpath, std::string& username, std::string& password, std::string& port, bool& didloadcredentials);

std::string findDocumentFolderAsString();
WCHAR findDocumentFolderAsTCHAR();