// Global chart variable
let weatherChart;

document.addEventListener('DOMContentLoaded', function() {
    // Chart initialisieren
    const ctx = document.getElementById('weatherChart').getContext('2d');
    weatherChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [], 
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
            animation: {
                duration: 750
            },
            scales: {
                x: {
                    type: 'category',
                    title: {
                        display: true,
                        text: 'Messung'
                    }
                },
                'y-temp': {
                    type: 'linear',
                    position: 'left',
                    title: {
                        display: true,
                        text: 'Temperatur (°C)'
                    },
                    min: 0,
                    max: 40
                },
                'y-hum': {
                    type: 'linear',
                    position: 'right',
                    title: {
                        display: true,
                        text: 'Luftfeuchtigkeit (%)'
                    },
                    min: 0,
                    max: 100,
                    grid: {
                        drawOnChartArea: false,
                    }
                }
            }
        }
    });

    // Initialize chart with existing data if available
    if (typeof weatherData !== 'undefined' && weatherData.length > 0) {
        console.log('Initial weather data:', weatherData);
        weatherData.forEach((data, index) => {
            weatherChart.data.labels.push(`Messung ${index + 1}`);
            weatherChart.data.datasets[0].data.push(parseFloat(data.Temperatur_Aktuell));
            weatherChart.data.datasets[1].data.push(parseFloat(data.Feuchtigkeit_Aktuell));
        });
        weatherChart.update('none'); // Update without animation for initial data
    }
});

function addData() {
    // Disable button während der Aktualisierung
    const button = document.getElementById('add-data');
    button.disabled = true;

    fetch(window.location.href, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded',
        }
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            // Warte 500ms bevor die Daten aktualisiert werden
            setTimeout(() => {
                console.log('Received data:', data);
                
                // Update der HTML-Elemente
                document.getElementById('temperature').textContent = data.temperature;
                document.getElementById('humidity').textContent = data.humidity;

                // Add new measurement label
                const newIndex = weatherChart.data.labels.length + 1;
                weatherChart.data.labels.push(`Messung ${newIndex}`);
                
                // Add new data points
                weatherChart.data.datasets[0].data.push(parseFloat(data.temperature));
                weatherChart.data.datasets[1].data.push(parseFloat(data.humidity));

                // Keep only last 10 measurements
                if (weatherChart.data.labels.length > 10) {
                    weatherChart.data.labels.shift();
                    weatherChart.data.datasets[0].data.shift();
                    weatherChart.data.datasets[1].data.shift();
                }

                weatherChart.update();
                // Button wieder aktivieren
                button.disabled = false;
            }, 500);
        } else {
            console.error('Error:', data.error);
            button.disabled = false;
        }
    })
    .catch(error => {
        console.error('Error:', error);
        button.disabled = false;
    });
}
