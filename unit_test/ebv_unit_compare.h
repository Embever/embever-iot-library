#ifndef INC_EBV_UNIT_COMPARE
#define INC_EBV_UNIT_COMPARE

#define _DBG_COLOR_RED "\033[31m"
#define _DBG_COLOR_GREEN "\033[32m"
#define _DBG_COLOR_YELLOW "\033[33m"
#define _DBG_COLOR_BLUE "\033[34m"
#define _DBG_COLOR_MAGENTA "\033[35m"
#define _DBG_COLOR_CYAN "\033[36m"
#define _DBG_COLOR_WHITE "\033[37m"

extern int test_passed;
extern int test_failed;

//#define LOG_TEST_RES_PASSED(TESTCASE)               printf("TEST GROUP: \033[35m%s\033[37m ==> %s \t\033[32mPASSED\033[37m\n", __FUNCTION__, TESTCASE); test_passed++;
#define LOG_TEST_RES_PASSED2                        printf("TEST GROUP: \033[35m%s:%d %s()\033[37m ==> \t\033[32mPASSED\033[37m\n", __FILE__, __LINE__, __FUNCTION__); test_passed++;
//#define LOG_TEST_RES_FAILED(TESTCASE)               printf("TEST GROUP: \033[35m%s\033[37m ==> %s \t\033[31mFAILED\033[37m\n", __FUNCTION__, TESTCASE); test_failed++;
#define LOG_TEST_RES_FAILED2                        printf("TEST GROUP: \033[35m%s:%d %s()\033[37m ==> \t\033[31mFAILED\033[37m\n", __FILE__, __LINE__, __FUNCTION__); test_failed++;
//#define LOG_TEST_RES_FAILED_NOT_EQ(x,y, TESTCASE)   printf("TEST GROUP: \033[35m%s\033[37m ==> %s \t\033[31mFAILED\033[37m %d != %d\n", __FUNCTION__, TESTCASE, x, y); test_failed++;
#define LOG_TEST_RES_FAILED_NOT_EQ2(x,y)            printf("TEST GROUP: \033[35m%s:%d %s()\033[37m ==> \t\033[31mFAILED\033[37m %d != %d\n", __FILE__, __LINE__, __FUNCTION__, x, y); test_failed++;
//#define TEST_EQUAL(x,y,testcase)                    if(x==y){LOG_TEST_RES_PASSED(testcase);}else{LOG_TEST_RES_FAILED_NOT_EQ(x,y,testcase);}
#define TEST_EQUAL(x,y)                             if(x==y){LOG_TEST_RES_PASSED2}else{LOG_TEST_RES_FAILED_NOT_EQ2(x,y);}
#define TEST_ARR_EQ(x,y,len)                        if(!memcmp((const char *)x,(const char *)y,len)){LOG_TEST_RES_PASSED2}else{LOG_TEST_RES_FAILED2}

#endif