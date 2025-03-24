<?php
include 'esp_server/config.php';

$sql = "SELECT * FROM sensor_data ORDER BY timestamp DESC";
$result = $conn->query($sql);

$data = array();

while ($row = $result->fetch_assoc()) {
    $data[] = $row;
}

header('Content-Type: application/json');
echo json_encode($data);

$conn->close();
?>
