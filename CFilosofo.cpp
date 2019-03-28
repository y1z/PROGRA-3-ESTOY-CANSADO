#include "pch.h"
#include "CFilosofo.h"


CFilosofo::CFilosofo()
{
	m_MutexPropio = CreateMutex(NULL, false, NULL);
	if (m_MutexPropio == NULL)
	{
		GetLastError();
		assert(m_MutexPropio != NULL, "ERROR : Failed Mutex creation");
	}

	m_Semaforo = CreateSemaphore(NULL, 1, 1, NULL);
	if (m_Semaforo == NULL);
	{
		GetLastError();
		assert(m_Semaforo != NULL, "ERROR : Failed Semaphore creation");
	}
}


CFilosofo::~CFilosofo()
{
	if (m_MutexPropio != NULL)
	{
		CloseHandle(m_MutexPropio);
	}
	if (m_Semaforo != NULL)
	{
		CloseHandle(m_Semaforo);
	}

}

/*! This is so I can later tell them apart*/
void CFilosofo::SetName(char * GivenName)
{
	int16_t Lenght = strlen(GivenName);
	if (Lenght < 50)
	{
		for (int16_t i = 0; i < Lenght; ++i)
		{
			m_name[i] = GivenName[i];
		}
	}
	else
	{
		strncpy_s(m_name, "SATANAS", sizeof("SATANAS"));
	}

}

void CFilosofo::SetRightHandFilosofo(CFilosofo * OtherFilosofo)
{
	this->mptr_RightHandFilosofo = OtherFilosofo;
}

void CFilosofo::SetLeftHandFilosofo(CFilosofo * OtherFilosofo)
{
	this->mptr_LeftHandFilosofo = OtherFilosofo;
}

void CFilosofo::SetPriorityLevel(int8_t Priority)
{
	this->m_Prioridad = Priority;
}

void CFilosofo::CheckDinied(CFilosofo *Other)
{
	DiniedCount++;

	mptr_DeniedFilosofer = Other;

	if(DiniedCount > 3)
	{
		AddPromise(Other);
		DiniedCount = 0;
	}

}

/*! checks for possible errors with mutex */
DWORD CFilosofo::IsMutexOK(DWORD Status)
{
	if (Status != WAIT_OBJECT_0)
	{
		GetLastError();
		return false;
	}
	else
	{
		return true;
	}

	return false;
}

bool CFilosofo::TakeFork()
{
	bool isForkGiven = false;

	if (m_CountMutex > 0 && !(m_CountMutex == 2))
	{
		isForkGiven = mptr_RightHandFilosofo->GiveFork(this);
	}


	return isForkGiven;
}

HANDLE& CFilosofo::GetMutexRef()
{
	return m_MutexPropio;
}

bool CFilosofo::GiveFork(CFilosofo *AskingFilosofo)
{


	DWORD Result = WaitForSingleObject(AskingFilosofo->GetMutexRef(), INFINITE);
	/*! So I can know what happens*/
	constexpr const char *DeniedMsg = ">> Denied <<";
	constexpr const char *GivenMsg = ">> Given <<";
	constexpr const char *PromisedMsg = ">> Promised <<";

	// Denied Condition 
	if (this->m_CountMutex == 0)
	{
		printf("Asking [%s] ASKED {%s} was %s \n", AskingFilosofo->m_name, this->m_name, DeniedMsg);
		CheckDinied(AskingFilosofo);

		Sleep(152);
		ReleaseMutex(AskingFilosofo->GetMutexRef());
		return false;
	}
	// take Condition
	else if (this->m_Prioridad > AskingFilosofo->m_Prioridad && this->m_EstadoActual == State::Pensar)
	{
		if (AskingFilosofo->m_CountMutex > 0 && this->m_CountMutex < 2)
		{
			this->AddCountMutex();
			AskingFilosofo->SubtractCountMutex();
			if (AddPromise(AskingFilosofo))
			{
				printf("Asking [%s] ASKED {%s} was %s \n", AskingFilosofo->m_name, this->m_name, PromisedMsg);
				ReleaseMutex(AskingFilosofo->GetMutexRef());
				return false;
			}
			else
			{
				printf("Asking [%s] ASKED {%s} was %s %s\n", AskingFilosofo->m_name, this->m_name, DeniedMsg, PromisedMsg);
			}
		}

	}
	// Promised Condition
	else if (this->m_Prioridad > AskingFilosofo->m_Prioridad && this->m_EstadoActual == State::Comer)
	{
		// see if we already made a promise 
		if (AddPromise(AskingFilosofo))
		{
			printf("Asking [%s] ASKED {%s} was %s \n", AskingFilosofo->m_name, this->m_name, PromisedMsg);
			ReleaseMutex(AskingFilosofo->GetMutexRef());
			return false;
		}
		else
		{
			printf("Asking [%s] ASKED {%s} was %s %s\n", AskingFilosofo->m_name, this->m_name, DeniedMsg, PromisedMsg);
		}

	}
	// given condition 
	else if (CheckForPromiseExists(AskingFilosofo))
	{
		ComplatePromise(AskingFilosofo);
		printf("\t[%s] ASKED {%s} was %s \n", AskingFilosofo->m_name, this->m_name, GivenMsg);
		return true;
	}

	//printf("[%s] ASKED {%s} was %s \n", OtherFilosofo->m_name, this->m_name, PromisedMsg);


	if (IsMutexOK(Result));

	Sleep(200);

	ReleaseMutex(AskingFilosofo->GetMutexRef());
	return false;
}


bool CFilosofo::CheckForPromiseExists(CFilosofo * OtherFilosofo)
{
	WaitForSingleObject(m_Semaforo, 300);
	if (m_PromistedQueue[0] == OtherFilosofo
			|| m_PromistedQueue[1] == OtherFilosofo)
	{
		ReleaseSemaphore(m_Semaforo, 1, NULL);
		return true;
	}

	ReleaseSemaphore(m_Semaforo, 1, NULL);
	return false;
}



bool CFilosofo::ComplatePromise(CFilosofo * OtherFilosofo)
{
	if (CheckForPromiseExists(OtherFilosofo))
	{
		if (m_PromistedQueue[0] == OtherFilosofo
				&& OtherFilosofo->m_CountMutex < 2)
		{
			//Trasforing Mutexs
			this->SubtractCountMutex();
			this->SubtractCountMutex();
			this->m_PromistedQueue[0] = nullptr;

			OtherFilosofo->AddCountMutex();
			OtherFilosofo->AddCountMutex();

			OtherFilosofo->AddPromise(this);
			return true;
		}
		else if (m_PromistedQueue[1] == OtherFilosofo
						 && OtherFilosofo->m_CountMutex < 2)
		{
			//Trasforing Mutexs
			this->SubtractCountMutex();
			this->SubtractCountMutex();
			this->m_PromistedQueue[1] = nullptr;

			OtherFilosofo->AddCountMutex();
			OtherFilosofo->AddCountMutex();

			OtherFilosofo->AddPromise(this);
			return true;
		}

	}

	return false;
}

void CFilosofo::ExecutePromise()
{
	// check right side first
	if (isFirstGivenToRight)
	{
		if (m_PromistedQueue[1] != nullptr)
		{
			ComplatePromise(m_PromistedQueue[1]);
			m_PromistedQueue[1] = nullptr;
		}
		else if (m_PromistedQueue[0] != nullptr)
		{
			ComplatePromise(m_PromistedQueue[0]);
			m_PromistedQueue[0] = nullptr;
		}
		isFirstGivenToRight = false;
	}
	else
	{
		if (m_PromistedQueue[0] != nullptr)
		{
			ComplatePromise(m_PromistedQueue[0]);
			m_PromistedQueue[0] = nullptr;
		}
		else if (m_PromistedQueue[1] != nullptr)
		{
			ComplatePromise(m_PromistedQueue[1]);
			m_PromistedQueue[1] = nullptr;
		}
		isFirstGivenToRight = true;
	}

}

bool CFilosofo::AddCountMutex()
{
	WaitForSingleObject(m_Semaforo, 300);
	if (m_CountMutex < 2)
	{
		m_CountMutex++;
		ReleaseSemaphore(m_Semaforo, 1, NULL);
		return true;
	}
	return false;


	if (m_CountMutex < 0)
	{
		assert(m_CountMutex == 432948328947, "ERROR : Negative Mutex Count ");
	}
	if (!(this->m_CountMutex <= 2))
	{
		printf("El codicioso es[{%s]} y sus mutexts son %d \n", this->m_name, this->m_CountMutex);
		assert(this->m_CountMutex <= 2, "ERROR :  Mutex 2 many Mutex ");
	}

	
	ReleaseSemaphore(m_Semaforo, 1, NULL);

}

bool CFilosofo::AddPromise(CFilosofo *OtherFilosofo)
{
	WaitForSingleObject(m_Semaforo, 300);
	// checks if any spaces are empty
	// and that there are no duplicates 
	if (m_PromistedQueue[0] == nullptr
			&& m_PromistedQueue[1] != OtherFilosofo)
	{
		m_PromistedQueue[0] = OtherFilosofo;
		ReleaseSemaphore(m_Semaforo, 1, NULL);
		return true;
	}
	else if (m_PromistedQueue[1] == nullptr
					 && m_PromistedQueue[0] != OtherFilosofo)
	{
		m_PromistedQueue[1] = OtherFilosofo;
		ReleaseSemaphore(m_Semaforo, 1, NULL);
		return true;
	}

	ReleaseSemaphore(m_Semaforo, 1, NULL);
	return false;
}

void CFilosofo::SubtractCountMutex()
{
	WaitForSingleObject(m_Semaforo, 300);
	if (m_CountMutex != 0)
	{
		 m_CountMutex--;
	}

	assert(m_CountMutex >= 0, "ERROR : Negative Mutex Count ");
	ReleaseSemaphore(m_Semaforo, 1, NULL);
}

void CFilosofo::ChangeState()
{

	if (m_EstadoActual == State::Init)
	{
		this->m_CountMutex = 1;
		this->m_EstadoActual = State::Pensar;
	}
	else if (m_EstadoActual == State::Pensar && m_CountMutex > 1)
	{
		this->m_EstadoActual = State::Comer;
	}
	else if (m_EstadoActual == State::Comer && m_CountMutex < 2)
	{
		this->m_EstadoActual = State::Pensar;
	}
	else
	{

		if (m_EstadoActual == State::Pensar)
		{
		}
		else if (m_EstadoActual == State::Comer)
		{
		}
		else
		{
			fprintf(stderr, "----------ERROR: UNKONW STATE ");
		}
	}

}

void CFilosofo::Comer()
{
	if (m_CountMutex > 1)
	{
		WaitForSingleObject(m_MutexPropio, 500);
		printf("Eating : [%s] Total <<<%d>>> \n", m_name, m_CountEating);
		m_CountEating++;
		Sleep(200);
		ReleaseMutex(m_MutexPropio);
	}
	else
	{
		fprintf(stderr, "------------------------ERROR: IN EATING STATE WITH LESS THAT 2 FORKS");
	}

}

void CFilosofo::Pensar()
{
	DWORD Result;
	if (m_CountMutex > 0)
	{
		Result = WaitForSingleObject(m_MutexPropio, 1000);

		printf("Think : [%s] \n", m_name);

		Sleep(100);

	}
	else
	{
		printf("Think : [%s] \n", m_name);
	}
	Sleep(200);
	ReleaseMutex(m_MutexPropio);


}



void CFilosofo::Comportamiento()
{
	while (true)
	{
		switch (m_EstadoActual)
		{
			case(State::Init):
				/*This represents a philosopher getting the fork on there left*/
				ChangeState();
				break;

			case(State::Pensar):
				Pensar();
				TakeFork();
				ChangeState();
				break;

			case(State::Comer):
				Comer();
				ExecutePromise();
				ChangeState();
				break;

			default:
				fprintf(stderr, "Como chigados ocurrio esto ", m_name);
				break;
		}

	}

}

DWORD __stdcall CFilosofo::InitFilosofo(LPVOID *param)
{
	CFilosofo* EsteFilosofo = reinterpret_cast<CFilosofo*>(param);

	EsteFilosofo->Comportamiento();

	return 0;
}
