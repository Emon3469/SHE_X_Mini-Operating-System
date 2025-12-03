#ifndef PROCESS_SCHEDULING_ALGORITHMS_H
#define PROCESS_SCHEDULING_ALGORITHMS_H

#define firstTimeRunning(_process) _process.getResponseTime() == -1

#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
#include "Process.h"
#include "simulate.h"

namespace ProcessSchedulingAlgorithms {

	static uint32_t maxProcessMultiprogramming = 0;
    static bool FCFS(std::vector<Process>* waitingQueue, std::vector<Process>* readyQueue, double _elapsedTime) {
        try {
            if (!waitingQueue->empty()) {
                Simulator::DebugLog(_elapsedTime,
                                    "Process " + std::to_string((*waitingQueue)[0].getPID()) + " pronto");

                (*waitingQueue)[0].updateSubmissionTime(_elapsedTime);
                readyQueue->push_back((*waitingQueue)[0]);
                waitingQueue->erase(waitingQueue->begin());
                return true;
            } else return false;
        }
        catch (...) {
            std::cout << "Erro no escalonamento. Nenhum processo escalonado\n";
            return false;
        }
    }

    static bool HRRN(std::vector<Process>* readyQueue, std::vector<Process>* runningList, int* _quantum, double _elapsedTime) {
        try {
            if (!readyQueue->empty()) {
                size_t i = 0, hrr = 0;
                for (auto responseratio = 0.0, maior = 0.0; i < (*readyQueue).size(); i++) {
	                (*readyQueue)[i].setWaitingTime(_elapsedTime);
                    responseratio = 1 + (*readyQueue)[i].getWaitingTime() / (*readyQueue)[i].getExecutionTime();
                    if (responseratio > maior) {
                        maior = responseratio;
                        hrr = i;
                    }
                }

                (*_quantum) = std::numeric_limits<int>::max();
	            if (firstTimeRunning((*readyQueue)[hrr])) (*readyQueue)[hrr].setResponseTime(_elapsedTime);


	            Simulator::DebugLog(_elapsedTime,
	                                "Process " + std::to_string((*readyQueue)[hrr].getPID()) + " running");

                runningList->push_back((*readyQueue)[hrr]);
                readyQueue->erase(readyQueue->begin()+hrr);
                return true;
            } else return false;
        }
        catch (...){
            std::cout << "Erro no escalonamento. Nenhum processo escalonado\n";
            return false;
        }
    }

    static bool PRIORITY(std::vector<Process> *readyQueue, std::vector<Process> *runningList, int* _quantum, double _elapsedTime) {
        try {
            if (!readyQueue->empty()) {
                int priority = (*readyQueue)[0].getPriority();
                size_t maxpriority = 0, i = 0;
                for (Process process: (*readyQueue)) {
                    if (process.getPriority() > priority) {
                        priority = process.getPriority();
                        maxpriority = i;
                    } i++;
                }

	            (*_quantum) = std::numeric_limits<int>::max();
	            if (firstTimeRunning((*readyQueue)[maxpriority])) (*readyQueue)[maxpriority].setResponseTime(_elapsedTime);
	            Simulator::DebugLog(_elapsedTime,
	                                "Process " + std::to_string((*readyQueue)[maxpriority].getPID()) + " running");

                runningList->push_back((*readyQueue)[maxpriority]);
                readyQueue->erase(readyQueue->begin()+maxpriority);
                return true;
            } else return false;
        }
        catch (...) {
            std::cout << "Erro no escalonamento. Nenhum processo escalonado\n";
            return false;
        }
    }

    static bool LOTTERY(std::vector<Process> *readyQueue, std::vector<Process> *runningList, int* _quantum, double _elapsedTime)  {
        try {
            if (!readyQueue->empty()) {
                uint32_t countTickets = 0; size_t i = 0; size_t winner = 0;

                for (; i < (*readyQueue).size(); i++) {
		            (*readyQueue)[i].firstTicket = countTickets;
		            countTickets += ((*readyQueue)[i].getPriority() + 1) * 10;
		            (*readyQueue)[i].lastTicket = countTickets;
	            } i = 0;

                srand((unsigned)time(0));
                uint32_t ticketWinner = static_cast<uint32_t>(rand() % (static_cast<int>(countTickets) + 1));
                size_t idx = 0;
                for (Process process: (*readyQueue)) {
                    if (process.firstTicket <= ticketWinner && process.lastTicket >= ticketWinner) {
                        winner = idx;
                        break;
                    } else idx++;
                }

                (*_quantum) = 2;
	            if (firstTimeRunning((*readyQueue)[winner])) (*readyQueue)[winner].setResponseTime(_elapsedTime);

	            Simulator::DebugLog(_elapsedTime,
	                                "Process " + std::to_string((*readyQueue)[winner].getPID()) + " running");

	            runningList->push_back((*readyQueue)[winner]);
                readyQueue->erase(readyQueue->begin()+winner);
                return true;
            } else return false;
        }
        catch (...) {
            std::cout << "Erro no escalonamento. Nenhum processo escalonado\n";
            return false;
        }
    }

	
    static bool RR(std::vector<Process>* readyQueue, std::vector<Process>* runningList, int* _quantum, double _elapsedTime) {
        try {
            if (!readyQueue->empty()) {
	            (*_quantum) = 2;
	            if (firstTimeRunning((*readyQueue)[0])) (*readyQueue)[0].setResponseTime(_elapsedTime);

	            Simulator::DebugLog(_elapsedTime,
	                                "Process " + std::to_string((*readyQueue)[0].getPID()) + " running");

                if (!(*runningList).empty()) {
                    (*readyQueue).push_back((*runningList)[0]);
                    (*runningList)[0] = (*readyQueue)[0];
                } else (*runningList).push_back((*readyQueue)[0]);

                readyQueue->erase(readyQueue->begin());
                return true;
            } else return false;
        } catch (...) {
            std::cout << "Erro no escalonamento. Nenhum processo escalonado\n";
            return false;
        }
    }

    static bool SD(std::vector<Process>* readyQueue, std::vector<Process>* runningList, int* _quantum, double _elapsedTime) {
        try {
            if (!readyQueue->empty()) {

	            (*_quantum) = maxProcessMultiprogramming / (*readyQueue).size();
	            if (firstTimeRunning((*readyQueue)[0])) (*readyQueue)[0].setResponseTime(_elapsedTime);

	            Simulator::DebugLog(_elapsedTime,
	                                "Process " + std::to_string((*readyQueue)[0].getPID()) + " running");

	            runningList->push_back((*readyQueue)[0]);
                readyQueue->erase(readyQueue->begin());
                return true;
            } else return false;
        } catch (...) {
            std::cout << "Erro no escalonamento. Nenhum processo escalonado\n";
            return false;
        }
    }

    static bool FEEDBACK(std::vector<Process> *readyQueue, std::vector<Process> *runningList, int* _quantum, double _elapsedTime) {
        try {
            if (!readyQueue->empty()) {
                for (Process process: (*readyQueue)){
                    if (process.getTimesExecuted() == 0) process.setPriority(0);
                    else if (process.getTimesExecuted() == 1) process.setPriority(1);
                    else if (process.getTimesExecuted() == 2) process.setPriority(2);
                    else if (process.getTimesExecuted() >= 3) process.setPriority(3);
                }

                size_t minpriority = 0; int priority = (*readyQueue)[0].getPriority(); size_t i = 0;
                for (Process process: (*readyQueue)) {
                    if (process.getPriority() < priority) {
                        priority = process.getPriority();
                        minpriority = i;
                    } i++;
                }

                (*readyQueue)[minpriority].incrementTimesExecuted();

                if (priority == 0) (*_quantum) = 1;
                else if (priority == 1) (*_quantum) = 2;
                else if (priority == 2) (*_quantum) = 4;
                else if (priority == 3) (*_quantum) = 8;

                if (firstTimeRunning((*readyQueue)[minpriority])) (*readyQueue)[minpriority].setResponseTime(_elapsedTime);
                Simulator::DebugLog(_elapsedTime,
                                    "Process " + std::to_string((*readyQueue)[minpriority].getPID()) + " running");

                runningList->push_back((*readyQueue)[minpriority]);
                readyQueue->erase(readyQueue->begin()+minpriority);
                return true;
            } else return false;
        }
        catch (...) {
            std::cout << "Erro no escalonamento. Nenhum processo escalonado\n";
            return false;
        }
    }
    
}

#endif