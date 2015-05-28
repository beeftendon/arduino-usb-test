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
	String^ response1;
	String^ response2;
	String^ response3;
	int baudRate = 500000;
	Console::WriteLine("Type in a port name and hit ENTER");
	portName = Console::ReadLine();

	SerialPort^ arduino;
	arduino = gcnew SerialPort(portName, baudRate);
	arduino->Open();
	int i = 0;
	do{

		arduino->WriteLine("1");
		arduino->WriteLine("1");
		arduino->WriteLine("1");
		StartCounter();
		response1 = arduino->ReadLine();
		response2 = arduino->ReadLine();
		response3 = arduino->ReadLine();
		Console::WriteLine(response1);
		Console::WriteLine(response2);
		Console::WriteLine(response3);
		timefile << GetCounter() << "\n";
		//serialfile << response1 << ", " << response2 << ", " << response3;
		i++;
		/*
		// ask on or off
		Console::WriteLine("Type \"on\" to turn the light on or \"off\" to turn it off");
		// get answer
		answer = Console::ReadLine();
		//check that user typed one of the options
		if (String::Compare(answer, "on") == 0)
		{
			arduino->WriteLine("1"); // send 1 to arduino
			response = arduino->ReadLine();
			Console::WriteLine(response);
		}
		else if (String::Compare(answer, "off") == 0)
		{
			arduino->WriteLine("0"); // send 0 to arduino
			response = arduino->ReadLine();
			Console::WriteLine(response);
		}
		else
			Console::WriteLine(answer + " was not an option");
		// ask user if he wants to continue
		Console::WriteLine("Try again? yes/no");
		// get answer
		answer = Console::ReadLine();
		// clear the screen
		Console::Clear(); // clear the command line
		*/
	} while (i<1000); // while (String::Compare(answer, "yes") == 0);
	timefile.close();
	Console::ReadLine();
	Console::Clear();

	return 0;
}
