<?php
session_start();
$db_link = mysqli_connect("localhost", "root", "", "login");

if (!$db_link) {
    die("Verbindung zur Datenbank fehlgeschlagen: " . mysqli_connect_error());
}

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $account = $_POST['account'];
    $passwort = $_POST['passwort'];

    $sql = "SELECT ID, Vorname, Rolle, Passwort FROM teilnehmer WHERE Account = '$account'";
    $result = mysqli_query($db_link, $sql);

    if ($row = mysqli_fetch_assoc($result)) {
        if ($passwort === $row["Passwort"]) {
            $_SESSION["user_id"] = $row["ID"];
            $_SESSION["username"] = $row["Vorname"];
            $_SESSION["rolle"] = $row["Rolle"];
            
            echo "Erfolgreich eingeloggt! <a href='dashboard.php'>Zum Dashboard</a>";
        } else {
            echo "Falsches Passwort!";
        }
    } else {
        echo "Kein Benutzer mit diesem Account gefunden!";
    }
}