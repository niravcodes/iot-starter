function appendNumber(num) {
  const display = document.getElementById("display");
  display.value += num;
}

function clearDisplay() {
  document.getElementById("display").value = "";
}

function sendNumber() {
  const number = document.getElementById("display").value;

  fetch("/submit", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ number: number }),
  })
    .then((response) => response.json())
    .then((data) => {
      alert("Number sent successfully!");
      clearDisplay();
    })
    .catch((error) => {
      console.error("Error:", error);
      alert("Error sending number");
    });
}
