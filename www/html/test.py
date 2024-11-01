#!/usr/bin/env python3
import os
import random
from http import cookies
import pickle
import uuid

def session_start():
    cookie = cookies.SimpleCookie()
    session_data = {}
    session_id = None
    is_new_session = True
    
    if 'HTTP_COOKIE' in os.environ:
        cookie.load(os.environ['HTTP_COOKIE'])
        if 'PYSESSID' in cookie:
            session_id = cookie['PYSESSID'].value
            session_file = f"/tmp/sessions/sess_{session_id}"
            if os.path.exists(session_file):
                with open(session_file, 'rb') as f:
                    session_data = pickle.load(f)
                is_new_session = False
                return session_data, session_id, is_new_session
    
    session_id = str(uuid.uuid4())
    return session_data, session_id, is_new_session

# Start session and get data
session, session_id, is_new_session = session_start()

# Output headers with proper \r\n line endings
print("Content-Type: text/html\r", end='')
print("\n", end='')
if is_new_session:
    cookie = cookies.SimpleCookie()
    cookie['PYSESSID'] = session_id
    cookie['PYSESSID']['path'] = '/'
    cookie['PYSESSID']['httponly'] = True
    # Manual output of Set-Cookie to ensure proper line ending
    print(f"Set-Cookie: {cookie['PYSESSID'].OutputString()}\r", end='')
    print("\n", end='')

# Extra \r\n to end headers
print("\r", end='')
print("\n", end='')

# Session logic
if 'x' not in session:
    session['x'] = random.randint(100, 1000)
    if not os.path.exists('/tmp/sessions'):
        os.makedirs('/tmp/sessions', mode=0o733)
    with open(f"/tmp/sessions/sess_{session_id}", 'wb') as f:
        pickle.dump(session, f)

print(f"id: {session['x']}")