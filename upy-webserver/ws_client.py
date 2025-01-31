import network
import socket
import time
import wifi_credentials

ssid = wifi_credentials.ssidAP
password = wifi_credentials.passwordAP
server_ip = '192.168.1.10'  # Server IP (AP address)
server_port = 80

wlan = network.WLAN(network.STA_IF)
wlan.active(True)

if not wlan.isconnected():
    print('Connecting to network...')
    wlan.disconnect()
    time.sleep(1)
    wlan.connect(ssid, password)
    while not wlan.isconnected():
        time.sleep(1)

print('Connected to WiFi:', wlan.ifconfig())

def send_joystick_data(x, y):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2) # Non-blocking
        s.connect((server_ip, server_port))
        
        data = "x={}&y={}".format(x, y)
        request = (
            "POST /joystick HTTP/1.1\r\n"
            "Host: {}:{}\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Content-Length: {}\r\n"
            "Connection: close\r\n\r\n"
            "{}".format(server_ip, server_port, len(data), data)
        )
        
        s.send(request.encode())
        response = s.recv(1024)
        print("Response:", response.decode())
        s.close()
    except Exception as e:
        print("Error:", e)

while True:
    send_joystick_data(50, 100)
    time.sleep(2)  # Send data every 2 seconds