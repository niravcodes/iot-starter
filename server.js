const express = require("express");
const mqtt = require("mqtt");
const app = express();
const port = 3000;

const mqttClient = mqtt.connect("mqtt://localhost:1883");

mqttClient.on("connect", () => {
  console.log("Connected to MQTT broker");
});

mqttClient.on("error", (error) => {
  console.error("MQTT Error:", error);
});

app.use(express.static("./"));
app.use(express.json());

app.post("/submit", (req, res) => {
  const number = req.body.number;
  console.log("Received number:", number);

  mqttClient.publish("team1/input", number, (err) => {
    if (err) {
      console.error("MQTT Publish Error:", err);
      res.json({ success: false, error: "Failed to publish to MQTT" });
    } else {
      console.log("Published to MQTT:", number);
      res.json({ success: true });
    }
  });
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
