<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wetterstation Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <h1>Wetterstation Dashboard</h1>

    <h2>Geräteauswahl</h2>
    <label for="device-code">Geräte-Code:</label>
    <input type="text" id="device-code" placeholder="Geräte-Code eingeben">
    <button onclick="fetchData()">Anzeigen</button>

    <h2>Letzte Daten</h2>
    <div id="latest-data"></div>
        <p>Helligkeit: <span id="light-level">Laden...</span></p>
        <p>Temperatur: <span id="temperature">Laden...</span></p>
        <p>Luftfeuchtigkeit: <span id="humidity">Laden...</span></p>
    </div>

    <h2>Diagramm der letzten 10 Daten</h2>
    <canvas id="dataChart" width="400" height="200"></canvas>

    <script>
        async function fetchData() {
            try {
                const deviceCode = document.getElementById('device-code').value;
                const response = await fetch(`get_data.php?device_code=${deviceCode}`);
                const data = await response.json();

                console.log('Empfangene Daten:', data); // Debugging: Logge die empfangenen Daten

                if (data.message) {
                    // Zeige die Nachricht an, wenn keine Daten vorhanden sind
                    document.getElementById('latest-data').innerHTML = `<p>${data.message}</p>`;
                    document.getElementById('dataChart').style.display = 'none';
                } else {
                    // Update latest data
                    const latest = data[0];
                    document.getElementById('light-level').textContent = latest.light_level;
                    document.getElementById('temperature').textContent = latest.temperature;
                    document.getElementById('humidity').textContent = latest.humidity;

                    // Prepare data for the chart
                    const labels = data.slice(0, 10).map(entry => new Date(entry.timestamp).toLocaleTimeString());
                    const lightLevels = data.slice(0, 10).map(entry => entry.light_level);
                    const temperatures = data.slice(0, 10).map(entry => entry.temperature);
                    const humidities = data.slice(0, 10).map(entry => entry.humidity);

                    // Debugging: Logge die vorbereiteten Daten für das Diagramm
                    console.log('Diagramm Labels (Uhrzeit):', labels);
                    console.log('Helligkeit:', lightLevels);
                    console.log('Temperatur:', temperatures);
                    console.log('Luftfeuchtigkeit:', humidities);

                    // Render chart
                    const ctx = document.getElementById('dataChart').getContext('2d');
                    new Chart(ctx, {
                        type: 'line',
                        data: {
                            labels: labels,
                            datasets: [
                                {
                                    label: 'Helligkeit',
                                    data: lightLevels,
                                    borderColor: 'yellow',
                                    fill: false
                                },
                                {
                                    label: 'Temperatur (°C)',
                                    data: temperatures,
                                    borderColor: 'red',
                                    fill: false
                                },
                                {
                                    label: 'Luftfeuchtigkeit (%)',
                                    data: humidities,
                                    borderColor: 'blue',
                                    fill: false
                                }
                            ]
                        },
                        options: {
                            responsive: true,
                            scales: {
                                x: {
                                    title: {
                                        display: true,
                                        text: 'Uhrzeit der Datenenthebung'
                                    }
                                },
                                y: {
                                    title: {
                                        display: true,
                                        text: 'Werte'
                                    }
                                }
                            }
                        }
                    });

                    // Stelle sicher, dass das Diagramm sichtbar ist
                    document.getElementById('dataChart').style.display = 'block';
                }
            } catch (error) {
                console.error('Fehler beim Abrufen der Daten:', error);
            }
        }

        // Fetch data on page load
        fetchData();
    </script>
</body>
</html>

