#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <cctype>
#include <vector>
#include <Psapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <conio.h>
#include <thread>
#include <mutex>
#include <tchar.h>
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Pdh.lib")
using namespace std;

int help();
int interfaces();
int allprocess();
int clear();
int logo();
int info();
int view();
int github();

int where(const string& processIdentifier);
int check(const string& root);
int processkill(const wstring& processName);
int processkillByID(DWORD processID);
int searchprocess(const wstring& processName);

string getlogo(ifstream& file);
string trim(const string& str);
wstring toLower(const wstring& str);

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
        else if (enter.substr(0, 7) == "/where ") {
            string argument = trim(enter.substr(7));
            where(argument);
        }
        else if (enter.substr(0, 7) == "/check ") {
            string argument = trim(enter.substr(7));
            check(argument);
        }
        else if (enter == "/view") view();
        else if (enter == "/git") github();
        else cout << "ERROR: Unknown command. Try again \n" << endl;
    }
}

int help() {
    cout << "1. /help - all commands \n2. /kill process_name or process_id - forced termination of the process";
    cout << "\n3. /search process_name - search for a running process \n4. /all - task manager parsing";
    cout << "\n5. /where process_name or process_id - show file path of the process";
    cout << "\n6. /check root_word - search processes by root word";
    cout << "\n7. /view - view processes with high RAM and CPU usage";
    cout << "\n8. /clear - clear console \n9. /info - information about the app and the creator";
    cout << "\n10. /git - open the project's github \n" << endl;
    return 0;
}

int info() {
    cout << "Name: Dispatcher Tools" << endl;
    cout << "Creator: Edelways (JeffPharaon)" << endl;
    cout << "License: MatteDair Studio" << endl;
    cout << "Version: 0.0.4 Release" << endl;
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
        else wcerr << L"ERROR: The process cannot be completed: " << processID << "\n" << endl;
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

wstring toLower(const wstring& str) {
    wstring result = str;
    transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}

int where(const string& processIdentifier) {
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
            if ((isNumber(processIdentifier) && entry.th32ProcessID == stoi(processIdentifier)) ||
                (!isNumber(processIdentifier) && wstring(entry.szExeFile) == wstring(processIdentifier.begin(), processIdentifier.end()))) {
                processFound = true;
                HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
                if (processHandle != NULL) {
                    wchar_t processPath[MAX_PATH];
                    if (GetModuleFileNameEx(processHandle, NULL, processPath, MAX_PATH))
                        wcout << L">> Process path: " << processPath << "\n" << endl;
                    else wcerr << L"ERROR: Unable to get the process path \n";
                    CloseHandle(processHandle);
                }
                else wcerr << L"ERROR: Process opening error \n";
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    else wcerr << L"ERROR: An error occurred while retrieving the list of processes \n" << endl;

    if (!processFound) wcerr << L"ERROR: The process was not found \n" << endl;

    CloseHandle(snapshot);
    return 0;
}

mutex mtx;

int check(const string& root) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        wcerr << L"ERROR: Error getting the list of processes \n" << endl;
        return 1;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    wstring rootW = wstring(root.begin(), root.end());
    rootW = toLower(rootW);

    if (Process32First(snapshot, &entry)) {
        wcout << L">> Matching processes: \n";
        do {
            wstring exeFileName = toLower(entry.szExeFile);
            if (exeFileName.find(rootW) != wstring::npos)
                wcout << L"Id: " << entry.th32ProcessID << L" Name: " << entry.szExeFile << endl;
        } while (Process32Next(snapshot, &entry));
    }
    else wcerr << L"ERROR: An error occurred while retrieving the list of processes \n" << endl;

    CloseHandle(snapshot);
    cout << endl;
    return 0;
}

void checkCpuUsage(DWORD processID, const wstring& processName) {
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PdhOpenQuery(NULL, NULL, &cpuQuery);
    wstring counterPath = L"\\Process(" + processName + L")\\% Processor Time";
    PdhAddCounter(cpuQuery, counterPath.c_str(), NULL, &cpuTotal);
    PdhCollectQueryData(cpuQuery);
    Sleep(100);
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

    if (counterVal.doubleValue > 10.0) {
        lock_guard<mutex> lock(mtx);
        wcout << L"Process ID: " << processID << L", Name: " << processName << L", CPU: " << counterVal.doubleValue << L"%" << endl;
    }

    PdhCloseQuery(cpuQuery);
}

void checkMemoryUsage(DWORD processID, const wstring& processName) {
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (processHandle != NULL) {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(processHandle, &pmc, sizeof(pmc))) {
            if (pmc.WorkingSetSize > 200 * 1024 * 1024) {
                lock_guard<mutex> lock(mtx);
                wcout << L"Process ID: " << processID << L", Name: " << processName << L", RAM: " << pmc.WorkingSetSize / (1024 * 1024) << L" MB" << endl;
            }
        }
        CloseHandle(processHandle);
    }
}

int view() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        wcerr << L"ERROR: Error getting the list of processes \n" << endl;
        return 1;
    }
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    vector<thread> threads;

    if (Process32First(snapshot, &entry)) {
        wcout << L">> Processes with high CPU usage: \n";
        do {
            threads.emplace_back(checkCpuUsage, entry.th32ProcessID, entry.szExeFile);
        } while (Process32Next(snapshot, &entry));

        for (auto& t : threads) t.join();

        threads.clear();
        wcout << L">> Processes with high RAM usage: \n";
        if (Process32First(snapshot, &entry)) {
            do {
                threads.emplace_back(checkMemoryUsage, entry.th32ProcessID, entry.szExeFile);
            } while (Process32Next(snapshot, &entry));
        }

        for (auto& t : threads) t.join();

    }
    else wcerr << L"ERROR: An error occurred while retrieving the list of processes \n" << endl;

    CloseHandle(snapshot);
    cout << endl;
    return 0;
}

int github() {
    cout << ">> Open a browser... \n" << endl;
    const char* url = "https://github.com/jeffpharaon/DispatcherTools/tree/master";
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    return 0;
}

