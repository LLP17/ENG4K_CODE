import network
import socket
import time
import wifi_credentials

ssid = wifi_credentials.ssid
password = wifi_credentials.password

# server_ip = '192.168.4.1'

server_wlan = network.WLAN(network.AP_IF)

def AP_Setup(ssid,password):
  print("Server: Setting soft-AP  ... ")
  server_wlan.active(True)
  server_wlan.config(essid=ssid, password=password)
  print('Server: Success, IP address:', server_wlan.ifconfig())
  print("Server: Setup End\n")

try:
  AP_Setup(ssid,password)
except:
  print("Failed, please disconnect the power and restart the operation.")
  server_wlan.disconnect()

# Server socket initialization
local_IP = server_wlan.ifconfig()[0]
server_socket = socket.socket()
socket_addr = (local_IP, 80)
server_socket.bind(socket_addr)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.listen(5)
print('Open socket server listening on adr {}.'.format(socket_addr))

# Adding external HTML file
try:
  with open("/static/index.html", "r") as f:
    html_string = f.read()
except Exception as e:
  print("ERROR: " + e)

while 1:
  conn, addr = server_socket.accept() 
  request = conn.recv(1024).decode('utf-8')
  
  # Parse the HTTP request
  if "POST /joystick" in request:
    # Handle joystick data
    content_length = int(request.split("Content-Length: ")[1].split("\r\n")[0])
    body_start = request.find("\r\n\r\n") + 4
    
    # Display joystick telemetry
    joystick_data = request[body_start:body_start + content_length]
    print(joystick_data)

    # Send response
    conn.send('HTTP/1.1 200 OK\n')
    conn.send('Content-Type: text/plain\n')
    conn.send('Connection: close\n\n')
    conn.send('Data received')
  
  elif "GET / " in request or "GET / HTTP/1.1" in request:
    # Serve the HTML page
    response = html_string
    conn.send('HTTP/1.1 200 OK\n')
    conn.send('Content-Type: text/html\n')
    conn.send('Connection: close\n\n')
    conn.sendall(response)

  conn.close()

server_socket.close()