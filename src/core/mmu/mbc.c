#include "mbc.h"
#include "mbc0.h"
#include "mbc1.h"
#include "mbc3.h"
#include "../../non_core/logger.h"

int setup_MBC(int MBC_no) {

    //MMBC0
    if (MBC_no == 0) {

        read_MBC = &read_MBC0;
        write_MBC = &write_MBC0;  

    //MBC1
   } else if(MBC_no >= 1 && MBC_no <= 3) {
     
        setup_MBC1(MBC_no - 1);
        read_MBC = &read_MBC1;
        write_MBC = &write_MBC1;

   // MBC3
   } else if(MBC_no >= 0xF && MBC_no <= 0x13) {
    
        read_MBC = &read_MBC3;
        write_MBC = &write_MBC3;

   } else { 

       log_message(LOG_ERROR, "unimplimented MBC mode for code %d\n",MBC_no);
       return 0;
   }

   return 1;
}



