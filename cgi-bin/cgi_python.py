#!/usr/bin/python3

#~~~~~~~~use it with upload.html~~~~~~~~~~~~~~~~~~~~~~~~~~~upload with form request~~~~~~~~~~~~~~~~start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# import cgi
# import os
# import cgitb
# import sys
# import io


# open('/Users/hlachkar/weba/Upload/papa', 'wb')
# cgitb.enable()

# try:
#     # Windows needs stdio set for binary mode.
#     import msvcrt
#     msvcrt.setmode(0, os.O_BINARY)  # stdin  = 0
#     msvcrt.setmode(1, os.O_BINARY)  # stdout = 1
# except ImportError:
#     pass

# # Wrap sys.stdin with io.TextIOWrapper to convert it to text mode
# sys.stdin = io.TextIOWrapper(sys.stdin.buffer, encoding='utf-8')

# form = cgi.FieldStorage()

# # A nested FieldStorage instance holds the file
# fileitem = form['file']

# # Test if the file was uploaded
# if fileitem.filename:
#     # Strip leading path from file name
#     # to avoid directory traversal attacks
#     fn = os.path.basename(fileitem.filename)
#     with open('/Users/hlachkar/weba/Upload/' + fn, 'wb') as f:
#         f.write(fileitem.file.read())
#     message = f'The file "{fn}" was uploaded successfully'
# else:
#     message = 'No file was uploaded'

# print("Content-Type: text/html\n")
# print("<html><body>")
# print(f"<p>{message}</p>")
# print("</body></html>")

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~upload with form request~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#~~~~~~~use it with postman binary data~~~~~~~~~~~~~~~~~~~~~~~~~~~upload binary files~~~~~~~~~~~~~~~~~~~~start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

import sys
import io
import os

# Wrap sys.stdin with io.BufferedReader to read binary data
sys.stdin = io.BufferedReader(sys.stdin.buffer)

# Read the request body as binary data
request_body = sys.stdin.read()

# Process the binary data
# Modify this section according to your specific processing logic
if request_body:
    # Process the binary data as needed
    # For example, you can save it to a file
    with open("/Users/hlachkar/weba/Upload/block.html", "wb") as f: # change the path to env variable
        f.write(request_body)
    message = "Binary data received and processed successfully"
else:
    message = "No binary data was received"

# Print the response
print("Content-Type: text/html\n")
print("<html><body>")
print(f"<p>{message}</p>")
print("</body></html>")

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~upload binary files~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~show the posted data~~~~~~~~~~~~~~~~~~~~~~start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# import os
# import sys

# # Get the content length from the environment variables

# # Read the specified number of bytes from stdin

# try:
#     content_length = int(os.environ.get('CONTENT_LENGTH', 0))
#     request_body = sys.stdin.read(content_length)
# except ValueError:
#     request_body = "no body"

# print ('<html>')
# print ('<head>')
# print ('<title>Hello World - First CGI Program</title>')
# print ('</head>')
# print ('<body>')
# print ('<h2>Hello World! This is my first CGI program</h2>')
# print(request_body)

# # for param in os.environ.keys():
# #     print ("%20s: %s" % (param, os.environ[param]))


# print ('</body>')
# print ('</html>')
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~show the posted data~~~~~~~~~~~~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~