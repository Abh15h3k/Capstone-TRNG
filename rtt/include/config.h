#pragma once

#ifndef CONFIG_H_UMA9BRL0
#define CONFIG_H_UMA9BRL0

#include <sys/types.h>
#include <vector>
#include <string>

static size_t nbits = 32; // maybe dont change this
static bool print_binary = false;
static std::vector<std::string> urls {
    "www.google.com",
    "www.facebook.com",
    "www.reddit.com",
    "web.whatsapp.com",
    "stackoverflow.com"
};

#endif /* end of include guard: CONFIG_H_UMA9BRL0 */
