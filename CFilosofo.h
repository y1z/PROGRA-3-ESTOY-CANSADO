#pragma once
#include <Windows.h>
#include <iostream>
#include <cinttypes>

/*!
All philosopher take the Forks thats on there left side first 
*/
class CFilosofo
{
public:
	CFilosofo();
	~CFilosofo();

public:// variables 

	//char *m_name = "SATANS";	
	char m_name[50] = "";

	/*!This represent how many time the philosopher 
	has takaing a bite*/
	uint32_t m_CountEating = 0;

	bool isFirstGivenToRight = true;

	bool isCheckingRight = true;

	int DiniedCount = 0;

	/*!this represents the Fork an the left */
	HANDLE m_MutexPropio;

	/*! For handling of important data*/
	HANDLE m_Semaforo;
	/*!This represents the amount of Forks*/
	int16_t m_CountMutex = 0;

	/*! This variable helps solves disputes*/
	int8_t m_Prioridad = 0;

	CFilosofo *mptr_RightHandFilosofo = nullptr;
	CFilosofo *mptr_LeftHandFilosofo = nullptr;

	/*! to Know which philosopher This one made a Promise to*/
	std::array<CFilosofo*, 2> m_PromistedQueue = { nullptr,nullptr };

	CFilosofo *mptr_DeniedFilosofer = nullptr;

	enum class State
	{
		NONE,
		Init,
		Comer,
		Pensar
	};
	/*! Dictates What it will do */
	State m_EstadoActual = State::Init;

public:// functions 
	void SetName(char *Name);

	void SetRightHandFilosofo(CFilosofo *Other);

	void SetLeftHandFilosofo(CFilosofo *Other);

	void SetPriorityLevel(int8_t Priority);
private:

	void CheckDinied(CFilosofo *Other);

	DWORD IsMutexOK(DWORD result);

	bool TakeFork();

	HANDLE& GetMutexRef();

	bool GiveFork(CFilosofo *OtherFilosofo);

	bool CheckForPromiseExists(CFilosofo *OtherFilosofo);

	bool ComplatePromise(CFilosofo *OtherFilosofo);

	void ExecutePromise();

	bool AddCountMutex();

	bool AddPromise(CFilosofo *OtherFilosofo);

	void SubtractCountMutex();

	void ChangeState();

	void Comer();

	void Pensar();
	/*!Dictates what the philosopher does  */
	void Comportamiento();
public:
	static DWORD WINAPI InitFilosofo(LPVOID *param);
};

