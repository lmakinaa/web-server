
#include "Response.hpp"


int isDirectory(const std::string& path) {
    struct stat info;
    
    // Check if the path exists and get its information
    if (stat(path.c_str(), &info) != 0) {
        // Error accessing the path
        return -1; // Path does not exist
    } else {
        // Check if the path is a directory
        return (info.st_mode & S_IFDIR) != 0; // Return true if it's a directory
    }
}


bool    fileExist(std::string &path)
{
    if (access(path.c_str(), F_OK) == 0)
        return (1);
    return (0);
}


std::string getFileFullPath(Server &serv, std::map<std::string, Location>::iterator &it, std::string &requestPath)
{
    std::string root = "";
    std::string path = "";

    /* ===== Location doesn't have a root directive ====*/
    if (it->second.directives.find("root") == it->second.directives.end())
    {
        root = serv.directives["root"].values[0];

        path = root + requestPath;

        // path.replace(0, it->first.size(), root);

        int val = isDirectory(path);
        if (val == -1)
        {
            // throw 404NotFoundClass;
            std::cout << "0\n";
            return "";
        }
        else if (val == 1)
        {
            for (size_t i = 0; i < serv.directives["index"].values.size(); i++)
            {
                path +=  "/" + serv.directives["index"].values[i];
                if (fileExist(path))
                    return (path);
            }
            if (it->second.directives.find("return") != it->second.directives.end())
            {
                // should redirect to ...
                std::cout << "redirect to " << it->second.directives["return"].values[0] << std::endl;
            }
            else if (it->second.directives.find("autoindex") != it->second.directives.end()
                        &&  it->second.directives["autoindex"].values[0] == "on")
            {
                // should list all files
                std::cout << "list all files\n";
            }
            else
            {
                // throw 404NotFoundClass;
                std::cout << "1\n";
                return "";
            }
        }
        else if (val == 0)
        {
            if (fileExist(path))
                return (path);
            else
            {
                // throw 404NotFoundClass;
                std::cout << "2\n";
                return "";
            }
        }

    }
    else
    {
        root = it->second.directives["root"].values[0];

        path = requestPath;

        path.replace(0, it->first.size(), root);

        int val = isDirectory(path);
        if (val == -1)
        {
            // throw 404NotFoundClass;
            std::cout << "3\n";
            return "";
        }
        else if (val == 1)
        {
            for (size_t i = 0; i < it->second.directives["index"].values.size(); i++)
            {
                path += "/" +  it->second.directives["index"].values[i];
                if (fileExist(path))
                    return (path);
            }
            if (it->second.directives.find("return") != it->second.directives.end())
            {
                // should redirect to ...
                std::cout << "redirect to " << it->second.directives["return"].values[0] << std::endl;
            }
            else if (it->second.directives.find("autoindex") != it->second.directives.end()
                        &&  it->second.directives["autoindex"].values[0] == "on")
            {
                // should list all files
                std::cout << "list all files" << std::endl;
            }
            else
            {
                // throw 404NotFoundClass;
                std::cout << "4\n";
                return "";
            }
        }
        else if (val == 0)
        {
            if (fileExist(path))
                return (path);
            else
            {
                // throw 404NotFoundClass;
                std::cout << "5\n";
                return "";
            }
        }

    }

    return (path);
}


bool    stringMaching(std::string locat , std::string &requestPath)
{

    if (requestPath.find(locat) == 0)
        return (1);
    return (0);
}

void    Response(Main &main)
{
    Server serv = main.servers[0];
    std::string requestPath = "/includes";
    std::string resquestedFile = "";
    std::string line;
    std::string response = "";

    std::map<std::string, Location>::iterator it2 = serv.locations.begin();
    std::map<std::string, Location>::iterator it = serv.locations.end();
    

    /* ======= Looking for the request path in server locations ======= */
    for ( ; it2 != serv.locations.end(); it2++)
    {
        if (stringMaching((*it2).first , requestPath))
        {
            if (it == serv.locations.end() || it2->first.size() >= it->first.size())
                it = it2;
            
        }
    }


    /* ======= Found the matching path ======= */
    if (it != serv.locations.end())
    {
        resquestedFile = getFileFullPath(serv, it, requestPath);
        if (resquestedFile == "")
        {
            // throw 404NotFoundClass;
            std::cout << "404 NOt found" << std::endl;
            return ;
        }

        std::ifstream file(resquestedFile);

        if (!file) {
            std::cerr << "Error opening file for reading." << std::endl;
            return ;
        }

        while (std::getline(file, line)) {
            response += line;
        }

        file.close();
    }
    else
    {
        // throw 404NotFoundClass;
        std::cout << "404 NOt found" << std::endl;
    }

    // Send the file to the Client.
    
    std::cout << "Result : " << resquestedFile << std::endl;
}