# webserver
for materials and code around webserver

Webserver puts all the inforamtion for a specific page



🌐 What is a Web Server? (In Simple Terms)
Think of a web server as a waiter at a restaurant:

You (the user) sit at a table and make a request (you ask for a menu item, like a burger).

The waiter (the web server) takes your request to the kitchen (the backend or files on the server).

The kitchen prepares your food and gives it back to the waiter.

The waiter brings it to your table — you get your burger!

In tech terms:

You (the browser) ask for a website (like www.google.com).

The web server receives your request.

It finds the right file (like an HTML page or image).

It sends it back to your browser to show it.



🧠 Why Do We Use a Web Server?
Because it's the middleman between:

Your browser (Chrome, Firefox, etc.)

The files or logic that make up a website

It makes websites accessible from anywhere in the world. Without a web server, websites would just sit on someone’s computer.



🛠️ How Does a Web Server Work? (Step-by-Step)
Here’s a simplified version of what happens when you visit a website:

You type a URL in your browser: http://mywebsite.com

Browser sends a request over the internet to the web server.

The web server receives the request.

It finds the right file (like index.html) on the computer.

It sends that file back to your browser.

Your browser shows the page to you.



🔧 How Do You Build One? (Concept Before Code)
Building a basic web server from scratch is like building your own waiter instead of hiring one — you tell it exactly what to do.

Here’s the step-by-step plan (we'll dive deeper later, but this is the high-level view):

Understand how computers talk (via network protocols):

Learn about HTTP — the language of web requests.

Learn about TCP/IP — the way computers send data to each other.

Set up a socket:

This is like opening a door on your computer that listens for web traffic (called a port, usually port 80 for websites).

Accept connections from browsers:

Wait for someone to ask for something (like a web page).

Read the browser's request:

"GET /index.html HTTP/1.1"

Find the requested file on your machine:

Check your www/ folder or something similar.

Send back the file:

Return the HTML, image, or whatever is needed.

Close the connection or keep it alive (depending on HTTP version).



🧱 What Will You Likely Have to Implement for 42?
At 42, your webserver project usually requires:

Listening to HTTP requests

Serving static files (like HTML, CSS, JS)

Handling multiple clients (maybe with select() or poll())

Handling errors (404, 403, etc.)

Possibly supporting CGI (run server-side scripts)

Configuration parsing (like nginx.conf)



📡 Step 1: What is HTTP?
🔤 HTTP stands for:
HyperText Transfer Protocol

It's the language your browser and a web server use to talk to each other. Just like two people using English to have a conversation, computers use HTTP to ask for and send web pages.



🧾 What Does an HTTP Request Look Like?
Let’s say you open your browser and visit:

arduino
Copy
Edit
http://example.com/index.html
The browser sends a request like this:

pgsql
Copy
Edit
GET /index.html HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0
👉 This tells the server:
“Hey! I want the file index.html. I'm using HTTP version 1.1. I'm coming from the domain example.com. I’m a browser!”



📨 What Does an HTTP Response Look Like?
If everything goes well, the web server replies with something like this:

php-template
Copy
Edit
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1254

<html>
  <head><title>Welcome</title></head>
  <body>Hello, world!</body>
</html>
This response has two parts:

Headers (meta-information like file type, size, status)

Body (the actual HTML page the browser displays)



❌ What if the Page Doesn’t Exist?
The server might respond with something like:

php-template
Copy
Edit
HTTP/1.1 404 Not Found
Content-Type: text/html

<html><body>Page not found</body></html>




✅ Key HTTP Methods to Know
GET: Ask for a file or page

POST: Send data to the server (used in forms)

DELETE: Ask the server to delete something (less common)

PUT: Upload or replace something

For your 42 project, you’ll mainly implement GET (and maybe POST).




📦 Summary of HTTP
Term	Meaning
GET /file.html	The browser is requesting a file
HTTP/1.1 200 OK	The server is responding with success
404 Not Found	The file wasn’t found
Content-Type	Tells the browser what kind of file it is (HTML, image, etc.)

HTTP is stateless — each request is like a brand-new conversation. The server doesn’t remember anything from the last request (unless you add sessions or cookies).




📦 Step 2: What is TCP/IP?
🔌 TCP/IP is the system that lets computers talk to each other across the internet.
It’s actually two protocols working together:

1. IP (Internet Protocol)
This is the address system — it helps find the computer you're talking to.

Every computer on the internet has an IP address, like:

scss
Copy
Edit
192.168.1.1
172.217.10.14 (Google)
So when you type www.example.com, your computer uses DNS to find out its IP address, and then sends data there.

2. TCP (Transmission Control Protocol)
This is the delivery system — it makes sure your message gets there reliably and in the right order.

TCP:

Breaks your data into packets (tiny chunks)

Sends them over the internet

Makes sure they arrive safely and correctly

Re-assembles them in the correct order

Without TCP, you’d get broken websites or missing parts.




📍 Imagine it like this:
You send a letter using:

IP: You write the address on the envelope (where to send it)

TCP: You use a reliable shipping company (FedEx) that guarantees delivery



🌐 How It All Comes Together (Browser to Server)
Let’s say you go to http://example.com:

Browser asks DNS: “What’s the IP of example.com?”

DNS replies: 93.184.216.34

Browser opens a TCP connection to that IP (usually on port 80 for HTTP)

It sends an HTTP request over that connection.

The web server gets the request, sends back an HTTP response.

Your browser shows you the web page.



🔢 What is a Port?
Every service on a computer runs on a port number — kind of like different doors into the same house.

HTTP = port 80

HTTPS = port 443

FTP = port 21

So when you connect to 93.184.216.34:80, you’re saying:

“Hey server, I want to talk to the web service (port 80).”




🛠️ How You Use TCP/IP in Your Webserver Project
When you write your own web server, you’ll need to:

Open a TCP socket on port 80 (or another port like 8080 for testing).

Wait for clients to connect

Receive their HTTP request

Send back an HTTP response

Close the connection (or keep it open)



🧠 Summary: TCP/IP vs HTTP
Layer	What it Does	Example
HTTP	The message format	"GET /index.html"
TCP	The delivery system	Ensures message arrives
IP	The addressing system	192.168.1.5 → 172.217.10.14

HTTP is like what you say, TCP/IP is how it gets there.




🔌 Step 3: What is a Socket?
🧠 Think of a socket as a "plug" in your computer:
It lets your computer send and receive data over the network.

When two computers want to talk:

They both open a socket

They connect to each other (like plugging cables together)

Then they send and receive messages (like HTTP requests and responses)




🗣️ Why Do We Use Sockets?
Because we need a way to:

Accept incoming connections from browsers

Read their requests

Send them back web pages

Without sockets, your program wouldn't know how to "hear" someone trying to visit your server.



🛠️ What Happens When You Use Sockets in a Server?
Here’s the high-level process your webserver will follow using sockets:

Create a socket 🛠️

Bind it to a port (e.g., port 8080)

Listen for incoming connections 🔊

Accept a connection from a browser

Receive data (the HTTP request) 📥

Send data back (the HTTP response) 📤

Close the connection 🔒



🧾 Pseudocode Example of a Simple Web Server
This is a simplified "human-readable" version of what your server does:

plaintext
Copy
Edit
Start server:
    Create a socket
    Bind it to a port (e.g., 8080)
    Listen for incoming connections

Loop:
    Accept a connection
    Read the HTTP request from the client
    Process the request:
        - Look for the requested file (like index.html)
        - If found, prepare a "200 OK" response with the file content
        - If not found, prepare a "404 Not Found" response
    Send the HTTP response back to the client
    Close the connection





🧱 First: The Big Picture
You want a structure like:

csharp
Copy
Edit
getConfig()     → Load settings (e.g., port, root dir, etc.)
setupSocket()   → Create the socket
bind()          → Bind socket to IP/port
listen()        → Mark socket as passive (waiting for clients)
accept()        → Accept a new client connection
recv()          → Receive HTTP request from client
parseRequest()  → Extract method, path, headers, etc.
handleRequest() → Look for the file, handle errors
buildResponse() → Format HTTP response (headers + body)
send()          → Send response back to client
This is exactly how a real webserver is structured.
Now let’s go through the code line by line, and then match it to your desired structure.



