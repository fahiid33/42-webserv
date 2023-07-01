import http.client

url = 'localhost'
port = 8080
path = '/cgi-bin/cgi_python.py'
chunk_size = 20  # Specify your desired chunk size in bytes

# Open a connection to the server
conn = http.client.HTTPConnection(url, port)

# Send the initial request headers
headers = {
    'Transfer-Encoding': 'chunked',
    'Content-Type': 'application/octet-stream'
}
conn.putrequest('POST', path)
for key, value in headers.items():
    conn.putheader(key, value)
conn.endheaders()

# Open the file and send the data in chunks
with open('test.txt', 'rb') as file:
    while True:
        data = file.read(chunk_size)
        if not data:
            break

        # Send the chunked data
        conn.send(f'{hex(len(data))[2:]}\r\n'.encode('utf-8'))
        conn.send(data)
        conn.send('\r\n'.encode('utf-8'))

# Send the final chunk to indicate the end of the request
conn.send('0\r\n\r\n'.encode('utf-8'))

# Get the response from the server
response = conn.getresponse()
print(response.read().decode('utf-8'))

# Close the connection
conn.close()



