import network
import socket
import wifi_credentials
import web_page

ssidAP = wifi_credentials.ssidAP
passwordAP = wifi_credentials.passwordAP

local_IP = '192.168.1.10'
# gateway = '192.168.1.1'
# subnet = '255.255.255.0'
# dns = '8.8.8.8'

wlan = network.WLAN(network.AP_IF)

def AP_Setup(ssidAP,passwordAP):
  print("Setting soft-AP  ... ")
  wlan.active(True)
  wlan.config(essid=ssidAP, password=passwordAP)
  print('Success, IP address:', wlan.ifconfig())
  print("Setup End\n")

try:
  AP_Setup(ssidAP,passwordAP)
except:
  print("Failed, please disconnect the power and restart the operation.")
  wlan.disconnect()

# Socket server initialization
s_addr = socket.getaddrinfo(local_IP, 80)[0][-1]
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(s_addr)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.listen(5)
print('Open socket server listening on adr {}.'.format(s_addr))

while 1:
  print('Waiting for a connection...')
  conn, addr = s.accept() 
  print("Got connection from %s" % str(addr))
  
  request = conn.recv(1024).decode('utf-8')
  print("Request:", request)
  
  # Parse the HTTP request
  if "POST /joystick" in request:
    # Handle joystick data
    content_length = int(request.split("Content-Length: ")[1].split("\r\n")[0])
    body_start = request.find("\r\n\r\n") + 4
    body = request[body_start:body_start + content_length]
    print("Joystick Data:", body)  # Process the joystick data here
    
    # Send response
    conn.send('HTTP/1.1 200 OK\n')
    conn.send('Content-Type: text/plain\n')
    conn.send('Connection: close\n\n')
    conn.send('Data received')
  
  elif "GET / " in request or "GET / HTTP/1.1" in request:
    # Serve the HTML page
    response = web_page.html
    conn.send('HTTP/1.1 200 OK\n')
    conn.send('Content-Type: text/html\n')
    conn.send('Connection: close\n\n')
    conn.sendall(response)
  
  conn.close()
s.close()
