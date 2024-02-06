String homePagePart1 = F(R"=====(
<!DOCTYPE html>
<html lang="en" > <!-- Needed for Screenreaders !-->
<head>
  <meta http-equiv="refresh" content="30">
<!-- UTF-8 character set covers most characters in the world -->
  <meta charset="utf-8">
  <!-- Make page respond to screen size !-->
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
 
<!--Include a Title. Used by Search Engines -->
<title> Temperature Reading WebServer </title>
<style>
   
    body {       
	    background-color: rgb(56, 141, 226);
	}

    h1{
        font: bold;
        font-size: 40px;
        font-family: Arial;
        color: white;
	    text-align: center;
        padding-top: 15%;
    }
    p{
        font-size: 25px;
        font-family: Arial;
        color: navy;
	    text-align: center;
    }  
    th, td {
        font-size: 25px;
        padding: 8px;
        text-align: left;
        color: white;
        border-bottom: 1px solid #ddd;
    }
</style>

</head>
	<body>
	    <div>
            <h1> Day Mode </h1>
        </div>
        <div>
            <table style="padding-left: 40%;">
                <tr>
                    <th>sensor</th>
                    <th>value</th>
                    <th>unit</th>
                </tr>
                <tr>
                    <td>Temperature</td>
                    <td>
)=====");
String homePagePart2 = F(R"=====(
</td>
                    <td>degrees Celcius</td>
                </tr>
                <tr>
                    <td>Humidity</td>
                    <td>
)=====");
String homePagePart3 = F(R"=====(
</td>
                    <td>%</td>
                </tr>
            </table>
        </div>
	</body>
</html>
)=====");

String homePagePart4 = F(R"=====(
<!DOCTYPE html>
<html lang="en" > <!-- Needed for Screenreaders !-->
<head>
  <meta http-equiv="refresh" content="30">
<!-- UTF-8 character set covers most characters in the world -->
  <meta charset="utf-8">
  <!-- Make page respond to screen size !-->
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
 
<!--Include a Title. Used by Search Engines -->
<title> Temperature Reading WebServer </title>
<style>
   
    body {       
	    background-color: navy;
	}

    h1{
        font: bold;
        font-size: 40px;
        font-family: Arial;
        color: white;
	    text-align: center;
        padding-top: 15%;
    }
    p{
        font-size: 25px;
        font-family: Arial;
        color: navy;
	    text-align: center;
    }  
    th, td {
        font-size: 25px;
        padding: 8px;
        text-align: left;
        color: white;
        border-bottom: 1px solid #ddd;
    }
</style>

</head>
	<body>
	    <div>
            <h1> Night Mode </h1>
        </div>
        <div>
            <table style="padding-left: 40%;">
                <tr>
                    <th>sensor</th>
                    <th>value</th>
                    <th>unit</th>
                </tr>
                <tr>
                    <td>Temperature</td>
                    <td>
)=====");