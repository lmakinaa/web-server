
#include "Methods.hpp"

std::string sessionIdGen(Server &Serv)
{
    std::string session_id;
    std::string elems = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    srand(time(NULL));
    while (true)
    {
        session_id = "";

        for (int i = 0; i < 20; i++)
        {
            session_id += elems[rand() % 62];
        }
        if (Serv.session_ids.find(session_id) == Serv.session_ids.end())
        {
            Serv.session_ids[session_id] = time(0);
            break ;
        }
    }

    return (session_id);
    
}

std::string getSessionIdFromRequest(std::string cookies) {
    size_t pos = cookies.find("SESSID=");
    if (pos != std::string::npos) {
        size_t end = cookies.find(';', pos);
        if (end == std::string::npos) {
            end = cookies.length();
        }
        return cookies.substr(pos + 7, end - (pos + 7));
    }
    return "";
}

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


std::string    listAllfiles(std::string path, Server &Serv, std::string reqpath)
{
    std::string file = "<!DOCTYPE html>\n"
                       "<html lang=\"en\">\n"
                       "<head>\n"
                       "    <meta charset=\"UTF-8\">\n"
                       "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                       "    <title>Directory Listing</title>\n"
                       "    <style>\n"
                       "        body {\n"
                       "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n"
                       "            background-color: #f0f0f0;\n"
                       "            margin: 0;\n"
                       "            padding: 0;\n"
                       "        }\n"
                       "        header {\n"
                       "            background-color: #4CAF50;\n"
                       "            color: white;\n"
                       "            padding: 20px;\n"
                       "            text-align: center;\n"
                       "            font-size: 24px;\n"
                       "        }\n"
                       "        .container {\n"
                       "            max-width: 800px;\n"
                       "            margin: 30px auto;\n"
                       "            padding: 20px;\n"
                       "            background-color: #fff;\n"
                       "            border-radius: 10px;\n"
                       "            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);\n"
                       "        }\n"
                       "        ul {\n"
                       "            list-style-type: none;\n"
                       "            padding: 0;\n"
                       "            margin: 0;\n"
                       "        }\n"
                       "        li {\n"
                       "            padding: 12px;\n"
                       "            border-bottom: 1px solid #eee;\n"
                       "            display: flex;\n"
                       "            justify-content: space-between;\n"
                       "            align-items: center;\n"
                       "        }\n"
                       "        li:last-child {\n"
                       "            border-bottom: none;\n"
                       "        }\n"
                       "        a {\n"
                       "            color: #4CAF50;\n"
                       "            text-decoration: none;\n"
                       "            font-weight: bold;\n"
                       "        }\n"
                       "        a:hover {\n"
                       "            text-decoration: underline;\n"
                       "        }\n"
                       "        .file-info {\n"
                       "            color: #555;\n"
                       "            font-size: 14px;\n"
                       "        }\n"
                       "        .icon {\n"
                       "            font-size: 20px;\n"
                       "            margin-right: 10px;\n"
                       "        }\n"
                       "        .file {\n"
                       "            display: flex;\n"
                       "            align-items: center;\n"
                       "        }\n"
                       "    </style>\n"
                       "</head>\n"
                       "<body>\n"
                       "    <header>\n"
                       "        Directory Listing\n"
                       "    </header>\n"
                       "    <div class=\"container\">\n"
                       "        <ul>\n";


    DIR* dir;
    struct dirent* entry;

    // Open the current directory
    M_DEBUG && std::cerr << "Path: " << path << std::endl;
    dir = opendir(path.c_str());
    if (dir == NULL) {
        M_DEBUG && std::cerr << "Error: Could not open current directory" << std::endl;
        return "";
    }

    if (reqpath.back() != '/')
        reqpath += "/";
    while ((entry = readdir(dir)) != NULL) {

            if (entry->d_type == DT_DIR) {
                file += "<li>"
                    "<div class=\"file\">"
                    "<span class=\"icon\">📁</span>";

                file += "<a href=\"http://" + Serv.directives["host"].values[0] + ":" + Serv.directives["listen"].values[0] + reqpath + static_cast<std::string>(entry->d_name) + "\">" + entry->d_name + "</a>";

                file += "</div>"
                "</li>";
            } else {
                file += "<li>"
                    "<div class=\"file\">"
                    "<span class=\"icon\">📄</span>";

                file += "<a href=\"http://" + Serv.directives["host"].values[0] + ":" + Serv.directives["listen"].values[0] + reqpath + static_cast<std::string>(entry->d_name) + "\">" + entry->d_name + "</a>";

                file += "</div>"
                "</li>";
            }
    }

    closedir(dir);


    file += "</ul>"
        "</div>"
        "</body>"
        "</html>";

    // std::ofstream ofs("filelisting.html", std::ios::out | std::ios::trunc);
    unlink("filelisting.html");
    int fd = open("filelisting.html",  O_WRONLY | O_TRUNC | O_CREAT, 0644);

    write(fd, file.c_str(), file.size());

    close(fd);
    return ("filelisting.html");

}

std::string getFileFullPath(Server &serv, std::map<std::string, Location>::iterator &it, std::string &requestPath)
{
    std::string root = "";
    std::string path = "";
    std::string _Method = "GET";
    std::string sec_path = "";

    /* ===== Location doesn't have a root directive ====*/
    if (it->second.directives.find("root") == it->second.directives.end())
    {
        root = serv.directives["root"].values[0];

        path = requestPath;

        path.replace(0, it->first.size(), root);

        int val = isDirectory(path);
        if (val == -1)
        {
            // throw 404NotFoundClass;
             throw ErrorStatus(404, "Path does not exist");
        }
        else if (val == 1)
        {
            if (_Method == "DELETE")
            {
                // throw 403 Forbidden
                throw ErrorStatus(403, "Can't delete a folder");
            }
            sec_path = path;
            for (size_t i = 0; i < serv.directives["index"].values.size(); i++)
            {
                path +=  "/" + serv.directives["index"].values[i];
                if (fileExist(path))
                    return (path);
            }
            if (serv.directives.find("return") != serv.directives.end())
            {
                // should redirect to ...
                std::cout << "redirect to " << serv.directives["return"].values[0] << std::endl;
            }
            else if (serv.directives.find("autoindex") != serv.directives.end()
                        &&  serv.directives["autoindex"].values[0] == "on")
            {
                // should list all files
                std::cout << "list all files\n";
                return (listAllfiles(sec_path, serv, requestPath));
            }
            else
            {
                // throw 403ForbiddenClass;
                throw ErrorStatus(403, "index file not found in directory");
            }
        }
        else if (val == 0)
        {
            if (fileExist(path))
                return (path);
            else
            {
                // throw 404NotFoundClass;
                throw ErrorStatus(404, "file doesn't exist");
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
            throw ErrorStatus(404, "Path does not exist");
        }
        else if (val == 1)
        {

            if (_Method == "DELETE")
            {
                // throw 403 Forbidden
                throw ErrorStatus(403, "Can't delete folder");
            }
            sec_path = path;
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
                std::cout << "list all files\n";
                return (listAllfiles(sec_path, serv, requestPath));
            }
            else
            {
                // throw 403ForbiddenClass;
                throw ErrorStatus(403, "idex file doesn't exit");
            }
        }
        else if (val == 0)
        {
            if (fileExist(path))
                return (path);
            else
            {
                // throw 404NotFoundClass;
                throw ErrorStatus(404, "File doesn't exist");
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

std::string    _GET_DELETE(Server &serv, std::string requestPath, std::string _Method)
{
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
            // std::cout << "404 NOt found" << std::endl;
            throw ErrorStatus(404, "1 in _GET_DELETE");
        }

        /* ===== Check Read Permession ===== */
        if (access(resquestedFile.c_str(), R_OK) != 0)
        {
            // Throw 403 Forbidden
            throw ErrorStatus(403, "2 in _GET_DELETE");
        }
    }
    else
    {
        // throw 404NotFoundClass;
        throw ErrorStatus(404, "3 in _GET_DELETE");
    }

    if (_Method == "DELETE")
    {
        // unlink(resquestedFile.c_str());
        // throw 204 No Content
    }

    // Send the file to the Client.
    std::cout << "Result : " << resquestedFile << std::endl;
    return (resquestedFile);
}