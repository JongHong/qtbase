const char * loaderScript = R"STRING_DELIMITER( 

var isChromeApp = %CHROMEAPP%

// Utility function for plitting Url query paramterers
// ("?Foo=bar&Bar=baz") into key-value pars.
function decodeQuery() {
    var query = window.location.search.substring(1);
    var vars = query.split('&');
    var keyValues = {}
    for (var i = 0; i < vars.length; i++) {
        // split key/value on the first '='
        var parts = vars[i].split(/=(.+)/);
        var key = decodeURIComponent(parts[0]);
        var value = decodeURIComponent(parts[1]);
        if (key && key.length > 0)
            keyValues[key] = value;
    }
    return keyValues;
}

function copyToClipboard(content)
{
    var copySource = document.createElement("div");
    copySource.contentEditable = true;
    var activeElement = document.activeElement.appendChild(copySource).parentNode;
    copySource.innerText = content;
    copySource.focus();
    document.execCommand("Copy", null, null);
    activeElement.removeChild(copySource);
}

function pasteFromClipboard()
{
    var pasteTarget = document.createElement("div");
    pasteTarget.contentEditable = true;
    var activeElement = document.activeElement.appendChild(pasteTarget).parentNode;
    pasteTarget.focus();
    document.execCommand("Paste", null, null);
    var content = pasteTarget.innerText;
    activeElement.removeChild(pasteTarget);
    return content;
}

// Qt message handler
function handleMessageEvent(messageEvent)
{
    // Expect messages to be in the form "tag:message",
    // and that the tag has a handler installed in the
    // qtMessageHandlers object.
    //
    // As a special case, messages with the "qtEval" tag
    // are evaluated with eval(). This allows Qt to inject
    // javscript into the web page, for example to install
    // message handlers.

    if (this.qtMessageHandlers === undefined) {
        this.qtMessageHandlers = {}
        // Install message handlers needed by Qt:
        this.qtMessageHandlers["qtGetAppVersion"] = function(url) {
            embed.postMessage("qtGetAppVersion: " + navigator.appVersion);
        };
        this.qtMessageHandlers["qtOpenUrl"] = function(url) {
            window.open(url);
        };
        this.qtMessageHandlers["qtClipboardRequestCopy"] = function(content) {
            console.log("copy to clipbard " + content);
            copyToClipboard(content);
        };
        this.qtMessageHandlers["qtClipboardRequestPaste"] = function() {
            var content = pasteFromClipboard();
            console.log("paste from clipboard " + content);
            embed.postMessage("qtClipboardtPaste: " + content);
        };
    }

    // Give the message handlers access to the nacl module.
    var embed = document.getElementById("nacl_module");

    var parts = messageEvent.data.split(/:(.+)/);
    var tag = parts[0];
    var message = parts[1];
    if (tag == "qtEval") {
        if (!isChromeApp) // "eval()" is not allowed for Chrome Apps
            eval(message)
    } else {
        this.qtMessageHandlers[tag](message);
    }
}

function createNaClEmbed()
{
    // Create NaCl <embed> element.
    var embed = document.createElement("EMBED");
    embed.setAttribute("id", "nacl_module");
    embed.setAttribute("name", "%APPNAME%");
    embed.setAttribute("src", "%APPNAME%.nmf");
    embed.setAttribute("type", "%APPTYPE%");

    // Decode and set URL query string values as attributes on the embed tag.
    // This allows passing for example ?QSG_VISUALIZE=overdraw
    var query = decodeQuery();
    for (var key in query) {
        if (key !== undefined)
            embed.setAttribute(key, query[key])
    }

    // Create container div which handles load and message events
    var listener = document.createElement("div");
    listener.addEventListener('message', handleMessageEvent, true);
    listener.appendChild(embed);
    listener.embed = embed;
    

    document.body.appendChild(listener);
}

document.addEventListener("DOMContentLoaded", createNaClEmbed);

)STRING_DELIMITER";