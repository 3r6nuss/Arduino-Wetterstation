<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "wetterstation";

// Create connection
$conn = new mysqli($servername, $username, $password);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Create database if it doesn't exist
$sql = "CREATE DATABASE IF NOT EXISTS $dbname";
if ($conn->query($sql) === TRUE) {
    echo "Database created successfully or already exists.";
} else {
    echo "Error creating database: " . $conn->error;
}

// Select the database
$conn->select_db($dbname);

// Create table if it doesn't exist
$sql = "CREATE TABLE IF NOT EXISTS wetterdaten (
    ID INT AUTO_INCREMENT PRIMARY KEY,
    Temperatur_Aktuell FLOAT,
    Feuchtigkeit_Aktuell FLOAT,
    Temperatur_V2 FLOAT,
    Feuchtigkeit_V2 FLOAT,
    Temperatur_V3 FLOAT,
    Feuchtigkeit_V3 FLOAT
)";
if ($conn->query($sql) === TRUE) {
    echo "Table created successfully or already exists.";
} else {
    echo "Error creating table: " . $conn->error;
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    header('Content-Type: application/json');
    
    // Hole die letzten Werte aus der Datenbank
    $sql = "SELECT Temperatur_Aktuell, Feuchtigkeit_Aktuell 
            FROM wetterdaten 
            ORDER BY ID DESC 
            LIMIT 1";
            
    $result = $conn->query($sql);
    
    if ($result && $result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $temperature = $row['Temperatur_Aktuell'];
        $humidity = $row['Feuchtigkeit_Aktuell'];
        
        echo json_encode([
            "temperature" => $temperature,
            "humidity" => $humidity,
            "success" => true
        ]);
    } else {
        echo json_encode([
            "error" => "Keine Daten in der Datenbank gefunden",
            "success" => false
        ]);
    }
    exit;
}

function getTemperature() {
    global $conn;
    $sql = "SELECT Temperatur_Aktuell FROM wetterdaten ORDER BY ID DESC LIMIT 1";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        return $row['Temperatur_Aktuell'];
    } else {
        return "N/A";
    }
}

function getHumidity() {
    global $conn;
    $sql = "SELECT Feuchtigkeit_Aktuell FROM wetterdaten ORDER BY ID DESC LIMIT 1";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        return $row['Feuchtigkeit_Aktuell'];
    } else {
        return "N/A";
    }
}

// Fetch all data for the chart
function getAllData() {
    global $conn;
    $sql = "SELECT * FROM wetterdaten ORDER BY ID DESC LIMIT 10";
    $result = $conn->query($sql);
    $data = [];
    if ($result->num_rows > 0) {
        while($row = $result->fetch_assoc()) {
            $data[] = $row;
        }
    }
    return array_reverse($data);
}

$data = getAllData();
?>

<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wetterstation</title>
    <link rel="stylesheet" href="styles.css">
    <link rel="icon" href="favicon.ico" type="image/x-icon">
    <meta name="description" content="ESP Wetterstation Projekt zur Überwachung von Temperatur und Luftfeuchtigkeit.">
    <!-- Chart.js und Date Adapter -->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/chartjs-adapter-date-fns"></script>
    <script>
        const weatherData = <?php echo json_encode($data); ?>;
    </script>
</head>
<body>
    <header>
        <h1>ESP Wetterstation</h1>
    </header>
    <main>
        <section class="measurements" aria-labelledby="measurements-heading">
            <h2 id="measurements-heading" class="visually-hidden">Messwerte</h2>
            <p>Temperatur: <span id="temperature"><?php echo getTemperature(); ?></span> °C <span class="spinner" aria-hidden="true"></span></p>
            <p>Luftfeuchtigkeit: <span id="humidity"><?php echo getHumidity(); ?></span> % <span class="spinner" aria-hidden="true"></span></p>
        </section>
        <section class="chart-section" aria-labelledby="chart-heading">
            <h2 id="chart-heading" class="visually-hidden">Wetterdiagramm</h2>
            <canvas id="weatherChart" role="img" aria-label="Wetterdiagramm"></canvas>
        </section>
        <button id="add-data" onclick="addData()">Aktualisieren</button>
    </main>
    <footer>
        <p>&copy; 2025 Wetterstation Projekt</p>
        <button id="back-to-top" aria-label="Zurück nach oben">⬆️</button>
    </footer>
    <script src="script.js"></script>
</body>
</html>

<?php
// Close the connection at the end of the script
$conn->close();
?>