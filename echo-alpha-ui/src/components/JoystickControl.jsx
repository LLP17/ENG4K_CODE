import React, { useEffect, useRef } from "react";
import { Joystick } from "react-joystick-component";

const JoystickControl = () => {
  const socketRef = useRef(null);
  const gateway = "ws://192.168.1.100/ws";

  useEffect(() => {
    // Connect to WebSocket server
    socketRef.current = new WebSocket(gateway); // ESP32 IP and WebSocket port

    socketRef.current.onopen = () => console.log("WebSocket Connected");
    socketRef.current.onclose = () => console.log("WebSocket Disconnected");

    return () => socketRef.current.close(); // Cleanup on unmount
  }, []);

  // TESTING: display of control data

//   const display = document.querySelector("#display");
//   const telemetry = document.createElement("p");
//   display.appendChild(telemetry);

  const handleMove = (event) => {
    const { x, y } = event;
    // telemetry.textContent = `x: ${x}, y: ${y}`;

    if (socketRef.current && socketRef.current.readyState === WebSocket.OPEN) {
      // Send joystick data to ESP32
      socketRef.current.send(`joystick:${x},${y}`);
    }
  };

  const handleStop = () => {
    if (socketRef.current && socketRef.current.readyState === WebSocket.OPEN) {
      socketRef.current.send("joystick:0,0"); // Stop signal
    }
  };

  return (
    <div>
      <Joystick
        size={100}
        baseColor="#bddcff"
        stickColor="#51a2ff"
        move={handleMove}
        stop={handleStop}
      />
    </div>
  );
};

export default JoystickControl;
