
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <stack>
#include "../main/main.hpp"

int parseConfigFile(std::string config, Main &main);
bool    isEmpty(std::string str);
std::string trimStr(std::string str);
std::vector<std::string>    split(std::string str, char delem);