<?php
include 'esp_server/config.php';

// Überprüfen, ob ein 'device_code' übergeben wurde
$device_code = isset($_GET['device_code']) ? $_GET['device_code'] : null;

// Debugging: Überprüfen, ob der device_code korrekt empfangen wurde
file_put_contents('debug.log', "Empfangener device_code: " . $device_code . "\n", FILE_APPEND);

// SQL-Abfrage anpassen, um nur Daten für den angegebenen 'device_code' abzurufen
if ($device_code) {
    $sql = "SELECT * FROM sensor_data WHERE device_code = ? ORDER BY timestamp DESC";
    $stmt = $conn->prepare($sql);
    if ($stmt) {
        $stmt->bind_param("s", $device_code);
        $stmt->execute();
        $result = $stmt->get_result();

        $data = array();

        while ($row = $result->fetch_assoc()) {
            $data[] = $row;
        }

        // Debugging: Überprüfen, ob Daten zurückgegeben wurden
        file_put_contents('debug.log', "Abfrageergebnisse: " . print_r($data, true) . "\n", FILE_APPEND);

        if (empty($data)) {
            $data = ["message" => "Keine Daten für den angegebenen device_code gefunden."];
        }
    } else {
        // Debugging: Fehler beim Vorbereiten der Abfrage
        file_put_contents('debug.log', "Fehler beim Vorbereiten der Abfrage: " . $conn->error . "\n", FILE_APPEND);
        $data = ["message" => "Fehler beim Abrufen der Daten."];
    }
} else {
    $data = ["message" => "Bitte geben Sie einen device_code ein."];
}

header('Content-Type: application/json');
echo json_encode($data);

$conn->close();
?>
