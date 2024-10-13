<?php

// echo "<pre>";
// print_r($_ENV);
// echo "</pre>";
// print_r(apache_request_headers());
printf("GET var1: %s\n", isset($_GET['var1']) ? $_GET['var1'] : "not set");
printf("GET var2: %s\n", isset($_GET['var2']) ? $_GET['var2'] : "not set");

printf("POST var1: %s\n", isset($_POST['var1']) ? $_POST['var1'] : "not set");
printf("POST var2: %s\n", isset($_POST['var2']) ? $_POST['var2'] : "not set");

?>