#!/usr/bin/python3

#~~~~~~~~use it with upload.html~~~~~~~~~~~~~~~~~~~~~~~~~~~upload with form request~~~~~~~~~~~~~~~~start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

import cgi
import os
import cgitb
import sys
import io


open('/Users/hlachkar/weba/Upload/papa', 'wb')
cgitb.enable()

try:
    # Windows needs stdio set for binary mode.
    import msvcrt
    msvcrt.setmode(0, os.O_BINARY)  # stdin  = 0
    msvcrt.setmode(1, os.O_BINARY)  # stdout = 1
except ImportError:
    pass

# Wrap sys.stdin with io.TextIOWrapper to convert it to text mode
sys.stdin = io.TextIOWrapper(sys.stdin.buffer, encoding='utf-8')

form = cgi.FieldStorage()

# A nested FieldStorage instance holds the file
fileitem = form['file']

# Test if the file was uploaded
if fileitem.filename:
    # Strip leading path from file name
    # to avoid directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    with open('/Users/hlachkar/weba/Upload/' + fn, 'wb') as f:
        f.write(fileitem.file.read())
    message = f'The file "{fn}" was uploaded successfully'
else:
    message = 'No file was uploaded'

print("Content-Type: text/html\n")
print("<html><body>")
print(f"<p>{message}</p>")
print("</body></html>")

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~upload with form request~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~