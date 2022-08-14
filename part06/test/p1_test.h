#pragma once

// test result
void p1_test_result01(void); // nwant = 0; nresult = 0;
void p1_test_result02(void); // nwant = 0; nresult = 1;
void p1_test_result03(void); // nwant = 0; nresult > 1;
void p1_test_result04(void); // nwant = 1; nresult = 0;
void p1_test_result05(void); // nwant = 1; nresult = 1;
void p1_test_result06(void); // nwant = 1; nresult > 1;
void p1_test_result07(void); // nwant > 1; nresult = 0;
void p1_test_result08(void); // nwant > 1; nresult > 0;
void p1_test_result09(void); // nwant = -1; nresult = 0;
void p1_test_result10(void); // nwant = -1; nresult > 0;

void p1_test_nestcall01(void); // call count < LUA_MAXCALLS
void p1_test_nestcall02(void); // call count >= LUA_MAXCALLS
