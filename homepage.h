String homePagePart1 = F(R"=====(
<!DOCTYPE html>
<html lang="en" > <!-- Needed for Screenreaders !-->
<head>
<!-- UTF-8 character set covers most characters in the world -->
  <meta charset="utf-8">
  <!-- Make page respond to screen size !-->
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
  <!--Include a Title. Used by Search Engines -->
<title> Temperature Reading WebServer </title>
<style>
    *{
        font-family: Arial;
    }
    .flex-container {
        display: flex;
        flex-direction: column;
        align-content: space-between;
    }
    .flex-row{
        display: flex;
        flex-direction: row;
        align-content: space-between; 
    }
    .slidecontainer{
        width: 100%;
    }
    .slider{
        width: 100%;
    }
    body {       
	    background-color: rgb(56, 141, 226);
	}

    h1{
        font: bold;
        font-size: 40px;
        color: white;
	    text-align: center;
    }
    h2{
        font: bold;
        font-size: 40px;
        color: white;
	    text-align: center;
        padding-top: 5%;
    }
    p{
        font-size: 25px;
        color: navy;
	    text-align: center;
    }  
    th, td {
        font-size: 25px;
        padding: 8px;
        padding-left: 8%;
        text-align: center;
        color: white;
        border-bottom: none;
    }
    .button{
        background-color: orange;
        width: 33.33%;
        border-top: none;
        border-bottom: none;
        border-left: none;
        font-size: 20px;
        font-weight: bold;
    }
    .button:hover{
        background-color: darkorange;
    }
    .button:active{
        background-color: rgb(240, 236, 10);
    }

</style>

</head>
	<body>
	    <div class="flex-container">
            <h1> Current Mode: </h1>
            <p style="padding-bottom: 5%;"></p>
        </div>
        <div class="flex-container" style="padding-bottom: 2%;">
            <table>
                <tr>
                    <th>Sensor</th>
                    <th>Value</th>
                    <th>Unit</th>
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
        <div class="flex-row" style="padding-left: 15%;">
            <!--add graphs here-->
            <iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/2412315/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>
            <iframe width="450" height="260" style="border: 1px solid #cccccc;" src="https://thingspeak.com/channels/2412315/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15"></iframe>

        </div>
        <div>
            <h2>Change the Mode of the Lamp</h2>
        </div>
        <div class="flex-row">
            <!--add buttons to change modes and slider here-->
            <button class="button" onclick="fetch('/sentKeyPressToWebServer?button=N')" style="border-radius:50px 0px 0px 50px"type="button">Night Mode</button>
            <button class="button" onclick="fetch('/sentKeyPressToWebServer?button=D')" type="button">Day Mode</button>
            <button class="button" onclick="fetch('/sentKeyPressToWebServer?button=M')" style="border-right:none;border-radius:0px 50px 50px 0px" type="button">Mannual Mode</button>
        </div>
        <div class="slidecontainer">
            <input type="range" min="0" max="255" class="slider">
        </div>
        <div>
            <!--add buttons to set alarms here-->
        </div>
	</body>
</html>

)=====");

String homePagePart4 = F(R"=====(
<!DOCTYPE html>
<html lang="en" > <!-- Needed for Screenreaders !-->
<head>
<!-- UTF-8 character set covers most characters in the world -->
  <meta charset="utf-8">
  <!-- Make page respond to screen size !-->
  <meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
  <!--Include a Title. Used by Search Engines -->
<title> Temperature Reading WebServer </title>
<style>
    *{
        font-family: Arial;
    }
    .flex-container {
        display: flex;
        flex-direction: column;
        align-content: space-between;
    }
    .flex-row{
        display: flex;
        flex-direction: row;
        align-content: space-between; 
    }
    .slidecontainer{
        width: 100%;
    }
    .slider{
        width: 100%;
    }
    body {       
	    background-color: navy;
	  }
    h1{
        font: bold;
        font-size: 40px;
        color: white;
	    text-align: center;
    }
    h2{
        font: bold;
        font-size: 40px;
        color: white;
	    text-align: center;
        padding-top: 5%;
    }
    p{
        font-size: 25px;
        color: navy;
	    text-align: center;
    }  
    th, td {
        font-size: 25px;
        padding: 8px;
        padding-left: 8%;
        text-align: center;
        color: white;
        border-bottom: none;
    }
    .button{
        background-color: orange;
        width: 33.33%;
        border-top: none;
        border-bottom: none;
        border-left: none;
        font-size: 20px;
        font-weight: bold;
    }
    .button:hover{
        background-color: darkorange;
    }
    .button:active{
        background-color: rgb(240, 236, 10);
    }

</style>

</head>
	<body>
	    <div class="flex-container">
            <h1> Current Mode: </h1>
            <p style="padding-bottom: 5%;"></p>
        </div>
        <div class="flex-container" style="padding-bottom: 2%;">
            <table>
                <tr>
                    <th>Sensor</th>
                    <th>Value</th>
                    <th>Unit</th>
                </tr>
                <tr>
                    <td>Temperature</td>
                    <td>
)=====");