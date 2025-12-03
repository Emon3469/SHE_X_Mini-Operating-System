#include "Process.h"

Process::Process() {
    this->PID = 0;
    this->submissionTime = 0;
    this->priority = 0;
    this->remainsExecutionTime = 0;
    this->blockTime = 0;
    this->timesExecuted = 0;

    this->waitingTime = 0;
    this->responseTime = 0;

	totalExecutationTime = remainsExecutionTime;
}

Process::Process(std::tuple<int, double, int, double, double, std::vector<Page>> _process) {
    this->PID = _getPID(_process);
    this->submissionTime = _getSubmissionTime(_process);
    this->priority = _getPriority(_process);
    this->remainsExecutionTime = _getExecutionTime(_process);
    this->blockTime = _getBlockedTime(_process);
    this->waitingTime = -1;
    this->responseTime = -1;
    this->timesExecuted = 0;
    this->pages = _getPages(_process);

	totalExecutationTime = remainsExecutionTime;
}

int Process::getPID() {
    return PID;
}

double Process::getSubmissionTime() {
    return submissionTime;
}

int Process::getPriority() {
    return priority;
}

int Process::getTimesExecuted() {
    return timesExecuted;
}

double Process::getExecutionTime() {
    return remainsExecutionTime;
}

double Process::getBlockTime() {
    return blockTime;
}

double Process::getResponseTime() {
    return responseTime;
}

double Process::getWaitingTime() {
    return waitingTime;
}

double Process::getTurnaroundTime() {
	return turnaroundTime;
}

Page Process::getPage() {
    if (pages.empty()) return Page(0,0,0,0);

    // If first use is in the future relative to remaining execution, no page available
    if (pages[0].getFirstUse() > this->totalExecutationTime - remainsExecutionTime)
        return Page(0,0,0,0);

    if (pages[0].getLifeTime() > 0)
        return pages[0];

    // page expired, remove it and try next one
    pages.erase(pages.begin());
    if (pages.empty()) return Page(0,0,0,0);
    if (pages[0].getLifeTime() > 0)
        return pages[0];

    // fallback: no valid page
    return Page(0,0,0,0);
}

std::vector<Page> Process::getAllPages() {
    return pages;
}

void Process::updateSubmissionTime(double _submissionTime) {
    this->submissionTime =  _submissionTime;
}


void Process::setResponseTime(double _elapsedTime) {
    this->responseTime = _elapsedTime - this->submissionTime;
}

void Process::setWaitingTime(double _elapsedTime) {
    this->waitingTime = _elapsedTime - (this->submissionTime + this->totalExecutationTime);
}

void Process::setTurnaroundTime(double _elapsedTime) {
	this->turnaroundTime = _elapsedTime - submissionTime;
}

void Process::incrementTimesExecuted() {
    this->timesExecuted += 1;
}

void Process::setPriority(int priority) {
    this->priority = priority;
}