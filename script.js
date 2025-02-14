document.addEventListener('DOMContentLoaded', function() {
    // Chart initialisieren
    const ctx = document.getElementById('weatherChart').getContext('2d');
    const weatherChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [], // Zeitstempel
            datasets: [{
                label: 'Temperatur (°C)',
                data: [],
                borderColor: 'rgba(255, 99, 132, 1)',
                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                fill: false,
                tension: 0.1,
                yAxisID: 'y-temp'
            }, {
                label: 'Luftfeuchtigkeit (%)',
                data: [],
                borderColor: 'rgba(54, 162, 235, 1)',
                backgroundColor: 'rgba(54, 162, 235, 0.2)',
                fill: false,
                tension: 0.1,
                yAxisID: 'y-hum'
            }]
        },
        options: {
            responsive: true,
            scales: {
                x: {
                    type: 'time',
                    time: {
                        unit: 'minute',
                        tooltipFormat: 'HH:mm:ss'
                    },
                    title: {
                        display: true,
                        text: 'Zeit'
                    }
                },
                'y-temp': {
                    type: 'linear',
                    position: 'left',
                    title: {
                        display: true,
                        text: 'Temperatur (°C)'
                    }
                },
                'y-hum': {
                    type: 'linear',
                    position: 'right',
                    title: {
                        display: true,
                        text: 'Luftfeuchtigkeit (%)'
                    },
                    grid: {
                        drawOnChartArea: false,
                    }
                }
            }
        }
    });

    // Funktion zur Simulation des Abrufs von Sensordaten
    function updateMeasurements() {
        const now = new Date();
        // Simulierte Messwerte (ersetze dies durch den Abruf von echten Daten vom ESP)
        const temp = (20 + Math.random() * 10).toFixed(1);
        const hum = (40 + Math.random() * 20).toFixed(1);

        // Update der HTML-Elemente
        document.getElementById('temperature').textContent = temp;
        document.getElementById('humidity').textContent = hum;

        // Neue Daten zum Diagramm hinzufügen
        weatherChart.data.labels.push(now);
        weatherChart.data.datasets[0].data.push(temp);
        weatherChart.data.datasets[1].data.push(hum);

        // Beschränke die Anzeige auf die letzten 20 Messwerte
        if (weatherChart.data.labels.length > 20) {
            weatherChart.data.labels.shift();
            weatherChart.data.datasets[0].data.shift();
            weatherChart.data.datasets[1].data.shift();
        }
        weatherChart.update();
    }

    // Aktualisiere Messwerte alle 5 Sekunden
    setInterval(updateMeasurements, 5000);
    updateMeasurements(); // Initialer Aufruf
});
