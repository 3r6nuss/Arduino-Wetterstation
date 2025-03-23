<?php
session_start();

if (!isset($_SESSION["user_id"])) {
    die("Bitte erst <a href='index.php'>einloggen</a>!");
}

echo "<h2>Willkommen, " . $_SESSION["username"] . "!</h2>";
echo "<p>Deine Rolle: " . $_SESSION["rolle"] . "</p>";
echo "<a href='logout.php'>Logout</a>";