
import os
import sys

# Get the content length from the environment variables

# Read the specified number of bytes from stdin

try:
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    request_body = sys.stdin.read(content_length)
except ValueError:
    request_body = "no body"

print ('<html>')
print ('<head>')
print ('<title>Hello World - First CGI Program</title>')
print ('</head>')
print ('<body>')
print ('<h2>Hello World! This is my first CGI program</h2>')
print(request_body)

for param in os.environ.keys():
    print ("%20s: %s" % (param, os.environ[param]))


print ('</body>')
print ('</html>')