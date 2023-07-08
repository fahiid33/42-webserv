#!/usr/bin/python3

#~~~~~~~~use it with upload.html~~~~~~~~~~~~~~~~~~~~~~~~~~~upload with form request~~~~~~~~~~~~~~~~start~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

import cgi
import os
import cgitb
import sys
import io


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
upload_path = form['upload_path']
print (upload_path.value)

# Test if the file was uploaded
if fileitem.filename:
    # Strip leading path from file name
    # to avoid directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    with open(upload_path.value + fn, 'wb') as f:
        print(upload_path.value + fileitem.filename)
        f.write(fileitem.file.read())
    message = f'The file "{fn}" was uploaded successfully'
else:
    message = 'No file was uploaded'

print("<html><body>")
print(f"<p>{message}</p>")
print("</body></html>")

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~upload with form request~~~~~~~~~~~~~~~~~end~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~