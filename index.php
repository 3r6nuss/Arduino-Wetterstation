<?php
// Daten abrufen, wenn ein Geräte-Code übermittelt wurde
$data = [];
if (isset($_GET['device_code']) && !empty($_GET['device_code'])) {
    $device_code = $_GET['device_code'];
    
    // Vollständigen Server-Pfad zur API verwenden
    $base_url = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://$_SERVER[HTTP_HOST]";
    $current_dir = dirname($_SERVER['PHP_SELF']);
    $api_url = $base_url . $current_dir . "/get_data.php?device_code=" . urlencode($device_code);
    
    try {
        // Für Debug-Zwecke
        error_log("Versuche Daten abzurufen von: " . $api_url);
        
        $response = file_get_contents($api_url);
        if ($response === false) {
            throw new Exception("Fehler beim Abrufen der Daten");
        }
        
        $data = json_decode($response, true);
        if (json_last_error() !== JSON_ERROR_NONE) {
            throw new Exception("Fehler beim Dekodieren der JSON-Daten");
        }
    } catch (Exception $e) {
        if (!isset($_GET['error'])) { // Verhindert Endlosschleifen
            $redirect_url = "index.php?status_message=" . urlencode($e->getMessage()) . "&status_type=error&error=1";
            if (isset($_GET['device_code'])) {
                $redirect_url .= "&device_code=" . urlencode($_GET['device_code']);
            }
            header("Location: $redirect_url");
            exit;
        } else {
            // Fehlernachricht direkt anzeigen, wenn bereits ein Fehler vorliegt
            $data = ['message' => $e->getMessage()];
        }
    }
}
?>
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wetterstation Dashboard</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link rel="stylesheet" href="styles.css">
    <meta http-equiv="refresh" content="30"> <!-- Auto-Refresh alle 30 Sekunden -->
</head>
<body>
    <h1>Wetterstation Dashboard</h1>

    <div class="container">
        <h2>Geräteauswahl</h2>
        <form action="index.php" method="GET">
            <div class="input-group">
                <label for="device-code">Geräte-Code:</label>
                <input type="text" id="device-code" name="device_code" placeholder="Geräte-Code eingeben" value="<?php echo isset($_GET['device_code']) ? htmlspecialchars($_GET['device_code']) : ''; ?>">
                <input type="submit" value="Anzeigen">
            </div>
        </form>
        
        <?php if (isset($_GET['status_message'])): ?>
            <div class="status-message <?php echo $_GET['status_type'] === 'success' ? 'success-message' : 'error-message'; ?>">
                <?php echo htmlspecialchars($_GET['status_message']); ?>
            </div>
        <?php endif; ?>
    </div>

    <div class="container">
        <h2>Letzte Daten</h2>
        <div id="latest-data">
            <div class="data-card">
                <p>Helligkeit</p>
                <p class="data-value" id="light-level">
                    <?php 
                    if (isset($data) && !empty($data) && !isset($data['message'])) {
                        echo htmlspecialchars($data[0]['light_level']);
                    } else {
                        echo '<span class="loading">Keine Daten</span>';
                    }
                    ?>
                </p>
            </div>
            <div class="data-card">
                <p>Temperatur</p>
                <p class="data-value" id="temperature">
                    <?php 
                    if (isset($data) && !empty($data) && !isset($data['message'])) {
                        echo htmlspecialchars($data[0]['temperature']) . ' °C';
                    } else {
                        echo '<span class="loading">Keine Daten</span>';
                    }
                    ?>
                </p>
            </div>
            <div class="data-card">
                <p>Luftfeuchtigkeit</p>
                <p class="data-value" id="humidity">
                    <?php 
                    if (isset($data) && !empty($data) && !isset($data['message'])) {
                        echo htmlspecialchars($data[0]['humidity']) . ' %';
                    } else {
                        echo '<span class="loading">Keine Daten</span>';
                    }
                    ?>
                </p>
            </div>
        </div>
    </div>

    <div class="container">
        <h2>Diagramm der letzten 10 Daten</h2>
        <div class="chart-container">
            <canvas id="dataChart"></canvas>
        </div>
    </div>

    <?php if (isset($_GET['device_code']) && !empty($_GET['device_code'])): ?>
    <script>
        // Einfaches Script nur für das Diagramm - Rest wird über PHP gehandhabt
        document.addEventListener('DOMContentLoaded', function() {
            // Daten aus PHP-Variable für Chart.js
            <?php
            if (isset($data) && !empty($data) && !isset($data['message'])) {
                $chartData = array_slice($data, 0, 10);
                $chartData = array_reverse($chartData);
                
                $labels = [];
                $lightLevels = [];
                $temperatures = [];
                $humidities = [];
                
                foreach ($chartData as $entry) {
                    $timestamp = new DateTime($entry['timestamp']);
                    $labels[] = $timestamp->format('H:i:s');
                    $lightLevels[] = $entry['light_level'];
                    $temperatures[] = $entry['temperature'];
                    $humidities[] = $entry['humidity'];
                }
                
                echo "const labels = " . json_encode($labels) . ";\n";
                echo "const lightLevels = " . json_encode($lightLevels) . ";\n";
                echo "const temperatures = " . json_encode($temperatures) . ";\n";
                echo "const humidities = " . json_encode($humidities) . ";\n";
            } else {
                echo "const labels = [];\n";
                echo "const lightLevels = [];\n";
                echo "const temperatures = [];\n";
                echo "const humidities = [];\n";
            }
            ?>
            
            // Chart-Erstellung
            const ctx = document.getElementById('dataChart').getContext('2d');
            const chart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [
                        {
                            label: 'Helligkeit',
                            data: lightLevels,
                            borderColor: 'rgb(255, 205, 86)',
                            backgroundColor: 'rgba(255, 205, 86, 0.1)',
                            borderWidth: 2,
                            tension: 0.1,
                            pointRadius: 3,
                            pointHoverRadius: 5
                        },
                        {
                            label: 'Temperatur (°C)',
                            data: temperatures,
                            borderColor: 'rgb(255, 99, 132)',
                            backgroundColor: 'rgba(255, 99, 132, 0.1)',
                            borderWidth: 2,
                            tension: 0.1,
                            pointRadius: 3,
                            pointHoverRadius: 5
                        },
                        {
                            label: 'Luftfeuchtigkeit (%)',
                            data: humidities,
                            borderColor: 'rgb(54, 162, 235)',
                            backgroundColor: 'rgba(54, 162, 235, 0.1)',
                            borderWidth: 2,
                            tension: 0.1,
                            pointRadius: 3,
                            pointHoverRadius: 5
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        x: {
                            title: {
                                display: true,
                                text: 'Uhrzeit der Datenenthebung',
                                font: {
                                    weight: 'bold'
                                }
                            },
                            grid: {
                                display: false
                            }
                        },
                        y: {
                            title: {
                                display: true,
                                text: 'Werte',
                                font: {
                                    weight: 'bold'
                                }
                            },
                            beginAtZero: true
                        }
                    },
                    plugins: {
                        tooltip: {
                            mode: 'index',
                            intersect: false
                        },
                        legend: {
                            position: 'top',
                            labels: {
                                boxWidth: 12,
                                usePointStyle: true,
                                pointStyle: 'circle'
                            }
                        }
                    },
                    interaction: {
                        mode: 'nearest',
                        axis: 'x',
                        intersect: false
                    }
                }
            });
        });
    </script>
    <?php endif; ?>
</body>
</html>

