function eraseCookie(name) {   
    document.cookie = name +'=; Path=/; Expires=Thu, 01 Jan 1970 00:00:01 GMT;';
}

function getCookie(name) {
    var nameEQ = name + "=";
    var ca = document.cookie.split(';');
    for(var i=0;i < ca.length;i++) {
        var c = ca[i];
        while (c.charAt(0)==' ') c = c.substring(1,c.length);
        if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
    }
    return null;
}

function setCookie(name,value,days) {
    var expires = "";
    if (days) {
        var date = new Date();
        date.setTime(date.getTime() + (days*24*60*60*1000));
        expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "")  + expires + "; path=/";
}

function setCookieAndRedirect(name,value,days,url) {
    setCookie(name,value,days);
    document.location=url;
}

function appendCookieAndRedirect(name,value,days,separator,url) {
    var cookie = getCookie(name);
    if (cookie) {
        cookie = cookie + separator + value;
        setCookieAndRedirect(name,cookie,days,url);
    }
    else {
        setCookieAndRedirect(name,value,days,url);
    }
}