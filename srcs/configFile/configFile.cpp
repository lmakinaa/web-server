

#include "configFile.hpp"

bool    isEmpty(std::string str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (!std::isspace(str[i]))
            return (0);
    }
    return (1);
}

std::string trimStr(std::string str)
{
    std::string newStr = "";

    int start = 0;
    while ((size_t)start < str.size() && std::isspace(str[start]))
        start++;
    
    int end = str.size() - 1;
    while (end >= 0 && std::isspace(str[end]))
        end--;
    return (str.substr(start,  end - start + 1));
}

bool    invalidBrackets(std::vector<std::string>& conf)
{
    std::stack<char> par;

    for (size_t i = 0; i < conf.size(); i++)
    {
        for (size_t j = 0; j < conf[i].size(); j++)
        {
            if (conf[i][j] == '{')
                par.push(conf[i][j]);
            if (conf[i][j] == '}')
            {
                if (par.empty())
                    return (1);
                par.pop();
            }
        }
    }

    if (!par.empty())
        return (1);

    return (0);
}

std::vector<std::string>    split(std::string str, char delem)
{
    std::vector<std::string> v;
    std::string part;


    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == delem)
        {
            if (!part.empty())
            {
                v.push_back(part);
                part.clear();
            }
        }
        else
            part += str[i];
    }

    if (!part.empty())
        v.push_back(part);
    return (v);
}


int checkLocation(std::vector<std::string> &conf, size_t i, Location &loc)
{
    std::vector<std::string> directive;

    for ( ; i < conf.size(); i++)
    {

        if (isEmpty(conf[i]))
            continue ;

        directive = split(conf[i], ' ');
        if (directive.size() == 1 && directive[0] == "}")
        {
            return (i);
        }
        else
        {
            if (directive[directive.size() - 1][directive[directive.size() - 1].size() - 1] != ';')
            {
                std::cout << "Syntaxe error in line " << i + 1 << ": missing semi-colon" << std::endl;
                return (-1);
            }
            else
            {
                if (loc.isAllowedDirective(directive[0]))
                {
                    for (size_t i = 1; i < directive.size(); i++)
                    {
                        if (i == directive.size() - 1 && directive[i].back() == ';')
                            directive[i].pop_back();
                        loc.directives[directive[0]].values.push_back(directive[i]);
                    }
                }
                else
                {
                    std::cout << "Syntaxe error in line " << i + 1 << ": invalid directive" << std::endl;
                    return (-1);
                }
            }
        }
    }

    return (i);
}


int checkDirectives(std::vector<std::string> &conf, size_t i, Server &serv)
{

    std::vector<std::string> directive;

    for ( ; i < conf.size(); i++)
    {

        if (isEmpty(conf[i]))
            continue ;

        directive = split(conf[i], ' ');
        if (directive.size() > 0 && directive[0] == "location")
        {
            if (directive.size() != 3 || directive[0] != "location" || directive[2] != "{")
            {
                std::cout << "Syntaxe error in line " << i + 1 << ": invalid location block" << std::endl;
                return (-1);
            }
            Location loc;
            int n = checkLocation(conf, i + 1, loc);
            if (n == -1)
                return (-1);
            serv.locations[directive[1]] = loc;
            i = n;
        }
        else if (directive.size() == 1 && directive[0] == "}")
        {
            return (i);
        }
        else
        {
            if (directive[directive.size() - 1][directive[directive.size() - 1].size() - 1] != ';')
            {
                std::cout << "Syntaxe error in line " << i + 1 << ": missing semi-colon" << std::endl;
                return (-1);
            }
            else
            {
                if (serv.isAllowedDirective(directive[0]))
                {
                    for (size_t i = 1; i < directive.size(); i++)
                    {
                        if (i == directive.size() - 1 && directive[i].back() == ';')
                            directive[i].pop_back();
                        serv.directives[directive[0]].values.push_back(directive[i]);
                    }
                }
                else
                {
                    std::cout << "Syntaxe error in line " << i + 1 << ": invalid directive" << std::endl;
                    return (-1);
                }
            }
        }
    }

    return (i);
}

int checkServerBlock(std::vector<std::string> &conf, Main &main)
{

    std::vector<std::string> block;

    for (size_t i = 0; i < conf.size(); i++)
    {

        if (isEmpty(conf[i]))
            continue ;

        block = split(conf[i], ' ');
        if (block.size() != 2 || block[0] != "server" || block[1] != "{")
        {
            std::cout << "Syntaxe error in line " << i + 1 << ": invalid server block" << std::endl;
            return (-1);
        }
        Server serv;
        int n = checkDirectives(conf, i + 1, serv);
        if (n == -1)
            return (-1);
        main.servers.push_back(serv);
        i = n;   
    }

    return (1);
}


int parseConfigFile(std::string config, Main &main)
{
    std::ifstream file(config);

    if (!file)
        return (0);
    
    std::vector<std::string> confLines;
    std::string line;
    while (std::getline(file, line))
    {
        confLines.push_back(trimStr(line));
    }


    if (invalidBrackets(confLines))
    {
        std::cerr << "Syntaxe error : invalid brackets" <<std::endl;
        return (-1);
    }

    if (checkServerBlock(confLines, main) == -1)
        return (-1);

    if (main.servers.size() < 1)
    {
        std::cerr << "Syntaxe error : no server block" <<std::endl;
        return (-1);
    }

    return (1);
}