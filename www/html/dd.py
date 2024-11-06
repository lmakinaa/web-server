#!/usr/bin/env python3
import cgi
import os
import http.cookies as Cookie
import random

# Set the HTTP header
print("Content-Type: text/html\r")
print("\r")

# Check if there's an existing session ID cookie
if 'HTTP_COOKIE' in os.environ:
    cookie = Cookie.SimpleCookie(os.environ['HTTP_COOKIE'])
    if 'session_id' in cookie:
        session_id = cookie['session_id'].value
    else:
        session_id = str(random.randint(1, 100))
        print(f"Set-Cookie: session_id={session_id}; Path=/")
else:
    session_id = str(random.randint(1, 100))
    print(f"Set-Cookie: session_id={session_id}; Path=/")

# Display the session ID

print(f"<html><body><p>id: {session_id}</p></body></html>")
