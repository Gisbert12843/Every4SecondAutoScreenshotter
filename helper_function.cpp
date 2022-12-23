#pragma once
#include "helper_functions.h"

//const wchar_t* GetWC(const char* c)
//{
//	const size_t cSize = strlen(c) + 1;
//	wchar_t* wc = new wchar_t[cSize];
//	mbstowcs(wc, c, cSize);
//
//	return wc;
//}

//#include "resource.h"


std::string findDocumentFolderAsString()
{
	TCHAR pf[MAX_PATH];

	SHGetSpecialFolderPath(
		0,
		pf,
		CSIDL_MYDOCUMENTS,
		FALSE);
	std::wstring test(&pf[0]);
	std::string test2(test.begin(), test.end());
	return test2;
}

WCHAR findDocumentFolderAsTCHAR()
{
	TCHAR pf[MAX_PATH];

	SHGetSpecialFolderPathW(
		0,
		pf,
		CSIDL_PROGRAM_FILES,
		FALSE);
	return (WCHAR)pf;
}


bool doCmdCommand(std::string thepath, std::string running_exe)
{
	{
		std::wstring thepath_w;
		thepath_w.assign(thepath.begin(), thepath.end());
		LPWSTR thepath_final = &thepath_w[0];

		std::wstring theexe_w;
		theexe_w.append(running_exe.begin(), running_exe.end());
		LPWSTR theexe_final = &theexe_w[0];
		if (running_exe == "")
			theexe_final = NULL;

		STARTUPINFO strtinfo = { sizeof(strtinfo) };
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&strtinfo, sizeof(strtinfo));


		if (CreateProcess(theexe_final, thepath_final, NULL, NULL, FALSE, 0, NULL, NULL, &strtinfo, &processInfo))
		{
			::ShowWindow(::GetConsoleWindow(), SW_HIDE);
			while (WaitForSingleObject(processInfo.hProcess, INFINITE))
			{
				std::cout << ".";
				Sleep(800);
			}
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			return true;
		}
		else
			return false;
	}
}

bool doCmdCommandInNewWindow(std::string command, std::string running_exe)
{
	{

		std::wstring thepath_w;
		//thepath_w.append(L"start cmd.exe /c ");
		thepath_w.append(command.begin(), command.end());
		LPWSTR thepath_final = &thepath_w[0];

		std::wstring theexe_w;
		theexe_w.append(running_exe.begin(), running_exe.end());
		LPWSTR theexe_final = &theexe_w[0];
		if (running_exe == "")
			theexe_final = NULL;

		STARTUPINFO strtinfo = { sizeof(strtinfo) };
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&strtinfo, sizeof(strtinfo));

		//CREATE_NO_WINDOW |
		if (CreateProcess(theexe_final, thepath_final, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &strtinfo, &processInfo))
		{
			::ShowWindow(::GetConsoleWindow(), SW_HIDE);
			while (WaitForSingleObject(processInfo.hProcess, INFINITE)) //we maybe do not want to wait here since we detached the process.. needs some experiments
			{
				std::cout << ".";
				Sleep(800);
			}

			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			return true;
		}
		else
			return false;
	}
}

bool killProcessByName(const wchar_t* filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32W pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32FirstW(hSnapShot, &pEntry);
	bool killed = false;

	DWORD currentProcessID = GetProcessId(GetCurrentProcess());

	std::vector<DWORD>process_id_vec;

	while (hRes)
	{

		if (lstrcmpW(pEntry.szExeFile, filename) == 0)
		{
			
			if(std::find(process_id_vec.begin(), process_id_vec.end(), (DWORD)pEntry.th32ProcessID) != process_id_vec.end())
			{
				break;
			}

			process_id_vec.push_back((DWORD)pEntry.th32ProcessID);

			if (currentProcessID == (DWORD)pEntry.th32ProcessID)
			{
				continue;
			}
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
				killed = true;
			}


		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	return killed;
}


HANDLE GetProcessByName(PCSTR name)
{
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			if (std::string(std::wstring(process.szExeFile).begin(), std::wstring(process.szExeFile).end()) == std::string(name))
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	if (pid != 0)
	{
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	}

	// Not found
	return NULL;
}

//only detects processes with windows opened
bool isRunning(LPCSTR pName)
{
	HWND hwnd;
	hwnd = FindWindowA(NULL, pName);
	if (hwnd != 0) {
		return true;
	}
	else {
		return false;
	}
}


void LoadFileInResource(int name, int type, DWORD& size, const char*& data)
{
	HMODULE handle = ::GetModuleHandle(NULL);
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name),
		MAKEINTRESOURCE(type));
	HGLOBAL rcData = ::LoadResource(handle, rc);
	size = ::SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

std::string fixPath(std::string theString)
{
	std::string returnedString = "";
	for (int it = 0; it < theString.length(); it++)
	{
		if (theString.at(it) != '/')
			returnedString += theString.at(it);
		else returnedString += '\\';
	}
	return returnedString;
}



int createWindowsMessage(std::string thestring)
{
	std::wstring message(thestring.begin(), thestring.end());

	return MessageBoxW(NULL, message.c_str(), L"!!HINWEIS!!", MB_OK | MB_ICONQUESTION);
}

//return: yes==6 no==7 cancel==2 abort==3
int createWindowsChoice(std::string thestring)
{
	std::wstring message(thestring.begin(), thestring.end());

	return MessageBoxW(NULL, message.c_str(), L"!!HINWEIS!!", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);
}

int createWindowsError(std::string thestring)
{
	std::wstring message(thestring.begin(), thestring.end());

	return MessageBoxW(NULL, message.c_str(), L"ERROR", MB_ICONERROR);
}

void killProgrammMessage()
{
	std::string thestring = "Programm got prematurely exitted by the user.";
	std::wstring message(thestring.begin(), thestring.end());

	MessageBoxW(NULL, message.c_str(), L"ERROR", MB_OK | MB_ICONEXCLAMATION);
	//exit;
}

void startSQLService(std::string servicename)
{
	const char* x;
	std::string y = "net start ";
	y.append(servicename);
	x = (y.c_str());
	system(x);
}
void stopSQLService(std::string servicename)
{
	const char* x;
	std::string y = "net stop ";
	y.append(servicename);
	x = (y.c_str());
	system(x);
}

bool fix_connectionphp(std::string projectpath, std::string username, std::string password, std::string port)
{
	std::vector<std::string> vec_connnection_php =
	{
		"<?php",
		"$link = mysqli_connect(",
		"\"localhost\"\,",
		"\"" + username +"\"\,",
		"\"" + password + "\"\,",
		"\"MehrMarktDatabase\"\,",
		"\"" + port + "\"",
		");",
		"if (!$link) {",
		"echo \"Verbindung fehlgeschlagen : \"\, mysqli_connect_error();",
		"exit();",
		"}",
		"$link-> set_charset(\"utf8\");"
	};
	std::ofstream output_connectionfile;
	output_connectionfile.open(projectpath + "\\connection.php", std::ofstream::out | std::ofstream::trunc);

	for (int i = 0; i < vec_connnection_php.size(); i++)
	{
		output_connectionfile << vec_connnection_php[i] << std::endl;
	}
	output_connectionfile.close();

}
//
//std::string initProjectFiles()
//{	//powershell.exe Expand-Archive .\project.zip -DestinationPath .\
//
//
//
//	CHAR my_documents[MAX_PATH];
//	HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
//
//
//
//	if (std::filesystem::exists(std::string(my_documents) + "\\SWE-Software"))
//	{
//		return (std::string(my_documents) + "\\SWE-Software");
//	}
//
//	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDB_PHP), __TEXT("BINARY"));
//	HGLOBAL hGlobal = LoadResource(NULL, hResource);
//	size_t exeSiz = SizeofResource(NULL, hResource);
//	void* exeBuf = LockResource(hGlobal);
//
//
//
//	DWORD at1;
//	HANDLE hFile = CreateFile(L"project.zip", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // Check MSDN for more info
//	BOOL success = WriteFile(hFile, exeBuf, exeSiz, &at1, NULL); // If it fails  -> success = 0
//
//
//	CloseHandle(hFile);
//
//	if (!success)
//	{
//		createWindowsError("couldn't unpack project.zip, though its needed for the programm\d\nProgram will now Exit.");
//		exit;
//	}
//
//
//
//	//this f conversion stuff.. string -> LPWSTR
//	// powershell.exe "Expand - Archive .\project.zip - DestinationPath C:\Users\IEUser\Documents\SWE-Software"
//	std::string my_documents_s = std::string("powershell -command \"Expand-Archive .\\project.zip -DestinationPath " + std::string(my_documents) + "\\SWE-Software\"");
//	std::wstring my_documents_ws;
//	my_documents_ws.assign(my_documents_s.begin(), my_documents_s.end());
//	LPWSTR my_documents_final = &my_documents_ws[0];
//	//std::cout << "powershell -command \"Expand-Archive .\\project.zip -DestinationPath " + std::string(my_documents) + "\\SWE-Software\"";
//	system(my_documents_s.c_str());
//	Sleep(6000);
//	DeleteFileA("project.zip");
//
//
//	return std::string(my_documents) + "\\SWE-Software";
//}

std::string findMaria()
{
	TCHAR pf[MAX_PATH];

	SHGetSpecialFolderPath(
		0,
		pf,
		CSIDL_PROGRAM_FILES,
		FALSE);

	auto iter = std::filesystem::recursive_directory_iterator(pf, std::filesystem::directory_options::skip_permission_denied);
	auto end_iter = std::filesystem::end(iter);
	auto ec = std::error_code();
	std::filesystem::directory_entry dirEntry;
	try {
		for (; iter != end_iter; iter.increment(ec))
		{
			if (ec)
			{
				std::cout << "\n" << ec << "\n";
				continue;
			}
			else if (!iter->is_regular_file() || iter->is_symlink())
			{
				continue;
			}
			dirEntry = std::filesystem::directory_entry(iter->path());
			//std::cout << "searching in" << dirEntry.path() << "\n";



			if (dirEntry.path().filename() == L"mariadb.exe")
			{
				std::cout << "Found Maria in:" + dirEntry.path().parent_path().string() << "\n";
				return dirEntry.path().parent_path().string();
			}


		}
	}
	catch (std::filesystem::filesystem_error error_ec)
	{
		createWindowsError(ec.message());
	}
	return "";
}

void setCredentials(std::string projectpath, std::string& username, std::string& password, std::string& port, bool& didloadcredentials)
{
	std::ofstream outputfile;
	outputfile.open(projectpath + "\\credentials_do_not_delete.txt", std::ofstream::out | std::ofstream::trunc);

	outputfile << username << std::endl << password << std::endl << port;
	outputfile.close();
	didloadcredentials = false;
}
void loadCredentials(std::string projectpath, std::string& username, std::string& password, std::string& port, bool& didloadcredentials)
{
	if (std::filesystem::exists(projectpath + "\\credentials_do_not_delete.txt"))
	{

		std::ifstream credential_file;
		credential_file.open(projectpath + "\\credentials_do_not_delete.txt");

		if (!credential_file || !credential_file.is_open() || !credential_file.good())
		{
			createWindowsError("Couldnt open credentials_do_not_delete.txt! The project may not gain access to the Database. Programm will continue.");
			return;
		}

		std::string checkedString;
		std::vector<std::string> vecOfStr;

		while (std::getline(credential_file, checkedString)) // Every line of the inputfile gets its own string in the vector
		{
			vecOfStr.push_back(checkedString);
		}

		username = vecOfStr.at(0);
		password = vecOfStr.at(1);
		port = vecOfStr.at(2);

		std::cout << username << "\n";
		std::cout << password << "\n";
		std::cout << port << "\n";

		credential_file.close();
		didloadcredentials = true;
		return;
	}
	else
	{
		std::ofstream outputfile;
		outputfile.open(projectpath + "\\credentials_do_not_delete.txt", std::ofstream::out | std::ofstream::trunc);

		outputfile << username << std::endl << password << std::endl << port;
		outputfile.close();
		didloadcredentials = false;

	}
}