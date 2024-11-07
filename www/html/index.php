<?php
// if ($_SERVER['REQUEST_METHOD'] === 'POST') {
//     // Retrieve name and nickname
//     $name = isset($_POST['name']) ? $_POST['name'] : 'Unknown';
//     $nickname = isset($_POST['nickname']) ? $_POST['nickname'] : 'Unknown';
//     echo $name . "--" . $nickname;
//     // Check if the PDF file is uploaded
//     if (isset($_FILES['pdfFile']) && $_FILES['pdfFile']['error'] === UPLOAD_ERR_OK) {
//         $fileTmpPath = $_FILES['pdfFile']['tmp_name'];
//         $fileName = $_FILES['pdfFile']['name'];
//         $fileSize = $_FILES['pdfFile']['size'];
//         $fileType = $_FILES['pdfFile']['type'];

//         // Check if file type is PDF
//         if ($fileType === 'application/pdf') {
//             // Set a destination path for the upload
//             $uploadDir = '/Users/ijaija/merge/www/uploads/';
//             $destPath = $uploadDir . basename($fileName);

//             // Move the file to the upload directory
//             if (move_uploaded_file($fileTmpPath, $destPath)) {
//                 echo "The file " . htmlspecialchars($fileName) . " has been uploaded successfully.<br>";
//                 echo "Name: " . htmlspecialchars($name) . "<br>";
//                 echo "Nickname: " . htmlspecialchars($nickname) . "<br>";
//             } else {
//                 echo "Error: Could not save the uploaded file.";
//             } 
//         } else {
//             echo "Error: Only PDF files are allowed.";
//         }
//     } else {
//         echo "Error: No file uploaded or there was an upload error.";
//     }
// } else {
//     echo "Error: Invalid request method.";
// }

session_start();

if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['msg'])) {
    $_SESSION['msg'] = $_POST['msg'];
}

if (!isset($_SESSION['msg'])) {
    $_SESSION['msg'] = rand(1, 100);
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Session Message</title>
</head>
<body>
    <form method="post" action="index.php">
        <label for="msg">Enter new message:</label>
        <input type="text" id="msg" name="msg" value="<?php echo htmlspecialchars($_SESSION['msg']); ?>">
        <button type="submit">Update Message</button>
    </form>

    <p>Current message: <?php echo htmlspecialchars($_SESSION['msg']); ?></p>
</body>
</html>