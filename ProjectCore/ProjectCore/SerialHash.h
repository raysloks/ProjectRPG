#ifndef SERIAL_HASH_H
#define SERIAL_HASH_H

#include <stdint.h>
#include <string>

uint32_t sdbm(const std::string& str);

#endif