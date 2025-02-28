import React, { useState, useRef, useEffect } from "react";

const Joystick = () => {
  const joystickRef = useRef(null);
  const [position, setPosition] = useState({ x: 0, y: 0 });

  const maxRadius = 50; // Max distance joystick can move

  const updateJoystick = (event) => {
    event.preventDefault();
    
    const rect = joystickRef.current.getBoundingClientRect();
    const centerX = rect.width / 2;
    const centerY = rect.height / 2;

    const x = event.touches ? event.touches[0].clientX - rect.left : event.clientX - rect.left;
    const y = event.touches ? event.touches[0].clientY - rect.top : event.clientY - rect.top;

    let dx = x - centerX;
    let dy = y - centerY;

    const distance = Math.sqrt(dx ** 2 + dy ** 2);
    if (distance > maxRadius) {
      const angle = Math.atan2(dy, dx);
      dx = maxRadius * Math.cos(angle);
      dy = maxRadius * Math.sin(angle);
    }

    setPosition({ x: dx, y: dy });
  };

  const resetJoystick = () => {
    setPosition({ x: 0, y: 0 });
  };

  return (
    <div
      ref={joystickRef}
      className="w-32 h-32 bg-blue-200 rounded-full flex justify-center items-center shadow-lg relative"
      onMouseDown={updateJoystick}
      onMouseMove={(e) => e.buttons && updateJoystick(e)}
      onMouseUp={resetJoystick}
      onMouseLeave={resetJoystick}
      onTouchStart={updateJoystick}
      onTouchMove={updateJoystick}
      onTouchEnd={resetJoystick}
    >
      <div
        className="w-16 h-16 bg-blue-400 rounded-full absolute"
        style={{ transform: `translate(${position.x}px, ${position.y}px)` }}
      ></div>
    </div>
  );
};

export default Joystick;
