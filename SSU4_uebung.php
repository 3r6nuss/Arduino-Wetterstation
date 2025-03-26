<?php
$db_link=mysqli_connect("localhost","root","","login" );
$account = "Phil";
$password  = "Phil";
insertSatz($db_link,$password,$account);
copyaccount($db_link,$account,$password,4);


if($db_link)
{
    $ergebnis = Suchdaten($db_link);
    while($row = mysqli_fetch_object($ergebnis))
    {
        deleteSatz($db_link,$row->ID,$row->Rolle);
        echo "<br>";
    }
    
}

function copyaccount($db_link,$account,$password,$number)
{
    for($i = 1;$i<=$number;$i++)
    {
        insertSatz($db_link,$password.'_x'.$i,$account.'_x'.$i);
    }
}


function deleteSatz($db_link, $id, $rolle)
{
    $sql_query = "DELETE FROM teilnehmer WHERE ID = $id AND Rolle !='admin'";
    if (mysqli_query($db_link, $sql_query)) {
        echo "Datensatz erfolgreich Bearbeitet";
    } else {
        echo "Fehler beim Bearbeiten des Datensatzes: " . mysqli_error($db_link);
    }
}

function getConnection(): mysqli
{
    $mysql_connection = mysqli_connect("localhost", "root", "", "login");
    if (mysqli_connect_error()) {
        die("Connection to database failed.");
    }
    return $mysql_connection;
}
function Suchdaten($db_link)
{
    $sql_query = "SELECT ID,Rolle FROM teilnehmer WHERE passwort LIKE '%neu%'";
    $ergebnis = mysqli_query($db_link, $sql_query);
    return $ergebnis;
    
}

function insertSatz($db_link,$password,$account)
{
    
    $sql_query = "INSERT INTO `teilnehmer` ( `Account`, `Passwort`, `Email`, `Rolle`) 
                  VALUES ('$account', '$password','testmail@test.test', 'tn');";

	if (mysqli_query($db_link, $sql_query)) 
	{
		echo "Datensatz erfolgreich eingefügt.";
	} else {
		echo "Fehler beim Einfügen des Datensatzes: " . mysqli_error($db_link);
	}
}
?>