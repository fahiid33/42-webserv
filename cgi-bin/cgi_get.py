#!/usr/bin/python3
# Import modules for CGI handling
import cgi, cgitb
import os

# BEGIN: 7d5f8a9j4d3h
import http.cookies

cookie_string = os.environ.get('HTTP_COOKIE')
# END: 7d5f8a9j4d3h
form = cgi.FieldStorage()
# Get data from fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')
print ("<html>")
print ("<head>")
print ("<title>Hello - Second CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello %s %s</h2>" % (first_name, last_name))
if cookie_string:
    cookie = http.cookies.SimpleCookie()
    cookie.load(cookie_string)
    if 'lala' in cookie:
        lala_value = cookie['lala'].value
        print(f"Value of cookie 'lala': {lala_value}")
    else:
        print("Cookie 'lala' does not exist")
print ("</body>")
print ("</html>")