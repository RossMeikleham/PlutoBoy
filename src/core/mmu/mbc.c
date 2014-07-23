#include "mbc.h"
#include "mbc0.h"
#include "mbc1.h"
#include "../../non_core/logger.h"

int setup_MBC(int MBC_no) {

   if (MBC_no == 0) {

     read_MBC = &read_MBC0;
     write_MBC = &write_MBC0;  

   } else if(MBC_no >= 1 && MBC_no <= 3) {
     
     setup_MBC1(MBC_no - 1);
     read_MBC = &read_MBC1;
     write_MBC = &write_MBC1;

   } else {

       log_message(LOG_ERROR, "unimplimented MBC mode for code %d\n",MBC_no);
       return 0;
   }

   return 1;
}



