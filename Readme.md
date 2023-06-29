To upload a binary from your computer
curl --request POST --data-binary "@img.jpg" http://localhost:8080/cgi-bin/cgi_python.py
img.jpg <==> the file you want to upload
cgi-bin/cgi_python.py <==> is the cgi script that handle the binary upload

To upload a file, image, anything using browser, go to http://localhost:8080/upload.html
select your file 
click on send
this one uses another cgi script: /cgi-bin/cgi_upload_form.py

...