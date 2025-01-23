import network
import socket
import wifi_credentials
import web_page

ssidAP         = wifi_credentials.ssidAP
passwordAP     = wifi_credentials.passwordAP

local_IP       = '192.168.1.10'
gateway        = '192.168.1.1'
subnet         = '255.255.255.0'
dns            = '8.8.8.8'

ap_if = network.WLAN(network.AP_IF)

def AP_Setup(ssidAP,passwordAP):
  ap_if.ifconfig([local_IP,gateway,subnet,dns])
  print("Setting soft-AP  ... ")
  ap_if.config(essid=ssidAP,authmode=network.AUTH_WPA_WPA2_PSK, password=passwordAP)
  ap_if.active(True)
  print('Success, IP address:', ap_if.ifconfig())
  print("Setup End\n")

try:
  AP_Setup(ssidAP,passwordAP)
except:
  print("Failed, please disconnect the power and restart the operation.")
  ap_if.disconnect()

# Initializing the socket server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(5)

while True:
  print('Waiting for a connection...')
  try:
    conn, addr = s.accept()
    print("Got connection from %s" % str(addr))
  except OSError as e:
    print("Socket accept error:", e)
  
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