#!/usr/bin/env python3
import os
import cgi
import cgitb
cgitb.enable()

form = cgi.FieldStorage()

print("Content-Type: text/plain;charset=utf-8")
print()

if os.environ['REQUEST_METHOD'] == 'GET':
    print("GET var1: %s" % form.getvalue('var1', 'not set'))
    print("GET var2: %s" % form.getvalue('var2', 'not set'))
elif os.environ['REQUEST_METHOD'] == 'POST':
    print("POST var1: %s" % form.getvalue('var1', 'not set'))
    print("POST var2: %s" % form.getvalue('var2', 'not set'))
