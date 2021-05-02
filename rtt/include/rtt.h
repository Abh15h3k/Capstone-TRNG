#pragma once
#ifndef RTT_H_DVX4U7H8
#define RTT_H_DVX4U7H8

#include <string>

std::string hostname_to_ip(std::string &ip_str);
uint8_t nextValue(void);
void printHelp(void);
bool repetitionCountTest(int64_t value);
bool adaptiveProportionTest(int64_t value);

#endif /* end of include guard: RTT_H_DVX4U7H8 */
