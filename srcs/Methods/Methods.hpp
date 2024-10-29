#pragma once

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include "../main/WebServ.hpp"
#include <algorithm>
#include <ctime>


std::string    _GET_DELETE(WebServ &main);
std::string sessionIdGen(Server &Serv);