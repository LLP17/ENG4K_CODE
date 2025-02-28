import React, { useState, useEffect, useRef } from "react";

const Joystick = () => {
  const canvasRef = useRef(null);
  const [joystick, setJoystick] = useState({ x: 0, y: 0 });
  const centerX = 150,
    centerY = 150,
    maxRadius = 100;

  useEffect(() => {
    drawJoystick();
  }, [joystick]);

  const updateJoystick = (event) => {
    event.preventDefault();
    const canvas = canvasRef.current;
    const rect = canvas.getBoundingClientRect();
    const x = event.touches ? event.touches[0].clientX : event.clientX;
    const y = event.touches ? event.touches[0].clientY : event.clientY;

    const offsetX = x - rect.left - centerX;
    const offsetY = y - rect.top - centerY;
    const distance = Math.min(Math.sqrt(offsetX ** 2 + offsetY ** 2), maxRadius);
    const angle = Math.atan2(offsetY, offsetX);

    setJoystick({
      x: Math.round(((distance * Math.cos(angle)) / maxRadius) * 100),
      y: Math.round(((distance * Math.sin(angle)) / maxRadius) * 100),
    });

    sendJoystickData({ x: joystick.x, y: joystick.y });
  };

  const resetJoystick = () => {
    setJoystick({ x: 0, y: 0 });
    sendJoystickData({ x: 0, y: 0 });
  };

  const sendJoystickData = async (joystick) => {
    try {
      await fetch("http://192.168.4.1/joystick", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(joystick),
      });
    } catch (error) {
      console.error("Error sending joystick data:", error);
    }
  };

  const drawJoystick = () => {
    const canvas = canvasRef.current;
    const ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Draw joystick base
    ctx.beginPath();
    ctx.arc(centerX, centerY, maxRadius, 0, Math.PI * 2);
    ctx.fillStyle = "#888";
    ctx.fill();

    // Draw joystick handle
    ctx.beginPath();
    ctx.arc(centerX + joystick.x, centerY + joystick.y, 50, 0, Math.PI * 2);
    ctx.fillStyle = "#444";
    ctx.fill();
  };

  return (
    <div>
      <h1>ECHO-ALPHA</h1>
      <canvas
        ref={canvasRef}
        id="joystick"
        width="300"
        height="300"
        onMouseDown={updateJoystick}
        onMouseMove={(e) => e.buttons && updateJoystick(e)}
        onMouseUp={resetJoystick}
        onMouseLeave={resetJoystick}
        onTouchStart={updateJoystick}
        onTouchMove={updateJoystick}
        onTouchEnd={resetJoystick}
      ></canvas>
    </div>
  );
};

export default Joystick;