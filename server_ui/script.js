const forwardButton = document.getElementById("forward");
const backwardButton = document.getElementById("backward");
const leftButton = document.getElementById("left");
const rightButton = document.getElementById("right");

forwardButton.addEventListener("mousedown", () => console.log("Forward button pressed"));
forwardButton.addEventListener("mouseup", () => console.log("Forward button released"));

backwardButton.addEventListener("mousedown", () => console.log("Backward button pressed"));
backwardButton.addEventListener("mouseup", () => console.log("Backward button released"));

leftButton.addEventListener("mousedown", () => console.log("Left button pressed"));
leftButton.addEventListener("mouseup", () => console.log("Left button released"));

rightButton.addEventListener("mousedown", () => console.log("Right button pressed"));
rightButton.addEventListener("mouseup", () => console.log("Right button released"));


