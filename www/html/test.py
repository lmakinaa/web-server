#!/usr/bin/env python3

import cgi
import cgitb
import os
import sys

# Enable detailed error reporting
cgitb.enable()

# Print the HTTP headers
print("Content-Type: text/html")
print()

try:
    # Create FieldStorage instance
    form = cgi.FieldStorage()

    # Get form fields
    name = form.getvalue('name', 'No Name')
    nickname = form.getvalue('nickname', 'No Nickname')
    
    # Handle file upload
    uploaded_file = None
    if 'pdfFile' in form:
        fileitem = form['pdfFile']
        if fileitem.filename:
            # Get file details
            filename = os.path.basename(fileitem.filename)
            file_content = fileitem.file.read()
            file_size = len(file_content)
            
            # You can save the file here if needed
            with open(f'/Users/ijaija/merge/www/uploads/{filename}', 'wb') as f:
                f.write(file_content)
            
            uploaded_file = {
                'filename': filename,
                'size': file_size,
                'type': fileitem.type
            }

    # Generate response HTML
    html = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Upload Result</title>
    </head>
    <body>
        <h1>Upload Result</h1>
        <p>Name: {name}</p>
        <p>Nickname: {nickname}</p>
        <p>File Upload Status:</p>
        """
    
    if uploaded_file:
        html += f"""
        <ul>
            <li>Filename: {uploaded_file['filename']}</li>
            <li>Size: {uploaded_file['size']} bytes</li>
            <li>Type: {uploaded_file['type']}</li>
        </ul>
        """
    else:
        html += "<p>No file was uploaded</p>"
    
    html += """
    </body>
    </html>
    """
    
    print(html)

except Exception as e:
    # Print any errors that occur
    print(f"""
    <h2>Error occurred:</h2>
    <pre>{str(e)}</pre>
    <h3>Environment Variables:</h3>
    <pre>
    {os.environ}
    </pre>
    """)