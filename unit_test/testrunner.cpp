#include "stdio.h"
#include "string.h"
#include "inttypes.h"
#include "string.h"

#include "test_ebv_local.h"
#include "test_ebv_iot.h"

#include "ebv_unit_compare.h"

int test_passed = 0;
int test_failed = 0;

#define RUN_GNSS_FUNCTIONS 0

int main(){
#if RUN_GNSS_FUNCTIONS == 1
    test_ebv_local_verify_gnss_response();
    test_ebv_local_parse_gnss_response();
#endif
    test_ebv_iot_list_builder_by_array();
    // Print summary
    printf("\nSummary: ");
    if(test_passed && !test_failed){
        printf("\033[32mAll test PASSED.\033[37m\n");
    } else if(test_passed && test_failed){
        printf("\033[32m%d PASSED\033[31m %d FAILED.\033[37m\n", test_passed, test_failed);
    }else if(!test_passed && test_failed){
        printf("\033[31mAll test FAILED\033[37m");
    } else {
        printf("No test was executed");
    }
return 0;
}