<?php
// Datenbank-Verbindungsdaten
$servername = "db";  // Dein Server, in der Regel 'localhost'
$username = "root";         // Dein MySQL-Benutzername (bei XAMPP normalerweise 'root')
$password = "password";             // Dein MySQL-Passwort (bei XAMPP normalerweise leer)
$dbname = "esp8266_db"; // Der Name der Datenbank, die du verwendest

// Erstelle eine Verbindung zur Datenbank
$conn = new mysqli($servername, $username, $password, $dbname);

// Überprüfe die Verbindung
if ($conn->connect_error) {
    die("Verbindung fehlgeschlagen: " . $conn->connect_error);
}
?>
