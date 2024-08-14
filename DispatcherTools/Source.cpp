#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <cctype>
using namespace std;

int help();
int interfaces();
int allprocess();
int clear();
int logo();
int info();

int processkill(const wstring& processName);
int processkillByID(DWORD processID);
int searchprocess(const wstring& processName);

string getlogo(ifstream& file);
string trim(const string& str);
bool isNumber(const string& str);

int main() { interfaces(); return 0; }

int interfaces() {
    logo();
    cout << "Input /help to view all commands \n";
    while (true) {
        string enter;
        cout << "Enter a command: \n>> ";
        getline(cin, enter);
        enter = trim(enter);
        cout << endl;

        if (enter == "/help") help();
        else if (enter.substr(0, 6) == "/kill ") {
            string argument = trim(enter.substr(6));
            if (isNumber(argument)) {
                DWORD processID = stoi(argument);
                processkillByID(processID);
            }
            else {
                wstring processName = wstring(argument.begin(), argument.end());
                processName.erase(0, processName.find_first_not_of(L" "));
                processkill(processName);
            }
        }
        else if (enter.substr(0, 8) == "/search ") {
            wstring processName = wstring(enter.begin() + 8, enter.end());
            processName.erase(0, processName.find_first_not_of(L" "));
            searchprocess(processName);
        }
        else if (enter == "/all") allprocess();
        else if (enter == "/clear") clear();
        else if (enter == "/info") info();
        else cout << "ERROR: Unknown command. Try again \n" << endl;
    }
}

int help() {
    cout << "1. /help - all commands \n2. /kill process_name or process_id - forced termination of the process";
    cout << "\n3. /search process_name - search for a running process \n4. /all - task manager parsing";
    cout << "\n5. /clear - clear console \n6. /info - information about the app and the creator \n" << endl;
    return 0;
}

int info() {
    cout << "Name: Dispatcher Tools" << endl;
    cout << "Creator: Edelways (JeffPharaon)" << endl;
    cout << "License: MatteDair Studio" << endl;
    cout << "Version: 0.0.1 Release" << endl;
    cout << "Platform: Windows 8 - 11" << endl;
    cout << "More: https://github.com/jeffpharaon \n" << endl;
    return 0;
}

int processkill(const wstring& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        wcerr << L"ERROR: Error getting the list of processes \n" << endl;
        return 1;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    bool processFound = false;

    if (Process32First(snapshot, &entry)) {
        do {
            wstring exeFileName = entry.szExeFile;
            if (processName == exeFileName) {
                processFound = true;
                HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                if (processHandle != NULL) {
                    if (TerminateProcess(processHandle, 0))
                        wcout << L">> Process " << processName << L" was killed \n" << endl;
                    else wcerr << L"ERROR: The process cannot be completed: " << processName << "\n";
                    CloseHandle(processHandle);
                }
                else wcerr << L"ERROR: Process opening error: " << processName << "\n";
            }
        } while (Process32Next(snapshot, &entry));
    }
    else wcerr << L"ERROR: An error occurred while retrieving the list of processes \n" << endl;

    if (!processFound) wcerr << L"ERROR: The process " << processName << L" was not found \n" << endl;

    CloseHandle(snapshot);
    return 0;
}

int processkillByID(DWORD processID) {
    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
    if (processHandle != NULL) {
        if (TerminateProcess(processHandle, 0))
            wcout << L">> Process with ID " << processID << L" was killed \n" << endl;
        else wcerr << L"ERROR: The process cannot be completed: " << processID << "\n";
        CloseHandle(processHandle);
    }
    else wcerr << L"ERROR: Process opening error: " << processID << "\n";
    return 0;
}

int searchprocess(const wstring& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        wcerr << L"ERROR: Error getting the list of processes \n" << endl;
        return 1;
    }
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    bool processFound = false;

    if (Process32First(snapshot, &entry)) {
        do {
            wstring exeFileName = entry.szExeFile;
            if (processName == exeFileName) {
                processFound = true;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    else wcerr << L"ERROR: An error occurred while retrieving the list of processes \n" << endl;

    if (processFound) wcout << L">> Process " << processName << L" is running \n" << endl;
    else wcout << L">> Process " << processName << L" is not running \n" << endl;

    CloseHandle(snapshot);
    return 0;
}

int allprocess() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        wcerr << L"ERROR: Error getting the list of processes \n" << endl;
        return 1;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snapshot, &entry)) {
        wcout << L">> List of all running processes: \n";
        do {
            wcout << L"Process ID: " << entry.th32ProcessID << L", Name: " << entry.szExeFile << endl;
        } while (Process32Next(snapshot, &entry));
    }
    else wcerr << L"ERROR: An error occurred while retrieving the list of processes \n" << endl;

    CloseHandle(snapshot);
    cout << endl;
    return 0;
}

int clear() {
    system("cls");
    interfaces();
    return 0;
}

int logo() {
    system("chcp 65001");
    cout << endl;
    ifstream reader("Logo.txt");
    if (!reader) return 1;

    string art = getlogo(reader);
    cout << art << endl;
    reader.close();
    return 0;
}

string getlogo(ifstream& file) {
    string lines = "";

    if (file) {
        while (file.good()) {
            string TempLine;
            getline(file, TempLine);
            TempLine += "\n";
            lines += TempLine;
        }
    }
    return lines;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

bool isNumber(const string& str) {
    return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
}
