#ifndef PAGE_REPLACEMENT_ALGORITHMS_H
#define PAGE_REPLACEMENT_ALGORITHMS_H

#include <cstdint>
#include <vector>
#include <iostream>
#include "Page.h"
#include "simulate.h"

namespace PageReplacementAlgorithms {

    uint32_t clock_p = 0;
    static bool FIFO(std::vector<Page>* pagesInMemory, std::vector<Page>* pagesInDisk, Page page, double _elapsedTime) {
        try {
            if (!pagesInDisk->empty() && page.getLifeTime() > 0) {
                size_t i = 0;
                for (; i < (*pagesInDisk).size() && !((*pagesInDisk)[i].getValue() == page.getValue() &&
                                                     (*pagesInDisk)[i].getPID() == page.getPID()); i++);
                if (i >= (*pagesInDisk).size()) { Simulator::DebugLog("ERRO. PÁGINA NÃO ENCONTRADA NO DISCO"); exit(1);} 
                if ((*pagesInMemory).size() >= Simulator::numberOfMemoryFrames) {
                    (*pagesInDisk).push_back((*pagesInMemory)[0]);
                    Simulator::DebugLog(_elapsedTime,
                                        "Page " + std::to_string((*pagesInMemory)[0].getValue()) + " of pid " +
                                        std::to_string((*pagesInMemory)[0].getPID()) + " moved to disk");
                    (*pagesInMemory).erase((*pagesInMemory).begin());
                }
                (*pagesInMemory).push_back((*pagesInDisk)[i]);
                Simulator::DebugLog(_elapsedTime, "Page "+std::to_string(page.getValue())+" of pid "+
                            std::to_string(page.getPID())+" moved from disk to memory");
                (*pagesInDisk).erase((*pagesInDisk).begin() + i);
                return true;
            } return false;
        } catch (...) {
            std::cout << "Erro ao substituir página." << std::endl;
            return false;
        }
    }
   
   
   static bool LRU(std::vector<Page>* pagesInMemory, std::vector<Page>* pagesInDisk, Page page, double _elapsedTime) {
      try {
         if (!(*pagesInDisk).empty() && page.getLifeTime() > 0) {
             int lru = 0, i = 0;
                         for (; i < (*pagesInDisk).size() && !((*pagesInDisk)[i].getValue() == page.getValue() &&
                                                                                                         (*pagesInDisk)[i].getPID() == page.getPID()); i++);
                             if (i >= (*pagesInDisk).size()) { Simulator::DebugLog("ERRO. PÁGINA NÃO ENCONTRADA NO DISCO"); exit(1);} 

               if ((*pagesInMemory).size() >= Simulator::numberOfMemoryFrames) {
                  for (auto j = (*pagesInMemory).size(); j-- > 0; )
                     if ((*pagesInMemory)[j].getLastTimeUsed() < (*pagesInMemory)[lru].getLastTimeUsed()) lru = j;
                  (*pagesInDisk).push_back((*pagesInMemory)[lru]);
                  Simulator::DebugLog(_elapsedTime, "Page "+std::to_string((*pagesInMemory)[lru].getValue())+" of pid "+
                                                      std::to_string((*pagesInMemory)[lru].getPID())+" moved from memory to disk");
                  (*pagesInMemory)[lru] = (*pagesInDisk)[i];
                  (*pagesInMemory)[lru].setLastTimeUsed(_elapsedTime);
                  (*pagesInDisk).erase((*pagesInDisk).begin()+i);
                  Simulator::DebugLog(_elapsedTime, "Page "+std::to_string(page.getValue())+" of pid "+
                           std::to_string(page.getPID())+" moved from disk to memory");
               } else {
                  (*pagesInMemory).push_back((*pagesInDisk)[i]);
                  (*pagesInMemory).back().setLastTimeUsed(_elapsedTime);
                  (*pagesInDisk).erase((*pagesInDisk).begin()+i);
                  Simulator::DebugLog(_elapsedTime, "Page "+std::to_string(page.getValue())+" of pid "+
                           std::to_string(page.getPID())+" moved from disk to memory");
                }
               return true;
            }
            return false;
        } catch(...) {
            std::cout << "Erro ao substituir página." << std::endl;
            return false;
        }
    }
    static bool CLOCK(std::vector<Page>* pagesInMemory, std::vector<Page>* pagesInDisk, Page page, double _elapsedTime) {
        try {
            if (!(*pagesInDisk).empty() && page.getLifeTime() > 0) {
                size_t i = 0;
                for (; i < (*pagesInDisk).size() && !((*pagesInDisk)[i].getValue() == page.getValue() &&
                                                      (*pagesInDisk)[i].getPID() == page.getPID()); i++);
                if (i >= (*pagesInDisk).size()) { Simulator::DebugLog("ERRO. PÁGINA NÃO ENCONTRADA NO DISCO"); exit(1);} 

                if ((*pagesInMemory).size() < Simulator::numberOfMemoryFrames) {
                    (*pagesInMemory).push_back((*pagesInDisk)[i]);
                    (*pagesInMemory).back().setreference();
                } else {
                    while (true) {
                        if (clock_p >= Simulator::numberOfMemoryFrames) clock_p = 0;
                        if ((*pagesInMemory)[clock_p].isreferenced()) {
                            (*pagesInMemory)[clock_p++].resetreference();
                            continue;
                        } else {
                            (*pagesInDisk).push_back((*pagesInMemory)[clock_p]);
                            Simulator::DebugLog(_elapsedTime,
                                                "Page " + std::to_string((*pagesInMemory)[clock_p].getValue()) + " of pid " +
                                                std::to_string((*pagesInMemory)[clock_p].getPID()) + " moved to disk");
                            (*pagesInMemory)[clock_p] = (*pagesInDisk)[i];
                            (*pagesInMemory)[clock_p++].setreference();
                            Simulator::DebugLog(_elapsedTime, "Page "+std::to_string(page.getValue())+" of pid "+
                                                              std::to_string(page.getPID())+" moved from disk to memory");
                            break;
                        }
                    }
                }
                (*pagesInDisk).erase((*pagesInDisk).begin() + i);
                return true;
            }
            return false;
        } catch(...) {
            std::cout << "Erro ao substituir página." << std::endl;
            return false;
        }
    }
}

#endif