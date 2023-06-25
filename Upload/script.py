def generate_hello_html():
    html = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Hello, World!</title>
    </head>
    <body>
        <h1>Hello, World!</h1>
    </body>
    </html>
    """
    return html

if __name__ == "__main__":
    # Generate the HTML content
    html_content = generate_hello_html()

    # Set the appropriate headers
    print("Content-Type: text/html")
    print("Content-Length:", len(html_content))
    print()

    # Print the HTML content
    print(html_content)