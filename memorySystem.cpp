/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

/*! @file
 *  This file contains an ISA-portable cache simulator
 *  data cache hierarchies
 */

#include "pin.H"

#include <iostream>
#include <fstream>
#include <cassert>

#include "cache.H"
#include "pin_profile.H"
#include "memorySystem.hpp"

#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <unistd.h>

using std::cerr;
using std::endl;
using namespace DRAMSim;

int getCPUIdx() {
  unsigned ret, cpu, node;
  ret = syscall(309, &cpu, &node);
  return ret==0 ? cpu : node;
}

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB< string > KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "memorySystem.out", "specify dcache file name");
KNOB< BOOL > KnobTrackLoads(KNOB_MODE_WRITEONCE, "pintool", "tl", "1", "track individual loads -- increases profiling time");
KNOB< BOOL > KnobTrackStores(KNOB_MODE_WRITEONCE, "pintool", "ts", "1", "track individual stores -- increases profiling time");
KNOB< UINT32 > KnobThresholdHit(KNOB_MODE_WRITEONCE, "pintool", "rh", "100", "only report memops with hit count above threshold");
KNOB< UINT32 > KnobThresholdMiss(KNOB_MODE_WRITEONCE, "pintool", "rm", "100",
                                 "only report memops with miss count above threshold");
KNOB< UINT32 > KnobCacheSize(KNOB_MODE_WRITEONCE, "pintool", "c", "32", "cache size in kilobytes");
KNOB< UINT32 > KnobLineSize(KNOB_MODE_WRITEONCE, "pintool", "b", "32", "cache block size in bytes");
KNOB< UINT32 > KnobAssociativity(KNOB_MODE_WRITEONCE, "pintool", "a", "4", "cache associativity (1 for direct mapped)");

/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool represents a cache simulator.\n"
            "\n";

    cerr << KNOB_BASE::StringKnobSummary();

    cerr << endl;

    return -1;
}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

TransactionCompleteCB *read_cb, *write_cb;
MultiChannelMemorySystem *mem;
static int memAccessCounter=0;
void some_object::read_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
  //printf("%d = getCPUIdx()\n", getCPUIdx());
	//printf("[Callback] read complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

void some_object::write_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
  //printf("%d = getCPUIdx()\n", getCPUIdx());
	//printf("[Callback] write complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

/* FIXME: this may be broken, currently */
void power_callback(double a, double b, double c, double d)
{
//	printf("power callback: %0.3f, %0.3f, %0.3f, %0.3f\n",a,b,c,d);
}


// wrap configuation constants into their own name space to avoid name clashes
namespace DL1
{
const UINT32 max_sets                          = KILO; // cacheSize / (lineSize * associativity);
const UINT32 max_associativity                 = 256;  // associativity;
const CACHE_ALLOC::STORE_ALLOCATION allocation = CACHE_ALLOC::STORE_ALLOCATE;

typedef CACHE_ROUND_ROBIN(max_sets, max_associativity, allocation) CACHE;
} // namespace DL1

DL1::CACHE* dl1 = NULL;

typedef enum
{
    COUNTER_MISS = 0,
    COUNTER_HIT  = 1,
    COUNTER_NUM
} COUNTER;

typedef COUNTER_ARRAY< UINT64, COUNTER_NUM > COUNTER_HIT_MISS;

// holds the counters with misses and hits
// conceptually this is an array indexed by instruction address
COMPRESSOR_COUNTER< ADDRINT, UINT32, COUNTER_HIT_MISS > profile;

/* ===================================================================== */

VOID LoadMulti(ADDRINT addr, UINT32 size, UINT32 instId)
{
    // first level D-cache
    const BOOL dl1Hit = dl1->Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD);
    memAccessCounter++;
    if(!dl1Hit) {
      mem->addTransaction(false, addr);
    }
    const COUNTER counter = dl1Hit ? COUNTER_HIT : COUNTER_MISS;
    profile[instId][counter]++;
}

/* ===================================================================== */

VOID StoreMulti(ADDRINT addr, UINT32 size, UINT32 instId)
{
    // first level D-cache
    const BOOL dl1Hit = dl1->Access(addr, size, CACHE_BASE::ACCESS_TYPE_STORE);
    memAccessCounter++;
    if(!dl1Hit) {
      mem->addTransaction(true, addr);
    }

    const COUNTER counter = dl1Hit ? COUNTER_HIT : COUNTER_MISS;
    profile[instId][counter]++;
}

/* ===================================================================== */

VOID LoadSingle(ADDRINT addr, UINT32 instId)
{
    // @todo we may access several cache lines for
    // first level D-cache
    const BOOL dl1Hit = dl1->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD);
    memAccessCounter++;
    if(!dl1Hit) {
      mem->addTransaction(false, addr);
    }

    const COUNTER counter = dl1Hit ? COUNTER_HIT : COUNTER_MISS;
    profile[instId][counter]++;
}
/* ===================================================================== */

VOID StoreSingle(ADDRINT addr, UINT32 instId)
{
    // @todo we may access several cache lines for
    // first level D-cache
    const BOOL dl1Hit = dl1->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_STORE);
    memAccessCounter++;
    if(!dl1Hit) {
      mem->addTransaction(true, addr);
    }
    const COUNTER counter = dl1Hit ? COUNTER_HIT : COUNTER_MISS;
    profile[instId][counter]++;
}

/* ===================================================================== */

VOID LoadMultiFast(ADDRINT addr, UINT32 size) { dl1->Access(addr, size, CACHE_BASE::ACCESS_TYPE_LOAD); }

/* ===================================================================== */

VOID StoreMultiFast(ADDRINT addr, UINT32 size) { dl1->Access(addr, size, CACHE_BASE::ACCESS_TYPE_STORE); }

/* ===================================================================== */

VOID LoadSingleFast(ADDRINT addr) { dl1->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_LOAD); }

/* ===================================================================== */

VOID StoreSingleFast(ADDRINT addr) { dl1->AccessSingleLine(addr, CACHE_BASE::ACCESS_TYPE_STORE); }

/* ===================================================================== */

VOID Instruction(INS ins, void* v)
{
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Instrument each memory operand. If the operand is both read and written
    // it will be processed twice.
    // Iterating over memory operands ensures that instructions on IA-32 with
    // two read operands (such as SCAS and CMPS) are correctly handled.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        const UINT32 size = INS_MemoryOperandSize(ins, memOp);
        const BOOL single = (size <= 4);

        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            if (KnobTrackLoads)
            {
                // map sparse INS addresses to dense IDs
                const ADDRINT iaddr = INS_Address(ins);
                const UINT32 instId = profile.Map(iaddr);

                if (single)
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)LoadSingle, IARG_MEMORYOP_EA, memOp, IARG_UINT32,
                                             instId, IARG_END);
                }
                else
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)LoadMulti, IARG_MEMORYOP_EA, memOp, IARG_UINT32, size,
                                             IARG_UINT32, instId, IARG_END);
                }
            }
            else
            {
                if (single)
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)LoadSingleFast, IARG_MEMORYOP_EA, memOp, IARG_END);
                }
                else
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)LoadMultiFast, IARG_MEMORYOP_EA, memOp, IARG_UINT32,
                                             size, IARG_END);
                }
            }
        }

        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            if (KnobTrackStores)
            {
                const ADDRINT iaddr = INS_Address(ins);
                const UINT32 instId = profile.Map(iaddr);

                if (single)
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)StoreSingle, IARG_MEMORYOP_EA, memOp, IARG_UINT32,
                                             instId, IARG_END);
                }
                else
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)StoreMulti, IARG_MEMORYOP_EA, memOp, IARG_UINT32, size,
                                             IARG_UINT32, instId, IARG_END);
                }
            }
            else
            {
                if (single)
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)StoreSingleFast, IARG_MEMORYOP_EA, memOp, IARG_END);
                }
                else
                {
                    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)StoreMultiFast, IARG_MEMORYOP_EA, memOp, IARG_UINT32,
                                             size, IARG_END);
                }
            }
        }
      mem->update();
    }
}

/* ===================================================================== */

VOID Fini(int code, VOID* v)
{
    std::ofstream out(KnobOutputFile.Value().c_str());

    // print D-cache profile
    // @todo what does this print

    out << "PIN:MEMLATENCIES 1.0. 0x0\n";

    out << "#\n"
           "# DCACHE stats\n"
           "#\n";

    out << dl1->StatsLong("# ", CACHE_BASE::CACHE_TYPE_DCACHE);

    if (KnobTrackLoads || KnobTrackStores)
    {
        out << "#\n"
               "# LOAD stats\n"
               "#\n";

        out << profile.StringLong();
        out << "\nmem accessed cnt = "<<memAccessCounter<<'\n';
    }
    out.close();
    mem->printStats(true);
}



/* ===================================================================== */

int main(int argc, char* argv[])
{
    PIN_InitSymbols();

    if (PIN_Init(argc, argv))
    {
        return Usage();
    }
    /////////////////////////////////////
	  some_object obj;
	  read_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::read_complete);
	  write_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::write_complete);
	  /* pick a DRAM part to simulate */
	  mem = getMemorySystemInstance("ini/DDR2_micron_16M_8b_x8_sg3E.ini", "ini/system.ini", "./", "memorySystem", 16384); 
    mem->setCPUClockSpeed((uint64_t)2.5*1000*1000*1000); // 2.5 GHz
    mem->RegisterCallbacks(read_cb, write_cb, power_callback);

	  printf("dramsim_test main()\n");
	  printf("-----MEM1------\n");
    printf("%d = getCPUIdx()\n", getCPUIdx());
    /////////////////////////////////////

    dl1 = new DL1::CACHE("L1 Data Cache", KnobCacheSize.Value() * KILO, KnobLineSize.Value(), KnobAssociativity.Value());

    profile.SetKeyName("iaddr          ");
    profile.SetCounterName("dcache:miss        dcache:hit");

    COUNTER_HIT_MISS threshold;

    threshold[COUNTER_HIT]  = KnobThresholdHit.Value();
    threshold[COUNTER_MISS] = KnobThresholdMiss.Value();

    profile.SetThreshold(threshold);

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns

    PIN_StartProgram();

    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
