<?php
// Konfigurasi server
$servername = "localhost";
$username = "root";
$password = "";

// Membuat koneksi
$conn = new mysqli($servername, $username, $password);

// Periksa koneksi
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Membuat database jika belum ada
$sql = "CREATE DATABASE IF NOT EXISTS projecttest";
if ($conn->query($sql) === TRUE) {
    echo "Database created successfully<br>";
} else {
    echo "Error creating database: " . $conn->error . "<br>";
}

$conn->close();

// Koneksi ulang ke database
$dbname = "projecttest";
$conn = new mysqli($servername, $username, $password, $dbname);

// Periksa koneksi
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Membuat tabel logs
$sql = "CREATE TABLE IF NOT EXISTS logs (
    no INT(7) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    `tanggal` DATE NULL,
    hari VARCHAR(30) NULL,
    `waktu` TIME NULL, 
    suhu VARCHAR(10) NULL,
    kelembapan VARCHAR(10) NULL,
    tegangan VARCHAR(10) NULL,
    arus VARCHAR(10) NULL,
    power VARCHAR(10) NULL
)";

if ($conn->query($sql) === TRUE) {
    echo "Table logs created successfully<br>";
} else {
    echo "Error creating table: " . $conn->error . "<br>";
}

$conn->close();
?>
