<?php
	$data = $_POST['data'];
	$remote_ip = "192.168.0.80";
	$remote_port = 7700;
	
	//Reduce errors
	error_reporting(~E_WARNING);
	 
	//Create a UDP socket
	if(!($sock = socket_create(AF_INET, SOCK_DGRAM, 0)))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);
		die("Couldn't create socket: [$errorcode] $errormsg \n");
	}
	echo "Socket created \n";
	 
	// Bind the source address
	if( !socket_bind($sock, "0.0.0.0" , 7700) )
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);
		die("Could not bind socket : [$errorcode] $errormsg \n");
	}
	echo "Socket bind OK \n";
	 
	//Do some communication, this loop can handle multiple clients
	socket_sendto($sock, $data, strlen($data), 0 , $remote_ip , $remote_port);
	socket_close($sock);

    echo $stringData;
?>