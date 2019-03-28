// FIlosofos Y Threads.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <vector>
#include "CFilosofo.h"


int main()
{
	CFilosofo filosofos[5];

	const uint8_t CountFilosofos = ARRAYSIZE(filosofos);

	char Name1[] = "Friedrich Nietzsche";
	char Name2[] = "Aristotle";
	char Name3[] = "Doomer";
	char Name4[] = "Gandhi";
	char Name5[] = "Homer";

	filosofos[0].SetName(Name1);
	filosofos[1].SetName(Name2);
	filosofos[2].SetName(Name3);
	filosofos[3].SetName(Name4);
	filosofos[4].SetName(Name5);

	//This is so that the philosophers can interact with each-other
	for (int i = 0; i < CountFilosofos; ++i)
	{
		if (i == 0)
		{
			filosofos[0].SetRightHandFilosofo(&filosofos[1]);
			filosofos[0].SetLeftHandFilosofo(&filosofos[CountFilosofos - 1]);
		}
		else if (i == (CountFilosofos - 1))
		{
			filosofos[i].SetRightHandFilosofo(&filosofos[0]);
			filosofos[i].SetLeftHandFilosofo(&filosofos[i - 1]);
		}
		else
		{
			filosofos[i].SetRightHandFilosofo(&filosofos[i + 1]);
			filosofos[i].SetLeftHandFilosofo(&filosofos[i - 1]);
		}
	}



	// For Solving disputes
	std::vector<int8_t>ValoresDeImportancia = { 4,1,0,2,3 };

	for (char i = 0; i < 5; ++i)
	{
		filosofos[i].SetPriorityLevel(ValoresDeImportancia[i]);
	}

	HANDLE Threads[5];
	DWORD ThreadIDs[5];

	for (int i = 0; i < 5; ++i)
	{
		Threads[i] = CreateThread(NULL, 0,
															reinterpret_cast<LPTHREAD_START_ROUTINE> (&CFilosofo::InitFilosofo),
															reinterpret_cast<LPVOID*>(&filosofos[i]), 0,
															&ThreadIDs[i]);
	}


	WaitForMultipleObjects(5, Threads, true, INFINITE);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
