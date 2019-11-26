String.prototype.replaceAll = function (search, replacement) {
    var target = this;
    return target.split(search).join(replacement);
};
var submit = document.getElementById('s');
joypixels.ascii = true;

function encodeQueryData(data) {
    const ret = [];
    for (let d in data)
        ret.push(encodeURIComponent(d) + '=' + encodeURIComponent(data[d]));
    return ret.join('&');
}

function submitForm() {
    var formElement = document.forms.namedItem("sendmsg")
    var formData = new FormData(formElement);
    if (formData.get("message") != "%0D%0A" && formData.get("message") != null) {
        var data = {'id': formData.get("id"), 'message': formData.get("message")}
        var xhr = new XMLHttpRequest();
        xhr.open('POST', window.location.href, true);
        xhr.onload = function () {

            console.log(this.responseText);
        };
        xhr.send(encodeQueryData(data));
    }
    $('#message').val('');
}

var fetch_messages = new XMLHttpRequest();
var url = "/peers/" + window.location.pathname + ".txt";
var last_text = "";

setInterval(function () {
    fetch_messages.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            loadMessages(this.responseText.split("\n"));
        }
        //$(".msg-box").animate({scrollTop: document.getElementById("msgs").offsetHeight }, "slow");
        last_text = this.responseText;
    };
    fetch_messages.open("GET", url, true);
    fetch_messages.send();
}, 1000);

$(function () {
    $("#message").keypress(function (e) {
        var code = (e.keyCode ? e.keyCode : e.which);
        if (code == 13) {
            $("#s").trigger('click');
            $('#message').val('');
            return true;
        }
    });
});


function loadMessages(arr) {
    var out = "";
    var i;
    for (i = 0; i < arr.length; i++) {
        if (arr[i] == '') continue;
        if (arr[i].indexOf("Me:") == -1) {
            out += '<div class="card"><div class="card-body"><h6 class="text-muted card-subtitle mb-2">' + window.location.pathname.replace("/", "") + '<br></h6><p class="card-text">' + joypixels.toImage(decodeURIComponent(arr[i].replaceAll('+', ' '))) + '</p></div></div>';
        } else {
            var msg = arr[i].replace("Me: ", "");
            out += '<div class="card"><div class="card-body"><h6 class="text-muted card-subtitle mb-2" style="text-align:right;">You<br></h6><p class="card-text" style="text-align:right;">' + joypixels.toImage((decodeURIComponent(msg.replaceAll('+', ' ')))) + '</p></div></div>';
        }
    }
    out += "<div id='endl'></div>";
    var elem = document.getElementById("msgs");
    elem.innerHTML = out;
    document.getElementById('endl').scrollIntoView();
}