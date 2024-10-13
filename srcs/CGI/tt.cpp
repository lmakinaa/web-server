#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Path to the PHP executable
    char *phpPath = "/usr/bin/php"; // Update this to your PHP path

    // Path to the PHP script
    char *scriptPath = "/Users/ijaija/web-server/srcs/CGI/test.php"; // Update this to your script path

    // Prepare the query string
    char *queryString = "var1=dfsd"; // Example query string

    // Create an environment variable for the query string
    char *env[] = { 
        "QUERY_STRING=var1=ada", 
        NULL 
    };

    // Create the arguments for execv
    char *args[] = { phpPath, scriptPath, NULL };

    // Execute the PHP script with the query string
    if (execve(phpPath, args, env) == -1) {
        perror("execve failed");
        exit(EXIT_FAILURE);
    }

    // This line will not be executed if execve is successful
    return 0;
}

