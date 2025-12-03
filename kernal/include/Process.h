#include <iostream>
#include <string>
#include <tuple>
#include <map>
#include <vector>
#include <limits>
#include "Page.h"

#ifndef PROCESS_H
#define PROCESS_H

#define _getPID(x) std::get<0>(x)
#define _getSubmissionTime(x) std::get<1>(x)
#define _getPriority(x) std::get<2>(x)
#define _getExecutionTime(x) std::get<3>(x)
#define _getBlockedTime(x) std::get<4>(x)
#define _getPages(x) std::get<5>(x)

class Process {
   public:
      Process();
      Process(std::tuple<int, double, int, double, double, std::vector<Page>>);
      int getPID();
      int getPriority();
	   int getTimesExecuted();
      double getSubmissionTime();
      double getExecutionTime();
      double getBlockTime();
      double getResponseTime();
      double getWaitingTime();
	   double getTurnaroundTime();
      Page getPage();
	   std::vector<Page> getAllPages();
	   void updateSubmissionTime(double _submissionTime);
      void setResponseTime(double _elapsedTime);
      void setWaitingTime(double _elapsedTime);
	   void setTurnaroundTime(double _elapsedTime);
	   void setPriority(int priority);
      void incrementTimesExecuted();
	   void decrementExecutionTime() { if (remainsExecutionTime > 0) --remainsExecutionTime; }
	   void decrementBlockTime() { if (blockTime > 0) --blockTime; }
      void decrementPageLifeTime() { if (!pages.empty() && pages[0].getLifeTime() > 0) pages[0].decrementLifeTime(); }

	  uint32_t firstTicket, lastTicket;
	  double totalExecutationTime;

     int willUsePage(Page page) {
        for (size_t i = 0; i < pages.size(); i++)
           if (page.getValue() == pages[i].getValue()) return static_cast<int>(i);
        return std::numeric_limits<int>::max();
      }

    private:
       std::vector<Page> pages;
       int PID, priority, timesExecuted;
       double submissionTime, remainsExecutionTime, blockTime, waitingTime, responseTime, turnaroundTime;
};

#endif