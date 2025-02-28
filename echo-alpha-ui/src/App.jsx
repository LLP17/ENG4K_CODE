import React from "react";
import {
  FaCog,
  FaBars,
  FaWifi,
  FaMicrophoneSlash,
  FaPowerOff,
  FaCamera,
} from "react-icons/fa";
import Joystick from "./components/Joystick.jsx";

const App = () => {
  return (
    <div className="w-full h-screen bg-white flex flex-col items-center">
      {/* Top Navbar */}
      <div className="w-full flex items-center justify-between px-4 py-2 bg-white shadow-md">
        <FaCog className="text-xl text-gray-700" />
        <h1 className="text-xl font-bold">ECHO-ALPHA</h1>
        <FaBars className="text-xl text-gray-700" />
      </div>

      {/* Robot Video Feed */}
      <div className="w-full h-1/3 bg-gray-300 flex justify-center items-center">
        <video className="w-full h-full object-cover" autoPlay></video>
      </div>

      {/* Connection Status */}
      <div className="w-full flex items-center justify-between px-4 py-2 bg-gray-100 shadow-md">
        <div className="flex items-center space-x-2">
          <div className="w-4 h-4 bg-red-500 rounded-full"></div>
          <FaWifi className="text-gray-600" />
          <span className="text-sm font-medium">CONNECTED</span>
        </div>
        <span className="text-sm text-gray-700">192.168.4.1</span>
      </div>

      {/* Controller */}
      <div className="w-full flex flex-col items-center py-4">
        <Joystick />

        {/* Sliders */}
        <div className="w-full flex justify-around mt-4 px-8">
          <div className="flex flex-col items-center">
            <span className="text-gray-600 mb-2">🔊</span>
            <input
              type="range"
              min="0"
              max="100"
              className="slider w-12 rotate-[-90deg]"
            />
          </div>
          <div className="flex flex-col items-center">
            <span className="text-gray-600 mb-2">↕️</span>
            <input
              type="range"
              min="0"
              max="100"
              className="slider w-12 rotate-[-90deg]"
            />
          </div>
        </div>
      </div>

      {/* Bottom Buttons */}
      <div className="w-full flex justify-around py-4 border-t bg-white">
        <button className="p-3 bg-red-100 rounded-full text-red-600">
          <FaMicrophoneSlash size={24} />
        </button>
        <button className="p-3 bg-gray-100 rounded-full text-gray-700">
          <FaPowerOff size={24} />
        </button>
        <button className="p-3 bg-gray-100 rounded-full text-gray-700">
          <FaCamera size={24} />
        </button>
      </div>
    </div>
  );
};

export default App;
