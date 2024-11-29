import qrcode

# Wi-Fi credentials and IP address
ssid = "ECHO-ALPHA"
password = "aLpHaEcHo1"
security_type = "WPA"  # Could also be WEP or nopass for open networks
ip_address = "http://192.168.4.1"  # Replace with your device's IP address

# Combined Wi-Fi and IP QR code data
wifi_and_ip_qr_data = f"WIFI:S:{ssid};T:{security_type};P:{password};;{ip_address}"

# Create the QR code
qr = qrcode.QRCode(
    version=1, error_correction=qrcode.constants.ERROR_CORRECT_L, box_size=10, border=4
)
qr.add_data(wifi_and_ip_qr_data)
qr.make(fit=True)

# Generate the QR code image
qr_img = qr.make_image(fill_color="black", back_color="white")

# Save the QR code image
file_path = "/home/lawrence/Documents/Projects/qr_codes/ECHO-ALPHA-CONNECT.png"
qr_img.save(file_path)
