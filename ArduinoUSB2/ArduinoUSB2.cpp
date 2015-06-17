// ArduinoUSB2.cpp : main project file.

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <Windows.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace System::IO::Ports;
using SysString = System::String;
using namespace std;
using namespace msclr::interop;

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

void arduino_tx(SerialPort^ arduino, SysString^ message)
{
	arduino->WriteLine(message);
	return;
}

SysString^ arduino_rx(SerialPort^ arduino, int timeout = SerialPort::InfiniteTimeout)
{
	SysString^ serial_response;
	SysString^ main_response;

	arduino->ReadTimeout = timeout;
	try
	{
		do
		{
			serial_response = arduino->ReadLine();
			if (SysString::IsNullOrEmpty(main_response))
			{
				main_response = serial_response;
			}

		} while (!serial_response->Equals("ok\r") && !serial_response->Contains("error:"));
	}
	catch (TimeoutException^ exception)
	{
		arduino->ReadTimeout = SerialPort::InfiniteTimeout;
		throw exception;
	}

	arduino->ReadTimeout = SerialPort::InfiniteTimeout;
	return main_response;
}
SysString^ arduino_tx_rx(SerialPort^ arduino, SysString^ serial_message)
{
	// Writes message to serial port and returns the response. Clears serial buffer of subsequent "ok" or "errors" (for now)

	// TODO Error handling 

	arduino->WriteLine(serial_message);

	SysString^ serial_response;
	SysString^ mainResponse; // This will be the one actually returned. Won't bother returning "ok"
	do
	{
		serial_response = arduino->ReadLine();
		if (SysString::IsNullOrEmpty(mainResponse))
		{
			mainResponse = serial_response;
		}

	} while (!serial_response->Equals("ok\r") && !serial_response->Contains("error:"));

	return mainResponse;
}

SysString^ query_grbl_status(SerialPort^ arduino)
{
	SysString^ response;
	bool ok_flag = false;
	
	arduino->WriteLine("?");
	do
	{
		response = arduino->ReadLine();
		if (response->Equals("ok\r"))
		{
			ok_flag = true;
		}
	} while (response->Equals("ok\r"));

	return response;
}

void ClearArduinoSerialBuf(SerialPort^ arduino)
{
	SysString^ response;
	do
	{
		response = arduino->ReadLine();
	} while (!response->Equals("ok\r") && !response->Contains("error:"));
}

int main()//array<System::String ^> ^args)
{
	ofstream timefile;
	timefile.open("timefile.txt");
	ofstream serialfile;
	serialfile.open("serialfile.txt");
	ofstream commandfile;
	commandfile.open("command_times.txt");

	StartCounter();
	double timer = GetCounter();
	double command_timer = GetCounter();
	double query_timer = GetCounter();
	double move_timer = GetCounter();

	SysString^ portName="com5";
	SysString^ response;
	int baudRate = 150000; // 115200;
	//Console::WriteLine("Type in a port name and hit ENTER");
	//portName = Console::ReadLine();

	SerialPort^ arduino;
	arduino = gcnew SerialPort(portName, baudRate);
	arduino->Open();

	// Set idle delay to none
	arduino_tx_rx(arduino, "$1=255");
	// Set status report mask
	arduino_tx_rx(arduino, "$10=2");

	// Set accelerations
	arduino_tx_rx(arduino, "$120=10");
	arduino_tx_rx(arduino, "$121=10");

	SysString^ input;

	long double radius = 10;
	long double x_command = 2;
	long double y_command = 2;
	long double x_pos[2] = { 1, 1 };
	long double y_pos[2] = { 0, 0 };
	int step = 0;
	int counter = 1000;
	int test_mode = 1; // 0 = No motion; 1 = Quickly back and forth; 2 = circular
	bool motor_idle = true;
	bool motor_idle_reset = true; // This flag is set to false after each command is sent, and then set to true again once the motor is running. This prevents multiple commands from being sent before the next move has started.
	bool first_query = true;
	while (counter > 0)
	{
		
		if (motor_idle == true)
		{
			switch (test_mode)
			{
			case 0:
				break;
			case 1:
				x_command *= -1;
				y_command *= -1;
				break;
			case 2:
				x_pos[0] = x_pos[1];
				y_pos[0] = y_pos[1];
				x_pos[1] = radius*cos(double(step) / 1000 * 2 * 3.14159);
				y_pos[1] = radius*sin(double(step) / 1000 * 2 * 3.14159);
				x_command = x_pos[1] - x_pos[0];
				y_command = y_pos[1] - y_pos[0];
				step++;
				break;
			}

			input = "G0 X" + x_command + " Y" + y_command;

			command_timer = GetCounter();
			arduino_tx(arduino, input);
			commandfile << GetCounter() - move_timer << "\n";
			motor_idle = false;
			move_timer = GetCounter();
		}
		/*
		do
		{
			response = arduino->ReadLine();
			if (response->Contains("error:"))
			{
				Console::WriteLine(response);
				getchar();
				break;
			}
		} while (!response->Equals("ok\r") && !response->Contains("error:"));
		*/

		SysString^ response;
		while (GetCounter() - query_timer < 0)
		{
		}
		//response = query_grbl_status(arduino);
		if (first_query == false)
		{
			do{
				response = arduino_rx(arduino);
				string response_str = marshal_as<string>(response);
				timefile << GetCounter() - query_timer << ", " << GetCounter() - move_timer << ", " << response_str << "\n";
				query_timer = GetCounter();
			} while (response->Equals("ok\r"));

			if (response->Contains("Idle") && motor_idle_reset == true)
			{
				motor_idle = true;
				motor_idle_reset = false;
			}
			else if (response->Contains("Run"))
			{
				motor_idle_reset = true;
			}
		}

		arduino_tx(arduino, "?");
		first_query = false;
	}

	timefile.close();
	Console::Clear();
	
	arduino->Close();

	return 0;
}

