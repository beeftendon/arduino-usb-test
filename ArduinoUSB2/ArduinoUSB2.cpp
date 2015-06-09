// ArduinoUSB2.cpp : main project file.

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <Windows.h>

using namespace System;
using namespace System::IO::Ports;
using namespace std;

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

int main(array<System::String ^> ^args)
{
	ofstream timefile;
	timefile.open("timefile.txt");
	ofstream serialfile;
	serialfile.open("serialfile.txt");

	String^ answer;
	String^ portName;
	String^ response;
	int baudRate = 115200;
	Console::WriteLine("Type in a port name and hit ENTER");
	portName = Console::ReadLine();

	SerialPort^ arduino;
	arduino = gcnew SerialPort(portName, baudRate);
	arduino->Open();

	String^ input;
	while (true)
	{

		input = Console::ReadLine();

		if (input->Equals("exit")) break;

		arduino->WriteLine(input);
		response = arduino->ReadLine();
		while (!String::IsNullOrEmpty(response))
		{
			Console::WriteLine(response);
			response = arduino->ReadLine();
		}

		timefile << GetCounter() << "\n";
	}

	timefile.close();
	Console::Clear();
	
	arduino->Close();

	return 0;
}
