#ifndef _DEFINES_H_
#define _DEFINES_H_

#pragma once

#include <signal.h>
#include <stdint.h>
#include <conio.h>

#include <iostream>

#include <boost\program_options.hpp>
#include <boost\thread\mutex.hpp>
#include <boost\filesystem.hpp>

#include "logic\PhonemsNet.h"

enum LOAD_OPTIONS_ENUM {
	none = 0, dict_from_file
};

#endif