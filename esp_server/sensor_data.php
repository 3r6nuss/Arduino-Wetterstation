<?php
// Konfigurationsdatei einbinden
include 'config.php';

// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Prüfen, ob die Anfrage eine POST-Anfrage ist
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Arrays für Parameter und deren Werte
    $params = array('light_level', 'temperature', 'humidity', 'pressure', 'wind_speed', 'rain', 'esp_id', 'device_code'); // Added 'device_code'
    $values = array();
    $valid_data = false;
    
    // Prüfen, welche Parameter übermittelt wurden
    foreach ($params as $param) {
        if (isset($_POST[$param])) {
            $values[$param] = $_POST[$param];
            $valid_data = true;
        }
    }
    
    if ($valid_data) {
        // Vorbereiten des SQL-Statements
        $columns = implode(", ", array_keys($values));
        $placeholders = implode(", ", array_fill(0, count($values), "?"));
        
        $sql = "INSERT INTO sensor_data ($columns) VALUES ($placeholders)";
        
        // Prepared Statement erstellen
        $stmt = $conn->prepare($sql);
        
        if ($stmt) {
            // Typen für Parameter festlegen
            $types = str_repeat("d", count($values) - 2) . "ss"; // Adjusted for 'esp_id' and 'device_code' being strings

            // Parameter an das Statement binden
            $stmt->bind_param($types, ...array_values($values));
            
            // Statement ausführen
            if ($stmt->execute()) {
                echo "Daten erfolgreich gespeichert: ";
                foreach ($values as $key => $value) {
                    echo "$key: $value, ";
                }
            } else {
                echo "Fehler beim Speichern: " . $stmt->error;
            }
            
            $stmt->close();
        } else {
            echo "Fehler beim Vorbereiten der Abfrage: " . $conn->error;
        }
    } else {
        echo "Keine gültigen Parameter übermittelt!";
    }
} else {
    echo "Nur POST-Anfragen erlaubt!";
}

// Verbindung schließen
$conn->close();
?>
