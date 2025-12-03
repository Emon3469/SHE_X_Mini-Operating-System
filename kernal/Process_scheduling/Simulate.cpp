#include <cmath>
#include "simulate.h"
#include "ProcessSchedulingAlgorithms.h"

bool Simulator::debugmode;
int Simulator::_quantum;

Simulator::Simulator(int maxMultiprogramming, bool step_by_step, bool debugmode) {
    this->_cpuIdle = true;
    this->_quantum = 0;
    this->debugmode = debugmode;
    this->SPEED_ = step_by_step? 1/2.0 : 0;
    this->maxProcessMultiprogramming = maxMultiprogramming;

    this->lastUpdate = 0;
    this->countProcess = 0;

    this->_elapsedTime = 0;
    this->_processorUse = 0;
    this->_avgWaitingTime = 0;
    this->_avgResponseTime = 0;
    this->_avgTurnaroundTime = 0;
    this->_avgServiceTime = 0;
    this->_throughput = 0;
    this->_cpuIdleTime = 0;
    this->_pageStatistics.hits = 0;
    this->_pageStatistics.miss = 0;
}


bool Simulator::isSysFull() {
    return (maxProcessMultiprogramming -
            (this->runningProcess.size() + this->readyQueue.size() + this->blockedQueue.size())) <= 0;
}

bool Simulator::StartProcess(std::tuple<int, double, int, double, double, std::vector<Page>> _process) {
    try {
        Process process(_process);
        if (process.getSubmissionTime() == 0) {
            !isSysFull()? readyQueue.push_back(process) : waitingQueue.push_back(process);
            for (Page page : process.getAllPages()) Disk.push_back(page);
        }
        else incomingQueue.push_back(process);
        countProcess++;
        return true;
    } catch (...) {
        return false;
    }
}

void Simulator::TerminateProcess(Process _process) {
    _avgWaitingTime += _process.getWaitingTime();
    _avgResponseTime += _process.getResponseTime();
    _avgServiceTime += (_elapsedTime - _process.getSubmissionTime() - _process.getWaitingTime());
    _avgTurnaroundTime += _process.getTurnaroundTime();
    DebugLog(_elapsedTime, ("Process " + std::to_string(_process.getPID()) + " terminated"));
}

void Simulator::CheckIncomingQueue() {
    for (auto i = incomingQueue.size(); i-- > 0;) {
        if (remainingSubmissionTime(incomingQueue[i]) <= 0) {
            for (Page page : incomingQueue[i].getAllPages()) Disk.push_back(page);
            waitingQueue.push_back(incomingQueue[i]);
            incomingQueue.erase(incomingQueue.begin() + i);
        }
    }
}

bool Simulator::PageInMemory() {
    if (runningProcess.empty()) return true;
    if (runningProcess[0].getPage().getLifeTime() == 0) return true;
    for (size_t i = 0; i < memoryFrames.size(); i++)
        if (memoryFrames[i].getValue() == runningProcess[0].getPage().getValue()
            && memoryFrames[i].getPID() == runningProcess[0].getPage().getPID()) {
            memoryFrames[i].setLastTimeUsed(_elapsedTime + 1);
            memoryFrames[i].setreference();
            _pageStatistics.hits++;
            return true;
        }
    _pageStatistics.miss++;
    return false;
}

void Simulator::RemovePages(uint32_t PID) {
    // iterate backwards when erasing to avoid skipping elements
    for (int i = static_cast<int>(memoryFrames.size()) - 1; i >= 0; --i)
        if (memoryFrames[i].getPID() == PID) memoryFrames.erase(memoryFrames.begin()+i);
}


void Simulator::CheckRunningProcess() {
    if (!runningProcess.empty()) {
        runningProcess[0].decrementExecutionTime();
        runningProcess[0].decrementPageLifeTime();
        if (runningProcess[0].getExecutionTime() < 1) {
            if (runningProcess[0].getBlockTime() > 0) {
                if (!waitingQueue.empty() && isSysFull()) {
                    blockedSuspensedQueue.push_back(runningProcess[0]);
                    DebugLog(_elapsedTime,
                             ("Process " + std::to_string(runningProcess[0].getPID()) + " bloqueado e suspenso"));
                } else {
                    blockedQueue.push_back(runningProcess[0]);
                    DebugLog(_elapsedTime,
                             ("Process " + std::to_string(runningProcess[0].getPID()) + " bloqueado"));
                }
                RemovePages(runningProcess[0].getPID());
                runningProcess.erase(runningProcess.begin());
            } else { 
                runningProcess[0].setTurnaroundTime(_elapsedTime);
                runningProcess[0].setWaitingTime(_elapsedTime);
                RemovePages(runningProcess[0].getPID());
                TerminateProcess(runningProcess[0]);
                runningProcess.erase(runningProcess.begin());
            }
        } else if (this->_quantum < 1) {
            readyQueue.push_back(runningProcess[0]);
            DebugLog(_elapsedTime,
                     ("Process " + std::to_string(runningProcess[0].getPID()) + " ready"));
            runningProcess.erase(runningProcess.begin());
        }
    }
}

void Simulator::CheckBlockedQueue() {
    for (auto i = blockedQueue.size(); i-- > 0; ) {
        if (blockedQueue[i].getBlockTime() > 0) blockedQueue[i].decrementBlockTime();
        if (blockedQueue[i].getBlockTime() <= 0 && waitingQueue.empty()) {
            readyQueue.push_back(blockedQueue[i]);
            blockedQueue.erase(blockedQueue.begin() + i);
        }
    }
    for (auto i = blockedSuspensedQueue.size(); i-- > 0; ) { 
        if (blockedSuspensedQueue[i].getBlockTime() > 0) blockedSuspensedQueue[i].decrementBlockTime();
        if (blockedSuspensedQueue[i].getBlockTime() <= 0) {
            readySuspensedQueue.push_back(blockedSuspensedQueue[i]);
            blockedSuspensedQueue.erase(blockedSuspensedQueue.begin() + i);
        }
    }
}


void Simulator::CheckReadySuspensedQueue() {
    while (waitingQueue.empty() && !readySuspensedQueue.empty() && !isSysFull())
        ProcessSchedulingAlgorithms::FCFS(&readySuspensedQueue, &readyQueue, _elapsedTime);
}

bool Simulator::EmptyQueue() {
    return incomingQueue.empty() && waitingQueue.empty() &&readySuspensedQueue.empty()
           && readyQueue.empty() && blockedQueue.empty() &&blockedSuspensedQueue.empty() && _cpuIdle;
}

void Simulator::StartSimulation(
        bool (*shortTermSchedulingAlgorithm)(std::vector<Process>*, std::vector<Process>*, int*, double),
        bool (*pageReplacementAlgorithm)(std::vector<Page>*, std::vector<Page>*, Page, double),
        std::vector<std::tuple<int, double, int, double, double,std::vector<Page>>> process) {

    DebugLog("Inicio da simulação:");

    for (std::tuple<int, double, int, double, double, std::vector<Page>> _process : process)
        StartProcess(_process);

    process.clear();

    for (;!EmptyQueue(); lastUpdate = time(NULL))
        if (time(NULL) - lastUpdate >= SPEED_) {

            CheckIncomingQueue();

            while (!isSysFull()) {
                if (!waitingQueue.empty()) {
                    if (ProcessSchedulingAlgorithms::FCFS(&waitingQueue, &readyQueue, _elapsedTime))
                        if (pageReplacementAlgorithm) pageReplacementAlgorithm(&memoryFrames, &Disk, readyQueue.back().getAllPages()[0], _elapsedTime);
                        else PageReplacementAlgorithm_OPTIMAL();
                } else break;
            }

            CheckBlockedQueue();
            CheckReadySuspensedQueue();
            CheckRunningProcess();

            if (noProcessRunning())
                _cpuIdle = !shortTermSchedulingAlgorithm(&readyQueue, &runningProcess, &_quantum, _elapsedTime);

            if (!_cpuIdle && runningProcess[0].getExecutionTime() > 0 && !PageInMemory())
                if (pageReplacementAlgorithm) pageReplacementAlgorithm(&memoryFrames, &Disk, runningProcess[0].getPage(), _elapsedTime);
                else PageReplacementAlgorithm_OPTIMAL();

            _elapsedTime++;
            decrementQuantum();
            if (_cpuIdle && noProcessRunning()) _cpuIdleTime++;
        }

    CalcStatistics();
    if (debugmode) std::cout << getResults() << std::endl;
}

void Simulator::CalcStatistics() {
    --_elapsedTime; --_cpuIdleTime;
    DebugLog("\nFim da simulação\n*******************************\nCalculando estatísticas");
    _processorUse = (static_cast<double>(_elapsedTime) - _cpuIdleTime) / _elapsedTime * 100;
    _throughput = static_cast<double>(countProcess) / _elapsedTime * 100;
    _avgWaitingTime /= countProcess;
    _avgResponseTime /= countProcess;
    _avgTurnaroundTime /= countProcess;
    _avgServiceTime /= countProcess;
    _pageStatistics.hitRate = (static_cast<double>(_pageStatistics.hits) / (_pageStatistics.miss + _pageStatistics.hits)) * 100;
}

std::string Simulator::getResults() {
    std::stringstream out;
    out << "*******************************\n";
    out << "Duração da Simulação (Simulation duration): " << _elapsedTime
        << "\nUtilização do processador (CPU efficiency): " << _processorUse
        << "%\nVazão (Throughput): " << _throughput
        << "%\nTempo médio de espera (Average waiting time): " << _avgWaitingTime
        << "\nTempo médio de resposta (Average response time): "
        << _avgResponseTime << "\nTempo médio de retorno (Average turnaround time): " << _avgTurnaroundTime
        << "\nTempo médio de serviço (Average service time): " << _avgServiceTime
        << "\nEstatísticas da paginação (Paging Statistics):" << "\n\tHits: " << _pageStatistics.hits
        << "\n\tMiss: " << _pageStatistics.miss
        << "\n\tHit Rate: " << std::setprecision(3) << _pageStatistics.hitRate << "%";
    return out.str();
}

void Simulator::DebugLog(std::string happen) {
    if (Simulator::debugmode) std::cout << happen << std::endl;
}

void Simulator::DebugLog(double instantTime, std::string happen) {
    if (Simulator::debugmode) std::cout << "\tT = " << instantTime << ": " << happen << std::endl;
}

void Simulator::Clear(int maxMultiprogramming, bool step_by_step, bool debugmode) {

	this->_cpuIdle = true;
	this->_quantum = 0;
	this->debugmode = debugmode;
	this->SPEED_ = step_by_step? 1/2.0 : 0;
	this->maxProcessMultiprogramming = maxMultiprogramming;

	this->lastUpdate = 0;
	this->countProcess = 0;
	this->_elapsedTime = 0;
	this->_processorUse = 0;
	this->_avgWaitingTime = 0;
	this->_avgResponseTime = 0;
	this->_avgTurnaroundTime = 0;
	this->_avgServiceTime = 0;
	this->_throughput = 0;
	this->_cpuIdleTime = 0;

	this->incomingQueue.clear();
	this->waitingQueue.clear();
	this->readyQueue.clear();
    this->readySuspensedQueue.clear();
	this->runningProcess.clear();
	this->blockedQueue.clear();
    this->blockedSuspensedQueue.clear();

	this->out.clear();
}


bool Simulator::PageReplacementAlgorithm_OPTIMAL() {
    try {
        Page page = runningProcess[0].getPage();
        if (!Disk.empty() && page.getLifeTime() > 0) {
            size_t i = 0; int opt_p = 0; int futureUse = -1;
            for (; i < Disk.size() && !(Disk[i].getValue() == page.getValue() && Disk[i].getPID() == page.getPID()); i++);
            if (i >= Disk.size()) { Simulator::DebugLog("ERRO. PÁGINA NÃO ENCONTRADA NO DISCO"); exit(1);} 

            if (memoryFrames.empty()) {
                memoryFrames.push_back(Disk[i]);
                Disk.erase(Disk.begin()+i);
                DebugLog(_elapsedTime, "Page "+std::to_string(page.getValue())+" of pid "+
                                       std::to_string(page.getPID())+" moved from disk to memory");
            }
            else {
                for (size_t j = 0; j < memoryFrames.size(); j++) {
                    if (memoryFrames[j].getPID() == runningProcess[0].getPID()) {
                        if (runningProcess[0].willUsePage(memoryFrames[j]) > futureUse) {
                            opt_p = j;
                            futureUse = runningProcess[0].willUsePage(memoryFrames[j]);
                        }
                    } else {
                        for (size_t k = 0; k < readyQueue.size(); k++) {
                            if (memoryFrames[j].getPID() == readyQueue[k].getPID()) {
                                if (readyQueue[k].willUsePage(memoryFrames[j]) > futureUse) {
                                    opt_p = j;
                                    futureUse = readyQueue[k].willUsePage(memoryFrames[j]);
                                }
                                break;
                            }
                        }
                    }
                    if (futureUse == std::numeric_limits<int>::max()) break;
                }

                Disk.push_back(memoryFrames[opt_p]);
                DebugLog(_elapsedTime, "Page " + std::to_string(memoryFrames[opt_p].getValue()) + " of pid " +
                                                       std::to_string(memoryFrames[opt_p].getPID()) + " moved to disk");

                memoryFrames[opt_p] = Disk[i];
                DebugLog(_elapsedTime, "Page "+std::to_string(page.getValue())+" of pid "+
                                                            std::to_string(page.getPID())+" moved from disk to memory");
            }
            return true;
        } return false;
    } catch(...) {
        std::cout << "Erro ao substituir página." << std::endl;
        return false;
    }
}