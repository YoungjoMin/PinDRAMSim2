#ifndef PIN_DRAM_H
#define PIN_DRAM_H

#include <string>
#include <stdint.h>
#include "DRAMSim2/DRAMSim.h"

TransactionCompleteCB *read_cb, *write_cb;
MultiChannelMemorySystem *mem;

class dram
{
	public: 
    dram()
		void read_complete(unsigned, uint64_t, uint64_t);
		void write_complete(unsigned, uint64_t, uint64_t);
		int add_one_and_run(DRAMSim::MultiChannelMemorySystem *mem, uint64_t addr);
};


void some_object::read_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
	printf("[Callback] read complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

void some_object::write_complete(unsigned id, uint64_t address, uint64_t clock_cycle)
{
	printf("[Callback] write complete: %d 0x%lx cycle=%lu\n", id, address, clock_cycle);
}

/* FIXME: this may be broken, currently */
void power_callback(double a, double b, double c, double d)
{
//	printf("power callback: %0.3f, %0.3f, %0.3f, %0.3f\n",a,b,c,d);
}


    /////////////////////////////////////
	  printf("dramsim_test main()\n");
	  some_object obj;
	  read_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::read_complete);
	  write_cb = new Callback<some_object, void, unsigned, uint64_t, uint64_t>(&obj, &some_object::write_complete);
	  /* pick a DRAM part to simulate */
	  mem = getMemorySystemInstance("ini/DDR2_micron_16M_8b_x8_sg3E.ini", "system.ini", "..", "example_app", 16384); 
    mem->RegisterCallbacks(read_cb, write_cb, power_callback);

	  printf("dramsim_test main()\n");
	  printf("-----MEM1------\n");
    /////////////////////////////////////

#endif