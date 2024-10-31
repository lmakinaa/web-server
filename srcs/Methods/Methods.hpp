#pragma once

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include "../main/WebServ.hpp"
#include "../server/Server.hpp"
#include <algorithm>
#include <ctime>


std::string    _GET_DELETE(Server &serv, std::string requestPath, std::string _Method);
std::string sessionIdGen(Server &Serv);
std::string getSessionIdFromRequest(std::string cookies);