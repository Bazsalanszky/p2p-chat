d8 = () => { Math.floor(Date.now()/1E3); }

var fetch_messages = new XMLHttpRequest();
var messages_url = window.location.href + ".txt"; // a szerver ezen az url-en várja az automatikus frissítéshez a lekérdezéseket, responsenak csak az üzenetek kellenek
var lastMsgID = -1; // a szervernek az ennél nagyobb id-jű üzeneteket kell csak visszadobnia

setInterval(function(){
    fetch_messages.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) loadMessages(this.responseText);
    };
    fetch_messages.open("GET", url, true);
    fetch_messages.send();
},1000);

function loadMessages(text) {
    //let arr = JSON.parse(text);
    var out = [];
    var i;
    for(i = 0; i < arr.length; i++) {
        out.push(`Message from ${arr[i].sender}: ${arr[i].msg}`);
        lastMsgID = arr[i].id;
    }
    document.getElementById("chat").innerHTML += out.join("<br>");
    // autoscroll:
    var elem = document.getElementById('chat');
    elem.scrollTop = elem.scrollHeight;
}